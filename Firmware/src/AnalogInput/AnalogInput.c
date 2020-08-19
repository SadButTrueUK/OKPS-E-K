/**
* \file    AnalogInput.c
* \brief   \copybrief AnalogInput.h
*
* \version 1.0.1
* \date    20-06-2018
* \author  ������ �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <string.h>
#include "AnalogInput.h"
#include "DigitalFilter.h"
#include "Adc.h"
#include "ProtectionState_codes.h"
#include "asserts_ex.h"

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ���������� ���������� ��������.
///
typedef struct tagAnalogInputSettings
{
    uint16_t script;          ///< �������� �������������� ��������� �������.
    uint16_t filterID;        ///< ������������� ������������� �������.
} AnalogInputSettings;

//*****************************************************************************
/// \brief ������ ���������� ���������� ��������.
///
typedef struct tagAnalogInputMeasuring
{
    int16_t  idx;                             ///< ������ ���������� �������� � ������ ������ ���� ���������� buf[].
    uint16_t buf[AVERAGE_WINDOW_SIZE];        ///< ����� ������ ���� ����������.
    uint32_t sum;                             ///< ������� ����� ���� �������� � ������ ������ ���� ���������� buf[].
} AnalogInputMeasuring;

//*****************************************************************************
/// \brief ������ ������� ��������� ���������� ��������.
///
typedef struct tagAnalogInputData
{
    AnalogInputValue     values;           ///< �������� ���������� ��������.
    AnalogInputSettings  settings;         ///< ��������� ���������� ��� ������� ����������� �������.
    AnalogInputMeasuring measuring;        ///< ������ ���������� �������.
} AnalogInputData;

//*****************************************************************************
// ����������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
/// \brief ������ ������� ��� ������ ����������� �������������� ���.
///
int16_t samples[AnalogInputDrvQueueSize];

//*****************************************************************************
static AnalogInputData analogInputData[eAinChCount];        ///< ������ ��������� ���������� ��������.

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ���������� ����������� �����, ����������������� �� �������.
/// \param x - �������� ������� � ������� unsigned int32.
/// \return ���������� ������ ��������� \a �.
///
uint16_t sqrt_topt( uint32_t x );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ����������
void AnalogInput_ctor( void )
{
    for( uint16_t i = 0; i < eAinChCount; i++ )
        memset( &analogInputData[i], 0, sizeof(analogInputData[i] ) );

    Adc_ctor( );
}
//*****************************************************************************
// ��������� �������������� ������
void AnalogInput_setChannelSettings( uint16_t channel,
                                     uint16_t filterId,
                                     AnalogInputScript script,
                                     uint16_t bias,
                                     uint16_t aws )
{
    ASSERT_EX_ID( eGrPS_AnalogInput, ePS_AinErrorChannel,
                  ( channel < eAinChCount ),
                  1, channel, 0, 0 );
    ASSERT_EX_ID( eGrPS_AnalogInput, ePS_AinErrorScript,
                  ( ( script > eAinScriptError ) &&
                    ( script <= eAinScriptDC ) ),
                  1, channel, script, 0 );
    ASSERT_EX_ID( eGrPS_AnalogInput, ePS_AinErrorDfId,
                  ( filterId < eDigitalFilterCount ) || ( filterId == eDigitalFilterOff ),
                  filterId, 0, 2, 0 );
    ASSERT_EX_ID( eGrPS_AnalogInput, ePS_AinErrorIncorrectValueAWS,
                  aws == 2  || aws == 4  || aws == 8   || aws == 16 ||
                  aws == 32 || aws == 64 || aws == 128 || aws == 256,
                  aws, 0, 0, 0 );

    memset( &analogInputData[channel], 0, sizeof(analogInputData[channel] ) );
    analogInputData[channel].settings.script = script;
    analogInputData[channel].settings.filterID = filterId;
    analogInputData[channel].values.bias = bias;
    analogInputData[channel].values.aws = aws;

    // ���������� ����� �������
    for( analogInputData[channel].values.shift = 0; ( aws = aws >> 1 ); analogInputData[channel].values.shift++ ) 
        ;
}

//*****************************************************************************
// ������ ���������� ��������
const AnalogInputValue *AnalogInput_getData( uint16_t channel )
{
    ASSERT_EX_ID( eGrPS_AnalogInput, ePS_AinErrorChannel,
                  ( channel < eAinChCount ),
                  2, channel, 0, 0 );

    return &analogInputData[channel].values;
}

//*****************************************************************************
// ��������� ���������� ��������
void AnalogInput_run( void )
{
    uint16_t adcData[AnalogInputDrvQueueSize][eAinChCount];
    uint16_t samplesCnt = 0;

    // ������ ������ �� ������ ���. ����� ���������� ����� � ���������� samplesCnt
    // �������� ���������� ����������� ��������
    while( Adc_isReady() )
    {
        // AnalogInput_run �� ���� ������ �� main ������ �������� ��� ������ � ���.
        ASSERT_EX_ID( eGrPS_AnalogInput, ePS_AinErrorDriverGet,
                      ( samplesCnt < AnalogInputDrvQueueSize ), 3, samplesCnt, AnalogInputDrvQueueSize, 0 );
        Adc_get( &adcData[samplesCnt++][0]);
    }

    // ��������� ������
    for( uint16_t i = 0; i < eAinChCount; i++ )
    {
        // ���������� �������� ������� �� ����� ��� (������ � ������ ���)
        analogInputData[i].values.value = adcData[samplesCnt - 1][i];           

        switch( analogInputData[i].settings.script )                            // �������������� ��������� �������
        {
            case eAinScriptNone:                                                // ��� ���������
                ;
                break;

            // ��������� ����������� ���������� (DC) �� ����� ���
            case eAinScriptDC:                                                  
                // ������� �������� �������� ������� (aver)
                for( uint16_t sample = 0; sample < samplesCnt; sample++ )
                {
                    analogInputData[i].measuring.sum = analogInputData[i].measuring.sum
                        - analogInputData[i].measuring.buf[analogInputData[i].measuring.idx]            // ��������� ������� �������� 
                        + adcData[sample][i];                                                           // ������������ � ����� ��������

                    // ������ ������ �������� � ����� ���� ����������
                    analogInputData[i].measuring.buf[analogInputData[i].measuring.idx++] = adcData[sample][i];
                    
                    // ��������� � �������� ������� ������ ���� ����������
                    analogInputData[i].measuring.idx != analogInputData[i].values.aws ? : ( analogInputData[i].measuring.idx = 0 );
                }

                // �������� aver ����������� ������ ������������
                analogInputData[i].values.aver = analogInputData[i].measuring.sum >> analogInputData[i].values.shift;
                break;

            case eAinScriptAC:   // ��������� ����������� ���������� (AC) �� ����� ���
                // ��������� ����������� �������� (bias) �� �������                
                for( uint16_t sample = 0; sample < samplesCnt; sample++ )
                    samples[sample] = (int16_t)adcData[sample][i] - (int16_t)analogInputData[i].values.bias;

                // ��������� ������� ��� ������ ��������� ������� (�����������)  
                analogInputData[i].settings.filterID == eDigitalFilterOff ? : DigitalFilter_run( analogInputData[i].settings.filterID, samples, samplesCnt );

                // ������� ��������������������� �������� (rms)
                for( uint16_t sample = 0; sample < samplesCnt; sample++ )
                {
                    // ��� ����������� Microchip � DSP-����� (dsPIC30F � dsPIC33EP) ������������ ������������ DSP-������� ��������� - MULSS
                    #if defined(__HAS_DSP__)
                        // ��������� ������� ��������
                        analogInputData[i].measuring.sum = analogInputData[i].measuring.sum
                            - __builtin_mulss( analogInputData[i].measuring.buf[analogInputData[i].measuring.idx], 
                            analogInputData[i].measuring.buf[analogInputData[i].measuring.idx] )
                            + __builtin_mulss( samples[sample], samples[sample] ); // �������� � ����� ��������
                    #else
                        // ��������� ������� ��������
                        analogInputData[i].measuring.sum = analogInputData[i].measuring.sum
                            - analogInputData[i].measuring.buf[analogInputData[i].measuring.idx] * 
                            analogInputData[i].measuring.buf[analogInputData[i].measuring.idx]
                            + samples[sample] * samples[sample]; // ���������� ������ ��������
                    #endif
                    
                    // ������ ������ �������� � ����� ���� ����������
                    analogInputData[i].measuring.buf[analogInputData[i].measuring.idx++] = samples[sample];

                    // �������� ������� ������ ���� ����������
                    analogInputData[i].measuring.idx != analogInputData[i].values.aws ? : ( analogInputData[i].measuring.idx = 0 );             
                }
                
                // ��������� �������������������� �������� ������� (RMS)
                analogInputData[i].values.rms = sqrt_topt( analogInputData[i].measuring.sum >> analogInputData[i].values.shift );
                break;

            default:
                ERROR_EX_ID( eGrPS_AnalogInput, ePS_AinErrorScript, 3, i, analogInputData[i].settings.script, 0 );
                break;
        }
    }
}

//*****************************************************************************
// ���������� �������������� ��������
void AnalogInput_runInterrupt( void )
{
    Adc_run( );
}

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   20-06-2018
* �����  ������ �.�.
* 
* ���������:
*     ������� ������.
*/
