/**
* \file    relayCtrlNineWire.c
* \brief   \copybrief relayCtrlNineWire.h
*
* \version 1.0.1
* \date    12-12-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "relayCtrlNineWire.h"

//*****************************************************************************
// ����������� ��������� �������������� ��������
//*****************************************************************************

//*****************************************************************************
/// \brief ���������� ��������� ����������.
///
const ArrayRelayCtrl nineWireRelCtrl =
{
    NineWireRelCtrl_ctor,
    NineWireRelCtrl_run,
    NineWireRelCtrl_setShiftDir,
    NineWireRelCtrl_setPosDet,
    NineWireRelCtrl_isSwitched,
    NineWireRelCtrl_getShiftDir,
    NineWireRelCtrl_getPosDet
};

//*****************************************************************************
// ����������� ��������� ����������
//*****************************************************************************
static          ShiftDirection    direct;           ///< ����������� ��������.
static          PositionDet_State position;         ///< �������� ���������.
static uint16_t                   switchCnt;        ///< ������� ������� ������������. 

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �������������  
void NineWireRelCtrl_ctor( void )
{
    switchCnt = 0; // �������� ������� ������������
    direct = eShiftStop; // ����������� �������� - �������
    position = eDetNon; // �������� ��������� - ���������
}

//*****************************************************************************
// ���������� �������
void NineWireRelCtrl_run( void )
{

}

//*****************************************************************************
// ���������� ����������� ��������
void NineWireRelCtrl_setShiftDir( ShiftDirection shift )
{
    direct = shift;
}

//*****************************************************************************
// ���������� �������� ���������
void NineWireRelCtrl_setPosDet( PositionDet_State pos )
{
    position = pos;
}

//*****************************************************************************
// �������� ���������� ���������
bool NineWireRelCtrl_isSwitched( void )
{
    return switchCnt == 0;
}

//*****************************************************************************
// �������� ������������� ����������� ��������
ShiftDirection NineWireRelCtrl_getShiftDir( void )
{
    return direct;
}

//*****************************************************************************
// �������� �������������� ���������
PositionDet_State NineWireRelCtrl_getPosDet( void )
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