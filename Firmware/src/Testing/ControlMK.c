/**
* \file    ControlMK.c
* \brief   \copybrief ControlMK.h
*
* \version 1.0.1
* \date    11-07-2016
* \author  ��������� �.�.
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

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ���������� ���������� ControlMK
void ControlMK_ctor( void )
{
#ifndef IGNORE_CONTROL_MK
    CheckRAM_ctor( );
    #ifdef FLASH_CHECK_ENABLE
        FlashCheck_ctor( );
    #endif
#endif
}

//*****************************************************************************
// �������� ����������������� ��
void ControlMK_run( void )
{
#ifndef IGNORE_CONTROL_MK
    ASSERT_ID( eGrPS_ControlMK, ePS_ControlMkFaultCPU, CheckCPU_run());
    ASSERT_ID( eGrPS_ControlMK, ePS_InterruptCPU, Main_getTimeWorkInterrupt() == 0 );
    ASSERT_ID( eGrPS_ControlMK, ePS_ControlMkFaultRegisters, CheckRegisters_run());
    #ifdef FLASH_CHECK_ENABLE
        FlashCheck_run( );
    #endif
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
*/
