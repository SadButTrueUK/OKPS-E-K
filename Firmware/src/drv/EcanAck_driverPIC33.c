/**
* \file    EcanAck_driverPIC33.c
* \brief   \copybrief EcanAck_driverPIC33.h
* 
* \version 1.0.3
* \date    08-07-2019
* \author  Годунок А.Н.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "typeMK.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"
#include "Ecan_driverPIC33.h"
#include "EcanAck_driverPIC33.h"
#include "EcanAck_Pins.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
/// \brief Размер пакета обмена.
/// 
#define SIZE_MESSAGE     6

//*****************************************************************************
/// \brief Время ожидания завершения передачи, в единицах 62,5 мкс.
/// 
#define WAITE_TRANSMITE  8

//*****************************************************************************
/// \brief Количество повторов передачи при отсутствии подтверждения.
/// 
#define REPEAT_TRANSMITE 6

//*****************************************************************************
/// \brief Время игнорирования повторяющегося пакета, в единицах 62,5 мкс.
/// 
#define TIME_IGNORE_IN   ( WAITE_TRANSMITE * ( REPEAT_TRANSMITE + 1 ) )

//*****************************************************************************
// Определение типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Состояние передачи.
///
typedef enum
{
    eEcanAckTx_Idle        = 0,          ///< передатчик свободен и готов к работе
    eEcanAckTx_InProgress                ///< процесс передачи данных
} EcanTxState;

//*****************************************************************************
/// \brief Состояние приемника.
///
typedef enum
{
    eEcanAckRx_Idle        = 0,        ///< приемник ожидает данные
    eEcanAckRx_InProgress              ///< приемник принял данные и формирует подтверждение приема
} EcanRxState;

//*****************************************************************************
/// \brief Данные драйвера.
///
typedef struct
{
    ArrayIoDriver pFunctions;            ///< Адреса интерфейсных функций.
    EcanRxState   receiveState;          ///< Состояние приемника.
    EcanTxState   transmiteState;        ///< Состояние передатчика.
    uint16_t      ackState;              ///< Состояние подтверждения приема по дискретной линии.
    uint8_t       aIn[SIZE_MESSAGE];     ///< Принятая информация.
    uint8_t       aTempIn[SIZE_MESSAGE]; ///< Промежуточный буфер для хранения последних принятых данных.
    bool          isInReady;             ///< Признак наличия принятой информации.
    uint16_t      cTimeIn;               ///< Счетчик времени от последнего приема.
    uint16_t      cWaiteTx;              ///< Счетчик циклов ожидания подтверждения приема соседним МК.
    uint16_t      cRepeatTx;             ///< Счетчик повторов передачи в соседний МК.
    const void    *pDrv;                 ///< Указатель на драйвер \a CAN.
} EcanAck;

//*****************************************************************************
// Определение локальных переменных
//*****************************************************************************

//*****************************************************************************
/// \brief Данные драйверов \a ECAN.
///
static EcanAck ecanAck =
{
    EcanAck_staticFunc() // инициализация таблицы вызовов для модуля CAN
};

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Подтверждение передачи от соседнего МК.
/// \details Функция опрашивает дискретный порт ввода/вывода для чтения 
/// подтверждения приема принимающим узлом.
/// \param self - указатель на данные объекта, который вернул конструктор.
/// \retval true  - принимающий узел принял сообщение;
/// \retval false - принимающий узел не принял сообщение.
///
static bool EcanAck_isBinAck( const void *self );

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//***************************************************************************** 
// Конструктор  
const ArrayIoDriver * EcanAck_ctor( const void *pDrv )
{
    uint8_t i;
    ecanAck.pDrv = pDrv;
    ecanAck.receiveState = eEcanAckRx_Idle;
    ecanAck.transmiteState = eEcanAckTx_Idle;
    ecanAck.ackState = 0;
    ecanAck.isInReady = false;
    ecanAck.cTimeIn = TIME_IGNORE_IN;
    for( i = 0; i < SIZE_MESSAGE; i++ )
    {
        ecanAck.aIn[i] = 0;
    }
    ECAN_ACK_INIC_PORTS;
    return &ecanAck.pFunctions;
}

//*****************************************************************************
// Чтение данных из периферийного устройства
int EcanAck_get( const void *self, uint8_t *array, int size )
{
    ASSERT_ID( eGrPS_CanAckDrv, ePS_CanAckDrvSetSize, ( size == SIZE_MESSAGE ) );

    uint8_t i;
    EcanAck *me = ( EcanAck * )( self );

    for( i = 0; i < SIZE_MESSAGE; i++ )
    {
        array[i] = me->aIn[i];
    }
    me->isInReady = false;

    return SIZE_MESSAGE;
}

//*****************************************************************************
// Готовность устройства к чтению принятых данных
bool EcanAck_isInReady( const void *self )
{
    EcanAck *me = ( EcanAck * )self;
    return me->isInReady;
}

//***************************************************************************** 
// Передача данных драйверу для вывода
void EcanAck_set( const void *self, uint8_t *array, int size )
{
    ASSERT_ID( eGrPS_CanAckDrv, ePS_CanAckDrvSetSize, ( size == SIZE_MESSAGE ) );
    EcanAck *me = ( EcanAck * )self;
    Ecan_AbortTransmite( me->pDrv );
    Ecan_set( me->pDrv, array, size );
    Ecan_startTransmite( me->pDrv );
    me->cWaiteTx = 0;
    me->cRepeatTx = 0;
    me->transmiteState = eEcanAckTx_InProgress;
}

//*****************************************************************************
// Готовность устройства к выводу новых данных 
bool EcanAck_isOutReady( const void *self )
{
    EcanAck *me = ( EcanAck * )self;
    return( me->transmiteState == eEcanAckTx_Idle ) ? true : false;
}

//***************************************************************************** 
// Рабочий цикл
void EcanAck_run( const void *self )
{

    EcanAck *me = ( EcanAck * )self;
    Ecan_run( me->pDrv );

    // Поскольку передатчик подтверждение прочтет только на следующем 
    // прерывании, готовность формируем на следующем вызове после приема
    if( ecanAck.receiveState == eEcanAckRx_InProgress )
    {
        ecanAck.receiveState = eEcanAckRx_Idle;
        me->isInReady = true;
    }

    // Прием данных
    if( Ecan_isInReady( me->pDrv ) )
    {
        Ecan_get( me->pDrv, me->aIn, SIZE_MESSAGE );

        // Поскольку передатчик подтверждение прочтет только на следующем 
        // прерывании, сразу после приема готовность не формируем.
        ecanAck.receiveState = eEcanAckRx_InProgress;
        ECAN_ACK_TOGGLE_LEVEL;
    }

    // Передача данных
    switch( me->transmiteState )
    {
        case eEcanAckTx_Idle:
            break;

        case eEcanAckTx_InProgress:
            if( EcanAck_isBinAck( me ) )
            {
                Ecan_AbortTransmite( me->pDrv );
                me->transmiteState = eEcanAckTx_Idle;
            }
            else
            {
                if( ++me->cWaiteTx >= WAITE_TRANSMITE )
                {
                    if( ++me->cRepeatTx >= REPEAT_TRANSMITE )
                    {
                        ERROR_ID( eGrPS_CanAckDrv, ePS_CanAckDrvNotAck );
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
// Подтверждение передачи от соседнего МК.
static bool EcanAck_isBinAck( const void *self )
{
    uint8_t i, count = 0;
    bool ackState;

    EcanAck *me = ( EcanAck * )self;

    for( i = 0; i < 5; i++ )
    {
        if( ECAN_ACK_CHECK_HIGH )
        {
            count++;
        }
    }
    ackState = ( count >= 3 );
    if( me->ackState != ackState )
    {
        me->ackState = ackState;
        return true;
    }
    return false;
}

//***************************************************************************** 
/**
* История изменений:
* 
* Версия 1.0.1
* Дата   26-05-2017
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Базовая версия.
* 
* 
* Версия  1.0.2
* Дата    07-08-2018
* Автор   Годунок А.Н.
* Изменения:
*     - Убран дополнительный буфер и блокировка приема одинаковых данных.
*     - Поскольку передатчик подтверждение прочтет только на следующем 
*     прерывании, готовность формируем на следующем вызове, после приема.
* 
* Версия  1.0.3
* Дата    08-07-2019
* Автор   Годунок А.Н.
* Изменения:
*     1) Изменен тип данных возвращаемых функцией dsPIC30CanAck_ctor с 
* const void * на const ArrayIoDriver *. 
*     2) В начале структуры DsPIC30CanAck добавлено поле pFunctions, которое  
* содержит адреса интерфейсных функций. Добавлена инициализация этого поля при 
* объявлении dsPIC30CanAck и функция dsPIC30CanAck_ctor возвращает указатель  
* на это поле, а поскольку оно расположено в начале этот же адрес соответствует 
* адресу всей структуры.
*/
