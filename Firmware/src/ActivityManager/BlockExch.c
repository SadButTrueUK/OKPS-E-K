/**
* \file    BlockExch.c
* \brief   \copybrief BlockExch.h
*
* \version 1.0.1
* \date    22-05-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <string.h>
#include "BlockExch.h"
#include "BlockExch_driver.h"
#include "InterChannel.h"
#include "interChannelId.h"
#include "Rs422_crc8.h"
#include "ProtectionState_codes.h"
#include "asserts_ex.h"
#include "DeviceAddress.h"
#include "Main.h"
#include "ConfigMK.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define T_O_DATA_SYNCHRO    5             ///< ������� ������������� �������� ������, ��.
#define SYNCHRO_EXCHANGE    0xAAAA        ///< �������� ������������� ������ ������.
#define SYNCHRO_EXCHANGE_TO 100           ///< ������� ������������� ������ ������, ��.

//*****************************************************************************
// ����������� ���������� �������������� ��������
//*****************************************************************************

//*****************************************************************************
const void *BlockExch_drvStr;              ///< ��������� �� �������.

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief �������������� ���������� ������� ������.
///
typedef enum
{
    eRecept_begin   = 0,        ///< �������� �������� ������
    eRecept_synchro             ///< �������� ���������� ������������� ������
} eRecept_run;

//*****************************************************************************
/// \brief ��� ��������� ������ ������ � ����� �����.
///
typedef struct
{
    uint8_t     buffArr[MAX_DATA_LENGTH];        ///< ����� ������.
    uint8_t     syncTimeOutCnt;                  ///< ������� �������� �������������.
    eRecept_run runCnt;                          ///< ������� ��������� ������ ������.
} ReceptStr;

//*****************************************************************************
/// \brief ��������� ���������.
///
struct BlockExch_Str_
{
    uint8_t   transmitData[MAX_DATA_LENGTH];          ///< ������ ��� �������� + CRC8.
    uint8_t   receptData[MAX_DATA_LENGTH - 1];        ///< �������� ������ - CRC8.
    uint8_t   dataLength;                             ///< ����� ������ ��� ������.
    uint8_t   eICId_BlockExchMy;                      ///< ID ���������������� ���������� (���� ��).
    uint8_t   eICId_BlockExchNeighbor;                ///< ID ���������������� ���������� (�������� ��).
    ReceptStr inStr;                                  ///< ��������� ������ ������.
    uint16_t  timeNoExchange;                         ///< ����� ������������ ������� ���������� ������.
    uint16_t  noExchangeCnt;                          ///< ������� ������� ���������� ������.
    bool      ctrl;                                   ///< ��������� ������.
    bool      startSync;                              ///< ���� ������������� ������ ������.
    uint16_t  myData;                                 ///< ������, ����������� � ��� ������ ��
    uint16_t  neighborData;                           ///< ������, ���������� �� ��� ������
    uint8_t   countInvalidTransact;                   ///< ������� ������� � ���������� ���������� ����������� ����� 
};

//*****************************************************************************
// ����������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
/// \brief �������� ��������� ���������.
///
static BlockExch_Str bExchStr =
{
    .ctrl = false,
    .startSync = false,
    .countInvalidTransact = 0
};

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ����� ������ � ����� �����.
/// 
static void checkSyncEvents( void );

//*****************************************************************************
/// \brief �������� ������������� ������ � ����� �����.
/// \param str - ��������� �� ��������� ���������.
///
static void transmitionRun( BlockExch_Str *str );

//*****************************************************************************
/// \brief ����� ������ � ����� �����.
/// \param str - ��������� �� ��������� ���������.
///
static void receptionRun( BlockExch_Str *str );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �������������
BlockExch_Str *BlockExch_ctor( uint8_t length, uint16_t noExchangeTime )
{
    uint8_t i;
    BlockExch_drvStr = BlockExchDrv_ctor( );
    if( length == 0 || length > MAX_DATA_LENGTH - 1 )
    {
        //����������� ������ ����� ������ ������
        ERROR_ID( eGrPS_BlockExch, ePS_BlockExchErrorDataLength );
    }
    bExchStr.dataLength = length;
    //������������� ��������
    for( i = 0; i < MAX_DATA_LENGTH; i++ )
    {
        bExchStr.transmitData[i] = 0;
        bExchStr.inStr.buffArr[i] = 0;
    }
    for( i = 0; i < MAX_DATA_LENGTH - 1; i++ ) bExchStr.receptData[i] = 0;
    bExchStr.timeNoExchange = noExchangeTime;
    bExchStr.noExchangeCnt = bExchStr.timeNoExchange;
    
    //���������� ������������� (������� ���� ��� ��������, ������� - ��� ������)
    bExchStr.eICId_BlockExchMy = ConfigMK_isMaster() ?  eICId_BlockExchM : eICId_BlockExchS;
    bExchStr.eICId_BlockExchNeighbor = ConfigMK_isMaster() ?  eICId_BlockExchS : eICId_BlockExchM;
    InterChannel_setParamSettings( bExchStr.eICId_BlockExchMy , eScriptTransmit, eProcSyncOff, eProcCheckOff, 0, 0,  
                                   0, 0, 0); 
    InterChannel_setParamSettings( bExchStr.eICId_BlockExchNeighbor, eScriptTransmit, eProcSyncOff, eProcCheckOff, 0, 0,  
                                   0, 0, 0); 
     //���������� ��������� ��������� ���������� �������
    bExchStr.inStr.runCnt = eRecept_begin;
    //���������������� ��������� ������
    if( !InterChannel_isHandling( bExchStr.eICId_BlockExchMy ) )
    {    
        InterChannel_synchronize( bExchStr.eICId_BlockExchMy, SYNCHRO_EXCHANGE );
    }
    
    //������� ������� ������������� 
    bExchStr.inStr.syncTimeOutCnt = SYNCHRO_EXCHANGE_TO;
    return &bExchStr;
}

//*****************************************************************************
//�������� ����, ��� ������ ������������� ������ ��������� � ��������� ������ ���������
static void checkSyncEvents( void )
{
    if( InterChannel_syncEvent( bExchStr.eICId_BlockExchMy ) )
        bExchStr.myData = InterChannel_getData( bExchStr.eICId_BlockExchMy );
    if( InterChannel_syncEvent( bExchStr.eICId_BlockExchNeighbor ) )
        bExchStr.neighborData = InterChannel_getData( bExchStr.eICId_BlockExchNeighbor );
}
//*****************************************************************************
// ���������� ������� � ���������
void BlockExch_run( BlockExch_Str *str )
{
    if( str->ctrl == 0 )
    { //����� ��� �� �������
        checkSyncEvents(); 
        if ( bExchStr.myData == SYNCHRO_EXCHANGE && bExchStr.neighborData == SYNCHRO_EXCHANGE )
            str->startSync = true;
        return;
    }
    if( str->noExchangeCnt != 0 ) str->noExchangeCnt--;
    transmitionRun( str );
    receptionRun( str );
}

//*****************************************************************************
// ����������
void BlockExch_interrupt( BlockExch_Str *str, uint16_t interruptNumb )
{
    if( !str->ctrl )
    {
        //��������� �������� �������������
        if( str->inStr.syncTimeOutCnt ) str->inStr.syncTimeOutCnt--;
        else
        {
            //������� ����� �������� �������������
            ERROR_ID( eGrPS_BlockExch, ePS_BlockExchErrorStartSync );
        }
        //�������� ����� ������ ������
        if( str->startSync && interruptNumb == 15 )
        { //�������� �����
            str->ctrl = true;
        }
    }
    BlockExchDrv_run( BlockExch_drvStr, interruptNumb == 15 );
}

//*****************************************************************************
// ������������ ������ ��� ��������
void BlockExch_setData( BlockExch_Str *str, const uint8_t *data )
{
    memcpy( str->transmitData, data, str->dataLength ); //����������� ������
    //������� � ���������� CRC8
    str->transmitData[str->dataLength] = crc8_update( 0, data, str->dataLength );
}

//*****************************************************************************
// �������� �������� ������
bool BlockExch_getData( BlockExch_Str *str, uint8_t *data )
{
    if( str->noExchangeCnt == 0 ) 
		return false; //������ �� �����������
    memcpy( data, str->receptData, str->dataLength ); //����������� ������
    return true;
}

//*****************************************************************************
// �������� ��������� ����� � �������� ��������
bool BlockExch_getConnectionState( BlockExch_Str *str )
{
    return str->noExchangeCnt != 0;
}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
// �������� ������������� ������ � ����� �����
static void transmitionRun( BlockExch_Str *str )
{
    //�������� ������������ ������
    if( BlockExchDrv_isOutReady( BlockExch_drvStr ) == 0 ) return;
    //�������� ������
    BlockExchDrv_set( BlockExch_drvStr, str->transmitData, str->dataLength + 1 );
}

//���� ������ �� ��������� �������
void receptionRun( BlockExch_Str *str )
{
    static const uint8_t NUM_INVALID_TRANSACTIONS = 4; //��������� �������� ��������� �������� CRC
  
    //��������� ��������
    if( str->inStr.syncTimeOutCnt != 0 ) str->inStr.syncTimeOutCnt--;
    switch( str->inStr.runCnt )
    {
        case eRecept_begin: //��������� �������������
            if( BlockExchDrv_get( BlockExch_drvStr, str->inStr.buffArr, str->dataLength + 1 ) == 0 ) 
                break;
            
            if( crc8_update( 0, str->inStr.buffArr, str->dataLength ) == str->inStr.buffArr[str->dataLength] )
            {                
                    //������������� �������� ������            
                if( !InterChannel_isHandling( str->eICId_BlockExchMy ) )
                {    
                    InterChannel_synchronize( str->eICId_BlockExchMy, str->inStr.buffArr[str->dataLength] );
                    //��������� �������� �������������
                    str->inStr.syncTimeOutCnt = T_O_DATA_SYNCHRO;
                    //������� � �������� ������ ������������������ ����������
                    str->inStr.runCnt = eRecept_synchro;
                }
            }
            break;
        case eRecept_synchro: //������������� �������� ������
            if( str->inStr.syncTimeOutCnt == 0 )
            {    
                checkSyncEvents(); 
                if( bExchStr.myData == bExchStr.neighborData ) //������ ��������� ����� ����� ��
                {
                    //������ �������� �������
                    memcpy( str->receptData, str->inStr.buffArr, str->dataLength );
                    //��������� �������� ������� ���������� ������
                    str->noExchangeCnt = str->timeNoExchange;
                    //������� � �������� ��������� ������� 
                    str->countInvalidTransact = 0; //�������� ������� ������
                }
                else 
                {    
                    str->countInvalidTransact++;
                }
                str->inStr.runCnt = eRecept_begin;
            }
            if (str->countInvalidTransact >= NUM_INVALID_TRANSACTIONS )    
                    ERROR_EX_ID( eGrPS_BlockExch, ePS_BlockExchErrorReceive,
                             ActivityManager_isActive( activityManager ),
                             DeviceAddress_isPrimary( ),
                             str->inStr.buffArr[0], //�������� ������ 
                             str->inStr.buffArr[1] ); //�������� CRC
            break;
    }
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
* 
* ������ 1.0.2
* ���� 10-10-2019  
* ���������: 
* ������ �������� ������������ � ����������� ��������� ����������� ����� ����������� ������ 
 
 */
