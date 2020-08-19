/**
* \file    turnOffdsPIC33.c
* \brief   \copybrief turnOffdsPIC33.h
*
* \version 1.0.1
* \date    08-01-2017
* \author  Третьяков В.Ж.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "typeMK.h"
#include "MainRegisters.h"
#include "posDetGen_drv.h"
#include "shuntShiftGenDrv.h"
#include "ConfigMK.h"

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Отключение периферии МК
void turnOffMK( void )
{
    __asm__ volatile( "disi    #0x3FFF" ); //Запрет маскируемых прерываний
    RCONbits.SWDTEN = 0; //Останов Watchdog-таймера
    MAIN_TIMER_STOP;
    CN_INTERRUPT_CLEAR_FLAG;
    MAIN_TIMER_INTERRUPT_CLEAR_FLAG;
    INTERRUPT_ALL_DISABLE;

    if( ConfigMK_isMaster( ) ) 
        ShuntShiftGenDrv_off( );
    else 
        PosDetGenDrv_setDisable( );

    TRISA = 0xFFFF;
    ODCA = 0;
    CNENA = 0;
    CNPUA = 0;
    CNPDA = 0;
    ANSELA = 0xFFFF;
    TRISB = 0xFFFF;
    ODCB = 0;
    CNENB = 0;
    CNPUB = 0;
    CNPDB = 0;
    ANSELB = 0xFFFF;
    TRISC = 0xFFFF;
    ODCC = 0;
    CNENC = 0;
    CNPUC = 0;
    CNPDC = 0;
    ANSELC = 0xFFFF;
    TRISD = 0xFFFF;
    ODCD = 0;
    CNEND = 0;
    CNPUD = 0;
    CNPDD = 0;
    ANSELD = 0xFFFF;
    TRISE = 0xFFFF;
    ODCE = 0;
    CNENE = 0;
    CNPUE = 0;
    CNPDE = 0;
    ANSELE = 0xFFFF;
    TRISF = 0xFFFF;
    ODCF = 0;
    CNENF = 0;
    CNPUF = 0;
    CNPDF = 0;
    ANSELF = 0xFFFF;
    TRISG = 0xFFFF;
    ODCG = 0;
    CNENG = 0;
    CNPUG = 0;
    CNPDG = 0;
    ANSELG = 0xFFFF;

    RPINR0 = 0;
    RPINR1 = 0;
    RPINR3 = 0;
    RPINR7 = 0;
    RPINR8 = 0;
    RPINR9 = 0;
    RPINR10 = 0;
    RPINR11 = 0;
    RPINR12 = 0;
    RPINR14 = 0;
    RPINR15 = 0;
    RPINR16 = 0;
    RPINR17 = 0;
    RPINR18 = 0;
    RPINR19 = 0;
    RPINR22 = 0;
    RPINR23 = 0;
    RPINR24 = 0;
    RPINR25 = 0;
    RPINR26 = 0;
    RPINR27 = 0;
    RPINR28 = 0;
    RPINR29 = 0;
    RPINR30 = 0;
    RPINR37 = 0;
    RPINR38 = 0;
    RPINR39 = 0;
    RPINR40 = 0;
    RPINR41 = 0;

    RPOR0 = 0;
    RPOR1 = 0;
    RPOR2 = 0;
    RPOR3 = 0;
    RPOR4 = 0;
    RPOR5 = 0;
    RPOR6 = 0;
    RPOR7 = 0;
    RPOR8 = 0;
    RPOR9 = 0;
    RPOR10 = 0;
    RPOR11 = 0;
    RPOR12 = 0;
}

//*****************************************************************************
/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   08-01-2017
* Автор  Третьяков В.Ж. 
* 
* Изменения:
*    Базовая версия.
*/
