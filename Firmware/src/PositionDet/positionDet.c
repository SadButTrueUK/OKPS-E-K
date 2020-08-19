/**
* \file    positionDet.c
* \brief   \copybrief positionDet.h
*
* \version 1.0.1
* \date    28-12-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "positionDet.h"
#include "positionDetTypes.h"
#include "positionDetFiveEC.h"
#include "positionDetNineWire.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"
#include "posDetGenerator.h"

//*****************************************************************************
// ����������� ��������� �������������� ��������
//*****************************************************************************
static const ArrayPositionDet *ptrPosDet;        ///< ��������� �� ���������� ��������� ����������.

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �������������
void PositionDet_ctor( RelayModuleType relayType )
{
    switch( relayType )
    {
        case eFiveWireEc:
            ptrPosDet = &fiveWireECPosDet;
            break;
        case eNineWire:
            ptrPosDet = &nineWirePosDet;
            break;
        default:
            ERROR_ID( eGrPS_PositionDet, ePS_PosDetStepCnt1error );
    }
    ptrPosDet->ctor( );
    PosDetGenerator_ctor( );
    PositionDet_turnOn( true ); // �������� ����������� ��������� ����� �� ������
}

//*****************************************************************************
// ���������� �������
void PositionDet_run( void )
{
    ptrPosDet->run( );
    PosDetGenerator_run( );
    PosDetGenerator_checkUfb( );
}

//*****************************************************************************
// ���������/���������� ����������� ���������
void PositionDet_turnOn( bool on )
{
    ptrPosDet->turnOn( on );
    PosDetGenerator_turnOn( on );
}

//*****************************************************************************
// �������� ��������� ������
bool PositionDet_isEnable( void )
{
    return ptrPosDet->isEnable( ) &&
            PosDetGenerator_isEnable( );
}

//*****************************************************************************
// �������� ������������� ���������
PositionDet_State PositionDet_getPosition( void )
{
    return ptrPosDet->getPos( );
}

//*****************************************************************************
// �������� ������ �� ��������� ������� �������� ���������
PositionDet_State PositionDet_getRequestPositionDet( void )
{
    return ptrPosDet->getReqPosDet( );
}

//*****************************************************************************
// �������� ������������� ������� ��������
void PositionDet_setPosDet( PositionDet_State pos )
{
    ptrPosDet->setPosDet( pos );
}

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
