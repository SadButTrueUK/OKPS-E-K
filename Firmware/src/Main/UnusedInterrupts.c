/**
* \file    UnusedInterrupts.c
* \brief   ќбработка неиспользуемых прерываний.
*
* \version 2.0.2
* \date    10-01-2020
* \author  јгулов ћ.ј.
*/

//*****************************************************************************
// ѕодключаемые файлы
//*****************************************************************************
#include <stdint.h>
#include "typeMK.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"
#include "BlackBox.h"

//*****************************************************************************
// ќбъ€вление глобальных переменных
//*****************************************************************************

extern volatile uint16_t _TrapAddressLow;   // јдрес возникновени€ прерывани€ в коде программы (младшее слово адреса, 2 байта)
extern volatile uint16_t _TrapAddressHigh;  // јдрес возникновени€ прерывани€ в коде программы (старшее слово адреса, 1 байт)                            

//*****************************************************************************
// ћакросы типа функци€
//*****************************************************************************

//*****************************************************************************
/// \brief ќпределение обработки вектора неиспользуемого прерывани€.
/// \param name - им€ вектора обработки (имена векторов прерываний вз€ты из 
///               файла \a p33EP512GM710.gld);
/// \param id   - номер вектора прерывани€ (Interrupt Vector Number) согласно 
///               “аблице 6-1 документа "Section 6. Interrupts. (DS70600B).pdf".
///
#define FUNC( name, id ) void __attribute__((__interrupt__(__preprologue__("rcall _TrapAddress")), no_auto_psv)) _##name( void )\
{                                                                                                                               \
    INTCON1 = INTCON2 = INTCON3 = INTCON4 = 0;                                                                                  \
    ERROR_EX_ID( eGrPS_Main, ePS_MainUnusedInterrupt, _TrapAddressLow, _TrapAddressHigh, id, 0);                                \
}

//*****************************************************************************
// –еализаци€ локальных функций
//*****************************************************************************

FUNC( OscillatorFail, 0 )                      ///< Oscillator Fail Trap Vector.
FUNC( AddressError, 1 )                        ///< Address Error Trap Vector.
FUNC( HardTrapError, 2 )                       ///< Generic Hard Trap Vector.
FUNC( StackError, 3 )                          ///< Stack Error Trap Vector.
FUNC( MathError, 4 )                           ///< Math Error Trap Vector.
FUNC( DMACError, 5 )                           ///< DMA Controller Error Trap Vector.
FUNC( SoftTrapError, 6 )                       ///< Generic Soft Trap Vector.
// Vector #7 Reserved.
FUNC( INT0Interrupt, 8 )                       ///< INT0 Ц External Interrupt 0.
FUNC( IC1Interrupt, 9 )                        ///< IC1 Ц Input Capture 1.
FUNC( OC1Interrupt, 10 )                       ///< OC1 Ц Output Compare 1.
FUNC( T1Interrupt, 11 )                        ///< T1 Ц Timer1.
FUNC( DMA0Interrupt, 12 )                      ///< DMA0 Ц DMA Channel 0.
FUNC( IC2Interrupt, 13 )                       ///< IC2 Ц Input Capture 2.
FUNC( OC2Interrupt, 14 )                       ///< OC2 Ц Output Compare 2.
FUNC( T2Interrupt, 15 )                        ///< T2 Ц Timer2.
FUNC( T3Interrupt, 16 )                        ///< T3 Ц Timer3.
FUNC( SPI1ErrInterrupt, 17 )                   ///< SPI1E Ц SPI1 Error.
FUNC( SPI1Interrupt, 18 )                      ///< SPI1 Ц SPI1 Transfer Done.
FUNC( U1RXInterrupt, 19 )                      ///< U1RX Ц UART1 Receiver.
FUNC( U1TXInterrupt, 20 )                      ///< U1TX Ц UART1 Transmitter.
// Used Vector #21 AD1 Ц ADC1 Convert Done.
FUNC( DMA1Interrupt, 22 )                      ///< DMA1 Ц DMA Channel 1.
// Vector #23 Reserved.
FUNC( SI2C1Interrupt, 24 )                     ///< SI2C1 Ц I2C1 Slave Event.
FUNC( MI2C1Interrupt, 25 )                     ///< MI2C1 Ц I2C1 Master Event.
FUNC( CM1Interrupt, 26 )                       ///< CMP1 Ц Comparator Combined Event.
// Used Vector #27 CN Ц Input Change Interrupt.
FUNC( INT1Interrupt, 28 )                      ///< INT1 Ц External Interrupt 1.
FUNC( AD2Interrupt, 29 )                       ///< AD2 Ц ADC2 Convert Done.
FUNC( IC7Interrupt, 30 )                       ///< IC7 Ц Input Capture 7.
FUNC( IC8Interrupt, 31 )                       ///< IC8 Ц Input Capture 8.
FUNC( DMA2Interrupt, 32 )                      ///< DMA2 Ц DMA Channel 2.
FUNC( OC3Interrupt, 33 )                       ///< OC3 Ц Output Compare 3.
FUNC( OC4Interrupt, 34 )                       ///< OC4 Ц Output Compare 4.
// Used Vector #35 T4 Ц Timer4.
// Used Vector #36 T5 Ц Timer5.
FUNC( INT2Interrupt, 37 )                      ///< INT2 Ц External Interrupt 2.
FUNC( U2RXInterrupt, 38 )                      ///< U2RX Ц UART2 Receiver.
FUNC( U2TXInterrupt, 39 )                      ///< U2TX Ц UART2 Transmitter.
FUNC( SPI2ErrInterrupt, 40 )                   ///< SPI2E Ц SPI2 Error.
FUNC( SPI2Interrupt, 41 )                      ///< SPI2 Ц SPI2 Transfer Done.
FUNC( C1RxRdyInterrupt, 42 )                   ///< C1RX Ц CAN1 RX Data Ready.
FUNC( C1Interrupt, 43 )                        ///< C1 Ц CAN1 Event.
FUNC( DMA3Interrupt, 44 )                      ///< DMA3 Ц DMA Channel 3.
FUNC( IC3Interrupt, 45 )                       ///< IC3 Ц Input Capture 3.
FUNC( IC4Interrupt, 46 )                       ///< IC4 Ц Input Capture 4.
FUNC( IC5Interrupt, 47 )                       ///< IC5 Ц Input Capture 5.
FUNC( IC6Interrupt, 48 )                       ///< IC6 Ц Input Capture 6.
FUNC( OC5Interrupt, 49 )                       ///< OC5 Ц Output Compare 5.
FUNC( OC6Interrupt, 50 )                       ///< OC6 Ц Output Compare 6.
FUNC( OC7Interrupt, 51 )                       ///< OC7 Ц Output Compare 7.
FUNC( OC8Interrupt, 52 )                       ///< OC8 Ц Output Compare 8.
FUNC( PMPInterrupt, 53 )                       ///< PMP Ц Parallel Master Port.
// Vector #54 Reserved.
FUNC( T6Interrupt, 55 )                        ///< T6 Ц Timer6.
FUNC( T7Interrupt, 56 )                        ///< T7 Ц Timer7.
FUNC( SI2C2Interrupt, 57 )                     ///< SI2C2 Ц I2C2 Slave Event.
FUNC( MI2C2Interrupt, 58 )                     ///< MI2C2 Ц I2C2 Master Event.
FUNC( T8Interrupt, 59 )                        ///< T8 Ц Timer8.
FUNC( T9Interrupt, 60 )                        ///< T9 Ц Timer9.
FUNC( INT3Interrupt, 61 )                      ///< INT3 Ц External Interrupt 3.
FUNC( INT4Interrupt, 62 )                      ///< INT4 Ц External Interrupt 4.
FUNC( C2RxRdyInterrupt, 63 )                   ///< C2RX Ц CAN2 RX Data Ready.
FUNC( C2Interrupt, 64 )                        ///< C2 Ц CAN2 Event.
FUNC( PWMSpEventMatchInterrupt, 65 )           ///< PSEM Ц PCPWM Primary Event.
FUNC( QEI1Interrupt, 66 )                      ///< QEI1 Ц QEI1 Position Counter Compare.
FUNC( DCIEInterrupt, 67 )                      ///< DCIE Ц DCI Fault Interrupt. 
FUNC( DCIInterrupt, 68 )                       ///< DCI Ц DCI Transfer Done.
// Vector #69 Reserved.                        
FUNC( RTCCInterrupt, 70 )                      ///< RTCC Ц Real-Time Clock and Calendar.
// Vectors #71-72 Reserved.
FUNC( U1ErrInterrupt, 73 )                     ///< U1E Ц UART1 Error Interrupt.
// Used Vector #74 U2E Ц UART2 Error Interrupt.
// Used Vector #75 CRC Ц CRC Generator Interrupt.
// Vectors #76-77 Reserved.
FUNC( C1TxReqInterrupt, 78 )                   ///< C1TX Ц CAN1 TX Data Request.
FUNC( C2TxReqInterrupt, 79 )                   ///< C2TX Ц CAN2 TX Data Request.
// Vector #80 Reserved.
FUNC( PWMSecSpEventMatchInterrupt, 81 )        ///< PSESM Ц PCPWM Secondary Event.
// Vector #82 Reserved.
FUNC( QEI2Interrupt, 83 )                      ///< QEI2 Ц QEI2 Position Counter Compare.
// Vector #84 Reserved.
FUNC( CTMUInterrupt, 85 )                      ///< CTMU Ц CTMU Interrupt.
// Vectors #86-88 Reserved.
FUNC( U3ErrInterrupt, 89 )                     ///< U3E Ц UART3 Error Interrupt.
FUNC( U3RXInterrupt, 90 )                      ///< U3RX Ц UART3 Receiver.
FUNC( U3TXInterrupt, 91 )                      ///< U3TX Ц UART3 Transmitter.
// Vectors #92-94 Reserved.
FUNC( U4ErrInterrupt, 95 )                     ///< U4E Ц UART4 Error Interrupt.
FUNC( U4RXInterrupt, 96 )                      ///< U4RX Ц UART4 Receiver.
FUNC( U4TXInterrupt, 97 )                      ///< U4TX Ц UART4 Transmitter.
FUNC( SPI3ErrInterrupt, 98 )                   ///< SPI3E Ц SPI3 Error.
FUNC( SPI3Interrupt, 99 )                      ///< SPI3 Ц SPI3 Transfer Done.
// Vectors #100-101 Reserved.
FUNC( PWM1Interrupt, 102 )                     ///< PWM1 Ц PWM Generator 1.
FUNC( PWM2Interrupt, 103 )                     ///< PWM2 Ц PWM Generator 2.
FUNC( PWM3Interrupt, 104 )                     ///< PWM3 Ц PWM Generator 3.
FUNC( PWM4Interrupt, 105 )                     ///< PWM4 Ц PWM Generator 4.
FUNC( PWM5Interrupt, 106 )                     ///< PWM5 Ц PWM Generator 5.
FUNC( PWM6Interrupt, 107 )                     ///< PWM6 Ц PWM Generator 6.
// Vectors #108-149 Reserved.
FUNC( ICDInterrupt, 150 )                      ///< ICD Ц ICD Application.
FUNC( JTAGInterrupt, 151 )                     ///< JTAG Ц JTAG Programming.
// Vector #152 Reserved.
FUNC( PTGSTEPInterrupt, 153 )                  ///< PTGSTEP Ц PTG Step.
FUNC( PTGWDTInterrupt, 154 )                   ///< PTGWDT Ц PTG Watchdog Time-out.
FUNC( PTG0Interrupt, 155 )                     ///< PTG0 Ц PTG Interrupt 0.
FUNC( PTG1Interrupt, 156 )                     ///< PTG1 Ц PTG Interrupt 1.
FUNC( PTG2Interrupt, 157 )                     ///< PTG2 Ц PTG Interrupt 2.
FUNC( PTG3Interrupt, 158 )                     ///< PTG3 Ц PTG Interrupt 3.
///< Vectors #159-245 Reserved.

        
        
        
//*****************************************************************************
asm("; GLOBAL: _TrapAddress \n\t"
    "; \n\t"
    "  .section *, bss, near \n\t"
    "  .global __TrapAddressLow, __TrapAddressHigh \n\t"
    "__TrapAddressLow:    .space 2                 ; ћладша€ часть PC (PC[15:0]) \n\t"           
    "__TrapAddressHigh:   .space 2                 ; —тарша€ часть PC (PC[22:16]) \n\t"
    "  .text \n\t"
    "  .global _TrapAddress \n\t"
    "_TrapAddress: \n\t"
    "  push.d W0 \n\t"
    "  sub W15,#12,W1 \n\t"
    "  mov [W1++], W0                              ; »звлекаем младшее слово (PC[15:0]) \n\t"
    "  mov W0, __TrapAddressLow		               ; —охран€ем младшее слово (PC[15:0]) \n\t"   
    "  mov [W1], W0                                ; »звлекаем старшее слово (PC[22:16]) \n\t"
    "  and.w #0x007F,W0                            ; ќставл€ем только PC[22:16] \n\t"
    "  mov W0, __TrapAddressHigh                   ; —охран€ем старшее слово (PC[22:16]) \n\t"
    "  pop.d W0 \n\t"
    "  return \n\t");
        
        
//*****************************************************************************
/**
* »стори€ изменений:
*  
* ¬ерси€ 1.0.1
* ƒата   23-03-2016
* јвтор  “реть€ков ¬.∆.
*
* »зменени€:
*    Ѕазова€ верси€.
* 
* ¬ерси€ 2.0.1
* ƒата   23-03-2017
* јвтор  “реть€ков ¬.∆.
* 
* »зменени€:
*    »зменени€ дл€ новой схемы на dsPIC33EP.
* 
* ¬ерси€ 2.0.2 
* ƒата   10-01-2020
* јвтор  јгулов ћ.ј.
*
* »зменени€:
*    ƒобавлено сохранение в параметрах адреса возникновени€ неиспользуемого прерывани€.
*/
