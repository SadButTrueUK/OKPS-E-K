/**
* \file    AnalogInput.c
* \brief   \copybrief AnalogInput.h
*
* \version 1.0.1
* \date    20-06-2018
* \author  Агулов М.А.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <string.h>
#include "AnalogInput.h"
#include "DigitalFilter.h"
#include "Adc.h"
#include "ProtectionState_codes.h"
#include "asserts_ex.h"

//*****************************************************************************
// Определение типов данных
//*****************************************************************************

//*****************************************************************************
/// \brief Настройки измерителя аналоговых сигналов.
///
typedef struct tagAnalogInputSettings
{
    uint16_t script;          ///< Алгоритм дополнительной обработки сигнала.
    uint16_t filterID;        ///< Идентификатор используемого фильтра.
} AnalogInputSettings;

//*****************************************************************************
/// \brief Данные измерителя аналоговых сигналов.
///
typedef struct tagAnalogInputMeasuring
{
    int16_t  idx;                             ///< Индекс последнего элемента в буфере данных окна усреднения buf[].
    uint16_t buf[AVERAGE_WINDOW_SIZE];        ///< Буфер данных окна усреднения.
    uint32_t sum;                             ///< Текущая сумма всех значений в буфере данных окна усреднения buf[].
} AnalogInputMeasuring;

//*****************************************************************************
/// \brief Данные объекта обработки аналоговых сигналов.
///
typedef struct tagAnalogInputData
{
    AnalogInputValue     values;           ///< Значения аналоговых сигналов.
    AnalogInputSettings  settings;         ///< Настройки измерителя для каждого аналогового сигнала.
    AnalogInputMeasuring measuring;        ///< Данные измерителя сигнала.
} AnalogInputData;

//*****************************************************************************
// Определение локальных переменных
//*****************************************************************************

//*****************************************************************************
/// \brief Массив очереди для записи результатов преобразований АЦП.
///
int16_t samples[AnalogInputDrvQueueSize];

//*****************************************************************************
static AnalogInputData analogInputData[eAinChCount];        ///< Объект обработки аналоговых сигналов.

//*****************************************************************************
// Прототипы локальных функций
//*****************************************************************************

//*****************************************************************************
/// \brief Вычисление квадратного корня, оптимизированного по времени.
/// \param x - аргумент функции в формате unsigned int32.
/// \return Квадратный корень аргумента \a х.
///
uint16_t sqrt_topt( uint32_t x );

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация компонента
void AnalogInput_ctor( void )
{
    for( uint16_t i = 0; i < eAinChCount; i++ )
        memset( &analogInputData[i], 0, sizeof(analogInputData[i] ) );

    Adc_ctor( );
}
//*****************************************************************************
// Настройка измерительного канала
void AnalogInput_setChannelSettings( uint16_t channel,
                                     uint16_t filterId,
                                     AnalogInputScript script,
                                     uint16_t bias,
                                     uint16_t aws )
{
    ASSERT_EX_ID( eGrPS_AnalogInput, ePS_AinErrorChannel,
                  ( channel < eAinChCount ),
                  1, channel, 0, 0 );
    ASSERT_EX_ID( eGrPS_AnalogInput, ePS_AinErrorScript,
                  ( ( script > eAinScriptError ) &&
                    ( script <= eAinScriptDC ) ),
                  1, channel, script, 0 );
    ASSERT_EX_ID( eGrPS_AnalogInput, ePS_AinErrorDfId,
                  ( filterId < eDigitalFilterCount ) || ( filterId == eDigitalFilterOff ),
                  filterId, 0, 2, 0 );
    ASSERT_EX_ID( eGrPS_AnalogInput, ePS_AinErrorIncorrectValueAWS,
                  aws == 2  || aws == 4  || aws == 8   || aws == 16 ||
                  aws == 32 || aws == 64 || aws == 128 || aws == 256,
                  aws, 0, 0, 0 );

    memset( &analogInputData[channel], 0, sizeof(analogInputData[channel] ) );
    analogInputData[channel].settings.script = script;
    analogInputData[channel].settings.filterID = filterId;
    analogInputData[channel].values.bias = bias;
    analogInputData[channel].values.aws = aws;

    // Вычисление числа сдвигов
    for( analogInputData[channel].values.shift = 0; ( aws = aws >> 1 ); analogInputData[channel].values.shift++ ) 
        ;
}

//*****************************************************************************
// Чтение измеренных значений
const AnalogInputValue *AnalogInput_getData( uint16_t channel )
{
    ASSERT_EX_ID( eGrPS_AnalogInput, ePS_AinErrorChannel,
                  ( channel < eAinChCount ),
                  2, channel, 0, 0 );

    return &analogInputData[channel].values;
}

//*****************************************************************************
// Обработка аналоговых сигналов
void AnalogInput_run( void )
{
    uint16_t adcData[AnalogInputDrvQueueSize][eAinChCount];
    uint16_t samplesCnt = 0;

    // Чтение данных из буфера АЦП. После выполнения цикла в переменной samplesCnt
    // содержит количествы прочитанных отсчётов
    while( Adc_isReady() )
    {
        // AnalogInput_run за один проход по main должен вычитать все данные с АЦП.
        ASSERT_EX_ID( eGrPS_AnalogInput, ePS_AinErrorDriverGet,
                      ( samplesCnt < AnalogInputDrvQueueSize ), 3, samplesCnt, AnalogInputDrvQueueSize, 0 );
        Adc_get( &adcData[samplesCnt++][0]);
    }

    // Обработка данных
    for( uint16_t i = 0; i < eAinChCount; i++ )
    {
        // Мгновенное значение сигнала на входе АЦП (данные с буфера АЦП)
        analogInputData[i].values.value = adcData[samplesCnt - 1][i];           

        switch( analogInputData[i].settings.script )                            // Дополнительная обработка сигнала
        {
            case eAinScriptNone:                                                // Нет обработки
                ;
                break;

            // Обработка постоянного напряжения (DC) на входе АЦП
            case eAinScriptDC:                                                  
                // Подсчет среднего значения сигнала (aver)
                for( uint16_t sample = 0; sample < samplesCnt; sample++ )
                {
                    analogInputData[i].measuring.sum = analogInputData[i].measuring.sum
                        - analogInputData[i].measuring.buf[analogInputData[i].measuring.idx]            // Вычитание старого значения 
                        + adcData[sample][i];                                                           // Суммирование с новым значенем

                    // Запись нового значения в буфер окна усреднения
                    analogInputData[i].measuring.buf[analogInputData[i].measuring.idx++] = adcData[sample][i];
                    
                    // Инкремент и контроль индекса буфера окна усреднения
                    analogInputData[i].measuring.idx != analogInputData[i].values.aws ? : ( analogInputData[i].measuring.idx = 0 );
                }

                // Значение aver обновляется каждую миллисекунду
                analogInputData[i].values.aver = analogInputData[i].measuring.sum >> analogInputData[i].values.shift;
                break;

            case eAinScriptAC:   // Обработка переменного напряжения (AC) на входе АЦП
                // Вычитание постоянного смещения (bias) из сигнала                
                for( uint16_t sample = 0; sample < samplesCnt; sample++ )
                    samples[sample] = (int16_t)adcData[sample][i] - (int16_t)analogInputData[i].values.bias;

                // Обработка сигнала при помощи цифрового фильтра (опционально)  
                analogInputData[i].settings.filterID == eDigitalFilterOff ? : DigitalFilter_run( analogInputData[i].settings.filterID, samples, samplesCnt );

                // Подсчет среднеквадратического значения (rms)
                for( uint16_t sample = 0; sample < samplesCnt; sample++ )
                {
                    // Для процессоров Microchip с DSP-ядром (dsPIC30F и dsPIC33EP) используются ассемблерная DSP-команда умножения - MULSS
                    #if defined(__HAS_DSP__)
                        // Вычитание старого значения
                        analogInputData[i].measuring.sum = analogInputData[i].measuring.sum
                            - __builtin_mulss( analogInputData[i].measuring.buf[analogInputData[i].measuring.idx], 
                            analogInputData[i].measuring.buf[analogInputData[i].measuring.idx] )
                            + __builtin_mulss( samples[sample], samples[sample] ); // Сложение с новым значенем
                    #else
                        // Вычитание старого значения
                        analogInputData[i].measuring.sum = analogInputData[i].measuring.sum
                            - analogInputData[i].measuring.buf[analogInputData[i].measuring.idx] * 
                            analogInputData[i].measuring.buf[analogInputData[i].measuring.idx]
                            + samples[sample] * samples[sample]; // Добавление нового значения
                    #endif
                    
                    // Запись нового значения в буфер окна усреднения
                    analogInputData[i].measuring.buf[analogInputData[i].measuring.idx++] = samples[sample];

                    // Контроль индекса буфера окна усреднения
                    analogInputData[i].measuring.idx != analogInputData[i].values.aws ? : ( analogInputData[i].measuring.idx = 0 );             
                }
                
                // Вычисляем среднеквадратическое значение сигнала (RMS)
                analogInputData[i].values.rms = sqrt_topt( analogInputData[i].measuring.sum >> analogInputData[i].values.shift );
                break;

            default:
                ERROR_EX_ID( eGrPS_AnalogInput, ePS_AinErrorScript, 3, i, analogInputData[i].settings.script, 0 );
                break;
        }
    }
}

//*****************************************************************************
// Управление дискретизацией сигналов
void AnalogInput_runInterrupt( void )
{
    Adc_run( );
}

//*****************************************************************************
/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   20-06-2018
* Автор  Агулов М.А.
* 
* Изменения:
*     Базовая версия.
*/
