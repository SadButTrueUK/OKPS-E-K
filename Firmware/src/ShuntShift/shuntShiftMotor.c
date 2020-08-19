/**
* \file    shuntShiftMotor.c
* \brief   \copybrief shuntShiftMotor.h
*
* \version 1.0.1
* \date    24-01-2018
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "shuntShiftMotor.h"
#include "shuntShiftGen.h"
#include "ProtectionState_codes.h"
#include "asserts.h"
#include "AnalogMeasurement.h"
#include "string.h"
#include "asserts_ex.h"
#include "InterChannel.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
// Номинальное напряжение используемых двигателей, В.
#define  MCT025_127V_VOLTAGE        127U        ///< Напряжение питания двигателя МСТ025-127, В.
#define  MCT025_220V_VOLTAGE        220U        ///< Напряжение питания двигателя МСТ025-220, В.
#define  MCT03_110V_VOLTAGE         110U        ///< Напряжение питания двигателя МСТ03-110, В.
#define  MCT03_190V_VOLTAGE         190U        ///< Напряжение питания двигателя МСТ03-190, В.
#define  MCT06_110V_VOLTAGE         110U        ///< Напряжение питания двигателя МСТ06-110, В.
#define  MCT06_190V_VOLTAGE         190U        ///< Напряжение питания двигателя МСТ06-190, В.

//*****************************************************************************
#define VOLTAGE_TO_LEVEL_RATIO      0.45        ///< Коэффициент преобразования напряжения в % генератора.
#define MOTOR_BREAK_CURRENT         40          ///< Ток отключения в режиме работы на стрелку, ед. АЦП * 2.826 = мА.
#define STAND_BREAK_CURRENT         50          ///< Ток отключения в режиме работы на стенде, ед. АЦП * 2.826 = мА.

//*****************************************************************************
#define ACCEL_STAGE_NUM             7                                     ///< Количество ступеней разгона.
#define SHUTDOWN_TIME               10U                                   ///< Время отключения.
#define SHUTDOWN_STAGE_NUM          10U                                   ///< Количество шагов отключения.
#define SHUTDOWN_STAGE_TIME         SHUTDOWN_TIME / SHUTDOWN_STAGE_NUM    ///< Время одного шага отключения.
#define K_RATIO_CURRENT             29U                                   ///< Коэффициент пересчёта тока в дискреты АЦП.

//*****************************************************************************
#define THR_OVL_SC          40U                                         ///< Значение тока перегрузки в 0.1А.
#define THR_OVL_SC_ADC      ( THR_OVL_SC * 1000 / K_RATIO_CURRENT )     ///< Значение тока перегрузки в дискретах АЦП.

//*****************************************************************************
/// \brief Таймаут, по истечении которого обрабатывается сигнал обратной связи фаз.
///
#define TIMEOUT_MEASURE_U_FB   150

//*****************************************************************************
/// \brief Таймаут, по истечении которого определяется перегрузка по току для отключения двигателя.
///
#define TIMEOUT_OVL_SC  1000

//*****************************************************************************
/// \brief Количество значений фазных токов выше порогового значения тока перегрузки.
///
#define NUM_OVL_SC      800                          

//*****************************************************************************
// Локальные типизированные константы
//***************************************************************************** 
static const uint8_t NUM_OF_PHASES = 3;    ///< Количество фаз сигнала.

//*****************************************************************************
// Определение типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Структура состояния.
///
typedef struct
{
    uint8_t ctrl   :1;        ///< Включение модуля. 
    uint8_t dir    :1;        ///< Направление вращения.
    uint8_t stand  :1;        ///< Режим работы на стенде.
    uint8_t unused :5;        ///< Неиспользуемые биты.
} ShiftMotor_flags;

//*****************************************************************************
/// \brief Структура контроля токовой перегрузки.
///
typedef struct
{
    uint16_t cntOvlCur;            ///< Счетчик количества токовых перегрузок.
    uint16_t timeoutOvlCur;        ///< Таймаут при определении токовой перегрузки.
}strOvlCurrTag;

//*****************************************************************************
/// \brief Идентификаторы значений автомата состояний функции #ShuntShiftMotor_run.
///
typedef enum
{
    eStop          = 0,    ///< выключенное состояние
    eAcceleration,         ///< разгон
    eRunning,              ///< движение
    eShutdown,             ///< отключение
    eFullShutdown          ///< полное отключение (отключение генератора)
} ShiftMotorStates;

//*****************************************************************************
// Определение локальных типизированных констант
//*****************************************************************************

//*****************************************************************************
/// \brief Массив номинальных уровней, % генератора.
///
const uint16_t generLevel[eMotorTypeNum] = 
{
    MCT025_127V_VOLTAGE * VOLTAGE_TO_LEVEL_RATIO,
    MCT025_220V_VOLTAGE * VOLTAGE_TO_LEVEL_RATIO,
    MCT03_110V_VOLTAGE  * VOLTAGE_TO_LEVEL_RATIO,
    MCT03_190V_VOLTAGE  * VOLTAGE_TO_LEVEL_RATIO,
    MCT06_110V_VOLTAGE  * VOLTAGE_TO_LEVEL_RATIO,
    MCT06_190V_VOLTAGE  * VOLTAGE_TO_LEVEL_RATIO 
};

//*****************************************************************************
/// \brief Массив частот для разгона.
///
const Freq accelFreq[ACCEL_STAGE_NUM] = 
{
    eFreq16hz,
    eFreq16hz,
    eFreq25hz,
    eFreq25hz,
    eFreq50hz,
    eFreq50hz,
    eFreq50hz 
};

//*****************************************************************************
/// \brief Массив уровней для разгона, %.
///
const uint8_t accelLevel[ACCEL_STAGE_NUM] = 
{
    30, 40, 50, 60, 65, 73, 80 
};
//*****************************************************************************
/// \brief Массив времени разгона, мс.
///
const uint16_t accelTime[ACCEL_STAGE_NUM] = 
{
    100 + 50, 70, 80, 100, 50, 50, 50 
};

//*****************************************************************************
/// \brief Массив уровней отключения, %.
///
const uint8_t shutdownLevel[SHUTDOWN_STAGE_NUM] = 
{
    80, 60, 45, 30, 20, 13, 7, 5, 3, 0
};

//*****************************************************************************
// Определение локальных переменных
//*****************************************************************************
static ShiftMotor_flags flags;              ///< Флаги состояния.
static MotorType        type;               ///< Тип двигателя.
static MotorFailure     failure;            ///< Неисправность двигателя.
static ShiftMotorStates stateCnt;           ///< Счетчик состояния.
static uint16_t         workTime;           ///< Время работы.
static strOvlCurrTag    strOvlCurr;         ///< Структура контроля напряжений обратной связи по фазным напряжениям.

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
//*****************************************************************************
// Инициализация компонента
void ShuntShiftMotor_ctor( MotorType motor )
{
    ShuntShiftGen_ctor( );    //Инициализация генератора
    type = motor;
    failure = eNorm;
    workTime = 0;
    stateCnt = eStop;
    flags.ctrl = 0;      // Модуль выключен
    
    InterChannel_setParamSettings( eICId_ShuntShiftMotor_failure,
                                   eScriptSync,
                                   eProcSyncOR,
                                   eProcCheckOff, 0, 0,
                                   50, 0, 0 );
    memset( &strOvlCurr, 0, sizeof( strOvlCurr ) );
}

//*****************************************************************************
// Управление работой
void ShuntShiftMotor_run( void )
{
   
    static uint16_t timeoutCnt;            //Счетчики времени
    static uint8_t accelCnt;               //Счетчик разгона и торможения
    uint16_t current[3];                   //Значения токов и напряжений обратной связи 3-фазного генератора
        
    ShuntShiftGen_run( );                  // Работа генератора 
    
    if( stateCnt != eStop )  
        workTime++;    //Инкремент времени перевода
        
    switch( stateCnt )
    {
        case eStop:    //Выключенное состояние
            if( flags.ctrl )
            {   //Включение
                accelCnt = 0;
                ShuntShiftGen_turnOn( flags.dir, accelFreq[accelCnt], accelLevel[accelCnt] );
                timeoutCnt = accelTime[accelCnt];
                workTime = 0;                 
                stateCnt = eAcceleration;
                failure = eNorm;
                memset( &strOvlCurr, 0, sizeof( strOvlCurr ) );
            }
            break;
     
        case eAcceleration:                   //Разгон
            if( timeoutCnt ) timeoutCnt--;    //Декремент таймаута разгона
            else
            {
                accelCnt++;    //Следующая ступень разгона
                if( accelCnt < ACCEL_STAGE_NUM &&
                    accelLevel[accelCnt] < generLevel[type] )  
                {   //Разгон еще не завершился
                    ShuntShiftGen_setParam( accelFreq[accelCnt], accelLevel[accelCnt] );
                    timeoutCnt = accelTime[accelCnt];
                } 
                else
                {   // Разгон завершился
                    ShuntShiftGen_setParam( eFreq50hz, generLevel[type] );
                    stateCnt = eRunning;    
                    timeoutCnt = TIMEOUT_MEASURE_U_FB;
                    failure = eNorm; 
                    if( !InterChannel_isHandling( eICId_ShuntShiftMotor_failure ) )    
                        InterChannel_synchronize( eICId_ShuntShiftMotor_failure, failure );
                }
            }
            break;
                
        case eRunning:    //Движение
            if( flags.ctrl )
            {   //Включено
                uint8_t cntBr = 0;
                                
                //Определение обрыва обмотки, контроль корректора коэффициента мощности
                for( uint8_t i = 0; i < NUM_OF_PHASES; i++ )
                {
                    current[i] = AnalogMeasurement_getData( eAinchIV + i )->rms; 
                    if( current[i] < ( flags.stand ? STAND_BREAK_CURRENT : MOTOR_BREAK_CURRENT ) )
                    {
                        cntBr++;
                    }
                    else if( current[i] > THR_OVL_SC_ADC ) //значение тока фазы выше тока перегрузки
                    {
                        strOvlCurr.cntOvlCur++; 
                    }    
                }
                switch( cntBr )
                {
                    case 0:
                        if( ++strOvlCurr.timeoutOvlCur == TIMEOUT_OVL_SC ) //истёк таймаут определения тока перегрузки
                        {
                            strOvlCurr.timeoutOvlCur = 0;
                            failure = ( strOvlCurr.cntOvlCur > NUM_OVL_SC ) ? eOverloadCircuit : eNorm;
                            strOvlCurr.cntOvlCur = 0;
                        }
                        break; 
                    case 1:
                        failure = eBreakOnePhase;
                        memset( &strOvlCurr, 0, sizeof(strOvlCurr) );
                        break;
                    case 2:
                    case 3:
                        failure = eBreakAllPhases;
                        memset( &strOvlCurr, 0, sizeof(strOvlCurr) );
                        break;
                    default: 
                        ERROR_ID( eGrPS_ShuntShift, ePS_ShiftMotorCnt2Err );
                }
            }
            else
            { //Переход к отключению двигателя
                for( accelCnt = 0; accelCnt < SHUTDOWN_STAGE_NUM - 1; accelCnt++ )
                {
                    // Определение ступени отключения ниже уровня генератора
                    if( generLevel[type] > shutdownLevel[accelCnt] ) 
                        break;
                }
                ShuntShiftGen_setParam( eFreq50hz, shutdownLevel[accelCnt] );
                timeoutCnt = SHUTDOWN_STAGE_TIME;
                stateCnt = eShutdown;
                if( !InterChannel_isHandling( eICId_ShuntShiftMotor_failure ) )    
                    InterChannel_synchronize( eICId_ShuntShiftMotor_failure, failure );
            }
            break;
 
        case eShutdown:    //Отключение 
            if( timeoutCnt ) timeoutCnt--;   //Декремент таймаута
            else
            {
                accelCnt++;
                if( accelCnt < SHUTDOWN_STAGE_NUM)
                {   // Отключение продолжается
                    ShuntShiftGen_setParam( eFreq50hz, shutdownLevel[accelCnt] );
                    timeoutCnt = SHUTDOWN_STAGE_TIME;
                }
                else
                {   // Отключение завершено
                    ShuntShiftGen_turnOff( );
                    stateCnt = eFullShutdown;
                }
            }
            break;

        case eFullShutdown:    //Полное отключение (отключение генератора)
            if( !ShuntShiftGen_isOn( ) )
            {
                stateCnt = eStop;
            }
            break;

        default:
            ERROR_ID( eGrPS_ShuntShift, ePS_ShiftMotorCnt1Err );
    }
}

//*****************************************************************************
// Включение
void ShuntShiftMotor_turnOn( bool stand, bool dir )
{
    if( !flags.ctrl ) 
    {
        flags.dir = dir;
        flags.stand = stand;
        flags.ctrl = 1;
    }
}

//*****************************************************************************
// Выключение
void ShuntShiftMotor_turnOff( void )
{
    flags.ctrl = 0;
}

//*****************************************************************************
// Проверка состояния модуля
bool ShuntShiftMotor_isOn( void )
{
    return stateCnt != eStop;
}

//*****************************************************************************
// Получить неисправность
MotorFailure ShuntShiftMotor_getFailure( void )
{
    return failure;
}
//*****************************************************************************
// Установить неисправность (используется из модуля IntegrCtrl)
void ShuntShiftMotor_setFailure( MotorFailure value )
{
    failure = value;
}

//*****************************************************************************
// Получить время работы двигателя
uint16_t ShuntShiftMotor_getWorkingTime( void )
{
    return workTime;
}

//*****************************************************************************
/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   24-01-2018
* Автор  Кругликов В.П.
* 
* Изменения:
*    Базовая версия.
*/
