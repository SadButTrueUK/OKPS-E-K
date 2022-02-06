/**
* \file    BlackBox.c
* \brief   \copybrief BlackBox.h
*
* \version 2.0.1
* \date    21-03-2016
* \author  Третьяков В.Ж.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <stdlib.h>
#include <string.h>
#include "typeMK.h"
#include "Eeprom.h"
#include "addressVarEeprom.h"
#include "InterChannel.h"
#include "EcanDriver.h"
#include "wait.h"
#include "Main.h"
#include "MainRegisters.h"
#include "ConfigMK.h"
#include "CopySwap.h"
#include "Tracing.h"
#include "BlackBox.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
#define ADDRESS_CAN_EXTERNAL_DEVICE_MASTER  0x2A0         ///< Адрес абонента \a CAN МКО (ВУ, от Master).
#define ADDRESS_CAN_EXTERNAL_DEVICE_SLAVE   0x2A1         ///< Адрес абонента \a CAN МКО (ВУ, от Slave).
#define BB_NUMBER_ARRAY_ADD_CODE            8             ///< Размер массива переменных, уточняющих причину ЗС.

//*****************************************************************************
/// \brief Размер массива с именем  файла, в котором произошел \a ASSERT.
///
#define N_FILE_ASSERT                       15

//*****************************************************************************
// Объявление локальных переменных
//*****************************************************************************

//*****************************************************************************
/// \brief Массив переменных содержит информацию, уточняющую причину ЗС.
/// 
static uint16_t addCodeProtectionState[BB_NUMBER_ARRAY_ADD_CODE];

//*****************************************************************************
/// \brief Переменная содержит информацию о причинах сброса (запуска) МК. 
/// 
static uint16_t sourceReset;

//*****************************************************************************
static uint16_t sourceReset;                       ///< Переменная содержит информацию о причинах сброса (запуска) МК. 
static uint16_t aFileASSERT[N_FILE_ASSERT];        ///< Имя файла, в котором произошел \a ASSERT
static uint16_t lineASSERT;                        ///< Номер строки, в которой произошел \a ASSERT

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Сохранить параметр, уточняющий причину ЗС 
void BlackBox_saveAddCodeProtectionState( uint16_t addCodePS, uint8_t number )
{
    if( number < BB_NUMBER_ARRAY_ADD_CODE )
    {
        addCodeProtectionState[number] = addCodePS;
    }
}

//*****************************************************************************
// Сохранить параметр, уточняющий причину запуска МК
void BlackBox_saveSourceReset( uint16_t data )
{
    sourceReset = data;
}

//*****************************************************************************
// Сохранить имя файла и номер строки ASSERT, вызвавшего ЗС.
void BlackBox_saveASSERT( char const * const nameFile, int16_t line )
{
    // удаление пути файла
    char * pNew;
    char * pOld = ( char * )nameFile;
    while( true )
    {
        pNew = strchr( pOld, '/' );
        if( pNew == NULL )
        {
            break;
        }
        pOld = ++pNew;
    }
    size_t size = strlen( pOld );
    // четность для копирования copySwap
    if( ( size % 2 ) != 0 )
    {
        size += 1;
    }
    // обрезание по длине
    if( size >= ( N_FILE_ASSERT * 2 ) )
    {
        size = ( N_FILE_ASSERT * 2 );
    }

    copySwap( aFileASSERT, pOld, size );
    aFileASSERT[N_FILE_ASSERT - 1] &= 0xFF00; // завершающий ноль

    lineASSERT = line;
}

//*****************************************************************************
// Сохранить в \a EEPROM информацию ЧЯ
void BlackBox_save( void )
{
    for( uint16_t i = 0; i < BB_NUMBER_ARRAY_ADD_CODE; i++ )
    {
        Eeprom_write( ADDRESS_EEPROM_PROTECTION_STATE_ADD_CODE + i * 2, addCodeProtectionState[i] );
    }

    Eeprom_write( ADDRESS_SOURCE_RESET, sourceReset );
    Eeprom_write( ADDRESS_EEPROM_TIME_SEC, Main_getTimeWorkSec( ) );
    Eeprom_write( ADDRESS_EEPROM_TIME_MS, Main_getTimeWorkMs( ) );
    Eeprom_write( ADDRESS_EEPROM_TIME_N_TMR, Main_getTimeWorkInterrupt( ) );
    Eeprom_write( ADDRESS_EEPROM_TIME_TMR, MAIN_TIMER );

    Tracing_saveBlackBox( );
}

//*****************************************************************************
// Чтение из EEPROM информации "черного ящика" и выдача ее в канал связи
void BlackBox_read( void )
{
    const void *drvPtr;
    uint16_t i, data, cTime;
    uint8_t aData[8];

    ConfigMK_ctor( );

    drvPtr = Ecan_ctor( eEcan1, 
                        ConfigMK_isMaster() ? ADDRESS_CAN_EXTERNAL_DEVICE_MASTER : ADDRESS_CAN_EXTERNAL_DEVICE_SLAVE, 
                        ConfigMK_isMaster() ? ADDRESS_CAN_EXTERNAL_DEVICE_SLAVE  :  ADDRESS_CAN_EXTERNAL_DEVICE_MASTER,
                        eEcanModeNormal, 8 );

    for( i = 0; i < SIZE_EEDATA; i += 4)
    {
        aData[0] = eICId_BlackBox;
        aData[1] = ( uint8_t )( i >> 8 ); // Адрес
        aData[2] = ( uint8_t )( i );
        data = Eeprom_read( i );
        aData[3] = ( uint8_t )( data >> 8 ); // Содержимое 1
        aData[4] = ( uint8_t )( data );
        data = Eeprom_read( i + 2 );
        aData[5] = ( uint8_t )( data >> 8 ); // Содержимое 2
        aData[6] = ( uint8_t )( data );

        if( i == 0 )
            aData[7] = 0x0B; // Начало передачи
        else
            i == ( SIZE_EEDATA - 4 ) ? (aData[7] = 0x0E) : (aData[7] = 0x0C);

        Ecan_set( drvPtr, aData, 8 );
        Ecan_startTransmite( drvPtr );
        wait1ms( );
        Ecan_AbortTransmite( drvPtr );

        for( cTime = 0; cTime < 15; cTime++ )
        {
            wait1ms( );
        }
        
        ClrWdt( );
    }
}

//*****************************************************************************
/**
* История изменений:
* 
* Версия 1.0.1
* Дата   22-01-2016
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Базовая версия.
* 
* Версия 2.0.1
* Дата   21-03-2016
* Автор  Третьяков В.Ж.
*
* Изменения:
*    Изменения под новую схему на dsPIC33.
*/
