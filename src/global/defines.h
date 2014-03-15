/**
 * @file global defines
 * Stephen Zhang, 2014
 */

#ifndef DEFINES_H
#define DEFINES_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* bit defines */
#define BIT_0   1
#define BIT_1   2
#define BIT_2   4
#define BIT_3   8
#define BIT_4   16
#define BIT_5   32
#define BIT_6   64
#define BIT_7   128
#define BIT_8   256
#define BIT_9   512
#define BIT_10  1024
#define BIT_11  2048
#define BIT_12  4096
#define BIT_13  8192
#define BIT_14  16384
#define BIT_15  32768

/* bit operations */
#define LO_BYTE(w) (unsigned char)(w & 0xff)
#define HI_BYTE(w) (unsigned char)(w & 0xff00) >> 8
#define WORD_B(h, l) ((unsigned short)h)<<8 | l

/* MB and KB, RANGE_8/16 definitions */
#define KB 1024
#define MB KB*KB
#define RANGE_8 256
#define RANGE_16 65536

#endif