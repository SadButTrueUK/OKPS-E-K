/**
* \file    shuntShiftNineWire.c
* \brief   \copybrief shuntShiftNineWire.h
*
* \version 1.0.4
* \date    23-04-20
* \author  Кругликов В.П.
*/

//****************************************************************************
// Подключаемые файлы
//****************************************************************************
#include "shuntShiftNineWire.h"
#include "shuntShift.h"
#include "shuntShiftMotor.h"
#include "ProtectionState_codes.h"
#include "asserts.h"
#include "InterChannel.h"
#include "IntegrCtrl.h"
#include "IntegrCtrlGen.h"
#include "CheckSupply.h"
#include "Main.h"
#include "ControlSystem.h"
#include "DebugTools.h"
#include "shuntShiftGen.h"
#include "addressVarEeprom.h"
#include "Eeprom.h"
#include <string.h>
#include "AnalogMeasurement.h"


//****************************************************************************
// Локальные константы, определенные через макросы
//****************************************************************************
#define INIT_TIMEOUT                500        ///< Начальный таймаут для корректного определения положения стрелки.
#define TIMEOUT_WORK_FRICTION       3000       ///< Таймаут для работы на фрикцию (3000 мс)
#define RS_DOUBLE_CALIBR            3          ///< Включить режим двухстороннего калибровочного перевода (0b11).
#define RS_ON                       2          ///< Включить режим повторного перевода (0b10).
#define RS_SINGLE_CALIBR            1          ///< Включить режим одностороннего калибровочного перевода (0b01).
#define RS_OFF                      0          ///< Включен режим однократного перевода (0b00).
#define SIZE_BUFF_FOR_MIDDLE_CURR   1024
#define SHIFTS_FOR_AVERAGE_CURRENT  10

//****************************************************************************
// Объявление типов данных
//****************************************************************************

//****************************************************************************
/// \brief Структура состояния. 
///
typedef struct 
{
    uint8_t ctrl   :1;        ///< Включение модуля.
    uint8_t st     :1;        ///< Работа на стенде.
    uint8_t dir    :1;        ///< Устанавливаемое положение.
    uint8_t rs     :2;        ///< Повторный перевод.
    uint8_t unused :3;        ///< Неиспользуемые биты.
} ShuntShiftNine_flags;

//****************************************************************************
/// \brief Объединение флагов состояния с \a uint8_t.
///
typedef union 
{
    ShuntShiftNine_flags str;         ///< Флаги состояния.
} uShuntShiftNine_flags;

enum States_t
{
    eInitState,
    eWaiting,
    eFillBuffer
};

//****************************************************************************
// Объявление глобальных типизированных констант
//****************************************************************************

//****************************************************************************
/// \brief Переменная структуры интерфейса.
///
const ArrayShuntShift nineWireShuntShift = 
{
    NineWireShuntShift_ctor,
    NineWireShuntShift_run,
    NineWireShuntShift_turnOn,
    NineWireShuntShift_turnOff,
    NineWireShuntShift_setDetPos,
    NineWireShuntShift_setShiftPos,
    NineWireShuntShift_isEnable,
    NineWireShuntShift_getReqPosDet,
    NineWireShuntShift_getReqShiftDir,
    NineWireShuntShift_getTime,
    NineWireShuntShift_getDgn,
    NineWireEcShuntShift_setCntPhaseShift,
    NineWireEcShuntShift_getCntPhaseShift,
    NineWireEcShuntShift_isWorkMode,
    NineWireShuntShift_setDgn,
    NineWireShuntShift_setTime
};

//****************************************************************************
// Объявление глобальных переменных
//****************************************************************************
static uShuntShiftNine_flags    flags;                                      ///< Флаги состояния.
static ShiftNineWireStates      stateCnt;                                   ///< Счетчик состояния.
static PositionDet_State        pos;                                        ///< Фактическое положение.
static uint32_t                 sumBuff;
static uint16_t                 setTime;                                    ///< Устанавливаемое время.
static uint16_t                 timeoutCnt;                                 ///< Счетчик таймаутов.
static ShiftDgn                 dgn;                                        ///< Состояние последнего перевода.
static uint16_t                 shiftTime;                                  ///< Время последнего перевода.
static uint8_t                  cntTriesRS;                                 ///< Счётчик фаз алгоритма работы двукратного перевода
static bool                     savePosDetInRepeatShuntShift;               ///< Состояние положения стрелки после фазы 2 двукратного перевода
//static uint16_t                 buffForMiddleCurr[SIZE_BUFF_FOR_MIDDLE_CURR];
static enum                     States_t statesPrepareBuff = eInitState;
uint16_t                        middleCurrShuntShift, middleCurrFriction;   ///< Средний ток стрелочного пеервода и средний ток при работе на фрикцию                
uint16_t                        thresholdFriction;
uint16_t                        t_o_friction;

//****************************************************************************
// Объявление локальных функций
//****************************************************************************
static uint16_t calcAverageCurrentShiftOrFriction( void );
static void prepareBuffMiddleCurr( void );

//****************************************************************************
// Реализация интерфейсных функций
//****************************************************************************

//****************************************************************************
// Инициализация
void NineWireShuntShift_ctor( void )
{
    flags.str.ctrl = 0;  //Выключен 
    pos = eDetNon;    
    dgn = eShiftNormal;   
    shiftTime = 0;
    stateCnt = eInit;
    timeoutCnt = INIT_TIMEOUT;
    cntTriesRS = 0;
    middleCurrShuntShift = 0; 
    middleCurrFriction = 0;
    savePosDetInRepeatShuntShift = false;
}

//****************************************************************************
// Управление работой
void NineWireShuntShift_run( void )
{
    static bool temp;
    static bool dirInternal;        //для изменения направления вращения при двукратном переводе
    static bool isPosDetNoControlForcalibrate;  //когда "Потеря контроля" и калибровочный превод, делаем его как обычный маршрутный
    
    if ( ActivityManager_isActive( activityManager ) == false )
    {
        dgn = eShiftNormal;
        return;
    }    
    if( timeoutCnt ) 
        timeoutCnt--;    // Декремент счетчика таймаутов
    
    //Функция наполнения буфера средних токов перевода и фрикции 
    //с учётом "выреза" 900 мс для компенсации переходных процессов при пуске двигателя и при переходе на фрикцию
    //при калибровочном переводе
    prepareBuffMiddleCurr( ); 
    
    switch( stateCnt )
    {
        case eInit:     //инициализация по включению питания
            if( timeoutCnt == 0 )
                stateCnt = eStart;
            break;
        
        case eStart:
            if( flags.str.ctrl )
            {
                dirInternal = flags.str.dir; //направление перевода берём из приказа
                stateCnt = eInitPosDet;
            } 
            break;
                                                                                               
        case eInitPosDet: // Ожидание определения положения
            // Проверка положения
            temp = ( ( dirInternal == 1 ) && ( pos == eDetPlus ) )   
               ||( ( dirInternal == 0 ) && ( pos == eDetMinus ) );
            if( temp )  // Фактическое положение соответствует 
            {           // устанавливаемому положению
                //Перевод не начинать
                flags.str.ctrl = 0; // Выключение
                stateCnt = eStart;     
            }
            else
            {   // Включить перевод
                ShuntShiftMotor_turnOn( flags.str.st, dirInternal );
                if( ( dgn != eShiftActive &&  flags.str.rs != RS_ON ) || 
                    ( flags.str.rs == RS_ON  &&
                    ( dgn != eShiftActive || 
                    ( dgn == eShiftActive && ControlSystem_getStateCheck220V( ) == eOff ) ) ) )
                {
                    dgn = eShiftActive;                         //Активное состояние перевода
                    ControlSystem_setTimeProcessingOrder( 0 );  //В начале перевода устанавливаем в ноль время выполнения приказа
                }
                //При потере контроля в начале перевода стрелки и команде калибровочного перевода
                //выполняем обычный маршрутный перевод
                if( pos == eDetNon && ( flags.str.rs == RS_SINGLE_CALIBR || flags.str.rs == RS_DOUBLE_CALIBR ) )
                    isPosDetNoControlForcalibrate = true;
                timeoutCnt = setTime;
                stateCnt = eRunning;
            }
            break;      
        case eRunning: // Перевод, ожидание отключения
            // Положение соответствует переводимому
            temp = ( ( dirInternal == 1 ) && ( pos == eDetPlus ) )   
                   ||( ( dirInternal == 0 ) && ( pos == eDetMinus ) );
            // Текущее положение соответствует заданному в приказе + задан маршрутный перевод (или однократный или двукратный)
            if( ( temp && ( ControlSystem_getStateOrderFieldRS() == eRouteShuntShift 
                          || ControlSystem_getStateOrderFieldRS() == eRouteRepeatShuntShift) ) ||
                timeoutCnt == 0 ||                           // Время перевода истекло 
                ShuntShiftMotor_getFailure( ) != eNorm ||    // Неисправность
                flags.str.ctrl == 0 ) // Перевод остановлен досрочно внешней командой (либо при пропадании 220В)
            {   //Запрос на синхронизацию признака аварии
                InterChannel_synchronize( eICId_IntegrCtrlScAndBreak, ShuntShiftMotor_getFailure( ) );
                if ( ShuntShiftMotor_getFailure( ) == eShortCircuit )
                     ShuntShiftGen_fastTurnOff();
                ShuntShiftMotor_turnOff( ); // Выключить двигатель 
                statesPrepareBuff = eInitState; //сбрасываем автомат функции наполнения буфера токов
                stateCnt = eShutdown;
            }
            else if ( temp && ( ControlSystem_getStateOrderFieldRS() == eRouteCalibrateShuntShift 
                          || ControlSystem_getStateOrderFieldRS() == eRouteRepeatCalibrateShuntShift ) )
            {
                if( isPosDetNoControlForcalibrate )
                {
                    ShuntShiftMotor_turnOff( );     // Выключить двигатель 
                    statesPrepareBuff = eInitState; //сбрасываем автомат функции наполнения буфера токов
                    stateCnt = eShutdown;
                }
                else
                {
                    //вычислить средний ток стрелочного перевода по всем трём фазам
                    middleCurrShuntShift = calcAverageCurrentShiftOrFriction( );
                                        //установить таймер работы на фрикцию и перейти на состояние автомата eFriction 
                    t_o_friction = TIMEOUT_WORK_FRICTION;
                    statesPrepareBuff = eInitState;    //сбрасываем автомат функции наполнения буфера токов
                    stateCnt = eFriction;
                }
            }
            break;
        case eShutdown:    // Ожидание отключения двигателя
            if( !ShuntShiftMotor_isOn( ) )
            {   // Двигатель отключен
                isPosDetNoControlForcalibrate = false;
                temp = ( ( dirInternal == 1 ) && ( pos == eDetPlus ) )   
                   ||( ( dirInternal == 0 ) && ( pos == eDetMinus ) );
                shiftTime = ShuntShiftMotor_getWorkingTime( );    // Считать время 
                if( temp )  //устанавливаем поле dgn сатуса
                {           //Фактическое положение соответствует устанавливаемому положению
                    if( flags.str.rs == RS_OFF && 
                        ControlSystem_getStateOrderFieldRS() != eRouteCalibrateShuntShift &&
                        ControlSystem_getStateOrderFieldRS() != eRouteRepeatCalibrateShuntShift ) 
                        dgn = eShiftNormal;    
                    else 
                    {
                        if( cntTriesRS == 2 ) //Двукратный перевод осуществлён со второй попытки
                            dgn = eShiftSecondTry; 
                        else if( cntTriesRS == 0 ) //Двукратный перевод осуществлён с первой попытки
                            dgn = eShiftNormal;
                        else if( cntTriesRS == 3 ) //Двукратный перевод завершился неудачей
                            dgn = eShiftFail;
                    }
                }
                else 
                {                        // Фактическое положение не соответствует устанавливаемому
                    if( flags.str.ctrl == 0 && ShuntShiftMotor_getFailure( ) == eNorm )
                    {
                        if( !ActivityManager_isNeighborWorking( activityManager ) || 
                             !ActivityManager_getNeighborConnection( activityManager ) )
                        {
                            if ( ControlSystem_getStatesCheck220VInShuntShift( ) != eShuntShiftIsFailed && CheckSupply_is24vOn( ) )
                            {
                                if( ControlSystem_getStatesCheck220VInShuntShift( ) == eNo220VInShuntShift && timeoutCnt )
                                {    
                                    //dgn = eShiftActive;
                                }
                                else if ( timeoutCnt == 0 )
                                {    
                                    //dgn = eShiftFail;
                                }
                                else
                                {    
                                    dgn = eShiftNormal;
                                }
                            }     
                            else 
                            {    
                                //dgn = eShiftFail;
                            }
                        }
                        else if ( CheckSupply_is220vOn( ) && CheckSupply_is24vOn( ) ) 
                        {
                            dgn = eShiftNormal;
                        }
                    }
                    else if( flags.str.rs == RS_OFF || 
                            ( flags.str.rs == RS_ON && 
                            ( cntTriesRS == 3 || 
                            ( cntTriesRS == 2 && savePosDetInRepeatShuntShift == false ) 
                            || ShuntShiftMotor_getFailure( ) != eNorm ) ) )
                    {    
                        dgn = eShiftFail;    // Перевод закончился неудачей 
                    }
                }
                if( flags.str.rs != RS_ON )  // !!!пока так, дальше условие усложнится
                {
                    flags.str.ctrl = 0;    // Выключение
                    stateCnt = eStart;
                }
                else
                {
                    cntTriesRS++;
                    if( cntTriesRS == 2 )
                        savePosDetInRepeatShuntShift = temp;  //для двукратного перевода сохраняем после второй фазы перевода значение положения стрелки
                    if ( ( cntTriesRS == 4 ) ||
                        ( ( temp && ( cntTriesRS != 2 ) ) || 
                          ( savePosDetInRepeatShuntShift == false &&  cntTriesRS == 3 ) ) ||
                        ( flags.str.ctrl == 0 ) || ShuntShiftMotor_getFailure( ) != eNorm )
                    {
                       savePosDetInRepeatShuntShift = false;
                       cntTriesRS = 0; 
                       flags.str.ctrl = 0;
                       stateCnt = eStart;
                    }    
                    else
                    {
                        dirInternal = !dirInternal; //изменить направление вращения для двукратного перевода
                        stateCnt = eInitPosDet;
                        setTime = ControlSystem_getTimeCurrentOrder( ); 
                    }
                }    
            }
            break;
        case eFriction:         //работа на фрикцию при калибровочном переводе
            if( ShuntShiftMotor_getFailure( ) != eNorm ||    // Неисправность
                flags.str.ctrl == 0 )                         //  Останов внешней командой
            {
                ShuntShiftMotor_turnOff( );     // Выключить двигатель 
                if ( flags.str.ctrl )
                    dgn = eShiftNormal; //на фрикции ставим dgn в "перевод окончен" по команде "Стоп двигателя"
                if ( ShuntShiftMotor_getFailure( ) != eNorm ) //признак неудачного перевода по аварии
                    dgn = eShiftFail;
                statesPrepareBuff = eInitState;
                stateCnt = eShutdown;
            }    
            else if( --t_o_friction == 0 )                   //истёк таймаут 3 с работы на фрикцию во время  калибровочного перевода
            {
                //Калибровка порогового значения тока фрикции и увеличенного времени перевода
                DeviceAddress_setCalibrCurrentAndTime( pos == eDetPlus ? Plus : Minus, 
                ( calcAverageCurrentShiftOrFriction( ) + middleCurrShuntShift ) >> 1,
                 __builtin_muluu( ControlSystem_getTimeProcOrder( ), 3 ) >> 1 );    // Устанавливаем новые значения тока и сохраняем его в EEPROM
                ShuntShiftMotor_turnOff( );                 // Выключить двигатель
                Eeprom_write_start(pos == eDetPlus ? ADDRESS_EEPROM_CALIBR_THRESHOLD_P : ADDRESS_EEPROM_CALIBR_THRESHOLD_M, 
                                   DeviceAddress_getCalibrCurrent( pos == eDetPlus ? Plus : Minus ) ); 
                statesPrepareBuff = eInitState;
                stateCnt = eWaitingForWrThresholdToEEPROM;
            }                                            
            break;
            
        case eWaitingForWrThresholdToEEPROM:
            if ( Eeprom_isReady( ) == false )
                break;            
            Eeprom_write_start( pos == eDetPlus ? ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_P : ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_M,
                                DeviceAddress_getIncreasedTime(pos == eDetPlus ? Plus : Minus ));
            stateCnt = eWaitingForWrShiftTimeToEEPROM;
            break;

        case eWaitingForWrShiftTimeToEEPROM:
            if ( Eeprom_isReady( ) == false )
                break;            
            dgn = eShiftNormal;
            stateCnt = eShutdown;
            break;            
            
        default:
            ERROR_ID( eGrPS_ShuntShift, ePS_ShuntShiftNineWireCntErr );
    } 
}

//****************************************************************************
// Включение
void NineWireShuntShift_turnOn( bool dir, uint16_t time, bool st, uint8_t rs )
{
    if( !NineWireShuntShift_isEnable( ) )
    {
        flags.str.dir  = dir;
        flags.str.ctrl = 1;
        flags.str.st   = st;
        setTime        = time;
        flags.str.rs   = rs;
    }
}

//****************************************************************************
// Выключение
void NineWireShuntShift_turnOff( void )
{
    flags.str.ctrl = 0;
}

//****************************************************************************
// Передача положения стрелки
void NineWireShuntShift_setDetPos( PositionDet_State detPos, PositionDet_State position )
{
    pos = position;
}

//****************************************************************************
// Передача установленной позиции перевода
void NineWireShuntShift_setShiftPos( ShiftDirection shiftPos )
{

}

//****************************************************************************
// Проверка состояния стрелочного перевода
bool NineWireShuntShift_isEnable( void )
{
    return stateCnt != eStart && stateCnt != eInit;
}

//****************************************************************************
// Запрос на установку позиции контроля
PositionDet_State NineWireShuntShift_getReqPosDet( void )
{
    return eDetNon;
}

//****************************************************************************
// Запрос на установку позиции направления перевода
ShiftDirection NineWireShuntShift_getReqShiftDir( void )
{
    return eShiftStop;
}

//****************************************************************************
// Получить время последнего перевода
uint16_t NineWireShuntShift_getTime( void )
{
    return shiftTime;
}

//****************************************************************************
// Получить состояние перевода
ShiftDgn NineWireShuntShift_getDgn( void )
{
    return dgn;
}

void NineWireShuntShift_setDgn( ShiftDgn val )
{
    dgn = val;
}

//*****************************************************************************
//Установить значение счётчика фазы стрелочного перевода.
void NineWireEcShuntShift_setCntPhaseShift( const uint8_t val )
{
    cntTriesRS = val;
}
//*****************************************************************************
//Получить значение счётчика фазы стрелочного перевода.

const uint8_t NineWireEcShuntShift_getCntPhaseShift( void )
{
    return cntTriesRS;
}

const bool NineWireEcShuntShift_isWorkMode( void )
{
    return  stateCnt != eInit;
}

void NineWireShuntShift_setTime( uint16_t val )
{
    shiftTime = val;
}

ShiftNineWireStates NineWireShuntShift_getStateCnt( void )
{
    return stateCnt;
}

//Считаем среднее значение буфера средних по 3 фазам токов
static uint16_t calcAverageCurrentShiftOrFriction( void )
{
    return sumBuff >> SHIFTS_FOR_AVERAGE_CURRENT;
}

//Функция работает по следующему алгоритму:
//1. При калибровочном переводе в начале перевода или в начале работы на фрикцию отсчитываем 900мс
//2. Наполняем буфер из средних значений токов 3 фаз в течение 1024мс при переводе и при работе на фрикцию
//3. Автомат состояний данной функции переводим в начальное состояние eOff снаружи из автомата перевода стрелки в функции run 
static void prepareBuffMiddleCurr( void )
{
    static const uint16_t DURATION_CUT_START_SHIFT = 900;
    static uint16_t t_o, cnt;
    
    switch( statesPrepareBuff )
    {
        case eInitState:
            if( ( stateCnt == eRunning || stateCnt == eFriction ) &&
                ( flags.str.rs == RS_DOUBLE_CALIBR || flags.str.rs == RS_SINGLE_CALIBR ) )
            {
                t_o = DURATION_CUT_START_SHIFT;
                statesPrepareBuff = eWaiting;
            }
            break;
        case eWaiting:
            if( --t_o == 0 )
            {
                statesPrepareBuff = eFillBuffer;
                sumBuff = 0;
                cnt = 0;
            }
            break;
        case eFillBuffer:
            if( cnt++ < SIZE_BUFF_FOR_MIDDLE_CURR )
            {
                sumBuff += __builtin_divud( AnalogMeasurement_getData( eAinchIV )->rms +
                AnalogMeasurement_getData( eAinchIU )->rms + AnalogMeasurement_getData( eAinchIW )->rms, 3 );
            }
            break;
    }
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
*     Базовая версия.
* 
* Версия 1.0.2
* Дата   23-10-2019
* Автор  Кругликов В.П.
* 
* Изменения:
* 	Устранены ошибки в алгоритме двукратного перевода стрелки в функции 
* 	NineWireShuntShift_run: 
* 	- некорректная отработка приказа "Стоп"  пока перевод не завершён
* 	- некорректное формирование поля dgn в телеграмме статуса
* 
* Версия 1.0.3
* Дата   12-12-19 
* Автор  Кругликов В.П.
* 
* Изменения:
* 	Удалён таймаут на определение положения в начале перевода стрелки
* 	в функции NineWireShuntShift_run (по причине изменения идеологии
* 	работы модуля генератора контрольного напряжения), за счёт чего
* 	поле dgn в телеграмме статуса формируется быстрее 
* 
* Версия 1.0.4
* Дата   23-04-20
* Автор  Кругликов В.П.
* 
* Изменения:
* 	Добавлено дополнительное условие для формирования поля dgn в значение eShiftNormal -
* 	наличие питания 24 и 220В
*/
