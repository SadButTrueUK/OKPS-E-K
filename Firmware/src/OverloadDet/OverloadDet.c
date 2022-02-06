/**
* \file    OverloadDet.c
* \brief   \copybrief OverloadDet.h
*
* \version 1.0.1
* \date    25-04-2018
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
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
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������� ����������� ����������� �� 3-������� ���������� (� ������ ������ ���� � ������� �����).
///
#define WAITING_FOR_SHUNT_SHIFT  ( 300 + TIMEOUT_TO_START_GEN + 2 )

//*****************************************************************************
/// \brief ������ ����������� ���� ����� ����������� �� ������� ����� � ��.
///
#define PERIOD_RESTART_POWER     4000

//*****************************************************************************
/// \brief ������ ����������� ���� ����� ����������� �� ������������ ���������� � ��.
///
#define PERIOD_RESTART_CONTR_GEN 5000 

//*****************************************************************************
/// \brief ����� ����������� �� ��������� ������� �� ������� ����� � ��.
///
#define HYST_ON_SC_POWER         7  

//*****************************************************************************
/// \brief ����� ����������� �� ���������� ������� �� ������� ����� � ��.
///
#define HYST_OFF_SC_POWER        7  

//*****************************************************************************
/// \brief ��������� �������� ���������� ��������� �� ������������ ����������.
///
#define THR_CNT_IMPULSE_SC_GEN   12       

//*****************************************************************************
/// \brief �������� ���������� �������� ��������� ��� ���������� 1 �� ����� ������� �� ������������ ����������.
///
#define STEP_INC                 2

//*****************************************************************************
/// \brief �������� ���������� �������� ��������� ��� ���������� 1 �� ����� ������� �� ������������ ����������.
///
#define STEP_DEC                 1

//*****************************************************************************
#define CHECK_SC_POWER_PIN       CHECKBIT( PORTG, 15 )        ///< ������ �������� ��������� ���� �� ������� �����.

//*****************************************************************************
/// \brief ������ �������� ��������� ���� �� ������������ ����������.
///
#define CHECK_SC_GEN_PIN         CHECKBIT( PORTF, 12 ) 

//*****************************************************************************
/// \brief ������ ������� ������ �������� ������� ������� ���������.
///
#define DISABLE_RESET_FAILURE    SETBIT( LATA, 10 )  

//*****************************************************************************
/// \brief ������ ���������� ������ �������� ������� ������� ���������.
///
#define ENABLE_RESET_FAILURE     CLEARBIT( LATA, 10 )         

//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief �������������� �������� ��������� ������� #procScThreePhaseGen_run ������ OverloadDet.h
///
typedef enum
{
    eScThreePhaseGenCheck,          ///< ��������� ����������� ������� �� 3-������� ����������
    eScThreePhaseGenTimeout,        ///< ��������� ������� ����-���� ����������� ��������
    
    /// \brief ��������� ������� ����-���� �� ��������� ���� ���������� ����������� (� ������ ������ � ������� ����� 
    /// ����)
    ///
    eWaitingShuntShift              
} StatesScThreePhaseGen;

//*****************************************************************************
/// \brief �������������� ��������� �������� �� ������������ ����������.
///
typedef enum
{
    eNoScContrGen,        ///< ��� �� ������������ ����������
    eIsScContrGen,        ///< ���� �� ������������ ����������
    eInProcess            ///< ��������� �� �� ����������
} CheckScContrGen;

//*****************************************************************************
/// \brief �������������� �������� ��������� ������� #procScContrGen_run ������ OverloadDet.h
///
typedef enum
{
    eScContrGenCheck,         ///< ��������� ����������� ������� ��
    eScContrGenTimeout        ///< ��������� ����-����
} StatesSCgen;

//*****************************************************************************
/// \brief ��������� ��������� ������ ����������� �� 3-������� ����������.
///
typedef struct
{
    bool     ctrl;                         ///< ��� ���������� ������ ������ ����������� �� 3-������� ����������.
    bool     isScThreePhaseGen;            ///< ��� ������� �� �� ������ ����������� �������.
    bool     ScThreePhaseGenTrig;          ///< ���������� ��� ������� ��, ������������ ������ ��� ���������� ��.
    uint16_t cntRestart;                   ///< ������ ������� �����������.
} ScThreePhaseGen;

//*****************************************************************************
/// \brief ��������� ��������� ����������� �� ������������ ����������.
///
typedef struct
{
    bool     ctrl;                      ///< ��� ���������� ������ ������ ����������� �� 3-������� ����������.
    bool     isScContrGen;              ///< ��� ������� �� �� ������ ����������� �������.
    bool     isScContrGenSynchr;        ///< ������������������ �������� ������� ��.
    bool     ScContrGenTrig;            ///< ���������� ��� ������� ��, ������������ ������ ��� ���������� ��.
    uint16_t cntRestart;                ///< ������ ������� �����������.
    uint8_t  cntImpHardFail;            ///< ������� ������ ���������� ������������� �������.
    uint8_t  cntImpWork;                ///< �������. 
} ScContrGen;

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ��������� ������ ����������� �� ������������ ����������.
///
static ScContrGen      strContrGen           = {0};

//*****************************************************************************
/// \brief ��������� ��������� ������ ����������� �� 3-������� ����������.
///
static ScThreePhaseGen strThreePhaseGen      = {0};

//*****************************************************************************
digInpHystFilt_type    checkScThreePhaseGen;               ///< ��������� �������������� ������� �������� ��.

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� �� ������������ ����������.
///
static void procScContrGen_run( void );

//*****************************************************************************
/// \brief ����������� �� � ����� 3-������� ����������.
///
static void procScThreePhaseGen_run( void );

//*****************************************************************************
/// \brief �������� �� ������������ ���������� �� �������� ��������� � �������.
/// \param counter  - ��������� �� ����������-�������, ������� ���� ������������� �� \a STEP_INC ���� 
/// ����������� �� \a STEP_DEC;
/// \param maxcount - ��������� �������� ���������� ���������;
/// \param pinSC - ��������� ������ ������� �� (���������� 0 ��� ���������� 1).
/// \return ��������� ����������� ������� �� ������������ ����������.
///
static CheckScContrGen procScContrGen_checkSC( uint8_t *counter, const uint8_t maxcount, bool pinSC );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ���������� OverloadDet
void OverloadDet_ctor( void )
{
    INIC_PORT_IN( G, 15, eIO_pullOff ); // ������������� ����� ����������� �� � ����� 3-������� ����������
    INIC_PORT_IN( F, 12, eIO_pullOff ); // ������������� ����� ����������� �� � ����� ������������ ����������
    
    // ������������� ������ ������ �������� ������������ �� �� 3-������� ����������
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
// ���������� ������� ���������� OverloadDet
void OverloadDet_run( void )
{
    procScContrGen_run( );
    procScThreePhaseGen_run( );
}

//*****************************************************************************
// �������� ������� �� � ����� ������������ ����������
bool OverloadDet_isScContrGen( void )
{
    return strContrGen.isScContrGenSynchr;
}

//*****************************************************************************
// �������� ������� �� � ����� 3-������� ����������
bool OverloadDet_isScThreePhaseGen( void )
{
    return strThreePhaseGen.isScThreePhaseGen;
}

//*****************************************************************************
// �������� ����� ��, ������� ��� � ���� ������� ��� ������ � ��
bool OverloadDet_getScThreePhaseGenTrig( void )
{
    return strThreePhaseGen.ScThreePhaseGenTrig;
}

//*****************************************************************************
// �������� ������� ������������ �������� �� � ����� ������������ ����������
bool OverloadDet_getScContrGenTrig( void )
{
    return strContrGen.ScContrGenTrig;
}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
// �������� �� ������������ ���������� �� �������� ��������� � ������� 
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
// ����������� �� ������������ ����������
static void procScContrGen_run( void )
{
    static StatesSCgen cntStep;
    static CheckScContrGen checkScContrGen;
    static uint8_t t_o = 0;

    if( !strContrGen.ctrl ) // ���� ������ ��������, ����� 
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
        if( InterChannel_isSynchronized( eICId_OverloadDetScContrGen ) ) // �������� ������������������ ������
            strContrGen.isScContrGenSynchr = InterChannel_getData( eICId_OverloadDetScContrGen );

        if( strContrGen.cntRestart ) 
            strContrGen.cntRestart--;
        switch( cntStep )
        {
            case eScContrGenCheck: // ����������� ������� ��
                // ��� ����������� �������� ��������� ��� - ���������� �������������
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
                        PositionDet_turnOn( false ); //��������� ��������� ������������ ���������� � ����������� ��������� �������
                    }
                    else if( checkScContrGen == eNoScContrGen )
                    {
                        strContrGen.ScContrGenTrig = false;

                    }
                }
                else // ���� �� � ��������, ���� ���������� �������������
                {
                    CASSERT_ID( eGrPS_OverloadDetector, ePS_OverloadDetectorHardFailGen, // ������ � ��� ������
                                strContrGen.cntImpHardFail, // ������� ������
                                THR_CNT_IMPULSE_SC_GEN, // ������������ �������� �������� ������
                                !CHECK_SC_GEN_PIN );
                }
                break;
            case eScContrGenTimeout: // ������ ����-���� �����������
                if( strContrGen.cntRestart == 0 )
                {
                    cntStep = eScContrGenCheck;
                    strContrGen.isScContrGen = false;
                    PositionDet_turnOn( true );     //�������� ��������� ������������ ���������� � ����������� ��������� �������
                }
                break;
            default: // ����������� ��������� ��������
                ERROR_ID( eGrPS_OverloadDetector, ePS_OverloadDetectorStateCntGenError );
        }
    }
    MARKED_CALL_FUNCTION;
}

//*****************************************************************************
// ����������� ��  � ����� 3-������� ����������
static void procScThreePhaseGen_run( void )
{
    static StatesScThreePhaseGen cntStep;
    static uint8_t t_o;

    if( !strThreePhaseGen.ctrl ) // ���� ����������� �� ���������, �������
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
            strThreePhaseGen.cntRestart--; // ������ ����-���� �����������  
        if( ++t_o == 100 )
        {
            if( !InterChannel_isHandling( eICId_OverloadDetScThreePhaseGen ) )
                InterChannel_synchronize( eICId_OverloadDetScThreePhaseGen, strThreePhaseGen.isScThreePhaseGen );
            t_o = 0;
        }

        switch( cntStep )
        {
            case eScThreePhaseGenCheck: // ����������� ������� ��
                strThreePhaseGen.isScThreePhaseGen = HystFltr_run( &checkScThreePhaseGen, CHECK_SC_POWER_PIN );
                if( strThreePhaseGen.isScThreePhaseGen ) // ���� �������� ������� ��
                {
                    // ��� ����������� �������� ������� � �������� ����������� ��� - ���������� �������������
                    if( !IntegrCtrl_isOn( ) && !ShuntShift_isEnable( ) )
                        ERROR_ID( eGrPS_OverloadDetector, ePS_OverloadDetectorHardwareFailurePower );
                    else // ���� �� � ��������, ���� ���������� �������������
                    {
                        strThreePhaseGen.ScThreePhaseGenTrig = true;
                        // ���������� ����-��� ��� ������ �������� ������� � ����������� �������� ��
                        strThreePhaseGen.cntRestart = PERIOD_RESTART_POWER;
                        ShuntShiftMotor_setFailure( eShortCircuit );
                        DISABLE_RESET_FAILURE; //��������� ����� ������ �� ��������� ����-���� �����������
                        cntStep = eScThreePhaseGenTimeout;
                    }
                }
                else
                {
                    strThreePhaseGen.ScThreePhaseGenTrig = false;
                }
                break;
            case eScThreePhaseGenTimeout: // ������ ����-���� �����������
                if( strThreePhaseGen.cntRestart == 0 )
                {
                    ENABLE_RESET_FAILURE; // ����-��� ����, �������� ������� ������ �� ��� ����������� ����
                    strThreePhaseGen.isScThreePhaseGen = false; // �������� ������� ��
                    checkScThreePhaseGen.inPrv = false;
                    strThreePhaseGen.cntRestart = WAITING_FOR_SHUNT_SHIFT;
                    cntStep = eWaitingShuntShift;
                }
                break;
            
            // ������ ����-���� �� ��������� ���� ���������� ����������� (� ������ ������ � ������� ����� ����)
            case eWaitingShuntShift: 
                if( strThreePhaseGen.cntRestart == 0 )
                {
                    strThreePhaseGen.cntRestart = 0;
                    cntStep = eScThreePhaseGenCheck;
                }
                break;
            default: // ����������� ��������� ��������
                ERROR_ID( eGrPS_OverloadDetector, ePS_OverloadDetectorStateCntSCError );
        }
    }
}

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   25-04-2018
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
*/
