/**
* \file    InterChannel.c
* \brief   \copybrief InterChannel.h
*
* \version 1.0.15
* \date    04-02-2021
* \author  ��������� �.�.
*/

//*****************************************************************************
// ������������ �����
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
// ������� ���� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ���������� ���� � ����� � �������, ������������ �� ����� �����.  
///
#define GET_COMMUNICATION_DATA( hi,lo ) ( ( ( hi ) << 8 ) + ( ( lo ) & 0x00FF ) )

//*****************************************************************************
/// \brief �������� ���� � ����� � �������, ������������ �� ����� �����.
///
#define SET_COMMUNICATION_DATA( d, hi,lo ) ( hi ) = ( ( d ) >> 8 ); ( lo ) = ( ( d ) & 0x00FF )       

//*****************************************************************************
/// \brief �������.
/// \note ������ ���������� ������� �� ���� ��������, ��������� � ���������� a � b.
///
#define MIN( a,b ) ( ( ( a )<( b ) )?( a ):( b ) )

//*****************************************************************************
/// \brief ��������.
/// \note ������ ���������� ������� �� ���� ��������, ��������� � ���������� a � b.
///
#define MAX( a,b ) ( ( ( a )>( b ) )?( a ):( b ) )  

//*****************************************************************************
/// \brief �������� ����������� ��������� �����������.  
///
#ifdef INTERCHANNEL_INTERCHANNEL_DEBUG_MODE
    #if ( INTERCHANNEL_INTERCHANNEL_DEBUG_MODE == INTERCHANNEL_INTERCHANNEL_DEBUG_MODE_ON  )  

        #ifndef INTERCHANNEL_INTERCHANNEL_DEBUG_TRACING_ARRAY
            #error "�� ��������� ������ ������� �����������!"
        #endif

        #if    ( INTERCHANNEL_INTERCHANNEL_DEBUG_TRACING_ARRAY < 0 )                \
            || ( INTERCHANNEL_INTERCHANNEL_DEBUG_TRACING_ARRAY >= TRACING_NUMBER )  
            #error "������ ������� ����������� ������ �� �����!"
        #endif

    #endif
#endif

//*****************************************************************************
/// \brief ����� ��������� �������������.
/// \note ������ ��������� ����� ����� �� ��� ��������, ���������� � 
/// ������������ ���� \a SignalFlags.
///
#define FLAG_MASK( flag ) (uint16_t)( 1 << (flag) )

//*****************************************************************************
/// \brief ������� ���������� �������� � ��������.
/// \param range_ � ��������, ��������������� 100%;
/// \param val_   � ����������� ��������.
///
#define valToPercent( range_, val_ ) ((100 * (val_)) / (range_))

//*****************************************************************************
/// \brief ������� ��������� � ���������� ��������.
/// \param range_ � ��������, ��������������� 100%;
/// \param per_   � ����������� ��������.
///
#define percentToVal( range_, per_ ) ( ( ( range_ ) * ( per_ ) ) / 100 )

//*****************************************************************************
/// \brief ������������� �������� ������������ �������.
/// \param id_ � ������������� ���������, ��� �������� �����������
/// �������;
/// \param conTime_ � ������������� ������������ �������� 
/// #InterChannelControlledTime;
/// \param time_ � �������� ��������������� �������, ��. 
/// \note ������ ��������� ������� �������, �������� �������� ���������� ���������.
/// \note �������� \a time_ ���������� � ����������� � 1 �� ��� �������, ��� 
/// ������� #InterChannel_run ���������� � ����� �� ��������. 
///
#define InterChannel_initTime( id_, conTime_, time_ ) do                         \
{                                                                                \
    interChannelData.timing[( conTime_ )].time[( id_ )] = ( time_ );             \
    Set64_insert( &interChannelData.timing[( conTime_ )].timeSet64, ( id_ ) );   \
} while( 0 )

//*****************************************************************************
/// \brief ������ �������� ������������ �������.
/// \param id_ � ������������� ���������, ��� �������� �����������
/// �������;
/// \param time_ � ������������� ��������, ���� �� �������� 
/// #InterChannelControlledTime.
/// \note ������ ��������� ������� �������, �������� �������� ���������� ���������.
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
/// \brief ������ �������� ������������ ������� �1.
/// \param id_ � ������������� ���������, ��� �������� �����������
/// �������.
///
#define InterChannel_startTime1( id_ )                               \
           InterChannel_startTime( id_, eInterChannelConTime1 ) 

//*****************************************************************************
/// \brief ������ �������� ������������ ������� �2.
/// \param id_ � ������������� ���������, ��� �������� �����������
/// �������.
/// \note ������ ��������� ������� �������, �������� �������� ���������� ���������.
///
#define InterChannel_startTime2( id_ )                                          \
           InterChannel_startTime( id_, eInterChannelConTime2 )  

//*****************************************************************************
/// \brief ������ �������� ������������ ������� �3.
/// \param id_ � ������������� ���������, ��� �������� �����������
/// �������.
/// \note ������ ��������� ������� �������, �������� �������� ���������� ���������.
///
#define InterChannel_startTime3( id_ )                                        \
           InterChannel_startTime( id_, eInterChannelConTime3 ) 

//*****************************************************************************
/// \brief ������ ��������� ������������ ������� �1 � �3.
/// \param id_ � ������������� ���������, ��� �������� �����������
/// �������.
/// \note �������� �1 � �3 ����������� ������������ ��� ��������� ������� 
/// �� �������������. 
/// \note ������ ��������� ������� �������, �������� �������� ���������� ���������.
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
/// \brief ��������� �������� ������������ ������� �1.
/// \param id_ � ������������� ���������, ��� �������� �����������
/// �������.
///
#define InterChannel_stopTime1( id_ )                                        \
    Set64_remove( &interChannelData.timing[eInterChannelConTime1].timeSet64, \
                  ( id_ ) )

//*****************************************************************************
/// \brief ��������� �������� ������������ ������� �2.
/// \param id_ � ������������� ���������, ��� �������� �����������
/// �������.
///
#define InterChannel_stopTime2( id_ )                                        \
    Set64_remove( &interChannelData.timing[eInterChannelConTime2].timeSet64, \
                  ( id_ ) )

//*****************************************************************************
/// \brief ��������� �������� ������������ ������� �3.
/// \param id_ � ������������� ���������, ��� �������� �����������
/// �������.
///
#define InterChannel_stopTime3( id_ )                                        \
    Set64_remove( &interChannelData.timing[eInterChannelConTime3].timeSet64, \
                  ( id_ ) )

//*****************************************************************************
/// \brief �������� ����, ��� ������� ������������ ������� �1 �������.
/// \param id_ � ������������� ���������, ��� �������� �����������
/// �������.
///
#define InterChannel_time1IsRunning( id_ )                                   \
    !Set64_hasElement( &interChannelData.timing[eInterChannelConTime1]       \
                       .timeSet64, ( id_ ) )

//*****************************************************************************
/// \brief �������� ����, ��� ������� ������������ ������� �2 �������.
/// \param id_ � ������������� ���������, ��� �������� �����������
/// �������.
///
#define InterChannel_time2IsRunning( id_ )                                   \
    !Set64_hasElement( &interChannelData.timing[eInterChannelConTime2]       \
                       .timeSet64, ( id_ ) )

//*****************************************************************************
/// \brief �������� ����, ��� ������� ������������ ������� �3 �������.
/// \param id_ � ������������� ���������, ��� �������� �����������
/// �������.
///
#define InterChannel_time3IsRunning( id_ )                                   \
    !Set64_hasElement( &interChannelData.timing[eInterChannelConTime3]       \
                       .timeSet64, ( id_ ) )

//*****************************************************************************
/// \brief ��������� �������� ������ �� ������ �1,�2 ��� �3
/// ��� ���� ���������� �������������.
/// \param conTime_ � ������������� ������� #InterChannelControlledTime,
/// ������� �������� ����� ���������� ��� ���� ���������� �������������.
///
#define InterChannel_stopTimeForAllId( conTime_ )                            \
    Set64_clear(&interChannelData.timing[ ( conTime_ ) ].timeSet64)

//*****************************************************************************
/// \brief ��������� ��������� � ����������� ������, ���������� ���� �� 
/// ���������� ������� ��� ������ � CAN.
///
#define ATOMIC_SYS( x ) do     \
{                              \
    sysLock();                 \
    x;                         \
    sysUnLock();               \
} while( 0 )

//*****************************************************************************
/// \brief ��������� ������� �������������.
/// \details ������������ ���������� �����: #eLocal, #eLocalAck, #eRemote � 
/// ������������� �����: #eSynchronized, #eSyncEvent, #eSyncEventOverflow.
/// \details ���� #eSyncEventOverflow ��������������� ������ ���� �� ����������� 
/// ������� ������������� ���� #eSyncEvent �� ��� �������.
/// \param id_ � ������������� ���������, ��� �������� ��������� �������
/// �������������.
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
// ��������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ���������� ���� ����� ������, ����������� �� ��������� ������ �� 
/// ����� ����� �� ���� ��������.
///
#define REMODE_DATA_BLOCK_SIZE    6

//*****************************************************************************
/// \brief ���������� ������ ������, ������� ����� ���� �������� � ��������� 
/// � �������� � ��� �� �� ��������� � #InterChannel_run.
///
#define REMODE_DATA_STORAGE_SIZE    10

//*****************************************************************************
// ���������� ����� ������
//*****************************************************************************

//*****************************************************************************
/// \brief ����� �������� ����������.
///
typedef enum
{
    eLocal,                   ///< ������� ������ �� ���������� ��������
    eLocalAck,                ///< �������� ������������� ������ ���������
    eRemote,                  ///< ������ �������� �� ��������� ������
    eSynchronized,            ///< ��������� �������������
    eSyncEvent,               ///< ��������� ������� �������������            
    eSyncEventOverflow,       ///< ��������� ������������ ������� �������������          
} InterChannelParamFlags;

//*****************************************************************************
/// \brief ��������� �������������, ����������� ��� ������ �������
/// ������������ ������������� �� ����������������� ���������. 
///
typedef enum
{
    eInterChannelConTime1     = 0,                   ///< ����� ��������� ��������� �� ����� ������� (�1)
    eInterChannelConTime2,                           ///< ����� ������������ ������������������� ��������� (�2)
    eInterChannelConTime3,                           ///< ������ ����������� �������� �� ������������� (�3)
    eInterChannelConTimeCount                        ///< ���������� �������������� ������
} InterChannelControlledTime;

//*****************************************************************************
/// \brief ��������� ��� ������, ������������ �� ����� �����.
///
typedef struct
{
    uint16_t m_id;                      ///< ������������� ���������.
    param_t  m_value;                   ///< �������� ���������.
} InterChannelTransmitData;

//*****************************************************************************
/// \brief ��������� ��������� �������������.
///
typedef struct
{
    InterChannelScript    m_script;                     ///< ��������, ����������� ���.
    InterChannelProcSync  m_procSync;                   ///< ��������� ������������� ������.
    InterChannelProcCheck m_procCheck;                  ///< ��������� �������� ������.
    param_t               m_paramCheck;                 ///< �������� ��������.
    param_t               m_middleRangeCheck;           ///< �������� �������� �������������� ��������� ��������������� ���������.

    
    // eInterChannelConTime1
    // eInterChannelConTime2
    // eInterChannelConTime3
    uint16_t m_time[eInterChannelConTimeCount];         ///< �������������� ��������� ��������������.

} InterChannelParamSettings;

//*****************************************************************************
/// \brief ��������� ����������������� ���������.
///
typedef struct
{
    uint16_t m_flags;                             ///< ����� �������� ����������.
    param_t  m_localValue;                        ///< �������� ��������� ������ ������.
    param_t  m_remoteValue;                       ///< �������� ��������� ��������� ������.
    param_t  m_operatingValue;                    ///< �������� ��������� ����� ���������.
    param_t  m_lastTranslateValue; // tret 04.02.2021 ///< ��������� ���������� �������� ���������.
                                 
} InterChannelParamState;

//*****************************************************************************
/// \brief �������� �������������.
/// \details ��������� ��������� ��� ������, ����������� ��� ������ ������� 
/// ������������ ������������� �� ����������������� ���������.
/// �������� �������������� ��������� ���������� ������ � �������� 
/// ���� #InterChannelControlledTime.
///
typedef struct
{
    InterChannelParamSettings m_settings;                          ///< ��������� ��������� �������������.
    InterChannelParamState    m_state;                             ///< ��������� ��������� � �������� �������������.
} InterChannelParamDump;

//*****************************************************************************
/// \brief �������� �������.
/// \details ��������� ��������� ��� ������, ����������� ��� ���������� 
/// �������� ���� �� �������������� ��������� ���������� ��� ������������� ������.
///
typedef struct
{
    //**************************************************************************
    /// \brief ������ ����������, ��� ������� �������������� �����.
    Set64_t  timeSet64;

    //**************************************************************************
    /// \brief �������� ��������������� ������� ��� ������� ����������������� 
    /// ��������� � �������.
    uint16_t time[eInterChannelIdCount];   

} InterChannelTiming;

//************************************************************************
/// \brief ��������������� ������ ��� ������ #InterChannel_runCommunication.
///
typedef struct
{
    bool     isTrasmitting;               ///< ������� ����, ��� �� ����� ����� ���������� ��������� � �������� �����.
    uint16_t trId1;                       ///< ������������� ������� ���������, ������������� �� ����� �����.
    uint16_t trId2;                       ///< ������������� ������� ���������, ������������� �� ����� �����.
} InterChannelTmp;

//************************************************************************
/// \brief ������ ������� ������������� ������.
///
typedef struct
{
    /// \brief ������ ��������� ������� ����������������� ���������.
    ///
    InterChannelParamDump paramDump[eInterChannelIdCount];

    /// \brief ��������� ����������� ���������� �������� ��������� �������������.
    /// �������� �������������� ��������� ���������� ������ � �������� 
    /// ���� #InterChannelControlledTime.
    ///
    InterChannelTiming    timing[eInterChannelConTimeCount]; 

    Set64_t               set64;               ///< ������������ ������� �������� �� �������������.

    Set64_t               updateSet64;         ///< ������ ������, ����������� � ����������.

    const ArrayIoDriver *drvPtr;               ///< ��������� �� ������ �������� ������ �����.

    InterChannelTmp       tmp;                 ///< ��������������� ������ ��� ������ #InterChannel_runCommunication.

    /// \brief ���� ��� �������� �������� ��������� ���������� ������ ������ ������������� ������.
    ///
    bool                  stateLock;

} InterChannelData;

//************************************************************************
/// \brief ��� ������, ����������� ��������� ������ ������, ����������� 
/// �� ��������� ������ �� ����� �����.
///
typedef struct
{
    uint8_t items [REMODE_DATA_STORAGE_SIZE][REMODE_DATA_BLOCK_SIZE];    ///< ����� ��� �������� ������ ������
    uint16_t count;                                                      ///< ������� ������, ���������� � ���������
} InterChannelStorageRemoteData;

//************************************************************************
/// \brief ��� ������, ����������� ��������� ��������������� ���������, 
/// � ������ ������� ���� �������� ������������� �� ����� ����� �� ��������� 
/// ������.
///
typedef struct
{
    id_t items [REMODE_DATA_STORAGE_SIZE * 2];   ///< ����� ��� �������� ���������������
    //// ��� ����� �������� ������ ����� ������������ 
    //// ��� ��������������, �������������, ����� ��������������� 
    //// ������ ���� � ��� ���� ������, ��� ����� ������ �������� 
    //// �� ��������� ������.
    uint16_t count;                              ///< ������� ���������������, ���������� � ���������
} InterChannelStorageAck;

//*****************************************************************************
// ���������� ��������� �������������� ��������
//*****************************************************************************

const int interChannelDataCheckErrorCount = 3;    ///< ���������� ���������� ������ ����� ��������.

//*****************************************************************************
// ���������� ��������� ����������
//*****************************************************************************

//*****************************************************************************
static InterChannelData interChannelData;         ///< ������ ������� ������������� ������.

//*****************************************************************************
/// \brief ������� ���������� �������.
/// \details ���������� ��������������� ������� ����� ������� ������
/// #InterChannel_run.
///
static uint32_t mainTime;

//************************************************************************
/// \brief ��������� ������ ������, ����������� �� ��������� ������ �� 
/// ����� �����.
///
static InterChannelStorageRemoteData interChannelStorageRemoteData;

//************************************************************************
/// \brief ��������� ��������������� ���������, � ������ ������� ���� ��������
/// ������������� �� ����� ����� �� ��������� ������.
///
static InterChannelStorageAck interChannelStorageAck;

//*****************************************************************************
// ���������� ��������� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ����������� ������, ����������� ������ ����������.
///
inline static void sysLock( void )
{
    interChannelData.stateLock = !CAN_TIMER_IS_ENABLE_INTERRUPT;
    CAN_TIMER_DISABLE_INTERRUPT;
}

//*****************************************************************************
/// \brief ����������� ������, ����������� ������ ����������.
///
inline static void sysUnLock( void )
{
    CAN_TIMER_SET_ENABLE_INTERRUPT( !interChannelData.stateLock );
}

//*****************************************************************************
/// \brief �������� ��������� ����������.
/// \details ��� ���������� ��������� ������������ ����� ������������ ����� ����������
/// ��������� �������. ���������� ��������� �������������� �������:
/// \details � ����� ��������� ��������� �� ����� ������� (�1);
/// \details � ����� ������������ ������������������� ��������� (�2);
/// \details � ������ ����������� �������� � ��� (�3).
/// \note ���������� �� ������� #InterChannel_run, ���������� 
/// ��������������� ������� ����� ������� ������ ���� �������.
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
/// \brief ���������� ������������� ���������.
/// \details ������������� ���������� ������ ������� � ����� ������������ � 
/// ����������� ������� � ���������� �������������.
/// \param id � ������������� ����������������� ���������.
///
void InterChannel_synchronization( id_t id )
{
    if( ( interChannelData.paramDump[id].m_state.m_flags
          & ( FLAG_MASK( eLocal ) | FLAG_MASK( eRemote ) | FLAG_MASK( eLocalAck ) )
          )
        == ( FLAG_MASK( eLocal ) | FLAG_MASK( eRemote ) | FLAG_MASK( eLocalAck ) )
        )
    {
        // ��������� �������� ������� �1
        InterChannel_stopTime1(id);

        // ������������� ������� ������������� �������� ������
        Set64_insert( &interChannelData.updateSet64, id );
    }
}

//*****************************************************************************
/// \brief ���������� ������������� ��������� �� ��������� ��������.
/// \details ��� 1-�� ������������� ���������, ����� ������ �� ��������� ������
/// �����������, ������� ���� ������ �� ����� �������, ��������� 
/// #InterChannel_synchronization.
/// \details ���� ������������� �� 1-��, ������� ��������� ��������� �����
/// ��������� ������ ��� ��������. � ���� ������, ��� ����������� ����� 
/// ������������ ����� ������ �� �������, ����������� ��� ���������� 
/// �������������.
/// \param id � ������������� ����������������� ���������.
///
void InterChannel_synchChangeValue( id_t id )
{

    if( ( ( interChannelData.paramDump[id].m_state.m_flags
            & ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) )
            )
          == ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) )
          )
        || // ���� ���������, ������������� ��� �� ���������
        // corr 04.02.21 ��������� 
        ( ( interChannelData.paramDump[id].m_state.m_flags
            &  FLAG_MASK( eRemote ) )
          ==  FLAG_MASK( eRemote )  )
      )      
    {
        // ������������� ������� ������������� �������� ������
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
/// \brief ���������� �������� ���������.
/// \note � �������� ���������� ������
/// �������� (��������� ��� ���������������) ����������� ���������� ��� �������� �������� ���������.
/// \param id � ������������� ����������������� ���������.
///
void InterChannel_transmission( id_t id )
{

    // �������� �������� (�������� ��������� ��������� ������)
    if( ( interChannelData.paramDump[id].m_state.m_flags
          & ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) ) )
        == ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) )
        )
    {

        // ���������, ��� �� ����� ���������� ��������� �������� 
        // �������� ������������� ��������� ��������, �� ���� ������ 
        // �� ��������� ������
        ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChTrError1,
                       ( !( interChannelData.paramDump[id].m_state.m_flags & FLAG_MASK( eRemote ) ) ),
                       id, 0, 0, 0 );

        interChannelData.paramDump[id].m_state.m_operatingValue
                = interChannelData.paramDump[id].m_state.m_localValue;

        // ������������� ������� ������������� �������� ������
        Set64_insert( &interChannelData.updateSet64, id );

        // �������������� �������� (����� ��������� �� ��������� ������)    
    }
    else if( interChannelData.paramDump[id].m_state.m_flags & FLAG_MASK( eRemote ) )
    {

        // ���������, ��� �� ����� ���������� ��������������� �������� 
        // �������� ������������� ��������� ��������, �� ���� ������ 
        // �� ������ ������
        ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChTrError2,
                       ( !( interChannelData.paramDump[id].m_state.m_flags
                            & ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) ) ) ),
                       id, 0, 0, 0 );

        interChannelData.paramDump[id].m_state.m_operatingValue
                = interChannelData.paramDump[id].m_state.m_remoteValue;

        // ������������� ������� ������������� �������� ������
        Set64_insert( &interChannelData.updateSet64, id );
    }
}

//*****************************************************************************
/// \brief ���������� �������� ����������� ���������.
/// \details ���������� �������� �� ��������� ������������� ������, � ������ 
/// �������� �� �� ������ ����� ��� ����, ����� �� ����� ���� �����������
/// ��� �������. ����� ������ ������� ����� �������� ����������.
/// \param id � ������������� ����������������� ���������.
///
void InterChannel_debug( id_t id )
{

    // �������� �������� (�������� ��������� ��������� ������)
    if( ( interChannelData.paramDump[id].m_state.m_flags
          & ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) ) )
        == ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) )
        )
    {
        interChannelData.paramDump[id].m_state.m_flags &= ~( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) );

        // �������������� �������� (����� ��������� �� ��������� ������)    
    }
    else if( interChannelData.paramDump[id].m_state.m_flags & FLAG_MASK( eRemote ) )
    {

        // ������� ������ ������ ������� �� ������, ��������� � �������������
        // � ��������� ����� ���������� �������������
        interChannelData.paramDump[id].m_state.m_flags &= ~FLAG_MASK( eRemote );
    }
}

//*****************************************************************************
/// \brief ��������� ��������. 
/// \details ��������� �������� �������� ����������, 
/// �������� �� ������ � ��������� �������, � ������������ � ���������� 
/// ��������, ��������� � ���������� �������.
/// \note ���� ��������� �������� ������� �� �����, ���������� ������� � ��
/// � ����� #ePS_IntChProcCheck ������ #eGrPS_InterChannel.
/// \param id � ������������� ����������������� ���������.
/// \retval true  - ������ ����������;
/// \retval false - ������ �� ����������.
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
                    // ��� ������������� ����������� ����� ������ ���� ������ ��������
                    // min = valToPercent( max, min );
                    min = __builtin_divud( __builtin_muluu( 100, min ), max );
                    result = ( ( 100 - ( param_t )min )
                               <= interChannelData.paramDump[id].m_settings.m_paramCheck ) ? true : false;
                }
                else
                {
                    result = true;
                }

                // ���� �������� ������ �������� ����������� ���������, 
                // �������� ������� ������������ �������� ���������
            }
            else
            {

                if( max - min > 0 )
                {
                    // ��� ������������� ����������� ����� ������ ���� ������ ��������
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
/// \brief ������������� �������� ����������.
/// \details ��������� ������������������ �������� �� ������ �������� �� ������ �
/// ��������� �������, � ������������ � ���������� �������������, ��������� 
/// � ���������� �������.
/// \note ���� ��������� ������������� ������� �������, ���������� ������� � ��
/// � ����� #ePS_IntChProcSync ������ #eGrPS_InterChannel.
/// \param id � ������������� ����������������� ���������.
///
void InterChannel_synchValue( id_t id )
{
    uint32_t tmp;

    switch( interChannelData.paramDump[id].m_settings.m_procSync )
    {
            //case eProcSyncNil : // �� ����������� ������� �������� � ���������� 
            //    break;
        case eProcSyncOff:
        { // ��� ������������� � ������������ ���������� 
            // ��������� � ������������ ����������, ��� ��������
            // �� ����������, ����������� ��� ������� � 
            interChannelData.paramDump[id].m_state.m_operatingValue =
                    interChannelData.paramDump[id].m_state.m_localValue;
            break;
        }
        case eProcSyncHi:
        { // ����� �������� ��������;
            interChannelData.paramDump[id].m_state.m_operatingValue = MAX(
                        interChannelData.paramDump[id].m_state.m_localValue,
                        interChannelData.paramDump[id].m_state.m_remoteValue );
            break;
        }
        case eProcSyncLo:
        {// ����� �������� ��������;
            interChannelData.paramDump[id].m_state.m_operatingValue = MIN(
                        interChannelData.paramDump[id].m_state.m_localValue,
                        interChannelData.paramDump[id].m_state.m_remoteValue );
            break;
        }
        case eProcSyncAverage:
        {// ����� ������������� ��������  ��������;
            tmp = ( uint32_t )interChannelData.paramDump[id].m_state.m_localValue
                    + ( uint32_t )interChannelData.paramDump[id].m_state.m_remoteValue;
            tmp /= 2U;
            interChannelData.paramDump[id].m_state.m_operatingValue = ( param_t )tmp;
            break;
        }
        case eProcSyncAND:
        {// ����� �� �AND�;
            interChannelData.paramDump[id].m_state.m_operatingValue =
                    ( interChannelData.paramDump[id].m_state.m_localValue
                      & interChannelData.paramDump[id].m_state.m_remoteValue );
            break;
        }
        case eProcSyncOR:
        {// ����� �� �OR�;
            interChannelData.paramDump[id].m_state.m_operatingValue =
                    ( interChannelData.paramDump[id].m_state.m_localValue
                      | interChannelData.paramDump[id].m_state.m_remoteValue );
            break;
        }
        case eProcSyncEqual:
        {// ����������, ���� ���������  

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
/// \brief ���������� ������������������ ������.
/// \details ������� ��������� �������� ������ ������ � ��������� �������. � ������
/// �� �������������, ��������� ������������������ ��������.
/// ��� �������� �������� �� ������� #InterChannel_synchronization ��� 
/// ���������� ������������ ��������� ���������� ��� ������ ������� 
/// #InterChannel_runCommunication. ������ ������� ���������� �� ������� 
/// #InterChannel_run � #InterChannel_getData.
/// \note ����� ������ ������� ������ ���� ������� �� �� ���������� ������� ������.
/// \param id � ������������� ����������������� ���������.
///
void InterChannel_updateData( id_t id )
{
    // ������������� ������� ������������� �������� ������
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

        // �������� ������ �������
        switch( interChannelData.paramDump[id].m_settings.m_script )
        {
            bool resultVerification;
            case eScriptChVal:
            {  // tret 04.02.2021 ��������� ���������� ����� ���������
                
                param_t localValueTemp = interChannelData.paramDump[id].m_state.m_localValue;
                interChannelData.paramDump[id].m_state.m_localValue = 
                interChannelData.paramDump[id].m_state.m_lastTranslateValue; 
                resultVerification = InterChannel_verification( id );
                interChannelData.paramDump[id].m_state.m_localValue = localValueTemp;

                if( resultVerification )
                {

                    InterChannel_synchValue( id );

                    // ���� ����������� ������ ������� ��������� ������� ������
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
                    // ���� ������� �� �������, ��������� ���
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
                
                // ������� ����� �������� ��������� �������
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
                    
                    // ���� ����������� ������ ������� ��������� ������� ������
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
                    // ���� ������� �� �������, ��������� ���
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

                // ������� ����� �������� ��������� �������
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
                // ������� ����� �������� ��������� �������
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

        // ������� �� ������� ��������� ����������
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
/// \brief ��������� ����� ��������� ���������.
/// \details ������� ���������� ������ ���������� ��������� ���������� ���������
/// � ������������� �������� �� ����������.
/// \param id � ������������� ����������������� ���������.
///
void InterChannel_stateChanged( id_t id )
{
    // �������� ������ �������
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
/// \brief ��������� ����� ������ � ���������� �������� ���������, ��������� 
/// �� ��������� ������.
/// \note ��� ������������ �������������� �������������� ��������� ���������� �������
/// � �� � ����� #ePS_IntChIdError ������ #eGrPS_InterChannel.
/// \note ��� ������� ������������� ���������, ��� �������� �� ���������� ������� 
/// #InterChannel_setParamSettings, ���������� ������� � ��
/// � ����� #ePS_IntChScript ������ #eGrPS_InterChannel.
/// \note ���� �������� ��� ������ �� ��������� ������ ��������, 
/// � �� ������ ������ ������ �� ������������� �� ��������,
/// ���������� ������� � �� � ����� #ePS_IntChRepeat ������ #eGrPS_InterChannel.
/// \note ���� �� ��������� ������ ��� ������ ��������,
/// � �������� �������� ��������� ��������, ���������� 
/// ������� � �� � ����� #ePS_IntChRemoteLocalScr ������ #eGrPS_InterChannel.
/// \param id   � ������������� ����������������� ���������;
/// \param data � �������� ����������������� ���������.
///
void InterChannel_setRemote( id_t id, param_t data )
{
    // �������� �������������� ���������
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError,
                   ( ( id > eInterChannelIdBegin ) && ( id < eInterChannelIdCount ) ),
                   1, id, data, eInterChannelIdCount );

    // �������� ������������� �������� 
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChScript,
                   interChannelData.paramDump[id].m_settings.m_script != eScriptNil,
                   1, id, data, interChannelData.paramDump[id].m_settings.m_script );

    // �������� ������ ��������� � ��������� ���������
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChRemoteLocalScr,
                   interChannelData.paramDump[id].m_settings.m_script != eScriptLocal,
                   1, id, data, 0 );

    // ��������� ������ �� ������������� ��������� �� ��������� ������, ���� ���������� �� 
    // ��������� - ��������� ������� ������ ����������������� ���������.
    ASSERT_EX_ID(  eGrPS_InterChannel,ePS_IntChRepeat,
              ! (interChannelData.paramDump[id].m_state.m_flags  & FLAG_MASK(eRemote) ),
                 1, id, data, interChannelData.paramDump[id].m_state.m_flags );

    // ��������� ����� ������ ��������� �� ��������� ������ Remote
    interChannelData.paramDump[id].m_state.m_flags |= FLAG_MASK( eRemote );

    // ���������� �������� ��������� ��������� ������
    interChannelData.paramDump[id].m_state.m_remoteValue = data;

    // ��� ��������� ����� �� ��������� ������, ���� ���� �� ������ ������  
    // �� ����������, ��������� ������������� ����������� ������.
    if( !( interChannelData.paramDump[id].m_state.m_flags & FLAG_MASK( eLocal ) ) )
    {
        InterChannel_startTime13( id );
    }

    // �������� ������� ����������� �������������            
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
/// \brief ��������� ����� ������������� ������.
/// \note ��� ������������ �������������� �������������� ��������� ���������� �������
/// � �� � ����� #ePS_IntChIdError ������ #eGrPS_InterChannel.
/// \note ���� �������� ��� ������ �� ��������� ������ ��������,
/// � �� ������ ������ ������ �� ������������� �� ��������,
/// ���������� ������� � �� � ����� #ePS_IntChRepeat ������ #eGrPS_InterChannel.
/// \param id � ������������� ����������������� ���������.
///
void InterChannel_setLocalAck( id_t id )
{
    // �������� �������������� ���������
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError,
                   ( ( id > eInterChannelIdBegin ) && ( id < eInterChannelIdCount ) ),
                   2, id, 0, eInterChannelIdCount );

    // ��������� ����� ������ ��������� �� ��������� ������ eLocalAck
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
// ���������� ������������ �������
//*****************************************************************************

//*****************************************************************************
// ������������� ���������� ����������
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
// ��������� ��������� ������������� ���������
void InterChannel_setParamSettings( id_t id,
                                    InterChannelScript script,
                                    InterChannelProcSync procSync,
                                    InterChannelProcCheck procCheck,
                                    param_t paramCheck, // �������� ��������
                                    param_t middleRangeCheck, // �������� �������� �������������� ��������� ��������������� ���������
                                    uint16_t time1, // ����� �1
                                    uint16_t time2, // ����� �2
                                    uint16_t time3 // ����� �3
                                    )
{
    // �������� ��������������
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   4, id, 0, eInterChannelIdCount );
    // �������� �������� �������������
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChScript,
                   ( ( script > eScriptNil )&&( script < eScriptCount ) ),
                   4, id, 0, script );
    // �������� ������������� ��������� �������� eProcCheckEqual 
    // ��� �������� ��������� ������������� eProcSyncEqual.
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChProcCheck, 
                  ( eProcSyncEqual == procSync ) ? ( eProcCheckEqual == procCheck) : true ,
                  id, script, procCheck, procSync  );
    // �������� ������������ ������� ������� �1. ���� ������������ ��������
    // ������������� eScriptSync, ����� time1 ����������� ������ ���� ������ 
    // (���� ������ ����). ��� ��������� ��������� - ������ ���� ���� ����.
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
// ������ ������������� ���������
void InterChannel_synchronize( id_t id, param_t data )
{
    //-------------------------------------------------------------------------
    // �������� ������������ �������
    //-------------------------------------------------------------------------

    // �������� �������������� ���������
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   3, id, data, eInterChannelIdCount );

    // ��������� ������ �� ������������� ���������, ���� ���������� �� 
    // ��������� ��������� ������� ������ ����������������� ���������.
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChRepeat,
                   !( interChannelData.paramDump[id].m_state.m_flags & ( FLAG_MASK( eLocal ) | FLAG_MASK( eLocalAck ) ) ),
                   3, id, data, interChannelData.paramDump[id].m_state.m_flags );
    //-------------------------------------------------------------------------
    // ���������� ������� �� ���������
    //-------------------------------------------------------------------------

    switch( interChannelData.paramDump[id].m_settings.m_script )
    {
        case eScriptChVal:
        case eScriptSync:
        case eScriptDebug:
        case eScriptTransmit:
        {

            sysLock();

            // ���������� �������� ������ ���������
            interChannelData.paramDump[id].m_state.m_localValue = data;

            // ��������� ����� ������ �� ������ ������ Local
            interChannelData.paramDump[id].m_state.m_flags |= FLAG_MASK( eLocal );

            // ��� ��������� ����� �� ��������� ������, ���� ���� �� ������ ������ �� 
            // ����������, ��������� ������������� ����������� ������.
            if( !( interChannelData.paramDump[id].m_state.m_flags & FLAG_MASK( eRemote ) ) )
            {
                InterChannel_startTime13( id );
            }

            // �������� ������� ����������� �������������            
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

            // �������� ����� ��������� ��������� 
            InterChannel_stateChanged( id );

            // ���������� ������� ���������, ���������� ��������
            Set64_insert( &interChannelData.set64, id );

            sysUnLock();

            break;
        }
        case eScriptLocal:
        {
            sysLock();
            interChannelData.paramDump[id].m_state.m_localValue = data; // �������� ��������� ������ ������
            interChannelData.paramDump[id].m_state.m_remoteValue = data; // �������� ��������� ��������� ������
            interChannelData.paramDump[id].m_state.m_operatingValue = data; // �������� ��������� ����� ���������

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
// ������� ���������� ������������� ���������
bool InterChannel_isSynchronized( id_t id )
{
    // �������� �������������� ���������
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   8, id, 0, eInterChannelIdCount );

    return (  interChannelData.paramDump[id].m_state.m_flags 
            & FLAG_MASK( eSynchronized ) ) ? true : false;
}

//*****************************************************************************
// ������� �������������.
bool InterChannel_syncEvent( id_t id )
{ 
    bool result;
    
    // �������� �������������� ���������
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   11, id, 0, eInterChannelIdCount );
    
    result = (  interChannelData.paramDump[id].m_state.m_flags 
            & FLAG_MASK( eSyncEvent ) ) ? true : false ;
    
    interChannelData.paramDump[id].m_state.m_flags &= ~FLAG_MASK( eSyncEvent );
    
    return result;
}

//*****************************************************************************
// ������� ������������ ������� �������������.
bool InterChannel_syncEventOverflow( id_t id )
{
    bool result;
    
    // �������� �������������� ���������
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   12, id, 0, eInterChannelIdCount );
    
    result = (  interChannelData.paramDump[id].m_state.m_flags 
            & FLAG_MASK( eSyncEventOverflow ) ) ? true : false ;
    
    interChannelData.paramDump[id].m_state.m_flags &= 
                                             ~FLAG_MASK( eSyncEventOverflow );
    
    return result;
}

//*****************************************************************************
// ������� �������� ���������� ������������� ���������
bool InterChannel_isHandling( id_t id )
{
    // �������� �������������� ���������
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   7, id, 0, eInterChannelIdCount );

    return( interChannelData.paramDump[id].m_state.m_flags
            & FLAG_MASK( eLocal ) ) ? true : false;

}

//*****************************************************************************
// ������ ������������������� �������� ���������
param_t InterChannel_getData( id_t id )
{
    param_t result;

    // �������� �������������� ���������
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   9, id, 0, eInterChannelIdCount );

    result = interChannelData.paramDump[id].m_state.m_operatingValue;
    return result;
}

//*****************************************************************************
// ������ �������� ������ ������
param_t InterChannel_getLocalData( id_t id )
{
    param_t result;

    // �������� �������������� ���������
    ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChIdError, id < eInterChannelIdCount,
                   10, id, 0, eInterChannelIdCount );

    result = interChannelData.paramDump[id].m_state.m_localValue;

    return result;
}

//*****************************************************************************
// ��������� �������� ��������� � ���������� ���������
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
// �������� �������� ��������� � ���������� ���������.
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
// ����� �������� � ����������
void InterChannel_runDrv( void )
{
    interChannelData.drvPtr->run( interChannelData.drvPtr );
}

//*****************************************************************************
// ������ � ������ ����� � �������� �������
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

    // ����� ���������
    if( interChannelData.drvPtr->isInReady( interChannelData.drvPtr ) )
    {
        i = interChannelData.drvPtr->get( interChannelData.drvPtr, bufChar, sizeof( bufWord ) );

        // �������� ������������ ����� ������
        ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChInputDriverGetError, i == sizeof( bufWord ),
                       1, i, sizeof( bufWord ), 0 );

        // ��������� � ��������� �������� ������
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

    // ����� ����������� 
    if( interChannelData.drvPtr->isOutReady( interChannelData.drvPtr ) )
    {

        // �������� �������������
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
                // ��������� � ��������� �������� ������
                ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChInputDriverGetError,
                               interChannelStorageAck.count < sizeof(interChannelStorageAck.items ),
                               4, interChannelStorageAck.count, sizeof(interChannelStorageAck.items ), 1 );

                interChannelStorageAck.items[interChannelStorageAck.count]
                        = interChannelData.tmp.trId1;
                interChannelStorageAck.count++;
            }

            if( interChannelData.tmp.trId2 > 0 )
            {
                // ��������� � ��������� �������� ������
                ASSERT_EX_ID( eGrPS_InterChannel, ePS_IntChInputDriverGetError,
                               interChannelStorageAck.count < sizeof(interChannelStorageAck.items ),
                               4, interChannelStorageAck.count, sizeof(interChannelStorageAck.items ), 2 );

                interChannelStorageAck.items[interChannelStorageAck.count]
                        = interChannelData.tmp.trId2;
                interChannelStorageAck.count++;
            }

            interChannelData.tmp.isTrasmitting = false;

            // ��� ����, ��� �� �� ��������� ���������� �������� �������� 
            // �� ���������� ���������� ����� ��������� �������������.

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
// ������������� ����������
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
    // ����������� ������, ��������� ����� �������� InterChannel_run, �� 
    // �������� � �����.
    // ������ ������ ���� ������ ���� �������� �� ���������� � ������ 
    // �������� InterChannel_runCommunication

    sysLock();

    // ���������� �� ��������� ������ ���������� �� ��������� ������           
    memcpy( bufRemoteData, interChannelStorageRemoteData.items,
            REMODE_DATA_BLOCK_SIZE *interChannelStorageRemoteData.count );
    countRemoteData = interChannelStorageRemoteData.count;
    interChannelStorageRemoteData.count = 0;

    // ���������� �� ��������� ��������������� ���������, �� ������� ���� 
    // �������� �������������
    memcpy( bufAck, interChannelStorageAck.items,
            sizeof(id_t ) *interChannelStorageAck.count );
    countAck = interChannelStorageAck.count;
    interChannelStorageAck.count = 0;

    sysUnLock();

    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // ��������� ������, ��������� ����� �������� InterChannel_run

    // ��������� ������, ���������� �� ��������� ������               
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

    // ��������� ��������������� ���������, �� ������� ���� 
    // �������� �������������
    for( i = 0; i < countAck; ++i )
    {
        InterChannel_setLocalAck( bufAck[i] );
    }
    
    //--------------------------------------------------------------------------

    ++mainTime;

    // ���������� ������������������ ������
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

    // �������� ������ (��� �3) 
    for( i = 0; i < ( eInterChannelConTimeCount - 1 ); i++ )
    { 
        InterChannel_checkTime( i );
    }
    MARKED_CALL_FUNCTION;
}

//*****************************************************************************
/**
* ������� ���������: 
* 
* ������ 1.0.1
* �����  ������� �.�.
* 
* ���������:
*    ������� ������, ������������ �� ����� ����-�.
*    
* ������ 1.0.2
* �����  ������� �.�.
* 
* ���������:
*    �������� ����� �������� ������������� � 
*    "������������� �� ��������� ��������".
* 
* ������ 1.0.3
* ����   22-04-2016
* �����  ������� �.�.
* 
* ���������:
*    �������� ����� \a InterChannel_getLocalData.
*
* ������ 1.0.4
* ����   03-08-2016
* �����  ������� �.�.
* 
* ���������:
*    1. ��� ������� �� �������������, ���� ������������� (eSynchronized)
*    �����������. � ��� ��������� ������ �� ��������� ������ ���. 
*    ����������: ��� ��������� ������ �� ��������� ������, 
*    ���� ������������� ��� �� ������������.
*    2. ������������� �� ��������� ������ (eScriptChVal) ��������� 
*    ��������� ������ �����, �������� ������ �� ������ (eRemote), 
*    ��� ������ �����, ������ �������� � �������� ������������� 
*    (eLocalAck && eLocal). 
*    ��������� ����� �������������� � �����������, � ��������� � main, 
*    ����� ��������� ��������. ����� ������ ������� �� ������ (eRemote), 
*    ��������������� ������ �� ��������� (updateSet64), � �� ���������� �����
*    ������� � main ����� ��������� ������ �� �������� (eLocal). ����� 
*    ���������� ��������� ����� ������������ (eLocalAck && eLocal && eRemote),
*    � ��������������� ���� ������������� (eSynchronized). 
*    � ����� ��������� ������������� ��������� ���� ������������� (eLocalAck),
*    ��� ���, ��� ����� ������� (eLocal) �������. ������ �������� �������������.
* 
* ����������: 
*    ��� ���������� �������� � ������� ��������� �����������  
*    �������������� ������ ������, � � ������ �� ���������� �������  
*    " ������ (eRemote) ��� ������ ��� (eLocalAck) � (eLocal) ", ������������ 
*    ���� ������ (eRemote), ��������� �� ������� ������ �� ���������  
*    (updateSet64), �� ���� ������������� (eSynchronized) �� ���������������.
*    ������������� ���������� ��� ��������� ������������� (eLocalAck).
*
* ������ 1.0.5
* ����   09-09-2017
* �����  ������� �.�.
* 
* ��������:
*    ���� ����������, ��� ��� ������ �������  InterChannel_updateData
*    �� �������� eScriptChVal, ����� ���������� ��������. ��������, ����� ��������
*    ��������� ���� �������� ������������� � ������. ��� ������� �������� ��� ��������
*    ���������  ����, ��� ������ ������ � ���������.  InterChannel_updateData ��������
*    �� ������������, ����������� �� �������� (��������� �� ������ �� ���� ������ ������
*    ��������) � ��� ������� ����������� ����������� � ������� ���������� ������ �� ������
*    � ��������������� ��������������� �������. �� ����� �������� ���������� 
*    InterChannel_updateData ���������� ��������� � �������� ������� ������� ������ �� ������.
*    ��� �������������� ������� ������� ���������� ��������� ���������� ������ ������ �������
*    ����� ��������.
* 
* ���������:
*    1. � ����������� �� ������������� ��, ��� ������� InterChannel_runCommunication
*    ����������� �� ������, ������� ����� ������������� � ����������� ��� ������ �������
*    sysLock( ), sysUnLock( );
*    2. ��� ���������� ������������ ���� ������� �� ����� drv\MainRegisters.h;
*    3. �������, ������� ����� ���� �������� ������� ������, ���� ������ ��������, 
*    �������� ����� ������ ��� ������ sysLock( ), sysUnLock( ) � �������� ATOMIC_CAN_TIMER.
*
* ������ 1.0.6
* ����   27-12-2017
* �����  ������� �.�.
*
* ��������:
*    1. ������� InterChannel_getData �������� ������� InterChannel_updateData, 
*    ������� ��������� �������� ������� ������, �������������� � �������������, ���� �������
*    �������, �������������� ��. ����� � ���� ����������� �������� ��������� ������ 
*    ������������, ����� ���������� ��� ��� � ����� ������ ������ ����� ������ � 
*    �������������, � � ������ ���. ��� �������� � ����, ��� � ���� ������� ������ �����
*    ���������� �� ���� ������������. ������ ������� ���������� � �� ������� InterChannel_run,
*    ������� ��� ������� ���������� �� �� ���������� ����� �������� � ����������� ������. 
*    ���������� ������ ����� InterChannel_updateData �� InterChannel_getData.
*    2. � ����-� ��������� ������� �������� ������� �������, ������� InterChannel_run 
*    ���������� �� �������� ������, ������� � ��� ���������� �������� ��������������� ������.
* 
* ���������:
*    1. ������ ����� ������� InterChannel_updateData �� InterChannel_getData.
*    2. � ������� InterChannel_run �������� ������ ������� �������� �������
*    MARKED_CALL_FUNCTION.
*    3. ������ ������ ��� �������� ������ �������� � 8-�� �� 6-��, ���������
*    �� 1-�� ��� ���������� ��� ��������� � ���������������, ������� �������� 6 ����.
* 
* ������ 1.0.7
* ����   26-06-2017
* �����  ������� �.�.
*
* ��������:
*    1. � ������� InterChannel_verification ��� ������������� ��������� 
*    ����������� eProcCheckDeltaP ��� ��������� ���������������� ���������� 
*    �� ����� ������� ������ ����, � ��� �� ��� ������� � �������� �������� 
*    ��������� �������� ���� ��� ��������� �������� � �������� �����������
*    ������� �� ���� � ��������� ����������� ��� �������������.
* 
* ���������:
*    1. � ������� InterChannel_verification ��� ������������� ��������� 
*    ����������� eProcCheckDeltaP ������������ �������� �������� ��������� � 
*    ������� �������� ���� ������� �� ����. 
* 
* ������ 1.0.8
* ����   04-07-2018
* �����  ������� �.�.
* 
* ��������:
*    � ����� ������ ������� ���������� ���������� �� ������������� ��������� 
*    (����� ������ �� ������ ��� ������������� ��������) ����������� � 
*    ���������� �� ������� � InterChannel_run � �������� �������� 
*    ������������������ ��� �� ���� ����� ��������� ������. � � ������ ������ 
*    ����������� �����, � ������������� ��������� �� ���� ����� �� �����������. 
*    ��� ���������� ����������� �� ����. � ���� �����, � ������� ��������� 
*    �������������, �� ��������� ����� ���� ����� ��������, �� � ��������� 
*    ������ ��� � �� ����������� �����������. � ������ ������ ���� �������
*    �������������
* 
* ���������:
*    �������� ��������� � ������ ������ ������� ����� �������� ������� 
*    � ������� ������������� (InterChannel_run � InterChannel_runCommunication).
*    ��������� ������� InterChannel_runDrv, ������� ��������� ��������� ����� 
*    ����� ��� ������ ������� �������� run � ������ ��������� � ������ 
*    ������������� ������� ��� ����, ��� �� � ����� ������� ��������� ����� �����
*    ������������ � ���� ������ �������. � ����� ������� 
*    ������� InterChannel_runCommunication, ������� �������� � ����������� ������
*    ������������� ������, ������������ ������ �� ���������, ������� ���� 
*    ������������� ������� InterChannel_runDrv. ��� ���� ������� 
*    InterChannel_runCommunication �� �������� ������� �������������� ����� 
*    ������������� ������ � ������ ������������� ������, � 
*    �������� ������, �������� �� ��������� ������, � �������������� ���������, 
*    ��� ������� ���� �������� ������������� ������, � 
*    ����������� ��������� \a interChannelStorageRemoteData 
*    � \a interChannelStorageAck. ������� InterChannel_run ������ ���������� 
*    ������ � ������ ������ �����, ��� ��������� ����� ������������� ������, 
*    ���������� ������ �� �������� � ������ ������������ ��. 
*    ����� �������, ��� ������ ���������� � ���������� �� ���������� �����
*    ����� ���������� �� ���������.
*
* ������ 1.0.9
* ����   04-07-2018
* �����  ������� �.�.
* 
* ��������: 
*     ���������� ���� �� ��������� � ���� 1 ��.
* ���������:
*     1) valToPercent ������� �� __builtin_divud( __builtin_muluu(..., ...), ...);
*
* ������ 1.0.10
* ����   23-05-2019
* �����  ������� �.�. 
*
* ���������:
*    1) ������� �������������� ���� m_checkErrorCount �� ��������� InterChannelParamDump.
*    2) �������:
*       � ����������� ���� InterChannelTrPhase
*       � �������� trPhase �� ��������� ������� void InterChannel_ctor( const void * drvPtr,  
*       InterChannelTrPhase trPhase );
*       � ��������� currentPhase, setPhase �� ��������� InterChannelData.
*    3) � ������� InterChannel_ctor ��������� ������������� ���� 
*    interChannelData.paramDump[i].m_settings.m_middleRangeCheck.
*    4) ��������� �� ������ InterChannel_updateData � ������������ ������� ���������� 
*    ���� ��������� � 1.0.6 27-12-2017.
*    5) � ������� InterChannel_setParamSettings �������� �������� �������� ������� �1. ��� �������� 
*    ������������� eScriptSync ��� ������ ���� ������ �����������. � ��� ��������� ������ ���� ����� ����.
*    6) � ������� InterChannel_setRemote ��������� ��������, ���������� ������� �� ������������� �� 
*    ��������� ������. ������ ���� �� ��������� ������ ������ ��������, ������� ��� ��������, �� ��� ��
*    ��� ���������, ���������� ����� � ������ eGrPS_InterChannel � ����� ePS_IntChRepeat.
*    7) ��������� ������������� eProcSyncEqual, ������� ����������� ���� ��������� ���������� ���������
*    ��������, ����������� ���������� operatingValue � remoteValue �������� ���������� localValue. 
*    ������ �� �����, ��������� ������������� eProcSyncEqual �� ����� ���� ������������ �������� �� 
*    ��������� �������� eProcCheckEqual. ��� �������������� ����� � ������� InterChannel_setParamSettings 
*    �������� �������� ����, ��� ��� ������������� ��������� ������������� eProcSyncEqual ��������� ��������
*    ������� eProcCheckEqual. � ������ ������������ ����� ������� ������ ����� ��������� � ��.
*    8) ��������� ���������������� InterChannel_stopTime1, InterChannel_stopTime2, InterChannel_stopTime3, 
*    InterChannel_time1IsRunning, InterChannel_time2IsRunning, InterChannel_time3IsRunning 
*    � InterChannel_stopTimeForAllId.
*    9) ��-�� ��������� � ��������� ��������� ArrayIoDriver ������� �������� ������� 
*    InterChannel_runCommunication( const ArrayIoDriver *drv ) �� InterChannel_runCommunication( void ),
*    ��������� ������ ������� ������� � ������ ���������� � ����� ���������.
*    ����� ������� �������� ������� void InterChannel_ctor( const void *drvPtr ) �� 
*    void InterChannel_ctor( const ArrayIoDriver *drvPtr ), � � ���� drvPtr ��������� InterChannelData
*    ������� ��� � const void * �� const ArrayIoDriver *.
*
* ������ 1.0.11
* ����   08-07-2019
* �����  ������� �.�. 
*
* ��������: 
*    1) ��������� ������, ��������� � ����������� ������ ������������� ������, ��������������
*    ������� ��� � ������ ������ ����� �������� ������. � ���� ������� � ����� ������, ���� �� 
*    ����� ������ ��� ������������� ������, ��������� �� ���� ���������� ������ ��� �����, ��� ��
*    ������ ������. ����� ���������, ��� � ����� ������ �������� �� ��������������� �� ������� �����,
*    � �� ������ �� ���������������� ������ �� ���������.
*
* ���������:
*    1) ��� ���������� ������ �������� ���� ������� ����������� �� �������� ������ �����
*    ������� ������ �����. ����������� ��������� �������� ������ � CAN ����, ���� �� ������ 
*    ������ ����� �������� ������� ������ ��� �� �������� ���� ���������. ���� ��������� � �����
*    �������, ������ � ������. ������ ����������� ��������� ���������� ������������ ������� ������
*    ������ � ������������� ������, � ������, � ������ �������� ����� � ����� ������� ����� 
*    ���������� ���������� ������� � ������.
*    � ������ ������� ��������� ���������:
*        - � ������� InterChannel_runCommunication �������� �������� transmitIsEnabled, ������� 
*    ��������� ��� ��������� ������ ������ �� ���� CAN.
* 
* ������ 1.0.12
* ����   23-08-2019
* �����  ��������� �.�. 
*
* ���������:
*  ������ ���� �� ���������� ������. ������� ��� ������ ������������� � ������ �������. 
* 
* ������ 1.0.13
* ����   13-09-2019
* �����  ������� �.�. 
*
* ��������: 
*    1) ��� ���������� �������� eScriptTransmit ����������� ������� �� �����
*    ����������, ��� ������ ����� ������. ������� InterChannel_getData ������ 
*    ���������� ��������� ������������������ ��������, ���� ���������� ���������, 
*    ������� ���������� ��������, �� ������ ���������. ������� 
*    InterChannel_isSynchronized ������������ ��� ����������� ������ ������� 
*    �� ������ ��� �� ��������� ������. ��������� ����������� ������� �� ���� 
*    ������, � ��� ������ ���� ������������ ��������� �� ���������� ����� �������� 
*    ������, ����� ���� ����� ���������, � ��� ��� ����������� ����������, ����� 
*    ������ ����� ������.
*
* ���������:
*    1) ����� �� �������� ������������ ������ ��� ���������� ������ �������� � 
*    ��� �������� ��� ������� InterChannel_syncEvent � 
*    InterChannel_syncEventOverflow - ������� ������������� � ������� 
*    ������������ ������� �������������. ������� ������������� ��������������� 
*    ������������ � ��������� ��������� �������������, �� ������������ ��� ������ 
*    ���� �������. ����� �������, ���������������� �� ����� ���������� ��� �������,
*     � ����� ��� ��������� ���������� ������ ��� ������ InterChannel_getData. 
*    ������� ������������ ������� ������������� ������ ��� ����, ��� �� ����� ���� 
*    �����������������, ��� �� ���� ����������� ������. ������ ������� ���������, 
*    ���� ���������� ��������� ������� ����� ���������� ��������� �����������������
*    �� � ������� InterChannel_syncEvent. ��� ������ ������ ������� ������������.
* 
* 
* ������ 1.0.14
* ����   21-01-2021
* �����  ��������� �. �. 
*
* ��������: 
*    1) ��� ���������� �������� eScriptChVal � ������� InterChannel_synchChangeValue
*       ���������� ������ ������� ������ ���������� ������� ����� ��������� ���� ������ � 
*       ����������. � ������ 1.0.13 ���� ������ ���� ���������, � ��� ��������� ��������� ��������.
* ���������:
*    �� ���� ������ �������� �����, � ����� ����� ���������� ���������. 
* 
* ������ 1.0.15
* ����   04-02-2021
* �����  ��������� �.�.
*
* ��������: 
*       ��� ���������� �������� eScriptChVal �������� ������� � ��.
*      ������� � ������� ��������� ����� ��������.
* ���������: ������� ��������.
* �������� ������� � ��������� "�������� ��������� InterChannel ������ 1.0.15"    
* 
*/
