/**
* \file    ECAN1_PinRemap.h
* \brief   ��������� ����� �� ������ � ECAN1.
*
* \version 1.0.1
* \date    17-03-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef ECAN1_PINREMAP_H
#define ECAN1_PINREMAP_H

//*****************************************************************************
// ������� ���� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ����� �� ������ � ECAN1.
///
#define ECAN1_PIN_REMAP                                             \
do                                                                  \
{                                                                   \
    TRISDbits.TRISD6  = 0;             /* CAN1 Tx pin directions */ \
    TRISDbits.TRISD5  = 1;             /* CAN1 Rx pin directions */ \
    RPOR8bits.RP70R   = _RPOUT_C1TX;   /* CAN1 Tx pin remap. */     \
    RPINR26bits.C1RXR = 69;            /* CAN1 Rx pin remap. */     \
} while( 0 )

#endif

//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   17-03-2017
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
*/