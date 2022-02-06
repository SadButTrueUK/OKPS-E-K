/**
* \file    IdentHex.c
* \brief   \copybrief IdentHex.h
* 
* \version 1.0.1
* \date    06-10-2017
* \author  Третьяков В.Ж.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <stdint.h>
#include <stdio.h>
#include "Eeprom.h"
#include "addressVarEeprom.h"
#include "nameHex.h"
#include "version.h"
#include "IdentHex.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
#define NAME_LENGTH_MAX    14        ///< Максимальная длина имени прошивки, в символах.
#define VERSION_LENGTH_MAX 16        ///< Максимальная длина версии прошивки, в символах.
#define DATE_LENGTH_MAX    16        ///< Максимальная длина даты прошивки, в символах.

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Запись идентификатора (строки символов).
/// \param ac - указатель на идентификатор;
/// \param address - адрес идентификатора в \a EEPROM;
/// \param maxLenght - длина записываемого идентификатора.
/// 
static void writeIdent( const char *ac, uint16_t address, uint16_t maxLenght );

//*****************************************************************************
// Реализация локальных функций
//*****************************************************************************

//*****************************************************************************
// Запись идентификатора (строки символов)
static void writeIdent( const char * ac, uint16_t address, uint16_t maxLenght )
{
    uint16_t i, dataRead, dataWrite;
    char char1, char2;
    i = 0;
    while( true )
    {
        dataRead = Eeprom_read( address + i );
        char1 = ac[i];  // warning: array subscript is above array bounds
        if( char1 == 0 )
        {
            char2 = ( char )( dataRead );
        }
        else
        {
            char2 = ac[i + 1];  // warning: array subscript is above array bounds
        }
        if( i >= ( maxLenght - 2 ) )
        {
            char2 = 0;
        }
        dataWrite = ( ( uint16_t )( char1 ) << 8 ) | ( ( uint16_t )( char2 ) & 0x00FF );
        if( dataWrite != dataRead )
        {
            Eeprom_write( address + i, dataWrite );
        }
        i += 2;
        if( ( char1 == 0 ) || ( char2 == 0 ) || ( i >= maxLenght ) )
        {
            break;
        }
    }
}

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Запись в \a EEPROM имени прибора, версии, даты прошивки
void IdentHex_run( void )
{
    char acVersion[VERSION_LENGTH_MAX * 2];
    char acDate[DATE_LENGTH_MAX];

    // Формирование параметров идентификации
    // формирование номера версии
    uint32_t version = VERSION_NUMBER;
    uint16_t major = ( uint16_t )( version >> 24 );
    uint16_t minor = ( ( uint16_t )( version >> 16 ) ) & 0x00FF;
    uint16_t build = ( uint16_t )version;
    
    
    sprintf( acVersion, "%s %d.%d.%d", "ver", major, minor, build );
    // формирование даты
    uint16_t day = VERSION_DATE & 0x001F;
    uint16_t month = ( VERSION_DATE >> 5 ) & 0x000F;
    uint16_t year = ( ( VERSION_DATE >> 9 ) & 0x003F ) + 2000;
    sprintf( acDate, "%s %d-%d-%d", "date", day, month, year );

    // Запись параметров идентификации в EEPROM
    writeIdent( acNameHex, ADDRESS_EEPROM_IDENT_NAME, NAME_LENGTH_MAX );
    writeIdent( acVersion, ADDRESS_EEPROM_IDENT_VERSION, VERSION_LENGTH_MAX );
    writeIdent( acDate, ADDRESS_EEPROM_IDENT_DATE, DATE_LENGTH_MAX );
    Eeprom_write( ADDRESS_EEPROM_IDENT_ID, iDdevice );
}

//*****************************************************************************
/** 
* Пояснения:
*    Имя прошивки представляет собой имя проекта, не более 15 символов.
*/

//*****************************************************************************
/**
* История изменений:
* 
* Версия 1.0.1
* Дата   06-10-2017
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Базовая версия.
*/
