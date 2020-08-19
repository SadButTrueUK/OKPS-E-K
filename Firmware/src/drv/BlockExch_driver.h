/**
* \file    BlockExch_driver.h
* \brief   ������� ������ ����� �������� � ��������� ���������
* \details ���������
*
* \version 1.0.1
* \date    05-05-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef BLOCKEXCH_DRIVER_H
#define BLOCKEXCH_DRIVER_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdbool.h>
#include <stdint.h>

//*****************************************************************************
// ���������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define MAX_DATA_LENGTH 10U        ///< ������������ ����� ������.

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief �����������.
/// \return ��������� �� ��������� ��������� ��������.
///
const void *BlockExchDrv_ctor( void );

//*****************************************************************************
/// \brief ������ ������ �� ������������� ����������.
/// \param self - ��������� �� ������ �������, ������� ������ �����������;
/// \param array - ��������� �� ������ ������������ ������;
/// \param size  - ����� �������.
/// \return ����� �������� ������, ������� ���� �������� � ������ \a array.
///
int BlockExchDrv_get( const void *self, uint8_t *array, int size );

//*****************************************************************************
/// \brief ���������� ���������� � ������ �������� ������.
/// \param self - ��������� �� ������ �������, ������� ������ �����������.
/// \retval true  - ���������� ������ � ���������� ������;
/// \retval false - ���������� �� ������ � ���������� ������.
///
bool BlockExchDrv_isInReady( const void *self );

//*****************************************************************************
/// \brief �������� ������ �������� ��� ������.
/// \param self - ��������� �� ������ �������, ������� ������ �����������;
/// \param array - ��������� �� ������, ������� ���������� ������;
/// \param size - ������ ������� ������.
///
void BlockExchDrv_set( const void *self, uint8_t *array, int size );

//*****************************************************************************
/// \brief ���������� ���������� � ������ ����� ������.
/// \param self - ��������� �� ������ �������, ������� ������ �����������.
/// \retval true  - ���������� ������ � ������ ����� ������;
/// \retval false - ���������� �� ������ � ������ ����� ������.
///
bool BlockExchDrv_isOutReady( const void *self );

//*****************************************************************************
/// \brief ������� ����.
/// \note ���������� �� ���������� � �������� 62,5 ���.
/// \param self - ��������� �� ������ �������, ������� ������ �����������;
/// \param update - ���������� �� ���������� ������ ��� ��������.
///
void BlockExchDrv_run( const void *self, bool update );

#endif

//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   05-05-2017
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
*/