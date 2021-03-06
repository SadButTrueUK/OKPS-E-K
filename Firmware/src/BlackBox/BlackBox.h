/**
* \file    BlackBox.h
* \brief   ������������ � ������ ���������� �� 
* \details ���������
*
* \version 2.0.1
* \date    06-04-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef BLACKBOX_H
#define BLACKBOX_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdint.h>

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� � \a EEPROM ���������� ��.
///
void BlackBox_save( void );

//*****************************************************************************
/// \brief ������ �� \a EEPROM ���������� �� � ������ �� � ����� �����.
///
void BlackBox_read( void );

//*****************************************************************************
/// \brief ��������� ��������, ���������� ������� ��.
/// \note ����������, ���������� ���, ����� �������� �� ���������� ����������.
/// \param addCodePS - ��������, ���������� ������� ��;
/// \param number - ������ � ������� 0-3.
/// 
void BlackBox_saveAddCodeProtectionState( uint16_t addCodePS, uint8_t number );

//*****************************************************************************
/// \brief ��������� ��������, ���������� ������� ������� ��.
/// \param data - ��������, ���������� ������� ������� ��.
///
void BlackBox_saveSourceReset( uint16_t data );

//*****************************************************************************
/// \brief ��������� ��� ����� � ����� ������ \a ASSERT, ���������� ��.
/// \param nameFile - ��� �����;
/// \param line - ����� ������.
///
void BlackBox_saveASSERT( char const * const nameFile, int16_t line );

#endif

//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   22-01-2016
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
*
* ������ 2.0.1
* ����   06-04-2017
* �����  ��������� �.�.
*
* ���������:
*    ��������� ��� ����� ����� �� dsPIC33.
*/
