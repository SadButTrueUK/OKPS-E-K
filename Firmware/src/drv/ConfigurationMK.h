/**
* \file    ConfigurationMK.h
* \brief   ������������ ��.
*
* \version 1.0.1
* \date    19-01-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef CONFIGURATIONMK_H
#define CONFIGURATIONMK_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "typeMK.h"

//*****************************************************************************
// ���������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
// ������������ ��
///
// FICD
#pragma config ICS     = NONE
#pragma config JTAGEN  = OFF           // JTAG ��������.

// FPOR
#pragma config WDTWIN  = WIN25         // ����� ���� ����������� ������� ������ 25% �� ��� �������.
#pragma config BOREN   = ON            // �������� �������� ������� ��.
#pragma config ALTI2C1 = OFF           // �������������� ���� I2C1 (ASDA1/ASCL1) ���������.
#pragma config ALTI2C2 = OFF           // �������������� ���� I2C2 (ASDA2/ASCL2) ���������.

// FWDT
#pragma config WDTPRE  = PR32          // ����� ������������ �� 1 ��.
#pragma config WDTPOST = PS1024        // ����� �������� �� 1024. ����� 1024 ��. 
#pragma config PLLKEN  = ON            // �������� ������������ ������� PLL ��������� � ����� ������������ �� ����.
#pragma config WINDIS  = OFF           // ����� ���� ��� ����������� ������� ��������.
#pragma config FWDTEN  = OFF           // ���������� ����������� ������� �� ���������.

// FOSC
#pragma config POSCMD  = EC            // ������� �������� ���������.
#pragma config OSCIOFNC= OFF           // �� OSC2 ������� �� �������.
#pragma config IOL1WAY = OFF           // �������������� ����� ���������.
#pragma config FCKSM   = CSECMD        // ������������ ��������� ���������� ���������

// FOSCSEL
#pragma config FNOSC   = FRC           // �� ������ ��������� �������� �������� �� FRC-����������.
#pragma config PWMLOCK = OFF           // ��� �������� ����� �������� ��� ����������� ������������������.
#pragma config IESO    = OFF           // ����� ������ � ��������� ������������, ������� �� ������ ���������� ��������.

// FGS
#pragma config GWRP    = OFF           // ��������� ������ ������.
#pragma config GCP     = OFF           // ��������� ������ ������.

#endif

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   19-01-2017
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
*/
