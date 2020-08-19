/**
* \file    Ecan_driverPIC33.c
* \brief   \copybrief Ecan_driverPIC33.h
*
* \version 1.0.1
* \date    06-10-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "typeMK.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"
#include "Ecan_regsPIC33.h"
#include "MainRegisters.h"
#include "CheckRam.h"
#include "Main.h"
#include "Tracing.h"
#include "ECAN_driverPIC33.h"
#include "Ecan_DMA.h"
#include "ECAN1_PinRemap.h"
#include "ECAN2_PinRemap.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
/// \brief �������������� ��������� �� ������ � ��������� �� \a ECAN.
/// \param self_ � ��������� �� ������.
///
#define DRV_DATA( self_ ) ( ( Ecan * ) self_ )

//*****************************************************************************
/// \brief ��������� �� �������� ������ \a CAN. 
/// \details ������ ������������ ��� ��������� ������ ���������� �� ��������
/// ������ \a CAN.
/// \param self_ � ��������� �� ��������� \a ECAN.
/// 
#define DRV_REGS( self_ ) ( ( volatile Ecan * ) self_ )->pCanRegs

//*****************************************************************************
#define NUM_BUFF_IN            8         ///< ����� ������ ��� �������� �������� ���������.
#define NUM_BUFF_OUT           0         ///< ����� ������ ��� �������� ������������ ���������.
#define NAME_BIT_RXFULL        RXFUL8    ///< ��� ���� � �������� �����������.
#define NUMBER_OF_ECAN_MESSAGE 32        ///< ���������� ������� ��������� ��� ������ \a ECAN.
#define SIZE_OF_ECAN_MESSAGE   8         ///< ������ ������ ��������� ��� ������ \a ECAN.

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� �����������.
///
typedef enum
{
    eEcanTrStateIdle         = 0,        ///< ���������� �������� � ����� � ������
    eEcanTrStateInProgress,              ///< ���������� � �������� �������� ������
    //  eEcanTrStateRequest,             ///< ���������� �������� ������ �� �������� ������
    eEcanTrStateError                    ///< �������� ����������� �������
} EcanTrState;

//*****************************************************************************
/// \brief ����������� ��� ��������� ������� � �������� \a CxINTF.
///
typedef union
{
    unsigned short data;        ///< ���������� ��� ��������� �������.
    C1INTFBITS     bits;        ///< ��������� C1INTFBITS.
} TempINTF;

//*****************************************************************************
/// \brief ������ ��������.
///
typedef struct
{
    bool        receiverIsReady;                  ///< ������ ������� � ������ � ������.
    EcanTrState transmissionState;                ///< ��������� �����������.
    TempINTF    tempINTF;                         ///< ����� ��� �������� �������� �������� \a INTF.
    EcanMode    workMode;                         ///< ����� ������ ������.
    uint16_t    lengthPacket;                     ///< ����� ������ ������.
    volatile    dsPIC33_CanRegs *pCanRegs;        ///< ��������� �� �������� ������ \a CAN.
    uint8_t     i;                                ///< �������.
    uint16_t    *pData;                           ///< ��������� ������� ������.
} Ecan;

//*****************************************************************************
// ����������� ��������� �������������� ��������
//*****************************************************************************

//*****************************************************************************
const uint16_t MAX_DATA_LENGHT = 8;        ///< ������������ ����� ������ ������.

//*****************************************************************************
// ����������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static Ecan aEcan[eEcanCount];        ///< ������ ��������� \a ECAN.

//*****************************************************************************
/// \brief ������ ��������� ������ \a ECAN1.
///
__eds__ uint16_t aEcan1MsgBuf[NUMBER_OF_ECAN_MESSAGE][SIZE_OF_ECAN_MESSAGE]
__attribute__( ( address( RAM_END_ADDRESS ) ) );

//*****************************************************************************
/// \brief ������ ��������� ������ \a ECAN2.
///
__eds__ uint16_t aEcan2MsgBuf[NUMBER_OF_ECAN_MESSAGE][SIZE_OF_ECAN_MESSAGE]
__attribute__( ( address( RAM_END_ADDRESS + NUMBER_OF_ECAN_MESSAGE * 16 ) ) );

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************   
/// \brief ������� ������ \a CAN � �������� �����.
/// \param self - ��������� �� ������ �������, ������� ������ �����������;
/// \param mode - �������� �����.
///
void Ecan_setMode( const void *self, uint16_t mode );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************  
// ����� ������ CAN.
//void Ecan_reset( const void *self );

//*****************************************************************************
// �����������
const void *Ecan_ctor( EcanNumber numEcan,
                       uint16_t txSid,
                       uint16_t rxSid,
                       uint16_t mode,
                       uint16_t lengthPacket )
{
    Ecan *pEcan;
    unsigned long localAddress;

    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvNumPort,
               ( numEcan >= eEcan1 ) && ( numEcan < eEcanCount ) );
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSetSize,
               lengthPacket <= MAX_DATA_LENGHT );

    pEcan = &aEcan[numEcan];
    pEcan->receiverIsReady = false;
    pEcan->transmissionState = eEcanTrStateIdle;
    pEcan->lengthPacket = lengthPacket;

    switch( numEcan )
    {
        case eEcan1:
            pEcan->pCanRegs = &dsPIC33_Can1Regs;
            ECAN1_PIN_REMAP;
            ECAN_CONFIG_DMA( DMA0, eDMA_readRAM, eDMA_ECAN1_TX, C1TXD, aEcan1MsgBuf, localAddress );
            ECAN_CONFIG_DMA( DMA1, eDMA_writeRAM, eDMA_ECAN1_RX, C1RXD, aEcan1MsgBuf, localAddress );
            aEcan1MsgBuf[NUM_BUFF_OUT][0] = ( txSid << 2 );
            aEcan1MsgBuf[NUM_BUFF_OUT][1] = 0x0000;
            aEcan1MsgBuf[NUM_BUFF_OUT][2] = lengthPacket;
            break;
        case eEcan2:
            pEcan->pCanRegs = &dsPIC33_Can2Regs;
            ECAN2_PIN_REMAP;
            ECAN_CONFIG_DMA( DMA2, eDMA_readRAM, eDMA_ECAN2_TX, C2TXD, aEcan2MsgBuf, localAddress );
            ECAN_CONFIG_DMA( DMA3, eDMA_writeRAM, eDMA_ECAN2_RX, C2RXD, aEcan2MsgBuf, localAddress );
            aEcan2MsgBuf[NUM_BUFF_OUT][0] = ( txSid << 2 );
            aEcan2MsgBuf[NUM_BUFF_OUT][1] = 0x0000;
            aEcan2MsgBuf[NUM_BUFF_OUT][2] = lengthPacket;
            break;
        default:
            ERROR_ID( eGrPS_CanDrv, ePS_CanDrvNumPort );
            break;
    }
    pEcan->workMode = mode; // ����� ������ ������

    //****************************************************************************
    // ��������� ������ CAN

    Ecan_setMode( ( const void * )pEcan, eConfigMode );

    pEcan->pCanRegs->CTRL1bits.WIN = 0;    // ���� ��������� 0
    pEcan->pCanRegs->CTRL1bits.CSIDL = 1;  // ������ ������ CAN � ������ "Idle" 
    pEcan->pCanRegs->CTRL1bits.CANCKS = 1; // ����� ������� Fcan = 2 * 59,904 MHz = 119,808 MHz
    pEcan->pCanRegs->CTRL1bits.CANCAP = 1; // CAN capture �������

    pEcan->pCanRegs->CFG1bits.SJW = 0; // ������ �������� ������������� ����� 1*Tq 
    pEcan->pCanRegs->CFG1bits.BRP = 7; // ���������� Tq (����� �������) ������ 119.808 MHz / 2 / 8
    // = 135/2 = 7,488 MHz 
    // ����������� ����� �������� ���� (Nominal Bit Time - NBT) 
    // = 8=**q = (936000 ���/�)
    pEcan->pCanRegs->CFG2bits.PRSEG = 1;
    pEcan->pCanRegs->CFG2bits.SEG1PH = 1;
    pEcan->pCanRegs->CFG2bits.SEG2PH = 2;
    pEcan->pCanRegs->CFG2bits.SAM = 1;
    pEcan->pCanRegs->CFG2bits.SEG2PHTS = 0;
    pEcan->pCanRegs->CFG2bits.WAKFIL = 0;

    pEcan->pCanRegs->FCTRLbits.DMABS = 0b110;

    // ������������ ��������� CAN 
    pEcan->pCanRegs->FMSKSEL1bits.F0MSK = 0;  // Select Acceptance Filter Mask 0 for Acceptance Filter 0
    pEcan->pCanRegs->CTRL1bits.WIN = 1;       // ���� ��������� 1
    pEcan->pCanRegs->RXM0SID = ( 0x7FF << 5 );
    pEcan->pCanRegs->RXF0SIDbits.SID = rxSid; // Configure Acceptance Filter 0 to match standard identifier
    pEcan->pCanRegs->RXM0SIDbits.MIDE = 0x1;  // Acceptance Filter 0 to check for Standard Identifier
    pEcan->pCanRegs->RXF0SIDbits.EXIDE = 0x0;
    pEcan->pCanRegs->BUFPNT1bits.F0BP = NUM_BUFF_IN; // Acceptance Filter 0 to use Message Buffer xx to store message
    pEcan->pCanRegs->CTRL1bits.WIN = 0;              // ���� ��������� 0
    pEcan->pCanRegs->FEN1bits.FLTEN0 = 0x1;          // Filter 0 enabled for Identifier match with incoming message 
    pEcan->pCanRegs->CTRL1bits.WIN = 0x0;            // Clear Window Bit to Access ECAN

    // ������������ ����������� CAN 
    /* Configure Message Buffer 0 for Transmission and assign priority */
    pEcan->pCanRegs->TR01CONbits.TXEN0 = 1;    // Configure Message Buffer xx for Transmission
    pEcan->pCanRegs->TR01CONbits.TX0PRI = 0x3; // Configure Message  priority 

    pEcan->pCanRegs->CTRL1bits.WIN = 0;
    Ecan_setMode( ( const void * )pEcan, pEcan->workMode );

    pEcan->pCanRegs->INTF = 0;

    return( const void * )pEcan;
}

//***************************************************************************** 
// ������ ������ �� ������������� ����������  
int Ecan_get( const void *self, uint8_t *array, int size )
{

    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSelf,
               ( ( self == &aEcan[eEcan1] ) || ( self == &aEcan[eEcan2] ) ) );
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSetSize,
               ( ( DRV_DATA( self )->lengthPacket == size ) && ( ( size % 2 ) == 0 ) ) );

    DRV_DATA( self )->pData = ( uint16_t * )array;

    if( self == &aEcan[eEcan1] )
    {
        for( DRV_DATA( self )->i = 0; DRV_DATA( self )->i < ( size / 2 ); DRV_DATA( self )->i++ )
        {
            DRV_DATA( self )->pData[DRV_DATA( self )->i] = aEcan1MsgBuf[NUM_BUFF_IN][3 + DRV_DATA( self )->i];
        }
    }
    else
    {
        for( DRV_DATA( self )->i = 0; DRV_DATA( self )->i < ( size / 2 ); DRV_DATA( self )->i++ )
        {
            DRV_DATA( self )->pData[DRV_DATA( self )->i] = aEcan2MsgBuf[NUM_BUFF_IN][3 + DRV_DATA( self )->i];
        }
    }

    DRV_DATA( self )->receiverIsReady = false;

    return DRV_DATA( self )->lengthPacket;
}

//*****************************************************************************
// ���������� ���������� � ������ �������� ������
bool Ecan_isInReady( const void *self )
{
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSelf,
               ( ( self == &aEcan[eEcan1] ) || ( self == &aEcan[eEcan2] ) ) );

    return DRV_DATA( self )->receiverIsReady;
}

//*****************************************************************************
// �������� ������ �������� ��� ������
void Ecan_set( const void *self, uint8_t *array, int size )
{
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSelf,
               ( ( self == &aEcan[eEcan1] ) || ( self == &aEcan[eEcan2] ) ) );
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSetSize,
               ( ( DRV_DATA( self )->lengthPacket == size ) && ( ( size % 2 ) == 0 ) ) );

    DRV_DATA( self )->pData = ( uint16_t * )array;

    if( self == &aEcan[eEcan1] )
    {
        for( DRV_DATA( self )->i = 0; DRV_DATA( self )->i < ( size / 2 ); DRV_DATA( self )->i++ )
        {
            aEcan1MsgBuf[NUM_BUFF_OUT][3 + DRV_DATA( self )->i] = DRV_DATA( self )->pData[DRV_DATA( self )->i];
        }
    }
    else
    {
        for( DRV_DATA( self )->i = 0; DRV_DATA( self )->i < ( size / 2 ); DRV_DATA( self )->i++ )
        {
            aEcan2MsgBuf[NUM_BUFF_OUT][3 + DRV_DATA( self )->i] = DRV_DATA( self )->pData[DRV_DATA( self )->i];
        }
    }
}

//*****************************************************************************
// ����� �������� �������� ������
void Ecan_AbortTransmite( const void *self )
{
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSelf,
               ( ( self == &aEcan[eEcan1] ) || ( self == &aEcan[eEcan2] ) ) );
    DRV_REGS( self )->CTRL1bits.ABAT = 1; // ����� ���� �������
    DRV_DATA( self )->transmissionState = eEcanTrStateIdle;
}

//*****************************************************************************
// ��������� �������� �������� ������
void Ecan_startTransmite( const void *self )
{
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSelf,
               ( ( self == &aEcan[eEcan1] ) || ( self == &aEcan[eEcan2] ) ) );
    DRV_REGS( self )->TR01CONbits.TXREQ0 = 1;
    DRV_DATA( self )->transmissionState = eEcanTrStateInProgress;
}

//*****************************************************************************
// ������� ����
void Ecan_run( const void *self )
{
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSelf,
               ( ( self == &aEcan[eEcan1] ) || ( self == &aEcan[eEcan2] ) ) );
    DRV_DATA( self )->tempINTF.data = DRV_REGS( self )->INTF;

    // ����� Bus-off state ������ CAN
    if( ( DRV_DATA( self )->tempINTF.bits.TXBP == 1 ) ||
        ( DRV_DATA( self )->tempINTF.bits.EWARN == 1 ) )
    {
        Ecan_reset( self );
    }

    // ����� ������
    if( DRV_DATA( self )->tempINTF.bits.IVRIF == 1 )
    {
        DRV_REGS( self )->INTFbits.RBIF = 0;
        DRV_REGS( self )->RXFUL1bits.NAME_BIT_RXFULL = 0;
        DRV_REGS( self )->INTFbits.IVRIF = 0;
        DRV_REGS( self )->INTFbits.ERRIF = 0;
    }
    if( DRV_DATA( self )->tempINTF.bits.RBIF == 1 )
    {
        DRV_DATA( self )->receiverIsReady = true;
        DRV_REGS( self )->INTFbits.RBIF = 0;
        DRV_REGS( self )->RXFUL1bits.NAME_BIT_RXFULL = 0;
    }

    // �������� ������
    switch( DRV_DATA( self )->transmissionState )
    {
        case eEcanTrStateIdle:
            break;
        case eEcanTrStateInProgress:
            if( DRV_REGS( self )->TR01CONbits.TXREQ0 == 0 )
            { // ���� ��������� ��������
                if( DRV_REGS( self )->INTFbits.ERRIF == 0 )
                {
                    DRV_DATA( self )->transmissionState = eEcanTrStateIdle;
                }
                else
                {
                    DRV_REGS( self )->CTRL1bits.ABAT = 1; // ����� ���� �������
                    DRV_DATA( self )->transmissionState = eEcanTrStateError;
                }
            }
            break;
        case eEcanTrStateError:
            break;
    }
}

//*****************************************************************************
// ����� �������� ������ � �������� ������
void Ecan_reset( const void *self )
{
    Ecan_setMode( self, eConfigMode );
    Ecan_setMode( self, DRV_DATA( self )->workMode );
}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
// ������� ������ #CAN � �������� �����
void Ecan_setMode( const void *self, uint16_t mode )
{
    DRV_REGS( self )->CTRL1bits.REQOP = mode;
    while( DRV_REGS( self )->CTRL1bits.OPMODE != mode )
    {
        ;
    }
}

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   06-10-2017
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
*/
