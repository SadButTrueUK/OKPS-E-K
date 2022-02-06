/**
* \file    positionDetNineWire.c
* \brief   \copybrief positionDetNineWire.h
*
* \version 1.0.4
* \date    12-07-2021
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "positionDetNineWire.h"
#include "InterChannel.h"
#include "OverloadDet.h"
#include "ProtectionState_codes.h"
#include "asserts.h"
#include "AnalogMeasurement.h"
#include "main.h"


//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************
#define NUM_NON_CONTACT_SENSORS  2     ///< ���������� ������������� �������� ��������� 
#define RMS_HIGH_40V             830   ///< ��������� �������� ���������� � �������������� ������� (������� ��������).
#define RMS_HIGH_40V_HYST        847   ///< ��������� �������� ���������� � �������������� ������� (������� ��������) � ������ �����������.
#define RMS_LOW_12V              260   ///< ��������� �������� ���������� � �������������� ������� (������ ��������).
#define RMS_LOW_12V_HYST         286   ///< ��������� �������� ���������� � �������������� ������� (������ ��������) � ������ �����������.

//*****************************************************************************
//  ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ���������.
///
typedef struct
{
    uint8_t ctrl   :1;        ///< ��������� ������.
    uint8_t unused :7;        ///< �������������� ����.
} PosDetNineWire_flags;

//*****************************************************************************
/// \brief ����������� ������ ��������� � \a uint8_t.
///
typedef union
{
    PosDetNineWire_flags str;         ///< ����� ���������.
} uPosDetNineWire_flags;

//*****************************************************************************
/// \brief ��������� ����������� ���������.
///
typedef struct
{
    uint16_t ur1rms;    ///< �������������������� �������� R1.
    uint16_t ur2rms;    ///< �������������������� �������� R2.
} MeasurResult;

//*****************************************************************************
/// \brief ����������� �������� "���" ������� � ������ ������� �������� �������
///
typedef enum 
{
    eZoneLowSignal,              ///< ������ ���� ������� � ������ �������������� ������� (�� 0 �� 12�)
    eZoneMediumSignal,           ///< ������� ���� ������� � ������ �������������� ������� (�� 12 �� 39�)
    eZoneHighSignal              ///< ������� ���� ������� � ������ �������������� ������� (����� 39�)
} ZonesOfSignalSensor;


//*****************************************************************************
// ���������� ��������� �������������� ��������
//*****************************************************************************

//*****************************************************************************
/// \brief ���������� ��������� ����������.
/// 
const ArrayPositionDet nineWirePosDet =
{
    NineWirePosDet_ctor,
    NineWirePosDet_run,
    NineWirePosDet_turnOn,
    NineWirePosDet_isEnable,
    NineWirePosDet_getPosition,
    NineWirePosDet_getRequestPosDet,
    NineWirePosDet_setPosDet
};

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************
static uPosDetNineWire_flags flags;               ///< ����� ���������.
static PositionDet_State     resultPosDet;        ///< ��������� ����������� ���������.
static MeasurResult          measur;              ///< ��������� ���������.
static ZonesOfSignalSensor stateSensor[NUM_NON_CONTACT_SENSORS];

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ����������� ��������� �� ����������� ���������.
/// \param data - ���������� ���������.
/// \return ���������.
///
static PositionDet_State posDetermin( MeasurResult *data );

//*****************************************************************************
/// \brief ����������� � ����� �� 3 ��� (������, �������, �������) ��������� ������ � ������ �������������� �������.
/// \param voltageValue - ���������� �� ������ �������.
/// \param index - ����� �������.
/// \return ����� ����.
///
static ZonesOfSignalSensor detectionZoneOfSensorsSignal( uint16_t voltageValue, uint8_t index );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �������������
void NineWirePosDet_ctor( void )
{
    // �������� ����������� ��������� 
    flags.str.ctrl = 1;
    for( uint8_t i = 0; i < NUM_NON_CONTACT_SENSORS; i++ )
        stateSensor[NUM_NON_CONTACT_SENSORS] = eZoneLowSignal; 
}

//*****************************************************************************
// ���������� �������
//���� ��������� ����� ��� ������ - ���� ������ � �������� ���������,
//���� ��� � ���� ����� � �������� �������� - ���� ������ � �������� ��������� �� ����,
//� ��������� ������ - ������ ��������
void NineWirePosDet_run( void )
{
    if( flags.str.ctrl )
    { // ��������
        // ������ ���������� ��������
        if ( PosDetGenerator_isReadyForUse( ) )
        {
            measur.ur1rms = InterChannel_getData( eICId_U_R1rms );
            measur.ur2rms = InterChannel_getData( eICId_U_R2rms );
            resultPosDet = posDetermin( &measur );
        }
        else if( ActivityManager_getNeighborConnection( activityManager ) ) 
        {
            resultPosDet = ActivityManager_getNeighborPositionShunt( activityManager );
        }
    }
    else 
        resultPosDet = eDetNon;
}

//*****************************************************************************
// ���������/����������
void NineWirePosDet_turnOn( bool on )
{
    flags.str.ctrl = on;
    if( flags.str.ctrl )
    { //��������
        measur.ur1rms = 0;
        measur.ur2rms = 0;
    }
    else
    { //���������
        resultPosDet = eDetNon;
    }
}

//*****************************************************************************
// �������� ��������� ������
bool NineWirePosDet_isEnable( void )
{
    return flags.str.ctrl;
}

//*****************************************************************************
// �������� ������������� ���������
PositionDet_State NineWirePosDet_getPosition( void )
{
    return resultPosDet;
}

//*****************************************************************************
// �������� ������ �� ��������� ������� �������� ���������
PositionDet_State NineWirePosDet_getRequestPosDet( void )
{
    return eDetNon;
}

//*****************************************************************************
// �������� ������������� ������� ��������
void NineWirePosDet_setPosDet( PositionDet_State pos )
{

}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//����������� � ����� �� 3 ��� (������, �������, �������) ��������� ������ � ������ �������������� �������
static ZonesOfSignalSensor detectionZoneOfSensorsSignal( uint16_t voltageValue, uint8_t index )
{
    if( stateSensor[index] == eZoneLowSignal && voltageValue > RMS_LOW_12V_HYST )
        stateSensor[index] = eZoneMediumSignal;
    if( stateSensor[index] == eZoneMediumSignal && voltageValue > RMS_HIGH_40V_HYST )
        stateSensor[index] = eZoneHighSignal;
    if( stateSensor[index] == eZoneHighSignal && voltageValue < RMS_HIGH_40V )
        stateSensor[index] = eZoneMediumSignal;
    if( stateSensor[index] == eZoneMediumSignal && voltageValue < RMS_LOW_12V )
        stateSensor[index] = eZoneLowSignal;
    return stateSensor[index];
}

//*****************************************************************************
// ����������� ��������� ������� �� ����������� ���������
static PositionDet_State posDetermin( MeasurResult *data )
{
    if( detectionZoneOfSensorsSignal( data->ur1rms, 0 ) == eZoneHighSignal 
        && detectionZoneOfSensorsSignal( data->ur2rms, 1 ) == eZoneLowSignal )
        return eDetMinus;               //"��������� �����"
    if( detectionZoneOfSensorsSignal( data->ur2rms, 1 ) == eZoneHighSignal 
        && detectionZoneOfSensorsSignal( data->ur1rms, 0 ) == eZoneLowSignal )
        return eDetPlus;                 //"��������� ����"
     if( detectionZoneOfSensorsSignal( data->ur2rms, 1 ) == eZoneHighSignal 
        && detectionZoneOfSensorsSignal( data->ur1rms, 0 ) == eZoneHighSignal )   //������������� ��������� (������ ��������)   
        return eDetUndefinedState;
    return eDetNon;                     //��������� "������ ��������"
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
*     ������� ������.
*
* ������ 1.0.2
* ����   23-04-2020
* �����  ��������� �.�.
*
* ���������:
*     ��������� ������� ������������ ������ �� �������� �������, ����� - ������ ��������
*
* ������ 1.0.3
* ����   14-04-2021
* �����  ��������� �.�.
*
* ���������:
*     ��������� ����������� ��������� ������� �� ������ ������ �� ������� �������.
*     ��� ���������� ��������� ��� ����� ����������
*
* ������ 1.0.4
* ���� 12-07-2021 
* �����  ��������� �.�.
*
* ���������:
*     �������� ���������� ������� � �������  �������� ����������� ��������� �������. 
*/