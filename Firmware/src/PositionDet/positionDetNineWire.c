/**
* \file    positionDetNineWire.c
* \brief   \copybrief positionDetNineWire.h
*
* \version 1.0.4
* \date    12-07-2021
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "positionDetNineWire.h"
#include "InterChannel.h"
#include "OverloadDet.h"
#include "ProtectionState_codes.h"
#include "asserts.h"
#include "AnalogMeasurement.h"
#include "main.h"


//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************
#define NUM_NON_CONTACT_SENSORS  2     ///< количество бесконтактных датчиков положения 
#define RMS_HIGH_40V             830   ///< Пороговое значение напряжения с бесконтактного датчика (верхнее значение).
#define RMS_HIGH_40V_HYST        847   ///< Пороговое значение напряжения с бесконтактного датчика (верхнее значение) с учётом гистерезиса.
#define RMS_LOW_12V              260   ///< Пороговое значение напряжения с бесконтактного датчика (нижнее значение).
#define RMS_LOW_12V_HYST         286   ///< Пороговое значение напряжения с бесконтактного датчика (нижнее значение) с учётом гистерезиса.

//*****************************************************************************
//  Объявление типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Структура состояния.
///
typedef struct
{
    uint8_t ctrl   :1;        ///< Включение модуля.
    uint8_t unused :7;        ///< Неиспользуемые биты.
} PosDetNineWire_flags;

//*****************************************************************************
/// \brief Объединение флагов состояния с \a uint8_t.
///
typedef union
{
    PosDetNineWire_flags str;         ///< Флаги состояния.
} uPosDetNineWire_flags;

//*****************************************************************************
/// \brief Структура результатов измерения.
///
typedef struct
{
    uint16_t ur1rms;    ///< Среднеквадратическое значение R1.
    uint16_t ur2rms;    ///< Среднеквадратическое значение R2.
} MeasurResult;

//*****************************************************************************
/// \brief Переисление условных "зон" сигнала с выхода датчика поожения стрелки
///
typedef enum 
{
    eZoneLowSignal,              ///< нижняя зона сигнала с выхода бесконтактного датчика (от 0 до 12В)
    eZoneMediumSignal,           ///< средняя зона сигнала с выхода бесконтактного датчика (от 12 до 39В)
    eZoneHighSignal              ///< верхняя зона сигнала с выхода бесконтактного датчика (более 39В)
} ZonesOfSignalSensor;


//*****************************************************************************
// Объявление локальных типизированных констант
//*****************************************************************************

//*****************************************************************************
/// \brief Переменная структуры интерфейса.
/// 
const ArrayPositionDet nineWirePosDet =
{
    NineWirePosDet_ctor,
    NineWirePosDet_run,
    NineWirePosDet_turnOn,
    NineWirePosDet_isEnable,
    NineWirePosDet_getPosition,
    NineWirePosDet_getRequestPosDet,
    NineWirePosDet_setPosDet
};

//*****************************************************************************
// Объявление локальных переменных
//*****************************************************************************
static uPosDetNineWire_flags flags;               ///< Флаги состояния.
static PositionDet_State     resultPosDet;        ///< Результат определения положения.
static MeasurResult          measur;              ///< Результат измерения.
static ZonesOfSignalSensor stateSensor[NUM_NON_CONTACT_SENSORS];

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Определение положения по результатам измерения.
/// \param data - результаты измерения.
/// \return Положение.
///
static PositionDet_State posDetermin( MeasurResult *data );

//*****************************************************************************
/// \brief Определение в какой из 3 зон (нижняя, средняя, верхняя) находится сигнал с выхода бесконтактного датчика.
/// \param voltageValue - напряжение на выходе датчика.
/// \param index - номер датчика.
/// \return Номер зоны.
///
static ZonesOfSignalSensor detectionZoneOfSensorsSignal( uint16_t voltageValue, uint8_t index );

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация
void NineWirePosDet_ctor( void )
{
    // Включить определение положения 
    flags.str.ctrl = 1;
    for( uint8_t i = 0; i < NUM_NON_CONTACT_SENSORS; i++ )
        stateSensor[NUM_NON_CONTACT_SENSORS] = eZoneLowSignal; 
}

//*****************************************************************************
// Управление работой
//Если генератор готов для работы - берём данные с датчиков положения,
//если нет и есть связь с соседним прибором - берём данные о контроле положения от него,
//в противном случае - потеря контроля
void NineWirePosDet_run( void )
{
    if( flags.str.ctrl )
    { // Включено
        // Запись измеренных значений
        if ( PosDetGenerator_isReadyForUse( ) )
        {
            measur.ur1rms = InterChannel_getData( eICId_U_R1rms );
            measur.ur2rms = InterChannel_getData( eICId_U_R2rms );
            resultPosDet = posDetermin( &measur );
        }
        else if( ActivityManager_getNeighborConnection( activityManager ) ) 
        {
            resultPosDet = ActivityManager_getNeighborPositionShunt( activityManager );
        }
    }
    else 
        resultPosDet = eDetNon;
}

//*****************************************************************************
// Включение/выключение
void NineWirePosDet_turnOn( bool on )
{
    flags.str.ctrl = on;
    if( flags.str.ctrl )
    { //Включить
        measur.ur1rms = 0;
        measur.ur2rms = 0;
    }
    else
    { //Выключить
        resultPosDet = eDetNon;
    }
}

//*****************************************************************************
// Проверка состояния модуля
bool NineWirePosDet_isEnable( void )
{
    return flags.str.ctrl;
}

//*****************************************************************************
// Получает установленное положение
PositionDet_State NineWirePosDet_getPosition( void )
{
    return resultPosDet;
}

//*****************************************************************************
// Получает запрос на установку позиции контроля положения
PositionDet_State NineWirePosDet_getRequestPosDet( void )
{
    return eDetNon;
}

//*****************************************************************************
// Передает установленную позицию контроля
void NineWirePosDet_setPosDet( PositionDet_State pos )
{

}

//*****************************************************************************
// Реализация локальных функций
//*****************************************************************************

//Определение в какой из 3 зон (нижняя, средняя, верхняя) находится сигнал с выхода бесконтактного датчика
static ZonesOfSignalSensor detectionZoneOfSensorsSignal( uint16_t voltageValue, uint8_t index )
{
    if( stateSensor[index] == eZoneLowSignal && voltageValue > RMS_LOW_12V_HYST )
        stateSensor[index] = eZoneMediumSignal;
    if( stateSensor[index] == eZoneMediumSignal && voltageValue > RMS_HIGH_40V_HYST )
        stateSensor[index] = eZoneHighSignal;
    if( stateSensor[index] == eZoneHighSignal && voltageValue < RMS_HIGH_40V )
        stateSensor[index] = eZoneMediumSignal;
    if( stateSensor[index] == eZoneMediumSignal && voltageValue < RMS_LOW_12V )
        stateSensor[index] = eZoneLowSignal;
    return stateSensor[index];
}

//*****************************************************************************
// Определение положения стрелки по результатам измерения
static PositionDet_State posDetermin( MeasurResult *data )
{
    if( detectionZoneOfSensorsSignal( data->ur1rms, 0 ) == eZoneHighSignal 
        && detectionZoneOfSensorsSignal( data->ur2rms, 1 ) == eZoneLowSignal )
        return eDetMinus;               //"Положение минус"
    if( detectionZoneOfSensorsSignal( data->ur2rms, 1 ) == eZoneHighSignal 
        && detectionZoneOfSensorsSignal( data->ur1rms, 0 ) == eZoneLowSignal )
        return eDetPlus;                 //"Положение плюс"
     if( detectionZoneOfSensorsSignal( data->ur2rms, 1 ) == eZoneHighSignal 
        && detectionZoneOfSensorsSignal( data->ur1rms, 0 ) == eZoneHighSignal )   //Неопределённое состояние (ложный контроль)   
        return eDetUndefinedState;
    return eDetNon;                     //Положение "Потеря контроля"
}

//*****************************************************************************
/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   28-12-2017
* Автор  Кругликов В.П.
* 
* Изменения:
*     Базовая версия.
*
* Версия 1.0.2
* Дата   23-04-2020
* Автор  Кругликов В.П.
*
* Изменения:
*     Положение стрелки определяется только на активном приборе, иначе - потеря контроля
*
* Версия 1.0.3
* Дата   14-04-2021
* Автор  Кругликов В.П.
*
* Изменения:
*     Добавлено определение положения стрелки на основе данных от второго прибора.
*     Для корректной обработки при смене активности
*
* Версия 1.0.4
* Дата 12-07-2021 
* Автор  Кругликов В.П.
*
* Изменения:
*     Добавлен гистерезис сигнала с выходов  датчиков определения положения стрелки. 
*/