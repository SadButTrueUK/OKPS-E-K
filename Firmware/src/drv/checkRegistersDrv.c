/**
* \file    checkRegistersDrv.c
* \brief   \copybrief checkRegistersDrv.h
*
* \version 2.0.1
* \date    26-09-2017
* \author  Третьяков В.Ж.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "typeMK.h"
#include "MainRegisters.h"
#include "Adc_Inic1.h"
#include "checkRegistersDrv.h"

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Проверка основных регистров, используемых при работе
bool checkRegistersDrv( void )
{
    bool ret = true;

    // Главный таймер
    PR5 = TIME_PERIOD_INTERRUPT;
    if( ( T5CON != 0x8000 ) || ( PR5 != TIME_PERIOD_INTERRUPT ) )
    {
        ret = false;
    }

    // Приоритет прерываний
    if( ( IPC7bits.T5IP != 3 ) || ( IPC4bits.CNIP != 4 ) || ( IPC3bits.AD1IP != 1 ) ||
        ( IPC6bits.T4IP != 2 ) || ( INTCON1bits.NSTDIS != 0 ) )
    {
        ret = false;
    }

    // АЦП
    if( ( ( AD1CON1 & 0xFFFE ) != ( INIC_ADC1_ADCON1 | 0x8000 ) ) ||
        ( ( AD1CON2 & 0xFF7F ) != INIC_ADC1_ADCON2 ) ||
        ( AD1CON3 != INIC_ADC1_ADCON3 ) ||
        ( AD1CON4 != INIC_ADC1_ADCON4 ) ) 
    {
        ret = false;
    }

    // CAN1 - межканальная синхронизация
    if( ( ( C1CTRL1 & 0xE81E ) != 0x2808 ) ||
        ( C1CTRL2 != 0 ) ||
        ( C1CFG1 != 7 ) ||
        ( C1FCTRL != 0xC000 ) ||
        ( ( C1TR01CON & 0x8383 ) != 0x0083 ) ||
        ( C1FEN1 != 0xFFFF ) )
    {
        ret = false;
    }

    // DMA CAN1 - межканальная синхронизация
    if( ( DMA0CON != 0xA020 ) ||
        ( DMA0REQ != 0x0046 ) ||
        ( DMA0CNT != 0x0007 ) ||
        ( DMA0PAD != 0x0442 ) )
    {
        ret = false;
    }
    if( ( DMA1CON != 0x8020 ) ||
        ( DMA1REQ != 0x0022 ) ||
        ( DMA1CNT != 0x0007 ) ||
        ( DMA1PAD != 0x0440 ) )
    {
        ret = false;
    }

    return ret;
}

//*****************************************************************************
/**
* История изменений:
* 
* Версия 1.0.1
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Базовая версия.
* 
* Версия 2.0.1
* Дата   26-09-2017
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Изменения под новую схему на dsPIC33.
*/
