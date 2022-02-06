/**
* \file    Tracing.c
* \brief   \copybrief Tracing.h
* 
* \version 1.0.2
* \date    05-03-2020
* \author  ������ �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "addressVarEeprom.h"
#include "Eeprom.h"
#include "Tracing.h"

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static uint16_t aTracingBuf[TRACING_BUF_QTY][TRACING_BUF_SIZE];    ///< ����� �����������.
static uint16_t aTracingBufIdx[TRACING_BUF_QTY];                   ///< ������ �������� ������� �����������.

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ���������� ������
void Tracing_ctor( void )
{
    for( uint16_t i = 0; i < TRACING_BUF_QTY; i++ ) 
        aTracingBufIdx[i] = 0;
}

//*****************************************************************************
// ������ ��������� � ����� �����������
void Tracing_parameter( uint16_t data, uint16_t numberArray )
{
    if( numberArray < TRACING_BUF_QTY )
    {
        (aTracingBufIdx[numberArray] < TRACING_BUF_SIZE) ? : (aTracingBufIdx[numberArray] = 0);     // �������� ������� ������ ����������� �� ����� �� �������

        aTracingBuf[numberArray][aTracingBufIdx[numberArray]++] = data;                          // ������ ��������� � ����� �����������
    }
}

//*****************************************************************************
// ����������� ��������� � ���������������
void Tracing_parameterId( uint16_t data, uint16_t id, uint16_t numberArray )
{
    Tracing_parameter( id,   numberArray );                                     // ������ � ����� ����������� �������������� ������
    Tracing_parameter( data, numberArray );                                     // ������ � ����� ����������� ������
}

//*****************************************************************************
// ���������� ������ ����������� � EEPROM
void Tracing_saveBlackBox( void )
{
    uint16_t adr = ADDRESS_EEPROM_TRACING_START;                                // ��������� ����� ������� ����������� � EEPROM

    Eeprom_write( ADDRESS_EEPROM_STRUCT_TRACING, ADDRESS_EEPROM_TRACING_START );// ���������� � EEPROM ���������� ������ ������� �����������
    Eeprom_write( ADDRESS_EEPROM_STRUCT_TRACING + 2, TRACING_BUF_QTY );         // ���������� � EEPROM ���������� ������� �����������
    Eeprom_write( ADDRESS_EEPROM_STRUCT_TRACING + 4, TRACING_BUF_SIZE );        // ���������� � EEPROM ������� ������ �����������
    
    for( uint16_t i = 0; i < TRACING_BUF_QTY; i++ )
    {
        for( uint16_t j = 0; j < TRACING_BUF_SIZE; j++ )
        {
            if ( aTracingBufIdx[i] >= TRACING_BUF_SIZE ) 
                aTracingBufIdx[i] = 0; 
            
            Eeprom_write( adr, aTracingBuf[i][aTracingBufIdx[i]++] );             // ���������� � EEPROM �������� ������ �����������
            adr += 2;
        }
    }
}

//*****************************************************************************
/*
* ���������:
*    ����������� ���������� ���������� � ����������� ������.
*    ��� �������� ������� � �� ������ �����������
*    ����������� � EEPROM.
*/

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   24-03-2016
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
* 
* ������ 1.0.2
* ����   05-03-2020
* �����  ������ �.�.
* 
* ���������:
*   ��������� ���������� ������� �� ����.
*/
