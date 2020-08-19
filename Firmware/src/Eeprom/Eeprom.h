/**
* \file    Eeprom.h
* \brief   ������ � ������� EEPROM 
* \details ���������
*
* \version 2.0.1
* \date    10-02-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef EEPROM_H
#define EEPROM_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
// ���������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define SIZE_EEDATA 4096            ///< ������ ������� ������ \a EEPROM � ������.

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������������� ������ EEPROM.
///
void Eeprom_ctor( void );

//*****************************************************************************
/// \brief ������� ���� ������ � \a EEPROM.
///
void Eeprom_run( void );

//*****************************************************************************
/// \brief ������ �� \a EEPROM 16-������� ��������.
/// \param addressCell � ����� ������ \a EEPROM.
/// \return ����������� ��������.
///
uint16_t Eeprom_read( uint16_t addressCell );

//*****************************************************************************
/// \brief ������ � ������ \a EEPROM 16-������� ��������.
/// \param addressCell � ����� ������ \a EEPROM;
/// \param data � �������� ��� ������.
///
void Eeprom_write( uint16_t addressCell, uint16_t data );

//*****************************************************************************
/// \brief ���������� \a EEPROM � ������ ��� ������ ������. 
/// \details ���������� �������� ���������.
/// \retval true  - ���������� ������;
/// \retval false - ���������� �� ������.
///
bool Eeprom_isReady( void );

#endif 

//*****************************************************************************
/**
* ������� ���������: 
*
* ������ 1.0.1
* �����  ��������� �.�.
*
* ���������:
*    ������� ��������.
* 
* ������ 2.0.1
* ����   10-02-2017
* �����  ��������� �.�.
*
* ���������:
*    ��������� ��� ����� ����� �� dsPIC33.
*/