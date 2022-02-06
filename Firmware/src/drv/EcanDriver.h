/**
* \file    EcanDriver.h
* \brief   ������� ������ ECAN ��� �� dsPIC33
* \details ���������
*
* \version 1.0.1
* \date    26-05-2017
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef ECANDRIVER_H
#define ECANDRIVER_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "iodrv.h"

//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ����� ������ ECAN, � ������� �������� �������.
///
typedef enum EcanNumber_tag
{
    eEcan1    = 0,          ///< ������ �� ECAN1
    eEcan2,                 ///< ������ �� ECAN2
    eEcanCount              ///< ���������� ��������� ������� ECAN
} EcanNumber;

//*****************************************************************************
/// \brief ����� ������ ������ ECAN.
///
typedef enum EcanMode_tag
{
    eEcanModeNormal = 0,               ///< ���������� ����� ������ ������ ECAN
    eEcanModeConfig = 4                ///< ����� ��������� ������ ECAN
} EcanMode;

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief �����������.
/// \param numEcan � ����� ������ ECAN, � ������� �������� �������; 
/// \param txSid � ����������� ������������� ECAN ��� ������������� ������;
/// \param rxSid � ����������� ������������� ECAN ��� ������������ ������;
/// \param mode - �������� ����� ������ ������ ECAN;
/// \param lengthPacket � ����� ������ ������ (������ ���� ������ � �� ����� 8).
/// \return ��������� �� ���������, ����������� ���������
/// ������� ��������.
/// \note ������ ��������� ������ ������������ � ���������� ����
/// ������ ��������, ����������� ������ ������� ��������.
///
const void *Ecan_ctor( EcanNumber numEcan,
                      uint16_t txSid,
                      uint16_t rxSid,
                      uint16_t mode,
                      uint16_t lengthPacket );

//*****************************************************************************
/// \brief ������ ������ �� ������������� ����������.
/// \param self  - ��������� �� ������ �������, ������� ������ �����������;
/// \param array - ��������� �� ������ ������������ ������;
/// \param size  - ����� �������.
/// \return ����� �������� ������, ������� ���� �������� � ������ \a array.
///
uint16_t Ecan_get( const void *self, uint8_t *array, uint16_t size );

//*****************************************************************************
/// \brief ���������� ���������� � ������ �������� ������.
/// \param self - ��������� �� ������ �������, ������� ������ �����������.
/// \retval true - ���������� ������ � ���������� ������;
/// \retval false - ���������� �� ������ � ���������� ������.
///
bool Ecan_isInReady( const void *self );

//*****************************************************************************
/// \brief �������� ������ �������� ��� ������.
/// \param self - ��������� �� ������ �������, ������� ������ �����������;
/// \param array - ��������� �� ������, ������� ���������� ������;
/// \param size - ������ ������� ������.
///
void Ecan_set( const void *self, uint8_t *array, uint16_t size );

//*****************************************************************************
/// \brief ���������� ���������� � ������ ����� ������.
/// \param self - ��������� �� ������ �������, ������� ������ �����������.
/// \retval true - ���������� ������ � ������ ����� ������;
/// \retval false - ���������� �� ������ � ������ ����� ������.
///
bool Ecan_isOutReady( const void *self );

//*****************************************************************************
/// \brief ������� ����.
/// \param self - ��������� �� ������ �������, ������� ������ �����������.
///
void Ecan_run( const void *self );

//*****************************************************************************
/// \brief ����� ������ ��������� CAN.
/// \param self - ��������� �� ������ �������, ������� ������ �����������.
/// \note ������� ��������� ���������� CAN � ����� ������������, � ����� � �����,
/// �������� ��� ������ ������������. 
///
void Ecan_reset( const void *self );

//*****************************************************************************
/// \brief ����� �������� �������� ������.
/// \param self - ��������� �� ������ �������, ������� ������ �����������.
///
void Ecan_AbortTransmite( const void *self );

//*****************************************************************************
/// \brief ��������� �������� �������� ������.
/// \param self - ��������� �� ������ �������, ������� ������ �����������.
///
void Ecan_startTransmite( const void *self );

//*****************************************************************************
/// \brief ������ ���������� �������� ��������.
/// \param self - ��������� �� ������ �������, ������� ������ �����������.
/// \return ��� ������ �������� ��������.
///
uint16_t Ecan_getError( const void *self );

//*****************************************************************************
/// \brief ������ �������� ��������.
/// \param self  - ��������� �� ������ �������� (�����, ������� ������ �����������);
/// \param array � ��������� �� ����� ������� ������;
/// \param size  � ����� ������� ������.
/// \note ������� �� ������������� � ��������� ��������, �� ����� �������� � �������� � ��.
/// 
void Ecan_start( const void *self, uint8_t *array, uint16_t size );

#endif 

//*****************************************************************************
/**
* ������� ���������: 
*
* ������ 1.0.1
* ����   26-05-2017
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
*/
