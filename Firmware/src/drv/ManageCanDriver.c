/**
* \file    ManageCanDriver.c
* \brief   \copybrief ManageCanDriver.h
*
* \version 1.0.6
* \date    05-02-2021
* \author  Третьяков В.Ж.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <xc.h>
#include <string.h>
#include "iodrv.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"
#include "Main.h" 
#include "EcanDriver.h"
#include "ManageCanDriver.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
/// \brief Максимальный размер пакета обмена.
///
#define SIZE_MAX_MESSAGE    8

//*****************************************************************************
/// \brief Время ожидания завершения передачи, в количествах вызовов функции.
///
#define WAITE_TRANSMITE     10

//*****************************************************************************
/// \brief Количество повторов передачи при отсутствии подтверждения.
///
#define REPEAT_TRANSMITE    4

//*****************************************************************************
/// \brief Время передачи двух пакетов, мкс.
///
#define TIME_TRANSMITE_2_PACKET ( 110 * 2 + 12 + 32 )  // 110 время пакета, 12 -между пакетами, 32 - на разброс каналов

//*****************************************************************************
// Объявление типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Состояние передачи.
///
typedef enum
{
    eTransmiteState_Idle        = 0,              ///< передатчик свободен и готов к работе
    eTransmiteState_Start,                        ///< запуск передачи данных
    eTransmiteState_InProgress                    ///< процесс передачи данных
} TransmiteState;

//*****************************************************************************
/// \brief Структура данных драйвера.
///
typedef struct
{
    ArrayIoDriver  pFunctions;                    ///< Адреса интерфейсных функций.
    TransmiteState transmiteState;                ///< Состояние передатчика.
    uint8_t        aIn[SIZE_MAX_MESSAGE];         ///< Принятая информация.
    uint8_t        aOut[SIZE_MAX_MESSAGE];        ///< Информация для передачи.
    bool           isInReady;                     ///< Признак наличия принятой информации.
    uint16_t       cWaiteTx;                      ///< Счетчик циклов ожидания подтверждения приема соседним МК.
    uint16_t       cRepeatTx;                     ///< Счетчик повторов передачи в соседний МК.
    const void     *pDrv;                         ///< Указатель на драйвер CAN.
    uint8_t        lengthPacket;                  ///< Длина пакета данных.
    uint8_t        numberPacketOut;               ///< Переменная для организации номера пакета в передаваемых данных. 
    uint8_t        numberPacketIn;                ///< Переменная для номера последнего принятого пакета данных. 
} ManageCanDriver;

//*****************************************************************************
// Объявление локальных переменных
//*****************************************************************************

//*****************************************************************************
static ManageCanDriver manageCanDriver;           ///< Данные драйвера.

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Запрос завершения передачи (готовности к передаче).
/// \param self - указатель на данные передачи от соседнего МК.
/// \retval true  - да;
/// \retval false - нет.
///
static bool ManageCanDriver_isOut( const void *self );

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Запрос результата операции.
uint16_t ManageCanDriver_getError( const void *self )
{
    ERROR_ID( eGrPS_ManageCanDrv, ePS_ErrorAlg );
    return 0;
}

//*****************************************************************************
// Сброс драйвера.
void ManageCanDriver_reset( const void *self )
{
    ERROR_ID( eGrPS_ManageCanDrv, ePS_ErrorAlg );
}

//*****************************************************************************
// Запуск операции в драйвере
void ManageCanDriver_start( const void *self, uint8_t *array, uint16_t size )
{
    ERROR_ID( eGrPS_ManageCanDrv, ePS_ErrorAlg );
}

//*****************************************************************************
// Конструктор  
const ArrayIoDriver *ManageCanDriver_ctor( EcanNumber numCan,
                                    uint16_t txSid,
                                    uint16_t rxSid,
                                    uint16_t mode,
                                    uint16_t lengthPacket )
{
    manageCanDriver.pFunctions.run = ManageCanDriver_run;
    manageCanDriver.pFunctions.isInReady = ManageCanDriver_isInReady;
    manageCanDriver.pFunctions.isOutReady = ManageCanDriver_isOutReady;
    manageCanDriver.pFunctions.set = ManageCanDriver_set;
    manageCanDriver.pFunctions.get = ManageCanDriver_get;
    manageCanDriver.pFunctions.getError = ManageCanDriver_getError;
    manageCanDriver.pFunctions.reset = ManageCanDriver_reset;
    manageCanDriver.pFunctions.start = ManageCanDriver_start;

    ASSERT_ID( eGrPS_ManageCanDrv, ePS_ErrorParameters, lengthPacket < SIZE_MAX_MESSAGE );

    manageCanDriver.lengthPacket = lengthPacket;
    manageCanDriver.numberPacketOut = 0;
    manageCanDriver.numberPacketIn = 0xAA;

    manageCanDriver.transmiteState = eTransmiteState_Idle;
    manageCanDriver.isInReady = false;
    memset( manageCanDriver.aIn, 0, SIZE_MAX_MESSAGE );

    manageCanDriver.pDrv = Ecan_ctor( numCan, txSid, rxSid, mode, manageCanDriver.lengthPacket + 1 );

    return( const ArrayIoDriver * )&manageCanDriver;
}

//*****************************************************************************
// Чтение данных из периферийного устройства
uint16_t ManageCanDriver_get( const void *self, uint8_t *array, uint16_t size )
{
    ManageCanDriver *me = ( ManageCanDriver * )( self );

    ASSERT_ID( eGrPS_ManageCanDrv, ePS_ErrorParameters, size == ( me->lengthPacket ) );
    memcpy( array, me->aIn, size );
    me->isInReady = false;

    return size;
}

//*****************************************************************************
// Готовность устройства к чтению принятых данных
bool ManageCanDriver_isInReady( const void *self )
{
    ManageCanDriver *me = ( ManageCanDriver * )self;
    return me->isInReady;
}

//*****************************************************************************
// Передача данных драйверу для вывода
void ManageCanDriver_set( const void *self, uint8_t *array, uint16_t size )
{
    ManageCanDriver *me = ( ManageCanDriver * )self;
    ASSERT_ID( eGrPS_ManageCanDrv, ePS_ErrorParameters, size == ( me->lengthPacket ) );

    memcpy( me->aOut, array, size );
    me->transmiteState = eTransmiteState_Start;
}

//*****************************************************************************
// Готовность устройства к выводу новых данных 
bool ManageCanDriver_isOutReady( const void * self )
{
    ManageCanDriver *me = ( ManageCanDriver * )self;
    return(me->transmiteState == eTransmiteState_Idle ) ? true : false;
}

//*****************************************************************************
// Рабочий цикл
void ManageCanDriver_run( const void *self )
{
    ManageCanDriver *me = ( ManageCanDriver * )self;
    
    Ecan_run( me->pDrv );

    // Прием данных
    if( Ecan_isInReady( me->pDrv ) )
    {
        Ecan_get( me->pDrv, me->aIn, me->lengthPacket + 1 );

        if( me->numberPacketIn != me->aIn[me->lengthPacket] )
        {
            me->numberPacketIn = me->aIn[me->lengthPacket];
            me->isInReady = true;
        }
    }

    // Передача данных
    switch( me->transmiteState )
    {
        case eTransmiteState_Idle:

            break;
        case eTransmiteState_Start:
        {
            uint16_t periodTime = Main_getPeriodCycle();
            uint16_t cycleTime  = Main_getCurrentTimeCycle();
            uint16_t remainTime = ( periodTime <= cycleTime ) ? 0 : ( periodTime - cycleTime ); 
            if( remainTime > TIME_TRANSMITE_2_PACKET )
            {
                me->aOut[me->lengthPacket] = me->numberPacketOut++;
                Ecan_set( me->pDrv, me->aOut, me->lengthPacket + 1 );
                Ecan_startTransmite( me->pDrv );

                me->cWaiteTx = 0;
                me->cRepeatTx = 0;
                me->transmiteState = eTransmiteState_InProgress;
            }
        }
            break;
        case eTransmiteState_InProgress:
            if( ManageCanDriver_isOut( me ) )
            {
                uint16_t error = Ecan_getError( me->pDrv );
                if( error == ePS_NoError )
                {
                    me->transmiteState = eTransmiteState_Idle;
                }
                else
                {
                    if( ++me->cRepeatTx >= REPEAT_TRANSMITE )
                    {
                        ERROR_ID( eGrPS_ManageCanDrv, error );
                    }
                    else
                    {
                        Ecan_startTransmite( me->pDrv );
                        me->cWaiteTx = 0;
                    }
                }
            }
            else
            {
                if( ++me->cWaiteTx >= WAITE_TRANSMITE )
                {
                    if( ++me->cRepeatTx >= REPEAT_TRANSMITE )
                    {
                        ERROR_ID( eGrPS_ManageCanDrv, ePS_NoAck );
                    }
                    else
                    {
                        Ecan_AbortTransmite( me->pDrv );
                        Ecan_startTransmite( me->pDrv );
                        me->cWaiteTx = 0;
                    }
                }
            }
            break;
    }
}

//*****************************************************************************
// Реализация локальных функций
//*****************************************************************************

//*****************************************************************************
// Запрос завершения передачи (готовности к передаче)
bool ManageCanDriver_isOut( const void *self )
{
    ManageCanDriver *me = ( ManageCanDriver * )self;
    return Ecan_isOutReady( me->pDrv );
}

//*****************************************************************************
/**
* История изменений:
* 
* Версия 1.0.1
* Дата   25-10-2018
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Базовая версия.
* 
* Версия 1.0.2
* Дата   25-06-2019
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Увеличено значение WAITE_TRANSMITE в связи с тем, что увеличлась
*    частота вызова функции run. 
* 
* Версия 1.0.3
* Дата   23-08-2019
* Автор  Третьяков В.Ж.
*
* Изменения:
*    Изменен интерфейс конструктора. 
*
* Версия 1.0.4
* Дата   06-07-2020
* Автор  Третьяков В.Ж.
*
* Изменения:
*    Увеличено время окна тишины: TIME_TRANSMITE_2_PACKET
* 
* Версия 1.0.5
* Дата   04-12-2020
* Автор  Третьяков В.Ж.
*
* Изменения:
*    Увеличено время окна тишины: TIME_TRANSMITE_2_PACKET
* 
* Версия 1.0.6
* Дата   05-02-2021
* Автор  Третьяков В.Ж.
*
* Изменения:
*    Учет перехода через 0 при определении времени до конца мс цикла.
*/
