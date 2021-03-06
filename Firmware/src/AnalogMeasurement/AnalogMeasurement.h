/**
* \file    AnalogMeasurement.h
* \brief   ���������, ����������, ���������� ���������� ���������� � ���������� �� � ���������� ��������� 
* \details ���������
*
* \version 1.0.1
* \date    08-10-2018
* \author  ������ �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef ANALOGMEASUREMENT_H
#define ANALOGMEASUREMENT_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "AnalogInput.h"

//*****************************************************************************
// ���������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define ANALOG_SYNCHRO_PERIOD    100          ///< ������ ������������� ���������� ����������.

//*****************************************************************************
// ����������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ���������, ����������� � ���������� ���������.
///
typedef struct AnalogMeasurementValue_tag 
{
    uint16_t rms;          ///< ������� �������� �������.
    int16_t  value;        ///< ���������� �������� �������.
} AnalogMeasurementValue;

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
void AnalogMeasurement_ctor( void );        ///< ������������� ���������� ���������� (�����������).

//*****************************************************************************
/// \brief ��������� ���������� ��������.
/// \details ������� ���������:
/// \details - �������������� � �������� �������;
/// \details - ��������� ������������� �������;
/// \details - ��������� ���������� � ������������� ��������;
/// \details - ���������� ���������� �������� � ���������� ���������;
/// \details - ������������� ���������� ���������� ����� ����� ��������.
/// \note ���������� � ������ ������� ������ ��������������� ����������� 
/// ������� #AnalogInput_run.
///
void AnalogMeasurement_run( void );

//*****************************************************************************
/// \brief ������ ����������� ��������� ���.
/// \note ���������� � ������ ������� ������ ��������������� ����������� 
/// ������� #AnalogInput_runInterrupt.
///
void AnalogMeasurement_runInterrupt( void );

//*****************************************************************************
/// \brief ������ ���������� �������� (������������ ������������ �������� ���������� �������).
/// \param channel � ����� ������.
/// \return ���������� �������� ��� ���������� ������ ���������.
///
const AnalogMeasurementValue *AnalogMeasurement_getData( uint16_t channel );

#endif

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   08-10-2018
* �����  ������ �.�.
* 
* ���������:
*    ������� ������.
*/
