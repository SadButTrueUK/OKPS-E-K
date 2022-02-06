/**
* \file    CheckCallFunctions.c
* \brief   \copybrief CheckCallFunctions.h
*
* \version 1.0.2
* \date    08-02-2021
* \author  Третьяков В.Ж.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "InterChannel.h"
#include "Eeprom.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
/// \brief Размер массива указателей вызываемых функций.
///
#define N_STACK_NAME_FUNCTION 8

//*****************************************************************************
// Объявление типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Времена контроля синхронизации параметров модуля ControlMC.h, мс.
///
typedef enum
{
    eOUT_T1 = 16,        ///< максимальное время неприхода второго параметра
    eOUT_T2 = 16,        ///< максимальное время несовпадения параметров
    eOUT_T3 = 128        ///< максимальное время обновления параметра
} eCheckCF_timeSynchr;

//*****************************************************************************
/// \brief Структура объекта "Контроль вызовов функций".
///
typedef struct CheckCallFunctions_Tag
{
    uint16_t   cCallFunctions;                         ///< Счетчик вызовов функций.
    const char *aNameFunction[N_STACK_NAME_FUNCTION];  ///< Массив указателей вызываемых функций.
    uint8_t    ndx;                                    ///< Индекс массива указателей вызываемых функций.
    uint8_t    cPeriod;                                ///< Счетчик периода контроля.
    uint16_t   fix_cCallFunctions;                     ///< Зафиксированное значение счетчика вызовов функций
    bool       beginSynchro;                           ///< Признак для запуска синхронизации счетчика вызовов функций
} CheckCallFunctions;

//*****************************************************************************
// Объявление локальных переменных
//*****************************************************************************
static CheckCallFunctions checkCallFunctions;    ///< Объект "Контроль вызовов функций".

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация переменных модуля CheckCallFunctions
void CheckCallFunctions_ctor( void )
{
    checkCallFunctions.cCallFunctions = 0U;
    checkCallFunctions.ndx = 0U;
    checkCallFunctions.cPeriod = 0U;
    checkCallFunctions.beginSynchro = false;
    InterChannel_setParamSettings( eICId_ControlMK_CF, eScriptSync, eProcSyncEqual, eProcCheckEqual, 0, 0,
                                   eOUT_T1, eOUT_T2, eOUT_T3 );
}

//*****************************************************************************
// Вызов функции с сохранением ее имени
void CheckCallFunctions_call( void ( *const pFunction )( void ), const char *const ac )
{
    if( checkCallFunctions.ndx >= N_STACK_NAME_FUNCTION )
    {
        checkCallFunctions.ndx = N_STACK_NAME_FUNCTION - 1;
    }
    checkCallFunctions.aNameFunction[checkCallFunctions.ndx++] = ac;
    pFunction();
    if( checkCallFunctions.ndx > 0 )
    {
        checkCallFunctions.ndx--;
    }
}

//*****************************************************************************
// Контроль вызовов функций
void CheckCallFunctions_run( void )
{
    if( ++checkCallFunctions.cPeriod >= ( eOUT_T3 / 2 ) )
    {
        checkCallFunctions.cPeriod = 0U;
        checkCallFunctions.beginSynchro = true;
        checkCallFunctions.fix_cCallFunctions = checkCallFunctions.cCallFunctions;
    }
    if( checkCallFunctions.beginSynchro && !InterChannel_isHandling( eICId_ControlMK_CF ) )
    {
        InterChannel_synchronize( eICId_ControlMK_CF, ( param_t )( checkCallFunctions.fix_cCallFunctions ) );
        checkCallFunctions.beginSynchro = false;        
    }
}

//*****************************************************************************
// Маркирование выполнения функции
void CheckCallFunctions_marked( void )
{
    checkCallFunctions.cCallFunctions++;
}

//*****************************************************************************
// Запись последних #N_STACK_NAME_FUNCTION имен функций в EEPROM
uint16_t CheckCallFunctions_saveBlackBox( uint16_t address )
{
    Eeprom_write( address, checkCallFunctions.ndx );
    address += 2;

    const char *addressChar;
    uint8_t char1, char2, ndx = 0;

    while( ndx != checkCallFunctions.ndx )
    {
        addressChar = checkCallFunctions.aNameFunction[ndx++];
        while( true )
        {
            char1 = *addressChar++;
            if( char1 == 0 )
            {
                char2 = 0;
            }
            else
            {
                char2 = *addressChar++;
            }
            Eeprom_write( address, ( ( uint16_t )( char1 ) << 8 ) | char2 );
            address += 2;
            if( ( char1 == 0 ) || ( char2 == 0 ) )
            {
                break;
            }
        }
    }

    return(address );
}

//*****************************************************************************
/**
* История изменений:
*
* Версия 1.0.1
* Дата   11-07-2016
* Автор  Третьяков В.Ж.
*
* Изменения:
*    Базовая версия.
* 
* Версия 1.0.2
* Дата   08-02-2021
* Автор  Третьяков В.Ж.
*
* Изменения:
*    Фиксация значения КС для передачи, изменение сценария из eScriptChVal на eScriptSync.
*/
