/** 
* \file    shuntShift.c
* \brief   \copybrief shuntShift.h
*
* \version 1.0.1
* \date    02-02-2018
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "shuntShift.h"
#include "shuntShiftTypes.h"
#include "shuntShiftFiveEC.h"
#include "shuntShiftNineWire.h"
#include "shuntShiftMotor.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"
#include "CheckCallFunctions.h"

//*****************************************************************************
// ���������� ��������� �������������� ��������
//*****************************************************************************
static const ArrayShuntShift *ptrShuntShift;        ///< ��������� �� ���������� ��������� ����������.

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �������������
void ShuntShift_ctor( RelayModuleType relayType, MotorType motor )
{
    switch( relayType )
    {
        case eFiveWireEc:
            ptrShuntShift = &fiveWireEcShuntShift;
            break;
        case eNineWire:
            ptrShuntShift = &nineWireShuntShift;
            break;
        default:
            ERROR_ID( eGrPS_ShuntShift, ePS_ShuntShiftTypeErr );
    }
    ptrShuntShift->ctor( );
    ShuntShiftMotor_ctor( motor );
}

//*****************************************************************************
// ���������� �������
void ShuntShift_run( void )
{
    ptrShuntShift->run( );
    ShuntShiftMotor_run( );
    MARKED_CALL_FUNCTION;
}

//*****************************************************************************
// ��������� �������� �������
void ShuntShift_turnOn( bool dir, uint16_t time, bool st, uint8_t rs )
{
    ptrShuntShift->turnOn( dir, time, st, rs );
}

//*****************************************************************************
// ���������� �������� �������
void ShuntShift_turnOff( void )
{
    ptrShuntShift->turnOff( );
}

//*****************************************************************************
// �������� ��������� �������
void ShuntShift_setDetPos( PositionDet_State detPos, PositionDet_State pos )
{
    ptrShuntShift->setDetPos( detPos, pos );
}

//*****************************************************************************
// �������� ������������� ������� ��������
void ShuntShift_setShiftPos( ShiftDirection shiftPos )
{
    ptrShuntShift->setShiftPos( shiftPos );
}

//*****************************************************************************
// �������� ��������� ���������
bool ShuntShift_isEnable( void )
{
    return ptrShuntShift->isEnable( );
}

//*****************************************************************************
// ������ �� ��������� ������� ��������
PositionDet_State ShuntShift_getReqPosDet( void )
{
    return ptrShuntShift->getReqPosDet( );
}

//*****************************************************************************
// ������ �� ��������� ������� ����������� ��������
ShiftDirection ShuntShift_getReqShiftDir( void )
{
    return ptrShuntShift->getReqShiftDir( );
}

//*****************************************************************************
// �������� ����� ���������� ��������
uint16_t ShuntShift_getTime( void )
{
    return ptrShuntShift->getTime( );
}

//*****************************************************************************
// �������� ��������� ��������
ShiftDgn ShuntShift_getDgn( void )
{
    return ptrShuntShift->getDgn( );
}

//*****************************************************************************
// ���������� �������� �������� ���� ����������� ��������.
void ShuntShift_setCntPhaseShift( const uint8_t val )
{
    ptrShuntShift->setCntPhaseShift( val );
}

//*****************************************************************************
// �������� �������� �������� ���� ����������� ��������.
const uint8_t ShuntShift_getCntPhaseShift( void )
{
    return ptrShuntShift->getCntPhaseShift();
}

//*****************************************************************************
// �������� ����, ��� ������ ������ ����� �������������.
const bool ShuntShift_isWorkMode( void )
{
    return ptrShuntShift->isWorkMode;
}

void ShuntShift_setDgn( ShiftDgn val )
{
    ptrShuntShift->setDgn( val );
}

void ShuntShift_setTime( uint16_t val )
{
    ptrShuntShift->setTimeShift( val ); 
}


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
