/**
* \file    ManageCanDriver.h
* \brief   ���������� ��������� CAN
* \details ���������
*
* \version 1.0.2
* \date    23-08-2019
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef MANAGECANDRIVER_H
#define MANAGECANDRIVER_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "EcanDriver.h"

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief �����������.
/// \param numCan       � ����� ������ CAN, � ������� �������� �������; 
/// \param txSid        � ����������� ������������� CAN ��� ������������� ������;
/// \param rxSid        � ����������� ������������� CAN ��� ������������ ������;
/// \param mode         - �������� ����� ������ ������ CAN;
/// \param lengthPacket � ����� ������ ������ (������ ���� ������ � �� ����� 8).
/// \return ��������� �� ���������, ����������� ���������
/// ������� ��������.
/// \note ������ ��������� ������ ������������ � ���������� ����
/// ������ ��������, ����������� ������ ������� ��������.
///
const ArrayIoDriver *ManageCanDriver_ctor( EcanNumber numCan,
                                   uint16_t txSid,
                                   uint16_t rxSid,
                                   uint16_t mode,
                                   uint16_t lengthPacket );

//*****************************************************************************
/// \brief ������ ������ �� ������������� ����������.
/// \param self  - ��������� �� ������ �������, ������� ������ �����������;
/// \param array - ��������� �� ������ ��� ������;
/// \param size  - ����� �������.
/// \return ����� �������� ������, ������� ���� �������� 
/// � ������ \a array.
///
uint16_t ManageCanDriver_get( const void *self, uint8_t *array, uint16_t size );

//*****************************************************************************
/// \brief ���������� ���������� � ������ �������� ������.
/// \param self - ��������� �� ������ �������, ������� ������ �����������.
/// \retval true - ���������� ������ � ���������� ������;
/// \retval false - ���������� �� ������ � ���������� ������.
///
bool ManageCanDriver_isInReady( const void *self );

//*****************************************************************************
/// \brief �������� ������ �������� ��� ������.
/// \param self  - ��������� �� ������ �������, ������� ������ �����������;
/// \param array - ��������� �� ������, ������� ���������� ������;
/// \param size  - ������ ������� ������.
///
void ManageCanDriver_set( const void *self, uint8_t *array, uint16_t size );

//*****************************************************************************
/// \brief ���������� ���������� � ������ ����� ������.
/// \param self - ��������� �� ������ �������, ������� ������ �����������.
/// \retval true - ���������� ������ � ������ ����� ������;
/// \retval false - ���������� �� ������ � ������ ����� ������.
///
bool ManageCanDriver_isOutReady( const void *self );

//*****************************************************************************
/// \brief ������� ����.
/// \param self - ��������� �� ������ �������, ������� ������ �����������.
///
void ManageCanDriver_run( const void *self );

//*****************************************************************************
/// \brief ������ ���������� �������� ��������.
/// \param self - ��������� �� ��������� ������ �������.
/// \return ��� ���������� ��������� ��������.
///
uint16_t ManageCanDriver_getError( const void *self );

//*****************************************************************************
/// \brief ����� ��������.
/// \param self - ��������� �� ��������� ������ �������.
///
void ManageCanDriver_reset( const void *self );

//*****************************************************************************
/// \brief ������ �������� �������� � ��������.
/// \param self  - ��������� �� ��������� ������ �������;
/// \param array - ��������� �� ������ ������;
/// \param size  - ����� ������� ������.
///
void ManageCanDriver_start( const void *self, uint8_t *array, uint16_t size );

#endif 

//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   25-10-2018
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
*
* ������ 1.0.2
* ����   23-08-2019
* �����  ��������� �.�.
*
* ���������:
*    ������� ��������� ������������. 
*/
