/**
* \file    Main.c
* \brief   \copybrief Main.h
*
* \version 2.0.5
* \date    14-05-2020
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <stdint.h>
#include "MainRegisters.h"  
#include "asserts_ex.h"
#include "Interchannel.h"
#include "AnalogMeasurement.h"
#include "Tracing.h"
#include "ConfigurationMK.h"
#include "Main.h"
#include "LedFailure.h"
#include "Rs422.h"
#include "ConfigMK.h"
#include "typeMK.h"
#include "SafetyPowerControl.h"
#include "Sheduler.h"
#include "DeviceAddress.h"
#include "BinIn.h"
#include "checkCauseReset.h"
#include "EcanDriver.h"
#include "CheckSupply.h"
#include "Eeprom.h"
#include "ModeProtection.h"
#include "addressVarEeprom.h"
#include "TimeBeginSynchronization.h"
#include "DebugTools.h"
#include "defCompil.h"
#include "Indication.h"
#include "wait.h"
#include "OverloadDet.h"
#include "FlashCheck.h"
#include "IdentHex.h"
#include "ControlMK.h"
#include "ManageCanDriver.h"
#include "MessageVersion.h"

//*****************************************************************************
// Объявление локальных переменных
//*****************************************************************************

//*****************************************************************************
Rs422_handler             *rs422;                  ///< Указатель на переменную состояния модуля Rs422.h.
BlockExch_Str             *blockExchange;          ///< Указатель на переменную состояния модуля BlockExch.h.
ActivityManager_DeviceStr *activityManager;        ///< Указатель на переменную состояния модуля ActivityManager.h.

//*****************************************************************************
indOutput_type            ledRs1,                  ///< Индикатор RS1.
                          ledRs2,                  ///< Индикатор RS2.
                          ledPlusPos,              ///< Индикатор "Положение плюс".
                          ledMinusPos,             ///< Индикатор "Положение минус".
                          ledLossCtrl;             ///< Индикатор "Потеря контроля".

//*****************************************************************************
volatile uint16_t cInterrMainFor;       ///< Счетчик прерываний синхронизации.
volatile uint16_t cErrorCheckPhase;     ///< Счетчик отказов по уровню временной синхронизации.
static volatile uint16_t cSec;          ///< Время работы (не более #MAX_TIME_FIX_SEC), с.
static volatile uint16_t cMs;           ///< Время работы (не более 1000), в мс.

//*****************************************************************************
uint16_t regDSWPAG;          ///< Состояние регистра \a DSWPAG.
uint16_t regDSRPAG;          ///< Состояние регистра \a DSRPAG.

//*****************************************************************************
// Объявление локальных типизированных констант
//*****************************************************************************

//*****************************************************************************
/// \brief Количество прерываний синхронизации, которые отводятся на одну итерацию главного цикла.
///
const uint16_t MAIN_NUMBER_OF_INTERRUPT = 16;

//*****************************************************************************
/// \brief Максимально фиксируемое время работы 32768 (~9 часов). 
///
static const uint16_t MAX_TIME_FIX_SEC = 0x8000;


//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Настройка тактового генератора и частоты ядра процессора.
/// \note 1. Определяются следующие частоты:
///     - Fin  = 14.7456 МГц - частота внешнего кварцевого генератора;
///     - Fref = 3.6864 МГц;
///     - Fsys = 235.9296 МГц;
///     - Fosc = 117.9648 МГц;
///     - Fcy  = 58.9824 МГц (Tcy = 16.954 нс).
/// \note 2. Настраивается регистры: предделитель PLLPRE (N1 = 4), постделитель PLLPOST (N2 = 2) и делитель в цепи обратной связь PLLDIV (M = 64).
/// \note 3. Инициируется переключение главного генератора в режим с PLL.
/// \note 4. Ожидание переключения главного генератора в режим EC с PLL.
///
static void ClockConfig( void );

//*****************************************************************************
/// \brief Подсчет времени работы.
///
static void cTimeWork( void );


//*****************************************************************************
/// \brief Синхронизация по времени.
/// \param num - номер ожидаемого прерывания.
/// \note Ожидает указанный номер прерывания, после чего возвращает 
/// управление #waitAndCheckInterrupt.
///
static void waitInterrupt( uint16_t num );

//*****************************************************************************
/// \brief Синхронизация по времени с контролем.
/// \param num - номер ожидаемого прерывания.
/// \note Ожидает указанный номер прерывания, после чего возвращает 
/// управление.
/// \note Если на момент вызова функции номер прерывания 
/// превысил ожидаемый, то происходит переход в состояние ЗС
/// с кодом #ePS_IntChIdError группы #eGrPS_InterChannel.
///
static void waitAndCheckInterrupt( uint16_t num );


//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Получить время работы внутри секунды, мс
uint16_t Main_getTimeWorkMs( void )
{
    return cMs;
}

//*****************************************************************************
// Получить время работы, с
uint16_t Main_getTimeWorkSec( void )
{
    return cSec;
}

//*****************************************************************************
//Получить количество выполненных прерываний (период 62,5 мкс) из основного цикла.
uint16_t Main_getTimeWorkInterrupt( void )
{
    return cInterrMainFor;
}

//*****************************************************************************
// Получить длительность основного цикла, мкс.
uint16_t Main_getPeriodCycle()
{
    return PERIOD_MAIN_CYCLE;
}

//*****************************************************************************
// Получить текущее время работы главного цикла, мкс.
uint16_t Main_getCurrentTimeCycle()
{
    uint16_t data1, data2;
    // количество прерываний
    data1 = __builtin_divud( __builtin_muluu( cInterrMainFor, TIME_PERIOD_INTERRUPT_NS ), 1000 );
    // время в прерывании
    data2 = __builtin_divud( __builtin_muluu( MAIN_TIMER, TIME_PERIOD_INTERRUPT_NS / 1000 ), MAIN_TIMER_LOAD_PR5 );

    return data1 + data2;; 
}

//*****************************************************************************
// Реализация локальных функций

//*****************************************************************************

//*****************************************************************************
// Синхронизация по времени
static void waitInterrupt( uint16_t num )
{
    while( cInterrMainFor < num )
    {

    }
}

//*****************************************************************************
// Синхронизация по времени с контролем
static void waitAndCheckInterrupt( uint16_t num )
{
    ASSERT_ID( eGrPS_Main, ePS_MainCheckInterrupt,
               cInterrMainFor < num );

    waitInterrupt( num );
}


// Подсчет времени работы
static void cTimeWork( void )
{
    cMs++;
    if( cMs >= 1000 )
    {
        cMs = 0;
        if( cSec < MAX_TIME_FIX_SEC ) cSec++;
    }
} 

//*****************************************************************************
// Настройка тактового генератора и частоты ядра процессора.
static void ClockConfig( void )
{
    // Конфигурация регистров прескаллера PLL, постскаллера PLL и делителя PLL
    CLKDIVbits.PLLPRE  = 2;     //  N1 = 4                  
    CLKDIVbits.PLLPOST = 0;     //  N2 = 2                  
    PLLFBDbits.PLLDIV  = 62;    //  M  = 64                

    // Инициируем переключение генератора
    __builtin_write_OSCCONH( 0x03 ); 
    __builtin_write_OSCCONL( 0x01 );          
    
    // Ожидаем переключения тактового генератора
    while( OSCCONbits.COSC != 0x03 )
        // Ожидание захвата PLL
        while( OSCCONbits.LOCK != 1 ) {};            
}

//*****************************************************************************
// Управление главным потоком
int main( void )
{
    ClockConfig();
    regDSWPAG = DSWPAG;
    regDSRPAG = DSRPAG;
    checkCauseReset_run( );
    ConfigMK_ctor( );
    Tracing_ctor( );
    
    if( ConfigMK_isMaster( ) )
    {
        InterChannel_ctor( ManageCanDriver_ctor( eEcan1, ADDRESS_CAN_CHANNEL_SLAVE,
                                                    ADDRESS_CAN_CHANNEL_MASTER,  eEcanModeNormal, 6 )  );
        Indication_ctorLed( &ledRs1, &TRISG, &LATG, 13, eCtrEnLow );
        Indication_ctorLed( &ledRs2, &TRISG, &LATG, 12, eCtrEnLow );
        Indication_ctorLed( &ledPlusPos, &TRISG, &LATG, 14, eCtrEnLow );
        Indication_ctorLed( &ledLossCtrl, &TRISB, &LATB, 11, eCtrEnLow );
        Indication_ctorLed( &ledMinusPos, &TRISF, &LATF, 7, eCtrEnLow );
    }
    else
    {
        InterChannel_ctor( ManageCanDriver_ctor( eEcan1, ADDRESS_CAN_CHANNEL_MASTER,
                                                    ADDRESS_CAN_CHANNEL_SLAVE,  eEcanModeNormal, 6 )  );
        Indication_ctorDummy( &ledRs1 );
        Indication_ctorDummy( &ledRs2 );
        Indication_ctorDummy( &ledPlusPos );
        Indication_ctorDummy( &ledLossCtrl );
        Indication_ctorDummy( &ledMinusPos );
    }
    Eeprom_ctor( ); // 1 раз для обеспечения возможности записи кодов отказов 
    DeviceAddress_ctor( );
    ModeProtection_ClearPS();  //сброс ЗС, если оно установлено 
    IdentHex_run( );
    DebugTools_ctor( );
#ifndef IGNORE_CHECK_PROTECTION_STATE
    // Анализ наличия кода ЗС в EEPROM
    uint16_t firstCell = Eeprom_read( ADDRESS_EEPROM_PROTECTION_CODE_1 ); 
    uint16_t secondCell = Eeprom_read( ADDRESS_EEPROM_PROTECTION_CODE_2 );
    uint16_t thirdCell = Eeprom_read( ADDRESS_EEPROM_PROTECTION_CODE_3 );
    if( ( firstCell != PROTECTION_CODE_ABSENCE ) ||
        ( secondCell != PROTECTION_CODE_ABSENCE ) ||
        ( thirdCell != PROTECTION_CODE_ABSENCE ) )
    {
    #ifdef ENABLE_DEBUG_SPI
        DebugTools_trWordSPI( firstCell );
        DebugTools_trWordSPI( secondCell );
        DebugTools_trWordSPI( thirdCell );
    #endif
    #ifdef DEBUG_BLACK_BOX_READ_INIC 
        BlackBox_read( );
    #endif 
        ModeProtectiont_transmitToWait( 0 );
    }
#endif    
    Eeprom_ctor( ); // 2 раз для инициализации процедуры синхронизации
    ModeProtection_ctor( );
    BinIn_ctor( );
    CheckSupply_ctor( );
    MessageVersion_ctor();
    SafetyPowerControl_ctor( );
    LedFailure_ctor( );
    AnalogMeasurement_ctor( ); 
    Sheduler_ctor( );
    ENABLE_WATCHDOG; // Разрешение Watchdog-таймера
    TimeBeginSynchronization_run( );
    ControlMK_ctor( );
    CAN_TIMER_INIT;
    MAIN_TIMER_INIT_AND_START;
    INTERRUPT_INIT;
   
    while( true )
    {
        cInterrMainFor = 0;
        ClrWdt( );
        Sheduler_run( );
        cTimeWork( );
        waitAndCheckInterrupt( MAIN_NUMBER_OF_INTERRUPT );
    }
    return 0;
}



//*****************************************************************************
/**
* История изменений:
*  
* Версия 1.0.1
* Дата   19-01-2016
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Базовая версия.
* 
* Версия 2.0.1
* Дата   19-01-2017
* Автор  Кругликов В.П.
* 
* Изменения:
*    Изменения под новую схему на dsPIC33.
*
* Версия 2.0.2
* Дата   28-09-2018
* Автор  Годунок А.Н.
* 
* Изменения:
*     Из-за изменений в МКО в функцию CN_FUNCTION_INTERRUPT 
* добавлен вызов функции interChannel_runDrv.
* 
* Версия 2.0.3
* Дата   08-07-2019
* Автор  Годунок А.Н.
* 
* Изменения:
*    Для поддержки изменений версии 1.0.11 модуля InterChannel.с внесены следующие изменения:
*    - добавлено макроопределение INTER_CH_RESTRICTION_ON_TRANSFER;
*    - в функции CAN_TIMER_FUNCTION_INTERRUPT при вызове функции InterChannel_runCommunication
* формируется признак разрешения передачи, если до начала следующего цикла главного потока 
* осталось времени больше чем значение INTER_CH_RESTRICTION_ON_TRANSFER;
*    - удалена передача параметра eInterChannelTrPhaseEven в функцию InterChannel_ctor.
*  
* Версия 2.0.4
* Дата   24-10-2019
* Автор  Кругликов В.П.
* 
* Изменения:
*   Изменена настройка системы тактирования микроконтроллера
* 
* Версия 2.0.5
* Дата 14-05-2020 
* Автор Кругликов В.П.
* 
* Изменения:
*   Обработчики прерываний удалены из Main.c и вынесены в InterruptsHandlers.c  
*/
