#include "Common.h"


typedef struct TComplex
{
    f32 Re;
    f32 Im;
} TComplex;

static FORCEINLINE TComplex CMultiply(TComplex A, TComplex B)
{
    TComplex AB = { A.Re * B.Re - A.Im * B.Im, A.Re * B.Im + A.Im * B.Re };
    return AB;
}

static FORCEINLINE TComplex CSquare(TComplex A)
{
    TComplex AA = { A.Re * A.Re - A.Im * A.Im, 2.0f * A.Re * A.Im };
    return AA;
}

static inline f32 ComputeDistance(f32 Cx, f32 Cy, u32 Bailout)
{
    TComplex Z = { 0.0f, 0.0f };
    TComplex Dz = { 1.0f, 0.0f };

    for (u32 Iteration = 0; Iteration < Bailout; ++Iteration)
    {
        Dz = CMultiply(Z, Dz);
        Dz.Re = Dz.Re + Dz.Re + 1.0f;
        Dz.Im = Dz.Im + Dz.Im;

        Z = CSquare(Z);
        Z.Re += Cx;
        Z.Im += Cy;

        f32 Magnitude2 = Z.Re * Z.Re + Z.Im * Z.Im;
        if (Magnitude2 > 100.0f)
        {
            // goes to infinity, does not belong to the Mandelbrot set, estimate the distance to the set
            return sqrtf(Magnitude2 / (Dz.Re * Dz.Re + Dz.Im * Dz.Im)) * 0.5f * logf(Magnitude2);
        }
    }

    // belongs to the set (distance is 0.0f)
    return 0.0f;
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
