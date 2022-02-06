/**
* \file    Rs422_lineExch.c
* \brief   \copybrief Rs422_lineExch.h
*
* \version 1.0.5
* \date    25-01-2021
* \author  Третьяков В.Ж.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <string.h>
#include "Rs422_lineExch.h"
#include "Rs422_transceiver.h"
#include "Rs422_dataTypes.h"
#include "ConfigMK.h"
#include "asserts_ex.h"
#include "InterChannel.h"
#include "Tracing.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************
#define RESET_SYNCHRO  0x8000        ///< Начальное значение переменной синхронизации.
#define FINISH_SYNCHRO 0x0800        ///< Значение переменной синхронизации окончания передачи.

//#define DEBUG_RS422

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация параметров линий связи RS-422
void Rs422_lineCtor( CommunicationRS422 *lineStr )
{
    inRS422_ctor( &lineStr->inRS422, lineStr->numLine );
    outRS422_ctor( &lineStr->outRS422, lineStr->numLine );
    lineStr -> timeOut = TIME_OUT_RESET_SYNC;
    lineStr -> exchangeEvent = false;
    lineStr -> state = eRs422exec_resetSynchro;
    //Переменная синхронизации
    InterChannel_setParamSettings( lineStr->eICId_Rs422Sync,
                                   eScriptChVal,
                                   eProcSyncEqual,
                                   eProcCheckEqual, 0, 0,
                                   0, 0, 0 ); 

    //Синхронизация
    if( !InterChannel_isHandling( lineStr -> eICId_Rs422Sync ) )
        InterChannel_synchronize( lineStr -> eICId_Rs422Sync, RESET_SYNCHRO );
    //Tracing_parameter( lineStr -> state, 0 );
    //Tracing_parameter( RESET_SYNCHRO, 1 );
}

//*****************************************************************************
// Управление приемом и передачей телеграмм по линии связи RS-422
void Rs422_lineRun( CommunicationRS422 *lineStr, StateRS422 *totalStr )
{
    uint8_t *aTele = 0;
    uint16_t sync, timeBegin = 0;
    uint8_t nCycle, typeStatus_A_B, temp;

    inRS422_run( &lineStr->inRS422, totalStr->address ); //tret 13.01.2020
    
    //Декремент тайм-аута отсутствия обмена
    if( lineStr -> noExchangeCnt )
        lineStr -> noExchangeCnt--;
    else
        lineStr -> newOrderEvent = false;
    //Декремент тайм-аута отсутствия обмена с УС
    if( lineStr -> noExchangeCtrlSystemCnt ) 
        lineStr -> noExchangeCtrlSystemCnt--;

    //Декремент переменной тайм-аута
    if( lineStr -> timeOut != 0 ) 
        lineStr -> timeOut--;

    switch( lineStr -> state )
    {
        case eRs422exec_resetSynchro: //0
            //Проверка синхронизации
            if( lineStr -> timeOut == 0 )
            {
                //ОШИБКА нет синхронизации
                ERROR_EX_ID( eGrPS_Rs422, ePS_Rs422TimeOutError, 1, eSynchroError_start, lineStr->numLine, 0 );
            }
            //Проверка синхронизируемой переменной
            if( InterChannel_getData( lineStr -> eICId_Rs422Sync ) != RESET_SYNCHRO )
            { //Синхронизируемый параметр не равен RESET_SYNCHRO 
                if( !InterChannel_isHandling( lineStr -> eICId_Rs422Sync ) )
                {
                    InterChannel_synchronize( lineStr -> eICId_Rs422Sync, RESET_SYNCHRO );
                }
                break;
            }

            //Синхронизируемая переменная соответствует
            //Переход к ожиданию 1-ой телеграммы
            lineStr -> state = eRs422exec_1stTeleReception;
            //Tracing_parameter( lineStr -> state, 0 ); //!!ddd
            //Tracing_parameter( RESET_SYNCHRO, 1 );    //!!ddd
            break;

        case eRs422exec_1stTeleReception: //1            // Ожидание приема 1-ой телеграммы
            // Запись длины телеграммы
            lineStr -> lengthInTele = InRS422_getTele( &lineStr -> inRS422, &aTele, &timeBegin );
// Третьяков В.
// Закомметирован код ниже. Его наличие приводило в пропуску телеграмм в следующем случае:
// По одному МК фиксировалось наличие телеграммы, запускалась синхронизация.
// По второму МК фиксировалось наличие телеграммы в следующем цикле. Но при этом выполнялось условие
// !InterChannel_isSynchronized(), и по break считанная телеграмма игнорировалась. 
//            if( !InterChannel_isSynchronized( lineStr -> eICId_Rs422Sync ) )
//            {
//                Rs422_lineCtor( lineStr );
//                break;
//            }
            if( lineStr -> lengthInTele == 0 ) break; // Телеграмма еще не принята 

            //Установка тайм-аута передачи ответной телеграммы
            if( timeBegin < TIME_OUT_1TELE )
            {
                timeBegin = ConfigMK_isMaster( ) ? TIME_OUT_1TELE - timeBegin : TIME_OUT_2TELE - timeBegin;
            }
            else
            {
                //Выполнить инициализацию
                Rs422_lineCtor( lineStr );
                break;
            }
            lineStr -> outRS422.timeOut = timeBegin;

            //Проверка типа телеграммы (длинная, короткая)
            if( lineStr -> lengthInTele == 1 )
            {
                //Короткая телеграмма запроса статуса
                if( !checkShortTelegramm( aTele[0] ) )
                { //Ошибка. Телеграмма не корректна
                    //Индикация ошибки  
                    lineStr -> exchangeError = errorRS422_shortTeleDoesNotCorrect;
                    //Выполнить инициализацию
                    Rs422_lineCtor( lineStr );
                    break;
                }
                lineStr -> aInTele[0][0] = *aTele;
                //Синхронизация
                if( !InterChannel_isHandling( lineStr -> eICId_Rs422Sync ) )
                    InterChannel_synchronize( lineStr -> eICId_Rs422Sync, lineStr -> aInTele[0][0] );
            }
            else
            { //Длинная телеграмма
                if( !checkTelegramm( aTele, lineStr -> lengthInTele ) || //Проверка на корректность телеграммы
                    lineStr -> lengthInTele - MIN_ORDER_TELE != totalStr -> orderLength )
                { //Проверка на соответствие длины блока данных
                    //Ошибка. Телеграмма не соответствует требованиям
                    lineStr -> exchangeError = errorRS422_tele1DoesNotCorrect;

                    //Выполнить инициализацию
                    Rs422_lineCtor( lineStr );
                    break;
                }
                memcpy( &lineStr -> aInTele[0][0], aTele, lineStr -> lengthInTele );

                //Синхронизация
                if( !InterChannel_isHandling( lineStr -> eICId_Rs422Sync ) )
                {
                    InterChannel_synchronize( lineStr -> eICId_Rs422Sync, lineStr -> aInTele[0][lineStr ->
                                              lengthInTele - 1] );
                }
            }
            //Установка тайм-аута контроля синхронизации
            lineStr -> timeOut = TIME_OUT_SYNC_TELE;
            //Переход к ожиданию синхронизации первой телеграммы
            lineStr -> state = eRs422exec_1stTeleSynchro;
            //Tracing_parameter( lineStr -> state, 0 );                                   //!!ddd
            //Tracing_parameter( lineStr -> aInTele[0][lineStr->lengthInTele - 1], 1 );   //!!ddd
            break;

        case eRs422exec_1stTeleSynchro: //2      //Проверка синхронизации 1-ой телеграммы
            if( lineStr -> timeOut == 0 )
            {
                //Выполнить инициализацию
                Rs422_lineCtor( lineStr );
                break;
            }

            sync = InterChannel_getData( lineStr -> eICId_Rs422Sync );
            //Проверка значения синхронизируемой переменной
            if( ( lineStr -> lengthInTele == 1 && sync != lineStr -> aInTele[0][0] ) ||
                ( lineStr -> lengthInTele > 1 && sync != lineStr -> aInTele[0][lineStr -> lengthInTele - 1] ) )
            {
                //Синхронизируемая переменная не соответствует
                break;
            }

            if( lineStr -> lengthInTele == 1 )
            { //Была принята короткая телеграмма
                //Подготовка данных для формирования ответной телеграммы
                typeStatus_A_B = ConfigMK_isMaster( ) ? CO_STATUS_A : CO_STATUS_B;
                //Установка тайм-аута контроля передачи       
                lineStr -> timeOut = ConfigMK_isMaster( ) ?
                TIME_OUT_TR_CONTR_SHORT_MASTER : TIME_OUT_TR_CONTR_SHORT_SLAVE;
                nCycle = totalStr -> numberCycleOK;
                //Переход к ожиданию окончания отправки телеграмм (2-ой телеграммы нет)
                lineStr -> state = eRs422exec_transmEnd;
                //Tracing_parameter( lineStr -> state, 0 );       //!!ddd
                //Tracing_parameter( sync, 1 );                   //!!ddd
            }
            else
            { //Была принята длинная телеграмма
                //Прием 2-ой телеграммы
                //Чтение 2-ой телеграммы
                lineStr -> lengthInTele = InRS422_getTele( &lineStr -> inRS422, &aTele, &timeBegin );
                if( !checkTelegramm( aTele, lineStr -> lengthInTele ) || //Телеграмма не корректна
                    lineStr -> lengthInTele - MIN_ORDER_TELE != totalStr -> orderLength )
                { //Длина телеграммы не соответствует длине блока данных
                    //Ошибка. 2-ая телеграмма не получена
                    lineStr -> exchangeError = errorRS422_tele2DoesNotCorrect;

                    //Выполнить инициализацию
                    Rs422_lineCtor( lineStr );
                    break;
                }
                memcpy( &lineStr -> aInTele[1][0], aTele, lineStr -> lengthInTele );

                //Проверка взаимной корректности телеграмм приказа А и В
                if( !checkTelegramAB( lineStr -> aInTele[0], lineStr -> aInTele[1] ) )
                {
                    //Ошибка. Телеграммы взаимно не корректны
                    lineStr -> exchangeError = errorRS422_teleMutuallyDoNotCorrect;

                    //Выполнить инициализацию
                    Rs422_lineCtor( lineStr );
                    break;
                }

                //Синхронизация 2-ой телеграммы
                if( !InterChannel_isHandling( lineStr -> eICId_Rs422Sync ) )
                    InterChannel_synchronize( lineStr -> eICId_Rs422Sync,
                    lineStr -> aInTele[1][lineStr -> lengthInTele - 1] );

                //Установка тайм-аута на прием синхронизированных данных
                lineStr -> timeOut = TIME_OUT_SYNC_TELE;

                //Подготовка данных для формирования ответной телеграммы
                nCycle = lineStr -> aInTele[0][INDEX_N_CICL]; //Количество циклов обмена, принятое значение
                if( ( lineStr -> aInTele[0][INDEX_ML_CO] & CO_MASK ) == CO_PRIKAZ_B )
                    nCycle = ~nCycle; //Количество циклов обмена преобразованное
                //Определение количества циклов для передачи
                if ( nCycle != (uint8_t)(totalStr->numberCycleOK + 1) && nCycle != (uint8_t)( totalStr->numberCycleOK + 2 ) )
                    nCycle = totalStr -> numberCycleOK; //устанавливается старое значение
                //Тип передаваемой телеграммы должен соответствовать типу принятой
                temp = ( lineStr -> aInTele[0][INDEX_ML_CO] & CO_MASK ) == CO_PRIKAZ_A;
                typeStatus_A_B = ( ConfigMK_isMaster( ) ^ temp ) ? CO_STATUS_B : CO_STATUS_A;

                //Переход к ожиданию синхронизации 2-ой телеграммы             
                lineStr -> state = eRs422exec_2stTeleSynchro;
                //Tracing_parameter( lineStr -> state, 0 );               //!!ddd
                //Tracing_parameter( lineStr -> aInTele[1][lineStr -> lengthInTele - 1], 1 );     //!!ddd
            }

            //Формирование ответной телеграммы 
            lineStr -> outRS422.length = formTeleStatus( lineStr -> outRS422.aOutRS422, typeStatus_A_B,
                                                         totalStr -> address, nCycle,
                                                         formAlarmRS422( nCycle, totalStr -> alarm ),
                                                         totalStr -> status, totalStr -> statusLength );
            break;

        case eRs422exec_2stTeleSynchro: //3     //Ожидание синхронизации 2-ой телеграммы
            if( lineStr -> timeOut == 0 )
            {
                //Выполнить инициализацию
                Rs422_lineCtor( lineStr );
                break;
            }

            //Проверка синхронизации
            if( InterChannel_getData( lineStr -> eICId_Rs422Sync ) != lineStr ->
                aInTele[1][lineStr -> lengthInTele - 1] )
            {
                //Синхронизируемая переменная не соответствует
                break;
            }

            //Запись приказа и установка количества циклов
            if( lineStr -> lengthInTele != 1 )
            {
                if( ( lineStr -> aInTele[0][INDEX_ML_CO] & CO_MASK ) == CO_PRIKAZ_A )
                    aTele = lineStr -> aInTele[0];
                else aTele = lineStr -> aInTele[1]; //телеграмма приказа А
                nCycle = aTele[INDEX_N_CICL]; //значение количества циклов в телеграмме
                lineStr -> newOrderEvent = true;
                if( nCycle == (uint8_t)(totalStr->numberCycleOK + 1) ||  nCycle == (uint8_t)( totalStr->numberCycleOK + 2 ) )
                {
                    totalStr -> isLineConnect = true;
                    //запись нового приказа
                    memcpy( totalStr -> order, &aTele[INDEX_IN_DATE_0], totalStr -> orderLength );
                }
                totalStr -> numberCycleOK = nCycle; //установка нового значения количества циклов                   
            }

            //Обновление времени отсутствия обмена c УС
            lineStr -> noExchangeCtrlSystemCnt = totalStr -> noExchangeTimeCtrlSystem;

            //Установка тайм-аута контроля передачи
            lineStr -> timeOut = ConfigMK_isMaster( ) ? TIME_OUT_TR_CONTR_MASTER : TIME_OUT_TR_CONTR_SLAVE;

            //Переход к ожиданию окончания передачи
            lineStr -> state = eRs422exec_transmEnd;
            //Tracing_parameter( lineStr -> state, 0 );           //!!ddd
            //Tracing_parameter( lineStr -> aInTele[1][lineStr -> lengthInTele - 1], 1 );     //!!ddd
        case eRs422exec_transmEnd: //4
            //Проверка окончания отправки
            if( !lineStr -> timeOut )
            {
                //Режим отладки DEBUG_RS422
#ifdef DEBUG_RS422   
                //ОШИБКА передачи
                ERROR_EX_ID( eGrPS_Rs422, ePS_Rs422TimeOutError, 4, eTimeOutError_transmit, lineStr->numLine, 0 );
#endif    //DEBUG_RS422
                //Выполнить инициализацию
                Rs422_lineCtor( lineStr );
            }

            //Проверка окончания отправки
            if( lineStr -> outRS422.length ) break;

            //Телеграмма отправлена
            //Синхронизация
            if( !InterChannel_isHandling( lineStr -> eICId_Rs422Sync ) )
                InterChannel_synchronize( lineStr -> eICId_Rs422Sync, FINISH_SYNCHRO );

            //Установка тайм-аута контроля синхронизации
            lineStr -> timeOut = TIME_OUT_SYNC_FINISH;

            //Переход к ожиданию синхронизации
            lineStr -> state = eRs422exec_synchroTransmEnd;
            //Tracing_parameter( lineStr -> state, 0 );           //!!ddd
            //Tracing_parameter( lineStr -> aInTele[1][lineStr -> lengthInTele - 1], 1 );     //!!ddd
            break;

        case eRs422exec_synchroTransmEnd: //5
            //Проверка синхронизации
            if( lineStr -> timeOut == 0 )
            {
                //Режим отладки DEBUG_RS422
#ifdef DEBUG_RS422
                //ОШИБКА нет синхронизации
                ERROR_EX_ID( eGrPS_Rs422, ePS_Rs422TimeOutError, 5, eSynchroError_finish, lineStr->numLine, 0 );
#endif    //DEBUG_RS422

                //Выполнить инициализацию
                Rs422_lineCtor( lineStr );
                break;
            }

            //Проверка синхронизации
            if( InterChannel_getData( lineStr -> eICId_Rs422Sync ) != FINISH_SYNCHRO )
                //Ошибка. Синхронизируемая переменная не соответствует
                break;

            //Передача завершена.
            //Индикация обмена
            //закомментировал КВП. Причина - после пропадания связи и её восстановления выполняется не текущий,
            // а последний на момент пропадания связи приказ  
            lineStr -> exchangeEvent = totalStr -> isLineConnect;
            //Обновление времени отсутствия обмена
            lineStr -> noExchangeCnt = totalStr -> noExchangeTime;

            //Переход к ожиданию 1-ой телеграммы
            //Переход к ожиданию первой телеграммы            
            //lineStr->state = eRs422exec_1stTeleReception;
            lineStr->state = eRs422exec_resetSynchro;
            lineStr->timeOut = TIME_OUT_RESET_SYNC;
            //Tracing_parameter( lineStr -> state, 0 );       //!!ddd
            //Tracing_parameter( lineStr -> aInTele[1][lineStr -> lengthInTele - 1], 1 );     //!!ddd
            //Начальная синхронизация
            if( !InterChannel_isHandling( lineStr -> eICId_Rs422Sync ) )
            {
                InterChannel_synchronize( lineStr -> eICId_Rs422Sync, RESET_SYNCHRO );
            }

            break;

        default:
            //Ошибка счетчика состояния
            ERROR_EX_ID( eGrPS_Rs422, ePS_Rs422StepCntError, 6, eStepCntError_rs422_run, lineStr->numLine, 0 );
    }
}

//*****************************************************************************
// Прием и передача данных по линии связи RS-422
void Rs422_lineInterrupt( CommunicationRS422 *lineStr )
{
    inRS422_readReg( &lineStr->inRS422, lineStr->numLine ); 
    outRS422_run( &lineStr->outRS422, lineStr->numLine ); 
}

//*****************************************************************************
/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   02-06-2017
* Автор  Кругликов В.П.
* 
* Изменения:
*    Базовая версия.
*
* Версия 1.0.2
* Дата   17-02-2020
* Автор  Кругликов В.П.
* 
* Изменения:
*    1) Счетчик номера цикла в приказе имеет разрядность 8 бит, поэтому после значения 0xFF следует
*    значение 0. При проверке согласованности внутреннего счётчика totalStr->numberCycleOK со счётчиком
*    в приказе результат вычислений имеет разрядность 16 бит и за значением 0xFF следует значение 0x0100,
*    в результате чего приказ игнорируется. Для устранения этой проблемы сделано приведение к типу uint8_t.
* 
* Версия 1.0.3
* Дата   13-01-2020
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    1. inRS422_run() перенесена из Rs422_lineInterrupt() в Rs422_lineRun()
*    2. В Rs422_lineInterrupt() добавлена inRS422_readReg().
* 
* Версия 1.0.4
* Дата   02-12-2020
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    1. Откорректирована функция Rs422_lineRun для устранения пропуска телеграмм.   
* 
* Версия 1.0.5
* Дата   25-01-2021
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    1. Изменен сценарий синхронизации: теперь по изменению значения. 
*/
