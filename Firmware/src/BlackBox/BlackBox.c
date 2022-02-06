/**
* \file    BlackBox.c
* \brief   \copybrief BlackBox.h
*
* \version 2.0.1
* \date    21-03-2016
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
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
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define ADDRESS_CAN_EXTERNAL_DEVICE_MASTER  0x2A0         ///< ����� �������� \a CAN ��� (��, �� Master).
#define ADDRESS_CAN_EXTERNAL_DEVICE_SLAVE   0x2A1         ///< ����� �������� \a CAN ��� (��, �� Slave).
#define BB_NUMBER_ARRAY_ADD_CODE            8             ///< ������ ������� ����������, ���������� ������� ��.

//*****************************************************************************
/// \brief ������ ������� � ������  �����, � ������� ��������� \a ASSERT.
///
#define N_FILE_ASSERT                       15

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
/// \brief ������ ���������� �������� ����������, ���������� ������� ��.
/// 
static uint16_t addCodeProtectionState[BB_NUMBER_ARRAY_ADD_CODE];

//*****************************************************************************
/// \brief ���������� �������� ���������� � �������� ������ (�������) ��. 
/// 
static uint16_t sourceReset;

//*****************************************************************************
static uint16_t sourceReset;                       ///< ���������� �������� ���������� � �������� ������ (�������) ��. 
static uint16_t aFileASSERT[N_FILE_ASSERT];        ///< ��� �����, � ������� ��������� \a ASSERT
static uint16_t lineASSERT;                        ///< ����� ������, � ������� ��������� \a ASSERT

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ��������� ��������, ���������� ������� �� 
void BlackBox_saveAddCodeProtectionState( uint16_t addCodePS, uint8_t number )
{
    if( number < BB_NUMBER_ARRAY_ADD_CODE )
    {
        addCodeProtectionState[number] = addCodePS;
    }
}

//*****************************************************************************
// ��������� ��������, ���������� ������� ������� ��
void BlackBox_saveSourceReset( uint16_t data )
{
    sourceReset = data;
}

//*****************************************************************************
// ��������� ��� ����� � ����� ������ ASSERT, ���������� ��.
void BlackBox_saveASSERT( char const * const nameFile, int16_t line )
{
    // �������� ���� �����
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
    // �������� ��� ����������� copySwap
    if( ( size % 2 ) != 0 )
    {
        size += 1;
    }
    // ��������� �� �����
    if( size >= ( N_FILE_ASSERT * 2 ) )
    {
        size = ( N_FILE_ASSERT * 2 );
    }

    copySwap( aFileASSERT, pOld, size );
    aFileASSERT[N_FILE_ASSERT - 1] &= 0xFF00; // ����������� ����

    lineASSERT = line;
}

//*****************************************************************************
// ��������� � \a EEPROM ���������� ��
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
// ������ �� EEPROM ���������� "������� �����" � ������ �� � ����� �����
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
        aData[1] = ( uint8_t )( i >> 8 ); // �����
        aData[2] = ( uint8_t )( i );
        data = Eeprom_read( i );
        aData[3] = ( uint8_t )( data >> 8 ); // ���������� 1
        aData[4] = ( uint8_t )( data );
        data = Eeprom_read( i + 2 );
        aData[5] = ( uint8_t )( data >> 8 ); // ���������� 2
        aData[6] = ( uint8_t )( data );

        if( i == 0 )
            aData[7] = 0x0B; // ������ ��������
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
* ������� ���������:
* 
* ������ 1.0.1
* ����   22-01-2016
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
* 
* ������ 2.0.1
* ����   21-03-2016
* �����  ��������� �.�.
*
* ���������:
*    ��������� ��� ����� ����� �� dsPIC33.
*/
