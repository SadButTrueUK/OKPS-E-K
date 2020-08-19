/**
* \file    I2Cdrv_PIC33.c
* \brief   \copybrief I2Cdrv_PIC33.h
*
* \version 1.0.1
* \date    30-01-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "typeMK.h"
#include "defCompil.h"
#include "I2Cdrv_PIC33.h"
#include "Tracing.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define N_ARRAY 16        ///< ������ �������� ��� ������ � �������� ������.
#define N_WAITE 2         ///< ����� �������� � ����������� ������ �������.

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief  ����������� ���������� �� �������� ������������ ���� I2C.
///
typedef struct
{
    uint16_t RCV;        ///< ������� ���������.
    uint16_t TRN;        ///< ������� �����������.
    uint16_t BRG;        ///< ������� �������� �������� �������.

    /// \brief CON: ������� ��������.
    ///
    union
    {
        uint16_t    CON;                    ///< ������� CON � ������� \a uint16_t.
        I2C1CONBITS CONbits;                ///< ������� CON � ������� ��� �������� �������.
    };

    /// \brief STAT: ������� �������.
    ///
    union
    {
        uint16_t     STAT;                    ///< ������� STAT � ������� \a uint16_t.
        I2C1STATBITS STATbits;                ///< ������� STAT � ������� ��� �������� �������.
    };

    uint16_t ADD;        ///< ������� ������, ����� Slave.
    uint16_t MSK;        ///< ������� ����� ������, ����� Slave.
} Ds33EP512GM710_I2C_Regs;

//*****************************************************************************
/// \brief �������������� ��������� ������� #I2Cdrv_run.
///
typedef enum
{
    eReady                = 0,         ///< �������� ������� �������� ������ ��� ������
    eOutStart             = 1,         ///< ������ �������� ������ ��� ������, ������ start-����
    eOutAddressDevice     = 2,         ///< ������ ������ ����������
    eOutAddressCell1      = 3,         ///< ������ ������ ������ (��������), ������� ����
    eOutAddressCell2      = 4,         ///< ������ ������ ������ (��������), ������� ����
    eOutData              = 5,         ///< ������ ������
    eOutStop              = 6,         ///< ����� �������� ������ ��� ������, ������ stop-����
    eWaiteOutBit          = 7,         ///< �������� ���������� ��������/������ ����
    eWaiteOutByte         = 8,         ///< �������� ���������� ��������/������ �����
    eOutReStart           = 9,         ///< ������ ��������
    eOutAddressDeviceRead = 10,        ///< ������ ������ ���������� c ����� ������
    eRCEN                 = 11,        ///< ���������� ������
    eInData               = 12         ///< ����� ������ 
} eI2C_run;

//*****************************************************************************
/// \brief ��������� ������ �������� I2C.
///
typedef struct
{
    uint8_t                          addressDevice;         ///< ����� ����������.
    uint16_t                         addressCell;           ///< ����� ������ ������ � ����������.
    uint8_t                          aData[N_ARRAY];        ///< ������ ������.
    uint8_t                          *pData;                ///< ��������� �� ������ ������ ��� �������� ������.
    uint8_t                          ndx;                   ///< ������ ������� ������.
    uint8_t                          length;                ///< ����� ����������� ��� ������������ ������.
    eI2C_modes                       mode;                  ///< �����: ����� ��� �������� ������.
    bool                             rezult;                ///< ��������� ������ ��� �������� ������: (true (�����)).
    uint16_t                         cWaite;                ///< ������� �������� �������� ��� ������ �����, ����.
    eI2C_run                         stateRun;              ///< ��������� ������� #I2Cdrv_run.
    volatile Ds33EP512GM710_I2C_Regs *pRegs;                ///< ��������� �� �������� ������ I2C.

    /// \brief ��������� ������� #I2Cdrv_run, � ������� ��� ������ ������� ����� ��������� \a wait.
    /// 
    eI2C_run statePost;

    /// \brief ��������� �� ������� ������ ����� \a I2C: MI2C1IF ��� \a MI2C2IF.
    /// 
    bool( *isMI2CxIF ) ( void ); ///< 
} I2Cdrv;

//*****************************************************************************
// ����������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static I2Cdrv aI2Cdrv[eI2C_moduleAmount];        ///< ������ ��������� I2C.

//*****************************************************************************
/// \brief ����������� ����� ������� ��������� I2C ������ 1.
/// 
extern volatile Ds33EP512GM710_I2C_Regs module1_I2C_Regs __attribute__( ( sfr( 0x0200 ) ) );

//*****************************************************************************
/// \brief ����������� ����� ������� ��������� I2C ������ 2.
/// 
extern volatile Ds33EP512GM710_I2C_Regs module2_I2C_Regs __attribute__( ( sfr( 0x0210 ) ) );

//*****************************************************************************
// ��������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ����� � ����� ����� \a MI2C1IF, ������ 1.
/// \retval true  - ���� ��� ����������;
/// \retval false - ���� �� ��� ����������. 
///
bool isMI2C1IF( void );

//*****************************************************************************
/// \brief ����� � ����� ����� \a MI2C2IF, ������ 2.
/// \retval true  - ���� ��� ����������;
/// \retval false - ���� �� ��� ����������.
///
bool isMI2C2IF( void );

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �����������
const void *I2Cdrv_ctor( eI2C_modules numberModule,
                         uint8_t addressDevice,
                         eIO_openDrain openDrain,
                         uint16_t brg )
{
    if( ( numberModule >= eI2C_moduleAmount ) || ( openDrain >= eIO_openDrainAmount ) ) return 0;

    I2Cdrv *me = &aI2Cdrv[numberModule];

    me->addressDevice = addressDevice;
    me->stateRun = eReady;
    me->ndx = 0;
    me->rezult = false;
    me->cWaite = 0;

    if( numberModule == eI2C_module1 )
    {
        me->pRegs = &module1_I2C_Regs;
        me->isMI2CxIF = isMI2C1IF;
        INIC_PORT_IN( C, 4, eIO_pullOff );
        INIC_PORT_IN( C, 5, eIO_pullOff );
        IFS1bits.MI2C1IF = 0;
    }
    else
    {
        me->pRegs = &module2_I2C_Regs;
        me->isMI2CxIF = isMI2C2IF;
        INIC_PORT_IN( B, 5, eIO_pullOff );
        INIC_PORT_IN( B, 6, eIO_pullOff );
        IFS3bits.MI2C2IF = 0;
    }
    me->pRegs->CONbits.A10M = 0; // 7-������ ����� Slave ����������
    me->pRegs->BRG = brg;
    me->pRegs->CONbits.I2CEN = 1; // ���������� I2C, ���������������� ������

    return me;
}

//*****************************************************************************
// ����� �� ���� I2C
void I2Cdrv_run( const void *self )
{
    I2Cdrv *me = ( I2Cdrv * )( self );

    switch( me->stateRun )
    {
        case eReady:
            break;
        case eOutStart:
            me->pRegs->CONbits.SEN = 1;
            me->statePost = eOutAddressDevice;
            me->stateRun = eWaiteOutBit;
            me->ndx = 0U;
            break;
        case eOutAddressDevice:
            me->pRegs->TRN = me->addressDevice;
            me->statePost = eOutAddressCell1;
            me->stateRun = eWaiteOutByte;
            break;
        case eOutAddressCell1:
            me->pRegs->TRN = ( uint8_t )( ( me->addressCell & 0xFF00 ) >> 8 );
            me->statePost = eOutAddressCell2;
            me->stateRun = eWaiteOutByte;
            break;
        case eOutAddressCell2:
            me->pRegs->TRN = ( uint8_t )( me->addressCell & 0x00FF );
            if( me->mode == eI2C_modeWrite )
            {
                me->statePost = eOutData;
                me->stateRun = eWaiteOutByte;
            }
            else
            {
                me->statePost = eOutReStart;
                me->stateRun = eWaiteOutBit;
            }
            break;
        case eOutData:
            if( me->ndx != me->length )
            {
                me->pRegs->TRN = me->aData[me->ndx++];
                me->stateRun = eWaiteOutByte;
            }
            else
            {
                me->rezult = true;
                me->stateRun = eOutStop;
            }
            break;
            // ������
        case eOutReStart:
            me->pRegs->CONbits.RSEN = 1;
            me->statePost = eOutAddressDeviceRead;
            me->stateRun = eWaiteOutBit;
            break;
        case eOutAddressDeviceRead:
            me->pRegs->TRN = me->addressDevice | 1;
            me->ndx = 0;
            me->statePost = eRCEN;
            me->stateRun = eWaiteOutByte;
            break;
        case eRCEN:
            me->pRegs->CONbits.RCEN = 1;
            me->statePost = eInData;
            me->stateRun = eWaiteOutBit;
            break;
        case eInData:
            if( me->pRegs->STATbits.RBF == 1 )
            {
                me->ndx++;
                *me->pData++ = me->pRegs->RCV;
                if( me->ndx == me->length )
                {
                    me->pRegs->CONbits.ACKDT = 1;
                    me->rezult = true;
                    me->statePost = eOutStop;
                }
                else
                {
                    me->pRegs->CONbits.ACKDT = 0;
                    me->statePost = eRCEN;
                }
                me->pRegs->CONbits.ACKEN = 1;
                me->stateRun = eWaiteOutBit;
            }
            else
            {

            }
            break;
        case eWaiteOutBit:
            if( ( *me->isMI2CxIF )( ) )
            {
                me->stateRun = me->statePost;
                me->cWaite = 0;
            }
            else
            {
                if( me->cWaite++ > N_WAITE )
                {
                    me->pRegs->CONbits.PEN = 1;
                    me->stateRun = eReady;
                    me->rezult = false;
                    me->cWaite = 0;
                }
            }
            break;

        case eWaiteOutByte:
            if( ( *me->isMI2CxIF )( ) )
            {
                if( me->pRegs->STATbits.ACKSTAT == 0 )
                {
                    me->stateRun = me->statePost;
                }
                else
                {
                    me->stateRun = eOutStop;
                    me->rezult = false;
                }
                me->cWaite = 0;
            }
            else
            {
                if( me->cWaite++ > N_WAITE )
                {
                    me->stateRun = eOutStop;
                    me->rezult = false;
                    me->cWaite = 0;
                }
            }
            break;
        case eOutStop:
            me->pRegs->CONbits.PEN = 1;
            me->statePost = eReady;
            me->stateRun = eWaiteOutBit;
            break;
    }
}

//*****************************************************************************
// ��������� ���������� �������� � ������ �������� �� ���� I2C
bool I2Cdrv_set( const void *self, uint8_t mode, uint16_t addressCell, uint8_t *aData, uint8_t length )
{
    bool ret;
    I2Cdrv *me = ( I2Cdrv * )( self );

    if( ( ( me == &aI2Cdrv[eI2C_module1] ) || ( me == &aI2Cdrv[eI2C_module2] ) ) &&
        ( mode < eI2C_modeAmount ) && ( length <= N_ARRAY ) && ( me->stateRun == eReady ) )
    {
        me->mode = mode;
        me->addressCell = addressCell;
        me->length = length;
        if( mode == eI2C_modeWrite )
        {
            uint8_t i;
            for( i = 0; i < length; i++ )
            {
                me->aData[i] = aData[i];
            }
        }
        else me->pData = aData;
        me->stateRun = eOutStart;
        ret = true;
    }
    else ret = false;

    return ret;
}

//*****************************************************************************
// ������ ���������� ��� ����� �������� �� ���� I2C
bool I2Cdrv_isReady( const void *self )
{
    I2Cdrv *me = ( I2Cdrv * )( self );
    return( me->stateRun == eReady ) ? true : false;
}

//*****************************************************************************
// ������ ���������� �������� �� ���� I2C
bool I2Cdrv_isOperationOk( const void *self )
{
    I2Cdrv *me = ( I2Cdrv * )( self );
    return me->rezult;
}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
// ����� � ����� ����� MI2C1IF, ������ 1
bool isMI2C1IF( void )
{
    if( IFS1bits.MI2C1IF != 0 )
    {
        IFS1bits.MI2C1IF = 0;
        return true;
    }
    else return false;
}

//*****************************************************************************
// ����� � ����� ����� MI2C2IF, ������ 2
bool isMI2C2IF( void )
{
    if( IFS3bits.MI2C2IF != 0 )
    {
        IFS3bits.MI2C2IF = 0;
        return true;
    }
    else return false;
}

//*****************************************************************************
/**
* ���������:
*    ����������� ������ ������� �������.  
*    ������� ��������� �������� ����� �� ������, ������ ���
*    ���� �� ������� ����� �� ����� Asc, �� ����� ������ ������
*    �� ���������� ������.
*    ��� ����� � ��, ������ ��� ��� ����� ����
*    ����� � EEPROM, ��� ����������� ��� ������.
*/

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   30-01-2017
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
*/
