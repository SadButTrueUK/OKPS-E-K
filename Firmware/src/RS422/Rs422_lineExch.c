/**
* \file    Rs422_lineExch.c
* \brief   \copybrief Rs422_lineExch.h
*
* \version 1.0.5
* \date    25-01-2021
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <string.h>
#include "Rs422_lineExch.h"
#include "Rs422_transceiver.h"
#include "Rs422_dataTypes.h"
#include "ConfigMK.h"
#include "asserts_ex.h"
#include "InterChannel.h"
#include "Tracing.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************
#define RESET_SYNCHRO  0x8000        ///< ��������� �������� ���������� �������������.
#define FINISH_SYNCHRO 0x0800        ///< �������� ���������� ������������� ��������� ��������.

//#define DEBUG_RS422

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ���������� ����� ����� RS-422
void Rs422_lineCtor( CommunicationRS422 *lineStr )
{
    inRS422_ctor( &lineStr->inRS422, lineStr->numLine );
    outRS422_ctor( &lineStr->outRS422, lineStr->numLine );
    lineStr -> timeOut = TIME_OUT_RESET_SYNC;
    lineStr -> exchangeEvent = false;
    lineStr -> state = eRs422exec_resetSynchro;
    //���������� �������������
    InterChannel_setParamSettings( lineStr->eICId_Rs422Sync,
                                   eScriptChVal,
                                   eProcSyncEqual,
                                   eProcCheckEqual, 0, 0,
                                   0, 0, 0 ); 

    //�������������
    if( !InterChannel_isHandling( lineStr -> eICId_Rs422Sync ) )
        InterChannel_synchronize( lineStr -> eICId_Rs422Sync, RESET_SYNCHRO );
    //Tracing_parameter( lineStr -> state, 0 );
    //Tracing_parameter( RESET_SYNCHRO, 1 );
}

//*****************************************************************************
// ���������� ������� � ��������� ��������� �� ����� ����� RS-422
void Rs422_lineRun( CommunicationRS422 *lineStr, StateRS422 *totalStr )
{
    uint8_t *aTele = 0;
    uint16_t sync, timeBegin = 0;
    uint8_t nCycle, typeStatus_A_B, temp;

    inRS422_run( &lineStr->inRS422, totalStr->address ); //tret 13.01.2020
    
    //��������� ����-���� ���������� ������
    if( lineStr -> noExchangeCnt )
        lineStr -> noExchangeCnt--;
    else
        lineStr -> newOrderEvent = false;
    //��������� ����-���� ���������� ������ � ��
    if( lineStr -> noExchangeCtrlSystemCnt ) 
        lineStr -> noExchangeCtrlSystemCnt--;

    //��������� ���������� ����-����
    if( lineStr -> timeOut != 0 ) 
        lineStr -> timeOut--;

    switch( lineStr -> state )
    {
        case eRs422exec_resetSynchro: //0
            //�������� �������������
            if( lineStr -> timeOut == 0 )
            {
                //������ ��� �������������
                ERROR_EX_ID( eGrPS_Rs422, ePS_Rs422TimeOutError, 1, eSynchroError_start, lineStr->numLine, 0 );
            }
            //�������� ���������������� ����������
            if( InterChannel_getData( lineStr -> eICId_Rs422Sync ) != RESET_SYNCHRO )
            { //���������������� �������� �� ����� RESET_SYNCHRO 
                if( !InterChannel_isHandling( lineStr -> eICId_Rs422Sync ) )
                {
                    InterChannel_synchronize( lineStr -> eICId_Rs422Sync, RESET_SYNCHRO );
                }
                break;
            }

            //���������������� ���������� �������������
            //������� � �������� 1-�� ����������
            lineStr -> state = eRs422exec_1stTeleReception;
            //Tracing_parameter( lineStr -> state, 0 ); //!!ddd
            //Tracing_parameter( RESET_SYNCHRO, 1 );    //!!ddd
            break;

        case eRs422exec_1stTeleReception: //1            // �������� ������ 1-�� ����������
            // ������ ����� ����������
            lineStr -> lengthInTele = InRS422_getTele( &lineStr -> inRS422, &aTele, &timeBegin );
// ��������� �.
// �������������� ��� ����. ��� ������� ��������� � �������� ��������� � ��������� ������:
// �� ������ �� ������������� ������� ����������, ����������� �������������.
// �� ������� �� ������������� ������� ���������� � ��������� �����. �� ��� ���� ����������� �������
// !InterChannel_isSynchronized(), � �� break ��������� ���������� ��������������. 
//            if( !InterChannel_isSynchronized( lineStr -> eICId_Rs422Sync ) )
//            {
//                Rs422_lineCtor( lineStr );
//                break;
//            }
            if( lineStr -> lengthInTele == 0 ) break; // ���������� ��� �� ������� 

            //��������� ����-���� �������� �������� ����������
            if( timeBegin < TIME_OUT_1TELE )
            {
                timeBegin = ConfigMK_isMaster( ) ? TIME_OUT_1TELE - timeBegin : TIME_OUT_2TELE - timeBegin;
            }
            else
            {
                //��������� �������������
                Rs422_lineCtor( lineStr );
                break;
            }
            lineStr -> outRS422.timeOut = timeBegin;

            //�������� ���� ���������� (�������, ��������)
            if( lineStr -> lengthInTele == 1 )
            {
                //�������� ���������� ������� �������
                if( !checkShortTelegramm( aTele[0] ) )
                { //������. ���������� �� ���������
                    //��������� ������  
                    lineStr -> exchangeError = errorRS422_shortTeleDoesNotCorrect;
                    //��������� �������������
                    Rs422_lineCtor( lineStr );
                    break;
                }
                lineStr -> aInTele[0][0] = *aTele;
                //�������������
                if( !InterChannel_isHandling( lineStr -> eICId_Rs422Sync ) )
                    InterChannel_synchronize( lineStr -> eICId_Rs422Sync, lineStr -> aInTele[0][0] );
            }
            else
            { //������� ����������
                if( !checkTelegramm( aTele, lineStr -> lengthInTele ) || //�������� �� ������������ ����������
                    lineStr -> lengthInTele - MIN_ORDER_TELE != totalStr -> orderLength )
                { //�������� �� ������������ ����� ����� ������
                    //������. ���������� �� ������������� �����������
                    lineStr -> exchangeError = errorRS422_tele1DoesNotCorrect;

                    //��������� �������������
                    Rs422_lineCtor( lineStr );
                    break;
                }
                memcpy( &lineStr -> aInTele[0][0], aTele, lineStr -> lengthInTele );

                //�������������
                if( !InterChannel_isHandling( lineStr -> eICId_Rs422Sync ) )
                {
                    InterChannel_synchronize( lineStr -> eICId_Rs422Sync, lineStr -> aInTele[0][lineStr ->
                                              lengthInTele - 1] );
                }
            }
            //��������� ����-���� �������� �������������
            lineStr -> timeOut = TIME_OUT_SYNC_TELE;
            //������� � �������� ������������� ������ ����������
            lineStr -> state = eRs422exec_1stTeleSynchro;
            //Tracing_parameter( lineStr -> state, 0 );                                   //!!ddd
            //Tracing_parameter( lineStr -> aInTele[0][lineStr->lengthInTele - 1], 1 );   //!!ddd
            break;

        case eRs422exec_1stTeleSynchro: //2      //�������� ������������� 1-�� ����������
            if( lineStr -> timeOut == 0 )
            {
                //��������� �������������
                Rs422_lineCtor( lineStr );
                break;
            }

            sync = InterChannel_getData( lineStr -> eICId_Rs422Sync );
            //�������� �������� ���������������� ����������
            if( ( lineStr -> lengthInTele == 1 && sync != lineStr -> aInTele[0][0] ) ||
                ( lineStr -> lengthInTele > 1 && sync != lineStr -> aInTele[0][lineStr -> lengthInTele - 1] ) )
            {
                //���������������� ���������� �� �������������
                break;
            }

            if( lineStr -> lengthInTele == 1 )
            { //���� ������� �������� ����������
                //���������� ������ ��� ������������ �������� ����������
                typeStatus_A_B = ConfigMK_isMaster( ) ? CO_STATUS_A : CO_STATUS_B;
                //��������� ����-���� �������� ��������       
                lineStr -> timeOut = ConfigMK_isMaster( ) ?
                TIME_OUT_TR_CONTR_SHORT_MASTER : TIME_OUT_TR_CONTR_SHORT_SLAVE;
                nCycle = totalStr -> numberCycleOK;
                //������� � �������� ��������� �������� ��������� (2-�� ���������� ���)
                lineStr -> state = eRs422exec_transmEnd;
                //Tracing_parameter( lineStr -> state, 0 );       //!!ddd
                //Tracing_parameter( sync, 1 );                   //!!ddd
            }
            else
            { //���� ������� ������� ����������
                //����� 2-�� ����������
                //������ 2-�� ����������
                lineStr -> lengthInTele = InRS422_getTele( &lineStr -> inRS422, &aTele, &timeBegin );
                if( !checkTelegramm( aTele, lineStr -> lengthInTele ) || //���������� �� ���������
                    lineStr -> lengthInTele - MIN_ORDER_TELE != totalStr -> orderLength )
                { //����� ���������� �� ������������� ����� ����� ������
                    //������. 2-�� ���������� �� ��������
                    lineStr -> exchangeError = errorRS422_tele2DoesNotCorrect;

                    //��������� �������������
                    Rs422_lineCtor( lineStr );
                    break;
                }
                memcpy( &lineStr -> aInTele[1][0], aTele, lineStr -> lengthInTele );

                //�������� �������� ������������ ��������� ������� � � �
                if( !checkTelegramAB( lineStr -> aInTele[0], lineStr -> aInTele[1] ) )
                {
                    //������. ���������� ������� �� ���������
                    lineStr -> exchangeError = errorRS422_teleMutuallyDoNotCorrect;

                    //��������� �������������
                    Rs422_lineCtor( lineStr );
                    break;
                }

                //������������� 2-�� ����������
                if( !InterChannel_isHandling( lineStr -> eICId_Rs422Sync ) )
                    InterChannel_synchronize( lineStr -> eICId_Rs422Sync,
                    lineStr -> aInTele[1][lineStr -> lengthInTele - 1] );

                //��������� ����-���� �� ����� ������������������ ������
                lineStr -> timeOut = TIME_OUT_SYNC_TELE;

                //���������� ������ ��� ������������ �������� ����������
                nCycle = lineStr -> aInTele[0][INDEX_N_CICL]; //���������� ������ ������, �������� ��������
                if( ( lineStr -> aInTele[0][INDEX_ML_CO] & CO_MASK ) == CO_PRIKAZ_B )
                    nCycle = ~nCycle; //���������� ������ ������ ���������������
                //����������� ���������� ������ ��� ��������
                if ( nCycle != (uint8_t)(totalStr->numberCycleOK + 1) && nCycle != (uint8_t)( totalStr->numberCycleOK + 2 ) )
                    nCycle = totalStr -> numberCycleOK; //��������������� ������ ��������
                //��� ������������ ���������� ������ ��������������� ���� ��������
                temp = ( lineStr -> aInTele[0][INDEX_ML_CO] & CO_MASK ) == CO_PRIKAZ_A;
                typeStatus_A_B = ( ConfigMK_isMaster( ) ^ temp ) ? CO_STATUS_B : CO_STATUS_A;

                //������� � �������� ������������� 2-�� ����������             
                lineStr -> state = eRs422exec_2stTeleSynchro;
                //Tracing_parameter( lineStr -> state, 0 );               //!!ddd
                //Tracing_parameter( lineStr -> aInTele[1][lineStr -> lengthInTele - 1], 1 );     //!!ddd
            }

            //������������ �������� ���������� 
            lineStr -> outRS422.length = formTeleStatus( lineStr -> outRS422.aOutRS422, typeStatus_A_B,
                                                         totalStr -> address, nCycle,
                                                         formAlarmRS422( nCycle, totalStr -> alarm ),
                                                         totalStr -> status, totalStr -> statusLength );
            break;

        case eRs422exec_2stTeleSynchro: //3     //�������� ������������� 2-�� ����������
            if( lineStr -> timeOut == 0 )
            {
                //��������� �������������
                Rs422_lineCtor( lineStr );
                break;
            }

            //�������� �������������
            if( InterChannel_getData( lineStr -> eICId_Rs422Sync ) != lineStr ->
                aInTele[1][lineStr -> lengthInTele - 1] )
            {
                //���������������� ���������� �� �������������
                break;
            }

            //������ ������� � ��������� ���������� ������
            if( lineStr -> lengthInTele != 1 )
            {
                if( ( lineStr -> aInTele[0][INDEX_ML_CO] & CO_MASK ) == CO_PRIKAZ_A )
                    aTele = lineStr -> aInTele[0];
                else aTele = lineStr -> aInTele[1]; //���������� ������� �
                nCycle = aTele[INDEX_N_CICL]; //�������� ���������� ������ � ����������
                lineStr -> newOrderEvent = true;
                if( nCycle == (uint8_t)(totalStr->numberCycleOK + 1) ||  nCycle == (uint8_t)( totalStr->numberCycleOK + 2 ) )
                {
                    totalStr -> isLineConnect = true;
                    //������ ������ �������
                    memcpy( totalStr -> order, &aTele[INDEX_IN_DATE_0], totalStr -> orderLength );
                }
                totalStr -> numberCycleOK = nCycle; //��������� ������ �������� ���������� ������                   
            }

            //���������� ������� ���������� ������ c ��
            lineStr -> noExchangeCtrlSystemCnt = totalStr -> noExchangeTimeCtrlSystem;

            //��������� ����-���� �������� ��������
            lineStr -> timeOut = ConfigMK_isMaster( ) ? TIME_OUT_TR_CONTR_MASTER : TIME_OUT_TR_CONTR_SLAVE;

            //������� � �������� ��������� ��������
            lineStr -> state = eRs422exec_transmEnd;
            //Tracing_parameter( lineStr -> state, 0 );           //!!ddd
            //Tracing_parameter( lineStr -> aInTele[1][lineStr -> lengthInTele - 1], 1 );     //!!ddd
        case eRs422exec_transmEnd: //4
            //�������� ��������� ��������
            if( !lineStr -> timeOut )
            {
                //����� ������� DEBUG_RS422
#ifdef DEBUG_RS422   
                //������ ��������
                ERROR_EX_ID( eGrPS_Rs422, ePS_Rs422TimeOutError, 4, eTimeOutError_transmit, lineStr->numLine, 0 );
#endif    //DEBUG_RS422
                //��������� �������������
                Rs422_lineCtor( lineStr );
            }

            //�������� ��������� ��������
            if( lineStr -> outRS422.length ) break;

            //���������� ����������
            //�������������
            if( !InterChannel_isHandling( lineStr -> eICId_Rs422Sync ) )
                InterChannel_synchronize( lineStr -> eICId_Rs422Sync, FINISH_SYNCHRO );

            //��������� ����-���� �������� �������������
            lineStr -> timeOut = TIME_OUT_SYNC_FINISH;

            //������� � �������� �������������
            lineStr -> state = eRs422exec_synchroTransmEnd;
            //Tracing_parameter( lineStr -> state, 0 );           //!!ddd
            //Tracing_parameter( lineStr -> aInTele[1][lineStr -> lengthInTele - 1], 1 );     //!!ddd
            break;

        case eRs422exec_synchroTransmEnd: //5
            //�������� �������������
            if( lineStr -> timeOut == 0 )
            {
                //����� ������� DEBUG_RS422
#ifdef DEBUG_RS422
                //������ ��� �������������
                ERROR_EX_ID( eGrPS_Rs422, ePS_Rs422TimeOutError, 5, eSynchroError_finish, lineStr->numLine, 0 );
#endif    //DEBUG_RS422

                //��������� �������������
                Rs422_lineCtor( lineStr );
                break;
            }

            //�������� �������������
            if( InterChannel_getData( lineStr -> eICId_Rs422Sync ) != FINISH_SYNCHRO )
                //������. ���������������� ���������� �� �������������
                break;

            //�������� ���������.
            //��������� ������
            //��������������� ���. ������� - ����� ���������� ����� � � �������������� ����������� �� �������,
            // � ��������� �� ������ ���������� ����� ������  
            lineStr -> exchangeEvent = totalStr -> isLineConnect;
            //���������� ������� ���������� ������
            lineStr -> noExchangeCnt = totalStr -> noExchangeTime;

            //������� � �������� 1-�� ����������
            //������� � �������� ������ ����������            
            //lineStr->state = eRs422exec_1stTeleReception;
            lineStr->state = eRs422exec_resetSynchro;
            lineStr->timeOut = TIME_OUT_RESET_SYNC;
            //Tracing_parameter( lineStr -> state, 0 );       //!!ddd
            //Tracing_parameter( lineStr -> aInTele[1][lineStr -> lengthInTele - 1], 1 );     //!!ddd
            //��������� �������������
            if( !InterChannel_isHandling( lineStr -> eICId_Rs422Sync ) )
            {
                InterChannel_synchronize( lineStr -> eICId_Rs422Sync, RESET_SYNCHRO );
            }

            break;

        default:
            //������ �������� ���������
            ERROR_EX_ID( eGrPS_Rs422, ePS_Rs422StepCntError, 6, eStepCntError_rs422_run, lineStr->numLine, 0 );
    }
}

//*****************************************************************************
// ����� � �������� ������ �� ����� ����� RS-422
void Rs422_lineInterrupt( CommunicationRS422 *lineStr )
{
    inRS422_readReg( &lineStr->inRS422, lineStr->numLine ); 
    outRS422_run( &lineStr->outRS422, lineStr->numLine ); 
}

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   02-06-2017
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
*
* ������ 1.0.2
* ����   17-02-2020
* �����  ��������� �.�.
* 
* ���������:
*    1) ������� ������ ����� � ������� ����� ����������� 8 ���, ������� ����� �������� 0xFF �������
*    �������� 0. ��� �������� ��������������� ����������� �������� totalStr->numberCycleOK �� ���������
*    � ������� ��������� ���������� ����� ����������� 16 ��� � �� ��������� 0xFF ������� �������� 0x0100,
*    � ���������� ���� ������ ������������. ��� ���������� ���� �������� ������� ���������� � ���� uint8_t.
* 
* ������ 1.0.3
* ����   13-01-2020
* �����  ��������� �.�.
* 
* ���������:
*    1. inRS422_run() ���������� �� Rs422_lineInterrupt() � Rs422_lineRun()
*    2. � Rs422_lineInterrupt() ��������� inRS422_readReg().
* 
* ������ 1.0.4
* ����   02-12-2020
* �����  ��������� �.�.
* 
* ���������:
*    1. ���������������� ������� Rs422_lineRun ��� ���������� �������� ���������.   
* 
* ������ 1.0.5
* ����   25-01-2021
* �����  ��������� �.�.
* 
* ���������:
*    1. ������� �������� �������������: ������ �� ��������� ��������. 
*/
