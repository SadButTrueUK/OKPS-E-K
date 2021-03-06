#ifndef VERSION_H

#define VERSION_H
#define VERSION_MAJOR   1L
#define VERSION_MINOR   0L
#define VERSION_BUILD   32L
// type long
#define VERSION_NUMBER  ( 0x00L | ( VERSION_MAJOR << 24 ) | ( VERSION_MINOR << 16 ) | ( VERSION_BUILD ) )

#define VERSION_YEAR     20     // digit 15-9
#define VERSION_MONTH    8     // digit 8-5
#define VERSION_DAY      17     // digit 4-0
// type short
#define VERSION_DATE (    ( VERSION_YEAR  << 9 ) \
                        | ( VERSION_MONTH << 5 ) \
                        |   VERSION_DAY )


#endif//_VERSION_H
