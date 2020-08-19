/**
* \file    ActivityManager_connect.c
* \brief   \copybrief ActivityManager_connect.h
* 
* \version 1.0.1
* \date    22-05-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#include "ActivityManager_connect.h"
#include "ActivityManager_internal.h"

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ��������� ������� ��� �������� ��������� �������.
///
typedef struct ExchangeDataStr
{
    uint8_t s1     :1;         ///< ��������� ������� (�������, ����������).
    uint8_t s2     :1;         ///< ���������� �������.
    uint8_t s3     :1;         ///< ������� ����� � �������� ��������.
    uint8_t s4     :1;         ///< ��������� ����� � �� �� ����� 1.
    uint8_t s5     :1;         ///< ��������� ����� � �� �� ����� 2.
    uint8_t s6     :1;         ///< ��������� ����� �� RS-422 ����� 1.
    uint8_t s7     :1;         ///< ��������� ����� �� RS-422 ����� 2.
    uint8_t s8     :1;         ///< ��������� ������������� ��������� ���������� ������� ( 0 - � ��������, 1 - �������� )
} ExchangeDataStr;

//*****************************************************************************
/// \brief ����������� ������ ��������� ExchangeDataStr � \a uint8_t.
///
typedef union uExchangeDataStr
{
    ExchangeDataStr str;         ///< ��������� ��������� ������� ��� �������� ��������� �������.
    uint8_t         data;        ///< ��������� ������� ��� �������� ��������� �������.
} uExchangeDataStr;

//*****************************************************************************
// ����������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ������ �������.
///
static uExchangeDataStr ownState =
{
    .data = 0
};

//*****************************************************************************
/// \brief ��������� ��������� �������.
///
static uExchangeDataStr neighborState =
{
    .data = 0
};

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������������ ��������� ������ �������.
/// \param activityManager - ��������� �� ���������, ����������� ��������� ������ ActivityManager.h;
/// \param blockExch - ��������� �� ���������, ����������� ��������� ������ BlockExch.h;
/// \param st - ��������� �� ������ ��������� ������� ��� �������� � ��������.
///
static void ownStateForming( ActivityManager_DeviceStr *activityManager, BlockExch_Str *blockExch, 
                             uExchangeDataStr *st );

//*****************************************************************************
/// \brief �������� ��������� ��������� �������.
/// \param blockExch - ��������� �� ���������, ����������� ��������� ������ BlockExch.h;
/// \param st - ��������� �� ����������� ������ ��������� ��������� �������.
///
static void receiveNeihgborState( BlockExch_Str *blockExch, uExchangeDataStr *st );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ���������� ��������������� ActivityManager � BlockExch
void ActiveManagerConnect_run( ActivityManager_DeviceStr *activityManager, BlockExch_Str *blockExch )
{
    //������������ ��������� ������ �������
    ownStateForming( activityManager, blockExch, &ownState );
    //�������� � �������� ������
    BlockExch_setData( blockExch, &ownState.data );
    //�������� ��������� ��������� �������
    receiveNeihgborState( blockExch, &neighborState );
    //���������� ��������� � ActivityManager
    //���������� ��������� ����� � �������
    ActivityManager_setNeighborConnection( activityManager, BlockExch_getConnectionState( blockExch ) );
    //���������� ����� ��������� ������ "�� ����"
    ActivityManager_setNeighborHearsMe( activityManager, neighborState.str.s3 );
    //���������� ��������� ������ (������� / ����������)
    ActivityManager_setNeighborState( activityManager, neighborState.str.s1 );
    //���������� ��������� ���������� ������ (�������� / ���������)
    ActivityManager_setNeighborActivity( activityManager, neighborState.str.s2 );
    //���������� ��������� ����� ������ � �� 
    ActivityManager_setNeighborRS422ctrlSysConnect( activityManager, eRs422_line_1, neighborState.str.s4 );
    ActivityManager_setNeighborRS422ctrlSysConnect( activityManager, eRs422_line_2, neighborState.str.s5 );
    //���������� ��������� ����� ������ �� RS-422
    ActivityManager_setNeighborRS422connect( activityManager, eRs422_line_1, neighborState.str.s6 );
    ActivityManager_setNeighborRS422connect( activityManager, eRs422_line_2, neighborState.str.s7 );
    ActivityManager_writeNeighborIsInWork( activityManager, neighborState.str.s8 );
}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
// ������������ ��������� ������ �������
static void ownStateForming( ActivityManager_DeviceStr *activityManager, BlockExch_Str *blockExch, uExchangeDataStr *st )
{
    //��������� �����������������
    st->str.s1 = ActivityManager_getState( activityManager );
    //��������� ����������
    st->str.s2 = ActivityManager_isActive( activityManager );
    //������� ����� � ������� � ������ �� ��������� ����������
    st->str.s3 = BlockExch_getConnectionState( blockExch );
    //��������� ����� � �� 
    st->str.s4 = ActivityManager_getRS422ctrlSysConnect( activityManager, eRs422_line_1 );
    st->str.s5 = ActivityManager_getRS422ctrlSysConnect( activityManager, eRs422_line_2 );
    //��������� ����� � �� �� RS-422
    st->str.s6 = ActivityManager_getRS422connect( activityManager, eRs422_line_1 );
    st->str.s7 = ActivityManager_getRS422connect( activityManager, eRs422_line_2 );
    st->str.s8 = ActivityManager_getInitDeviceState( activityManager );
}

//*****************************************************************************
// �������� ��������� ��������� �������
static void receiveNeihgborState( BlockExch_Str *blockExch, uExchangeDataStr *st )
{
    if( BlockExch_getData( blockExch, &st->data ) ) 
        return;
    st->data = 0;
}

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
