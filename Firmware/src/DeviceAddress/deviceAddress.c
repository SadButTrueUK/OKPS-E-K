/**
* \file    deviceAddress.c
* \brief   \copybrief deviceAddress.h
*
* \version 1.0.1
* \date    27-03-2017
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "deviceAddress.h"
#include "BinIn.h"
#include "Rs422_crc8.h"
#include "ProtectionState_codes.h"
#include "asserts_ex.h"
#include "WorkWithBits.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
#define FAILURE_TRANSITION_T_O 20000        ///< Время перехода в ЗС.
#define FIRST_CHECK_T_O        300          ///< Таймаут повторения первичной проверки.
#define PERIODIC_CHECK_T_O     5000         ///< Таймаут периодической проверки.

//*****************************************************************************
// Кодирование типа модуля реле
#define SEVEN_WIRE   0b100                  ///< Семипроводный режим.
#define FIVE_WIRE_EC 0b111                  ///< Режим "5-ЭЦ".
#define RESERVE      0b101                  ///< Зарезервировано.
#define NINE_WIRE    0b110                  ///< Девятипроводный режим.
#define FIVE_WIRE    0b011                  ///< Пятипроводный режим.

//*****************************************************************************
// Кодирование типа двигателя и напряжение питания двигателя
#define MCT025_127V 0b00110                 ///< Двигатель MCT025, 127 В.
#define MCT025_220V 0b00111                 ///< Двигатель MCT025, 220 В.
#define MCT03_110V  0b01000                 ///< Двигатель MCT03, 110 В.
#define MCT03_190V  0b01001                 ///< Двигатель MCT03, 190 В.
#define MCT06_110V  0b01010                 ///< Двигатель MCT06, 110 В.
#define MCT06_190V  0b01011                 ///< Двигатель MCT06, 190 В.

//*****************************************************************************
// Определение типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Идентификаторы управления работой модуля.
///
typedef enum
{
    eDeviceAddr_firstCheck  = 0,        ///< первичная проверка
    eDeviceAddr_periodCheck             ///< периодическая проверка
} eDeviceAddr_Run;

//*****************************************************************************
/// \brief Адрес устройства.
///
typedef union
{
    uint8_t  array[2];        ///< Адрес устройства для 8-битного доступа.
    uint16_t value;           ///< Адрес устройства для 16-битного доступа.
} uDeviceAddr_Addr;

//*****************************************************************************
/// \brief Конфигурация устройства.
///
typedef struct
{
    uint8_t motorType  :5;         ///< Тип мотора.
    uint8_t modRelType :3;         ///< Тип модуля реле.
} DeviceAddr_Config;

//*****************************************************************************
/// \brief Объединение конфигурации устройства с \a uint8_t.
///
typedef union
{
    uint8_t           value;         ///< Адрес устройства для 8-битного доступа.
    DeviceAddr_Config str;           ///< Адрес устройства для 16-битного доступа.
} uDeviceAddr_Config;

//*****************************************************************************
/// \brief Состояние перемычек.
///
typedef struct
{
    uDeviceAddr_Addr   addr;        ///< Адрес устройства.
    uDeviceAddr_Config conf;        ///< Конфигурация устройства.
    uint8_t            crc;         ///< Контрольная сумма.
} DeviceAddr_Jumpers;

//*****************************************************************************
/// \brief Флаги модуля.
///
typedef struct
{
    uint8_t status   :1;        ///< Статус прибора (основной / резервный).
    uint8_t validity :1;        ///< Корректность считанных данных (1 / 0).
    uint8_t ctrl     :1;        ///< Включение модуля.
    uint8_t unused   :5;        ///< Неиспользуемые.
} DeviceAddr_flags;

//*****************************************************************************
/// \brief Обьединение флагов модуля с \a uint8_t.
///
typedef union
{
    uint8_t          data;        ///< Флаги модуля.
    DeviceAddr_flags str;         ///< Данные флага.
} uDeviceAddr_flags;

//*****************************************************************************
// Определение локальных переменных
//*****************************************************************************

//*****************************************************************************
static          DeviceAddr_Jumpers    jumpers;         ///< Состояние установленных перемычек.
static          uDeviceAddr_Addr      address;         ///< Адрес устройства.
static          uDeviceAddr_Config    config;          ///< Конфигурация устройства.
static uint16_t failureTimeoutCnt;                     ///< Счетчик таймаута перехода в ЗС.
static uint16_t timeoutCnt;                            ///< Счетчик таймаута проверки.
static          eDeviceAddr_Run       ctrlCnt;         ///< Счетчик состояния.
static          DeviceAddress_sysType sys_Type;        ///< Тип системы.

//*****************************************************************************
/// \brief Флаги модуля.
///
static uDeviceAddr_flags flags =
{
    .data = 0
};

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация и запуск модуля
void DeviceAddress_ctor( void )
{
    address.value = 0;
    config.value = 0;
    failureTimeoutCnt = FAILURE_TRANSITION_T_O;    //Установить таймаут перехода в ЗС
    timeoutCnt = FIRST_CHECK_T_O;                  //Установить таймаут первичной проверки 
    ctrlCnt = eDeviceAddr_firstCheck;              //Переход к выполнению первичной проверки
    flags.str.ctrl = 1;                            //Включить модуль
}

//*****************************************************************************
// Управление работой
void DeviceAddress_run( void )
{
    uint8_t crc8;
    if( flags.str.ctrl == 0 ) return; //Модуль не включен
    //Декремент таймаутов
    if( failureTimeoutCnt != 0 ) failureTimeoutCnt--;
    if( timeoutCnt != 0 ) timeoutCnt--;
    switch( ctrlCnt )
    {
        case eDeviceAddr_firstCheck: //Первичная проверка
            //Проверка таймаута перехода в ЗС
            if( failureTimeoutCnt == 0 )
            { //Переход в ЗС
                ERROR_ID( eGrPS_DeviceAddress, ePS_DeviceAddressFirstCheckError );
            }
            //Ожидание таймаута для выполнения проверки
            if( timeoutCnt != 0 ) break;
            jumpers.addr.value = BinIn_getAddrJumpers( );
            jumpers.conf.value = BinIn_getConfigJumpers( );
            jumpers.crc = BinIn_getCrcJumpers( );
            //Вычисление CRC8
            crc8 = crc8_update( 0xFF, jumpers.addr.array, 2 );
            crc8 = crc8_update( crc8, &jumpers.conf.value, 1 );
            //Проверка CRC8
            if( /*jumpers.crc == 0 || */ jumpers.crc != crc8 )
            {
                //Проверка не пройдена
                timeoutCnt = FIRST_CHECK_T_O;
                break;
            }
            //Формирование статуса
            flags.str.status = CHECKBIT( jumpers.addr.array[0], 0 ) ? 0 : 1;
            //Формирование типа УС
            sys_Type = CHECKBIT( jumpers.addr.array[1], 0 ) ? eEbilock : eStrela;
            //Формирование конфигурации
            //Тип двигателя
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
            // Тип модуля реле
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
            //Формирование адреса
            address.value = jumpers.addr.value;
            SETBIT( address.array[0], 0 );
            CLEARBIT( address.array[1], 0 );
            flags.str.validity = 1; //Считанные данные корректны
            //Первичная проверка окончена, задаю таймауты для периодической проверки
            timeoutCnt = PERIODIC_CHECK_T_O;
            failureTimeoutCnt = FAILURE_TRANSITION_T_O;
            ctrlCnt = eDeviceAddr_periodCheck;
            break;
        case eDeviceAddr_periodCheck: //Периодическая проверка
            //Проверка таймаута перехода в ЗС
            if( failureTimeoutCnt == 0 )
            { //Переход в ЗС
                ERROR_ID( eGrPS_DeviceAddress, ePS_DeviceAddressCheckError );
            }
            //Ожидание таймаута для выполнения проверки
            if( timeoutCnt != 0 ) break;
            if( jumpers.addr.value != BinIn_getAddrJumpers( ) ||
                jumpers.conf.value != BinIn_getConfigJumpers( ) ||
                jumpers.crc != BinIn_getCrcJumpers( ) )
            {
                //Периодическая проверка не пройдена
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
// Получает адрес прибора
uint16_t DeviceAddress_getAddress( void )
{
    return address.value;
}

//*****************************************************************************
// Получает статус прибора
bool DeviceAddress_isPrimary( void )
{
    return flags.str.status != 0;
}

//*****************************************************************************
// Получает тип УС
DeviceAddress_sysType DeviceAddress_getSysType( void )
{
    return sys_Type;
}

//*****************************************************************************
// Получает флаг корректности адреса
bool DeviceAddress_isValid( void )
{
    return flags.str.validity != 0;
}

//*****************************************************************************
// Получает тип модуля реле
RelayModuleType DeviceAddress_getRelayModuleType( void )
{
    return config.str.modRelType;
}

//*****************************************************************************
// Получить тип двигателя
MotorType DeviceAddress_getMotorType( void )
{
    return config.str.motorType;
}

//*****************************************************************************
/**
* История изменений:
*
* Версия 1.0.1
* Дата   27-03-2017
* Автор  Кругликов В.П.
*
* Изменения:
*    Базовая версия.
*/
