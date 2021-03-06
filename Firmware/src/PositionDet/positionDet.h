/**
* \file    positionDet.h
* \brief   ����������� ��������� �������
* \details ���������
*
* \version 1.0.1
* \date    26-12-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef POSITIONDET_H
#define POSITIONDET_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdbool.h>
#include <stdint.h>
#include "DeviceAddress.h"

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief �������� ���������.
///
typedef enum
{
    eDetNon         = 0,    ///< "������ ��������"
    eDetMinus,              ///< "��������� �����"
    eDetPlus,               ///< "��������� ����"
    eDetUndefinedState          ///< ������������� ���������
} PositionDet_State;

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������������� ����������� ��������� ��������.
/// \param relayType - ��� ������ ����.
///
void PositionDet_ctor( RelayModuleType relayType );

//*****************************************************************************
/// \brief ���������� �������.
/// \note ���������� �� �������� ����� � �������� 1 ��.
///
void PositionDet_run( void );

//*****************************************************************************
/// \brief ���������/���������� ����������� ���������.
/// \param on - ��������� ����������� ��������� ������� (true (��������), false (���������).
///
void PositionDet_turnOn( bool on );

//*****************************************************************************
/// \brief �������� ��������� ������.
/// \retval true  - �������;
/// \retval false - ��������.
///
bool PositionDet_isEnable( void );

//*****************************************************************************
/// \brief �������� ������� ��������� �������.
/// \return ������������� ���������.
///
PositionDet_State PositionDet_getPosition( void );

//*****************************************************************************
/// \brief �������� ������ �� ��������� ������� �������� ���������.
/// \return ��������� ������� �������� ���������.
///
PositionDet_State PositionDet_getRequestPositionDet( void );

//*****************************************************************************
/// \brief �������� ������������� ������� ��������.
/// \param pos - ������������� ������� ��������.
///
void PositionDet_setPosDet( PositionDet_State pos );

#endif

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   26-12-2017
* �����  ��������� �.�.
* 
* ���������:
*     ������� ������.
*/
