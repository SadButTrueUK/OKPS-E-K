/**
* \file    shuntShiftMotor.c
* \brief   \copybrief shuntShiftMotor.h
*
* \version 1.0.3
* \date    18-02-2021
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "shuntShiftMotor.h"
#include "shuntShiftGen.h"
#include "ProtectionState_codes.h"
#include "asserts.h"
#include "string.h"
#include "asserts_ex.h"
#include "InterChannel.h"
#include "ControlSystem.h"
#include "BinIn.h"
#include "shuntShift.h"

#include "DebugTools.h"

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
/// \brief Таймаут вызовов функции вычисления среднего тока при переводе или при работе на фрикцию 
#define TIMEOUT_FOR_FUNC_CALC_MIDDLE_VAL   10 

#define TIMEOUT_FOR_AVERAGE_IN_WINDOW 900

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

#define  NUM_OF_PHASES  3    ///< Количество фаз сигнала.


//*****************************************************************************
// Объявление типов данных
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
    bool isOvlCurr;                ///< Бит перегрузки по току
}strOvlCurrTag;

//*****************************************************************************
/// \brief Структура для работы с кольцевым буфером для получения
/// скользящего среднего этого буфера 
///
typedef struct CircularBuffer_tag 
{
    uint16_t  buff[MIDDLE_VAL_WINDOW_SIZE]; //буфер отсчётов        
    uint8_t   index;                         //индекс текущего отсчёта
    uint16_t  averageVal;                   //конечное усреднённое значение
    uint8_t  sizeBuf;
    uint8_t  shifts;
} CircularBuffer;

//*****************************************************************************
/// \brief Идентификаторы значений автомата состояний функции #ShuntShiftMotor_run.
///
typedef enum
{
    eStartState          = 0,    ///< выключенное состояние
    eAcceleration,         ///< разгон
    eRunning,              ///< движение
    eShutdown,             ///< отключение
    eFullShutdown          ///< полное отключение (отключение генератора)
} ShiftMotorStates;

//*****************************************************************************
// Объявление локальных типизированных констант
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
// Объявление локальных переменных
//*****************************************************************************
static ShiftMotor_flags flags;              ///< Флаги состояния.
static MotorType        type;               ///< Тип двигателя.
static MotorFailure     failure;            ///< Неисправность двигателя.
static ShiftMotorStates stateCntMotor;           ///< Счетчик состояния.
static uint16_t         workTime;           ///< Время работы.
static strOvlCurrTag    strOvlCurr;         ///< Структура контроля напряжений обратной связи по фазным напряжениям.
static uint16_t         timeoutBreakPh;     ///< Счетчик времени накопления состояния обрыва фаз
static CircularBuffer circularBuffer[NUM_OF_PHASES]; ///< Массив структур для вычисения средних значений фазных токов в выбранном временном окне 
uint16_t timeoutForAverageInWindow; 


//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

/// \brief  Функция для определения типа обрыва фаз
/// \param state - тип обрыва (1 фаза либо 2(3));
static void checkBreakPhase( MotorFailure state );

static void calcMiddleValueInWindow( const uint16_t data, CircularBuffer* structOfData, const uint8_t t_o_value );

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
    stateCntMotor = eStartState;
    flags.ctrl = 0;      // Модуль выключен
    
    InterChannel_setParamSettings( eICId_ShuntShiftMotor_failure,
                                   eScriptSync,
                                   eProcSyncOR,
                                   eProcCheckOff, 0, 0,
                                   50, 0, 0 );
    memset( &strOvlCurr, 0, sizeof( strOvlCurr ) );
    
    for( uint8_t i = 0; i < NUM_OF_PHASES; i++ ) //инициализация структур для вычисления среднего значения токов фаз
    {
        memset( &circularBuffer[i].buff, 0, sizeof( circularBuffer[i].buff ) );
        memset( &circularBuffer[i].index, 0, sizeof( circularBuffer[i].index ) );
        memset( &circularBuffer[i].sizeBuf, MIDDLE_VAL_WINDOW_SIZE, sizeof( circularBuffer[i].sizeBuf ) );
        memset( &circularBuffer[i].shifts, 7, sizeof( circularBuffer[i].shifts ) );
    }
}

//Функция для определения типа обрыва фаз
static void checkBreakPhase( MotorFailure state )
{
    static const uint16_t BREAK_THR_TIME = 250; //порог времени для определения обрыва
    
    if( timeoutBreakPh++ == BREAK_THR_TIME )
    {
        timeoutBreakPh = 0;
        failure = state;
        memset( &strOvlCurr, 0, sizeof( strOvlCurr ) );
    }
}

//*****************************************************************************
// Управление работой
void ShuntShiftMotor_run( void )
{
   
    static uint16_t timeoutCnt, timeoutBreakPh /*, timeoutForAverageInWindow*/; //Счетчики времени
    static uint8_t accelCnt;                    //Счетчик разгона и торможения
    uint16_t current[3];                        //Значения токов и напряжений обратной связи 3-фазного генератора
    
    ShuntShiftGen_run( );                  // Работа генератора 
    //Время перевода считаем пока мотор не остановлен либо когда пропало 220В во время перевода стрелки
    if( stateCntMotor != eStartState || ControlSystem_getStateCheck220V( ) == eCount )    
        workTime++;    //Инкремент времени перевода
    if( timeoutForAverageInWindow > 0 )
        timeoutForAverageInWindow--;
    
    switch( stateCntMotor )
    {
        case eStartState:    //Выключенное состояние
            if( flags.ctrl )
            {   //Включение
                accelCnt = 0;
                ShuntShiftGen_turnOn( flags.dir, accelFreq[accelCnt], accelLevel[accelCnt] );
                timeoutCnt = accelTime[accelCnt];
                timeoutForAverageInWindow = TIMEOUT_FOR_AVERAGE_IN_WINDOW;
                workTime = 0;                 
                failure = eNorm;
                memset( &strOvlCurr, 0, sizeof( strOvlCurr ) );
                for( uint8_t i = 0; i < NUM_OF_PHASES; i++ ) //инициализация структур для вычисления среднего значения токов фаз
                {
                    memset( &circularBuffer[i].buff, 0, sizeof( circularBuffer[i].buff ) );
                    memset( &circularBuffer[i].index, 0, sizeof( circularBuffer[i].index ) );
                    circularBuffer[i].averageVal = 0;
                }
                stateCntMotor = eAcceleration;
            }
            break;
        case eAcceleration:                   //Разгон
            if( timeoutCnt ) 
                timeoutCnt--;    //Декремент тайм-аута разгона
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
                    timeoutCnt = TIMEOUT_MEASURE_U_FB;
                    failure = eNorm; 
                    if( !InterChannel_isHandling( eICId_ShuntShiftMotor_failure ) )    
                        InterChannel_synchronize( eICId_ShuntShiftMotor_failure, failure );
                    stateCntMotor = eRunning; 
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
                        if( timeoutForAverageInWindow == 0 ) //считаем среднее в кольцевом буфере через 900 мс после начала перевода
                        {
                            calcMiddleValueInWindow( InterChannel_getData( eICId_IV_rms + i ), 
                            &circularBuffer[i], TIMEOUT_FOR_FUNC_CALC_MIDDLE_VAL );
                        }
                        current[i] = AnalogMeasurement_getData( eAinchIV + i )->rms; 
                        if( current[i] < ( flags.stand ? STAND_BREAK_CURRENT : MOTOR_BREAK_CURRENT ) && BinIn_is220vOk( ) )
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
                            if( ++strOvlCurr.timeoutOvlCur == TIMEOUT_OVL_SC ) //истёк тайм-аут определения тока перегрузки
                            {
                                strOvlCurr.timeoutOvlCur = 0;
                                if ( strOvlCurr.cntOvlCur > NUM_OVL_SC )
                                {
                                    strOvlCurr.isOvlCurr = true;
                                    failure = eOverloadCircuit;
                                }
                                    strOvlCurr.cntOvlCur = 0;
                            }
                            timeoutBreakPh = 0;
                            break; 
                        case 1:
                            checkBreakPhase( eBreakOnePhase );
                            break;
                        case 2:
                        case 3:
                            checkBreakPhase( eBreakAllPhases );
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
                stateCntMotor = eShutdown;
                if( !InterChannel_isHandling( eICId_ShuntShiftMotor_failure ) )    
                    InterChannel_synchronize( eICId_ShuntShiftMotor_failure, failure );
            }
            break;
 
        case eShutdown:    //Отключение 
            if( timeoutCnt ) 
                timeoutCnt--;   //Декремент тайм-аута
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
                    stateCntMotor = eFullShutdown;
                }
            }
            break;

        case eFullShutdown:    //Полное отключение (отключение генератора)
            if( !ShuntShiftGen_isOn( ) )
            {
                stateCntMotor = eStartState;
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
    return stateCntMotor != eStartState;
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
// Получить состояние перегрузки по току
bool ShuntShiftMotor_getIsOvlCurr( void )
{
    return strOvlCurr.isOvlCurr;
}

//*****************************************************************************
// Установить состояние перегрузки по току
void ShuntShiftMotor_setIsOvlCurr( bool val )
{
    strOvlCurr.isOvlCurr = val;
}

//*****************************************************************************
// Возвращает среднее значение тока фаз управления двигателя
uint16_t ShuntShiftMotor_getAverageThreePhasesCurrent( void )
{
    return __builtin_divud( circularBuffer[0].averageVal + circularBuffer[1].averageVal +
                            circularBuffer[2].averageVal, 3);
}
//*****************************************************************************
// Работа с кольцевым буфером для вычисления скользящего среднего.
// Входные параметры :
// data  – данные АЦП для помещения в буфер.
// structOfData - указатель на структуру кольцевого буфера
// t_o_value  - значение таймаута для работы функции
static void calcMiddleValueInWindow( const uint16_t data, CircularBuffer* structOfData, const uint8_t t_o_value )
{ 
    //return;
    
    uint32_t sumBuff = 0;
    static uint8_t timeout;
    
    if( ++timeout < t_o_value ) //Функция вызывается в единицу времени t_o_value (в миллисекундах)
        return; 
    timeout = 0;
    
    if( structOfData->index == structOfData->sizeBuf )
        structOfData->index = 0;
    structOfData->buff[structOfData->index++] = data; 
    for( uint8_t i = 0; i < structOfData->sizeBuf; i++ )
    {
        sumBuff += structOfData->buff[i]; 
    }
    structOfData->averageVal = sumBuff >> structOfData->shifts;
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
* 
* Версия 1.0.2
* Дата   15-10-2020
* Автор  Кругликов В.П.
* 
* Изменения:
*   - изменена логика формирования признака перегрузки по току во время перевода стрелки в ShuntShiftMotor_run;
*   - добавлены реализации интерфейсов ShuntShiftMotor_getIsOvlCurr и ShuntShiftMotor_setIsOvlCurr. 
*
* Версия 1.0.3
* Дата   18-02-2021
* Автор  Кругликов В.П.
* 
* Изменения:
*   Добавлена функция обработки состояния обрыва фаз(фазы) checkBreakPhase 
*/
