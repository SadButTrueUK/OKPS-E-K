/**
* \file    EepromDrv.c
* \brief   \copybrief EepromDrv.h
*
* \version 2.0.1
* \date    08-01-2017
* \author  Третьяков В.Ж.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "asserts.h"
#include "wait.h"
#include "ModeProtection.h"
#include "EepromDrv.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
#define ADDRESS_DEVICE      0xA0        ///< Адрес устройства по шине I2C.
#define N_ATTEMPT           3           ///< Количество попыток обмена по шине I2C.
#define TIME_OPERATION_LINK 256         ///< Максимальное время обмена по шине I2C, 16 мс.
#define TIME_WRITE_EEPROM   96          ///< Время записи в \a EEPROM, 6 мс.
#define SIZE_EEPROM_DATA         4096        ///< Размер используемой области памяти \a EEPROM в байтах.

//*****************************************************************************
// Макросы типа функция
//*****************************************************************************

//*****************************************************************************
/// \brief Проверка утверждения с использованием типа и идентификатора исключения.
/// \details Макрос проверяет, что условие \a test_ выполняется. Если аргумент
/// \a test_ имеет значение false, происходит переход в ЗС
/// или ЗО (недоступен \a EEPROM для фиксации кода отказа).
/// \param group_ - тип отказа;
/// \param id_ - идентификатор отказа;
/// \param test_ - проверяемое условие.
///
#define ASSERT_ID_EEPROM( group_, id_, test_ )                     \
    do                                                             \
    {                                                              \
        if( !( test_ ) )                                           \
        {                                                          \
            if( me->modeProtection == eEeProtectionState )         \
            {                                                      \
                ModeProtection_run( SET_CODE_ID( group_, id_ ) ); \
            }                                                      \
            else                                                   \
            {                                                      \
                 ModeProtectiont_transmitToWait( SET_CODE_ID( group_, id_ ) );  \
            }                                                      \
        }                                                          \
    } while( 0 )

//*****************************************************************************
// Объявление типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Времена контроля синхронизации параметров модуля \a EEPROM, мс.
///
typedef enum
{
    eEE_T1 = 50,         ///< максимальное время неприхода 2-го параметра
    eEE_T2 = 100,        ///< максимальное время несовпадения параметров
    eEE_T3 = 0           ///< максимальное время обновления параметра (не контролируем)
} Eeprom_timeSynchr;

//*****************************************************************************
/// \brief Идентификаторы состояния работы с устройством \a EEPROM.
///
typedef enum
{
    eEESW_ready               = 0,        ///< устройство готово к работе
    eEESW_begin,                          ///< запуск операции
    eEESW_waiteEndI2C,                    ///< ожидание завершения обмена по шине I2C 
    eEESW_startSynchroEndI2C,             ///< запуск синхронизации результатов обмена по шине I2C
    eEESW_waiteSynchroEndI2C,             ///< ожидание синхронизации результатов обмена по шине I2C
    eEESW_EndI2C,                         ///< конец работы с обменом по шине I2C 
    eEESW_waiteWriteEeprom                ///< задержка на запись в \a EEPROM
} eEepromStateWork;

//*****************************************************************************
/// \brief Структура объекта работы с \a EEPROM.
///
typedef struct EepromDrvTag
{
    eEeExistence         exist;                 ///< Наличие \a EEPROM, (true (присутствует)).
    InterChannelId       icId;                  ///< Идентификатор синхронизируемого параметра.
    InterChannelProcSync script;                ///< Сценарий обмена данных.
    bool                 synchro;               ///< Признак синхронизации данных.
    eEepromStateWork     stateWork;             ///< Состояние автомата. 
    eEeProtectionModes   modeProtection;        ///< Тип защитного режима, в который уходит объект.
    const void           *drvPtr;               ///< Указатель на данные драйвера канала связи.
    uint16_t             addressCell;           ///< Адрес ячейки в \a EEPROM.
    eI2C_modes           modeEeprom;            ///< Тип операции: чтение или запись. 
    uint16_t             dataEeprom;            ///< Данные \a EEPROM.
    uint16_t             dataRezult;            ///< Данные \a EEPROM, синхронизированные.
    uint16_t             cWaite;                ///< Счетчик ожидания операции по шине I2C.
    uint16_t             cAttempt;              ///< Счетчик попыток операции по шине I2C.
} EepromDrv;

//*****************************************************************************
/// \brief Объекты работы с \a EEPROM.
/// \note \a eeprom - для сохранения данных;
/// \note \a eepromCalibration - для сохранения калибровочных коэффициентов.
///
struct EepromDrvTag eepromDrv;

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация переменных
void EepromDrv_ctor( const void *self,
                     eEeExistence exist,
                     eI2C_modules numberModule,
                     uint8_t modeProtection,
                     InterChannelProcSync script,
                     InterChannelId icId )
{
    EepromDrv *me = ( EepromDrv * )self;
    if( modeProtection >= eEeProtectionAmount )
    {
         ModeProtectiont_transmitToWait( SET_CODE_ID( eGrPS_Eeprom, ePS_EepromParameters ) );
    }
    me->modeProtection = modeProtection;

    me->exist = exist;
    if( me->exist )
    {
        ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromParameters, numberModule < eI2C_moduleAmount );
        me->drvPtr = I2Cdrv_ctor( numberModule, ADDRESS_DEVICE, eIO_openDrainOff, I2C_F_100 );
        ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromInicLink, me->drvPtr != 0 );
    }

    me->icId = icId;
    me->stateWork = eEESW_ready;

    me->script = script;
    if( script == eProcSyncEqual )
    {
        InterChannel_setParamSettings( icId, eScriptSync, script, eProcCheckEqual,
                                       0, 0, eEE_T1, eEE_T2, eEE_T3 );
    }
    else
    {
        if( script == eProcSyncHi )
        {
            InterChannel_setParamSettings( icId, eScriptSync, script, eProcCheckOff,
                                           0, 0, eEE_T1, eEE_T2, eEE_T3 );
        }
        else
        {
            ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromParameters, false ); // Недопустимый сценарий синхронизации
        }
    }
}

//*****************************************************************************
// Рабочий цикл работы с EEPROM
void EepromDrv_run( const void *self )
{
    EepromDrv *me = ( EepromDrv * )self;

    switch( me->stateWork )
    {
        case eEESW_ready:
            break;
        case eEESW_begin:
            if( me->exist )
            { // Есть EEPROM
                bool rezult = I2Cdrv_set( me->drvPtr, me->modeEeprom, me->addressCell,
                                          ( uint8_t * ) & me->dataEeprom, 2 );
                ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromAccess, rezult );
                me->cWaite = 0;
                me->stateWork = eEESW_waiteEndI2C;
            }
            else
            {
                me->stateWork = eEESW_startSynchroEndI2C;
            }
            break;
        case eEESW_waiteEndI2C:
            if( I2Cdrv_isReady( me->drvPtr ) )
            {
                if( I2Cdrv_isOperationOk( me->drvPtr ) )
                {
                    me->dataRezult = me->dataEeprom;
                    me->stateWork = eEESW_startSynchroEndI2C;
                }
                else
                {
                    ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromAccess, ++me->cAttempt < N_ATTEMPT );
                    me->stateWork = eEESW_begin;
                }
            }
            else
            {
                if( ++me->cWaite > TIME_OPERATION_LINK )
                {
                    ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromAccess, 0 ); //Подвисание драйвера I2C
                    me->stateWork = eEESW_begin;
                }
            }
            break;
        case eEESW_startSynchroEndI2C:
            if( me->synchro )
            {
                if( ( me->script == eProcSyncEqual ) || ( me->exist ) )
                {
                    InterChannel_synchronize( me->icId, me->dataEeprom );
                }
                else
                {
                    InterChannel_synchronize( me->icId, 0 );
                }
                me->stateWork = eEESW_waiteSynchroEndI2C;
            }
            else
            {
                me->dataRezult = me->dataEeprom;
                me->stateWork = eEESW_EndI2C;
            }
            break;
        case eEESW_waiteSynchroEndI2C:
            if( InterChannel_isSynchronized( me->icId ) )
            {
                me->dataRezult = ( uint16_t )( InterChannel_getData( me->icId ) );
                me->stateWork = eEESW_EndI2C;
            }
            break;
        case eEESW_EndI2C:
            if( me->modeEeprom == eI2C_modeWrite )
            {
                me->cWaite = 0;
                me->stateWork = eEESW_waiteWriteEeprom;
            }
            else
            {
                me->stateWork = eEESW_ready;
            }
            break;
        case eEESW_waiteWriteEeprom:
            if( ++me->cWaite > TIME_WRITE_EEPROM )
            {
                me->stateWork = eEESW_ready;
            }
            break;
        default:
            me->stateWork = eEESW_ready;
            break;
    }
    I2Cdrv_run( me->drvPtr );
}

//*****************************************************************************
// Начать процедуру записи в EEPROM 16-битного значения
void EepromDrv_setWrite( const void *self, uint16_t addressCell, uint16_t data )
{
    EepromDrv *me = ( EepromDrv * )self;

    ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromParameters,
                      ( me->stateWork == eEESW_ready ) &&
                      ( addressCell < SIZE_EEPROM_DATA ) &&
                      ( ( addressCell % 2 ) == 0 ) );
    me->modeEeprom = eI2C_modeWrite;
    me->synchro = true;
    me->addressCell = addressCell;
    me->dataEeprom = data;
    me->cAttempt = 0;
    me->stateWork = eEESW_begin;
}

//*****************************************************************************
// Начать процедуру чтения в EEPROM 16-битного значения
void EepromDrv_setRead( const void *self, uint16_t addressCell )
{
    EepromDrv *me = ( EepromDrv * )self;

    ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromParameters,
                      ( me->stateWork == eEESW_ready ) &&
                      ( addressCell < SIZE_EEPROM_DATA ) &&
                      ( ( addressCell % 2 ) == 0 ) );
    me->modeEeprom = eI2C_modeRead;
    me->synchro = true;
    me->addressCell = addressCell;
    me->cAttempt = 0;
    me->stateWork = eEESW_begin;
}

//*****************************************************************************
// Запись в EEPROM 16-битного значения, одноканальный режим работы
void EepromDrv_write( const void *self, uint16_t addressCell, uint16_t data )
{
    EepromDrv *me = ( EepromDrv * )self;

    ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromAccess, me->exist );
    EepromDrv_setWrite( me, addressCell, data );
    me->synchro = false;

    while( me->stateWork != eEESW_ready )
    {
        EepromDrv_run( me );
        wait62mks( );
    }
}


//*****************************************************************************
// Старт записи в EEPROM 16-битного значения, одноканальный режим работы
void EepromDrv_write_start( const void *self, uint16_t addressCell, uint16_t data )
{
    EepromDrv *me = ( EepromDrv * )self;

    ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromAccess, me->exist );
    EepromDrv_setWrite( me, addressCell, data );
    me->synchro = false;
}

//*****************************************************************************
// Чтение из EEPROM 16-битного значения, одноканальный режим работы
uint16_t EepromDrv_read( const void *self, uint16_t addressCell )
{
    EepromDrv *me = ( EepromDrv * )self;

    ASSERT_ID_EEPROM( eGrPS_Eeprom, ePS_EepromAccess, me->exist );
    EepromDrv_setRead( me, addressCell );
    me->synchro = false;

    while( me->stateWork != eEESW_ready )
    {
        EepromDrv_run( me );
        wait62mks( );
    }
    return me->dataRezult;
}

//*****************************************************************************
// Готовность EEPROM к чтению или записи данных, предыдущая операция завершена
bool EepromDrv_isReady( const void *self )
{
    EepromDrv *me = ( EepromDrv * )self;
    return( me->stateWork == eEESW_ready ) ? true : false;
}

//*****************************************************************************
// Получить данные, считанные из EEPROM
uint16_t EepromDrv_getData( const void *self )
{
    EepromDrv *me = ( EepromDrv * )self;
    return me->dataRezult;
}

//*****************************************************************************
/**
* История изменений:
* 
* Версия 1.0.1
* Дата   19-12-2016
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Базовая версия.
* 
* Версия 2.0.1
* Дата   08-01-2017
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Изменения под новую схему на dsPIC33.
*/
