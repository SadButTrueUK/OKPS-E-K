/**
* \file    StateMain.c
* \brief   ��������� �������� ������.
*
* \version 1.0.1
* \date    16-01-2018
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "Main.h"
#include "ControlSystem.h"
#include "States.h"
#include "deviceAddress.h"
#include "relayCtrl.h"
#include "positionDet.h"
#include "ShuntShiftMotor.h"
#include "SafetyPowerControl.h"
#include "OverloadDet.h"
#include "IntegrCtrl.h"
#include "IntegrCtrlGen.h"

//*****************************************************************************
// ����������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static Fsm StateMain_fsm;        ///< �������� ��������� ��� ��������� �������� ������.

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ����� � ��������� �������� ������.
/// \details ������������� �����������, ��������� �� ����������, ���������� � ��������� �������������.
///
static void StateMain_onEntry( void );

//*****************************************************************************
/// \brief ��������� ��������� �������� ������.
/// \note ���������� ������ \a run �����������, ����� ���������� � ����������� ��������� ���������.
///
static bool StateMain_onRun( void );

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
// ��������� ����� � ��������� �������� ������
static void StateMain_onEntry( void )
{
    rs422 = Rs422_ctor( DeviceAddress_getAddress( ), RS422_LINE_TIMEOUT, RS422_SYSTEM_TIMEOUT, 
                        RS422_ORDER_DATA_LENGTH );
    Rs422_setEnable( rs422, true );
    blockExchange = BlockExch_ctor( BLOCK_EXCHANGE_SIZE, BLOCK_EXCHANGE_TIMEOUT );
    activityManager = ActivityManager_ctor( DeviceAddress_isPrimary( ) );
    ActivityManager_setEnable( activityManager, true );
    ControlSystem_ctor( );
    PositionDet_ctor( DeviceAddress_getRelayModuleType( ) );
    ShuntShift_ctor( DeviceAddress_getRelayModuleType( ), DeviceAddress_getMotorType( ) );
    OverloadDet_ctor( );
    IntegrCtrl_ctor( );
}

//*****************************************************************************
// ��������� ��������� �������� ������
static bool StateMain_onRun( void )
{
    Rs422_run( rs422 );
    BlockExch_run( blockExchange );
    ActiveManagerConnect_run( activityManager, blockExchange );
    ActivityManager_run( activityManager );
    RelayCtrl_run( );
    ControlSystem_run( );
    PositionDet_run( );
    ShuntShift_run( );
    IntegrCtrl_run( );
    IntegrCtrlGen_run( );
    return true;
}

//*****************************************************************************
// ����������� ��������� �������������� ��������
//*****************************************************************************

//*****************************************************************************
/// \brief ����������� ��������� ��������� �������� "��������� �������� ������".
///
const FsmState StateMain =
{
    StateMain_onEntry, 0, StateMain_onRun,
    &StateTop, &StateTop, &StateMain_fsm, &StateSafePassive, eStateMainId
};

//*****************************************************************************
/**
* ������� ���������: 
*
* ������ 1.0.1
* ����   16-01-2018
* �����  ��������� �.�.
*
* ���������:
*    ������� ��������.
*/
