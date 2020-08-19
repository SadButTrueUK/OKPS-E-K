/**
* \file    shuntShiftFiveEC.c
* \brief   \copybrief shuntShiftFiveEC.h
*
* \version 1.0.1
* \date    02-02-2018
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "shuntShiftFiveEC.h"
#include "shuntShift.h"
#include "shuntShiftMotor.h"
#include "ProtectionState_codes.h"
#include "asserts.h"
#include "InterChannel.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************
#define POSITION_DETECT_TO    300        ///< ����� ����������� ��������� ����� �������.
#define SET_SHIFT_POSITION_TO 200        ///< ����� ��������� ������� ��������.
#define MOTOR_SHUTDOWN_TO     300        ///< ����� ���������� ���������.

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief �������������� ��������� ������� #FiveWireEcShuntShift_run.
///
typedef enum
{
    eStop        = 0,        ///< ��������� "����"
    eInitPosDet,             ///< ��������� �������� ����������� ���������
    ePrepForRun,             ///< ��������� �������� ��������� ��������� ��������
    eRunning,                ///< �������, �������� ����������
    eShutdown,               ///< �������� ��������� ���������
    eFinalPosDet             ///< �������� ����������� ���������
} ShiftFiveEcStates;

//*****************************************************************************
/// \brief ��������� ��������� ������ ������ �������� �������.
///
typedef struct
{
    uint8_t ctrl   :1;        ///< ��������� ������.
    uint8_t st     :1;        ///< ������ �� ������.
    uint8_t dir    :1;        ///< ��������������� ���������.
    uint8_t unused :5;        ///< �������������� ����.
} ShuntShiftFiveEc_flags;

//*****************************************************************************
/// \brief ����������� ������ ��������� � \a uint8_t.
///
typedef union
{
    ShuntShiftFiveEc_flags str;         ///< ����� ��������� \a uint8_t. 
} uShuntShiftFiveEc_flags;

//*****************************************************************************
// ����������� ��������� �������������� ��������
//*****************************************************************************

//*****************************************************************************
/// \brief ���������� ��������� ����������.
///
const ArrayShuntShift fiveWireEcShuntShift =
{
    FiveWireEcShuntShift_ctor,
    FiveWireEcShuntShift_run,
    FiveWireEcShuntShift_turnOn,
    FiveWireEcShuntShift_turnOff,
    FiveWireEcShuntShift_setDetPos,
    FiveWireEcShuntShift_setShiftPos,
    FiveWireEcShuntShift_isEnable,
    FiveWireEcShuntShift_getReqPosDet,
    FiveWireEcShuntShift_getReqShiftDir,
    FiveWireEcShuntShift_getTime,
    FiveWireEcShuntShift_getDgn,
    FiveWireEcShuntShift_setCntPhaseShift,
    FiveWireEcShuntShift_getCntPhaseShift,
    FiveWireEcShuntShift_isWorkMode
};

//*****************************************************************************
// ����������� ��������� ����������
//*****************************************************************************
static          uShuntShiftFiveEc_flags flags;              ///< ����� ���������.
static          ShiftFiveEcStates       stateCnt;           ///< ������� ���������.
static          PositionDet_State       reqPos;             ///< ��������� ������� ��� ����������� ���������.
static          PositionDet_State       setPos;             ///< ������������� ������� ��� ����������� ���������.
static          ShiftDirection          reqDir;             ///< ��������� ������� ��� ��������.
static          ShiftDirection          setDir;             ///< ������������� ������� ��� ��������.
static          PositionDet_State       pos;                ///< ����������� ���������.
static uint16_t                         setTime;            ///< ��������������� �����.
static uint16_t                         timeoutCnt;         ///< ������� ���������.
static          ShiftDgn                dgn;                ///< ��������� ���������� ��������. 
static uint16_t                         shiftTime;          ///< ����� ���������� ��������.
static uint8_t                          repeatShift;        ///< ����� ���������� ��������.
static uint8_t                          cntPhaseShift;      ///< �������� �������� ���� ����������� ��������.

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �������������
void FiveWireEcShuntShift_ctor( void )
{
    flags.str.ctrl = 0; //��������   
    reqDir = eShiftStop;
    setDir = eShiftStop;
    reqPos = eDetNon;
    setPos = eDetNon;
    pos = eDetNon;
    dgn = eShiftNormal;
    shiftTime = 0;
    stateCnt = eStop;
}

//*****************************************************************************
// ���������� �������
void FiveWireEcShuntShift_run( void )
{
    if( timeoutCnt ) timeoutCnt--; //��������� �������� ���������

    switch( stateCnt )
    {
        case eStop: //��������
            if( flags.str.ctrl )
            {
                //������ �� ��������� ���������
                reqPos = flags.str.dir ? eDetPlus : eDetMinus; // ����������� ������������ ���������
                setPos = eDetNon;
                pos = eDetNon;
                reqDir = eShiftStop;
                setDir = eShiftStop;
                timeoutCnt = POSITION_DETECT_TO;
                stateCnt = eInitPosDet;
            }
            break;

        case eInitPosDet: // �������� ����������� ���������
            if( timeoutCnt == 0 && setPos == reqPos )
            { // ������� �����
                // �������� ������������� ������� ��� ������� ���������
                if( ( flags.str.dir && pos == eDetPlus ) || // ����������� ��������� ������������� 
                    ( flags.str.dir == 0 && pos == eDetMinus ) )
                { // ���������������� ���������
                    //������� �� ��������
                    flags.str.ctrl = 0; // ����������
                    stateCnt = eStop;
                }
                else
                { // ���������� ��������� ��� �������� 
                    reqPos = eDetNon;
                    reqDir = flags.str.dir ? eShiftPlus : eShiftMinus;
                    timeoutCnt = SET_SHIFT_POSITION_TO;
                    stateCnt = ePrepForRun;
                }
            }
            else if( timeoutCnt == 0 && setPos != reqPos )
            {
                ERROR_ID( eGrPS_ShuntShift, ePS_ShuntShiftFiveEcMismatchSetPos );
            }
            break;

        case ePrepForRun: //�������� ��������� ��������� ��������  
            if( timeoutCnt == 0 && setDir == reqDir )
            { //��������� ��� �������� �����������
                //�������� �������
                ShuntShiftMotor_turnOn( flags.str.st, flags.str.dir );
                dgn = eShiftActive; //�������� ���������
                timeoutCnt = setTime; //���������� �������� ����� ��������
                stateCnt = eRunning;
            }
            else if( timeoutCnt == 0 && setDir != reqDir )
            { // ��������� �� �����������
                ERROR_ID( eGrPS_ShuntShift, ePS_ShuntShiftFiveEcMismatchSetDir );
            }
            break;

        case eRunning: //�������, �������� ����������
            if( timeoutCnt == 0 ||
                InterChannel_getData( eICId_ShuntShiftMotor_failure ) != eNorm || //�������������
                flags.str.ctrl == 0 )
            { //���������� �������
                shiftTime = ShuntShiftMotor_getWorkingTime( ); // ������� ����� 
                ShuntShiftMotor_turnOff( ); //��������� ��������� 
                timeoutCnt = MOTOR_SHUTDOWN_TO;
                stateCnt = eShutdown;
            }
            break;

        case eShutdown: //�������� �������� ���������
            if( timeoutCnt == 0 && !ShuntShiftMotor_isOn( ) )
            { // ��������� ��������
                reqDir = eShiftStop;
                setDir = eShiftStop;
                reqPos = flags.str.dir ? eDetPlus : eDetMinus; // ����������� ������������ ���������
                setPos = eDetNon;
                pos = eDetNon;
                timeoutCnt = POSITION_DETECT_TO;
                stateCnt = eFinalPosDet;
            }
            else if( timeoutCnt == 0 && ShuntShiftMotor_isOn( ) )
            { // ��������� �� ����������
                ERROR_ID( eGrPS_ShuntShift, ePS_ShuntShiftFiveEcMotorNotStoped );
            }
            break;

        case eFinalPosDet: // �������� ����������� ���������
            if( timeoutCnt == 0 && setPos == reqPos )
            { // ������� �����
                // �������� ������������� ������� ��� ������� ���������
                if( ( flags.str.dir && pos == eDetPlus ) || // ����������� ��������� ������������� 
                    ( flags.str.dir == 0 && pos == eDetMinus ) )
                { // ���������������� ���������
                    dgn = eShiftNormal; // ���������� ��������� - ������� ��������
                }
                else
                { // 
                    dgn = eShiftFail; // ������� ���������� �������� 
                }
                flags.str.ctrl = 0; //��������� �������
                stateCnt = eStop;
            }
            else if( timeoutCnt == 0 && setPos != reqPos )
            {
                ERROR_ID( eGrPS_ShuntShift, ePS_ShuntShiftFiveEcMismatchSetPos );
            }
            break;

        default:
            ERROR_ID( eGrPS_ShuntShift, ePS_ShuntShiftFiveEcCntErr );
    }
}

//*****************************************************************************
// ���������
void FiveWireEcShuntShift_turnOn( bool dir, uint16_t time, bool st, uint8_t rs )
{
    if( !FiveWireEcShuntShift_isEnable( ) )
    {
        flags.str.dir = dir;
        flags.str.ctrl = 1;
        flags.str.st = st;
        setTime = time;
        repeatShift = rs;
    }
}

//*****************************************************************************
// ����������
void FiveWireEcShuntShift_turnOff( void )
{
    flags.str.ctrl = 0;
}

//*****************************************************************************
// �������� ��������� �������
void FiveWireEcShuntShift_setDetPos( PositionDet_State detPos, PositionDet_State position )
{
    setPos = detPos;
    pos = position;
}

//*****************************************************************************
// �������� ������������� ������� ��������
void FiveWireEcShuntShift_setShiftPos( ShiftDirection shiftPos )
{
    setDir = shiftPos;
}

//*****************************************************************************
// �������� ��������� ���������
bool FiveWireEcShuntShift_isEnable( void )
{
    return stateCnt != eStop;
}

//*****************************************************************************
// ������ �� ��������� ������� ��������
PositionDet_State FiveWireEcShuntShift_getReqPosDet( void )
{
    return reqPos;
}

//*****************************************************************************
// ������ �� ��������� ������� ����������� ��������
ShiftDirection FiveWireEcShuntShift_getReqShiftDir( void )
{
    return reqDir;
}

//*****************************************************************************
// �������� ����� ���������� ��������
uint16_t FiveWireEcShuntShift_getTime( void )
{
    return shiftTime;
}

//*****************************************************************************
// �������� ��������� ��������
ShiftDgn FiveWireEcShuntShift_getDgn( void )
{
    return dgn;
}

//*****************************************************************************
//���������� �������� �������� ���� ����������� ��������.
void FiveWireEcShuntShift_setCntPhaseShift( const uint8_t val )
{
    cntPhaseShift = val;
}
//*****************************************************************************
//�������� �������� �������� ���� ����������� ��������.

const uint8_t FiveWireEcShuntShift_getCntPhaseShift( void )
{
    return cntPhaseShift;
}

const bool FiveWireEcShuntShift_isWorkMode( void )
{
    return false;
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
*    ������� ������.
*/
