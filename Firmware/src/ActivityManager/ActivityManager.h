/**
* \file    ActivityManager.h
* \brief   ���������� ����������� ������� 
* \details ���������
*
* \version 1.0.1
* \date    18-05-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdbool.h>
#include <stdint.h>
#include "Rs422.h"

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
typedef struct ActivityManager_DeviceStr_ ActivityManager_DeviceStr;        ///< ����������� ���� ��������� ���������.

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief �������������.
/// \param status � ��������� ������� (true (��������), false (���������)).
/// \return ��������� �� ��������� ���������,
/// ������� ������ ������������ �� ��� ������������ �������
/// \a ActivityManager � �������� ���������.
///
ActivityManager_DeviceStr *ActivityManager_ctor( bool status );

//*****************************************************************************
/// \brief ���������� ������� ������.
/// \note ���������� �� �������� ����� � �������� 1 ��.
/// \param str - ��������� �� ��������� ���������.
///
void ActivityManager_run( ActivityManager_DeviceStr *str );

//*****************************************************************************
/// \brief ��������� / ���������� ������.
/// \param str - ��������� �� ��������� ���������;
/// \param enable - ��������� ������ (true (�������), false (��������)).
///
void ActivityManager_setEnable( ActivityManager_DeviceStr *str, bool enable );

//*****************************************************************************
/// \brief ��������� ��������� �������.
/// \param str - ��������� �� ��������� ���������;
/// \param state - ��������������� ��������� ������ �������  (true (�������), false (����������)).
///
void ActivityManager_setState( ActivityManager_DeviceStr *str, bool state );

//*****************************************************************************
/// \brief ��������� ��������� ��������� ���� ���.
/// \param str - ��������� �� ��������� ���������;
/// \param state - ��������������� ��������� ������ ������� (true (�������), false (����������)).
///
void ActivityManager_setRPVoutState( ActivityManager_DeviceStr *str, bool state );

//*****************************************************************************
/// \brief ��������� ������� �������� �������.
/// \param str - ��������� �� ��������� ���������;
/// \param presence - ��������� ������� (true (���� �������), false (��� �������)).
///
void ActivityManager_setActiveCommand( ActivityManager_DeviceStr *str, bool presence );

//*****************************************************************************
/// \brief ��������� ��������� ����� �� ���������� RS-422.
/// \param str - ��������� �� ��������� ���������;
/// \param line - ����� ����� �����;
/// \param connect - ��������� ����� (true (���� �����), false (��� �����)).
///
void ActivityManager_setRS422connect( ActivityManager_DeviceStr *str, Rs422_numLine line, bool connect );

//*****************************************************************************
/// \brief ��������� ��������� ����� � �� �� ���������� RS-422.
/// \param str - ��������� �� ��������� ���������;
/// \param line - ����� ����� �����;
/// \param connect - ��������� ����� (true (���� �����), false (��� �����)).
///
void ActivityManager_setRS422ctrlSysConnect( ActivityManager_DeviceStr *str, Rs422_numLine line, bool connect );

//*****************************************************************************
/// \brief �������� ��������� ���������� �������.
/// \param str - ��������� �� ��������� ���������.
/// \retval true  - ��������;
/// \retval false - ���������.
///
bool ActivityManager_isActive( ActivityManager_DeviceStr *str );

//*****************************************************************************
/// \brief �������� ��������� ����� ������ c ��.
/// \param str - ��������� �� ��������� ���������;
/// \param line - ����� ����� �����.
/// \retval true  - ���� �����;
/// \retval false - ��� �����.
///
bool ActivityManager_getRS422neighborCtrlSysConnect( ActivityManager_DeviceStr *str, Rs422_numLine line );

//*****************************************************************************
/// \brief �������� ��������� ���������� ����.
/// \param str - ��������� �� ��������� ���������.
/// \retval true  - ��������;
/// \retval false - ���������.
///
bool ActivityManager_isRelayCtrlOn( ActivityManager_DeviceStr *str );

//*****************************************************************************
/// \brief �������� ��������� ������.
/// \param str - ��������� �� ��������� ���������.
/// \retval true  - �������;
/// \retval false - ����������.
///
bool ActivityManager_isNeighborWorking( ActivityManager_DeviceStr *str );

//*****************************************************************************
/// \brief �������� ��������� ����� � �������.
/// \param str - ��������� �� ��������� ���������.
/// \retval true  - ���� �����;
/// \retval false - ��� �����.
///
bool ActivityManager_getNeighborConnection( ActivityManager_DeviceStr *str );

//*****************************************************************************
///\brief ����� ���������� �� ������� �� ����������� �������.
/// \param str - ��������� �� ��������� ���������.
/// \param value - ��������� ��������� ����������, ������������� � �������.
///
void ActivityManager_changeActivity( ActivityManager_DeviceStr *str, uint8_t value );

//*****************************************************************************
///\brief �������� ��������� ������������� ��������� ���������� ������ �������.
/// \param str - ��������� �� ��������� ���������.
/// \retval true  - ������������� ���������.
/// \retval false - ������������� �� ���������.
bool ActivityManager_getInitDeviceState( ActivityManager_DeviceStr *str );

#endif

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   18-05-2017
* �����  ��������� �.�.
* 
* ���������:
*     ������� ������.
*/
