/**
* \file    UnusedInterrupts.c
* \brief   ��������� �������������� ����������.
*
* \version 2.0.2
* \date    10-01-2020
* \author  ������ �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdint.h>
#include "typeMK.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"
#include "BlackBox.h"

//*****************************************************************************
// ���������� ���������� ����������
//*****************************************************************************

extern volatile uint16_t _TrapAddressLow;   // ����� ������������� ���������� � ���� ��������� (������� ����� ������, 2 �����)
extern volatile uint16_t _TrapAddressHigh;  // ����� ������������� ���������� � ���� ��������� (������� ����� ������, 1 ����)                            

//*****************************************************************************
// ������� ���� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ����������� ��������� ������� ��������������� ����������.
/// \param name - ��� ������� ��������� (����� �������� ���������� ����� �� 
///               ����� \a p33EP512GM710.gld);
/// \param id   - ����� ������� ���������� (Interrupt Vector Number) �������� 
///               ������� 6-1 ��������� "Section 6. Interrupts. (DS70600B).pdf".
///
#define FUNC( name, id ) void __attribute__((__interrupt__(__preprologue__("rcall _TrapAddress")), no_auto_psv)) _##name( void )\
{                                                                                                                               \
    INTCON1 = INTCON2 = INTCON3 = INTCON4 = 0;                                                                                  \
    ERROR_EX_ID( eGrPS_Main, ePS_MainUnusedInterrupt, _TrapAddressLow, _TrapAddressHigh, id, 0);                                \
}

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

FUNC( OscillatorFail, 0 )                      ///< Oscillator Fail Trap Vector.
FUNC( AddressError, 1 )                        ///< Address Error Trap Vector.
FUNC( HardTrapError, 2 )                       ///< Generic Hard Trap Vector.
FUNC( StackError, 3 )                          ///< Stack Error Trap Vector.
FUNC( MathError, 4 )                           ///< Math Error Trap Vector.
FUNC( DMACError, 5 )                           ///< DMA Controller Error Trap Vector.
FUNC( SoftTrapError, 6 )                       ///< Generic Soft Trap Vector.
// Vector #7 Reserved.
FUNC( INT0Interrupt, 8 )                       ///< INT0 � External Interrupt 0.
FUNC( IC1Interrupt, 9 )                        ///< IC1 � Input Capture 1.
FUNC( OC1Interrupt, 10 )                       ///< OC1 � Output Compare 1.
FUNC( T1Interrupt, 11 )                        ///< T1 � Timer1.
FUNC( DMA0Interrupt, 12 )                      ///< DMA0 � DMA Channel 0.
FUNC( IC2Interrupt, 13 )                       ///< IC2 � Input Capture 2.
FUNC( OC2Interrupt, 14 )                       ///< OC2 � Output Compare 2.
FUNC( T2Interrupt, 15 )                        ///< T2 � Timer2.
FUNC( T3Interrupt, 16 )                        ///< T3 � Timer3.
FUNC( SPI1ErrInterrupt, 17 )                   ///< SPI1E � SPI1 Error.
FUNC( SPI1Interrupt, 18 )                      ///< SPI1 � SPI1 Transfer Done.
FUNC( U1RXInterrupt, 19 )                      ///< U1RX � UART1 Receiver.
FUNC( U1TXInterrupt, 20 )                      ///< U1TX � UART1 Transmitter.
// Used Vector #21 AD1 � ADC1 Convert Done.
FUNC( DMA1Interrupt, 22 )                      ///< DMA1 � DMA Channel 1.
// Vector #23 Reserved.
FUNC( SI2C1Interrupt, 24 )                     ///< SI2C1 � I2C1 Slave Event.
FUNC( MI2C1Interrupt, 25 )                     ///< MI2C1 � I2C1 Master Event.
FUNC( CM1Interrupt, 26 )                       ///< CMP1 � Comparator Combined Event.
// Used Vector #27 CN � Input Change Interrupt.
FUNC( INT1Interrupt, 28 )                      ///< INT1 � External Interrupt 1.
FUNC( AD2Interrupt, 29 )                       ///< AD2 � ADC2 Convert Done.
FUNC( IC7Interrupt, 30 )                       ///< IC7 � Input Capture 7.
FUNC( IC8Interrupt, 31 )                       ///< IC8 � Input Capture 8.
FUNC( DMA2Interrupt, 32 )                      ///< DMA2 � DMA Channel 2.
FUNC( OC3Interrupt, 33 )                       ///< OC3 � Output Compare 3.
FUNC( OC4Interrupt, 34 )                       ///< OC4 � Output Compare 4.
// Used Vector #35 T4 � Timer4.
// Used Vector #36 T5 � Timer5.
FUNC( INT2Interrupt, 37 )                      ///< INT2 � External Interrupt 2.
FUNC( U2RXInterrupt, 38 )                      ///< U2RX � UART2 Receiver.
FUNC( U2TXInterrupt, 39 )                      ///< U2TX � UART2 Transmitter.
FUNC( SPI2ErrInterrupt, 40 )                   ///< SPI2E � SPI2 Error.
FUNC( SPI2Interrupt, 41 )                      ///< SPI2 � SPI2 Transfer Done.
FUNC( C1RxRdyInterrupt, 42 )                   ///< C1RX � CAN1 RX Data Ready.
FUNC( C1Interrupt, 43 )                        ///< C1 � CAN1 Event.
FUNC( DMA3Interrupt, 44 )                      ///< DMA3 � DMA Channel 3.
FUNC( IC3Interrupt, 45 )                       ///< IC3 � Input Capture 3.
FUNC( IC4Interrupt, 46 )                       ///< IC4 � Input Capture 4.
FUNC( IC5Interrupt, 47 )                       ///< IC5 � Input Capture 5.
FUNC( IC6Interrupt, 48 )                       ///< IC6 � Input Capture 6.
FUNC( OC5Interrupt, 49 )                       ///< OC5 � Output Compare 5.
FUNC( OC6Interrupt, 50 )                       ///< OC6 � Output Compare 6.
FUNC( OC7Interrupt, 51 )                       ///< OC7 � Output Compare 7.
FUNC( OC8Interrupt, 52 )                       ///< OC8 � Output Compare 8.
FUNC( PMPInterrupt, 53 )                       ///< PMP � Parallel Master Port.
// Vector #54 Reserved.
FUNC( T6Interrupt, 55 )                        ///< T6 � Timer6.
FUNC( T7Interrupt, 56 )                        ///< T7 � Timer7.
FUNC( SI2C2Interrupt, 57 )                     ///< SI2C2 � I2C2 Slave Event.
FUNC( MI2C2Interrupt, 58 )                     ///< MI2C2 � I2C2 Master Event.
FUNC( T8Interrupt, 59 )                        ///< T8 � Timer8.
FUNC( T9Interrupt, 60 )                        ///< T9 � Timer9.
FUNC( INT3Interrupt, 61 )                      ///< INT3 � External Interrupt 3.
FUNC( INT4Interrupt, 62 )                      ///< INT4 � External Interrupt 4.
FUNC( C2RxRdyInterrupt, 63 )                   ///< C2RX � CAN2 RX Data Ready.
FUNC( C2Interrupt, 64 )                        ///< C2 � CAN2 Event.
FUNC( PWMSpEventMatchInterrupt, 65 )           ///< PSEM � PCPWM Primary Event.
FUNC( QEI1Interrupt, 66 )                      ///< QEI1 � QEI1 Position Counter Compare.
FUNC( DCIEInterrupt, 67 )                      ///< DCIE � DCI Fault Interrupt. 
FUNC( DCIInterrupt, 68 )                       ///< DCI � DCI Transfer Done.
// Vector #69 Reserved.                        
FUNC( RTCCInterrupt, 70 )                      ///< RTCC � Real-Time Clock and Calendar.
// Vectors #71-72 Reserved.
FUNC( U1ErrInterrupt, 73 )                     ///< U1E � UART1 Error Interrupt.
// Used Vector #74 U2E � UART2 Error Interrupt.
// Used Vector #75 CRC � CRC Generator Interrupt.
// Vectors #76-77 Reserved.
FUNC( C1TxReqInterrupt, 78 )                   ///< C1TX � CAN1 TX Data Request.
FUNC( C2TxReqInterrupt, 79 )                   ///< C2TX � CAN2 TX Data Request.
// Vector #80 Reserved.
FUNC( PWMSecSpEventMatchInterrupt, 81 )        ///< PSESM � PCPWM Secondary Event.
// Vector #82 Reserved.
FUNC( QEI2Interrupt, 83 )                      ///< QEI2 � QEI2 Position Counter Compare.
// Vector #84 Reserved.
FUNC( CTMUInterrupt, 85 )                      ///< CTMU � CTMU Interrupt.
// Vectors #86-88 Reserved.
FUNC( U3ErrInterrupt, 89 )                     ///< U3E � UART3 Error Interrupt.
FUNC( U3RXInterrupt, 90 )                      ///< U3RX � UART3 Receiver.
FUNC( U3TXInterrupt, 91 )                      ///< U3TX � UART3 Transmitter.
// Vectors #92-94 Reserved.
FUNC( U4ErrInterrupt, 95 )                     ///< U4E � UART4 Error Interrupt.
FUNC( U4RXInterrupt, 96 )                      ///< U4RX � UART4 Receiver.
FUNC( U4TXInterrupt, 97 )                      ///< U4TX � UART4 Transmitter.
FUNC( SPI3ErrInterrupt, 98 )                   ///< SPI3E � SPI3 Error.
FUNC( SPI3Interrupt, 99 )                      ///< SPI3 � SPI3 Transfer Done.
// Vectors #100-101 Reserved.
FUNC( PWM1Interrupt, 102 )                     ///< PWM1 � PWM Generator 1.
FUNC( PWM2Interrupt, 103 )                     ///< PWM2 � PWM Generator 2.
FUNC( PWM3Interrupt, 104 )                     ///< PWM3 � PWM Generator 3.
FUNC( PWM4Interrupt, 105 )                     ///< PWM4 � PWM Generator 4.
FUNC( PWM5Interrupt, 106 )                     ///< PWM5 � PWM Generator 5.
FUNC( PWM6Interrupt, 107 )                     ///< PWM6 � PWM Generator 6.
// Vectors #108-149 Reserved.
FUNC( ICDInterrupt, 150 )                      ///< ICD � ICD Application.
FUNC( JTAGInterrupt, 151 )                     ///< JTAG � JTAG Programming.
// Vector #152 Reserved.
FUNC( PTGSTEPInterrupt, 153 )                  ///< PTGSTEP � PTG Step.
FUNC( PTGWDTInterrupt, 154 )                   ///< PTGWDT � PTG Watchdog Time-out.
FUNC( PTG0Interrupt, 155 )                     ///< PTG0 � PTG Interrupt 0.
FUNC( PTG1Interrupt, 156 )                     ///< PTG1 � PTG Interrupt 1.
FUNC( PTG2Interrupt, 157 )                     ///< PTG2 � PTG Interrupt 2.
FUNC( PTG3Interrupt, 158 )                     ///< PTG3 � PTG Interrupt 3.
///< Vectors #159-245 Reserved.

        
        
        
//*****************************************************************************
asm("; GLOBAL: _TrapAddress \n\t"
    "; \n\t"
    "  .section *, bss, near \n\t"
    "  .global __TrapAddressLow, __TrapAddressHigh \n\t"
    "__TrapAddressLow:    .space 2                 ; ������� ����� PC (PC[15:0]) \n\t"           
    "__TrapAddressHigh:   .space 2                 ; ������� ����� PC (PC[22:16]) \n\t"
    "  .text \n\t"
    "  .global _TrapAddress \n\t"
    "_TrapAddress: \n\t"
    "  push.d W0 \n\t"
    "  sub W15,#12,W1 \n\t"
    "  mov [W1++], W0                              ; ��������� ������� ����� (PC[15:0]) \n\t"
    "  mov W0, __TrapAddressLow		               ; ��������� ������� ����� (PC[15:0]) \n\t"   
    "  mov [W1], W0                                ; ��������� ������� ����� (PC[22:16]) \n\t"
    "  and.w #0x007F,W0                            ; ��������� ������ PC[22:16] \n\t"
    "  mov W0, __TrapAddressHigh                   ; ��������� ������� ����� (PC[22:16]) \n\t"
    "  pop.d W0 \n\t"
    "  return \n\t");
        
        
//*****************************************************************************
/**
* ������� ���������:
*  
* ������ 1.0.1
* ����   23-03-2016
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
* 
* ������ 2.0.1
* ����   23-03-2017
* �����  ��������� �.�.
* 
* ���������:
*    ��������� ��� ����� ����� �� dsPIC33EP.
* 
* ������ 2.0.2 
* ����   10-01-2020
* �����  ������ �.�.
*
* ���������:
*    ��������� ���������� � ���������� ������ ������������� ��������������� ����������.
*/
