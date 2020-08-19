/**
* \file    shuntShiftNineWire.c
* \brief   \copybrief shuntShiftNineWire.h
*
* \version 1.0.1
* \date    02-02-2018
* \author  Кругликов В.П.
*/

//****************************************************************************
// Подключаемые файлы
//****************************************************************************
#include "shuntShiftNineWire.h"
#include "shuntShift.h"
#include "shuntShiftMotor.h"
#include "ProtectionState_codes.h"
#include "asserts.h"
#include "InterChannel.h"
#include "IntegrCtrl.h"
#include "IntegrCtrlGen.h"
#include "CheckSupply.h"
#include "Main.h"


//****************************************************************************
// Локальные константы, определенные через макросы
//****************************************************************************
#define INIT_TIMEOUT          500        ///< Начальный таймаут для корректного определения положения стрелки.
#define RS_ON_2               3          ///< Повторный перевод включен (0b11).
#define RS_ON_1               2          ///< Повторный перевод включен (0b10).
#define RS_OFF_2              1          ///< Повторный перевод выключен (0b01).
#define RS_OFF_1              0          ///< Повторный перевод выключен (0b00).

//****************************************************************************
// Определение типов данных
//****************************************************************************

//****************************************************************************
/// \brief Идентификаторы состояния функции #NineWireShuntShift_run.
///
typedef enum 
{
    eInit        = 0,        ///< начальная инициализация
    eStart,                  ///< начало движения
    eStop,                   ///< состояние "Стоп"
    eInitPosDet,             ///< состояние ожидания определения положения
    eRunning,                ///< движение
    eShutdown                ///< отключение
} ShiftNineWireStates;

//****************************************************************************
/// \brief Структура состояния. 
///
typedef struct 
{
    uint8_t ctrl   :1;        ///< Включение модуля.
    uint8_t st     :1;        ///< Работа на стенде.
    uint8_t dir    :1;        ///< Устанавливаемое положение.
    uint8_t rs     :2;        ///< Повторный перевод.
    uint8_t unused :3;        ///< Неиспользуемые биты.
} ShuntShiftNine_flags;

//****************************************************************************
/// \brief Объединение флагов состояния с \a uint8_t.
///
typedef union 
{
    ShuntShiftNine_flags str;         ///< Флаги состояния.
} uShuntShiftNine_flags;

//****************************************************************************
// Определение глобальных типизированных констант
//****************************************************************************

//****************************************************************************
/// \brief Переменная структуры интерфейса.
///
const ArrayShuntShift nineWireShuntShift = 
{
    NineWireShuntShift_ctor,
    NineWireShuntShift_run,
    NineWireShuntShift_turnOn,
    NineWireShuntShift_turnOff,
    NineWireShuntShift_setDetPos,
    NineWireShuntShift_setShiftPos,
    NineWireShuntShift_isEnable,
    NineWireShuntShift_getReqPosDet,
    NineWireShuntShift_getReqShiftDir,
    NineWireShuntShift_getTime,
    NineWireShuntShift_getDgn,
    NineWireEcShuntShift_setCntPhaseShift,
    NineWireEcShuntShift_getCntPhaseShift,
    NineWireEcShuntShift_isWorkMode
};

//****************************************************************************
// Определение глобальных переменных
//****************************************************************************
static uShuntShiftNine_flags flags;          ///< Флаги состояния.
static ShiftNineWireStates   stateCnt;       ///< Счетчик состояния.
static PositionDet_State     pos;            ///< Фактическое положение.
static uint16_t              setTime;        ///< Устанавливаемое время.
static uint16_t              timeoutCnt;     ///< Счетчик таймаутов.
static ShiftDgn              dgn;            ///< Состояние последнего перевода.
static uint16_t              shiftTime;      ///< Время последнего перевода.
static uint8_t               cntTriesRS;     ///< Cчётчик фаз алгоритма работы двукратного перевода

//****************************************************************************
// Реализация интерфейсных функций
//****************************************************************************

//****************************************************************************
// Инициализация
void NineWireShuntShift_ctor( void )
{
    flags.str.ctrl = 0;  //Выключен 
    pos = eDetNon;    
    dgn = eShiftNormal;   
    shiftTime = 0;
    stateCnt = eInit;
    timeoutCnt = INIT_TIMEOUT;
    cntTriesRS = 0;
}

//****************************************************************************
// Управление работой
void NineWireShuntShift_run( void )
{
    static bool temp;
    static bool dirInternal;        //для изменения направления вращения при двукратном переводе
    
    if ( ActivityManager_isActive( activityManager ) == false )
    {
        dgn = eShiftNormal;
        return;
    }    
    
    if( timeoutCnt ) 
        timeoutCnt--;    // Декремент счетчика таймаутов
    switch( stateCnt )
    {
        case eInit:     //инициализация по включению питания
            if( timeoutCnt == 0 )
                stateCnt = eStart;
            break;
        
        case eStart:
            if( flags.str.ctrl )
            {
                dirInternal = flags.str.dir; //направление перевода берём из приказа
                stateCnt = eInitPosDet;
            } 
            break;
  
        case eInitPosDet: // Ожидание определения положения
            // Проверка положения
            temp = ( ( dirInternal == 1 ) && ( pos == eDetPlus ) )   
               ||( ( dirInternal == 0 ) && ( pos == eDetMinus ) );
            if( temp ) // Фактическое положение соответствует 
            {   // устанавливаемому положению
                //Перевод не начинать
                flags.str.ctrl = 0; // Выключение
                stateCnt = eStart;      //eStop;
            }
            else
            {   // Включить перевод
                ShuntShiftMotor_turnOn( flags.str.st, dirInternal );
                dgn = eShiftActive;    //Активное состояние
                timeoutCnt = setTime;
                stateCnt = eRunning;
            }
            break;      

        case eRunning: // Перевод, ожидание отключения
            // Положение соответствует переводимому
            temp = ( ( dirInternal == 1 ) && ( pos == eDetPlus ) )   
                   ||( ( dirInternal == 0 ) && ( pos == eDetMinus ) );
            if( timeoutCnt == 0 ||                           // Время перевода истекло 
                ShuntShiftMotor_getFailure( ) != eNorm ||    // Неисправность
                temp ||                                      // Текущее положение соответствует заданному в приказе                                    
                flags.str.ctrl == 0 )                       // Перевод остановлен досрочно внешней командой
            {   //Запрос на синхронизацию признака аварии
                InterChannel_synchronize( eICId_IntegrCtrlScAndBreak, ShuntShiftMotor_getFailure( ) );
                shiftTime = ShuntShiftMotor_getWorkingTime( );    // Считать время 
                ShuntShiftMotor_turnOff( );                       // Выключить двигатель 
                stateCnt = eShutdown;
            }
            break;
            
        case eShutdown:    // Ожидание отключения двигателя
            if( !ShuntShiftMotor_isOn( ) )
            {   // Двигатель отключен
                if( temp )  //устанавливаем поле dgn сатуса
                {                          // Фактическое положение соответствует устанавливаемому положению
                    if( flags.str.rs == RS_OFF_2 || flags.str.rs == RS_OFF_1 ) 
                        dgn = eShiftNormal;    
                    else 
                    {
                        if( cntTriesRS == 2 ) //Двукратный перевод осуществлён со второй попытки
                            dgn = eShiftSecondTry; 
                        else if( cntTriesRS == 0 ) //Двукратный перевод осуществлён с первой попытки
                            dgn = eShiftNormal;
                        else if( cntTriesRS == 3 ) //Двукратный перевод завершился неудачей
                            dgn = eShiftFail;
                    }
                }
                else 
                {                        // Фактическое положение не соответствует устанавливаемому
                    if( flags.str.ctrl == 0 &&  ShuntShiftMotor_getFailure( ) == eNorm )
                    {
                        if( !ActivityManager_isNeighborWorking( activityManager ) || 
                             !ActivityManager_getNeighborConnection( activityManager ) )
                        {
                            if ( CheckSupply_is220vOn( ) && CheckSupply_is24vOn( ) )
                            {
                                dgn = eShiftNormal;
                            }     
                            else 
                                dgn = eShiftFail;
                        }
                        else if ( CheckSupply_is220vOn( ) && CheckSupply_is24vOn( ) ) 
                            dgn = eShiftNormal;
                    }
                    else if( ( flags.str.rs == RS_OFF_2 || flags.str.rs == RS_OFF_1 ) || 
                            ( (flags.str.rs == RS_ON_2 || flags.str.rs == RS_ON_1 ) && 
                            ( cntTriesRS == 3 || ShuntShiftMotor_getFailure( ) != eNorm ) ) )
                    {    
                        dgn = eShiftFail;    // Перевод закончился неудачей 
                    }
                }
                if( flags.str.rs == RS_OFF_2 || flags.str.rs == RS_OFF_1 )  // !!!пока так, дальше условие усложнится
                {
                    flags.str.ctrl = 0;    // Выключение
                    stateCnt = eStart;
                }
                else if( flags.str.rs == RS_ON_2 || flags.str.rs == RS_ON_1 )
                {
                    cntTriesRS++;
                    if ( ( cntTriesRS == 4 ) ||
                        ( temp && (cntTriesRS != 2 ) ) ||
                        ( flags.str.ctrl == 0 ) || ShuntShiftMotor_getFailure( ) != eNorm )
                    {
                       cntTriesRS = 0; 
                       flags.str.ctrl = 0;
                       stateCnt = eStart;
                    }    
                    else
                    {
                        dirInternal = !dirInternal; //изменить направление вращения для двукратного перевода
                        stateCnt = eInitPosDet;
                    }
                }    
            }
            break;

        default:
            ERROR_ID( eGrPS_ShuntShift, ePS_ShuntShiftNineWireCntErr );
    } 
}

//****************************************************************************
// Включение
void NineWireShuntShift_turnOn( bool dir, uint16_t time, bool st, uint8_t rs )
{
    if( !NineWireShuntShift_isEnable( ) )
    {
        flags.str.dir  = dir;
        flags.str.ctrl = 1;
        flags.str.st   = st;
        setTime        = time;
        flags.str.rs   = rs;
    }
}

//****************************************************************************
// Выключение
void NineWireShuntShift_turnOff( void )
{
    flags.str.ctrl = 0;
}

//****************************************************************************
// Передача положения стрелки
void NineWireShuntShift_setDetPos( PositionDet_State detPos, PositionDet_State position )
{
    pos = position;
}

//****************************************************************************
// Передача установленной позиции перевода
void NineWireShuntShift_setShiftPos( ShiftDirection shiftPos )
{

}

//****************************************************************************
// Проверка состояния включения
bool NineWireShuntShift_isEnable( void )
{
    return stateCnt != eStart && stateCnt != eInit;
}

//****************************************************************************
// Запрос на установку позиции контроля
PositionDet_State NineWireShuntShift_getReqPosDet( void )
{
    return eDetNon;
}

//****************************************************************************
// Запрос на установку позиции направления перевода
ShiftDirection NineWireShuntShift_getReqShiftDir( void )
{
    return eShiftStop;
}

//****************************************************************************
// Получить время последнего перевода
uint16_t NineWireShuntShift_getTime( void )
{
    return shiftTime;
}

//****************************************************************************
// Получить состояние перевода
ShiftDgn NineWireShuntShift_getDgn( void )
{
    return dgn;
}

//*****************************************************************************
//Установить значение счётчика фазы стрелочного перевода.
void NineWireEcShuntShift_setCntPhaseShift( const uint8_t val )
{
    cntTriesRS = val;
}
//*****************************************************************************
//Получить значение счётчика фазы стрелочного перевода.

const uint8_t NineWireEcShuntShift_getCntPhaseShift( void )
{
    return cntTriesRS;
}

const bool NineWireEcShuntShift_isWorkMode( void )
{
    return  stateCnt != eInit;
}
//*****************************************************************************
/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   02-02-2018
* Автор  Кругликов В.П.
* Изменения:
*     Базовая версия.
*
*
* Версия 1.0.2
* Дата   23-10-2019
* Автор  Кругликов В.П.
* Изменения:
* 	Устранены ошибки в алгоритме двукратного перевода стрелки в функции 
* 	NineWireShuntShift_run: 
* 	- некорректная отработка приказа "Стоп"  пока перевод не завершён
* 	- некорректное формирование поля dgn в телеграмме статуса
* 
*
* Версия 1.0.3
* Дата 12-12-19 
* Автор  Кругликов В.П.
* Изменения:
* 	Удалён таймаут на определение положения в начале перевода стрелки
* 	в функции NineWireShuntShift_run (по причине изменения идеологии
* 	работы модуля генератора контрольного напряжения), за счёт чего
* 	поле dgn в телеграмме статуса формируется быстрее 
*
*
* Версия 1.0.4
* Дата 23-04-20 
* Автор  Кругликов В.П.
* Изменения:
* 	Добавлено дополнительное условие для формирования поля dgn в значение eShiftNormal -
* 	наличие питания 24 и 220В
*/
