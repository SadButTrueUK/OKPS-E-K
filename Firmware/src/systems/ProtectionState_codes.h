/**
* \file    ProtectionState_codes.h
* \brief   ���� ������� ����-�-�.
*
* \version 2.0.4
* \date    15-03-2021
* \author  ������ �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef PROTECTIONSTATE_CODES_H
#define PROTECTIONSTATE_CODES_H

//*****************************************************************************
// ���������� ���������, ������������ ����� �������
//*****************************************************************************
#define PROTECTION_CODE_ABSENCE 0xA55A        ///< ���, ������������ ���������� ��.

//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ��� ������.
///
typedef enum
{
    eGrPS_Main                 = 0x01,    ///< ������ ���������� Main.h
    eGrPS_InterChannel         = 0x02,    ///< ������ ���������� InterChannel.h
    eGrPS_EcanDrv              = 0x03,    ///< ������ �������� ������ \a CAN
    eGrPS_CanAckDrv            = 0x04,    ///< ������ �������� ������ \a CAN � �������������� ������
    eGrPS_BinIn                = 0x05,    ///< ������ ���������� BinIn.h
    eGrPS_AnalogInput          = 0x06,    ///< ������ ���������� AnalogInput.h
    eGrPS_AdcDrv               = 0x07,    ///< ������ �������� ������ ���
    eGrPS_Eeprom               = 0x08,    ///< ������ �������� \a EEPROM
    eGrPS_ControlMK            = 0x09,    ///< ������ ���������� �������� ��
    eGrPS_Rs422                = 0x0B,    ///< ������ ������ ������ �� ���������� RS-422
    eGrPS_BlockExch            = 0x0C,    ///< ������ ������ BlockExch.h
    eGrPS_DeviceAddress        = 0x0D,    ///< ������ ������ DeviceAddress.h
    eGrPS_RelayCtrl            = 0x0E,    ///< ������ ������ RelayCtrl.h
    eGrPS_OverloadDetector     = 0x10,    ///< ������ ������ ����������� ���������� �� ����
    eGrPS_PositionDet          = 0x11,    ///< ������ ������ ����������� ���������
    eGrPS_ShuntShift           = 0x12,    ///< ������ ������ �������� �������          
    eGrPS_SafetyPowerControl   = 0x13,    ///< ������ ������ ����������� ���������� ��������
    eGrPS_IntegrCtrl           = 0x14,    ///< ������ ������ �������� ����������� ������� ���������
    eGrPS_ControlSupplyVoltage = 0x15,    ///< ������ �������� ���������� �������, ��������, ��������� �������� ���
    eGrPS_ManageCanDrv         = 0x16     ///< ������ ��������� �������� ������ \a CAN  
} ProtectionStateGroup;


typedef enum
{
    ePS_NoError         = 0,                    ///< ��� ������
    ePS_ErrorParameters = 1,                    ///< ������ ���������� ������ �������        
    ePS_BufOverflow     = 2,                    ///< ������������ ������ 
    ePS_ErrorDevice     = 3,                    ///< ������������� ����������         
    ePS_TimeOut         = 4,                    ///< ����-��� ����������         
    ePS_NoAck           = 5,                    ///< ��� ������������� ��������  
    ePS_ErrorAlg        = 6,                    ///< ������ ����������
    ePS_NoInic          = 7                     ///< �� ���� �������������             
} TypicalErrors;

//eGrPS_Main = 0x01 ***********************************************************
/// \brief �������������� ������� ���������� Main.h.
///
typedef enum
{
    ePS_MainCheckInterrupt    = 0x01,    ///< ��������� ���������� ��������� �� ������ ����������� �������
    ePS_MainCheckPhase0       = 0x02,    ///< ������ ������� ��������� ������������� � ���� 0
    ePS_MainCheckPhase1       = 0x03,    ///< ������ ������� ��������� ������������� � ���� 1
	
	//*****************************************************************************
	/// \brief �� ������ ���������� ������� �� ���� ��������� ������ ������� �� ��������� ��
    ///
    ePS_MainCheckPeriodTimer  = 0x04,

    //*************************************************************************
    /// \brief �������� �������������� ����������.
    /// ��� ������������� ��������������� ���������� � ���������� ����������� ����� ����������,
    /// ��� ���������� ������� �������� �������������� ����������. ����� ����������� ����� ������� 
    /// ���������� (Interrupt Vector Number) �������� ������� 6-1 ��������� "Section 6. Interrupts. (DS70600B).pdf".
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ������� ����� ������ ���������� [15...0];
    ///     - �������� 2 - ������� ����� ������ ���������� [23...16];
    ///     - �������� 3 - ����� ������� ���������� (Interrupt Vector Number);
    ///     - �������� 4 - 0.
    ///
    ePS_MainUnusedInterrupt   = 0x05,    ///< �������� �������������� ����������

    ///     - �������� 1 - ������� RCON;
    ePS_MainIncorrectReset    = 0x08,    ///< ������������ ������� ������ ��

    ePS_MainErrorBeginSynchro = 0x09,    ///< ��� ������������� �� �� ������ ������ 
    ePS_MainErrorConfigCPU    = 0x0A,    ///< ������ ��� ����������� ������������ (Master/Slave) ����������
    ePS_MainCheckMaxInterrupt = 0x0C     ///< ���������� ���������� ��������� ����������
} ProtectionStateMainId;

//eGrPS_InterChannel = 0x02 ***************************************************
/// \brief �������������� ������� ���������� InterChannel.h
///
typedef enum
{
    //*************************************************************************
    /// \brief ������ �������� ������������� ���������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - :
    ///     - �������� 2 - ������������� ���������;
    ///     - �������� 3 - ���������������� ������;
    ///     - �������� 4 - 0.
    ///
    ePS_IntChIdError = 0x01,

    //*************************************************************************
    /// \brief �� ��������� ��������� �������� �������������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ����� �������:
    ///          - \a setRemote;
    ///          - \a setLocalAck;
    ///          - \a synchronize;
    ///          - \a setParamSettings;
    ///          - \a updateData;
    ///          - \a stateChanged.
    ///     - �������� 2 - ������������� ���������;
    ///     - �������� 3 - ���������������� ������;
    ///     - �������� 4 - ����� ��������.
    ///
    ePS_IntChScript = 0x02,

    //*************************************************************************
    /// \brief �� ��������� ������ ������ �������� � ��������� ���������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///
    ///     - �������� 1 - ����� �������:
    ///          - \a setRemote;
    ///          - \a setLocalAck;
    ///          - \a synchronize;
    ///     - �������� 2 - ������������� ���������;
    ///     - �������� 3 - ���������������� ������;
    ///     - �������� 4 - 0.
    ///
    ePS_IntChRemoteLocalScr = 0x03,

    //*************************************************************************
    /// \brief ��������� ������ �� ������������� ���������
    /// \note ��� ������ � ���� ��������������� � "������ ����" �����������:
    ///     - �������� 1 - ����� �������:
    ///          - \a setRemote;
    ///          - \a setLocalAck;
    ///          - \a synchronize;
    ///     - �������� 2 - ������������� ���������;
    ///     - �������� 3 - ���������������� ������;
    ///     - �������� 4 - 0.
    ///
    ePS_IntChRepeat = 0x04,

    //*************************************************************************
    /// \brief ������ ��� ���������� ��������� �������� �������� ������
    /// ��� ���������� �������� ��� ������ �������� �� ��������� ������.
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ������������� ���������;
    ///     - �������� 2 - 0;
    ///     - �������� 3 - 0;
    ///     - �������� 4 - 0.
    ///
    ePS_IntChTrError1 = 0x05,

    //*************************************************************************
    /// \brief ������ ��� ���������� ��������������� �������� �������� ������
    /// \note ��� ���������� ������ ��������� �������� ������ �� �������������.
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ������������� ���������;
    ///     - �������� 2 - 0;
    ///     - �������� 3 - 0;
    ///     - �������� 4 - 0.
    ///
    ePS_IntChTrError2 = 0x06,

    //*************************************************************************
    /// \brief ������� ������ ��������� ��������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ������������� ���������;
    ///     - �������� 2 - 0;
    ///     - �������� 3 - ������������� ��������� ��������;
    ///     - �������� 4 - 0.
    ///
    ePS_IntChProcCheck = 0x07,

    //*************************************************************************
    /// \brief ������� ������ ��������� �������������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ������������� ���������;
    ///     - �������� 2 - 0;
    ///     - �������� 3 - ������������� ��������� ��������;
    ///     - �������� 4 - 0.
    ///
    ePS_IntChProcSync = 0x08,

    //*************************************************************************
    /// \brief ������� ����� ����� ������ ������������ ���������� ����
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ����� �������: 
    ///          - \a runCommunication;
    ///          - \a setCommunicationData;
    ///          - \a getCommunicationData.
    ///     - �������� 2 - ���������� ����������� ������;
    ///     - �������� 3 - ��������� ���������� ������;
    ///     - �������� 4 - 0.
    ///
    ePS_IntChInputDriverGetError = 0x09,

    //*************************************************************************
    /// \brief �� ������ ������� ����� �����
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ����� �������:
    ///          - \a ctor;
    ///          - \a runCommunication.
    ///     - �������� 2 - ������������� ���������� ���������:
    ///          - ������ ��������� ������ ��������; 
    ///          - ������ ��������� ������� ������� ��������;
    ///          - ������ ��������� �� ������� \a isInReady; 
    ///          - ������ ��������� �� ������� \a get;
    ///          - ������ ��������� �� ������� \a isOutReady;
    ///          - ������ ��������� �� ������� \a set;
    ///          - ������ ��������� �� ������� \a run.
    ///     - �������� 3 - 0;
    ///     - �������� 4 - 0.
    ///
    ePS_IntChIoDriverError = 0x0A,

    //*************************************************************************
    /// \brief ��������� ����� �������� ���������� ������������� �� ����� �������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ������������� ���������;
    ///     - �������� 2 - \a m_operatingValue;
    ///     - �������� 3 - \a m_remoteValue;
    ///     - �������� 4 - \a m_localValue.
    ///
    ePS_IntChTime1Error = 0x0B,
    
    //*************************************************************************
    /// \brief ��������� ����� ������������ ������������������� ��������� T2
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ������������� ���������;
    ///     - �������� 2 - \a m_operatingValue;
    ///     - �������� 3 - \a m_remoteValue;
    ///     - �������� 4 - \a m_localValue.
    ///
    ePS_IntChTime2Error = 0x0C,
    
    //*************************************************************************
    /// \brief ��������� ����� ������� ����������� �������� �� ������������� T3
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ������������� ���������;
    ///     - �������� 2 - \a m_operatingValue;
    ///     - �������� 3 - \a m_remoteValue;
    ///     - �������� 4 - \a m_localValue.
    ///
    ePS_IntChTime3Error = 0x0D,        
  
    //*************************************************************************
    /// \brief ��� ��������� �������������� ������, ������ �������������� 
    /// ������ ��������������� �������.
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ������ ��������������� ������� 0, 1, 2;
    ///     - �������� 2 - ���������� �������������� ��������;
    ///     - �������� 3 - 0;
    ///     - �������� 4 - 0.
    ///
    ePS_IntChConTimeIndexError = 0x0E,

    //*************************************************************************
    /// \brief ����������� ������� ���� ������ ����������� \a CAN
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - 0;
    ///     - �������� 2 - 0;
    ///     - �������� 3 - 0;
    ///     - �������� 4 - 0.
    ///
    ePS_IntChTrPhase = 0x0F,

    //*************************************************************************
    /// \brief ������������ ��������� ������ ��� ���������� ������ �� 
    /// ����������� �������������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ������������� ���������;
    ///     - �������� 2 - �����;
    ///     - �������� 3 - ������;
    ///     - �������� 4 - 0.
    ///
    ePS_IntChCollision = 0x10,

    //*************************************************************************
    /// \brief ������� ������������ �������� �1
    /// \details ����� �1 �������������� ������ ��� �������� ������������� ��������� eScriptSync
    /// � ��� ����� �������� ������ ���� ����������� ������. ��� ������ ���������
    /// ����� �1 ������ ���� ������ ��� ����.
    /// \note ��� ������ � ���� ��������������� � ������ ���� ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ������������� ���������
    ///     - �������� 2 - �������� ������������� #InterChannelScript
    ///     - �������� 3 - �������� ������� �1
    ///     - �������� 4 - 0
    ///           
    ePS_IntChT1ValueError  = 0x11,
            
} ProtectionStateInterChannelId;

//eGrPS_CanDrv = 0x03 *********************************************************
/// \brief �������������� ������� ������ �������� \a CAN.
///
typedef enum
{
    ePS_CanDrvSetSize  = 1,    ///< ������, ������� ���������� �������� ������ ��� ������ ������ �����������
    ePS_CanDrvNumPort  = 2,    ///< ����������� ������ ���� ������ \a CAN 
    ePS_CanDrvSelf     = 3,    ///< ������������ ��������� �� ��������� � ������� ��������
    ePS_CanDrvErrorAlg = 4,    ///< ������ ����������
} ProtectionStateCanDrvId;

//eGrPS_CanAckDrv = 0x04 ******************************************************
/// \brief �������������� ������� ������ ���������� ��������� EcanAck_driverPIC33.h.
///
typedef enum
{
    ePS_CanAckDrvSetSize = 1,    ///< ������, ������� ���������� �������� ������ ��� ������ ������ �����������
    ePS_CanAckDrvNotAck  = 2,    ///< ���������� ������������� �� �������� ������
} ProtectionStateCanAckDrvId;

//eGrPS_BinIn = 0x05 **********************************************************
/// \brief �������������� ������ ���������� BinIn.h.
///
typedef enum
{
    /// \brief ������������� ������������ ����������� ���������� ������.
    /// \note ��� ������ � ���� ��������������� � ������ ���� ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ��������� ������ ����������� ��� ����� (���������������);
    ///     - �������� 2 - ��������� ��������� ������ ����������� ��� ����� (���������������);
    ///     - �������� 3 - 0;
    ///     - �������� 4 - 0.
    ///
    ePS_BinInErrorTestDecoder = 1,    ///< 
} ProtectionStateBinInId;

//eGrPS_AnalogInput = 0x06 ****************************************************
/// \brief �������������� ������� ���������� AnalogInput.h.
///
typedef enum
{
    /// \brief ������ �������������� ������������� �������������� ������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ����� �������:
    ///          - \a setChannelSettings;
    ///          - #AnalogInput_getData.
    ///     - �������� 2 - ������������� �������������� ������;
    ///     - �������� 3 - 0;
    ///     - �������� 4 - 0.
    ///
    ePS_AinErrorChannel = 1,

    /// \brief ������ �������������� �������� �������������� ��������� �������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ����� �������:
    ///          - \a setChannelSettings;
    ///          - #AnalogInput_getData;
    ///          - #AnalogInput_run.
    ///     - �������� 2 - ������������� �������������� ������;
    ///     - �������� 3 - ������������� ��������� �������������� ��������� �������;
    ///     - �������� 4 - 0.
    ///
    ePS_AinErrorScript = 2,

    //*************************************************************************
    /// \brief ������ �������������� ������������� �������
    /// \note ��� ������ � ���� ��������������� � "������ ����" �����������
    /// ��������� ���������:
    ///     - �������� 1 - ������������� �������;
    ///     - �������� 2 - ������� �����������;
    ///     - �������� 3 - ����� �������:
    ///          - #DigitalFilter_run;
    ///          - #AnalogInput_setChannelSettings.
    ///     - �������� 4 - 0.
    ///
    ePS_AinErrorDfId = 3,

    //*************************************************************************
    /// \brief ������� �������������� ������� �����������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ������������� �������;
    ///     - �������� 2 - ������� �����������;
    ///     - �������� 3 - 0;
    ///     - �������� 4 - 0.
    ///
    ePS_AinErrorDfFreq = 4,

    //*************************************************************************
    /// \brief ������� ��� �������� � ������� ������ ��������, ��� ���������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ����� �������:
    ///          - #AnalogInput_run.
    ///     - �������� 2 - ���������� ������, ����������� �� �������;
    ///     - �������� 3 - ��������� ���������� ������;
    ///     - �������� 4 - 0.
    ///
    ePS_AinErrorDriverGet = 5,

    //*************************************************************************
    /// \brief �������� #AVERAGE_WINDOW_SIZE ����� ������������ ��������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ����� �������:
    ///          - #AnalogInput_run.
    ///     - �������� 2 - AVERAGE_WINDOW_SIZE
    ///
    ePS_AinErrorIncorrectValueAWS = 6
} ProtectionStateAnalogInputId;

//eGrPS_AdcDrv = 0x07  ********************************************************
/// \brief �������������� ������� ������ �������� ADC.h.
///
typedef enum
{
    ePS_AdcOverflow   = 1,              ///< ������� �������� �������������
    ePS_AdcErrorDMA   = 2               ///< ������ � ������ DMA. �� ��� ���������� ��������� 
} ProtectionStateAdcDrvId;

//eGrPS_Eeprom = 0x08  ********************************************************
/// \brief �������������� ������� �������� \a EEPROM.
///
typedef enum
{
    ePS_EepromNoWork     = 1,    ///< \a EEPROM ����������
    ePS_EepromAccess     = 2,    ///< �� ��������� ���������� �������� � \a EEPROM
    ePS_EepromParameters = 3,    ///< ������ ���������� ������
    ePS_EepromInicLink   = 4     ///< ������ ��� ������������� ����� ����� � \a EEPROM
} ProtectionStateEepromId;

//eGrPS_ControlMK  = 0x09  ****************************************************
/// \brief �������������� ������� ���������� ControlMK.h.
///
typedef enum
{
    //*************************************************************************
    /// \brief ������������ ����������� ���������� �������� �� � ���������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ���������� ��������� ��������;
    ///     - �������� 2 - ��������� �������� ���������� ��������;
    ///     - �������� 3 - 0;
    ///     - �������� 4 - 0.
    ///
    ePS_ControlMkFault = 1,

    //*************************************************************************
    /// \brief ������ ��� �������� ����������� ����� Flash-������
    /// \note ��� ������ � ���� ��������������� ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ������������ �������� ����������� ����� Flash-������;
    ///     - �������� 2 - ��������� �������� ����������� ����� Flash-������;
    ///     - �������� 3 - 0;
    ///     - �������� 4 - 0.
    ///
    ePS_ControlMkFaultFlash = 2,

    //*************************************************************************
    /// \brief ������ ����������� ����� ���
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - 0;
    ///     - �������� 2 - 0;
    ///     - �������� 3 - 0;
    ///     - �������� 4 - 0.
    ///
    ePS_ControlMkFaultRAM = 3,

    //*************************************************************************
    /// \brief ������ ����������� ����� ��������� ������������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ������������ �������� ����������� ����� ��������� ������������;
    ///     - �������� 2 - ��������� �������� ����������� ����� ��������� ������������;
    ///     - �������� 3 - 0;
    ///     - �������� 4 - 0.
    ///
    ePS_ControlMkFaultConfig = 4,

    //*************************************************************************
    /// \brief ������ �������� ���������, ������������ ��� ������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - 0;
    ///     - �������� 2 - 0;
    ///     - �������� 3 - 0;
    ///     - �������� 4 - 0.
    ///
    ePS_ControlMkFaultRegisters = 5,

    //*************************************************************************
    /// \brief ������ ��� �������� ������� �������
    /// \note ��� ������ � ���� ��������������� � "������ ����" ����������� 
    /// ��������� ���������:
    ///     - �������� 1 - ������� �������� �������� ������� �������;
    ///     - �������� 2 - ��������� �������� �������� ������� �������;
    ///     - �������� 3 - 0;
    ///     - �������� 4 - 0.
    ///
    ePS_ControlMkFaultCallFunctions = 6,

    //*************************************************************************
    ePS_ControlMkFaultCPU = 7,    ///< ������ CPU

    //*************************************************************************
    ePS_InterruptCPU = 8          ///< ���������� �� ����� ����� ����������, ������ ���������� ������
} ProtectionStateControlMkId;

// eGrPS_Rs422 = 0x0B  ********************************************************
/// \brief ������ ������ ������ �� ���������� RS-422.
///
typedef enum
{
    ePS_Rs422StepCntError  = 1,    ///< ������ �������� ���������
    ePS_Rs422TimeOutError  = 2,    ///< ������ ��������
    ePS_Rs422SettingsError = 3     ///< ������ ��������� ����������
} ProtectionStateRs422;

//eGrPS_BlockExch = 0x0C *************************************************************
/// \brief ������ ������ BlockExch.h.
///
typedef enum
{
    ePS_BlockExchErrorDataLength = 1,    ///< ����������� ������ ����� ������ ������ 
    ePS_BlockExchErrorReceive    = 2,    ///< ������ ������������� ����������� ������
    ePS_BlockExchErrorStartSync  = 3     ///< ������ ��������� ������������� 
} ProtectionStateBlockExch;

//eGrPS_DeviceAddress = 0x0D **************************************************
/// \brief ������ ������ DeviceAddress.h.
///
typedef enum
{
    ePS_DeviceAddressFirstCheckError = 1,    ///< ������ ��� 1-�� �������� ���������
    ePS_DeviceAddressCheckError      = 2,    ///< ������ ��� ������������� �������� ���������
    ePS_DeviceAddressStepCntError    = 3     ///< ������ ���������� ��������� 
} ProtectionStateDeviceAddress;

//eGrPS_RelayCtrl = 0x0E  *****************************************************
/// \brief ������ ������ RelayCtrl.h.
///
typedef enum
{
    ePS_RelayCtrlStepCnt1error = 1,    ///< ������ �������� ��������� 1
    ePS_RelayCtrlStepCnt2error = 2,    ///< ������ �������� ��������� 2
    ePS_RelayCtrlStepCnt3error = 3,    ///< ������ �������� ��������� 3
    ePS_RelayCtrlStepCnt4error = 4     ///< ������ �������� ��������� 4
} ProtectionStateRelayCtrl;

//eGrPS_OverloadDetector = 0�10 ***********************************************
/// \brief �������������� ������ ������ ����������� ���������� �� ����. 
///
typedef enum
{
    ePS_OverloadDetectorHardwareFailurePower = 1,    ///< ���������� ������������� ������� �����
	
	//*****************************************************************************
	/// \brief ������������� ��������� �������� ��������� ����������� �� �������� ����������
	///
    ePS_OverloadDetectorStateCntSCError      = 2,
	
	//*****************************************************************************
	/// \brief ������������� ��������� �������� ��������� ����������� �� ������������ ����������
	///
    ePS_OverloadDetectorStateCntGenError     = 3,
	
    ePS_OverloadDetectorHardFailGen          = 4     ///< ���������� ������������� ������������ ����������
} ProtStateOverlDetect;

//eGrPS_PositionDet = 0x11  ***************************************************
/// \brief �������������� ������ ������ ����������� ���������.
///
typedef enum
{
    ePS_PosDetStepCnt1error       = 1,    ///< �������������� ��� ����� �������� ��������� �������
    ePS_PosDetGenStepCntError     = 2,    ///< ������ �������� ��������� �������������� ����������
    ePS_PosDetGenFeedBackContrGen = 3     ///< ����� �� ������������� ������� ���������� �������������� ����������
} ProtStatePositionDet;

//eGrPS_ShuntShift = 0x12  ****************************************************
/// \brief �������������� ������ ������ �������� �������.
///
typedef enum
{
    ePS_ShuntShiftTypeErr              = 1,    ///< �������������� ��� ��������� �������
    ePS_ShiftMotorCnt1Err              = 2,    ///< ������ �������� ��������� Cnt1
    ePS_ShiftMotorCnt2Err              = 3,    ///< ������ �������� ��������� Cnt2 
    ePS_ShuntShiftFiveEcCntErr         = 4,    ///< ������ �������� ���������� ��������� c ������� ���� 5-��
    ePS_ShuntShiftNineWireCntErr       = 5,    ///< ������ �������� ���������� ��������� c ������� ���� 9-�� ���������
    ePS_ShuntShiftFiveEcMismatchSetPos = 6,    ///< �������������� ������������� ������� ����������� ���������
    ePS_ShuntShiftFiveEcMismatchSetDir = 7,    ///< �������������� ������������� ������� �����������
    ePS_ShuntShiftFiveEcMotorNotStoped = 8,    ///< ��������� �� ����������.
    ePS_ShuntShiftGenStepCntErr        = 9,    ///< ������ �������� �������� ��������� ������ ShuntShiftGen.h
    /// \brief ������ �������� ����� �� ���������� 3-������� ���������� ��� ������ ���������
    ///
    ePS_ShiftMotorUfeedback                = 10
} ProtStateShuntShift;

// eGrPS_SafetyPowerControl = 0x13 (19d) **************************************
/// \brief ������ ������ ����������� ���������� ��������.
///
typedef enum
{
    ePS_SPC_PeriodicCheckFault = 1,    ///< ������ �������������� �������� ��������� ������
    ePS_SPC_TestCheckFault     = 2,    ///< ������ �������� ��������� �������� �����
    ePS_SPC_FSM_UnknownState   = 3     ///< ������ ������ �������� ��������� (����������� ���������)
} ProtectionStateSafetyPowerControl;

//ePS_IntegrCtrl = 0x14(20d)  ******************************************************
/// \brief �������������� ������ ������ �������� ����������� ������� ����� �������.
///
typedef enum
{
    ePS_IntegrCtrlStepCntError    = 1,    ///< ������ �������� ��������� ������ IntegrCtrl.h
    ePS_IntegrCtrlGenStepCntError = 2,    ///< ������ �������� ��������� ������ IntegrCtrlGen.h
    ePS_CorrectorIsFail           = 3,    ///< ������ ��� ������������ ���������� ������������ �������� 
    ePS_SafePhaseVoltage          = 4     ///< ������� ������� ���������� ��� ������������ ������� �����  
} ProtectionStateIntegrCtrl;

//0x15(21d)****************************************************************************
/// \brief �������������� ������ ���������� ControlSupplyVoltage.h
typedef enum 
{

    ePS_FaultU3_3V       = 1,                  ///< ������ ��� �������� ������� 3.3 �
                                             // ��� ������ � ���� ��������������� � ������ ���� ����������� 
                                             // ��������� ���������:
                                             // �������� 1 - ������� �������� ��������������� �������,
                                             // �������� 2 - ��������� �������� ��������������� �������,
                                             // �������� 3 - ������� ������, %,
                                             // �������� 4 - ������ ������, %.
                                             //

    ePS_FaultURef       = 2,                 ///< ������ ��� �������� �������� ������� ���
                                             // ��� ������ � ���� ��������������� � ������ ���� ����������� 
                                             // ��������� ���������:
                                             // �������� 1 - ������� �������� ��������������� �������,
                                             // �������� 2 - ��������� �������� ��������������� �������,
                                             // �������� 3 - ������� ������, %,
                                             // �������� 4 - ������ ������, %.
                                             //

    ePS_FaultBufferADC    = 3,               ///< ������ ��� �������� ��������� �������� ���
                                             // ��� ������ � ���� ��������������� � ������ ���� ����������� 
                                             // ��������� ���������:
                                             // �������� 1 - ������� �������� ��������������� �������,
                                             // �������� 2 - ��������� �������� ��������������� �������,
                                             // �������� 3 - ����� �������,
                                             // �������� 4 - 0.
                                             //
    } ProtectionStateControlSupplyVoltage;

#endif

//*****************************************************************************
/**
* ������� ���������:
*
* ������ 1.0.1
* ����   15-03-2017
* �����  ������� �.�.
* 
* ���������:
*    ������� ������.
* 
* ������ 2.0.1
* ����   20-05-2017
* �����  ��������� �.�.
*
* ���������:
*    ��������� ��� ����� ����� �� dsPIC33.
* 
* ������ 2.0.2
* ����   08-07-2019
* �����  ������� �.�. 
* 
* ���������:
*     1) �������� ����� ��� ��������� ������ ePS_IntChT1ValueError ��� 
*  ������ eGrPS_InterChannel;
* 
* ������ 2.0.3 
* ����   10-01-2020
* �����  ������ �.�.
* 
* ���������:
*   ��������� ���. ��������� ��� ���� ������ ePS_MainUnusedInterrupt.
* 
* ������ 2.0.4
* ����   15-03-2021
* �����  ������ �.�.
* 
* ���������:
* �������� ���� ������� ProtectionStateAdcDrvId, �.�. ��� �������� � DMA
*/
