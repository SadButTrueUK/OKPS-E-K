/**
* \file    ActivityManager_internal.c
* \brief   \copybrief ActivityManager_internal.h
*
* \version 1.0.1
* \date    19-05-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "ActivityManager_internal.h"
#include "ActivityManager_dataTypes.h"

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ��������� ��������� ����� � �������� ��������
void ActivityManager_setNeighborConnection( ActivityManager_DeviceStr *str, bool connect )
{
    str->inputData.str.neighborConnection = connect;
}

//*****************************************************************************
// ������������ ��������� ��������� ������� � ����� ������ �� "�����" �������
void ActivityManager_setNeighborHearsMe( ActivityManager_DeviceStr *str, bool reception )
{
    str->inputData.str.neighborHearsMe = reception;
}

//*****************************************************************************
// ��������� ��������� ������ (������� / ����������)
void ActivityManager_setNeighborState( ActivityManager_DeviceStr *str, bool state )
{
    str->inputData.str.neighborState = state;
}

//*****************************************************************************
// ��������� ��������� ���������� ������ (�������� / ���������)
void ActivityManager_setNeighborActivity( ActivityManager_DeviceStr *str, bool active )
{
    str->inputData.str.neighborActivity = active;
}

//*****************************************************************************
// ������ ��������� ������������� ������ � ��������� ��������� ��������� ����������
void ActivityManager_writeNeighborIsInWork( ActivityManager_DeviceStr *str, bool state )
{
   str->inputData.str.neighborIsInWork = state;
}

//*****************************************************************************
// ��������� ��������� ����� ������ �� ���������� RS-422
void ActivityManager_setNeighborRS422connect( ActivityManager_DeviceStr *str, Rs422_numLine line, bool connect )
{
    if( line == eRs422_line_1 ) 
        str->rs422.str.neighborLine1connect = connect;
    else if( line == eRs422_line_2 ) 
        str->rs422.str.neighborLine2connect = connect;
}

//*****************************************************************************
// ��������� ��������� ����� ������ � �� �� ���������� RS-422
void ActivityManager_setNeighborRS422ctrlSysConnect( ActivityManager_DeviceStr *str, Rs422_numLine line, bool connect )
{
    if( line == eRs422_line_1 ) 
        str->rs422.str.neighborLine1CtrlSystemConnect = connect;
    else if( line == eRs422_line_2 ) 
        str->rs422.str.neighborLine2CtrlSystemConnect = connect;
}

//*****************************************************************************
// �������� ��������� �������
bool ActivityManager_getState( ActivityManager_DeviceStr *str )
{
    return str->inputData.str.ownState;
}

//*****************************************************************************
// �������� ��������� ����� �� ���������� RS-422
bool ActivityManager_getRS422connect( ActivityManager_DeviceStr *str, Rs422_numLine line )
{
    if( line == eRs422_line_1 ) 
        return str->rs422.str.ownLine1connect;
    if( line == eRs422_line_2 ) 
        return str->rs422.str.ownLine2connect;
    return false;
}

//*****************************************************************************
// �������� ��������� ����� c ��
bool ActivityManager_getRS422ctrlSysConnect( ActivityManager_DeviceStr *str, Rs422_numLine line )
{
    if( line == eRs422_line_1 ) 
        return str->rs422.str.ownLine1CtrlSystemConnect;
    if( line == eRs422_line_2 ) 
        return str->rs422.str.ownLine2CtrlSystemConnect;
    return false;
}

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   19-05-2017
* �����  ��������� �.�.
* 
* ���������:
*     ������� ������.
*/
