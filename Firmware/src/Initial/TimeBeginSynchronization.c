/**
* \file    TimeBeginSynchronization.c
* \brief   \copybrief TimeBeginSynchronization.h
*
* \version 2.0.1
* \date    19-04-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "ProtectionState_codes.h"
#include "asserts.h"
#include "wait.h"
#include "TimeBeginSynchronization.h"
#include "TimeSynchronizationPorts.h"
#include "defCompil.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define TIME_BEGIN_WAITE        1000000UL        ///< ����� �������� 1-�� ��������, 1 �.
#define TIME_FIX_STATE_125_MKS  4                ///< �������� ��������� ����� � ��������� 125 ���.
#define TIME_FIX_STATE_1_MKS    16               ///< �������� ��������� ����� � ��������� 1 ���.

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ��������� ������������� �� �� �������
void TimeBeginSynchronization_run( void )
{
    int16_t count;
    int32_t i;

    TIME_SYNCHRO_INIC_PORTS;

    i = count = 0;

    TIME_SYNCHRO_SET_HIGH;
    while( true )
    {
        ClrWdt( );
        wait1mks( );
        if( TIME_SYNCHRO_CHECK_HIGH )
        {
            if( count++ == TIME_FIX_STATE_125_MKS ) break;
        }
        else count = 0;

#ifdef IGNORE_TIME_BEGIN_SYNC
        if( 0 )
#else
        if( i++ >= TIME_BEGIN_WAITE )
#endif
        {
            ERROR_ID( eGrPS_Main, ePS_MainErrorBeginSynchro );
        }
    }
    wait125mks( );
    wait125mks( );

    TIME_SYNCHRO_SET_LOW;
    count = 0;
    for( i = 0; i < 128; i++ )
    { // ������� �� ����� 125 ���
        wait1mks( );
        if( TIME_SYNCHRO_CHECK_LOW )
        {
            if( ++count >= TIME_FIX_STATE_1_MKS )
            {
                break;
            }
        }
    }
    if( count != TIME_FIX_STATE_1_MKS )
    {
        ERROR_ID( eGrPS_Main, ePS_MainErrorBeginSynchro );
    }
    wait125mks( );

    TIME_SYNCHRO_SET_HIGH;
    count = 0;
    for( i = 0; i < 128; i++ )
    { // ������� �� ����� 125 ���
        if( TIME_SYNCHRO_CHECK_LOW )
        {
#ifdef IGNORE_TIME_BEGIN_SYNC
            if( 0 )
#else
            if( ++count >= TIME_FIX_STATE_1_MKS )
#endif
            {
                ERROR_ID( eGrPS_Main, ePS_MainErrorBeginSynchro );
            }
        }
    }
}

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   16-03-2016
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
* 
* ������ 2.0.1
* ����   19-04-2017
* �����  ��������� �.�.
* 
* ���������:
*    ��������� ��� ����� ����� �� dsPIC33.
*/
