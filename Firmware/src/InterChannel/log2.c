/**
* \file    log2.c
* \brief   Определение таблицы Log2_log2Lkup[]
* \details Таблица используется очередями с приоритетом set.h
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
uint8_t const Log2_log2Lkup[256] =
{
    ( uint8_t )0, ( uint8_t )1, ( uint8_t )2, ( uint8_t )2,
    ( uint8_t )3, ( uint8_t )3, ( uint8_t )3, ( uint8_t )3,
    ( uint8_t )4, ( uint8_t )4, ( uint8_t )4, ( uint8_t )4,
    ( uint8_t )4, ( uint8_t )4, ( uint8_t )4, ( uint8_t )4,
    ( uint8_t )5, ( uint8_t )5, ( uint8_t )5, ( uint8_t )5,
    ( uint8_t )5, ( uint8_t )5, ( uint8_t )5, ( uint8_t )5,
    ( uint8_t )5, ( uint8_t )5, ( uint8_t )5, ( uint8_t )5,
    ( uint8_t )5, ( uint8_t )5, ( uint8_t )5, ( uint8_t )5,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )6, ( uint8_t )6, ( uint8_t )6, ( uint8_t )6,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )7, ( uint8_t )7, ( uint8_t )7, ( uint8_t )7,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8,
    ( uint8_t )8, ( uint8_t )8, ( uint8_t )8, ( uint8_t )8
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
