/**
* \file    CheckCallFunctions.c
* \brief   \copybrief CheckCallFunctions.h
*
* \version 1.0.4
* \date    11-06-2019
* \author  ��������� �.�., ������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include "asserts_ex.h"
#include "Eeprom.h"
#include "InterChannel.h"
#include "CheckCallFunctions.h"

//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//*****************************************************************************
#define N_STACK_NAME_FUNCTION  8        ///< ������ ������� ���������� ���������� �������.

//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ������� �������� ������������� ���������� ������ ControlMK.h, ��.
///
typedef enum  {
    eOUT_T1 = 10,        ///< ������������ ����� ��������� ������� ���������
    eOUT_T2 = 20,        ///< ������������ ����� ������������ ����������
    eOUT_T3 = 128        ///< ������������ ����� ���������� ���������
} CheckCF_timeSynchr;

//*****************************************************************************
/// \brief ��������� ���������.
///
typedef struct CheckCallFunctions_Tag {
     uint16_t    cCallFunctions;                                 ///< ������� ������� �������.
     uint16_t    cCallFunctionsSync;                             ///< ���������������� �������� �������.
     const char *aNameFunction[N_STACK_NAME_FUNCTION];           ///< ��������� �� ������ ���������� �������.
     uint8_t     ndx;                                            ///< ������ ������� ���������� ���������� �������. 
     uint8_t     cPeriod;                                        ///< ������� ������� ��������.
} CheckCallFunctions; 

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static CheckCallFunctions checkCallFunctions;          ///< ���������� ��������� ���������.

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// �������������
void CheckCallFunctions_ctor(void) 
{
    checkCallFunctions.cCallFunctions = 0U;
    checkCallFunctions.ndx            = 0U;
    checkCallFunctions.cPeriod        = 0U;
     
    InterChannel_setParamSettings( eICId_ControlMK_CF, eScriptSync, eProcSyncEqual, eProcCheckEqual, 0, 0,
                                   eOUT_T1, eOUT_T2, eOUT_T3);
}

//*****************************************************************************
// ����� ������� � ����������� �� �����
void CheckCallFunctions_call(void (*pFunction)(void), const char *ac)
{
    if (checkCallFunctions.ndx >= N_STACK_NAME_FUNCTION) {
        checkCallFunctions.ndx  = N_STACK_NAME_FUNCTION - 1;
    }
    checkCallFunctions.aNameFunction[checkCallFunctions.ndx++] = ac;
    pFunction();
    if (checkCallFunctions.ndx > 0) {
        checkCallFunctions.ndx--;
    }
}

//*****************************************************************************
//  �������� ���������� ��������� �������
void CheckCallFunctions_run(void) 
{
    if ((++checkCallFunctions.cPeriod  >= (eOUT_T3/2)) &&  !InterChannel_isHandling(eICId_ControlMK_CF)) {
        checkCallFunctions.cPeriod = 0U; 
        InterChannel_synchronize(eICId_ControlMK_CF, (param_t)(checkCallFunctions.cCallFunctionsSync));
    }

     MARKED_CALL_FUNCTION;
}

//*****************************************************************************
// �������� ��� �������� ���������� ��������� �������  
void CheckCallFunctions_interrupt(bool sync) {
     if (sync) {
          checkCallFunctions.cCallFunctionsSync = checkCallFunctions.cCallFunctions;
     }
}

//*****************************************************************************
// ������������ ���������� �������
void CheckCallFunctions_marked(void)
{
    checkCallFunctions.cCallFunctions++;
}

//*****************************************************************************
// ������ ��������� #N_STACK_NAME_FUNCTION ���� ������� � EEPROM
uint16_t CheckCallFunctions_saveBlackBox(uint16_t address)
{
    Eeprom_write(address, checkCallFunctions.ndx); 
    address += 2;

    const char * addressChar;
    uint8_t char1, char2, ndx = 0;

    while (ndx != checkCallFunctions.ndx) {
        addressChar = checkCallFunctions.aNameFunction[ndx++];
        while(true) {
            char1  = *addressChar++; 
            if (char1 == 0) {
                char2  = 0;
            } else {
                char2  = *addressChar++;
            }   
            Eeprom_write(address, ((uint16_t)(char1) << 8) | char2); 
            address += 2;
            if ((char1 == 0) || (char2 == 0)) {
                break;
            }
        } 
    }

    return (address);  
}

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����:  11-07-2016
* �����: ��������� �.�.
* 
* ���������:
*     ������� ������.
*
* ������ 1.0.2
* ����   29-09-2017
* �����  ��������� �.�.
* 
* ���������:
*     ��������� �������  CheckCallFunctions_interrupt(bool sync)
*     ��� �������� ���������� ������� � ����������, ��������� ���������� 
*     cCallFunctionsSync � ��������� checkCallFunctions ��� ����������
*     ���������� ���������� ������� � 15-� ����������.
*
* ������ 1.0.3
* ����   24-05-2019
* �����  ������� �.�.
* 
* ���������:
*     ��� ��������� eICId_ControlMK_CF ������� �������� �������������
*     � eScriptChVal �� eScriptSync.
*
* ������ 1.0.4
* ����   11-06-2019
* �����  ������� �.�.
* 
* ���������:
*    ��� �������� ������ ������� CheckCallFunctions_run � ��� �������� ������ MARKED_CALL_FUNCTION.
*/
