/**
* \file    shuntShiftMotor.c
* \brief   \copybrief shuntShiftMotor.h
*
* \version 1.0.1
* \date    24-01-2018
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "shuntShiftMotor.h"
#include "shuntShiftGen.h"
#include "ProtectionState_codes.h"
#include "asserts.h"
#include "AnalogMeasurement.h"
#include "string.h"
#include "asserts_ex.h"
#include "InterChannel.h"

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

//*****************************************************************************
// ��������� �������������� ���������
//***************************************************************************** 
static const uint8_t NUM_OF_PHASES = 3;    ///< ���������� ��� �������.

//*****************************************************************************
// ����������� ����� ������
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
}strOvlCurrTag;

//*****************************************************************************
/// \brief �������������� �������� �������� ��������� ������� #ShuntShiftMotor_run.
///
typedef enum
{
    eStop          = 0,    ///< ����������� ���������
    eAcceleration,         ///< ������
    eRunning,              ///< ��������
    eShutdown,             ///< ����������
    eFullShutdown          ///< ������ ���������� (���������� ����������)
} ShiftMotorStates;

//*****************************************************************************
// ����������� ��������� �������������� ��������
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
// ����������� ��������� ����������
//*****************************************************************************
static ShiftMotor_flags flags;              ///< ����� ���������.
static MotorType        type;               ///< ��� ���������.
static MotorFailure     failure;            ///< ������������� ���������.
static ShiftMotorStates stateCnt;           ///< ������� ���������.
static uint16_t         workTime;           ///< ����� ������.
static strOvlCurrTag    strOvlCurr;         ///< ��������� �������� ���������� �������� ����� �� ������ �����������.

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
    stateCnt = eStop;
    flags.ctrl = 0;      // ������ ��������
    
    InterChannel_setParamSettings( eICId_ShuntShiftMotor_failure,
                                   eScriptSync,
                                   eProcSyncOR,
                                   eProcCheckOff, 0, 0,
                                   50, 0, 0 );
    memset( &strOvlCurr, 0, sizeof( strOvlCurr ) );
}

//*****************************************************************************
// ���������� �������
void ShuntShiftMotor_run( void )
{
   
    static uint16_t timeoutCnt;            //�������� �������
    static uint8_t accelCnt;               //������� ������� � ����������
    uint16_t current[3];                   //�������� ����� � ���������� �������� ����� 3-������� ����������
        
    ShuntShiftGen_run( );                  // ������ ���������� 
    
    if( stateCnt != eStop )  
        workTime++;    //��������� ������� ��������
        
    switch( stateCnt )
    {
        case eStop:    //����������� ���������
            if( flags.ctrl )
            {   //���������
                accelCnt = 0;
                ShuntShiftGen_turnOn( flags.dir, accelFreq[accelCnt], accelLevel[accelCnt] );
                timeoutCnt = accelTime[accelCnt];
                workTime = 0;                 
                stateCnt = eAcceleration;
                failure = eNorm;
                memset( &strOvlCurr, 0, sizeof( strOvlCurr ) );
            }
            break;
     
        case eAcceleration:                   //������
            if( timeoutCnt ) timeoutCnt--;    //��������� �������� �������
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
                    stateCnt = eRunning;    
                    timeoutCnt = TIMEOUT_MEASURE_U_FB;
                    failure = eNorm; 
                    if( !InterChannel_isHandling( eICId_ShuntShiftMotor_failure ) )    
                        InterChannel_synchronize( eICId_ShuntShiftMotor_failure, failure );
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
                    current[i] = AnalogMeasurement_getData( eAinchIV + i )->rms; 
                    if( current[i] < ( flags.stand ? STAND_BREAK_CURRENT : MOTOR_BREAK_CURRENT ) )
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
                        if( ++strOvlCurr.timeoutOvlCur == TIMEOUT_OVL_SC ) //���� ������� ����������� ���� ����������
                        {
                            strOvlCurr.timeoutOvlCur = 0;
                            failure = ( strOvlCurr.cntOvlCur > NUM_OVL_SC ) ? eOverloadCircuit : eNorm;
                            strOvlCurr.cntOvlCur = 0;
                        }
                        break; 
                    case 1:
                        failure = eBreakOnePhase;
                        memset( &strOvlCurr, 0, sizeof(strOvlCurr) );
                        break;
                    case 2:
                    case 3:
                        failure = eBreakAllPhases;
                        memset( &strOvlCurr, 0, sizeof(strOvlCurr) );
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
                stateCnt = eShutdown;
                if( !InterChannel_isHandling( eICId_ShuntShiftMotor_failure ) )    
                    InterChannel_synchronize( eICId_ShuntShiftMotor_failure, failure );
            }
            break;
 
        case eShutdown:    //���������� 
            if( timeoutCnt ) timeoutCnt--;   //��������� ��������
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
                    stateCnt = eFullShutdown;
                }
            }
            break;

        case eFullShutdown:    //������ ���������� (���������� ����������)
            if( !ShuntShiftGen_isOn( ) )
            {
                stateCnt = eStop;
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
    return stateCnt != eStop;
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
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   24-01-2018
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
*/
