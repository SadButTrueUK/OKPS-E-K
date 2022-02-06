/**
* \file    positionDetFiveEC.c
* \brief   \copybrief positionDetFiveEC.h
*
* \version 1.0.1
* \date    28-12-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "positionDetFiveEC.h"
#include "positionDet.h"
#include "InterChannel.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define MEASUR_TO                100          ///< ����� ��������� ��������, ��.
#define FIRST_CHANGE_POS_DET_TO  1000         ///< ����� ����� ��������� ��������� (1-�� ����� ���������), ��.
#define CHANGE_POS_DET_TO        10000        ///< ����� ����� ��������� ���������, ��.

//*****************************************************************************
/// \brief ������ ����� �������� RMS ������� �������� ����� � ������� �������� ���������.
///
#define RMS_LOW_0_65V            163U

//*****************************************************************************
/// \brief ������� ����� �������� RMS ������� �������� ����� � ������� �������� ���������.
///
#define RMS_HIGH_1_5V            375U

//*****************************************************************************
/// \brief ������ ����� ���������� ������������ RMS ������� �������� ����� ��� ���������+.
///
#define ZERO_PLUS_LOW_2_9V       725U

//*****************************************************************************
/// \brief ������� ����� ���������� ������������ RMS ������� �������� ����� ��� ���������+.
///
#define ZERO_PLUS_HIGH_3_5V      875U

//*****************************************************************************
/// \brief ������ ����� ���������� ������������ RMS ������� �������� ����� ��� ���������-.
///
#define ZERO_MINUS_LOW_1_0V      250U

//*****************************************************************************
/// \brief ������� ����� ���������� ������������ RMS ������� �������� ����� ��� ���������-.
///
#define ZERO_MINUS_HIGH_1_7V     425U

//*****************************************************************************
#define VALUES_R1R2_MISMATCH_0_1 25U          ///< ������� ����� ������������� RMS �������.

//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ���������.
///
typedef struct
{
    uint8_t ctrl   :1;        ///< ��������� ������.
    uint8_t unused :7;        ///< �������������� ����.
} PosDetFiveEC_flags;

//*****************************************************************************
/// \brief ����������� ������ ��������� � \a uint8_t.
///
typedef union
{
    PosDetFiveEC_flags str;         ///< ����� ���������.
} uPosDetFiveEC_flags;

//*****************************************************************************
/// \brief ��������� ����������� ���������.
///
typedef struct
{
    uint16_t ur1zero;        ///< ���������� �������� R1.
    uint16_t ur1rms;         ///< �������������������� �������� R1.
    uint16_t ur2zero;        ///< ���������� �������� R2.
    uint16_t ur2rms;         ///< �������������������� �������� R2.
} MeasurResult;

//*****************************************************************************
// ���������� ��������� �������������� ��������
//*****************************************************************************

//*****************************************************************************
/// \brief ���������� ��������� ����������.
///
const ArrayPositionDet fiveWireECPosDet =
{
    FiveWireECPosDet_ctor,
    FiveWireECPosDet_run,
    FiveWireECPosDet_turnOn,
    FiveWireECPosDet_isEnable,
    FiveWireECPosDet_getPosition,
    FiveWireECPosDet_getRequestPosDet,
    FiveWireECPosDet_setPosDet
};

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************
static          uPosDetFiveEC_flags flags;                ///< ����� ���������.
static          PositionDet_State   requestPosDet;        ///< ������� ������� ����������� ���������.
static          PositionDet_State   resultPosDet;         ///< ��������� ����������� ���������.
static          PositionDet_State   lastPosDet;           ///< ��������� ��������� ����������� ���������.
static          PositionDet_State   ctrlPosDet;           ///< ������������� ������� ��������.
static          MeasurResult        measur;               ///< ��������� ���������.
static uint16_t measurToCnt;                              ///< ������� ������� ���������.
static uint16_t changePosDetCnt;                          ///< ������� ��������� ��������� ���������.

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ����������� ��������� �� ����������� ���������.
/// \param data - ��������� �� ���������� ���������.
/// \return ���������.
///
static PositionDet_State posDetermin( MeasurResult *data );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �������������
void FiveWireECPosDet_ctor( void )
{
    requestPosDet = eDetPlus;
    // ��������� ����������� ��������� 
    flags.str.ctrl = 0;
    lastPosDet = eDetPlus; // ��� ��������� ���������"��������� ����"
    resultPosDet = eDetNon;
    requestPosDet = eDetNon;
    ctrlPosDet = eDetNon;
}

//*****************************************************************************
// ���������� �������
void FiveWireECPosDet_run( void )
{
    if( flags.str.ctrl )
    { // ��������
        if( measurToCnt ) measurToCnt--; // ��������� ����-���� ���������
        if( changePosDetCnt ) changePosDetCnt--; // ��������� ����-���� ����� ���������
        // �������� ����-���� ���������
        if( measurToCnt == 0 )
        { // ������ ���������� ��������
            measur.ur1zero = InterChannel_getData( eICId_U_R1bias );
            measur.ur1rms = InterChannel_getData( eICId_U_R1rms );
            measur.ur2zero = InterChannel_getData( eICId_U_R2bias );
            measur.ur2rms = InterChannel_getData( eICId_U_R2rms );
            resultPosDet = posDetermin( &measur );
            if( resultPosDet != eDetNon )
            { // ��������� ���������� 
                requestPosDet = ctrlPosDet;
                changePosDetCnt = CHANGE_POS_DET_TO; // �������� ����-��� ���������
            }
            else if( changePosDetCnt == 0 )
            { // ��������� �� ���������� � ������ ����� ������� ��������� ���������
                requestPosDet = requestPosDet == eDetPlus ? eDetMinus : eDetPlus;
                ctrlPosDet = requestPosDet;
                changePosDetCnt = CHANGE_POS_DET_TO; // ������� ����-��� ���������� ���������
            }
            measurToCnt = MEASUR_TO; // ������� ����-��� ���������� ���������
        }
    }
}

//*****************************************************************************
// ���������/���������� ����������� ���������
void FiveWireECPosDet_turnOn( bool on )
{
    flags.str.ctrl = on;
    if( flags.str.ctrl )
    { // ��������
        requestPosDet = lastPosDet;
        measur.ur1zero = 0;
        measur.ur1rms = 0;
        measur.ur2zero = 0;
        measur.ur2rms = 0;
        measurToCnt = MEASUR_TO;
        changePosDetCnt = FIRST_CHANGE_POS_DET_TO;
    }
    else
    { // ���������
        lastPosDet = requestPosDet;
        resultPosDet = eDetNon;
        requestPosDet = eDetNon;
    }
}

//*****************************************************************************
// �������� ��������� ������
bool FiveWireECPosDet_isEnable( void )
{
    return flags.str.ctrl;
}

//*****************************************************************************
// �������� ������������� ���������
PositionDet_State FiveWireECPosDet_getPosition( void )
{
    return resultPosDet;
}

//*****************************************************************************
// �������� ������ �� ��������� ������� �������� ���������
PositionDet_State FiveWireECPosDet_getRequestPosDet( void )
{
    return requestPosDet;
}

//*****************************************************************************
// �������� ������������� ������� ��������
void FiveWireECPosDet_setPosDet( PositionDet_State pos )
{
    ctrlPosDet = pos;
}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
// ����������� ��������� �� ����������� ���������
PositionDet_State posDetermin( MeasurResult *data )
{
    uint16_t urZeroDiff, urRmsDiff; // ������� �������� zero � rms
    uint16_t urZeroAver, urRmsAver; // ������� �������� zero � rms

    urZeroDiff = data->ur1zero > data->ur2zero ? data->ur1zero - data->ur2zero :
            data->ur2zero - data->ur1zero;
    urRmsDiff = data->ur1rms > data->ur2rms ? data->ur1rms - data->ur2rms : data->ur2rms - data->ur1rms;

    // �������� ������������ ������� �������� zero � rms
    if( urZeroDiff < VALUES_R1R2_MISMATCH_0_1 ||
        urRmsDiff < VALUES_R1R2_MISMATCH_0_1 )
    {
        urZeroAver = ( data->ur1zero + data->ur2zero ) >> 1; // ������ �������� �������� zero
        urRmsAver = ( data->ur1rms + data->ur2rms ) >> 1; // ������ �������� �������� rms
    }
    else return eDetNon; // ��������� �� ����������

    // �������� ������������ ������������� ����������
    if( urZeroAver < ZERO_MINUS_LOW_1_0V ||
        urZeroAver > ZERO_PLUS_HIGH_3_5V ||
        urRmsAver > RMS_HIGH_1_5V ||
        urRmsAver < RMS_LOW_0_65V ) return eDetNon; // ��������� �� ����������

    if( urZeroAver > ZERO_PLUS_LOW_2_9V ) return eDetPlus; // ��������� ����
    if( urZeroAver < ZERO_MINUS_HIGH_1_7V ) return eDetMinus; // ��������� �����

    return eDetNon; // ��������� �� ����������
}

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   28-12-2017
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
*/
