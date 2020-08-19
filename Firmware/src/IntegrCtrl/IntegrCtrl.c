/**
* \file    IntegrCtrl.c
* \brief   \copybrief IntegrCtrl.h
*
* \version 1.0.1
* \date    05-02-2018
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "IntegrCtrlGen.h"
#include "IntegrCtrl.h"
#include "InterChannel.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"
#include "AnalogMeasurement.h"
#include "Main.h"
#include "Tracing.h"
#include "OverloadDet.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
// Особенности вычислений
// 1. Ток 1А - напряжение 0.2592 В 
// 2. Ток 0А - напряжение постоянного смещения Udc = Uref/2 = 1.5 В
// 3. 1 дискрета АЦП = 3 В/(2^12)
// 4. 1 дискрета АЦП = 2.826 мА
// 5. Динамический диапазон измерения тока (средневыпрямленное значение)  Iav = (Uref - Udc)/0.2592/sqrt(2) = 4.092 А

#define NUM_PHASES                 3            ///< Количество фаз сигнала.
#define TIME_PAUSE_PHASE_TO_PHASE  400U         ///< Пауза в мс между выдачей напряжения в соседних фазах сигнала.

//*****************************************************************************
/// \brief Пауза в мс перед началом следующего периода тестирования целостности.
///
#define PAUSE_BEFORE_NEXT_CHECK    400U

//*****************************************************************************
#define START_TIMEOUT              5000U        ///< Начальный таймаут а также пауза после снятия КЗ.

//*****************************************************************************
/// \brief Значение уровня напряжения, по скорости нарастания которого принимается решение о наличии КЗ (для двигателя)
///
#define LEVEL_THR_SC_MOTOR               26U 

//*****************************************************************************
/// \brief Значение уровня напряжения, по скорости нарастания которого принимается решение о наличии КЗ (для эквивалента стрелки)
///
#define LEVEL_THR_SC_STAND               30U

//*****************************************************************************
/// \brief Значение тока в мА*10, которое считается номинальным для номинальной нагрузки.
///
#define CURR_INTEGR_WORK_MA_x10    5060U    

//*****************************************************************************
#define CURR_INTEGR_STAND_MA_x10   2200U    ///< Значение тока в мА, которое считается номинальным для работы на стенде.

//*****************************************************************************
/// \brief Коэффициент преобразования мА в единицы АЦП (умножен на 10) 1 дискрета - 2.826 мА.
///
#define  CURR_RATIO          29U         

//*****************************************************************************
/// \brief Значение номинального рабочего тока в дискретах АЦП.
///                   
#define CURR_INTEGR_WORK_ADC   ( CURR_INTEGR_WORK_MA_x10 / CURR_RATIO ) 

//***************************************************************************** 
/// \brief Значение номинального тока на стенде в дискретах АЦП. 
///  
#define CURR_INTEGR_STAND_ADC  ( CURR_INTEGR_STAND_MA_x10 / CURR_RATIO )

//*****************************************************************************
/// \brief Порог по току в начальной зоне увеличения напряжения сигнала.
///                
#define THR_CURR_RISE_ZONE     ( CURR_INTEGR_WORK_ADC / 2 )

//*****************************************************************************
/// \brief Максимальный уровень сигнала, по которому определяется обрыв.
///
#define MAX_LEVEL              ( 14U * MULTIPLY_VOLTAGE_STEP_INT_CTRL )

//*****************************************************************************
#define MIN_LEVEL_INT_CTRL       0               ///< Минимальный уровень сигнала.

//*****************************************************************************
/// \brief Уровень напряжения, при котором тестируется корректор коэффициента мощности.
///
#define LEVEL_TESTING_CORRECTOR  180U   

//*****************************************************************************
/// \brief Норма напряжения фаз (по обратной связи) при рабочем корректоре коэффициента мощности.
///
#define THR_U_CORRECTOR_NORMAL   3005U 

//*****************************************************************************
/// \brief Значение нижнего допуска напряжения фаз при проверке ККМ, процентов.
///
#define NUM_PERCENTS_LOW        10U

//*****************************************************************************
/// \brief Значение верхнего допуска напряжения фаз при проверке ККМ, процентов.
///
#define NUM_PERCENTS_HIGH       5U

//*****************************************************************************
/// \brief Нижнее минимально допустимое значение напряжения при проверке ККМ.
///
#define THR_U_CORRECTOR_IS_FAIL_LOW  ( THR_U_CORRECTOR_NORMAL - ( THR_U_CORRECTOR_NORMAL * NUM_PERCENTS_LOW / 100 ) )

//*****************************************************************************
/// \brief Верхнее максимально допустимое значение напряжения при проверке ККМ.
///
#define THR_U_CORRECTOR_IS_FAIL_HIGH ( THR_U_CORRECTOR_NORMAL + ( THR_U_CORRECTOR_NORMAL * NUM_PERCENTS_HIGH / 100 ) )

//*****************************************************************************
/// \brief Шаг приращения сигнала по времени в мс при тестировании корректора коэффициента мощности.
/// 
#define STEP_INCR_TESTING_CORRECTOR   1U  

//*****************************************************************************
/// \brief Шаг времени в мс приращения сигнала для стенда (зона 1).
///
#define STEP_INCR_VALUE_RISE_STAND   10U  

//*****************************************************************************
/// \brief Шаг времени в мс приращения сигнала для рабочей нагрузки (зона 1).
///
#define STEP_INCR_VALUE_RISE_WORK    20U

//*****************************************************************************    
/// \brief Шаг времени в мс приращения сигнала для стенда(зона 2).
///                               
#define STEP_INCR_VALUE_MIDDLE_STAND 10U

//*****************************************************************************    
/// \brief Шаг времени в мс приращения сигнала для рабочей нагрузки(зона 2).
///       
#define STEP_INCR_VALUE_MIDDLE_WORK  20U

//*****************************************************************************
#define STEP_DECR_VALUE        1U               ///< Шаг времени в мс уменьшения сигнала.

//*****************************************************************************
/// \brief Количество чередований фаз при 2-х проходах 3-х фаз (АВС, АСВ). 
///
#define NUM_OF_INTERLEAVING    2


//*****************************************************************************
/// \brief Уровень напряжения двух фаз (RMS), принятого за опасное при тестировании третьей фазы 
///
#define THR_OF_UNSAFE_VOLTAGE_RMS    20


//*****************************************************************************
/// \brief Уровень напряжения двух фаз (мгновенное), принятого за опасное при тестировании третьей фазы 
///
#define THR_OF_UNSAFE_VOLTAGE_VAL    50



//*****************************************************************************
/// \brief Количество фаз, превішение напряжения на которыъ приводит к опасному отказу 
///
#define NUM_UNSAFE_PHASE_VOLTAGES     2

//*****************************************************************************
// Определение типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief  Значения автомата состояний функции #IntegrCtrl_run.
/// 
typedef enum
{
    eSwitchedOff,                     ///< выключен
    
    /// \brief начальный таймаут либо таймаут после появления нештатной ситуации (КЗ или обрыв)
    ///
    eStartTimeout,
    eIncLevel,                        ///< увеличение значения уровня до перехода к следующему состоянию автомата
    eDecLevel,                        ///< уменьшение значения до минимума
    ePauseToOtherPhase,               ///< пауза при переходе к другой фазе
    eWaitingForPeriod,                ///< ожидание повтора по истечении периода проверки
    eSetPauseToOtherPhase,            ///< таймаут перед переходом к тестированию следующей фазы
    
    /// \brief вычисление значения напряжения обратной связи при тестировании корректора коэффициента мощности
    ///
    eCalcUfbPhase
} IntDetCtrlStates;

//*****************************************************************************
/// \brief Значения фазного тока и напряжения.
///
typedef struct
{
    uint16_t currentPhase;        ///< Измеренный ток фазы (мгновенное значение).
    uint16_t levelPhase;          ///< Измеренное напряжение фазы (мгновенное значение).
} strStatusPhases;

//*****************************************************************************
// Определение локальных типизированных констант
//*****************************************************************************

//*****************************************************************************
/// \brief Пороги рабочего тока для номинальной нагрузки и для стенда.
/// 
const uint16_t currThrWork[] =
{
    CURR_INTEGR_WORK_ADC,
    CURR_INTEGR_STAND_ADC
};

//*****************************************************************************
/// \brief Шаги приращения сигнала по времени для номинальной нагрузки и для стенда (зона 1)
/// 
const uint8_t stepsIncr[] =
{
    STEP_INCR_VALUE_RISE_WORK, 
    STEP_INCR_VALUE_RISE_STAND  
};

//*****************************************************************************
/// \brief Шаги приращения сигнала по времени для номинальной нагрузки и для стенда (зона 2)
/// 
const uint8_t stepsMiddle[] =
{
    STEP_INCR_VALUE_MIDDLE_WORK,
    STEP_INCR_VALUE_MIDDLE_STAND
};

//*****************************************************************************
/// \brief Маска текущего значения фазы (для реализации чередования фаз в соседних периодах тестирования).
///
const uint8_t maskSeqPh[NUM_OF_INTERLEAVING][NUM_PHASES] =
{
    { ePhaseV, ePhaseW, ePhaseU },
    { ePhaseV, ePhaseU, ePhaseW }
};

//*****************************************************************************
/// \brief Индексы двух фаз, на которых будет проверяться наличие напряжения при тестировании третьей фазы
///
const uint8_t otherPhaseIndex[NUM_PHASES][NUM_UNSAFE_PHASE_VOLTAGES] =
{
    { eAinchUW, eAinchUU },
    { eAinchUV, eAinchUU },
    { eAinchUV, eAinchUW }
};

//*****************************************************************************
// Определение локальных переменных
//*****************************************************************************

//*****************************************************************************
static uint16_t level;          ///< Уровень генератора импульсов тестирования целостности обмоток в процентах.
static uint8_t  stepCnt;        ///< Значение автомата состояний \a IntDetGen.

//*****************************************************************************
static bool stand;              ///< Текущая фаза 3-фазного сигнала.
static bool ctrl;               ///< Разрешение/запрет работы.
static bool isActiveIntCtrl;    ///< Флаг, по которому фиксируется положение стрелки до окончания теста всех фаз

//*****************************************************************************  
static Phase phase = ePhaseV;                         ///< Текущая фаза сигнала.
static strStatusPhases strPhase[NUM_PHASES];          ///< Массив структур тока и напряжения для каждой фазы.

//*****************************************************************************
/// \brief Состояние фазы (норма, обрыв одной фазы, обрыв более одной фазы, КЗ).
///
static MotorFailure failure = eNorm;

//*****************************************************************************
static uint8_t  sumBreak;               ///< Счётчик обрывов фаз.
static uint8_t  sumShortCircuit;        ///< Счётчик КЗ фаз.
static bool     dir;                    ///< Для установки чередования фаз по принципу АВС, АСВ.

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация модуля контроля целостности обмоток
void IntegrCtrl_ctor( void )
{
    IntegrCtrlGen_ctor( ); // Отключение ШИМ, назначение выводов как IO Ports
    ctrl = true;
    stepCnt = eSwitchedOff; // Начальная инициализация автомата состояний в положении выкл
    phase = ePhaseV;        // Начальная инициализация фазы сигнала
    InterChannel_setParamSettings( eICId_IntegrCtrlScAndBreak,  
                                   eScriptSync,
                                   eProcSyncEqual,
                                   eProcCheckEqual, 0, 0,
                                   100, 200, 0 );            // Статус фаз сигнала помещаем в межпроцессорную синхронизацию
    level = MIN_LEVEL_INT_CTRL;      // Начальная инициализация минимального уровня напряжения сигнала тестирования
    sumBreak = 0;
    sumShortCircuit = 0;
    isActiveIntCtrl = false;
}

//*****************************************************************************
// Управление работой алгоритма контроля целостности обмоток
void IntegrCtrl_run( void )
{
    // Величина буфера для усреднения результата тестирования корректора и величина таймаута.
    static const uint8_t NUM_MEASURES_VALUES_U_FB_PHASE = 32, TIMEOUT_BEFORE_MEASURE_U_FB = 6;
    
    static const uint8_t NUM_SHIFTS = 5;               // Количество сдвигов при усреднении.
    static const uint8_t THRESHOLDS_SC[2] = { LEVEL_THR_SC_MOTOR, LEVEL_THR_SC_STAND };  //Пороги для определения тока КЗ
    static const uint8_t NUM_OF_SEQUENTIAL_TEST_CORR_TRIES = 3; //если за это число попыток тест ККМ в норме - 
                                                                //отсчитываем следующие 3 попытки
    static const uint8_t STEP_INC_CORR = 2;  //шаг приращения счётчика неудачных тестов ККМ
    
    
    static uint8_t time_step; // Индекс временной зоны на этапе увеличения напряжения.
    static uint8_t stepVal;   // Шаг приращения по времени на этапе увеличения напряжения.
        
    //перечисление типов тестирования (целостность обмоток или корректора коэфф. мощности)
    static IntDetTypeTesting  typeTesting = eTestingIntegrCtrl;
    static uint16_t cntTime;                    ///< Счётчик таймаутов.
    static uint32_t buffValUfb[NUM_PHASES];     // массив буфера  напряжения фаз сигнала (обратная связь)
    static uint8_t cntTestCorrFail[NUM_PHASES] = {0, 0, 0};
    static uint8_t cntTriesTestCorr;          
    
    
    if( cntTime ) 
        cntTime--;     //счётчик шагов автомата
    
    //Добавляем трассировку токов и напряжений по каждой фазе
    if( stepCnt != eSwitchedOff &&  stepCnt != eStartTimeout )
    {
        for(uint8_t cnt = 0; cnt  < NUM_PHASES; cnt++  )
        {
            Tracing_parameter( InterChannel_getData( eICId_IV_value + cnt ), cnt );
            Tracing_parameter( AnalogMeasurement_getData( eAinchUV + cnt )->value, cnt + 3 );
        }
    }
        
    switch( stepCnt )
    {
        case eSwitchedOff:              // Алгоритм выключен, ожидание включения
            if( ctrl )
            {
                cntTime = START_TIMEOUT;    //работа модуля возможна по истечении таймаута START_TIMEOUT
                stepCnt = eStartTimeout;
            }
            break;
        case eStartTimeout:
            if( cntTime == 0 )
            {
                IntegrCtrlGen_turnOn( ); // Включить формирователь сигнала   
                level = MIN_LEVEL_INT_CTRL;
                time_step = 0;
                phase = ePhaseV;
                cntTriesTestCorr = 0;
                for( uint8_t cnt = 0; cnt < NUM_PHASES; cnt++ ) 
                {
                    buffValUfb[cnt] = 0; 
                    cntTestCorrFail[cnt] = 0;
                }
                sumBreak = 0;
                sumShortCircuit = 0;
                typeTesting = eTestingIntegrCtrl;
                stepCnt = eIncLevel;     // Переход к следующему шагу автомата  
            }
            break;
        case eIncLevel:                 // Увеличение уровня level до тока CURR_INTEGR_WORK_ADC
            //Чтение значения измеренного АЦП тока фазы, на которой производится тестирование (мгновенное значение)
            time_step++;
            if( typeTesting == eTestingCorrector )
            {
                if( time_step == STEP_INCR_TESTING_CORRECTOR )  //отсчитан нужный шаг приращения
                {
                    if( ++level >= LEVEL_TESTING_CORRECTOR )
                    {
                        stepCnt = eCalcUfbPhase;        //перейти на обработку полученного результата
                        cntTime = TIMEOUT_BEFORE_MEASURE_U_FB;
                    }
                    else
                    {
                        for( uint8_t cnt = 0; cnt < NUM_PHASES; cnt++ )
                            //установить на всех фазах одинаковое напряжение
                            IntegrCtrlGen_setParam( ePhaseV + cnt, level, typeTesting );     
                    }
                    time_step = 0;
                }
            }
            else if( typeTesting == eTestingIntegrCtrl && ActivityManager_isActive( activityManager ) == true )
            {    
                uint16_t phVolt1RMS = AnalogMeasurement_getData( otherPhaseIndex[maskSeqPh[dir][phase]][0] )->rms;
                uint16_t phVolt2RMS = AnalogMeasurement_getData( otherPhaseIndex[maskSeqPh[dir][phase]][1] )->rms;
                uint16_t phVolt1Val = AnalogMeasurement_getData( otherPhaseIndex[maskSeqPh[dir][phase]][0] )->value;
                uint16_t phVolt2Val = AnalogMeasurement_getData( otherPhaseIndex[maskSeqPh[dir][phase]][1] )->value;
                
                if( phVolt1RMS >= THR_OF_UNSAFE_VOLTAGE_RMS 
                    || phVolt2RMS >= THR_OF_UNSAFE_VOLTAGE_RMS 
                    || phVolt1Val >= THR_OF_UNSAFE_VOLTAGE_VAL
                    || phVolt2Val >= THR_OF_UNSAFE_VOLTAGE_VAL )
                {
                    //Переход в ЗС если на двух фазах формируется напряжениеболее порогового значения
                    ERROR_EX_ID( eGrPS_IntegrCtrl, ePS_SafePhaseVoltage, phVolt1Val, phVolt2Val, phVolt1RMS, phVolt2RMS ); 
                }    
                strPhase[maskSeqPh[dir][phase]].currentPhase = InterChannel_getData( eICId_IV_value + 
                maskSeqPh[dir][phase] );
                // Рабочий диапазон с учётом "перезаброса"
                if( strPhase[maskSeqPh[dir][phase]].currentPhase >= currThrWork[stand] ) 
                {
                    time_step = 0;
                    // Проверка на ток КЗ
                    if( level < THRESHOLDS_SC[stand] )      //произошло КЗ
                    {
                        level = MIN_LEVEL_INT_CTRL;
                        IntegrCtrlGen_setParam( maskSeqPh[dir][phase], level, typeTesting );
                        stepCnt = eSetPauseToOtherPhase;
                        sumShortCircuit++;
                    }
                    else 
                        stepCnt = eDecLevel;      // КЗ фазы нет - переход к следующему шагу автомата    
                }
                else
                {
                    // Установка шага в начальной зоне увеличения напряжения (зона 1)
                    if( strPhase[maskSeqPh[dir][phase]].currentPhase < THR_CURR_RISE_ZONE )
                        stepVal = stepsIncr[stand];
                        // Установка шага в зоне приближения к максимуму (зона 2)
                    else
                        stepVal = stepsMiddle[stand];
                    if( time_step == stepVal )
                    {
                        if( level < MAX_LEVEL ) 
                            level++;
                        else
                        {
                            stepCnt = eDecLevel;        //переход к уменьшению напряжения
                            sumBreak++;                 //увеличение счётчика обрыва фазы
                        }
                        strPhase[maskSeqPh[dir][phase]].levelPhase = level;
                        // Установка фазы и уровня формирователя сигнала контроля целостности в минимум
                        IntegrCtrlGen_setParam( maskSeqPh[dir][phase], level, typeTesting );
                        time_step = 0;
                    }
                }
            }
            break;
        case eCalcUfbPhase:
            //Вычисление полученного значения с датчиков напряжения фаз (с интегрированием) и принятие решения 
            if( cntTime == 0 )    
            {    
                if( time_step++ < NUM_MEASURES_VALUES_U_FB_PHASE )   //используем усреднение
                {
                    //в цикле читаем значения напряжения обратной связи фаз  
                    for( uint8_t cnt = 0; cnt < NUM_PHASES; cnt++ )
                        buffValUfb[cnt] += AnalogMeasurement_getData( eAinchUV + cnt )->value;
                }    
                else
                {
                    time_step = 0;
                    for( uint8_t cnt = 0; cnt  < NUM_PHASES; cnt++ )
                    {   
                        buffValUfb[cnt] >>= NUM_SHIFTS;          //усреднение результата
                    }    
                    for( uint8_t cnt = 0; cnt  < NUM_PHASES; cnt++ )
                    {
                         CASSERT_ID( eGrPS_IntegrCtrl, ePS_CorrectorIsFail,              //Группа и код отказа
                                     cntTestCorrFail[cnt],                               //Счётчик ошибок
                                     NUM_OF_SEQUENTIAL_TEST_CORR_TRIES * STEP_INC_CORR,  //Максимальное значение счётчика ошибок
                                     buffValUfb[cnt] >= THR_U_CORRECTOR_IS_FAIL_LOW &&
                                     buffValUfb[cnt] <= THR_U_CORRECTOR_IS_FAIL_HIGH );  //Проверяемое условие
                        // Контроль пороговых значений напряжения ККМ (при превышении - уход в ЗС)
                        buffValUfb[cnt] = 0;
                    }
                    if(++cntTriesTestCorr >= NUM_OF_SEQUENTIAL_TEST_CORR_TRIES )
                    {
                        cntTriesTestCorr = 0;
                        for( uint8_t cnt = 0; cnt < NUM_PHASES; cnt++ )  cntTestCorrFail[cnt] = 0;
                    }
                    stepCnt = eDecLevel;
                }
            }
            break;
        case eDecLevel:                 // Уменьшение уровня level до минимума с шагом STEP_DECR_VALUE
            if( --level )
            {
                if( ++time_step == STEP_DECR_VALUE )
                {
                    if( typeTesting == eTestingCorrector )
                    {
                        // Установка фазы и уровня формирователя (одновременно все фазы)
                        for( uint8_t cnt = 0; cnt  < NUM_PHASES; cnt++ )    
                            IntegrCtrlGen_setParam( ePhaseV + cnt, level, typeTesting );   
                    }
                    else if( typeTesting == eTestingIntegrCtrl )
                    {
                        // Установка фазы и уровня формирователя
                        IntegrCtrlGen_setParam( maskSeqPh[dir][phase], level, typeTesting );   
                    }
                    time_step = 0;
                }
            }
            else
            {
                time_step = 0;
                if( typeTesting == eTestingIntegrCtrl )
                    stepCnt = eSetPauseToOtherPhase;
                else if( typeTesting == eTestingCorrector )
                {
                    // Установка времени паузы перед началом следующего цикла проверки
                    cntTime = PAUSE_BEFORE_NEXT_CHECK;
                    stepCnt = eWaitingForPeriod; 
                }    
                else {}
            }
            break;
        case eSetPauseToOtherPhase:
            cntTime = TIME_PAUSE_PHASE_TO_PHASE;        // Установка времени паузы перед переходом на другую фазу
            stepCnt = ePauseToOtherPhase;               // Переход к следующему шагу автомата   
            break;
        case ePauseToOtherPhase:                        // Пауза перед переходом на другую фазу        
            if( cntTime == 0 )                          // Время таймаута окончено
            {
                // Если не все фазы пройдены, увеличить счётчик фазы, вернуться к началу автомата и повторить
                if( ++phase < NUM_PHASES )
                {
                    stepCnt = eIncLevel;
                }
                else                                    // Все фазы пройдены, проверка состояния всех фаз
                {
                    phase = ePhaseV;                    // Установка начального значения фазы
                    if( sumShortCircuit > 0 )           // КЗ хотя бы по одной фазе
                    {
                        failure = eShortCircuit;
                        sumShortCircuit = 0;
                    }   
                    else if( sumBreak == 1 )            // Если есть обрыв хотя бы одной фазы
                        failure = eBreakOnePhase;       // Обрыв одной фазы
                    else if( sumBreak == 2 || sumBreak == 3 )
                        failure = eBreakAllPhases;      // Обрыв двух (курбель) или трёх фаз
                    else
                        failure = eNorm;                // Нормальное состояние обмоток двигателя
                    sumBreak = 0;
                    // Установка времени паузы перед началом следующего цикла проверки
                    cntTime = PAUSE_BEFORE_NEXT_CHECK;
                    
                    //выдача в межпроцессорный обмен  состояния фаз (норма, КЗ, обрыв) 
                    if( !InterChannel_isHandling( eICId_IntegrCtrlScAndBreak ) )    
                        InterChannel_synchronize( eICId_IntegrCtrlScAndBreak, failure );
                    stepCnt = eWaitingForPeriod;               // Переход к следующему шагу автомата
                     
                }
            }
            break;
        case eWaitingForPeriod:             // Пауза перед началом следующего цикла проверки
            if( cntTime == 0 )  
            {
                if( typeTesting == eTestingIntegrCtrl && 
                    failure != eShortCircuit && 
                    OverloadDet_getScThreePhaseGenTrig() == false ) 
                    typeTesting = eTestingCorrector;        //переход на тестирование корректора !!ТОЛЬКО!! если нет КЗ
                else 
                    typeTesting = eTestingIntegrCtrl;
                isActiveIntCtrl = false;
                stepCnt = eIncLevel;                       // По истечении времени паузы вернуться к началу автомата
            }
            break;
        default:                            // Ошибка автомата состояний (неизвестное состояние)
            ERROR_ID( eGrPS_IntegrCtrl, ePS_IntegrCtrlStepCntError );
    }
}

//*****************************************************************************
// Включение модуля контроля целостности обмоток
void IntegrCtrl_turnOn( bool st, PositionDet_State direction )
{
    stand = st;
    ctrl = true;
    if( !isActiveIntCtrl )
    {
        dir = direction == eDetPlus;
        isActiveIntCtrl = true;
    }
}

//*****************************************************************************
// Проверка состояния модуля контроля целостности обмоток
bool IntegrCtrl_isOn( void )
{
    return stepCnt != eSwitchedOff;
}

//*****************************************************************************
// Получить значение состояния рабочих цепей
MotorFailure IntegrCtrl_getFailure( void )
{
    return failure;
}

//*****************************************************************************
// Отключение модуля контроля целостности обмоток (на минимуме сигнала)
void IntegrCtrl_turnOff( void )
{
    if ( stepCnt != eCalcUfbPhase )
        stepCnt = eDecLevel;
    if( level == MIN_LEVEL_INT_CTRL )
    {
        ctrl = false;
        stepCnt = eSwitchedOff;
        IntegrCtrlGen_turnOff( );
    }
}

//*****************************************************************************
/**
* История изменений: 
*
* Версия 1.0.1
* Дата   05-02-2018
* Автор  Кругликов В.П.
*
* Изменения:
*    Базовая редакция.
*
* Версия 1.0.2
* Дата   23-04-2020
* Автор  Кругликов В.П.
* 
* Изменения:
*   Разделены временные шаги приращения сигнала для рабочей нагрузки и для стенда
*   всвязи с ложной подработкой КЗ (для рабочей нагрузки время требуется дольше, составляет 20мс, 
*   для стенда 10 мс)
*/
