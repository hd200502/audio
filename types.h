#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

typedef signed char                       int8;
typedef signed short                      int16;
typedef signed long                       int32;
typedef signed int                        intx;
typedef unsigned char                     uint8;
typedef unsigned short                    uint16;
typedef unsigned long                     uint32;
typedef unsigned int                      uintx;

typedef unsigned char                     U8;
typedef signed char                       S8;
typedef unsigned short                    U16;
typedef signed short                      S16;
typedef unsigned int                      U32;
typedef signed int                        S32;

#define HPRINTF printf

typedef struct {
    float real;
    float imag;
} COMPLEX;

#endif

