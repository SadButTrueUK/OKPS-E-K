/**
* \file    ControlSystem.c
* \brief   \copybrief ControlSystem.h
*
* \version 1.0.3
* \date    14-01-2020
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
#include "deviceAddress.h"
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
// ��������� �������������� ���������
//***************************************************************************** 
static const uint8_t NUM_OF_PHASES = 3;    ///< ���������� ��� �������.

//*****************************************************************************
// ����������� ����� ������
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
    uint8_t resB3   :4;        ///< ����������������� ����
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
    ePhaseReservedFieldFirstBegin,
    ePhaseReservedFieldFirstEnd      = 0x08,
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
// ����������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static PositionDet_State         oldPosDet = eDetUndefinedState;       ///< ���������� ��������� �������.
static PositionDet_State         newPosDet;                            ///< ����� ��������� �������.
static ControlSystem_Order       currentOrder;                         ///< ������� ������.
static ControlSystem_Status      currentStatus;                        ///< ������� ������.
static ControlSystem_ShortStatus currentShortStatus;                   ///< ������� �������� ������.
static bool                      rsEvent1 = false;                     ///< ������� ������� ������ ������� �� ����� "RS1".
static bool                      rsEvent2 = false;                     ///< ������� ������� ������ ������� �� ����� "RS2".
static ReqPosition               reqPos = eShiftUnknown;               ///< ��������� ��������� �������.
static PhaseOfShiftField         phaseOfShift = eShiftNotActive;       ///< ���� �������� �������.
static uint8_t                   previousFieldSW = UNDEFINED_ORDER;    ///< ���������� ��������� ���� ST ������� �� ��

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

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
    ControlSystem_activityManagerRun( );    // ���������� ����������� �������   
    ControlSystem_indicationRun( );         // ��������� ����������� RS1, RS2
    ControlSystem_shuntShiftPosDetRun( );   // ��������� ���� ������ "����-5-��" � ����������� �� ��������� ������� 
    ControlSystem_processingOrders( );      // ��������� ����������� �� �� ��������
}

//*****************************************************************************
// �������� ������� �������� ������
bool ControlSystem_workCondition( void )
{
    return CheckSupply_is220vOn( ) &&
            CheckSupply_is24vOn( ) &&
            !Rs422_getNoExchangeCtrlSystem( rs422 ) &&
            RelayCtrl_getRpvState( ) == eNormal;
}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
//���������� ��������� ���� �������� ������� ��� �� ��������.
static PhaseOfShiftField formFieldPhaseShift( uint8_t cntPhaseShift )
{
    static const uint8_t INIT_VAL_CNT_PHASE_SHIFT = 0;
    
    if( ShuntShift_isEnable( ) )
    {
        if( currentOrder.sw == MINUS_POS || currentOrder.sw == NO_ACTION )
            phaseOfShift = ePhaseFirstShiftFromPlusToMinus + cntPhaseShift; 
        else if( currentOrder.sw == PLUS_POS || currentOrder.sw == NO_ACTION )
            phaseOfShift = ePhaseFirstShiftFromMinusToPlus + cntPhaseShift;
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
                    }
                    reqPos = eShiftInPlus;
                }
            }
        }
        else if( currentOrder.sw == STOP_P && ActivityManager_isActive( activityManager )  /*&& checkEnableProcessingOrder( )*/ )
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
            if( !ShuntShift_isEnable( ) ) 
            {
               IntegrCtrl_turnOn( currentOrder.cm, PositionDet_getPosition( ) );
            }
            previousFieldSW = currentOrder.sw; 
        }
    }
    else
    {
        //previousFieldSW = currentOrder.sw; //!!!
        if( ShuntShift_isEnable( ) ) 
        {
            if ( InterChannel_getData( eICId_OverloadDetScThreePhaseGen ) ) // ���� ���� �� � ����� ���������
            {
                ShuntShiftGen_fastTurnOff( );
            }
            ShuntShift_turnOff( ); 
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
    if( OverloadDet_isScThreePhaseGen( )
        || OverloadDet_isScContrGen( )
        || InterChannel_getData( eICId_IntegrCtrlScAndBreak ) == eShortCircuit )    
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
    if( currentOrder.t1 == 0 || currentOrder.t1 > MAX_TIME_SHIFT ) currentOrder.t1 = MAX_TIME_SHIFT;
    //���������� ��������� ��������
    if( ActivityManager_isActive( activityManager ) )
    {
        currentStatus.t1 = currentOrder.t1;
        currentStatus.st = PositionDet_getPosition( );

        if( OverloadDet_getScThreePhaseGenTrig( ) )
            currentStatus.err1 = eShortCircuit;
        else
            currentStatus.err1 = InterChannel_getData( eICId_IntegrCtrlScAndBreak );
        currentStatus.dgn = ShuntShift_getDgn( );
        //currentStatus.err2 = currentOrder.swm ? IS_EMULATOR : ( InterChannel_getData( eICId_ShuntShiftMotor_failure ) == eOverloadCircuit ? 
        //                     IS_OVL_CURR_MOTOR : isPosDetChangedNoShift( PositionDet_getPosition( ) ) );
        if( currentOrder.swm )
        {
            currentStatus.err2 = IS_EMULATOR;
        }
        else 
        {
            if( ShuntShiftMotor_getFailure() == eOverloadCircuit )
                currentStatus.err2 = IS_OVL_CURR_MOTOR;
            if( isPosDetChangedNoShift( currentStatus.st ) )
            {
                ShuntShiftMotor_setFailure( eNorm );
                currentStatus.err2 = IS_LOST_CONTR_WO_ORDER;
            }
            else if ( currentStatus.st != eDetNon || ShuntShift_isEnable( ) )
                currentStatus.err2 = ShuntShiftMotor_getFailure();
            else {}
        }
        {
            uint16_t tempTime = ShuntShift_getTime( );
            static const uint16_t RATIO = 100 / NUM_VALS_IN_SEC;
            currentStatus.t2 = __builtin_divud( tempTime, RATIO * 10 );
            if( __builtin_divud( tempTime, RATIO ) % 10 > 5 )
            currentStatus.t2 += 1; //���������� ��� ��������� ��������
        }
        const uint8_t IDcurrPhases[] = { eICId_IV_rms, eICId_IU_rms, eICId_IW_rms };
        for( uint8_t cnt = 0; cnt < NUM_OF_PHASES; cnt++ )
        {
            uint16_t temp = InterChannel_getData( IDcurrPhases[cnt] );
            //������� �� ������� ��� � ��� � ����������� 0.1�
            currentStatus.currPhase[cnt] = __builtin_divud( __builtin_muluu( temp, CURRENT_RATIO ), RATIO_TO_100MA );
        }
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
    if( OverloadDet_getScThreePhaseGenTrig( ) || IntegrCtrl_getFailure( ) == eShortCircuit )
        Rs422_addAlarm( rs422, eAlarmShortCircuitThreePhaseGen );
    else
        Rs422_removeAlarm( rs422, eAlarmShortCircuitThreePhaseGen );
    // ���������� � ����� ������������ ���������� ������� (0x6E).
    if( OverloadDet_getScContrGenTrig( ) )
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
    // ���������� ����������� ������� �� ����� �� ����� ����� �������� ������� ������ 1,5 � (0xF3).
    if( !( ActivityManager_getRS422neighborCtrlSysConnect( activityManager, eRs422_line_1 )
           && ActivityManager_getRS422neighborCtrlSysConnect( activityManager, eRs422_line_2 ) )
        && ActivityManager_getNeighborConnection( activityManager ) )
        Rs422_addAlarm( rs422, eAlarmNoValidOrderToNeighbor );
    else
        Rs422_removeAlarm( rs422, eAlarmNoValidOrderToNeighbor );
    // ������� ������ ��������� � ���������� ��������� (0xF4).
    if( !ActivityManager_isNeighborWorking( activityManager )
        && ActivityManager_getNeighborConnection( activityManager ) )
        Rs422_addAlarm( rs422, eAlarmNeighborInSafeMode );
    else
        Rs422_removeAlarm( rs422, eAlarmNeighborInSafeMode );
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
    //��� ����� � �������� �������� (��� ���������� ����� �����)
    if ( BlockExch_getConnectionState( blockExchange ) )
        Rs422_removeAlarm( rs422, eAlarmNoConnectWithNeighbor );
    else
        Rs422_addAlarm( rs422, eAlarmNoConnectWithNeighbor );
    //���������� ��������� ������� ����������� �����
    if( PosDetGenerator_isGeneratorFail( ) )
        Rs422_addAlarm( rs422, eAlarmControlGeneratorIsFail );
    else
        Rs422_removeAlarm( rs422, eAlarmControlGeneratorIsFail );
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
    ActivityManager_setRS422ctrlSysConnect( activityManager, eRs422_line_1,
                                            !Rs422_getNoLineExchangeCtrlSystem( rs422, eRs422_line_1 ) );
    ActivityManager_setRS422ctrlSysConnect( activityManager, eRs422_line_2,
                                            !Rs422_getNoLineExchangeCtrlSystem( rs422, eRs422_line_2 ) );
    if( ShuntShift_isEnable( ) && ControlSystem_workCondition( ) )
    {    
        ActivityManager_setActiveCommand( activityManager, true );
        timeOutChAct = 0;
    }
    else if( currentOrder.sw == NO_ACTION || !ControlSystem_workCondition( ) )
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

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   18-01-2018
* �����  ��������� �.�.
* ���������:
*     ������� ������.
*
* ������ 1.0.2 
* ����   23-10-2019
* �����  ��������� �.�.
  ���������:
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
*/
