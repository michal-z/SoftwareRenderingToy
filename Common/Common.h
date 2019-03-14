#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
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

FORCEINLINE f32x8 f32x8_setzero(void) { return _mm256_setzero_ps(); }
FORCEINLINE f32x8 f32x8_mul(f32x8 A, f32x8 B) { return _mm256_mul_ps(A, B); }
FORCEINLINE f32x8 f32x8_add(f32x8 A, f32x8 B) { return _mm256_add_ps(A, B); }
FORCEINLINE f32x8 f32x8_sub(f32x8 A, f32x8 B) { return _mm256_sub_ps(A, B); }
FORCEINLINE f32x8 f32x8_blendv(f32x8 A, f32x8 B, f32x8 M) { return _mm256_blendv_ps(A, B, M); }
#define f32x8_cmple(A, B) _mm256_cmp_ps((A), (B), _CMP_LE_OQ)
FORCEINLINE i32 f32x8_movemask(f32x8 A) { return _mm256_movemask_ps(A); }

GLOBALCONST TF32x8 GF32x8_1_0 = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
GLOBALCONST TF32x8 GF32x8_100_0 = { 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f };
