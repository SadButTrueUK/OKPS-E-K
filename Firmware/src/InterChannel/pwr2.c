/**
* \file    pwr2.c
* \brief   Определение таблиц Pwr2_pwr2Lkup[], Pwr2_invPwr2Lkup[], and Pwr2_div8Lkup[]
* \details Таблицы используются очередями с приоритетом set.h
*
* \version 1.0.1
* \date    12-06-2013
* \author  Годунок А.Н.
*/

//*****************************************************************************
// Подключаемые файлы
//*****************************************************************************
#include <stdint.h>

//*****************************************************************************
// Объявление локальных типизированных констант
//*****************************************************************************

//*****************************************************************************
/// \brief Таблица поиска для \f$\left(1 \ll \frac{n-1}{8}\right)\f$, где \a n - индекс в таблицу.
/// \note Диапазон индексов \a n = 0..64. Первый индекс ( n == 0 ) никогда не должен использоваться.
///
uint8_t const Pwr2_pwr2Lkup[65] =
{
    ( uint8_t )0x00, /* неиспользуемое значение */
    ( uint8_t )0x01, ( uint8_t )0x02, ( uint8_t )0x04, ( uint8_t )0x08,
    ( uint8_t )0x10, ( uint8_t )0x20, ( uint8_t )0x40, ( uint8_t )0x80,
    ( uint8_t )0x01, ( uint8_t )0x02, ( uint8_t )0x04, ( uint8_t )0x08,
    ( uint8_t )0x10, ( uint8_t )0x20, ( uint8_t )0x40, ( uint8_t )0x80,
    ( uint8_t )0x01, ( uint8_t )0x02, ( uint8_t )0x04, ( uint8_t )0x08,
    ( uint8_t )0x10, ( uint8_t )0x20, ( uint8_t )0x40, ( uint8_t )0x80,
    ( uint8_t )0x01, ( uint8_t )0x02, ( uint8_t )0x04, ( uint8_t )0x08,
    ( uint8_t )0x10, ( uint8_t )0x20, ( uint8_t )0x40, ( uint8_t )0x80,
    ( uint8_t )0x01, ( uint8_t )0x02, ( uint8_t )0x04, ( uint8_t )0x08,
    ( uint8_t )0x10, ( uint8_t )0x20, ( uint8_t )0x40, ( uint8_t )0x80,
    ( uint8_t )0x01, ( uint8_t )0x02, ( uint8_t )0x04, ( uint8_t )0x08,
    ( uint8_t )0x10, ( uint8_t )0x20, ( uint8_t )0x40, ( uint8_t )0x80,
    ( uint8_t )0x01, ( uint8_t )0x02, ( uint8_t )0x04, ( uint8_t )0x08,
    ( uint8_t )0x10, ( uint8_t )0x20, ( uint8_t )0x40, ( uint8_t )0x80,
    ( uint8_t )0x01, ( uint8_t )0x02, ( uint8_t )0x04, ( uint8_t )0x08,
    ( uint8_t )0x10, ( uint8_t )0x20, ( uint8_t )0x40, ( uint8_t )0x80
};

//*****************************************************************************
/// \brief Таблица поиска для \f$\left(\sim\left(1 \ll \frac{n-1}{8}\right)\right)\f$, где \a n - индекс в таблицу.
/// \note Диапазон индексов \a n = 0..64. Первый индекс ( n == 0 ) никогда не должен использоваться.
///
uint8_t const Pwr2_invPwr2Lkup[65] =
{
    ( uint8_t )0xFF, /* неиспользуемое значение */
    ( uint8_t )0xFE, ( uint8_t )0xFD, ( uint8_t )0xFB, ( uint8_t )0xF7,
    ( uint8_t )0xEF, ( uint8_t )0xDF, ( uint8_t )0xBF, ( uint8_t )0x7F,
    ( uint8_t )0xFE, ( uint8_t )0xFD, ( uint8_t )0xFB, ( uint8_t )0xF7,
    ( uint8_t )0xEF, ( uint8_t )0xDF, ( uint8_t )0xBF, ( uint8_t )0x7F,
    ( uint8_t )0xFE, ( uint8_t )0xFD, ( uint8_t )0xFB, ( uint8_t )0xF7,
    ( uint8_t )0xEF, ( uint8_t )0xDF, ( uint8_t )0xBF, ( uint8_t )0x7F,
    ( uint8_t )0xFE, ( uint8_t )0xFD, ( uint8_t )0xFB, ( uint8_t )0xF7,
    ( uint8_t )0xEF, ( uint8_t )0xDF, ( uint8_t )0xBF, ( uint8_t )0x7F,
    ( uint8_t )0xFE, ( uint8_t )0xFD, ( uint8_t )0xFB, ( uint8_t )0xF7,
    ( uint8_t )0xEF, ( uint8_t )0xDF, ( uint8_t )0xBF, ( uint8_t )0x7F,
    ( uint8_t )0xFE, ( uint8_t )0xFD, ( uint8_t )0xFB, ( uint8_t )0xF7,
    ( uint8_t )0xEF, ( uint8_t )0xDF, ( uint8_t )0xBF, ( uint8_t )0x7F,
    ( uint8_t )0xFE, ( uint8_t )0xFD, ( uint8_t )0xFB, ( uint8_t )0xF7,
    ( uint8_t )0xEF, ( uint8_t )0xDF, ( uint8_t )0xBF, ( uint8_t )0x7F,
    ( uint8_t )0xFE, ( uint8_t )0xFD, ( uint8_t )0xFB, ( uint8_t )0xF7,
    ( uint8_t )0xEF, ( uint8_t )0xDF, ( uint8_t )0xBF, ( uint8_t )0x7F
};

//*****************************************************************************
/// \brief Таблица поиска для \f$\frac{n-1}{8}\f$.
/// \note Диапазон индексов \a n = 0..64. Первый индекс ( n == 0 ) никогда не должен использоваться.
///
uint8_t const Pwr2_div8Lkup[65] =
{
    ( uint8_t )0, /* неиспользуемое значение */
    ( uint8_t )0, ( uint8_t )0, ( uint8_t )0, ( uint8_t )0,
    ( uint8_t )0, ( uint8_t )0, ( uint8_t )0, ( uint8_t )0,
    ( uint8_t )1, ( uint8_t )1, ( uint8_t )1, ( uint8_t )1,
    ( uint8_t )1, ( uint8_t )1, ( uint8_t )1, ( uint8_t )1,
    ( uint8_t )2, ( uint8_t )2, ( uint8_t )2, ( uint8_t )2,
    ( uint8_t )2, ( uint8_t )2, ( uint8_t )2, ( uint8_t )2,
    ( uint8_t )3, ( uint8_t )3, ( uint8_t )3, ( uint8_t )3,
    ( uint8_t )3, ( uint8_t )3, ( uint8_t )3, ( uint8_t )3,
    ( uint8_t )4, ( uint8_t )4, ( uint8_t )4, ( uint8_t )4,
    ( uint8_t )4, ( uint8_t )4, ( uint8_t )4, ( uint8_t )4,
    ( uint8_t )5, ( uint8_t )5, ( uint8_t )5, ( uint8_t )5,
    ( uint8_t )5, ( uint8_t )5, ( uint8_t )5, ( uint8_t )5,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7
};

//*****************************************************************************
/**
* История изменений:
* 
* Версия 1.0.1
* Дата   12-06-2013
* Автор  Годунок А.Н.
*
* Изменения:
*    Базовая версия.
*/
