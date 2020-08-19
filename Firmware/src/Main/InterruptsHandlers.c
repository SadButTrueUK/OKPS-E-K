/**
* \file    InterruptsHandlers.c
* \brief   Обработка прерываний.
*
* \version 1.0.0
* \date    14-05-2020
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************

#include "MainRegisters.h"
#include "BinIn.h"
#include "AnalogMeasurement.h" 
#include "SafetyPowerControl.h"
#include "Interchannel.h"
#include "ShuntShiftGen.h"
#include "Rs422.h"
#include "ConfigMK.h"
#include "posDetGenerator.h"
#include "AdcDriver_PIC33.h"
#include "asserts_ex.h"
#include "Main.h"
#include "typeMK.h"
#include "CheckRAM.h"


//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
/// \brief Количество прерываний до завершения цикла главного потока, меньше
/// которого передача запрещена.
/// \details Ограничение на передачу данных перед началом нового цикла. 
/// \note Ограничение запрещает МКО выдавать данные в CAN шину, если до начала нового 
/// цикла осталось времени меньше чем на передачу двух сообщений.
/// \details Время задается в количестве прерываний потока синхронизации по времени. 
/// \note При выбранной скорости передачи данных время передачи пакета составляет
/// 115 мкс. Что при частоте синхронизации 16кГц составляет два прерывания.
/// Два пакета передаются – на протяжении времени равным четырем прерываниям.
///
#define INTER_CH_RESTRICTION_ON_TRANSFER 4

//*****************************************************************************
// Определение локальных типизированных констант
//*****************************************************************************

//*****************************************************************************
/// \brief Количество допустимых прерываний синхронизации при неправильной фазе сигнала.
///
static const uint16_t NUMBER_OF_ERROR_PHASE = ( 4 * CASSERT_INC );


//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Обработка прерывания главного таймера.
///
void MAIN_TIMER_FUNCTION_INTERRUPT( void );

//*****************************************************************************
/// \brief Обработка прерывания таймера для работы с \a CAN.
///
void CAN_TIMER_FUNCTION_INTERRUPT( void );

//*****************************************************************************
/// \brief Обработка прерывания АЦП1.
///
void ADC1_FUNCTION_INTERRUPT( void );

//*****************************************************************************
/// \brief Обработка прерывания по изменению уровня входного сигнала.
///
void CN_FUNCTION_INTERRUPT( void );


//*****************************************************************************
// Обработка прерывания главного таймера
void MAIN_TIMER_FUNCTION_INTERRUPT( void )
{
    MODCON = 0;
    DSWPAG = regDSWPAG;
    DSRPAG = regDSRPAG;

    if( CHECK_INTERRUPT_CN )
    { // За период прерывания небыло синхронизации
        ERROR_ID( eGrPS_Main, ePS_MainCheckPeriodTimer );
    }

    TIME_SYNCHRO_TOGGLE_LEVEL; // Вызов прерывания в соседнем МК по измен.уровня
    ENABLE_INTERRUPT_CN; // Разрешение прерываний по измен.уровня 
    MAIN_TIMER_INTERRUPT_CLEAR_FLAG;
}

//*****************************************************************************
// Обработка прерывания по изменению уровня входного сигнала
void CN_FUNCTION_INTERRUPT( void )
{
    MODCON = 0;
    DSWPAG = regDSWPAG;
    DSRPAG = regDSRPAG;
    MAIN_TMR_SYNCHRO;                                                           // Синхронизация таймеров МК Master, Slave
    DISABLE_INTERRUPT_CN;                                                       // Запрет прерываний по изменению уровня 

    SafetyPowerControl_runInterrupt( );

    if( cInterrMainFor == 15 )
        BinInDataRead_interrupt( );
    if( cInterrMainFor == 1 )
        AnalogMeasurement_runInterrupt( );
    if( cInterrMainFor == 0 )
        BinInAddrSet_interrupt( );
    if( cInterrMainFor == 3 )
        ASSERT_ID( eGrPS_ControlMK, ePS_ControlMkFaultRAM, CheckRAM_run( ) );
    InterChannel_runDrv( );
    if( rs422 )
        Rs422_interrupt( rs422 );
    if( ConfigMK_isMaster( ) )
        ShuntShiftGen_interrupt( );
    else
        PosDetGenerator_interrupt( );
    if( blockExchange )
        BlockExch_interrupt( blockExchange, cInterrMainFor );
    // Контроль уровня сигнала временной синхронизации
    if( ( cInterrMainFor % 2 ) == 0 )
    {
        CASSERT_ID( eGrPS_Main, ePS_MainCheckPhase0,
                    cErrorCheckPhase, NUMBER_OF_ERROR_PHASE,
                    TIME_SYNCHRO_CHECK_LOW );
    }
    else
    {
        CASSERT_ID( eGrPS_Main, ePS_MainCheckPhase1,
                    cErrorCheckPhase, NUMBER_OF_ERROR_PHASE,
                    TIME_SYNCHRO_CHECK_HIGH );
    }
    CAN_TIMER_START;
    cInterrMainFor++;
    ASSERT_ID( eGrPS_Main, ePS_MainCheckMaxInterrupt,
               cInterrMainFor < ( MAIN_NUMBER_OF_INTERRUPT + 1 ) );
    CN_INTERRUPT_CLEAR_FLAG;
}

//*****************************************************************************
// Обработка прерывания таймера для работы с \a CAN
void CAN_TIMER_FUNCTION_INTERRUPT( void )
{
    MODCON = 0;
    DSWPAG = regDSWPAG;
    DSRPAG = regDSRPAG;

    CAN_TIMER_STOP;
    InterChannel_runCommunication( cInterrMainFor 
                                   <= (   MAIN_NUMBER_OF_INTERRUPT 
                                        - INTER_CH_RESTRICTION_ON_TRANSFER ) );

    CAN_TIMER_INTERRUPT_CLEAR_FLAG;
}

//*****************************************************************************
// Обработка прерывания АЦП1
void ADC1_FUNCTION_INTERRUPT( void )
{
    MODCON = 0;
    DSWPAG = regDSWPAG;
    DSRPAG = regDSRPAG;

    IFS0bits.AD1IF = 0;
    AdcDriver_interruptAdc1( );
}

//*****************************************************************************
/**
* История изменений:
*  
* Версия 1.0.0
* Дата   14-05-2020
* Автор  Кругликов В.П.
*
* Изменения:
*    Базовая версия.
* 
*/

