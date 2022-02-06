/**
* \file    AnalogMeasurement.c
* \brief   \copybrief AnalogMeasurement.h 
*
* \version 2.0.4
* \date    15-03-2021
* \author  Агулов М.А.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <stdlib.h>
#include "defCompil.h"
#include "asserts_ex.h"
#include "InterChannelId.h"
#include "DigitalFilter.h"
#include "InterChannel.h"
#include "AnalogMeasurement.h"
#include "IntegrCtrl.h"
#include "CheckCallFunctions.h"

//*****************************************************************************
// Локальные константы, определенные через макросы
//*****************************************************************************
// U3V3_NOMINAL = (3.3/2)*4096/3

//*****************************************************************************
/// \brief Максимальное отклонение напряжения питания контроллера (в одну сторону), %.
///
#define   U3V3_MAX_ERR                  10U

//*****************************************************************************
/// \brief Номинальное значение напряжения питания контроллера (в единицах АЦП).
///
#define   U3V3_NOMINAL                  2252UL

//*****************************************************************************
/// \brief Максимальное отклонение опорного напряжения (в одну сторону), %.
///
#define   UREF_MAX_ERR                  10U

//*****************************************************************************
#define   UREF_NOMINAL                  2047UL         ///< Номинальное значение опорного напряжения (в единицах АЦП).

//*****************************************************************************
/// \brief Максимальное отклонение буферного напряжения (в одну сторону), %.
///
#define   UBUF_MAX_ERR                  10U

//*****************************************************************************
#define   UBUF_NOMINAL                  4095UL         ///< Номинальное значение буферного напряжения (в единицах АЦП).

//*****************************************************************************
// Объявление локальных переменных
//*****************************************************************************

//*****************************************************************************
static int synchroTimeControl;                    ///< Счетчик времени для синхронизации контролируемых параметров.

//*****************************************************************************
static AnalogMeasurementValue analogMeasData[eAinChCount];             ///< Объект обработки аналоговых сигналов.


//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация переменных компонента (конструктор)
void AnalogMeasurement_ctor( void )
{
    uint16_t i;
    //    Инициализация модуля чтения аналоговых входов
    AnalogInput_ctor( );
    //    Инициализация возвращаемых значений
    for( i = 0; i < eAinChCount; i++ )
    {
        const AnalogInputValue *ain = AnalogInput_getData( i );
        analogMeasData[i].rms = ain->rms;
        analogMeasData[i].value = ain->value - ain->bias;
    }
    // Настройка измерительный каналов
    AnalogInput_setChannelSettings( eAinchKREF, // Канал половины опорного напряжения соседнего контроллера
            eDigitalFilterOff,
            eAinScriptDC,
            0,
            128 );
    AnalogInput_setChannelSettings( eAinchK3V3, // Канал половины напряжения питания контроллера
            eDigitalFilterOff,
            eAinScriptDC,
            0,
            128 );
    AnalogInput_setChannelSettings( eAinchREF, // Канал напряжения для контроля правильности функционирования АЦП 
            eDigitalFilterOff,
            eAinScriptDC,
            0,
            128 );
    AnalogInput_setChannelSettings( eAinchGEN, // Канал контроля сигнала тестового генератора 62,5 Гц
            0,
            eAinScriptAC,
            2048,
            128 );
    AnalogInput_setChannelSettings( eAinchR1,
            1,
            eAinScriptAC, // Канал сигнала с датчика R1
            2048,
            128 );
    AnalogInput_setChannelSettings( eAinchR2, // Канал сигнала с датчика R2
            2,
            eAinScriptAC,
            2048,
            128 );
    AnalogInput_setChannelSettings( eAinchIV, // Канал тока фазы V
            eDigitalFilterOff,
            eAinScriptAC,
            2048,
            128 );
    AnalogInput_setChannelSettings( eAinchIU, // Канал тока фазы U
            eDigitalFilterOff,
            eAinScriptAC,
            2048,
            128 );
    AnalogInput_setChannelSettings( eAinchIW, // Канал тока фазы W
            eDigitalFilterOff,
            eAinScriptAC,
            2048,
            128 );
    AnalogInput_setChannelSettings( eAinchUV, // Канал напряжения фазы V   
            eDigitalFilterOff,
            eAinScriptAC,
            0,
            128 );
    AnalogInput_setChannelSettings( eAinchUU, // Канал напряжения фазы U 
            eDigitalFilterOff,
            eAinScriptAC,
            0,
            128 );
    AnalogInput_setChannelSettings( eAinchUW, // Канал напряжения фазы W
            eDigitalFilterOff,
            eAinScriptAC,
            0,
            128 );

    // Переменные сигналы инициализируем значением соответствующем нулю

    // Настройка фильтров на частоту 62,5Гц
    DigitalFilter_setFreq( eDigitalFilterGEN, eDfFreq62_5Hz );
    DigitalFilter_setFreq( eDigitalFilterR1, eDfFreq62_5Hz );
    DigitalFilter_setFreq( eDigitalFilterR2, eDfFreq62_5Hz );

    //*****************************************************************************
    // Настройка параметров МКО для 
    // синхронизации измеренных параметров 
    
    static const uint16_t rangeControlUr1 = 15, rangeControlUr2 = 15;
    static const uint16_t MidRangeUr1 = 450, MidRangeUr2 = 450, MidRangeUGEN = 450;
    static const uint16_t rangeControlIUval = 100, rangeControlIVval = 100, rangeControlIWval = 100;
    static const uint16_t rangeControlIUrms = 100, rangeControlIVrms = 100, rangeControlIWrms = 100;
    static const uint16_t timeT1Ur1 = 50, timeT2Ur1 = 300, timeT3Ur1 = 0;
    static const uint16_t timeT1Ur2 = 50, timeT2Ur2 = 300, timeT3Ur2 = 0;
    static const uint16_t timeT1IUval = 50, timeT2IUval = 300, timeT3IUval = 0;
    static const uint16_t timeT1IVval = 50, timeT2IVval = 300, timeT3IVval = 0;
    static const uint16_t timeT1IWval = 50, timeT2IWval = 300, timeT3IWval = 0;
    static const uint16_t timeT1IUrms = 50, timeT2IUrms = 300, timeT3IUrms = 0;
    static const uint16_t timeT1IVrms = 50, timeT2IVrms = 300, timeT3IVrms = 0;
    static const uint16_t timeT1IWrms = 50, timeT2IWrms = 300, timeT3IWrms = 0;
    static const uint16_t timeT1UGENrms = 50, timeT2UGENrms = 300, timeT3UGENrms = 0;
    
    InterChannel_setParamSettings( eICId_U_R1rms, eScriptSync,
                                   eProcSyncAverage, eProcCheckDeltaP,
                                   rangeControlUr1, MidRangeUr1, timeT1Ur1, timeT2Ur1, timeT3Ur1 );
    InterChannel_setParamSettings( eICId_U_R2rms, eScriptSync,
                                   eProcSyncAverage, eProcCheckDeltaP,
                                   rangeControlUr2, MidRangeUr2, timeT1Ur2, timeT2Ur2, timeT3Ur2 );
    InterChannel_setParamSettings( eICId_U_GENrms, eScriptSync,
                                   eProcSyncAverage, eProcCheckOff,
                                    0, MidRangeUGEN, timeT1UGENrms, timeT2UGENrms, timeT3UGENrms );
    
    InterChannel_setParamSettings( eICId_IV_value, eScriptSync,
            eProcSyncAverage, eProcCheckDeltaP,
            rangeControlIVval, 0, timeT1IVval, timeT2IVval, timeT3IVval );
    InterChannel_setParamSettings( eICId_IU_value, eScriptSync,
            eProcSyncAverage, eProcCheckDeltaP,
            rangeControlIUval, 0, timeT1IUval, timeT2IUval, timeT3IUval );
    InterChannel_setParamSettings( eICId_IW_value, eScriptSync,
            eProcSyncAverage, eProcCheckDeltaP,
            rangeControlIWval, 0, timeT1IWval, timeT2IWval, timeT3IWval );
        
    InterChannel_setParamSettings( eICId_IV_rms, eScriptSync,
            eProcSyncAverage, eProcCheckDeltaX,
            rangeControlIVrms, 0, timeT1IVrms, timeT2IVrms, timeT3IVrms );
    InterChannel_setParamSettings( eICId_IU_rms, eScriptSync,
            eProcSyncAverage, eProcCheckDeltaX,
            rangeControlIUrms, 0, timeT1IUrms, timeT2IUrms, timeT3IUrms  );
    InterChannel_setParamSettings( eICId_IW_rms, eScriptSync,
            eProcSyncAverage, eProcCheckDeltaX,
            rangeControlIWrms, 0, timeT1IWrms, timeT2IWrms, timeT3IWrms );
           
    //*****************************************************************************
    // Инициализация внутренних переменных
    synchroTimeControl = ANALOG_SYNCHRO_PERIOD;
}

//*****************************************************************************
// Обработка аналоговых сигналов
void AnalogMeasurement_run( void )
{
    #define TIMEOUT_START_CTRL_VOLT  500
    static uint16_t t_o = TIMEOUT_START_CTRL_VOLT;
    
    //*****************************************************************************
    // Обработка данных прочитанных с АЦП
    AnalogInput_run( );

    analogMeasData[eAinchGEN].rms = AnalogInput_getData( eAinchGEN )->rms;
    analogMeasData[eAinchR1].rms = AnalogInput_getData( eAinchR1 )->rms;
    analogMeasData[eAinchR2].rms = AnalogInput_getData( eAinchR2 )->rms;

    analogMeasData[eAinchIV].rms = AnalogInput_getData( eAinchIV )->rms;
    analogMeasData[eAinchIV].value = abs( ( int16_t ) AnalogInput_getData( eAinchIV )->value
            - ( int16_t ) AnalogInput_getData( eAinchIV )->bias );
    analogMeasData[eAinchIU].rms = AnalogInput_getData( eAinchIU )->rms;
    analogMeasData[eAinchIU].value = abs( ( int16_t ) AnalogInput_getData( eAinchIU )->value
            - ( int16_t ) AnalogInput_getData( eAinchIU )->bias );
    analogMeasData[eAinchIW].rms = AnalogInput_getData( eAinchIW )->rms;
    analogMeasData[eAinchIW].value = abs( ( int16_t ) AnalogInput_getData( eAinchIW )->value
            - ( int16_t ) AnalogInput_getData( eAinchIW )->bias );
    analogMeasData[eAinchUV].rms = AnalogInput_getData( eAinchUV )->rms;
    analogMeasData[eAinchUV].value = AnalogInput_getData( eAinchUV )->value;
    analogMeasData[eAinchUU].rms = AnalogInput_getData( eAinchUU )->rms;
    analogMeasData[eAinchUU].value = AnalogInput_getData( eAinchUU )->value;
    analogMeasData[eAinchUW].rms = AnalogInput_getData( eAinchUW )->rms;
    analogMeasData[eAinchUW].value = AnalogInput_getData( eAinchUW )->value;

    analogMeasData[eAinchKREF].value = AnalogInput_getData( eAinchKREF )->aver;
    analogMeasData[eAinchK3V3].value = AnalogInput_getData( eAinchK3V3 )->aver;
    analogMeasData[eAinchREF].value = AnalogInput_getData( eAinchREF )->aver;

    uint16_t temp;
    // Синхронизация аналоговых параметров
    switch( --synchroTimeControl )
    {
        case 0:
            InterChannel_synchronize( eICId_U_R1rms, analogMeasData[eAinchR1].rms );
            synchroTimeControl = ANALOG_SYNCHRO_PERIOD;
            break;

        case 4:
            InterChannel_synchronize( eICId_U_R2rms, analogMeasData[eAinchR2].rms );
            break;

        case 8:
            InterChannel_synchronize( eICId_U_GENrms, analogMeasData[eAinchGEN].rms );
            break;

        case 12:
            if( IntegrCtrl_isOn( ) )
                temp = analogMeasData[eAinchIW].value;
            else
                temp = analogMeasData[eAinchIW].rms;
            InterChannel_synchronize( eICId_IW_rms, temp );
            break;

        case 16:
            if( IntegrCtrl_isOn( ) )
                temp = analogMeasData[eAinchIU].value;
            else
                temp = analogMeasData[eAinchIU].rms;
            InterChannel_synchronize( eICId_IU_rms, temp );
            break;

        case 20:
            if( IntegrCtrl_isOn( ) )
                temp = analogMeasData[eAinchIV].value;
            else
                temp = analogMeasData[eAinchIV].rms;
            InterChannel_synchronize( eICId_IV_rms, temp );
            break;
            
        case 24:
            InterChannel_synchronize( eICId_IV_value, analogMeasData[eAinchIV].value );
            break;

        case 28:
            InterChannel_synchronize( eICId_IU_value, analogMeasData[eAinchIU].value );
            break;

        case 32:
            InterChannel_synchronize( eICId_IW_value, analogMeasData[eAinchIW].value );
            break;
        
        default:
            break;
    }
    
    
    if( t_o > 0 ) 
    {
        t_o--;
    } 
    else 
    {
        ASSERT_EX_ID( eGrPS_ControlSupplyVoltage, ePS_FaultU3_3V,
                abs( analogMeasData[ eAinchK3V3 ].value - U3V3_NOMINAL ) < ( U3V3_NOMINAL * U3V3_MAX_ERR / 100U ),
                analogMeasData[ eAinchK3V3 ].value, // параметр 1 - текущее значение контролируемого сигнала,
                U3V3_NOMINAL, // параметр 2 - эталонное значение контролируемого сигнала,                
                U3V3_MAX_ERR, // параметр 3 - допустимое отклонение,  %              
                0 ); // параметр 4 - отсутствует.

        ASSERT_EX_ID( eGrPS_ControlSupplyVoltage, ePS_FaultURef,
                abs( analogMeasData[ eAinchKREF ].value - UREF_NOMINAL ) < ( UREF_NOMINAL * UREF_MAX_ERR / 100U ),
                analogMeasData[ eAinchKREF ].value, // параметр 1 - текущее значение контролируемого сигнала,
                UREF_NOMINAL, // параметр 2 - эталонное значение контролируемого сигнала,                
                UREF_MAX_ERR, // параметр 3 - допустимое отклонение,  %              
                0 ); // параметр 4 - отсутствует.

        ASSERT_EX_ID( eGrPS_ControlSupplyVoltage, ePS_FaultBufferADC,
                abs( analogMeasData[ eAinchREF ].value - UBUF_NOMINAL ) < ( UBUF_NOMINAL * UBUF_MAX_ERR / 100U ),
                analogMeasData[ eAinchREF ].value, // параметр 1 - текущее значение контролируемого сигнала,
                UBUF_NOMINAL, // параметр 2 - эталонное значение контролируемого сигнала,                
                UBUF_MAX_ERR, // параметр 3 - допустимое отклонение,  %              
                0 );
        t_o = TIMEOUT_START_CTRL_VOLT;
    }
    MARKED_CALL_FUNCTION;
}


//*****************************************************************************
// Чтение измеренных значений, определяется используемой системой физических величин
const AnalogMeasurementValue *AnalogMeasurement_getData( uint16_t channel )
{
    ASSERT_EX_ID( eGrPS_AnalogInput, ePS_AinErrorChannel,
            ( channel < eAinChCount ),
            2, channel, 0, 0 );
    return &analogMeasData[channel];
}




//*****************************************************************************
/**
* История изменений:
* 
* Версия 1.0.1
* Дата   22-01-2016
* Автор  Годунок А.Н.
* 
* Изменения:
*    Базовая версия.
* 
* Версия 2.0.1
* Дата   16-05-2017
* Автор  Васильченко А.В.
* 
* Изменения:
*    1. Введены процедуры межканального контроля значения параметров.
*    2. Убраны синхронизации значений Uref и теста буферного регистра.
*    3. Изменен период синхронизации параметров.
*
* Версия 2.0.2
* Дата   16-09-2020
* Автор  Кругликов В.П. 
*
* Изменения:
*   Добавлена интерфейсная функция  AnalogMeasurement_getMiddleValue
*   для фильтрации мгновенных значений при использовании сценария eAinScriptAC
*
* Версия 2.0.3
* Дата   24-02-2021
* Автор  Кругликов В.П. 
*
* Изменения:
*   Устранена ошибка неустановки тайм-аута измерений напряжений питания, опорного и буферных регистров АЦП
*
* Версия 2.0.4
* Дата   15-03-2021
* Автор  Агулов М.А.
* 
* Изменения:
* Функция AnalogMeasurement_runInterrupt() убрано, т.к. АЦП работает с DMA
*/
