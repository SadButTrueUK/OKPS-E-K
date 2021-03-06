/**
* \file    relayCtrlDrv.h
* \brief   ������� ���������� ����, ����������� �� ������ ����
* \details ���������
* \details ���������� ���� ���,����� ���� �������� ����� 220�, ���� ����� �����������, 
* \details ���� ���������� ������ � 5 ��������� ����� �������� ��������� �������. 
* \version 1.0.1
* \date    29-03-2018
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef RELAYCTRLDRV_H
#define RELAYCTRLDRV_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdbool.h>

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������������� ������ ����������.
///
void RelayCtrlDrv_ctor( void );

//*****************************************************************************
/// \brief ������������ ���� ��� (�1...�4 ������ 5-��).
/// \param state - ��������� ���� (true (��������), false (���������)).
///
void RelayCtrlDrv_switchRpv( bool state );

//*****************************************************************************
/// \brief ����������� ��������� � L1, L2 (�5 ������ 5-��).
/// \param state - ��������� ��������� (true (�������), false (��������)).
///
void RelayCtrlDrv_switchReceiverL1L2( bool state );

//*****************************************************************************
/// \brief ����������� ��������� � L3, L4 (�6 ������ 5-��).
/// \param state - ��������� ��������� (true (�������), false (��������)).
///
void RelayCtrlDrv_switchReceiverL3L4( bool state );

//*****************************************************************************
/// \brief ��������� ��������������.
/// \details �������� � Slave.
/// \param state - ��������� �������������� (true (�������), false (��������)).
///
void RelayCtrlDrv_switchTermoShunt( bool state );

//*****************************************************************************
/// \brief ��������/��������� ���� � � � (������������� �����).
/// \param level - ��������� ��� (true (��������), false (���������)).
///
void RelayCtrlDrv_switchPhasesAB( bool level );

#endif

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
