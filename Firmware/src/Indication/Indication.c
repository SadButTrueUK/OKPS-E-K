/**
* \file    Indication.c
* \brief   \copybrief Indication.h
*
* \version 1.0.1
* \date    24-02-2017
* \author  ���������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "Indication.h"
#include "typeMK.h"
#include "WorkWithBits.h"

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ����� �����-������ � ������� ���������� ���������.
/// \param trisPort - ��������� �� ������� ��������� ����������� ������;
/// \param pinPort - ��������� �� ����� �����;
/// \param pinNum - ����� ����� �����;
/// \param enCtrType - ��� ���������� �����������;
/// \param outPhase - ���� ��������� ����������.
///
static void indBitSet( volatile uint16_t *trisPort, volatile uint16_t *pinPort, uint16_t pinNum,
                       IndCtrType enCtrType, IndOutPhase outPhase );

//*****************************************************************************
/// \brief ��������� ����� �����-������ � ������ ���������� ���������.
/// \param trisPort - ��������� �� ������� ��������� ����������� ������;
/// \param pinPort - ��������� �� ����� �����;
/// \param pinNum - ����� ����� �����;
/// \param enCtrType - ��� ���������� �����������;
/// \param outPhase - ���� ��������� ����������.
///
static void indBitClr( volatile uint16_t *trisPort, volatile uint16_t *pinPort, uint16_t pinNum,
                       IndCtrType enCtrType, IndOutPhase outPhase );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ����������� ������� ����������
void Indication_ctorLed( indOutput_type *h, volatile uint16_t *trisOut, volatile uint16_t *portOut, uint16_t pinOut,
                         IndCtrType ctrType )
{
    //Set output parameters
    h->outPin.trisPort = trisOut;
    h->outPin.pinPort = portOut;
    h->outPin.pinNum = pinOut;
    h->outCtrType = ctrType;
    h->outState = eOffState;
    h->isDummy = false;
}

//*****************************************************************************
// ������������� ����������� ������� ���������� ��� ��������
void Indication_ctorDummy( indOutput_type *h )
{
    h->isDummy = true;
}

//*****************************************************************************
// ��������� ����������
void Indication_on( indOutput_type *h )
{
    h->outState = eOnState;
    h->cntOn1 = h->tOn1 = 0;
    h->cntOn2 = h->tOn2 = 0;
    h->cntOff1 = h->tOff1 = 0;
    h->cntOff2 = h->tOff2 = 0;
}

//*****************************************************************************
// ���������� ����������
void Indication_off( indOutput_type *h )
{
    h->outState = eOffState;
    h->cntOn1 = h->tOn1 = 0;
    h->cntOn2 = h->tOn2 = 0;
    h->cntOff1 = h->tOff1 = 0;
    h->cntOff2 = h->tOff2 = 0;
}

//*****************************************************************************
// ������� ����������� ���������� � �������� ��������� �����
void Indication_blink( indOutput_type *h, uint16_t timeOn, uint16_t timeOff, IndOutPhase outPhase )
{
    h->outState = eBlinkPrdState;
    h->outPhase = outPhase;
    h->cntOn1 = h->tOn1 = timeOn;
    h->cntOn2 = h->tOn2 = 0;
    h->cntOff1 = h->tOff1 = timeOff;
    h->cntOff2 = h->tOff2 = 0;
}

//*****************************************************************************
// ������� ����������� ���������� � ������� �������� � �������� ��������� �����
void Indication_blink2( indOutput_type *h, uint16_t timeOn1, uint16_t timeOff1, uint16_t timeOn2, uint16_t timeOff2,
                        IndOutPhase outPhase )
{
    h->outState = eBlinkTwiceState;
    h->outPhase = outPhase;
    h->cntOn1 = h->tOn1 = timeOn1;
    h->cntOn2 = h->tOn2 = timeOn2;
    h->cntOff1 = h->tOff1 = timeOff1;
    h->cntOff2 = h->tOff2 = timeOff2;
}

//*****************************************************************************
// ��������� ���������� ���������� �� �������� �����
void Indication_pulseOn( indOutput_type *h, uint16_t time )
{
    h->outState = ePulseOnState;
    h->cntOn1 = h->tOn1 = time;
    h->cntOn2 = h->tOn2 = 0;
    h->cntOff1 = h->tOff1 = 0;
    h->cntOff2 = h->tOff2 = 0;
}

//*****************************************************************************
// ��������� ��������� ����������� ������� ����������
void Indication_run( indOutput_type *h )
{

    //Return if structure is dummy
    if( h->isDummy == true ) return;
    //Output settings
    switch( h->outState )
    {
        case eOffState:
            indBitClr( h->outPin.trisPort, h->outPin.pinPort, h->outPin.pinNum, h->outCtrType, eIndPhNormal );
            break;
        case eOnState:
            indBitSet( h->outPin.trisPort, h->outPin.pinPort, h->outPin.pinNum, h->outCtrType, eIndPhNormal );
            break;
        case ePulseOnState:
            if( h->cntOn1 != 0 )
            {
                indBitSet( h->outPin.trisPort, h->outPin.pinPort, h->outPin.pinNum, h->outCtrType, eIndPhNormal );
                h->cntOn1--;
            }
            else
            {
                indBitClr( h->outPin.trisPort, h->outPin.pinPort, h->outPin.pinNum, h->outCtrType, eIndPhNormal );
                h->outState = eOffState;
            }
            break;
        case ePulseOffState:
            if( h->cntOff1 != 0 )
            {
                indBitClr( h->outPin.trisPort, h->outPin.pinPort, h->outPin.pinNum, h->outCtrType, eIndPhNormal );
                h->cntOff1--;
            }
            else
            {
                indBitSet( h->outPin.trisPort, h->outPin.pinPort, h->outPin.pinNum, h->outCtrType, eIndPhNormal );
                h->outState = eOnState;
            }
            break;
        case eBlinkPrdState:
            if( h->cntOn1 != 0 )
            {
                indBitSet( h->outPin.trisPort, h->outPin.pinPort, h->outPin.pinNum, h->outCtrType, h->outPhase );
                h->cntOn1--;
            }
            else if( h->cntOff1 != 0 )
            {
                indBitClr( h->outPin.trisPort, h->outPin.pinPort, h->outPin.pinNum, h->outCtrType, h->outPhase );
                h->cntOff1--;
            }
            else
            {
                h->cntOn1 = h->tOn1;
                h->cntOff1 = h->tOff1;
            }
            break;
        case eBlinkTwiceState:
            if( h->cntOn1 != 0 )
            {
                indBitSet( h->outPin.trisPort, h->outPin.pinPort, h->outPin.pinNum, h->outCtrType, h->outPhase );
                h->cntOn1--;
            }
            else if( h->cntOff1 != 0 )
            {
                indBitClr( h->outPin.trisPort, h->outPin.pinPort, h->outPin.pinNum, h->outCtrType, h->outPhase );
                h->cntOff1--;
            }
            else if( h->cntOn2 != 0 )
            {
                indBitSet( h->outPin.trisPort, h->outPin.pinPort, h->outPin.pinNum, h->outCtrType, h->outPhase );
                h->cntOn2--;
            }
            else if( h->cntOff2 != 0 )
            {
                indBitClr( h->outPin.trisPort, h->outPin.pinPort, h->outPin.pinNum, h->outCtrType, h->outPhase );
                h->cntOff2--;
            }
            else
            {
                h->cntOn1 = h->tOn1;
                h->cntOff1 = h->tOff1;
                h->cntOn2 = h->tOn2;
                h->cntOff2 = h->tOff2;
            }
            break;
        default:
            break;
    }
}

//*****************************************************************************
// ���������� ��������� �������*
//*****************************************************************************

//*****************************************************************************
// ��������� ����� �����-������ � ������� ���������� ���������
static void indBitSet( volatile uint16_t *trisPort, volatile uint16_t *pinPort, uint16_t pinNum,
                       IndCtrType enCtrType, IndOutPhase outPhase )
{
    //Set pin type as output
    CLEARBIT( *trisPort, pinNum );
    //Set bit
    switch( enCtrType )
    {
        case eCtrEnHigh:
            if( outPhase == eIndPhNormal )
            {
                SETBIT( *pinPort, pinNum );
            }
            else
            {
                CLEARBIT( *pinPort, pinNum );
            }
            break;
        case eCtrEnLow:
            if( outPhase == eIndPhNormal )
            {
                CLEARBIT( *pinPort, pinNum );
            }
            else
            {
                SETBIT( *pinPort, pinNum );
            }
            break;
        default:
            break;
    }
}

//*****************************************************************************
// ��������� ����� �����-������ � ������ ���������� ���������
static void indBitClr( volatile uint16_t *trisPort, volatile uint16_t *pinPort, uint16_t pinNum,
                       IndCtrType enCtrType, IndOutPhase outPhase )
{
    //Set pin type as output
    CLEARBIT( *trisPort, pinNum );
    //Clear bit
    switch( enCtrType )
    {
        case eCtrEnHigh:
            if( outPhase == eIndPhNormal )
            {
                CLEARBIT( *pinPort, pinNum );
            }
            else
            {
                SETBIT( *pinPort, pinNum );
            }
            break;
        case eCtrEnLow:
            if( outPhase == eIndPhNormal )
            {
                SETBIT( *pinPort, pinNum );
            }
            else
            {
                CLEARBIT( *pinPort, pinNum );
            }
            break;
        default:
            break;
    }
}

//*****************************************************************************
/**
* ������� ���������: 
*
* ������ 1.0.1
* ����   24-02-2017
* �����  ���������� �.�.
*
* ���������:
*    ������� ��������.
*/
