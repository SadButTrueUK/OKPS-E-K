/**
* \file    Ecan_driverPIC33.c
* \brief   \copybrief Ecan_driverPIC33.h
*
* \version 1.0.1
* \date    06-10-2017
* \author  Третьяков В.Ж.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "typeMK.h"
#include "asserts_ex.h"
#include "ProtectionState_codes.h"
#include "Ecan_regsPIC33.h"
#include "MainRegisters.h"
#include "CheckRam.h"
#include "Main.h"
#include "Tracing.h"
#include "ECAN_driverPIC33.h"
#include "Ecan_DMA.h"
#include "ECAN1_PinRemap.h"
#include "ECAN2_PinRemap.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
/// \brief Преобразование указателя на данные в указатель на \a ECAN.
/// \param self_ – указатель на данные.
///
#define DRV_DATA( self_ ) ( ( Ecan * ) self_ )

//*****************************************************************************
/// \brief Указатель на регистры модуля \a CAN. 
/// \details Макрос предназначен для упрощения записи указателей на регистры
/// модуля \a CAN.
/// \param self_ – указатель на структуру \a ECAN.
/// 
#define DRV_REGS( self_ ) ( ( volatile Ecan * ) self_ )->pCanRegs

//*****************************************************************************
#define NUM_BUFF_IN            8         ///< Номер буфера для хранения принятых сообщений.
#define NUM_BUFF_OUT           0         ///< Номер буфера для хранения передаваемых сообщений.
#define NAME_BIT_RXFULL        RXFUL8    ///< Имя бита с принятой информацией.
#define NUMBER_OF_ECAN_MESSAGE 32        ///< Количество буферов сообщений для модуля \a ECAN.
#define SIZE_OF_ECAN_MESSAGE   8         ///< Размер буфера сообщения для модуля \a ECAN.

//*****************************************************************************
// Определение типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Состояние передатчика.
///
typedef enum
{
    eEcanTrStateIdle         = 0,        ///< передатчик свободен и готов к работе
    eEcanTrStateInProgress,              ///< передатчик в процессе передачи данных
    //  eEcanTrStateRequest,             ///< передатчик поступил запрос на передачу данных
    eEcanTrStateError                    ///< передача закончилась ошибкой
} EcanTrState;

//*****************************************************************************
/// \brief Объединение для байтового доступа к регистру \a CxINTF.
///
typedef union
{
    unsigned short data;        ///< Переменная для байтового доступа.
    C1INTFBITS     bits;        ///< Структура C1INTFBITS.
} TempINTF;

//*****************************************************************************
/// \brief Данные драйвера.
///
typedef struct
{
    bool        receiverIsReady;                  ///< Данные приняты и готовы к чтению.
    EcanTrState transmissionState;                ///< Состояние передатчика.
    TempINTF    tempINTF;                         ///< Буфер для хранения значения регистра \a INTF.
    EcanMode    workMode;                         ///< Режим работы модуля.
    uint16_t    lengthPacket;                     ///< Длина пакета данных.
    volatile    dsPIC33_CanRegs *pCanRegs;        ///< Указатель на регистры модуля \a CAN.
    uint8_t     i;                                ///< Счетчик.
    uint16_t    *pData;                           ///< Указатель массива данных.
} Ecan;

//*****************************************************************************
// Определение локальных типизированных констант
//*****************************************************************************

//*****************************************************************************
const uint16_t MAX_DATA_LENGHT = 8;        ///< Максимальная длина пакета данных.

//*****************************************************************************
// Определение локальных переменных
//*****************************************************************************

//*****************************************************************************
static Ecan aEcan[eEcanCount];        ///< Данные драйверов \a ECAN.

//*****************************************************************************
/// \brief Буфера сообщений модуля \a ECAN1.
///
__eds__ uint16_t aEcan1MsgBuf[NUMBER_OF_ECAN_MESSAGE][SIZE_OF_ECAN_MESSAGE]
__attribute__( ( address( RAM_END_ADDRESS ) ) );

//*****************************************************************************
/// \brief Буфера сообщений модуля \a ECAN2.
///
__eds__ uint16_t aEcan2MsgBuf[NUMBER_OF_ECAN_MESSAGE][SIZE_OF_ECAN_MESSAGE]
__attribute__( ( address( RAM_END_ADDRESS + NUMBER_OF_ECAN_MESSAGE * 16 ) ) );

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************   
/// \brief Перевод модуля \a CAN в заданный режим.
/// \param self - указатель на данные объекта, который вернул конструктор;
/// \param mode - заданный режим.
///
void Ecan_setMode( const void *self, uint16_t mode );

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************  
// Сброс модуля CAN.
//void Ecan_reset( const void *self );

//*****************************************************************************
// Конструктор
const void *Ecan_ctor( EcanNumber numEcan,
                       uint16_t txSid,
                       uint16_t rxSid,
                       uint16_t mode,
                       uint16_t lengthPacket )
{
    Ecan *pEcan;
    unsigned long localAddress;

    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvNumPort,
               ( numEcan >= eEcan1 ) && ( numEcan < eEcanCount ) );
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSetSize,
               lengthPacket <= MAX_DATA_LENGHT );

    pEcan = &aEcan[numEcan];
    pEcan->receiverIsReady = false;
    pEcan->transmissionState = eEcanTrStateIdle;
    pEcan->lengthPacket = lengthPacket;

    switch( numEcan )
    {
        case eEcan1:
            pEcan->pCanRegs = &dsPIC33_Can1Regs;
            ECAN1_PIN_REMAP;
            ECAN_CONFIG_DMA( DMA0, eDMA_readRAM, eDMA_ECAN1_TX, C1TXD, aEcan1MsgBuf, localAddress );
            ECAN_CONFIG_DMA( DMA1, eDMA_writeRAM, eDMA_ECAN1_RX, C1RXD, aEcan1MsgBuf, localAddress );
            aEcan1MsgBuf[NUM_BUFF_OUT][0] = ( txSid << 2 );
            aEcan1MsgBuf[NUM_BUFF_OUT][1] = 0x0000;
            aEcan1MsgBuf[NUM_BUFF_OUT][2] = lengthPacket;
            break;
        case eEcan2:
            pEcan->pCanRegs = &dsPIC33_Can2Regs;
            ECAN2_PIN_REMAP;
            ECAN_CONFIG_DMA( DMA2, eDMA_readRAM, eDMA_ECAN2_TX, C2TXD, aEcan2MsgBuf, localAddress );
            ECAN_CONFIG_DMA( DMA3, eDMA_writeRAM, eDMA_ECAN2_RX, C2RXD, aEcan2MsgBuf, localAddress );
            aEcan2MsgBuf[NUM_BUFF_OUT][0] = ( txSid << 2 );
            aEcan2MsgBuf[NUM_BUFF_OUT][1] = 0x0000;
            aEcan2MsgBuf[NUM_BUFF_OUT][2] = lengthPacket;
            break;
        default:
            ERROR_ID( eGrPS_CanDrv, ePS_CanDrvNumPort );
            break;
    }
    pEcan->workMode = mode; // режим работы модуля

    //****************************************************************************
    // Настройка модуля CAN

    Ecan_setMode( ( const void * )pEcan, eConfigMode );

    pEcan->pCanRegs->CTRL1bits.WIN = 0;    // Окно регистров 0
    pEcan->pCanRegs->CTRL1bits.CSIDL = 1;  // Запрет работы CAN в режиме "Idle" 
    pEcan->pCanRegs->CTRL1bits.CANCKS = 1; // Выбор частоты Fcan = 2 * 59,904 MHz = 119,808 MHz
    pEcan->pCanRegs->CTRL1bits.CANCAP = 1; // CAN capture включен

    pEcan->pCanRegs->CFG1bits.SJW = 0; // Ширина перехода синхронизации равна 1*Tq 
    pEcan->pCanRegs->CFG1bits.BRP = 7; // Определяем Tq (квант времени) равным 119.808 MHz / 2 / 8
    // = 135/2 = 7,488 MHz 
    // Номинальное время передачи бита (Nominal Bit Time - NBT) 
    // = 8=**q = (936000 бит/с)
    pEcan->pCanRegs->CFG2bits.PRSEG = 1;
    pEcan->pCanRegs->CFG2bits.SEG1PH = 1;
    pEcan->pCanRegs->CFG2bits.SEG2PH = 2;
    pEcan->pCanRegs->CFG2bits.SAM = 1;
    pEcan->pCanRegs->CFG2bits.SEG2PHTS = 0;
    pEcan->pCanRegs->CFG2bits.WAKFIL = 0;

    pEcan->pCanRegs->FCTRLbits.DMABS = 0b110;

    // Конфигурация приемника CAN 
    pEcan->pCanRegs->FMSKSEL1bits.F0MSK = 0;  // Select Acceptance Filter Mask 0 for Acceptance Filter 0
    pEcan->pCanRegs->CTRL1bits.WIN = 1;       // Окно регистров 1
    pEcan->pCanRegs->RXM0SID = ( 0x7FF << 5 );
    pEcan->pCanRegs->RXF0SIDbits.SID = rxSid; // Configure Acceptance Filter 0 to match standard identifier
    pEcan->pCanRegs->RXM0SIDbits.MIDE = 0x1;  // Acceptance Filter 0 to check for Standard Identifier
    pEcan->pCanRegs->RXF0SIDbits.EXIDE = 0x0;
    pEcan->pCanRegs->BUFPNT1bits.F0BP = NUM_BUFF_IN; // Acceptance Filter 0 to use Message Buffer xx to store message
    pEcan->pCanRegs->CTRL1bits.WIN = 0;              // Окно регистров 0
    pEcan->pCanRegs->FEN1bits.FLTEN0 = 0x1;          // Filter 0 enabled for Identifier match with incoming message 
    pEcan->pCanRegs->CTRL1bits.WIN = 0x0;            // Clear Window Bit to Access ECAN

    // Конфигурация передатчика CAN 
    /* Configure Message Buffer 0 for Transmission and assign priority */
    pEcan->pCanRegs->TR01CONbits.TXEN0 = 1;    // Configure Message Buffer xx for Transmission
    pEcan->pCanRegs->TR01CONbits.TX0PRI = 0x3; // Configure Message  priority 

    pEcan->pCanRegs->CTRL1bits.WIN = 0;
    Ecan_setMode( ( const void * )pEcan, pEcan->workMode );

    pEcan->pCanRegs->INTF = 0;

    return( const void * )pEcan;
}

//***************************************************************************** 
// Чтение данных из периферийного устройства  
int Ecan_get( const void *self, uint8_t *array, int size )
{

    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSelf,
               ( ( self == &aEcan[eEcan1] ) || ( self == &aEcan[eEcan2] ) ) );
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSetSize,
               ( ( DRV_DATA( self )->lengthPacket == size ) && ( ( size % 2 ) == 0 ) ) );

    DRV_DATA( self )->pData = ( uint16_t * )array;

    if( self == &aEcan[eEcan1] )
    {
        for( DRV_DATA( self )->i = 0; DRV_DATA( self )->i < ( size / 2 ); DRV_DATA( self )->i++ )
        {
            DRV_DATA( self )->pData[DRV_DATA( self )->i] = aEcan1MsgBuf[NUM_BUFF_IN][3 + DRV_DATA( self )->i];
        }
    }
    else
    {
        for( DRV_DATA( self )->i = 0; DRV_DATA( self )->i < ( size / 2 ); DRV_DATA( self )->i++ )
        {
            DRV_DATA( self )->pData[DRV_DATA( self )->i] = aEcan2MsgBuf[NUM_BUFF_IN][3 + DRV_DATA( self )->i];
        }
    }

    DRV_DATA( self )->receiverIsReady = false;

    return DRV_DATA( self )->lengthPacket;
}

//*****************************************************************************
// Готовность устройства к чтению принятых данных
bool Ecan_isInReady( const void *self )
{
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSelf,
               ( ( self == &aEcan[eEcan1] ) || ( self == &aEcan[eEcan2] ) ) );

    return DRV_DATA( self )->receiverIsReady;
}

//*****************************************************************************
// Передача данных драйверу для вывода
void Ecan_set( const void *self, uint8_t *array, int size )
{
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSelf,
               ( ( self == &aEcan[eEcan1] ) || ( self == &aEcan[eEcan2] ) ) );
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSetSize,
               ( ( DRV_DATA( self )->lengthPacket == size ) && ( ( size % 2 ) == 0 ) ) );

    DRV_DATA( self )->pData = ( uint16_t * )array;

    if( self == &aEcan[eEcan1] )
    {
        for( DRV_DATA( self )->i = 0; DRV_DATA( self )->i < ( size / 2 ); DRV_DATA( self )->i++ )
        {
            aEcan1MsgBuf[NUM_BUFF_OUT][3 + DRV_DATA( self )->i] = DRV_DATA( self )->pData[DRV_DATA( self )->i];
        }
    }
    else
    {
        for( DRV_DATA( self )->i = 0; DRV_DATA( self )->i < ( size / 2 ); DRV_DATA( self )->i++ )
        {
            aEcan2MsgBuf[NUM_BUFF_OUT][3 + DRV_DATA( self )->i] = DRV_DATA( self )->pData[DRV_DATA( self )->i];
        }
    }
}

//*****************************************************************************
// Сброс операции передачи данных
void Ecan_AbortTransmite( const void *self )
{
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSelf,
               ( ( self == &aEcan[eEcan1] ) || ( self == &aEcan[eEcan2] ) ) );
    DRV_REGS( self )->CTRL1bits.ABAT = 1; // Сброс всех передач
    DRV_DATA( self )->transmissionState = eEcanTrStateIdle;
}

//*****************************************************************************
// Запустить операцию передачи данных
void Ecan_startTransmite( const void *self )
{
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSelf,
               ( ( self == &aEcan[eEcan1] ) || ( self == &aEcan[eEcan2] ) ) );
    DRV_REGS( self )->TR01CONbits.TXREQ0 = 1;
    DRV_DATA( self )->transmissionState = eEcanTrStateInProgress;
}

//*****************************************************************************
// Рабочий цикл
void Ecan_run( const void *self )
{
    ASSERT_ID( eGrPS_CanDrv, ePS_CanDrvSelf,
               ( ( self == &aEcan[eEcan1] ) || ( self == &aEcan[eEcan2] ) ) );
    DRV_DATA( self )->tempINTF.data = DRV_REGS( self )->INTF;

    // Сброс Bus-off state модуля CAN
    if( ( DRV_DATA( self )->tempINTF.bits.TXBP == 1 ) ||
        ( DRV_DATA( self )->tempINTF.bits.EWARN == 1 ) )
    {
        Ecan_reset( self );
    }

    // Прием данных
    if( DRV_DATA( self )->tempINTF.bits.IVRIF == 1 )
    {
        DRV_REGS( self )->INTFbits.RBIF = 0;
        DRV_REGS( self )->RXFUL1bits.NAME_BIT_RXFULL = 0;
        DRV_REGS( self )->INTFbits.IVRIF = 0;
        DRV_REGS( self )->INTFbits.ERRIF = 0;
    }
    if( DRV_DATA( self )->tempINTF.bits.RBIF == 1 )
    {
        DRV_DATA( self )->receiverIsReady = true;
        DRV_REGS( self )->INTFbits.RBIF = 0;
        DRV_REGS( self )->RXFUL1bits.NAME_BIT_RXFULL = 0;
    }

    // Передача данных
    switch( DRV_DATA( self )->transmissionState )
    {
        case eEcanTrStateIdle:
            break;
        case eEcanTrStateInProgress:
            if( DRV_REGS( self )->TR01CONbits.TXREQ0 == 0 )
            { // ждем окончания передачи
                if( DRV_REGS( self )->INTFbits.ERRIF == 0 )
                {
                    DRV_DATA( self )->transmissionState = eEcanTrStateIdle;
                }
                else
                {
                    DRV_REGS( self )->CTRL1bits.ABAT = 1; // Сброс всех передач
                    DRV_DATA( self )->transmissionState = eEcanTrStateError;
                }
            }
            break;
        case eEcanTrStateError:
            break;
    }
}

//*****************************************************************************
// Сброс операций приема и передачи данных
void Ecan_reset( const void *self )
{
    Ecan_setMode( self, eConfigMode );
    Ecan_setMode( self, DRV_DATA( self )->workMode );
}

//*****************************************************************************
// Реализация локальных функций
//*****************************************************************************

//*****************************************************************************
// Перевод модуля #CAN в заданный режим
void Ecan_setMode( const void *self, uint16_t mode )
{
    DRV_REGS( self )->CTRL1bits.REQOP = mode;
    while( DRV_REGS( self )->CTRL1bits.OPMODE != mode )
    {
        ;
    }
}

//*****************************************************************************
/**
* История изменений:
* 
* Версия 1.0.1
* Дата   06-10-2017
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Базовая версия.
*/
