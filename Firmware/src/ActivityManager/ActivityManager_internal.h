/**
* \file    ActivityManager_internal.h
* \brief   ���������� ������� ActivityManager 
* \details ���������
*
* \version 1.0.1
* \date    19-05-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef ACTIVITYMANAGER_INTERNAL_H
#define ACTIVITYMANAGER_INTERNAL_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "ActivityManager.h"

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������������ ��������� ����� � �������� ��������.
/// \param str - ��������� �� ��������� ���������;
/// \param connect - ��������� ����� (true (���� �����), false (��� �����)).
///
void ActivityManager_setNeighborConnection( ActivityManager_DeviceStr *str, bool connect );

//*****************************************************************************
/// \brief ������������ ��������� ��������� ������� � ����� ������ �� "�����" �������.
/// \param str - ��������� �� ��������� ���������;
/// \param reception - ��������� ��������� ������� (true (���������), false (�� ���������)).
///
void ActivityManager_setNeighborHearsMe( ActivityManager_DeviceStr *str, bool reception );

//*****************************************************************************
/// \brief ������������ ��������� ������ (������� / ����������).
/// \param str - ��������� �� ��������� ���������;
/// \param state - ��������� ������ (true (�������), false (����������)).
///
void ActivityManager_setNeighborState( ActivityManager_DeviceStr *str, bool state );

//*****************************************************************************
/// \brief ������������ ��������� ���������� ������ (�������� / ���������).
/// \param str - ��������� �� ��������� ���������;
/// \param active - ��������� ���������� ������ (true (��������), false (���������)).
///
void ActivityManager_setNeighborActivity( ActivityManager_DeviceStr *str, bool active );

//*****************************************************************************
/// \brief ������������ ��������� ����� ������ �� ���������� RS-422.
/// \param str - ��������� �� ��������� ���������;
/// \param line - ����� ����� �����;
/// \param connect - ��������� ����� (true (���� �����), false (��� �����)).
///
void ActivityManager_setNeighborRS422connect( ActivityManager_DeviceStr *str, Rs422_numLine line, bool connect );

//*****************************************************************************
/// \brief ������������ ��������� ����� ������ � �� �� ���������� RS-422.
/// \param str - ��������� �� ��������� ���������;
/// \param line - ����� ����� �����;
/// \param connect - ��������� ����� ������ (true (���� �����), false (��� �����)).
///
void ActivityManager_setNeighborRS422ctrlSysConnect( ActivityManager_DeviceStr *str, Rs422_numLine line,
                                                     bool connect );

//*****************************************************************************
/// \brief �������� ��������� �������.
/// \param str - ��������� �� ��������� ���������.
/// \retval true  - �������;
/// \retval false - ����������.
///
bool ActivityManager_getState( ActivityManager_DeviceStr *str );

//*****************************************************************************
/// \brief �������� ��������� ����� �� ���������� RS-422.
/// \param str - ��������� �� ��������� ���������;
/// \param line - ����� ����� �����.
/// \retval true  - ���� �����;
/// \retval false - ��� �����.
///
bool ActivityManager_getRS422connect( ActivityManager_DeviceStr *str, Rs422_numLine line );

//*****************************************************************************
/// \brief �������� ��������� ����� c ��.
/// \param str - ��������� �� ��������� ���������;
/// \param line - ����� ����� �����.
/// \retval true  - ���� �����;
/// \retval false - ��� �����.
///
bool ActivityManager_getRS422ctrlSysConnect( ActivityManager_DeviceStr *str, Rs422_numLine line );

//*****************************************************************************
///\brief ������ ��������� ������������� ������ � ��������� ��������� ��������� ����������.
/// \param str - ��������� �� ��������� ���������.
/// \param state - ��������� ������������� ������ (false - � ��������, true - ��������).
/// 
void ActivityManager_writeNeighborIsInWork( ActivityManager_DeviceStr *str, bool state );

#endif 

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   19-05-2017
* �����  ��������� �.�.
* 
* ���������:
*     ������� ������.
*/
