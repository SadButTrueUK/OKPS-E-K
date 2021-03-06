/**
* \file    relayCtrlDrv.c
* \brief   \copybrief relayCtrlDrv.h
*
* \version 1.0.1
* \date    29-03-2018
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "typeMK.h"
#include "relayCtrlDrv.h"
#include "ConfigMK.h"


//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ������ ���������� 
void RelayCtrlDrv_ctor( void )
{
    //���� ���
    _TRISC7 = 0;
    _LATC7 = 1;
    //����������� ��������� � ���1, ���2 (�5 ������ 5-��)
    _TRISD13 = 0;
    _LATD13 = 1;
    //����������� ��������� � ���3, ���4 (�6 ������ 5-��)
    _TRISD12 = 0;
    _LATD12 = 1;
    //���������-���������� ���������� ������������ ����������  
    _TRISC8 = 0;
    _LATC8 = 1;

    if( ConfigMK_isSlave( ) ) // Slave
    {
        //��������� ��������������
        _TRISG13 = 0;
        _LATG13 = 1;
    }
}

//*****************************************************************************
// ������������ ���� ��� (�1...�4 ������ 5-��)
void RelayCtrlDrv_switchRpv( bool state )
{
    _LATC7 = !state;
}

//*****************************************************************************
// ����������� ��������� � L1, L2 (�5 ������ 5-��)
void RelayCtrlDrv_switchReceiverL1L2( bool state )
{
    _LATD13 = !state;
}

//*****************************************************************************
// ����������� ��������� � L3, L4 (�6 ������ 5-��)
void RelayCtrlDrv_switchReceiverL3L4( bool state )
{
    _LATD12 = !state;
}

//*****************************************************************************
// ��������/��������� ���� � � � (������������� �����).
void RelayCtrlDrv_switchPhasesAB( bool level )
{

}

//*****************************************************************************
// ��������� �������������� (�������� ������ � Slave).
void RelayCtrlDrv_switchTermoShunt( bool state )
{
        if( ConfigMK_isSlave( ) )  _LATG13 = !state;
}

//*****************************************************************************
/**
* ������� ���������: 
*
* ������ 1.0.1
* ����   29-03-2018
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
*/
