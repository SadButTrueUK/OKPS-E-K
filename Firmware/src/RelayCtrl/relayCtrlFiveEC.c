/**
* \file    relayCtrlFiveEC.c
* \brief   \copybrief relayCtrlFiveEC.h
*
* \version 1.0.1
* \date    12-12-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "relayCtrlFiveEC.h"
#include "relayCtrlDrv.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************
#define FIVE_WIRE_SWITCH_TO 200        ///< ����-��� ������������.

//*****************************************************************************
// ���������� ���������� �������������� ��������
//*****************************************************************************

//*****************************************************************************
/// \brief ���������� ��������� ����������.
///
const ArrayRelayCtrl fiveWireECRelCtrl =
{
    FiveWireECRelCtrl_ctor,
    FiveWireECRelCtrl_run,
    FiveWireECRelCtrl_setShiftDir,
    FiveWireECRelCtrl_setPosDet,
    FiveWireECRelCtrl_isSwitched,
    FiveWireECRelCtrl_getShiftDir,
    FiveWireECRelCtrl_getPosDet
};

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************
static ShiftDirection    direct;           ///< ����������� ��������.
static PositionDet_State position;         ///< �������� ���������.
static uint16_t          switchCnt;        ///< ������� ������� ������������.

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �������������
void FiveWireECRelCtrl_ctor( void )
{
    RelayCtrlDrv_switchPhasesAB( false );     //���������� ���� ����������� ��� (���� � L3, L4)
    RelayCtrlDrv_switchReceiverL1L2( false ); // ��������� �������� �� L1,L2
    RelayCtrlDrv_switchReceiverL3L4( false ); // ��������� �������� �� L3,L4
    switchCnt = FIVE_WIRE_SWITCH_TO;          // ������� ������� ������������
    direct = eShiftStop;                      // ����������� �������� - �������
    position = eDetNon;                       // �������� ��������� - ���������
}

//*****************************************************************************
// ���������� �������
void FiveWireECRelCtrl_run( void )
{
    if( switchCnt ) switchCnt--; // ��������� �������� ������������
}

//*****************************************************************************
// ���������� ����������� ��������.
void FiveWireECRelCtrl_setShiftDir( ShiftDirection shift )
{
    direct = shift; //���������� ��������� ����������� ��������

    switch( shift )
    {
        case eShiftStop: //��� ��������
            RelayCtrlDrv_switchPhasesAB( false ); //���������� ���� ����������� ��� (���� � L3, L4)
            break;

        case eShiftPlus:
            RelayCtrlDrv_switchPhasesAB( false ); //���������� ���� � L3, L4
            break;

        case eShiftMinus:
            RelayCtrlDrv_switchPhasesAB( true ); //���������� ���� � L1, L2
            break;

        default:
            ERROR_ID( eGrPS_RelayCtrl, ePS_RelayCtrlStepCnt3error );
    }
    switchCnt = FIVE_WIRE_SWITCH_TO; //���������� ����-��� ������������   
}

//*****************************************************************************
// ���������� �������� ���������
void FiveWireECRelCtrl_setPosDet( PositionDet_State pos )
{
    position = pos; //���������� ��������� �������������� ���������

    switch( pos )
    {
        case eDetNon: //��������� �������� ���������
            RelayCtrlDrv_switchReceiverL1L2( false ); //��������� �������� �� L1, L2
            RelayCtrlDrv_switchReceiverL3L4( false ); //��������� �������� �� L3, L4
            break;

        case eDetPlus: //�������� "��������� ����" (�������� � L3, L4)
            RelayCtrlDrv_switchReceiverL1L2( false ); //��������� �������� �� L1, L2
            RelayCtrlDrv_switchReceiverL3L4( true ); //���������� �������� �� L3, L4
            break;

        case eDetMinus: //�������� "��������� �����" (�������� � L3, L4)
            RelayCtrlDrv_switchReceiverL1L2( true ); //���������� �������� � L1, L2
            RelayCtrlDrv_switchReceiverL3L4( false ); //��������� �������� �� L3, L4
            break;

        default:
            ERROR_ID( eGrPS_RelayCtrl, ePS_RelayCtrlStepCnt4error );
    }
    switchCnt = FIVE_WIRE_SWITCH_TO; //���������� ����-��� ������������  
}

//*****************************************************************************
// �������� ���������� ���������
bool FiveWireECRelCtrl_isSwitched( void )
{
    return switchCnt == 0;
}

//*****************************************************************************
// �������� ������������� ����������� ��������
ShiftDirection FiveWireECRelCtrl_getShiftDir( void )
{
    return direct;
}

//*****************************************************************************
// �������� �������������� ���������
PositionDet_State FiveWireECRelCtrl_getPosDet( void )
{
    return position;
}

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   12-12-2017
* �����  ��������� �.�.
* 
* ���������:
*     ������� ������.
*/
