/**
* \file    shuntShiftTypes.h
* \brief   ��������� ������� ���������� ���������.
*
* \version 1.0.1
* \date    06-11-2018
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef SHUNTSHIFTTYPES_H
#define SHUNTSHIFTTYPES_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "shuntShift.h"
#include "positionDet.h"

//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief �������� ������� �������������.
/// 
typedef void( *pfShuntShiftCtor )( void );

//*****************************************************************************
/// \brief �������� ������� ���������� �������.
/// 
typedef void( *pfShuntShiftRun )( void );

//*****************************************************************************
/// \brief �������� ������� ����������.
/// 
typedef void( *pfShuntShiftTurnOff )( void );

//*****************************************************************************
/// \brief �������� ������� ������� �� ��������� ������� �������� ���������.
///
typedef PositionDet_State( *pfShuntShiftGetReqPosDet )( void );

//*****************************************************************************
/// \brief �������� ������� ������� �� ��������� ������� ����������� ��������.
///
typedef ShiftDirection( *pfShuntShiftGetReqShiftDir )( void );

//*****************************************************************************
/// \brief �������� ������� ��������� ������� ���������� ��������.
///
typedef uint16_t( *pfShuntShiftGetTime )( void );

//*****************************************************************************
/// \brief �������� ������� ��������� ��������� ��������.
///
typedef ShiftDgn( *pfShuntShiftGetDgn )( void );

//*****************************************************************************
/// \brief �������� ������� �������� ��������� ���������.
///
typedef bool( *pfShuntShiftIsEnable )( void );

//*****************************************************************************
/// \brief �������� ��������� �� ������� ���������.
/// \param dir  - ��������� ����������� (true (+), false (-));
/// \param time - ��������������� ����� ��������, ��;
/// \param st   - ��������� ������ (true (�� ������), false (�� �� ������));
/// \param rs - ����� ���������� �������� �������/��������.
///
typedef void( *pfShuntShiftTurnOn )( bool dir, uint16_t time, bool st, uint8_t rs );

//*****************************************************************************
/// \brief �������� ��������� �� ������� �������� ���������.
/// \param detPos   - ������������� ������� �������� ���������;
/// \param position - ����������� ��������� �������.
///
typedef void( *pfShuntShiftSetDetPos )( PositionDet_State detPos, PositionDet_State position );

//*****************************************************************************
/// \brief �������� ��������� �� ������� �������� ������������� ������� ��������.
/// \param shiftPos - ������������� ������� ��������.
///
typedef void( *pfShuntShiftSetShiftPos )( ShiftDirection shiftPos );

//*****************************************************************************
/// \brief �������� ��������� �� ������� ��������� �������� �������� ���� ����������� ��������.
/// \param val - ��������������� ��������. 
///
typedef void ( *pfSetCntPhaseShift ) (const uint8_t val);

//*****************************************************************************
/// \brief �������� ��������� �� ������� ��������� �������� �������� ���� ����������� ��������.
/// \return - �������� �������� ���� ����������� �������� 
///
typedef const uint8_t ( *pfGetCntPhaseShift ) ( void );

//*****************************************************************************
/// \brief �������� ��������� �� ������� ��������� ���� dgn ���������� �������
/// \param val - ��������������� ��������. 
///
typedef const bool ( *pfShuntShiftIsWorkMode) ( void );

//*****************************************************************************
/// \brief �������� ��������� �� ������� �������� ����, ��� ������� ������� �� � �������������
/// \return - �������� �������� ���� ����������� �������� 
///
typedef void ( *pfShuntShiftSetDgn ) ( ShiftDgn val ); 

//*****************************************************************************
/// \brief �������� ��������� �� ������� ��������� ������� ���������� ��������
/// \return - �������� �������� ���� ����������� �������� 
///
typedef void ( *pfShuntShiftSetTime ) ( uint16_t val );

//*****************************************************************************
/// \brief ��������� ����������� ���������� ������.
///
typedef struct ArrayShuntShift_tag 
{
    pfShuntShiftCtor           ctor;                  ///< ������������� ���������� "������� �������". 
    pfShuntShiftRun            run;                   ///< ���������� ������� ���������� "������� �������".
    pfShuntShiftTurnOn         turnOn;                ///< ��������� �������� �������.
    pfShuntShiftTurnOff        turnOff;               ///< ���������� �������� �������.
    pfShuntShiftSetDetPos      setDetPos;             ///< �������� ��������� �������.
    pfShuntShiftSetShiftPos    setShiftPos;           ///< �������� ������������� ������� �������� �������.
    pfShuntShiftIsEnable       isEnable;              ///< �������� ��������� �������� �������.
    pfShuntShiftGetReqPosDet   getReqPosDet;          ///< ������ �� ��������� ������� �������� ��������� �������.
    pfShuntShiftGetReqShiftDir getReqShiftDir;        ///< ������ �� ��������� ������� ����������� �������� �������.
    pfShuntShiftGetTime        getTime;               ///< �������� ����� ���������� �������� �������.
    pfShuntShiftGetDgn         getDgn;                ///< �������� ��������� �������� �������.
    pfSetCntPhaseShift         setCntPhaseShift;      ///< ���������� �������� �������� ���� ����������� ��������
    pfGetCntPhaseShift         getCntPhaseShift;      ///< �������� �������� �������� ���� ����������� ��������.
    pfShuntShiftIsWorkMode     isWorkMode;            ///< �������� ����, ��� ������ �� � �������������
    pfShuntShiftSetDgn         setDgn;                ///< ��������� ���� dgn ������� �� ������� ������
    pfShuntShiftSetTime        setTimeShift;          ///< ���������� ����� ���������� ��������
} ArrayShuntShift;

#endif

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   06-11-2018
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
*/
