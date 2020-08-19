/**
* \file    IntegrCtrlGen.c
* \brief   \copybrief IntegrCtrlGen.h 
*
* \version 1.0.1
* \date    02-02-2018
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdbool.h>
#include "IntegrCtrlGen.h"
#include "shuntShiftGenDrv.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"


//*****************************************************************************
// ��������� ���������, ������������ ����� �������
//***************************************************************************** 

//*****************************************************************************
#define PWM_PERIOD_INT_CTRL_GEN          4208U                          ///< �������� PTPER ��� 28 ��� ��� ����������.
#define MAX_PDC_VALUE       4208U                          ///< �������� PDC (������������ ���), ��������������� 100%.

//***************************************************************************** 
/// \brief ����������� ���������� ������� ������� ��� ������������ �������� ����������� (� ��������� �� #MAX_PDC_VALUE).
///
#define MAX_LEVEL_INT_CTRL  ( 78 * MULTIPLY_VOLTAGE_STEP_INT_CTRL )

//***************************************************************************** 
/// \brief ����������� ���������� ������� ������� ��� ������������ ���������� (� ��������� �� #MAX_PDC_VALUE).
///
#define MAX_LEVEL_TEST_CORR  ( 78 * MULTIPLY_VOLTAGE_STEP_TEST_CORR )

//*****************************************************************************
/// \brief ���������� ���������� ������� ������� (� ��������� �� #MAX_PDC_VALUE).
///
#define MIN_LEVEL_GEN           0

//*****************************************************************************
#define NUM_TESTING_MODES   2                             ///< ���������� ������� ������������.

//***************************************************************************** 
/// \brief  ������ ����� ������ ������������� � ���������.
///
#define MAX_SCALE_VALUE_INT_CTRL    ( 100 * MULTIPLY_VOLTAGE_STEP_INT_CTRL ) 

//***************************************************************************** 
/// \brief ������ ����� ������ ������������� � ���������.
///
#define MAX_SCALE_VALUE_TEST_CORR   ( 100 * MULTIPLY_VOLTAGE_STEP_TEST_CORR )

//*****************************************************************************
// ��������� �������������� ���������
//***************************************************************************** 
static const uint8_t NUM_OF_PHASES = 3;    ///< ���������� ��� �������.

//*****************************************************************************
// ����������� ����� ������
//***************************************************************************** 

//***************************************************************************** 
/// \brief �������� �������� ��������� ������� #IntegrCtrlGen_run.
///
typedef enum
{
    eSwitchedOff = 0,        ///< ��������
    eSwitchedOn,             ///< �������
} IntDetGen;

//***************************************************************************** 
/// \brief ������ ����� ������� (��� ������������ ��� � ��� ������������ ����������).
///
const uint16_t IntegrCtrlGen_maxScales[NUM_TESTING_MODES] =
{
    MAX_SCALE_VALUE_INT_CTRL,
    MAX_SCALE_VALUE_TEST_CORR
};

//***************************************************************************** 
/// \brief ��������� ������� (��� ������������ ��� � ��� ������������ ����������).
///
const uint16_t IntegrCtrlGen_maxLevels[NUM_TESTING_MODES] =
{
    MAX_LEVEL_INT_CTRL, 
    MAX_LEVEL_TEST_CORR
};

//*****************************************************************************
// ����������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static uint16_t level;          ///< ������� ���������� � ���������.

//*****************************************************************************
static Phase phase;        ///< ������� ���� 3-������� �������.

//*****************************************************************************
static bool ctrl;          ///< ����������/������ ������.


static uint8_t  stepCnt;        ///< �������� �������� ��������� \a IntDetGen.
//*****************************************************************************
static int16_t valueArr[3];        ///< ������ �������� ������������ ����.

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ����������
void IntegrCtrlGen_ctor( void )
{
    ShuntShiftGenDrv_off( ); // ���������� ���, ���������� ������� ��� IO Ports
    ctrl = 0;                // ������ ������ ���������� 
    level = 0;               // ���������� ��������� ������� = 0
    stepCnt = eSwitchedOff;  // ��������� ������������� �������� ��������� � ��������� ����
}

//*****************************************************************************
// ���������� ������� ������������� ������� �������� ����������� �������
void IntegrCtrlGen_run( void )
{
    uint8_t counter;
    static uint8_t timeCnt;

    if( timeCnt ) timeCnt--;
    switch( stepCnt )
    {
        case eSwitchedOff: //������������� ��������, �������� ���������, ������������� ������������� ����
            if( ctrl )
            {
                for( counter = 0; counter < NUM_OF_PHASES; counter++ ) valueArr [counter] = 0;
                ShuntShiftGenDrv_on( PWM_PERIOD_INT_CTRL_GEN, valueArr );
                stepCnt = eSwitchedOn;
            }
            break;
        case eSwitchedOn: //������������� �������, �������� ����������, ���������� ������������� ���� 
            if( !ctrl )
            {
                ShuntShiftGenDrv_off( );
                stepCnt = eSwitchedOff;
            }
            else 
                ShuntShiftGenDrv_setValues( valueArr );
            break;
        default: //������ �������� ���������
            ERROR_ID( eGrPS_IntegrCtrl, ePS_IntegrCtrlGenStepCntError );
            stepCnt = eSwitchedOff;
    }
}

//*****************************************************************************
// ��������� ���������� �������� ����������� �������
void IntegrCtrlGen_turnOn( void )
{
    ctrl = true;
}

//*****************************************************************************
// ��������� ���������� ����������
void IntegrCtrlGen_setParam( Phase phVal, uint16_t lvlVal, IntDetTypeTesting type )
{
    phase = phVal;
    if( phVal == eNone )
    {
        level = MIN_LEVEL_GEN;
        return;
    }
    if( lvlVal > IntegrCtrlGen_maxLevels[type] ) //����������� �� ��������� �������
        level = IntegrCtrlGen_maxLevels[type];
    else if( lvlVal < MIN_LEVEL_GEN ) //����������� �� �������� �������
        level = MIN_LEVEL_GEN;
    else
        level = lvlVal;
    valueArr[phase] = __builtin_divsd( __builtin_mulsu( MAX_PDC_VALUE, level ), IntegrCtrlGen_maxScales[type] );
}

//*****************************************************************************
// ���������� ������������� ������� �������� ����������� �������
void IntegrCtrlGen_turnOff( void )
{
    ctrl = false;
}

//*****************************************************************************
/**
* ������� ���������: 
*
* ������ 1.0.1
* ����   02-02-2018
* �����  ��������� �.�.
*
* ���������:
*    ������� ��������.
*/