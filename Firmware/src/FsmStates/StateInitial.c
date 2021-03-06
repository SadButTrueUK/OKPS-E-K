/**
* \file    StateInitial.c
* \brief   ��������� �������������.
*
* \version 1.0.1
* \date    28-09-2017
* \author  ������������ �.�., ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "Main.h"
#include "States.h"
#include "DebugTools.h"
#include "deviceAddress.h"
#include "relayCtrl.h"
#include "relayCtrlDrv.h"  // !!!��������

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ����� � ��������� �������������.
/// \note �� ����� ����������� ��������� ��������� ��������� �������������.
///
static void StateInitial_onEntry( void );

//*****************************************************************************
/// \brief ��������� ������ �� ��������� �������������.
/// \note �� ����� ����������� ���������� ���� �����������.
///
static void StateInitial_onExit( void );

//*****************************************************************************
/// \brief ��������� ������� � �������� ��������� ��� ��������� �������������.
/// \note ���� ���������� ���������� ����� ����������, ���������� ������� � ��������� �������� ������.
/// \note ���� ����� �� ������� ������� ����� #TIME_INIT_ADDRESS ��, ���������� ��������� �������������
/// ��������� ��������� ������ �� ��� ���, ���� ������ �� ����� ��������� � ��.
///
static bool StateInitial_onRun( void );

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
// ��������� ����� � ��������� �������������
static void StateInitial_onEntry( void )
{
    Indication_on( &ledRs1 );
    Indication_on( &ledRs2 );
    Indication_on( &ledPlusPos );
    Indication_on( &ledMinusPos );
    Indication_on( &ledLossCtrl );
}

//*****************************************************************************
// ��������� ������ �� ��������� �������������
static void StateInitial_onExit( void )
{
    Indication_off( &ledRs1 );
    Indication_off( &ledRs2 );
    Indication_off( &ledPlusPos );
    Indication_off( &ledMinusPos );
    Indication_off( &ledLossCtrl );
}

//*****************************************************************************
// ��������� ������� � �������� ��������� ��� ��������� �������������
static bool StateInitial_onRun( void )
{
    static uint16_t timeCounter = TIME_INIT_ADDRESS; // ��������� �������� �������� ����������� ������
    static bool notCorrect = false;

    if( timeCounter ) timeCounter--; // ��������� �������� �������
    if( DeviceAddress_isValid( ) )
    { // ������� ���������� �����
        // ���� �� ��������
        RelayCtrl_ctor( DeviceAddress_getRelayModuleType( ) ); // debug ����� ������� �������� ���� ����
        // ���������� ����� �������, ���� �������������
        Fsm_transit( &StateMain ); // ������� � StateMain
        return false;
    }
    else
    { // ���������� ����� ��� �� �������
        if( timeCounter == 0 && !notCorrect )
        { //������� ����������
            notCorrect = true;
            // ��������� ��������� ���������� ����������� ������
            Indication_blink( &ledRs1, LED_INCORRECT_ADDRESS_BLINK_ON, LED_INCORRECT_ADDRESS_BLINK_OFF, eIndPhNormal ); 
            Indication_blink( &ledRs2, LED_INCORRECT_ADDRESS_BLINK_ON, LED_INCORRECT_ADDRESS_BLINK_OFF, eIndPhInvert );
            Indication_off( &ledPlusPos );
            Indication_off( &ledMinusPos );
            Indication_off( &ledLossCtrl );
        }
    }
    return true;
}

//*****************************************************************************
// ����������� ��������� �������������� ��������
//*****************************************************************************
/// \brief ����������� ��������� ��������� �������� "��������� �������������".
///
const FsmState StateInitial =
{
    StateInitial_onEntry, StateInitial_onExit, StateInitial_onRun,
    &StateTop, &StateTop, 0, 0, eStateInitialId
};

//*****************************************************************************
/**
* ������� ���������: 
*
* ������ 1.0.1
* ����   28-09-2017
* �����  ������������ �.�., ��������� �.�.
*
* ���������:
*    ������� ��������.
*/
