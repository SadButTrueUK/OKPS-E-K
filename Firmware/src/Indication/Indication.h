/**
* \file    Indication.h
* \brief   ���������� ����������
* \details ���������
*
* \version 1.0.1
* \date    24-02-2017
* \author  ���������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef INDICATION_H
#define INDICATION_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdbool.h>
#include <stdint.h>

//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ����.
///
typedef enum 
{
    eIndPhNormal = 0,   ///< ���������� ��������� ����
    eIndPhInvert        ///< ��������������� ��������� ����
} IndOutPhase;

//*****************************************************************************
/// \brief ������ ������ ��������� ����������.
///
typedef enum 
{
    eOffState = 0,      ///< ��������� ��������
    eOnState,           ///< ��������� �������
    ePulseOnState,      ///< ��������� ������� ���������� �� �������� �����
    ePulseOffState,     ///< ��������� �������� ���������� �� �������� �����
    eBlinkPrdState,     ///< ��������� ������ ����������
    eBlinkTwiceState    ///< ��������� ������ ���������� � ������� ��������
} IndOutSetting;

//*****************************************************************************
/// \brief ��� ���������� �����������.
///
typedef enum 
{
    eCtrEnHigh = 0, ///< ��������� ���������� ���������� ��������
    eCtrEnLow       ///< ��������� ���������� ���������� �����
} IndCtrType;

//*****************************************************************************
/// \brief ��� - ��������� ����� ����� �����-������ ����������.
///
typedef struct 
{
    volatile uint16_t *trisPort;                   ///< ��������� �� ������� ��������� ����������� ������.
    volatile uint16_t *pinPort;                    ///< ��������� �� ����� �����.
    uint16_t          pinNum;                      ///< ����� ����� �����.
} indPortSet_type;

//*****************************************************************************
/// \brief ��������� ������� ����������.
///
typedef struct 
{
    /// \brief ���� � ���������� �������: (true (������ �������� ���������), false (������ �������� ��������������)).
    ///
    bool isDummy;
    
    indPortSet_type outPin;         ///< ���������� ���� ��������� ����� ����� �����-������ ����������.
    IndCtrType      outCtrType;     ///< ���������� ���� ���������� �����������.
    IndOutSetting   outState;       ///< ���������� ���� ���� ��������� ����������.
    IndOutPhase     outPhase;       ///< ���������� ���� ��������� ���� (������������ ��� ����������� ��������).
    uint16_t        tOn1;           ///< ������� ����������� ��������� 1-�� �������.
    uint16_t        tOff1;          ///< ������� ������������ ��������� 1-�� �������.
    uint16_t        tOn2;           ///< ������� ����������� ��������� 2-�� �������.
    uint16_t        tOff2;          ///< ������� ������������ ��������� 2-�� �������.
    uint16_t        cntOn1;         ///< ������� ����������� ��������� 1-�� �������.
    uint16_t        cntOff1;        ///< ������� ������������ ��������� 1-�� �������.
    uint16_t        cntOn2;         ///< ������� ����������� ��������� 2-�� �������.
    uint16_t        cntOff2;        ///< ������� ������������ ��������� 2-�� �������.
} indOutput_type;

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������������� ����������� ������� ����������.
/// \param h - ��������� �� ������ ���������� � ��� ������� �������;
/// \param trisOut - ��������� �� ������� ��������� ����������� ������;
/// \param portOut - ��������� �� ����� �����;
/// \param pinOut - ����� ����� �����;
/// \param ctrType - ��� ���������� �����������.
///
void Indication_ctorLed( indOutput_type *h, volatile uint16_t *trisOut,
        volatile uint16_t *portOut, uint16_t pinOut, IndCtrType ctrType );

//*****************************************************************************
/// \brief ������������� ����������� ������� ���������� ��� ��������.
/// \param h - ��������� �� ������ ���������� � ��� ������� �������.
///
void Indication_ctorDummy( indOutput_type *h );

//*****************************************************************************
/// \brief ��������� ����������.
/// \param h - ��������� �� ������ ���������� � ��� ������� �������.
///
void Indication_on( indOutput_type *h );

//*****************************************************************************
/// \brief ���������� ����������.
/// \param h - ��������� �� ������ ���������� � ��� ������� �������.
///
void Indication_off( indOutput_type *h );

//*****************************************************************************
/// \brief ������� ����������� ���������� � �������� ��������� �����.
/// \param h - ��������� �� ������ ���������� � ��� ������� �������;
/// \param timeOn - ����� ����������� ���������, ��;
/// \param timeOff - ����� ������������ ���������, ��;
/// \param outPhase - ��������� ���� ��������� ����������.
///
void Indication_blink( indOutput_type *h, uint16_t timeOn, uint16_t timeOff, IndOutPhase outPhase );

//*****************************************************************************
/// \brief ������� ����������� ���������� � ������� �������� � �������� ��������� �����.
/// \param h - ��������� �� ������ ���������� � ��� ������� �������;
/// \param timeOn1 - ����� ����������� ��������� 1-�� �������, ��;
/// \param timeOff1 - ����� ������������ ��������� 1-�� �������, ��;
/// \param timeOn2 - ����� ����������� ��������� 2-�� �������, ��;
/// \param timeOff2 - ����� ������������ ��������� 2-�� �������, ��;
/// \param outPhase - ��������� ���� ��������� ����������.
///
void Indication_blink2( indOutput_type *h, uint16_t timeOn1, uint16_t timeOff1,
        uint16_t timeOn2, uint16_t timeOff2, IndOutPhase outPhase );

//*****************************************************************************
/// \brief ��������� ���������� ���������� �� �������� �����.
/// \param h - ��������� �� ������ ���������� � ��� ������� �������;
/// \param time - ����� ����������� ��������� ����������, ��.
///
void Indication_pulseOn( indOutput_type *h, uint16_t time );

//*****************************************************************************
/// \brief ��������� ��������� ����������� ������� ����������.
/// \param h - ��������� �� ������ ���������� � ��� ������� �������.
///
void Indication_run( indOutput_type *h );

#endif

//*****************************************************************************
/**
* ������� ���������: 
*
* ������ 1.0.1
* ����   24-02-2017
* �����  ���������� �.�.
*
* ���������:
*    ������� ��������.
*/
