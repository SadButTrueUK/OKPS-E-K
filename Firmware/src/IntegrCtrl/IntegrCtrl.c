/**
* \file    IntegrCtrl.c
* \brief   \copybrief IntegrCtrl.h
*
* \version 1.0.6
* \date    13-07-2021
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
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
#include <string.h>
#include "CheckCallFunctions.h"
#include "shuntShiftMotor.h"


//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
// ����������� ����������
// 1. ��� 1� - ���������� 0.2592 � 
// 2. ��� 0� - ���������� ����������� �������� Udc = Uref/2 = 1.5 �
// 3. 1 �������� ��� = 3 �/(2^12)
// 4. 1 �������� ��� = 2.826 ��
// 5. ������������ �������� ��������� ���� (������������������ ��������)  Iav = (Uref - Udc)/0.2592/sqrt(2) = 4.092 �


#define TIME_PAUSE_PHASE_TO_PHASE  400U         ///< ����� � �� ����� ������� ���������� � �������� ����� �������.

//*****************************************************************************
/// \brief ����� � �� ����� ������� ���������� ������� ������������ �����������.
///
#define PAUSE_BEFORE_NEXT_CHECK    400U

//*****************************************************************************
#define START_TIMEOUT              5000U        ///< ��������� ����-��� � ����� ����� ����� ������ ��.

//*****************************************************************************
/// \brief �������� ������ ����������, �� �������� ���������� �������� ����������� ������� � ������� �� (��� ���������)
///
#define LEVEL_THR_SC_MOTOR         26U 

//*****************************************************************************
/// \brief �������� ������ ����������, �� �������� ���������� �������� ����������� ������� � ������� �� (��� ����������� �������)
///
#define LEVEL_THR_SC_STAND         30U

//*****************************************************************************
/// \brief �������� ���� � ��*10, ������� ��������� ����������� ��� ����������� ��������.
///
#define CURR_INTEGR_WORK_MA_x10    5060U    

//*****************************************************************************
#define CURR_INTEGR_STAND_MA_x10   2200U    ///< �������� ���� � ��, ������� ��������� ����������� ��� ������ �� ������.

//*****************************************************************************
/// \brief ����������� �������������� �� � ������� ��� (������� �� 10) 1 �������� - 2.826 ��.
///
#define  CURR_RATIO                29U         

//*****************************************************************************
/// \brief �������� ������������ �������� ���� � ��������� ���.
///                   
#define CURR_INTEGR_WORK_ADC   ( CURR_INTEGR_WORK_MA_x10 / CURR_RATIO ) 

//***************************************************************************** 
/// \brief �������� ������������ ���� �� ������ � ��������� ���. 
///  
#define CURR_INTEGR_STAND_ADC  ( CURR_INTEGR_STAND_MA_x10 / CURR_RATIO )

//*****************************************************************************
/// \brief ����� �� ���� � ��������� ���� ���������� ���������� �������.
///                
#define THR_CURR_RISE_ZONE     ( CURR_INTEGR_WORK_ADC / 2 )

//*****************************************************************************
/// \brief ������������ ������� �������, �� �������� ������������ �����.
///
#define MAX_LEVEL              ( 18U * MULTIPLY_VOLTAGE_STEP_INT_CTRL )

//*****************************************************************************
#define MIN_LEVEL_INT_CTRL       0               ///< ����������� ������� �������.

//*****************************************************************************
/// \brief ������� ����������, ��� ������� ����������� ��������� ������������ ��������.
///
#define LEVEL_TESTING_CORRECTOR  180U   

//*****************************************************************************
/// \brief ����� ���������� ��� (�� �������� �����) ��� ������� ���������� ������������ ��������.
///
#define THR_U_CORRECTOR_NORMAL   3005U 

//*****************************************************************************
/// \brief �������� ������� ������� ���������� ��� ��� �������� ���, ���������.
///
#define NUM_PERCENTS_LOW        10U

//*****************************************************************************
/// \brief �������� �������� ������� ���������� ��� ��� �������� ���, ���������.
///
#define NUM_PERCENTS_HIGH       5U

//*****************************************************************************
/// \brief ������ ���������� ���������� �������� ���������� ��� �������� ���.
///
#define THR_U_CORRECTOR_IS_FAIL_LOW  ( THR_U_CORRECTOR_NORMAL - ( THR_U_CORRECTOR_NORMAL * NUM_PERCENTS_LOW / 100 ) )

//*****************************************************************************
/// \brief ������� ����������� ���������� �������� ���������� ��� �������� ���.
///
#define THR_U_CORRECTOR_IS_FAIL_HIGH ( THR_U_CORRECTOR_NORMAL + ( THR_U_CORRECTOR_NORMAL * NUM_PERCENTS_HIGH / 100 ) )

//*****************************************************************************
/// \brief ��� ���������� ������� �� ������� � �� ��� ������������ ���������� ������������ ��������.
/// 
#define STEP_INCR_TESTING_CORRECTOR   1U  

//*****************************************************************************
/// \brief ��� ������� � �� ���������� ������� ��� ������ (���� 1).
///
#define STEP_INCR_VALUE_RISE_STAND   10U  

//*****************************************************************************
/// \brief ��� ������� � �� ���������� ������� ��� ������� �������� (���� 1).
///
#define STEP_INCR_VALUE_RISE_WORK    20U

//*****************************************************************************    
/// \brief ��� ������� � �� ���������� ������� ��� ������(���� 2).
///                               
#define STEP_INCR_VALUE_MIDDLE_STAND 10U

//*****************************************************************************    
/// \brief ��� ������� � �� ���������� ������� ��� ������� ��������(���� 2).
///       
#define STEP_INCR_VALUE_MIDDLE_WORK  20U

//*****************************************************************************
#define STEP_DECR_VALUE        1U               ///< ��� ������� � �� ���������� �������.

//*****************************************************************************
/// \brief ���������� ����������� ��� ��� 2-� �������� 3-� ��� (���, ���). 
///
#define NUM_OF_INTERLEAVING    2U


//*****************************************************************************
/// \brief ������� ���������� ���� ��� (RMS), ��������� �� ������� ��� ������������ ������� ���� 
///
#define THR_OF_UNSAFE_VOLTAGE_RMS    90


//*****************************************************************************
/// \brief ������� ���������� ���� ��� (����������), ��������� �� ������� ��� ������������ ������� ���� 
///
#define THR_OF_UNSAFE_VOLTAGE_VAL    90

//*****************************************************************************
/// \brief ���������� �������������� ���, ���������� ���������� �� ������� �������� � �������� ������ 
///
#define NUM_UNSAFE_PHASE_VOLTAGES     2


//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief  �������� �������� ��������� ������� #IntegrCtrl_run.
/// 
typedef enum
{
    eSwitchedOff,                     ///< ��������
    
    /// \brief ��������� ����-��� ���� ����-��� ����� ��������� ��������� �������� (�� ��� �����)
    ///
    eStartTimeout,
    eIncLevel,                        ///< ���������� �������� ������ �� �������� � ���������� ��������� ��������
    eDecLevel,                        ///< ���������� �������� �� ��������
    ePauseToOtherPhase,               ///< ����� ��� �������� � ������ ����
    eWaitingForPeriod,                ///< �������� ������� �� ��������� ������� ��������
    eSetPauseToOtherPhase,            ///< ����-��� ����� ��������� � ������������ ��������� ����
    
    /// \brief ���������� �������� ���������� �������� ����� ��� ������������ ���������� ������������ ��������
    ///
    eCalcUfbPhase
} IntDetCtrlStates;

//*****************************************************************************
/// \brief �������� ������� ���� � ����������.
///
typedef struct
{
    uint16_t currentPhase;        ///< ���������� ��� ���� (���������� ��������).
    uint16_t levelPhase;          ///< ���������� ���������� ���� (���������� ��������).
} strStatusPhases;

//*****************************************************************************
// ����������� ��������� �������������� ��������
//*****************************************************************************

//*****************************************************************************
/// \brief ������ �������� ���� ��� ����������� �������� � ��� ������.
/// 
const uint16_t currThrWork[] =
{
    CURR_INTEGR_WORK_ADC,
    CURR_INTEGR_STAND_ADC
};

//*****************************************************************************
/// \brief ���� ���������� ������� �� ������� ��� ����������� �������� � ��� ������ (���� 1)
/// 
const uint8_t stepsIncr[] =
{
    STEP_INCR_VALUE_RISE_WORK, 
    STEP_INCR_VALUE_RISE_STAND  
};

//*****************************************************************************
/// \brief ���� ���������� ������� �� ������� ��� ����������� �������� � ��� ������ (���� 2)
/// 
const uint8_t stepsMiddle[] =
{
    STEP_INCR_VALUE_MIDDLE_WORK,
    STEP_INCR_VALUE_MIDDLE_STAND
};

//*****************************************************************************
/// \brief ����� �������� �������� ���� (��� ���������� ����������� ��� � �������� �������� ������������).
///
const uint8_t maskSeqPh[NUM_OF_INTERLEAVING][NUM_PHASES] =
{
    { ePhaseV, ePhaseW, ePhaseU },
    { ePhaseV, ePhaseU, ePhaseW }
};

//*****************************************************************************
/// \brief ������� ���� ���, �� ������� ����� ����������� ������� ���������� ��� ������������ ������� ����
///
const uint8_t otherPhIndex[NUM_PHASES][NUM_UNSAFE_PHASE_VOLTAGES] =
{
    { eAinchUW, eAinchUU },
    { eAinchUV, eAinchUU },
    { eAinchUV, eAinchUW }
};

//*****************************************************************************
/// \brief ������� �������� ��������� ������� ��� ���� ���, �� ������� ����� ����������� ������� 
/// ���������� ��� ������������ ������� ����
///
const uint8_t maskOtherPhForStruct[NUM_PHASES][NUM_UNSAFE_PHASE_VOLTAGES] =
{
    {ePhaseU, ePhaseW},
    {ePhaseV, ePhaseW},
    {ePhaseV, ePhaseU}
};

uint8_t statePhaseForAlarm[NUM_PHASES];

//*****************************************************************************
// ����������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static uint16_t level;          ///< ������� ���������� ��������� ������������ ����������� ������� � ���������.
static uint8_t  stepCnt;        ///< �������� �������� ��������� \a IntDetGen.

//*****************************************************************************
static bool stand;              ///< ������� ���� 3-������� �������.
static bool ctrl;               ///< ����������/������ ������.
static bool isActiveIntCtrl;    ///< ����, �� �������� ����������� ��������� ������� �� ��������� ����� ���� ���

//*****************************************************************************  
static Phase phase = ePhaseV;                         ///< ������� ���� �������.
static strStatusPhases strPhase[NUM_PHASES];          ///< ������ �������� ���� � ���������� ��� ������ ����.

//*****************************************************************************
/// \brief ��������� ���� (�����, ����� ����� ����, ����� ����� ����� ����, ��).
///
static MotorFailure failure = eNorm;

//*****************************************************************************
static uint8_t  sumBreak;               ///< ������� ������� ���.
static uint8_t  sumShortCircuit;        ///< ������� �� ���.
static bool     dir;                    ///< ��� ��������� ����������� ��� �� �������� ���, ���.
static bool     isSynchronizeScAndBreak = false;
//*****************************************************************************
/// \brief ������ �������� ��� ������ � ���������� �������� �� �������� ���
///
//static AnalogMeasurementAverage analogMeasAverage[NUM_PHASES];  

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ������ �������� ����������� �������
void IntegrCtrl_ctor( void )
{
//    static const uint8_t SIZE_BUF_ADC_AVERAGE = 32;
//    static const uint8_t NUM_SHIFTS_BUF_ADC_AVERAGE = 5;
    
    IntegrCtrlGen_ctor( ); // ���������� ���, ���������� ������� ��� IO Ports
    ctrl = true;
    stepCnt = eSwitchedOff; // ��������� ������������� �������� ��������� � ��������� ����
    phase = ePhaseV;        // ��������� ������������� ���� �������
    InterChannel_setParamSettings( eICId_IntegrCtrlScAndBreak,  
                                   eScriptSync,
                                   eProcSyncEqual,
                                   eProcCheckEqual, 0, 0,
                                   100, 200, 0 );            // ������ ��� ������� �������� � ��������������� �������������
    level = MIN_LEVEL_INT_CTRL;      // ��������� ������������� ������������ ������ ���������� ������� ������������
    sumBreak = 0;
    sumShortCircuit = 0;
    isActiveIntCtrl = false;
    for( uint8_t i = 0; i < NUM_PHASES; i++ ) //������������� �������� ��������� �������
    {    
        statePhaseForAlarm[i] = eNorm;
//        for( uint8_t j = 0; j < MIDDLE_VAL_WINDOW_SIZE; j++ )
//            analogMeasAverage[i].buff[j] = 0;                      //����� ��������        
//        analogMeasAverage[i].index = 0;                            //������ �������� �������
//        analogMeasAverage[i].averageVal = 0;                       //�������� ���������� ��������
//        analogMeasAverage[i].sizeBuf = SIZE_BUF_ADC_AVERAGE;       //���������� ������������ ������ ������
//        analogMeasAverage[i].shifts = NUM_SHIFTS_BUF_ADC_AVERAGE;  //�������� ����������� ��������
    }
}

//*****************************************************************************
// ���������� ������� ��������� �������� ����������� �������
void IntegrCtrl_run( void )
{
    // �������� ������ ��� ���������� ���������� ������������ ���������� � �������� ����-����.
    static const uint8_t NUM_MEASURES_VALUES_U_FB_PHASE = 32, TIMEOUT_BEFORE_MEASURE_U_FB = 6;
    
    static const uint8_t NUM_SHIFTS = 5;               // ���������� ������� ��� ����������.
    static const uint8_t THRESHOLDS_SC[2] = { LEVEL_THR_SC_MOTOR, LEVEL_THR_SC_STAND };  //������ ��� ����������� ���� ��
    static const uint8_t NUM_OF_SEQUENTIAL_TEST_CORR_TRIES = 3; //���� �� ��� ����� ������� ���� ��� � ����� - 
                                                                //����������� ��������� 3 �������
    static const uint8_t STEP_INC_CORR = 2;  //��� ���������� �������� ��������� ������ ���
    static const uint8_t  T_O_SYNCHRO = 100;
    
    static uint8_t time_step; // ������ ��������� ���� �� ����� ���������� ����������.
    static uint8_t stepVal;   // ��� ���������� �� ������� �� ����� ���������� ����������.
        
    //������������ ����� ������������ (����������� ������� ��� ���������� �����. ��������)
    static IntDetTypeTesting  typeTesting = eTestingIntegrCtrl;
    static uint16_t cntTime;                    // ������� ���������.
    static uint32_t buffValUfb[NUM_PHASES];     // ������ ������  ���������� ��� ������� (�������� �����)
    static uint8_t cntTestCorrFail[NUM_PHASES] = {0, 0, 0};
    static uint8_t cntTriesTestCorr;   //������� ���������� ������ ���, �� �������� ����������� ������� � ��� �������������       
    static uint8_t t_o_Synchro;
    
    
    if( cntTime ) 
        cntTime--;     //������� ����� ��������
    if( ActivityManager_isActive( activityManager ) == false )
    {
        sumShortCircuit = 0;
        sumBreak = 0;
        for( uint8_t cnt = 0; cnt < NUM_PHASES; cnt++ ) 
        {
           statePhaseForAlarm[cnt] = eNorm;
        }
        failure = eNorm;               
    }    
    if( ++t_o_Synchro == T_O_SYNCHRO ) //�������� � ������������� ������ � ��������� ������� ����� ������ � 100 ��.
    {
        if( !InterChannel_isHandling( eICId_IntegrCtrlScAndBreak ) )    
            InterChannel_synchronize( eICId_IntegrCtrlScAndBreak, failure );
        t_o_Synchro = 0;
    }
    
    switch( stepCnt )
    {
        case eSwitchedOff:              // �������� ��������, �������� ���������
            if( ctrl )
            {
                cntTime = START_TIMEOUT;    //������ ������ �������� �� ��������� ����-���� START_TIMEOUT
                stepCnt = eStartTimeout;
            }
            break;
        case eStartTimeout:
            if( cntTime == 0 )
            {
                IntegrCtrlGen_turnOn( ); // �������� ������������� �������   
                level = MIN_LEVEL_INT_CTRL;
                time_step = 0;
                phase = ePhaseV;
                cntTriesTestCorr = 0;
                for( uint8_t cnt = 0; cnt < NUM_PHASES; cnt++ ) 
                {
                    buffValUfb[cnt] = 0; 
                    statePhaseForAlarm[cnt] = 0;
                }
                sumBreak = 0;
                sumShortCircuit = 0;
                typeTesting = eTestingIntegrCtrl;
                isSynchronizeScAndBreak = false;
                stepCnt = eIncLevel;     // ������� � ���������� ���� ��������  
            }
            break;
        case eIncLevel:                 // ���������� ������ level �� ���� CURR_INTEGR_WORK_ADC
            //������ �������� ����������� ��� ���� ����, �� ������� ������������ ������������ (���������� ��������)
            time_step++;
            if( typeTesting == eTestingCorrector )
            {
                if( time_step == STEP_INCR_TESTING_CORRECTOR )  //�������� ������ ��� ����������
                {
                    if( ++level >= LEVEL_TESTING_CORRECTOR )
                    {
                        stepCnt = eCalcUfbPhase;        //������� �� ��������� ����������� ����������
                        cntTime = TIMEOUT_BEFORE_MEASURE_U_FB;
                    }
                    else
                    {
                        for( uint8_t cnt = 0; cnt < NUM_PHASES; cnt++ )
                            //���������� �� ���� ����� ���������� ����������
                            IntegrCtrlGen_setParam( ePhaseV + cnt, level, typeTesting );     
                    }
                    time_step = 0;
                }
            }
            else if( typeTesting == eTestingIntegrCtrl && ActivityManager_isActive( activityManager ) == true )
            {    
                strPhase[maskSeqPh[dir][phase]].currentPhase = InterChannel_getData( eICId_IV_value + 
                maskSeqPh[dir][phase] );
                // ������� �������� � ������ "�����������"
                if( strPhase[maskSeqPh[dir][phase]].currentPhase >= currThrWork[stand] ) 
                {
                    time_step = 0;
                    // �������� �� ��� ��
                    if( level < THRESHOLDS_SC[stand] )      //��������� ��
                    {
                        level = MIN_LEVEL_INT_CTRL;
                        IntegrCtrlGen_setParam( maskSeqPh[dir][phase], level, typeTesting );
                        stepCnt = eSetPauseToOtherPhase;
                        sumShortCircuit++;
                        statePhaseForAlarm[maskSeqPh[dir][phase]] = eShortCircuit;
                    }
                    else 
                    {
                        statePhaseForAlarm[maskSeqPh[dir][phase]] = eNorm;
                        stepCnt = eDecLevel;      // �� ���� ��� - ������� � ���������� ���� ��������    
                    }    
                }
                else
                {
                    // ��������� ���� � ��������� ���� ���������� ���������� (���� 1)
                    if( strPhase[maskSeqPh[dir][phase]].currentPhase < THR_CURR_RISE_ZONE )
                        stepVal = stepsIncr[stand];
                        // ��������� ���� � ���� ����������� � ��������� (���� 2)
                    else
                        stepVal = stepsMiddle[stand];
                    if( time_step == stepVal )
                    {
                        { 
                            uint8_t j = 0;
                            //��������� �����������, ����� ���������������� ������ ���������� ���� ��� (���������� � ���)
                            for( uint8_t cnt = 0; cnt  < NUM_PHASES; cnt ++ )
                            {
                                Tracing_parameter( AnalogMeasurement_getData( eAinchUV + cnt )->value, j++ );
                                Tracing_parameter( AnalogMeasurement_getData( eAinchUV + cnt )->rms, j++ );
                            }
                        }
                        //������������ �������� ���������� �� ����� �� ���� ������������� ��� �� �������� ������ �������
                        int16_t phVolt1RMS = AnalogMeasurement_getData( otherPhIndex[maskSeqPh[dir][phase]][0] )->rms;
                        int16_t phVolt2RMS = AnalogMeasurement_getData( otherPhIndex[maskSeqPh[dir][phase]][1] )->rms;
                        if( phVolt1RMS >= THR_OF_UNSAFE_VOLTAGE_RMS 
                            || phVolt2RMS >= THR_OF_UNSAFE_VOLTAGE_RMS )
                        {
                            //������� � �� ���� �� ���� ����� ����������� ���������� ����� ���������� ��������
                            ERROR_EX_ID( eGrPS_IntegrCtrl, ePS_SafePhaseVoltage, otherPhIndex[maskSeqPh[dir][phase]][0], 
                                    otherPhIndex[maskSeqPh[dir][phase]][1], phVolt1RMS, phVolt2RMS ); 
                        }    
                        
                        if( level < MAX_LEVEL ) 
                            level++;
                        else
                        {
                            stepCnt = eDecLevel;        //������� � ���������� ����������
                            sumBreak++;                 //���������� �������� ������ ����
                            statePhaseForAlarm[maskSeqPh[dir][phase]] = eBreakOnePhase;
                        }
                        strPhase[maskSeqPh[dir][phase]].levelPhase = level;
                        // ��������� ���� � ������ ������������� ������� �������� ����������� � �������
                        IntegrCtrlGen_setParam( maskSeqPh[dir][phase], level, typeTesting );
                        time_step = 0;
                    }
                }
            }
            break;
        case eCalcUfbPhase:
            //���������� ����������� �������� � �������� ���������� ��� (� ���������������) � �������� ������� 
            if( cntTime == 0 )    
            {    
                if( time_step++ < NUM_MEASURES_VALUES_U_FB_PHASE )   //���������� ����������
                {
                    //� ����� ������ �������� ���������� �������� ����� ���  
                    for( uint8_t cnt = 0; cnt < NUM_PHASES; cnt++ )
                        buffValUfb[cnt] += AnalogMeasurement_getData( eAinchUV + cnt )->value;
                }    
                else
                {
                    time_step = 0;
                    for( uint8_t cnt = 0; cnt  < NUM_PHASES; cnt++ )
                    {   
                        buffValUfb[cnt] >>= NUM_SHIFTS;          //���������� ����������
                    }    
                    for( uint8_t cnt = 0; cnt  < NUM_PHASES; cnt++ )
                    {
                         CASSERT_ID( eGrPS_IntegrCtrl, ePS_CorrectorIsFail,              //������ � ��� ������
                                     cntTestCorrFail[cnt],                               //������� ������
                                     NUM_OF_SEQUENTIAL_TEST_CORR_TRIES * STEP_INC_CORR,  //������������ �������� �������� ������
                                     buffValUfb[cnt] >= THR_U_CORRECTOR_IS_FAIL_LOW &&
                                     buffValUfb[cnt] <= THR_U_CORRECTOR_IS_FAIL_HIGH );  //����������� �������
                        // �������� ��������� �������� ���������� ��� (��� ���������� - ���� � ��)
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
        case eDecLevel:                 // ���������� ������ level �� �������� � ����� STEP_DECR_VALUE
            if( --level )
            {
                if( ++time_step == STEP_DECR_VALUE )
                {
                    if( typeTesting == eTestingCorrector )
                    {
                        // ��������� ���� � ������ ������������� (������������ ��� ����)
                        for( uint8_t cnt = 0; cnt  < NUM_PHASES; cnt++ )    
                            IntegrCtrlGen_setParam( ePhaseV + cnt, level, typeTesting );   
                    }
                    else if( typeTesting == eTestingIntegrCtrl )
                    {
                        // ��������� ���� � ������ �������������
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
                    // ��������� ������� ����� ����� ������� ���������� ����� ��������
                    cntTime = PAUSE_BEFORE_NEXT_CHECK;
                    stepCnt = eWaitingForPeriod; 
                }    
                else {}
            }
            break;
        case eSetPauseToOtherPhase:
            cntTime = TIME_PAUSE_PHASE_TO_PHASE;        // ��������� ������� ����� ����� ��������� �� ������ ����
            stepCnt = ePauseToOtherPhase;               // ������� � ���������� ���� ��������   
            break;
        case ePauseToOtherPhase:                        // ����� ����� ��������� �� ������ ����        
            if( cntTime == 0 )                          // ����� ����-���� ��������
            {
                // ���� �� ��� ���� ��������, ��������� ������� ����, ��������� � ������ �������� � ���������
                if( ++phase < NUM_PHASES )
                {
                    stepCnt = eIncLevel;
                }
                else                                    // ��� ���� ��������, �������� ��������� ���� ���
                {
                    phase = ePhaseV;                    // ��������� ���������� �������� ����
                    if( sumShortCircuit > 0 )           // �� ���� �� �� ����� ����
                    {
                        failure = eShortCircuit;
                        sumShortCircuit = 0;
                    }   
                    else if( sumBreak == 1 )            // ���� ���� ����� ���� �� ����� ����
                        failure = eBreakOnePhase;       // ����� ����� ����
                    else if( sumBreak == 2 || sumBreak == 3 )
                        failure = eBreakAllPhases;      // ����� ���� (�������) ��� ��� ���
                    else
                        failure = eNorm;                // ���������� ��������� ������� ���������
                    sumBreak = 0;
                    // ��������� ������� ����� ����� ������� ���������� ����� ��������
                    cntTime = PAUSE_BEFORE_NEXT_CHECK;
                    stepCnt = eWaitingForPeriod;               // ������� � ���������� ���� ��������
                     
                }
            }
            break;
        case eWaitingForPeriod:             // ����� ����� ������� ���������� ����� ��������
            if( cntTime == 0 )  
            {
                if( typeTesting == eTestingIntegrCtrl && 
                    failure != eShortCircuit && 
                    OverloadDet_getScThreePhaseGenTrig() == false ) 
                    typeTesting = eTestingCorrector;        //������� �� ������������ ���������� !!������!! ���� ��� ��
                else 
                    typeTesting = eTestingIntegrCtrl;
                isActiveIntCtrl = false;
                stepCnt = eIncLevel;                       // �� ��������� ������� ����� ��������� � ������ ��������
            }
            break;
        default:                            // ������ �������� ��������� (����������� ���������)
            ERROR_ID( eGrPS_IntegrCtrl, ePS_IntegrCtrlStepCntError );
    }
    MARKED_CALL_FUNCTION;
}

//*****************************************************************************
// ��������� ������ �������� ����������� �������
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
// �������� ��������� ������ �������� ����������� �������
bool IntegrCtrl_isOn( void )
{
    return stepCnt != eSwitchedOff;
}

//*****************************************************************************
// �������� �������� ��������� ������� �����
MotorFailure IntegrCtrl_getFailure( void )
{
    return failure;
}

//*****************************************************************************
// ���������� ������ �������� ����������� ������� (�� �������� �������)
void IntegrCtrl_turnOff( void )
{
    if ( stepCnt != eCalcUfbPhase )
        stepCnt = eDecLevel;
    if( level == MIN_LEVEL_INT_CTRL )
    {
        ctrl = false;
        stepCnt = eSwitchedOff;
        failure = eNorm;
        if( !InterChannel_isHandling( eICId_IntegrCtrlScAndBreak ) )    
        {
            if( isSynchronizeScAndBreak == false )
            {
                InterChannel_synchronize( eICId_IntegrCtrlScAndBreak, failure );
                isSynchronizeScAndBreak = true;
            }
        }
        IntegrCtrlGen_turnOff( );
    }
}

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   05-02-2018
* �����  ��������� �.�.
* 
* ���������:
*    ������� ��������.
*
* ������ 1.0.2
* ����   23-04-2020
* �����  ��������� �.�.
* 
* ���������:
*   ��������� ��������� ���� ���������� ������� ��� ������� �������� � ��� ������
*   � ����� � ������ ����������� �� (��� ������� �������� ����� ��������� ������, ���������� 20��, 
*   ��� ������ 10 ��)
*
* ������ 1.0.3
* ����   11-09-2020 
* �����  ��������� �.�.
* 
* ���������:
*   ��������� ��������� �������� ���������� �������� ������������:
*   ��� ������ ������������ �������� ������������ �� �����-���� �� ��� 
*   (�� ���� ��� ���������� ������� �� ����������� �������) ����������� ��, 
*   ��� �� ����� �� ��������� ���� ����� ����������� ����������(��� 
*   �������������������� ��������, ��� � ���������� ������������). 
*  
* ������ 1.0.4
* ���� 16-09-2020 
* �����  ��������� �.�.
* 
* ���������:
*   �������� ��������� ���������� ������ �����
*   �������, ������������ ���������� ������� �� �������� ���������� �������� ��� �������� 32 �������� 
* 
* ������ 1.0.5
* ���� 15-10-2020 
* �����  ��������� �.�.
* 
* ���������:
*   �������� ����� ��������� ������� ����� ��� ���������� �������� ����������� ������� �����
*   � ������ ����� �������� � ����� �������������
*
* ������ 1.0.6
* ���� 13-07-2021 
* �����  ��������� �.�.
* 
* ���������:
*   1. ���������� ��������� ������� ����� �������  � ����� ������������� �� �������� 100 ��;
*   2. ��������� ���������� ������� statePhaseForAlarm ��� ������������ ���������� � ��������� ������ ���� ������� ���������� ���������� 
*/
