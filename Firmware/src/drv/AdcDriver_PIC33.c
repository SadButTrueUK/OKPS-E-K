/**
* \file    AdcDriver_PIC33.c
* \brief   \copybrief AdcDriver_PIC33.h
*
* \version 1.0.1
* \date    28-03-2016
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "typeMK.h"
#include "asserts.h"
#include "ProtectionState_codes.h"
#include "Adc_Inic1.h"
#include "Adc_regsPIC33.h"
#include "AdcDriver_PIC33.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define NUMBER_CANALS 32        ///< ���������� ������� ���������.

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ������ ��������.
///
typedef struct
{
    bool     isInic;                         ///< ������� ������������� ������.
    volatile Adc_regsPIC33 *pAdcRegs;        ///< ��������� �� �������� ������ ���.
    uint16_t aBUFx[NUMBER_CANALS];           ///< ������ ���������� ��������.
    uint16_t canal;                          ///< ������� ���������� �������.
    uint32_t maskShift;                      ///< ����� ������.
    uint32_t canals;                         ///< ���������� ������.
    bool done;                               ///< ������� ���������� ���������.
} AdcModule;

//*****************************************************************************
// ����������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static AdcModule aAdcModule[eAdcCount];        ///< ������� ������ ��������� ���.

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief �������� ����� ���.
/// \return �������� ��� ������ � ������� ADxCSSL.
///
uint32_t AdcDriver_getCanal( AdcNumber number );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ������ ���
void AdcDriver_ctor( AdcNumber number )
{
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcDrvSelf, ( number < eAdcCount ) );
    AdcModule *me = &aAdcModule[number];
    INIC_ADC1_PIN;
    me->pAdcRegs = &adc1_regsPIC33;
    me->pAdcRegs->CON1 = INIC_ADC1_ADCON1;
    me->pAdcRegs->CON2 = INIC_ADC1_ADCON2;
    me->pAdcRegs->CON3 = INIC_ADC1_ADCON3;
    me->pAdcRegs->CON4 = INIC_ADC1_ADCON4;
    me->canals = INIC_ADC1_ADCSSL | INIC_ADC1_ADCSSH << 16;
    me->pAdcRegs->CSSH = 0;
    me->pAdcRegs->CSSL = 0;
    me->done = false;
    me->isInic = true;
    me->pAdcRegs->CON1bits.ADON = 1;
}

//*****************************************************************************
// ��������� ��������� ������ ����������
void AdcDriver_start( AdcNumber number )
{
    uint32_t temp;
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcDrvSelf, ( number < eAdcCount ) );
    AdcModule *me = &aAdcModule[number];
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcNoInic, ( me->isInic ) );
    me->pAdcRegs->CON1bits.DONE = 0;
    me->maskShift = 1;
    me->canal = 0;
    me->pAdcRegs->CON1bits.ADON = 0; // ������ � ADxCSSL ������� ��������� ��� ����������� ���
    temp = AdcDriver_getCanal( number );
    me->pAdcRegs->CSSL = ( uint16_t )( 0x0000FFFF & temp );
    me->pAdcRegs->CSSH = ( uint16_t )( temp >> 16 );
    me->pAdcRegs->CON1bits.ADON = 1;
    me->pAdcRegs->CON1bits.SAMP = 1;
}

//*****************************************************************************
// ������ ���������� ��������
uint8_t AdcDriver_get( AdcNumber number, uint16_t *array, uint8_t size )
{
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcDrvSelf, ( number < eAdcCount ) );
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcDrvSetSize, ( size < NUMBER_CANALS ) );
    AdcModule *me = &aAdcModule[number];
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcNoInic, ( me->isInic ) );
    uint8_t i;
    for( i = 0; i < size; i++ )
    {
        array[i] = me->aBUFx[i];
    }
    me->done = false;
    return size;
}

//*****************************************************************************
// ��������� ���������� ����������� ���, ������ 1
void AdcDriver_interruptAdc1( void )
{
    AdcModule *me = &aAdcModule[eAdc1];
    uint32_t temp;
    if( me->pAdcRegs->CSSL != 0 || me->pAdcRegs->CSSH != 0 )
    {
        ASSERT_ID( eGrPS_AdcDrv, ePS_CanDrvErrorAlg, ( me->canal < NUMBER_CANALS ) );
        me->aBUFx[me->canal++] = me->pAdcRegs->BUF0;
        me->pAdcRegs->CON1bits.ADON = 0;
        temp = AdcDriver_getCanal( eAdc1 );
        me->pAdcRegs->CSSL = ( uint16_t )( 0x0000FFFF & temp );
        me->pAdcRegs->CSSH = ( uint16_t )( temp >> 16 );
        me->pAdcRegs->CON1bits.ADON = 1;
        if( me->pAdcRegs->CSSL != 0 || me->pAdcRegs->CSSH != 0 )
        {
            me->pAdcRegs->CON1bits.SAMP = 1;
        }
        else
        {
            me->done = true;
        }
    }
    return;
}

//*****************************************************************************
// ������ ������� ���������� ��������
bool AdcDriver_isReady( AdcNumber number )
{
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcDrvSelf, ( number < eAdcCount ) );
    AdcModule *me = &aAdcModule[number];
    return me->done;
}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
// �������� ����� ���
uint32_t AdcDriver_getCanal( AdcNumber number )
{
    uint32_t ret = 0;
    AdcModule *me = &aAdcModule[number];
    ASSERT_ID( eGrPS_AdcDrv, ePS_CanDrvErrorAlg, ( me->maskShift != 0 ) );
    while( true )
    {
        if( ( me->canals & me->maskShift ) != 0 )
        {
            ret = me->maskShift;
            me->maskShift = me->maskShift << 1;
            break;
        }
        else
        {
            if( me->maskShift == 0x80000000 )
            {
                break;
            }
            else
            {
                me->maskShift = me->maskShift << 1;
            }
        }
    }
    return ret;
}

//*****************************************************************************
/**
* ���������:  
*    ������� ��������� �� ������������ ������ �� ������ 1.
*    ������ ��� ������������ �������� � ���������� INIC_ADC1_ADCSSLH INIC_ADC1_ADCSSL.
*    ������������ �����������  ����������.
*    ��� ������������ � �������������� ������ ��� ���� ������� 
*    ������������ ���������. ����������� ������� �������� ������������ �������,
*    ��� � ������ ������� ��������� ������������ �������� ��������. 
*/

/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   28-03-2016
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
*/
