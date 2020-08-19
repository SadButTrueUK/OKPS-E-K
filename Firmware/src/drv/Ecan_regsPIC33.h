/**
* \file    Ecan_regsPIC33.h
* \brief   Порты модуля ECAN
*
* \version 1.0.1
* \date    16-03-2017
* \author  Третьяков В.Ж.
*/

//*****************************************************************************
// Команды препроцессора для предотвращения повторного включения содержимого файла
//*****************************************************************************
#ifndef ECAN_REGSPIC33_H
#define ECAN_REGSPIC33_H

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "typeMK.h"

//*****************************************************************************
// Определение типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Структура портов модуля \a ECAN.
///
typedef struct
{
    /// \brief Регистр CxCTRL1.
    ///
    union
    {
        unsigned short  CTRL1;              ///< Регистр CxCTRL1 в формате unsigned short.
        C1CTRL1BITS     CTRL1bits;          ///< Регистр CxCTRL1 в формате для битового доступа.
    };

    /// \brief Регистр CxCTRL2.
    ///
    union 
    {
        unsigned short  CTRL2;              ///< Регистр CxCTRL2 в формате unsigned short.
        C1CTRL2BITS     CTRL2bits;          ///< Регистр CxCTRL2 в формате для битового доступа.
    };

    /// \brief Регистр CxVEC.
    ///
    union 
    {
        unsigned short  VEC;              ///< Регистр CxVEC в формате unsigned short.
        C1VECBITS       VECbits;          ///< Регистр CxVEC в формате для битового доступа.
    };

    /// \brief Регистр CxFCTRL.
    ///
    union 
    {
        unsigned short  FCTRL;              ///< Регистр CxFCTRL в формате unsigned short.
        C1FCTRLBITS     FCTRLbits;          ///< Регистр CxFCTRL в формате для битового доступа.
    };

    /// \brief Регистр CxFIFO.
    ///
    union 
    {
        unsigned short  FIFO;              ///< Регистр CxFIFO в формате unsigned short.
        C1FIFOBITS      FIFObits;          ///< Регистр CxFIFO в формате для битового доступа.
    };

    /// \brief Регистр CxINTF.
    ///
    union 
    {
        unsigned short  INTF;              ///< Регистр CxINTF в формате unsigned short.
        C1INTFBITS      INTFbits;          ///< Регистр CxINTF в формате для битового доступа.
    }; 

    /// \brief Регистр CxINTE.
    ///
    union 
    {
        unsigned short  INTE;              ///< Регистр CxINTE в формате unsigned short.
        C1INTEBITS      INTEbits;          ///< Регистр CxINTE в формате для битового доступа.
    }; 

    /// \brief Регистр CxEC.
    ///
    union 
    {
        unsigned short  EC;              ///< Регистр CxEC в формате unsigned short.
        C1ECBITS        ECbits;          ///< Регистр CxEC в формате для битового доступа.
    }; 

    /// \brief Регистр CxCFG1.
    ///
    union 
    {
        unsigned short  CFG1;              ///< Регистр CxCFG1 в формате unsigned short.
        C1CFG1BITS      CFG1bits;          ///< Регистр CxCFG1 в формате для битового доступа.
    };
    
    /// \brief Регистр CxCFG2.
    ///
    union 
    {
        unsigned short  CFG2;              ///< Регистр CxCFG2 в формате unsigned short.
        C1CFG2BITS      CFG2bits;          ///< Регистр CxCFG2 в формате для битового доступа.
    };

    /// \brief Регистр CxFEN1.
    ///
    union 
    {
        unsigned short  FEN1;              ///< Регистр CxFEN1 в формате unsigned short.
        C1FEN1BITS      FEN1bits;          ///< Регистр CxFEN1 в формате для битового доступа.
    };

    unsigned short aNoUseRegs1;             ///< Неиспользуемый регистр.

    /// \brief Регистр CxFMSKSEL1.
    ///
    union 
    {
        unsigned short  FMSKSEL1;              ///< Регистр CxFMSKSEL1 в формате unsigned short.
        C1FMSKSEL1BITS  FMSKSEL1bits;          ///< Регистр CxFMSKSEL1 в формате для битового доступа.
    };
    
    /// \brief Регистр CxFMSKSEL2.
    ///
    union 
    {
        unsigned short  FMSKSEL2;              ///< Регистр CxFMSKSEL2 в формате unsigned short.
        C1FMSKSEL2BITS  FMSKSEL2bits;          ///< Регистр CxFMSKSEL2 в формате для битового доступа.
    };
  
    unsigned short aNoUseRegs2[2];             ///< Неиспользуемая область.

    /// \brief Регистр CxRXFUL1  CxBUFPNT1.
    ///
    union 
    {
        unsigned short  RXFUL1;              ///< Регистр CxRXFUL1 в формате unsigned short.
        C1RXFUL1BITS    RXFUL1bits;          ///< Регистр CxRXFUL1 в формате для битового доступа.
        unsigned short  BUFPNT1;             ///< Регистр CxBUFPNT1 в формате unsigned short.
        C1BUFPNT1BITS   BUFPNT1bits;         ///< Регистр CxBUFPNT1 в формате для битового доступа.
    }; 

    /// \brief Регистр CxRXFUL2.
    ///
    union 
    {
        unsigned short  RXFUL2;              ///< Регистр CxRXFUL2 в формате unsigned short.
        C1RXFUL2BITS    RXFUL2bits;          ///< Регистр CxRXFUL2 в формате для битового доступа.
    }; 

    unsigned short aNoUseRegs3[2];             ///< Неиспользуемая область.

    /// \brief Регистр CxRXOVF1.
    ///
    union 
    {
        unsigned short  RXOVF1;              ///< Регистр CxRXOVF1 в формате unsigned short.
        C1RXOVF1BITS    RXOVF1bits;          ///< Регистр CxRXOVF1 в формате для битового доступа.
    }; 
    
    /// \brief Регистр CxRXOVF2.
    ///
    union 
    {
        unsigned short  RXOVF2;              ///< Регистр CxRXOVF2 в формате unsigned short.
        C1RXOVF2BITS    RXOVF2bits;          ///< Регистр CxRXOVF2 в формате для битового доступа.
    }; 

    unsigned short aNoUseRegs4[2];           ///< Неиспользуемая область.

    /// \brief Регистр CxTR01CON CxRXM0SID.
    ///
    union 
    {
        unsigned short  TR01CON;              ///< Регистр CxTR01CON в формате unsigned short.
        C1TR01CONBITS   TR01CONbits;          ///< Регистр CxTR01CON в формате для битового доступа  (WIN = 0).
        unsigned short  RXM0SID;              ///< Регистр CxRXM0SID в формате unsigned short.
        C1RXM0SIDBITS   RXM0SIDbits;          ///< Регистр CxRXM0SID в формате для битового доступа (WIN = 1).
    }; 

    unsigned short aNoUseRegs5[7];           ///< Неиспользуемая область.

    /// \brief Регистр CxRXF0SID.
    ///
    union 
    {
        unsigned short  RXF0SID;              ///< Регистр CxRXF0SID в формате unsigned short.
        C1RXF0SIDBITS   RXF0SIDbits;          ///< Регистр CxRXF0SID в формате для битового доступа (WIN = 1).
    }; 

} dsPIC33_CanRegs;

//*****************************************************************************
// Определение глобальных переменных
//*****************************************************************************

//*****************************************************************************
/// \brief SFR blocks for each CAN1 module.
/// 
extern volatile dsPIC33_CanRegs dsPIC33_Can1Regs __attribute__ ( ( sfr( 0x0400 ) ) );

//*****************************************************************************
/// \brief SFR blocks for each CAN2 module.
/// 
extern volatile dsPIC33_CanRegs dsPIC33_Can2Regs __attribute__ ( ( sfr( 0x0500 ) ) );

#endif

//*****************************************************************************
/**
* История изменений: 
*
* Версия 1.0.1
* Дата   16-03-2017
* Автор  Третьяков В.Ж.
*
* Изменения:
*    Базовая версия.
*/
