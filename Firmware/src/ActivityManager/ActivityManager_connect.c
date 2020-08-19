/**
* \file    ActivityManager_connect.c
* \brief   \copybrief ActivityManager_connect.h
* 
* \version 1.0.1
* \date    22-05-2017
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Команды препроцессора для предотвращения повторного включения содержимого файла
//*****************************************************************************
#include "ActivityManager_connect.h"
#include "ActivityManager_internal.h"

//*****************************************************************************
// Определение типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Структура состояния прибора для передачи соседнему прибору.
///
typedef struct ExchangeDataStr
{
    uint8_t s1     :1;         ///< Состояние прибора (рабочее, безопасное).
    uint8_t s2     :1;         ///< Активность прибора.
    uint8_t s3     :1;         ///< Наличие связи с соседним прибором.
    uint8_t s4     :1;         ///< Состояние связи с УС по линии 1.
    uint8_t s5     :1;         ///< Состояние связи с УС по линии 2.
    uint8_t s6     :1;         ///< Состояние связи по RS-422 линия 1.
    uint8_t s7     :1;         ///< Состояние связи по RS-422 линия 2.
    uint8_t s8     :1;         ///< Состояние инициализации менеджера активности прибора ( 0 - в процессе, 1 - окончена )
} ExchangeDataStr;

//*****************************************************************************
/// \brief Объединение данных структуры ExchangeDataStr с \a uint8_t.
///
typedef union uExchangeDataStr
{
    ExchangeDataStr str;         ///< Структура состояния прибора для передачи соседнему прибору.
    uint8_t         data;        ///< Состояние прибора для передачи соседнему прибору.
} uExchangeDataStr;

//*****************************************************************************
// Определение локальных переменных
//*****************************************************************************

//*****************************************************************************
/// \brief Состояние своего прибора.
///
static uExchangeDataStr ownState =
{
    .data = 0
};

//*****************************************************************************
/// \brief Состояние соседнего прибора.
///
static uExchangeDataStr neighborState =
{
    .data = 0
};

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Формирование состояния своего прибора.
/// \param activityManager - указатель на структуру, описывающую состояние модуля ActivityManager.h;
/// \param blockExch - указатель на структуру, описывающую состояние модуля BlockExch.h;
/// \param st - указатель на данные состояния прибора для передачи в соседний.
///
static void ownStateForming( ActivityManager_DeviceStr *activityManager, BlockExch_Str *blockExch, 
                             uExchangeDataStr *st );

//*****************************************************************************
/// \brief Получает состояние соседнего прибора.
/// \param blockExch - указатель на структуру, описывающую состояние модуля BlockExch.h;
/// \param st - указатель на принимаемые данные состояния соседнего прибора.
///
static void receiveNeihgborState( BlockExch_Str *blockExch, uExchangeDataStr *st );

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Управление взаимодействием ActivityManager и BlockExch
void ActiveManagerConnect_run( ActivityManager_DeviceStr *activityManager, BlockExch_Str *blockExch )
{
    //Сформировать состояния своего прибора
    ownStateForming( activityManager, blockExch, &ownState );
    //Передать в соседний прибор
    BlockExch_setData( blockExch, &ownState.data );
    //Получить состояние соседнего прибора
    receiveNeihgborState( blockExch, &neighborState );
    //Установить параметры в ActivityManager
    //Установить состояние связи с соседом
    ActivityManager_setNeighborConnection( activityManager, BlockExch_getConnectionState( blockExch ) );
    //Установить сосед принимает данные "от меня"
    ActivityManager_setNeighborHearsMe( activityManager, neighborState.str.s3 );
    //Установить состояние соседа (рабочее / безопасное)
    ActivityManager_setNeighborState( activityManager, neighborState.str.s1 );
    //Установить состояние активности соседа (активный / пассивный)
    ActivityManager_setNeighborActivity( activityManager, neighborState.str.s2 );
    //Установить состояние связи соседа с УС 
    ActivityManager_setNeighborRS422ctrlSysConnect( activityManager, eRs422_line_1, neighborState.str.s4 );
    ActivityManager_setNeighborRS422ctrlSysConnect( activityManager, eRs422_line_2, neighborState.str.s5 );
    //Установить состояние связи соседа по RS-422
    ActivityManager_setNeighborRS422connect( activityManager, eRs422_line_1, neighborState.str.s6 );
    ActivityManager_setNeighborRS422connect( activityManager, eRs422_line_2, neighborState.str.s7 );
    ActivityManager_writeNeighborIsInWork( activityManager, neighborState.str.s8 );
}

//*****************************************************************************
// Реализация локальных функций
//*****************************************************************************

//*****************************************************************************
// Формирование состояния своего прибора
static void ownStateForming( ActivityManager_DeviceStr *activityManager, BlockExch_Str *blockExch, uExchangeDataStr *st )
{
    //Состояние работоспособности
    st->str.s1 = ActivityManager_getState( activityManager );
    //Состояние активности
    st->str.s2 = ActivityManager_isActive( activityManager );
    //Наличие связи с соседом и запрос на получение активности
    st->str.s3 = BlockExch_getConnectionState( blockExch );
    //Состояние связи с УС 
    st->str.s4 = ActivityManager_getRS422ctrlSysConnect( activityManager, eRs422_line_1 );
    st->str.s5 = ActivityManager_getRS422ctrlSysConnect( activityManager, eRs422_line_2 );
    //Состояние связи с УС по RS-422
    st->str.s6 = ActivityManager_getRS422connect( activityManager, eRs422_line_1 );
    st->str.s7 = ActivityManager_getRS422connect( activityManager, eRs422_line_2 );
    st->str.s8 = ActivityManager_getInitDeviceState( activityManager );
}

//*****************************************************************************
// Получает состояние соседнего прибора
static void receiveNeihgborState( BlockExch_Str *blockExch, uExchangeDataStr *st )
{
    if( BlockExch_getData( blockExch, &st->data ) ) 
        return;
    st->data = 0;
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
*/
