/**
* \file    CheckCallFunctions.h
* \brief   �������� ������ �������
* \details ��������� 
*
* \version 1.0.2
* \date    29-09-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef CheckCallFunctions_h
#define CheckCallFunctions_h

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
// ������� ���� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ����� ������� � ����������� �� �����.
///
#define CALL_FUNCTION( function )    CheckCallFunctions_call( function, #function );

//*****************************************************************************
/// \brief ������������ ���������� �������.
///
#define MARKED_CALL_FUNCTION   CheckCallFunctions_marked( );

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
void CheckCallFunctions_ctor( void );               ///< �������������.

//*****************************************************************************
/// \brief �������� ���������� ��������� �������. 
/// \note ���������� � �������� 1 ��. 
/// \note ��� ����������� ������������� ���������� ������� � ��.
///
void CheckCallFunctions_run( void );

//*****************************************************************************
/// \brief �������� ��� �������� ���������� ��������� �������.
/// \note  ���������� � �������� 62,5 ��� �� 15-�� ����������.
///
void CheckCallFunctions_interrupt( bool sync );

//*****************************************************************************
/// \brief ������������ ���������� �������. 
///
void CheckCallFunctions_marked( void );

//*****************************************************************************
/// \brief ����� ������� � ����������� �� �����.
/// \param pFunction - ��������� �� ����������� �������
/// \param ac - ������, �������������� ����� ��� ����������� �������
///
void CheckCallFunctions_call( void ( *pFunction )( void ), const char *ac );

//*****************************************************************************
/// \brief ������ ��������� #N_STACK_NAME_FUNCTION ���� ������� � \a EEPROM.
/// \param address - ��������� ����� \a EEPROM.
///
uint16_t CheckCallFunctions_saveBlackBox( uint16_t address );

#endif

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����:  11-07-2016
* �����: ��������� �.�.
* 
* ���������:
*     ������� ������.
*
* ������ 1.0.2
* ����   29-09-2017
* �����  ��������� �.�.
* 
* ���������:
*     ��������� �������  CheckCallFunctions_interrupt(bool sync)
*     ��� �������� ���������� ������� � ����������, ��������� ���������� 
*     cCallFunctionsSync � ��������� checkCallFunctions ��� ����������
*     ���������� ���������� ������� � 15-� ����������.
*/