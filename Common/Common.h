#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <immintrin.h>

typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;
typedef __m128i i32x4;
typedef __m256i i32x8;
typedef __m128 f32x4;
typedef __m256 f32x8;
typedef __m128d f64x2;
typedef __m256d f64x4;

typedef struct TSetupInfo
{
    u32 WindowSize;
} TSetupInfo;

f64 GetTime(void);
