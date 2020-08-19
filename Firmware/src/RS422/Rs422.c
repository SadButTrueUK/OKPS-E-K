/**
* \file    Rs422.c
* \brief   \copybrief Rs422.h
*
* \version 1.0.1
* \date    02-06-2017
* \author  ������ �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <string.h>
#include "Rs422.h"
#include "Rs422_lineExch.h"
#include "dsPIC33_rs422.h"
#include "Rs422_dataTypes.h"
#include "asserts_ex.h"
#include "ConfigMK.h"
#include "InterChannelID.h"
#include "InterChannel.h"

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ��������� ������ RS-422.
///
struct Rs422_handler_
{
    CommunicationRS422 line_1;        ///< ��������� ������ RS-422 1-�� �����.
    CommunicationRS422 line_2;        ///< ��������� ������ RS-422 2-�� �����.
    StateRS422         state;         ///< ��������� ����� ������ ��� ���� �����.
};

//*****************************************************************************
// ����������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
/// \brief ������������� ��������� ���������� ����� L1 � L2.
///
static Rs422_handler handl =
{
    //������������� ���������� ����� 1
    .line_1.numLine = eRs422_line_1,
    .line_1.eICId_Rs422Sync = eICId_Rs422Sync_Rs1,
    //    .line_1.iniPortCtrl = initPortContr_L1,
    .line_1.iniUart = init_L1,
    .line_1.inRS422.inByte = RxByte_L1,
    .line_1.outRS422.outByte = TxByte_L1,
    .line_1.outRS422.manageInOut = dirL1,
    .line_1.outRS422.transmCompl = isTxCompl_L1,
    //������������� ���������� ����� 2 
    .line_2.numLine = eRs422_line_2,
    .line_2.eICId_Rs422Sync = eICId_Rs422Sync_Rs2,
    //    .line_2.iniPortCtrl = initPortContr_L2,
    .line_2.iniUart = init_L2,
    .line_2.inRS422.inByte = RxByte_L2,
    .line_2.outRS422.outByte = TxByte_L2,
    .line_2.outRS422.manageInOut = dirL2,
    .line_2.outRS422.transmCompl = isTxCompl_L2,
    //������������� ����� ����������
    .state.ctrl = false
};

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �������������
Rs422_handler *Rs422_ctor( uint16_t addr, uint16_t noExchangeTime, uint16_t noExchangeTimeCtrlSystem,
                           uint8_t orderLength )
{
    uint8_t i = 0;

    //������������� state
    handl.state.numberCycleOK = 0;
    for( i = 0; i < MAX_ALARM_COUNT; i++ ) handl.state.alarm[i] = 0;
    handl.state.numAlarm = 0;
    for( i = 0; i < MAX_ORDER_DATA; i++ ) handl.state.order[i] = 0;
    //�������� ����� ����������
    if( orderLength == 0 || orderLength > MAX_ORDER_DATA )
    {
        //������. �������� ����� �� �������������         
        ERROR_EX_ID( eGrPS_Rs422, ePS_Rs422SettingsError, 0xAAAA, orderLength, 0xBBBB, 0 );
    }
    handl.state.orderLength = orderLength;
    for( i = 0; i < MAX_STATUS_DATA; i++ ) handl.state.status[i] = 0;
    handl.state.statusLength = 0;
    handl.state.address = addr;
    handl.state.noExchangeTime = noExchangeTime;
    handl.state.noExchangeTimeCtrlSystem = noExchangeTimeCtrlSystem;

    //������������� ���������� ����� 1 
    Rs422_lineCtor( &handl.line_1 );
    handl.line_1.noExchangeCnt = noExchangeTime;
    handl.line_1.noExchangeCtrlSystemCnt = noExchangeTimeCtrlSystem;
    //������������� ���������� ����� 2      
    Rs422_lineCtor( &handl.line_2 );
    handl.line_2.noExchangeCnt = noExchangeTime;
    handl.line_2.noExchangeCtrlSystemCnt = noExchangeTimeCtrlSystem;

    return &handl;
}

//*****************************************************************************
// ���������� ������� � ���������
void Rs422_run( Rs422_handler *handler )
{
    if( handl.state.ctrl ) // ��������� ��������, ���� ��������� �������
    {
        Rs422_lineRun( &handler->line_1, &handler->state );
        Rs422_lineRun( &handler->line_2, &handler->state );
    }
}

//*****************************************************************************
// ����� � �������� ������
void Rs422_interrupt( Rs422_handler *handler )
{
    if( handl.state.ctrl ) // ��������� ��������, ���� ��������� �������
    {
        Rs422_lineInterrupt( &handler->line_1, &handler->state );
        Rs422_lineInterrupt( &handler->line_2, &handler->state );
    }
}

//*****************************************************************************
// ���������/���������� ��������� ����� RS422
void Rs422_setEnable( Rs422_handler *handler, bool enable )
{
    handler->state.ctrl = enable;
}

//*****************************************************************************
// ��������� ������� �������
void Rs422_setStatus( Rs422_handler *handler, const uint8_t *data, uint8_t dataSize )
{
    if( dataSize <= MAX_STATUS_DATA )
    {
        handler->state.statusLength = dataSize;
        memcpy( handler->state.status, data, dataSize );
    }
}

//*****************************************************************************
// ��������� ������� ������
bool Rs422_getLineEvent( Rs422_handler *handler, Rs422_numLine nLine )
{
    bool event = false;

    if( nLine == eRs422_line_1 )
    {
        event = handler->line_1.exchangeEvent; //| handler->state.newOrderEvent;
        //handler->state.newOrderEvent = false;
        handler->line_1.exchangeEvent = false;
        if( event ) handler->state.isLineConnect = false;
        return event;
    }
    else if( nLine == eRs422_line_2 )
    {
        event = handler->line_2.exchangeEvent; // | handler->state.newOrderEvent;
        //handler->state.newOrderEvent = false;
        handler->line_2.exchangeEvent = false;
        if( event ) handler->state.isLineConnect = false;
        return event;
    }
    else return false;
}

//*****************************************************************************
// ��������� ������� ������ �������
bool Rs422_getNewOrderEventLine( Rs422_handler *handler, Rs422_numLine nLine )
{
    bool temp;

    if( nLine == eRs422_line_1 )
    {
        temp = handler->line_1.newOrderEvent;
        handler->line_1.newOrderEvent = false;
        return temp;
    }
    else if( nLine == eRs422_line_2 )
    {
        temp = handler->line_2.newOrderEvent;
        handler->line_2.newOrderEvent = false;
        return temp;
    }
    else
        return false;
}

//*****************************************************************************
// ��������� ���������� ������
bool Rs422_getNoLineExchange( Rs422_handler *handler, Rs422_numLine nLine )
{
    if( nLine == eRs422_line_1 )
        return handler->line_1.noExchangeCnt == 0;
    else
        if( nLine == eRs422_line_2 )
        return handler->line_2.noExchangeCnt == 0;
    else
        return false;
}

//*****************************************************************************
// ��������� ���������� ������ � ��
bool Rs422_getNoLineExchangeCtrlSystem( Rs422_handler *handler, Rs422_numLine nLine )
{
    if( nLine == eRs422_line_1 )
        return handler->line_1.noExchangeCtrlSystemCnt == 0;
    else
        if( nLine == eRs422_line_2 )
        return handler->line_2.noExchangeCtrlSystemCnt == 0;
    else
        return false;
}

//*****************************************************************************
// ��������� ���������� ������ c �� �� ����� ������
bool Rs422_getNoExchangeCtrlSystem( Rs422_handler *handler )
{
    return Rs422_getNoLineExchangeCtrlSystem( handler, eRs422_line_1 ) &&
            Rs422_getNoLineExchangeCtrlSystem( handler, eRs422_line_2 );
}

//*****************************************************************************
// ��������� ������� � ������ ��� �����
uint8_t Rs422_getOrder( Rs422_handler *handler, uint8_t *data )
{
    memcpy( data, handler->state.order, handler->state.orderLength );
    return handler->state.orderLength;
}

//*****************************************************************************
// ���������� ���� ������ � ������ ���������� ��� �������
void Rs422_addAlarm( Rs422_handler *handler, Rs422_alarmCode alarm )
{
    uint8_t i = 0;

    //�������� ������� alarm � ������ 
    for( i = 0; i < MAX_ALARM_COUNT; i++ )
    {
        if( handler->state.alarm[i] == alarm ) break; // ��� ������ � ������ ��� ���� 
        else if( handler->state.alarm[i] == 0 )
        {   // ���� ������ ��� ����������
            handler->state.alarm[i] = alarm; // �������� � ������ ��� ������
            break;
        }
    }
}

//*****************************************************************************
// �������� ���� ������ �� ������ ���������� ��� �������
void Rs422_removeAlarm( Rs422_handler *handler, Rs422_alarmCode alarm )
{
    uint8_t i = 0;
    bool exist = false;

    //�������� ������� alarm � ������ 
    for( i = 0; i < MAX_ALARM_COUNT; i++ )
    {
        if( handler->state.alarm[i] == alarm )
        {
            exist = true;
            break;
        }
        else if( handler->state.alarm[i] == 0 ) break;
    }
    if( exist == false ) return; //� ������ ��� ������ alarm

    //��������� ��� ��������� � ������
    if( i == MAX_ALARM_COUNT - 1 )
    {
        handler->state.alarm[i] = 0;
        return;
    }

    for(; i < MAX_ALARM_COUNT - 1; i++ )
    {
        handler->state.alarm[i] = handler->state.alarm[i + 1];
        if( handler->state.alarm[i] == 0 ) break;
    }
}

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   02-06-2017
* �����  ������ �.�.
* 
* ���������:
*    ������� ������.
*/
