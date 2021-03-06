/**
* \file    shuntShiftGenDrv.c
* \brief   \copybrief shuntShiftGenDrv.h
*
* \version 1.0.1
* \date    12-10-2018
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdbool.h>
#include "typeMK.h"
#include "shuntShiftGenDrv.h"
#include "ConfigMK.h"

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ���������� ���� ������ 3-������� �����.
///
static void OffSw( void );

//*****************************************************************************
/// \brief ��������� ������������� � ������� ���.
/// \param value - ��������� �� �������� ���.
///
static void SetPDC( const int16_t *value );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� 3-������� ����������
void ShuntShiftGenDrv_init( void )
{
    if( ConfigMK_isMaster( ) )
    {
        //��������� ������� 3-������� ����
        PTCONbits.PTEN     = 0;
        PTCONbits.SEVTPS   = 1;        // Postscaler 1:2
        IOCON1bits.PENL    = 0;        // ������ ���� �������������� ������� PWM
        IOCON1bits.PENH    = 0;        // ������� ���� �������������� ������� PWM
        IOCON2bits.PENL    = 0;        // ������ ���� �������������� ������� PWM
        IOCON2bits.PENH    = 0;        // ������� ���� �������������� ������� PWM
        IOCON3bits.PENL    = 0;        // ������ ���� �������������� ������� PWM
        IOCON3bits.PENH    = 0;        // ������� ���� �������������� ������� PWM
        IOCON4bits.PMOD    = 1;        // For Redundant Output mode
        IOCON4bits.PENL    = 1;        // ������ ���� �������������� ������� PWM
        IOCON4bits.PENH    = 0;        // ������� ���� �������������� ��� GPIO
        IOCON5bits.PMOD    = 1;        // For Redundant Output mode
        IOCON5bits.PENL    = 1;        // ������ ���� �������������� ������� PWM
        IOCON5bits.PENH    = 0;        // ������� ���� �������������� ��� GPIO
        IOCON6bits.PMOD    = 1;        // For Redundant Output mode
        IOCON6bits.PENL    = 1;        // ������ ���� �������������� ������� PWM
        IOCON6bits.PENH    = 0;        // ������� ���� �������������� ��� GPIO
        PWMCON4bits.DTC    = 2;        // ������ ������� ������� ������ PWM4
        PWMCON5bits.DTC    = 2;        // ������ ������� ������� ������ PWM5
        PWMCON6bits.DTC    = 2;        // ������ ������� ������� ������ PWM6
        DTR4               = 0;        // Deadtime setting
        ALTDTR4            = 0;        // Deadtime setting
        DTR5               = 0;        // Deadtime setting
        ALTDTR5            = 0;        // Deadtime setting
        DTR6               = 0;        // Deadtime setting
        ALTDTR6            = 0;        // Deadtime setting
        PHASE4             = 0;        // No phase shift
        PHASE5             = 0;        // No phase shift
        PHASE6             = 0;        // No phase shift
        PWMCON4bits.CAM    = 0;        // ��������� ������������ �� ���� ���
        FCLCON4bits.FLTMOD = 3;        
        PWMCON5bits.CAM    = 0;        // ��������� ������������ �� ���� ���
        FCLCON5bits.FLTMOD = 3;        
        PWMCON6bits.CAM    = 0;        // ��������� ������������ �� ���� ���
        FCLCON6bits.FLTMOD = 3;        
        // ���� ������ � ������� - �� �����
        _TRISA7            = 0;
        _TRISD1            = 0;
        _TRISD3            = 0;
        OffSw( );
    }
}

//*****************************************************************************
// ����������, ���������� ������� ��� IO Ports
void ShuntShiftGenDrv_off( void )
{
    //3-������ PWM ���������� � Master 
    if( ConfigMK_isMaster( ) ) 
    {
        PTCONbits.PTEN = 0; //���������� ������ ���
        OffSw( );
    }
}

//*****************************************************************************
// ���������, ��������� ������� � ��������� �������� 
bool ShuntShiftGenDrv_on( uint16_t period, const int16_t *iniValueArr )
{
    if( ConfigMK_isMaster( ) ) //����� �� �������, ���� �� - Slave
    {    
        //����������� ��������� ��������� ��� Master
        PTPER = period; // ������� �������� � PTPER
        SetPDC( iniValueArr ); //��������� ��������� ��������
        PTCONbits.PTEN = 1; //��������� ������ PWM
        return true;
    }
    return false;
}

//*****************************************************************************
// ��������� ������� ��������
bool ShuntShiftGenDrv_setValues( const int16_t *valueArr )
{
    //����� �� �������, ���� �� - Slave
    if( ConfigMK_isMaster( ) )
    {
        SetPDC( valueArr ); //��������� ������� �������� 
        return true;
    }
    return false;
}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
// ���������� ���� ������ 3-������� �����
static void OffSw( void )
{
    //� ����������� ��������� ���������� �������� ������ �����
    _LATA7 = 0;
    _LATD1 = 0;
    _LATD3 = 0;
}

//*****************************************************************************
// ��������� ������������� � ������� ���
static void SetPDC( const int16_t *value )
{
    PDC4 = value[0];
    PDC5 = value[1];
    PDC6 = value[2];
}

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   12-10-2018
* �����  ��������� �.�.
* 
* ���������:
*     ������� ������.
*/
