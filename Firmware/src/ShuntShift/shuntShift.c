/** 
* \file    shuntShift.c
* \brief   \copybrief shuntShift.h
*
* \version 1.0.1
* \date    02-02-2018
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "shuntShift.h"
#include "shuntShiftTypes.h"
#include "shuntShiftFiveEC.h"
#include "shuntShiftNineWire.h"
#include "shuntShiftMotor.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"
#include "CheckCallFunctions.h"

//*****************************************************************************
// Объявление локальных типизированных констант
//*****************************************************************************
static const ArrayShuntShift *ptrShuntShift;        ///< Указатель на переменную структуры интерфейса.

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация
void ShuntShift_ctor( RelayModuleType relayType, MotorType motor )
{
    switch( relayType )
    {
        case eFiveWireEc:
            ptrShuntShift = &fiveWireEcShuntShift;
            break;
        case eNineWire:
            ptrShuntShift = &nineWireShuntShift;
            break;
        default:
            ERROR_ID( eGrPS_ShuntShift, ePS_ShuntShiftTypeErr );
    }
    ptrShuntShift->ctor( );
    ShuntShiftMotor_ctor( motor );
}

//*****************************************************************************
// Управление работой
void ShuntShift_run( void )
{
    ptrShuntShift->run( );
    ShuntShiftMotor_run( );
    MARKED_CALL_FUNCTION;
}

//*****************************************************************************
// Включение перевода стрелки
void ShuntShift_turnOn( bool dir, uint16_t time, bool st, uint8_t rs )
{
    ptrShuntShift->turnOn( dir, time, st, rs );
}

//*****************************************************************************
// Выключение перевода стрелки
void ShuntShift_turnOff( void )
{
    ptrShuntShift->turnOff( );
}

//*****************************************************************************
// Передача положения стрелки
void ShuntShift_setDetPos( PositionDet_State detPos, PositionDet_State pos )
{
    ptrShuntShift->setDetPos( detPos, pos );
}

//*****************************************************************************
// Передача установленной позиции перевода
void ShuntShift_setShiftPos( ShiftDirection shiftPos )
{
    ptrShuntShift->setShiftPos( shiftPos );
}

//*****************************************************************************
// Проверка состояния включения
bool ShuntShift_isEnable( void )
{
    return ptrShuntShift->isEnable( );
}

//*****************************************************************************
// Запрос на установку позиции контроля
PositionDet_State ShuntShift_getReqPosDet( void )
{
    return ptrShuntShift->getReqPosDet( );
}

//*****************************************************************************
// Запрос на установку позиции направления перевода
ShiftDirection ShuntShift_getReqShiftDir( void )
{
    return ptrShuntShift->getReqShiftDir( );
}

//*****************************************************************************
// Получить время последнего перевода
uint16_t ShuntShift_getTime( void )
{
    return ptrShuntShift->getTime( );
}

//*****************************************************************************
// Получить состояние перевода
ShiftDgn ShuntShift_getDgn( void )
{
    return ptrShuntShift->getDgn( );
}

//*****************************************************************************
// Установить значение счётчика фазы стрелочного перевода.
void ShuntShift_setCntPhaseShift( const uint8_t val )
{
    ptrShuntShift->setCntPhaseShift( val );
}

//*****************************************************************************
// Получить значение счётчика фазы стрелочного перевода.
const uint8_t ShuntShift_getCntPhaseShift( void )
{
    return ptrShuntShift->getCntPhaseShift();
}

//*****************************************************************************
// Проверка того, что модуль прошёл режим инициализации.
const bool ShuntShift_isWorkMode( void )
{
    return ptrShuntShift->isWorkMode;
}

void ShuntShift_setDgn( ShiftDgn val )
{
    ptrShuntShift->setDgn( val );
}

void ShuntShift_setTime( uint16_t val )
{
    ptrShuntShift->setTimeShift( val ); 
}


//*****************************************************************************
/**
* История изменений:
*
* Версия 1.0.1
* Дата   02-02-2018
* Автор  Кругликов В.П.
*
* Изменения:
*    Базовая версия.
*/
