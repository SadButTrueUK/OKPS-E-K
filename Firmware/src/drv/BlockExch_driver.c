/**
* \file    BlockExch_driver.c
* \brief   \copybrief BlockExch_driver.h
*
* \version 1.0.2
* \date    10-10-2019
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <string.h>
#include "typeMK.h"
#include "BlockExch_driver.h"
#include "ConfigMK.h"
#include "IOports.h"

//*****************************************************************************
// Глобальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
#define BIT_QUANTUM_NUMBER 4U        ///< Количество выборок для приема одного бита.
#define STOP_SEQUENCE      8U        ///< Количество битов в Stop-последовательности.

//*****************************************************************************
// Определение типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Идентификаторы управления передачей данных.
///
typedef enum
{
    eTransmit_begin     = 0,        ///< инициализация данных
    eTransmit_startBit,             ///< передача Start-бита
    eTransmit_data,                 ///< передача данных
    eTransmit_stopBit,              ///< передача Stop-бита
    eTransmit_stopSequence          ///< передача Stop-посылки
} eTransmit_Run;

//*****************************************************************************
/// \brief Структура управления передачей данных.
///
typedef struct
{
    uint8_t       data[MAX_DATA_LENGTH];        ///< Массив данных для передачи.
    uint8_t       dataLength;                   ///< Длина передаваемых данных, байт.
    bool          startTransm;                  ///< Флаг начала передачи.
    uint8_t       byteCnt;                      ///< Счетчик передаваемых байтов.
    uint8_t       bitCnt;                       ///< Счетчик передаваемых битов в пределах одного байта.
    eTransmit_Run runCnt;                       ///< Переменная состояния передачи.
    uint8_t       prescaler;                    ///< Значение предделителя скорости передачи.
    uint8_t       prescalerCnt;                 ///< Счетчик предделителя скорости передачи.
    void( *iniOutPin )( void );                 ///< Указатель на функцию инициализации порта.
    void( *setOutPin )( bool level );           ///< Указатель на функцию установки логического вывода порта.
} TransmitDataStr;

//*****************************************************************************
/// \brief Идентификаторы управления приемом данных.
///
typedef enum
{
    eReceive_begin         = 0,        ///< ожидание логической 1
    eReceive_stopSequence,             ///< прием Stop-посылки 
    eReceive_startBit,                 ///< прием Start-бита
    eReceive_data,                     ///< прием данных
    eReceive_stopBit                   ///< прием Stop-бита
} eReceive_Run;

//*****************************************************************************
/// \brief Структура управления приемом данных.
///
typedef struct
{
    uint8_t      data[MAX_DATA_LENGTH];              ///< Массив принятых данных.
    uint8_t      dataBuffer[MAX_DATA_LENGTH];        ///< Массив принимаемых данных.
    uint8_t      dataLength;                         ///< Длина принятых данных, байт.
    uint8_t      byteCnt;                            ///< Счетчик принимаемых байтов.
    uint8_t      bitCnt;                             ///< Счетчик принимаемых битов в пределах одного байта.
    uint8_t      quantumCnt;                         ///< Счетчик выборок значений порта.
    uint8_t      lowLevelCnt;                        ///< Счетчик выборок со значением логического 0.
    uint8_t      highLevelCnt;                       ///< Счетчик выборок со значением логической 1.
    eReceive_Run runCnt;                             ///< Переменная состояния приема.
    uint8_t      prescaler;                          ///< Значение предделителя частоты выборок.
    uint8_t      prescalerCnt;                       ///< Счетчик предделителя частоты выборок.
    void( *iniInPin )( void );                       ///< Указатель на функцию инициализации порта.
    bool( *getInPin ) ( void );                      ///< Указатель на функцию чтения порта.
} ReceiveDataStr;

//*****************************************************************************
/// \brief Структура, описывающая состояние модуля.
///
typedef struct
{
    TransmitDataStr txData;        ///< Переменная структуры управления передачей данных.
    ReceiveDataStr  rxData;        ///< Переменная структуры управления приемом данных.   
} dsPIC33BlockExch_dataStr;

//*****************************************************************************
dsPIC33BlockExch_dataStr driverStr;        ///< Создание структуры, описывающей состояние драйвера.

//*****************************************************************************
/// \brief Идентификаторы битового селектора.
///
typedef enum
{
    eBitSelector_insuffSamples = 0,        ///< недостаточное количество выборок
    eBitSelector_logicLow,                 ///< логический 0
    eBitSelector_logicHigh,                ///< логическая 1
    eBitSelector_error                     ///< ошибка формата
} eBitSelector;

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Инициализация данных для передачи.
/// \param transmStr – указатель на структуру передачи данных.
///
static void transmitIni( TransmitDataStr *transmStr );

//*****************************************************************************
/// \brief Инициализация данных для приема.
/// \param receiveStr – указатель на структуру приема данных. 
///
static void receptionIni( ReceiveDataStr *receiveStr );

//*****************************************************************************
/// \brief Инициализация выхода Tx.
///
static void iniTxPin( void );

//*****************************************************************************
/// \brief Инициализация входа Rx.
///
static void iniRxPin( void );

//*****************************************************************************
/// \brief Управление выходом Tx.
/// \param level – уровень. 
///
static void setTxPin( bool level );

//*****************************************************************************
/// \brief Передача данных.
/// \param transmStr – указатель на структуру передачи данных.
///
static void transmition( TransmitDataStr *transmStr );

//*****************************************************************************
/// \brief Управление выходом Rx.
/// \retval true  - логический уровень 1;
/// \retval false - логический уровень 0.
///
static bool getRxPin( void );

//*****************************************************************************
/// \brief Битовый селектор.
/// \param quantumCnt   – указатель на счетчик квантов; 
/// \param highLevelCnt – указатель на счетчик количества выборок с логической 1;
/// \param lowLewelCnt  – указатель на счетчик количества выборок с логическим 0.
/// \return Идентификатор битового селектора.
///
static eBitSelector bitSelector( uint8_t *quantumCnt, uint8_t *highLevelCnt, uint8_t *lowLewelCnt );

//*****************************************************************************
/// \brief Прием данных.
/// \param receiveStr – указатель на структуру приема данных. 
///
static void reception( ReceiveDataStr *receiveStr );

//*****************************************************************************
// Реализация интерфейсных Функций
//*****************************************************************************

//*****************************************************************************
// Конструктор
const void *BlockExchDrv_ctor( void )
{
    transmitIni( &driverStr.txData );
    receptionIni( &driverStr.rxData );

    //Инициализация прескалера обработки приема
    driverStr.rxData.prescaler = 2 - 1;
    driverStr.rxData.prescalerCnt = driverStr.rxData.prescaler;

    //Инициализация прескалера обработки передачи
    driverStr.txData.prescaler = ( driverStr.rxData.prescaler + 1 ) * BIT_QUANTUM_NUMBER - 1;
    driverStr.txData.prescalerCnt = driverStr.txData.prescaler;

    return( const void * )&driverStr;
}

//*****************************************************************************
// Чтение данных из периферийного устройства
int BlockExchDrv_get( const void *self, uint8_t *array, int size )
{
    dsPIC33BlockExch_dataStr *str = ( dsPIC33BlockExch_dataStr * )self;
    static const uint8_t CORRECT_LENGTH_VALUE = 2;
    uint8_t length = 0;
    uint8_t i = 0;
    
    //Проверить, что данные приняты
    if( !BlockExchDrv_isInReady( self ) || size == 0 ) return 0;
    //Определение длины данных
    length = size > str->rxData.dataLength ? str->rxData.dataLength : size;
    if ( length < CORRECT_LENGTH_VALUE ) // если принято менее 2 байт, не обрабатывать
        return 0;
    //Копирование принятых данных в обратном порядке (первый - последний)
    for( i = 0; i < length; i++ ) array[i] = str->rxData.data[length - 1 - i];
    str->rxData.dataLength = 0;
    return length;
}

//*****************************************************************************
// Готовность устройства к чтению принятых данных
bool BlockExchDrv_isInReady( const void *self )
{
    dsPIC33BlockExch_dataStr *str = ( dsPIC33BlockExch_dataStr * )self;
    return str->rxData.dataLength != 0;
}

//*****************************************************************************
// Передача данных драйверу для вывода
void BlockExchDrv_set( const void *self, uint8_t *array, int size )
{
    dsPIC33BlockExch_dataStr *str = ( dsPIC33BlockExch_dataStr * )self;
    str->txData.dataLength = size > MAX_DATA_LENGTH ? MAX_DATA_LENGTH : size;
    memcpy( str->txData.data, array, str->txData.dataLength );
}

//*****************************************************************************
// Готовность устройства к выводу новых данных
bool BlockExchDrv_isOutReady( const void *self )
{
    dsPIC33BlockExch_dataStr *str = ( dsPIC33BlockExch_dataStr * )self;
    return str->txData.dataLength == 0;
}

//*****************************************************************************
// Рабочий цикл
void BlockExchDrv_run( const void *self, bool update )
{
    dsPIC33BlockExch_dataStr *str = ( dsPIC33BlockExch_dataStr * )self;
    //Определение начала передачи
    if( update &&
        str->txData.runCnt == eTransmit_begin &&
        str->txData.dataLength != 0 )
    {
        //Начать передачу
        str->txData.startTransm = true;
    }

    //Вызов функции передачи
    if( str->txData.prescalerCnt != 0 ) str->txData.prescalerCnt--;
    else
    {
        transmition( &str->txData );
        str->txData.prescalerCnt = str->txData.prescaler;
    }

    //Вызов функции приема
    if( str->rxData.prescalerCnt != 0 ) str->rxData.prescalerCnt--;
    else
    {
        reception( &str->rxData );
        str->rxData.prescalerCnt = str->rxData.prescaler;
    }
}

//*****************************************************************************
// Реализация локальных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация данных для передачи
void transmitIni( TransmitDataStr *transmStr )
{
    //Инициализация указателей функций
    transmStr->iniOutPin = iniTxPin;
    transmStr->setOutPin = setTxPin;
    //Инициализация вывода Tx
    transmStr->iniOutPin( );
    transmStr->setOutPin( true ); //Установить вывод в лог. 1
    //Инициализация переменных
    transmStr->dataLength = 0;
    transmStr->byteCnt = 0;
    transmStr->bitCnt = 0;
    transmStr->runCnt = eTransmit_begin;
    //Запрет начала передачи
    transmStr->startTransm = false;
}

//*****************************************************************************
// Инициализация данных для приема
void receptionIni( ReceiveDataStr *receiveStr )
{
    //Инициализация указателей функций
    receiveStr->iniInPin = iniRxPin;
    receiveStr->getInPin = getRxPin;
    //Инициализация вывода Rx
    receiveStr->iniInPin( );
    //Инициализация переменных
    receiveStr->dataLength = 0;
    receiveStr->byteCnt = 0;
    receiveStr->bitCnt = 0;
    receiveStr->quantumCnt = 0;
    receiveStr->lowLevelCnt = 0;
    receiveStr->highLevelCnt = 0;
    receiveStr->runCnt = eReceive_begin;
}

//*****************************************************************************
// Инициализация выхода Tx
void iniTxPin( void )
{
    INIC_PORT_OUT( B, 7, eIO_openDrainOff );
}

//*****************************************************************************
// Инициализация входа Rx
void iniRxPin( void )
{
    INIC_PORT_IN( C, 13, eIO_pullOff );
}

//*****************************************************************************
// Управление выходом Tx
static void setTxPin( bool level )
{
    _LATB7 = ConfigMK_isMaster( ) ? !level : level;
}

//*****************************************************************************
// Передача данных
static void transmition( TransmitDataStr *transmStr )
{
    switch( transmStr->runCnt )
    {
        case eTransmit_begin:
            //Проверка флага разрешения передачи
            if( !transmStr->startTransm ) break;
            //Сброс флага разрешения передачи
            transmStr->startTransm = false;
            //Инициализация счетчика байтов
            transmStr->byteCnt = transmStr->dataLength - 1;
            //Переход к передаче Start бита
            transmStr->runCnt = eTransmit_startBit;
            break;
        case eTransmit_startBit:
            //Передача Start бита
            transmStr->setOutPin( false );
            //Инициализация счетчика битов
            transmStr->bitCnt = 8 - 1;
            //Переход к передаче байта данных
            transmStr->runCnt = eTransmit_data;
            break;
        case eTransmit_data:
            //Передача бита данных 
            transmStr->setOutPin( CHECKBIT( transmStr->data[transmStr->byteCnt], transmStr->bitCnt ) != 0 );
            //Декремент счетчика битов
            if( transmStr->bitCnt ) transmStr->bitCnt--;
            else
            { //Байт передан
                //Переход к передаче Stop бита
                transmStr->runCnt = eTransmit_stopBit;
                break;
            }
            break;
        case eTransmit_stopBit:
            //Передача Stop бита
            transmStr->setOutPin( true );
            //Декремент счетчиков байтов
            if( transmStr->byteCnt ) transmStr->byteCnt--;
            else
            { //Данные переданы
                transmStr->dataLength = 0; //Обнулить длину передаваемых данных
                //Переход к передаче Stop последовательности
                transmStr->bitCnt = STOP_SEQUENCE - 1;
                transmStr->runCnt = eTransmit_stopSequence;
                break;
            }
            //Переход к передаче Start бита
            transmStr->runCnt = eTransmit_startBit;
            break;
        case eTransmit_stopSequence:
            //Передача Stop последовательности
            transmStr->setOutPin( true );
            //Декремент счетчика битов
            if( transmStr->bitCnt ) transmStr->bitCnt--;
            else
            { //Stop последовательность передана
                //Переход к началу
                transmStr->runCnt = eTransmit_begin;
                break;
            }
            break;
        default:
            transmStr->runCnt = eTransmit_begin;
    }
}

//*****************************************************************************
// Управление выходом Rx
static bool getRxPin( void )
{
    return CHECKBIT( PORTC, 13 ) != 0;
}

//*****************************************************************************
// Битовый селектор
static eBitSelector bitSelector( uint8_t *quantumCnt, uint8_t *highLevelCnt, uint8_t *lowLewelCnt )
{

    eBitSelector levelBit;
    if( *quantumCnt < BIT_QUANTUM_NUMBER )
    {
        //Недостаточное количество выборок
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
// Прием данных
static void reception( ReceiveDataStr *receiveStr )
{
    switch( receiveStr->runCnt )
    {
        case eReceive_begin:
            //Ожидание уровня "1"
            if( receiveStr->getInPin( ) != 0 )
            {
                receiveStr->highLevelCnt = 1;
                receiveStr->lowLevelCnt = 0;
                receiveStr->runCnt = eReceive_stopSequence;
                break;
            }
            break;
        case eReceive_stopSequence:
            //Прием STOP посылки 
            if( receiveStr->getInPin( ) != 0 )
            {
                //Принят лог. "1"
                receiveStr->highLevelCnt++;
                receiveStr->lowLevelCnt = 0;
                break;
            }
            //Принят лог. "0"
            receiveStr->lowLevelCnt++;
            if( receiveStr->lowLevelCnt > BIT_QUANTUM_NUMBER / 2 )
            {
                if( receiveStr->highLevelCnt >= ( STOP_SEQUENCE + 1 ) * BIT_QUANTUM_NUMBER - 2 )
                {
                    //Stop посылка закончилась 
                    //Инициализация счетчика квантов
                    receiveStr->quantumCnt = receiveStr->lowLevelCnt;
                    //Инициализация счетчиков логических уровней
                    receiveStr->highLevelCnt = 0;
                    //Инициализация счетчика байтов
                    receiveStr->byteCnt = 0;
                    //Переход к приему Start бита
                    receiveStr->runCnt = eReceive_startBit;
                    break;
                }
                //Это не STOP посылка, переход к ожиданию лог. "1"
                receiveStr->runCnt = eReceive_begin;
                break;
            }
            break;
        case eReceive_startBit:
            //Прием Start-бита
            if( receiveStr->getInPin( ) == 0 ) receiveStr->lowLevelCnt++;
            else receiveStr->highLevelCnt++;
            receiveStr->quantumCnt++;
            switch( bitSelector( &receiveStr->quantumCnt, &receiveStr->highLevelCnt, &receiveStr->lowLevelCnt ) )
            {
                case eBitSelector_insuffSamples:
                    break;
                case eBitSelector_logicLow: //Start-бит получен
                    receiveStr->bitCnt = 8 - 1; //Инициализация счетчика битов
                    receiveStr->runCnt = eReceive_data;
                    break;
                case eBitSelector_logicHigh:
                    if( receiveStr->byteCnt == 0 )
                    { //Это не старт бит
                        receiveStr->runCnt = eReceive_begin;
                        break;
                    }
                    //Начало Stop посылки
                    //Посылка принята успешно. Установка длины принятой посылки
                    receiveStr->dataLength = receiveStr->byteCnt;
                    //Считывание данных из буфера
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
            //Прием данных
            if( receiveStr->getInPin( ) == 0 ) receiveStr->lowLevelCnt++;
            else receiveStr->highLevelCnt++;
            receiveStr->quantumCnt++;
            switch( bitSelector( &receiveStr->quantumCnt, &receiveStr->highLevelCnt, &receiveStr->lowLevelCnt ) )
            {
                case eBitSelector_insuffSamples:
                    break;
                case eBitSelector_logicLow:
                    CLEARBIT( receiveStr->dataBuffer[receiveStr->byteCnt], receiveStr->bitCnt );
                    if( receiveStr->bitCnt ) receiveStr->bitCnt--; //Байт еще не принят, ожидание следующего бита
                    else receiveStr->runCnt = eReceive_stopBit; //Байт принят, переход к ожиданию Stop бита
                    break;
                case eBitSelector_logicHigh:
                    SETBIT( receiveStr->dataBuffer[receiveStr->byteCnt], receiveStr->bitCnt );
                    if( receiveStr->bitCnt ) receiveStr->bitCnt--; //Байт еще не принят, ожидание следующего бита
                    else receiveStr->runCnt = eReceive_stopBit; //Байт принят, переход к ожиданию Stop бита
                    break;
                case eBitSelector_error:
                    receiveStr->runCnt = eReceive_begin;
                    break;
            }
            break;
        case eReceive_stopBit:
            //Прием Stop бита
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
                        receiveStr->runCnt = eReceive_startBit; //Переход к приему Start бита
                        break;
                    }
                    //Ошибка. Количество байт превышает MAX_DATA_LENGTH
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
* История изменений:
*
* Версия 1.0.1
* Дата   05-05-2017
* Автор  Кругликов В.П.
*
* Изменения:
*    Базовая версия.
* 
* Версия 1.0.2
* Дата   10-10-2019
* Автор  Кругликов В.П.
*
* Изменения:
* В функции BlockExchDrv_get добавлена проверка того, что принятый пакет содержит не менее 2 байт
*/
