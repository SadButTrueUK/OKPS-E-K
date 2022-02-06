/**
* \file    Rs422_transceiver.c
* \brief   \copybrief Rs422_transceiver.h
*
* \version 1.0.4
* \date    15-03-2021
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <xc.h>
#include "Rs422_transceiver.h"
#include "Rs422_crc8.h"
#include "asserts_ex.h"
#include "WorkWithBits.h"
#include "InterChannel.h"
#include "Rs422DriverMacros.h"
#include "UartPinRemap.h"
#include "MessageVersion.h"
#include "defCompil.h"

//*****************************************************************************
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ���������� ������ ��������� �� RS-422
void inRS422_ctor( InRS422 *inStr, Rs422_numLine numLine )
{
    uint8_t i;

    inStr->indWriteArray = 0;
    inStr->indReadArray = 0;
    inStr->indDate = 0;
    inStr->ndxReadReg = 0;  
    inStr->ndxWriteReg = 0;
        
    for( i = 0; i < V_ARRAY_IN_RS422; i++ ) 
        inStr->aLength[i] = 0;
        
    if ( numLine == eRs422_line_1 )        
    {  
        UART1_PIN_REMAP;
        INIC_UART_L1;          
        INIC_PORT_DE_L1;
    }
    else
    {
        UART2_PIN_REMAP;
        INIC_UART_L2;   
        INIC_PORT_DE_L2;
    }
    inStr->state = eStateInWaitDle;
}

//*****************************************************************************
// ������������� ���������� �������� ������ �� RS-422
void outRS422_ctor( OutRS422 *outStr, Rs422_numLine numLine )
{
    uint8_t i;

    if ( numLine == eRs422_line_1 ) 
    {
        SET_DE_L1( OBMEN_PRIEM );
    }
    else 
    {
        SET_DE_L2( OBMEN_PRIEM );
    }
    for( i = 0; i < N_ARRAY_OUT_RS422; i++ ) 
        outStr->aOutRS422[i] = 0;
    outStr->indDate = 0;
    outStr->length = 0;
    outStr->state = eStateOutWaitData;
    outStr->timeOut = 0;
}

//*****************************************************************************
// ����� ������ � �������� UART.
void inRS422_readReg( InRS422 *inStr, Rs422_numLine numLine )
{
    bool rezData;
    uint8_t data;
  
    inStr->cInReadRegTime++;
    
    while ( true )
    { 
        rezData = false;
        if ( numLine == eRs422_line_1 )        
        {     
            if( L1_UART_IS_DATA_IN )
            {
                data = L1_UART_REG_IN;
                //17.02.21.- ������- L1_UART_CLEAR_OERR; // ����� ������������ ������
                rezData = true;
            }
        } 
        else
        {
            if( L2_UART_IS_DATA_IN )
            {
                data = L2_UART_REG_IN;
                //17.02.21.- ������- L2_UART_CLEAR_OERR; // ����� ������������ ������
                rezData = true;
            }
        }
        if( rezData )
        {
            inStr->aInReadRegTime[ inStr->ndxWriteReg ] = inStr->cInReadRegTime;
            inStr->aInReadReg[ inStr->ndxWriteReg++ ] = data;
            if ( inStr->ndxWriteReg >= V_ARRAY_IN_REG ) inStr->ndxWriteReg = 0;
        }
        else break;
    }
    /* 17.02.21.0 ������: �������� datasheet ���� ������ ������������ OERR ����� ���������� ����� ������� ������, */
    /* ����� ��� ������ � ������ (4 ����� � FIFO + 1 ���� � UxRSR) ����� �������� */
    if( numLine == eRs422_line_1 )
    {
        L1_UART_CLEAR_OERR; // ����� ������������ ������ ������ UART1.
    }
    else
    {
        L2_UART_CLEAR_OERR; // ����� ������������ ������ ������ UART2.
    }
}

//*****************************************************************************
// ����� � HDLC-������������� ���������� �� ����� ����� RS-422
void inRS422_run( InRS422 *inStr, uint16_t addr )
{
    uint8_t data, myAddr, time;
    
    while ( true )
    { 
        if( inStr->ndxReadReg == inStr->ndxWriteReg )  
            break;
        
        time = inStr->aInReadRegTime[ inStr->ndxReadReg ];
        data = inStr->aInReadReg[ inStr->ndxReadReg++ ];
        if ( inStr->ndxReadReg >= V_ARRAY_IN_REG ) 
            inStr->ndxReadReg = 0;
        
        switch( inStr->state )
        {
            case eStateInWaitDle: //�������� ������ ���������� - ����� DLE
                if( data == DLE )
                {
                    inStr->state = eStateInStx;
                    inStr->acTimeIn[inStr->indWriteArray] = time; //���� DLE ������, �������� �������
                }
                break;

            case eStateInStx: //�������� ����� STX
                if( data == STX )
                {
                    inStr->indDate = 0; //���� STX ������, ��������� ������� �������
                    inStr->aLength[inStr->indWriteArray] = 0; //��������� �������� ����� ����������
                    inStr->state = eStateInData;
                    break;
                }
                inStr->state = eStateInWaitDle;
                break;

            case eStateInData: //����� ������ ������, �������� ����� DLE
                if( data == DLE )
                {
                    inStr->state = eStateInPostDle;
                    break;
                }
                if( inStr->indDate >= MAX_ORDER_TELE - 1 )
                {
                    inStr->state = eStateInWaitDle; //����� ���������� ��������� ���������� ��������
                    break;
                }
                inStr->aInRS422[inStr->indWriteArray][inStr->indDate++] = data; //������ �����                 
                break;

            case eStateInPostDle: //������ ���� DLE 
                if( data == DLE )
                { //���� DLE ��������� 
                    if( inStr->indDate >= MAX_ORDER_TELE - 1 )
                    {
                        inStr->state = eStateInWaitDle; //����� ���������� ��������� ���������� ��������
                        break;
                    }
                    inStr->aInRS422[inStr->indWriteArray][inStr->indDate++] = data; //������ �����
                    inStr->state = eStateInData;
                    break;
                }
                if( data == ETX )
                { // ������� ����� �������
                    //�������� ������
                    myAddr = false;
                    if( inStr->indDate == 1 )
                    { //�������� ���������
                        if( ( inStr->aInRS422[inStr->indWriteArray][0] & 0x0F ) == ( addr & 0x000F ) ) myAddr = true; //����� ���������
                    }
                    else
                    { //������
                        if( inStr->aInRS422[inStr->indWriteArray][0] == ( addr >> 8 & 0x00FF ) && //������ ���� ������� ����
                            inStr->aInRS422[inStr->indWriteArray][1] == ( addr & 0x00FF ) )
                        {
                            myAddr = true;
                        }
                    }

                    if( myAddr == true )
                    { //����� ���������
                        // ���������� ����� ������ ����������
                        inStr->aLength[inStr->indWriteArray] = inStr->indDate; //������ ����� �������� ����������
                        if( ++inStr->indWriteArray >= V_ARRAY_IN_RS422 ) inStr->indWriteArray = 0; //��������� ������� ���������� �������
                        inStr->aLength[inStr->indWriteArray] = 0; //��������� ����� �������
                    }
                }
                inStr->state = eStateInWaitDle;
                break;

            default:
                //������ �������� ���������
                ERROR_EX_ID( eGrPS_Rs422, ePS_Rs422StepCntError, 0xAAAA, eStepCntError_inRS422_run, 0xBBBB, 0 );
        } //end switch
    }//end for     

}

//*****************************************************************************
// �������� ���������� �� ����� ����� RS-422
void outRS422_run( OutRS422 *outStr, Rs422_numLine numLine )
{
    uint16_t i;
    
    if( outStr->timeOut ) 
        outStr->timeOut--; //��������� ����-���� 

    switch( outStr->state )
    {
        case eStateOutWaitData:
            if( outStr->length && !outStr->timeOut )
            {
                if ( numLine == eRs422_line_1 ) 
                {
                    SET_DE_L1( OBMEN_PEREDACHA );
                }
                else 
                {
                    SET_DE_L2( OBMEN_PEREDACHA );
                }
                outStr->indDate = 0;
                outStr->state = eStateOutWaitManageOut;
            }
            break;

        case eStateOutWaitManageOut: //�������� �� ����� ������������
            outStr->state = eStateOutDataTransmit;
            break;

        case eStateOutDataTransmit:
            for ( i = 0; i < 2; i++ ) 
            {
                if ( numLine == eRs422_line_1 ) 
                {
                    if( !L1_UART_IS_READY_OUT ) 
                    	break;
                    else 
                    	L1_UART_REG_OUT = outStr->aOutRS422[outStr->indDate];
                }
                else 
                {
                    if( !L2_UART_IS_READY_OUT ) 
                    	break;
                    else 
                    	L2_UART_REG_OUT = outStr->aOutRS422[outStr->indDate];
                } 
                outStr->indDate++;
                if( outStr->length > 1 ) 
                    outStr->length--;
                else
                { //�������� ������ ��������
                    outStr->length = 0;
                    outStr->state = eStateOutWaitEndTransmit;
                    break;
                }
            }
            break;

        case eStateOutWaitEndTransmit:
            if ( numLine == eRs422_line_1 ) 
            {
                if( !L1_UART_IS_END_OUT ) 
                    break;
            }
            else 
            {
                if( !L2_UART_IS_END_OUT ) 
                    break;
            }
            outStr->state = eStateOutEnd;
            break;

        case eStateOutEnd:
            if ( numLine == eRs422_line_1 ) 
            {
                SET_DE_L1( OBMEN_PRIEM );
            }
            else 
            {
                SET_DE_L2( OBMEN_PRIEM );
            }
            outStr->state = eStateOutWaitData;
            break;

        default: //������ �������� ���������
            ERROR_EX_ID( eGrPS_Rs422, ePS_Rs422StepCntError, 0xAAAA, eStepCntError_outRS422_run, 0xBBBB, 0 );
    }
}

//*****************************************************************************
// ������������ ���� ������ ��� ��������� �������
uint8_t formAlarmRS422( uint8_t nCycle, uint8_t *alarmArray )
{
    static uint8_t nCycleOld, alarmOld, index;
    static uint8_t retCode;
    static bool isMessage;

    if( nCycleOld != nCycle )
    {
        nCycleOld = nCycle;
        if ( isMessage )
        { // ��������� � ������
            retCode = MessageVersion_get();
            isMessage = false;
        }
        else
        { // ������������ ������    
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
            retCode = alarmOld;
            
#ifdef RS422_ALARM_OUT_VERSION            
            isMessage = true;
#endif            
        }
    }
    return retCode;
}

//*****************************************************************************
// �������� ������������ �������� ����������
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
// �������� ������������ ���������� ����������
bool checkTelegramm( uint8_t *aTele, uint8_t lengthTele )
{

    uint8_t date, crc;

    date = aTele[INDEX_ML_CO] >> 4; // ����� ���������� � ������ ����������� �����

    // �������� �����
    if( date != lengthTele || date > MAX_ORDER_TELE || date < MIN_ORDER_TELE )
        return false;

    //�������� ���� ����������
    date = aTele[INDEX_ML_CO] & CO_MASK;
    if( date != CO_PRIKAZ_A && date != CO_PRIKAZ_B )
        return false;

    //������� CRC8 �����: ADDR, ML/CO, DATA
    crc = crc8_update( 0, aTele, 3 );
    if( lengthTele != MIN_ORDER_TELE )
    {
        crc = crc8_update( crc, ( aTele + INDEX_IN_DATE_0 ), lengthTele - MIN_ORDER_TELE );
    }

    // �������� ����������� �����
    if( crc != aTele[lengthTele - 1] ) return false;
    return true;
}

//*****************************************************************************
// �������� �������� ������������ ��������� ������� � � �
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
// ��������� ������ � ����� �������� ����������
uint8_t InRS422_getTele( InRS422 *inStr,
                         uint8_t **aTele,
                         uint16_t *ptrTime )
{
    uint8_t ret = 0;

    //�������� ������� ���������� 
    if( inStr->aLength[inStr->indReadArray] == 0 ) return ret;
    //���������� �������, ���������� �� ������
    *aTele = &inStr->aInRS422[inStr->indReadArray][0];
    //*ptrTime = inStr->acTimeIn[inStr->indReadArray];
    ret = inStr->aLength[inStr->indReadArray];
    *ptrTime = ( inStr->cInReadRegTime > inStr->acTimeIn[inStr->indReadArray] ) ?
                 inStr->cInReadRegTime - inStr->acTimeIn[inStr->indReadArray]   :
                 inStr->cInReadRegTime + (0xFF - inStr->acTimeIn[inStr->indReadArray] );
    inStr->aLength[inStr->indReadArray] = 0; //��������� �������� ������� ����������
    if( ++inStr->indReadArray >= V_ARRAY_IN_RS422 ) 
        inStr->indReadArray = 0; //��������� ������� ������
    return ret;
}

//*****************************************************************************
// ������������ ���������� �������
uint8_t formTeleStatus( uint8_t *aTele, uint8_t type, uint16_t adrOK,
                        uint8_t nCycle, uint8_t alarm, uint8_t *statusArray,
                        uint8_t statusLength )
{

    uint8_t i, j, size, aDate[MAX_STATUS_TELE], crc;

    if( statusLength == 0 ) return 0;

    aDate[INDEX_ADDRESS_OK_0] = adrOK >> 8 & 0x00FF; // ������ ���� ������� ����
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
    //������� CRC8 �����: ADDR, ML/CO, DATA
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
* ������� ���������: 
* 
* ������ 1.0.1
* ����   22-05-2017
* �����  ��������� �.�.
* 
* ���������:
*    ������� ������.
* 
* ������ 1.0.2
* ����   29-12-2020
* �����  ��������� �.�.
* 
* ���������:
*    1. ������� � ��������� ������� �������� RS422 � ���� ������.
*    2. ������� � ��������� HDLC-������������� �� ���������� � ������� ����.
*    3. ��������� ������ �� ����� ������� �� ������ ������ ����������.
*
* ������ 1.0.3
* ����   25-02-2021
* �����  ��������� �.�.
* 
* ���������: 
*    ����� ����� ������������ ����� ������� ������ ������.
*
* ������ 1.0.4
* ����   15-03-2021
* �����  ��������� �.�.
* 
* ���������: 
*    � ������� formAlarmRS422() ��������� �������� ������ ������ � ���� ������ ��.
*/