/**
* \file    CheckCallFunctions.c
* \brief   \copybrief CheckCallFunctions.h
*
* \version 1.0.2
* \date    08-02-2021
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "InterChannel.h"
#include "Eeprom.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������ ������� ���������� ���������� �������.
///
#define N_STACK_NAME_FUNCTION 8

//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ������� �������� ������������� ���������� ������ ControlMC.h, ��.
///
typedef enum
{
    eOUT_T1 = 16,        ///< ������������ ����� ��������� ������� ���������
    eOUT_T2 = 16,        ///< ������������ ����� ������������ ����������
    eOUT_T3 = 128        ///< ������������ ����� ���������� ���������
} eCheckCF_timeSynchr;

//*****************************************************************************
/// \brief ��������� ������� "�������� ������� �������".
///
typedef struct CheckCallFunctions_Tag
{
    uint16_t   cCallFunctions;                         ///< ������� ������� �������.
    const char *aNameFunction[N_STACK_NAME_FUNCTION];  ///< ������ ���������� ���������� �������.
    uint8_t    ndx;                                    ///< ������ ������� ���������� ���������� �������.
    uint8_t    cPeriod;                                ///< ������� ������� ��������.
    uint16_t   fix_cCallFunctions;                     ///< ��������������� �������� �������� ������� �������
    bool       beginSynchro;                           ///< ������� ��� ������� ������������� �������� ������� �������
} CheckCallFunctions;

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************
static CheckCallFunctions checkCallFunctions;    ///< ������ "�������� ������� �������".

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ���������� ������ CheckCallFunctions
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
// ����� ������� � ����������� �� �����
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
// �������� ������� �������
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
// ������������ ���������� �������
void CheckCallFunctions_marked( void )
{
    checkCallFunctions.cCallFunctions++;
}

//*****************************************************************************
// ������ ��������� #N_STACK_NAME_FUNCTION ���� ������� � EEPROM
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
* ������� ���������:
*
* ������ 1.0.1
* ����   11-07-2016
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
* 
* ������ 1.0.2
* ����   08-02-2021
* �����  ��������� �.�.
*
* ���������:
*    �������� �������� �� ��� ��������, ��������� �������� �� eScriptChVal �� eScriptSync.
*/
