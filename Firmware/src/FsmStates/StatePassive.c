/**
* \file    StatePassive.c
* \brief   ��������� ���������.
*
* \version 1.0.1
* \date    28-09-2017
* \author  ������������ �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "Main.h"
#include "ControlSystem.h"
#include "States.h"

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ����� � ��������� ���������.
///
static void StatePassive_onEntry( void );

//*****************************************************************************
/// \brief ��������� ������ �� ���������� ���������.
///
static void StatePassive_onExit( void );

//*****************************************************************************
/// \brief ��������� ���������� ���������.
/// \note ��� ��������� ���������� ���������� ������� � �������� ���������.
/// \note ��� ������������ ������� �������� ������ ���������� ������� � ���������� ��������� ���������.
///
static bool StatePassive_onRun( void );

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
// ��������� ����� � ��������� ���������
static void StatePassive_onEntry( void )
{

}

//*****************************************************************************
// ��������� ������ �� ���������� ���������
static void StatePassive_onExit( void )
{

}

//*****************************************************************************
// ��������� ���������� ���������
static bool StatePassive_onRun( void )
{

    if( ActivityManager_isActive( activityManager ) )
    {
        Fsm_transit( &StateActive );
        return false;
    }
    if( !ControlSystem_workCondition( ) )
    {
        Fsm_transit( &StateSafePassive );
        return false;
    }

    return true;
}

//*****************************************************************************
// ����������� ��������� �������������� ��������
//*****************************************************************************

//*****************************************************************************
/// \brief ����������� ��������� ��������� �������� "��������� ���������".
///
const FsmState StatePassive =
{
    StatePassive_onEntry, StatePassive_onExit, StatePassive_onRun,
    &StateTop, &StateWorking, 0, 0
};

//*****************************************************************************
/**
* ������� ���������: 
*
* ������ 1.0.1
* ����   28-09-2017
* �����  ������������ �.�.
*
* ���������:
*    ������� ��������.
*/