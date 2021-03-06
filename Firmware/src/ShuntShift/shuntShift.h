/**
* \file    shuntShift.h
* \brief   ���������� ��������� �������
* \details ���������
*
* \version 1.0.1
* \date    02-02-2018
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef SHUNTSHIFT_H
#define SHUNTSHIFT_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdbool.h>
#include <stdint.h>
#include "DeviceAddress.h"
#include "positionDet.h"

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ����������� ��������.
///
typedef enum 
{
    eShiftStop  = 0,        ///< ���������
    eShiftPlus,             ///< � "��������� ����"
    eShiftMinus             ///< � "��������� �����"
} ShiftDirection;

//*****************************************************************************
/// \brief ��������� ��������.
///
typedef enum 
{
    eShiftNormal     = 0,         ///< ���������� ��������� � ������� ��������
    eShiftActive,                 ///< ������� �������� �������
    eShiftSecondTry,              ///< ������� ����������� �� 2-�� �������
    eShiftFail                    ///< ������� ���������� ��������
} ShiftDgn;

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief �������������.
/// \param relayType - ��� ������ ����;
/// \param motor     - ��� ����������������.
///
void ShuntShift_ctor( RelayModuleType relayType, MotorType motor );

//*****************************************************************************
/// \brief ���������� �������.
/// \note ���������� �� �������� ����� � �������� 1 ��.
///
void ShuntShift_run( void );

//*****************************************************************************
/// \brief ��������� �������� �������.
/// \param dir  - ����������� ��������(true (+), false (-));
/// \param time - ��������������� ����� ��������, ��;
/// \param st   - ��������� ������ (true (�� ������), false (�� �� ������));
/// \param rs   - ����� ���������� �������� (true (�������), false (��������)).
///
void ShuntShift_turnOn( bool dir, uint16_t time, bool st, uint8_t rs );

//*****************************************************************************
/// \brief ���������� �������� �������.
/// 
void ShuntShift_turnOff( void );

//*****************************************************************************
/// \brief �������� ��������� �������.
/// \param detPos - ������������� ������� �������� ���������;
/// \param pos    - ����������� ��������� �������.
///
void ShuntShift_setDetPos( PositionDet_State detPos, PositionDet_State pos );

//*****************************************************************************
/// \brief �������� ������������� ������� ��������.
/// \param shiftPos - ������������� ������� ��������.
///
void ShuntShift_setShiftPos( ShiftDirection shiftPos );

//*****************************************************************************
/// \brief �������� ��������� �������� �������.
/// \retval true  - �������;
/// \retval false - ��������.
///
bool ShuntShift_isEnable( void );

//*****************************************************************************
/// \brief ������ �� ��������� ������� �������� ���������.
/// \return ������� �������� ��������� 
/// \retval 0 - ������ ��������;
/// \retval 1 - ��������� ����;
/// \retval 2 - ��������� �����;
/// \retval 3 - ������������� ���������.
///
PositionDet_State ShuntShift_getReqPosDet( void );

//*****************************************************************************
/// \brief ������ �� ��������� ������� ����������� ��������.
/// \return ������� ����������� ��������.
///
ShiftDirection ShuntShift_getReqShiftDir( void );

//*****************************************************************************
/// \brief �������� ����� ���������� ��������.
/// \return ����� ���������� ��������, ��.
///
uint16_t ShuntShift_getTime( void );

//*****************************************************************************
/// \brief �������� �������� ���� ������� dgn.
/// \return �������� dgn.
///
ShiftDgn ShuntShift_getDgn( void );


//*****************************************************************************
/// \brief ���������� �������� �������� ���� ����������� ��������.
/// \param val - ��������������� ��������. 
///
void ShuntShift_setCntPhaseShift( const uint8_t val );

//*****************************************************************************
/// \brief �������� �������� �������� ���� ����������� ��������.
/// \return - �������� �������� ���� ����������� �������� 
///
const uint8_t ShuntShift_getCntPhaseShift( void );


//*****************************************************************************
/// \brief �������� ����, ��� ������ ������ ����� �������������
/// \return true - ������ � ������� ������ , false - ������ �  ������ ������������� 
///
const bool ShuntShift_isWorkMode( void );

#endif

//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   02-02-2018
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
*/
