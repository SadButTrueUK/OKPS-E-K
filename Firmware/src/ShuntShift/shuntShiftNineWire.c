/**
* \file    shuntShiftNineWire.c
* \brief   \copybrief shuntShiftNineWire.h
*
* \version 1.0.4
* \date    23-04-20
* \author  ��������� �.�.
*/

//****************************************************************************
// ������������ �����
//****************************************************************************
#include "shuntShiftNineWire.h"
#include "shuntShift.h"
#include "shuntShiftMotor.h"
#include "ProtectionState_codes.h"
#include "asserts.h"
#include "InterChannel.h"
#include "IntegrCtrl.h"
#include "IntegrCtrlGen.h"
#include "CheckSupply.h"
#include "Main.h"
#include "ControlSystem.h"
#include "DebugTools.h"
#include "shuntShiftGen.h"
#include "addressVarEeprom.h"
#include "Eeprom.h"
#include <string.h>
#include "AnalogMeasurement.h"


//****************************************************************************
// ��������� ���������, ������������ ����� �������
//****************************************************************************
#define INIT_TIMEOUT                500        ///< ��������� ������� ��� ����������� ����������� ��������� �������.
#define TIMEOUT_WORK_FRICTION       3000       ///< ������� ��� ������ �� ������� (3000 ��)
#define RS_DOUBLE_CALIBR            3          ///< �������� ����� �������������� �������������� �������� (0b11).
#define RS_ON                       2          ///< �������� ����� ���������� �������� (0b10).
#define RS_SINGLE_CALIBR            1          ///< �������� ����� �������������� �������������� �������� (0b01).
#define RS_OFF                      0          ///< ������� ����� ������������ �������� (0b00).
#define SIZE_BUFF_FOR_MIDDLE_CURR   1024
#define SHIFTS_FOR_AVERAGE_CURRENT  10

//****************************************************************************
// ���������� ����� ������
//****************************************************************************

//****************************************************************************
/// \brief ��������� ���������. 
///
typedef struct 
{
    uint8_t ctrl   :1;        ///< ��������� ������.
    uint8_t st     :1;        ///< ������ �� ������.
    uint8_t dir    :1;        ///< ��������������� ���������.
    uint8_t rs     :2;        ///< ��������� �������.
    uint8_t unused :3;        ///< �������������� ����.
} ShuntShiftNine_flags;

//****************************************************************************
/// \brief ����������� ������ ��������� � \a uint8_t.
///
typedef union 
{
    ShuntShiftNine_flags str;         ///< ����� ���������.
} uShuntShiftNine_flags;

enum States_t
{
    eInitState,
    eWaiting,
    eFillBuffer
};

//****************************************************************************
// ���������� ���������� �������������� ��������
//****************************************************************************

//****************************************************************************
/// \brief ���������� ��������� ����������.
///
const ArrayShuntShift nineWireShuntShift = 
{
    NineWireShuntShift_ctor,
    NineWireShuntShift_run,
    NineWireShuntShift_turnOn,
    NineWireShuntShift_turnOff,
    NineWireShuntShift_setDetPos,
    NineWireShuntShift_setShiftPos,
    NineWireShuntShift_isEnable,
    NineWireShuntShift_getReqPosDet,
    NineWireShuntShift_getReqShiftDir,
    NineWireShuntShift_getTime,
    NineWireShuntShift_getDgn,
    NineWireEcShuntShift_setCntPhaseShift,
    NineWireEcShuntShift_getCntPhaseShift,
    NineWireEcShuntShift_isWorkMode,
    NineWireShuntShift_setDgn,
    NineWireShuntShift_setTime
};

//****************************************************************************
// ���������� ���������� ����������
//****************************************************************************
static uShuntShiftNine_flags    flags;                                      ///< ����� ���������.
static ShiftNineWireStates      stateCnt;                                   ///< ������� ���������.
static PositionDet_State        pos;                                        ///< ����������� ���������.
static uint32_t                 sumBuff;
static uint16_t                 setTime;                                    ///< ��������������� �����.
static uint16_t                 timeoutCnt;                                 ///< ������� ���������.
static ShiftDgn                 dgn;                                        ///< ��������� ���������� ��������.
static uint16_t                 shiftTime;                                  ///< ����� ���������� ��������.
static uint8_t                  cntTriesRS;                                 ///< ������� ��� ��������� ������ ����������� ��������
static bool                     savePosDetInRepeatShuntShift;               ///< ��������� ��������� ������� ����� ���� 2 ����������� ��������
//static uint16_t                 buffForMiddleCurr[SIZE_BUFF_FOR_MIDDLE_CURR];
static enum                     States_t statesPrepareBuff = eInitState;
uint16_t                        middleCurrShuntShift, middleCurrFriction;   ///< ������� ��� ����������� �������� � ������� ��� ��� ������ �� �������                
uint16_t                        thresholdFriction;
uint16_t                        t_o_friction;

//****************************************************************************
// ���������� ��������� �������
//****************************************************************************
static uint16_t calcAverageCurrentShiftOrFriction( void );
static void prepareBuffMiddleCurr( void );

//****************************************************************************
// ���������� ������������ �������
//****************************************************************************

//****************************************************************************
// �������������
void NineWireShuntShift_ctor( void )
{
    flags.str.ctrl = 0;  //�������� 
    pos = eDetNon;    
    dgn = eShiftNormal;   
    shiftTime = 0;
    stateCnt = eInit;
    timeoutCnt = INIT_TIMEOUT;
    cntTriesRS = 0;
    middleCurrShuntShift = 0; 
    middleCurrFriction = 0;
    savePosDetInRepeatShuntShift = false;
}

//****************************************************************************
// ���������� �������
void NineWireShuntShift_run( void )
{
    static bool temp;
    static bool dirInternal;        //��� ��������� ����������� �������� ��� ���������� ��������
    static bool isPosDetNoControlForcalibrate;  //����� "������ ��������" � ������������� ������, ������ ��� ��� ������� ����������
    
    if ( ActivityManager_isActive( activityManager ) == false )
    {
        dgn = eShiftNormal;
        return;
    }    
    if( timeoutCnt ) 
        timeoutCnt--;    // ��������� �������� ���������
    
    //������� ���������� ������ ������� ����� �������� � ������� 
    //� ������ "������" 900 �� ��� ����������� ���������� ��������� ��� ����� ��������� � ��� �������� �� �������
    //��� ������������� ��������
    prepareBuffMiddleCurr( ); 
    
    switch( stateCnt )
    {
        case eInit:     //������������� �� ��������� �������
            if( timeoutCnt == 0 )
                stateCnt = eStart;
            break;
        
        case eStart:
            if( flags.str.ctrl )
            {
                dirInternal = flags.str.dir; //����������� �������� ���� �� �������
                stateCnt = eInitPosDet;
            } 
            break;
                                                                                               
        case eInitPosDet: // �������� ����������� ���������
            // �������� ���������
            temp = ( ( dirInternal == 1 ) && ( pos == eDetPlus ) )   
               ||( ( dirInternal == 0 ) && ( pos == eDetMinus ) );
            if( temp )  // ����������� ��������� ������������� 
            {           // ���������������� ���������
                //������� �� ��������
                flags.str.ctrl = 0; // ����������
                stateCnt = eStart;     
            }
            else
            {   // �������� �������
                ShuntShiftMotor_turnOn( flags.str.st, dirInternal );
                if( ( dgn != eShiftActive &&  flags.str.rs != RS_ON ) || 
                    ( flags.str.rs == RS_ON  &&
                    ( dgn != eShiftActive || 
                    ( dgn == eShiftActive && ControlSystem_getStateCheck220V( ) == eOff ) ) ) )
                {
                    dgn = eShiftActive;                         //�������� ��������� ��������
                    ControlSystem_setTimeProcessingOrder( 0 );  //� ������ �������� ������������� � ���� ����� ���������� �������
                }
                //��� ������ �������� � ������ �������� ������� � ������� �������������� ��������
                //��������� ������� ���������� �������
                if( pos == eDetNon && ( flags.str.rs == RS_SINGLE_CALIBR || flags.str.rs == RS_DOUBLE_CALIBR ) )
                    isPosDetNoControlForcalibrate = true;
                timeoutCnt = setTime;
                stateCnt = eRunning;
            }
            break;      
        case eRunning: // �������, �������� ����������
            // ��������� ������������� ������������
            temp = ( ( dirInternal == 1 ) && ( pos == eDetPlus ) )   
                   ||( ( dirInternal == 0 ) && ( pos == eDetMinus ) );
            // ������� ��������� ������������� ��������� � ������� + ����� ���������� ������� (��� ����������� ��� ����������)
            if( ( temp && ( ControlSystem_getStateOrderFieldRS() == eRouteShuntShift 
                          || ControlSystem_getStateOrderFieldRS() == eRouteRepeatShuntShift) ) ||
                timeoutCnt == 0 ||                           // ����� �������� ������� 
                ShuntShiftMotor_getFailure( ) != eNorm ||    // �������������
                flags.str.ctrl == 0 ) // ������� ���������� �������� ������� �������� (���� ��� ���������� 220�)
            {   //������ �� ������������� �������� ������
                InterChannel_synchronize( eICId_IntegrCtrlScAndBreak, ShuntShiftMotor_getFailure( ) );
                if ( ShuntShiftMotor_getFailure( ) == eShortCircuit )
                     ShuntShiftGen_fastTurnOff();
                ShuntShiftMotor_turnOff( ); // ��������� ��������� 
                statesPrepareBuff = eInitState; //���������� ������� ������� ���������� ������ �����
                stateCnt = eShutdown;
            }
            else if ( temp && ( ControlSystem_getStateOrderFieldRS() == eRouteCalibrateShuntShift 
                          || ControlSystem_getStateOrderFieldRS() == eRouteRepeatCalibrateShuntShift ) )
            {
                if( isPosDetNoControlForcalibrate )
                {
                    ShuntShiftMotor_turnOff( );     // ��������� ��������� 
                    statesPrepareBuff = eInitState; //���������� ������� ������� ���������� ������ �����
                    stateCnt = eShutdown;
                }
                else
                {
                    //��������� ������� ��� ����������� �������� �� ���� ��� �����
                    middleCurrShuntShift = calcAverageCurrentShiftOrFriction( );
                                        //���������� ������ ������ �� ������� � ������� �� ��������� �������� eFriction 
                    t_o_friction = TIMEOUT_WORK_FRICTION;
                    statesPrepareBuff = eInitState;    //���������� ������� ������� ���������� ������ �����
                    stateCnt = eFriction;
                }
            }
            break;
        case eShutdown:    // �������� ���������� ���������
            if( !ShuntShiftMotor_isOn( ) )
            {   // ��������� ��������
                isPosDetNoControlForcalibrate = false;
                temp = ( ( dirInternal == 1 ) && ( pos == eDetPlus ) )   
                   ||( ( dirInternal == 0 ) && ( pos == eDetMinus ) );
                shiftTime = ShuntShiftMotor_getWorkingTime( );    // ������� ����� 
                if( temp )  //������������� ���� dgn ������
                {           //����������� ��������� ������������� ���������������� ���������
                    if( flags.str.rs == RS_OFF && 
                        ControlSystem_getStateOrderFieldRS() != eRouteCalibrateShuntShift &&
                        ControlSystem_getStateOrderFieldRS() != eRouteRepeatCalibrateShuntShift ) 
                        dgn = eShiftNormal;    
                    else 
                    {
                        if( cntTriesRS == 2 ) //���������� ������� ���������� �� ������ �������
                            dgn = eShiftSecondTry; 
                        else if( cntTriesRS == 0 ) //���������� ������� ���������� � ������ �������
                            dgn = eShiftNormal;
                        else if( cntTriesRS == 3 ) //���������� ������� ���������� ��������
                            dgn = eShiftFail;
                    }
                }
                else 
                {                        // ����������� ��������� �� ������������� ����������������
                    if( flags.str.ctrl == 0 && ShuntShiftMotor_getFailure( ) == eNorm )
                    {
                        if( !ActivityManager_isNeighborWorking( activityManager ) || 
                             !ActivityManager_getNeighborConnection( activityManager ) )
                        {
                            if ( ControlSystem_getStatesCheck220VInShuntShift( ) != eShuntShiftIsFailed && CheckSupply_is24vOn( ) )
                            {
                                if( ControlSystem_getStatesCheck220VInShuntShift( ) == eNo220VInShuntShift && timeoutCnt )
                                {    
                                    //dgn = eShiftActive;
                                }
                                else if ( timeoutCnt == 0 )
                                {    
                                    //dgn = eShiftFail;
                                }
                                else
                                {    
                                    dgn = eShiftNormal;
                                }
                            }     
                            else 
                            {    
                                //dgn = eShiftFail;
                            }
                        }
                        else if ( CheckSupply_is220vOn( ) && CheckSupply_is24vOn( ) ) 
                        {
                            dgn = eShiftNormal;
                        }
                    }
                    else if( flags.str.rs == RS_OFF || 
                            ( flags.str.rs == RS_ON && 
                            ( cntTriesRS == 3 || 
                            ( cntTriesRS == 2 && savePosDetInRepeatShuntShift == false ) 
                            || ShuntShiftMotor_getFailure( ) != eNorm ) ) )
                    {    
                        dgn = eShiftFail;    // ������� ���������� �������� 
                    }
                }
                if( flags.str.rs != RS_ON )  // !!!���� ���, ������ ������� ����������
                {
                    flags.str.ctrl = 0;    // ����������
                    stateCnt = eStart;
                }
                else
                {
                    cntTriesRS++;
                    if( cntTriesRS == 2 )
                        savePosDetInRepeatShuntShift = temp;  //��� ����������� �������� ��������� ����� ������ ���� �������� �������� ��������� �������
                    if ( ( cntTriesRS == 4 ) ||
                        ( ( temp && ( cntTriesRS != 2 ) ) || 
                          ( savePosDetInRepeatShuntShift == false &&  cntTriesRS == 3 ) ) ||
                        ( flags.str.ctrl == 0 ) || ShuntShiftMotor_getFailure( ) != eNorm )
                    {
                       savePosDetInRepeatShuntShift = false;
                       cntTriesRS = 0; 
                       flags.str.ctrl = 0;
                       stateCnt = eStart;
                    }    
                    else
                    {
                        dirInternal = !dirInternal; //�������� ����������� �������� ��� ����������� ��������
                        stateCnt = eInitPosDet;
                        setTime = ControlSystem_getTimeCurrentOrder( ); 
                    }
                }    
            }
            break;
        case eFriction:         //������ �� ������� ��� ������������� ��������
            if( ShuntShiftMotor_getFailure( ) != eNorm ||    // �������������
                flags.str.ctrl == 0 )                         //  ������� ������� ��������
            {
                ShuntShiftMotor_turnOff( );     // ��������� ��������� 
                if ( flags.str.ctrl )
                    dgn = eShiftNormal; //�� ������� ������ dgn � "������� �������" �� ������� "���� ���������"
                if ( ShuntShiftMotor_getFailure( ) != eNorm ) //������� ���������� �������� �� ������
                    dgn = eShiftFail;
                statesPrepareBuff = eInitState;
                stateCnt = eShutdown;
            }    
            else if( --t_o_friction == 0 )                   //���� ������� 3 � ������ �� ������� �� �����  �������������� ��������
            {
                //���������� ���������� �������� ���� ������� � ������������ ������� ��������
                DeviceAddress_setCalibrCurrentAndTime( pos == eDetPlus ? Plus : Minus, 
                ( calcAverageCurrentShiftOrFriction( ) + middleCurrShuntShift ) >> 1,
                 __builtin_muluu( ControlSystem_getTimeProcOrder( ), 3 ) >> 1 );    // ������������� ����� �������� ���� � ��������� ��� � EEPROM
                ShuntShiftMotor_turnOff( );                 // ��������� ���������
                Eeprom_write_start(pos == eDetPlus ? ADDRESS_EEPROM_CALIBR_THRESHOLD_P : ADDRESS_EEPROM_CALIBR_THRESHOLD_M, 
                                   DeviceAddress_getCalibrCurrent( pos == eDetPlus ? Plus : Minus ) ); 
                statesPrepareBuff = eInitState;
                stateCnt = eWaitingForWrThresholdToEEPROM;
            }                                            
            break;
            
        case eWaitingForWrThresholdToEEPROM:
            if ( Eeprom_isReady( ) == false )
                break;            
            Eeprom_write_start( pos == eDetPlus ? ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_P : ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_M,
                                DeviceAddress_getIncreasedTime(pos == eDetPlus ? Plus : Minus ));
            stateCnt = eWaitingForWrShiftTimeToEEPROM;
            break;

        case eWaitingForWrShiftTimeToEEPROM:
            if ( Eeprom_isReady( ) == false )
                break;            
            dgn = eShiftNormal;
            stateCnt = eShutdown;
            break;            
            
        default:
            ERROR_ID( eGrPS_ShuntShift, ePS_ShuntShiftNineWireCntErr );
    } 
}

//****************************************************************************
// ���������
void NineWireShuntShift_turnOn( bool dir, uint16_t time, bool st, uint8_t rs )
{
    if( !NineWireShuntShift_isEnable( ) )
    {
        flags.str.dir  = dir;
        flags.str.ctrl = 1;
        flags.str.st   = st;
        setTime        = time;
        flags.str.rs   = rs;
    }
}

//****************************************************************************
// ����������
void NineWireShuntShift_turnOff( void )
{
    flags.str.ctrl = 0;
}

//****************************************************************************
// �������� ��������� �������
void NineWireShuntShift_setDetPos( PositionDet_State detPos, PositionDet_State position )
{
    pos = position;
}

//****************************************************************************
// �������� ������������� ������� ��������
void NineWireShuntShift_setShiftPos( ShiftDirection shiftPos )
{

}

//****************************************************************************
// �������� ��������� ����������� ��������
bool NineWireShuntShift_isEnable( void )
{
    return stateCnt != eStart && stateCnt != eInit;
}

//****************************************************************************
// ������ �� ��������� ������� ��������
PositionDet_State NineWireShuntShift_getReqPosDet( void )
{
    return eDetNon;
}

//****************************************************************************
// ������ �� ��������� ������� ����������� ��������
ShiftDirection NineWireShuntShift_getReqShiftDir( void )
{
    return eShiftStop;
}

//****************************************************************************
// �������� ����� ���������� ��������
uint16_t NineWireShuntShift_getTime( void )
{
    return shiftTime;
}

//****************************************************************************
// �������� ��������� ��������
ShiftDgn NineWireShuntShift_getDgn( void )
{
    return dgn;
}

void NineWireShuntShift_setDgn( ShiftDgn val )
{
    dgn = val;
}

//*****************************************************************************
//���������� �������� �������� ���� ����������� ��������.
void NineWireEcShuntShift_setCntPhaseShift( const uint8_t val )
{
    cntTriesRS = val;
}
//*****************************************************************************
//�������� �������� �������� ���� ����������� ��������.

const uint8_t NineWireEcShuntShift_getCntPhaseShift( void )
{
    return cntTriesRS;
}

const bool NineWireEcShuntShift_isWorkMode( void )
{
    return  stateCnt != eInit;
}

void NineWireShuntShift_setTime( uint16_t val )
{
    shiftTime = val;
}

ShiftNineWireStates NineWireShuntShift_getStateCnt( void )
{
    return stateCnt;
}

//������� ������� �������� ������ ������� �� 3 ����� �����
static uint16_t calcAverageCurrentShiftOrFriction( void )
{
    return sumBuff >> SHIFTS_FOR_AVERAGE_CURRENT;
}

//������� �������� �� ���������� ���������:
//1. ��� ������������� �������� � ������ �������� ��� � ������ ������ �� ������� ����������� 900��
//2. ��������� ����� �� ������� �������� ����� 3 ��� � ������� 1024�� ��� �������� � ��� ������ �� �������
//3. ������� ��������� ������ ������� ��������� � ��������� ��������� eOff ������� �� �������� �������� ������� � ������� run 
static void prepareBuffMiddleCurr( void )
{
    static const uint16_t DURATION_CUT_START_SHIFT = 900;
    static uint16_t t_o, cnt;
    
    switch( statesPrepareBuff )
    {
        case eInitState:
            if( ( stateCnt == eRunning || stateCnt == eFriction ) &&
                ( flags.str.rs == RS_DOUBLE_CALIBR || flags.str.rs == RS_SINGLE_CALIBR ) )
            {
                t_o = DURATION_CUT_START_SHIFT;
                statesPrepareBuff = eWaiting;
            }
            break;
        case eWaiting:
            if( --t_o == 0 )
            {
                statesPrepareBuff = eFillBuffer;
                sumBuff = 0;
                cnt = 0;
            }
            break;
        case eFillBuffer:
            if( cnt++ < SIZE_BUFF_FOR_MIDDLE_CURR )
            {
                sumBuff += __builtin_divud( AnalogMeasurement_getData( eAinchIV )->rms +
                AnalogMeasurement_getData( eAinchIU )->rms + AnalogMeasurement_getData( eAinchIW )->rms, 3 );
            }
            break;
    }
}

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   02-02-2018
* �����  ��������� �.�.
* 
* ���������:
*     ������� ������.
* 
* ������ 1.0.2
* ����   23-10-2019
* �����  ��������� �.�.
* 
* ���������:
* 	��������� ������ � ��������� ����������� �������� ������� � ������� 
* 	NineWireShuntShift_run: 
* 	- ������������ ��������� ������� "����"  ���� ������� �� ��������
* 	- ������������ ������������ ���� dgn � ���������� �������
* 
* ������ 1.0.3
* ����   12-12-19 
* �����  ��������� �.�.
* 
* ���������:
* 	����� ������� �� ����������� ��������� � ������ �������� �������
* 	� ������� NineWireShuntShift_run (�� ������� ��������� ���������
* 	������ ������ ���������� ������������ ����������), �� ���� ����
* 	���� dgn � ���������� ������� ����������� ������� 
* 
* ������ 1.0.4
* ����   23-04-20
* �����  ��������� �.�.
* 
* ���������:
* 	��������� �������������� ������� ��� ������������ ���� dgn � �������� eShiftNormal -
* 	������� ������� 24 � 220�
*/
