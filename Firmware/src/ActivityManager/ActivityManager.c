/**
* \file    ActivityManager.c
* \brief   \copybrief ActivityManager.h
* 
* \version 1.0.1
* \date    18-05-2017
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "ActivityManager.h"
#include "ActivityManager_dataTypes.h"
#include "relayCtrl.h"
#include "DebugTools.h"

//*****************************************************************************
// Определение локальных переменных
//*****************************************************************************

//*****************************************************************************
static ActivityManager_DeviceStr deviceStr;        ///< Структура состояния модуля
static uint16_t timeoutChAct;                      ///< таймаут для смены активности по приказу от УС 

//*****************************************************************************
// Определение типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Изменение состояния при переводе.
///
typedef enum
{
    eNoAction,         ///< нет действия
    eSetActivePrimary, ///< установить активным основной прибор
    eSetActiveReserve, ///< установить активным резервный прибор
    eReserved          ///< зарезервировано (нет действия)    
} ChangeActivityState;

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Обработка состояния "Пассивный".
/// \param str - указатель на структуру состояния.
/// \return Идентификатор управления.
///
static ActivityManager_States passiveProcessing( ActivityManager_DeviceStr *str );

//*****************************************************************************
/// \brief Обработка состояния "Попытка перехода в активное состояние".
/// \param str - указатель на структуру состояния.
/// \return Идентификатор управления.
///
static ActivityManager_States transitionToActive( ActivityManager_DeviceStr *str );

//*****************************************************************************
/// \brief Обработка состояния "Активный".
/// \param str - указатель на структуру состояния.
/// \return Идентификатор управления.
///
static ActivityManager_States activeProcessing( ActivityManager_DeviceStr *str );

//*****************************************************************************
/// \brief Обработка состояния "Попытка перехода в пассивное состояние".
/// \param str - указатель на структуру состояния.
/// \return Идентификатор управления.
///
static ActivityManager_States transitionToPassive( ActivityManager_DeviceStr *str );

//*****************************************************************************
/// \brief Обработка таймаута смены активности в БС.
/// \param str - указатель на структуру служебных таймаутов.
/// \retval true - обработка закончена;
/// \retval false - обработка не закончена.
///
static bool timeoutProcessing( ActivityManager_TimeoutStr *str );

//*****************************************************************************
/// \brief Сброс таймаута смены активности в БС.
/// \param str - указатель на структуру таймаутов.
///
static void timeoutReset( ActivityManager_TimeoutStr *str );

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация
ActivityManager_DeviceStr *ActivityManager_ctor( bool status )
{
    ActivityManager_DeviceStr *str = &deviceStr;
    
    //Установить статус прибора
    str->status = status;
    //Установить флаг инициализации
    str->ini = true;
    //Выключить модуль
    str->ctrl = false;
    str->initIsActive = false; //инициализация прибора не заврешена
    return str;
}

bool ActivityManager_getInitDeviceState( ActivityManager_DeviceStr *str )
{
    return str->initIsActive;
}

// Смена активности по приказу от управляющей системы.
void ActivityManager_changeActivity( ActivityManager_DeviceStr *str, uint8_t value )
{
    
    str->changeActivity = ( str->outputData.str.activityState       //если прибор активный
                            && ( ( ( value == eSetActivePrimary ) && !str->status ) ||  //если прибор резервный, получена команда стать активным основному
                               ( ( value == eSetActiveReserve )  && str->status ) ) );  //если прибор основной, активный, получена команда стать активным резервному
}

//*****************************************************************************
// Управление работой модуля
void ActivityManager_run( ActivityManager_DeviceStr *str )
{                                                                                              
    
	if( !str->ctrl || !str->ini ) return;

    // Декремент таймаутов
#ifdef ENABLE_UNIFORM_LOAD_TIMER 
    if( str->uniformLoadTimeCnt ) 
        str->uniformLoadTimeCnt--;
#endif
    if( str->transitionTimeCnt ) 
        str->transitionTimeCnt--;
    
    //Сброс таймаута смены активности в БС
    timeoutReset( & str->timeout );
    
    switch( str->runCnt )
    {
        case eRunStart:                                   //Включение (был сброс)
            if( str->transitionTimeCnt == 0)
            {
                str->initIsActive = true;  //инициализация прибора завершена
                if( str->inputData.str.neighborConnection == 0 && str->status )
                    str->runCnt = eRunTransitionToActive;
                else
                    str->runCnt = eRunPassiveState;
            }
            break;
        case eRunPassiveState:                            //Пассивное состояние
            str->runCnt = passiveProcessing( str );
            break;
        case eRunTransitionToActive:                      //Переход в активное состояние
            str->runCnt = transitionToActive( str );
            break;
        case eRunActiveState:                             //Активное состояние
            str->runCnt = activeProcessing( str );
            break;
        case eRunTransitionToPassive:                     //Переход в пассивное состояние
            str->runCnt = transitionToPassive( str );
            break;
    }
}

//*****************************************************************************
// Включение / выключение модуля
void ActivityManager_setEnable( ActivityManager_DeviceStr *str, bool enable )
{
    static const uint16_t TIMEOUT_START_CHANGE_ACTIVITY = T_O_START_STANDBY - T_O_START_MASTER;
    
    
    if( enable )
    {   //Включение модуля
        //Инициализация входных данных
        str->inputData.data = 0;

        //Инициализация выходных данных
        str->outputData.data = 0;

        //Инициализация счетчиков состояния
        str->runCnt = eRunStart;
        str->trToActCnt = eTrToActStart;
        str->trToPassCnt = eTrToPassWaitDisableCtrl;

        //Инициализация счетчиков таймеров
    #ifdef ENABLE_UNIFORM_LOAD_TIMER    
        str->uniformLoadTimeCnt = 0;
    #endif
        if( str->status )
        { //Основной
            str->transitionTimeCnt = T_O_START_MASTER;
            timeoutChAct = TIMEOUT_START_CHANGE_ACTIVITY;
        }
        else
        { //Резервный
            str->transitionTimeCnt = T_O_START_STANDBY;
            timeoutChAct = 0;
        }
        str->timeout.changeSafeActivityCnt = 0;
        str->timeout.changeSafeActivityRun = 0;
    }
    str->ctrl = enable;
}

//*****************************************************************************
// Установка состояния прибора
void ActivityManager_setState( ActivityManager_DeviceStr *str, bool state )
{
    str->inputData.str.ownState = state;
}

//*****************************************************************************
// Установка состояния контактов реле РПВ
void ActivityManager_setRPVoutState( ActivityManager_DeviceStr *str, bool state )
{
    str->inputData.str.relayRPVcontactsState = state;
}

//*****************************************************************************
// Установка наличия активной команды
void ActivityManager_setActiveCommand( ActivityManager_DeviceStr *str, bool presence )
{
    str->inputData.str.ownActiveCommand = presence;
}

//*****************************************************************************
// Установка состояния связи по интерфейсу RS-422
void ActivityManager_setRS422connect( ActivityManager_DeviceStr *str, Rs422_numLine line, bool connect )
{
    if( line == eRs422_line_1 ) str->rs422.str.ownLine1connect = connect;
    else if( line == eRs422_line_2 ) str->rs422.str.ownLine2connect = connect;
}

//*****************************************************************************
// Установка состояния связи с УС по интерфейсу RS-422
void ActivityManager_setRS422ctrlSysConnect( ActivityManager_DeviceStr *str, Rs422_numLine line, bool connect )
{
    if( line == eRs422_line_1 ) str->rs422.str.ownLine1CtrlSystemConnect = connect;
    else if( line == eRs422_line_2 ) str->rs422.str.ownLine2CtrlSystemConnect = connect;
}

//*****************************************************************************
// Получает состояние активности прибора
bool ActivityManager_isActive( ActivityManager_DeviceStr *str )
{
    return str->outputData.str.activityState;
}

//*****************************************************************************
// Получает состояние связи соседа с УС
bool ActivityManager_getRS422neighborCtrlSysConnect( ActivityManager_DeviceStr *str, Rs422_numLine line )
{
    if( line == eRs422_line_1 ) return str->rs422.str.neighborLine1CtrlSystemConnect;
    else if( line == eRs422_line_2 ) return str->rs422.str.neighborLine2CtrlSystemConnect;
    else return false;
}

//*****************************************************************************
// Получает состояние управления реле
bool ActivityManager_isRelayCtrlOn( ActivityManager_DeviceStr *str )
{
    return str->outputData.str.relayCtrlCommand;
}

//*****************************************************************************
// Получает состояние соседа 
bool ActivityManager_isNeighborWorking( ActivityManager_DeviceStr *str )
{
    return str->inputData.str.neighborState;
}

//*****************************************************************************
// Получает состояние связи с соседом
bool ActivityManager_getNeighborConnection( ActivityManager_DeviceStr *str )
{
    return str->inputData.str.neighborConnection;
}

//*****************************************************************************
// Реализация локальных функций
//*****************************************************************************

//*****************************************************************************
// Обработка состояния "Пассивный"
ActivityManager_States passiveProcessing( ActivityManager_DeviceStr *str )
{
    if( str->transitionTimeCnt )
    {
        //Проверка таймаута после перехода прибора из активного в пассивное состояние
        return eRunPassiveState;
    }
    //Обработка смены состояний РС, БС закончена            
    //Если Прибор в безопасном состоянии
    if( str->inputData.str.ownState == 0 )
    {
        //Если Нет связи по RS-422
        if( str->rs422.str.ownLine1connect == 0 && str->rs422.str.ownLine2connect == 0 )
        {
            //Если Нет связи с соседом
            if( str->inputData.str.neighborConnection == 0 )
            {
                return eRunTransitionToActive;
            }
            //Есть связь с соседом            
            //Если Сосед в безопасном состоянии
            if( str->inputData.str.neighborState == 0 )
            {
                //Если У соседа нет связи по RS-422
                if( str->rs422.str.neighborLine1connect == 0 && str->rs422.str.neighborLine2connect == 0 )
                {
                    //Если Сосед пассивный
                    if( str->inputData.str.neighborActivity == 0 )
                    {
                        //Попытка стать активным 2
                        if( timeoutProcessing( &str->timeout ) )
                        {
                            //str->outputData.str.activityState = 1; //06.08.19 Kruglikov
                        #ifdef ENABLE_UNIFORM_LOAD_TIMER
                            str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                        #endif
                            //return eRunActiveState; //изменено 18.06 КВП
                            return eRunTransitionToActive;
                        }
                    }//Если Сосед пассивный

                    //Если Сосед активный
                    return eRunPassiveState;
                }//Есть связь с соседом

                //У соседа есть связь с УС
                return eRunPassiveState;
            }//Если Сосед в безопасном состоянии

            //Если Сосед в рабочем состоянии
            return eRunPassiveState;
        }//Если Нет связи по RS-422

        //Если Есть связь с УС по RS-422        
        //Если Нет связи с соседом
       
        if( str->inputData.str.neighborConnection == 0 )
        {
            return eRunTransitionToActive;
        }//Если Нет связи с соседом

        //Если Есть связь с соседом
        //Если Сосед в безопасном состоянии
        if( str->inputData.str.neighborState == 0 )
        {
            //Если У соседа нет связи по RS-422
            if( str->rs422.str.neighborLine1connect == 0 && str->rs422.str.neighborLine2connect == 0 )
            {
                //Попытка стать активным 2
                if( timeoutProcessing( &str->timeout ) )
                {
                    //str->outputData.str.activityState = 1;  //6.08.19 Kruglikov
                #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                    str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                #endif    
                    //return eRunActiveState;//изменено 18.06 КВП
                    return eRunTransitionToActive;
                }
            }
            //Если У соседа есть связь по RS-422
            //Если Сосед пассивный
            if( str->inputData.str.neighborActivity == 0 )
            {
                //Попытка стать активным 2
                if( timeoutProcessing( &str->timeout ) )
                {
                    //str->outputData.str.activityState = 1; //06.08.19 Kruglikov
                #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                    str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                #endif    
                    //return eRunActiveState;//изменено 18.06 КВП
                    return eRunTransitionToActive;
                }
            }
            //Если Сосед активный
            return eRunPassiveState;
        }//Если Сосед в безопасном состоянии

        //Сосед в рабочем состоянии
        return eRunPassiveState;
    }//Если Прибор в безопасном состоянии
    
    //Если Прибор в рабочем состоянии
    //Если Нет связи с соседом
    if( str->inputData.str.neighborConnection == 0 )
    {
        return eRunTransitionToActive;
    }
    //Если Есть связь с соседом
    //Если Сосед в безопасном состоянии
    if( str->inputData.str.neighborState == 0 )
    {
        return eRunTransitionToActive;
    }
    //Если Сосед в рабочем состоянии
    //Если Сосед пассивный
    if( str->inputData.str.neighborActivity == 0 )
    {
        return eRunTransitionToActive;
    }
    //Если Сосед активный
    return eRunPassiveState;
}

//*****************************************************************************
// Обработка состояния "Попытка перехода в активное состояние"
ActivityManager_States transitionToActive( ActivityManager_DeviceStr *str )
{
    ////!!! 8.8.19 Kruglikov
    //Стать активным БЕЗУСЛОВНО если неисправно РПВ
    if( RelayCtrl_getRpvState() != eNormal )
    {
        str->outputData.str.activityState = 1; //Теперь активный
        return eRunActiveState;
    }
    else
    {    
        switch( str->trToActCnt )
        {
            case eTrToActStart:
                if( str->status == 0 )
                {
                    //Резервный
                    if( str->inputData.str.relayRPVcontactsState == 0 )
                    {
                        //РПВ не "на мне"                    
                        return eRunPassiveState; //Не удалось стать активным
                    }
                    //РПВ на мне
                    if( RelayCtrl_getRpvState() != eNotTurnOn )   ////!!!ddd 05.08.19 Kruglikov
                    {
                        str->outputData.str.activityState = 1; //Теперь активный
                    #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                        str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                    #endif    
                        return eRunActiveState; //Активный
                    }
                }
                //Основной
                if( RelayCtrl_getRpvState() != eNotTurnOn )   ////!!!ddd 05.08.19 Kruglikov
                {
                    str->outputData.str.relayCtrlCommand = 1; //Включить РПВ
                    str->transitionTimeCnt = T_O_RELAY_OPERATE; //Запуск таймаута включения реле
                    str->trToActCnt = eTrToActRelaySwitchDelay;
                }
                break;

            case eTrToActRelaySwitchDelay:
                //Основной
                //Ожидание переключения реле
                if( str->transitionTimeCnt == 0 )
                {
                    //Таймаут истек
                    str->trToActCnt = eTrToActStart;
                    str->outputData.str.relayCtrlCommand = 0; //Выключение РПВ
                    str->transitionTimeCnt = T_O_RELAY_RELEASE; //Задержка на отключение РПВ
                    return eRunPassiveState; //Не удалось стать активным
                }
                //Таймаут не истек
                if( str->inputData.str.relayRPVcontactsState == 0 )
                {
                    //РПВ не "на мне"
                    break;
                }
                //РПВ "на мне"
                str->trToActCnt = eTrToActStart;
                str->outputData.str.activityState = 1; //Теперь активный
            #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
            #endif    
                return eRunActiveState; //Активный
        }
        return eRunTransitionToActive;
    }
}

//*****************************************************************************
// Обработка состояния "Активный"
ActivityManager_States activeProcessing( ActivityManager_DeviceStr *str )
{
    //приказ по смене активности от УС выполняется активным прибор только по истечении таймаута 
    //Это 5 с после подачи питания на прибор, так как именно в течение этого времени пассивного прибор не может 
    //пытаться стать активным
    if( timeoutChAct)  
        timeoutChAct--;
    
    if( str->transitionTimeCnt )
    {
        //Проверка таймаута после перехода прибора из активного в пассивное состояние
        return eRunActiveState;
    }
    //Обработка смены состояний РС, БС закончена
    if( str->inputData.str.ownState == 0 )
    {
        //Прибор в безопасном состоянии
        if( str->rs422.str.ownLine1connect == 0 && str->rs422.str.ownLine2connect == 0 )
        {
            //Нет связи с соседом
            if( str->inputData.str.neighborConnection == 0 )
            {
                //Нет связи с соседом
                //Попытка стать пассивным 2
                if( str->inputData.str.relayRPVcontactsState == 0 ) //если РПВ не на мне
                    return eRunTransitionToPassive;
                else
                {
                    return eRunActiveState;
                }
            }
            //Есть связь с соседом
            if( str->inputData.str.neighborState == 0 )
            {
                //Сосед в безопасном состоянии
                if( str->rs422.str.neighborLine1connect == 0 && str->rs422.str.neighborLine2connect == 0 )
                {
                    //У соседа нет связи по RS-422
                    if( str->inputData.str.neighborActivity == 0 || str->status == 1 )
                    {
                        //Сосед пассивный или резервный
                    #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                        if( str->uniformLoadTimeCnt == 0 )
                        {
                            //Истек таймер равномерной нагрузки
                            //Попытка стать пассивным 2
                            if( timeoutProcessing( &str->timeout ) )
                            {
                                str->outputData.str.activityState = 0;
                                str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
                                str->uniformLoadTimeCnt = 0;
                                return eRunTransitionToPassive;
                            }
                        }
                    #endif    
                        //Таймер равномерной нагрузки не истек
                        return eRunActiveState;
                    }
                    //Сосед активный и основной
                    //Попытка стать пассивным 2
                    if( timeoutProcessing( &str->timeout ) )
                    {
                        //str->outputData.str.activityState = 0;  //06.08.19 Kruglikov
                        str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
                    #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                        str->uniformLoadTimeCnt = 0;
                    #endif    
                        //return eRunPassiveState; //изменено КВП 18.06
                        return eRunTransitionToPassive;
                    }
                }
                //У соседа есть связь по RS-422
                //Попытка стать пассивным 2
                if( timeoutProcessing( &str->timeout ) )
                {
                    //str->outputData.str.activityState = 0; //06.08.19 Kruglikov
                    str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
                #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                    str->uniformLoadTimeCnt = 0;
                #endif    
                    return eRunTransitionToPassive;
                }
            }
            //Сосед в рабочем состоянии
            //Попытка стать пассивным 2
            if( timeoutProcessing( &str->timeout ) )
            {
                //str->outputData.str.activityState = 0; //06.08.19 Kruglikov
                str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
            #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                str->uniformLoadTimeCnt = 0;
            #endif    
                return eRunTransitionToPassive;
            }
        }
        //Есть связь по RS-422
        if( str->inputData.str.neighborConnection == 0 )
        {
            if( str->inputData.str.relayRPVcontactsState == 0 ) //если РПВ не на мне
                 return eRunTransitionToPassive;
            else
            {
                //Нет пакетов от соседа
            #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                if( str->uniformLoadTimeCnt == 0 )
                {
                    //Таймер ТРН истек
                    str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                    return eRunActiveState;
                }
            #endif
                //Таймер ТРН не истек
                return eRunActiveState;
            }
        }
        //Есть пакеты от соседа
        if( str->inputData.str.neighborState == 0 )
        {
            //Сосед в БС
            if( str->rs422.str.neighborLine1connect == 0 && str->rs422.str.neighborLine2connect == 0 )
            {
                //У соседа нет связи по RS-422
            #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                if( str->uniformLoadTimeCnt == 0 )
                {
                    //Истек таймер ТРН
                    str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                    return eRunActiveState;
                }
            #endif
                //Таймер ТРН не истек
                return eRunActiveState;
            }
            //У соседа есть связь с УС
            if( str->inputData.str.neighborHearsMe == 0 )
            {
                //Сосед меня не слышит
                if( str->inputData.str.neighborActivity == 0 )
                {
                    //Сосед пассивный
                    return eRunActiveState;
                }
                //Сосед активный
                //Попытка стать пассивным 2
                if( timeoutProcessing( &str->timeout ) )
                {
                    //str->outputData.str.activityState = 0;  //06.08.19 Kruglikov
                    str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
                #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                    str->uniformLoadTimeCnt = 0;
                #endif    
                    return eRunTransitionToPassive;
                }
            }
            //Сосед меня слышит
            if( str->inputData.str.neighborActivity == 0 || str->status == 1 )
            {
                //Сосед пассивный или резервный
            #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                if( str->uniformLoadTimeCnt == 0 )
                {
                    //Истек таймер ТРН
                    //Попытка стать пассивным 2
                    if( timeoutProcessing( &str->timeout ) )
                    {
                        str->outputData.str.activityState = 0;
                        str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
                        str->uniformLoadTimeCnt = 0;
                        return eRunTransitionToPassive;
                    }
                }
            #endif    
                //Таймер ТРН не истек
                return eRunActiveState;
            }
            //Сосед основной и активный
            //Попытка стать пассивным 2
            if( timeoutProcessing( &str->timeout ) )
            {
                //str->outputData.str.activityState = 0;  //06.08.19 Kruglikov
                str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
            #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                str->uniformLoadTimeCnt = 0;
            #endif    
                return eRunTransitionToPassive; //вставка КВП
            }
        }
        //Сосед в РС
        //Попытка стать пассивным 2
        if( timeoutProcessing( &str->timeout ) )
        {
            //str->outputData.str.activityState = 0;   //06.08.19 Kruglikov
            str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
        #ifdef ENABLE_UNIFORM_LOAD_TIMER   
            str->uniformLoadTimeCnt = 0;
        #endif    
            return eRunTransitionToPassive; //вставка КВП
        }
        
    }
    //Прибор в рабочем состоянии
    if( str->inputData.str.relayRPVcontactsState == 0 )
    {
        //Реле не "на мне"
        return eRunTransitionToPassive;
    }
    //Реле РПВ "на мне"    
    if( str->inputData.str.neighborConnection == 0 )
    {
    //Нет связи с соседом
    #ifdef ENABLE_UNIFORM_LOAD_TIMER    
        if( str->uniformLoadTimeCnt == 0 )
        {
            //Истек таймер равномерной нагрузки
            str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
            return eRunActiveState;
        }
    #endif    
        //Не истек таймер равномерной нагрузки
        return eRunActiveState;
    }
    //Есть связь с соседом
    if( str->inputData.str.neighborHearsMe == 0 )
    {
        //Сосед меня не слышит
        if( str->inputData.str.neighborState == 0 )
        {
            //Сосед в безопасном состоянии
        #ifdef ENABLE_UNIFORM_LOAD_TIMER    
            if( str->uniformLoadTimeCnt == 0 )
            {
                //Истек таймер равномерной нагрузки
                str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                return eRunActiveState;
            }
        #endif    
            //Не истек таймер равномерной нагрузки
            return eRunActiveState;
        }
        //Сосед в рабочем состоянии
        return eRunTransitionToPassive;
    }
    //Сосед меня слышит
    if( str->inputData.str.ownActiveCommand == 0 )
    {
        //Нет активной команды
    #ifdef ENABLE_UNIFORM_LOAD_TIMER    
        if( str->uniformLoadTimeCnt == 0 )
        {
            //Истек таймер равномерной нагрузки
            if( str->inputData.str.neighborState == 0 )
            {
                //Сосед в безопасном состоянии
                str->uniformLoadTimeCnt = T_O_UNIFORM_LOAD;
                return eRunActiveState;
            }
            //Сосед в рабочем состоянии
            return eRunTransitionToPassive;
        }
    #endif    
        if( str->changeActivity && 
            str->inputData.str.neighborState && 
            str->inputData.str.neighborIsInWork ) 
        //пришла команда от управляющей системы по смене активности (я в РС, второй прибор тоже в РС)
            return eRunTransitionToPassive;
        
        return eRunActiveState;
    }
    //Есть активная команда
    return eRunActiveState;
}

//*****************************************************************************
// Обработка состояния "Попытка перехода в пассивное состояние"
ActivityManager_States transitionToPassive( ActivityManager_DeviceStr *str )
{
    switch( str->trToPassCnt )
    {
        case eTrToPassWaitDisableCtrl:
            if( str->inputData.str.ownActiveCommand == 0 )
            {
                //Отключены управляющие воздействия
                str->outputData.str.requestDisableCtrlActions = 0; //Снять запрос на выключение управляющих воздействий
                str->outputData.str.relayCtrlCommand = 0; //Выключить РПВ
                if( str->status == 0 )
                {
                    //Резервный
                    str->outputData.str.activityState = 0; //Пассивный
                    str->transitionTimeCnt = T_O_REMAIN_PASSIVE;
                #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                    str->uniformLoadTimeCnt = 0; //Сбросить таймер равномерной нагрузки
                #endif    
                    str->trToPassCnt = eTrToPassWaitDisableCtrl;
                    return eRunPassiveState; //Пассивный
                }
                //Основной
                str->transitionTimeCnt = T_O_RELAY_RELEASE; //Запуск таймаута отключения реле
                str->trToPassCnt = eTrToPassWaitRelaySwitch;
                break;
            }
            //Не отключены управляющие воздействия
            str->outputData.str.requestDisableCtrlActions = 1; //Установить запрос на выключение управляющих воздействий
            break;

        case eTrToPassWaitRelaySwitch:
            if( str->transitionTimeCnt == 0 )
            {
                //Таймаут ожидания переключения реле истек, не получилось стать пассивным
                str->outputData.str.relayCtrlCommand = 1; // Включить РПВ
                str->transitionTimeCnt = T_O_RELAY_OPERATE; //Запуск таймаута включение реле
                str->trToPassCnt = eTrToPassRemainInActive;
                break;
            }
            if( str->inputData.str.relayRPVcontactsState == 0 )
            {
                //Реле РПВ не "на мне"
                 str->outputData.str.activityState = 0;
                str->transitionTimeCnt = T_O_REMAIN_PASSIVE; //Запуск таймера пассивного состояния
            #ifdef ENABLE_UNIFORM_LOAD_TIMER    
                str->uniformLoadTimeCnt = 0; //Сбросить таймер равномерной нагрузки
            #endif    
                str->trToPassCnt = eTrToPassWaitDisableCtrl;
                return eRunPassiveState; //Переход в пассивное состояние
            }
            //Реле РПВ "на мне"
            break;

        case eTrToPassRemainInActive:
            //Ожидание таймаута переключения реле
            if( str->transitionTimeCnt ) 
                break;
            //Таймаут истек
            str->trToPassCnt = eTrToPassWaitDisableCtrl;
            return eRunActiveState; //Остается в активном состоянии
    }
    return eRunTransitionToPassive;
}

//*****************************************************************************
// Обработка таймаута смены активности в БС
bool timeoutProcessing( ActivityManager_TimeoutStr *str )
{
    //Установка флага запуска таймаута
    str->changeSafeActivityRun = 1;
    if( str->changeSafeActivityCnt == 0 )
    { //Таймаут не был запущен
        //Запуск таймаута
        str->changeSafeActivityCnt = T_O_CHANGE_ACTIVITY_IN_SAFE;
        return false;
    }
    //Декремент таймаута
    if( --str->changeSafeActivityCnt == 0 )
    { //Таймаут истек
        return true;
    }
    return false;
}

//*****************************************************************************
// Сбрасывание таймаута смены активности в БС
void timeoutReset( ActivityManager_TimeoutStr *str )
{
    if( str->changeSafeActivityRun == 0 ) str->changeSafeActivityCnt = 0;
    str->changeSafeActivityRun = 0;
}

//*****************************************************************************
/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   18-05-2017
* Автор  Кругликов В.П.
* Изменения:
*     Базовая версия.
*
* Версия 1.0.2  
* Дата 8-08-2019
* Автор  Кругликов В.П.
* Изменения:
* Изменён алгоритм обработки неисправности контрольной цепи реле РПВ
* 
* Версия 1.0.3  
* Дата 2-12-2019
* Автор  Кругликов В.П.
* Изменения:
* Добавлена обработка приказа от управляющей системы по смене активности
*
* Версия 1.0.4
* Дата 12-12-2019 
* Мзменена обработка пассивного состояния.
* Причина в следующей ощибке: 
* Основной - активный, рабочий. выключаем на нём 220В, он отдаёт активность рабочему резервному. 
* Выключаем на резервном 24В, 
* при этом активность не возвращается на основной прибор, а возвращается только если включить у него 220В 
*/
