/**
* \file    ControlMK.c
* \brief   \copybrief ControlMK.h
*
* \version 1.0.2
* \date    15-03-2021
* \author  Агулов М.А.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "typeMK.h"
#include "defCompil.h"
#include "asserts_ex.h"
#include "InterChannel.h"
#include "Main.h" 
#include "ControlMK.h"
#include "FlashCheck.h"
#include "CheckRAM.h"
#include "CheckCPU.h"
#include "CheckRegisters.h"
#include "CheckCallFunctions.h"


//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация переменных компонента ControlMK
void ControlMK_ctor( void )
{
#ifndef IGNORE_CONTROL_MK
    CheckRAM_ctor( );
    #ifndef IGNORE_CHECK_FLASH
        FlashCheck_ctor( );
    #endif
    CheckCallFunctions_ctor();
#endif
}

//*****************************************************************************
// Проверка работоспособности МК
void ControlMK_run( void )
{
#ifndef IGNORE_CONTROL_MK
    ASSERT_ID( eGrPS_ControlMK, ePS_ControlMkFaultCPU, CheckCPU_run( ) );
    ASSERT_ID( eGrPS_ControlMK, ePS_InterruptCPU, Main_getTimeWorkInterrupt( ) == 0 );
    ASSERT_ID( eGrPS_ControlMK, ePS_ControlMkFaultRegisters, CheckRegisters_run( ) );
    #ifndef IGNORE_CHECK_FLASH
        FlashCheck_run( );
    #endif
    CheckCallFunctions_run( );
    MARKED_CALL_FUNCTION;
#endif
}

//*****************************************************************************
/**
* Пояснения:
*    Модуль ControlMK является главным модулем проверок МК.
*    В его задачи входят вызов функций контроля, проверок и контроль совпадения
*    обобщенных результатов контроля по двум МК.
*/

//*****************************************************************************
/**
* История изменений:
*
* Версия 1.0.1
* Дата   11-07-2016
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Базовая версия.
* 
* Версия 1.0.2
* Дата   15-03-2021
* Автор  Агулов М.А.
* 
* Изменения:
*   Коррекции связаны с изменением написанием макросов в файле defCompil.h
*/
