/**
* \file    ManageCanDriver.c
* \brief   \copybrief ManageCanDriver.h
*
* \version 1.0.6
* \date    05-02-2021
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <xc.h>
#include <string.h>
#include "iodrv.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"
#include "Main.h" 
#include "EcanDriver.h"
#include "ManageCanDriver.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������������ ������ ������ ������.
///
#define SIZE_MAX_MESSAGE    8

//*****************************************************************************
/// \brief ����� �������� ���������� ��������, � ����������� ������� �������.
///
#define WAITE_TRANSMITE     10

//*****************************************************************************
/// \brief ���������� �������� �������� ��� ���������� �������������.
///
#define REPEAT_TRANSMITE    4

//*****************************************************************************
/// \brief ����� �������� ���� �������, ���.
///
#define TIME_TRANSMITE_2_PACKET ( 110 * 2 + 12 + 32 )  // 110 ����� ������, 12 -����� ��������, 32 - �� ������� �������

//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ��������.
///
typedef enum
{
    eTransmiteState_Idle        = 0,              ///< ���������� �������� � ����� � ������
    eTransmiteState_Start,                        ///< ������ �������� ������
    eTransmiteState_InProgress                    ///< ������� �������� ������
} TransmiteState;

//*****************************************************************************
/// \brief ��������� ������ ��������.
///
typedef struct
{
    ArrayIoDriver  pFunctions;                    ///< ������ ������������ �������.
    TransmiteState transmiteState;                ///< ��������� �����������.
    uint8_t        aIn[SIZE_MAX_MESSAGE];         ///< �������� ����������.
    uint8_t        aOut[SIZE_MAX_MESSAGE];        ///< ���������� ��� ��������.
    bool           isInReady;                     ///< ������� ������� �������� ����������.
    uint16_t       cWaiteTx;                      ///< ������� ������ �������� ������������� ������ �������� ��.
    uint16_t       cRepeatTx;                     ///< ������� �������� �������� � �������� ��.
    const void     *pDrv;                         ///< ��������� �� ������� CAN.
    uint8_t        lengthPacket;                  ///< ����� ������ ������.
    uint8_t        numberPacketOut;               ///< ���������� ��� ����������� ������ ������ � ������������ ������. 
    uint8_t        numberPacketIn;                ///< ���������� ��� ������ ���������� ��������� ������ ������. 
} ManageCanDriver;

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static ManageCanDriver manageCanDriver;           ///< ������ ��������.

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������ ���������� �������� (���������� � ��������).
/// \param self - ��������� �� ������ �������� �� ��������� ��.
/// \retval true  - ��;
/// \retval false - ���.
///
static bool ManageCanDriver_isOut( const void *self );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������ ���������� ��������.
uint16_t ManageCanDriver_getError( const void *self )
{
    ERROR_ID( eGrPS_ManageCanDrv, ePS_ErrorAlg );
    return 0;
}

//*****************************************************************************
// ����� ��������.
void ManageCanDriver_reset( const void *self )
{
    ERROR_ID( eGrPS_ManageCanDrv, ePS_ErrorAlg );
}

//*****************************************************************************
// ������ �������� � ��������
void ManageCanDriver_start( const void *self, uint8_t *array, uint16_t size )
{
    ERROR_ID( eGrPS_ManageCanDrv, ePS_ErrorAlg );
}

//*****************************************************************************
// �����������  
const ArrayIoDriver *ManageCanDriver_ctor( EcanNumber numCan,
                                    uint16_t txSid,
                                    uint16_t rxSid,
                                    uint16_t mode,
                                    uint16_t lengthPacket )
{
    manageCanDriver.pFunctions.run = ManageCanDriver_run;
    manageCanDriver.pFunctions.isInReady = ManageCanDriver_isInReady;
    manageCanDriver.pFunctions.isOutReady = ManageCanDriver_isOutReady;
    manageCanDriver.pFunctions.set = ManageCanDriver_set;
    manageCanDriver.pFunctions.get = ManageCanDriver_get;
    manageCanDriver.pFunctions.getError = ManageCanDriver_getError;
    manageCanDriver.pFunctions.reset = ManageCanDriver_reset;
    manageCanDriver.pFunctions.start = ManageCanDriver_start;

    ASSERT_ID( eGrPS_ManageCanDrv, ePS_ErrorParameters, lengthPacket < SIZE_MAX_MESSAGE );

    manageCanDriver.lengthPacket = lengthPacket;
    manageCanDriver.numberPacketOut = 0;
    manageCanDriver.numberPacketIn = 0xAA;

    manageCanDriver.transmiteState = eTransmiteState_Idle;
    manageCanDriver.isInReady = false;
    memset( manageCanDriver.aIn, 0, SIZE_MAX_MESSAGE );

    manageCanDriver.pDrv = Ecan_ctor( numCan, txSid, rxSid, mode, manageCanDriver.lengthPacket + 1 );

    return( const ArrayIoDriver * )&manageCanDriver;
}

//*****************************************************************************
// ������ ������ �� ������������� ����������
uint16_t ManageCanDriver_get( const void *self, uint8_t *array, uint16_t size )
{
    ManageCanDriver *me = ( ManageCanDriver * )( self );

    ASSERT_ID( eGrPS_ManageCanDrv, ePS_ErrorParameters, size == ( me->lengthPacket ) );
    memcpy( array, me->aIn, size );
    me->isInReady = false;

    return size;
}

//*****************************************************************************
// ���������� ���������� � ������ �������� ������
bool ManageCanDriver_isInReady( const void *self )
{
    ManageCanDriver *me = ( ManageCanDriver * )self;
    return me->isInReady;
}

//*****************************************************************************
// �������� ������ �������� ��� ������
void ManageCanDriver_set( const void *self, uint8_t *array, uint16_t size )
{
    ManageCanDriver *me = ( ManageCanDriver * )self;
    ASSERT_ID( eGrPS_ManageCanDrv, ePS_ErrorParameters, size == ( me->lengthPacket ) );

    memcpy( me->aOut, array, size );
    me->transmiteState = eTransmiteState_Start;
}

//*****************************************************************************
// ���������� ���������� � ������ ����� ������ 
bool ManageCanDriver_isOutReady( const void * self )
{
    ManageCanDriver *me = ( ManageCanDriver * )self;
    return(me->transmiteState == eTransmiteState_Idle ) ? true : false;
}

//*****************************************************************************
// ������� ����
void ManageCanDriver_run( const void *self )
{
    ManageCanDriver *me = ( ManageCanDriver * )self;
    
    Ecan_run( me->pDrv );

    // ����� ������
    if( Ecan_isInReady( me->pDrv ) )
    {
        Ecan_get( me->pDrv, me->aIn, me->lengthPacket + 1 );

        if( me->numberPacketIn != me->aIn[me->lengthPacket] )
        {
            me->numberPacketIn = me->aIn[me->lengthPacket];
            me->isInReady = true;
        }
    }

    // �������� ������
    switch( me->transmiteState )
    {
        case eTransmiteState_Idle:

            break;
        case eTransmiteState_Start:
        {
            uint16_t periodTime = Main_getPeriodCycle();
            uint16_t cycleTime  = Main_getCurrentTimeCycle();
            uint16_t remainTime = ( periodTime <= cycleTime ) ? 0 : ( periodTime - cycleTime ); 
            if( remainTime > TIME_TRANSMITE_2_PACKET )
            {
                me->aOut[me->lengthPacket] = me->numberPacketOut++;
                Ecan_set( me->pDrv, me->aOut, me->lengthPacket + 1 );
                Ecan_startTransmite( me->pDrv );

                me->cWaiteTx = 0;
                me->cRepeatTx = 0;
                me->transmiteState = eTransmiteState_InProgress;
            }
        }
            break;
        case eTransmiteState_InProgress:
            if( ManageCanDriver_isOut( me ) )
            {
                uint16_t error = Ecan_getError( me->pDrv );
                if( error == ePS_NoError )
                {
                    me->transmiteState = eTransmiteState_Idle;
                }
                else
                {
                    if( ++me->cRepeatTx >= REPEAT_TRANSMITE )
                    {
                        ERROR_ID( eGrPS_ManageCanDrv, error );
                    }
                    else
                    {
                        Ecan_startTransmite( me->pDrv );
                        me->cWaiteTx = 0;
                    }
                }
            }
            else
            {
                if( ++me->cWaiteTx >= WAITE_TRANSMITE )
                {
                    if( ++me->cRepeatTx >= REPEAT_TRANSMITE )
                    {
                        ERROR_ID( eGrPS_ManageCanDrv, ePS_NoAck );
                    }
                    else
                    {
                        Ecan_AbortTransmite( me->pDrv );
                        Ecan_startTransmite( me->pDrv );
                        me->cWaiteTx = 0;
                    }
                }
            }
            break;
    }
}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
// ������ ���������� �������� (���������� � ��������)
bool ManageCanDriver_isOut( const void *self )
{
    ManageCanDriver *me = ( ManageCanDriver * )self;
    return Ecan_isOutReady( me->pDrv );
}

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   25-10-2018
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
* 
* ������ 1.0.2
* ����   25-06-2019
* �����  ��������� �.�.
* 
* ���������:
*    ��������� �������� WAITE_TRANSMITE � ����� � ���, ��� ����������
*    ������� ������ ������� run. 
* 
* ������ 1.0.3
* ����   23-08-2019
* �����  ��������� �.�.
*
* ���������:
*    ������� ��������� ������������. 
*
* ������ 1.0.4
* ����   06-07-2020
* �����  ��������� �.�.
*
* ���������:
*    ��������� ����� ���� ������: TIME_TRANSMITE_2_PACKET
* 
* ������ 1.0.5
* ����   04-12-2020
* �����  ��������� �.�.
*
* ���������:
*    ��������� ����� ���� ������: TIME_TRANSMITE_2_PACKET
* 
* ������ 1.0.6
* ����   05-02-2021
* �����  ��������� �.�.
*
* ���������:
*    ���� �������� ����� 0 ��� ����������� ������� �� ����� �� �����.
*/
