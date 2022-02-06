/**
* \file    AdcDriver_PIC33.c
* \brief   \copybrief AdcDriver_PIC33.h
*
* \version 1.0.2
* \date    15-03-2021
* \author  Агулов М.А.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "typeMK.h"
#include "asserts.h"
#include "ProtectionState_codes.h"
#include "AdcDriver_PIC33.h"
#include "IOports.h"
#include "CheckRAM.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
/// \brief Число каналов АЦП в процессоре dsPIC33EP512GM710
#define ADC_CHANNELS_NUMBER     32

//*****************************************************************************
/// \brief Имена каналов АЦП, доступные для процессора dsPIC33EP512GM710
#ifdef __dsPIC33EP512GM710__
    #define     AN0     0
    #define     AN1     1   
    #define     AN2     2   
    #define     AN3     3   
    #define     AN4     4       
    #define     AN5     5       
    #define     AN6     6       
    #define     AN7     7       
    #define     AN8     8       
    #define     AN9     9       
    #define     AN10   10       
    #define     AN11   11       
    #define     AN12   12       
    #define     AN13   13       
    #define     AN14   14       
    #define     AN15   15       
    #define     AN16   16       
    #define     AN17   17       
    #define     AN18   18       
    #define     AN19   19       
    #define     AN20   20       
    #define     AN21   21       
    #define     AN22   22       
    #define     AN23   23       
    #define     AN24   24       
    #define     AN25   25       
    #define     AN26   26       
    #define     AN27   27       
    #define     AN28   28       
    #define     AN29   29       
    #define     AN30   30       
    #define     AN31   31                   
#endif

//*****************************************************************************
// Объявление типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Структура, описывающая аналоговый пин процессора
typedef struct 
{
    volatile uint16_t * TRIS;
    volatile uint16_t * ANS ;
    uint16_t            mask;
} ain_pin_;

//*****************************************************************************
// Объявление локальных переменных
//*****************************************************************************

__eds__ uint16_t ADCbuf[ADC_CHANNELS_NUMBER]  __attribute__((address(RAM_END_ADDRESS + 0x400)));         // Буфер DMA

//*****************************************************************************
/// \brief Структура, описывающая настройки аналоговых пинов процессора dsPIC33EP512GM710 в корпусе TQFP-100
///
const ain_pin_ ain_pin[ADC_CHANNELS_NUMBER] = {
    {.TRIS = &TRISA,   .ANS = &ANSELA,   .mask = _ANSELA_ANSA0_MASK},    // Аналоговый вход AN0
    {.TRIS = &TRISA,   .ANS = &ANSELA,   .mask = _ANSELA_ANSA1_MASK},    // Аналоговый вход AN1
    {.TRIS = &TRISB,   .ANS = &ANSELB,   .mask = _ANSELB_ANSB0_MASK},    // Аналоговый вход AN2    
    {.TRIS = &TRISB,   .ANS = &ANSELB,   .mask = _ANSELB_ANSB1_MASK},    // Аналоговый вход AN3    
    {.TRIS = &TRISB,   .ANS = &ANSELB,   .mask = _ANSELB_ANSB2_MASK},    // Аналоговый вход AN4
    {.TRIS = &TRISB,   .ANS = &ANSELB,   .mask = _ANSELB_ANSB3_MASK},    // Аналоговый вход AN5
    {.TRIS = &TRISC,   .ANS = &ANSELC,   .mask = _ANSELC_ANSC0_MASK},    // Аналоговый вход AN6
    {.TRIS = &TRISC,   .ANS = &ANSELC,   .mask = _ANSELC_ANSC1_MASK},    // Аналоговый вход AN7
    {.TRIS = &TRISC,   .ANS = &ANSELC,   .mask = _ANSELC_ANSC2_MASK},    // Аналоговый вход AN8    
    {.TRIS = &TRISA,   .ANS = &ANSELA,   .mask = _ANSELA_ANSA11_MASK},   // Аналоговый вход AN9        

    {.TRIS = &TRISA,   .ANS = &ANSELA,   .mask = _ANSELA_ANSA12_MASK},   // Аналоговый вход AN10
    {.TRIS = &TRISC,   .ANS = &ANSELC,   .mask = _ANSELC_ANSC11_MASK},   // Аналоговый вход AN11
    {.TRIS = &TRISE,   .ANS = &ANSELE,   .mask = _ANSELE_ANSE12_MASK},   // Аналоговый вход AN12    
    {.TRIS = &TRISE,   .ANS = &ANSELE,   .mask = _ANSELE_ANSE13_MASK},   // Аналоговый вход AN13    
    {.TRIS = &TRISE,   .ANS = &ANSELE,   .mask = _ANSELE_ANSE14_MASK},   // Аналоговый вход AN14
    {.TRIS = &TRISE,   .ANS = &ANSELE,   .mask = _ANSELE_ANSE15_MASK},   // Аналоговый вход AN15
    {.TRIS = &TRISG,   .ANS = &ANSELG,   .mask = _ANSELG_ANSG9_MASK},    // Аналоговый вход AN16
    {.TRIS = &TRISG,   .ANS = &ANSELG,   .mask = _ANSELG_ANSG8_MASK},    // Аналоговый вход AN17
    {.TRIS = &TRISG,   .ANS = &ANSELG,   .mask = _ANSELG_ANSG7_MASK},    // Аналоговый вход AN18    
    {.TRIS = &TRISG,   .ANS = &ANSELG,   .mask = _ANSELG_ANSG6_MASK},    // Аналоговый вход AN19
    
    {.TRIS = &TRISE,   .ANS = &ANSELE,   .mask = _ANSELE_ANSE9_MASK},    // Аналоговый вход AN20
    {.TRIS = &TRISE,   .ANS = &ANSELE,   .mask = _ANSELE_ANSE8_MASK},    // Аналоговый вход AN21
    {.TRIS = &TRISG,   .ANS = &ANSELG,   .mask = _ANSELG_ANSG10_MASK},   // Аналоговый вход AN22    
    {.TRIS = &TRISG,   .ANS = &ANSELG,   .mask = _ANSELG_ANSG15_MASK},   // Аналоговый вход AN23    
    {.TRIS = &TRISA,   .ANS = &ANSELA,   .mask = _ANSELA_ANSA4_MASK},    // Аналоговый вход AN24
    {.TRIS = &TRISB,   .ANS = &ANSELB,   .mask = _ANSELB_ANSB7_MASK},    // Аналоговый вход AN25
    {.TRIS = &TRISB,   .ANS = &ANSELB,   .mask = _ANSELB_ANSB8_MASK},    // Аналоговый вход AN26
    {.TRIS = &TRISB,   .ANS = &ANSELB,   .mask = _ANSELB_ANSB9_MASK},    // Аналоговый вход AN27
    {.TRIS = &TRISA,   .ANS = &ANSELA,   .mask = _ANSELA_ANSA9_MASK},    // Аналоговый вход AN28    
    {.TRIS = &TRISC,   .ANS = &ANSELC,   .mask = _ANSELC_ANSC3_MASK},    // Аналоговый вход AN29

    {.TRIS = &TRISC,   .ANS = &ANSELC,   .mask = _ANSELC_ANSC4_MASK},    // Аналоговый вход AN30    
    {.TRIS = &TRISC,   .ANS = &ANSELC,   .mask = _ANSELC_ANSC5_MASK},    // Аналоговый вход AN31
    
};
//*****************************************************************************



//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

/*   Назначение каналов АЦП в приборе ОКПС-Е-К
 *  ---------------------------------------------------------------------------------------------------------------
 * | Канал | Порт | Пин | Сигнал | Имя сигнала  |    Назначение сигнала                                            |
 * |-------+------+-----+--------+--------------+------------------------------------------------------------------|
 * |  AN0  | RA0  |  22 | R1     | eAinchR1     | Сигнал датчика положения положения R1                            |
 * |  AN1  | RA1  |  23 | R2     | eAinchR2     | Сигнал датчика положения положения R2                            |
 * |  AN4  | RB2  |  26 | K3V3   | eAinchK3V3   | Контроль напряжения питания процессора                           |
 * |  AN5  | RB3  |  27 | VREF   | eAinchUREF   | Контроль работоспособности АЦП подачей на вход напряжения UREF   |
 * |  AN6  | RB4  |  29 | КREF   | eAinchKREF   | Контроль опорного напряжения смежного контроллера                |
 * |  AN9  | RA11 |  21 | GEN    | eAinchGEN    | Контроль напряжения генератора определения положения 62,5 Гц     |
 * |                                                                                                               |
 * |  AN16 | RG9  |  14 | I-V    | eAinchIV     | Сигнал датчика тока двигателя фазы V                             |
 * |  AN17 | RG8  |  12 | I-U    | eAinchIU     | Сигнал датчика тока двигателя фазы U                             |
 * |  AN18 | RG7  |  11 | I-W    | eAinchIW     | Сигнал датчика тока двигателя фазы W                             |
 * |                                                                                                               |
 * |  AN21 | RE8  |  18 | U-W    | eAinchUW     | Сигнал датчика напряжения двигателя фазы W                       |
 * |  AN20 | RE9  |  19 | U-U    | eAinchUU     | Сигнал датчика напряжения двигателя фазы U                       |
 * |  AN10 | RA12 |  20 | U-V    | eAinchUV     | Сигнал датчика напряжения двигателя фазы V                       |
 *  ---------------------------------------------------------------------------------------------------------------
 */
// *** Используемые аналоговые каналы в режиме сканирования АЦП.
// Неиспользуемые каналы необходимо закомментировать. Все 
// раскомментированные каналы будут участвовать в сканировании, а результаты измерений
// помещены в буфер ADCbuf в порядке возрастания их номеров. 
const uint8_t ADC_scan_ch [] = {
      AN0,                  // Аналоговый канал AN0
      AN1,                  // Аналоговый канал AN1
//    AN2,                  // Аналоговый канал AN2
//    AN3,                  // Аналоговый канал AN3
      AN4,                  // Аналоговый канал AN4
      AN5,                  // Аналоговый канал AN5
      AN6,                  // Аналоговый канал AN6
//    AN7,                  // Аналоговый канал AN7
//    AN8,                  // Аналоговый канал AN8
      AN9,                  // Аналоговый канал AN9
      AN10,                 // Аналоговый канал AN10
//    AN11,                 // Аналоговый канал AN11
//    AN12,                 // Аналоговый канал AN12
//    AN13,                 // Аналоговый канал AN13
//    AN14,                 // Аналоговый канал AN14
//    AN15,                 // Аналоговый канал AN15 
      AN16,                 // Аналоговый канал AN16
      AN17,                 // Аналоговый канал AN17
      AN18,                 // Аналоговый канал AN18
//    AN19,                 // Аналоговый канал AN19
      AN20,                 // Аналоговый канал AN20
      AN21,                 // Аналоговый канал AN21
//    AN22,                 // Аналоговый канал AN22
//    AN23,                 // Аналоговый канал AN23
//    AN24,                 // Аналоговый канал AN24
//    AN25,                 // Аналоговый канал AN25
//    AN26,                 // Аналоговый канал AN26
//    AN27,                 // Аналоговый канал AN27
//    AN28,                 // Аналоговый канал AN28
//    AN29,                 // Аналоговый канал AN29
//    AN30,                 // Аналоговый канал AN30
//    AN31                  // Аналоговый канал AN31
};


//*****************************************************************************
// Инициализация модуля АЦП
void AdcDrv_ctor( )
{
    /* Инициализация модуля АЦП */
    AD1CON1bits.ASAM    = 1;            
    AD1CON1bits.SIMSAM  = 0;            
    AD1CON1bits.SSRCG   = 0;
    AD1CON1bits.SSRC    = 0b111;
    AD1CON1bits.FORM    = 0b00;
    AD1CON1bits.AD12B   = 1;
    AD1CON1bits.ADDMABM = 0;

    AD1CON2bits.ALTS  = 0;
    AD1CON2bits.BUFM  = 0;
    AD1CON2bits.SMPI  = 0b00000;
    AD1CON2bits.BUFS  = 0;
    AD1CON2bits.CHPS  = 0;
    AD1CON2bits.CSCNA = 1;
    AD1CON2bits.VCFG  = 0b001;

    AD1CON3bits.ADCS = 0x0F;
    AD1CON3bits.SAMC = 0b01111;
    AD1CON3bits.ADRC = 0;

    AD1CON4bits.DMABL = 0b000;
    AD1CON4bits.ADDMAEN = 1;

    AD1CHS0bits.CH0SA = 0; 
    AD1CHS0bits.CH0NA = 0; 


    //  Установка в регистрах AD1CSSL и AD1CSSH признаков автоматически сканируемых АЦП каналов.
    // В массиве ADC_scan_chan[] список сканируемых АЦП каналов (определяется пользователем).
    for(uint8_t i = 0;  i < sizeof (ADC_scan_ch);  i++){
    //  Информация об автоматически сканируемых АЦП каналах с номерами 0...15 содержится в регистре ADхCSSL,
    // с номерами 16...31 содержатся в регистре ADхCSSH. Канал участвует в сканировании - лог.1, НЕ участвует - лог. 0.
        ADC_scan_ch[i] < 16 ? (AD1CSSL |= 1<<ADC_scan_ch[i]) : (AD1CSSH |= 1<<(ADC_scan_ch[i] - 16));
    }

    
    //  Инициализация пинов внешнего опорного напряжения (Vref+ и Vref-) как аналоговые входы. Эти пины
    // инициализируются отдельно, т.к. не входят в список сканируемых каналов.
    INIC_PIN_ANALOG(F,  9);
    INIC_PIN_ANALOG(F, 10);    

    
    // Инициализация пинов для сканируемых АЦП аналоговых каналов.
    for(uint8_t i = 0;  i < sizeof (ADC_scan_ch);  i++){
        *ain_pin[i].TRIS = *ain_pin[i].TRIS | ain_pin[i].mask;                // Инициализация регистра направления TRIS на вход
        *ain_pin[i].ANS  = *ain_pin[i].ANS  | ain_pin[i].mask;                // Инициализация регистра направления ANSEL в режим аналоговый вход    
    }
    
    // В регистр SMPI записывается число на единицу меньше числа сканируемых АЦП каналов
    AD1CON2bits.SMPI = sizeof(ADC_scan_ch) - 1;    


    /* Инициализация модуля DMA */
    DMA2CONbits.AMODE = 0;          
    DMA2CONbits.MODE  = 1;          
    DMA2CONbits.SIZE  = 0;          
    DMA2CONbits.HALF  = 0;
    DMA2CONbits.DIR   = 0;
    
    DMA2PAD = (volatile uint16_t)&ADC1BUF0;
    DMA2CNT = AD1CON2bits.SMPI;    
    DMA2REQ = 13;                   
    DMA2STAL = __builtin_dmaoffset(ADCbuf);
    DMA2STAH = 0x0000;

    AdcDrv_Start( );      
}


void AdcDrv_Start( )
{
    DMA2CONbits.CHEN = 1;
    AD1CON1bits.ADON = 1;    
}


void AdcDrv_Stop( )
{
    // Проверка на завершение всех транзакций DMA
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcErrorDMA, AdcDrv_isReady());
    
    AD1CON1bits.ADON = 0;
}


bool AdcDrv_isReady ( )
{
    return !DMA2CONbits.CHEN;
}

//*****************************************************************************
/**
* Пояснения:  
*    Драйвер рассчитан на сканирование входов по каналу 1.
*    Каналы для сканирования задаются в константах INIC_ADC1_ADCSSLH INIC_ADC1_ADCSSL.
*    Сканирование реализовано  программно.
*    При сканировании в автоматическом режиме АЦП были изредка 
*    некорректные измерения. Требовалось вводить ожидание сканирование каналов,
*    что в данном проекте требовало значительных расходов ресурсов. 
*/

/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   28-03-2016
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
*   Драйвер переделан для использования DMA. АЦП работает в режиме сканирования каналов.
*   Каналы задаются в структуре ADC_scan_ch.
*/
