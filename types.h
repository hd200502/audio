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


struct RIFF_HEADER
{
	U8  szRiffID[4];  // 'R','I','F','F'
	U32 dwRiffSize;
	U8  szRiffFormat[4]; // 'W','A','V','E'
};

struct WAVE_FORMAT
{
	U16 wFormatTag; // 1:PCM
	U16 wChannels;  // 1:单通道 2：双通道
	U32 dwSamplesPerSec; //采样率
	U32 dwAvgBytesPerSec; //每秒钟数据量 wChannels*dwSamplesPerSec*wBitsPerSample/8
	U16 wBlockAlign; // wChannels*wBitsPerSample/8
	U16 wBitsPerSample; //采样位数
};

struct FMT_BLOCK
{
	U8  szFmtID[4]; // 'f','m','t',' '
	U32 dwFmtSize;
	struct WAVE_FORMAT wavFormat;
};

struct FACT_BLOCK
{
	U8  szFactID[4]; // 'f','a','c','t'
	U32 dwFactSize;
};


#endif

