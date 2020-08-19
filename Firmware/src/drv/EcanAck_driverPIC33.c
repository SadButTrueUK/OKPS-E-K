/**
* \file    EcanAck_driverPIC33.c
* \brief   \copybrief EcanAck_driverPIC33.h
* 
* \version 1.0.3
* \date    08-07-2019
* \author  ������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "typeMK.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"
#include "Ecan_driverPIC33.h"
#include "EcanAck_driverPIC33.h"
#include "EcanAck_Pins.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������ ������ ������.
/// 
#define SIZE_MESSAGE     6

//*****************************************************************************
/// \brief ����� �������� ���������� ��������, � �������� 62,5 ���.
/// 
#define WAITE_TRANSMITE  8

//*****************************************************************************
/// \brief ���������� �������� �������� ��� ���������� �������������.
/// 
#define REPEAT_TRANSMITE 6

//*****************************************************************************
/// \brief ����� ������������� �������������� ������, � �������� 62,5 ���.
/// 
#define TIME_IGNORE_IN   ( WAITE_TRANSMITE * ( REPEAT_TRANSMITE + 1 ) )

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ��������.
///
typedef enum
{
    eEcanAckTx_Idle        = 0,          ///< ���������� �������� � ����� � ������
    eEcanAckTx_InProgress                ///< ������� �������� ������
} EcanTxState;

//*****************************************************************************
/// \brief ��������� ���������.
///
typedef enum
{
    eEcanAckRx_Idle        = 0,        ///< �������� ������� ������
    eEcanAckRx_InProgress              ///< �������� ������ ������ � ��������� ������������� ������
} EcanRxState;

//*****************************************************************************
/// \brief ������ ��������.
///
typedef struct
{
    ArrayIoDriver pFunctions;            ///< ������ ������������ �������.
    EcanRxState   receiveState;          ///< ��������� ���������.
    EcanTxState   transmiteState;        ///< ��������� �����������.
    uint16_t      ackState;              ///< ��������� ������������� ������ �� ���������� �����.
    uint8_t       aIn[SIZE_MESSAGE];     ///< �������� ����������.
    uint8_t       aTempIn[SIZE_MESSAGE]; ///< ������������� ����� ��� �������� ��������� �������� ������.
    bool          isInReady;             ///< ������� ������� �������� ����������.
    uint16_t      cTimeIn;               ///< ������� ������� �� ���������� ������.
    uint16_t      cWaiteTx;              ///< ������� ������ �������� ������������� ������ �������� ��.
    uint16_t      cRepeatTx;             ///< ������� �������� �������� � �������� ��.
    const void    *pDrv;                 ///< ��������� �� ������� \a CAN.
} EcanAck;

//*****************************************************************************
// ����������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
/// \brief ������ ��������� \a ECAN.
///
static EcanAck ecanAck =
{
    EcanAck_staticFunc() // ������������� ������� ������� ��� ������ CAN
};

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������������� �������� �� ��������� ��.
/// \details ������� ���������� ���������� ���� �����/������ ��� ������ 
/// ������������� ������ ����������� �����.
/// \param self - ��������� �� ������ �������, ������� ������ �����������.
/// \retval true  - ����������� ���� ������ ���������;
/// \retval false - ����������� ���� �� ������ ���������.
///
static bool EcanAck_isBinAck( const void *self );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//***************************************************************************** 
// �����������  
const ArrayIoDriver * EcanAck_ctor( const void *pDrv )
{
    uint8_t i;
    ecanAck.pDrv = pDrv;
    ecanAck.receiveState = eEcanAckRx_Idle;
    ecanAck.transmiteState = eEcanAckTx_Idle;
    ecanAck.ackState = 0;
    ecanAck.isInReady = false;
    ecanAck.cTimeIn = TIME_IGNORE_IN;
    for( i = 0; i < SIZE_MESSAGE; i++ )
    {
        ecanAck.aIn[i] = 0;
    }
    ECAN_ACK_INIC_PORTS;
    return &ecanAck.pFunctions;
}

//*****************************************************************************
// ������ ������ �� ������������� ����������
int EcanAck_get( const void *self, uint8_t *array, int size )
{
    ASSERT_ID( eGrPS_CanAckDrv, ePS_CanAckDrvSetSize, ( size == SIZE_MESSAGE ) );

    uint8_t i;
    EcanAck *me = ( EcanAck * )( self );

    for( i = 0; i < SIZE_MESSAGE; i++ )
    {
        array[i] = me->aIn[i];
    }
    me->isInReady = false;

    return SIZE_MESSAGE;
}

//*****************************************************************************
// ���������� ���������� � ������ �������� ������
bool EcanAck_isInReady( const void *self )
{
    EcanAck *me = ( EcanAck * )self;
    return me->isInReady;
}

//***************************************************************************** 
// �������� ������ �������� ��� ������
void EcanAck_set( const void *self, uint8_t *array, int size )
{
    ASSERT_ID( eGrPS_CanAckDrv, ePS_CanAckDrvSetSize, ( size == SIZE_MESSAGE ) );
    EcanAck *me = ( EcanAck * )self;
    Ecan_AbortTransmite( me->pDrv );
    Ecan_set( me->pDrv, array, size );
    Ecan_startTransmite( me->pDrv );
    me->cWaiteTx = 0;
    me->cRepeatTx = 0;
    me->transmiteState = eEcanAckTx_InProgress;
}

//*****************************************************************************
// ���������� ���������� � ������ ����� ������ 
bool EcanAck_isOutReady( const void *self )
{
    EcanAck *me = ( EcanAck * )self;
    return( me->transmiteState == eEcanAckTx_Idle ) ? true : false;
}

//***************************************************************************** 
// ������� ����
void EcanAck_run( const void *self )
{

    EcanAck *me = ( EcanAck * )self;
    Ecan_run( me->pDrv );

    // ��������� ���������� ������������� ������� ������ �� ��������� 
    // ����������, ���������� ��������� �� ��������� ������ ����� ������
    if( ecanAck.receiveState == eEcanAckRx_InProgress )
    {
        ecanAck.receiveState = eEcanAckRx_Idle;
        me->isInReady = true;
    }

    // ����� ������
    if( Ecan_isInReady( me->pDrv ) )
    {
        Ecan_get( me->pDrv, me->aIn, SIZE_MESSAGE );

        // ��������� ���������� ������������� ������� ������ �� ��������� 
        // ����������, ����� ����� ������ ���������� �� ���������.
        ecanAck.receiveState = eEcanAckRx_InProgress;
        ECAN_ACK_TOGGLE_LEVEL;
    }

    // �������� ������
    switch( me->transmiteState )
    {
        case eEcanAckTx_Idle:
            break;

        case eEcanAckTx_InProgress:
            if( EcanAck_isBinAck( me ) )
            {
                Ecan_AbortTransmite( me->pDrv );
                me->transmiteState = eEcanAckTx_Idle;
            }
            else
            {
                if( ++me->cWaiteTx >= WAITE_TRANSMITE )
                {
                    if( ++me->cRepeatTx >= REPEAT_TRANSMITE )
                    {
                        ERROR_ID( eGrPS_CanAckDrv, ePS_CanAckDrvNotAck );
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
// ������������� �������� �� ��������� ��.
static bool EcanAck_isBinAck( const void *self )
{
    uint8_t i, count = 0;
    bool ackState;

    EcanAck *me = ( EcanAck * )self;

    for( i = 0; i < 5; i++ )
    {
        if( ECAN_ACK_CHECK_HIGH )
        {
            count++;
        }
    }
    ackState = ( count >= 3 );
    if( me->ackState != ackState )
    {
        me->ackState = ackState;
        return true;
    }
    return false;
}

//***************************************************************************** 
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   26-05-2017
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
* 
* 
* ������  1.0.2
* ����    07-08-2018
* �����   ������� �.�.
* ���������:
*     - ����� �������������� ����� � ���������� ������ ���������� ������.
*     - ��������� ���������� ������������� ������� ������ �� ��������� 
*     ����������, ���������� ��������� �� ��������� ������, ����� ������.
* 
* ������  1.0.3
* ����    08-07-2019
* �����   ������� �.�.
* ���������:
*     1) ������� ��� ������ ������������ �������� dsPIC30CanAck_ctor � 
* const void * �� const ArrayIoDriver *. 
*     2) � ������ ��������� DsPIC30CanAck ��������� ���� pFunctions, �������  
* �������� ������ ������������ �������. ��������� ������������� ����� ���� ��� 
* ���������� dsPIC30CanAck � ������� dsPIC30CanAck_ctor ���������� ���������  
* �� ��� ����, � ��������� ��� ����������� � ������ ���� �� ����� ������������� 
* ������ ���� ���������.
*/
