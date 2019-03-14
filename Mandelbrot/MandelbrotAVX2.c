#include "Common.h"


#if 0
static __m256d ComputeDistance(__m256d vcx, __m256d vcy, int bailout)
{
    ComplexPacket z = { _mm256_setzero_pd(), _mm256_setzero_pd() };
    ComplexPacket dz = { s_1_0, _mm256_setzero_pd() };
    __m256d m2, lessMask;
    do
    {
        m2 = _mm256_add_pd(_mm256_mul_pd(z.re, z.re), _mm256_mul_pd(z.im, z.im));
        lessMask = _mm256_cmp_pd(m2, s_100_0, _CMP_LE_OQ);
        if (_mm256_movemask_pd(lessMask) == 0)
            break;

        ComplexPacket dzN = ComplexPacketMul(z, dz);
        dzN.re = _mm256_add_pd(_mm256_add_pd(dzN.re, dzN.re), s_1_0);
        dzN.im = _mm256_add_pd(dzN.im, dzN.im);

        ComplexPacket zN = ComplexPacketSqr(z);
        zN.re = _mm256_add_pd(zN.re, vcx);
        zN.im = _mm256_add_pd(zN.im, vcy);

        z.re = _mm256_blendv_pd(z.re, zN.re, lessMask);
        z.im = _mm256_blendv_pd(z.im, zN.im, lessMask);
        dz.re = _mm256_blendv_pd(dz.re, dzN.re, lessMask);
        dz.im = _mm256_blendv_pd(dz.im, dzN.im, lessMask);
    } while (--bailout);

    __declspec(align(32)) double logTemp[4];
    _mm256_store_pd(logTemp, m2);
    logTemp[0] = log(logTemp[0]);
    logTemp[1] = log(logTemp[1]);
    logTemp[2] = log(logTemp[2]);
    logTemp[3] = log(logTemp[3]);
    __m256d logRes = _mm256_load_pd(logTemp);

    __m256d dzDot2 = _mm256_add_pd(_mm256_mul_pd(dz.re, dz.re), _mm256_mul_pd(dz.im, dz.im));

    __m256d dist = _mm256_sqrt_pd(_mm256_div_pd(m2, dzDot2));
    dist = _mm256_mul_pd(logRes, _mm256_mul_pd(dist, s_0_5));

    return _mm256_andnot_pd(lessMask, dist);
}

static void DrawTile(Demo *demo, uint32_t tileIndex)
{
    uint32_t x0 = (tileIndex % k_NumTilesX) * k_TileSize;
    uint32_t y0 = (tileIndex / k_NumTilesX) * k_TileSize;
    uint32_t x1 = x0 + k_TileSize;
    uint32_t y1 = y0 + k_TileSize;
    uint8_t *displayPtr = demo->displayPtr;

    __m256d xOffsets = _mm256_set_pd(3.0f, 2.0f, 1.0f, 0.0f);
    __m256d rcpResX = _mm256_set1_pd(k_DemoRcpResolutionX);
    __m256d aspectRatio = _mm256_set1_pd(k_DemoAspectRatio);
    __m256d zoom = _mm256_broadcast_sd(&demo->zoom);
    __m256d posX = _mm256_broadcast_sd(&demo->position[0]);

    for (uint32_t y = y0; y < y1; ++y)
    {
        double cy = 2.0 * (y * k_DemoRcpResolutionY - 0.5);
        cy = (cy * demo->zoom) - demo->position[1];
        __m256d vcy = _mm256_broadcast_sd(&cy);

        for (uint32_t x = x0; x < x1; x += 4)
        {
            // vcx = 2.0 * (x * k_DemoRcpResolutionX - 0.5) * k_DemoAspectRatio;
            double xd = (double)x;
            __m256d vcx = _mm256_add_pd(_mm256_broadcast_sd(&xd), xOffsets);
            vcx = _mm256_sub_pd(_mm256_mul_pd(vcx, rcpResX), s_0_5);
            vcx = _mm256_mul_pd(_mm256_add_pd(vcx, vcx), aspectRatio);

            // vcx = (vcx * demo->zoom) - demo->position[0];
            vcx = _mm256_sub_pd(_mm256_mul_pd(vcx, zoom), posX);

            __m256d d = ComputeDistance(vcx, vcy, 256);
            d = _mm256_sqrt_pd(_mm256_sqrt_pd(_mm256_div_pd(d, zoom)));
            d = _mm256_min_pd(d, s_1_0);

            __declspec(align(32)) double ds[4];
            _mm256_store_pd(ds, d);
            uint32_t idx = (x + y * k_DemoResolutionX) * 4;
            displayPtr[idx +  0] = (uint8_t)(255.0 * ds[0]);
            displayPtr[idx +  1] = (uint8_t)(255.0 * ds[0]);
            displayPtr[idx +  2] = (uint8_t)(255.0 * ds[0]);
            displayPtr[idx +  3] = 255;
            displayPtr[idx +  4] = (uint8_t)(255.0 * ds[1]);
            displayPtr[idx +  5] = (uint8_t)(255.0 * ds[1]);
            displayPtr[idx +  6] = (uint8_t)(255.0 * ds[1]);
            displayPtr[idx +  7] = 255;
            displayPtr[idx +  8] = (uint8_t)(255.0 * ds[2]);
            displayPtr[idx +  9] = (uint8_t)(255.0 * ds[2]);
            displayPtr[idx + 10] = (uint8_t)(255.0 * ds[2]);
            displayPtr[idx + 11] = 255;
            displayPtr[idx + 12] = (uint8_t)(255.0 * ds[3]);
            displayPtr[idx + 13] = (uint8_t)(255.0 * ds[3]);
            displayPtr[idx + 14] = (uint8_t)(255.0 * ds[3]);
            displayPtr[idx + 15] = 255;
        }
    }
}
#endif

typedef struct TComplex
{
    f32x8 Re;
    f32x8 Im;
} TComplex;

static FORCEINLINE TComplex CMultiply(TComplex A, TComplex B)
{
    TComplex AB;
    AB.Re = f32x8_sub(f32x8_mul(A.Re, B.Re), f32x8_mul(A.Im, B.Im));
    AB.Im = f32x8_add(f32x8_mul(A.Re, B.Im), f32x8_mul(A.Im, B.Re));
    return AB;
}

static FORCEINLINE TComplex CSquare(TComplex A)
{
    TComplex AA;
    AA.Re = f32x8_sub(f32x8_mul(A.Re, A.Re), f32x8_mul(A.Im, A.Im));
    AA.Im = f32x8_mul(f32x8_add(A.Re, A.Re), A.Im);
    return AA;
}

static inline f32x8 ComputeDistance(f32x8 Cx, f32x8 Cy, u32 Bailout)
{
    TComplex Z = { f32x8_setzero(), f32x8_setzero() };
    TComplex DZ = { GF32x8_1_0.V, f32x8_setzero() };

    for (u32 Iteration = 0; Iteration < Bailout; ++Iteration)
    {
        f32x8 Magnitude2 = f32x8_add(f32x8_mul(Z.Re, Z.Re), f32x8_mul(Z.Im, Z.Im));
        f32x8 LessEqualMask = f32x8_cmple(Magnitude2, GF32x8_100_0);
        if (f32x8_movemask(LessEqualMask) == 0)
        {
            break;
        }

        TComplex NewDZ = CMultiply(Z, DZ);
        NewDZ.Re = f32x8_add(f32x8_add(NewDZ.Re, NewDZ.Re), 1.0f);
        NewDZ.Im = f32x8_add(NewDZ.Im, NewDZ.Im);

        TComplex NewZ = CSquare(Z);
        NewZ.Re = f32x8_add(NewZ.Re, Cx);
        NewZ.Im = f32x8_add(NewZ.Im, Cy);
    }

    // belongs to the set (distance is 0.0f)
    return f32x8_setzero();
}

TSetupInfo Setup(void)
{
    TSetupInfo Info =
    {
        .Name = "Mandelbrot",
        .WindowSize = 800
    };
    return Info;
}

void RenderTile(u8 *ImagePtr, u32 BeginX, u32 BeginY, u32 EndX, u32 EndY)
{
    f32 ReciprocalWindowSize = 1.0f / GWindowSize;
    f32 Zoom = 0.8f;
    f32 PositionX = 0.5f;
    f32 PositionY = 0.1f;

    for (u32 CurrentY = BeginY; CurrentY < EndY; ++CurrentY)
    {
        f32 Cy = 2.0f * (CurrentY * ReciprocalWindowSize - 0.5f);
        Cy = (Cy * Zoom) - PositionY;

        for (u32 CurrentX = BeginX; CurrentX < EndX; ++CurrentX)
        {
            f32 Cx = 2.0f * (CurrentX * ReciprocalWindowSize - 0.5f);
            Cx = (Cx * Zoom) - PositionX;

            f32 Distance = ComputeDistance(Cx, Cy, 128);
            if (Distance > 0.0f)
            {
                Distance = sqrtf(sqrtf(Distance / Zoom));
                Distance = (Distance > 1.0f) ? 1.0f : Distance;
            }

            u32 Index = (CurrentX + CurrentY * GWindowSize) * 4;
            ImagePtr[Index + 0] = (u8)(255.0f * Distance);
            ImagePtr[Index + 1] = (u8)(255.0f * Distance);
            ImagePtr[Index + 2] = (u8)(255.0f * Distance);
            ImagePtr[Index + 3] = 0xff;
        }
    }
}
