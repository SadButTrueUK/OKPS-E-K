/**
* \file    ControlMK.c
* \brief   \copybrief ControlMK.h
*
* \version 1.0.2
* \date    15-03-2021
* \author  ������ �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "typeMK.h"
#include "defCompil.h"
#include "asserts_ex.h"
#include "InterChannel.h"
#include "Main.h" 
#include "ControlMK.h"
#include "FlashCheck.h"
#include "CheckRAM.h"
#include "CheckCPU.h"
#include "CheckRegisters.h"
#include "CheckCallFunctions.h"


//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ���������� ���������� ControlMK
void ControlMK_ctor( void )
{
#ifndef IGNORE_CONTROL_MK
    CheckRAM_ctor( );
    #ifndef IGNORE_CHECK_FLASH
        FlashCheck_ctor( );
    #endif
    CheckCallFunctions_ctor();
#endif
}

//*****************************************************************************
// �������� ����������������� ��
void ControlMK_run( void )
{
#ifndef IGNORE_CONTROL_MK
    ASSERT_ID( eGrPS_ControlMK, ePS_ControlMkFaultCPU, CheckCPU_run( ) );
    ASSERT_ID( eGrPS_ControlMK, ePS_InterruptCPU, Main_getTimeWorkInterrupt( ) == 0 );
    ASSERT_ID( eGrPS_ControlMK, ePS_ControlMkFaultRegisters, CheckRegisters_run( ) );
    #ifndef IGNORE_CHECK_FLASH
        FlashCheck_run( );
    #endif
    CheckCallFunctions_run( );
    MARKED_CALL_FUNCTION;
#endif
}

//*****************************************************************************
/**
* ���������:
*    ������ ControlMK �������� ������� ������� �������� ��.
*    � ��� ������ ������ ����� ������� ��������, �������� � �������� ����������
*    ���������� ����������� �������� �� ���� ��.
*/

//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   11-07-2016
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
* 
* ������ 1.0.2
* ����   15-03-2021
* �����  ������ �.�.
* 
* ���������:
*   ��������� ������� � ���������� ���������� �������� � ����� defCompil.h
*/
