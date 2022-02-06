/**
* \file    ControlSystem.c
* \brief   \copybrief ControlSystem.h
*
* \version 1.0.10
* \date    7-07-2021
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <string.h>
#include <stdint.h>
#include "Main.h"
#include "ControlSystem.h"
#include "BinIn.h"
#include "DeviceAddress.h"
#include "LedFailure.h"
#include "CheckSupply.h"
#include "relayCtrl.h"
#include "ShuntShiftMotor.h"
#include "shuntShift.h"
#include "shuntShiftGen.h"
#include "Interchannel.h"  
#include "OverloadDet.h"
#include "IntegrCtrl.h"
#include "defCompil.h"
#include "posDetGenerator.h"
#include "CheckCallFunctions.h"

#include "shuntShiftNineWire.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define MAX_ORDER_SIZE          10                    ///< ������������ ����� �������.
#define NO_ACTION               0x0                   ///< ������ � ��������������� "�������� �������".
#define MINUS_POS               0x1                   ///< ������ � ��������������� "������� � ��������� -".
#define PLUS_POS                0x2                   ///< ������ � ��������������� "������� � ��������� +".
#define STOP_P                  0x3                   ///< ������ � ��������������� "��������� ���������"
#define UNDEFINED_ORDER         0x4                   ///< ����������� ������ (�������� �������, �� ������ ���������)
#define ERR2_NORMAL_STATE       0                     ///< ���� ������� ERR2=0 - ���������� ���������
#define IS_LOST_CONTR_WO_ORDER  1                     ///< ������ �������� ��� ������� �� �������
#define IS_OVL_CURR_MOTOR       2                     ///< ������� ���������� �� ����   
#define IS_EMULATOR             3                     ///< ������� ����, ��� ������ - ����� �������.
#define NUM_MS_IN_SEC           1000                  ///< ���������� ����������� � �������.
#define TO_PLUS                 true                  ///< �������� ��������� (��� �������� ������� � ��������� "����")
#define TO_MINUS                false                 ///< �������� ��������� (��� �������� ������� � ��������� "�����")
#define CURRENT_RATIO           29U                   ///< ����������� ��������� �������� ��� � ���.
#define RATIO_TO_100MA          1000                  ///< ����������� �����������.

//*****************************************************************************
/// \brief ����������� ����������� ������� �������� (��������������� �� ���� � 1/2 ���).
///
#define NUM_VALS_IN_SEC         2 

//*****************************************************************************
/// \brief ������������ ����� ��������, ������������ � �������.
///
#define MAX_TIME_SHIFT          15 * NUM_VALS_IN_SEC  

//*****************************************************************************
/// \brief �����, ����� ��������� "�����" ������� ��� ����������� �� � ����� ���������.
///
#define TIME_LED_ON_SC_BLINK    300

//*****************************************************************************
/// \brief �����, ����� ��������� "�����" �������� ��� ����������� �� � ����� ���������.
///
#define TIME_LED_OFF_SC_BLINK   1000

//*****************************************************************************
// ���������� ��������� �������������� ��������
//*****************************************************************************

static const uint8_t NUM_OF_PHASES = 3;    ///< ���������� ��� �������.

//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ������.
///
typedef struct ControlSystem_Order_
{
    uint8_t t1    :6;        ///< ��������������� ������������ ����� ��������
    uint8_t sw    :2;        ///< ��������������� ���������
    uint8_t act   :2;        ///< ������������ ���������� �� ��
    uint8_t resB2 :1;        ///< ����������������� ����
    uint8_t cm    :1;        ///< ����� ������ �������
    uint8_t swm   :2;        ///< �������� � �������
    uint8_t rs    :2;        ///< ��������� / ���������� ������ ���������� ��������, 1/2 �
    uint8_t resB3 :4;        ///< ����������������� ����
    uint8_t lev   :4;        ///< ������� ���������� ����������� ����� 
} ControlSystem_Order;

//*****************************************************************************
/// \brief ������.
///
typedef struct ControlSystem_Status_
{
    uint8_t t1        :6;                 ///< ������������� ������������ ����� ��������, 1/2 �.
    uint8_t st        :2;                 ///< ��������� ����������� ��������.
    uint8_t state2    :2;                 ///< ����������� ���������� �������.
    uint8_t state1    :2;                 ///< ����������� ��������� �������.
    uint8_t err1      :2;                 ///< ������� / ���������.
    uint8_t dgn       :2;                 ///< ���������� ���������� ��������.
    uint8_t t2        :6;                 ///< ����������� ����� ���������� ��������, 1/2 �.
    uint8_t err2      :2;                 ///< ������� / ���������.
    uint8_t currPhase [3];                ///< ������ �������� ���������� ��������� ���� �������� ��������������.
    uint8_t ph        :4;                 ///< ���� �������� �������
    uint8_t lev       :4;                 ///< ������� ���������� ����������� �����
} ControlSystem_Status;

//*****************************************************************************
/// \brief ���� "���� ��������" ���������� �������.
///
typedef enum
{
    eShiftNotActive,
    ePhaseFirstShiftFromPlusToMinus,
    ePhaseSecondShiftFromMinusToPlus,
    ePhaseThirdShiftFromPlusToMinus,
    ePhaseFourthShiftFromMinusToPlus,
    ePhaseCalibrateShiftFromPlusToMinus = 0x07,
    ePhaseCalibrateShiftFromMinusToPlus = 0x08,
    ePhaseFirstShiftFromMinusToPlus,
    ePhaseSecondShiftFromPlusToMinus,
    ePhaseThirdShiftFromMinusToPlus,
    ePhaseFourthShiftFromPlusToMinus,        
    ePhaseReservedFieldSecondBegin,
    ePhaseReservedFieldSecondEnd     = 0x0F
} PhaseOfShiftField;
    
//*****************************************************************************
/// \brief ��������� ��������� ��� ��������.
///
typedef enum
{
    eNotChanged,        ///< ��������� �� ����������
    eChanged,           ///< ��������� ����������
    eInProcess          ///< � �������� ��������
} PosDetChangedInShift;

typedef enum
{
    eNormalState,       ///< ���������� ���������
    eFrictionMode,      ///< ������ �� �������
    eShortCircuitMotor, ///< �������� ��������� � ���� ������� ����������������
    eBreakMotor         ///< ����� �������� � ���� ������� ����������������
}StatusFieldERR1_t;



//*****************************************************************************
/// \brief �������� ������.
///
typedef struct ControlSystem_ShortStatus_
{
    uint8_t reserved :4;        ///< ���������������.
    uint8_t state2   :2;        ///< ����������� ��������� �������.
    uint8_t state1   :2;        ///< ����������� ���������� �������.
} ControlSystem_ShortStatus;

//*****************************************************************************
/// \brief ��������� ��������� ������� �������� ��������.
///
typedef enum
{
    eShiftInPlus,         ///< ��������� "��������� ����"
    eShiftInMinus,        ///< ��������� "��������� �����"
    eShiftUnknown         ///< ��������� ��������� ����������
} ReqPosition;

//*****************************************************************************
/// \brief ������ ��������� ����������� "RS1" � "RS2".
///
typedef enum RsMode_
{
    RsMode_Active              = 1,                                       ///< ������ � �������� ���������
    RsMode_Connected           = 2,                                       ///< ���� ����� �� RS-422
    RsMode_Undefined           = 4,                                       ///< �� ����������
    RsMode_ActiveConnected     = RsMode_Active | RsMode_Connected,        ///< �������� ���������, ���� �����
    RsMode_PassiveConnected    = RsMode_Connected,                        ///< ��������� ���������, ���� �����
    RsMode_ActiveDisconnected  = RsMode_Active,                           ///< �������� ���������, ��� �����
    RsMode_PassiveDisconnected = 0                                        ///< ��������� ���������, ��� �����
} RsMode;
   
//*****************************************************************************
/// \brief ���� ����������� ��������
///
typedef enum 
{
    eNoActiveOrder,     ///< ��� ������� (��� �������������)
    eToPlus,            ///< ������� � "����"
    eToMinus,           ///< ������� � "�����"
}StateActiveOrder;

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static PositionDet_State           oldPosDet = eDetUndefinedState;         ///< ���������� ��������� �������.
static PositionDet_State           newPosDet;                              ///< ����� ��������� �������.
static ControlSystem_Order         currentOrder;                           ///< ������� ������.
static ControlSystem_Status        currentStatus;                          ///< ������� ������.
static ControlSystem_ShortStatus   currentShortStatus;                     ///< ������� �������� ������.
static bool                        rsEvent1 = false;                       ///< ������� ������� ������ ������� �� ����� "RS1".
static bool                        rsEvent2 = false;                       ///< ������� ������� ������ ������� �� ����� "RS2".
static ReqPosition                 reqPos = eShiftUnknown;                 ///< ��������� ��������� �������.
static PhaseOfShiftField           phaseOfShift = eShiftNotActive;         ///< ���� �������� �������.
static uint8_t                     previousFieldSW = UNDEFINED_ORDER;      ///< ���������� �������� ���� ST ������� �� ��
static StatesCheck220VInShuntShift statesCheck220VInShuntShift = eNoCheck;
static StatesCheckSupply220V       statesCheck220V = eOff;
static uint16_t                    timeProcOrder;
static StateActiveOrder            stateActiveOrder = eNoActiveOrder;
 static bool                        shiftDir;

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
///\brief �������, � ������� ������������ ��������� ���������� 220 � �� ����� �������� �������
static void managementShuntShiftWithout220V( void );

//*****************************************************************************
/// \brief ���������� ��������� ���� �������� ������� ������� ��� �� ��������.
/// \param cntPhaseShift - �������� �������� ���� ��������;
/// \return phaseOfShift - �������� ���� ��������.
///
static PhaseOfShiftField formFieldPhaseShift( uint8_t cntPhaseShift );

//*****************************************************************************
/// \brief ���������� ��������� ��������� ������� ��� �� ��������.
///
static PosDetChangedInShift isPosDetchangedInShift( void );

//*****************************************************************************
/// \brief ���������� ������ �������� ������� ��� ������� �� ������� �������.
///
static bool isPosDetChangedNoShift( PositionDet_State newPos );

//*****************************************************************************
/// \brief �������� ���������� ������ �������� ����������� �������.
///
static bool ControlSystem_enIntegrCtrl( void );

//*****************************************************************************
/// \brief ������������ ������ ��������� ����������� "RS1" � "RS2".
/// \param active - ���������� �������;
/// \param connected - ������� ����� � ��.
/// \return ����� ���������.
///
static uint16_t rsIndicationMode( bool active, bool connected );

//*****************************************************************************
/// \brief ���������� ������������ "RS1" � "RS2".
/// \param ind - ��������� �� ��������� ������ ����������;
/// \param mode - ��������� �� ������� ����� ���������;
/// \param modeNew - ����� ����� ���������;
/// \param event - ������� ������� ������ �� ��������������� ����� RS-422.
///
static void rsIndication( indOutput_type *ind, uint16_t *mode, uint16_t modeNew, bool event );

//*****************************************************************************
/// \brief ���������� ����������.
///
static void ControlSystem_indicationRun( void );

//*****************************************************************************
/// \brief ����� � ��.
///
static void ControlSystem_exchangeRun( void );

//*****************************************************************************
/// \brief ���������� �����������.
///
static void ControlSystem_activityManagerRun( void );

//*****************************************************************************
/// \brief ����������� ���������.
///
static void ControlSystem_shuntShiftPosDetRun( void );

//*****************************************************************************
/// \brief ��������� ����������� �� �� ��������.
///
static void ControlSystem_processingOrders( void );

//*****************************************************************************
///\brief �������, � ������� ������������ �������� ���������� ���������� �������
///� ������, ���� ������� ������ �� ����� �����������
static bool checkEnableProcessingOrder( void );


//*****************************************************************************
///\brief �������, � ������� ������������ ������� ������������ ������� ���������� ������� �� ������� �������
///�� ���� ���� dgn = eShiftActive
///
static void countTimeProcessingOrder( void );

//*****************************************************************************
///\brief �������, ������������ ������� �� ����� ���������� �������� �������
///
//static bool isRepeatShuntShiftEn( void )
//{
//    return currentOrder.rs == 2; 
//}

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ������ ������ ����������
void ControlSystem_ctor( void )
{
    currentStatus.state1 = 2; //�������� �������� ������
    currentStatus.state2 = 2; //�������� ��������� ������
}

//*****************************************************************************
// ������ ������ ������ ����������
void ControlSystem_run( void )
{
    ControlSystem_exchangeRun( );           // ����� � ����������� �������� (������������ ��������, �������)
    ControlSystem_indicationRun( );         // ��������� ����������� RS1, RS2
    ControlSystem_shuntShiftPosDetRun( );   // ��������� ���� ������ "����-5-��" � ����������� �� ��������� ������� 
    ControlSystem_processingOrders( );      // ��������� ����������� �� �� ��������
    ControlSystem_activityManagerRun( );    // ���������� ����������� �������   
    MARKED_CALL_FUNCTION;
}

//*****************************************************************************
// �������� ������� �������� ������
bool ControlSystem_workCondition( void )
{
    return ( CheckSupply_is220vOn( ) ) &&
            CheckSupply_is24vOn( ) &&
            !Rs422_getNoExchangeCtrlSystem( rs422 ) &&
            RelayCtrl_getRpvState( ) == eNormal;
}

//��������� ������� ��������� ������� �� ������ �������
void ControlSystem_setTimeProcessingOrder( uint16_t value)
{
    timeProcOrder = value;
}

//�������� ������������ ����� ��������
uint16_t ControlSystem_getTimeCurrentOrder( void )
{
    return currentOrder.t1 * NUM_MS_IN_SEC / NUM_VALS_IN_SEC;
}

//�������� ��������� �������� ��������� ������� managementShuntShiftWithout220V
StatesCheckSupply220V ControlSystem_getStateCheck220V( void )
{
    return statesCheck220V;
} 

//�������� ��������� �������� 220� �� ������� managementShuntShiftWithout220V
StatesCheck220VInShuntShift ControlSystem_getStatesCheck220VInShuntShift( void ) 
{
    return statesCheck220VInShuntShift;
}

//�������� �������� ������� ��������(������������ ��� ���������� � ������������� ��������)
uint16_t ControlSystem_getTimeProcOrder( void )
{
    return timeProcOrder;
}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

///\brief �������, � ������� ������������ ������� ������������ ������� ���������� ������� �� ������� �������
///�� ���� ���� dgn = eShiftActive (������ ������� ���������� �� ������ �������� ���������)
static void countTimeProcessingOrder( void )
{
    enum StatesCountTime_t
    {
        eWaitingForLossControl,
        eCount
    };
    static enum StatesCountTime_t statesCountTime = eWaitingForLossControl;
    
    if( ShuntShift_getDgn() == eShiftActive )
    {
        switch( statesCountTime )
        {
            case eWaitingForLossControl:
                if(PositionDet_getPosition() == eDetNon)
                    statesCountTime = eCount;
                break;
            case eCount:
                timeProcOrder++;
                break;
        }
    }
    else
        statesCountTime = eWaitingForLossControl;
}


//*****************************************************************************
///\brief �������, � ������� ������������ ��������� ���������� 220 � �� ����� �������� �������
static void  managementShuntShiftWithout220V( void )
{
    static const uint16_t TIME_CONTROL_220V = 2700U;   //����� ����� �������� ���������� ���������� 220 �
    static const uint16_t TIMEOUT_FAIL = 300U;          //�����, ����������� ��� �������� ������� � ��
    static const uint16_t TIMEOUT_RESTART_SHUNT_SHIFT = 150U;
    static const uint16_t TIMEOUT_RESTART_TIME = 2150U;
    static       uint16_t timeControl220V = 0, t_o = 0;
    uint16_t tempTime;
    
    
    if( t_o ) //������� ���������
        t_o--;
    
    tempTime = currentOrder.t1 * NUM_MS_IN_SEC / NUM_VALS_IN_SEC;
    
    switch( statesCheck220V )
    {
        case eOff: //0
            if( statesCheck220VInShuntShift == eNo220VInShuntShift )    //������� 220 � �� ����� ��������
            {    
                statesCheck220V = eCount;
            }
            else if ( ShuntShift_isEnable( ) )
            {    
                statesCheck220VInShuntShift = eNoCheck;
            }
            else {}
                break;
        case eCount: //1
            if( CheckSupply_is220vOn( ) ) 
            {
                statesCheck220VInShuntShift = eNoCheck;
                previousFieldSW = UNDEFINED_ORDER;
                //!!!��� ���� �������� ����� �� ������ �������� �������� ������� ��� ���������� 220�
                //220� ��������������, ���������� ������� ������� (c ������ ������� ��������� ���������� �������� 
                //� ������� ��� ���������� 220�)
                ShuntShift_turnOn( shiftDir, 
                                   tempTime - timeProcOrder - timeControl220V + TIMEOUT_RESTART_TIME,
                                   currentOrder.cm, currentOrder.rs );
                previousFieldSW = currentOrder.sw; 
                timeControl220V = 0;
                t_o = TIMEOUT_RESTART_SHUNT_SHIFT;
                statesCheck220V = eTimeout2;
            }    
            //������� ������� ����������� ��������, ����
            //������� ��������� �������� ������� ���������� 220� ��� ������� ����� �1, ���������� � �������
            else if ( ( timeControl220V++ >= TIME_CONTROL_220V ) || 
                        timeProcOrder >= tempTime || 
                       ((PositionDet_getPosition( ) == eDetMinus && shiftDir == TO_MINUS )
                        || ( PositionDet_getPosition( ) == eDetPlus && shiftDir == TO_PLUS ) ) )
            {
                statesCheck220V = eFinishShuntShift;
                timeControl220V = 0; 
            }
            break;
        case eFinishShuntShift: //2
            previousFieldSW = currentOrder.sw;
            statesCheck220VInShuntShift = eShuntShiftIsFailed; 
            if ( (PositionDet_getPosition( ) == eDetMinus && shiftDir == TO_MINUS )
                 || ( PositionDet_getPosition( ) == eDetPlus && shiftDir == TO_PLUS ) )
            {    
                ShuntShift_setDgn( eShiftNormal );  //���������� ������� ��������� ��������� ��������
            }
            else    
            {    
                ShuntShift_setDgn( eShiftFail );    //���������� ������� ���������� ��������
            }
            t_o = TIMEOUT_FAIL;
            statesCheck220V = eTimeout1;    
            break;
        case eTimeout1:  //3  
        case eTimeout2:  //4 
            if( t_o == 0 )
            {
                statesCheck220V = eOff;
            }
            break;
        }
}
 
//*****************************************************************************
//���������� ��������� "��������" ���� �������� ������� ��� �� ��������.
static PhaseOfShiftField formFieldPhaseShift( uint8_t cntPhaseShift )
{
    static const uint8_t INIT_VAL_CNT_PHASE_SHIFT = 0;
      
    if( ShuntShift_isEnable( ) )
    {
        if( currentOrder.sw == MINUS_POS || currentOrder.sw == NO_ACTION )
        {
            if( currentOrder.rs == eRouteShuntShift || currentOrder.rs == eRouteRepeatShuntShift )
                phaseOfShift = ePhaseFirstShiftFromPlusToMinus + cntPhaseShift; 
            else if( currentOrder.rs == eRouteCalibrateShuntShift || currentOrder.rs == eRouteRepeatCalibrateShuntShift )
                phaseOfShift = ePhaseCalibrateShiftFromPlusToMinus;
            else {}     //���� ��� ��� ������ ��������� ���� RS, ��... �� ����� ���� :)
        }
        else if( currentOrder.sw == PLUS_POS || currentOrder.sw == NO_ACTION )
        {
            if( currentOrder.rs == eRouteShuntShift || currentOrder.rs == eRouteRepeatShuntShift )
                phaseOfShift = ePhaseFirstShiftFromMinusToPlus + cntPhaseShift; 
            else if( currentOrder.rs == eRouteCalibrateShuntShift || currentOrder.rs == eRouteRepeatCalibrateShuntShift )
                phaseOfShift = ePhaseCalibrateShiftFromMinusToPlus;
            else{}
        }    
        else {}
    }
    else
    {
        phaseOfShift = eShiftNotActive;
        ShuntShift_setCntPhaseShift( INIT_VAL_CNT_PHASE_SHIFT );
    }
    return phaseOfShift;
}

//*****************************************************************************
//�������, � ������� ������������ �������� ���������� ���������� �������
//� ������, ���� ������� ������ �� ����� �����������
static bool checkEnableProcessingOrder( void )
{
    if( ActivityManager_isActive( activityManager ) && ShuntShift_isWorkMode( ) )
    {
        if( currentOrder.sw == previousFieldSW )
            return false;
        previousFieldSW = currentOrder.sw; 
        return true;
    }
    else
    {    
        previousFieldSW = UNDEFINED_ORDER;
        return false;
    }
}

//*****************************************************************************
 //��������� ����������� �� �� ��������
static void ControlSystem_processingOrders( void )
{
    managementShuntShiftWithout220V( );
    countTimeProcessingOrder( );
    
    // ���������� ������� � ���������� �������� ����������� �������
    if( CheckSupply_is24vOn( )
        && CheckSupply_is220vOn( )
        && RelayCtrl_getRpvState( ) == eNormal
        && ( InterChannel_getData( eICId_IntegrCtrlScAndBreak ) == eNorm || 
            InterChannel_getData( eICId_IntegrCtrlScAndBreak ) == eOverloadCircuit  ) )
    {
        if( currentOrder.sw == MINUS_POS  && 
          ( PosDetGenerator_isReadyForUse( ) || OverloadDet_isScContrGen( ) ) )
        { // ������ �� ����������� � "�����"
            if( PositionDet_getPosition( ) == eDetMinus  || ( previousFieldSW == currentOrder.sw && !ShuntShift_isEnable( ) ) )
            {
                previousFieldSW = currentOrder.sw;      
                IntegrCtrl_turnOn( currentOrder.cm, PositionDet_getPosition( ) );
            }
            else
            {    
                IntegrCtrl_turnOff( ); //��������� �������� ����������� �������
            }
            if( !IntegrCtrl_isOn( ) )
            {
                //��� ������� �������� � ���� �������� ������� �� ������� � ����� (������� "�� ����")
                if( reqPos == eShiftInPlus )
                {
                    ShuntShift_turnOff( ); //��������� ������� �������
                    if( !ShuntShift_isEnable( ) ) 
                    {
                        reqPos = eShiftInMinus;
                    }
                }
                else
                {
                    if( checkEnableProcessingOrder( ) )
                    {
                        ShuntShift_turnOn( TO_MINUS, currentOrder.t1 * NUM_MS_IN_SEC / NUM_VALS_IN_SEC, 
                                       currentOrder.cm, currentOrder.rs );
                        shiftDir = TO_MINUS;
                        stateActiveOrder = eToMinus;
                    }
                    reqPos = eShiftInMinus;
                }
            }
        }
        else if( currentOrder.sw == PLUS_POS && 
                ( PosDetGenerator_isReadyForUse( ) || OverloadDet_isScContrGen( ) ) )
        { // ������ �� ����������� � "����"
            if( PositionDet_getPosition( ) == eDetPlus || ( previousFieldSW == currentOrder.sw && !ShuntShift_isEnable( ) ) )
            {
                previousFieldSW = currentOrder.sw;         
                IntegrCtrl_turnOn( currentOrder.cm, PositionDet_getPosition( ) );
            }
            else
            {
                IntegrCtrl_turnOff( );
            }
            if( !IntegrCtrl_isOn( ) )
            {
                if( reqPos == eShiftInMinus )
                {
                    ShuntShift_turnOff( );
                    if( !ShuntShift_isEnable( ) ) 
                    {
                        reqPos = eShiftInPlus;
                    }
                }
                else
                {
                    if( checkEnableProcessingOrder( ) )
                    {
                        ShuntShift_turnOn( TO_PLUS, currentOrder.t1 * NUM_MS_IN_SEC / NUM_VALS_IN_SEC, 
                                       currentOrder.cm, currentOrder.rs );
                        shiftDir = TO_PLUS;
                        stateActiveOrder = eToPlus;
                    }
                    reqPos = eShiftInPlus;
                }
            }
        }
        else if( currentOrder.sw == STOP_P && ActivityManager_isActive( activityManager ) )
        { // ������ "������� ���������"
            if( ShuntShift_isEnable( ) )
            {
                ShuntShift_turnOff( );
            }
            IntegrCtrl_turnOn( currentOrder.cm, PositionDet_getPosition( ) );
            previousFieldSW = currentOrder.sw;
            reqPos = eShiftUnknown;
        }
        else if( currentOrder.sw == NO_ACTION )
        { // ������ ������ (�.� �������� �������)
            if( statesCheck220V == eTimeout2 )
            {
                IntegrCtrl_turnOff( );
                if( !IntegrCtrl_isOn( ) )
                {
                    if( stateActiveOrder == eToPlus )
                    {
                        ShuntShift_turnOn( TO_PLUS, currentOrder.t1 * NUM_MS_IN_SEC / NUM_VALS_IN_SEC, 
                                       currentOrder.cm, currentOrder.rs );
                        shiftDir = TO_PLUS;
                        statesCheck220V = eOff;
                    }
                    else if( stateActiveOrder == eToMinus )
                    {
                        ShuntShift_turnOn( TO_MINUS, currentOrder.t1 * NUM_MS_IN_SEC / NUM_VALS_IN_SEC, 
                                       currentOrder.cm, currentOrder.rs );
                        shiftDir = TO_MINUS;
                        statesCheck220V = eOff;
                    }
                }
            } 
            else if( !ShuntShift_isEnable( ) ) 
            {
               IntegrCtrl_turnOn( currentOrder.cm, PositionDet_getPosition( ) );
            }
            previousFieldSW = currentOrder.sw;  
        }
    }
    else
    {
        if( ShuntShift_isEnable( ) ) 
        {
            if( CheckSupply_is220vOn() == false ) //������� 220� ��� �������� �������
            {    
                statesCheck220VInShuntShift = eNo220VInShuntShift;
                ShuntShift_turnOff( );
            }
        }
        else if( ControlSystem_enIntegrCtrl( ) )
        {
            IntegrCtrl_turnOn( currentOrder.cm, PositionDet_getPosition( ) );
        }
        else
        {    
            IntegrCtrl_turnOff( );
        }
        reqPos = eShiftUnknown;
    }
}

//*****************************************************************************
// �������� ���������� ������ �������� ����������� �������
static bool ControlSystem_enIntegrCtrl( void )
{
    return CheckSupply_is220vOn( ) &&
           CheckSupply_is24vOn( ) &&
           RelayCtrl_getRpvState( ) == eNormal ;
}

//*****************************************************************************
// ������������ ������ ��������� ����������� "RS1" � "RS2"
static uint16_t rsIndicationMode( bool active, bool connected )
{
    uint16_t r = 0;
    if( active )
    {
        r |= RsMode_Active;
    }
    if( connected )
    {
        r |= RsMode_Connected;
    }
    return r;
}

//*****************************************************************************
// ���������� ������������ "RS1" � "RS2"
static void rsIndication( indOutput_type *ind, uint16_t *mode, uint16_t modeNew, bool event )
{
    if( *mode != modeNew )
    {
        switch( modeNew )
        {
            case RsMode_ActiveConnected:
                Indication_off( ind );
                break;
            case RsMode_ActiveDisconnected:
                Indication_blink2( ind, 300, 300, 300, 1100, eIndPhNormal );
                break;
            case RsMode_PassiveConnected:
                Indication_blink( ind, 300, 300, eIndPhNormal );
                break;
            case RsMode_PassiveDisconnected:
                Indication_blink( ind, 300, 1700, eIndPhNormal );
                break;
        }
        *mode = modeNew;
    }
    if( modeNew == RsMode_ActiveConnected && event )
    {
        Indication_pulseOn( ind, 50 );
    }
}

//*****************************************************************************
//���������� ��������� ��������� ������� ��� � ��������
static PosDetChangedInShift isPosDetchangedInShift( void )
{
    if( ShuntShift_getDgn( ) == eShiftActive )
    {
        if( oldPosDet != newPosDet )
        {
            oldPosDet = newPosDet;
            return eChanged;
        }
        else 
            return eInProcess;
    }
    else
    {
        oldPosDet = eDetUndefinedState;
        return eNotChanged;
    }
}

//*****************************************************************************
// ���������� ������ �������� ������� ��� ������� �� ������� �������
static bool isPosDetChangedNoShift( PositionDet_State newPos )
{
    static const uint16_t TIME_STABLE_STATE = 50;
    static PositionDet_State oldPos = eDetUndefinedState;
    static uint16_t cnt;
    
    // ���� ������� ������� �������� ��� �������, �� ��� ������� �� ������� � �����-���� ���������
    if( !ShuntShift_isEnable( ) && 
        IntegrCtrl_isOn( ) &&  
        PosDetGenerator_isEnable() &&
        !OverloadDet_getScContrGenTrig() && 
        PosDetGenerator_isReadyForUse() )
    {
        if( newPos == eDetMinus || newPos == eDetPlus ) //��������� ������� ��������� �������
        {
            oldPos = newPos;  //� �������������� ���������� ���������
            cnt = 0;
            return false;
        }
        if( ( oldPos == eDetMinus || oldPos == eDetPlus ) && newPos == eDetNon )
        {    
            if ( cnt < TIME_STABLE_STATE )
            {
                cnt++;
                return false; 
            }
            return true;        // ��������� ������ �������� ��� ������� �������� �������
        }
        oldPos = eDetUndefinedState;
        return false;
    }
    else 
    {
        cnt = 0;
        oldPos = eDetUndefinedState;
        return false;
    }
}

//*****************************************************************************
// ���������� ����������
static void ControlSystem_indicationRun( void )
{
#ifdef IND_SHORT_CIRCUIT
    if( (OverloadDet_isScThreePhaseGen( )
        || OverloadDet_isScContrGen( )
        || InterChannel_getData( eICId_IntegrCtrlScAndBreak ) == eShortCircuit) 
        && ActivityManager_isActive( activityManager ) 
        && CheckSupply_is220vOn( ) 
        && CheckSupply_is24vOn( ) )
       //��� ������� �� ��������� "�����" ������ � ������ �������
    {        
        OverloadDet_isScContrGen( ) ? LedFailure_blink( TIME_LED_ON_SC_BLINK, TIME_LED_OFF_SC_BLINK ) 
                                    : LedFailure_blink( TIME_LED_ON_SC_BLINK, TIME_LED_ON_SC_BLINK );
    }
    else
    {
        if( CheckSupply_is220vOn( ) && CheckSupply_is24vOn( ) ) 
            LedFailure_set( false ); //��������� "�����" ��������
        else 
            LedFailure_set( true ); //��������� "�����" �������
    }
#else
    if( CheckSupply_is220vOn( ) && CheckSupply_is24vOn( ) ) LedFailure_set( false ); //��������� "�����" ��������
    else LedFailure_set( true ); //��������� "�����" �������
#endif
    // Rs1, Rs2
    bool active = ActivityManager_isActive( activityManager );
    static uint16_t
    rsMode1 = RsMode_Undefined,
            rsMode2 = RsMode_Undefined;
    uint16_t
    rsNewMode1 = rsIndicationMode( active, !Rs422_getNoLineExchange( rs422, eRs422_line_1 ) ),
            rsNewMode2 = rsIndicationMode( active, !Rs422_getNoLineExchange( rs422, eRs422_line_2 ) );
    if( !( rsNewMode1 & RsMode_Connected ) && ( rsNewMode1 == rsNewMode2 ) && ( rsNewMode1 != rsMode1 || rsNewMode2 != 
        rsMode2 ) )
    {
        rsMode1 = RsMode_Undefined;
        rsMode2 = RsMode_Undefined;
    }
    rsIndication( &ledRs1, &rsMode1, rsNewMode1, rsEvent1 | Rs422_getNewOrderEventLine( rs422, eRs422_line_1 ) );
    rsIndication( &ledRs2, &rsMode2, rsNewMode2, rsEvent2 | Rs422_getNewOrderEventLine( rs422, eRs422_line_2 ) );
    if( active ) //��������� ����������� ��������� ������� ������ �� �������� �������
    {
        newPosDet = PositionDet_getPosition( );
            switch( newPosDet )
            {
                case eDetNon: //"������ ��������" ��� "������� + � -"
                case eDetUndefinedState:    
                    isPosDetchangedInShift( ) == eChanged ? Indication_blink( &ledLossCtrl, 100, 100, eIndPhNormal ) :
                            isPosDetchangedInShift( ) == eInProcess ? : Indication_on( &ledLossCtrl );
                    Indication_off( &ledPlusPos );
                    Indication_off( &ledMinusPos );
                    break;

                case eDetPlus: //"��������� ����"        
                    isPosDetchangedInShift( ) == eChanged ? Indication_blink( &ledPlusPos, 100, 100, eIndPhNormal ) :
                            isPosDetchangedInShift( ) == eInProcess ? : Indication_on( &ledPlusPos );
                    Indication_off( &ledLossCtrl );
                    break;

                case eDetMinus: //"��������� �����"
                    isPosDetchangedInShift( ) == eChanged ? Indication_blink( &ledMinusPos, 100, 100, eIndPhNormal ) :
                            isPosDetchangedInShift( ) == eInProcess ? : Indication_on( &ledMinusPos );
                    Indication_off( &ledPlusPos );
                    Indication_off( &ledLossCtrl );
                    break;

                default:
                    Indication_off( &ledLossCtrl );
                    Indication_off( &ledPlusPos );
                    Indication_off( &ledMinusPos );
                    oldPosDet = eDetUndefinedState;
            }
    }
    else
    {
        Indication_off( &ledLossCtrl );
        Indication_off( &ledPlusPos );
        Indication_off( &ledMinusPos );
    }
}

//*****************************************************************************
// ����� � ��
static void ControlSystem_exchangeRun( void )
{

    rsEvent1 = Rs422_getLineEvent( rs422, eRs422_line_1 );
    rsEvent2 = Rs422_getLineEvent( rs422, eRs422_line_2 );
    if( Rs422_getNoExchangeCtrlSystem( rs422 ) )
    {
        memset( &currentOrder, 0, sizeof( currentOrder ) );
    }
    else if( rsEvent1 || rsEvent2 )
    {
        uint8_t rawOrder[MAX_ORDER_SIZE];
        uint8_t orderSize = Rs422_getOrder( rs422, rawOrder );
        if( orderSize == sizeof( currentOrder ) )
        {
            memcpy( &currentOrder, rawOrder, sizeof( currentOrder ) );
        }
    }
    if( DeviceAddress_isPrimary( ) )
    {
        currentStatus.state1 = currentShortStatus.state1 = 2;
        currentStatus.state2 = currentShortStatus.state2 = BlockExch_getConnectionState( blockExchange ) ? 2 : 1;
    }
    else
    {
        currentStatus.state1 = currentShortStatus.state1 = BlockExch_getConnectionState( blockExchange ) ? 2 : 1;
        currentStatus.state2 = currentShortStatus.state2 = 2;
    }
    //����������� �������� ������� ��������
    if( currentOrder.t1 == 0 || currentOrder.t1 > MAX_TIME_SHIFT ) 
        currentOrder.t1 = MAX_TIME_SHIFT;
    //���������� ��������� ��������
    if( ActivityManager_isActive( activityManager ) )
    {
        if ( ShuntShift_getDgn( ) != eShiftActive  && !ShuntShift_isEnable( ) ) //������ � ������� �1(� 1/2 �) � �2(� 1/128 �)
        {
            static const uint16_t RATIO = 100 / NUM_VALS_IN_SEC;
            currentStatus.t1 = __builtin_divud( __builtin_muluu( timeProcOrder, 16 ), 125 ) & 0x3F;
            currentStatus.t2 = __builtin_divud( timeProcOrder, RATIO * 10 );
            if( __builtin_divud( timeProcOrder, RATIO ) % 10 > 5 )
                currentStatus.t2 += 1;  //���������� ��� ��������� ��������
        }
        currentStatus.st = PositionDet_getPosition( );
        
        //��������� ���� ������� ERR1
        if( InterChannel_getData( eICId_IntegrCtrlScAndBreak ) == eShortCircuit )
            currentStatus.err1 = eShortCircuitMotor;
        else if( InterChannel_getData( eICId_IntegrCtrlScAndBreak ) == eBreakOnePhase ||
                InterChannel_getData( eICId_IntegrCtrlScAndBreak ) == eBreakAllPhases )
            currentStatus.err1 = eBreakMotor;
        else if( ( ShuntShift_getDgn( ) == eShiftActive && ( timeoutForAverageInWindow == 0 ) ) && 
                ( currentOrder.rs == eRouteShuntShift || currentOrder.rs == eRouteRepeatShuntShift ) &&
                ( ( shiftDir == TO_PLUS && ( ShuntShiftMotor_getAverageThreePhasesCurrent( ) > DeviceAddress_getCalibrCurrent( Plus ) ) ) ||
                ( shiftDir == TO_MINUS && ( ShuntShiftMotor_getAverageThreePhasesCurrent( ) > DeviceAddress_getCalibrCurrent( Minus ) ) ) ) )
            currentStatus.err1 = eFrictionMode;
        else
            currentStatus.err1 = eNorm;
        
        currentStatus.dgn = ShuntShift_getDgn( );
        if( currentOrder.swm )
        {
            currentStatus.err2 = IS_EMULATOR;
        }
        else 
        {
            if( ShuntShiftMotor_getIsOvlCurr( ) )    
                currentStatus.err2 = IS_OVL_CURR_MOTOR;
            else if( isPosDetChangedNoShift( currentStatus.st ) )
            {
                ShuntShiftMotor_setIsOvlCurr( false );
                currentStatus.err2 = IS_LOST_CONTR_WO_ORDER;
            }
            else 
                currentStatus.err2 = ERR2_NORMAL_STATE;
        }
        const uint8_t IDcurrPhases[] = { eICId_IV_rms, eICId_IU_rms, eICId_IW_rms };
        for( uint8_t cnt = 0; cnt < NUM_OF_PHASES; cnt++ )
        {
            uint16_t temp = InterChannel_getData( IDcurrPhases[cnt] );
            //������� �� ������� ��� � ��� � ����������� 0.1�
            currentStatus.currPhase[cnt] = __builtin_divud( __builtin_muluu( temp, CURRENT_RATIO ), RATIO_TO_100MA );
        }
//        currentStatus.currPhase[0] = __builtin_divud( __builtin_muluu( middleCurrShuntShift, CURRENT_RATIO ), RATIO_TO_100MA );  
//        currentStatus.currPhase[1] = __builtin_divud( __builtin_muluu( middleCurrFriction, CURRENT_RATIO ), RATIO_TO_100MA );
//        currentStatus.currPhase[2] = __builtin_divud( __builtin_muluu( thresholdFriction, CURRENT_RATIO ), RATIO_TO_100MA );       
        currentStatus.lev = PosDetGenerator_getLevel( );
        currentStatus.ph = formFieldPhaseShift( ShuntShift_getCntPhaseShift());
        Rs422_setStatus( rs422, ( uint8_t * ) &currentStatus, sizeof currentStatus );
    }
    else if( DeviceAddress_getSysType( ) == eEbilock )
    {
        Rs422_setStatus( rs422, 0, 0 );
    }
    else
    {
        Rs422_setStatus( rs422, ( uint8_t * ) &currentShortStatus, sizeof currentShortStatus );
    }
    // ������������ �������.
    // �������������� ����.
    // ���������� � ����� 3-������� ���������� ������� (0x60).
    if( (OverloadDet_getScThreePhaseGenTrig( ) || IntegrCtrl_getFailure( ) == eShortCircuit )
         && CheckSupply_is220vOn( ) && CheckSupply_is24vOn( ) )
        Rs422_addAlarm( rs422, eAlarmShortCircuitThreePhaseGen );
    else
        Rs422_removeAlarm( rs422, eAlarmShortCircuitThreePhaseGen );
    // ���������� � ����� ������������ ���������� ������� (0x6E).
    if( OverloadDet_getScContrGenTrig( ) && CheckSupply_is24vOn( ) )
        Rs422_addAlarm( rs422, eAlarmShortCircuitContrGen );
    else
        Rs422_removeAlarm( rs422, eAlarmShortCircuitContrGen );
    // ���������� ��������� ������� �220 » �� � ����� (0x65).
    if( CheckSupply_is220vOn( ) )
        Rs422_removeAlarm( rs422, eAlarmNoSupplyVoltage220V );
    else
        Rs422_addAlarm( rs422, eAlarmNoSupplyVoltage220V );
    // ����������� ����.
    // ���������� ��������� ������� �24 » �� � ����� (0xF0).
    if( CheckSupply_is24vOn( ) )
        Rs422_removeAlarm( rs422, eAlarmNoSupplyVoltage24V );
    else
        Rs422_addAlarm( rs422, eAlarmNoSupplyVoltage24V );
    // ���������� ����������� ������� �� ����� ����� 1 ������ 1,5 � (0xF1).
    if( Rs422_getNoLineExchangeCtrlSystem( rs422, eRs422_line_1 ) )
        Rs422_addAlarm( rs422, eAlarmNoFirstLineExchCtrlSys );
    else
        Rs422_removeAlarm( rs422, eAlarmNoFirstLineExchCtrlSys );
    // ���������� ����������� ������� �� ����� ����� 2 ������ 1,5 � (0xF2).
    if( Rs422_getNoLineExchangeCtrlSystem( rs422, eRs422_line_2 ) )
        Rs422_addAlarm( rs422, eAlarmNoSecondLineExchCtrlSys );
    else
        Rs422_removeAlarm( rs422, eAlarmNoSecondLineExchCtrlSys );
    if( ActivityManager_getNeighborRS422connect( activityManager ) == false
    && ActivityManager_getNeighborConnection( activityManager ) )
        Rs422_addAlarm( rs422, eAlarmNoValidOrderToNeighbor );
    else
        Rs422_removeAlarm( rs422, eAlarmNoValidOrderToNeighbor );
    // ������� ������ ��������� � ���������� ��������� (0xF4).
    static uint16_t cntDelayTransitToSafe; 
    static const uint16_t DELAY_TRANSIT_TO_SAFE_MS = 300; 
    if( !ActivityManager_isNeighborWorking( activityManager )
        && ActivityManager_getNeighborConnection( activityManager ) )
    {
        //������ ��� ����, ����� �� �������� ����� � �� ����� ���������� ������ ������
        //� ��� ������������� �������� ��������� ������ ������ � ��, ��� � ������� �� ����������� ������
        if ( ++cntDelayTransitToSafe >= DELAY_TRANSIT_TO_SAFE_MS )
        {
            Rs422_addAlarm( rs422, eAlarmNeighborInSafeMode );
        }
    }
    else
    {   
        cntDelayTransitToSafe = 0;    
        Rs422_removeAlarm( rs422, eAlarmNeighborInSafeMode );
    }
        //�� ���������� ���� ���
    if( RelayCtrl_getRpvState( ) == eNotTurnOn )
        Rs422_addAlarm( rs422, eAlarmRPVnotTurnOn );       
    else
        Rs422_removeAlarm( rs422, eAlarmRPVnotTurnOn );
    //�� ����������� ���� ���
    if( RelayCtrl_getRpvState( ) == eNotTurnOff )
        Rs422_addAlarm( rs422, eAlarmRPVnotTurnOff );       
    else
        Rs422_removeAlarm( rs422, eAlarmRPVnotTurnOff );
    //��� ����� � �������� �������� (��� ������������� ������ �����)
    if ( ( BlockExch_getConnectionState( blockExchange ) && ActivityManager_isNeighborHearsMe( activityManager ) )
            || !BlockExch_getConnectionState( blockExchange ) )
        Rs422_removeAlarm( rs422, eAlarmNoConnectWithNeighbor );
    if ( BlockExch_getConnectionState( blockExchange ) && !ActivityManager_isNeighborHearsMe( activityManager ) )
        Rs422_addAlarm( rs422, eAlarmNoConnectWithNeighbor );
    //���������� ��������� ������� ����������� �����
    if( PosDetGenerator_isGeneratorFail( ) )
        Rs422_addAlarm( rs422, eAlarmControlGeneratorIsFail );
    else
        Rs422_removeAlarm( rs422, eAlarmControlGeneratorIsFail );
    if( statePhaseForAlarm[0] == eShortCircuit )
        Rs422_addAlarm( rs422, eAlarmOverloadPhaseU );
    else if( statePhaseForAlarm[0] == eBreakOnePhase )
        Rs422_addAlarm( rs422, eAlarmBreakPhaseU );
    else if ( statePhaseForAlarm[0] == eNorm )
    {
        Rs422_removeAlarm( rs422, eAlarmOverloadPhaseU );
        Rs422_removeAlarm( rs422, eAlarmBreakPhaseU ); 
    }    
    if( statePhaseForAlarm[1] == eShortCircuit )
        Rs422_addAlarm( rs422, eAlarmOverloadPhaseV );
    else if( statePhaseForAlarm[1] == eBreakOnePhase )
        Rs422_addAlarm( rs422, eAlarmBreakPhaseV );
    else if ( statePhaseForAlarm[1] == eNorm )
    {
        Rs422_removeAlarm( rs422, eAlarmOverloadPhaseV );
        Rs422_removeAlarm( rs422, eAlarmBreakPhaseV ); 
    }    
    if( statePhaseForAlarm[2] == eShortCircuit )
        Rs422_addAlarm( rs422, eAlarmOverloadPhaseW );
    else if( statePhaseForAlarm[2] == eBreakOnePhase )
        Rs422_addAlarm( rs422, eAlarmBreakPhaseW );
    else if ( statePhaseForAlarm[2] == eNorm )
    {
        Rs422_removeAlarm( rs422, eAlarmOverloadPhaseW );
        Rs422_removeAlarm( rs422, eAlarmBreakPhaseW ); 
    }    
    if( DeviceAddress_getIsCalibrPresents( Minus ) == true )
        Rs422_removeAlarm( rs422, eAlarmNeedCalibrateShiftToMinus ); 
    else    
        Rs422_addAlarm( rs422, eAlarmNeedCalibrateShiftToMinus );
    if( DeviceAddress_getIsCalibrPresents( Plus ) == true )
        Rs422_removeAlarm( rs422, eAlarmNeedCalibrateShiftToPlus ); 
    else    
        Rs422_addAlarm( rs422, eAlarmNeedCalibrateShiftToPlus );
    //�������� ������ ������������ ������� �������� �������
    if( ( ShuntShift_getDgn( ) == eShiftNormal || ShuntShift_getDgn( ) == eShiftSecondTry ) &&
        ( currentOrder.rs == eRouteShuntShift || currentOrder.rs == eRouteRepeatShuntShift ) &&     
        ( timeProcOrder > DeviceAddress_getIncreasedTime( Plus ) || timeProcOrder > DeviceAddress_getIncreasedTime( Minus ) ) )
        Rs422_addAlarm( rs422, eAlarmIncreasedTimeShuntShift );
    else
        Rs422_removeAlarm( rs422, eAlarmIncreasedTimeShuntShift );
}   

//*****************************************************************************
// ���������� �����������
static void ControlSystem_activityManagerRun( void )
{
    static const uint8_t TIMEOUT_CHANGE_ACT = 25;   //������� �� ������������ ����������, �� ������� �������������� ���� ������
    static uint8_t timeOutChAct;
    
    ActivityManager_setRS422connect( activityManager, eRs422_line_1,
                                     !Rs422_getNoLineExchange( rs422, eRs422_line_1 ) );
    ActivityManager_setRS422connect( activityManager, eRs422_line_2,
                                     !Rs422_getNoLineExchange( rs422, eRs422_line_2 ) );
    ActivityManager_setOwnStatePosShift( activityManager, PositionDet_getPosition( ) );
    
    if( ShuntShift_isEnable( ) && ControlSystem_workCondition( )  )
    {    
        ActivityManager_setActiveCommand( activityManager, true );
        timeOutChAct = 0;
    }
    else if( ( ( currentOrder.sw == NO_ACTION || currentOrder.sw == STOP_P ) && !ShuntShift_isEnable( ) ) 
             || !ControlSystem_workCondition( ) )
    {
        if ( ++timeOutChAct >= TIMEOUT_CHANGE_ACT )
        {
            timeOutChAct = 0;
            ActivityManager_setActiveCommand( activityManager, false );
            ActivityManager_changeActivity( activityManager, currentOrder.act );
        }
    }
    else 
        timeOutChAct = 0;
    ActivityManager_setRPVoutState( activityManager, BinIn_isRpvOnMe( ) );
    RelayCtrl_switchRpv( ActivityManager_isRelayCtrlOn( activityManager ) );
}

//*****************************************************************************
// ����������� ���������
static void ControlSystem_shuntShiftPosDetRun( void )
{
    if( ShuntShift_isEnable( ) )
    {
        // ������� �������
        ShiftDirection reqDir = ShuntShift_getReqShiftDir( );
        switch( reqDir )
        {
            case eShiftPlus:
            case eShiftMinus:
                RelayCtrl_setPosDet( eDetNon );
                RelayCtrl_setShiftDir( reqDir );
                break;
            default: 
                switch( ShuntShift_getReqPosDet( ) )
                {
                    case eDetPlus:
                        RelayCtrl_setPosDet( eDetPlus );
                        RelayCtrl_setShiftDir( eShiftMinus );
                        break;
                    case eDetMinus:
                        RelayCtrl_setPosDet( eDetMinus );
                        RelayCtrl_setShiftDir( eShiftPlus );
                        break;
                    default:
                        RelayCtrl_setPosDet( eDetNon );
                        RelayCtrl_setShiftDir( eShiftStop );
                }
        }
        ShuntShift_setDetPos( RelayCtrl_getPosDet( ), PositionDet_getPosition( ) );
        ShuntShift_setShiftPos( RelayCtrl_getShiftDir( ) );
        PositionDet_setPosDet( RelayCtrl_getPosDet( ) );
    }
    else
    {
        // ��� �������� �������  
        switch( PositionDet_getRequestPositionDet( ) )
        {
            case eDetPlus:
                RelayCtrl_setPosDet( eDetPlus );
                RelayCtrl_setShiftDir( eShiftMinus );
                break;

            case eDetMinus:
                RelayCtrl_setPosDet( eDetMinus );
                RelayCtrl_setShiftDir( eShiftPlus );
                break;

            default:
                RelayCtrl_setPosDet( eDetNon );
                RelayCtrl_setShiftDir( eShiftStop );
        }
    }
    PosDetGenerator_setLevel ( currentOrder.lev ); //���������� ������� ���������� ������������ ����������
}


StatesOrderRS ControlSystem_getStateOrderFieldRS( void )
{
    StatesOrderRS statesOrderRS;
        statesOrderRS = currentOrder.rs;
        return statesOrderRS;
}


//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   18-01-2018
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
*     ������� ��� ������� isPosDetchangedInShift
*     �������� � �������� ��������� ������� ��� �������� ������ �� ����
*     dgn ���������� ������� 
*
* ������ 1.0.3
* ����   14-01-2020
* �����  ��������� �.�. 
* 
* ���������:
*      ��������� �������, � ������� ������������ �������� ���������� ���������� �������
*      � ������, ���� ������� ������ �� ����� ����������� checkEnableProcessingOrder
*      � ��������� ��������. ����� ��������� � ��������� �������� �������� ��������� ������������ ����������
*      (��������� ����������� ������ �����, ����� ������ ������������ ���������� ���������) 
*
* ������ 1.0.4
* ����   10-09-2020
* �����  ��������� �.�.
* 
* ���������:
*   ��������� � ����� ��������� ���������� 220 � ��� �������� �������:
*   - ��������� ������� managementShuntShiftWithout220V;
*   - ��������� � ����� ��������� ������� ������� � ������� ControlSystem_processingOrders
* 
* ������ 1.0.5
* ����   15-10-2020
* �����  ��������� �.�.
* 
* ���������:
*   �������� ������ ��������� ���� ERR2 ���������� �������
*
* ������ 1.0.6
* ����   18-01-2021
* �����  ��������� �.�.   
* 
* ���������:
*   ��������� ������� ��������� �������� ������� ��� ���������� 220� ��� �������� 
*       managementShuntShiftWithout220V
* 
* ������ 1.0.7
* ����   27-01-2021
* �����  ��������� �.�.   
* 
* ���������:
*   ��������� ���������� ������� ControlSystem_getStateCheck220V,
*   ControlSystem_getStatesCheck220VInShuntShift, ControlSystem_getTimeCurrentOrder, 
*   ControlSystem_setTimeProcessingOrder.
*   ��������� ������� � ������ ������������ �� ���������������� ������� ��� ���������� 220� �� 
*   ����� �������� �������
*
* ������ 1.0.8
* ����   27-01-2021
* �����  ��������� �.�.
* 
* ���������:
*   �������� ������������ ���� ������ "���������� ����������� ������� �� ����� �� ����� ����� 
*   c������� ������� ������ 1,5��"
*
* ������ 1.0.9
* ����   14-04-2021
* �����  ��������� �.�.  
* 
* ���������:
*   � ������� ��� ����������� ������������ ���������� �� ������� �� ����������� ������� ��������� 
*       ������� � ������� � ���� SW ��������� "���� ���������"* 
*
* ������ 1.0.10 
* ����   7-07-2021
* �����  ��������� �.�.
*
* ���������:
* 1. ��������� ��������� ����� ����� ������ (��� �������� ���������� � ��������� ������ ���� ������� ������� �����):
*    eAlarmOverloadPhaseU, eAlarmBreakPhaseU, eAlarmOverloadPhaseV, eAlarmBreakPhaseV, eAlarmOverloadPhaseW, eAlarmBreakPhaseW
* 2. ��������� ������������ ������ 0xF4, ���� ������� ������ ��������� � ���������� ��������� ����� ����� �������������
* 3. �������� ������ ��������� ������������ ������� �������� ��� ���������� ��������. ����� ����, ����� �������� �������
*    �������� ������ �� ������ �������� �� �������� �������� ��������.
*/
