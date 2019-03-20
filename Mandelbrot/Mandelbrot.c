#include "Common.h"


static FORCEINLINE TVector2 ComplexMultiply(TVector2 A, TVector2 B)
{
    TVector2 R = { A.X * B.X - A.Y * B.Y, A.X * B.Y + A.Y * B.X };
    return R;
}

static FORCEINLINE TVector2 ComplexSquare(TVector2 A)
{
    TVector2 R = { A.X * A.X - A.Y * A.Y, 2.0f * A.X * A.Y };
    return R;
}

static FORCEINLINE f32 ComputeDistance(f32 CX, f32 CY)
{
    TVector2 Z = { 0.0f, 0.0f };
    TVector2 DZ = { 1.0f, 0.0f };

    for (u32 Iteration = 0; Iteration < 128; ++Iteration)
    {
        DZ = Vector2Scale(ComplexMultiply(Z, DZ), 2.0f);
        DZ.X += 1.0f;

        Z = Vector2Add(ComplexSquare(Z), Vector2Set(CX, CY));

        f32 Magnitude2 = Vector2Dot(Z, Z);
        if (Magnitude2 > 100.0f)
        {
            // goes to infinity, does not belong to the Mandelbrot set, estimate the distance to the set
            return sqrtf(Magnitude2 / Vector2Dot(DZ, DZ)) * 0.5f * logf(Magnitude2);
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

void BeginFrame(void)
{
}

void RenderTile(u8 *Image, u32 BeginX, u32 BeginY, u32 EndX, u32 EndY)
{
    f32 ReciprocalWindowSize = 1.0f / GWindowSize;
    f32 Zoom = 0.8f;
    f32 PositionX = 0.5f;
    f32 PositionY = 0.1f;

    for (u32 CurrentY = BeginY; CurrentY < EndY; ++CurrentY)
    {
        f32 CY = 2.0f * (CurrentY * ReciprocalWindowSize - 0.5f);
        CY = (CY * Zoom) - PositionY;

        for (u32 CurrentX = BeginX; CurrentX < EndX; ++CurrentX)
        {
            f32 CX = 2.0f * (CurrentX * ReciprocalWindowSize - 0.5f);
            CX = (CX * Zoom) - PositionX;

            f32 Distance = ComputeDistance(CX, CY);
            if (Distance > 0.0f)
            {
                Distance = sqrtf(sqrtf(Distance / Zoom));
                Distance = (Distance > 1.0f) ? 1.0f : Distance;
            }

            u32 Index = (CurrentX + CurrentY * GWindowSize) * 4;
            Image[Index + 0] = (u8)(255.0f * Distance);
            Image[Index + 1] = (u8)(255.0f * Distance);
            Image[Index + 2] = (u8)(255.0f * Distance);
            Image[Index + 3] = 0xff;
        }
    }
}
