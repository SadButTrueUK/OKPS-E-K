/**
* \file    CheckSupply.c
* \brief   \copybrief CheckSupply.h
*
* \version 1.0.2
* \date    11-09-2018
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "CheckSupply.h"
#include "HystFltr.h"
#include "BinIn.h"
#include "CheckCallFunctions.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
/// \brief �������� ����������� ��������� ���������� ������� 220 � �� ��� ���������, ��.
///
#define SUPPLY_220V_EN  600 


//*****************************************************************************
/// \brief �������� ����������� ��������� ���������� ������� 220 � �� ��� ����������, ��.
///
#define SUPPLY_220V_DIS 10


//*****************************************************************************
/// \brief �������� ����������� ��������� ���������� ������� 24 � �� ��� ���������, ��.
///
#define SUPPLY_24V_EN   500

//*****************************************************************************
/// \brief �������� ����������� ��������� ���������� ������� 24 � �� ��� ����������, ��.
///
#define SUPPLY_24V_DIS  250

//*****************************************************************************
//  ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ����� ������.
///
typedef struct
{
    uint8_t ctrl       :1;        ///< ��������� ������.
    uint8_t supply220v :1;        ///< ��������� ���������� ������� 220 �.
    uint8_t supply24v  :1;        ///< ��������� ���������� ������� 24 �.
    uint8_t unused     :5;        ///< ��������������.
} CheckSupply_flags;

//*****************************************************************************
/// \brief ����������� ������ � \a uint8_t.
///
typedef union
{
    CheckSupply_flags str;         ///< ����� ������.
    uint8_t           data;        ///< ������ �����.
} uCheckSupply_flags;

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
digInpHystFilt_type checkSupply_220v;        ///< ������ � ������������ ��������� ������� 220 �.
digInpHystFilt_type checkSupply_24v;         ///< ������ � ������������ ��������� ������� 24 �.
uCheckSupply_flags  flags;                   ///< ����� ������.

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� � ������
void CheckSupply_ctor( void )
{
    //������������� ������� � ������������ ��� ���������� ������� 220 �
    HystFltr_ctor( &checkSupply_220v, SUPPLY_220V_EN, SUPPLY_220V_DIS );
    //������������� ������� � ������������ ��� ���������� ������� 24 � 
    HystFltr_ctor( &checkSupply_24v, SUPPLY_24V_EN, SUPPLY_24V_DIS );
    flags.data = 0;        // ������������� ������
    flags.str.ctrl = true; // ��������� ������
}

//*****************************************************************************
// ���������� �������
void CheckSupply_run( void )
{
    if( flags.str.ctrl )
    {
        //������ ��������� ������� ��� ������� 220 �
        flags.str.supply220v = HystFltr_run( &checkSupply_220v, BinIn_is220vOk( ) );
        //������ ��������� ������� ��� ������� 24 �
        flags.str.supply24v = HystFltr_run( &checkSupply_24v, BinIn_is24vOk( ) );
    }
    MARKED_CALL_FUNCTION;
}

//*****************************************************************************
// ���������� ��������� ���������� ������� 220 �
bool CheckSupply_is220vOn( void )
{
    return flags.str.supply220v;
}

//*****************************************************************************
// ���������� ��������� ������� 24 �
bool CheckSupply_is24vOn( void )
{
    return flags.str.supply24v;
}

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* ����   11-05-2018
* �����  ������ �.�.
* 
* ���������:
*     ������� ������.
* 
* ������ 1.0.2
* ����   11-09-2018
* �����  ��������� �.�.
* 
* ���������:
*     ������� �������    
*     CheckSupply_isInst220vOn( )
*     CheckSupply_isInst24vOn( )
*/
