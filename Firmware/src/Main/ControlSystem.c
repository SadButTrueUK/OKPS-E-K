/**
* \file    ControlSystem.c
* \brief   \copybrief ControlSystem.h
*
* \version 1.0.3
* \date    14-01-2020
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <string.h>
#include <stdint.h>
#include "Main.h"
#include "ControlSystem.h"
#include "BinIn.h"
#include "deviceAddress.h"
#include "LedFailure.h"
#include "CheckSupply.h"
#include "relayCtrl.h"
#include "ShuntShiftMotor.h"
#include "shuntShift.h"
#include "shuntShiftGen.h"
#include "Interchannel.h"  
#include "OverloadDet.h"
#include "IntegrCtrl.h"
#include "defCompil.h"
#include "posDetGenerator.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
#define MAX_ORDER_SIZE          10                    ///< Максимальная длина приказа.
#define NO_ACTION               0x0                   ///< Приказ с идентификатором "Передача статуса".
#define MINUS_POS               0x1                   ///< Приказ с идентификатором "Перевод в положение -".
#define PLUS_POS                0x2                   ///< Приказ с идентификатором "Перевод в положение +".
#define STOP_P                  0x3                   ///< Приказ с идентификатором "Остановка двигателя"
#define UNDEFINED_ORDER         0x4                   ///< Неизвестный приказ (условное понятие, со старта программы)
#define IS_LOST_CONTR_WO_ORDER  1                     ///< Потеря контроля без команды на перевод
#define IS_OVL_CURR_MOTOR       2                     ///< Признак перегрузки по току   
#define IS_EMULATOR             3                     ///< Признак того, что прибор - макет стрелки.
#define NUM_MS_IN_SEC           1000                  ///< Количество миллисекунд в секунде.
#define TO_PLUS                 true                  ///< Вращение двигателя (при переводе стрелки в положение "Плюс")
#define TO_MINUS                false                 ///< Вращение двигателя (при переводе стрелки в положение "Минус")
#define CURRENT_RATIO           29U                   ///< Коэффициент пересчета дискрета АЦП в ток.
#define RATIO_TO_100MA          1000                  ///< Нормирующий коэффициент.

//*****************************************************************************
/// \brief Нормирующий коэффициент времени перевода (устанавливается на АРМе в 1/2 сек).
///
#define NUM_VALS_IN_SEC         2 

//*****************************************************************************
/// \brief Максимальное время перевода, передаваемое в приказе.
///
#define MAX_TIME_SHIFT          15 * NUM_VALS_IN_SEC  

//*****************************************************************************
/// \brief Время, когда индикатор "ОТКАЗ" включен при обнаружении КЗ в цепях двигателя.
///
#define TIME_LED_ON_SC_BLINK    300

//*****************************************************************************
/// \brief Время, когда индикатор "ОТКАЗ" выключен при обнаружении КЗ в цепях двигателя.
///
#define TIME_LED_OFF_SC_BLINK   1000

//*****************************************************************************
// Локальные типизированные константы
//***************************************************************************** 
static const uint8_t NUM_OF_PHASES = 3;    ///< Количество фаз сигнала.

//*****************************************************************************
// Определение типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Приказ.
///
typedef struct ControlSystem_Order_
{
    uint8_t t1    :6;        ///< Устанавливаемое максимальное время перевода
    uint8_t sw    :2;        ///< Устанавливаемое положение
    uint8_t act   :2;        ///< Переключение активности от УС
    uint8_t resB2 :1;        ///< Зарезервированное поле
    uint8_t cm    :1;        ///< Режим работы прибора
    uint8_t swm   :2;        ///< Действия с макетом
    uint8_t rs    :2;        ///< Включение / отключение режима повторного перевода, 1/2 с
    uint8_t resB3   :4;        ///< Зарезервированное поле
    uint8_t lev   :4;        ///< Уровень напряжения контрольных цепей 
} ControlSystem_Order;

//*****************************************************************************
/// \brief Статус.
///
typedef struct ControlSystem_Status_
{
    uint8_t t1        :6;                 ///< Установленное максимальное время перевода, 1/2 с.
    uint8_t st        :2;                 ///< Положение стрелочного перевода.
    uint8_t state2    :2;                 ///< Исправность резервного прибора.
    uint8_t state1    :2;                 ///< Исправность основного прибора.
    uint8_t err1      :2;                 ///< Событие / состояние.
    uint8_t dgn       :2;                 ///< Результаты последнего перевода.
    uint8_t t2        :6;                 ///< Фактическое время последнего перевода, 1/2 с.
    uint8_t err2      :2;                 ///< Событие / состояние.
    uint8_t currPhase [3];                ///< Массив значений последнего измерения тока перевода электропривода.
    uint8_t ph        :4;                 ///< Фаза перевода стрелки
    uint8_t lev       :4;                 ///< Уровень напряжения контрольных цепей
} ControlSystem_Status;

//*****************************************************************************
/// \brief Поле "Фаза перевода" телеграммы статуса.
///
typedef enum
{
    eShiftNotActive,
    ePhaseFirstShiftFromPlusToMinus,
    ePhaseSecondShiftFromMinusToPlus,
    ePhaseThirdShiftFromPlusToMinus,
    ePhaseFourthShiftFromMinusToPlus,
    ePhaseReservedFieldFirstBegin,
    ePhaseReservedFieldFirstEnd      = 0x08,
    ePhaseFirstShiftFromMinusToPlus,
    ePhaseSecondShiftFromPlusToMinus,
    ePhaseThirdShiftFromMinusToPlus,
    ePhaseFourthShiftFromPlusToMinus,        
    ePhaseReservedFieldSecondBegin,
    ePhaseReservedFieldSecondEnd     = 0x0F
} PhaseOfShiftField;
    
//*****************************************************************************
/// \brief Изменение состояния при переводе.
///
typedef enum
{
    eNotChanged,        ///< состояние не изменилось
    eChanged,           ///< состояние изменилось
    eInProcess          ///< в процессе контроля
} PosDetChangedInShift;

//*****************************************************************************
/// \brief Короткий статус.
///
typedef struct ControlSystem_ShortStatus_
{
    uint8_t reserved :4;        ///< Зарезервировано.
    uint8_t state2   :2;        ///< Исправность основного прибора.
    uint8_t state1   :2;        ///< Исправность резервного прибора.
} ControlSystem_ShortStatus;

//*****************************************************************************
/// \brief Требуемое положение стрелки приказом перевода.
///
typedef enum
{
    eShiftInPlus,         ///< требуемое "Положение плюс"
    eShiftInMinus,        ///< требуемое "Положение минус"
    eShiftUnknown         ///< требуемое положение неизвестно
} ReqPosition;

//*****************************************************************************
/// \brief Режимы индикации светодиодов "RS1" и "RS2".
///
typedef enum RsMode_
{
    RsMode_Active              = 1,                                       ///< прибор в активном состоянии
    RsMode_Connected           = 2,                                       ///< есть связь по RS-422
    RsMode_Undefined           = 4,                                       ///< не определено
    RsMode_ActiveConnected     = RsMode_Active | RsMode_Connected,        ///< активное состояние, есть связь
    RsMode_PassiveConnected    = RsMode_Connected,                        ///< пассивное состояние, есть связь
    RsMode_ActiveDisconnected  = RsMode_Active,                           ///< активное состояние, нет связи
    RsMode_PassiveDisconnected = 0                                        ///< пассивное состояние, нет связи
} RsMode;

//*****************************************************************************
// Определение локальных переменных
//*****************************************************************************

//*****************************************************************************
static PositionDet_State         oldPosDet = eDetUndefinedState;       ///< Предыдущее положение стрелки.
static PositionDet_State         newPosDet;                            ///< Новое положение стрелки.
static ControlSystem_Order       currentOrder;                         ///< Текущий приказ.
static ControlSystem_Status      currentStatus;                        ///< Текущий статус.
static ControlSystem_ShortStatus currentShortStatus;                   ///< Текущий короткий статус.
static bool                      rsEvent1 = false;                     ///< Признак наличия нового приказа на линии "RS1".
static bool                      rsEvent2 = false;                     ///< Признак наличия нового приказа на линии "RS2".
static ReqPosition               reqPos = eShiftUnknown;               ///< Требуемое положение стрелки.
static PhaseOfShiftField         phaseOfShift = eShiftNotActive;       ///< Фаза перевода стрелки.
static uint8_t                   previousFieldSW = UNDEFINED_ORDER;    ///< Предыдущее значяение поля ST приказа от УС

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Возвращает состояние фазы перевода стрелки стрелки при ее переводе.
/// \param cntPhaseShift - значение счётчика фазы перевода;
/// \return phaseOfShift - значение фазы перевода.
///
static PhaseOfShiftField formFieldPhaseShift( uint8_t cntPhaseShift );

//*****************************************************************************
/// \brief Определяет изменение положения стрелки при ее переводе.
///
static PosDetChangedInShift isPosDetchangedInShift( void );

//*****************************************************************************
/// \brief Определяет потерю контроля стрелки без команды на перевод стрелки.
///
static bool isPosDetChangedNoShift( PositionDet_State newPos );

//*****************************************************************************
/// \brief Проверка разрешения работы контроля целостности обмоток.
///
static bool ControlSystem_enIntegrCtrl( void );

//*****************************************************************************
/// \brief Формирование режима индикации светодиодов "RS1" и "RS2".
/// \param active - активность прибора;
/// \param connected - наличие связи с УС.
/// \return Режим индикации.
///
static uint16_t rsIndicationMode( bool active, bool connected );

//*****************************************************************************
/// \brief Управление индикаторами "RS1" и "RS2".
/// \param ind - указатель на структуру данных индикатора;
/// \param mode - указатель на текущий режим индикации;
/// \param modeNew - новый режим индикации;
/// \param event - наличие события обмена по соответствующей линии RS-422.
///
static void rsIndication( indOutput_type *ind, uint16_t *mode, uint16_t modeNew, bool event );

//*****************************************************************************
/// \brief Управление индикацией.
///
static void ControlSystem_indicationRun( void );

//*****************************************************************************
/// \brief Обмен с УС.
///
static void ControlSystem_exchangeRun( void );

//*****************************************************************************
/// \brief Управление активностью.
///
static void ControlSystem_activityManagerRun( void );

//*****************************************************************************
/// \brief Определение положения.
///
static void ControlSystem_shuntShiftPosDetRun( void );

//*****************************************************************************
/// \brief Обработка поступающих от УС приказов.
///
static void ControlSystem_processingOrders( void );

//*****************************************************************************
///\brief Функция, в которой производится проверка разрешения выполнения приказа
///в случае, если текущий приказ не равен предыдущему
static bool checkEnableProcessingOrder( void );

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация модуля общего управления
void ControlSystem_ctor( void )
{
    currentStatus.state1 = 2; //исправен основной прибор
    currentStatus.state2 = 2; //исправен резервный прибор
}

//*****************************************************************************
// Работа модуля общего управления
void ControlSystem_run( void )
{
    ControlSystem_exchangeRun( );           // Обмен с управляющей системой (формирование статусов, алармов)
    ControlSystem_activityManagerRun( );    // Управление активностью прибора   
    ControlSystem_indicationRun( );         // Индикация светодиодов RS1, RS2
    ControlSystem_shuntShiftPosDetRun( );   // Установка реле модуля "АУКС-5-ЭЦ" в зависимости от положения стрелки 
    ControlSystem_processingOrders( );      // Обработка поступающих от УС приказов
}

//*****************************************************************************
// Проверка условий рабочего режима
bool ControlSystem_workCondition( void )
{
    return CheckSupply_is220vOn( ) &&
            CheckSupply_is24vOn( ) &&
            !Rs422_getNoExchangeCtrlSystem( rs422 ) &&
            RelayCtrl_getRpvState( ) == eNormal;
}

//*****************************************************************************
// Реализация локальных функций
//*****************************************************************************

//*****************************************************************************
//Возвращает состояние фазы перевода стрелки при ее переводе.
static PhaseOfShiftField formFieldPhaseShift( uint8_t cntPhaseShift )
{
    static const uint8_t INIT_VAL_CNT_PHASE_SHIFT = 0;
    
    if( ShuntShift_isEnable( ) )
    {
        if( currentOrder.sw == MINUS_POS || currentOrder.sw == NO_ACTION )
            phaseOfShift = ePhaseFirstShiftFromPlusToMinus + cntPhaseShift; 
        else if( currentOrder.sw == PLUS_POS || currentOrder.sw == NO_ACTION )
            phaseOfShift = ePhaseFirstShiftFromMinusToPlus + cntPhaseShift;
        else {}
    }
    else
    {
        phaseOfShift = eShiftNotActive;
        ShuntShift_setCntPhaseShift( INIT_VAL_CNT_PHASE_SHIFT );
    }
    return phaseOfShift;
}

//*****************************************************************************
//Функция, в которой производится проверка разрешения выполнения приказа
//в случае, если текущий приказ не равен предыдущему
static bool checkEnableProcessingOrder( void )
{
    if( ActivityManager_isActive( activityManager ) && ShuntShift_isWorkMode( ) )
    {
        if( currentOrder.sw == previousFieldSW )
            return false;
        previousFieldSW = currentOrder.sw; 
        return true;
    }
    else
    {    
        previousFieldSW = UNDEFINED_ORDER;
        return false;
    }
}

//*****************************************************************************
 //Обработка поступающих от УС приказов
static void ControlSystem_processingOrders( void )
{
    // Управление мотором и алгоритмом контроля целостности обмоток
    if( CheckSupply_is24vOn( )
        && CheckSupply_is220vOn( )
        && RelayCtrl_getRpvState( ) == eNormal
        && ( InterChannel_getData( eICId_IntegrCtrlScAndBreak ) == eNorm || 
            InterChannel_getData( eICId_IntegrCtrlScAndBreak ) == eOverloadCircuit  ) )
    {
        if( currentOrder.sw == MINUS_POS  && 
          ( PosDetGenerator_isReadyForUse( ) || OverloadDet_isScContrGen( ) ) )
        { // Приказ на перемещение в "Минус"
            if( PositionDet_getPosition( ) == eDetMinus  || ( previousFieldSW == currentOrder.sw && !ShuntShift_isEnable( ) ) )
            {
                previousFieldSW = currentOrder.sw;      
                IntegrCtrl_turnOn( currentOrder.cm, PositionDet_getPosition( ) );
            }
            else
            {    
                IntegrCtrl_turnOff( ); //отключить контроль целостности обмоток
            }
            if( !IntegrCtrl_isOn( ) )
            {
                //при текущем переводе в плюс получена команда на перевод в минус (перевод "на лету")
                if( reqPos == eShiftInPlus )
                {
                    ShuntShift_turnOff( ); //отключить перевод стрелки
                    if( !ShuntShift_isEnable( ) ) 
                    {
                        reqPos = eShiftInMinus;
                    }
                }
                else
                {
                    if( checkEnableProcessingOrder( ) )
                    {
                        ShuntShift_turnOn( TO_MINUS, currentOrder.t1 * NUM_MS_IN_SEC / NUM_VALS_IN_SEC, 
                                       currentOrder.cm, currentOrder.rs );
                    }
                    reqPos = eShiftInMinus;
                }
            }
        }
        else if( currentOrder.sw == PLUS_POS && 
                ( PosDetGenerator_isReadyForUse( ) || OverloadDet_isScContrGen( ) ) )
        { // Приказ на перемещение в "Плюс"
            if( PositionDet_getPosition( ) == eDetPlus || ( previousFieldSW == currentOrder.sw && !ShuntShift_isEnable( ) ) )
            {
                previousFieldSW = currentOrder.sw;         
                IntegrCtrl_turnOn( currentOrder.cm, PositionDet_getPosition( ) );
            }
            else
            {
                IntegrCtrl_turnOff( );
            }
            if( !IntegrCtrl_isOn( ) )
            {
                if( reqPos == eShiftInMinus )
                {
                    ShuntShift_turnOff( );
                    if( !ShuntShift_isEnable( ) ) 
                    {
                        reqPos = eShiftInPlus;
                    }
                }
                else
                {
                    if( checkEnableProcessingOrder( ) )
                    {
                        ShuntShift_turnOn( TO_PLUS, currentOrder.t1 * NUM_MS_IN_SEC / NUM_VALS_IN_SEC, 
                                       currentOrder.cm, currentOrder.rs );
                    }
                    reqPos = eShiftInPlus;
                }
            }
        }
        else if( currentOrder.sw == STOP_P && ActivityManager_isActive( activityManager )  /*&& checkEnableProcessingOrder( )*/ )
        { // Приказ "Останов двигателя"
            if( ShuntShift_isEnable( ) )
            {
                ShuntShift_turnOff( );
            }
            IntegrCtrl_turnOn( currentOrder.cm, PositionDet_getPosition( ) );
            previousFieldSW = currentOrder.sw;
            reqPos = eShiftUnknown;
        }
        else if( currentOrder.sw == NO_ACTION )
        { // Пустой приказ (т.е передача статуса)
            if( !ShuntShift_isEnable( ) ) 
            {
               IntegrCtrl_turnOn( currentOrder.cm, PositionDet_getPosition( ) );
            }
            previousFieldSW = currentOrder.sw; 
        }
    }
    else
    {
        //previousFieldSW = currentOrder.sw; //!!!
        if( ShuntShift_isEnable( ) ) 
        {
            if ( InterChannel_getData( eICId_OverloadDetScThreePhaseGen ) ) // если есть КЗ в цепях двигателя
            {
                ShuntShiftGen_fastTurnOff( );
            }
            ShuntShift_turnOff( ); 
        }
        else if( ControlSystem_enIntegrCtrl( ) )
        {
            IntegrCtrl_turnOn( currentOrder.cm, PositionDet_getPosition( ) );
        }
        else
        {    
            IntegrCtrl_turnOff( );
        }
        reqPos = eShiftUnknown;
    }
}

//*****************************************************************************
// Проверка разрешения работы контроля целостности обмоток
static bool ControlSystem_enIntegrCtrl( void )
{
    return CheckSupply_is220vOn( ) &&
           CheckSupply_is24vOn( ) &&
           RelayCtrl_getRpvState( ) == eNormal ;
}

//*****************************************************************************
// Формирование режима индикации светодиодов "RS1" и "RS2"
static uint16_t rsIndicationMode( bool active, bool connected )
{
    uint16_t r = 0;
    if( active )
    {
        r |= RsMode_Active;
    }
    if( connected )
    {
        r |= RsMode_Connected;
    }
    return r;
}

//*****************************************************************************
// Управление индикаторами "RS1" и "RS2"
static void rsIndication( indOutput_type *ind, uint16_t *mode, uint16_t modeNew, bool event )
{
    if( *mode != modeNew )
    {
        switch( modeNew )
        {
            case RsMode_ActiveConnected:
                Indication_off( ind );
                break;
            case RsMode_ActiveDisconnected:
                Indication_blink2( ind, 300, 300, 300, 1100, eIndPhNormal );
                break;
            case RsMode_PassiveConnected:
                Indication_blink( ind, 300, 300, eIndPhNormal );
                break;
            case RsMode_PassiveDisconnected:
                Indication_blink( ind, 300, 1700, eIndPhNormal );
                break;
        }
        *mode = modeNew;
    }
    if( modeNew == RsMode_ActiveConnected && event )
    {
        Indication_pulseOn( ind, 50 );
    }
}

//*****************************************************************************
//Определяет изменение положения стрелки при её переводе
static PosDetChangedInShift isPosDetchangedInShift( void )
{
    if( ShuntShift_getDgn( ) == eShiftActive )
    {
        if( oldPosDet != newPosDet )
        {
            oldPosDet = newPosDet;
            return eChanged;
        }
        else 
            return eInProcess;
    }
    else
    {
        oldPosDet = eDetUndefinedState;
        return eNotChanged;
    }
}

//*****************************************************************************
// Определяет потерю контроля стрелки без команды на перевод стрелки
static bool isPosDetChangedNoShift( PositionDet_State newPos )
{
    static const uint16_t TIME_STABLE_STATE = 50;
    static PositionDet_State oldPos = eDetUndefinedState;
    static uint16_t cnt;
    
    // Если перевод стрелки отключен или включен, но нет команды на перевод в какое-либо положение
    if( !ShuntShift_isEnable( ) && 
        IntegrCtrl_isOn( ) &&  
        PosDetGenerator_isEnable() &&
        !OverloadDet_getScContrGenTrig() && 
        PosDetGenerator_isReadyForUse() )
    {
        if( newPos == eDetMinus || newPos == eDetPlus ) //считываем текущее положение стрелки
        {
            oldPos = newPos;  //и перезаписываем предыдущее положение
            cnt = 0;
            return false;
        }
        if( ( oldPos == eDetMinus || oldPos == eDetPlus ) && newPos == eDetNon )
        {    
            if ( cnt < TIME_STABLE_STATE )
            {
                cnt++;
                return false; 
            }
            return true;        // произошла потеря контроля без команды перевода стрелки
        }
        oldPos = eDetUndefinedState;
        return false;
    }
    else 
    {
        cnt = 0;
        oldPos = eDetUndefinedState;
        return false;
    }
}

//*****************************************************************************
// Управление индикацией
static void ControlSystem_indicationRun( void )
{
#ifdef IND_SHORT_CIRCUIT
    if( OverloadDet_isScThreePhaseGen( )
        || OverloadDet_isScContrGen( )
        || InterChannel_getData( eICId_IntegrCtrlScAndBreak ) == eShortCircuit )    
    //при наличии КЗ светодиод "Отказ" мигает в разных режимах
    {        
        OverloadDet_isScContrGen( ) ? LedFailure_blink( TIME_LED_ON_SC_BLINK, TIME_LED_OFF_SC_BLINK ) 
                                    : LedFailure_blink( TIME_LED_ON_SC_BLINK, TIME_LED_ON_SC_BLINK );
    }
    else
    {
        if( CheckSupply_is220vOn( ) && CheckSupply_is24vOn( ) ) 
            LedFailure_set( false ); //светодиод "Отказ" выключен
        else 
            LedFailure_set( true ); //светодиод "Отказ" включен
    }
#else
    if( CheckSupply_is220vOn( ) && CheckSupply_is24vOn( ) ) LedFailure_set( false ); //светодиод "Отказ" выключен
    else LedFailure_set( true ); //светодиод "Отказ" включен
#endif
    // Rs1, Rs2
    bool active = ActivityManager_isActive( activityManager );
    static uint16_t
    rsMode1 = RsMode_Undefined,
            rsMode2 = RsMode_Undefined;
    uint16_t
    rsNewMode1 = rsIndicationMode( active, !Rs422_getNoLineExchange( rs422, eRs422_line_1 ) ),
            rsNewMode2 = rsIndicationMode( active, !Rs422_getNoLineExchange( rs422, eRs422_line_2 ) );
    if( !( rsNewMode1 & RsMode_Connected ) && ( rsNewMode1 == rsNewMode2 ) && ( rsNewMode1 != rsMode1 || rsNewMode2 != 
        rsMode2 ) )
    {
        rsMode1 = RsMode_Undefined;
        rsMode2 = RsMode_Undefined;
    }
    rsIndication( &ledRs1, &rsMode1, rsNewMode1, rsEvent1 | Rs422_getNewOrderEventLine( rs422, eRs422_line_1 ) );
    rsIndication( &ledRs2, &rsMode2, rsNewMode2, rsEvent2 | Rs422_getNewOrderEventLine( rs422, eRs422_line_2 ) );
    if( active ) //индикация светодиодов положения стрелки только на активном приборе
    {
        newPosDet = PositionDet_getPosition( );
            switch( newPosDet )
            {
                case eDetNon: //"ПОТЕРЯ КОНТРОЛЯ" или "НАЛИЧИЕ + и -"
                case eDetUndefinedState:    
                    isPosDetchangedInShift( ) == eChanged ? Indication_blink( &ledLossCtrl, 100, 100, eIndPhNormal ) :
                            isPosDetchangedInShift( ) == eInProcess ? : Indication_on( &ledLossCtrl );
                    Indication_off( &ledPlusPos );
                    Indication_off( &ledMinusPos );
                    break;

                case eDetPlus: //"Положение плюс"        
                    isPosDetchangedInShift( ) == eChanged ? Indication_blink( &ledPlusPos, 100, 100, eIndPhNormal ) :
                            isPosDetchangedInShift( ) == eInProcess ? : Indication_on( &ledPlusPos );
                    Indication_off( &ledLossCtrl );
                    break;

                case eDetMinus: //"Положение минус"
                    isPosDetchangedInShift( ) == eChanged ? Indication_blink( &ledMinusPos, 100, 100, eIndPhNormal ) :
                            isPosDetchangedInShift( ) == eInProcess ? : Indication_on( &ledMinusPos );
                    Indication_off( &ledPlusPos );
                    Indication_off( &ledLossCtrl );
                    break;

                default:
                    Indication_off( &ledLossCtrl );
                    Indication_off( &ledPlusPos );
                    Indication_off( &ledMinusPos );
                    oldPosDet = eDetUndefinedState;
            }
    }
    else
    {
        Indication_off( &ledLossCtrl );
        Indication_off( &ledPlusPos );
        Indication_off( &ledMinusPos );
    }
}

//*****************************************************************************
// Обмен с УС
static void ControlSystem_exchangeRun( void )
{

    rsEvent1 = Rs422_getLineEvent( rs422, eRs422_line_1 );
    rsEvent2 = Rs422_getLineEvent( rs422, eRs422_line_2 );
    if( Rs422_getNoExchangeCtrlSystem( rs422 ) )
    {
        memset( &currentOrder, 0, sizeof( currentOrder ) );
    }
    else if( rsEvent1 || rsEvent2 )
    {
        uint8_t rawOrder[MAX_ORDER_SIZE];
        uint8_t orderSize = Rs422_getOrder( rs422, rawOrder );
        if( orderSize == sizeof( currentOrder ) )
        {
            memcpy( &currentOrder, rawOrder, sizeof( currentOrder ) );
        }
    }
    if( DeviceAddress_isPrimary( ) )
    {
        currentStatus.state1 = currentShortStatus.state1 = 2;
        currentStatus.state2 = currentShortStatus.state2 = BlockExch_getConnectionState( blockExchange ) ? 2 : 1;
    }
    else
    {
        currentStatus.state1 = currentShortStatus.state1 = BlockExch_getConnectionState( blockExchange ) ? 2 : 1;
        currentStatus.state2 = currentShortStatus.state2 = 2;
    }
    //Ограничение значения времени перевода
    if( currentOrder.t1 == 0 || currentOrder.t1 > MAX_TIME_SHIFT ) currentOrder.t1 = MAX_TIME_SHIFT;
    //Заполнение структуры статусов
    if( ActivityManager_isActive( activityManager ) )
    {
        currentStatus.t1 = currentOrder.t1;
        currentStatus.st = PositionDet_getPosition( );

        if( OverloadDet_getScThreePhaseGenTrig( ) )
            currentStatus.err1 = eShortCircuit;
        else
            currentStatus.err1 = InterChannel_getData( eICId_IntegrCtrlScAndBreak );
        currentStatus.dgn = ShuntShift_getDgn( );
        //currentStatus.err2 = currentOrder.swm ? IS_EMULATOR : ( InterChannel_getData( eICId_ShuntShiftMotor_failure ) == eOverloadCircuit ? 
        //                     IS_OVL_CURR_MOTOR : isPosDetChangedNoShift( PositionDet_getPosition( ) ) );
        if( currentOrder.swm )
        {
            currentStatus.err2 = IS_EMULATOR;
        }
        else 
        {
            if( ShuntShiftMotor_getFailure() == eOverloadCircuit )
                currentStatus.err2 = IS_OVL_CURR_MOTOR;
            if( isPosDetChangedNoShift( currentStatus.st ) )
            {
                ShuntShiftMotor_setFailure( eNorm );
                currentStatus.err2 = IS_LOST_CONTR_WO_ORDER;
            }
            else if ( currentStatus.st != eDetNon || ShuntShift_isEnable( ) )
                currentStatus.err2 = ShuntShiftMotor_getFailure();
            else {}
        }
        {
            uint16_t tempTime = ShuntShift_getTime( );
            static const uint16_t RATIO = 100 / NUM_VALS_IN_SEC;
            currentStatus.t2 = __builtin_divud( tempTime, RATIO * 10 );
            if( __builtin_divud( tempTime, RATIO ) % 10 > 5 )
            currentStatus.t2 += 1; //округление для повышения точности
        }
        const uint8_t IDcurrPhases[] = { eICId_IV_rms, eICId_IU_rms, eICId_IW_rms };
        for( uint8_t cnt = 0; cnt < NUM_OF_PHASES; cnt++ )
        {
            uint16_t temp = InterChannel_getData( IDcurrPhases[cnt] );
            //перевод из дискрет АЦП в ток с разрешением 0.1А
            currentStatus.currPhase[cnt] = __builtin_divud( __builtin_muluu( temp, CURRENT_RATIO ), RATIO_TO_100MA );
        }
        currentStatus.lev = PosDetGenerator_getLevel( );
        currentStatus.ph = formFieldPhaseShift( ShuntShift_getCntPhaseShift());
        Rs422_setStatus( rs422, ( uint8_t * ) &currentStatus, sizeof currentStatus );
    }
    else if( DeviceAddress_getSysType( ) == eEbilock )
    {
        Rs422_setStatus( rs422, 0, 0 );
    }
    else
    {
        Rs422_setStatus( rs422, ( uint8_t * ) &currentShortStatus, sizeof currentShortStatus );
    }
    // Формирование алармов.
    // Дополнительные коды.
    // Перегрузка в цепях 3-фазного генератора прибора (0x60).
    if( OverloadDet_getScThreePhaseGenTrig( ) || IntegrCtrl_getFailure( ) == eShortCircuit )
        Rs422_addAlarm( rs422, eAlarmShortCircuitThreePhaseGen );
    else
        Rs422_removeAlarm( rs422, eAlarmShortCircuitThreePhaseGen );
    // Перегрузка в цепях контрольного генератора прибора (0x6E).
    if( OverloadDet_getScContrGenTrig( ) )
        Rs422_addAlarm( rs422, eAlarmShortCircuitContrGen );
    else
        Rs422_removeAlarm( rs422, eAlarmShortCircuitContrGen );
    // Напряжение источника питания «220 В» не в норме (0x65).
    if( CheckSupply_is220vOn( ) )
        Rs422_removeAlarm( rs422, eAlarmNoSupplyVoltage220V );
    else
        Rs422_addAlarm( rs422, eAlarmNoSupplyVoltage220V );
    // Стандартные коды.
    // Напряжение источника питания «24 В» не в норме (0xF0).
    if( CheckSupply_is24vOn( ) )
        Rs422_removeAlarm( rs422, eAlarmNoSupplyVoltage24V );
    else
        Rs422_addAlarm( rs422, eAlarmNoSupplyVoltage24V );
    // Отсутствие корректного приказа по линии связи 1 дольше 1,5 с (0xF1).
    if( Rs422_getNoLineExchangeCtrlSystem( rs422, eRs422_line_1 ) )
        Rs422_addAlarm( rs422, eAlarmNoFirstLineExchCtrlSys );
    else
        Rs422_removeAlarm( rs422, eAlarmNoFirstLineExchCtrlSys );
    // Отсутствие корректного приказа по линии связи 2 дольше 1,5 с (0xF2).
    if( Rs422_getNoLineExchangeCtrlSystem( rs422, eRs422_line_2 ) )
        Rs422_addAlarm( rs422, eAlarmNoSecondLineExchCtrlSys );
    else
        Rs422_removeAlarm( rs422, eAlarmNoSecondLineExchCtrlSys );
    // Отсутствие корректного приказа по любой из линий связи смежного прибора дольше 1,5 с (0xF3).
    if( !( ActivityManager_getRS422neighborCtrlSysConnect( activityManager, eRs422_line_1 )
           && ActivityManager_getRS422neighborCtrlSysConnect( activityManager, eRs422_line_2 ) )
        && ActivityManager_getNeighborConnection( activityManager ) )
        Rs422_addAlarm( rs422, eAlarmNoValidOrderToNeighbor );
    else
        Rs422_removeAlarm( rs422, eAlarmNoValidOrderToNeighbor );
    // Смежный прибор находится в безопасном состоянии (0xF4).
    if( !ActivityManager_isNeighborWorking( activityManager )
        && ActivityManager_getNeighborConnection( activityManager ) )
        Rs422_addAlarm( rs422, eAlarmNeighborInSafeMode );
    else
        Rs422_removeAlarm( rs422, eAlarmNeighborInSafeMode );
    //Не включается реле РПВ
    if( RelayCtrl_getRpvState( ) == eNotTurnOn )
        Rs422_addAlarm( rs422, eAlarmRPVnotTurnOn );       
    else
        Rs422_removeAlarm( rs422, eAlarmRPVnotTurnOn );
    //Не выключается реле РПВ
    if( RelayCtrl_getRpvState( ) == eNotTurnOff )
        Rs422_addAlarm( rs422, eAlarmRPVnotTurnOff );       
    else
        Rs422_removeAlarm( rs422, eAlarmRPVnotTurnOff );
    //Нет связи с соседним прибором (или неисправен кнала связи)
    if ( BlockExch_getConnectionState( blockExchange ) )
        Rs422_removeAlarm( rs422, eAlarmNoConnectWithNeighbor );
    else
        Rs422_addAlarm( rs422, eAlarmNoConnectWithNeighbor );
    //Неисправен генератор сигнала контрольных цепей
    if( PosDetGenerator_isGeneratorFail( ) )
        Rs422_addAlarm( rs422, eAlarmControlGeneratorIsFail );
    else
        Rs422_removeAlarm( rs422, eAlarmControlGeneratorIsFail );
}   

//*****************************************************************************
// Управление активностью
static void ControlSystem_activityManagerRun( void )
{
    static const uint8_t TIMEOUT_CHANGE_ACT = 25;   //таймаут на переключение активности, за который гарантированно уйдёт статус
    static uint8_t timeOutChAct;
    
    ActivityManager_setRS422connect( activityManager, eRs422_line_1,
                                     !Rs422_getNoLineExchange( rs422, eRs422_line_1 ) );
    ActivityManager_setRS422connect( activityManager, eRs422_line_2,
                                     !Rs422_getNoLineExchange( rs422, eRs422_line_2 ) );
    ActivityManager_setRS422ctrlSysConnect( activityManager, eRs422_line_1,
                                            !Rs422_getNoLineExchangeCtrlSystem( rs422, eRs422_line_1 ) );
    ActivityManager_setRS422ctrlSysConnect( activityManager, eRs422_line_2,
                                            !Rs422_getNoLineExchangeCtrlSystem( rs422, eRs422_line_2 ) );
    if( ShuntShift_isEnable( ) && ControlSystem_workCondition( ) )
    {    
        ActivityManager_setActiveCommand( activityManager, true );
        timeOutChAct = 0;
    }
    else if( currentOrder.sw == NO_ACTION || !ControlSystem_workCondition( ) )
    {
        if ( ++timeOutChAct >= TIMEOUT_CHANGE_ACT )
        {
            timeOutChAct = 0;
            ActivityManager_setActiveCommand( activityManager, false );
            ActivityManager_changeActivity( activityManager, currentOrder.act );
        }
    }
    else 
        timeOutChAct = 0;
    ActivityManager_setRPVoutState( activityManager, BinIn_isRpvOnMe( ) );
    RelayCtrl_switchRpv( ActivityManager_isRelayCtrlOn( activityManager ) );
}

//*****************************************************************************
// Определение положения
static void ControlSystem_shuntShiftPosDetRun( void )
{
    if( ShuntShift_isEnable( ) )
    {
        // Перевод стрелки
        ShiftDirection reqDir = ShuntShift_getReqShiftDir( );
        switch( reqDir )
        {
            case eShiftPlus:
            case eShiftMinus:
                RelayCtrl_setPosDet( eDetNon );
                RelayCtrl_setShiftDir( reqDir );
                break;
            default: 
                switch( ShuntShift_getReqPosDet( ) )
                {
                    case eDetPlus:
                        RelayCtrl_setPosDet( eDetPlus );
                        RelayCtrl_setShiftDir( eShiftMinus );
                        break;
                    case eDetMinus:
                        RelayCtrl_setPosDet( eDetMinus );
                        RelayCtrl_setShiftDir( eShiftPlus );
                        break;
                    default:
                        RelayCtrl_setPosDet( eDetNon );
                        RelayCtrl_setShiftDir( eShiftStop );
                }
        }
        ShuntShift_setDetPos( RelayCtrl_getPosDet( ), PositionDet_getPosition( ) );
        ShuntShift_setShiftPos( RelayCtrl_getShiftDir( ) );
        PositionDet_setPosDet( RelayCtrl_getPosDet( ) );
    }
    else
    {
        // Нет перевода стрелки  
        switch( PositionDet_getRequestPositionDet( ) )
        {
            case eDetPlus:
                RelayCtrl_setPosDet( eDetPlus );
                RelayCtrl_setShiftDir( eShiftMinus );
                break;

            case eDetMinus:
                RelayCtrl_setPosDet( eDetMinus );
                RelayCtrl_setShiftDir( eShiftPlus );
                break;

            default:
                RelayCtrl_setPosDet( eDetNon );
                RelayCtrl_setShiftDir( eShiftStop );
        }
    }
    PosDetGenerator_setLevel ( currentOrder.lev ); //установить уровень напряжения контрольного генератора
}

//*****************************************************************************
/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   18-01-2018
* Автор  Кругликов В.П.
* Изменения:
*     Базовая версия.
*
* Версия 1.0.2 
* Дата   23-10-2019
* Автор  Кругликов В.П.
  Изменения:
*     Изменен код функции isPosDetchangedInShift
*     Привязка к текущему положению стрелки при переводе берётся из поля
*     dgn телеграммы статуса 
*  
* Версия 1.0.3
* Дата   14-01-2020
* Автор  Кругликов В.П. 
* 
* Изменения:
*      Добавлена Функция, в которой производится проверка разрешения выполнения приказа
*      в случае, если текущий приказ не равен предыдущему checkEnableProcessingOrder
*      в обработку приказов. Также добавлена в обработку приказов проверка состояния контрольного генератора
*      (обработка выполняется только тогда, когда данные контрольного генератора актуальны) 
*/
