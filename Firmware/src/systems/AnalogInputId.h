/**
* \file    AnalogInputId.h
* \brief   �������������� ������� ���������� ��������.
*
* \version 1.0.1
* \date    05-10-2018
* \author  ������ �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef ANALOGINPUTID_H
#define ANALOGINPUTID_H

//*****************************************************************************
// ���������� ���������, ������������ ����� �������
//*****************************************************************************

////*****************************************************************************
#define AVERAGE_WINDOW_SIZE     128        ///< ���������� �������� ��� ������� ��������������������� ��������.
#define AnalogInputDrvQueueSize 3          ///< ������ ������� �������� � ���.

//*****************************************************************************
/// \brief ������ ���������� ����������.
///
typedef enum AnalogInputChannels_tag
{
    eAinchKREF = 0,      ///< �������� �������� ���������� ��������� �����������
    eAinchK3V3 = 1,      ///< �������� ���������� ������� ��������� �����������
    eAinchREF  = 2,      ///< ���������� ������������ ���
    eAinchGEN  = 3,      ///< ���������� ������������ ����������
    eAinchR1   = 4,      ///< ���������� ������� ��������� ������� R1
    eAinchR2   = 5,      ///< ���������� ������� ��������� ������� R2
    eAinchIV   = 6,      ///< ��� ���������, ���� V
    eAinchIU   = 7,      ///< ��� ���������, ���� U
    eAinchIW   = 8,      ///< ��� ���������, ���� W
    eAinchUV   = 9,      ///< ���������� ���� V
    eAinchUU   = 10,     ///< ���������� ���� U
    eAinchUW   = 11,     ///< ���������� ���� W
    eAinChCount          ///< ���������� ������� ���������� ��������
} AnalogInputChannels;

#endif

//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   05-10-2018
* �����  ������ �.�.
*
* ���������:
*    ������� ������.
*/