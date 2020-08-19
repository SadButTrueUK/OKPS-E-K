/**
* \file    Tracing.h
* \brief   ����������� ����� ������� 
* \version 1.0.2
* \date    05-03-2020
* \author  ��������� �.�., ������ �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef TRACING_H
#define TRACING_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "Eeprom.h"

//*****************************************************************************
// ���������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define TRACING_BUF_QTY      7U                                                 ///< ���������� ������� �����������.
#define TRACING_BUF_SIZE   256U                                                 ///< ������ ������ �����������.

// �������� ������������� ������� ������� �����������
#if (ADDRESS_EEPROM_TRACING_START + TRACING_BUF_QTY * TRACING_BUF_SIZE * 2) >= SIZE_EEDATA
    #error "The tracing buffers is too large"
#endif

// �������� �� ��������� ������� ������ ����������� ����� 16-��
#if (TRACING_BUF_SIZE & 0x000F) != 0                                            
    #error "The size of tracing buffer must be multiple 16"
#endif

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������������� ������.
///
void Tracing_ctor( void );

//*****************************************************************************
/// \brief ���������� ������� ����������� � \a EEPROM.
///
void Tracing_saveBlackBox( void );

//*****************************************************************************
/// \brief ������ ������ � ����� �����������.
/// \param data - ������;
/// \param numberArray - ����� ������ ����������� 0...( TRACING_BUF_QTY-1 ).
///
void Tracing_parameter( uint16_t data, uint16_t numberArray );

//*****************************************************************************
/// \brief ������ ������ � ����� ����������� � ���������������.
/// \param data - ������;
/// \param id - ������������� ������;
/// \param numberArray - ����� ������ ����������� 0...( TRACING_BUF_QTY-1 ).
///
void Tracing_parameterId( uint16_t data, uint16_t id, uint16_t numberArray );

#endif

//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   24-03-2016
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
*  
* ������ 1.0.2
* ����   05-03-2020
* �����  ������ �.�.
* ��������� ���������� ������� �� ����. ����� �������� �������� ������
*
*/