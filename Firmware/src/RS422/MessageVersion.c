/**
* \file    MessageVersion.c
* \brief   \copybrief MessageVersion.h
* 
* \version 1.0.3
* \date    14-07-2021
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdbool.h>
#include "version.h"
#include "crc16_version.h"
#include "addressVarEeprom.h"
#include "Eeprom.h"
#include "MessageVersion.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������������ ����� ��������� (� ������).
///
#define NAME_LENGTH_MAX 48

//*****************************************************************************
/// \brief ������������� ������ ���������.
///
#define ID_BEGIN 0x01

//*****************************************************************************
/// \brief ��� ������� "�����".
///
#define ID_POINT 0x2E

//*****************************************************************************
/// \brief ��� ������� "������".
///
#define ID_SPACE 0x20

//*****************************************************************************
/// \brief ��� ������� "������ �����������".
///
#define ID_BRACKET_BEGIN 0x28

//*****************************************************************************
/// \brief ��� ������� "������ �����������".
///
#define ID_BRACKET_END 0x29

//*****************************************************************************
/// \brief ��� ������� "2".
///
#define ID_SYMBOL_2 0x32

//*****************************************************************************
/// \brief ��� ������� "0".
///
#define ID_SYMBOL_0 0x30

//*****************************************************************************
/// \brief ��� ������� ";".
///
#define ID_SYMBOL_SEMICOLON 0x3B

//*****************************************************************************
/// \brief ��� ������� "#".
///
#define ID_SYMBOL_HASH 0x23

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static uint8_t  aMessage[NAME_LENGTH_MAX]; ///< ���������.
static uint16_t len;                       ///< ����� ���������.
static uint16_t ndx;                       ///< ������ ���������.

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������ ��������� � ������ � ����.
void MessageVersion_ctor( void )
{
    uint16_t value;
    ndx = 0;
    aMessage[ ndx++ ] = ID_BEGIN;

    value = VERSION_MAJOR;
    if ( ( value / 100 ) != 0 ) aMessage[ ndx++ ] = ( value / 100 ) | 0x30;
    value = value % 100;
    if ( ( value / 10 ) != 0 ) aMessage[ ndx++ ] = ( value / 10 ) | 0x30;
    aMessage[ ndx++ ] = ( value % 10 ) | 0x30;
    aMessage[ ndx++ ] = ID_POINT;

    value = VERSION_MINOR;
    if ( ( value / 100 ) != 0 ) aMessage[ ndx++ ] = ( value / 100 ) | 0x30;
    value = value % 100;
    if ( ( value / 10 ) != 0 ) aMessage[ ndx++ ] = ( value / 10 ) | 0x30;
    aMessage[ ndx++ ] = ( value % 10 ) | 0x30;
    aMessage[ ndx++ ] = ID_POINT;

    value = VERSION_BUILD;
    if ( ( value / 10000 ) != 0 ) aMessage[ ndx++ ] = ( value / 10000 ) | 0x30;
    value = value % 10000;
    if ( ( value / 1000 ) != 0 ) aMessage[ ndx++ ] = ( value / 1000 ) | 0x30;
    value = value % 1000;
    if ( ( value / 100 ) != 0 ) aMessage[ ndx++ ] = ( value / 100 ) | 0x30;
    value = value % 100;
    if ( ( value / 10 ) != 0 ) aMessage[ ndx++ ] = ( value / 10 ) | 0x30;
    aMessage[ ndx++ ] = ( value % 10 ) | 0x30;
	
    aMessage[ ndx++ ] = ID_SPACE;
    aMessage[ ndx++ ] = ID_BRACKET_BEGIN;
    	
    aMessage[ ndx++ ] = ( VERSION_DAY / 10 ) | 0x30;
    aMessage[ ndx++ ] = ( VERSION_DAY % 10 ) | 0x30;
    aMessage[ ndx++ ] = ID_POINT;

    aMessage[ ndx++ ] = ( VERSION_MONTH / 10 ) | 0x30;
    aMessage[ ndx++ ] = ( VERSION_MONTH % 10 ) | 0x30;
    aMessage[ ndx++ ] = ID_POINT;

    aMessage[ ndx++ ] = ID_SYMBOL_2; 
    aMessage[ ndx++ ] = ID_SYMBOL_0;
    aMessage[ ndx++ ] = ( VERSION_YEAR / 10 ) | 0x30;
    aMessage[ ndx++ ] = ( VERSION_YEAR % 10 ) | 0x30;
	
    aMessage[ ndx++ ] = ID_BRACKET_END;

    // ������������ ���������� ������
    aMessage[ ndx++ ] = ID_SYMBOL_SEMICOLON;
    aMessage[ ndx++ ] = ID_SPACE;
    aMessage[ ndx++ ] = ID_SYMBOL_HASH;
    uint16_t addrEe = ADDRESS_EEPROM_MANUFACTURERS_NUMBER;
    uint16_t date;
    uint8_t symbol;
    while( true ) 
    {
        date = Eeprom_read( addrEe );
        symbol = (uint8_t)( date >> 8 );
        if ( ( symbol == 0 ) || (symbol == 0xFF ) ) break;
        aMessage[ ndx++ ] = symbol;
        symbol = (uint8_t)( date );
        if ( ( symbol == 0 ) || (symbol == 0xFF ) ) break;
        aMessage[ ndx++ ] = symbol;
        addrEe += 2;
        if ( ndx > ( NAME_LENGTH_MAX - 4) ) break;
    }
    aMessage[ ndx++ ] = ID_SPACE;

    // ������������ ��        
    uint8_t * ptr = aMessage;
    ptr++;
    uint16_t crc = crc16_version( ptr, (ndx - 1) );
    aMessage[ ndx++ ] = ( uint8_t ) ( ( crc >> 8) & 0x00FF );
    aMessage[ ndx++ ] = ( uint8_t ) ( crc & 0x00FF );
    len = ndx;
    ndx = 0;	
}

//*****************************************************************************
// �������� ������� ���� ��������� � ������ � ���� ��, � ��������� ������ �������.
uint8_t MessageVersion_get( void )
{
    uint8_t ret = aMessage[ ndx++ ];
    if ( ndx == len ) ndx = 0;
    
    return ret;
}

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   05-03-2021
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
* 
* ������ 1.0.2
* ����   12-07-2021
* �����  ��������� �.�.
* 
* ���������:
*    �������� ����� ���������� ������.
* 
* ������ 1.0.3
* ����   14-07-2021
* �����  ��������� �.�.
* 
* ���������:
*    ��������� ����������� �� ����� ��� ������������ ���������.
*/
