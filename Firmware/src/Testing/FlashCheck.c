/**
* \file    FlashCheck.c
* \brief   \copybrief FlashCheck.h
* 
* \version 1.0.2
* \date    03-12-2019
* \author  ��������� �.�.
*/

// ��� ���������� ������ ������ ��������
// ����� File\Project Properties->Building\Insert unprotected checksum in user ID memory.

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <libpic30.h>
#include "typeMK.h"
#include "InterChannel.h"
#include "FlashCheck.h"
#include "asserts_ex.h"

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ��������� ������ �������� ����������� Flash-������.
///
struct
{
    uint16_t sum;                                                   ///< ����� ������, ��������� �� Flash-������.
    uint32_t adr;                                                   ///< ����� �������� ������ �� Flash-������.
    uint8_t  data[FLASH_CELL_TO_READ_NUM * FLASH_BYTES_PER_CELL];   ///< ����� ������ ������ �� Flash-������.
} crc;

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ���������� ������� FlashCheck_run
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
// ������� ����
void FlashCheck_run( void )
{
    uint16_t len;

    ( crc.adr + FLASH_CELL_TO_READ_NUM * FLASH_ADDRESS_STEP ) < FLASH_PROG_SIZE ? ( len = sizeof(crc.data ) ) :
            ( len = ( FLASH_PROG_SIZE - crc.adr ) / FLASH_ADDRESS_STEP * FLASH_BYTES_PER_CELL );

    crc.adr = _memcpy_p2d24( &crc.data, crc.adr, len );    // ����������� ����� ������  �� Flash-������ � RAM

    for( uint8_t i = 0; i < len; )                         // ������������ ����� ������, ������������  �� Flash-������
        crc.sum += crc.data[i++];

    if( crc.adr >= ( FLASH_PROG_SIZE - FLASH_ADDRESS_START ) )
    {
        // ������ Configuration Bit � ������� �� �����
		// �������� ��������� "dsPIC33EPXXXGM3XX/6XX/7XX Flash Programming Specification"
        _memcpy_p2d16( &crc.data, __FGS_BASE, 1 );         // ������ �������� ����� Configuration Bit FGS
        crc.sum += crc.data[0] & 0x47;
        _memcpy_p2d16( &crc.data, __FOSCSEL_BASE, 1 );     // ������ �������� ����� Configuration Bit FOSCSEL
        crc.sum += crc.data[0] & 0xF7;
        _memcpy_p2d16( &crc.data, __FOSC_BASE, 1 );        // ������ �������� ����� Configuration Bit FOSC
        crc.sum += crc.data[0] & 0xF3;
        _memcpy_p2d16( &crc.data, __FWDT_BASE, 1 );        // ������ �������� ����� Configuration Bit FWDT
        crc.sum += crc.data[0] & 0xFF;
        _memcpy_p2d16( &crc.data, __FPOR_BASE, 1 );        // ������ �������� ����� Configuration Bit FPOR
        crc.sum += crc.data[0] & 0xF8;
        _memcpy_p2d16( &crc.data, __FICD_BASE, 1 );        // ������ �������� ����� Configuration Bit FICD
        crc.sum += crc.data[0] & 0x6F;

        // ������ Checksum �� User ID ������� Flash-������ (����������� ������������)
        _memcpy_p2d16( &crc.data[1], __FUID0_BASE, 1 );    // ������ �������� ����� Checksum �� User ID Memory
        _memcpy_p2d16( &crc.data[0], __FUID1_BASE, 1 );    // ������ �������� ����� Checksum �� User ID Memory

        ASSERT_EX_ID( eGrPS_ControlMK, ePS_ControlMkFaultFlash,
                      crc.sum == (( uint16_t )crc.data[0] | ( uint16_t )( crc.data[1] << 8 )),
                      crc.sum,
                      ( uint16_t )crc.data[0] | ( uint16_t )crc.data[1] << 8,
                      0,
                      0 );

        InterChannel_synchronize( eICId_ControlMK_Flash, crc.sum );    // ������������� �������� Checksum

        crc.sum = 0;
        crc.adr = FLASH_ADDRESS_START;
    }
}

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   30-07-2018
* �����  ������ �.�.
* 
* ���������:
*    ������� ������.
* 
* ������ 1.0.2
* ����   03-12-2019
* �����  ��������� �.�.
* 
* ���������:
*    � �������� ��������� ������������� ����������� ����� ��� ��������� ����� �2.
*    ��-�� ��� ���������� ��������� �������� �������������� ���������� �� �����������.  
*    ��������� ����� �1 � ��������� ����� �3
 */
