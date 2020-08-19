/**
* \file    BlockExch_driver.c
* \brief   \copybrief BlockExch_driver.h
*
* \version 1.0.2
* \date    10-10-2019
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <string.h>
#include "typeMK.h"
#include "BlockExch_driver.h"
#include "ConfigMK.h"
#include "IOports.h"

//*****************************************************************************
// ���������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define BIT_QUANTUM_NUMBER 4U        ///< ���������� ������� ��� ������ ������ ����.
#define STOP_SEQUENCE      8U        ///< ���������� ����� � Stop-������������������.

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief �������������� ���������� ��������� ������.
///
typedef enum
{
    eTransmit_begin     = 0,        ///< ������������� ������
    eTransmit_startBit,             ///< �������� Start-����
    eTransmit_data,                 ///< �������� ������
    eTransmit_stopBit,              ///< �������� Stop-����
    eTransmit_stopSequence          ///< �������� Stop-�������
} eTransmit_Run;

//*****************************************************************************
/// \brief ��������� ���������� ��������� ������.
///
typedef struct
{
    uint8_t       data[MAX_DATA_LENGTH];        ///< ������ ������ ��� ��������.
    uint8_t       dataLength;                   ///< ����� ������������ ������, ����.
    bool          startTransm;                  ///< ���� ������ ��������.
    uint8_t       byteCnt;                      ///< ������� ������������ ������.
    uint8_t       bitCnt;                       ///< ������� ������������ ����� � �������� ������ �����.
    eTransmit_Run runCnt;                       ///< ���������� ��������� ��������.
    uint8_t       prescaler;                    ///< �������� ������������ �������� ��������.
    uint8_t       prescalerCnt;                 ///< ������� ������������ �������� ��������.
    void( *iniOutPin )( void );                 ///< ��������� �� ������� ������������� �����.
    void( *setOutPin )( bool level );           ///< ��������� �� ������� ��������� ����������� ������ �����.
} TransmitDataStr;

//*****************************************************************************
/// \brief �������������� ���������� ������� ������.
///
typedef enum
{
    eReceive_begin         = 0,        ///< �������� ���������� 1
    eReceive_stopSequence,             ///< ����� Stop-������� 
    eReceive_startBit,                 ///< ����� Start-����
    eReceive_data,                     ///< ����� ������
    eReceive_stopBit                   ///< ����� Stop-����
} eReceive_Run;

//*****************************************************************************
/// \brief ��������� ���������� ������� ������.
///
typedef struct
{
    uint8_t      data[MAX_DATA_LENGTH];              ///< ������ �������� ������.
    uint8_t      dataBuffer[MAX_DATA_LENGTH];        ///< ������ ����������� ������.
    uint8_t      dataLength;                         ///< ����� �������� ������, ����.
    uint8_t      byteCnt;                            ///< ������� ����������� ������.
    uint8_t      bitCnt;                             ///< ������� ����������� ����� � �������� ������ �����.
    uint8_t      quantumCnt;                         ///< ������� ������� �������� �����.
    uint8_t      lowLevelCnt;                        ///< ������� ������� �� ��������� ����������� 0.
    uint8_t      highLevelCnt;                       ///< ������� ������� �� ��������� ���������� 1.
    eReceive_Run runCnt;                             ///< ���������� ��������� ������.
    uint8_t      prescaler;                          ///< �������� ������������ ������� �������.
    uint8_t      prescalerCnt;                       ///< ������� ������������ ������� �������.
    void( *iniInPin )( void );                       ///< ��������� �� ������� ������������� �����.
    bool( *getInPin ) ( void );                      ///< ��������� �� ������� ������ �����.
} ReceiveDataStr;

//*****************************************************************************
/// \brief ���������, ����������� ��������� ������.
///
typedef struct
{
    TransmitDataStr txData;        ///< ���������� ��������� ���������� ��������� ������.
    ReceiveDataStr  rxData;        ///< ���������� ��������� ���������� ������� ������.   
} dsPIC33BlockExch_dataStr;

//*****************************************************************************
dsPIC33BlockExch_dataStr driverStr;        ///< �������� ���������, ����������� ��������� ��������.

//*****************************************************************************
/// \brief �������������� �������� ���������.
///
typedef enum
{
    eBitSelector_insuffSamples = 0,        ///< ������������� ���������� �������
    eBitSelector_logicLow,                 ///< ���������� 0
    eBitSelector_logicHigh,                ///< ���������� 1
    eBitSelector_error                     ///< ������ �������
} eBitSelector;

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������������� ������ ��� ��������.
/// \param transmStr � ��������� �� ��������� �������� ������.
///
static void transmitIni( TransmitDataStr *transmStr );

//*****************************************************************************
/// \brief ������������� ������ ��� ������.
/// \param receiveStr � ��������� �� ��������� ������ ������. 
///
static void receptionIni( ReceiveDataStr *receiveStr );

//*****************************************************************************
/// \brief ������������� ������ Tx.
///
static void iniTxPin( void );

//*****************************************************************************
/// \brief ������������� ����� Rx.
///
static void iniRxPin( void );

//*****************************************************************************
/// \brief ���������� ������� Tx.
/// \param level � �������. 
///
static void setTxPin( bool level );

//*****************************************************************************
/// \brief �������� ������.
/// \param transmStr � ��������� �� ��������� �������� ������.
///
static void transmition( TransmitDataStr *transmStr );

//*****************************************************************************
/// \brief ���������� ������� Rx.
/// \retval true  - ���������� ������� 1;
/// \retval false - ���������� ������� 0.
///
static bool getRxPin( void );

//*****************************************************************************
/// \brief ������� ��������.
/// \param quantumCnt   � ��������� �� ������� �������; 
/// \param highLevelCnt � ��������� �� ������� ���������� ������� � ���������� 1;
/// \param lowLewelCnt  � ��������� �� ������� ���������� ������� � ���������� 0.
/// \return ������������� �������� ���������.
///
static eBitSelector bitSelector( uint8_t *quantumCnt, uint8_t *highLevelCnt, uint8_t *lowLewelCnt );

//*****************************************************************************
/// \brief ����� ������.
/// \param receiveStr � ��������� �� ��������� ������ ������. 
///
static void reception( ReceiveDataStr *receiveStr );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �����������
const void *BlockExchDrv_ctor( void )
{
    transmitIni( &driverStr.txData );
    receptionIni( &driverStr.rxData );

    //������������� ���������� ��������� ������
    driverStr.rxData.prescaler = 2 - 1;
    driverStr.rxData.prescalerCnt = driverStr.rxData.prescaler;

    //������������� ���������� ��������� ��������
    driverStr.txData.prescaler = ( driverStr.rxData.prescaler + 1 ) * BIT_QUANTUM_NUMBER - 1;
    driverStr.txData.prescalerCnt = driverStr.txData.prescaler;

    return( const void * )&driverStr;
}

//*****************************************************************************
// ������ ������ �� ������������� ����������
int BlockExchDrv_get( const void *self, uint8_t *array, int size )
{
    dsPIC33BlockExch_dataStr *str = ( dsPIC33BlockExch_dataStr * )self;
    static const uint8_t CORRECT_LENGTH_VALUE = 2;
    uint8_t length = 0;
    uint8_t i = 0;
    
    //���������, ��� ������ �������
    if( !BlockExchDrv_isInReady( self ) || size == 0 ) return 0;
    //����������� ����� ������
    length = size > str->rxData.dataLength ? str->rxData.dataLength : size;
    if ( length < CORRECT_LENGTH_VALUE ) // ���� ������� ����� 2 ����, �� ������������
        return 0;
    //����������� �������� ������ � �������� ������� (������ - ���������)
    for( i = 0; i < length; i++ ) array[i] = str->rxData.data[length - 1 - i];
    str->rxData.dataLength = 0;
    return length;
}

//*****************************************************************************
// ���������� ���������� � ������ �������� ������
bool BlockExchDrv_isInReady( const void *self )
{
    dsPIC33BlockExch_dataStr *str = ( dsPIC33BlockExch_dataStr * )self;
    return str->rxData.dataLength != 0;
}

//*****************************************************************************
// �������� ������ �������� ��� ������
void BlockExchDrv_set( const void *self, uint8_t *array, int size )
{
    dsPIC33BlockExch_dataStr *str = ( dsPIC33BlockExch_dataStr * )self;
    str->txData.dataLength = size > MAX_DATA_LENGTH ? MAX_DATA_LENGTH : size;
    memcpy( str->txData.data, array, str->txData.dataLength );
}

//*****************************************************************************
// ���������� ���������� � ������ ����� ������
bool BlockExchDrv_isOutReady( const void *self )
{
    dsPIC33BlockExch_dataStr *str = ( dsPIC33BlockExch_dataStr * )self;
    return str->txData.dataLength == 0;
}

//*****************************************************************************
// ������� ����
void BlockExchDrv_run( const void *self, bool update )
{
    dsPIC33BlockExch_dataStr *str = ( dsPIC33BlockExch_dataStr * )self;
    //����������� ������ ��������
    if( update &&
        str->txData.runCnt == eTransmit_begin &&
        str->txData.dataLength != 0 )
    {
        //������ ��������
        str->txData.startTransm = true;
    }

    //����� ������� ��������
    if( str->txData.prescalerCnt != 0 ) str->txData.prescalerCnt--;
    else
    {
        transmition( &str->txData );
        str->txData.prescalerCnt = str->txData.prescaler;
    }

    //����� ������� ������
    if( str->rxData.prescalerCnt != 0 ) str->rxData.prescalerCnt--;
    else
    {
        reception( &str->rxData );
        str->rxData.prescalerCnt = str->rxData.prescaler;
    }
}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
// ������������� ������ ��� ��������
void transmitIni( TransmitDataStr *transmStr )
{
    //������������� ���������� �������
    transmStr->iniOutPin = iniTxPin;
    transmStr->setOutPin = setTxPin;
    //������������� ������ Tx
    transmStr->iniOutPin( );
    transmStr->setOutPin( true ); //���������� ����� � ���. 1
    //������������� ����������
    transmStr->dataLength = 0;
    transmStr->byteCnt = 0;
    transmStr->bitCnt = 0;
    transmStr->runCnt = eTransmit_begin;
    //������ ������ ��������
    transmStr->startTransm = false;
}

//*****************************************************************************
// ������������� ������ ��� ������
void receptionIni( ReceiveDataStr *receiveStr )
{
    //������������� ���������� �������
    receiveStr->iniInPin = iniRxPin;
    receiveStr->getInPin = getRxPin;
    //������������� ������ Rx
    receiveStr->iniInPin( );
    //������������� ����������
    receiveStr->dataLength = 0;
    receiveStr->byteCnt = 0;
    receiveStr->bitCnt = 0;
    receiveStr->quantumCnt = 0;
    receiveStr->lowLevelCnt = 0;
    receiveStr->highLevelCnt = 0;
    receiveStr->runCnt = eReceive_begin;
}

//*****************************************************************************
// ������������� ������ Tx
void iniTxPin( void )
{
    INIC_PORT_OUT( B, 7, eIO_openDrainOff );
}

//*****************************************************************************
// ������������� ����� Rx
void iniRxPin( void )
{
    INIC_PORT_IN( C, 13, eIO_pullOff );
}

//*****************************************************************************
// ���������� ������� Tx
static void setTxPin( bool level )
{
    _LATB7 = ConfigMK_isMaster( ) ? !level : level;
}

//*****************************************************************************
// �������� ������
static void transmition( TransmitDataStr *transmStr )
{
    switch( transmStr->runCnt )
    {
        case eTransmit_begin:
            //�������� ����� ���������� ��������
            if( !transmStr->startTransm ) break;
            //����� ����� ���������� ��������
            transmStr->startTransm = false;
            //������������� �������� ������
            transmStr->byteCnt = transmStr->dataLength - 1;
            //������� � �������� Start ����
            transmStr->runCnt = eTransmit_startBit;
            break;
        case eTransmit_startBit:
            //�������� Start ����
            transmStr->setOutPin( false );
            //������������� �������� �����
            transmStr->bitCnt = 8 - 1;
            //������� � �������� ����� ������
            transmStr->runCnt = eTransmit_data;
            break;
        case eTransmit_data:
            //�������� ���� ������ 
            transmStr->setOutPin( CHECKBIT( transmStr->data[transmStr->byteCnt], transmStr->bitCnt ) != 0 );
            //��������� �������� �����
            if( transmStr->bitCnt ) transmStr->bitCnt--;
            else
            { //���� �������
                //������� � �������� Stop ����
                transmStr->runCnt = eTransmit_stopBit;
                break;
            }
            break;
        case eTransmit_stopBit:
            //�������� Stop ����
            transmStr->setOutPin( true );
            //��������� ��������� ������
            if( transmStr->byteCnt ) transmStr->byteCnt--;
            else
            { //������ ��������
                transmStr->dataLength = 0; //�������� ����� ������������ ������
                //������� � �������� Stop ������������������
                transmStr->bitCnt = STOP_SEQUENCE - 1;
                transmStr->runCnt = eTransmit_stopSequence;
                break;
            }
            //������� � �������� Start ����
            transmStr->runCnt = eTransmit_startBit;
            break;
        case eTransmit_stopSequence:
            //�������� Stop ������������������
            transmStr->setOutPin( true );
            //��������� �������� �����
            if( transmStr->bitCnt ) transmStr->bitCnt--;
            else
            { //Stop ������������������ ��������
                //������� � ������
                transmStr->runCnt = eTransmit_begin;
                break;
            }
            break;
        default:
            transmStr->runCnt = eTransmit_begin;
    }
}

//*****************************************************************************
// ���������� ������� Rx
static bool getRxPin( void )
{
    return CHECKBIT( PORTC, 13 ) != 0;
}

//*****************************************************************************
// ������� ��������
static eBitSelector bitSelector( uint8_t *quantumCnt, uint8_t *highLevelCnt, uint8_t *lowLewelCnt )
{

    eBitSelector levelBit;
    if( *quantumCnt < BIT_QUANTUM_NUMBER )
    {
        //������������� ���������� �������
        return eBitSelector_insuffSamples;
    }
    if( *quantumCnt > BIT_QUANTUM_NUMBER ) levelBit = eBitSelector_error;
    else if( *highLevelCnt > *lowLewelCnt ) levelBit = eBitSelector_logicHigh;
    else if( *highLevelCnt < *lowLewelCnt ) levelBit = eBitSelector_logicLow;
    else levelBit = eBitSelector_error;
    *quantumCnt = 0;
    *highLevelCnt = 0;
    *lowLewelCnt = 0;

    return levelBit;
}

//*****************************************************************************
// ����� ������
static void reception( ReceiveDataStr *receiveStr )
{
    switch( receiveStr->runCnt )
    {
        case eReceive_begin:
            //�������� ������ "1"
            if( receiveStr->getInPin( ) != 0 )
            {
                receiveStr->highLevelCnt = 1;
                receiveStr->lowLevelCnt = 0;
                receiveStr->runCnt = eReceive_stopSequence;
                break;
            }
            break;
        case eReceive_stopSequence:
            //����� STOP ������� 
            if( receiveStr->getInPin( ) != 0 )
            {
                //������ ���. "1"
                receiveStr->highLevelCnt++;
                receiveStr->lowLevelCnt = 0;
                break;
            }
            //������ ���. "0"
            receiveStr->lowLevelCnt++;
            if( receiveStr->lowLevelCnt > BIT_QUANTUM_NUMBER / 2 )
            {
                if( receiveStr->highLevelCnt >= ( STOP_SEQUENCE + 1 ) * BIT_QUANTUM_NUMBER - 2 )
                {
                    //Stop ������� ����������� 
                    //������������� �������� �������
                    receiveStr->quantumCnt = receiveStr->lowLevelCnt;
                    //������������� ��������� ���������� �������
                    receiveStr->highLevelCnt = 0;
                    //������������� �������� ������
                    receiveStr->byteCnt = 0;
                    //������� � ������ Start ����
                    receiveStr->runCnt = eReceive_startBit;
                    break;
                }
                //��� �� STOP �������, ������� � �������� ���. "1"
                receiveStr->runCnt = eReceive_begin;
                break;
            }
            break;
        case eReceive_startBit:
            //����� Start-����
            if( receiveStr->getInPin( ) == 0 ) receiveStr->lowLevelCnt++;
            else receiveStr->highLevelCnt++;
            receiveStr->quantumCnt++;
            switch( bitSelector( &receiveStr->quantumCnt, &receiveStr->highLevelCnt, &receiveStr->lowLevelCnt ) )
            {
                case eBitSelector_insuffSamples:
                    break;
                case eBitSelector_logicLow: //Start-��� �������
                    receiveStr->bitCnt = 8 - 1; //������������� �������� �����
                    receiveStr->runCnt = eReceive_data;
                    break;
                case eBitSelector_logicHigh:
                    if( receiveStr->byteCnt == 0 )
                    { //��� �� ����� ���
                        receiveStr->runCnt = eReceive_begin;
                        break;
                    }
                    //������ Stop �������
                    //������� ������� �������. ��������� ����� �������� �������
                    receiveStr->dataLength = receiveStr->byteCnt;
                    //���������� ������ �� ������
                    memcpy( receiveStr->data, receiveStr->dataBuffer, receiveStr->dataLength );
                    receiveStr->highLevelCnt = BIT_QUANTUM_NUMBER * 2;
                    receiveStr->runCnt = eReceive_stopSequence;
                    break;
                case eBitSelector_error:
                    receiveStr->runCnt = eReceive_begin;
                    break;
            }
            break;
        case eReceive_data:
            //����� ������
            if( receiveStr->getInPin( ) == 0 ) receiveStr->lowLevelCnt++;
            else receiveStr->highLevelCnt++;
            receiveStr->quantumCnt++;
            switch( bitSelector( &receiveStr->quantumCnt, &receiveStr->highLevelCnt, &receiveStr->lowLevelCnt ) )
            {
                case eBitSelector_insuffSamples:
                    break;
                case eBitSelector_logicLow:
                    CLEARBIT( receiveStr->dataBuffer[receiveStr->byteCnt], receiveStr->bitCnt );
                    if( receiveStr->bitCnt ) receiveStr->bitCnt--; //���� ��� �� ������, �������� ���������� ����
                    else receiveStr->runCnt = eReceive_stopBit; //���� ������, ������� � �������� Stop ����
                    break;
                case eBitSelector_logicHigh:
                    SETBIT( receiveStr->dataBuffer[receiveStr->byteCnt], receiveStr->bitCnt );
                    if( receiveStr->bitCnt ) receiveStr->bitCnt--; //���� ��� �� ������, �������� ���������� ����
                    else receiveStr->runCnt = eReceive_stopBit; //���� ������, ������� � �������� Stop ����
                    break;
                case eBitSelector_error:
                    receiveStr->runCnt = eReceive_begin;
                    break;
            }
            break;
        case eReceive_stopBit:
            //����� Stop ����
            if( receiveStr->getInPin( ) == 0 ) receiveStr->lowLevelCnt++;
            else receiveStr->highLevelCnt++;
            receiveStr->quantumCnt++;
            switch( bitSelector( &receiveStr->quantumCnt, &receiveStr->highLevelCnt, &receiveStr->lowLevelCnt ) )
            {
                case eBitSelector_insuffSamples:
                    break;
                case eBitSelector_logicHigh:
                    if( receiveStr->byteCnt < MAX_DATA_LENGTH - 1 )
                    {
                        receiveStr->byteCnt++;
                        receiveStr->runCnt = eReceive_startBit; //������� � ������ Start ����
                        break;
                    }
                    //������. ���������� ���� ��������� MAX_DATA_LENGTH
                    receiveStr->runCnt = eReceive_begin;
                    break;
                case eBitSelector_logicLow:
                    receiveStr->runCnt = eReceive_begin;
                    break;
                case eBitSelector_error:
                    receiveStr->runCnt = eReceive_begin;
                    break;
            }
            break;
        default:
            receiveStr->runCnt = eReceive_begin;
    }
}

//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   05-05-2017
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
* 
* ������ 1.0.2
* ����   10-10-2019
* �����  ��������� �.�.
*
* ���������:
* � ������� BlockExchDrv_get ��������� �������� ����, ��� �������� ����� �������� �� ����� 2 ����
*/
