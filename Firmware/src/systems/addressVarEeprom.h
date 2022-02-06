/**
* \file    addressVarEeprom.h
* \brief   ������ ���������� � ��������, ���������� � EEPROM.
*
* \version 1.0.1
* \date    18-01-2017
* \author  ��������� �.�.
*/

#ifndef addressVarEeprom_h
#define addressVarEeprom_h

//*****************************************************************************
// ���������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define ADDRESS_EEPROM_IDENT_ID          0x0000    ///< ��������� ����� ������������� (ID �������).
#define ADDRESS_EEPROM_IDENT_NAME        0x0002    ///< ��������� ����� ������������� (��� �������).
#define ADDRESS_EEPROM_IDENT_VERSION     0x0010    ///< ��������� ����� ������������� (������ ��).
#define ADDRESS_EEPROM_IDENT_DATE        0x0020    ///< ��������� ����� ������������� (���� ���������� ��).

//*****************************************************************************
#define ADDRESS_EEPROM_PROTECTION_CODE_1 0x0030    ///< ����� 1-�� ������ \a EEPROM � ����� ���.
#define ADDRESS_EEPROM_PROTECTION_CODE_4 0x0032    ///< ����� 4-�� ������ \a EEPROM � ����� ���.
#define ADDRESS_SOURCE_RESET             0x0034    ///< ����� ������ \a EEPROM � �������� ������ (�������) ��.
#define ADDRESS_EEPROM_TIME_SEC          0x0036    ///< ����� ������ \a EEPROM � ������� �������� ������, �.
#define ADDRESS_EEPROM_TIME_MS           0x0038    ///< ����� ������ \a EEPROM � ������� �������� ������, ��.

//*****************************************************************************
/// \brief ����� ������ \a EEPROM � ������� �������� ������ (����� ���������� �� ���������� �������).
///
#define ADDRESS_EEPROM_TIME_N_TMR        0x003A

//*****************************************************************************
/// \brief ����� ������ \a EEPROM � ������� �������� ������ (��������� ���������� �������).
///
#define ADDRESS_EEPROM_TIME_TMR          0x003C


#define ADDRESS_EEPROM_PROTECTION_STATE_ADD_CODE 0x0040    ///< ����� ������� \a EEPROM � ���������� ����������� ��� ���.

//*****************************************************************************
/// \brief ��������� ����� ��������� �����������: �����, ���������� ��������, ������ �������.
///
#define ADDRESS_EEPROM_STRUCT_TRACING            0x0070
#define ADDRESS_EEPROM_TRACING_START             0x0080    ///< ��������� ����� ������� �����������.

//*****************************************************************************
/// \brief ������ ���������� �������
///
#define ADDRESS_EEPROM_CALIBR_ADDR                   0x0E90   ///< ����� ������� ��� ��������� ���������
#define ADDRESS_EEPROM_CALIBR_CONF                   0x0E92   ///< ������������ ������� ��� ��������� ���������
#define ADDRESS_EEPROM_CALIBR_CRC                    0x0E94   ///< �RC ��� ��������� ��� ��������� ���������
#define ADDRESS_EEPROM_MAGIC_WORD                    0x0E96   ///< ������� ������ ���������� ������
#define ADDRESS_EEPROM_CALIBR_THRESHOLD_P            0x0E98   ///< ������������� �������� ���� ��� �������� � ����
#define ADDRESS_EEPROM_CALIBR_THRESHOLD_M            0x0E9A   ///< ������������� �������� ���� ��� �������� � �����
#define ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_P 0x0E9C   ///< ������������� �������� ������������ ������� �������� � ����
#define ADDRESS_EEPROM_CALIBR_INCREASED_TIME_SHIFT_M 0x0E9E   ///< ������������� �������� ������������ ������� �������� � ����
#define  ADDRESS_EEPROM_MANUFACTURERS_NUMBER         0x0EA0


//*****************************************************************************
#define ADDRESS_EEPROM_PROTECTION_CODE_2         0x0FF0    ///< ����� 2-�� ������ \a EEPROM � ����� ���.
#define ADDRESS_EEPROM_PROTECTION_CODE_3         0x0FF2    ///< ����� 3-�� ������ \a EEPROM � ����� ���.


#endif

//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   18-01-2017
* �����  ��������� �.�.
*
* ���������:
*    ������� ������.
*/
