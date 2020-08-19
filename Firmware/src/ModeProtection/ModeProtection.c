/**
* \file    ModeProtection.c
* \brief   \copybrief ModeProtection.h
*
* \version 2.0.2
* \date    03-06-2020
* \author  Третьяков В.Ж., Агулов М.А.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "ProtectionState_codes.h"
#include "Eeprom.h"
#include "addressVarEeprom.h"
#include "Main.h"
#include "InterChannel.h"
#include "BlackBox.h"
#include "ModeProtection.h"
#include "turnOffdsPIC33.h"
#include "IOports.h"
#include "ConfigMK.h"
#include "wait.h"
#include "LedFailure.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
#define JUMPER_DETECT_TRY_NUMBER 32    ///< Количество попыток чтения перемычек ЗС.

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Контроль состояния перемычки снятия ЗС.
/// \retval true  - перемычка снятия ЗС установлена;
/// \retval false - перемычка снятия ЗС не установлена.
/// \return Состояние перемычки снятия ЗС.
///
static bool checkJumperPS( void );

//*****************************************************************************
/// \brief Передача кода отказа по \a CAN.
/// \retval true - код отправлен;
/// \retval false - код не отправлен.
///
static bool outCodePS( uint16_t codePS );

//*****************************************************************************
// Макроопределения, управляющие компиляцией
//*****************************************************************************

//*****************************************************************************
#ifdef IGNORE_PROTECTION_STATE
    
    //*****************************************************************************
    // Локальные макроопределения, определенные через константы
    //*****************************************************************************
    
     //*****************************************************************************
    #define MAX_REPEAT_PS 16        ///< Количество игнорируемых для передачи повторяющихся кодов отказов.
    
    //*****************************************************************************
    // Объявление локальных переменных
    //*****************************************************************************

    //*****************************************************************************
    static uint16_t codePS_old;     ///< Последний переданный код отказа.
    static uint16_t count;          ///< Счетчик повторяющихся кодов отказов.
#endif

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Настройка передачи кода ЗОт (используется в отладочном режиме)
void ModeProtection_ctor( void )
{
#ifdef IGNORE_PROTECTION_STATE
    codePS_old = 0U;
    count = 0U;
    // InterChannel_setParamSettings( eICId_ProtStateCode, eScriptDebug, eProcSyncOff, eProcCheckOff, 0, 0, 0, 0, 0 );
#endif
}

//*****************************************************************************
// Переход в состояние ЗС
void ModeProtection_run( uint16_t codePS )
{
    if( outCodePS( codePS ) )
    {
        return;
    }
#ifdef ENABLE_DEBUG_SPI
    DebugTools_trWordSPI( codePS );
#endif
    while( true )
    { // Завершение операции на EEPROM
        if( Eeprom_isReady( ) ) break;
        else Eeprom_run( );
    }
    turnOffMK( ); // выключение периферии
    Eeprom_ctor( );
    static const uint16_t addrEEPROM[ ] = { ADDRESS_EEPROM_PROTECTION_CODE_1, ADDRESS_EEPROM_PROTECTION_CODE_2, 
                                            ADDRESS_EEPROM_PROTECTION_CODE_3, ADDRESS_EEPROM_PROTECTION_CODE_4 };
    for( uint16_t i = 0; i < sizeof( addrEEPROM ) / sizeof( addrEEPROM[0] ); i++ )
        Eeprom_write( addrEEPROM[i], codePS );
    
    BlackBox_save( );

#ifdef DEBUG_BLACK_BOX_READ_PS 
    BlackBox_read( );
#endif
    ModeProtection_infinityLoop( );
}

//*****************************************************************************
// Переход в состояние ЗО
void ModeProtectiont_transmitToWait( uint16_t codePS )
{
    if( outCodePS( codePS ) ) return;
#ifdef ENABLE_DEBUG_SPI
    DebugTools_trWordSPI( codePS );
#endif
    turnOffMK( ); // выключение периферии
    ModeProtection_infinityLoop( );
}

//*****************************************************************************
// Реализация локальных функций
//*****************************************************************************

//*****************************************************************************
// Пустой бесконечный цикл
void ModeProtection_infinityLoop( void )
{
    while( true )
    {
        __asm__ volatile( "pwrsav  #0" );
    }
}

//*****************************************************************************
// Передача кода отказа по CAN
static bool outCodePS( uint16_t codePS )
{
#ifdef IGNORE_PROTECTION_STATE
    if( ( codePS & 0xC000 ) == 0 )
    {
        if( !InterChannel_isHandling( eICId_ProtStateCode ) )
        {
            if( codePS_old == codePS )
            {
                count++;
            }
            if( ( codePS_old != codePS ) || ( count >= MAX_REPEAT_PS ) )
            {
                InterChannel_synchronize( eICId_ProtStateCode, ( param_t )( codePS ) );
                codePS_old = codePS;
                count = 0;
            }
        }
        return true;
    }
    else
    {
        return false;
    }
#else
    return false;
#endif
}

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//***************************************************************************** 
// Контроль состояния перемычки снятия ЗС
static bool checkJumperPS( void )
{
    uint8_t cnt = 0;
    
    INIC_PORT_IN( B, 0, eIO_pullUp );                                                   // Инициализация PORTB0 на вход. Подтяжка обязательна, чтобы не ловить помехи.
    INIC_PORT_OUT( B, 1, eIO_openDrainOff );                                            // Инициализация PORTB1 на выход. 

    for( uint8_t i = 0; i < JUMPER_DETECT_TRY_NUMBER; i++ )
    {
        wait1ms( );                                                                     // Задержка для установления уровня на входе PORTB0.
        if( _RB0 == _RB1 ) 
            cnt++;

        __builtin_btg( ( uint16_t* )&LATB, 1 ) ;                                        // Инверсия состояния порта PORTB1.
    }

    return cnt == JUMPER_DETECT_TRY_NUMBER;
}

//Снятие ЗОт
void ModeProtection_ClearPS( void )
{
// *** Проверка джампера снятия ЗС и стирание кода ЗС, если он установлен
    if( checkJumperPS( ) )
    {
        static const uint16_t addrEEPROM[ ] = { ADDRESS_EEPROM_PROTECTION_CODE_1, ADDRESS_EEPROM_PROTECTION_CODE_2, 
                                                ADDRESS_EEPROM_PROTECTION_CODE_3 };

        for( uint16_t i = 0; i < sizeof( addrEEPROM ) / sizeof( addrEEPROM[0] ); i++ )  // Запись в EEPROM кода отсутствия отказа.
            Eeprom_write( addrEEPROM[i], PROTECTION_CODE_ABSENCE );
        
        static indOutput_type* const ptrLed[] = { &ledRs1, &ledRs2 };        
        static const uint16_t timeOn = 500, timeOff = 500;

        for( uint8_t i = 0; i < sizeof( ptrLed ) / sizeof( ptrLed[0] ); i++ )
            Indication_blink( ptrLed[i], timeOn, timeOff, eIndPhNormal );
        
        LedFailure_set( false ); 
        static const uint16_t timeBlink = 3000;

        for( uint16_t i = 0; i < timeBlink; i++ )
        { // Трёхкратное мигание индикаторами "RS1" и "RS2" для индикации снятия отказа. Время мигания 3000 мс.
            for( uint8_t i = 0; i < sizeof( ptrLed ) / sizeof( ptrLed[0] ); i++ )
                 Indication_run( ptrLed[i] );
            wait1ms( );
            ClrWdt( );
        }

        LedFailure_set( true );                                                         // Включаем индикатор "ОТКАЗ" для индикации режима работы.
        ModeProtection_infinityLoop();
    }
}

//*****************************************************************************
/**
* История изменений:
* 
* Версия 1.0.1
* Дата   26-08-2016
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Базовая версия (встроенный EEPROM).
*
* Версия 2.0.1
* Дата   08-01-2017
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Внешний EEPROM.
*    Добавлен режим "Безопасное состояние".
*
* Версия 2.0.2
* Дата   03-06-2020
* Автор  Агулов М.А.
* 
* Изменения:
*    Добавлены комментарии.
*/
