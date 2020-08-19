/**
* \file    CheckCallFunctions.c
* \brief   \copybrief CheckCallFunctions.h
*
* \version 1.0.4
* \date    11-06-2019
* \author  Третьяков В.Ж., Годунок А.Н.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "asserts_ex.h"
#include "Eeprom.h"
#include "InterChannel.h"
#include "CheckCallFunctions.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************
#define N_STACK_NAME_FUNCTION  8        ///< Размер массива указателей вызываемых функций.

//*****************************************************************************
// Объявление типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Времена контроля синхронизации параметров модуля ControlMK.h, мс.
///
typedef enum  {
    eOUT_T1 = 10,        ///< максимальное время неприхода второго параметра
    eOUT_T2 = 20,        ///< максимальное время несовпадения параметров
    eOUT_T3 = 128        ///< максимальное время обновления параметра
} CheckCF_timeSynchr;

//*****************************************************************************
/// \brief Структура состояния.
///
typedef struct CheckCallFunctions_Tag {
     uint16_t    cCallFunctions;                                 ///< Счетчик вызовов функций.
     uint16_t    cCallFunctionsSync;                             ///< Синхронизируемое значение функций.
     const char *aNameFunction[N_STACK_NAME_FUNCTION];           ///< Указатель на массив вызываемых функций.
     uint8_t     ndx;                                            ///< Индекс массива указателей вызываемых функций. 
     uint8_t     cPeriod;                                        ///< Счетчик периода контроля.
} CheckCallFunctions; 

//*****************************************************************************
// Объявление локальных переменных
//*****************************************************************************

//*****************************************************************************
static CheckCallFunctions checkCallFunctions;          ///< Переменная структуры состояния.

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация
void CheckCallFunctions_ctor(void) 
{
    checkCallFunctions.cCallFunctions = 0U;
    checkCallFunctions.ndx            = 0U;
    checkCallFunctions.cPeriod        = 0U;
     
    InterChannel_setParamSettings( eICId_ControlMK_CF, eScriptSync, eProcSyncEqual, eProcCheckEqual, 0, 0,
                                   eOUT_T1, eOUT_T2, eOUT_T3);
}

//*****************************************************************************
// Вызов функции с сохранением ее имени
void CheckCallFunctions_call(void (*pFunction)(void), const char *ac)
{
    if (checkCallFunctions.ndx >= N_STACK_NAME_FUNCTION) {
        checkCallFunctions.ndx  = N_STACK_NAME_FUNCTION - 1;
    }
    checkCallFunctions.aNameFunction[checkCallFunctions.ndx++] = ac;
    pFunction();
    if (checkCallFunctions.ndx > 0) {
        checkCallFunctions.ndx--;
    }
}

//*****************************************************************************
//  Контроль количества вызванных функций
void CheckCallFunctions_run(void) 
{
    if ((++checkCallFunctions.cPeriod  >= (eOUT_T3/2)) &&  !InterChannel_isHandling(eICId_ControlMK_CF)) {
        checkCallFunctions.cPeriod = 0U; 
        InterChannel_synchronize(eICId_ControlMK_CF, (param_t)(checkCallFunctions.cCallFunctionsSync));
    }

     MARKED_CALL_FUNCTION;
}

//*****************************************************************************
// Передача для контроля количества вызванных функций  
void CheckCallFunctions_interrupt(bool sync) {
     if (sync) {
          checkCallFunctions.cCallFunctionsSync = checkCallFunctions.cCallFunctions;
     }
}

//*****************************************************************************
// Маркирование выполнения функции
void CheckCallFunctions_marked(void)
{
    checkCallFunctions.cCallFunctions++;
}

//*****************************************************************************
// Запись последних #N_STACK_NAME_FUNCTION имен функций в EEPROM
uint16_t CheckCallFunctions_saveBlackBox(uint16_t address)
{
    Eeprom_write(address, checkCallFunctions.ndx); 
    address += 2;

    const char * addressChar;
    uint8_t char1, char2, ndx = 0;

    while (ndx != checkCallFunctions.ndx) {
        addressChar = checkCallFunctions.aNameFunction[ndx++];
        while(true) {
            char1  = *addressChar++; 
            if (char1 == 0) {
                char2  = 0;
            } else {
                char2  = *addressChar++;
            }   
            Eeprom_write(address, ((uint16_t)(char1) << 8) | char2); 
            address += 2;
            if ((char1 == 0) || (char2 == 0)) {
                break;
            }
        } 
    }

    return (address);  
}

//*****************************************************************************
/**
* История изменений:
* 
* Версия 1.0.1
* Дата:  11-07-2016
* Автор: Третьяков В.Ж.
* 
* Изменения:
*     Базовая версия.
*
* Версия 1.0.2
* Дата   29-09-2017
* Автор  Третьяков В.Ж.
* 
* Изменения:
*     Добавлена функция  CheckCallFunctions_interrupt(bool sync)
*     для контроля вызываемых функций в прерывании, добавлена переменная 
*     cCallFunctionsSync в структуру checkCallFunctions для перезаписи
*     количества вызываемых функций в 15-м прерывании.
*
* Версия 1.0.3
* Дата   24-05-2019
* Автор  Годунок А.Н.
* 
* Изменения:
*     Для параметра eICId_ControlMK_CF изменен сценарий синхронизации
*     с eScriptChVal на eScriptSync.
*
* Версия 1.0.4
* Дата   11-06-2019
* Автор  Годунок А.Н.
* 
* Изменения:
*    Для контроля вызова функции CheckCallFunctions_run в нее добавлен макрос MARKED_CALL_FUNCTION.
*/
