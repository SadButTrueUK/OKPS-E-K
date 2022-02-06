/**
* \file    EepromDrv.c
* \brief   \copybrief EepromDrv.h
*
* \version 2.0.1
* \date    08-01-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "asserts.h"
#include "wait.h"
#include "ModeProtection.h"
#include "EepromDrv.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define ADDRESS_DEVICE      0xA0        ///< ����� ���������� �� ���� I2C.
#define N_ATTEMPT           3           ///< ���������� ������� ������ �� ���� I2C.
#define TIME_OPERATION_LINK 256         ///< ������������ ����� ������ �� ���� I2C, 16 ��.
#define TIME_WRITE_EEPROM   96          ///< ����� ������ � \a EEPROM, 6 ��.
#define SIZE_EEPROM_DATA         4096        ///< ������ ������������ ������� ������ \a EEPROM � ������.

//*****************************************************************************
// ������� ���� �������
//*****************************************************************************

//*****************************************************************************
/// \brief �������� ����������� � �������������� ���� � �������������� ����������.
/// \details ������ ���������, ��� ������� \a test_ �����������. ���� ��������
/// \a test_ ����� �������� false, ���������� ������� � ��
/// ��� �� (���������� \a EEPROM ��� �������� ���� ������).
/// \param group_ - ��� ������;
/// \param id_ - ������������� ������;
/// \param test_ - ����������� �������.
///
#define ASSERT_ID_EEPROM( group_, id_, test_ )                     \
    do                                                             \
    {                                                              \
        if( !( test_ ) )                                           \
        {                                                          \
            if( me->modeProtection == eEeProtectionState )         \
            {                                                      \
                ModeProtection_run( SET_CODE_ID( group_, id_ ) ); \
            }                                                      \
            else                                                   \
            {                                                      \
                 ModeProtectiont_transmitToWait( SET_CODE_ID( group_, id_ ) );  \
            }                                                      \
        }                                                          \
    } while( 0 )

//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ������� �������� ������������� ���������� ������ \a EEPROM, ��.
///
typedef enum
{
    eEE_T1 = 50,         ///< ������������ ����� ��������� 2-�� ���������
    eEE_T2 = 100,        ///< ������������ ����� ������������ ����������
    eEE_T3 = 0           ///< ������������ ����� ���������� ��������� (�� ������������)
} Eeprom_timeSynchr;

//*****************************************************************************
/// \brief �������������� ��������� ������ � ����������� \a EEPROM.
///
typedef enum
{
    eEESW_ready               = 0,        ///< ���������� ������ � ������
    eEESW_begin,                          ///< ������ ��������
    eEESW_waiteEndI2C,                    ///< �������� ���������� ������ �� ���� I2C 
    eEESW_startSynchroEndI2C,             ///< ������ ������������� ����������� ������ �� ���� I2C
    eEESW_waiteSynchroEndI2C,             ///< �������� ������������� ����������� ������ �� ���� I2C
    eEESW_EndI2C,                         ///< ����� ������ � ������� �� ���� I2C 
    eEESW_waiteWriteEeprom                ///< �������� �� ������ � \a EEPROM
} eEepromStateWork;

//*****************************************************************************
/// \brief ��������� ������� ������ � \a EEPROM.
///
typedef struct EepromDrvTag
{
    eEeExistence         exist;                 ///< ������� \a EEPROM, (true (������������)).
    InterChannelId       icId;                  ///< ������������� ����������������� ���������.
    InterChannelProcSync script;                ///< �������� ������ ������.
    bool                 synchro;               ///< ������� ������������� ������.
    eEepromStateWork     stateWork;             ///< ��������� ��������. 
    eEeProtectionModes   modeProtection;        ///< ��� ��������� ������, � ������� ������ ������.
    const void           *drvPtr;               ///< ��������� �� ������ �������� ������ �����.
    uint16_t             addressCell;           ///< ����� ������ � \a EEPROM.
    eI2C_modes           modeEeprom;            ///< ��� ��������: ������ ��� ������. 
    uint16_t             dataEeprom;            ///< ������ \a EEPROM.
    uint16_t             dataRezult;            ///< ������ \a EEPROM, ������������������.
    uint16_t             cWaite;                ///< ������� �������� �������� �� ���� I2C.
    uint16_t             cAttempt;              ///< ������� ������� �������� �� ���� I2C.
} EepromDrv;

//*****************************************************************************
/// \brief ������� ������ � \a EEPROM.
/// \note \a eeprom - ��� ���������� ������;
/// \note \a eepromCalibration - ��� ���������� ������������� �������������.
///
struct EepromDrvTag eepromDrv;

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ����������
void EepromDrv_ctor( const void *self,
                     eEeExistence exist,
                     eI2C_modules numberModule,
                     uint8_t modeProtection,
                     InterChannelProcSync script,
                     InterChannelId icId )
{
    EepromDrv *me = ( EepromDrv * )self;
    if( modeProtection >= eEeProtectionAmount )
    {
         ModeProtectiont_transmitToWait( SET_CODE_ID( eGrPS_Eeprom, ePS_EepromParameters ) );
    }
    me->modeProtection = modeProtection;

    me->exist = exist;
    if( me->exist )
    {
        ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromParameters, numberModule < eI2C_moduleAmount );
        me->drvPtr = I2Cdrv_ctor( numberModule, ADDRESS_DEVICE, eIO_openDrainOff, I2C_F_100 );
        ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromInicLink, me->drvPtr != 0 );
    }

    me->icId = icId;
    me->stateWork = eEESW_ready;

    me->script = script;
    if( script == eProcSyncEqual )
    {
        InterChannel_setParamSettings( icId, eScriptSync, script, eProcCheckEqual,
                                       0, 0, eEE_T1, eEE_T2, eEE_T3 );
    }
    else
    {
        if( script == eProcSyncHi )
        {
            InterChannel_setParamSettings( icId, eScriptSync, script, eProcCheckOff,
                                           0, 0, eEE_T1, eEE_T2, eEE_T3 );
        }
        else
        {
            ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromParameters, false ); // ������������ �������� �������������
        }
    }
}

//*****************************************************************************
// ������� ���� ������ � EEPROM
void EepromDrv_run( const void *self )
{
    EepromDrv *me = ( EepromDrv * )self;

    switch( me->stateWork )
    {
        case eEESW_ready:
            break;
        case eEESW_begin:
            if( me->exist )
            { // ���� EEPROM
                bool rezult = I2Cdrv_set( me->drvPtr, me->modeEeprom, me->addressCell,
                                          ( uint8_t * ) & me->dataEeprom, 2 );
                ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromAccess, rezult );
                me->cWaite = 0;
                me->stateWork = eEESW_waiteEndI2C;
            }
            else
            {
                me->stateWork = eEESW_startSynchroEndI2C;
            }
            break;
        case eEESW_waiteEndI2C:
            if( I2Cdrv_isReady( me->drvPtr ) )
            {
                if( I2Cdrv_isOperationOk( me->drvPtr ) )
                {
                    me->dataRezult = me->dataEeprom;
                    me->stateWork = eEESW_startSynchroEndI2C;
                }
                else
                {
                    ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromAccess, ++me->cAttempt < N_ATTEMPT );
                    me->stateWork = eEESW_begin;
                }
            }
            else
            {
                if( ++me->cWaite > TIME_OPERATION_LINK )
                {
                    ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromAccess, 0 ); //���������� �������� I2C
                    me->stateWork = eEESW_begin;
                }
            }
            break;
        case eEESW_startSynchroEndI2C:
            if( me->synchro )
            {
                if( ( me->script == eProcSyncEqual ) || ( me->exist ) )
                {
                    InterChannel_synchronize( me->icId, me->dataEeprom );
                }
                else
                {
                    InterChannel_synchronize( me->icId, 0 );
                }
                me->stateWork = eEESW_waiteSynchroEndI2C;
            }
            else
            {
                me->dataRezult = me->dataEeprom;
                me->stateWork = eEESW_EndI2C;
            }
            break;
        case eEESW_waiteSynchroEndI2C:
            if( InterChannel_isSynchronized( me->icId ) )
            {
                me->dataRezult = ( uint16_t )( InterChannel_getData( me->icId ) );
                me->stateWork = eEESW_EndI2C;
            }
            break;
        case eEESW_EndI2C:
            if( me->modeEeprom == eI2C_modeWrite )
            {
                me->cWaite = 0;
                me->stateWork = eEESW_waiteWriteEeprom;
            }
            else
            {
                me->stateWork = eEESW_ready;
            }
            break;
        case eEESW_waiteWriteEeprom:
            if( ++me->cWaite > TIME_WRITE_EEPROM )
            {
                me->stateWork = eEESW_ready;
            }
            break;
        default:
            me->stateWork = eEESW_ready;
            break;
    }
    I2Cdrv_run( me->drvPtr );
}

//*****************************************************************************
// ������ ��������� ������ � EEPROM 16-������� ��������
void EepromDrv_setWrite( const void *self, uint16_t addressCell, uint16_t data )
{
    EepromDrv *me = ( EepromDrv * )self;

    ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromParameters,
                      ( me->stateWork == eEESW_ready ) &&
                      ( addressCell < SIZE_EEPROM_DATA ) &&
                      ( ( addressCell % 2 ) == 0 ) );
    me->modeEeprom = eI2C_modeWrite;
    me->synchro = true;
    me->addressCell = addressCell;
    me->dataEeprom = data;
    me->cAttempt = 0;
    me->stateWork = eEESW_begin;
}

//*****************************************************************************
// ������ ��������� ������ � EEPROM 16-������� ��������
void EepromDrv_setRead( const void *self, uint16_t addressCell )
{
    EepromDrv *me = ( EepromDrv * )self;

    ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromParameters,
                      ( me->stateWork == eEESW_ready ) &&
                      ( addressCell < SIZE_EEPROM_DATA ) &&
                      ( ( addressCell % 2 ) == 0 ) );
    me->modeEeprom = eI2C_modeRead;
    me->synchro = true;
    me->addressCell = addressCell;
    me->cAttempt = 0;
    me->stateWork = eEESW_begin;
}

//*****************************************************************************
// ������ � EEPROM 16-������� ��������, ������������� ����� ������
void EepromDrv_write( const void *self, uint16_t addressCell, uint16_t data )
{
    EepromDrv *me = ( EepromDrv * )self;

    ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromAccess, me->exist );
    EepromDrv_setWrite( me, addressCell, data );
    me->synchro = false;

    while( me->stateWork != eEESW_ready )
    {
        EepromDrv_run( me );
        wait62mks( );
    }
}


//*****************************************************************************
// ����� ������ � EEPROM 16-������� ��������, ������������� ����� ������
void EepromDrv_write_start( const void *self, uint16_t addressCell, uint16_t data )
{
    EepromDrv *me = ( EepromDrv * )self;

    ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromAccess, me->exist );
    EepromDrv_setWrite( me, addressCell, data );
    me->synchro = false;
}

//*****************************************************************************
// ������ �� EEPROM 16-������� ��������, ������������� ����� ������
uint16_t EepromDrv_read( const void *self, uint16_t addressCell )
{
    EepromDrv *me = ( EepromDrv * )self;

    ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromAccess, me->exist );
    EepromDrv_setRead( me, addressCell );
    me->synchro = false;

    while( me->stateWork != eEESW_ready )
    {
        EepromDrv_run( me );
        wait62mks( );
    }
    return me->dataRezult;
}

//*****************************************************************************
// ���������� EEPROM � ������ ��� ������ ������, ���������� �������� ���������
bool EepromDrv_isReady( const void *self )
{
    EepromDrv *me = ( EepromDrv * )self;
    return( me->stateWork == eEESW_ready ) ? true : false;
}

//*****************************************************************************
// �������� ������, ��������� �� EEPROM
uint16_t EepromDrv_getData( const void *self )
{
    EepromDrv *me = ( EepromDrv * )self;
    return me->dataRezult;
}

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   19-12-2016
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
* 
* ������ 2.0.1
* ����   08-01-2017
* �����  ��������� �.�.
* 
* ���������:
*    ��������� ��� ����� ����� �� dsPIC33.
*/
