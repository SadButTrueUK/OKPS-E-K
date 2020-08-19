/**
* \file    CheckRegisters.c
* \brief   \copybrief CheckRegisters.h
*
* \version 2.0.1
* \date    05-07-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "asserts.h"
#include "ProtectionState_codes.h"
#include "checkRegistersDrv.h"
#include "CheckRegisters.h"

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �������� �������� ���������, ������������ ��� ������
bool CheckRegisters_run( void )
{
    return checkRegistersDrv();
}

//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   05-07-2015
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
*
* ������ 2.0.1
* ����   05-07-2017
* �����  ��������� �.�.
*
* ���������:
*    ��������� ��� ����� ����� �� dsPIC33.
*/
