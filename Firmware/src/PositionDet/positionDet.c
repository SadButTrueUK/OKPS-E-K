/**
* \file    positionDet.c
* \brief   \copybrief positionDet.h
*
* \version 1.0.1
* \date    28-12-2017
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "positionDet.h"
#include "positionDetTypes.h"
#include "positionDetFiveEC.h"
#include "positionDetNineWire.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"
#include "posDetGenerator.h"

//*****************************************************************************
// Определение локальных типизированных констант
//*****************************************************************************
static const ArrayPositionDet *ptrPosDet;        ///< Указатель на переменную структуры интерфейса.

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация
void PositionDet_ctor( RelayModuleType relayType )
{
    switch( relayType )
    {
        case eFiveWireEc:
            ptrPosDet = &fiveWireECPosDet;
            break;
        case eNineWire:
            ptrPosDet = &nineWirePosDet;
            break;
        default:
            ERROR_ID( eGrPS_PositionDet, ePS_PosDetStepCnt1error );
    }
    ptrPosDet->ctor( );
    PosDetGenerator_ctor( );
    PositionDet_turnOn( true ); // включаем контрольный генератор сразу со старта
}

//*****************************************************************************
// Управление работой
void PositionDet_run( void )
{
    ptrPosDet->run( );
    PosDetGenerator_run( );
    PosDetGenerator_checkUfb( );
}

//*****************************************************************************
// Включение/выключение определения положения
void PositionDet_turnOn( bool on )
{
    ptrPosDet->turnOn( on );
    PosDetGenerator_turnOn( on );
}

//*****************************************************************************
// Проверка состояния модуля
bool PositionDet_isEnable( void )
{
    return ptrPosDet->isEnable( ) &&
            PosDetGenerator_isEnable( );
}

//*****************************************************************************
// Получает установленное положение
PositionDet_State PositionDet_getPosition( void )
{
    return ptrPosDet->getPos( );
}

//*****************************************************************************
// Получает запрос на установку позиции контроля положения
PositionDet_State PositionDet_getRequestPositionDet( void )
{
    return ptrPosDet->getReqPosDet( );
}

//*****************************************************************************
// Передает установленную позицию контроля
void PositionDet_setPosDet( PositionDet_State pos )
{
    ptrPosDet->setPosDet( pos );
}

//*****************************************************************************
/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   28-12-2017
* Автор  Кругликов В.П.
* 
* Изменения:
*     Базовая версия.
*/
