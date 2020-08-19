/**
* \file    AdcDriver_PIC33.c
* \brief   \copybrief AdcDriver_PIC33.h
*
* \version 1.0.1
* \date    28-03-2016
* \author  Третьяков В.Ж.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "typeMK.h"
#include "asserts.h"
#include "ProtectionState_codes.h"
#include "Adc_Inic1.h"
#include "Adc_regsPIC33.h"
#include "AdcDriver_PIC33.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************

//*****************************************************************************
#define NUMBER_CANALS 32        ///< Количество каналов измерений.

//*****************************************************************************
// Определение типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Данные драйвера.
///
typedef struct
{
    bool     isInic;                         ///< Признак инициализации модуля.
    volatile Adc_regsPIC33 *pAdcRegs;        ///< Указатель на регистры модуля АЦП.
    uint16_t aBUFx[NUMBER_CANALS];           ///< Массив измеренных значений.
    uint16_t canal;                          ///< Счетчик измеряемых каналов.
    uint32_t maskShift;                      ///< Маска сдвига.
    uint32_t canals;                         ///< Измеряемые каналы.
    bool done;                               ///< Признак готовности измерений.
} AdcModule;

//*****************************************************************************
// Определение локальных переменных
//*****************************************************************************

//*****************************************************************************
static AdcModule aAdcModule[eAdcCount];        ///< Объекты данных драйверов АЦП.

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Получить канал АЦП.
/// \return Значение для записи в регистр ADxCSSL.
///
uint32_t AdcDriver_getCanal( AdcNumber number );

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация модуля АЦП
void AdcDriver_ctor( AdcNumber number )
{
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcDrvSelf, ( number < eAdcCount ) );
    AdcModule *me = &aAdcModule[number];
    INIC_ADC1_PIN;
    me->pAdcRegs = &adc1_regsPIC33;
    me->pAdcRegs->CON1 = INIC_ADC1_ADCON1;
    me->pAdcRegs->CON2 = INIC_ADC1_ADCON2;
    me->pAdcRegs->CON3 = INIC_ADC1_ADCON3;
    me->pAdcRegs->CON4 = INIC_ADC1_ADCON4;
    me->canals = INIC_ADC1_ADCSSL | INIC_ADC1_ADCSSH << 16;
    me->pAdcRegs->CSSH = 0;
    me->pAdcRegs->CSSL = 0;
    me->done = false;
    me->isInic = true;
    me->pAdcRegs->CON1bits.ADON = 1;
}

//*****************************************************************************
// Запустить измерения набора параметров
void AdcDriver_start( AdcNumber number )
{
    uint32_t temp;
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcDrvSelf, ( number < eAdcCount ) );
    AdcModule *me = &aAdcModule[number];
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcNoInic, ( me->isInic ) );
    me->pAdcRegs->CON1bits.DONE = 0;
    me->maskShift = 1;
    me->canal = 0;
    me->pAdcRegs->CON1bits.ADON = 0; // Запись в ADxCSSL следует проводить при выключенном АЦП
    temp = AdcDriver_getCanal( number );
    me->pAdcRegs->CSSL = ( uint16_t )( 0x0000FFFF & temp );
    me->pAdcRegs->CSSH = ( uint16_t )( temp >> 16 );
    me->pAdcRegs->CON1bits.ADON = 1;
    me->pAdcRegs->CON1bits.SAMP = 1;
}

//*****************************************************************************
// Чтение измеренных значений
uint8_t AdcDriver_get( AdcNumber number, uint16_t *array, uint8_t size )
{
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcDrvSelf, ( number < eAdcCount ) );
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcDrvSetSize, ( size < NUMBER_CANALS ) );
    AdcModule *me = &aAdcModule[number];
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcNoInic, ( me->isInic ) );
    uint8_t i;
    for( i = 0; i < size; i++ )
    {
        array[i] = me->aBUFx[i];
    }
    me->done = false;
    return size;
}

//*****************************************************************************
// Обработка завершения конвертации АЦП, модуль 1
void AdcDriver_interruptAdc1( void )
{
    AdcModule *me = &aAdcModule[eAdc1];
    uint32_t temp;
    if( me->pAdcRegs->CSSL != 0 || me->pAdcRegs->CSSH != 0 )
    {
        ASSERT_ID( eGrPS_AdcDrv, ePS_CanDrvErrorAlg, ( me->canal < NUMBER_CANALS ) );
        me->aBUFx[me->canal++] = me->pAdcRegs->BUF0;
        me->pAdcRegs->CON1bits.ADON = 0;
        temp = AdcDriver_getCanal( eAdc1 );
        me->pAdcRegs->CSSL = ( uint16_t )( 0x0000FFFF & temp );
        me->pAdcRegs->CSSH = ( uint16_t )( temp >> 16 );
        me->pAdcRegs->CON1bits.ADON = 1;
        if( me->pAdcRegs->CSSL != 0 || me->pAdcRegs->CSSH != 0 )
        {
            me->pAdcRegs->CON1bits.SAMP = 1;
        }
        else
        {
            me->done = true;
        }
    }
    return;
}

//*****************************************************************************
// Запрос наличия измеренных значений
bool AdcDriver_isReady( AdcNumber number )
{
    ASSERT_ID( eGrPS_AdcDrv, ePS_AdcDrvSelf, ( number < eAdcCount ) );
    AdcModule *me = &aAdcModule[number];
    return me->done;
}

//*****************************************************************************
// Реализация локальных функций
//*****************************************************************************

//*****************************************************************************
// Получить канал АЦП
uint32_t AdcDriver_getCanal( AdcNumber number )
{
    uint32_t ret = 0;
    AdcModule *me = &aAdcModule[number];
    ASSERT_ID( eGrPS_AdcDrv, ePS_CanDrvErrorAlg, ( me->maskShift != 0 ) );
    while( true )
    {
        if( ( me->canals & me->maskShift ) != 0 )
        {
            ret = me->maskShift;
            me->maskShift = me->maskShift << 1;
            break;
        }
        else
        {
            if( me->maskShift == 0x80000000 )
            {
                break;
            }
            else
            {
                me->maskShift = me->maskShift << 1;
            }
        }
    }
    return ret;
}

//*****************************************************************************
/**
* Пояснения:  
*    Драйвер рассчитан на сканирование входов по каналу 1.
*    Каналы для сканирования задаются в константах INIC_ADC1_ADCSSLH INIC_ADC1_ADCSSL.
*    Сканирование реализовано  программно.
*    При сканировании в автоматическом режиме АЦП были изредка 
*    некорректные измерения. Требовалось вводить ожидание сканирование каналов,
*    что в данном проекте требовало значительных расходов ресурсов. 
*/

/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   28-03-2016
* Автор  Третьяков В.Ж.
* 
* Изменения:
*    Базовая версия.
*/
