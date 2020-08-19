/**
* \file    Rs422_transceiver.c
* \brief   \copybrief Rs422_transceiver.h
*
* \version 1.0.1
* \date    22-05-2017
* \author  Кругликов В.П.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include "Rs422_transceiver.h"
#include "Rs422_crc8.h"
#include "asserts_ex.h"
#include "dsPIC33_rs422.h"
#include "WorkWithBits.h"
#include "InterChannel.h"

//*****************************************************************************
// Реализация интерфейсных функций
//*****************************************************************************

//*****************************************************************************
// Инициализация переменных приема телеграмм по RS-422
void inRS422_ctor( InRS422 *inStr )
{
    uint8_t i;

    inStr->indWriteArray = 0;
    inStr->indReadArray = 0;
    inStr->indDate = 0;

    for( i = 0; i < V_ARRAY_IN_RS422; i++ ) inStr->aLength[i] = 0;
    inStr->state = eStateInWaitDle;
}

//*****************************************************************************
// Инициализация переменных передачи данных по RS-422
void outRS422_ctor( OutRS422 *outStr )
{
    uint8_t i;

    outStr->manageInOut( OBMEN_PRIEM );
    for( i = 0; i < N_ARRAY_OUT_RS422; i++ ) outStr->aOutRS422[i] = 0;
    outStr->indDate = 0;
    outStr->length = 0;
    outStr->state = eStateOutWaitData;
    outStr->timeOut = 0;
}

//*****************************************************************************
// Прием и HDLC-декодирование телеграммы по линии связи RS-422
void inRS422_run( InRS422 *inStr, uint16_t addr )
{
    uint8_t data, i, myAddr;

    for( i = 0; i < V_ARRAY_IN_RS422; i++ )
        inStr->acTimeIn[i]++;

    for( i = 0; i < 4; i++ )
    { // 4 - количество буферов в UART
        if( !inStr->inByte( &data ) )
        {
            break;
        }
        switch( inStr->state )
        {
            case eStateInWaitDle: //Ожидание начала телеграммы - байта DLE
                if( data == DLE )
                {
                    inStr->state = eStateInStx;
                    inStr->acTimeIn[inStr->indWriteArray] = 0; //байт DLE принят, сброс таймера
                }
                break;

            case eStateInStx: //Ожидание байта STX
                if( data == STX )
                {
                    inStr->indDate = 0; //байт STX принят, обнуление индекса массива
                    inStr->aLength[inStr->indWriteArray] = 0; //обнуление счетчика длины телеграммы
                    inStr->state = eStateInData;
                    break;
                }
                inStr->state = eStateInWaitDle;
                break;

            case eStateInData: //прием байтов данных, ожидание байта DLE
                if( data == DLE )
                {
                    inStr->state = eStateInPostDle;
                    break;
                }
                if( inStr->indDate >= MAX_ORDER_TELE - 1 )
                {
                    inStr->state = eStateInWaitDle; //длина телеграммы превышает допустимое значение
                    break;
                }
                inStr->aInRS422[inStr->indWriteArray][inStr->indDate++] = data; //запись байта
                break;

            case eStateInPostDle: //принят байт DLE 
                if( data == DLE )
                { //байт DLE повторный 
                    if( inStr->indDate >= MAX_ORDER_TELE - 1 )
                    {
                        inStr->state = eStateInWaitDle; //длина телеграммы превышает допустимое значение
                        break;
                    }
                    inStr->aInRS422[inStr->indWriteArray][inStr->indDate++] = data; //запись байта
                    inStr->state = eStateInData;
                    break;
                }
                if( data == ETX )
                { // признак конца массива
                    //Проверка адреса
                    myAddr = false;
                    if( inStr->indDate == 1 )
                    { //короткое сообщение

                        //адрес совпадает
                        if( ( inStr->aInRS422[inStr->indWriteArray][0] &0x0F ) == ( addr & 0x000F ) ) myAddr = true;
                    }
                    else
                    { //приказ
                        if( inStr->aInRS422[inStr->indWriteArray][0] == //1-ым идет старший байт
                            ( addr >> 8 & 0x00FF ) && inStr->aInRS422[inStr->indWriteArray][1] == ( addr & 0x00FF ) )
                        {
                            myAddr = true;
                        }
                    }
                    if( myAddr == true )
                    { //Адрес совпадает
                        inStr->aLength[inStr->indWriteArray] = inStr->indDate; //запись длины принятой телеграммы

                        //установка индекса следующего массива
                        if( ++inStr->indWriteArray >= V_ARRAY_IN_RS422 ) inStr->indWriteArray = 0;
                        inStr->aLength[inStr->indWriteArray] = 0; //обнуление длины массива
                    }
                }
                inStr->state = eStateInWaitDle;
                break;

            default:
                //Ошибка счетчика состояния
                ERROR_EX_ID( eGrPS_Rs422, ePS_Rs422StepCntError, 0xAAAA, eStepCntError_inRS422_run, 0xBBBB, 0 );
        }
    }
}

//*****************************************************************************
// Передача телеграммы по линии связи RS-422
void outRS422_run( OutRS422 *outStr )
{

    if( outStr->timeOut ) outStr->timeOut--; //декремент таймаута 

    switch( outStr->state )
    {
        case eStateOutWaitData:
            if( outStr->length && !outStr->timeOut )
            {
                outStr->manageInOut( OBMEN_PEREDACHA );
                outStr->indDate = 0;
                outStr->state = eStateOutWaitManageOut;
            }
            break;

        case eStateOutWaitManageOut: //задержка на время переключения
            outStr->state = eStateOutDataTransmit;
            break;

        case eStateOutDataTransmit:
            while( outStr->outByte( outStr->aOutRS422[outStr->indDate] ) )
            {
                outStr->indDate++;
                if( outStr->length > 1 ) outStr->length--;
                else
                { //Передача данных окончена
                    outStr->length = 0;
                    outStr->state = eStateOutWaitEndTransmit;
                    break;
                }
            }
            break;

        case eStateOutWaitEndTransmit:
            if( !outStr->transmCompl( ) ) break;
            outStr->state = eStateOutEnd;
            break;

        case eStateOutEnd:
            outStr->manageInOut( OBMEN_PRIEM );
            outStr->state = eStateOutWaitData;
            break;

        default: //Ошибка счетчика состояния
            ERROR_EX_ID( eGrPS_Rs422, ePS_Rs422StepCntError, 0xAAAA, eStepCntError_outRS422_run, 0xBBBB, 0 );
    }
}

//*****************************************************************************
// Формирование кода аларма для телеграмм статуса
uint8_t formAlarmRS422( uint8_t nCycle, uint8_t *alarmArray )
{
    static uint8_t nCycleOld, alarmOld, index;

    if( nCycleOld != nCycle )
    {
        nCycleOld = nCycle;

        if( alarmArray[0] == 0 )
        {
            alarmOld = index = 0;
        }
        else
        {
            if( alarmOld == alarmArray[index] )
                if( ++index == MAX_ALARM_COUNT )
                    index = 0;
            if( alarmArray[index] == 0 )
                index = 0;
            alarmOld = alarmArray[index];
        }
    }
    return alarmOld;
}

//*****************************************************************************
// Проверка корректности короткой телеграммы
bool checkShortTelegramm( uint8_t date )
{
    uint8_t bc, br1, br2;
    bool ret = false;

    bc = 0;
    br1 = date & 0x0F;
    br2 = ( date >> 4 ) &0x0F;

    if( CHECKBIT( date, 0 ) != 0 ) bc++;
    if( CHECKBIT( date, 1 ) != 0 ) bc++;
    if( CHECKBIT( date, 2 ) != 0 ) bc++;
    if( CHECKBIT( date, 3 ) != 0 ) bc++;

    if( CHECKBIT( bc, 0 ) != 0 )
    {
        br2 = ( ~br2 ) & 0x0F;
    }

    if( br1 == br2 && CHECKBIT( date, 0 ) != 0 )
    {
        ret = true;
    }
    return( ret );
}

//*****************************************************************************
// Проверка корректности нормальной телеграммы
bool checkTelegramm( uint8_t *aTele, uint8_t lengthTele )
{

    uint8_t date, crc;

    date = aTele[INDEX_ML_CO] >> 4; // длина телеграммы с учетом контрольной суммы

    // Проверка длины
    if( date != lengthTele || date > MAX_ORDER_TELE || date < MIN_ORDER_TELE )
        return false;

    //Проверка типа телеграммы
    date = aTele[INDEX_ML_CO] & CO_MASK;
    if( date != CO_PRIKAZ_A && date != CO_PRIKAZ_B )
        return false;

    //Подсчет CRC8 полей: ADDR, ML/CO, DATA
    crc = crc8_update( 0, aTele, 3 );
    if( lengthTele != MIN_ORDER_TELE )
    {
        crc = crc8_update( crc, ( aTele + INDEX_IN_DATE_0 ), lengthTele - MIN_ORDER_TELE );
    }

    // Проверка контрольной суммы
    if( crc != aTele[lengthTele - 1] ) return false;
    return true;
}

//*****************************************************************************
// Проверка взаимной корректности телеграмм приказа А и В
bool checkTelegramAB( uint8_t *aTele1, uint8_t *aTele2 )
{

    uint8_t i, date, lenght;
    bool ret = false;

    if( aTele1[INDEX_N_CICL] + aTele2[INDEX_N_CICL] == 0xFF &&
        ( aTele1[INDEX_ML_CO] & CO_MASK ) + ( aTele2[INDEX_ML_CO] & CO_MASK ) == CO_PRIKAZ_A + CO_PRIKAZ_B &&
        ( aTele1[INDEX_ML_CO] & LENGHT_MASK ) == ( aTele2[INDEX_ML_CO] & LENGHT_MASK ) )
    {
        lenght = ( aTele1[INDEX_ML_CO] >> 4 ) - ( INDEX_IN_DATE_0 + 1 );
        ret = true;
        for( i = 0; i < lenght; i++ )
        {
            date = ~aTele2[INDEX_IN_DATE_0 + i];
            if( aTele1[INDEX_IN_DATE_0 + i] != date )
            {
                ret = false;
            }
        }
    }
    return( ret );
}

//*****************************************************************************
// Получение адреса и длины принятой телеграммы
uint8_t InRS422_getTele( InRS422 *inStr,
                         uint8_t **aTele,
                         uint16_t *ptrTime )
{
    uint8_t ret = 0;

    //Проверка наличия телеграммы 
    if( inStr->aLength[inStr->indReadArray] == 0 ) return ret;
    //Телеграмма принята, считывание из буфера
    *aTele = &inStr->aInRS422[inStr->indReadArray][0];
    *ptrTime = inStr->acTimeIn[inStr->indReadArray];
    ret = inStr->aLength[inStr->indReadArray];
    inStr->aLength[inStr->indReadArray] = 0; //Обнуление признака наличия телеграммы
    if( ++inStr->indReadArray >= V_ARRAY_IN_RS422 ) inStr->indReadArray = 0; //Инкремент индекса чтения
    return ret;
}

//*****************************************************************************
// Формирование телеграммы статуса
uint8_t formTeleStatus( uint8_t *aTele, uint8_t type, uint16_t adrOK,
                        uint8_t nCycle, uint8_t alarm, uint8_t *statusArray,
                        uint8_t statusLength )
{

    uint8_t i, j, size, aDate[MAX_STATUS_TELE], crc;

    if( statusLength == 0 ) return 0;

    aDate[INDEX_ADDRESS_OK_0] = adrOK >> 8 & 0x00FF; // Первым идет старший байт
    aDate[INDEX_ADDRESS_OK_1] = adrOK & 0x00FF;
    size = MAX_STATUS_TELE - MAX_STATUS_DATA + statusLength;
    aDate[INDEX_ML_CO] = type | ( size << 4 );

    if( type == CO_STATUS_A )
    {
        aDate[INDEX_N_CICL] = nCycle;
    }
    else
    {
        aDate[INDEX_N_CICL] = ~nCycle;
    }

    aDate[INDEX_OUT_ALARM] = alarm; // ttt
    for( i = 0; i < statusLength; i++ )
    {
        if( type == CO_STATUS_A )
        {
            aDate[INDEX_OUT_DATE_0 + i] = statusArray[i];
        }
        else
        {
            aDate[INDEX_OUT_DATE_0 + i] = ~statusArray[i];
        }
    }
    //Подсчет CRC8 полей: ADDR, ML/CO, DATA
    crc = crc8_update( 0, aDate, 3 );
    crc = crc8_update( crc, ( aDate + INDEX_OUT_DATE_0 ), statusLength );
    aDate[size - 1] = crc;

    j = 0;
    aTele[j++] = DLE;
    aTele[j++] = STX;
    for( i = 0; i < size; i++ )
    {
        aTele[j++] = aDate[i];
        if( aDate[i] == DLE )
        {
            aTele[j++] = DLE;
        }
    }
    aTele[j++] = DLE;
    aTele[j++] = ETX;
    return( j );
}

//*****************************************************************************
/**
* История изменений: 
* 
* Версия 1.0.1
* Дата   22-05-2017
* Автор  Кругликов В.П.
* 
* Изменения:
*    Базовая версия.
*/
