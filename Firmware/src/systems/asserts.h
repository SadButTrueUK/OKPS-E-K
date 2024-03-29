/**
* \file    asserts.h
* \brief   ����������� ����������� (assert)
* \details ���������
* \details ����������� (assert) � ��� �����������, ������� ����� ���� ������������ 
* � ����������, ���������� �� ������ C � C++, � ����� ��������� ���������
* ���� ������������� � ���������. 
* \details ������ ����������� �������� ���������� ���������, �������, �� ������ 
* ������������, ����� ������ � ������ ����������. � ��������� ������, �����
* ������� ������� ��������� ����������. ��������, ��� ���������� ��������� 
* �� ����� ���� �����, ����������� (assert) ������������ ������������� 
* (��������) � ��������� ���������, ���������� ����������� � ���, ��� 
* ��������� �� �������� ������.
* \details ��������� DISABLED_ASSERT ��������� �������� �����������.
* \details ��������� DEBUG_EXCEPTION ��������� ����� ������� �������� � 
* �� � �������� ���������� �������.
*
* \version 1.0.2
* \date    09-07-2018
* \author  ������� �.�.
*/

//*****************************************************************************
// ������� ������������� ��� �������������� ���������� ��������� ����������� �����
//*****************************************************************************
#ifndef ASSERTS_H
#define ASSERTS_H

//*****************************************************************************
// ������������ �����
//*****************************************************************************
#include <stdint.h>

//#define DISABLED_ASSERT
//*****************************************************************************
// ���������� ���������, ������������ ����� �������
//*****************************************************************************

//*****************************************************************************
#define ASSERT_DEF_CODE    0xFF        ///< ��� ������ � ������ ������ \a ASSERT ��� \a ERROR ��� ��������������.

//*****************************************************************************
/// \brief ��������, �� ������� ������������� ������� ��� �������� \a CASSERT � \a CASSERT_ID.
/// \note � ������, ����� ����������� ������� false.
///
#define CASSERT_INC        2

//*****************************************************************************
/// \brief ��������, �� ������� ����������� ������� ��� �������� \a CASSERT � \a CASSERT_ID.
/// \note � ������, ����� ����������� ������� true. 
///
#define CASSERT_DEC        1

//*****************************************************************************
// ������� ���� �������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ���� ������, ��������� ��� ������ � �������������.
/// \details ������ ���������� ��� ������, ������� � ������� ����� �������� ���, � � ������� - �������������.
/// \param group_ - ��� ������;
/// \param id_    - ������������� ������.
///
#define SET_CODE_ID( group_, id_ )   ( ( ( group_ ) << 8 ) + ( ( id_ ) &0x00FF ) )

//*****************************************************************************
// ����������������, ����������� �����������
//*****************************************************************************

#ifdef DONT_SAVE_LINE_NUMBER        // ����� ������� �������� � �� 

    //*****************************************************************************
    // ���������� ���������, ������������ ����� �������
    //*****************************************************************************
    
    //*****************************************************************************
    /// \brief �������� ������ ������ ��������� ����.
    /// \details � ������, ����� ����������� �� �����������, � ������� ��������� 
    /// ���������� #sysAssertException � �������� ��������� ���������� ��� 
    /// ����� � ����� ������. ��� ������������� ������ ������, ��� �������� 
    /// ����������� �� ������ ������ ����� �������� ��������� ����. 
    /// ������ �������� ������������ ����� ��������� ���������� �����
    /// �� ��������� (� ������ ��������� ������������ ��� ������ �����) � ����� ������ �������� �� ������������.
    /// \note �������� �������� ����� ��������� � ��������� 
    /// ������� ��������� \a DONT_SAVE_LINE_NUMBER. � ���� ������, ��� 
    /// ���������� ����� ���������� �������������� �The project is compiled 
    /// with the directive DONT_SAVE_LINE_NUMBER�.
    /// \note ��������� ������ �������������� �The project is compiled with 
    /// the directive DONT_SAVE_LINE_NUMBER� ����� ��������� � ��������� 
    /// ������� ��������� \a IGNORE_DONT_SAVE_LINE_NUMBER_WARNING.
    ///
    #define LINE_NUMBER 0

#ifndef IGNORE_DONT_SAVE_LINE_NUMBER_WARNING
    #warning "The project is compiled with the directive DONT_SAVE_LINE_NUMBER"
#endif

#else

    //*****************************************************************************
    // ���������� ���������, ������������ ����� �������
    //*****************************************************************************
    
    //*****************************************************************************
    /// \brief ����� ������ ��������� ����, �� ������� ����������� ����������� 
    /// �����������.
    /// \details � ������, ����� ����������� �� �����������, � ������� ��������� 
    /// ���������� #sysAssertException � �������� ��������� ���������� ��� 
    /// ����� � ����� ������. ����� ������ ������������ ���� ��������.
    /// \note ��� ������������� ������ ������, ��� �������� ����������� �� 
    /// ������ ������ ����� �������� ��������� ����. 
    /// \note ��������� �������� ������ ������ ����� ��������� � ��������� 
    /// ������� ��������� \a DONT_SAVE_LINE_NUMBER. � ���� ������, ��� 
    /// ���������� ����� ���������� �������������� �The project is compiled 
    /// with the directive DONT_SAVE_LINE_NUMBER�.
    /// \note ��������� ������ �������������� �The project is compiled with 
    /// the directive DONT_SAVE_LINE_NUMBER� ����� ��������� � ��������� 
    /// ������� ��������� \a IGNORE_DONT_SAVE_LINE_NUMBER_WARNING.
    ///
    #define LINE_NUMBER __LINE__

#endif

//*****************************************************************************
/* ���� ���������� DISABLED_ASSERT - �������� ���������� ���������, ������� ������ */
#ifdef DISABLED_ASSERT   

    #warning "The project is compiled with the directive DISABLED_ASSERT"
    
    //*****************************************************************************
    // ������� ���� �������
    //*****************************************************************************
    
    //*****************************************************************************
    #define ERROR_ID( group_, id_ )                                ( ( void )0 )
    #define ERROR( )                                               ( ( void )0 )
    #define ASSERT_ID( group_, id_, test_ )                        ( ( void )0 )
    #define ASSERT( test_ )                                        ( ( void )0 )
    
    //*****************************************************************************
    #define CASSERT_ID( group_, id_, counter_, maxcount_, test_ )  ( ( void )0 )
    #define CASSERT( counter_, maxcount_, test_ )                  ( ( void )0 )
    
    //*****************************************************************************
    #define DERROR_ID( group_, id_ )                               ( ( void )0 )
    #define DERROR( )                                              ( ( void )0 )
    #define DASSERT_ID( group_, id_, test_ )                       ( ( void )0 )
    #define DASSERT( test_ )                                       ( ( void )0 )
    #define DCASSERT_ID( group_, id_, counter_, maxcount_, test_ ) ( ( void )0 )
    #define DCASSERT( counter_, maxcount_, test_ )                 ( ( void )0 )

#else  // ���� ENABLED_ASSERT �� ���������� �������� ����������� ���������

    //*****************************************************************************
    // ������� ���� �������
    //*****************************************************************************
    
    //*****************************************************************************
    /// \brief ������, � �������������� ���� � ��������������.
    /// \details ������ ��������� ����� ������� #sysAssertException. 
    /// \note � ������� #sysAssertException � ���������� ���������� ��� �����, 
    /// � ������� ����������� ����������� � ����� ������ � ������������ ������������.
    /// \note ���� ���������� ��������� \a DISABLED_ASSERT, �������� ����������� 
    /// �� �����������.
    /// \param group_ - ��� ������;
    /// \param id_    - ������������� ������.
    ///
    
    //*****************************************************************************
    #define ERROR_ID( group_, id_ ) \
              sysAssertException( __FILE__, LINE_NUMBER, ( group_ ), ( id_ ) )
    
    /// \brief ������.
    /// \details ������ �������� #ERROR_ID c
    /// ����������� \a group_ � \a id_, ������� #ASSERT_DEF_CODE.
    /// \note ���� ���������� ��������� \a DISABLED_ASSERT, �������� ����������� 
    /// �� �����������.
    ///
    #define ERROR( )    ERROR_ID( ASSERT_DEF_CODE, ASSERT_DEF_CODE )
    
    //*****************************************************************************
    /// \brief �������� ����������� � �������������� ���� � ��������������
    /// ����������.
    /// \details ������ ���������, ��� ������� \a test_ �����������.
    /// \details ���� �������� \a test_ ����� �������� false, ���������� ������ #ERROR_ID. 
    /// \note ���� ���������� ��������� \a DISABLED_ASSERT, �������� ����������� 
    /// �� �����������.
    /// \param group_ - ��� ������;
    /// \param id_    - ������������� ������;
    /// \param test_  - ����������� �������.
    ///
    #define ASSERT_ID( group_, id_, test_ )                        \
        do                                                         \
        {                                                          \
            if( !( test_ ) )                                       \
            {                                                      \
                ERROR_ID( ( group_ ), ( id_ ) );                   \
            }                                                      \
        } while( 0 )
    
    //*****************************************************************************
    /// \brief �������� �����������.
    /// \details ������ �������� #ASSERT_ID c
    /// ����������� \a group_ � \a id_, ������� #ASSERT_DEF_CODE.
    /// \note ���� ���������� ��������� \a DISABLED_ASSERT, �������� ����������� 
    /// �� �����������.
    /// \param test_ - ����������� �������.
    ///
    #define ASSERT( test_ ) ASSERT_ID( ASSERT_DEF_CODE, ASSERT_DEF_CODE, test_ ) 
    
    //*****************************************************************************
    /// \brief �������� ����������� �� ���������, � �������������� ���� 
    /// � �������������� ����������.
    /// \details ������ ���������, ��� ������� \a test_ �����������. 
    /// \note ���� �������� \a test_ ����� �������� false, ������� \a counter_ ������������� ��
    /// �������� #CASSERT_INC. 
    /// \note ���� ������� ������ ��� �������� ��������
    /// \a maxcount_, ���������� ������ #ERROR_ID.
    /// \note ���� �������� \a test_ ����� �������� true, ������� \a counter_
    /// ����������� �� �������� #CASSERT_DEC �� ����. 
    /// \note ������������� �������� ��������� ��������� ������ 
    /// ��������� ������������.
    /// \param group_    - ��� ������;
    /// \param id_       - ������������� ������;
    /// \param counter_  - �������;
    /// \param maxcount_ - ����������� ���������� �������� ��������;
    /// \param test_     - ����������� �������.
    ///
    #define CASSERT_ID( group_, id_, counter_, maxcount_, test_ )     \
        do                                                            \
        {                                                             \
            if( !( test_ ) )                                          \
            {                                                         \
                if( ( ( counter_ ) += CASSERT_INC ) >= maxcount_ )    \
                {                                                     \
                    ERROR_ID( ( group_ ), ( id_ ) );                  \
                    ( counter_ ) = ( maxcount_ );                     \
                }                                                     \
            }                                                         \
            else                                                      \
            {                                                         \
                if( ( counter_ ) > 0 )                                \
                {                                                     \
                    if( ( counter_ ) >= CASSERT_DEC )                 \
                    {                                                 \
                        ( counter_ ) -= CASSERT_DEC;                  \
                    }                                                 \
                    else                                              \
                    {                                                 \
                        ( counter_ ) = 0;                             \
                    }                                                 \
                }                                                     \
            }                                                         \
        } while( 0 )
    
    //*****************************************************************************
    /// \brief �������� ����������� �� ���������.
    /// \details ������ �������� #CASSERT_ID c
    /// ����������� \a group_ � \a id_, ������� #ASSERT_DEF_CODE.
    /// \note ���� ���������� ��������� \a DISABLED_ASSERT, �������� ����������� 
    /// �� �����������.
    /// \param counter_  - �������;
    /// \param maxcount_ - ����������� ���������� �������� ��������;
    /// \param test_     - ����������� �������.
    ///
    #define CASSERT( counter_, maxcount_, test_ ) \
            CASSERT_ID( ASSERT_DEF_CODE, ASSERT_DEF_CODE, counter_, maxcount_, test_ ) 

// �������������� �������� �������, ������� ����������� ��� ������ 
// ��������� ENABLED_DEBUG_ASSERT.
// \note ������������ ��� ���������� ������������ � �������
//
#ifdef ENABLED_DEBUG_ASSERT

    #warning "The project is compiled with the directive ENABLED_DEBUG_ASSERT"

    #define DERROR_ID( group_, id_ )   ERROR_ID( group_, id_ )
    
    #define DERROR( )                 ERROR( )
    
    #define DASSERT_ID( group_, id_, test_ )  ASSERT_ID( ( group_ ), ( id_ ), ( test_ ) )
    
    #define DASSERT( test_ )     ASSERT( ( test_ ) )
    
    #define DCASSERT_ID( group_, id_, counter_, maxcount_, test_ ) \
                CASSERT_ID( ( group_ ), ( id_ ), ( counter_ ), ( maxcount_ ), ( test_ ) )
    
    #define DCASSERT( counter_, maxcount_, test_) \
                CASSERT( ( counter_ ), ( maxcount_ ), ( test_ ) ) 
    
#else                               // ENABLED_DEBUG_ASSERT

    //*****************************************************************************
    // ���������� ���������, ������������ ����� �������
    //*****************************************************************************
    
    //*****************************************************************************
    /// \brief �������������� ����������������: ������ � �������������� ���� � 
    /// �������������� ����������.
    /// \note ������������ #ERROR_ID, ����������� ��� ������ ���������
    /// \a ENABLED_DEBUG_ASSERT).
    ///
    #define DERROR_ID( group_, id_ )                               ( ( void )0 )
    
    //*****************************************************************************
    /// \brief �������������� ����������������: ������.
    /// \note ������������ #ERROR, ����������� ��� ������ ���������
    /// \a ENABLED_DEBUG_ASSERT).
    ///
    #define DERROR( )                                               ( ( void )0 )
    
    //*****************************************************************************
    /// \brief �������������� ����������������, ����������� �������.
    /// \note ������������ #ASSERT_ID, ����������� ��� ������ ���������
    /// \a ENABLED_DEBUG_ASSERT).
    ///
    #define DASSERT_ID( group_, id_, test_ )                       ( ( void )0 )
    
    //*****************************************************************************
    /// \brief �������������� ����������������, ����������� �������.
    /// \note ������������ #ASSERT, ����������� ��� ������ ���������
    /// \a ENABLED_DEBUG_ASSERT).
    ///
    #define DASSERT( test_ )                                       ( ( void )0 )
    
    //*****************************************************************************
    /// \brief �������������� ���������������� 
    /// \note ���������� #CASSERT_ID, ����������� ��� ������ ���������
    /// \a ENABLED_DEBUG_ASSERT).
    ///
    #define DCASSERT_ID( group_, id_, counter_, maxcount_, test_ ) ( ( void )0 )
    
    //*****************************************************************************
    /// \brief �������������� ���������������� 
    /// \note ���������� #CASSERT, ����������� ��� ������ ���������
    /// \a ENABLED_DEBUG_ASSERT).
    ///
    #define DCASSERT( counter_, maxcount_, test_)                 ( ( void )0 )

#endif                                            // ENABLED_DEBUG_ASSERT == 1 #else

#endif                                            // DISABLED_ASSERT #else

#ifdef DEBUG_EXCEPTION // ����� ������� �������� � ��

    #warning "The project is compiled with the directive DEBUG_EXCEPTION"
    
    //*****************************************************************************
    // ����������� ���������� ����������
    //*****************************************************************************
    
    //*****************************************************************************
    // ��� ������, ������� ��������� � �������. 
    // ��� ����� � ����� � ����� ����� ���� �� fail-����
    extern uint16_t exceptionExpectedErrorCode;
    
    //*****************************************************************************
    // ������� ����, ��� ����� � ��������� ����� ���������. 
    // ��� ������������ ������ � �����, �������� �� ����������, 
    // ���� ������������.
    extern BOOL exceptionErrorCodeIsExpected;

#endif

//*****************************************************************************
// ��������� ������������ �������
//*****************************************************************************

//*****************************************************************************
/// \brief ��������� ����������.
/// \param file  � ��������� �� ��� �����, � ������� ���� ���������� ������������
/// ��������� �������;
/// \param line  � ����� ������, � ������� ���� ���������� ������������ 
/// ��������� ������� ��� ��� � ������������� ����������;
/// \param group � ��� ������;
/// \param id    � ������������� ������.
/// \note ���������� ����� ����������� ����������� �������
/// � ��������� ������ � ��.
/// \note ��� ���������� ���������� ������������ ������ ������� ����� 
/// ���� ��������������.
///
void sysAssertException( char const *const file,
                         int16_t           line,
                         int16_t           group,
                         int16_t           id );

#endif

//*****************************************************************************
/**
* ������� ���������:
* 
* ������ 1.0.1
* ����   09-02-2016
* �����  ������� �.�.
* 
* ���������:
*    ������� ������.
* 
* ������ 1.0.2
* ����   09-07-2018
* �����  ������� �.�.
* 
* ���������:
*     ��������� ����������� ��������� �������� � ������� ��������� ���������� 
*     \a sysAssertException ������ ������, �� ������� ����������� �������� 
*     ����������� ��������� ����������.
*     ���� �������� ������ ������ ������������ ��� ������ ������� 
*     \a LINE_NUMBER, ������� ���������� ���������� ����� ������ ��� ������ 
*     ���� ���������� ����. �� ��������� ���������� ����� ������. ��� ����, 
*     ����� ������ ������ ������������ �������� ����, ���������� � ��������� 
*     ������� ���������� ��������� \a DONT_SAVE_LINE_NUMBER. � ���� ������, ��� 
*     ���������� ����� ���������� �������������� �The project is compiled with the 
*     directive DONT_SAVE_LINE_NUMBER�.
*     ��������� ������ �������������� �The project is compiled with 
*     the directive DONT_SAVE_LINE_NUMBER� ����� ��������� � ��������� 
*     ������� ��������� \a IGNORE_DONT_SAVE_LINE_NUMBER_WARNING.
*/
