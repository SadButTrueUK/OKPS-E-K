/**
* \file    DeviceAddress.c
* \brief   \copybrief DeviceAddress.h
*
* \version 1.0.1
* \date    27-03-2017
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
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
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
#define FAILURE_TRANSITION_T_O 20000        ///< Время перехода в ЗС.
#define FIRST_CHECK_T_O        300          ///< Тайм-аут повторения первичной проверки.
#define PERIODIC_CHECK_T_O     5000         ///< Тайм-аут периодической проверки.
#define NUM_CALIBRATIONS       2            ///< Количество калибровочных данных (калибровка в "+" и в "-")

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
// Кодирование типа двигателя и напряжение питания двигателя
#define MAGIC_WORD  0x1059
//*****************************************************************************
// Объявление типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Идентификаторы управления работой модуля.
///
typedef enum
{
    eDeviceAddr_firstCheck  = 0,                ///< первичная проверка
    eDeviceAddr_periodCheck,                    ///< периодическая проверка
    eDeviceAddr_WriteEEPROM_addr,               ///< Запись адреса в EEPROM
    eDeviceAddr_WriteEEPROM_config,             ///< Запись конфигурации в EEPROM
    eDeviceAddr_WriteEEPROM_crc,                ///< Запись CRC в EEPROM
    eDeviceAddr_WriteEEPROM_status,                ///< Запись CRC в EEPROM   
    eDeviceAddr_WaitingForWriteCompleteEEPROM,   ///< Ожидание завершения записи в EEPROM
    eDeviceAddr_WaitingForWriteCompleteEEPROM1        
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
/// \brief Объединение флагов модуля с \a uint8_t.
///
typedef union
{
    uint8_t          data;        ///< Флаги модуля.
    DeviceAddr_flags str;         ///< Данные флага.
} uDeviceAddr_flags;

//*****************************************************************************
/// \brief Данные режима калибровки.
///
typedef struct
{
    uint16_t address;               ///< Адрес, прочитанный из EEPROM при включении прибора.
    uint16_t config;                ///< Конфигурация, прочитанная из EEPROM при включении прибора.
    uint16_t crc;                   ///< CRC, прочитанная из EEPROM при включении прибора.
    uint16_t threshold_p;          ///< Калибровочное значение тока в режиме перевода в плюс.    
    uint16_t threshold_m;          ///< Калибровочное значение тока в режиме перевода в минус.        
    uint16_t increasedTime_p;      ///< значение увеличенного тока времнени при калибровочном переводе в плюс
    uint16_t increasedTime_m;      ///< значение увеличенного тока времнени при калибровочном переводе в минус
} DeviceAddr_calibr;


//*****************************************************************************
// Объявление локальных переменных
//*****************************************************************************

//*****************************************************************************
static          DeviceAddr_Jumpers    jumpers;         ///< Состояние установленных перемычек.
static          uDeviceAddr_Addr      address;         ///< Адрес устройства.
static          uDeviceAddr_Config    config;          ///< Конфигурация устройства.
static uint16_t failureTimeoutCnt;                     ///< Счетчик тайм-аута перехода в ЗС.
static uint16_t timeoutCnt;                            ///< Счетчик тайм-аута проверки.
static          eDeviceAddr_Run       ctrlCnt;         ///< Счетчик состояния.
static          DeviceAddress_sysType sys_Type;        ///< Тип системы.
static          DeviceAddr_calibr     calibr;          ///< Данные о калибровке прибора.

static const char SerialNumber[16] = "1234567";
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
    failureTimeoutCnt = FAILURE_TRANSITION_T_O;                 //Установить тайм-аут перехода в ЗС
    timeoutCnt = FIRST_CHECK_T_O;                               //Установить тайм-аут первичной проверки 
    ctrlCnt = eDeviceAddr_firstCheck;                           //Переход к выполнению первичной проверки


    // Запись в EEPROM серийного номера прибора
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
    
    
    
    // Чтение из EEPROM пороговых значений тока калибровки        
    calibr.crc     = Eeprom_read( ADDRESS_EEPROM_CALIBR_CRC );
    calibr.config  = Eeprom_read( ADDRESS_EEPROM_CALIBR_CONF );
    calibr.address = Eeprom_read( ADDRESS_EEPROM_CALIBR_ADDR );
    calibr.threshold_p  = Eeprom_read( ADDRESS_EEPROM_CALIBR_THRESHOLD_P );
    calibr.threshold_m  = Eeprom_read( ADDRESS_EEPROM_CALIBR_THRESHOLD_M );    
    calibr.increasedTime_m = Eeprom_read( ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_M );
    calibr.increasedTime_p = Eeprom_read( ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_P );

    flags.str.ctrl = 1;                                         //Включить = Eeprom_read( ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_M );
}

//*****************************************************************************
// Управление работой
void DeviceAddress_run( void )
{
    if( flags.str.ctrl == 0 )
        return;                               //Модуль не включен

    failureTimeoutCnt == 0 ? : failureTimeoutCnt--;
    timeoutCnt        == 0 ? : timeoutCnt--;

    switch( ctrlCnt )
    {
        case eDeviceAddr_firstCheck:                    //Проверка при первом включении
            ASSERT_ID( eGrPS_DeviceAddress, ePS_DeviceAddressFirstCheckError, failureTimeoutCnt != 0 );                       //Проверка тайм-аута перехода в ЗС

            //Ожидание тайм-аута для выполнения проверки
            if( timeoutCnt != 0 ) 
                break;

            jumpers.addr.value = BinIn_getAddrJumpers( );
            jumpers.conf.value = BinIn_getConfigJumpers( );
            jumpers.crc        = BinIn_getCrcJumpers( );

            if( jumpers.crc != crc8_update( crc8_update( 0xFF, jumpers.addr.array, 2 ), &jumpers.conf.value, 1 ) )
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
            flags.str.validity = 1;                             //Считанные данные корректны
            
            //Первичная проверка окончена, задаю тайм-ауты для периодической проверки
            timeoutCnt        = PERIODIC_CHECK_T_O;
            failureTimeoutCnt = FAILURE_TRANSITION_T_O;

            // Проверка значения адреса калибровки из EEPROM фактически установленному адресу
            if (calibr.address != jumpers.addr.value || calibr.config != jumpers.conf.value || calibr.crc != jumpers.crc)
                ctrlCnt = eDeviceAddr_WriteEEPROM_addr;         // Запись нового адреса прибора в EEPROM
            else
                ctrlCnt = eDeviceAddr_periodCheck;              // Адрес не поменялся, можно продолжать работу
            break;

        case eDeviceAddr_WriteEEPROM_addr:
            Eeprom_write_start(ADDRESS_EEPROM_CALIBR_ADDR, calibr.address = jumpers.addr.value);                        // Начинаем запись в EEPROM адреса прибора
            ctrlCnt = eDeviceAddr_WriteEEPROM_config;
            break;

        case eDeviceAddr_WriteEEPROM_config:
            if (Eeprom_isReady() == false)
                break;
            Eeprom_write_start(ADDRESS_EEPROM_CALIBR_CONF, calibr.config = jumpers.conf.value);  // Начинаем запись в EEPROM конфигурации прибора
            ctrlCnt = eDeviceAddr_WriteEEPROM_crc;
            break;


        case eDeviceAddr_WriteEEPROM_crc:
            if (Eeprom_isReady() == false)
                break;
            Eeprom_write_start(ADDRESS_EEPROM_CALIBR_CRC, calibr.crc = jumpers.crc);           // Начинаем запись в EEPROM CRC
            ctrlCnt =  eDeviceAddr_WriteEEPROM_status;
            break;

        case eDeviceAddr_WriteEEPROM_status:
            if (Eeprom_isReady() == false)
                break;
            Eeprom_write_start(ADDRESS_EEPROM_CALIBR_THRESHOLD_P, 0x0000);                     // Начинаем запись в EEPROM Status (сбрасываем признаки калибровки))
            ctrlCnt = eDeviceAddr_WaitingForWriteCompleteEEPROM;
            break;

            
        case eDeviceAddr_WaitingForWriteCompleteEEPROM:
            if (Eeprom_isReady() == false)
                break;
            Eeprom_write_start(ADDRESS_EEPROM_CALIBR_THRESHOLD_M, 0x0000);                                   // Начинаем запись в EEPROM Status (сбрасываем признаки калибровки))          
            ctrlCnt = eDeviceAddr_WaitingForWriteCompleteEEPROM1;
            break;


        case eDeviceAddr_WaitingForWriteCompleteEEPROM1:
            if (Eeprom_isReady() )
            {
                ctrlCnt = eDeviceAddr_periodCheck;
            }
            break;
            
        case eDeviceAddr_periodCheck: //Периодическая проверка
            ASSERT_ID( eGrPS_DeviceAddress, ePS_DeviceAddressCheckError, failureTimeoutCnt != 0 );                       //Проверка тайм-аута перехода в ЗС

            if( timeoutCnt != 0 )                                                                                       //Ожидание тайм-аута для выполнения проверки
                break;
            if( jumpers.addr.value == BinIn_getAddrJumpers( ) && jumpers.conf.value == BinIn_getConfigJumpers( ) &&
                jumpers.crc == BinIn_getCrcJumpers( ) )
            {
                failureTimeoutCnt = FAILURE_TRANSITION_T_O;                                                             // Проверка проёдена - сбрасываем таймер
            }    
            timeoutCnt = PERIODIC_CHECK_T_O;
            break;

        default:
            ERROR_ID( eGrPS_DeviceAddress, ePS_DeviceAddressStepCntError );
    }
    MARKED_CALL_FUNCTION;
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
// Получить состояние калибровки (наличие калибровки) 
bool DeviceAddress_getIsCalibrPresents( CalibrDir dir )
{
    if (dir == Plus)
        return calibr.threshold_p == 0 ? false : true;
    else
        return calibr.threshold_m == 0 ? false : true;
}

//*****************************************************************************
// Получить значения тока калибровки 
uint16_t DeviceAddress_getCalibrCurrent( CalibrDir dir )
{
        return dir == Plus ? calibr.threshold_p : calibr.threshold_m;
}

uint16_t DeviceAddress_getIncreasedTime( CalibrDir dir )
{
        return dir == Plus ? calibr.increasedTime_p : calibr.increasedTime_m;
}

//*****************************************************************************
// Установить значение тока калибровки и начать его запись в EEPROM
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
* История изменений:
*
* Версия 1.0.1
* Дата   27-03-2017
* Автор  Кругликов В.П.
*
* Изменения:
*    Базовая версия.
*/
