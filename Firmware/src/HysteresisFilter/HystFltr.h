/**
* \file    HystFltr.h
* \brief   ���������� �������� ������ � ������������
* \details ���������
*
* \version 1.0.1
* \date    11-04-2017
* \author  ���������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef HYSTFILTR_H
#define HYSTFILTR_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ������� ����������� �������� �����.
///
typedef struct 
{
    uint16_t inCnt;          ///< ������� ������ ���������.
    uint16_t enHyst;         ///< ����� ����������� �� ���������.
    uint16_t disHyst;        ///< ����� ����������� �� ����������.
    bool     inPrv;          ///< ��������� ����� ��� ���������� ���������.
} digInpHystFilt_type;

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief ������������� ����������� �������� ����� � ��������� ���������.
/// \param h - ��������� �� ������ ����������� �����;
/// \param enHyst - ����� ����������� �� ���������;
/// \param disHyst - ����� ����������� �� ����������.
///
void HystFltr_ctor( digInpHystFilt_type *h, uint16_t enHyst, uint16_t disHyst );

//*****************************************************************************
/// \brief ���������� ��������� �������.
/// \param h - ��������� �� ������ ����������� �����;
/// \param inAct - ���������� ��������� �����.
/// \return ������������� ��������� �����.
///
bool HystFltr_run( digInpHystFilt_type *h, bool inAct );

#endif

//*****************************************************************************
/**
* ������� ���������: 
*
* ������ 1.0.1
* ����   11-04-2017
* �����  ���������� �.�.
*
* ���������:
*    ������� ��������.
*/
