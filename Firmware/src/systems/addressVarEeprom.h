/**
* \file    addressVarEeprom.h
* \brief   Адреса переменных и констант, хранящихся в EEPROM.
*
* \version 1.0.1
* \date    18-01-2017
* \author  Третьяков В.Ж.
*/

#ifndef addressVarEeprom_h
#define addressVarEeprom_h

//*****************************************************************************
// Глобальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
#define ADDRESS_EEPROM_IDENT_ID          0x0000    ///< Начальный адрес идентификации (ID прибора).
#define ADDRESS_EEPROM_IDENT_NAME        0x0002    ///< Начальный адрес идентификации (имя прибора).
#define ADDRESS_EEPROM_IDENT_VERSION     0x0010    ///< Начальный адрес идентификации (версия ПО).
#define ADDRESS_EEPROM_IDENT_DATE        0x0020    ///< Начальный адрес идентификации (дата компиляции ПО).

//*****************************************************************************
#define ADDRESS_EEPROM_PROTECTION_CODE_1 0x0030    ///< Адрес 1-ой ячейки \a EEPROM с кодом ЗОт.
#define ADDRESS_EEPROM_PROTECTION_CODE_4 0x0032    ///< Адрес 4-ой ячейки \a EEPROM с кодом ЗОт.
#define ADDRESS_SOURCE_RESET             0x0034    ///< Адрес ячейки \a EEPROM с причиной сброса (запуска) МК.
#define ADDRESS_EEPROM_TIME_SEC          0x0036    ///< Адрес ячейки \a EEPROM с текущим временем работы, с.
#define ADDRESS_EEPROM_TIME_MS           0x0038    ///< Адрес ячейки \a EEPROM с текущим временем работы, мс.

//*****************************************************************************
/// \brief Адрес ячейки \a EEPROM с текущим временем работы (номер прерывания от системного таймера).
///
#define ADDRESS_EEPROM_TIME_N_TMR        0x003A

//*****************************************************************************
/// \brief Адрес ячейки \a EEPROM с текущим временем работы (показания системного таймера).
///
#define ADDRESS_EEPROM_TIME_TMR          0x003C


#define ADDRESS_EEPROM_PROTECTION_STATE_ADD_CODE 0x0040    ///< Адрес массива \a EEPROM с уточняющей информацией для ЗОт.

//*****************************************************************************
/// \brief Начальный адрес структуры трассировки: адрес, количество массивов, размер массива.
///
#define ADDRESS_EEPROM_STRUCT_TRACING            0x0070
#define ADDRESS_EEPROM_TRACING_START             0x0080    ///< Начальный адрес буферов трассировки.

//*****************************************************************************
/// \brief Данные калибровки прибора
///
#define ADDRESS_EEPROM_CALIBR_ADDR                   0x0E90   ///< Адрес прибора при последнем включении
#define ADDRESS_EEPROM_CALIBR_CONF                   0x0E92   ///< Конфигурация прибора при последнем включении
#define ADDRESS_EEPROM_CALIBR_CRC                    0x0E94   ///< СRC при последней при последнем включении
#define ADDRESS_EEPROM_MAGIC_WORD                    0x0E96   ///< Признак записи заводского номера
#define ADDRESS_EEPROM_CALIBR_THRESHOLD_P            0x0E98   ///< Калибровочное значение тока при переводе в плюс
#define ADDRESS_EEPROM_CALIBR_THRESHOLD_M            0x0E9A   ///< Калибровочное значение тока при переводе в минус
#define ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_P 0x0E9C   ///< Калибровочное значение увеличенного времени перевода в пдюс
#define ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_M 0x0E9E   ///< Калибровочное значение увеличенного времени перевода в пдюс
#define  ADDRESS_EEPROM_MANUFACTURERS_NUMBER         0x0EA0


//*****************************************************************************
#define ADDRESS_EEPROM_PROTECTION_CODE_2         0x0FF0    ///< Адрес 2-ой ячейки \a EEPROM с кодом ЗОт.
#define ADDRESS_EEPROM_PROTECTION_CODE_3         0x0FF2    ///< Адрес 3-ей ячейки \a EEPROM с кодом ЗОт.


#endif

//*****************************************************************************
/**
* История изменений:
*
* Версия 1.0.1
* Дата   18-01-2017
* Автор  Третьяков В.Ж.
*
* Изменения:
*    Базовая версия.
*/
