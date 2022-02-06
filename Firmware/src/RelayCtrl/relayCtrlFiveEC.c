/**
* \file    relayCtrlFiveEC.c
* \brief   \copybrief relayCtrlFiveEC.h
*
* \version 1.0.1
* \date    12-12-2017
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "relayCtrlFiveEC.h"
#include "relayCtrlDrv.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************
#define FIVE_WIRE_SWITCH_TO 200        ///< Тайм-аут переключения.

//*****************************************************************************
// Объявление глобальных типизированных констант
//*****************************************************************************

//*****************************************************************************
/// \brief Переменная структуры интерфейса.
///
const ArrayRelayCtrl fiveWireECRelCtrl =
{
    FiveWireECRelCtrl_ctor,
    FiveWireECRelCtrl_run,
    FiveWireECRelCtrl_setShiftDir,
    FiveWireECRelCtrl_setPosDet,
    FiveWireECRelCtrl_isSwitched,
    FiveWireECRelCtrl_getShiftDir,
    FiveWireECRelCtrl_getPosDet
};

//*****************************************************************************
// Объявление локальных переменных
//*****************************************************************************
static ShiftDirection    direct;           ///< Направление перевода.
static PositionDet_State position;         ///< Контроль положения.
static uint16_t          switchCnt;        ///< Счетчик времени переключения.

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация
void FiveWireECRelCtrl_ctor( void )
{
    RelayCtrlDrv_switchPhasesAB( false );     //Обесточить реле подключения фаз (фазы к L3, L4)
    RelayCtrlDrv_switchReceiverL1L2( false ); // Отключить приемник от L1,L2
    RelayCtrlDrv_switchReceiverL3L4( false ); // Отключить приемник от L3,L4
    switchCnt = FIVE_WIRE_SWITCH_TO;          // Взвести счетчик переключения
    direct = eShiftStop;                      // Направление перевода - останов
    position = eDetNon;                       // Контроль положения - отключено
}

//*****************************************************************************
// Управление работой
void FiveWireECRelCtrl_run( void )
{
    if( switchCnt ) switchCnt--; // Декремент счетчика переключения
}

//*****************************************************************************
// Установить направление перевода.
void FiveWireECRelCtrl_setShiftDir( ShiftDirection shift )
{
    direct = shift; //Установить требуемое направление перевода

    switch( shift )
    {
        case eShiftStop: //Нет перевода
            RelayCtrlDrv_switchPhasesAB( false ); //Обесточить реле подключения фаз (фазы к L3, L4)
            break;

        case eShiftPlus:
            RelayCtrlDrv_switchPhasesAB( false ); //Подключить фазы к L3, L4
            break;

        case eShiftMinus:
            RelayCtrlDrv_switchPhasesAB( true ); //Подключить фазы к L1, L2
            break;

        default:
            ERROR_ID( eGrPS_RelayCtrl, ePS_RelayCtrlStepCnt3error );
    }
    switchCnt = FIVE_WIRE_SWITCH_TO; //Установить тайм-аут переключения   
}

//*****************************************************************************
// Установить контроль положения
void FiveWireECRelCtrl_setPosDet( PositionDet_State pos )
{
    position = pos; //Установить требуемое контролируемое положение

    switch( pos )
    {
        case eDetNon: //Отключить контроль положения
            RelayCtrlDrv_switchReceiverL1L2( false ); //Отключить приемник от L1, L2
            RelayCtrlDrv_switchReceiverL3L4( false ); //Отключить приемник от L3, L4
            break;

        case eDetPlus: //Контроль "Положение плюс" (приемник к L3, L4)
            RelayCtrlDrv_switchReceiverL1L2( false ); //Отключить приемник от L1, L2
            RelayCtrlDrv_switchReceiverL3L4( true ); //Подключить приемник от L3, L4
            break;

        case eDetMinus: //Контроль "Положение минус" (приемник к L3, L4)
            RelayCtrlDrv_switchReceiverL1L2( true ); //Подключить приемник к L1, L2
            RelayCtrlDrv_switchReceiverL3L4( false ); //Отключить приемник от L3, L4
            break;

        default:
            ERROR_ID( eGrPS_RelayCtrl, ePS_RelayCtrlStepCnt4error );
    }
    switchCnt = FIVE_WIRE_SWITCH_TO; //Установить тайм-аут переключения  
}

//*****************************************************************************
// Получает завершение установки
bool FiveWireECRelCtrl_isSwitched( void )
{
    return switchCnt == 0;
}

//*****************************************************************************
// Получает установленное направление перевода
ShiftDirection FiveWireECRelCtrl_getShiftDir( void )
{
    return direct;
}

//*****************************************************************************
// Получает контролируемое положение
PositionDet_State FiveWireECRelCtrl_getPosDet( void )
{
    return position;
}

//*****************************************************************************
/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   12-12-2017
* Автор  Кругликов В.П.
* 
* Изменения:
*     Базовая версия.
*/
