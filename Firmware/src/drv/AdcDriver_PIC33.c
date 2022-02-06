/**
* \file    AdcDriver_PIC33.c
* \brief   \copybrief AdcDriver_PIC33.h
*
* \version 1.0.2
* \date    15-03-2021
* \author  ������ �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "typeMK.h"
#include "asserts.h"
#include "ProtectionState_codes.h"
#include "AdcDriver_PIC33.h"
#include "IOports.h"
#include "CheckRAM.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ����� ������� ��� � ���������� dsPIC33EP512GM710
#define ADC_CHANNELS_NUMBER     32

//*****************************************************************************
/// \brief ����� ������� ���, ��������� ��� ���������� dsPIC33EP512GM710
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
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ���������, ����������� ���������� ��� ����������
typedef struct 
{
    volatile uint16_t * TRIS;
    volatile uint16_t * ANS ;
    uint16_t            mask;
} ain_pin_;

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************

__eds__ uint16_t ADCbuf[ADC_CHANNELS_NUMBER]  __attribute__((address(RAM_END_ADDRESS + 0x400)));         // ����� DMA

//*****************************************************************************
/// \brief ���������, ����������� ��������� ���������� ����� ���������� dsPIC33EP512GM710 � ������� TQFP-100
///
const ain_pin_ ain_pin[ADC_CHANNELS_NUMBER] = {
    {.TRIS = &TRISA,   .ANS = &ANSELA,   .mask = _ANSELA_ANSA0_MASK},    // ���������� ���� AN0
    {.TRIS = &TRISA,   .ANS = &ANSELA,   .mask = _ANSELA_ANSA1_MASK},    // ���������� ���� AN1
    {.TRIS = &TRISB,   .ANS = &ANSELB,   .mask = _ANSELB_ANSB0_MASK},    // ���������� ���� AN2    
    {.TRIS = &TRISB,   .ANS = &ANSELB,   .mask = _ANSELB_ANSB1_MASK},    // ���������� ���� AN3    
    {.TRIS = &TRISB,   .ANS = &ANSELB,   .mask = _ANSELB_ANSB2_MASK},    // ���������� ���� AN4
    {.TRIS = &TRISB,   .ANS = &ANSELB,   .mask = _ANSELB_ANSB3_MASK},    // ���������� ���� AN5
    {.TRIS = &TRISC,   .ANS = &ANSELC,   .mask = _ANSELC_ANSC0_MASK},    // ���������� ���� AN6
    {.TRIS = &TRISC,   .ANS = &ANSELC,   .mask = _ANSELC_ANSC1_MASK},    // ���������� ���� AN7
    {.TRIS = &TRISC,   .ANS = &ANSELC,   .mask = _ANSELC_ANSC2_MASK},    // ���������� ���� AN8    
    {.TRIS = &TRISA,   .ANS = &ANSELA,   .mask = _ANSELA_ANSA11_MASK},   // ���������� ���� AN9        

    {.TRIS = &TRISA,   .ANS = &ANSELA,   .mask = _ANSELA_ANSA12_MASK},   // ���������� ���� AN10
    {.TRIS = &TRISC,   .ANS = &ANSELC,   .mask = _ANSELC_ANSC11_MASK},   // ���������� ���� AN11
    {.TRIS = &TRISE,   .ANS = &ANSELE,   .mask = _ANSELE_ANSE12_MASK},   // ���������� ���� AN12    
    {.TRIS = &TRISE,   .ANS = &ANSELE,   .mask = _ANSELE_ANSE13_MASK},   // ���������� ���� AN13    
    {.TRIS = &TRISE,   .ANS = &ANSELE,   .mask = _ANSELE_ANSE14_MASK},   // ���������� ���� AN14
    {.TRIS = &TRISE,   .ANS = &ANSELE,   .mask = _ANSELE_ANSE15_MASK},   // ���������� ���� AN15
    {.TRIS = &TRISG,   .ANS = &ANSELG,   .mask = _ANSELG_ANSG9_MASK},    // ���������� ���� AN16
    {.TRIS = &TRISG,   .ANS = &ANSELG,   .mask = _ANSELG_ANSG8_MASK},    // ���������� ���� AN17
    {.TRIS = &TRISG,   .ANS = &ANSELG,   .mask = _ANSELG_ANSG7_MASK},    // ���������� ���� AN18    
    {.TRIS = &TRISG,   .ANS = &ANSELG,   .mask = _ANSELG_ANSG6_MASK},    // ���������� ���� AN19
    
    {.TRIS = &TRISE,   .ANS = &ANSELE,   .mask = _ANSELE_ANSE9_MASK},    // ���������� ���� AN20
    {.TRIS = &TRISE,   .ANS = &ANSELE,   .mask = _ANSELE_ANSE8_MASK},    // ���������� ���� AN21
    {.TRIS = &TRISG,   .ANS = &ANSELG,   .mask = _ANSELG_ANSG10_MASK},   // ���������� ���� AN22    
    {.TRIS = &TRISG,   .ANS = &ANSELG,   .mask = _ANSELG_ANSG15_MASK},   // ���������� ���� AN23    
    {.TRIS = &TRISA,   .ANS = &ANSELA,   .mask = _ANSELA_ANSA4_MASK},    // ���������� ���� AN24
    {.TRIS = &TRISB,   .ANS = &ANSELB,   .mask = _ANSELB_ANSB7_MASK},    // ���������� ���� AN25
    {.TRIS = &TRISB,   .ANS = &ANSELB,   .mask = _ANSELB_ANSB8_MASK},    // ���������� ���� AN26
    {.TRIS = &TRISB,   .ANS = &ANSELB,   .mask = _ANSELB_ANSB9_MASK},    // ���������� ���� AN27
    {.TRIS = &TRISA,   .ANS = &ANSELA,   .mask = _ANSELA_ANSA9_MASK},    // ���������� ���� AN28    
    {.TRIS = &TRISC,   .ANS = &ANSELC,   .mask = _ANSELC_ANSC3_MASK},    // ���������� ���� AN29

    {.TRIS = &TRISC,   .ANS = &ANSELC,   .mask = _ANSELC_ANSC4_MASK},    // ���������� ���� AN30    
    {.TRIS = &TRISC,   .ANS = &ANSELC,   .mask = _ANSELC_ANSC5_MASK},    // ���������� ���� AN31
    
};
//*****************************************************************************



//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

/*   ���������� ������� ��� � ������� ����-�-�
 *  ---------------------------------------------------------------------------------------------------------------
 * | ����� | ���� | ��� | ������ | ��� �������  |    ���������� �������                                            |
 * |-------+------+-----+--------+--------------+------------------------------------------------------------------|
 * |  AN0  | RA0  |  22 | R1     | eAinchR1     | ������ ������� ��������� ��������� R1                            |
 * |  AN1  | RA1  |  23 | R2     | eAinchR2     | ������ ������� ��������� ��������� R2                            |
 * |  AN4  | RB2  |  26 | K3V3   | eAinchK3V3   | �������� ���������� ������� ����������                           |
 * |  AN5  | RB3  |  27 | VREF   | eAinchUREF   | �������� ����������������� ��� ������� �� ���� ���������� UREF   |
 * |  AN6  | RB4  |  29 | �REF   | eAinchKREF   | �������� �������� ���������� �������� �����������                |
 * |  AN9  | RA11 |  21 | GEN    | eAinchGEN    | �������� ���������� ���������� ����������� ��������� 62,5 ��     |
 * |                                                                                                               |
 * |  AN16 | RG9  |  14 | I-V    | eAinchIV     | ������ ������� ���� ��������� ���� V                             |
 * |  AN17 | RG8  |  12 | I-U    | eAinchIU     | ������ ������� ���� ��������� ���� U                             |
 * |  AN18 | RG7  |  11 | I-W    | eAinchIW     | ������ ������� ���� ��������� ���� W                             |
 * |                                                                                                               |
 * |  AN21 | RE8  |  18 | U-W    | eAinchUW     | ������ ������� ���������� ��������� ���� W                       |
 * |  AN20 | RE9  |  19 | U-U    | eAinchUU     | ������ ������� ���������� ��������� ���� U                       |
 * |  AN10 | RA12 |  20 | U-V    | eAinchUV     | ������ ������� ���������� ��������� ���� V                       |
 *  ---------------------------------------------------------------------------------------------------------------
 */
// *** ������������ ���������� ������ � ������ ������������ ���.
// �������������� ������ ���������� ����������������. ��� 
// ������������������� ������ ����� ����������� � ������������, � ���������� ���������
// �������� � ����� ADCbuf � ������� ����������� �� �������. 
const uint8_t ADC_scan_ch [] = {
      AN0,                  // ���������� ����� AN0
      AN1,                  // ���������� ����� AN1
//    AN2,                  // ���������� ����� AN2
//    AN3,                  // ���������� ����� AN3
      AN4,                  // ���������� ����� AN4
      AN5,                  // ���������� ����� AN5
      AN6,                  // ���������� ����� AN6
//    AN7,                  // ���������� ����� AN7
//    AN8,                  // ���������� ����� AN8
      AN9,                  // ���������� ����� AN9
      AN10,                 // ���������� ����� AN10
//    AN11,                 // ���������� ����� AN11
//    AN12,                 // ���������� ����� AN12
//    AN13,                 // ���������� ����� AN13
//    AN14,                 // ���������� ����� AN14
//    AN15,                 // ���������� ����� AN15 
      AN16,                 // ���������� ����� AN16
      AN17,                 // ���������� ����� AN17
      AN18,                 // ���������� ����� AN18
//    AN19,                 // ���������� ����� AN19
      AN20,                 // ���������� ����� AN20
      AN21,                 // ���������� ����� AN21
//    AN22,                 // ���������� ����� AN22
//    AN23,                 // ���������� ����� AN23
//    AN24,                 // ���������� ����� AN24
//    AN25,                 // ���������� ����� AN25
//    AN26,                 // ���������� ����� AN26
//    AN27,                 // ���������� ����� AN27
//    AN28,                 // ���������� ����� AN28
//    AN29,                 // ���������� ����� AN29
//    AN30,                 // ���������� ����� AN30
//    AN31                  // ���������� ����� AN31
};


//*****************************************************************************
// ������������� ������ ���
void AdcDrv_ctor( )
{
    /* ������������� ������ ��� */
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


    //  ��������� � ��������� AD1CSSL � AD1CSSH ��������� ������������� ����������� ��� �������.
    // � ������� ADC_scan_chan[] ������ ����������� ��� ������� (������������ �������������).
    for(uint8_t i = 0;  i < sizeof (ADC_scan_ch);  i++){
    //  ���������� �� ������������� ����������� ��� ������� � �������� 0...15 ���������� � �������� AD�CSSL,
    // � �������� 16...31 ���������� � �������� AD�CSSH. ����� ��������� � ������������ - ���.1, �� ��������� - ���. 0.
        ADC_scan_ch[i] < 16 ? (AD1CSSL |= 1<<ADC_scan_ch[i]) : (AD1CSSH |= 1<<(ADC_scan_ch[i] - 16));
    }

    
    //  ������������� ����� �������� �������� ���������� (Vref+ � Vref-) ��� ���������� �����. ��� ����
    // ���������������� ��������, �.�. �� ������ � ������ ����������� �������.
    INIC_PIN_ANALOG(F,  9);
    INIC_PIN_ANALOG(F, 10);    

    
    // ������������� ����� ��� ����������� ��� ���������� �������.
    for(uint8_t i = 0;  i < sizeof (ADC_scan_ch);  i++){
        *ain_pin[i].TRIS = *ain_pin[i].TRIS | ain_pin[i].mask;                // ������������� �������� ����������� TRIS �� ����
        *ain_pin[i].ANS  = *ain_pin[i].ANS  | ain_pin[i].mask;                // ������������� �������� ����������� ANSEL � ����� ���������� ����    
    }
    
    // � ������� SMPI ������������ ����� �� ������� ������ ����� ����������� ��� �������
    AD1CON2bits.SMPI = sizeof(ADC_scan_ch) - 1;    


    /* ������������� ������ DMA */
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
    // �������� �� ���������� ���� ���������� DMA
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcErrorDMA, AdcDrv_isReady());
    
    AD1CON1bits.ADON = 0;
}


bool AdcDrv_isReady ( )
{
    return !DMA2CONbits.CHEN;
}

//*****************************************************************************
/**
* ���������:  
*    ������� ��������� �� ������������ ������ �� ������ 1.
*    ������ ��� ������������ �������� � ���������� INIC_ADC1_ADCSSLH INIC_ADC1_ADCSSL.
*    ������������ �����������  ����������.
*    ��� ������������ � �������������� ������ ��� ���� ������� 
*    ������������ ���������. ����������� ������� �������� ������������ �������,
*    ��� � ������ ������� ��������� ������������ �������� ��������. 
*/

/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   28-03-2016
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
* 
* ������ 1.0.2
* ����   15-03-2021
* �����  ������ �.�.
* 
* ���������:
*   ������� ��������� ��� ������������� DMA. ��� �������� � ������ ������������ �������.
*   ������ �������� � ��������� ADC_scan_ch.
*/
