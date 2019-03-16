#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <math.h>
#include <assert.h>
#include <immintrin.h>

#define FORCEINLINE __forceinline
#define GLOBALCONST extern const __declspec(selectany)
#define ALIGN(X) __declspec(align(X))

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
    const char* Name;
    u32 WindowSize;
} TSetupInfo;

typedef ALIGN(32) struct TF32x8
{
    union
    {
        f32 F[8];
        f32x8 V;
    };
} TF32x8;

extern u32 GWindowSize;
extern f64 GTime;
extern f32 GDeltaTime;

TSetupInfo Setup(void);
void RenderTile(u8 *ImagePtr, u32 BeginX, u32 BeginY, u32 EndX, u32 EndY);

FORCEINLINE f32x8 _mm256_log_ps(f32x8 A)
{
    // MS compiler generates one call to __vdecl_logf8
    ALIGN(32) f32 Temp[8];
    _mm256_store_ps(Temp, A);
    Temp[0] = logf(Temp[0]);
    Temp[1] = logf(Temp[1]);
    Temp[2] = logf(Temp[2]);
    Temp[3] = logf(Temp[3]);
    Temp[4] = logf(Temp[4]);
    Temp[5] = logf(Temp[5]);
    Temp[6] = logf(Temp[6]);
    Temp[7] = logf(Temp[7]);
    return _mm256_load_ps(Temp);
}

GLOBALCONST TF32x8 GF32x8_1_0 = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
GLOBALCONST TF32x8 GF32x8_2_0 = { 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f };
GLOBALCONST TF32x8 GF32x8_0_5 = { 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };
GLOBALCONST TF32x8 GF32x8_100_0 = { 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f };
GLOBALCONST TF32x8 GF32x8_255_0 = { 255.0f, 255.0f, 255.0f, 255.0f, 255.0f, 255.0f, 255.0f, 255.0f };
GLOBALCONST TF32x8 GF32x8_XCenterOffsets = { 0.5f, 1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f };
