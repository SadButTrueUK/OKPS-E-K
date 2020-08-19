/**
* \file    ActivityManager.c
* \brief   \copybrief ActivityManager.h
* 
* \version 1.0.1
* \date    18-05-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "ActivityManager.h"
#include "ActivityManager_dataTypes.h"
#include "relayCtrl.h"
#include "DebugTools.h"

//*****************************************************************************
// ����������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static ActivityManager_DeviceStr deviceStr;        ///< ��������� ��������� ������
static uint16_t timeoutChAct;                      ///< ������� ��� ����� ���������� �� ������� �� �� 

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ��������� ��� ��������.
///
typedef enum
{
    eNoAction,         ///< ��� ��������
    eSetActivePrimary, ///< ���������� �������� �������� ������
    eSetActiveReserve, ///< ���������� �������� ��������� ������
    eReserved          ///< ��������������� (��� ��������)    
} ChangeActivityState;

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ��������� "���������".
/// \param str - ��������� �� ��������� ���������.
/// \return ������������� ����������.
///
static ActivityManager_States passiveProcessing( ActivityManager_DeviceStr *str );

//*****************************************************************************
/// \brief ��������� ��������� "������� �������� � �������� ���������".
/// \param str - ��������� �� ��������� ���������.
/// \return ������������� ����������.
///
static ActivityManager_States transitionToActive( ActivityManager_DeviceStr *str );

//*****************************************************************************
/// \brief ��������� ��������� "��������".
/// \param str - ��������� �� ��������� ���������.
/// \return ������������� ����������.
///
static ActivityManager_States activeProcessing( ActivityManager_DeviceStr *str );

//*****************************************************************************
/// \brief ��������� ��������� "������� �������� � ��������� ���������".
/// \param str - ��������� �� ��������� ���������.
/// \return ������������� ����������.
///
static ActivityManager_States transitionToPassive( ActivityManager_DeviceStr *str );

//*****************************************************************************
/// \brief ��������� �������� ����� ���������� � ��.
/// \param str - ��������� �� ��������� ��������� ���������.
/// \retval true - ��������� ���������;
/// \retval false - ��������� �� ���������.
///
static bool timeoutProcessing( ActivityManager_TimeoutStr *str );

//*****************************************************************************
/// \brief ����� �������� ����� ���������� � ��.
/// \param str - ��������� �� ��������� ���������.
///
static void timeoutReset( ActivityManager_TimeoutStr *str );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �������������
ActivityManager_DeviceStr *ActivityManager_ctor( bool status )
{
    ActivityManager_DeviceStr *str = &deviceStr;
    
    //���������� ������ �������
    str->status = status;
    //���������� ���� �������������
    str->ini = true;
    //��������� ������
    str->ctrl = false;
    str->initIsActive = false; //������������� ������� �� ���������
    return str;
}

bool ActivityManager_getInitDeviceState( ActivityManager_DeviceStr *str )
{
    return str->initIsActive;
}

// ����� ���������� �� ������� �� ����������� �������.
void ActivityManager_changeActivity( ActivityManager_DeviceStr *str, uint8_t value )
{
    
    str->changeActivity = ( str->outputData.str.activityState       //���� ������ ��������
                            && ( ( ( value == eSetActivePrimary ) && !str->status ) ||  //���� ������ ���������, �������� ������� ����� �������� ���������
                               ( ( value == eSetActiveReserve )  && str->status ) ) );  //���� ������ ��������, ��������, �������� ������� ����� �������� ����������
}

//*****************************************************************************
// ���������� ������� ������
void ActivityManager_run( ActivityManager_DeviceStr *str )
{                                                                                              
    
	if( !str->ctrl || !str->ini ) return;

    // ��������� ���������
#ifdef ENABLE_UNIFORM_LOAD_TIMER 
    if( str->uniformLoadTimeCnt ) 
        str->uniformLoadTimeCnt--;
#endif
    if( str->transitionTimeCnt ) 
        str->transitionTimeCnt--;
    
    //����� �������� ����� ���������� � ��
    timeoutReset( & str->timeout );
    
    switch( str->runCnt )
    {
        case eRunStart:                                   //��������� (��� �����)
            if( str->transitionTimeCnt == 0)
            {
                str->initIsActive = true;  //������������� ������� ���������
                if( str->inputData.str.neighborConnection == 0 && str->status )
                    str->runCnt = eRunTransitionToActive;
                else
                    str->runCnt = eRunPassiveState;
            }
            break;
        case eRunPassiveState:                            //��������� ���������
            str->runCnt = passiveProcessing( str );
            break;
        case eRunTransitionToActive:                      //������� � �������� ���������
            str->runCnt = transitionToActive( str );
            break;
        case eRunActiveState:                             //�������� ���������
            str->runCnt = activeProcessing( str );
            break;
        case eRunTransitionToPassive:                     //������� � ��������� ���������
            str->runCnt = transitionToPassive( str );
            break;
    }
}

//*****************************************************************************
// ��������� / ���������� ������
void ActivityManager_setEnable( ActivityManager_DeviceStr *str, bool enable )
{
    static const uint16_t TIMEOUT_START_CHANGE_ACTIVITY = T_O_START_STANDBY - T_O_START_MASTER;
    
    
    if( enable )
    {   //��������� ������
        //������������� ������� ������
        str->inputData.data = 0;

        //������������� �������� ������
        str->outputData.data = 0;

        //������������� ��������� ���������
        str->runCnt = eRunStart;
        str->trToActCnt = eTrToActStart;
        str->trToPassCnt = eTrToPassWaitDisableCtrl;

        //������������� ��������� ��������
    #ifdef ENABLE_UNIFORM_LOAD_TIMER    
        str->uniformLoadTimeCnt = 0;
    #endif
        if( str->status )
        { //��������
            str->transitionTimeCnt = T_O_START_MASTER;
            timeoutChAct = TIMEOUT_START_CHANGE_ACTIVITY;
        }
        else
        { //���������
            str->transitionTimeCnt = T_O_START_STANDBY;
            timeoutChAct = 0;
        }
        str->timeout.changeSafeActivityCnt = 0;
        str->timeout.changeSafeActivityRun = 0;
    }
    str->ctrl = enable;
}

//*****************************************************************************
// ��������� ��������� �������
void ActivityManager_setState( ActivityManager_DeviceStr *str, bool state )
{
    str->inputData.str.ownState = state;
}

//*****************************************************************************
// ��������� ��������� ��������� ���� ���
void ActivityManager_setRPVoutState( ActivityManager_DeviceStr *str, bool state )
{
    str->inputData.str.relayRPVcontactsState = state;
}

//*****************************************************************************
// ��������� ������� �������� �������
void ActivityManager_setActiveCommand( ActivityManager_DeviceStr *str, bool presence )
{
    str->inputData.str.ownActiveCommand = presence;
}

//*****************************************************************************
// ��������� ��������� ����� �� ���������� RS-422
void ActivityManager_setRS422connect( ActivityManager_DeviceStr *str, Rs422_numLine line, bool connect )
{
    if( line == eRs422_line_1 ) str->rs422.str.ownLine1connect = connect;
    else if( line == eRs422_line_2 ) str->rs422.str.ownLine2connect = connect;
}

//*****************************************************************************
// ��������� ��������� ����� � �� �� ���������� RS-422
void ActivityManager_setRS422ctrlSysConnect( ActivityManager_DeviceStr *str, Rs422_numLine line, bool connect )
{
    if( line == eRs422_line_1 ) str->rs422.str.ownLine1CtrlSystemConnect = connect;
    else if( line == eRs422_line_2 ) str->rs422.str.ownLine2CtrlSystemConnect = connect;
}

//*****************************************************************************
// �������� ��������� ���������� �������
bool ActivityManager_isActive( ActivityManager_DeviceStr *str )
{
    return str->outputData.str.activityState;
}

//*****************************************************************************
// �������� ��������� ����� ������ � ��
bool ActivityManager_getRS422neighborCtrlSysConnect( ActivityManager_DeviceStr *str, Rs422_numLine line )
{
    if( line == eRs422_line_1 ) return str->rs422.str.neighborLine1CtrlSystemConnect;
    else if( line == eRs422_line_2 ) return str->rs422.str.neighborLine2CtrlSystemConnect;
    else return false;
}

//*****************************************************************************
// �������� ��������� ���������� ����
bool ActivityManager_isRelayCtrlOn( ActivityManager_DeviceStr *str )
{
    return str->outputData.str.relayCtrlCommand;
}

//*****************************************************************************
// �������� ��������� ������ 
bool ActivityManager_isNeighborWorking( ActivityManager_DeviceStr *str )
{
    return str->inputData.str.neighborState;
}

//*****************************************************************************
// �������� ��������� ����� � �������
bool ActivityManager_getNeighborConnection( ActivityManager_DeviceStr *str )
{
    return str->inputData.str.neighborConnection;
}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
// ��������� ��������� "���������"
ActivityManager_States passiveProcessing( ActivityManager_DeviceStr *str )
{
    if( str->transitionTimeCnt )
    {
        //�������� �������� ����� �������� ������� �� ��������� � ��������� ���������
        return eRunPassiveState;
    }
    //��������� ����� ��������� ��, �� ���������            
    //���� ������ � ���������� ���������
    if( str->inputData.str.ownState == 0 )
    {
        //���� ��� ����� �� RS-422
        if( str->rs422.str.ownLine1connect == 0 && str->rs422.str.ownLine2connect == 0 )
        {
            //���� ��� ����� � �������
            if( str->inputData.str.neighborConnection == 0 )
            {
                return eRunTransitionToActive;
            }
            //���� ����� � �������            
            //���� ����� � ���������� ���������
            if( str->inputData.str.neighborState == 0 )
            {
                //���� � ������ ��� ����� �� RS-422
                if( str->rs422.str.neighborLine1connect == 0 && str->rs422.str.neighborLine2connect == 0 )
                {
                    //���� ����� ���������
                    if( str->inputData.str.neighborActivity == 0 )
                    {
                        //������� ����� �������� 2
                        if( timeoutProcessing( &str->timeout ) )
                        {
                            //str->outputData.str.activityState = 1; //06.08.19 Kruglikov
                        #ifdef ENABLE_UNIFORM_LOAD_TIMER
                            str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                        #endif
                            //return eRunActiveState; //�������� 18.06 ���
                            return eRunTransitionToActive;
                        }
                    }//���� ����� ���������

                    //���� ����� ��������
                    return eRunPassiveState;
                }//���� ����� � �������

                //� ������ ���� ����� � ��
                return eRunPassiveState;
            }//���� ����� � ���������� ���������

            //���� ����� � ������� ���������
            return eRunPassiveState;
        }//���� ��� ����� �� RS-422

        //���� ���� ����� � �� �� RS-422        
        //���� ��� ����� � �������
       
        if( str->inputData.str.neighborConnection == 0 )
        {
            return eRunTransitionToActive;
        }//���� ��� ����� � �������

        //���� ���� ����� � �������
        //���� ����� � ���������� ���������
        if( str->inputData.str.neighborState == 0 )
        {
            //���� � ������ ��� ����� �� RS-422
            if( str->rs422.str.neighborLine1connect == 0 && str->rs422.str.neighborLine2connect == 0 )
            {
                //������� ����� �������� 2
                if( timeoutProcessing( &str->timeout ) )
                {
                    //str->outputData.str.activityState = 1;  //6.08.19 Kruglikov
                #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                    str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                #endif    
                    //return eRunActiveState;//�������� 18.06 ���
                    return eRunTransitionToActive;
                }
            }
            //���� � ������ ���� ����� �� RS-422
            //���� ����� ���������
            if( str->inputData.str.neighborActivity == 0 )
            {
                //������� ����� �������� 2
                if( timeoutProcessing( &str->timeout ) )
                {
                    //str->outputData.str.activityState = 1; //06.08.19 Kruglikov
                #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                    str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                #endif    
                    //return eRunActiveState;//�������� 18.06 ���
                    return eRunTransitionToActive;
                }
            }
            //���� ����� ��������
            return eRunPassiveState;
        }//���� ����� � ���������� ���������

        //����� � ������� ���������
        return eRunPassiveState;
    }//���� ������ � ���������� ���������
    
    //���� ������ � ������� ���������
    //���� ��� ����� � �������
    if( str->inputData.str.neighborConnection == 0 )
    {
        return eRunTransitionToActive;
    }
    //���� ���� ����� � �������
    //���� ����� � ���������� ���������
    if( str->inputData.str.neighborState == 0 )
    {
        return eRunTransitionToActive;
    }
    //���� ����� � ������� ���������
    //���� ����� ���������
    if( str->inputData.str.neighborActivity == 0 )
    {
        return eRunTransitionToActive;
    }
    //���� ����� ��������
    return eRunPassiveState;
}

//*****************************************************************************
// ��������� ��������� "������� �������� � �������� ���������"
ActivityManager_States transitionToActive( ActivityManager_DeviceStr *str )
{
    ////!!! 8.8.19 Kruglikov
    //����� �������� ���������� ���� ���������� ���
    if( RelayCtrl_getRpvState() != eNormal )
    {
        str->outputData.str.activityState = 1; //������ ��������
        return eRunActiveState;
    }
    else
    {    
        switch( str->trToActCnt )
        {
            case eTrToActStart:
                if( str->status == 0 )
                {
                    //���������
                    if( str->inputData.str.relayRPVcontactsState == 0 )
                    {
                        //��� �� "�� ���"                    
                        return eRunPassiveState; //�� ������� ����� ��������
                    }
                    //��� �� ���
                    if( RelayCtrl_getRpvState() != eNotTurnOn )   ////!!!ddd 05.08.19 Kruglikov
                    {
                        str->outputData.str.activityState = 1; //������ ��������
                    #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                        str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                    #endif    
                        return eRunActiveState; //��������
                    }
                }
                //��������
                if( RelayCtrl_getRpvState() != eNotTurnOn )   ////!!!ddd 05.08.19 Kruglikov
                {
                    str->outputData.str.relayCtrlCommand = 1; //�������� ���
                    str->transitionTimeCnt = T_O_RELAY_OPERATE; //������ �������� ��������� ����
                    str->trToActCnt = eTrToActRelaySwitchDelay;
                }
                break;

            case eTrToActRelaySwitchDelay:
                //��������
                //�������� ������������ ����
                if( str->transitionTimeCnt == 0 )
                {
                    //������� �����
                    str->trToActCnt = eTrToActStart;
                    str->outputData.str.relayCtrlCommand = 0; //���������� ���
                    str->transitionTimeCnt = T_O_RELAY_RELEASE; //�������� �� ���������� ���
                    return eRunPassiveState; //�� ������� ����� ��������
                }
                //������� �� �����
                if( str->inputData.str.relayRPVcontactsState == 0 )
                {
                    //��� �� "�� ���"
                    break;
                }
                //��� "�� ���"
                str->trToActCnt = eTrToActStart;
                str->outputData.str.activityState = 1; //������ ��������
            #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
            #endif    
                return eRunActiveState; //��������
        }
        return eRunTransitionToActive;
    }
}

//*****************************************************************************
// ��������� ��������� "��������"
ActivityManager_States activeProcessing( ActivityManager_DeviceStr *str )
{
    //������ �� ����� ���������� �� �� ����������� �������� ������ ������ �� ��������� �������� 
    //��� 5 � ����� ������ ������� �� ������, ��� ��� ������ � ������� ����� ������� ���������� ������ �� ����� 
    //�������� ����� ��������
    if( timeoutChAct)  
        timeoutChAct--;
    
    if( str->transitionTimeCnt )
    {
        //�������� �������� ����� �������� ������� �� ��������� � ��������� ���������
        return eRunActiveState;
    }
    //��������� ����� ��������� ��, �� ���������
    if( str->inputData.str.ownState == 0 )
    {
        //������ � ���������� ���������
        if( str->rs422.str.ownLine1connect == 0 && str->rs422.str.ownLine2connect == 0 )
        {
            //��� ����� � �������
            if( str->inputData.str.neighborConnection == 0 )
            {
                //��� ����� � �������
                //������� ����� ��������� 2
                if( str->inputData.str.relayRPVcontactsState == 0 ) //���� ��� �� �� ���
                    return eRunTransitionToPassive;
                else
                {
                    return eRunActiveState;
                }
            }
            //���� ����� � �������
            if( str->inputData.str.neighborState == 0 )
            {
                //����� � ���������� ���������
                if( str->rs422.str.neighborLine1connect == 0 && str->rs422.str.neighborLine2connect == 0 )
                {
                    //� ������ ��� ����� �� RS-422
                    if( str->inputData.str.neighborActivity == 0 || str->status == 1 )
                    {
                        //����� ��������� ��� ���������
                    #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                        if( str->uniformLoadTimeCnt == 0 )
                        {
                            //����� ������ ����������� ��������
                            //������� ����� ��������� 2
                            if( timeoutProcessing( &str->timeout ) )
                            {
                                str->outputData.str.activityState = 0;
                                str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
                                str->uniformLoadTimeCnt = 0;
                                return eRunTransitionToPassive;
                            }
                        }
                    #endif    
                        //������ ����������� �������� �� �����
                        return eRunActiveState;
                    }
                    //����� �������� � ��������
                    //������� ����� ��������� 2
                    if( timeoutProcessing( &str->timeout ) )
                    {
                        //str->outputData.str.activityState = 0;  //06.08.19 Kruglikov
                        str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
                    #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                        str->uniformLoadTimeCnt = 0;
                    #endif    
                        //return eRunPassiveState; //�������� ��� 18.06
                        return eRunTransitionToPassive;
                    }
                }
                //� ������ ���� ����� �� RS-422
                //������� ����� ��������� 2
                if( timeoutProcessing( &str->timeout ) )
                {
                    //str->outputData.str.activityState = 0; //06.08.19 Kruglikov
                    str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
                #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                    str->uniformLoadTimeCnt = 0;
                #endif    
                    return eRunTransitionToPassive;
                }
            }
            //����� � ������� ���������
            //������� ����� ��������� 2
            if( timeoutProcessing( &str->timeout ) )
            {
                //str->outputData.str.activityState = 0; //06.08.19 Kruglikov
                str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
            #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                str->uniformLoadTimeCnt = 0;
            #endif    
                return eRunTransitionToPassive;
            }
        }
        //���� ����� �� RS-422
        if( str->inputData.str.neighborConnection == 0 )
        {
            if( str->inputData.str.relayRPVcontactsState == 0 ) //���� ��� �� �� ���
                 return eRunTransitionToPassive;
            else
            {
                //��� ������� �� ������
            #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                if( str->uniformLoadTimeCnt == 0 )
                {
                    //������ ��� �����
                    str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                    return eRunActiveState;
                }
            #endif
                //������ ��� �� �����
                return eRunActiveState;
            }
        }
        //���� ������ �� ������
        if( str->inputData.str.neighborState == 0 )
        {
            //����� � ��
            if( str->rs422.str.neighborLine1connect == 0 && str->rs422.str.neighborLine2connect == 0 )
            {
                //� ������ ��� ����� �� RS-422
            #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                if( str->uniformLoadTimeCnt == 0 )
                {
                    //����� ������ ���
                    str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                    return eRunActiveState;
                }
            #endif
                //������ ��� �� �����
                return eRunActiveState;
            }
            //� ������ ���� ����� � ��
            if( str->inputData.str.neighborHearsMe == 0 )
            {
                //����� ���� �� ������
                if( str->inputData.str.neighborActivity == 0 )
                {
                    //����� ���������
                    return eRunActiveState;
                }
                //����� ��������
                //������� ����� ��������� 2
                if( timeoutProcessing( &str->timeout ) )
                {
                    //str->outputData.str.activityState = 0;  //06.08.19 Kruglikov
                    str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
                #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                    str->uniformLoadTimeCnt = 0;
                #endif    
                    return eRunTransitionToPassive;
                }
            }
            //����� ���� ������
            if( str->inputData.str.neighborActivity == 0 || str->status == 1 )
            {
                //����� ��������� ��� ���������
            #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                if( str->uniformLoadTimeCnt == 0 )
                {
                    //����� ������ ���
                    //������� ����� ��������� 2
                    if( timeoutProcessing( &str->timeout ) )
                    {
                        str->outputData.str.activityState = 0;
                        str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
                        str->uniformLoadTimeCnt = 0;
                        return eRunTransitionToPassive;
                    }
                }
            #endif    
                //������ ��� �� �����
                return eRunActiveState;
            }
            //����� �������� � ��������
            //������� ����� ��������� 2
            if( timeoutProcessing( &str->timeout ) )
            {
                //str->outputData.str.activityState = 0;  //06.08.19 Kruglikov
                str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
            #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                str->uniformLoadTimeCnt = 0;
            #endif    
                return eRunTransitionToPassive; //������� ���
            }
        }
        //����� � ��
        //������� ����� ��������� 2
        if( timeoutProcessing( &str->timeout ) )
        {
            //str->outputData.str.activityState = 0;   //06.08.19 Kruglikov
            str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
        #ifdef ENABLE_UNIFORM_LOAD_TIMER   
            str->uniformLoadTimeCnt = 0;
        #endif    
            return eRunTransitionToPassive; //������� ���
        }
        
    }
    //������ � ������� ���������
    if( str->inputData.str.relayRPVcontactsState == 0 )
    {
        //���� �� "�� ���"
        return eRunTransitionToPassive;
    }
    //���� ��� "�� ���"    
    if( str->inputData.str.neighborConnection == 0 )
    {
    //��� ����� � �������
    #ifdef ENABLE_UNIFORM_LOAD_TIMER    
        if( str->uniformLoadTimeCnt == 0 )
        {
            //����� ������ ����������� ��������
            str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
            return eRunActiveState;
        }
    #endif    
        //�� ����� ������ ����������� ��������
        return eRunActiveState;
    }
    //���� ����� � �������
    if( str->inputData.str.neighborHearsMe == 0 )
    {
        //����� ���� �� ������
        if( str->inputData.str.neighborState == 0 )
        {
            //����� � ���������� ���������
        #ifdef ENABLE_UNIFORM_LOAD_TIMER    
            if( str->uniformLoadTimeCnt == 0 )
            {
                //����� ������ ����������� ��������
                str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                return eRunActiveState;
            }
        #endif    
            //�� ����� ������ ����������� ��������
            return eRunActiveState;
        }
        //����� � ������� ���������
        return eRunTransitionToPassive;
    }
    //����� ���� ������
    if( str->inputData.str.ownActiveCommand == 0 )
    {
        //��� �������� �������
    #ifdef ENABLE_UNIFORM_LOAD_TIMER    
        if( str->uniformLoadTimeCnt == 0 )
        {
            //����� ������ ����������� ��������
            if( str->inputData.str.neighborState == 0 )
            {
                //����� � ���������� ���������
                str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                return eRunActiveState;
            }
            //����� � ������� ���������
            return eRunTransitionToPassive;
        }
    #endif    
        if( str->changeActivity && 
            str->inputData.str.neighborState && 
            str->inputData.str.neighborIsInWork ) 
        //������ ������� �� ����������� ������� �� ����� ���������� (� � ��, ������ ������ ���� � ��)
            return eRunTransitionToPassive;
        
        return eRunActiveState;
    }
    //���� �������� �������
    return eRunActiveState;
}

//*****************************************************************************
// ��������� ��������� "������� �������� � ��������� ���������"
ActivityManager_States transitionToPassive( ActivityManager_DeviceStr *str )
{
    switch( str->trToPassCnt )
    {
        case eTrToPassWaitDisableCtrl:
            if( str->inputData.str.ownActiveCommand == 0 )
            {
                //��������� ����������� �����������
                str->outputData.str.requestDisableCtrlActions = 0; //����� ������ �� ���������� ����������� �����������
                str->outputData.str.relayCtrlCommand = 0; //��������� ���
                if( str->status == 0 )
                {
                    //���������
                    str->outputData.str.activityState = 0; //���������
                    str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
                #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                    str->uniformLoadTimeCnt = 0; //�������� ������ ����������� ��������
                #endif    
                    str->trToPassCnt = eTrToPassWaitDisableCtrl;
                    return eRunPassiveState; //���������
                }
                //��������
                str->transitionTimeCnt = T_O_RELAY_RELEASE; //������ �������� ���������� ����
                str->trToPassCnt = eTrToPassWaitRelaySwitch;
                break;
            }
            //�� ��������� ����������� �����������
            str->outputData.str.requestDisableCtrlActions = 1; //���������� ������ �� ���������� ����������� �����������
            break;

        case eTrToPassWaitRelaySwitch:
            if( str->transitionTimeCnt == 0 )
            {
                //������� �������� ������������ ���� �����, �� ���������� ����� ���������
                str->outputData.str.relayCtrlCommand = 1; // �������� ���
                str->transitionTimeCnt = T_O_RELAY_OPERATE; //������ �������� ��������� ����
                str->trToPassCnt = eTrToPassRemainInActive;
                break;
            }
            if( str->inputData.str.relayRPVcontactsState == 0 )
            {
                //���� ��� �� "�� ���"
                 str->outputData.str.activityState = 0;
                str->transitionTimeCnt = T_O_REMAIN_PASSIVE; //������ ������� ���������� ���������
            #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                str->uniformLoadTimeCnt = 0; //�������� ������ ����������� ��������
            #endif    
                str->trToPassCnt = eTrToPassWaitDisableCtrl;
                return eRunPassiveState; //������� � ��������� ���������
            }
            //���� ��� "�� ���"
            break;

        case eTrToPassRemainInActive:
            //�������� �������� ������������ ����
            if( str->transitionTimeCnt ) 
                break;
            //������� �����
            str->trToPassCnt = eTrToPassWaitDisableCtrl;
            return eRunActiveState; //�������� � �������� ���������
    }
    return eRunTransitionToPassive;
}

//*****************************************************************************
// ��������� �������� ����� ���������� � ��
bool timeoutProcessing( ActivityManager_TimeoutStr *str )
{
    //��������� ����� ������� ��������
    str->changeSafeActivityRun = 1;
    if( str->changeSafeActivityCnt == 0 )
    { //������� �� ��� �������
        //������ ��������
        str->changeSafeActivityCnt = T_O_CHANGE_ACTIVITY_IN_SAFE;
        return false;
    }
    //��������� ��������
    if( --str->changeSafeActivityCnt == 0 )
    { //������� �����
        return true;
    }
    return false;
}

//*****************************************************************************
// ����������� �������� ����� ���������� � ��
void timeoutReset( ActivityManager_TimeoutStr *str )
{
    if( str->changeSafeActivityRun == 0 ) str->changeSafeActivityCnt = 0;
    str->changeSafeActivityRun = 0;
}

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   18-05-2017
* �����  ��������� �.�.
* ���������:
*     ������� ������.
*
* ������ 1.0.2  
* ���� 8-08-2019
* �����  ��������� �.�.
* ���������:
* ������ �������� ��������� ������������� ����������� ���� ���� ���
* 
* ������ 1.0.3  
* ���� 2-12-2019
* �����  ��������� �.�.
* ���������:
* ��������� ��������� ������� �� ����������� ������� �� ����� ����������
*
* ������ 1.0.4
* ���� 12-12-2019 
* �������� ��������� ���������� ���������.
* ������� � ��������� ������: 
* �������� - ��������, �������. ��������� �� �� 220�, �� ����� ���������� �������� ����������. 
* ��������� �� ��������� 24�, 
* ��� ���� ���������� �� ������������ �� �������� ������, � ������������ ������ ���� �������� � ���� 220� 
*/
