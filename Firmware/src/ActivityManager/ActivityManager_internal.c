/**
* \file    ActivityManager_internal.c
* \brief   \copybrief ActivityManager_internal.h
*
* \version 1.0.2
* \date    9-04-2021
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
//�������� ��������� ����� ������ � �� �� ���������� RS-422;
bool ActivityManager_getNeighborRS422connect( ActivityManager_DeviceStr *str )
{
    static const uint16_t TIMEOUT_NO_EXCH_VAL = 1500;
    static uint16_t timeout = 0;
    static bool ret = false;
   
    if( str->rs422.str.neighborLine1CtrlSystemConnect == 0
        || str->rs422.str.neighborLine1CtrlSystemConnect == 0 )
    {
        
        if( ++timeout >= TIMEOUT_NO_EXCH_VAL )
        {
            timeout = 0;
            ret = true;
        }
    }
    else
    {
        timeout = 0;
        ret = false;
    }
    return ret;
}
//*****************************************************************************
// ��������� ��������� ����� ������ � �� �� ���������� RS-422
void ActivityManager_setNeighborPositionShunt( ActivityManager_DeviceStr *str, uint8_t state )
{
    str->inputData.str.neighborStatePosShift = state;
}

//*****************************************************************************
// �������� ��������� ����� ������ � �� �� ���������� RS-422
uint8_t ActivityManager_getNeighborPositionShunt( ActivityManager_DeviceStr *str )
{
    return str->inputData.str.neighborStatePosShift;
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
uint8_t ActivityManager_getOwnStatePosShift( ActivityManager_DeviceStr *str )
{
    return str->inputData.str.ownStatePosShift;
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
*   ������� ������.
* 
* ������ 1.0.2
* ����   9-04-2021
* �����  ��������� �.�.
* 
* ���������:
*   �������� ����� ��������� ActivityManager_getNeighborRS422connect(
*/
