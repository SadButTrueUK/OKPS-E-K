/**
* \file    Rs422.h
* \brief   ����������� ����� � ����������� �������� �� ���������� RS-422 
* \details ���������
* \details ������������ ��� ������ ����� �������������� 
*          ������� ������ � �� �� ���� ������ RS-422.
*
* \version 1.0.1
* \date    02-06-2017
* \author  ������ �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef RS422_H
#define RS422_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "Rs422_dataTypes.h"

//*****************************************************************************
// �������� ����� ������
//*****************************************************************************

//*****************************************************************************
typedef struct Rs422_handler_ Rs422_handler;        ///< ��������� ��������� ������ RS-422

//*****************************************************************************
/// \brief ������ ����� �����.
///
typedef enum 
{
    eRs422_line_1 = 1,        ///< 1-�� ����� �����
    eRs422_line_2             ///< 2-�� ����� �����
} Rs422_numLine;

//*****************************************************************************
/// \brief ���� �������.
///
typedef enum 
{
    eAlarmShortCircuitThreePhaseGen = 0x60,        ///< ��� ������ "�������� ��������� � ����� 3-������� ����������"
    eAlarmNoSupplyVoltage220V       = 0x65,        ///< ��� ������ "���������� ���������� 220 �"
    eAlarmRPVnotTurnOn              = 0x66,        ///< ��� ������ "�� ���������� ���� ������������ ������" 
    eAlarmRPVnotTurnOff             = 0x67,        ///< ��� ������ "�� ����������� ���� ������������ ������" 
    eAlarmShortCircuitContrGen      = 0x6A,        ///< ��� ������ "�������� ��������� � ����� ������������ ����������"
    eAlarmControlGeneratorIsFail    = 0x6B,        ///< ��� ������ "���������� � ����� ������������ ���������� �� � �����"
    eAlarmNoSupplyVoltage24V        = 0xF0,        ///< ��� ������ "���������� ���������� 24 �"

    /// \brief ��� ������ "���������� ����������� ������� �� ����� ����� 1 ������ 1,5 �"
    ///
    eAlarmNoFirstLineExchCtrlSys    = 0xF1,

    /// \brief ��� ������ "���������� ����������� ������� �� ����� ����� 2 ������ 1,5 �"
    ///
    eAlarmNoSecondLineExchCtrlSys   = 0xF2,

    /// \brief ��� ������ "���������� ����������� ������� �� ����� �� ����� ����� �������� ������� ������ 1,5 �"
    ///
    eAlarmNoValidOrderToNeighbor    = 0xF3,
    eAlarmNeighborInSafeMode        = 0xF4,      ///< ��� ������ "�������� ������ � ���������� ���������"  
    eAlarmNoConnectWithNeighbor     = 0xF5       ///< ��� ����� � ��c����� �������� (��� ���������� ����� �����)
} Rs422_alarmCode;

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief  �������������.
/// \param addr                     - ����� �������;
/// \param noExchangeTime           - ����� ������������ ������� "��� ������" �� ����� �����, ��;
/// \param noExchangeTimeCtrlSystem - ����� ������������ ������� "���������� ������ � ��", ��;
/// \param orderLength              - ����� ������ � �������. 
/// \return ��������� �� ��������� ���������. 
/// \note ������ ��������� ������ ������������ � ���������� ���� ������ ��������,
/// ����������� ������ ������.
///
Rs422_handler *Rs422_ctor( uint16_t addr, uint16_t noExchangeTime, uint16_t noExchangeTimeCtrlSystem,
                           uint8_t orderLength );

//*****************************************************************************
/// \brief ���������� ������� � ��������� ������.
/// \param handler - ��������� �� ��������� ���������.
/// \note ���������� �� �������� ����� � �������� 1 ��.
///
void Rs422_run( Rs422_handler *handler );

//*****************************************************************************
/// \brief ����� � �������� ������.
/// \param handler - ��������� �� ��������� ���������.
/// \note ���������� �� ���������� � �������� 62,5 ���.
///
void Rs422_interrupt( Rs422_handler *handler );

//*****************************************************************************
/// \brief ���������� ������ ������.
/// \param handler - ��������� �� ��������� ���������;
/// \param enable  - ��������� ����� RS422 (true (��������), false (���������)).
///
void Rs422_setEnable( Rs422_handler *handler, bool enable );

//*****************************************************************************
/// \brief ��������� ������� �������.
/// \param handler  - ��������� �� ��������� ���������;
/// \param data     - ��������� �� ������ ������ �������;
/// \param dataSize - ����� ������� ������ �������.
///
void Rs422_setStatus( Rs422_handler *handler, const uint8_t *data, uint8_t dataSize );

//*****************************************************************************
/// \brief ���������� ������� ������� ������ �� ��������� ����� �����.
/// \param handler - ��������� �� ��������� ���������;
/// \param nLine   - ����� ����� (#eRs422_line_1, #eRs422_line_2).
/// \retval true  - ���� �������;
/// \retval false - ��� �������.
///
bool Rs422_getLineEvent( Rs422_handler *handler, Rs422_numLine nLine );

//*****************************************************************************
/// \brief ���������� ������� ���������� ����� � �� �� ����� ������.
/// \param handler - ��������� �� ��������� ���������;
/// \param nLine   - ����� ����� (#eRs422_line_1, #eRs422_line_2).
/// \retval true  - ��� ������;
/// \retval false - ���� �����.
///
bool Rs422_getNoLineExchange( Rs422_handler *handler, Rs422_numLine nLine );

//*****************************************************************************
/// \brief ���������� ������� ���������� ������ � ��.
/// \param handler - ��������� �� ��������� ���������;
/// \param nLine   - ����� ����� (#eRs422_line_1, #eRs422_line_2).
/// \retval true  - ��� ������;
/// \retval false - ���� �����.
///
bool Rs422_getNoLineExchangeCtrlSystem( Rs422_handler *handler, Rs422_numLine nLine );

//*****************************************************************************
/// \brief ���������� ������� ���������� �����  c �� �� ����� ������.
/// \param handler - ��������� �� ��������� ���������.
/// \retval true  - ��� ������;
/// \retval false - ���� �����.
///
bool Rs422_getNoExchangeCtrlSystem( Rs422_handler *handler );

//*****************************************************************************
/// \brief ��������� ������� � ������ ��� �����.
/// \param handler - ��������� �� ��������� ���������;
/// \param data    - ��������� �� ������ ��� ������ �������.
/// \return ����� �������, ����.
///
uint8_t Rs422_getOrder( Rs422_handler *handler, uint8_t *data );

//*****************************************************************************
/// \brief ���������� ���� ������ � ������ ���������� ��� �������.
/// \param handler - ��������� �� ��������� ���������;
/// \param alarm   - ��� ������.
///
void Rs422_addAlarm( Rs422_handler *handler, Rs422_alarmCode alarm );

//*****************************************************************************
/// \brief �������� ���� ������ �� ������ ���������� ��� �������.
/// \param handler - ��������� �� ��������� ���������;
/// \param alarm   - ��� ������.
///
void Rs422_removeAlarm( Rs422_handler *handler, Rs422_alarmCode alarm );

//*****************************************************************************
/// \brief ������� ������� ������ �������.
/// \param handler - ��������� �� ��������� ���������;
/// \param nLine   - ����� ����� (#eRs422_line_1, #eRs422_line_2).
/// \retval true  - ���� ������;
/// \retval false - ��� �������.
///
bool Rs422_getNewOrderEventLine( Rs422_handler *handler, Rs422_numLine nLine );

#endif

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   02-06-2017
* �����  ������ �.�.
* 
* ���������:
*    ������� ������.
*/
