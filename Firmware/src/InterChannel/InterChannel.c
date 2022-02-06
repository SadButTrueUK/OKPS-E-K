/**
* \file    InterChannel.c
* \brief   \copybrief InterChannel.h
*
* \version 1.0.15
* \date    04-02-2021
* \author  Третьяков В.Ж.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <string.h>
#include <stdlib.h>
#include "Interchannel.h"
#include "asserts_ex.h"
#include "set.h"
#include "MainRegisters.h"
#include "Main.h"
#include "CheckCallFunctions.h"
#include "Tracing.h"
#include "DefCompil.h"

//*****************************************************************************
// Макросы типа функция
//*****************************************************************************

//*****************************************************************************
/// \brief Распаковка байт в слова в формате, передаваемом по линии связи.  
///
#define GET_COMMUNICATION_DATA( hi,lo ) ( ( ( hi ) << 8 ) + ( ( lo ) & 0x00FF ) )

//*****************************************************************************
/// \brief Упаковка байт в слова в формате, передаваемом по линии связи.
///
#define SET_COMMUNICATION_DATA( d, hi,lo ) ( hi ) = ( ( d ) >> 8 ); ( lo ) = ( ( d ) & 0x00FF )       

//*****************************************************************************
/// \brief Минимум.
/// \note Макрос возвращает меньшее из двух значений, указанных в параметрах a и b.
///
#define MIN( a,b ) ( ( ( a )<( b ) )?( a ):( b ) )

//*****************************************************************************
/// \brief Максимум.
/// \note Макрос возвращает большее из двух значений, указанных в параметрах a и b.
///
#define MAX( a,b ) ( ( ( a )>( b ) )?( a ):( b ) )  

//*****************************************************************************
/// \brief Проверка корректного включения трассировки.  
///
#ifdef INTERCHANNEL_INTERCHANNEL_DEBUG_MODE
    #if ( INTERCHANNEL_INTERCHANNEL_DEBUG_MODE == INTERCHANNEL_INTERCHANNEL_DEBUG_MODE_ON  )  

        #ifndef INTERCHANNEL_INTERCHANNEL_DEBUG_TRACING_ARRAY
            #error "Не определен индекс массива трассировки!"
        #endif

        #if    ( INTERCHANNEL_INTERCHANNEL_DEBUG_TRACING_ARRAY < 0 )                \
            || ( INTERCHANNEL_INTERCHANNEL_DEBUG_TRACING_ARRAY >= TRACING_NUMBER )  
            #error "Индекс массива трассировки указан не верно!"
        #endif

    #endif
#endif

//*****************************************************************************
/// \brief Флаги состояния синхронизации.
/// \note Макрос формирует маску флага по его значению, описанному в 
/// причисляемом типе \a SignalFlags.
///
#define FLAG_MASK( flag ) (uint16_t)( 1 << (flag) )

//*****************************************************************************
/// \brief Перевод физической величины в проценты.
/// \param range_ – величина, соответствующая 100%;
/// \param val_   – переводимое значение.
///
#define valToPercent( range_, val_ ) ((100 * (val_)) / (range_))

//*****************************************************************************
/// \brief Перевод процентов в физическую величину.
/// \param range_ – величина, соответствующая 100%;
/// \param per_   – переводимые проценты.
///
#define percentToVal( range_, per_ ) ( ( ( range_ ) * ( per_ ) ) / 100 )

//*****************************************************************************
/// \brief Инициализация счетчика контрольного времени.
/// \param id_ – идентификатор параметра, для которого запускается
/// счетчик;
/// \param conTime_ – идентификатор запускаемого счетчика 
/// #InterChannelControlledTime;
/// \param time_ – значение контролируемого времени, мс. 
/// \note Макрос запускает счетчик времени, контроль которого необходимо выполнить.
/// \note Параметр \a time_ измеряется с разрешением в 1 мс при условии, что 
/// функция #InterChannel_run вызывается с таким же периодом. 
///
#define InterChannel_initTime( id_, conTime_, time_ ) do                         \
{                                                                                \
    interChannelData.timing[( conTime_ )].time[( id_ )] = ( time_ );             \
    Set64_insert( &interChannelData.timing[( conTime_ )].timeSet64, ( id_ ) );   \
} while( 0 )

//*****************************************************************************
/// \brief Запуск счетчика контрольного времени.
/// \param id_ – идентификатор параметра, для которого запускается
/// счетчик;
/// \param time_ – идентификатор счетчика, одно из значений 
/// #InterChannelControlledTime.
/// \note Макрос запускает счетчик времени, контроль которого необходимо выполнить.
///
#define InterChannel_startTime( id_, time_ ) do                      \
{                                                                    \
    if ( interChannelData.paramDump[( id_ )].m_settings              \
                                   .m_time[ ( time_ ) ] > 0 ) {      \
        InterChannel_initTime(                                       \
                       ( id_ ),                                      \
                       ( time_ ),                                    \
                       interChannelData.paramDump[( id_ )]           \
                                     .m_settings.m_time[ ( time_ ) ] \
                              );                                     \
    }                                                                \
} while( 0 )

//*****************************************************************************
/// \brief Запуск счетчика контрольного времени Т1.
/// \param id_ – идентификатор параметра, для которого запускается
/// счетчик.
///
#define InterChannel_startTime1( id_ )                               \
           InterChannel_startTime( id_, eInterChannelConTime1 ) 

//*****************************************************************************
/// \brief Запуск счетчика контрольного времени Т2.
/// \param id_ – идентификатор параметра, для которого запускается
/// счетчик.
/// \note Макрос запускает счетчик времени, контроль которого необходимо выполнить.
///
#define InterChannel_startTime2( id_ )                                          \
           InterChannel_startTime( id_, eInterChannelConTime2 )  

//*****************************************************************************
/// \brief Запуск счетчика контрольного времени Т3.
/// \param id_ – идентификатор параметра, для которого запускается
/// счетчик.
/// \note Макрос запускает счетчик времени, контроль которого необходимо выполнить.
///
#define InterChannel_startTime3( id_ )                                        \
           InterChannel_startTime( id_, eInterChannelConTime3 ) 

//*****************************************************************************
/// \brief Запуск счетчиков контрольного времени Т1 и Т3.
/// \param id_ – идентификатор параметра, для которого запускается
/// счетчик.
/// \note Счетчики Т1 и Т3 запускаются одновременно при получении запроса 
/// на синхронизацию. 
/// \note Макрос запускает счетчик времени, контроль которого необходимо выполнить.
///
#define InterChannel_startTime13( id_ ) do                                    \
{                                                                             \
    if ( interChannelData.paramDump[( id_ )].m_settings.m_script              \
                                                    == eScriptSync ) {        \
        InterChannel_startTime1( id_ );                                       \
    }                                                                         \
                                                                              \
    InterChannel_startTime3( id_ );                                           \
                                                                              \
} while( 0 )

//*****************************************************************************
/// \brief Остановка счетчика контрольного времени Т1.
/// \param id_ – идентификатор параметра, для которого запускается
/// счетчик.
///
#define InterChannel_stopTime1( id_ )                                        \
    Set64_remove( &interChannelData.timing[eInterChannelConTime1].timeSet64, \
                  ( id_ ) )

//*****************************************************************************
/// \brief Остановка счетчика контрольного времени Т2.
/// \param id_ – идентификатор параметра, для которого запускается
/// счетчик.
///
#define InterChannel_stopTime2( id_ )                                        \
    Set64_remove( &interChannelData.timing[eInterChannelConTime2].timeSet64, \
                  ( id_ ) )

//*****************************************************************************
/// \brief Остановка счетчика контрольного времени Т3.
/// \param id_ – идентификатор параметра, для которого запускается
/// счетчик.
///
#define InterChannel_stopTime3( id_ )                                        \
    Set64_remove( &interChannelData.timing[eInterChannelConTime3].timeSet64, \
                  ( id_ ) )

//*****************************************************************************
/// \brief Проверка того, что счетчик контрольного времени Т1 запущен.
/// \param id_ – идентификатор параметра, для которого проверяется
/// счетчик.
///
#define InterChannel_time1IsRunning( id_ )                                   \
    !Set64_hasElement( &interChannelData.timing[eInterChannelConTime1]       \
                       .timeSet64, ( id_ ) )

//*****************************************************************************
/// \brief Проверка того, что счетчик контрольного времени Т2 запущен.
/// \param id_ – идентификатор параметра, для которого проверяется
/// счетчик.
///
#define InterChannel_time2IsRunning( id_ )                                   \
    !Set64_hasElement( &interChannelData.timing[eInterChannelConTime2]       \
                       .timeSet64, ( id_ ) )

//*****************************************************************************
/// \brief Проверка того, что счетчик контрольного времени Т3 запущен.
/// \param id_ – идентификатор параметра, для которого проверяется
/// счетчик.
///
#define InterChannel_time3IsRunning( id_ )                                   \
    !Set64_hasElement( &interChannelData.timing[eInterChannelConTime3]       \
                       .timeSet64, ( id_ ) )

//*****************************************************************************
/// \brief Остановка подсчета одного из времен Т1,Т2 или Т3
/// для всех параметров синхронизации.
/// \param conTime_ – идентификатор времени #InterChannelControlledTime,
/// подсчет которого будет остановлен для всех параметров синхронизации.
///
#define InterChannel_stopTimeForAllId( conTime_ )                            \
    Set64_clear(&interChannelData.timing[ ( conTime_ ) ].timeSet64)

//*****************************************************************************
/// \brief Выполнить выражение в критической секции, защищающей блок от 
/// прерываний таймера для работы с CAN.
///
#define ATOMIC_SYS( x ) do     \
{                              \
    sysLock();                 \
    x;                         \
    sysUnLock();               \
} while( 0 )

//*****************************************************************************
/// \brief Установка события синхронизации.
/// \details Макрофункция сбрасывает флаги: #eLocal, #eLocalAck, #eRemote и 
/// устанавливает флаги: #eSynchronized, #eSyncEvent, #eSyncEventOverflow.
/// \details Флаг #eSyncEventOverflow устанавливается только если до наступления 
/// события синхронизации флаг #eSyncEvent не был сброшен.
/// \param id_ – идентификатор параметра, для которого произошло событие
/// синхронизации.
///
#define InterChannel_setSyncEvent( id_ ) do                                   \
{                                                                             \
    if (    (   interChannelData.paramDump[( id_ )].m_state.m_flags           \
              & FLAG_MASK( eSyncEvent ) )                                     \
         == FLAG_MASK( eSyncEvent ) )                                         \
    {                                                                         \
        interChannelData.paramDump[( id_ )].m_state.m_flags |=                \
                                            FLAG_MASK( eSyncEventOverflow );  \
    }                                                                         \
    interChannelData.paramDump[( id_ )].m_state.m_flags &=                    \
                                                 ~(   FLAG_MASK( eLocal )     \
                                                    | FLAG_MASK( eLocalAck )  \
                                                    | FLAG_MASK( eRemote ) ); \
    interChannelData.paramDump[( id_ )].m_state.m_flags |=                    \
            ( FLAG_MASK( eSynchronized ) | FLAG_MASK( eSyncEvent ) );         \
} while( 0 )
    
//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
/// \brief Количество байт блока данных, принимаемых от соседнего канала по 
/// линии связи за одну передачу.
///
#define REMODE_DATA_BLOCK_SIZE    6

//*****************************************************************************
/// \brief Количество блоков данных, которые могут быть помещены в хранилище 
/// и хранится в нем до их обработки в #InterChannel_run.
///
#define REMODE_DATA_STORAGE_SIZE    10

//*****************************************************************************
// Объявление типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Флаги процесса выполнения.
///
typedef enum
{
    eLocal,                   ///< получен запрос на выполнение сценария
    eLocalAck,                ///< получено подтверждение приема параметра
    eRemote,                  ///< принят параметр от соседнего канала
    eSynchronized,            ///< выполнена синхронизация
    eSyncEvent,               ///< произошло событие синхронизации            
    eSyncEventOverflow,       ///< произошло переполнение событий синхронизации          
} InterChannelParamFlags;

//*****************************************************************************
/// \brief Параметры синхронизации, необходимые для работы системы
/// межканальной синхронизации по синхронизируемому параметру. 
///
typedef enum
{
    eInterChannelConTime1     = 0,                   ///< время получения параметра от обоих каналов (Т1)
    eInterChannelConTime2,                           ///< время формирования синхронизированного параметра (Т2)
    eInterChannelConTime3,                           ///< период поступления запросов на синхронизацию (Т3)
    eInterChannelConTimeCount                        ///< количество контролируемых времен
} InterChannelControlledTime;

//*****************************************************************************
/// \brief Контейнер для данных, передаваемых по линии связи.
///
typedef struct
{
    uint16_t m_id;                      ///< Идентификатор параметра.
    param_t  m_value;                   ///< Значение параметра.
} InterChannelTransmitData;

//*****************************************************************************
/// \brief Настройки параметра синхронизации.
///
typedef struct
{
    InterChannelScript    m_script;                     ///< Сценарий, выполняемый МКО.
    InterChannelProcSync  m_procSync;                   ///< Процедура синхронизации данных.
    InterChannelProcCheck m_procCheck;                  ///< Процедура контроля данных.
    param_t               m_paramCheck;                 ///< Параметр контроля.
    param_t               m_middleRangeCheck;           ///< Значение половины измерительного диапазона контролируемого параметра.

    
    // eInterChannelConTime1
    // eInterChannelConTime2
    // eInterChannelConTime3
    uint16_t m_time[eInterChannelConTimeCount];         ///< Контролируемые временные характеристики.

} InterChannelParamSettings;

//*****************************************************************************
/// \brief Состояние синхронизируемого параметра.
///
typedef struct
{
    uint16_t m_flags;                             ///< Флаги процесса выполнения.
    param_t  m_localValue;                        ///< Значение параметра своего канала.
    param_t  m_remoteValue;                       ///< Значение параметра соседнего канала.
    param_t  m_operatingValue;                    ///< Значение параметра после сравнения.
    param_t  m_lastTranslateValue; // tret 04.02.2021 ///< Последнее переданное значение параметра.
                                 
} InterChannelParamState;

//*****************************************************************************
/// \brief Параметр синхронизации.
/// \details Структура описывает все данные, необходимые для работы системы 
/// межканальной синхронизации по синхронизируемому параметру.
/// Описание контролируемых временных параметров смотри в описании 
/// типа #InterChannelControlledTime.
///
typedef struct
{
    InterChannelParamSettings m_settings;                          ///< Настройка сценариев синхронизации.
    InterChannelParamState    m_state;                             ///< Состояние параметра в процессе синхронизации.
} InterChannelParamDump;

//*****************************************************************************
/// \brief Контроль времени.
/// \details Структура описывает все данные, необходимые для выполнения 
/// контроля любо из контролируемых временных параметров при синхронизации данных.
///
typedef struct
{
    //**************************************************************************
    /// \brief Список параметров, для которых контролируется время.
    Set64_t  timeSet64;

    //**************************************************************************
    /// \brief Счетчики контролируемого времени для каждого синхронизируемого 
    /// параметра в системе.
    uint16_t time[eInterChannelIdCount];   

} InterChannelTiming;

//************************************************************************
/// \brief Вспомогательные данные для работы #InterChannel_runCommunication.
///
typedef struct
{
    bool     isTrasmitting;               ///< Признак того, что по линии связи передаются параметры в соседний канал.
    uint16_t trId1;                       ///< Идентификатор первого параметра, передаваемого по линии связи.
    uint16_t trId2;                       ///< Идентификатор второго параметра, передаваемого по линии связи.
} InterChannelTmp;

//************************************************************************
/// \brief Данные объекта межканального обмена.
///
typedef struct
{
    /// \brief Массив состояний каждого синхронизируемого параметра.
    ///
    InterChannelParamDump paramDump[eInterChannelIdCount];

    /// \brief Структура управляющая счетчиками контроля временных характеристик.
    /// Описание контролируемых временных параметров смотри в описании 
    /// типа #InterChannelControlledTime.
    ///
    InterChannelTiming    timing[eInterChannelConTimeCount]; 

    Set64_t               set64;               ///< Приоритетная очередь запросов на синхронизацию.

    Set64_t               updateSet64;         ///< Список данных, нуждающихся в обновлении.

    const ArrayIoDriver *drvPtr;               ///< Указатель на данные драйвера канала связи.

    InterChannelTmp       tmp;                 ///< Вспомогательные данные для работы #InterChannel_runCommunication.

    /// \brief Поле для хранения текущего состояния блокировки работы потока межканального обмена.
    ///
    bool                  stateLock;

} InterChannelData;

//************************************************************************
/// \brief Тип данных, описывающий хранилище блоков данных, принимаемых 
/// от соседнего канала по линии связи.
///
typedef struct
{
    uint8_t items [REMODE_DATA_STORAGE_SIZE][REMODE_DATA_BLOCK_SIZE];    ///< Буфер для хранения блоков данных
    uint16_t count;                                                      ///< Счетчик блоков, помещенных в хранилище
} InterChannelStorageRemoteData;

//************************************************************************
/// \brief Тип данных, описывающий хранилище идентификаторов сообщений, 
/// о приеме которых было получено подтверждение по линии связи от соседнего 
/// канала.
///
typedef struct
{
    id_t items [REMODE_DATA_STORAGE_SIZE * 2];   ///< Буфер для хранения идентификаторов
    //// При одной передаче данных может передаваться 
    //// два идентификатора, следовательно, буфер идентификаторов 
    //// должен быть в два раза больше, чем буфер данных принятых 
    //// от соседнего канала.
    uint16_t count;                              ///< Счетчик идентификаторов, помещенных в хранилище
} InterChannelStorageAck;

//*****************************************************************************
// Объявление локальных типизированных констант
//*****************************************************************************

const int interChannelDataCheckErrorCount = 3;    ///< Количество допустимых ошибок между каналами.

//*****************************************************************************
// Объявление локальных переменных
//*****************************************************************************

//*****************************************************************************
static InterChannelData interChannelData;         ///< Данные объекта межканального обмена.

//*****************************************************************************
/// \brief Счетчик системного времени.
/// \details Разрешение контролируемого времени равно периоду вызова
/// #InterChannel_run.
///
static uint32_t mainTime;

//************************************************************************
/// \brief Хранилище блоков данных, принимаемых от соседнего канала по 
/// линии связи.
///
static InterChannelStorageRemoteData interChannelStorageRemoteData;

//************************************************************************
/// \brief Хранилище идентификаторов сообщений, о приеме которых было получено
/// подтверждение по линии связи от соседнего канала.
///
static InterChannelStorageAck interChannelStorageAck;

//*****************************************************************************
// Реализация локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Критическая секция, блокирующая работу прерываний.
///
inline static void sysLock( void )
{
    interChannelData.stateLock = !CAN_TIMER_IS_ENABLE_INTERRUPT;
    CAN_TIMER_DISABLE_INTERRUPT;
}

//*****************************************************************************
/// \brief Критическая секция, разрешающая работу прерываний.
///
inline static void sysUnLock( void )
{
    CAN_TIMER_SET_ENABLE_INTERRUPT( !interChannelData.stateLock );
}

//*****************************************************************************
/// \brief Контроль временных параметров.
/// \details При выполнении сценариев межканальный обмен контролирует время выполнения
/// отдельных функций. Выделяются следующие контролируемые времена:
/// \details – время получения параметра от обоих каналов (Т1);
/// \details – время формирования синхронизированного параметра (Т2);
/// \details – период поступления запросов в МКО (Т3).
/// \note Вызывается из функции #InterChannel_run, разрешение 
/// контролируемого времени равно периоду вызова этой функции.
///
void InterChannel_checkTime( InterChannelControlledTime conTime )
{
    id_t id;
    static Set64_t set64Tmp;
    bool result;

    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChConTimeIndexError,
                   ( conTime >= 0 ) && ( conTime < eInterChannelConTimeCount ),
                   conTime, eInterChannelConTimeCount, 0, 0 );

    result = Set64_notEmpty( &interChannelData.timing[conTime].timeSet64 );

    if( result )
    {

        Set64_copy( &set64Tmp, &interChannelData.timing[conTime].timeSet64 );

        Set64_findMax( &set64Tmp, id );
        while( id > 0 )
        {

            --interChannelData.timing[conTime].time[id];

            ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChTime1Error + conTime,
                           ( interChannelData.timing[conTime].time[id] > 0 ),
                           id,
                           interChannelData.paramDump[id].m_state.m_operatingValue,
                           interChannelData.paramDump[id].m_state.m_remoteValue,
                           interChannelData.paramDump[id].m_state.m_localValue );

            Set64_remove( &set64Tmp, id );

            Set64_findMax( &set64Tmp, id );
        }
    }
}

//*****************************************************************************
/// \brief Завершение синхронизации параметра.
/// \details Анализируется завершение обмена данными в обоих направлениях и 
/// принимается решение о завершении синхронизации.
/// \param id – идентификатор синхронизируемого параметра.
///
void InterChannel_synchronization( id_t id )
{
    if( ( interChannelData.paramDump[id].m_state.m_flags
          & ( FLAG_MASK( eLocal ) | FLAG_MASK( eRemote ) | FLAG_MASK( eLocalAck ) )
          )
        == ( FLAG_MASK( eLocal ) | FLAG_MASK( eRemote ) | FLAG_MASK( eLocalAck ) )
        )
    {
        // Отключаем контроль времени Т1
        InterChannel_stopTime1(id);

        // Устанавливаем признак необходимости обновить данные
        Set64_insert( &interChannelData.updateSet64, id );
    }
}

//*****************************************************************************
/// \brief Завершение синхронизации параметра по изменению значения.
/// \details При 1-ой синхронизации параметра, когда данные от соседнего канала
/// отсутствуют, функция ждет приема от обоих каналов, используя 
/// #InterChannel_synchronization.
/// \details Если синхронизация не 1-ая, функция завершает обработку после
/// успешного приема или передачи. В этом случае, при верификации будут 
/// сравниваться новые данные со старыми, полученными при предыдущей 
/// синхронизации.
/// \param id – идентификатор синхронизируемого параметра.
///
void InterChannel_synchChangeValue( id_t id )
{

    if( ( ( interChannelData.paramDump[id].m_state.m_flags
            & ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) )
            )
          == ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) )
          )
        || // Если удаленный, синхронизация так же выполнена
        // corr 04.02.21 Третьяков 
        ( ( interChannelData.paramDump[id].m_state.m_flags
            &  FLAG_MASK( eRemote ) )
          ==  FLAG_MASK( eRemote )  )
      )      
    {
        // Устанавливаем признак необходимости обновить данные
        Set64_insert( &interChannelData.updateSet64, id );
    }
    //ggggg 
#ifdef INTERCHANNEL_DEBUG_MODE
    #if ( INTERCHANNEL_DEBUG_MODE == INTERCHANNEL_DEBUG_MODE_ON  ) 
    if( id == INTERCHANNEL_DEBUG_TRACING_PARAM )
    {
        Tracing_parameterId( id, 0xF4, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
        Tracing_parameter( interChannelData.paramDump[id].m_state.m_flags, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
        Tracing_parameter( Main_getTimeWorkMs(), INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
    }
    #endif
#endif
    //ggggg 
}

//*****************************************************************************
/// \brief Завершение передачи параметра.
/// \note В качестве результата работы
/// сценария (основного или альтернативного) принимается переданное или принятое значение параметра.
/// \param id – идентификатор синхронизируемого параметра.
///
void InterChannel_transmission( id_t id )
{

    // Основной сценарий (передача параметра соседнему каналу)
    if( ( interChannelData.paramDump[id].m_state.m_flags
          & ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) ) )
        == ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) )
        )
    {

        // Проверяем, что во время выполнения основного сценария 
        // варианта использования «Передача параметр», не было приема 
        // от соседнего канала
        ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChTrError1,
                       ( !( interChannelData.paramDump[id].m_state.m_flags & FLAG_MASK( eRemote ) ) ),
                       id, 0, 0, 0 );

        interChannelData.paramDump[id].m_state.m_operatingValue
                = interChannelData.paramDump[id].m_state.m_localValue;

        // Устанавливаем признак необходимости обновить данные
        Set64_insert( &interChannelData.updateSet64, id );

        // Альтернативный сценарий (прием параметра от соседнего канала)    
    }
    else if( interChannelData.paramDump[id].m_state.m_flags & FLAG_MASK( eRemote ) )
    {

        // Проверяем, что во время выполнения альтернативного сценария 
        // варианта использования «Передача параметр», не было приема 
        // от своего канала
        ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChTrError2,
                       ( !( interChannelData.paramDump[id].m_state.m_flags
                            & ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) ) ) ),
                       id, 0, 0, 0 );

        interChannelData.paramDump[id].m_state.m_operatingValue
                = interChannelData.paramDump[id].m_state.m_remoteValue;

        // Устанавливаем признак необходимости обновить данные
        Set64_insert( &interChannelData.updateSet64, id );
    }
}

//*****************************************************************************
/// \brief Завершение передачи отладочного параметра.
/// \details Отладочный сценарий не выполняет синхронизацию данных, а только 
/// передает их по каналу связи для того, чтобы их можно было просмотреть
/// при отладке. Метод просто очищает флаги процесса выполнения.
/// \param id – идентификатор синхронизируемого параметра.
///
void InterChannel_debug( id_t id )
{

    // Основной сценарий (передача параметра соседнему каналу)
    if( ( interChannelData.paramDump[id].m_state.m_flags
          & ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) ) )
        == ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) )
        )
    {
        interChannelData.paramDump[id].m_state.m_flags &= ~( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) );

        // Альтернативный сценарий (прием параметра от соседнего канала)    
    }
    else if( interChannelData.paramDump[id].m_state.m_flags & FLAG_MASK( eRemote ) )
    {

        // Очистка флагов приема сигнала от своего, соседнего и подтверждения
        // и установка флага выполнения синхронизации
        interChannelData.paramDump[id].m_state.m_flags &= ~FLAG_MASK( eRemote );
    }
}

//*****************************************************************************
/// \brief Процедура контроля. 
/// \details Выполняет контроль значений параметров, 
/// принятых от своего и соседнего каналов, в соответствии с процедурой 
/// контроля, указанной в настройках сигнала.
/// \note Если процедура контроля указана не верно, произойдет переход в ЗС
/// с кодом #ePS_IntChProcCheck группы #eGrPS_InterChannel.
/// \param id – идентификатор синхронизируемого параметра.
/// \retval true  - данные достоверны;
/// \retval false - данные не достоверны.
///
bool InterChannel_verification( id_t id )
{

    bool result = false;

    switch( interChannelData.paramDump[id].m_settings.m_procCheck )
    {
        case eProcCheckOff:
            result = true;
            break;
        case eProcCheckEqual:
            result = ( interChannelData.paramDump[id].m_state.m_localValue
                       == interChannelData.paramDump[id].m_state.m_remoteValue ) ? true : false;
            break;
        case eProcCheckMask:
            result = (
                       ( interChannelData.paramDump[id].m_state.m_localValue
                         & interChannelData.paramDump[id].m_settings.m_paramCheck )
                       ==
                       ( interChannelData.paramDump[id].m_state.m_remoteValue
                         & interChannelData.paramDump[id].m_settings.m_paramCheck )
                       ) ? true : false;
            break;
        case eProcCheckDeltaX:
        {
            param_t param;
            if( interChannelData.paramDump[id].m_state.m_localValue
                > interChannelData.paramDump[id].m_state.m_remoteValue )
            {

                param = interChannelData.paramDump[id].m_state.m_localValue
                        - interChannelData.paramDump[id].m_state.m_remoteValue;

            }
            else
            {
                param = interChannelData.paramDump[id].m_state.m_remoteValue
                        - interChannelData.paramDump[id].m_state.m_localValue;
            }

            result = ( param <= interChannelData.paramDump[id].m_settings.m_paramCheck ) ? true : false;

            break;
        }
        case eProcCheckDeltaP:
        {

            //            uint32_t  min;
            //            uint32_t  max;
            uint16_t min;
            uint16_t max;

            if( interChannelData.paramDump[id].m_state.m_localValue
                < interChannelData.paramDump[id].m_state.m_remoteValue )
            {

                min = interChannelData.paramDump[id].m_state.m_localValue;
                max = interChannelData.paramDump[id].m_state.m_remoteValue;
            }
            else
            {
                min = interChannelData.paramDump[id].m_state.m_remoteValue;
                max = interChannelData.paramDump[id].m_state.m_localValue;
            }


            if( ( max > interChannelData.paramDump[id].m_settings.m_middleRangeCheck )
                || ( interChannelData.paramDump[id].m_settings.m_middleRangeCheck == 0 ) )
            {

                if( max > 0 && ( max - min > 0 ) )
                {
                    // При целочисленных вычислениях такой способ дает больше точность
                    // min = valToPercent( max, min );
                    min = __builtin_divud( __builtin_muluu( 100, min ), max );
                    result = ( ( 100 - ( param_t )min )
                               <= interChannelData.paramDump[id].m_settings.m_paramCheck ) ? true : false;
                }
                else
                {
                    result = true;
                }

                // Если значение меньше половины измеряемого диапазона, 
                // проценты считаем относительно половины диапазона
            }
            else
            {

                if( max - min > 0 )
                {
                    // При целочисленных вычислениях такой способ дает больше точность
                    // min = valToPercent( interChannelData.paramDump[id].m_settings.m_middleRangeCheck, max - min );
                    min = __builtin_divud( __builtin_muluu( 100, max - min ), interChannelData.paramDump[id].m_settings.m_middleRangeCheck );
                    result = ( min
                               <= interChannelData.paramDump[id].m_settings.m_paramCheck ) ? true : false;
                }
                else
                {
                    result = true;
                }
            }
            break;
        }
        default:
            ERROR_EX_ID( eGrPS_InterChannel, ePS_IntChProcCheck,
                          id, 0, interChannelData.paramDump[id].m_settings.m_procCheck, 0 );
            break;
    }

    return result;
}

//*****************************************************************************
/// \brief Синхронизация значений параметров.
/// \details Формирует синхронизированное значение из данных принятых от своего и
/// соседнего каналов, в соответствии с процедурой синхронизации, указанной 
/// в настройках сигнала.
/// \note Если процедура синхронизации указана неверно, произойдет переход в ЗС
/// с кодом #ePS_IntChProcSync группы #eGrPS_InterChannel.
/// \param id – идентификатор синхронизируемого параметра.
///
void InterChannel_synchValue( id_t id )
{
    uint32_t tmp;

    switch( interChannelData.paramDump[id].m_settings.m_procSync )
    {
            //case eProcSyncNil : // Не выполняется никаких действий с параметром 
            //    break;
        case eProcSyncOff:
        { // Нет синхронизации – формирование публичного 
            // сообщения с передаваемым параметром, сам параметр
            // не передается, применяется при отладке в 
            interChannelData.paramDump[id].m_state.m_operatingValue =
                    interChannelData.paramDump[id].m_state.m_localValue;
            break;
        }
        case eProcSyncHi:
        { // выбор большего значения;
            interChannelData.paramDump[id].m_state.m_operatingValue = MAX(
                        interChannelData.paramDump[id].m_state.m_localValue,
                        interChannelData.paramDump[id].m_state.m_remoteValue );
            break;
        }
        case eProcSyncLo:
        {// выбор меньшего значения;
            interChannelData.paramDump[id].m_state.m_operatingValue = MIN(
                        interChannelData.paramDump[id].m_state.m_localValue,
                        interChannelData.paramDump[id].m_state.m_remoteValue );
            break;
        }
        case eProcSyncAverage:
        {// выбор арифметически среднего  значения;
            tmp = ( uint32_t )interChannelData.paramDump[id].m_state.m_localValue
                    + ( uint32_t )interChannelData.paramDump[id].m_state.m_remoteValue;
            tmp /= 2U;
            interChannelData.paramDump[id].m_state.m_operatingValue = ( param_t )tmp;
            break;
        }
        case eProcSyncAND:
        {// выбор по «AND»;
            interChannelData.paramDump[id].m_state.m_operatingValue =
                    ( interChannelData.paramDump[id].m_state.m_localValue
                      & interChannelData.paramDump[id].m_state.m_remoteValue );
            break;
        }
        case eProcSyncOR:
        {// выбор по «OR»;
            interChannelData.paramDump[id].m_state.m_operatingValue =
                    ( interChannelData.paramDump[id].m_state.m_localValue
                      | interChannelData.paramDump[id].m_state.m_remoteValue );
            break;
        }
        case eProcSyncEqual:
        {// обновление, если совпадают  

            interChannelData.paramDump[id].m_state.m_operatingValue =
                    interChannelData.paramDump[id].m_state.m_remoteValue =
                    interChannelData.paramDump[id].m_state.m_localValue;

            break;
        }
        default:

            ERROR_EX_ID( eGrPS_InterChannel, ePS_IntChProcSync,
                          id, 0, interChannelData.paramDump[id].m_settings.m_procSync, 0 );
            break;
    }
    // tret 4.02.2021 InterChannel_setSyncEvent( id );
}

//*****************************************************************************
/// \brief Обновление синхронизированных данных.
/// \details Функция выполняет контроль данных своего и соседнего каналов. В случае
/// их достоверности, обновляет синхронизированное значение.
/// Эти действия вынесены из функции #InterChannel_synchronization для 
/// облегчения подпрограммы обработки прерываний при работе функции 
/// #InterChannel_runCommunication. Данная функция вызывается из функций 
/// #InterChannel_run и #InterChannel_getData.
/// \note Вызов данной функции должен быть защищен от ее прерывания потоком обмена.
/// \param id – идентификатор синхронизируемого параметра.
///
void InterChannel_updateData( id_t id )
{
    // Устанавливаем признак необходимости обновить данные
    if( Set64_hasElement( &interChannelData.updateSet64, id ) )
    {

        //ggggg   
#if ( INTERCHANNEL_DEBUG_MODE == INTERCHANNEL_DEBUG_MODE_ON )  
        if( id == INTERCHANNEL_DEBUG_TRACING_PARAM )
        {
            Tracing_parameterId( id, 0xF5, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
            Tracing_parameter( interChannelData.paramDump[id].m_state.m_flags, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
            Tracing_parameter( Main_getTimeWorkMs(), INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
        }
#endif
        //ggggg 

        // Проверка флагов статуса
        switch( interChannelData.paramDump[id].m_settings.m_script )
        {
            bool resultVerification;
            case eScriptChVal:
            {  // tret 04.02.2021 Третьяков Переписана ветка полностью
                
                param_t localValueTemp = interChannelData.paramDump[id].m_state.m_localValue;
                interChannelData.paramDump[id].m_state.m_localValue = 
                interChannelData.paramDump[id].m_state.m_lastTranslateValue; 
                resultVerification = InterChannel_verification( id );
                interChannelData.paramDump[id].m_state.m_localValue = localValueTemp;

                if( resultVerification )
                {

                    InterChannel_synchValue( id );

                    // Если верификация прошла успешно отключаем счетчик отказа
                    InterChannel_stopTime2( id );

                    //ggggg   
#if ( INTERCHANNEL_DEBUG_MODE == INTERCHANNEL_DEBUG_MODE_ON )  
                    if( id == INTERCHANNEL_DEBUG_TRACING_PARAM )
                    {
                        Tracing_parameterId( id, 0xF6, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                        Tracing_parameter( interChannelData.paramDump[id].m_state.m_operatingValue, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                        Tracing_parameter( Main_getTimeWorkMs(), INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                    }
#endif
                    //ggggg 
                }
                else
                {
                    // Если счетчик не запущен, запускаем его
                    if( InterChannel_time2IsRunning( id ) )
                    {
                        InterChannel_startTime2( id );
                    }
                    //ggggg   
#if ( INTERCHANNEL_DEBUG_MODE == INTERCHANNEL_DEBUG_MODE_ON )  
                    if( id == INTERCHANNEL_DEBUG_TRACING_PARAM )
                    {
                        Tracing_parameterId( id, 0xF7, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                        Tracing_parameter( interChannelData.paramDump[id].m_state.m_operatingValue, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                        Tracing_parameter( Main_getTimeWorkMs(), INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                    }
#endif
                    //ggggg 
                }
                
                // Очищаем флаги процесса обработки сигнала
                if ( interChannelData.paramDump[id].m_state.m_flags &     FLAG_MASK( eRemote ) ) 
                     interChannelData.paramDump[id].m_state.m_flags &= ~( FLAG_MASK( eRemote ) );
                     
                if ( ( interChannelData.paramDump[id].m_state.m_flags & 
                       ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) ) ) == 
                       ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) ) ) 
                    interChannelData.paramDump[id].m_state.m_flags &=
                      ~( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) );

                if ( resultVerification )
                {
                    if (  interChannelData.paramDump[( id )].m_state.m_flags &  FLAG_MASK( eSyncEvent ) )  
                          interChannelData.paramDump[( id )].m_state.m_flags |= FLAG_MASK( eSyncEventOverflow );
                    interChannelData.paramDump[( id )].m_state.m_flags |=   
                        ( FLAG_MASK( eSynchronized ) | FLAG_MASK( eSyncEvent ) );
                }
                break;
            }

            case eScriptSync:
            {
                if( InterChannel_verification( id ) )
                {

                    InterChannel_synchValue( id );
                    InterChannel_setSyncEvent( id ); // tret 4.02.2021
                    
                    // Если верификация прошла успешно отключаем счетчик отказа
                    InterChannel_stopTime2( id );

                    //ggggg   
#if ( INTERCHANNEL_DEBUG_MODE == INTERCHANNEL_DEBUG_MODE_ON )  
                    if( id == INTERCHANNEL_DEBUG_TRACING_PARAM )
                    {
                        Tracing_parameterId( id, 0xF6, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                        Tracing_parameter( interChannelData.paramDump[id].m_state.m_operatingValue, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                        Tracing_parameter( Main_getTimeWorkMs(), INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                    }
#endif
                    //ggggg 
                }
                else
                {
                    // Если счетчик не запущен, запускаем его
                    if( InterChannel_time2IsRunning( id ) )
                    {
                        InterChannel_startTime2( id );
                    }
                    //ggggg   
#if ( INTERCHANNEL_DEBUG_MODE == INTERCHANNEL_DEBUG_MODE_ON )  
                    if( id == INTERCHANNEL_DEBUG_TRACING_PARAM )
                    {
                        Tracing_parameterId( id, 0xF7, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                        Tracing_parameter( interChannelData.paramDump[id].m_state.m_operatingValue, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                        Tracing_parameter( Main_getTimeWorkMs(), INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                    }
#endif
                    //ggggg 
                }

                // Очищаем флаги процесса обработки сигнала
                interChannelData.paramDump[id].m_state.m_flags &=
                            ~( FLAG_MASK( eRemote ) | FLAG_MASK( eLocal ) 
                                                    | FLAG_MASK( eLocalAck ) );
                break;
            }
            case eScriptTransmit:
            {
                InterChannel_setSyncEvent( id );
                break;
            }
            case eScriptDebug:
                // Очищаем флаги процесса обработки сигнала
                interChannelData.paramDump[id].m_state.m_flags &=
                            ~( FLAG_MASK( eRemote ) | FLAG_MASK( eLocal ) 
                                                    | FLAG_MASK( eLocalAck ) );
                break;
            default:
            {
                ERROR_EX_ID( eGrPS_InterChannel, ePS_IntChScript,
                              5, id, 0, interChannelData.paramDump[id].m_settings.m_script );
                break;
            }
        }

        // Удаляем из очереди требующих обновления
        Set64_remove( &interChannelData.updateSet64, id );

        //ggggg   
#if ( INTERCHANNEL_DEBUG_MODE == INTERCHANNEL_DEBUG_MODE_ON )  
        if( id == INTERCHANNEL_DEBUG_TRACING_PARAM )
        {
            Tracing_parameterId( id, 0xF8, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
            Tracing_parameter( interChannelData.paramDump[id].m_state.m_flags, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
            Tracing_parameter( Main_getTimeWorkMs(), INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
        }
#endif
        //ggggg 

    }
}

//*****************************************************************************
/// \brief Обработка смены состояния параметра.
/// \details Функция возвращает анализ выполнения критериев завершения сценариев
/// и устанавливает признаки их завершения.
/// \param id – идентификатор синхронизируемого параметра.
///
void InterChannel_stateChanged( id_t id )
{
    // Проверка флагов статуса
    switch( interChannelData.paramDump[id].m_settings.m_script )
    {
        case eScriptChVal:
            InterChannel_synchChangeValue( id );
            break;
        case eScriptSync:
            InterChannel_synchronization( id );
            break;
        case eScriptTransmit:
            InterChannel_transmission( id );
            break;
        case eScriptDebug:
            InterChannel_debug( id );
            break;
        default:
            ERROR_EX_ID( eGrPS_InterChannel, ePS_IntChScript,
                          6, id, 0, interChannelData.paramDump[id].m_settings.m_script );
            break;
    }
}

//*****************************************************************************
/// \brief Установка флага приема и сохранение значения параметра, принятого 
/// от соседнего канала.
/// \note При неправильном идентификаторе настраиваемого параметра произойдет переход
/// в ЗС с кодом #ePS_IntChIdError группы #eGrPS_InterChannel.
/// \note При запросе синхронизации параметра, для которого не вызывалась функция 
/// #InterChannel_setParamSettings, произойдет переход в ЗС
/// с кодом #ePS_IntChScript группы #eGrPS_InterChannel.
/// \note Если параметр был принят от соседнего канала повторно, 
/// а от своего канала запрос на синхронизацию не поступал,
/// произойдет переход в ЗС с кодом #ePS_IntChRepeat группы #eGrPS_InterChannel.
/// \note Если от соседнего канала был принят параметр,
/// у которого настроен локальный сценарий, произойдет 
/// переход в ЗС с кодом #ePS_IntChRemoteLocalScr группы #eGrPS_InterChannel.
/// \param id   – идентификатор синхронизируемого параметра;
/// \param data – значение синхронизируемого параметра.
///
void InterChannel_setRemote( id_t id, param_t data )
{
    // Проверка идентификатора параметра
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError,
                   ( ( id > eInterChannelIdBegin ) && ( id < eInterChannelIdCount ) ),
                   1, id, data, eInterChannelIdCount );

    // Проверка инициализации сценария 
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChScript,
                   interChannelData.paramDump[id].m_settings.m_script != eScriptNil,
                   1, id, data, interChannelData.paramDump[id].m_settings.m_script );

    // Проверка приема параметра с локальным сценарием
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChRemoteLocalScr,
                   interChannelData.paramDump[id].m_settings.m_script != eScriptLocal,
                   1, id, data, 0 );

    // Повторный запрос на синхронизацию параметра от соседнего канала, пока предыдущий не 
    // обработан - считается ошибкой работы пользовательского алгоритма.
    ASSERT_EX_ID(  eGrPS_InterChannel,ePS_IntChRepeat,
              ! (interChannelData.paramDump[id].m_state.m_flags  & FLAG_MASK(eRemote) ),
                 1, id, data, interChannelData.paramDump[id].m_state.m_flags );

    // Установка флага приема параметра от соседнего канала Remote
    interChannelData.paramDump[id].m_state.m_flags |= FLAG_MASK( eRemote );

    // Сохранение значения параметра соседнего канала
    interChannelData.paramDump[id].m_state.m_remoteValue = data;

    // При установки флага от соседнего канала, если флаг от своего канала  
    // не установлен, выполняем инициализацию контрольных времен.
    if( !( interChannelData.paramDump[id].m_state.m_flags & FLAG_MASK( eLocal ) ) )
    {
        InterChannel_startTime13( id );
    }

    // Сбросить признак выполненной синхронизации            
    interChannelData.paramDump[id].m_state.m_flags &= ~FLAG_MASK( eSynchronized );

    //ggggg   
#if ( INTERCHANNEL_DEBUG_MODE == INTERCHANNEL_DEBUG_MODE_ON )  
    if( id == INTERCHANNEL_DEBUG_TRACING_PARAM )
    {
        Tracing_parameterId( id, 0xF3, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
        Tracing_parameter( data, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
        Tracing_parameter( Main_getTimeWorkMs(), INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
    }
#endif
    //ggggg 

    InterChannel_stateChanged( id );

}

//*****************************************************************************
/// \brief Установка флага подтверждения приема.
/// \note При неправильном идентификаторе настраиваемого параметра произойдет переход
/// в ЗС с кодом #ePS_IntChIdError группы #eGrPS_InterChannel.
/// \note Если параметр был принят от соседнего канала повторно,
/// а от своего канала запрос на синхронизацию не поступал,
/// произойдет переход в ЗС с кодом #ePS_IntChRepeat группы #eGrPS_InterChannel.
/// \param id – идентификатор синхронизируемого параметра.
///
void InterChannel_setLocalAck( id_t id )
{
    // Проверка идентификатора параметра
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError,
                   ( ( id > eInterChannelIdBegin ) && ( id < eInterChannelIdCount ) ),
                   2, id, 0, eInterChannelIdCount );

    // Установка флага приема параметра от соседнего канала eLocalAck
    interChannelData.paramDump[id].m_state.m_flags |= FLAG_MASK( eLocalAck );
    
    interChannelData.paramDump[id].m_state.m_lastTranslateValue = // tret 04.02.2021
    interChannelData.paramDump[id].m_state.m_localValue; 
    //ggggg   
#if ( INTERCHANNEL_DEBUG_MODE == INTERCHANNEL_DEBUG_MODE_ON )  
    if( id == INTERCHANNEL_DEBUG_TRACING_PARAM )
    {
        Tracing_parameterId( id, 0xF2, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
        Tracing_parameter( interChannelData.paramDump[id].m_state.m_flags, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
        Tracing_parameter( Main_getTimeWorkMs(), INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
    }
#endif
    //ggggg 

    InterChannel_stateChanged( id );

}

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация переменных компонента
void InterChannel_ctor( const ArrayIoDriver *drvPtr )
{
    int i;

    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIoDriverError, drvPtr != 0,
                   1, 1, 0, 0 );

    sysLock();

    interChannelData.drvPtr = drvPtr;

    interChannelData.tmp.isTrasmitting = false;
    interChannelData.tmp.trId1 = 0;
    interChannelData.tmp.trId2 = 0;
    interChannelData.stateLock = false;

    for( i = 0; i < eInterChannelIdCount; i++ )
    {
        interChannelData.paramDump[i].m_settings.m_script = eScriptNil;
        interChannelData.paramDump[i].m_settings.m_procSync = eProcSyncNil;
        interChannelData.paramDump[i].m_settings.m_procCheck = eProcCheckNil;
        interChannelData.paramDump[i].m_settings.m_paramCheck = 0;
        interChannelData.paramDump[i].m_settings.m_middleRangeCheck = 0;
        interChannelData.paramDump[i].m_settings.m_time[eInterChannelConTime1] = 0;
        interChannelData.paramDump[i].m_settings.m_time[eInterChannelConTime2] = 0;
        interChannelData.paramDump[i].m_settings.m_time[eInterChannelConTime3] = 0;

        interChannelData.paramDump[i].m_state.m_flags = 0;
        interChannelData.paramDump[i].m_state.m_localValue = 0;
        interChannelData.paramDump[i].m_state.m_remoteValue = 0;
        interChannelData.paramDump[i].m_state.m_operatingValue = 0;
    }

    Set64_clear( &interChannelData.set64 );

    for( i = 0; i < eInterChannelConTimeCount; i++ )
    {
        InterChannel_stopTimeForAllId( i );  
    }

    interChannelStorageRemoteData.count = 0;
    interChannelStorageAck.count = 0;

    sysUnLock();
}

//*****************************************************************************
// Настройка сценариев синхронизации параметра
void InterChannel_setParamSettings( id_t id,
                                    InterChannelScript script,
                                    InterChannelProcSync procSync,
                                    InterChannelProcCheck procCheck,
                                    param_t paramCheck, // Параметр контроля
                                    param_t middleRangeCheck, // Значение половины измерительного диапазона контролируемого параметра
                                    uint16_t time1, // Время Т1
                                    uint16_t time2, // Время Т2
                                    uint16_t time3 // Время Т3
                                    )
{
    // Проверка идентификатора
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   4, id, 0, eInterChannelIdCount );
    // Проверка сценария синхронизации
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChScript,
                   ( ( script > eScriptNil )&&( script < eScriptCount ) ),
                   4, id, 0, script );
    // Проверка использования процедуры контроля eProcCheckEqual 
    // при заданной процедуре синхронизации eProcSyncEqual.
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChProcCheck, 
                  ( eProcSyncEqual == procSync ) ? ( eProcCheckEqual == procCheck) : true ,
                  id, script, procCheck, procSync  );
    // Проверка корректности задания времени Т1. Если используется сценарий
    // синхронизации eScriptSync, время time1 обязательно должно быть задано 
    // (быть больше нуля). Для остальных сценариев - должно быть рано нулю.
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChT1ValueError,  
                  ( eScriptSync == script ) ? ( time1 > 0 ) : ( time1 == 0),
                  id, script, time1, 0 );

    interChannelData.paramDump[id].m_settings.m_script = script;
    interChannelData.paramDump[id].m_settings.m_procSync = procSync;
    interChannelData.paramDump[id].m_settings.m_procCheck = procCheck;
    interChannelData.paramDump[id].m_settings.m_paramCheck = paramCheck;
    interChannelData.paramDump[id].m_settings.m_middleRangeCheck = middleRangeCheck;
    interChannelData.paramDump[id].m_settings.m_time[eInterChannelConTime1] = time1;
    interChannelData.paramDump[id].m_settings.m_time[eInterChannelConTime2] = time2;
    interChannelData.paramDump[id].m_settings.m_time[eInterChannelConTime3] = time3;
}
//*****************************************************************************
// Запрос синхронизации параметра
void InterChannel_synchronize( id_t id, param_t data )
{
    //-------------------------------------------------------------------------
    // Проверка обязательных условий
    //-------------------------------------------------------------------------

    // Проверка идентификатора параметра
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   3, id, data, eInterChannelIdCount );

    // Повторный запрос на синхронизацию параметра, пока предыдущий не 
    // обработан считается ошибкой работы пользовательского алгоритма.
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChRepeat,
                   !( interChannelData.paramDump[id].m_state.m_flags & ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) ) ),
                   3, id, data, interChannelData.paramDump[id].m_state.m_flags );
    //-------------------------------------------------------------------------
    // Постановка сигнала на обработку
    //-------------------------------------------------------------------------

    switch( interChannelData.paramDump[id].m_settings.m_script )
    {
        case eScriptChVal:
        case eScriptSync:
        case eScriptDebug:
        case eScriptTransmit:
        {

            sysLock();

            // Сохранение значения своего параметра
            interChannelData.paramDump[id].m_state.m_localValue = data;

            // Установка флага приема от своего канала Local
            interChannelData.paramDump[id].m_state.m_flags |= FLAG_MASK( eLocal );

            // При установке флага от соседнего канала, если флаг от своего канала не 
            // установлен, выполняем инициализацию контрольных времен.
            if( !( interChannelData.paramDump[id].m_state.m_flags & FLAG_MASK( eRemote ) ) )
            {
                InterChannel_startTime13( id );
            }

            // Сбросить признак выполненной синхронизации            
            interChannelData.paramDump[id].m_state.m_flags &= ~FLAG_MASK( eSynchronized );


            //ggggg   
#if ( INTERCHANNEL_DEBUG_MODE == INTERCHANNEL_DEBUG_MODE_ON )  
            if( id == INTERCHANNEL_DEBUG_TRACING_PARAM )
            {
                Tracing_parameterId( id, 0xF1, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                Tracing_parameter( data, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                Tracing_parameter( Main_getTimeWorkMs(), INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
            }
#endif
            //ggggg 

            // Проверка смены состояний параметра 
            InterChannel_stateChanged( id );

            // Установить признак сообщения, ожидаемого передачу
            Set64_insert( &interChannelData.set64, id );

            sysUnLock();

            break;
        }
        case eScriptLocal:
        {
            sysLock();
            interChannelData.paramDump[id].m_state.m_localValue = data; // Значение параметра своего канала
            interChannelData.paramDump[id].m_state.m_remoteValue = data; // Значение параметра соседнего канала
            interChannelData.paramDump[id].m_state.m_operatingValue = data; // Значение параметра после сравнения

            InterChannel_startTime13( id );

            InterChannel_setSyncEvent( id );

            sysUnLock();
            break;
        }
        default:
        {
            ERROR_EX_ID( eGrPS_InterChannel, ePS_IntChScript,
                          3, id, data, interChannelData.paramDump[id].m_settings.m_script );
            break;
        }
    }
}

//*****************************************************************************
// Признак завершения синхронизации параметра
bool InterChannel_isSynchronized( id_t id )
{
    // Проверка идентификатора параметра
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   8, id, 0, eInterChannelIdCount );

    return (  interChannelData.paramDump[id].m_state.m_flags 
            & FLAG_MASK( eSynchronized ) ) ? true : false;
}

//*****************************************************************************
// Событие синхронизации.
bool InterChannel_syncEvent( id_t id )
{ 
    bool result;
    
    // Проверка идентификатора параметра
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   11, id, 0, eInterChannelIdCount );
    
    result = (  interChannelData.paramDump[id].m_state.m_flags 
            & FLAG_MASK( eSyncEvent ) ) ? true : false ;
    
    interChannelData.paramDump[id].m_state.m_flags &= ~FLAG_MASK( eSyncEvent );
    
    return result;
}

//*****************************************************************************
// Признак переполнения событий синхронизации.
bool InterChannel_syncEventOverflow( id_t id )
{
    bool result;
    
    // Проверка идентификатора параметра
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   12, id, 0, eInterChannelIdCount );
    
    result = (  interChannelData.paramDump[id].m_state.m_flags 
            & FLAG_MASK( eSyncEventOverflow ) ) ? true : false ;
    
    interChannelData.paramDump[id].m_state.m_flags &= 
                                             ~FLAG_MASK( eSyncEventOverflow );
    
    return result;
}

//*****************************************************************************
// Признак процесса выполнения синхронизации параметра
bool InterChannel_isHandling( id_t id )
{
    // Проверка идентификатора параметра
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   7, id, 0, eInterChannelIdCount );

    return( interChannelData.paramDump[id].m_state.m_flags
            & FLAG_MASK( eLocal ) ) ? true : false;

}

//*****************************************************************************
// Чтение синхронизированного значения параметра
param_t InterChannel_getData( id_t id )
{
    param_t result;

    // Проверка идентификатора параметра
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   9, id, 0, eInterChannelIdCount );

    result = interChannelData.paramDump[id].m_state.m_operatingValue;
    return result;
}

//*****************************************************************************
// Чтение значения своего канала
param_t InterChannel_getLocalData( id_t id )
{
    param_t result;

    // Проверка идентификатора параметра
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   10, id, 0, eInterChannelIdCount );

    result = interChannelData.paramDump[id].m_state.m_localValue;

    return result;
}

//*****************************************************************************
// Установка значения параметра в физическом протоколе
void InterChannel_setCommunicationData( uint8_t *buf, int size, int commPos, id_t id, param_t data )
{
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChInputDriverGetError, size >= 6,
                   12, size, sizeof(buf ), 0 );
    switch( commPos )
    {
        case eInterChannelCommPos0:
            buf[0] = id;
            SET_COMMUNICATION_DATA( data, buf[1], buf[2] );
            break;
        case eInterChannelCommPos1:
            buf[3] = id;
            SET_COMMUNICATION_DATA( data, buf[4], buf[5] );
            break;
        default:
            buf[0] = 0;
            buf[1] = 0;
            buf[2] = 0;
            buf[3] = 0;
            buf[4] = 0;
            buf[5] = 0;
            break;
    }
}

//*****************************************************************************
// Получает значение параметра в физическом протоколе.
void InterChannel_getCommunicationData( uint8_t *buf, int size, int commPos, id_t *id, param_t *data )
{
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChInputDriverGetError, size >= 6,
                   3, size, sizeof(buf ), 0 );
    switch( commPos )
    {
        case eInterChannelCommPos0:
            *id = buf[0];
            *data = GET_COMMUNICATION_DATA( buf[1], buf[2] );
            break;
        case eInterChannelCommPos1:
            *id = buf[3];
            *data = GET_COMMUNICATION_DATA( buf[4], buf[5] );
            break;
        default:
            *id = 0;
            *data = 0;
            break;
    }
}

//*****************************************************************************
// Опрос драйвера о готовности
void InterChannel_runDrv( void )
{
    interChannelData.drvPtr->run( interChannelData.drvPtr );
}

//*****************************************************************************
// Работа с линией связи с соседним каналом
void InterChannel_runCommunication( bool transmitIsEnabled )
{
    uint16_t i;

    static uint16_t bufWord[REMODE_DATA_BLOCK_SIZE / sizeof(uint16_t) ];
    uint8_t *bufChar = (uint8_t *)bufWord;

    DASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIoDriverError, drv != 0,
                    2, 2, 0, 0 );
    DASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIoDriverError, drv->isInReady != 0,
                    2, 3, 0, 0 );
    DASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIoDriverError, drv->get != 0,
                    2, 4, 0, 0 );
    DASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIoDriverError, drv->isOutReady != 0,
                    2, 5, 0, 0 );
    DASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIoDriverError, drv->set != 0,
                    2, 6, 0, 0 );
    DASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIoDriverError, drv->run != 0,
                    2, 7, 0, 0 );

    // Опрос приемника
    if( interChannelData.drvPtr->isInReady( interChannelData.drvPtr ) )
    {
        i = interChannelData.drvPtr->get( interChannelData.drvPtr, bufChar, sizeof( bufWord ) );

        // Проверка корректности длины данных
        ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChInputDriverGetError, i == sizeof( bufWord ),
                       1, i, sizeof( bufWord ), 0 );

        // Помещение в хранилище принятых данных
        ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChInputDriverGetError,
                       interChannelStorageRemoteData.count < REMODE_DATA_STORAGE_SIZE,
                       3, interChannelStorageRemoteData.count, REMODE_DATA_STORAGE_SIZE, 0 );


        memcpy( &interChannelStorageRemoteData.items[interChannelStorageRemoteData.count],
                bufChar, sizeof( bufWord ) );
        interChannelStorageRemoteData.count++;

        //ggggg   
#if ( INTERCHANNEL_DEBUG_MODE == INTERCHANNEL_DEBUG_MODE_ON )  
        if(  bufChar[0] == INTERCHANNEL_DEBUG_TRACING_PARAM ||  bufChar[3] == INTERCHANNEL_DEBUG_TRACING_PARAM )
        {
            Tracing_parameterId( Main_getTimeWorkInterrupt(), 0xFB, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
            Tracing_parameter( (  bufChar[3] << 8 ) | (  bufChar[0] & 0x00FF ), INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
            Tracing_parameter( Main_getTimeWorkMs(), INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
        }
#endif
        //ggggg
    }

    // Опрос передатчика 
    if( interChannelData.drvPtr->isOutReady( interChannelData.drvPtr ) )
    {

        // Проверка подтверждения
        if( interChannelData.tmp.isTrasmitting )
        {
            //ggggg   
#if ( INTERCHANNEL_DEBUG_MODE == INTERCHANNEL_DEBUG_MODE_ON )  
            if( interChannelData.tmp.trId1 == INTERCHANNEL_DEBUG_TRACING_PARAM || interChannelData.tmp.trId2 == INTERCHANNEL_DEBUG_TRACING_PARAM )
            {
                Tracing_parameterId( Main_getTimeWorkInterrupt(), 0xFA, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                Tracing_parameter( ( interChannelData.tmp.trId2 << 8 ) | ( interChannelData.tmp.trId1 & 0x00FF ), INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
                Tracing_parameter( Main_getTimeWorkMs(), INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
            }
#endif
            //ggggg 
            if( interChannelData.tmp.trId1 > 0 )
            {
                // Помещение в хранилище принятых данных
                ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChInputDriverGetError,
                               interChannelStorageAck.count < sizeof(interChannelStorageAck.items ),
                               4, interChannelStorageAck.count, sizeof(interChannelStorageAck.items ), 1 );

                interChannelStorageAck.items[interChannelStorageAck.count]
                        = interChannelData.tmp.trId1;
                interChannelStorageAck.count++;
            }

            if( interChannelData.tmp.trId2 > 0 )
            {
                // Помещение в хранилище принятых данных
                ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChInputDriverGetError,
                               interChannelStorageAck.count < sizeof(interChannelStorageAck.items ),
                               4, interChannelStorageAck.count, sizeof(interChannelStorageAck.items ), 2 );

                interChannelStorageAck.items[interChannelStorageAck.count]
                        = interChannelData.tmp.trId2;
                interChannelStorageAck.count++;
            }

            interChannelData.tmp.isTrasmitting = false;

            // Для того, что бы не загружать прерывания передачу начинаем 
            // со следующего прерывания после получения подтверждения.

        }
        else if ( transmitIsEnabled )
        {
            
            Set64_findMax( &interChannelData.set64, interChannelData.tmp.trId1 );
            if( interChannelData.tmp.trId1 > 0 )
            {
                Set64_remove( &interChannelData.set64, interChannelData.tmp.trId1 );
                bufChar[0] = interChannelData.tmp.trId1;
                SET_COMMUNICATION_DATA( interChannelData.paramDump[
                                        interChannelData.tmp.trId1].m_state.m_localValue, bufChar[1], bufChar[2] );
                Set64_findMax( &interChannelData.set64, interChannelData.tmp.trId2 );
                if( interChannelData.tmp.trId2 > 0 )
                {
                    Set64_remove( &interChannelData.set64, interChannelData.tmp.trId2 );
                    bufChar[3] = interChannelData.tmp.trId2;
                    SET_COMMUNICATION_DATA( interChannelData.paramDump[
                                            interChannelData.tmp.trId2].m_state.m_localValue, bufChar[4], bufChar[5] );
                }
                else
                {
                    bufChar[3] = 0;
                    bufChar[4] = 0;
                    bufChar[5] = 0;
                }
                interChannelData.drvPtr->set( interChannelData.drvPtr, bufChar, sizeof( bufWord ) );
                interChannelData.tmp.isTrasmitting = true;
            }
        }
    }
}

//*****************************************************************************
// Синхронизация параметров
void InterChannel_run( void )
{
    uint8_t bufRemoteData[REMODE_DATA_STORAGE_SIZE][REMODE_DATA_BLOCK_SIZE];
    uint16_t countRemoteData;
    id_t bufAck[REMODE_DATA_STORAGE_SIZE * 2];
    uint16_t countAck;
    int i;
    param_t data;
    id_t id;

    //--------------------------------------------------------------------------
    // Копирование данных, собранных между вызовами InterChannel_run, из 
    // хранилищ в буфер.
    // Данная секция кода должна быть защищена от прерывания в работе 
    // функцией InterChannel_runCommunication

    sysLock();

    // Извлечение из хранилища данных полученных от соседнего канала           
    memcpy( bufRemoteData, interChannelStorageRemoteData.items,
            REMODE_DATA_BLOCK_SIZE *interChannelStorageRemoteData.count );
    countRemoteData = interChannelStorageRemoteData.count;
    interChannelStorageRemoteData.count = 0;

    // Извлечение из хранилища идентификаторов сообщений, на которые были 
    // получены подтверждения
    memcpy( bufAck, interChannelStorageAck.items,
            sizeof(id_t ) *interChannelStorageAck.count );
    countAck = interChannelStorageAck.count;
    interChannelStorageAck.count = 0;

    sysUnLock();

    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // Обработка данных, собранных между вызовами InterChannel_run

    // Обработка данных, полученных от соседнего канала               
    for( i = 0; i < countRemoteData; ++i )
    {
        id = bufRemoteData[i][0];
        data = GET_COMMUNICATION_DATA( bufRemoteData[i][1], bufRemoteData[i][2] );
        if( id > eInterChannelIdBegin )
        {
            InterChannel_setRemote( id, data );
        }

        id = bufRemoteData[i][3];
        data = GET_COMMUNICATION_DATA( bufRemoteData[i][4], bufRemoteData[i][5] );
        if( id > eInterChannelIdBegin )
        {
            InterChannel_setRemote( id, data );
        }

    }

    // Обработка идентификаторов сообщений, на которые были 
    // получены подтверждения
    for( i = 0; i < countAck; ++i )
    {
        InterChannel_setLocalAck( bufAck[i] );
    }
    
    //--------------------------------------------------------------------------

    ++mainTime;

    // Обновление синхронизированных данных
    Set64_findMax( &interChannelData.updateSet64, id );
    while( id > 0 )
    {

        //ggggg   
#if ( INTERCHANNEL_DEBUG_MODE == INTERCHANNEL_DEBUG_MODE_ON )  
        if( id == INTERCHANNEL_DEBUG_TRACING_PARAM )
        {
            Tracing_parameterId( id, 0xF9, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
            Tracing_parameter( interChannelData.paramDump[id].m_state.m_flags, INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
            Tracing_parameter( Main_getTimeWorkMs(), INTERCHANNEL_DEBUG_TRACING_ARRAY ); //ggggg
        }
#endif
        //ggggg 

        InterChannel_updateData( id );
        Set64_findMax( &interChannelData.updateSet64, id );
    }

    // Контроль времен (без Т3) 
    for( i = 0; i < ( eInterChannelConTimeCount - 1 ); i++ )
    { 
        InterChannel_checkTime( i );
    }
    MARKED_CALL_FUNCTION;
}

//*****************************************************************************
/**
* История изменений: 
* 
* Версия 1.0.1
* Автор  Годунок А.Н.
* 
* Изменения:
*    Базовая версия, отработанная на плате ГКЛС-К.
*    
* Версия 1.0.2
* Автор  Годунок А.Н.
* 
* Изменения:
*    Добавлен новый сценарий синхронизаций – 
*    "синхронизация по изменению значения".
* 
* Версия 1.0.3
* Дата   22-04-2016
* Автор  Годунок А.Н.
* 
* Изменения:
*    Добавлен метод \a InterChannel_getLocalData.
*
* Версия 1.0.4
* Дата   03-08-2016
* Автор  Годунок А.Н.
* 
* Изменения:
*    1. При запросе на синхронизацию, флаг синхронизации (eSynchronized)
*    сбрасывался. А при получении данных из соседнего канала нет. 
*    Исправлено: При получении данных от соседнего канала, 
*    флаг синхронизации так же сбрасывается.
*    2. Синхронизация по изменению уровню (eScriptChVal) выполняет 
*    обработку только когда, получены данные от соседа (eRemote), 
*    или только когда, данные переданы и получено подтверждение 
*    (eLocalAck && eLocal). 
*    Поскольку прием осуществляется в прерываниях, а обработка в main, 
*    может возникать коллизия. Когда данные приняты от соседа (eRemote), 
*    устанавливается запрос на обработку (updateSet64), а до выполнения этого
*    запроса в main может поступить запрос на передачу (eLocal). После 
*    выполнения обработки флаги сбрасываются (eLocalAck && eLocal && eRemote),
*    и устанавливается флаг синхронизации (eSynchronized). 
*    А после получения подтверждения взводится флаг подтверждения (eLocalAck),
*    при том, что флага запроса (eLocal) сброшен. Дальше алгоритм разваливается.
* 
* Исправлено: 
*    Для разрешения коллизии в функции обработки выполняется  
*    дополнительный анализ флагов, и в случае не выполнения условия  
*    " только (eRemote) или только оба (eLocalAck) и (eLocal) ", сбрасывается 
*    флаг приема (eRemote), удаляется из очереди запрос на обработку  
*    (updateSet64), но флаг синхронизации (eSynchronized) не устанавливается.
*    Синхронизация выполнится при получении подтверждения (eLocalAck).
*
* Версия 1.0.5
* Дата   09-09-2017
* Автор  Годунок А.Н.
* 
* Проблема:
*    Было обнаружено, что при работе функции  InterChannel_updateData
*    по сценарию eScriptChVal, может возникнуть коллизия. Например, после передачи
*    параметра было получено подтверждение о приеме. Для данного сценария это является
*    признаком  того, что данные готовы к обработке.  InterChannel_updateData начинает
*    их обрабатывать, верификация не проходит (поскольку от соседа не было приема нового
*    значения) и тут функция прерывается прерыванием в котором появляются данные от соседа
*    и устанавливается соответствующий признак. Но после возврата управления 
*    InterChannel_updateData продолжает обработку и затирает признак наличия данных от соседа.
*    Для предотвращения данного дефекта необходимо запрещать прерывания потока обмена данными
*    между каналами.
* 
* Изменения:
*    1. В архитектуре ПО предусмотрено то, что функция InterChannel_runCommunication
*    выполняется из потока, который может блокироваться и запускаться при помощи функций
*    sysLock( ), sysUnLock( );
*    2. Для блокировки используется файл макросы из файла drv\MainRegisters.h;
*    3. Функции, которые могут быть прерваны потоком обмена, свою работу защищают, 
*    блокируя поток обмена при помощи sysLock( ), sysUnLock( ) и макросом ATOMIC_CAN_TIMER.
*
* Версия 1.0.6
* Дата   27-12-2017
* Автор  Годунок А.Н.
*
* Проблема:
*    1. Функция InterChannel_getData вызывала функцию InterChannel_updateData, 
*    которая выполняет проверку наличия данных, подготовленных к синхронизации, если таковые
*    имеются, синхронизирует их. Когда в двух процессорах загрузка основного потока 
*    неравномерна, может получиться так что в одном канале данные будут готовы к 
*    синхронизации, а в другом нет. Что приводит к тому, что в двух каналах данные могут
*    отличаться на одну миллисекунду. Данная функция вызывается в из функции InterChannel_run,
*    поэтому все функции работающие до ее выполнения будут работать с одинаковыми данным. 
*    Необходимо убрать вызов InterChannel_updateData из InterChannel_getData.
*    2. В ГКЛС-К применена система контроля вызовов функций, функция InterChannel_run 
*    вызывается из главного потока, поэтому в нее необходимо добавить соответствующий макрос.
* 
* Изменения:
*    1. Удален вызов функции InterChannel_updateData из InterChannel_getData.
*    2. В функцию InterChannel_run добавлен макрос системы контроля вызовов
*    MARKED_CALL_FUNCTION.
*    3. Размер буфера для передачи данных уменьшен с 8-ми до 6-ти, поскольку
*    за 1-ин раз передается два параметра с идентификаторам, которые занимают 6 байт.
* 
* Версия 1.0.7
* Дата   26-06-2017
* Автор  Годунок А.Н.
*
* Проблема:
*    1. В функции InterChannel_verification при использовании процедуры 
*    верификации eProcCheckDeltaP при значениях синхронизируемых параметров 
*    от обоих каналов равных нулю, а так же при задании в качестве середины 
*    диапазона значения нуля при пересчете значений в проценты происходило
*    деление на ноль и результат верификации был отрицательным.
* 
* Изменения:
*    1. В функции InterChannel_verification при использовании процедуры 
*    верификации eProcCheckDeltaP производится проверка середины диапазона и 
*    разницы значений двух каналов на ноль. 
* 
* Версия 1.0.8
* Дата   04-07-2018
* Автор  Годунок А.Н.
* 
* Проблема:
*    В одном канале событие завещающее транзакцию по синхронизации параметра 
*    (прием данных от соседа или подтверждение передачи) происходило в 
*    прерывании до анализа в InterChannel_run и параметр считался 
*    синхронизированным уже на этом цикле основного потока. А в другом канале 
*    происходило позже, и синхронизация параметра на этом цикле не происходила. 
*    Два процессора расходились на цикл. А если канал, в котором произошла 
*    синхронизация, на следующем цикле слал новое значение, то в отстающем 
*    канале еще и не происходила верификация. И дальше работа двух каналов
*    разваливалась
* 
* Изменения:
*    Изменена стратегия в работе обмена данными между основным потоком 
*    и потоком синхронизации (InterChannel_run и InterChannel_runCommunication).
*    Добавлена функция InterChannel_runDrv, которая считывает состояние линии 
*    связи при помощи функции драйвера run и должна вызваться в потоке 
*    синхронизации времени для того, что бы в обоих каналах состояние линии связи
*    определялось в один момент времени. В совою очередь 
*    функция InterChannel_runCommunication, которая работает в блокируемом потоке
*    синхронизации данных, обрабатывает только те состояние, которые были 
*    зафиксированы функция InterChannel_runDrv. При этом функция 
*    InterChannel_runCommunication не вызывает функций модифицирующие флаги 
*    межканального обмена и данные межканального обмена, а 
*    помещает данные, принятые от соседнего канала, и идентификаторы сообщений, 
*    для которых было получено подтверждение приема, в 
*    специальные хранилища \a interChannelStorageRemoteData 
*    и \a interChannelStorageAck. Функция InterChannel_run должна вызываться 
*    всегда в начале нового цикла, она блокирует поток синхронизации данных, 
*    вычитывает данные из хранилищ и дальше обрабатывает их. 
*    Таким образом, все данные полученные в прерывания на предыдущем цикле
*    будут обработаны на следующем.
*
* Версия 1.0.9
* Дата   04-07-2018
* Автор  Годунок А.Н.
* 
* Проблема: 
*     Выполнение кода не вмещается в цикл 1 мс.
* Изменения:
*     1) valToPercent заменен на __builtin_divud( __builtin_muluu(..., ...), ...);
*
* Версия 1.0.10
* Дата   23-05-2019
* Автор  Годунок А.Н. 
*
* Изменения:
*    1) Удалено неиспользуемое поле m_checkErrorCount из структуры InterChannelParamDump.
*    2) Удалены:
*       – определение типа InterChannelTrPhase
*       – параметр trPhase из прототипа функции void InterChannel_ctor( const void * drvPtr,  
*       InterChannelTrPhase trPhase );
*       – параметры currentPhase, setPhase из структуры InterChannelData.
*    3) В функцию InterChannel_ctor добавлена инициализация поля 
*    interChannelData.paramDump[i].m_settings.m_middleRangeCheck.
*    4) Замечание по вызову InterChannel_updateData и маркированию функций компоненты 
*    было устранено в 1.0.6 27-12-2017.
*    5) В функции InterChannel_setParamSettings добавлен контроль значения времени Т1. Для сценария 
*    синхронизации eScriptSync оно должно быть задано обязательно. А для остальных должно быть равно нулю.
*    6) В функцию InterChannel_setRemote добавлена проверка, повторного запроса на синхронизацию от 
*    соседнего канала. Теперь если от соседнего канала придет параметр, который уже приходил, но еще не
*    был обработан, произойдет отказ с группы eGrPS_InterChannel и кодом ePS_IntChRepeat.
*    7) Процедура синхронизации eProcSyncEqual, которая запускается поле успешного выполнения процедуры
*    контроля, присваивает переменным operatingValue и remoteValue значение переменной localValue. 
*    Исходя из этого, процедура синхронизации eProcSyncEqual не может быть использована отдельно от 
*    процедуры контроля eProcCheckEqual. Для предотвращения этого в функцию InterChannel_setParamSettings 
*    добавлен контроль того, что при использовании процедуры синхронизации eProcSyncEqual процедура контроля
*    указана eProcCheckEqual. В случае невыполнения этого условия пробор будет переведен в ЗС.
*    8) Добавлены макроопределения InterChannel_stopTime1, InterChannel_stopTime2, InterChannel_stopTime3, 
*    InterChannel_time1IsRunning, InterChannel_time2IsRunning, InterChannel_time3IsRunning 
*    и InterChannel_stopTimeForAllId.
*    9) Из-за изменений в структуре драйверов ArrayIoDriver изменен прототип функции 
*    InterChannel_runCommunication( const ArrayIoDriver *drv ) на InterChannel_runCommunication( void ),
*    поскольку теперь таблица функций и данные объединены в одной структуре.
*    Также изменен прототип функции void InterChannel_ctor( const void *drvPtr ) на 
*    void InterChannel_ctor( const ArrayIoDriver *drvPtr ), а у поля drvPtr структуры InterChannelData
*    заменен тип с const void * на const ArrayIoDriver *.
*
* Версия 1.0.11
* Дата   08-07-2019
* Автор  Годунок А.Н. 
*
* Проблема: 
*    1) Обработка данных, собранных в прерываниях потока синхронизации данных, осуществляется
*    модулем МКО в начале нового цикла главного потока. И если события в одном канале, будь то 
*    прием данных или подтверждение приема, произошли на одно прерывание раньше или позже, чем во
*    втором канале. Может оказаться, что в одном канале параметр за синхронизирован на текущем цикле,
*    а во втором за синхронизируется только на следующем.
*
* Изменения:
*    1) Для устранения данной проблемы было введено ограничение на передачу данных перед
*    началом нового цикла. Ограничение запрещает выдавать данные в CAN шину, если до начала 
*    нового цикла осталось времени меньше чем на передачу двух сообщений. Одно сообщение с одной
*    стороны, другое с другой. Данное ограничение обеспечит отсутствие формирования событий приема
*    данных и подтверждения приема, а значит, в начале главного цикла в обоих каналах будут 
*    обработаны одинаковые события и данные.
*    В модуль внесены следующие изменения:
*        - в функцию InterChannel_runCommunication добавлен параметр transmitIsEnabled, который 
*    разрешает или запрещает выдачу данных на шину CAN.
* 
* Версия 1.0.12
* Дата   23-08-2019
* Автор  Третьяков В.Ж. 
*
* Изменения:
*  Убраны пути во включаемых файлах. Сделано для лучшей переносимости в другие проекты. 
* 
* Версия 1.0.13
* Дата   13-09-2019
* Автор  Годунок А.Н. 
*
* Проблема: 
*    1) При выполнении сценария eScriptTransmit принимающая сторона не могла
*    определить, что пришли новые данные. Функция InterChannel_getData всегда 
*    возвращает последнее синхронизированное значение, если передаются параметры, 
*    имеющие одинаковое значение, их нельзя различить. Функция 
*    InterChannel_isSynchronized сбрасывается при поступлении нового запроса 
*    от своего или от соседнего канала. Поскольку принимающая сторона не шлет 
*    данных, а при приеме флаг сбрасывается буквально до следующего цикла главного 
*    потока, после чего снова взводится, у нее нет возможности определить, когда 
*    пришли новые данные.
*
* Изменения:
*    1) Чтобы не нарушать существующую логику для устранения данной проблемы в 
*    МКО вводятся две функции InterChannel_syncEvent и 
*    InterChannel_syncEventOverflow - событие синхронизации и признак 
*    переполнения события синхронизации. Событие синхронизации устанавливается 
*    одновременно с признаком окончания синхронизации, но сбрасывается при вызове 
*    этой функции. Таким образом, пользовательское ПО может опрашивать это событие,
*     а когда оно наступило вычитывать данные при помощи InterChannel_getData. 
*    Признак переполнения события синхронизации введен для того, что бы можно было 
*    проконтролировать, что не было пропущенных данных. Данный признак взведется, 
*    если произойдет несколько событий после последнего обращения пользовательского
*    ПО к функции InterChannel_syncEvent. При чтении данный признак сбрасывается.
* 
* 
* Версия 1.0.14
* Дата   21-01-2021
* Автор  Третьяков В. Ж. 
*
* Проблема: 
*    1) При выполнении сценария eScriptChVal в функции InterChannel_synchChangeValue
*       проводился анализ наличия только удаленного запроса путем сравнения поля флагов с 
*       константой. В версии 1.0.13 поле флагов было расширено, и это сравнение перестало работать.
* Изменения:
*    На поле флагов наложена маска, и после этого проводится сравнение. 
* 
* Версия 1.0.15
* Дата   04-02-2021
* Автор  Третьяков В.Ж.
*
* Проблема: 
*       При выполнении сценария eScriptChVal возникал переход в ЗС.
*      Связано с ошибкой алгоритма этого сценария.
* Изменения: изменен алгоритм.
* Подробно описано в документе "Описание изменений InterChannel версии 1.0.15"    
* 
*/
