/**
* \file    Adc.h
* \brief   ������ ������ � ���, �� dsPIC33 
* \details ���������
* \details <b>1 ����� ��������</b>
* \details 1.1  ������ ������������ ����� ����������� �������������� ��� � ������������
* �� � �������. ����� �������, � ��� ������� ����� ������������ ����������� ������ � ���.
* ����� ����������� �������������� ���������� ����������� ������ � �������� �������������, 
* � ��������� �������� ����������� ��������� �� �� �������.
* \details <b>2 ��������� ������</b>
* \details 2.1  ��� ������ ������ ��� � ������ AnalogInputId.h ������ ���� ��������� 
* ������ ��������������� ���������� ����������. �������������� ������ ���� ����������
* � ������������� ���� #AnalogInputChannels, ������� � ���� � ���������� ��������� 
* #eAinChCount, ������� ������������ ������� ��� ����������� ���������� ����������.
* \note �������� ��������������� ������ ���� ������ ��� ��������� ���, ����� ��������
* ����������� ���� �� 1 ������, � �������� ������������ - �� 1 ������. �� ������ ���� 
* ���� � ����� ��������������� � ����������� ����������.
* \details <b>3 ������������� ������</b>
* \details 3.1  ����� �������������� ������ ��� ���������� �������������������, 
* ������ ������� #Adc_ctor.
* \details 3.2  � ��������� ��������� ���������� ������� ��� ������� ��������� 
* ������������ ��������� ���������� ������� ������������� ���������� �������� 
* ������� #Adc_run, ������� ��������� ���������� ���������-��������� 
* �������������� � �������� �� � �������.
* \note ���� ��� ������ #Adc_run �� ����� ���������� ��� #doneErrorCount
* ���, ���������� ������� � �� � ����� #ePS_AdcDoneError 
* ������ #eGrPS_AdcDrv. ������� � ������� � �� �������������� ��� ������ ����������������
* \a CASSERT_ID.
* \details 3.3  � �������� ����� ������� \a main ���������� ���������� ������� #Adc_isReady �, ���� 
* ��� ���������� true, ���������� ������ �� ������� �������� #Adc_get.
* \note ������� ������ ������������ �������, ��� ����������. ��� �� ������������ ����������
* ������� � �� � ����� #ePS_AdcOverflow ������ #eGrPS_AdcDrv.
*
* \version 2.0.2
* \date    12-03-2020
* \author  ��������� �.�., ������ �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef ADC_H
#define ADC_H

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������������� ������.
/// \note ������ ������� ������ ���� ����������� ������� ����� ������� ���������
/// ������� ����� ������.
///
void Adc_ctor( void );

//*****************************************************************************
/// \brief ���������� ��������� ���.
/// \details ��������� ���������� ���������-��������� �������������� 
/// � �������� �� � �������.
/// \note ����������� #doneErrorCount #CASSERT_INC
/// (��. ������ asserts.c #CASSERT_ID) ������� ������ ��� ����������
/// ���������� ���������������.
///
void Adc_run( void );

//*****************************************************************************
/// \brief ������ ����������� �� ������� ���������.
/// \details ��������� ���������� �� ������� ���������
/// � �������� �� � \a array. 
/// \note ������ ����� ������ ���������� ������ ����� #Adc_isReady
/// ���������� true (���� ������ � ������� ���������).
/// \param array - ��������� �� ������ ������������ ������.
/// \note ���� ����� ������� \a size �� ������������� �������� #eAinChCount, 
/// ���������� ������� � �� � ����� #ePS_AdcDrvSetSize
/// ������ #eGrPS_AdcDrv.
/// 
void Adc_get( uint16_t *array);

//*****************************************************************************
/// \brief �������� ������� ����������� � ������� ���������.
/// \retval true  - ���� ���������� � ������� ���������;
/// \retval false - ��� ����������� � ������� ���������.
///
bool Adc_isReady( void );

#endif 

//*****************************************************************************
/**
* ������� ���������: 
*
* ������ 1.0.1
* ����   27-03-2016
* �����  ������� �.�. 
*
* ���������:
*     ������� ������.
*
* ������ 2.0.1
* ����   27-03-2017
* �����  ��������� �.�. 
*
* ���������:
*    ��������� ��� ���������� XC16.
*
* ������ 2.0.2
* ����   12-03-2020
* �����  ������ �.�.
*
* ���������:
*    ���������������� ��������������.
 */