#include "Common.h"


typedef struct TComplex
{
    f32x8 Re;
    f32x8 Im;
} TComplex;

static FORCEINLINE TComplex CMultiply(TComplex A, TComplex B)
{
    TComplex AB;
    AB.Re = _mm256_sub_ps(_mm256_mul_ps(A.Re, B.Re), _mm256_mul_ps(A.Im, B.Im));
    AB.Im = _mm256_add_ps(_mm256_mul_ps(A.Re, B.Im), _mm256_mul_ps(A.Im, B.Re));
    return AB;
}

static FORCEINLINE TComplex CSquare(TComplex A)
{
    TComplex AA;
    AA.Re = _mm256_sub_ps(_mm256_mul_ps(A.Re, A.Re), _mm256_mul_ps(A.Im, A.Im));
    AA.Im = _mm256_mul_ps(_mm256_add_ps(A.Re, A.Re), A.Im);
    return AA;
}

static inline f32x8 ComputeDistance(f32x8 CX, f32x8 CY)
{
    TComplex Z = { _mm256_setzero_ps(), _mm256_setzero_ps() };
    TComplex DZ = { GF32x8_1_0.V, _mm256_setzero_ps() };

    f32x8 Magnitude2, InSetMask;

    for (u32 Iteration = 0; Iteration < 128; ++Iteration)
    {
        Magnitude2 = _mm256_add_ps(_mm256_mul_ps(Z.Re, Z.Re), _mm256_mul_ps(Z.Im, Z.Im));
        InSetMask = _mm256_cmp_ps(Magnitude2, GF32x8_100_0.V, _CMP_LE_OQ);
        if (_mm256_movemask_ps(InSetMask) == 0)
        {
            break;
        }

        TComplex NewDZ = CMultiply(Z, DZ);
        NewDZ.Re = _mm256_add_ps(NewDZ.Re, NewDZ.Re);
        NewDZ.Re = _mm256_add_ps(NewDZ.Re, GF32x8_1_0.V);
        NewDZ.Im = _mm256_add_ps(NewDZ.Im, NewDZ.Im);

        TComplex NewZ = CSquare(Z);
        NewZ.Re = _mm256_add_ps(NewZ.Re, CX);
        NewZ.Im = _mm256_add_ps(NewZ.Im, CY);

        Z.Re = _mm256_blendv_ps(Z.Re, NewZ.Re, InSetMask);
        Z.Im = _mm256_blendv_ps(Z.Im, NewZ.Im, InSetMask);

        DZ.Re = _mm256_blendv_ps(DZ.Re, NewDZ.Re, InSetMask);
        DZ.Im = _mm256_blendv_ps(DZ.Im, NewDZ.Im, InSetMask);
    }

    f32x8 DZMagnitude2 = _mm256_add_ps(_mm256_mul_ps(DZ.Re, DZ.Re), _mm256_mul_ps(DZ.Im, DZ.Im));
    f32x8 Magnitude2Log = _mm256_log_ps(Magnitude2);

    f32x8 Distance = _mm256_div_ps(Magnitude2, DZMagnitude2);
    Distance = _mm256_sqrt_ps(Distance);
    Distance = _mm256_mul_ps(Distance, GF32x8_0_5.V);
    Distance = _mm256_mul_ps(Distance, Magnitude2Log);

    // return 0.0f for points in the set
    // return 'Distance' for points not in the set
    return _mm256_andnot_ps(InSetMask, Distance);
}

TSetupInfo Setup(void)
{
    TSetupInfo Info =
    {
        .Name = "MandelbrotAVX2",
        .WindowSize = 800
    };
    return Info;
}

void RenderTile(u8 *Image, u32 BeginX, u32 BeginY, u32 EndX, u32 EndY)
{
    f32x8 ReciprocalWindowSize = _mm256_set1_ps(1.0f / GWindowSize);
    f32x8 Zoom = _mm256_set1_ps(0.8f);
    f32x8 PositionX = _mm256_set1_ps(0.5f);
    f32x8 PositionY = _mm256_set1_ps(0.1f);

    for (u32 CurrentY = BeginY; CurrentY < EndY; ++CurrentY)
    {
        f32x8 CY = _mm256_set1_ps((f32)CurrentY);
        CY = _mm256_mul_ps(CY, ReciprocalWindowSize);
        CY = _mm256_sub_ps(CY, GF32x8_0_5.V);
        CY = _mm256_add_ps(CY, CY);

        CY = _mm256_mul_ps(CY, Zoom);
        CY = _mm256_sub_ps(CY, PositionY);

        for (u32 CurrentX = BeginX; CurrentX < EndX; CurrentX += 8)
        {
            f32x8 CX = _mm256_set1_ps((f32)CurrentX);
            CX = _mm256_add_ps(CX, GF32x8_XCenterOffsets.V);
            CX = _mm256_mul_ps(CX, ReciprocalWindowSize);
            CX = _mm256_sub_ps(CX, GF32x8_0_5.V);
            CX = _mm256_add_ps(CX, CX);

            CX = _mm256_mul_ps(CX, Zoom);
            CX = _mm256_sub_ps(CX, PositionX);

            f32x8 Distance = ComputeDistance(CX, CY);
            Distance = _mm256_div_ps(Distance, Zoom);
            Distance = _mm256_sqrt_ps(Distance);
            Distance = _mm256_sqrt_ps(Distance);
            Distance = _mm256_min_ps(Distance, GF32x8_1_0.V);

            Distance = _mm256_mul_ps(Distance, GF32x8_255_0.V);
            i32x8 ColorR = _mm256_cvttps_epi32(Distance);
            i32x8 ColorG = _mm256_slli_epi32(ColorR, 8);
            i32x8 ColorB = _mm256_slli_epi32(ColorR, 16);

            i32x8 ColorRGB = _mm256_or_si256(ColorR, ColorG);
            ColorRGB = _mm256_or_si256(ColorRGB, ColorB);

            u32 Index = (CurrentX + CurrentY * GWindowSize) * 4;
            _mm256_store_si256((i32x8 *)&Image[Index], ColorRGB);
        }
    }
}
