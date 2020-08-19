/**
* \file    dsPIC33_rs422.h
* \brief   ������� ������ RS-422 ��� �� dsPIC33 
* \details ���������
* \details ������ �������� �������, ����������� ��� ������������� 
*          ���������� UART � ����������� ������ �� ���������� RS-422.
*
* \version 1.0.1
* \date    14-05-2018
* \author  ������ �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef DSPIC33_RS422_H
#define DSPIC33_RS422_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdbool.h>
#include "typeMK.h"

//*****************************************************************************
// ��������� ������������ ������� 
//*****************************************************************************

//*****************************************************************************
/// \brief ������������� ��������� UART1 � ���� DE1.
///
void init_L1( void );

//*****************************************************************************
/// \brief ������������� ��������� UART2 � ���� DE2.
///
void init_L2( void );

//*****************************************************************************
/// \brief ���������� �������/��������� ������ �� RS-422, ����� 1.
/// \param dir - ����������� ������: 0 - �����, 1 - ��������.
///
void dirL1( bool dir );

//*****************************************************************************
/// \brief ����� ����� ������ �� RS-422 UART1.
/// \param data - ��������� �� ����������, � ������� ��������� �������� ����.
/// \retval true  - ����� ���;
/// \retval false - ������ �� ����.
///
bool RxByte_L1( uint8_t *data );

//*****************************************************************************
/// \brief �������� ����� ������ �� RS-422 UART1.
/// \param data - ���� ��� ��������.
/// \retval true  - ���� ������� � ����� \a UART ��� ��������;
/// \retval false - ��� ���������� �����������.
///
bool TxByte_L1( uint8_t data );

//*****************************************************************************
/// \brief ����������� ��������� �������� �� RS-422 UART1.
/// \retval true  - �������� ���������;
/// \retval false - �������� �� ���������.
///
bool isTxCompl_L1( void );

//*****************************************************************************
/// \brief ���������� �������/��������� ������ �� RS-422 UART2.
/// \param dir - ����������� ������: 0 - �����, 1 - ��������.
///
void dirL2( bool dir );

//*****************************************************************************
/// \brief ����� ����� ������ �� RS-422 UART2.
/// \param data - ��������� �� ����������, � ������� ��������� �������� ����.
/// \retval true  - ����� ���;
/// \retval false - ������ �� ����.
///
bool RxByte_L2( uint8_t *data );

//*****************************************************************************
/// \brief �������� ����� ������ �� RS-422 UART2.
/// \param data - ���� ��� ��������.
/// \retval true  - ���� ������� � ����� \a UART ��� ��������;
/// \retval false - ��� ���������� �����������.
///
bool TxByte_L2( uint8_t data );

//*****************************************************************************
/// \brief ����������� ��������� �������� �� RS-422 UART2.
/// \retval true  - �������� ���������;
/// \retval false - �������� �� ���������.
///
bool isTxCompl_L2( void );

#endif

//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   14-05-2018
* �����  ������ �.�.
*
* ���������:
*    ������� ������.
*/
