/**
* \file    ControlMK.c
* \brief   \copybrief ControlMK.h
*
* \version 1.0.1
* \date    11-07-2016
* \author  Третьяков В.Ж.
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

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация переменных компонента ControlMK
void ControlMK_ctor( void )
{
#ifndef IGNORE_CONTROL_MK
    CheckRAM_ctor( );
    #ifdef FLASH_CHECK_ENABLE
        FlashCheck_ctor( );
    #endif
#endif
}

//*****************************************************************************
// Проверка работоспособности МК
void ControlMK_run( void )
{
#ifndef IGNORE_CONTROL_MK
    ASSERT_ID( eGrPS_ControlMK, ePS_ControlMkFaultCPU, CheckCPU_run());
    ASSERT_ID( eGrPS_ControlMK, ePS_InterruptCPU, Main_getTimeWorkInterrupt() == 0 );
    ASSERT_ID( eGrPS_ControlMK, ePS_ControlMkFaultRegisters, CheckRegisters_run());
    #ifdef FLASH_CHECK_ENABLE
        FlashCheck_run( );
    #endif
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
*/
