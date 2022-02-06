/**
* \file    FlashCheck.c
* \brief   \copybrief FlashCheck.h
* 
* \version 1.0.2
* \date    03-12-2019
* \author  Кругликов В.П.
*/

// Для корректной работы модуля включите
// опцию File\Project Properties->Building\Insert unprotected checksum in user ID memory.

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <libpic30.h>
#include "typeMK.h"
#include "InterChannel.h"
#include "FlashCheck.h"
#include "asserts_ex.h"

//*****************************************************************************
// Объявление локальных переменных
//*****************************************************************************

//*****************************************************************************
/// \brief Структура состояния модуля контроля целостности Flash-памяти.
///
struct
{
    uint16_t sum;                                                   ///< Сумма данных, считанных из Flash-памяти.
    uint32_t adr;                                                   ///< Адрес читаемых данных из Flash-памяти.
    uint8_t  data[FLASH_CELL_TO_READ_NUM * FLASH_BYTES_PER_CELL];   ///< Буфер данных чтения из Flash-памяти.
} crc;

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация переменных функции FlashCheck_run
void FlashCheck_ctor( void )
{
    static const uint16_t CRC_CHECK_T1 = 50, CRC_CHECK_T2 = 100, CRC_CHECK_T3 = 45000;
       
    crc.sum = 0;
    crc.adr = FLASH_ADDRESS_START;
    InterChannel_setParamSettings( eICId_ControlMK_Flash,
                                   eScriptSync,
                                   eProcSyncEqual,
                                   eProcCheckEqual, 0, 0, CRC_CHECK_T1, CRC_CHECK_T2, CRC_CHECK_T3 );
}

//*****************************************************************************
// Рабочий цикл
void FlashCheck_run( void )
{
    uint16_t len;

    ( crc.adr + FLASH_CELL_TO_READ_NUM * FLASH_ADDRESS_STEP ) < FLASH_PROG_SIZE ? ( len = sizeof(crc.data ) ) :
            ( len = ( FLASH_PROG_SIZE - crc.adr ) / FLASH_ADDRESS_STEP * FLASH_BYTES_PER_CELL );

    crc.adr = _memcpy_p2d24( &crc.data, crc.adr, len );    // Копирование блока данных  из Flash-памяти в RAM

    for( uint8_t i = 0; i < len; )                         // Суммирование блока данных, прочитанного  из Flash-памяти
        crc.sum += crc.data[i++];

    if( crc.adr >= ( FLASH_PROG_SIZE - FLASH_ADDRESS_START ) )
    {
        // Чтение Configuration Bit и очистка по маске
		// согласно документу "dsPIC33EPXXXGM3XX/6XX/7XX Flash Programming Specification"
        _memcpy_p2d16( &crc.data, __FGS_BASE, 1 );         // Чтения младшего байта Configuration Bit FGS
        crc.sum += crc.data[0] & 0x47;
        _memcpy_p2d16( &crc.data, __FOSCSEL_BASE, 1 );     // Чтения младшего байта Configuration Bit FOSCSEL
        crc.sum += crc.data[0] & 0xF7;
        _memcpy_p2d16( &crc.data, __FOSC_BASE, 1 );        // Чтения младшего байта Configuration Bit FOSC
        crc.sum += crc.data[0] & 0xF3;
        _memcpy_p2d16( &crc.data, __FWDT_BASE, 1 );        // Чтения младшего байта Configuration Bit FWDT
        crc.sum += crc.data[0] & 0xFF;
        _memcpy_p2d16( &crc.data, __FPOR_BASE, 1 );        // Чтения младшего байта Configuration Bit FPOR
        crc.sum += crc.data[0] & 0xF8;
        _memcpy_p2d16( &crc.data, __FICD_BASE, 1 );        // Чтения младшего байта Configuration Bit FICD
        crc.sum += crc.data[0] & 0x6F;

        // Чтение Checksum из User ID области Flash-памяти (заполняется компилятором)
        _memcpy_p2d16( &crc.data[1], __FUID0_BASE, 1 );    // Чтения младшего байта Checksum из User ID Memory
        _memcpy_p2d16( &crc.data[0], __FUID1_BASE, 1 );    // Чтения старшего байта Checksum из User ID Memory

        ASSERT_EX_ID( eGrPS_ControlMK, ePS_ControlMkFaultFlash,
                      crc.sum == (( uint16_t )crc.data[0] | ( uint16_t )( crc.data[1] << 8 )),
                      crc.sum,
                      ( uint16_t )crc.data[0] | ( uint16_t )crc.data[1] << 8,
                      0,
                      0 );

        InterChannel_synchronize( eICId_ControlMK_Flash, crc.sum );    // Синхронизация значения Checksum

        crc.sum = 0;
        crc.adr = FLASH_ADDRESS_START;
    }
}

//*****************************************************************************
/**
* История изменений:
* 
* Версия 1.0.1
* Дата   30-07-2018
* Автор  Агулов М.А.
* 
* Изменения:
*    Базовая версия.
* 
* Версия 1.0.2
* Дата   03-12-2019
* Автор  Кругликов В.П.
* 
* Изменения:
*    В сценарий настройки синхронизации контрольной суммы ПЗУ добавлено время Т2.
*    Из-за его отсутствия процедура контроля верифицируемых параметров не выполнялась.  
*    Уменьшено время Т1 и добавлено время Т3
 */
