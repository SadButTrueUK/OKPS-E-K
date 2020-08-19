/**
* \file    defCompil.h
* \brief   ����������� ������� ����������.
*
* \version 1.0.1
* \date    13-01-2016
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef DEFCOMPIL_H
#define DEFCOMPIL_H

//*****************************************************************************
// ���������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������ ����������� \a EEPROM � \a CAN ��� �������������.
///
#define DEBUG_BLACK_BOX_READ_INIC 

//*****************************************************************************
/// \brief ������ ����������� \a EEPROM � \a CAN ��� �������� � ��.
///
#define DEBUG_BLACK_BOX_READ_PS    

//*****************************************************************************
//#define IGNORE_CONTROL_MK 
//#define IGNORE_PROTECTION_STATE
//#define IGNORE_CHECK_PROTECTION_STATE    ///< ��������� ������� � �� (�������������� ��� ���������� �� build-������).
//#define IGNORE_TIME_BEGIN_SYNC           ///< ����������� �������� ���. ������������� ��� ������ ���������.
#define FLASH_CHECK_ENABLE                 ///< ���������� �������� ����������� ����� ��� 
#define INTERCHANNEL_DEBUG_MODE_ON     1                               ///< ��������� ����������� ������.
#define INTERCHANNEL_DEBUG_MODE_OFF    0                               ///< ���������� ����������� ������.
#define INTERCHANNEL_DEBUG_MODE        INTERCHANNEL_DEBUG_MODE_ON      ///< ��������� ����������� ������.
//#define INTERCHANNEL_DEBUG_MODE      INTERCHANNEL_DEBUG_MODE_OFF     ///< ��������� ����������� ������.

//*****************************************************************************
/// \brief ������ �������, � ������� ������� ���c�������� ���������.
///
#define INTERCHANNEL_DEBUG_TRACING_ARRAY 6

//*****************************************************************************
#define INTERCHANNEL_DEBUG_TRACING_PARAM eICId_BinInPin         ///< ������������ ��������.

//*****************************************************************************
/// \brief ���������� ��������� �� �������� ���������� "�����".
///
#define IND_SHORT_CIRCUIT

#endif

//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   13-01-2016
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
*/