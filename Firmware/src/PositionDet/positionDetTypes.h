/**
* \file    positionDetTypes.h
* \brief   ��������� ������� ����������� ���������.
*
* \version 1.0.1
* \date    28-12-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef POSITIONDETTYPES_H
#define POSITIONDETTYPES_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "positionDet.h"

//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief �������� ������� �������������.
///
typedef void( *pfPosDetCtor )( void );

//*****************************************************************************
/// \brief �������� ������� �������� ������.
///
typedef void( *pfPosDetRun )( void );

//*****************************************************************************
/// \brief �������� ������� ���������/���������� ����������� ���������.
/// \param on - ���������� ������������ ��������� ������� (true (��������), false (���������)).
///
typedef void( *pfPosDetTurnOn )( bool on );

//*****************************************************************************
/// \brief �������� ������� �������� ���������.
///
typedef bool( *pfPosDetIsEnable )( void );

//*****************************************************************************
/// \brief �������� ������� ����������� ���������� ����������� ���������.
///
typedef PositionDet_State( *pfPosDetGetPosition )( void );

//*****************************************************************************
/// \brief �������� ������� ������� �� ��������� ������� �������� ���������.
///
typedef PositionDet_State( *pfPosDetGetReqPosDet )( void );

//*****************************************************************************
/// \brief �������� ������� �������� ������������� ������� ��������.
/// \param pos - ������������� ������� �������� ��������� �������. 
///
typedef void( *pfPosDetSetPosDet )( PositionDet_State pos );

//*****************************************************************************
/// \brief ��������� ����������� ���������� ������.
///
typedef struct ArrayPositionDet 
{
    pfPosDetCtor         ctor;                ///< �������������.
    pfPosDetRun          run;                 ///< ���������� �������.
    pfPosDetTurnOn       turnOn;              ///< ���������/���������� ����������� ���������.
    pfPosDetIsEnable     isEnable;            ///< �������� ���������.
    pfPosDetGetPosition  getPos;              ///< ����������� ����������.
    pfPosDetGetReqPosDet getReqPosDet;        ///< ������ �� ��������� ������� ��������.
    pfPosDetSetPosDet    setPosDet;           ///< �������� ������������� ������� ��������.
} ArrayPositionDet;

#endif

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   28-12-2017
* �����  ��������� �.�.
* 
* ���������:
*     ������� ������.
*/
