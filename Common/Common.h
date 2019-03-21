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
typedef int b32;
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

typedef ALIGN(32) struct TF32X8
{
    union
    {
        f32 F[8];
        f32x8 V;
        struct
        {
            f32x4 VL;
            f32x4 VH;
        };
    };
} TF32X8;

typedef struct TVector2
{
    f32 X;
    f32 Y;
} TVector2;

typedef struct TVector3
{
    f32 X;
    f32 Y;
    f32 Z;
} TVector3;

typedef struct TVector4
{
    f32 X;
    f32 Y;
    f32 Z;
    f32 W;
} TVector4;

typedef ALIGN(32) struct TVector2X8
{
    f32x8 X;
    f32x8 Y;
} TVector2X8;

typedef ALIGN(32) struct TVector3X8
{
    f32x8 X;
    f32x8 Y;
    f32x8 Z;
} TVector3X8;

typedef ALIGN(32) struct TVector4X8
{
    f32x8 X;
    f32x8 Y;
    f32x8 Z;
    f32x8 W;
} TVector4X8;

extern u32 GWindowSize;
extern f64 GTime;
extern f32 GDeltaTime;

TSetupInfo Setup(void);
void BeginFrame(void);
void RenderTile(u8 *Image, u32 BeginX, u32 BeginY, u32 EndX, u32 EndY);

#ifdef _MSC_VER
#if _MSC_VER < 1920
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
#endif
#endif

FORCEINLINE TVector2 Vector2Set(f32 X, f32 Y)
{
    TVector2 R = { X, Y};
    return R;
}
FORCEINLINE TVector3 Vector3Set(f32 X, f32 Y, f32 Z)
{
    TVector3 R = { X, Y, Z };
    return R;
}
FORCEINLINE TVector4 Vector4Set(f32 X, f32 Y, f32 Z, f32 W)
{
    TVector4 R = { X, Y, Z, W };
    return R;
}

FORCEINLINE TVector2 Vector2Scale(TVector2 A, f32 B)
{
    TVector2 R = { A.X * B, A.Y * B };
    return R;
}
FORCEINLINE TVector3 Vector3Scale(TVector3 A, f32 B)
{
    TVector3 R = { A.X * B, A.Y * B, A.Z * B };
    return R;
}
FORCEINLINE TVector4 Vector4Scale(TVector4 A, f32 B)
{
    TVector4 R = { A.X * B, A.Y * B, A.Z * B, A.W * B };
    return R;
}

FORCEINLINE TVector2 Vector2Add(TVector2 A, TVector2 B)
{
    TVector2 R = { A.X + B.X, A.Y + B.Y };
    return R;
}
FORCEINLINE TVector3 Vector3Add(TVector3 A, TVector3 B)
{
    TVector3 R = { A.X + B.X, A.Y + B.Y, A.Z + B.Z };
    return R;
}
FORCEINLINE TVector4 Vector4Add(TVector4 A, TVector4 B)
{
    TVector4 R = { A.X + B.X, A.Y + B.Y, A.Z + B.Z, A.W + B.W };
    return R;
}

FORCEINLINE TVector3 Vector3Subtract(TVector3 A, TVector3 B)
{
    TVector3 R = { A.X - B.X, A.Y - B.Y, A.Z - B.Z };
    return R;
}

FORCEINLINE f32 Vector2Dot(TVector2 A, TVector2 B)
{
    return A.X * B.X + A.Y * B.Y;
}
FORCEINLINE f32 Vector3Dot(TVector3 A, TVector3 B)
{
    return A.X * B.X + A.Y * B.Y + A.Z * B.Z;
}
FORCEINLINE f32 Vector4Dot(TVector4 A, TVector4 B)
{
    return A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;
}

FORCEINLINE f32 Vector3Length(TVector3 A)
{
    return sqrtf(Vector3Dot(A, A));
}
FORCEINLINE f32 Vector4Length(TVector4 A)
{
    return sqrtf(Vector4Dot(A, A));
}

FORCEINLINE TVector3 Vector3Normalize(TVector3 A)
{
    f32 Length = Vector3Length(A);
    assert(Length != 0.0f);
    f32 ReciprocalLength = 1.0f / Length;
    return Vector3Scale(A, ReciprocalLength);
}

FORCEINLINE TVector3 Vector3Cross(TVector3 A, TVector3 B)
{
    TVector3 R;
    R.X = A.Y * B.Z - A.Z * B.Y;
    R.Y = A.Z * B.X - A.X * B.Z;
    R.Z = A.X * B.Y - A.Y * B.X;
    return R;
}

FORCEINLINE f32 Vector3Distance(TVector3 A, TVector3 B)
{
    return Vector3Length(Vector3Subtract(A, B));
}

GLOBALCONST TF32X8 GF32X8_1_0 = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
GLOBALCONST TF32X8 GF32X8_2_0 = { 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f };
GLOBALCONST TF32X8 GF32X8_0_5 = { 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };
GLOBALCONST TF32X8 GF32X8_100_0 = { 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f };
GLOBALCONST TF32X8 GF32X8_255_0 = { 255.0f, 255.0f, 255.0f, 255.0f, 255.0f, 255.0f, 255.0f, 255.0f };
GLOBALCONST TF32X8 GF32X8_XCenterOffsets = { 0.5f, 1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f };

FORCEINLINE f32 F32Saturate(f32 A)
{
    f32x4 VA = _mm_load_ss(&A);
    VA = _mm_max_ss(VA, _mm_setzero_ps());
    VA = _mm_min_ss(VA, GF32X8_1_0.VL);
    _mm_store_ss(&A, VA);
    return A;
}

FORCEINLINE f32 F32Min(f32 A, f32 B)
{
	f32x4 VA = _mm_load_ss(&A);
	f32x4 VB = _mm_load_ss(&B);
	_mm_store_ss(&A, _mm_min_ss(VA, VB));
	return A;
}

FORCEINLINE f32 F32Max(f32 A, f32 B)
{
	f32x4 VA = _mm_load_ss(&A);
	f32x4 VB = _mm_load_ss(&B);
	_mm_store_ss(&A, _mm_max_ss(VA, VB));
	return A;
}

FORCEINLINE f32 F32Abs(f32 A)
{
    f32x4 VA = _mm_load_ss(&A);
	VA = _mm_max_ss(_mm_sub_ss(_mm_setzero_ps(), VA), VA);
	_mm_store_ss(&A, VA);
	return A;
}

FORCEINLINE TVector3 Vector3Saturate(TVector3 A)
{
    TVector3 R = { F32Saturate(A.X), F32Saturate(A.Y), F32Saturate(A.Z) };
    return R;
}

FORCEINLINE TVector3 Vector3Abs(TVector3 A)
{
    TVector3 R = { F32Abs(A.X), F32Abs(A.Y), F32Abs(A.Z) };
    return R;
}
