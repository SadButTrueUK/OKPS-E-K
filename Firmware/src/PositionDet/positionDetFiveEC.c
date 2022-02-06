/**
* \file    positionDetFiveEC.c
* \brief   \copybrief positionDetFiveEC.h
*
* \version 1.0.1
* \date    28-12-2017
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "positionDetFiveEC.h"
#include "positionDet.h"
#include "InterChannel.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
#define MEASUR_TO                100          ///< Время измерения сигналов, мс.
#define FIRST_CHANGE_POS_DET_TO  1000         ///< Время смены положения измерения (1-ое после включения), мс.
#define CHANGE_POS_DET_TO        10000        ///< Время смены положения измерения, мс.

//*****************************************************************************
/// \brief Нижний порог значения RMS сигнала обратной связи с датчика контроля положения.
///
#define RMS_LOW_0_65V            163U

//*****************************************************************************
/// \brief Верхний порог значения RMS сигнала обратной связи с датчика контроля положения.
///
#define RMS_HIGH_1_5V            375U

//*****************************************************************************
/// \brief Нижний порог постоянной составляющей RMS сигнала обратной связи при положении+.
///
#define ZERO_PLUS_LOW_2_9V       725U

//*****************************************************************************
/// \brief Верхний порог постоянной составляющей RMS сигнала обратной связи при положении+.
///
#define ZERO_PLUS_HIGH_3_5V      875U

//*****************************************************************************
/// \brief Нижний порог постоянной составляющей RMS сигнала обратной связи при положении-.
///
#define ZERO_MINUS_LOW_1_0V      250U

//*****************************************************************************
/// \brief Верхний порог постоянной составляющей RMS сигнала обратной связи при положении-.
///
#define ZERO_MINUS_HIGH_1_7V     425U

//*****************************************************************************
#define VALUES_R1R2_MISMATCH_0_1 25U          ///< Разница между составляющими RMS сигнала.

//*****************************************************************************
// Объявление типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Структура состояния.
///
typedef struct
{
    uint8_t ctrl   :1;        ///< Включение модуля.
    uint8_t unused :7;        ///< Неиспользуемые биты.
} PosDetFiveEC_flags;

//*****************************************************************************
/// \brief Объединение флагов состояния с \a uint8_t.
///
typedef union
{
    PosDetFiveEC_flags str;         ///< Флаги состояния.
} uPosDetFiveEC_flags;

//*****************************************************************************
/// \brief Структура результатов измерения.
///
typedef struct
{
    uint16_t ur1zero;        ///< Напряжение смещения R1.
    uint16_t ur1rms;         ///< Среднеквадратическое значение R1.
    uint16_t ur2zero;        ///< Напряжение смещения R2.
    uint16_t ur2rms;         ///< Среднеквадратическое значение R2.
} MeasurResult;

//*****************************************************************************
// Объявление локальных типизированных констант
//*****************************************************************************

//*****************************************************************************
/// \brief Переменная структуры интерфейса.
///
const ArrayPositionDet fiveWireECPosDet =
{
    FiveWireECPosDet_ctor,
    FiveWireECPosDet_run,
    FiveWireECPosDet_turnOn,
    FiveWireECPosDet_isEnable,
    FiveWireECPosDet_getPosition,
    FiveWireECPosDet_getRequestPosDet,
    FiveWireECPosDet_setPosDet
};

//*****************************************************************************
// Объявление локальных переменных
//*****************************************************************************
static          uPosDetFiveEC_flags flags;                ///< Флаги состояния.
static          PositionDet_State   requestPosDet;        ///< Текущая позиция определения положения.
static          PositionDet_State   resultPosDet;         ///< Результат определения положения.
static          PositionDet_State   lastPosDet;           ///< Последний результат определения положения.
static          PositionDet_State   ctrlPosDet;           ///< Установленная позиция контроля.
static          MeasurResult        measur;               ///< Результат измерения.
static uint16_t measurToCnt;                              ///< Счетчик времени измерения.
static uint16_t changePosDetCnt;                          ///< Счетчик изменения положения измерения.

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Определение положения по результатам измерения.
/// \param data - указатель на результаты измерения.
/// \return Положение.
///
static PositionDet_State posDetermin( MeasurResult *data );

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация
void FiveWireECPosDet_ctor( void )
{
    requestPosDet = eDetPlus;
    // Выключить определения положения 
    flags.str.ctrl = 0;
    lastPosDet = eDetPlus; // При включении запросить"Положение плюс"
    resultPosDet = eDetNon;
    requestPosDet = eDetNon;
    ctrlPosDet = eDetNon;
}

//*****************************************************************************
// Управление работой
void FiveWireECPosDet_run( void )
{
    if( flags.str.ctrl )
    { // Включено
        if( measurToCnt ) measurToCnt--; // Декремент тайм-аута измерения
        if( changePosDetCnt ) changePosDetCnt--; // Декремент тайм-аута смены положения
        // Проверка тайм-аута измерений
        if( measurToCnt == 0 )
        { // Запись измеренных значений
            measur.ur1zero = InterChannel_getData( eICId_U_R1bias );
            measur.ur1rms = InterChannel_getData( eICId_U_R1rms );
            measur.ur2zero = InterChannel_getData( eICId_U_R2bias );
            measur.ur2rms = InterChannel_getData( eICId_U_R2rms );
            resultPosDet = posDetermin( &measur );
            if( resultPosDet != eDetNon )
            { // Положение определено 
                requestPosDet = ctrlPosDet;
                changePosDetCnt = CHANGE_POS_DET_TO; // Обновить тайм-аут изменения
            }
            else if( changePosDetCnt == 0 )
            { // Положение не определено и пришло время сменить положение измерения
                requestPosDet = requestPosDet == eDetPlus ? eDetMinus : eDetPlus;
                ctrlPosDet = requestPosDet;
                changePosDetCnt = CHANGE_POS_DET_TO; // Взвести тайм-аут следующего изменения
            }
            measurToCnt = MEASUR_TO; // Взвести тайм-аут следующего измерения
        }
    }
}

//*****************************************************************************
// Включение/выключение определения положения
void FiveWireECPosDet_turnOn( bool on )
{
    flags.str.ctrl = on;
    if( flags.str.ctrl )
    { // Включить
        requestPosDet = lastPosDet;
        measur.ur1zero = 0;
        measur.ur1rms = 0;
        measur.ur2zero = 0;
        measur.ur2rms = 0;
        measurToCnt = MEASUR_TO;
        changePosDetCnt = FIRST_CHANGE_POS_DET_TO;
    }
    else
    { // Выключить
        lastPosDet = requestPosDet;
        resultPosDet = eDetNon;
        requestPosDet = eDetNon;
    }
}

//*****************************************************************************
// Проверка состояния модуля
bool FiveWireECPosDet_isEnable( void )
{
    return flags.str.ctrl;
}

//*****************************************************************************
// Получает установленное положение
PositionDet_State FiveWireECPosDet_getPosition( void )
{
    return resultPosDet;
}

//*****************************************************************************
// Получает запрос на установку позиции контроля положения
PositionDet_State FiveWireECPosDet_getRequestPosDet( void )
{
    return requestPosDet;
}

//*****************************************************************************
// Передает установленную позицию контроля
void FiveWireECPosDet_setPosDet( PositionDet_State pos )
{
    ctrlPosDet = pos;
}

//*****************************************************************************
// Реализация локальных функций
//*****************************************************************************

//*****************************************************************************
// Определение положения по результатам измерения
PositionDet_State posDetermin( MeasurResult *data )
{
    uint16_t urZeroDiff, urRmsDiff; // Разница значений zero и rms
    uint16_t urZeroAver, urRmsAver; // Средние значения zero и rms

    urZeroDiff = data->ur1zero > data->ur2zero ? data->ur1zero - data->ur2zero :
            data->ur2zero - data->ur1zero;
    urRmsDiff = data->ur1rms > data->ur2rms ? data->ur1rms - data->ur2rms : data->ur2rms - data->ur1rms;

    // Проверка соответствия разницы значений zero и rms
    if( urZeroDiff < VALUES_R1R2_MISMATCH_0_1 ||
        urRmsDiff < VALUES_R1R2_MISMATCH_0_1 )
    {
        urZeroAver = ( data->ur1zero + data->ur2zero ) >> 1; // Расчет среднего значения zero
        urRmsAver = ( data->ur1rms + data->ur2rms ) >> 1; // Расчет среднего значения rms
    }
    else return eDetNon; // Положение не определено

    // Проверка соответствия установленным диапазонам
    if( urZeroAver < ZERO_MINUS_LOW_1_0V ||
        urZeroAver > ZERO_PLUS_HIGH_3_5V ||
        urRmsAver > RMS_HIGH_1_5V ||
        urRmsAver < RMS_LOW_0_65V ) return eDetNon; // Положение не определено

    if( urZeroAver > ZERO_PLUS_LOW_2_9V ) return eDetPlus; // Положение плюс
    if( urZeroAver < ZERO_MINUS_HIGH_1_7V ) return eDetMinus; // Положение минус

    return eDetNon; // Положение не определено
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
*    Базовая версия.
*/
