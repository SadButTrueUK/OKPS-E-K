/**
* \file    ModeProtection.c
* \brief   \copybrief ModeProtection.h
*
* \version 2.0.2
* \date    03-06-2020
* \author  ��������� �.�., ������ �.�.
*/

//*****************************************************************************
// ������������ �����
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
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define JUMPER_DETECT_TRY_NUMBER 32    ///< ���������� ������� ������ ��������� ��.

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief �������� ��������� ��������� ������ ��.
/// \retval true  - ��������� ������ �� �����������;
/// \retval false - ��������� ������ �� �� �����������.
/// \return ��������� ��������� ������ ��.
///
static bool checkJumperPS( void );

//*****************************************************************************
/// \brief �������� ���� ������ �� \a CAN.
/// \retval true - ��� ���������;
/// \retval false - ��� �� ���������.
///
static bool outCodePS( uint16_t codePS );

//*****************************************************************************
// ����������������, ����������� �����������
//*****************************************************************************

//*****************************************************************************
#ifdef IGNORE_PROTECTION_STATE
    
    //*****************************************************************************
    // ��������� ����������������, ������������ ����� ���������
    //*****************************************************************************
    
     //*****************************************************************************
    #define MAX_REPEAT_PS 16        ///< ���������� ������������ ��� �������� ������������� ����� �������.
    
    //*****************************************************************************
    // ���������� ��������� ����������
    //*****************************************************************************

    //*****************************************************************************
    static uint16_t codePS_old;     ///< ��������� ���������� ��� ������.
    static uint16_t count;          ///< ������� ������������� ����� �������.
#endif

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ��������� �������� ���� ��� (������������ � ���������� ������)
void ModeProtection_ctor( void )
{
#ifdef IGNORE_PROTECTION_STATE
    codePS_old = 0U;
    count = 0U;
    // InterChannel_setParamSettings( eICId_ProtStateCode, eScriptDebug, eProcSyncOff, eProcCheckOff, 0, 0, 0, 0, 0 );
#endif
}

//*****************************************************************************
// ������� � ��������� ��
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
    { // ���������� �������� �� EEPROM
        if( Eeprom_isReady( ) ) break;
        else Eeprom_run( );
    }
    turnOffMK( ); // ���������� ���������
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
// ������� � ��������� ��
void ModeProtectiont_transmitToWait( uint16_t codePS )
{
    if( outCodePS( codePS ) ) return;
#ifdef ENABLE_DEBUG_SPI
    DebugTools_trWordSPI( codePS );
#endif
    turnOffMK( ); // ���������� ���������
    ModeProtection_infinityLoop( );
}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
// ������ ����������� ����
void ModeProtection_infinityLoop( void )
{
    while( true )
    {
        __asm__ volatile( "pwrsav  #0" );
    }
}

//*****************************************************************************
// �������� ���� ������ �� CAN
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
// ���������� ������������ �������
//*****************************************************************************

//***************************************************************************** 
// �������� ��������� ��������� ������ ��
static bool checkJumperPS( void )
{
    uint8_t cnt = 0;
    
    INIC_PORT_IN( B, 0, eIO_pullUp );                                                   // ������������� PORTB0 �� ����. �������� �����������, ����� �� ������ ������.
    INIC_PORT_OUT( B, 1, eIO_openDrainOff );                                            // ������������� PORTB1 �� �����. 

    for( uint8_t i = 0; i < JUMPER_DETECT_TRY_NUMBER; i++ )
    {
        wait1ms( );                                                                     // �������� ��� ������������ ������ �� ����� PORTB0.
        if( _RB0 == _RB1 ) 
            cnt++;

        __builtin_btg( ( uint16_t* )&LATB, 1 ) ;                                        // �������� ��������� ����� PORTB1.
    }

    return cnt == JUMPER_DETECT_TRY_NUMBER;
}

//������ ���
void ModeProtection_ClearPS( void )
{
// *** �������� �������� ������ �� � �������� ���� ��, ���� �� ����������
    if( checkJumperPS( ) )
    {
        static const uint16_t addrEEPROM[ ] = { ADDRESS_EEPROM_PROTECTION_CODE_1, ADDRESS_EEPROM_PROTECTION_CODE_2, 
                                                ADDRESS_EEPROM_PROTECTION_CODE_3 };

        for( uint16_t i = 0; i < sizeof( addrEEPROM ) / sizeof( addrEEPROM[0] ); i++ )  // ������ � EEPROM ���� ���������� ������.
            Eeprom_write( addrEEPROM[i], PROTECTION_CODE_ABSENCE );
        
        static indOutput_type* const ptrLed[] = { &ledRs1, &ledRs2 };        
        static const uint16_t timeOn = 500, timeOff = 500;

        for( uint8_t i = 0; i < sizeof( ptrLed ) / sizeof( ptrLed[0] ); i++ )
            Indication_blink( ptrLed[i], timeOn, timeOff, eIndPhNormal );
        
        LedFailure_set( false ); 
        static const uint16_t timeBlink = 3000;

        for( uint16_t i = 0; i < timeBlink; i++ )
        { // ���������� ������� ������������ "RS1" � "RS2" ��� ��������� ������ ������. ����� ������� 3000 ��.
            for( uint8_t i = 0; i < sizeof( ptrLed ) / sizeof( ptrLed[0] ); i++ )
                 Indication_run( ptrLed[i] );
            wait1ms( );
            ClrWdt( );
        }

        LedFailure_set( true );                                                         // �������� ��������� "�����" ��� ��������� ������ ������.
        ModeProtection_infinityLoop();
    }
}

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   26-08-2016
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������ (���������� EEPROM).
*
* ������ 2.0.1
* ����   08-01-2017
* �����  ��������� �.�.
* 
* ���������:
*    ������� EEPROM.
*    �������� ����� "���������� ���������".
*
* ������ 2.0.2
* ����   03-06-2020
* �����  ������ �.�.
* 
* ���������:
*    ��������� �����������.
*/
