/**
* \file    I2Cdrv_PIC33.c
* \brief   \copybrief I2Cdrv_PIC33.h
*
* \version 1.0.1
* \date    30-01-2017
* \author  Третьяков В.Ж.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "typeMK.h"
#include "defCompil.h"
#include "I2Cdrv_PIC33.h"
#include "Tracing.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
#define N_ARRAY 16        ///< Размер массивов для приема и передачи данных.
#define N_WAITE 2         ///< Время ожидания в количествах вызова функции.

//*****************************************************************************
// Определение типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief  Определение указателей на регистры интерфейсной шины I2C.
///
typedef struct
{
    uint16_t RCV;        ///< Регистр приемника.
    uint16_t TRN;        ///< Регистр передатчика.
    uint16_t BRG;        ///< Регистр делителя тактовой частоты.

    /// \brief CON: регистр контроля.
    ///
    union
    {
        uint16_t    CON;                    ///< Регистр CON в формате \a uint16_t.
        I2C1CONBITS CONbits;                ///< Регистр CON в формате для битового доступа.
    };

    /// \brief STAT: регистр статуса.
    ///
    union
    {
        uint16_t     STAT;                    ///< Регистр STAT в формате \a uint16_t.
        I2C1STATBITS STATbits;                ///< Регистр STAT в формате для битового доступа.
    };

    uint16_t ADD;        ///< Регистр адреса, режим Slave.
    uint16_t MSK;        ///< Регистр маски адреса, режим Slave.
} Ds33EP512GM710_I2C_Regs;

//*****************************************************************************
/// \brief Идентификаторы состояния функции #I2Cdrv_run.
///
typedef enum
{
    eReady                = 0,         ///< ожидание запуска операции чтения или записи
    eOutStart             = 1,         ///< начало операции чтения или записи, выдача start-бита
    eOutAddressDevice     = 2,         ///< выдача адреса устройства
    eOutAddressCell1      = 3,         ///< выдача адреса памяти (регистра), старший байт
    eOutAddressCell2      = 4,         ///< выдача адреса памяти (регистра), младший байт
    eOutData              = 5,         ///< выдача данных
    eOutStop              = 6,         ///< конец операции чтения или записи, выдача stop-бита
    eWaiteOutBit          = 7,         ///< ожидание завершения передачи/приема бита
    eWaiteOutByte         = 8,         ///< ожидание завершения передачи/приема байта
    eOutReStart           = 9,         ///< выдача рестарта
    eOutAddressDeviceRead = 10,        ///< выдача адреса устройства c битом чтения
    eRCEN                 = 11,        ///< разрешение приема
    eInData               = 12         ///< прием данных 
} eI2C_run;

//*****************************************************************************
/// \brief Структура данных драйвера I2C.
///
typedef struct
{
    uint8_t                          addressDevice;         ///< Адрес устройства.
    uint16_t                         addressCell;           ///< Адрес ячейки данных в устройстве.
    uint8_t                          aData[N_ARRAY];        ///< Массив данных.
    uint8_t                          *pData;                ///< Указатель на массив данных для операций приема.
    uint8_t                          ndx;                   ///< Индекс массива данных.
    uint8_t                          length;                ///< Длина принимаемых или передаваемых данных.
    eI2C_modes                       mode;                  ///< Режим: прием или передача данных.
    bool                             rezult;                ///< Результат приема или передачи данных: (true (норма)).
    uint16_t                         cWaite;                ///< Счетчик ожидания передачи или приема байта, бита.
    eI2C_run                         stateRun;              ///< Состояние функции #I2Cdrv_run.
    volatile Ds33EP512GM710_I2C_Regs *pRegs;                ///< Указатель на регистры модуля I2C.

    /// \brief Состояние функции #I2Cdrv_run, в которое она должна перейти после состояний \a wait.
    /// 
    eI2C_run statePost;

    /// \brief Указатель на функцию опроса флага \a I2C: MI2C1IF или \a MI2C2IF.
    /// 
    bool( *isMI2CxIF ) ( void ); ///< 
} I2Cdrv;

//*****************************************************************************
// Определение локальных переменных
//*****************************************************************************

//*****************************************************************************
static I2Cdrv aI2Cdrv[eI2C_moduleAmount];        ///< Данные драйверов I2C.

//*****************************************************************************
/// \brief Определение блока адресов регистров I2C модуля 1.
/// 
extern volatile Ds33EP512GM710_I2C_Regs module1_I2C_Regs __attribute__( ( sfr( 0x0200 ) ) );

//*****************************************************************************
/// \brief Определение блока адресов регистров I2C модуля 2.
/// 
extern volatile Ds33EP512GM710_I2C_Regs module2_I2C_Regs __attribute__( ( sfr( 0x0210 ) ) );

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Опрос и сброс флага \a MI2C1IF, модуль 1.
/// \retval true  - флаг был установлен;
/// \retval false - флаг не был установлен. 
///
bool isMI2C1IF( void );

//*****************************************************************************
/// \brief Опрос и сброс флага \a MI2C2IF, модуль 2.
/// \retval true  - флаг был установлен;
/// \retval false - флаг не был установлен.
///
bool isMI2C2IF( void );

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Конструктор
const void *I2Cdrv_ctor( eI2C_modules numberModule,
                         uint8_t addressDevice,
                         eIO_openDrain openDrain,
                         uint16_t brg )
{
    if( ( numberModule >= eI2C_moduleAmount ) || ( openDrain >= eIO_openDrainAmount ) ) return 0;

    I2Cdrv *me = &aI2Cdrv[numberModule];

    me->addressDevice = addressDevice;
    me->stateRun = eReady;
    me->ndx = 0;
    me->rezult = false;
    me->cWaite = 0;

    if( numberModule == eI2C_module1 )
    {
        me->pRegs = &module1_I2C_Regs;
        me->isMI2CxIF = isMI2C1IF;
        INIC_PORT_IN( C, 4, eIO_pullOff );
        INIC_PORT_IN( C, 5, eIO_pullOff );
        IFS1bits.MI2C1IF = 0;
    }
    else
    {
        me->pRegs = &module2_I2C_Regs;
        me->isMI2CxIF = isMI2C2IF;
        INIC_PORT_IN( B, 5, eIO_pullOff );
        INIC_PORT_IN( B, 6, eIO_pullOff );
        IFS3bits.MI2C2IF = 0;
    }
    me->pRegs->CONbits.A10M = 0; // 7-битный адрес Slave устройства
    me->pRegs->BRG = brg;
    me->pRegs->CONbits.I2CEN = 1; // Разрешение I2C, конфигурирование входов

    return me;
}

//*****************************************************************************
// Обмен по шине I2C
void I2Cdrv_run( const void *self )
{
    I2Cdrv *me = ( I2Cdrv * )( self );

    switch( me->stateRun )
    {
        case eReady:
            break;
        case eOutStart:
            me->pRegs->CONbits.SEN = 1;
            me->statePost = eOutAddressDevice;
            me->stateRun = eWaiteOutBit;
            me->ndx = 0U;
            break;
        case eOutAddressDevice:
            me->pRegs->TRN = me->addressDevice;
            me->statePost = eOutAddressCell1;
            me->stateRun = eWaiteOutByte;
            break;
        case eOutAddressCell1:
            me->pRegs->TRN = ( uint8_t )( ( me->addressCell & 0xFF00 ) >> 8 );
            me->statePost = eOutAddressCell2;
            me->stateRun = eWaiteOutByte;
            break;
        case eOutAddressCell2:
            me->pRegs->TRN = ( uint8_t )( me->addressCell & 0x00FF );
            if( me->mode == eI2C_modeWrite )
            {
                me->statePost = eOutData;
                me->stateRun = eWaiteOutByte;
            }
            else
            {
                me->statePost = eOutReStart;
                me->stateRun = eWaiteOutBit;
            }
            break;
        case eOutData:
            if( me->ndx != me->length )
            {
                me->pRegs->TRN = me->aData[me->ndx++];
                me->stateRun = eWaiteOutByte;
            }
            else
            {
                me->rezult = true;
                me->stateRun = eOutStop;
            }
            break;
            // Чтение
        case eOutReStart:
            me->pRegs->CONbits.RSEN = 1;
            me->statePost = eOutAddressDeviceRead;
            me->stateRun = eWaiteOutBit;
            break;
        case eOutAddressDeviceRead:
            me->pRegs->TRN = me->addressDevice | 1;
            me->ndx = 0;
            me->statePost = eRCEN;
            me->stateRun = eWaiteOutByte;
            break;
        case eRCEN:
            me->pRegs->CONbits.RCEN = 1;
            me->statePost = eInData;
            me->stateRun = eWaiteOutBit;
            break;
        case eInData:
            if( me->pRegs->STATbits.RBF == 1 )
            {
                me->ndx++;
                *me->pData++ = me->pRegs->RCV;
                if( me->ndx == me->length )
                {
                    me->pRegs->CONbits.ACKDT = 1;
                    me->rezult = true;
                    me->statePost = eOutStop;
                }
                else
                {
                    me->pRegs->CONbits.ACKDT = 0;
                    me->statePost = eRCEN;
                }
                me->pRegs->CONbits.ACKEN = 1;
                me->stateRun = eWaiteOutBit;
            }
            else
            {

            }
            break;
        case eWaiteOutBit:
            if( ( *me->isMI2CxIF )( ) )
            {
                me->stateRun = me->statePost;
                me->cWaite = 0;
            }
            else
            {
                if( me->cWaite++ > N_WAITE )
                {
                    me->pRegs->CONbits.PEN = 1;
                    me->stateRun = eReady;
                    me->rezult = false;
                    me->cWaite = 0;
                }
            }
            break;

        case eWaiteOutByte:
            if( ( *me->isMI2CxIF )( ) )
            {
                if( me->pRegs->STATbits.ACKSTAT == 0 )
                {
                    me->stateRun = me->statePost;
                }
                else
                {
                    me->stateRun = eOutStop;
                    me->rezult = false;
                }
                me->cWaite = 0;
            }
            else
            {
                if( me->cWaite++ > N_WAITE )
                {
                    me->stateRun = eOutStop;
                    me->rezult = false;
                    me->cWaite = 0;
                }
            }
            break;
        case eOutStop:
            me->pRegs->CONbits.PEN = 1;
            me->statePost = eReady;
            me->stateRun = eWaiteOutBit;
            break;
    }
}

//*****************************************************************************
// Установка параметров операции и запуск операции на шине I2C
bool I2Cdrv_set( const void *self, uint8_t mode, uint16_t addressCell, uint8_t *aData, uint8_t length )
{
    bool ret;
    I2Cdrv *me = ( I2Cdrv * )( self );

    if( ( ( me == &aI2Cdrv[eI2C_module1] ) || ( me == &aI2Cdrv[eI2C_module2] ) ) &&
        ( mode < eI2C_modeAmount ) && ( length <= N_ARRAY ) && ( me->stateRun == eReady ) )
    {
        me->mode = mode;
        me->addressCell = addressCell;
        me->length = length;
        if( mode == eI2C_modeWrite )
        {
            uint8_t i;
            for( i = 0; i < length; i++ )
            {
                me->aData[i] = aData[i];
            }
        }
        else me->pData = aData;
        me->stateRun = eOutStart;
        ret = true;
    }
    else ret = false;

    return ret;
}

//*****************************************************************************
// Запрос готовности для новой операции на шине I2C
bool I2Cdrv_isReady( const void *self )
{
    I2Cdrv *me = ( I2Cdrv * )( self );
    return( me->stateRun == eReady ) ? true : false;
}

//*****************************************************************************
// Запрос результата операции на шине I2C
bool I2Cdrv_isOperationOk( const void *self )
{
    I2Cdrv *me = ( I2Cdrv * )( self );
    return me->rezult;
}

//*****************************************************************************
// Реализация локальных функций
//*****************************************************************************

//*****************************************************************************
// Опрос и сброс флага MI2C1IF, модуль 1
bool isMI2C1IF( void )
{
    if( IFS1bits.MI2C1IF != 0 )
    {
        IFS1bits.MI2C1IF = 0;
        return true;
    }
    else return false;
}

//*****************************************************************************
// Опрос и сброс флага MI2C2IF, модуль 2
bool isMI2C2IF( void )
{
    if( IFS3bits.MI2C2IF != 0 )
    {
        IFS3bits.MI2C2IF = 0;
        return true;
    }
    else return false;
}

//*****************************************************************************
/**
* Пояснения:
*    Реализованы только функции мастера.  
*    Попыток повторной передачи байта не делаем, потому что
*    если до мастера ложно не дошел Asc, то будет лишняя запись
*    по следующему адресу.
*    Нет ухода в ЗС, потому что это может быть
*    связь с EEPROM, где фиксируется код отказа.
*/

//*****************************************************************************
/**
* История изменений:
* 
* Версия 1.0.1
* Дата   30-01-2017
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Базовая версия.
*/
