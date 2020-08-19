/**
* \file    relayCtrl.h
* \brief   ���������� ���� � �������� ��������� ��������� ���
* \details ���������
*
* \version 1.0.1
* \date    22-05-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef RELAYCTRL_H
#define RELAYCTRL_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdbool.h>
#include <stdint.h>
#include "DeviceAddress.h"
#include "positionDet.h"
#include "shuntShift.h"

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ������������� ���� ���.
///
typedef enum 
{
    eNormal     = 0,        ///< ���������� ���������
    eNotTurnOn,             ///< �� ����������
    eNotTurnOff             ///< �� �����������
} RelayCtrl_RpvState;

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������������� � ������.
/// \param relayType - ��� ������ ����.
///
void RelayCtrl_ctor( RelayModuleType relayType );

//*****************************************************************************
/// \brief ���������� �������.
/// \note ���������� �� �������� ����� � �������� 1 ��.
///
void RelayCtrl_run( void );

//*****************************************************************************
/// \brief ���������� ����������� �������� �������.
/// \param shift - ����������� ��������.
///
void RelayCtrl_setShiftDir( ShiftDirection shift );

//*****************************************************************************
/// \brief ���������� �������� ���������.
/// \param pos - �������������� ���������.
///
void RelayCtrl_setPosDet( PositionDet_State pos );

//*****************************************************************************
/// \brief �������� ������������� ����������� �������� .
/// \return ����������� ��������.
///
ShiftDirection RelayCtrl_getShiftDir( void );

//*****************************************************************************
/// \brief  �������� �������������� ���������.
/// \return �������������� ���������.
///
PositionDet_State RelayCtrl_getPosDet( void );

//*****************************************************************************
/// \brief ���������/���������� ���� ���.
/// \param state - ��������� ��������� ���� ��� (true (��������), false (���������)).
///
void RelayCtrl_switchRpv( bool state );

//*****************************************************************************
/// \brief �������� ��������� ����������� ���� ���.
/// \return ��������� ���� ���.
///
RelayCtrl_RpvState RelayCtrl_getRpvState( void );


#endif

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   22-05-2017
* �����  ��������� �.�.
* 
* ���������:
*     ������� ������.
*/
