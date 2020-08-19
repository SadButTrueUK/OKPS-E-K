/**
* \file    BlockExch.c
* \brief   \copybrief BlockExch.h
*
* \version 1.0.1
* \date    22-05-2017
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
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
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
#define T_O_DATA_SYNCHRO    5             ///< Таймаут синхронизации принятых данных, мс.
#define SYNCHRO_EXCHANGE    0xAAAA        ///< Значение синхронизации начала обмена.
#define SYNCHRO_EXCHANGE_TO 100           ///< Таймаут синхронизации начала обмена, мс.

//*****************************************************************************
// Определение глобальных типизированных констант
//*****************************************************************************

//*****************************************************************************
const void *BlockExch_drvStr;              ///< Указатель на драйвер.

//*****************************************************************************
// Определение типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Идентификаторы управления приемом данных.
///
typedef enum
{
    eRecept_begin   = 0,        ///< ожидание принятых данных
    eRecept_synchro             ///< ожидание результата синхронизации данных
} eRecept_run;

//*****************************************************************************
/// \brief Тип структуры приема данных с линии связи.
///
typedef struct
{
    uint8_t     buffArr[MAX_DATA_LENGTH];        ///< Буфер данных.
    uint8_t     syncTimeOutCnt;                  ///< Счетчик таймаута синхронизации.
    eRecept_run runCnt;                          ///< Счетчик состояния приема данных.
} ReceptStr;

//*****************************************************************************
/// \brief Структура состояния.
///
struct BlockExch_Str_
{
    uint8_t   transmitData[MAX_DATA_LENGTH];          ///< Данные для передачи + CRC8.
    uint8_t   receptData[MAX_DATA_LENGTH - 1];        ///< Принятые данные - CRC8.
    uint8_t   dataLength;                             ///< Длина данных для обмена.
    uint8_t   eICId_BlockExchMy;                      ///< ID синхронизируемой переменной (свой МК).
    uint8_t   eICId_BlockExchNeighbor;                ///< ID синхронизируемой переменной (соседний МК).
    ReceptStr inStr;                                  ///< Структура приема данных.
    uint16_t  timeNoExchange;                         ///< Время формирования сигнала отсутствия обмена.
    uint16_t  noExchangeCnt;                          ///< Счетчик времени отсутствия обмена.
    bool      ctrl;                                   ///< Включение обмена.
    bool      startSync;                              ///< Флаг синхронизации начала обмена.
    uint16_t  myData;                                 ///< данные, пеедаваемые в МКО своего МК
    uint16_t  neighborData;                           ///< данные, получаемые из МКО соседа
    uint8_t   countInvalidTransact;                   ///< счётчик пакетов с ошибочными значениями контрольной суммы 
};

//*****************************************************************************
// Определение локальных переменных
//*****************************************************************************

//*****************************************************************************
/// \brief Создание структуры состояния.
///
static BlockExch_Str bExchStr =
{
    .ctrl = false,
    .startSync = false,
    .countInvalidTransact = 0
};

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Прием данных с линии связи.
/// 
static void checkSyncEvents( void );

//*****************************************************************************
/// \brief Передача установленных данных в линию связи.
/// \param str - указатель на структуру состояния.
///
static void transmitionRun( BlockExch_Str *str );

//*****************************************************************************
/// \brief Прием данных с линии связи.
/// \param str - указатель на структуру состояния.
///
static void receptionRun( BlockExch_Str *str );

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация
BlockExch_Str *BlockExch_ctor( uint8_t length, uint16_t noExchangeTime )
{
    uint8_t i;
    BlockExch_drvStr = BlockExchDrv_ctor( );
    if( length == 0 || length > MAX_DATA_LENGTH - 1 )
    {
        //Неправильно задана длина данных обмена
        ERROR_ID( eGrPS_BlockExch, ePS_BlockExchErrorDataLength );
    }
    bExchStr.dataLength = length;
    //Инициализация массивов
    for( i = 0; i < MAX_DATA_LENGTH; i++ )
    {
        bExchStr.transmitData[i] = 0;
        bExchStr.inStr.buffArr[i] = 0;
    }
    for( i = 0; i < MAX_DATA_LENGTH - 1; i++ ) bExchStr.receptData[i] = 0;
    bExchStr.timeNoExchange = noExchangeTime;
    bExchStr.noExchangeCnt = bExchStr.timeNoExchange;
    
    //Переменная синхронизации (старший байт для передачи, младший - для приема)
    bExchStr.eICId_BlockExchMy = ConfigMK_isMaster() ?  eICId_BlockExchM : eICId_BlockExchS;
    bExchStr.eICId_BlockExchNeighbor = ConfigMK_isMaster() ?  eICId_BlockExchS : eICId_BlockExchM;
    InterChannel_setParamSettings( bExchStr.eICId_BlockExchMy , eScriptTransmit, eProcSyncOff, eProcCheckOff, 0, 0,  
                                   0, 0, 0); 
    InterChannel_setParamSettings( bExchStr.eICId_BlockExchNeighbor, eScriptTransmit, eProcSyncOff, eProcCheckOff, 0, 0,  
                                   0, 0, 0); 
     //Установить начальное состояние управления приемом
    bExchStr.inStr.runCnt = eRecept_begin;
    //Синхронизировать включение обмена
    if( !InterChannel_isHandling( bExchStr.eICId_BlockExchMy ) )
    {    
        InterChannel_synchronize( bExchStr.eICId_BlockExchMy, SYNCHRO_EXCHANGE );
    }
    
    //Взвести таймаут синхронизации 
    bExchStr.inStr.syncTimeOutCnt = SYNCHRO_EXCHANGE_TO;
    return &bExchStr;
}

//*****************************************************************************
//Проверка того, что собітия синхронизации своего параметра и параметра соседи наступили
static void checkSyncEvents( void )
{
    if( InterChannel_syncEvent( bExchStr.eICId_BlockExchMy ) )
        bExchStr.myData = InterChannel_getData( bExchStr.eICId_BlockExchMy );
    if( InterChannel_syncEvent( bExchStr.eICId_BlockExchNeighbor ) )
        bExchStr.neighborData = InterChannel_getData( bExchStr.eICId_BlockExchNeighbor );
}
//*****************************************************************************
// Управление приемом и передачей
void BlockExch_run( BlockExch_Str *str )
{
    if( str->ctrl == 0 )
    { //Обмен еще не включен
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
// Прерывание
void BlockExch_interrupt( BlockExch_Str *str, uint16_t interruptNumb )
{
    if( !str->ctrl )
    {
        //Инкремент таймаута синхронизации
        if( str->inStr.syncTimeOutCnt ) str->inStr.syncTimeOutCnt--;
        else
        {
            //Истекло время ожидания синхронизации
            ERROR_ID( eGrPS_BlockExch, ePS_BlockExchErrorStartSync );
        }
        //Проверка флага начала обмена
        if( str->startSync && interruptNumb == 15 )
        { //Включить обмен
            str->ctrl = true;
        }
    }
    BlockExchDrv_run( BlockExch_drvStr, interruptNumb == 15 );
}

//*****************************************************************************
// Установление данных для передачи
void BlockExch_setData( BlockExch_Str *str, const uint8_t *data )
{
    memcpy( str->transmitData, data, str->dataLength ); //Копирование данных
    //Подсчет и добавление CRC8
    str->transmitData[str->dataLength] = crc8_update( 0, data, str->dataLength );
}

//*****************************************************************************
// Получает принятые данные
bool BlockExch_getData( BlockExch_Str *str, uint8_t *data )
{
    if( str->noExchangeCnt == 0 ) 
		return false; //Данные не принимаются
    memcpy( data, str->receptData, str->dataLength ); //Копирование данных
    return true;
}

//*****************************************************************************
// Получает состояние связи с соседним прибором
bool BlockExch_getConnectionState( BlockExch_Str *str )
{
    return str->noExchangeCnt != 0;
}

//*****************************************************************************
// Реализация локальных функций
//*****************************************************************************

//*****************************************************************************
// Передача установленных данных в линию связи
static void transmitionRun( BlockExch_Str *str )
{
    //Ожидание освобождения буфера
    if( BlockExchDrv_isOutReady( BlockExch_drvStr ) == 0 ) return;
    //Передача данных
    BlockExchDrv_set( BlockExch_drvStr, str->transmitData, str->dataLength + 1 );
}

//Приём данных от соседнего прибора
void receptionRun( BlockExch_Str *str )
{
    static const uint8_t NUM_INVALID_TRANSACTIONS = 4; //пороговое значение ошибочных значений CRC
  
    //Декремент таймаута
    if( str->inStr.syncTimeOutCnt != 0 ) str->inStr.syncTimeOutCnt--;
    switch( str->inStr.runCnt )
    {
        case eRecept_begin: //Начальная синхронизация
            if( BlockExchDrv_get( BlockExch_drvStr, str->inStr.buffArr, str->dataLength + 1 ) == 0 ) 
                break;
            
            if( crc8_update( 0, str->inStr.buffArr, str->dataLength ) == str->inStr.buffArr[str->dataLength] )
            {                
                    //Синхронизация принятых данных            
                if( !InterChannel_isHandling( str->eICId_BlockExchMy ) )
                {    
                    InterChannel_synchronize( str->eICId_BlockExchMy, str->inStr.buffArr[str->dataLength] );
                    //Установка таймаута синхронизации
                    str->inStr.syncTimeOutCnt = T_O_DATA_SYNCHRO;
                    //Переход к ожиданию приема синхронизированной переменной
                    str->inStr.runCnt = eRecept_synchro;
                }
            }
            break;
        case eRecept_synchro: //Синхронизация принятых данных
            if( str->inStr.syncTimeOutCnt == 0 )
            {    
                checkSyncEvents(); 
                if( bExchStr.myData == bExchStr.neighborData ) //данные совпадают между двумя МК
                {
                    //Запись принятой посылки
                    memcpy( str->receptData, str->inStr.buffArr, str->dataLength );
                    //Установка счетчика времени отсутствия обмена
                    str->noExchangeCnt = str->timeNoExchange;
                    //Переход к ожиданию следующей посылки 
                    str->countInvalidTransact = 0; //обнулить счётчик ошибок
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
                             str->inStr.buffArr[0], //принятые данные 
                             str->inStr.buffArr[1] ); //принятая CRC
            break;
    }
}

 
//*****************************************************************************
/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   22-05-2017
* Автор  Кругликов В.П.
* 
* Изменения:
*     Базовая версия.
* 
* Версия 1.0.2
* Дата 10-10-2019  
* Изменения: 
* Изменён сценарий сихронизации и последующая обработка контрольной суммы межблочного обмена 
 
 */
