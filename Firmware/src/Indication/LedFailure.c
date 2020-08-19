/**
* \file    Dk.c
* \brief   \copybrief LedFailure.h
*
* \version 1.0.1
* \date    20-03-2018
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "typeMK.h"
#include "ConfigMK.h"
#include "LedFailure.h"
    
//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief �������� ������� ��� �������� � ��������� ���� ���������.
/// \param time - �����, �� �������� ������� �������;
/// \param counter - ��������� �� ������� �������;
/// \param stateDK - ��������� ���������� "�����" (������� ��� ��������).
///
static bool checkTime( const uint16_t time, uint16_t* counter, const bool stateDK );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ���������� ����������� "�����" (�������� ��������� - ��������)
void LedFailure_ctor( void )
{
    _LATB12  = 1;
    _TRISB12 = 0;
}

//*****************************************************************************
// ��������� ��������� ���������� "�����"
void LedFailure_set( bool state )
{
    _TRISB12 = state;
    if( !state ) _LATB12 = 1;
}

//*****************************************************************************
static bool checkTime( const uint16_t time, uint16_t* counter, const bool stateDK )
{
    LedFailure_set( stateDK );
    if( (*counter)++ >=  time )
    {
        *counter = 0;
        return true;
    }
    else 
        return false;
}

//*****************************************************************************
// ������� ����������� "�����".
void LedFailure_blink( const uint16_t timeOn, const uint16_t timeOff )
{
    
    typedef enum 
    {
        eOn,
        eOff
    } PhaseInd;

    static PhaseInd phaseInd = eOn;
    static uint16_t cnt;
    
    switch( phaseInd )
    {
        case eOn:  
            if( checkTime( timeOn, &cnt, true ) )
                phaseInd = eOff;
                break;
        case eOff:  
            if( checkTime( timeOff, &cnt, false ) )
                phaseInd = eOn;
            break; 
    }
}

//*****************************************************************************
/**
* ������� ���������: 
*
* ������ 1.0.1
* ����   20-03-2018
* �����  ��������� �.�.
*
* ���������:
*    ������� ��������.
*/
