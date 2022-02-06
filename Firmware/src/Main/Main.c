/**
* \file    Main.c
* \brief   \copybrief Main.h
*
* \version 2.0.5
* \date    14-05-2020
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
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
// ���������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
Rs422_handler             *rs422;                  ///< ��������� �� ���������� ��������� ������ Rs422.h.
BlockExch_Str             *blockExchange;          ///< ��������� �� ���������� ��������� ������ BlockExch.h.
ActivityManager_DeviceStr *activityManager;        ///< ��������� �� ���������� ��������� ������ ActivityManager.h.

//*****************************************************************************
indOutput_type            ledRs1,                  ///< ��������� RS1.
                          ledRs2,                  ///< ��������� RS2.
                          ledPlusPos,              ///< ��������� "��������� ����".
                          ledMinusPos,             ///< ��������� "��������� �����".
                          ledLossCtrl;             ///< ��������� "������ ��������".

//*****************************************************************************
volatile uint16_t cInterrMainFor;       ///< ������� ���������� �������������.
volatile uint16_t cErrorCheckPhase;     ///< ������� ������� �� ������ ��������� �������������.
static volatile uint16_t cSec;          ///< ����� ������ (�� ����� #MAX_TIME_FIX_SEC), �.
static volatile uint16_t cMs;           ///< ����� ������ (�� ����� 1000), � ��.

//*****************************************************************************
uint16_t regDSWPAG;          ///< ��������� �������� \a DSWPAG.
uint16_t regDSRPAG;          ///< ��������� �������� \a DSRPAG.

//*****************************************************************************
// ���������� ��������� �������������� ��������
//*****************************************************************************

//*****************************************************************************
/// \brief ���������� ���������� �������������, ������� ��������� �� ���� �������� �������� �����.
///
const uint16_t MAIN_NUMBER_OF_INTERRUPT = 16;

//*****************************************************************************
/// \brief ����������� ����������� ����� ������ 32768 (~9 �����). 
///
static const uint16_t MAX_TIME_FIX_SEC = 0x8000;


//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ��������� ���������� � ������� ���� ����������.
/// \note 1. ������������ ��������� �������:
///     - Fin  = 14.7456 ��� - ������� �������� ���������� ����������;
///     - Fref = 3.6864 ���;
///     - Fsys = 235.9296 ���;
///     - Fosc = 117.9648 ���;
///     - Fcy  = 58.9824 ��� (Tcy = 16.954 ��).
/// \note 2. ������������� ��������: ������������ PLLPRE (N1 = 4), ������������ PLLPOST (N2 = 2) � �������� � ���� �������� ����� PLLDIV (M = 64).
/// \note 3. ������������ ������������ �������� ���������� � ����� � PLL.
/// \note 4. �������� ������������ �������� ���������� � ����� EC � PLL.
///
static void ClockConfig( void );

//*****************************************************************************
/// \brief ������� ������� ������.
///
static void cTimeWork( void );


//*****************************************************************************
/// \brief ������������� �� �������.
/// \param num - ����� ���������� ����������.
/// \note ������� ��������� ����� ����������, ����� ���� ���������� 
/// ���������� #waitAndCheckInterrupt.
///
static void waitInterrupt( uint16_t num );

//*****************************************************************************
/// \brief ������������� �� ������� � ���������.
/// \param num - ����� ���������� ����������.
/// \note ������� ��������� ����� ����������, ����� ���� ���������� 
/// ����������.
/// \note ���� �� ������ ������ ������� ����� ���������� 
/// �������� ���������, �� ���������� ������� � ��������� ��
/// � ����� #ePS_IntChIdError ������ #eGrPS_InterChannel.
///
static void waitAndCheckInterrupt( uint16_t num );


//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �������� ����� ������ ������ �������, ��
uint16_t Main_getTimeWorkMs( void )
{
    return cMs;
}

//*****************************************************************************
// �������� ����� ������, �
uint16_t Main_getTimeWorkSec( void )
{
    return cSec;
}

//*****************************************************************************
//�������� ���������� ����������� ���������� (������ 62,5 ���) �� ��������� �����.
uint16_t Main_getTimeWorkInterrupt( void )
{
    return cInterrMainFor;
}

//*****************************************************************************
// �������� ������������ ��������� �����, ���.
uint16_t Main_getPeriodCycle()
{
    return PERIOD_MAIN_CYCLE;
}

//*****************************************************************************
// �������� ������� ����� ������ �������� �����, ���.
uint16_t Main_getCurrentTimeCycle()
{
    uint16_t data1, data2;
    // ���������� ����������
    data1 = __builtin_divud( __builtin_muluu( cInterrMainFor, TIME_PERIOD_INTERRUPT_NS ), 1000 );
    // ����� � ����������
    data2 = __builtin_divud( __builtin_muluu( MAIN_TIMER, TIME_PERIOD_INTERRUPT_NS / 1000 ), MAIN_TIMER_LOAD_PR5 );

    return data1 + data2;; 
}

//*****************************************************************************
// ���������� ��������� �������

//*****************************************************************************

//*****************************************************************************
// ������������� �� �������
static void waitInterrupt( uint16_t num )
{
    while( cInterrMainFor < num )
    {

    }
}

//*****************************************************************************
// ������������� �� ������� � ���������
static void waitAndCheckInterrupt( uint16_t num )
{
    ASSERT_ID( eGrPS_Main, ePS_MainCheckInterrupt,
               cInterrMainFor < num );

    waitInterrupt( num );
}


// ������� ������� ������
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
// ��������� ��������� ���������� � ������� ���� ����������.
static void ClockConfig( void )
{
    // ������������ ��������� ����������� PLL, ������������ PLL � �������� PLL
    CLKDIVbits.PLLPRE  = 2;     //  N1 = 4                  
    CLKDIVbits.PLLPOST = 0;     //  N2 = 2                  
    PLLFBDbits.PLLDIV  = 62;    //  M  = 64                

    // ���������� ������������ ����������
    __builtin_write_OSCCONH( 0x03 ); 
    __builtin_write_OSCCONL( 0x01 );          
    
    // ������� ������������ ��������� ����������
    while( OSCCONbits.COSC != 0x03 )
        // �������� ������� PLL
        while( OSCCONbits.LOCK != 1 ) {};            
}

//*****************************************************************************
// ���������� ������� �������
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
    Eeprom_ctor( ); // 1 ��� ��� ����������� ����������� ������ ����� ������� 
    DeviceAddress_ctor( );
    ModeProtection_ClearPS();  //����� ��, ���� ��� ����������� 
    IdentHex_run( );
    DebugTools_ctor( );
#ifndef IGNORE_CHECK_PROTECTION_STATE
    // ������ ������� ���� �� � EEPROM
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
    Eeprom_ctor( ); // 2 ��� ��� ������������� ��������� �������������
    ModeProtection_ctor( );
    BinIn_ctor( );
    CheckSupply_ctor( );
    MessageVersion_ctor();
    SafetyPowerControl_ctor( );
    LedFailure_ctor( );
    AnalogMeasurement_ctor( ); 
    Sheduler_ctor( );
    ENABLE_WATCHDOG; // ���������� Watchdog-�������
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
* ������� ���������:
*  
* ������ 1.0.1
* ����   19-01-2016
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
* 
* ������ 2.0.1
* ����   19-01-2017
* �����  ��������� �.�.
* 
* ���������:
*    ��������� ��� ����� ����� �� dsPIC33.
*
* ������ 2.0.2
* ����   28-09-2018
* �����  ������� �.�.
* 
* ���������:
*     ��-�� ��������� � ��� � ������� CN_FUNCTION_INTERRUPT 
* �������� ����� ������� interChannel_runDrv.
* 
* ������ 2.0.3
* ����   08-07-2019
* �����  ������� �.�.
* 
* ���������:
*    ��� ��������� ��������� ������ 1.0.11 ������ InterChannel.� ������� ��������� ���������:
*    - ��������� ���������������� INTER_CH_RESTRICTION_ON_TRANSFER;
*    - � ������� CAN_TIMER_FUNCTION_INTERRUPT ��� ������ ������� InterChannel_runCommunication
* ����������� ������� ���������� ��������, ���� �� ������ ���������� ����� �������� ������ 
* �������� ������� ������ ��� �������� INTER_CH_RESTRICTION_ON_TRANSFER;
*    - ������� �������� ��������� eInterChannelTrPhaseEven � ������� InterChannel_ctor.
*  
* ������ 2.0.4
* ����   24-10-2019
* �����  ��������� �.�.
* 
* ���������:
*   �������� ��������� ������� ������������ ����������������
* 
* ������ 2.0.5
* ���� 14-05-2020 
* ����� ��������� �.�.
* 
* ���������:
*   ����������� ���������� ������� �� Main.c � �������� � InterruptsHandlers.c  
*/
