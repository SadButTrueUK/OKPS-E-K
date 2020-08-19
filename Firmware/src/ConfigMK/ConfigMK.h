/**
* \file    ConfigMK.h
* \brief   ����������� ������������ ���������� 
* \details ���������
* 
* \version 1.0.1
* \date    21-02-2018
* \author  ������ �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef CONFIGMK_H
#define CONFIGMK_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdbool.h>

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief ����������� ���� ����������: Master ��� Slave.
/// 
void ConfigMK_ctor( void );

//*****************************************************************************
/// \brief ����������� ������������ ���������� ��� Master.
/// \retval true  - ��������� ��������������� ��� Master;
/// \retval false - ��������� ��������������� ��� Slave.
/// 
bool ConfigMK_isMaster( void );

//*****************************************************************************
/// \brief ����������� ������������ ���������� ��� Slave.
/// \retval true  - ��������� ��������������� ��� Slave;
/// \retval false - ��������� ��������������� ��� Master.
/// 
bool ConfigMK_isSlave( void );

#endif

//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   21-02-2018
* �����  ������ �.�.
*
* ���������:
*    ������� ������.
*/
