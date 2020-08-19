/**
* \file    Ecan_regsPIC33.h
* \brief   ����� ������ ECAN
*
* \version 1.0.1
* \date    16-03-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef ECAN_REGSPIC33_H
#define ECAN_REGSPIC33_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "typeMK.h"

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ������ ������ \a ECAN.
///
typedef struct
{
    /// \brief ������� CxCTRL1.
    ///
    union
    {
        unsigned short  CTRL1;              ///< ������� CxCTRL1 � ������� unsigned short.
        C1CTRL1BITS     CTRL1bits;          ///< ������� CxCTRL1 � ������� ��� �������� �������.
    };

    /// \brief ������� CxCTRL2.
    ///
    union 
    {
        unsigned short  CTRL2;              ///< ������� CxCTRL2 � ������� unsigned short.
        C1CTRL2BITS     CTRL2bits;          ///< ������� CxCTRL2 � ������� ��� �������� �������.
    };

    /// \brief ������� CxVEC.
    ///
    union 
    {
        unsigned short  VEC;              ///< ������� CxVEC � ������� unsigned short.
        C1VECBITS       VECbits;          ///< ������� CxVEC � ������� ��� �������� �������.
    };

    /// \brief ������� CxFCTRL.
    ///
    union 
    {
        unsigned short  FCTRL;              ///< ������� CxFCTRL � ������� unsigned short.
        C1FCTRLBITS     FCTRLbits;          ///< ������� CxFCTRL � ������� ��� �������� �������.
    };

    /// \brief ������� CxFIFO.
    ///
    union 
    {
        unsigned short  FIFO;              ///< ������� CxFIFO � ������� unsigned short.
        C1FIFOBITS      FIFObits;          ///< ������� CxFIFO � ������� ��� �������� �������.
    };

    /// \brief ������� CxINTF.
    ///
    union 
    {
        unsigned short  INTF;              ///< ������� CxINTF � ������� unsigned short.
        C1INTFBITS      INTFbits;          ///< ������� CxINTF � ������� ��� �������� �������.
    }; 

    /// \brief ������� CxINTE.
    ///
    union 
    {
        unsigned short  INTE;              ///< ������� CxINTE � ������� unsigned short.
        C1INTEBITS      INTEbits;          ///< ������� CxINTE � ������� ��� �������� �������.
    }; 

    /// \brief ������� CxEC.
    ///
    union 
    {
        unsigned short  EC;              ///< ������� CxEC � ������� unsigned short.
        C1ECBITS        ECbits;          ///< ������� CxEC � ������� ��� �������� �������.
    }; 

    /// \brief ������� CxCFG1.
    ///
    union 
    {
        unsigned short  CFG1;              ///< ������� CxCFG1 � ������� unsigned short.
        C1CFG1BITS      CFG1bits;          ///< ������� CxCFG1 � ������� ��� �������� �������.
    };
    
    /// \brief ������� CxCFG2.
    ///
    union 
    {
        unsigned short  CFG2;              ///< ������� CxCFG2 � ������� unsigned short.
        C1CFG2BITS      CFG2bits;          ///< ������� CxCFG2 � ������� ��� �������� �������.
    };

    /// \brief ������� CxFEN1.
    ///
    union 
    {
        unsigned short  FEN1;              ///< ������� CxFEN1 � ������� unsigned short.
        C1FEN1BITS      FEN1bits;          ///< ������� CxFEN1 � ������� ��� �������� �������.
    };

    unsigned short aNoUseRegs1;             ///< �������������� �������.

    /// \brief ������� CxFMSKSEL1.
    ///
    union 
    {
        unsigned short  FMSKSEL1;              ///< ������� CxFMSKSEL1 � ������� unsigned short.
        C1FMSKSEL1BITS  FMSKSEL1bits;          ///< ������� CxFMSKSEL1 � ������� ��� �������� �������.
    };
    
    /// \brief ������� CxFMSKSEL2.
    ///
    union 
    {
        unsigned short  FMSKSEL2;              ///< ������� CxFMSKSEL2 � ������� unsigned short.
        C1FMSKSEL2BITS  FMSKSEL2bits;          ///< ������� CxFMSKSEL2 � ������� ��� �������� �������.
    };
  
    unsigned short aNoUseRegs2[2];             ///< �������������� �������.

    /// \brief ������� CxRXFUL1  CxBUFPNT1.
    ///
    union 
    {
        unsigned short  RXFUL1;              ///< ������� CxRXFUL1 � ������� unsigned short.
        C1RXFUL1BITS    RXFUL1bits;          ///< ������� CxRXFUL1 � ������� ��� �������� �������.
        unsigned short  BUFPNT1;             ///< ������� CxBUFPNT1 � ������� unsigned short.
        C1BUFPNT1BITS   BUFPNT1bits;         ///< ������� CxBUFPNT1 � ������� ��� �������� �������.
    }; 

    /// \brief ������� CxRXFUL2.
    ///
    union 
    {
        unsigned short  RXFUL2;              ///< ������� CxRXFUL2 � ������� unsigned short.
        C1RXFUL2BITS    RXFUL2bits;          ///< ������� CxRXFUL2 � ������� ��� �������� �������.
    }; 

    unsigned short aNoUseRegs3[2];             ///< �������������� �������.

    /// \brief ������� CxRXOVF1.
    ///
    union 
    {
        unsigned short  RXOVF1;              ///< ������� CxRXOVF1 � ������� unsigned short.
        C1RXOVF1BITS    RXOVF1bits;          ///< ������� CxRXOVF1 � ������� ��� �������� �������.
    }; 
    
    /// \brief ������� CxRXOVF2.
    ///
    union 
    {
        unsigned short  RXOVF2;              ///< ������� CxRXOVF2 � ������� unsigned short.
        C1RXOVF2BITS    RXOVF2bits;          ///< ������� CxRXOVF2 � ������� ��� �������� �������.
    }; 

    unsigned short aNoUseRegs4[2];           ///< �������������� �������.

    /// \brief ������� CxTR01CON CxRXM0SID.
    ///
    union 
    {
        unsigned short  TR01CON;              ///< ������� CxTR01CON � ������� unsigned short.
        C1TR01CONBITS   TR01CONbits;          ///< ������� CxTR01CON � ������� ��� �������� �������  (WIN = 0).
        unsigned short  RXM0SID;              ///< ������� CxRXM0SID � ������� unsigned short.
        C1RXM0SIDBITS   RXM0SIDbits;          ///< ������� CxRXM0SID � ������� ��� �������� ������� (WIN = 1).
    }; 

    unsigned short aNoUseRegs5[7];           ///< �������������� �������.

    /// \brief ������� CxRXF0SID.
    ///
    union 
    {
        unsigned short  RXF0SID;              ///< ������� CxRXF0SID � ������� unsigned short.
        C1RXF0SIDBITS   RXF0SIDbits;          ///< ������� CxRXF0SID � ������� ��� �������� ������� (WIN = 1).
    }; 

} dsPIC33_CanRegs;

//*****************************************************************************
// ����������� ���������� ����������
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
* ������� ���������: 
*
* ������ 1.0.1
* ����   16-03-2017
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
*/
