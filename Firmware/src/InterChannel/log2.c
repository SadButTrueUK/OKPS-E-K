/**
* \file    log2.c
* \brief   ����������� ������� Log2_log2Lkup[]
* \details ������� ������������ ��������� � ����������� set.h
* 
* \version 1.0.1
* \date    12-06-2013
* \author  ������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdint.h>

//*****************************************************************************
// ���������� ��������� �������������� ��������
//*****************************************************************************

//*****************************************************************************
/// \brief ������� ������ ��� \f$\left(1 \ll \frac{n-1}{8}\right)\f$, ��� \a n - ������ � �������.
/// \note �������� �������� \a n = 0..64. ������ ������ ( n == 0 ) ������� �� ������ ��������������.
///
uint8_t const Log2_log2Lkup[256] =
{
    ( uint8_t )0, ( uint8_t )1, ( uint8_t )2, ( uint8_t )2,
    ( uint8_t )3, ( uint8_t )3, ( uint8_t )3, ( uint8_t )3,
    ( uint8_t )4, ( uint8_t )4, ( uint8_t )4, ( uint8_t )4,
    ( uint8_t )4, ( uint8_t )4, ( uint8_t )4, ( uint8_t )4,
    ( uint8_t )5, ( uint8_t )5, ( uint8_t )5, ( uint8_t )5,
    ( uint8_t )5, ( uint8_t )5, ( uint8_t )5, ( uint8_t )5,
    ( uint8_t )5, ( uint8_t )5, ( uint8_t )5, ( uint8_t )5,
    ( uint8_t )5, ( uint8_t )5, ( uint8_t )5, ( uint8_t )5,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8
};

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   12-06-2013
* �����  ������� �.�.
*
* ���������:
*    ������� ������.
*/
