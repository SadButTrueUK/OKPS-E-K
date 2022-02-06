/**
* \file    TimeBeginSynchronization.c
* \brief   \copybrief TimeBeginSynchronization.h
*
* \version 2.0.1
* \date    19-04-2017
* \author  Третьяков В.Ж.
*/

//*****************************************************************************
// Подключаемые файлы
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
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
#define TIME_BEGIN_WAITE        1000000UL        ///< Время ожидания 1-го импульса, 1 с.
#define TIME_FIX_STATE_125_MKS  4                ///< Фиксация состояния входа с дискретом 125 мкс.
#define TIME_FIX_STATE_1_MKS    16               ///< Фиксация состояния входа с дискретом 1 мкс.

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Начальная синхронизация МК по времени
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
    { // Ожидать не менее 125 мкс
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
    { // Ожидать не менее 125 мкс
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
* История изменений: 
* 
* Версия 1.0.1
* Дата   16-03-2016
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Базовая версия.
* 
* Версия 2.0.1
* Дата   19-04-2017
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Изменения под новую схему на dsPIC33.
*/
