/**
* \file    checkRegistersDrv.c
* \brief   \copybrief checkRegistersDrv.h
*
* \version 2.0.1
* \date    26-09-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "typeMK.h"
#include "MainRegisters.h"
#include "Adc_Inic1.h"
#include "checkRegistersDrv.h"

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �������� �������� ���������, ������������ ��� ������
bool checkRegistersDrv( void )
{
    bool ret = true;

    // ������� ������
    PR5 = TIME_PERIOD_INTERRUPT;
    if( ( T5CON != 0x8000 ) || ( PR5 != TIME_PERIOD_INTERRUPT ) )
    {
        ret = false;
    }

    // ��������� ����������
    if( ( IPC7bits.T5IP != 3 ) || ( IPC4bits.CNIP != 4 ) || ( IPC3bits.AD1IP != 1 ) ||
        ( IPC6bits.T4IP != 2 ) || ( INTCON1bits.NSTDIS != 0 ) )
    {
        ret = false;
    }

    // ���
    if( ( ( AD1CON1 & 0xFFFE ) != ( INIC_ADC1_ADCON1 | 0x8000 ) ) ||
        ( ( AD1CON2 & 0xFF7F ) != INIC_ADC1_ADCON2 ) ||
        ( AD1CON3 != INIC_ADC1_ADCON3 ) ||
        ( AD1CON4 != INIC_ADC1_ADCON4 ) ) 
    {
        ret = false;
    }

    // CAN1 - ������������ �������������
    if( ( ( C1CTRL1 & 0xE81E ) != 0x2808 ) ||
        ( C1CTRL2 != 0 ) ||
        ( C1CFG1 != 7 ) ||
        ( C1FCTRL != 0xC000 ) ||
        ( ( C1TR01CON & 0x8383 ) != 0x0083 ) ||
        ( C1FEN1 != 0xFFFF ) )
    {
        ret = false;
    }

    // DMA CAN1 - ������������ �������������
    if( ( DMA0CON != 0xA020 ) ||
        ( DMA0REQ != 0x0046 ) ||
        ( DMA0CNT != 0x0007 ) ||
        ( DMA0PAD != 0x0442 ) )
    {
        ret = false;
    }
    if( ( DMA1CON != 0x8020 ) ||
        ( DMA1REQ != 0x0022 ) ||
        ( DMA1CNT != 0x0007 ) ||
        ( DMA1PAD != 0x0440 ) )
    {
        ret = false;
    }

    return ret;
}

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
* 
* ������ 2.0.1
* ����   26-09-2017
* �����  ��������� �.�.
* 
* ���������:
*    ��������� ��� ����� ����� �� dsPIC33.
*/
