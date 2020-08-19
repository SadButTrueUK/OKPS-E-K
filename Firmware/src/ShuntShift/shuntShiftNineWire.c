/**
* \file    shuntShiftNineWire.c
* \brief   \copybrief shuntShiftNineWire.h
*
* \version 1.0.1
* \date    02-02-2018
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


//****************************************************************************
// ��������� ���������, ������������ ����� �������
//****************************************************************************
#define INIT_TIMEOUT          500        ///< ��������� ������� ��� ����������� ����������� ��������� �������.
#define RS_ON_2               3          ///< ��������� ������� ������� (0b11).
#define RS_ON_1               2          ///< ��������� ������� ������� (0b10).
#define RS_OFF_2              1          ///< ��������� ������� �������� (0b01).
#define RS_OFF_1              0          ///< ��������� ������� �������� (0b00).

//****************************************************************************
// ����������� ����� ������
//****************************************************************************

//****************************************************************************
/// \brief �������������� ��������� ������� #NineWireShuntShift_run.
///
typedef enum 
{
    eInit        = 0,        ///< ��������� �������������
    eStart,                  ///< ������ ��������
    eStop,                   ///< ��������� "����"
    eInitPosDet,             ///< ��������� �������� ����������� ���������
    eRunning,                ///< ��������
    eShutdown                ///< ����������
} ShiftNineWireStates;

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

//****************************************************************************
// ����������� ���������� �������������� ��������
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
    NineWireEcShuntShift_isWorkMode
};

//****************************************************************************
// ����������� ���������� ����������
//****************************************************************************
static uShuntShiftNine_flags flags;          ///< ����� ���������.
static ShiftNineWireStates   stateCnt;       ///< ������� ���������.
static PositionDet_State     pos;            ///< ����������� ���������.
static uint16_t              setTime;        ///< ��������������� �����.
static uint16_t              timeoutCnt;     ///< ������� ���������.
static ShiftDgn              dgn;            ///< ��������� ���������� ��������.
static uint16_t              shiftTime;      ///< ����� ���������� ��������.
static uint8_t               cntTriesRS;     ///< C������ ��� ��������� ������ ����������� ��������

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
}

//****************************************************************************
// ���������� �������
void NineWireShuntShift_run( void )
{
    static bool temp;
    static bool dirInternal;        //��� ��������� ����������� �������� ��� ���������� ��������
    
    if ( ActivityManager_isActive( activityManager ) == false )
    {
        dgn = eShiftNormal;
        return;
    }    
    
    if( timeoutCnt ) 
        timeoutCnt--;    // ��������� �������� ���������
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
            if( temp ) // ����������� ��������� ������������� 
            {   // ���������������� ���������
                //������� �� ��������
                flags.str.ctrl = 0; // ����������
                stateCnt = eStart;      //eStop;
            }
            else
            {   // �������� �������
                ShuntShiftMotor_turnOn( flags.str.st, dirInternal );
                dgn = eShiftActive;    //�������� ���������
                timeoutCnt = setTime;
                stateCnt = eRunning;
            }
            break;      

        case eRunning: // �������, �������� ����������
            // ��������� ������������� ������������
            temp = ( ( dirInternal == 1 ) && ( pos == eDetPlus ) )   
                   ||( ( dirInternal == 0 ) && ( pos == eDetMinus ) );
            if( timeoutCnt == 0 ||                           // ����� �������� ������� 
                ShuntShiftMotor_getFailure( ) != eNorm ||    // �������������
                temp ||                                      // ������� ��������� ������������� ��������� � �������                                    
                flags.str.ctrl == 0 )                       // ������� ���������� �������� ������� ��������
            {   //������ �� ������������� �������� ������
                InterChannel_synchronize( eICId_IntegrCtrlScAndBreak, ShuntShiftMotor_getFailure( ) );
                shiftTime = ShuntShiftMotor_getWorkingTime( );    // ������� ����� 
                ShuntShiftMotor_turnOff( );                       // ��������� ��������� 
                stateCnt = eShutdown;
            }
            break;
            
        case eShutdown:    // �������� ���������� ���������
            if( !ShuntShiftMotor_isOn( ) )
            {   // ��������� ��������
                if( temp )  //������������� ���� dgn ������
                {                          // ����������� ��������� ������������� ���������������� ���������
                    if( flags.str.rs == RS_OFF_2 || flags.str.rs == RS_OFF_1 ) 
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
                    if( flags.str.ctrl == 0 &&  ShuntShiftMotor_getFailure( ) == eNorm )
                    {
                        if( !ActivityManager_isNeighborWorking( activityManager ) || 
                             !ActivityManager_getNeighborConnection( activityManager ) )
                        {
                            if ( CheckSupply_is220vOn( ) && CheckSupply_is24vOn( ) )
                            {
                                dgn = eShiftNormal;
                            }     
                            else 
                                dgn = eShiftFail;
                        }
                        else if ( CheckSupply_is220vOn( ) && CheckSupply_is24vOn( ) ) 
                            dgn = eShiftNormal;
                    }
                    else if( ( flags.str.rs == RS_OFF_2 || flags.str.rs == RS_OFF_1 ) || 
                            ( (flags.str.rs == RS_ON_2 || flags.str.rs == RS_ON_1 ) && 
                            ( cntTriesRS == 3 || ShuntShiftMotor_getFailure( ) != eNorm ) ) )
                    {    
                        dgn = eShiftFail;    // ������� ���������� �������� 
                    }
                }
                if( flags.str.rs == RS_OFF_2 || flags.str.rs == RS_OFF_1 )  // !!!���� ���, ������ ������� ����������
                {
                    flags.str.ctrl = 0;    // ����������
                    stateCnt = eStart;
                }
                else if( flags.str.rs == RS_ON_2 || flags.str.rs == RS_ON_1 )
                {
                    cntTriesRS++;
                    if ( ( cntTriesRS == 4 ) ||
                        ( temp && (cntTriesRS != 2 ) ) ||
                        ( flags.str.ctrl == 0 ) || ShuntShiftMotor_getFailure( ) != eNorm )
                    {
                       cntTriesRS = 0; 
                       flags.str.ctrl = 0;
                       stateCnt = eStart;
                    }    
                    else
                    {
                        dirInternal = !dirInternal; //�������� ����������� �������� ��� ����������� ��������
                        stateCnt = eInitPosDet;
                    }
                }    
            }
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
// �������� ��������� ���������
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
//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   02-02-2018
* �����  ��������� �.�.
* ���������:
*     ������� ������.
*
*
* ������ 1.0.2
* ����   23-10-2019
* �����  ��������� �.�.
* ���������:
* 	��������� ������ � ��������� ����������� �������� ������� � ������� 
* 	NineWireShuntShift_run: 
* 	- ������������ ��������� ������� "����"  ���� ������� �� ��������
* 	- ������������ ������������ ���� dgn � ���������� �������
* 
*
* ������ 1.0.3
* ���� 12-12-19 
* �����  ��������� �.�.
* ���������:
* 	����� ������� �� ����������� ��������� � ������ �������� �������
* 	� ������� NineWireShuntShift_run (�� ������� ��������� ���������
* 	������ ������ ���������� ������������ ����������), �� ���� ����
* 	���� dgn � ���������� ������� ����������� ������� 
*
*
* ������ 1.0.4
* ���� 23-04-20 
* �����  ��������� �.�.
* ���������:
* 	��������� �������������� ������� ��� ������������ ���� dgn � �������� eShiftNormal -
* 	������� ������� 24 � 220�
*/
