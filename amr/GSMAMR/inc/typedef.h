/*
 * ===================================================================
 *  TS 26.104
 *  R99   V3.5.0 2003-03
 *  REL-4 V4.5.0 2003-06
 *  REL-5 V5.2.0 2003-06
 *  3GPP AMR Floating-point Speech Codec
 * ===================================================================
 *
 */
/* This is valid for PC */

#ifndef AMR_TYPEDEF_H
#define AMR_TYPEDEF_H

typedef char Word8;
typedef unsigned char UWord8;
typedef short Word16;
typedef long Word32;
typedef float Float32;
typedef double Float64;

#if 0
#define AMR_MEM_ALLOC(x) malloc(x)
#define AMR_MEM_FREE(x) free(x)
#define AMRPRINTF(...) fprintf(stderr, ##__VA_ARGS__)
#else
//#define AMRPRINTF(...) kal_prompt_trace(MOD_MXGPS, ##__VA_ARGS__)
#define AMRPRINTF(...)
#define AMR_MEM_ALLOC(x) malloc(x)
#define AMR_MEM_FREE(x) free(x)
#endif
#endif
