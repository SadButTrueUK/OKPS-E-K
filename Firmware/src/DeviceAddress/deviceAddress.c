/**
* \file    DeviceAddress.c
* \brief   \copybrief DeviceAddress.h
*
* \version 1.0.1
* \date    27-03-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "DeviceAddress.h"
#include "BinIn.h"
#include "Rs422_crc8.h"
#include "ProtectionState_codes.h"
#include "asserts_ex.h"
#include "WorkWithBits.h"
#include "CheckCallFunctions.h"
#include "Eeprom.h"
#include "addressVarEeprom.h"
#include "DebugTools.h"
#include "MessageVersion.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define FAILURE_TRANSITION_T_O 20000        ///< ����� �������� � ��.
#define FIRST_CHECK_T_O        300          ///< ����-��� ���������� ��������� ��������.
#define PERIODIC_CHECK_T_O     5000         ///< ����-��� ������������� ��������.
#define NUM_CALIBRATIONS       2            ///< ���������� ������������� ������ (���������� � "+" � � "-")

//*****************************************************************************
// ����������� ���� ������ ����
#define SEVEN_WIRE   0b100                  ///< ������������� �����.
#define FIVE_WIRE_EC 0b111                  ///< ����� "5-��".
#define RESERVE      0b101                  ///< ���������������.
#define NINE_WIRE    0b110                  ///< ��������������� �����.
#define FIVE_WIRE    0b011                  ///< ������������� �����.

//*****************************************************************************
// ����������� ���� ��������� � ���������� ������� ���������
#define MCT025_127V 0b00110                 ///< ��������� MCT025, 127 �.
#define MCT025_220V 0b00111                 ///< ��������� MCT025, 220 �.
#define MCT03_110V  0b01000                 ///< ��������� MCT03, 110 �.
#define MCT03_190V  0b01001                 ///< ��������� MCT03, 190 �.
#define MCT06_110V  0b01010                 ///< ��������� MCT06, 110 �.
#define MCT06_190V  0b01011                 ///< ��������� MCT06, 190 �.

//*****************************************************************************
// ����������� ���� ��������� � ���������� ������� ���������
#define MAGIC_WORD  0x1059
//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief �������������� ���������� ������� ������.
///
typedef enum
{
    eDeviceAddr_firstCheck  = 0,                ///< ��������� ��������
    eDeviceAddr_periodCheck,                    ///< ������������� ��������
    eDeviceAddr_WriteEEPROM_addr,               ///< ������ ������ � EEPROM
    eDeviceAddr_WriteEEPROM_config,             ///< ������ ������������ � EEPROM
    eDeviceAddr_WriteEEPROM_crc,                ///< ������ CRC � EEPROM
    eDeviceAddr_WriteEEPROM_status,                ///< ������ CRC � EEPROM   
    eDeviceAddr_WaitingForWriteCompleteEEPROM,   ///< �������� ���������� ������ � EEPROM
    eDeviceAddr_WaitingForWriteCompleteEEPROM1        
} eDeviceAddr_Run;

//*****************************************************************************
/// \brief ����� ����������.
///
typedef union
{
    uint8_t  array[2];        ///< ����� ���������� ��� 8-������� �������.
    uint16_t value;           ///< ����� ���������� ��� 16-������� �������.
} uDeviceAddr_Addr;

//*****************************************************************************
/// \brief ������������ ����������.
///
typedef struct
{
    uint8_t motorType  :5;         ///< ��� ������.
    uint8_t modRelType :3;         ///< ��� ������ ����.
} DeviceAddr_Config;

//*****************************************************************************
/// \brief ����������� ������������ ���������� � \a uint8_t.
///
typedef union
{
    uint8_t           value;         ///< ����� ���������� ��� 8-������� �������.
    DeviceAddr_Config str;           ///< ����� ���������� ��� 16-������� �������.
} uDeviceAddr_Config;

//*****************************************************************************
/// \brief ��������� ���������.
///
typedef struct
{
    uDeviceAddr_Addr   addr;        ///< ����� ����������.
    uDeviceAddr_Config conf;        ///< ������������ ����������.
    uint8_t            crc;         ///< ����������� �����.
} DeviceAddr_Jumpers;

//*****************************************************************************
/// \brief ����� ������.
///
typedef struct
{
    uint8_t status   :1;        ///< ������ ������� (�������� / ���������).
    uint8_t validity :1;        ///< ������������ ��������� ������ (1 / 0).
    uint8_t ctrl     :1;        ///< ��������� ������.
    uint8_t unused   :5;        ///< ��������������.
} DeviceAddr_flags;

//*****************************************************************************
/// \brief ����������� ������ ������ � \a uint8_t.
///
typedef union
{
    uint8_t          data;        ///< ����� ������.
    DeviceAddr_flags str;         ///< ������ �����.
} uDeviceAddr_flags;

//*****************************************************************************
/// \brief ������ ������ ����������.
///
typedef struct
{
    uint16_t address;               ///< �����, ����������� �� EEPROM ��� ��������� �������.
    uint16_t config;                ///< ������������, ����������� �� EEPROM ��� ��������� �������.
    uint16_t crc;                   ///< CRC, ����������� �� EEPROM ��� ��������� �������.
    uint16_t threshold_p;          ///< ������������� �������� ���� � ������ �������� � ����.    
    uint16_t threshold_m;          ///< ������������� �������� ���� � ������ �������� � �����.        
    uint16_t increasedTime_p;      ///< �������� ������������ ���� �������� ��� ������������� �������� � ����
    uint16_t increasedTime_m;      ///< �������� ������������ ���� �������� ��� ������������� �������� � �����
} DeviceAddr_calibr;


//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static          DeviceAddr_Jumpers    jumpers;         ///< ��������� ������������� ���������.
static          uDeviceAddr_Addr      address;         ///< ����� ����������.
static          uDeviceAddr_Config    config;          ///< ������������ ����������.
static uint16_t failureTimeoutCnt;                     ///< ������� ����-���� �������� � ��.
static uint16_t timeoutCnt;                            ///< ������� ����-���� ��������.
static          eDeviceAddr_Run       ctrlCnt;         ///< ������� ���������.
static          DeviceAddress_sysType sys_Type;        ///< ��� �������.
static          DeviceAddr_calibr     calibr;          ///< ������ � ���������� �������.

static const char SerialNumber[16] = "1234567";
//*****************************************************************************
/// \brief ����� ������.
///
static uDeviceAddr_flags flags =
{
    .data = 0
};

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� � ������ ������
void DeviceAddress_ctor( void )
{
    address.value = 0;
    config.value = 0;
    failureTimeoutCnt = FAILURE_TRANSITION_T_O;                 //���������� ����-��� �������� � ��
    timeoutCnt = FIRST_CHECK_T_O;                               //���������� ����-��� ��������� �������� 
    ctrlCnt = eDeviceAddr_firstCheck;                           //������� � ���������� ��������� ��������


    // ������ � EEPROM ��������� ������ �������
    if ( Eeprom_read(ADDRESS_EEPROM_MAGIC_WORD) != MAGIC_WORD)
    {
        for (uint8_t i=0; ; i += 2 )
        {
            Eeprom_write( ADDRESS_EEPROM_MANUFACTURERS_NUMBER + i, SerialNumber[i]<<8 | SerialNumber[i+1]);
            
            if (SerialNumber[i] == 0 || SerialNumber[i+1] == 0 )
                break;
        }
        
        Eeprom_write(ADDRESS_EEPROM_MAGIC_WORD, MAGIC_WORD);
    }
    
    
    
    // ������ �� EEPROM ��������� �������� ���� ����������        
    calibr.crc     = Eeprom_read( ADDRESS_EEPROM_CALIBR_CRC );
    calibr.config  = Eeprom_read( ADDRESS_EEPROM_CALIBR_CONF );
    calibr.address = Eeprom_read( ADDRESS_EEPROM_CALIBR_ADDR );
    calibr.threshold_p  = Eeprom_read( ADDRESS_EEPROM_CALIBR_THRESHOLD_P );
    calibr.threshold_m  = Eeprom_read( ADDRESS_EEPROM_CALIBR_THRESHOLD_M );    
    calibr.increasedTime_m = Eeprom_read( ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_M );
    calibr.increasedTime_p = Eeprom_read( ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_P );

    flags.str.ctrl = 1;                                         //�������� = Eeprom_read( ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_M );
}

//*****************************************************************************
// ���������� �������
void DeviceAddress_run( void )
{
    if( flags.str.ctrl == 0 )
        return;                               //������ �� �������

    failureTimeoutCnt == 0 ? : failureTimeoutCnt--;
    timeoutCnt        == 0 ? : timeoutCnt--;

    switch( ctrlCnt )
    {
        case eDeviceAddr_firstCheck:                    //�������� ��� ������ ���������
            ASSERT_ID( eGrPS_DeviceAddress, ePS_DeviceAddressFirstCheckError, failureTimeoutCnt != 0 );                       //�������� ����-���� �������� � ��

            //�������� ����-���� ��� ���������� ��������
            if( timeoutCnt != 0 ) 
                break;

            jumpers.addr.value = BinIn_getAddrJumpers( );
            jumpers.conf.value = BinIn_getConfigJumpers( );
            jumpers.crc        = BinIn_getCrcJumpers( );

            if( jumpers.crc != crc8_update( crc8_update( 0xFF, jumpers.addr.array, 2 ), &jumpers.conf.value, 1 ) )
            {
                //�������� �� ��������
                timeoutCnt = FIRST_CHECK_T_O;
                break;
            }
            //������������ �������
            flags.str.status = CHECKBIT( jumpers.addr.array[0], 0 ) ? 0 : 1;
            //������������ ���� ��
            sys_Type = CHECKBIT( jumpers.addr.array[1], 0 ) ? eEbilock : eStrela;
            //������������ ������������
            //��� ���������
            switch( jumpers.conf.str.motorType )
            {
                case MCT025_127V:
                    config.str.motorType = eMct025_127v;
                    break;
                case MCT025_220V:
                    config.str.motorType = eMct025_220v;
                    break;
                case MCT03_110V:
                    config.str.motorType = eMct03_110v;
                    break;
                case MCT03_190V:
                    config.str.motorType = eMct03_190v;
                    break;
                case MCT06_110V:
                    config.str.motorType = eMct06_110v;
                    break;
                case MCT06_190V:
                    config.str.motorType = eMct06_190v;
                    break;
                default:
                    config.str.motorType = eMct025_220v;
            }
            // ��� ������ ����
            switch( jumpers.conf.str.modRelType )
            {
                case SEVEN_WIRE:
                    config.str.modRelType = eSevenWire;
                    break;
                case FIVE_WIRE_EC:
                    config.str.modRelType = eFiveWireEc;
                    break;
                case RESERVE:
                    config.str.modRelType = eReserve;
                    break;
                case NINE_WIRE:
                    config.str.modRelType = eNineWire;
                    break;
                case FIVE_WIRE:
                    config.str.modRelType = eFiveWire;
                    break;
                default:
                    config.str.modRelType = eFiveWireEc;
            }
            //������������ ������
            address.value = jumpers.addr.value;
            SETBIT( address.array[0], 0 );
            CLEARBIT( address.array[1], 0 );
            flags.str.validity = 1;                             //��������� ������ ���������
            
            //��������� �������� ��������, ����� ����-���� ��� ������������� ��������
            timeoutCnt        = PERIODIC_CHECK_T_O;
            failureTimeoutCnt = FAILURE_TRANSITION_T_O;

            // �������� �������� ������ ���������� �� EEPROM ���������� �������������� ������
            if (calibr.address != jumpers.addr.value || calibr.config != jumpers.conf.value || calibr.crc != jumpers.crc)
                ctrlCnt = eDeviceAddr_WriteEEPROM_addr;         // ������ ������ ������ ������� � EEPROM
            else
                ctrlCnt = eDeviceAddr_periodCheck;              // ����� �� ���������, ����� ���������� ������
            break;

        case eDeviceAddr_WriteEEPROM_addr:
            Eeprom_write_start(ADDRESS_EEPROM_CALIBR_ADDR, calibr.address = jumpers.addr.value);                        // �������� ������ � EEPROM ������ �������
            ctrlCnt = eDeviceAddr_WriteEEPROM_config;
            break;

        case eDeviceAddr_WriteEEPROM_config:
            if (Eeprom_isReady() == false)
                break;
            Eeprom_write_start(ADDRESS_EEPROM_CALIBR_CONF, calibr.config = jumpers.conf.value);  // �������� ������ � EEPROM ������������ �������
            ctrlCnt = eDeviceAddr_WriteEEPROM_crc;
            break;


        case eDeviceAddr_WriteEEPROM_crc:
            if (Eeprom_isReady() == false)
                break;
            Eeprom_write_start(ADDRESS_EEPROM_CALIBR_CRC, calibr.crc = jumpers.crc);           // �������� ������ � EEPROM CRC
            ctrlCnt =  eDeviceAddr_WriteEEPROM_status;
            break;

        case eDeviceAddr_WriteEEPROM_status:
            if (Eeprom_isReady() == false)
                break;
            Eeprom_write_start(ADDRESS_EEPROM_CALIBR_THRESHOLD_P, 0x0000);                     // �������� ������ � EEPROM Status (���������� �������� ����������))
            ctrlCnt = eDeviceAddr_WaitingForWriteCompleteEEPROM;
            break;

            
        case eDeviceAddr_WaitingForWriteCompleteEEPROM:
            if (Eeprom_isReady() == false)
                break;
            Eeprom_write_start(ADDRESS_EEPROM_CALIBR_THRESHOLD_M, 0x0000);                                   // �������� ������ � EEPROM Status (���������� �������� ����������))          
            ctrlCnt = eDeviceAddr_WaitingForWriteCompleteEEPROM1;
            break;


        case eDeviceAddr_WaitingForWriteCompleteEEPROM1:
            if (Eeprom_isReady() )
            {
                ctrlCnt = eDeviceAddr_periodCheck;
            }
            break;
            
        case eDeviceAddr_periodCheck: //������������� ��������
            ASSERT_ID( eGrPS_DeviceAddress, ePS_DeviceAddressCheckError, failureTimeoutCnt != 0 );                       //�������� ����-���� �������� � ��

            if( timeoutCnt != 0 )                                                                                       //�������� ����-���� ��� ���������� ��������
                break;
            if( jumpers.addr.value == BinIn_getAddrJumpers( ) && jumpers.conf.value == BinIn_getConfigJumpers( ) &&
                jumpers.crc == BinIn_getCrcJumpers( ) )
            {
                failureTimeoutCnt = FAILURE_TRANSITION_T_O;                                                             // �������� ������� - ���������� ������
            }    
            timeoutCnt = PERIODIC_CHECK_T_O;
            break;

        default:
            ERROR_ID( eGrPS_DeviceAddress, ePS_DeviceAddressStepCntError );
    }
    MARKED_CALL_FUNCTION;
}

//*****************************************************************************
// �������� ����� �������
uint16_t DeviceAddress_getAddress( void )
{
    return address.value;
}

//*****************************************************************************
// �������� ������ �������
bool DeviceAddress_isPrimary( void )
{
    return flags.str.status != 0;
}

//*****************************************************************************
// �������� ��� ��
DeviceAddress_sysType DeviceAddress_getSysType( void )
{
    return sys_Type;
}

//*****************************************************************************
// �������� ���� ������������ ������
bool DeviceAddress_isValid( void )
{
    return flags.str.validity != 0;
}

//*****************************************************************************
// �������� ��� ������ ����
RelayModuleType DeviceAddress_getRelayModuleType( void )
{
    return config.str.modRelType;
}

//*****************************************************************************
// �������� ��� ���������
MotorType DeviceAddress_getMotorType( void )
{
    return config.str.motorType;
}

//*****************************************************************************
// �������� ��������� ���������� (������� ����������) 
bool DeviceAddress_getIsCalibrPresents( CalibrDir dir )
{
    if (dir == Plus)
        return calibr.threshold_p == 0 ? false : true;
    else
        return calibr.threshold_m == 0 ? false : true;
}

//*****************************************************************************
// �������� �������� ���� ���������� 
uint16_t DeviceAddress_getCalibrCurrent( CalibrDir dir )
{
        return dir == Plus ? calibr.threshold_p : calibr.threshold_m;
}

uint16_t DeviceAddress_getIncreasedTime( CalibrDir dir )
{
        return dir == Plus ? calibr.increasedTime_p : calibr.increasedTime_m;
}

//*****************************************************************************
// ���������� �������� ���� ���������� � ������ ��� ������ � EEPROM
void DeviceAddress_setCalibrCurrentAndTime( CalibrDir dir, uint16_t current, uint16_t timeValue )
{
    if (dir == Plus)
    {
//        Eeprom_write_start(ADDRESS_EEPROM_CALIBR_THRESHOLD_P, current ); 
        calibr.threshold_p = current;
//        Eeprom_write_start(ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_P, timeValue );
        calibr.increasedTime_p = timeValue;
    }
    else
    {
//        Eeprom_write_start(ADDRESS_EEPROM_CALIBR_THRESHOLD_M, current ); 
        calibr.threshold_m = current;
//        Eeprom_write_start(ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_M, timeValue );
        calibr.increasedTime_m = timeValue;
    }
}



//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   27-03-2017
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
*/
