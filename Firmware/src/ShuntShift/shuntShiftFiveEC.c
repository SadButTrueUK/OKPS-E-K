/**
* \file    shuntShiftFiveEC.c
* \brief   \copybrief shuntShiftFiveEC.h
*
* \version 1.0.1
* \date    02-02-2018
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "shuntShiftFiveEC.h"
#include "shuntShift.h"
#include "shuntShiftMotor.h"
#include "ProtectionState_codes.h"
#include "asserts.h"
#include "InterChannel.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************
#define POSITION_DETECT_TO    300        ///< Время определения положения после запроса.
#define SET_SHIFT_POSITION_TO 200        ///< Время установки позиции перевода.
#define MOTOR_SHUTDOWN_TO     300        ///< Время отключения двигателя.

//*****************************************************************************
// Определение типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Идентификаторы состояния функции #FiveWireEcShuntShift_run.
///
typedef enum
{
    eStop        = 0,        ///< состояние "Стоп"
    eInitPosDet,             ///< состояние ожидания определения положения
    ePrepForRun,             ///< состояние ожидания установки положения перевода
    eRunning,                ///< перевод, ожидание отключения
    eShutdown,               ///< ожидание остановки двигателя
    eFinalPosDet             ///< ожидание определения положения
} ShiftFiveEcStates;

//*****************************************************************************
/// \brief Структура состояния работы модуля перевода стрелки.
///
typedef struct
{
    uint8_t ctrl   :1;        ///< Включение модуля.
    uint8_t st     :1;        ///< Работа на стенде.
    uint8_t dir    :1;        ///< Устанавливаемое положение.
    uint8_t unused :5;        ///< Неиспользуемые биты.
} ShuntShiftFiveEc_flags;

//*****************************************************************************
/// \brief Объединение флагов состояния с \a uint8_t.
///
typedef union
{
    ShuntShiftFiveEc_flags str;         ///< Флаги состояния \a uint8_t. 
} uShuntShiftFiveEc_flags;

//*****************************************************************************
// Определение локальных типизированных констант
//*****************************************************************************

//*****************************************************************************
/// \brief Переменная структуры интерфейса.
///
const ArrayShuntShift fiveWireEcShuntShift =
{
    FiveWireEcShuntShift_ctor,
    FiveWireEcShuntShift_run,
    FiveWireEcShuntShift_turnOn,
    FiveWireEcShuntShift_turnOff,
    FiveWireEcShuntShift_setDetPos,
    FiveWireEcShuntShift_setShiftPos,
    FiveWireEcShuntShift_isEnable,
    FiveWireEcShuntShift_getReqPosDet,
    FiveWireEcShuntShift_getReqShiftDir,
    FiveWireEcShuntShift_getTime,
    FiveWireEcShuntShift_getDgn,
    FiveWireEcShuntShift_setCntPhaseShift,
    FiveWireEcShuntShift_getCntPhaseShift,
    FiveWireEcShuntShift_isWorkMode
};

//*****************************************************************************
// Определение локальных переменных
//*****************************************************************************
static          uShuntShiftFiveEc_flags flags;              ///< Флаги состояния.
static          ShiftFiveEcStates       stateCnt;           ///< Счетчик состояния.
static          PositionDet_State       reqPos;             ///< Требуемая позиция для определения положения.
static          PositionDet_State       setPos;             ///< Установленная позиция для определения положения.
static          ShiftDirection          reqDir;             ///< Требуемая позиция для перевода.
static          ShiftDirection          setDir;             ///< Установленная позиция для перевода.
static          PositionDet_State       pos;                ///< Фактическое положение.
static uint16_t                         setTime;            ///< Устанавливаемое время.
static uint16_t                         timeoutCnt;         ///< Счетчик таймаутов.
static          ShiftDgn                dgn;                ///< Состояние последнего перевода. 
static uint16_t                         shiftTime;          ///< Время последнего перевода.
static uint8_t                          repeatShift;        ///< Режим повторного перевода.
static uint8_t                          cntPhaseShift;      ///< Значение счётчика фазы стрелочного перевода.

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация
void FiveWireEcShuntShift_ctor( void )
{
    flags.str.ctrl = 0; //Выключен   
    reqDir = eShiftStop;
    setDir = eShiftStop;
    reqPos = eDetNon;
    setPos = eDetNon;
    pos = eDetNon;
    dgn = eShiftNormal;
    shiftTime = 0;
    stateCnt = eStop;
}

//*****************************************************************************
// Управление работой
void FiveWireEcShuntShift_run( void )
{
    if( timeoutCnt ) timeoutCnt--; //Декремент счетчика таймаутов

    switch( stateCnt )
    {
        case eStop: //Выключен
            if( flags.str.ctrl )
            {
                //Запрос на установку положения
                reqPos = flags.str.dir ? eDetPlus : eDetMinus; // Определение необходимого положения
                setPos = eDetNon;
                pos = eDetNon;
                reqDir = eShiftStop;
                setDir = eShiftStop;
                timeoutCnt = POSITION_DETECT_TO;
                stateCnt = eInitPosDet;
            }
            break;

        case eInitPosDet: // Ожидание определения положения
            if( timeoutCnt == 0 && setPos == reqPos )
            { // Таймаут истек
                // Проверка установленной позиции для определ положения
                if( ( flags.str.dir && pos == eDetPlus ) || // Фактическое положение соответствует 
                    ( flags.str.dir == 0 && pos == eDetMinus ) )
                { // устанавливаемому положению
                    //Перевод не начинать
                    flags.str.ctrl = 0; // Выключение
                    stateCnt = eStop;
                }
                else
                { // Установить положение для перевода 
                    reqPos = eDetNon;
                    reqDir = flags.str.dir ? eShiftPlus : eShiftMinus;
                    timeoutCnt = SET_SHIFT_POSITION_TO;
                    stateCnt = ePrepForRun;
                }
            }
            else if( timeoutCnt == 0 && setPos != reqPos )
            {
                ERROR_ID( eGrPS_ShuntShift, ePS_ShuntShiftFiveEcMismatchSetPos );
            }
            break;

        case ePrepForRun: //Ожидание установки положения перевода  
            if( timeoutCnt == 0 && setDir == reqDir )
            { //Положение для перевода установлено
                //Включить перевод
                ShuntShiftMotor_turnOn( flags.str.st, flags.str.dir );
                dgn = eShiftActive; //Активное состояние
                timeoutCnt = setTime; //Установить заданное время перевода
                stateCnt = eRunning;
            }
            else if( timeoutCnt == 0 && setDir != reqDir )
            { // Положение не установлено
                ERROR_ID( eGrPS_ShuntShift, ePS_ShuntShiftFiveEcMismatchSetDir );
            }
            break;

        case eRunning: //Перевод, ожидание отключения
            if( timeoutCnt == 0 ||
                InterChannel_getData( eICId_ShuntShiftMotor_failure ) != eNorm || //Неисправность
                flags.str.ctrl == 0 )
            { //Остановить перевод
                shiftTime = ShuntShiftMotor_getWorkingTime( ); // Считать время 
                ShuntShiftMotor_turnOff( ); //Выключить двигатель 
                timeoutCnt = MOTOR_SHUTDOWN_TO;
                stateCnt = eShutdown;
            }
            break;

        case eShutdown: //Ожидание останова двигателя
            if( timeoutCnt == 0 && !ShuntShiftMotor_isOn( ) )
            { // Двигатель отключен
                reqDir = eShiftStop;
                setDir = eShiftStop;
                reqPos = flags.str.dir ? eDetPlus : eDetMinus; // Определение необходимого положения
                setPos = eDetNon;
                pos = eDetNon;
                timeoutCnt = POSITION_DETECT_TO;
                stateCnt = eFinalPosDet;
            }
            else if( timeoutCnt == 0 && ShuntShiftMotor_isOn( ) )
            { // Двигатель не остановлен
                ERROR_ID( eGrPS_ShuntShift, ePS_ShuntShiftFiveEcMotorNotStoped );
            }
            break;

        case eFinalPosDet: // Ожидание определения положения
            if( timeoutCnt == 0 && setPos == reqPos )
            { // Таймаут истек
                // Проверка установленной позиции для определ положения
                if( ( flags.str.dir && pos == eDetPlus ) || // Фактическое положение соответствует 
                    ( flags.str.dir == 0 && pos == eDetMinus ) )
                { // устанавливаемому положению
                    dgn = eShiftNormal; // Нормальное состояние - перевод закончен
                }
                else
                { // 
                    dgn = eShiftFail; // Перевод закончился неудачей 
                }
                flags.str.ctrl = 0; //Отключить перевод
                stateCnt = eStop;
            }
            else if( timeoutCnt == 0 && setPos != reqPos )
            {
                ERROR_ID( eGrPS_ShuntShift, ePS_ShuntShiftFiveEcMismatchSetPos );
            }
            break;

        default:
            ERROR_ID( eGrPS_ShuntShift, ePS_ShuntShiftFiveEcCntErr );
    }
}

//*****************************************************************************
// Включение
void FiveWireEcShuntShift_turnOn( bool dir, uint16_t time, bool st, uint8_t rs )
{
    if( !FiveWireEcShuntShift_isEnable( ) )
    {
        flags.str.dir = dir;
        flags.str.ctrl = 1;
        flags.str.st = st;
        setTime = time;
        repeatShift = rs;
    }
}

//*****************************************************************************
// Выключение
void FiveWireEcShuntShift_turnOff( void )
{
    flags.str.ctrl = 0;
}

//*****************************************************************************
// Передача положения стрелки
void FiveWireEcShuntShift_setDetPos( PositionDet_State detPos, PositionDet_State position )
{
    setPos = detPos;
    pos = position;
}

//*****************************************************************************
// Передача установленной позиции перевода
void FiveWireEcShuntShift_setShiftPos( ShiftDirection shiftPos )
{
    setDir = shiftPos;
}

//*****************************************************************************
// Проверка состояния включения
bool FiveWireEcShuntShift_isEnable( void )
{
    return stateCnt != eStop;
}

//*****************************************************************************
// Запрос на установку позиции контроля
PositionDet_State FiveWireEcShuntShift_getReqPosDet( void )
{
    return reqPos;
}

//*****************************************************************************
// Запрос на установку позиции направления перевода
ShiftDirection FiveWireEcShuntShift_getReqShiftDir( void )
{
    return reqDir;
}

//*****************************************************************************
// Получить время последнего перевода
uint16_t FiveWireEcShuntShift_getTime( void )
{
    return shiftTime;
}

//*****************************************************************************
// Получить состояние перевода
ShiftDgn FiveWireEcShuntShift_getDgn( void )
{
    return dgn;
}

//*****************************************************************************
//Установить значение счётчика фазы стрелочного перевода.
void FiveWireEcShuntShift_setCntPhaseShift( const uint8_t val )
{
    cntPhaseShift = val;
}
//*****************************************************************************
//Получить значение счётчика фазы стрелочного перевода.

const uint8_t FiveWireEcShuntShift_getCntPhaseShift( void )
{
    return cntPhaseShift;
}

const bool FiveWireEcShuntShift_isWorkMode( void )
{
    return false;
}


//*****************************************************************************
/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   02-02-2018
* Автор  Кругликов В.П.
* 
* Изменения:
*    Базовая версия.
*/
