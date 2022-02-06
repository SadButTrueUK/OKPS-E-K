/**
* \file    checkRegistersDrv.c
* \brief   \copybrief checkRegistersDrv.h
*
* \version 2.0.2
* \date    15-03-2021
* \author  Агулов М.А.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "typeMK.h"
#include "MainRegisters.h"
#include "checkRegistersDrv.h"

//*****************************************************************************
// Объявление глобальных переменных
//*****************************************************************************

extern uint16_t ADCbuf[32];

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Проверка основных регистров, используемых при работе
bool checkRegistersDrv( void )
{
    bool ret = true;

    // Главный таймер
    PR5 = MAIN_TIMER_LOAD_PR5;
    if( ( T5CON != 0x8000 ) || ( PR5 != MAIN_TIMER_LOAD_PR5 ) )
    {
        ret = false;
    }

    // Приоритет прерываний
    if( ( IPC7bits.T5IP != 3 ) || ( IPC4bits.CNIP != 4 ) ||
        ( IPC6bits.T4IP != 2 ) || ( INTCON1bits.NSTDIS != 0 ) )
    {
        ret = false;
    }

    // Проверка регистров управления АЦП
    if(( (AD1CON1 & (_AD1CON1_ASAM_MASK  | _AD1CON1_SIMSAM_MASK | _AD1CON1_SSRCG_MASK | _AD1CON1_SSRC_MASK | _AD1CON1_FORM_MASK | _AD1CON1_AD12B_MASK | _AD1CON1_ADDMABM_MASK)) ) != 0x04E4 ||
       ( (AD1CON2 & (_AD1CON2_ALTS_MASK  | _AD1CON2_BUFM_MASK   | _AD1CON2_SMPI_MASK  | _AD1CON2_BUFS_MASK | _AD1CON2_CHPS_MASK | _AD1CON2_CSCNA_MASK | _AD1CON2_VCFG_MASK   )) ) != 0x242C ||
       ( (AD1CON3 & (_AD1CON3_ADCS_MASK  | _AD1CON3_SAMC_MASK   | _AD1CON3_ADRC_MASK)) ) != 0x0F0F ||
       ( (AD1CON4 & (_AD1CON4_DMABL_MASK | _AD1CON4_ADDMAEN_MASK)) )       != 0x0100 ||     
       ( (AD1CHS0 & (_AD1CHS0_CH0SA_POSITION | _AD1CHS0_CH0NA_POSITION)) ) != 0x0000)
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

    
    // Проверка регистров управления DMA2 (АЦП)
    if( ((DMA2CON & (_DMA2CON_AMODE_POSITION | _DMA2CON_MODE_POSITION | _DMA2CON_SIZE_POSITION |_DMA2CON_HALF_POSITION |_DMA2CON_DIR_POSITION)) != 0x0001 ) ||
         (DMA2PAD  != (volatile uint16_t)&ADC1BUF0) ||
         (DMA2CNT  != AD1CON2bits.SMPI) ||
         (DMA2REQ  != 13) ||
         (DMA2STAL != __builtin_dmaoffset(ADCbuf)) ||
         (DMA2STAH != 0x0000))
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
* Дата   11-09-2017
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
* 
* Версия 2.0.2
* Дата   15-03-2021
* Автор  Агулов М.А.
* 
* Изменения:
*    Изменения для работы АЦП с DMA
*/
