/**
* \file    InterruptsHandlers.c
* \brief   ��������� ����������.
*
* \version 1.0.0
* \date    14-05-2020
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
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
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ���������� ���������� �� ���������� ����� �������� ������, ������
/// �������� �������� ���������.
/// \details ����������� �� �������� ������ ����� ������� ������ �����. 
/// \note ����������� ��������� ��� �������� ������ � CAN ����, ���� �� ������ ������ 
/// ����� �������� ������� ������ ��� �� �������� ���� ���������.
/// \details ����� �������� � ���������� ���������� ������ ������������� �� �������. 
/// \note ��� ��������� �������� �������� ������ ����� �������� ������ ����������
/// 115 ���. ��� ��� ������� ������������� 16��� ���������� ��� ����������.
/// ��� ������ ���������� � �� ���������� ������� ������ ������� �����������.
///
#define INTER_CH_RESTRICTION_ON_TRANSFER 4

//*****************************************************************************
// ����������� ��������� �������������� ��������
//*****************************************************************************

//*****************************************************************************
/// \brief ���������� ���������� ���������� ������������� ��� ������������ ���� �������.
///
static const uint16_t NUMBER_OF_ERROR_PHASE = ( 4 * CASSERT_INC );


//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ���������� �������� �������.
///
void MAIN_TIMER_FUNCTION_INTERRUPT( void );

//*****************************************************************************
/// \brief ��������� ���������� ������� ��� ������ � \a CAN.
///
void CAN_TIMER_FUNCTION_INTERRUPT( void );

//*****************************************************************************
/// \brief ��������� ���������� ���1.
///
void ADC1_FUNCTION_INTERRUPT( void );

//*****************************************************************************
/// \brief ��������� ���������� �� ��������� ������ �������� �������.
///
void CN_FUNCTION_INTERRUPT( void );


//*****************************************************************************
// ��������� ���������� �������� �������
void MAIN_TIMER_FUNCTION_INTERRUPT( void )
{
    MODCON = 0;
    DSWPAG = regDSWPAG;
    DSRPAG = regDSRPAG;

    if( CHECK_INTERRUPT_CN )
    { // �� ������ ���������� ������ �������������
        ERROR_ID( eGrPS_Main, ePS_MainCheckPeriodTimer );
    }

    TIME_SYNCHRO_TOGGLE_LEVEL; // ����� ���������� � �������� �� �� �����.������
    ENABLE_INTERRUPT_CN; // ���������� ���������� �� �����.������ 
    MAIN_TIMER_INTERRUPT_CLEAR_FLAG;
}

//*****************************************************************************
// ��������� ���������� �� ��������� ������ �������� �������
void CN_FUNCTION_INTERRUPT( void )
{
    MODCON = 0;
    DSWPAG = regDSWPAG;
    DSRPAG = regDSRPAG;
    MAIN_TMR_SYNCHRO;                                                           // ������������� �������� �� Master, Slave
    DISABLE_INTERRUPT_CN;                                                       // ������ ���������� �� ��������� ������ 

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
    // �������� ������ ������� ��������� �������������
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
// ��������� ���������� ������� ��� ������ � \a CAN
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
// ��������� ���������� ���1
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
* ������� ���������:
*  
* ������ 1.0.0
* ����   14-05-2020
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
* 
*/

