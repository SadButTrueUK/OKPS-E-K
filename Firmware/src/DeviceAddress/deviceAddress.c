/**
* \file    deviceAddress.c
* \brief   \copybrief deviceAddress.h
*
* \version 1.0.1
* \date    27-03-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "deviceAddress.h"
#include "BinIn.h"
#include "Rs422_crc8.h"
#include "ProtectionState_codes.h"
#include "asserts_ex.h"
#include "WorkWithBits.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define FAILURE_TRANSITION_T_O 20000        ///< ����� �������� � ��.
#define FIRST_CHECK_T_O        300          ///< ������� ���������� ��������� ��������.
#define PERIODIC_CHECK_T_O     5000         ///< ������� ������������� ��������.

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
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief �������������� ���������� ������� ������.
///
typedef enum
{
    eDeviceAddr_firstCheck  = 0,        ///< ��������� ��������
    eDeviceAddr_periodCheck             ///< ������������� ��������
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
// ����������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static          DeviceAddr_Jumpers    jumpers;         ///< ��������� ������������� ���������.
static          uDeviceAddr_Addr      address;         ///< ����� ����������.
static          uDeviceAddr_Config    config;          ///< ������������ ����������.
static uint16_t failureTimeoutCnt;                     ///< ������� �������� �������� � ��.
static uint16_t timeoutCnt;                            ///< ������� �������� ��������.
static          eDeviceAddr_Run       ctrlCnt;         ///< ������� ���������.
static          DeviceAddress_sysType sys_Type;        ///< ��� �������.

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
    failureTimeoutCnt = FAILURE_TRANSITION_T_O;    //���������� ������� �������� � ��
    timeoutCnt = FIRST_CHECK_T_O;                  //���������� ������� ��������� �������� 
    ctrlCnt = eDeviceAddr_firstCheck;              //������� � ���������� ��������� ��������
    flags.str.ctrl = 1;                            //�������� ������
}

//*****************************************************************************
// ���������� �������
void DeviceAddress_run( void )
{
    uint8_t crc8;
    if( flags.str.ctrl == 0 ) return; //������ �� �������
    //��������� ���������
    if( failureTimeoutCnt != 0 ) failureTimeoutCnt--;
    if( timeoutCnt != 0 ) timeoutCnt--;
    switch( ctrlCnt )
    {
        case eDeviceAddr_firstCheck: //��������� ��������
            //�������� �������� �������� � ��
            if( failureTimeoutCnt == 0 )
            { //������� � ��
                ERROR_ID( eGrPS_DeviceAddress, ePS_DeviceAddressFirstCheckError );
            }
            //�������� �������� ��� ���������� ��������
            if( timeoutCnt != 0 ) break;
            jumpers.addr.value = BinIn_getAddrJumpers( );
            jumpers.conf.value = BinIn_getConfigJumpers( );
            jumpers.crc = BinIn_getCrcJumpers( );
            //���������� CRC8
            crc8 = crc8_update( 0xFF, jumpers.addr.array, 2 );
            crc8 = crc8_update( crc8, &jumpers.conf.value, 1 );
            //�������� CRC8
            if( /*jumpers.crc == 0 || */ jumpers.crc != crc8 )
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
            flags.str.validity = 1; //��������� ������ ���������
            //��������� �������� ��������, ����� �������� ��� ������������� ��������
            timeoutCnt = PERIODIC_CHECK_T_O;
            failureTimeoutCnt = FAILURE_TRANSITION_T_O;
            ctrlCnt = eDeviceAddr_periodCheck;
            break;
        case eDeviceAddr_periodCheck: //������������� ��������
            //�������� �������� �������� � ��
            if( failureTimeoutCnt == 0 )
            { //������� � ��
                ERROR_ID( eGrPS_DeviceAddress, ePS_DeviceAddressCheckError );
            }
            //�������� �������� ��� ���������� ��������
            if( timeoutCnt != 0 ) break;
            if( jumpers.addr.value != BinIn_getAddrJumpers( ) ||
                jumpers.conf.value != BinIn_getConfigJumpers( ) ||
                jumpers.crc != BinIn_getCrcJumpers( ) )
            {
                //������������� �������� �� ��������
                timeoutCnt = PERIODIC_CHECK_T_O;
                break;
            }
            timeoutCnt = PERIODIC_CHECK_T_O;
            failureTimeoutCnt = FAILURE_TRANSITION_T_O;
            break;
        default:
            ERROR_ID( eGrPS_DeviceAddress, ePS_DeviceAddressStepCntError );
    }
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
