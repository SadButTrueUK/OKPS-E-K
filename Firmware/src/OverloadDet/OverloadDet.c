/**
* \file    OverloadDet.c
* \brief   \copybrief OverloadDet.h
*
* \version 1.0.1
* \date    25-04-2018
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <string.h>
#include "OverloadDet.h"
#include "HystFltr.h"
#include "typeMK.h"
#include "IOports.h"
#include "ShuntShift.h"
#include "IntegrCtrl.h"
#include "ProtectionState_codes.h"
#include "asserts_ex.h"
#include "ConfigMK.h"
#include "PositionDet.h"
#include "CheckSupply.h"
#include "InterChannel.h"
#include "shuntShiftMotor.h"
#include "shuntShiftGen.h"
#include "CheckCallFunctions.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
/// \brief Таймаут перезапуска определения КЗ 3-фазного генератора (с учётом выхода ШИМа в среднюю точку).
///
#define WAITING_FOR_SHUNT_SHIFT  ( 300 + TIMEOUT_TO_START_GEN + 2 )

//*****************************************************************************
/// \brief Период перезапуска ШИМа после определения КЗ силовой части в мс.
///
#define PERIOD_RESTART_POWER     4000

//*****************************************************************************
/// \brief Период перезапуска ШИМа после определения КЗ контрольного генератора в мс.
///
#define PERIOD_RESTART_CONTR_GEN 5000 

//*****************************************************************************
/// \brief Время гистерезиса на включение сигнала КЗ силовой части в мс.
///
#define HYST_ON_SC_POWER         7  

//*****************************************************************************
/// \brief Время гистерезиса на выключение сигнала КЗ силовой части в мс.
///
#define HYST_OFF_SC_POWER        7  

//*****************************************************************************
/// \brief Пороговое значение количества импульсов КЗ контрольного генератора.
///
#define THR_CNT_IMPULSE_SC_GEN   12       

//*****************************************************************************
/// \brief Величина приращения счётчика импульсов при логической 1 на входе датчика КЗ контрольного генератора.
///
#define STEP_INC                 2

//*****************************************************************************
/// \brief Величина уменьшения счётчика импульсов при логической 1 на входе датчика КЗ контрольного генератора.
///
#define STEP_DEC                 1

//*****************************************************************************
#define CHECK_SC_POWER_PIN       CHECKBIT( PORTG, 15 )        ///< Макрос проверки состояния пина КЗ силовой части.

//*****************************************************************************
/// \brief Макрос проверки состояния пина КЗ контрольного генератора.
///
#define CHECK_SC_GEN_PIN         CHECKBIT( PORTF, 12 ) 

//*****************************************************************************
/// \brief Макрос запрета сброса триггера защёлки силовых драйверов.
///
#define DISABLE_RESET_FAILURE    SETBIT( LATA, 10 )  

//*****************************************************************************
/// \brief Макрос разрешения сброса триггера защёлки силовых драйверов.
///
#define ENABLE_RESET_FAILURE     CLEARBIT( LATA, 10 )         

//*****************************************************************************
// Объявление типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Идентификаторы автомата состояний функции #procScThreePhaseGen_run модуля OverloadDet.h
///
typedef enum
{
    eScThreePhaseGenCheck,          ///< состояние определения наличия КЗ 3-фазного генератора
    eScThreePhaseGenTimeout,        ///< состояние отсчёта тайм-аута перезапуска контроля
    
    /// \brief состояние отсчёта тайм-аута на включение ШИМа управления двигателями (с учётом выхода в среднюю точку 
    /// ШИМа)
    ///
    eWaitingShuntShift              
} StatesScThreePhaseGen;

//*****************************************************************************
/// \brief Идентификаторы состояния контроля КЗ контрольного генератора.
///
typedef enum
{
    eNoScContrGen,        ///< нет КЗ контрольного генератора
    eIsScContrGen,        ///< есть КЗ контрольного генератора
    eInProcess            ///< состояние КЗ не определено
} CheckScContrGen;

//*****************************************************************************
/// \brief Идентификаторы автомата состояний функции #procScContrGen_run модуля OverloadDet.h
///
typedef enum
{
    eScContrGenCheck,         ///< состояние определения наличия КЗ
    eScContrGenTimeout        ///< состояние тайм-аута
} StatesSCgen;

//*****************************************************************************
/// \brief Структура состояния модуля определения КЗ 3-фазного генератора.
///
typedef struct
{
    bool     ctrl;                         ///< Бит разрешения работы модуля определения КЗ 3-фазного генератора.
    bool     isScThreePhaseGen;            ///< Бит наличия КЗ по опросу логического датчика.
    bool     ScThreePhaseGenTrig;          ///< Триггерный бит наличия КЗ, сбрасываемый только при отсутствии КЗ.
    uint16_t cntRestart;                   ///< Таймер отсчёта перезапуска.
} ScThreePhaseGen;

//*****************************************************************************
/// \brief Структура состояния определения КЗ контрольного генератора.
///
typedef struct
{
    bool     ctrl;                      ///< Бит разрешения работы модуля определения КЗ 3-фазного генератора.
    bool     isScContrGen;              ///< Бит наличия КЗ по опросу логического датчика.
    bool     isScContrGenSynchr;        ///< Синхронизированное значение наличия КЗ.
    bool     ScContrGenTrig;            ///< Триггерный бит наличия КЗ, сбрасываемый только при отсутствии КЗ.
    uint16_t cntRestart;                ///< Таймер отсчёта перезапуска.
    uint8_t  cntImpHardFail;            ///< Счётчик ошибок аппаратной неисправности датчика.
    uint8_t  cntImpWork;                ///< Счетчик. 
} ScContrGen;

//*****************************************************************************
// Объявление локальных переменных
//*****************************************************************************

//*****************************************************************************
/// \brief Структура состояния модуля определения КЗ контрольного генератора.
///
static ScContrGen      strContrGen           = {0};

//*****************************************************************************
/// \brief Структура состояния модуля определения КЗ 3-фазного генератора.
///
static ScThreePhaseGen strThreePhaseGen      = {0};

//*****************************************************************************
digInpHystFilt_type    checkScThreePhaseGen;               ///< Структура гистерезисного фильтра сигналов КЗ.

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Обработка КЗ контрольного генератора.
///
static void procScContrGen_run( void );

//*****************************************************************************
/// \brief Определение КЗ в цепях 3-фазного генератора.
///
static void procScThreePhaseGen_run( void );

//*****************************************************************************
/// \brief Проверка КЗ контрольного генератора по подсчёту импульсов с датчика.
/// \param counter  - указатель на переменную-счётчик, которая либо увеличивается на \a STEP_INC либо 
/// уменьшается на \a STEP_DEC;
/// \param maxcount - пороговое значение количества импульсов;
/// \param pinSC - состояние выхода датчика КЗ (логический 0 или логическая 1).
/// \return Результат определения наличия КЗ контрольного генератора.
///
static CheckScContrGen procScContrGen_checkSC( uint8_t *counter, const uint8_t maxcount, bool pinSC );

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация компонента OverloadDet
void OverloadDet_ctor( void )
{
    INIC_PORT_IN( G, 15, eIO_pullOff ); // инициализация входа определения КЗ в цепях 3-фазного генератора
    INIC_PORT_IN( F, 12, eIO_pullOff ); // инициализация входа определения КЗ в цепях контрольного генератора
    
    // инициализация выхода сброса триггера срабатывания по КЗ 3-фазного генератора
    INIC_PORT_OUT( A, 10, eIO_openDrainOff ); 
    HystFltr_ctor( &checkScThreePhaseGen, HYST_ON_SC_POWER, HYST_OFF_SC_POWER );
    strThreePhaseGen.ctrl = true;
    strContrGen.ctrl = true;

    InterChannel_setParamSettings( eICId_OverloadDetScContrGen,
                                   eScriptSync,
                                   eProcSyncOR,
                                   eProcCheckOff, 0, 0,
                                   50, 200, 0 );
    InterChannel_setParamSettings( eICId_OverloadDetScThreePhaseGen,
                                   eScriptSync,
                                   eProcSyncEqual,
                                   eProcCheckEqual, 0, 0,
                                   50, 200, 0 ); 
}

//*****************************************************************************
// Управление работой компонента OverloadDet
void OverloadDet_run( void )
{
    procScContrGen_run( );
    procScThreePhaseGen_run( );
}

//*****************************************************************************
// Проверка наличия КЗ в цепях контрольного генератора
bool OverloadDet_isScContrGen( void )
{
    return strContrGen.isScContrGenSynchr;
}

//*****************************************************************************
// Проверка наличия КЗ в цепях 3-фазного генератора
bool OverloadDet_isScThreePhaseGen( void )
{
    return strThreePhaseGen.isScThreePhaseGen;
}

//*****************************************************************************
// Проверка флага КЗ, который идёт в поле статуса при обмене с УС
bool OverloadDet_getScThreePhaseGenTrig( void )
{
    return strThreePhaseGen.ScThreePhaseGenTrig;
}

//*****************************************************************************
// Проверка наличия срабатывания триггера КЗ в цепях контрольного генератора
bool OverloadDet_getScContrGenTrig( void )
{
    return strContrGen.ScContrGenTrig;
}

//*****************************************************************************
// Реализация локальных функций
//*****************************************************************************

//*****************************************************************************
// Проверка КЗ контрольного генератора по подсчёту импульсов с датчика 
static CheckScContrGen procScContrGen_checkSC( uint8_t *counter, const uint8_t maxcount, bool pinSC )
{
    if( pinSC )
    {
        if( ( ( *counter ) += STEP_INC ) >= maxcount )
        {
            *counter = maxcount;
            return eIsScContrGen;
        }
        else
            return eInProcess;
    }
    else
    {
        if( *counter >= STEP_DEC )
        {
            *counter -= STEP_DEC;
            return eInProcess;
        }
    }
    return eNoScContrGen;
}

//*****************************************************************************
// Определение КЗ контрольного генератора
static void procScContrGen_run( void )
{
    static StatesSCgen cntStep;
    static CheckScContrGen checkScContrGen;
    static uint8_t t_o = 0;

    if( !strContrGen.ctrl ) // если модуль выключен, выйти 
    {
        cntStep = eScContrGenCheck;
        strContrGen.ScContrGenTrig = false;
        strContrGen.isScContrGen = false;
        strContrGen.isScContrGenSynchr = false;
        return;
    }
    else if( CheckSupply_is24vOn( ) )
    {
        if( ++t_o == 100 )
        {
            if( !InterChannel_isHandling( eICId_OverloadDetScContrGen ) )
                InterChannel_synchronize( eICId_OverloadDetScContrGen, strContrGen.isScContrGen );
            t_o = 0;
        }
        if( InterChannel_isSynchronized( eICId_OverloadDetScContrGen ) ) // Получить синхронизированные данные
            strContrGen.isScContrGenSynchr = InterChannel_getData( eICId_OverloadDetScContrGen );

        if( strContrGen.cntRestart ) 
            strContrGen.cntRestart--;
        switch( cntStep )
        {
            case eScContrGenCheck: // Определение наличия КЗ
                // При отключенном контроле положения это - аппаратная неисправность
                if( PositionDet_isEnable( ) )
                {
                    checkScContrGen = procScContrGen_checkSC( &strContrGen.cntImpWork, THR_CNT_IMPULSE_SC_GEN,
                                                              CHECK_SC_GEN_PIN );
                    if( checkScContrGen == eIsScContrGen )
                    {
                        strContrGen.isScContrGen = true;
                        strContrGen.cntRestart = PERIOD_RESTART_CONTR_GEN;
                        strContrGen.cntImpWork = 0;
                        strContrGen.ScContrGenTrig = true;
                        cntStep = eScContrGenTimeout;
                        PositionDet_turnOn( false ); //выключить генерацию контрольного напряжения и определение положения стрелки
                    }
                    else if( checkScContrGen == eNoScContrGen )
                    {
                        strContrGen.ScContrGenTrig = false;

                    }
                }
                else // либо КЗ в нагрузке, либо аппаратная неисправность
                {
                    CASSERT_ID( eGrPS_OverloadDetector, ePS_OverloadDetectorHardFailGen, // Группа и код отказа
                                strContrGen.cntImpHardFail, // Счётчик ошибок
                                THR_CNT_IMPULSE_SC_GEN, // Максимальное значение счётчика ошибок
                                !CHECK_SC_GEN_PIN );
                }
                break;
            case eScContrGenTimeout: // Отсчёт тайм-аута перезапуска
                if( strContrGen.cntRestart == 0 )
                {
                    cntStep = eScContrGenCheck;
                    strContrGen.isScContrGen = false;
                    PositionDet_turnOn( true );     //включить генерацию контрольного напряжения и определение положения стрелки
                }
                break;
            default: // Неизвестное состояние автомата
                ERROR_ID( eGrPS_OverloadDetector, ePS_OverloadDetectorStateCntGenError );
        }
    }
    MARKED_CALL_FUNCTION;
}

//*****************************************************************************
// Определение КЗ  в цепях 3-фазного генератора
static void procScThreePhaseGen_run( void )
{
    static StatesScThreePhaseGen cntStep;
    static uint8_t t_o;

    if( !strThreePhaseGen.ctrl ) // если определение КЗ отключено, выходим
    {
        cntStep = eScThreePhaseGenCheck;
        ENABLE_RESET_FAILURE;
        checkScThreePhaseGen.inPrv = false;
        strThreePhaseGen.ScThreePhaseGenTrig = false;
        return;
    }
    else if( CheckSupply_is24vOn( ) )
    {
        if( strThreePhaseGen.cntRestart ) 
            strThreePhaseGen.cntRestart--; // отсчёт тайм-аута перезапуска  
        if( ++t_o == 100 )
        {
            if( !InterChannel_isHandling( eICId_OverloadDetScThreePhaseGen ) )
                InterChannel_synchronize( eICId_OverloadDetScThreePhaseGen, strThreePhaseGen.isScThreePhaseGen );
            t_o = 0;
        }

        switch( cntStep )
        {
            case eScThreePhaseGenCheck: // Определение наличия КЗ
                strThreePhaseGen.isScThreePhaseGen = HystFltr_run( &checkScThreePhaseGen, CHECK_SC_POWER_PIN );
                if( strThreePhaseGen.isScThreePhaseGen ) // если активный уровень КЗ
                {
                    // При отключенных переводе стрелки и контроле целостности это - аппаратная неисправность
                    if( !IntegrCtrl_isOn( ) && !ShuntShift_isEnable( ) )
                        ERROR_ID( eGrPS_OverloadDetector, ePS_OverloadDetectorHardwareFailurePower );
                    else // либо КЗ в нагрузке, либо аппаратная неисправность
                    {
                        strThreePhaseGen.ScThreePhaseGenTrig = true;
                        // установить тайм-аут для сброса триггера защёлки и перезапуска проверки КЗ
                        strThreePhaseGen.cntRestart = PERIOD_RESTART_POWER;
                        ShuntShiftMotor_setFailure( eShortCircuit );
                        DISABLE_RESET_FAILURE; //запретить сброс аварии до истечения тайм-аута перезапуска
                        cntStep = eScThreePhaseGenTimeout;
                    }
                }
                else
                {
                    strThreePhaseGen.ScThreePhaseGenTrig = false;
                }
                break;
            case eScThreePhaseGenTimeout: // Отсчёт тайм-аута перезапуска
                if( strThreePhaseGen.cntRestart == 0 )
                {
                    ENABLE_RESET_FAILURE; // тайм-аут истёк, сбросить триггер аварии КЗ для перезапуска ШИМа
                    strThreePhaseGen.isScThreePhaseGen = false; // сбросить признак КЗ
                    checkScThreePhaseGen.inPrv = false;
                    strThreePhaseGen.cntRestart = WAITING_FOR_SHUNT_SHIFT;
                    cntStep = eWaitingShuntShift;
                }
                break;
            
            // отсчёт тайм-аута на включение ШИМа управления двигателями (с учётом выхода в среднюю точку ШИМа)
            case eWaitingShuntShift: 
                if( strThreePhaseGen.cntRestart == 0 )
                {
                    strThreePhaseGen.cntRestart = 0;
                    cntStep = eScThreePhaseGenCheck;
                }
                break;
            default: // Неизвестное состояние автомата
                ERROR_ID( eGrPS_OverloadDetector, ePS_OverloadDetectorStateCntSCError );
        }
    }
}

//*****************************************************************************
/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   25-04-2018
* Автор  Кругликов В.П.
* 
* Изменения:
*    Базовая версия.
*/
