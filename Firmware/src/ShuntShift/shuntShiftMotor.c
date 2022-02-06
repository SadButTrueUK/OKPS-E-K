/**
* \file    shuntShiftMotor.c
* \brief   \copybrief shuntShiftMotor.h
*
* \version 1.0.3
* \date    18-02-2021
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
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
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
// ����������� ���������� ������������ ����������, �.
#define  MCT025_127V_VOLTAGE        127U        ///< ���������� ������� ��������� ���025-127, �.
#define  MCT025_220V_VOLTAGE        220U        ///< ���������� ������� ��������� ���025-220, �.
#define  MCT03_110V_VOLTAGE         110U        ///< ���������� ������� ��������� ���03-110, �.
#define  MCT03_190V_VOLTAGE         190U        ///< ���������� ������� ��������� ���03-190, �.
#define  MCT06_110V_VOLTAGE         110U        ///< ���������� ������� ��������� ���06-110, �.
#define  MCT06_190V_VOLTAGE         190U        ///< ���������� ������� ��������� ���06-190, �.

//*****************************************************************************
#define VOLTAGE_TO_LEVEL_RATIO      0.45        ///< ����������� �������������� ���������� � % ����������.
#define MOTOR_BREAK_CURRENT         40          ///< ��� ���������� � ������ ������ �� �������, ��. ��� * 2.826 = ��.
#define STAND_BREAK_CURRENT         50          ///< ��� ���������� � ������ ������ �� ������, ��. ��� * 2.826 = ��.

//*****************************************************************************
#define ACCEL_STAGE_NUM             7                                     ///< ���������� �������� �������.
#define SHUTDOWN_TIME               10U                                   ///< ����� ����������.
#define SHUTDOWN_STAGE_NUM          10U                                   ///< ���������� ����� ����������.
#define SHUTDOWN_STAGE_TIME         SHUTDOWN_TIME / SHUTDOWN_STAGE_NUM    ///< ����� ������ ���� ����������.
#define K_RATIO_CURRENT             29U                                   ///< ����������� ��������� ���� � �������� ���.

//*****************************************************************************
#define THR_OVL_SC          40U                                         ///< �������� ���� ���������� � 0.1�.
#define THR_OVL_SC_ADC      ( THR_OVL_SC * 1000 / K_RATIO_CURRENT )     ///< �������� ���� ���������� � ��������� ���.

//*****************************************************************************
/// \brief ������� ������� ������� ���������� �������� ���� ��� �������� ��� ��� ������ �� ������� 
#define TIMEOUT_FOR_FUNC_CALC_MIDDLE_VAL   10 

#define TIMEOUT_FOR_AVERAGE_IN_WINDOW 900

//*****************************************************************************
/// \brief �������, �� ��������� �������� �������������� ������ �������� ����� ���.
///
#define TIMEOUT_MEASURE_U_FB   150

//*****************************************************************************
/// \brief �������, �� ��������� �������� ������������ ���������� �� ���� ��� ���������� ���������.
///
#define TIMEOUT_OVL_SC  1000

//*****************************************************************************
/// \brief ���������� �������� ������ ����� ���� ���������� �������� ���� ����������.
///
#define NUM_OVL_SC      800                          

#define  NUM_OF_PHASES  3    ///< ���������� ��� �������.


//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ���������.
///
typedef struct
{
    uint8_t ctrl   :1;        ///< ��������� ������. 
    uint8_t dir    :1;        ///< ����������� ��������.
    uint8_t stand  :1;        ///< ����� ������ �� ������.
    uint8_t unused :5;        ///< �������������� ����.
} ShiftMotor_flags;

//*****************************************************************************
/// \brief ��������� �������� ������� ����������.
///
typedef struct
{
    uint16_t cntOvlCur;            ///< ������� ���������� ������� ����������.
    uint16_t timeoutOvlCur;        ///< ������� ��� ����������� ������� ����������.
    bool isOvlCurr;                ///< ��� ���������� �� ����
}strOvlCurrTag;

//*****************************************************************************
/// \brief ��������� ��� ������ � ��������� ������� ��� ���������
/// ����������� �������� ����� ������ 
///
typedef struct CircularBuffer_tag 
{
    uint16_t  buff[MIDDLE_VAL_WINDOW_SIZE]; //����� ��������        
    uint8_t   index;                         //������ �������� �������
    uint16_t  averageVal;                   //�������� ���������� ��������
    uint8_t  sizeBuf;
    uint8_t  shifts;
} CircularBuffer;

//*****************************************************************************
/// \brief �������������� �������� �������� ��������� ������� #ShuntShiftMotor_run.
///
typedef enum
{
    eStartState          = 0,    ///< ����������� ���������
    eAcceleration,         ///< ������
    eRunning,              ///< ��������
    eShutdown,             ///< ����������
    eFullShutdown          ///< ������ ���������� (���������� ����������)
} ShiftMotorStates;

//*****************************************************************************
// ���������� ��������� �������������� ��������
//*****************************************************************************

//*****************************************************************************
/// \brief ������ ����������� �������, % ����������.
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
/// \brief ������ ������ ��� �������.
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
/// \brief ������ ������� ��� �������, %.
///
const uint8_t accelLevel[ACCEL_STAGE_NUM] = 
{
    30, 40, 50, 60, 65, 73, 80 
};
//*****************************************************************************
/// \brief ������ ������� �������, ��.
///
const uint16_t accelTime[ACCEL_STAGE_NUM] = 
{
    100 + 50, 70, 80, 100, 50, 50, 50 
};

//*****************************************************************************
/// \brief ������ ������� ����������, %.
///
const uint8_t shutdownLevel[SHUTDOWN_STAGE_NUM] = 
{
    80, 60, 45, 30, 20, 13, 7, 5, 3, 0
};

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************
static ShiftMotor_flags flags;              ///< ����� ���������.
static MotorType        type;               ///< ��� ���������.
static MotorFailure     failure;            ///< ������������� ���������.
static ShiftMotorStates stateCntMotor;           ///< ������� ���������.
static uint16_t         workTime;           ///< ����� ������.
static strOvlCurrTag    strOvlCurr;         ///< ��������� �������� ���������� �������� ����� �� ������ �����������.
static uint16_t         timeoutBreakPh;     ///< ������� ������� ���������� ��������� ������ ���
static CircularBuffer circularBuffer[NUM_OF_PHASES]; ///< ������ �������� ��� ��������� ������� �������� ������ ����� � ��������� ��������� ���� 
uint16_t timeoutForAverageInWindow; 


//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

/// \brief  ������� ��� ����������� ���� ������ ���
/// \param state - ��� ������ (1 ���� ���� 2(3));
static void checkBreakPhase( MotorFailure state );

static void calcMiddleValueInWindow( const uint16_t data, CircularBuffer* structOfData, const uint8_t t_o_value );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
//*****************************************************************************
// ������������� ����������
void ShuntShiftMotor_ctor( MotorType motor )
{
    ShuntShiftGen_ctor( );    //������������� ����������
    type = motor;
    failure = eNorm;
    workTime = 0;
    stateCntMotor = eStartState;
    flags.ctrl = 0;      // ������ ��������
    
    InterChannel_setParamSettings( eICId_ShuntShiftMotor_failure,
                                   eScriptSync,
                                   eProcSyncOR,
                                   eProcCheckOff, 0, 0,
                                   50, 0, 0 );
    memset( &strOvlCurr, 0, sizeof( strOvlCurr ) );
    
    for( uint8_t i = 0; i < NUM_OF_PHASES; i++ ) //������������� �������� ��� ���������� �������� �������� ����� ���
    {
        memset( &circularBuffer[i].buff, 0, sizeof( circularBuffer[i].buff ) );
        memset( &circularBuffer[i].index, 0, sizeof( circularBuffer[i].index ) );
        memset( &circularBuffer[i].sizeBuf, MIDDLE_VAL_WINDOW_SIZE, sizeof( circularBuffer[i].sizeBuf ) );
        memset( &circularBuffer[i].shifts, 7, sizeof( circularBuffer[i].shifts ) );
    }
}

//������� ��� ����������� ���� ������ ���
static void checkBreakPhase( MotorFailure state )
{
    static const uint16_t BREAK_THR_TIME = 250; //����� ������� ��� ����������� ������
    
    if( timeoutBreakPh++ == BREAK_THR_TIME )
    {
        timeoutBreakPh = 0;
        failure = state;
        memset( &strOvlCurr, 0, sizeof( strOvlCurr ) );
    }
}

//*****************************************************************************
// ���������� �������
void ShuntShiftMotor_run( void )
{
   
    static uint16_t timeoutCnt, timeoutBreakPh /*, timeoutForAverageInWindow*/; //�������� �������
    static uint8_t accelCnt;                    //������� ������� � ����������
    uint16_t current[3];                        //�������� ����� � ���������� �������� ����� 3-������� ����������
    
    ShuntShiftGen_run( );                  // ������ ���������� 
    //����� �������� ������� ���� ����� �� ���������� ���� ����� ������� 220� �� ����� �������� �������
    if( stateCntMotor != eStartState || ControlSystem_getStateCheck220V( ) == eCount )    
        workTime++;    //��������� ������� ��������
    if( timeoutForAverageInWindow > 0 )
        timeoutForAverageInWindow--;
    
    switch( stateCntMotor )
    {
        case eStartState:    //����������� ���������
            if( flags.ctrl )
            {   //���������
                accelCnt = 0;
                ShuntShiftGen_turnOn( flags.dir, accelFreq[accelCnt], accelLevel[accelCnt] );
                timeoutCnt = accelTime[accelCnt];
                timeoutForAverageInWindow = TIMEOUT_FOR_AVERAGE_IN_WINDOW;
                workTime = 0;                 
                failure = eNorm;
                memset( &strOvlCurr, 0, sizeof( strOvlCurr ) );
                for( uint8_t i = 0; i < NUM_OF_PHASES; i++ ) //������������� �������� ��� ���������� �������� �������� ����� ���
                {
                    memset( &circularBuffer[i].buff, 0, sizeof( circularBuffer[i].buff ) );
                    memset( &circularBuffer[i].index, 0, sizeof( circularBuffer[i].index ) );
                    circularBuffer[i].averageVal = 0;
                }
                stateCntMotor = eAcceleration;
            }
            break;
        case eAcceleration:                   //������
            if( timeoutCnt ) 
                timeoutCnt--;    //��������� ����-���� �������
            else
            {
                accelCnt++;    //��������� ������� �������
                if( accelCnt < ACCEL_STAGE_NUM &&
                    accelLevel[accelCnt] < generLevel[type] )  
                {   //������ ��� �� ����������
                    ShuntShiftGen_setParam( accelFreq[accelCnt], accelLevel[accelCnt] );
                    timeoutCnt = accelTime[accelCnt];
                } 
                else
                {   // ������ ����������
                    ShuntShiftGen_setParam( eFreq50hz, generLevel[type] );
                    timeoutCnt = TIMEOUT_MEASURE_U_FB;
                    failure = eNorm; 
                    if( !InterChannel_isHandling( eICId_ShuntShiftMotor_failure ) )    
                        InterChannel_synchronize( eICId_ShuntShiftMotor_failure, failure );
                    stateCntMotor = eRunning; 
                }
            }
            break;
                
        case eRunning:    //��������
            if( flags.ctrl )
            {   //��������
                    uint8_t cntBr = 0;

                    //����������� ������ �������, �������� ���������� ������������ ��������
                    for( uint8_t i = 0; i < NUM_OF_PHASES; i++ )
                    {
                        if( timeoutForAverageInWindow == 0 ) //������� ������� � ��������� ������ ����� 900 �� ����� ������ ��������
                        {
                            calcMiddleValueInWindow( InterChannel_getData( eICId_IV_rms + i ), 
                            &circularBuffer[i], TIMEOUT_FOR_FUNC_CALC_MIDDLE_VAL );
                        }
                        current[i] = AnalogMeasurement_getData( eAinchIV + i )->rms; 
                        if( current[i] < ( flags.stand ? STAND_BREAK_CURRENT : MOTOR_BREAK_CURRENT ) && BinIn_is220vOk( ) )
                        {
                            cntBr++;
                        }
                        else if( current[i] > THR_OVL_SC_ADC ) //�������� ���� ���� ���� ���� ����������
                        {
                            strOvlCurr.cntOvlCur++; 
                        }    
                    }
                    switch( cntBr )
                    {
                        case 0:
                            if( ++strOvlCurr.timeoutOvlCur == TIMEOUT_OVL_SC ) //���� ����-��� ����������� ���� ����������
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
            { //������� � ���������� ���������
                for( accelCnt = 0; accelCnt < SHUTDOWN_STAGE_NUM - 1; accelCnt++ )
                {
                    // ����������� ������� ���������� ���� ������ ����������
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
 
        case eShutdown:    //���������� 
            if( timeoutCnt ) 
                timeoutCnt--;   //��������� ����-����
            else
            {
                accelCnt++;
                if( accelCnt < SHUTDOWN_STAGE_NUM)
                {   // ���������� ������������
                    ShuntShiftGen_setParam( eFreq50hz, shutdownLevel[accelCnt] );
                    timeoutCnt = SHUTDOWN_STAGE_TIME;
                }
                else
                {   // ���������� ���������
                    ShuntShiftGen_turnOff( );
                    stateCntMotor = eFullShutdown;
                }
            }
            break;

        case eFullShutdown:    //������ ���������� (���������� ����������)
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
// ���������
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
// ����������
void ShuntShiftMotor_turnOff( void )
{
    flags.ctrl = 0;
}

//*****************************************************************************
// �������� ��������� ������
bool ShuntShiftMotor_isOn( void )
{
    return stateCntMotor != eStartState;
}

//*****************************************************************************
// �������� �������������
MotorFailure ShuntShiftMotor_getFailure( void )
{
    return failure;
}
//*****************************************************************************
// ���������� ������������� (������������ �� ������ IntegrCtrl)
void ShuntShiftMotor_setFailure( MotorFailure value )
{
    failure = value;
}

//*****************************************************************************
// �������� ����� ������ ���������
uint16_t ShuntShiftMotor_getWorkingTime( void )
{
    return workTime;
}

//*****************************************************************************
// �������� ��������� ���������� �� ����
bool ShuntShiftMotor_getIsOvlCurr( void )
{
    return strOvlCurr.isOvlCurr;
}

//*****************************************************************************
// ���������� ��������� ���������� �� ����
void ShuntShiftMotor_setIsOvlCurr( bool val )
{
    strOvlCurr.isOvlCurr = val;
}

//*****************************************************************************
// ���������� ������� �������� ���� ��� ���������� ���������
uint16_t ShuntShiftMotor_getAverageThreePhasesCurrent( void )
{
    return __builtin_divud( circularBuffer[0].averageVal + circularBuffer[1].averageVal +
                            circularBuffer[2].averageVal, 3);
}
//*****************************************************************************
// ������ � ��������� ������� ��� ���������� ����������� ��������.
// ������� ��������� :
// data  � ������ ��� ��� ��������� � �����.
// structOfData - ��������� �� ��������� ���������� ������
// t_o_value  - �������� �������� ��� ������ �������
static void calcMiddleValueInWindow( const uint16_t data, CircularBuffer* structOfData, const uint8_t t_o_value )
{ 
    //return;
    
    uint32_t sumBuff = 0;
    static uint8_t timeout;
    
    if( ++timeout < t_o_value ) //������� ���������� � ������� ������� t_o_value (� �������������)
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
* ������� ���������: 
* 
* ������ 1.0.1
* ����   24-01-2018
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
* 
* ������ 1.0.2
* ����   15-10-2020
* �����  ��������� �.�.
* 
* ���������:
*   - �������� ������ ������������ �������� ���������� �� ���� �� ����� �������� ������� � ShuntShiftMotor_run;
*   - ��������� ���������� ����������� ShuntShiftMotor_getIsOvlCurr � ShuntShiftMotor_setIsOvlCurr. 
*
* ������ 1.0.3
* ����   18-02-2021
* �����  ��������� �.�.
* 
* ���������:
*   ��������� ������� ��������� ��������� ������ ���(����) checkBreakPhase 
*/
