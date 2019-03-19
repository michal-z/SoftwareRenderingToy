#include "Common.h"


#define KHitDistance 0.001f
#define KMaxDistance 20.0f
#define KEscapeThreshold 10.0f
#define KEpsilon 0.001f
static TVector3 GViewTransform[3];
static TVector3 GEyePosition = { 0.0f, 0.0f, 2.1f };
static TVector4 GQuaternion = { 0.6f, 0.2f, 0.2f, -0.2f };

FORCEINLINE static TVector4 QuaternionMultiply(TVector4 A, TVector4 B)
{
	TVector4 R;
	R.W = A.W * B.W - A.X * B.X - A.Y * B.Y - A.Z * B.Z;
	R.X = A.W * B.X + A.X * B.W + A.Y * B.Z - A.Z * B.Y;
	R.Y = A.W * B.Y - A.X * B.Z + A.Y * B.W + A.Z * B.X;
	R.Z = A.W * B.Z + A.X * B.Y - A.Y * B.X + A.Z * B.W;
	return R;
}

FORCEINLINE static TVector4 QuaternionSquare(TVector4 A)
{
	TVector4 R;
	R.W = A.W * A.W - A.X * A.X - A.Y * A.Y - A.Z * A.Z;
	R.X = 2.0f * A.X * A.W;
	R.Y = 2.0f * A.Y * A.W;
	R.Z = 2.0f * A.Z * A.W;
	return R;
}

FORCEINLINE static TVector3 ComputeNormalVector(TVector3 Position)
{
	TVector4 QP = { Position.Y, Position.Z, 0.0f, Position.X };

	TVector4 GX0 = Vector4Add(QP, Vector4Set(0.0f, 0.0f, 0.0f, -KEpsilon));
	TVector4 GX1 = Vector4Add(QP, Vector4Set(0.0f, 0.0f, 0.0f, KEpsilon));
	TVector4 GY0 = Vector4Add(QP, Vector4Set(-KEpsilon, 0.0f, 0.0f, 0.0f));
	TVector4 GY1 = Vector4Add(QP, Vector4Set(KEpsilon, 0.0f, 0.0f, 0.0f));
	TVector4 GZ0 = Vector4Add(QP, Vector4Set(0.0f, -KEpsilon, 0.0f, 0.0f));
	TVector4 GZ1 = Vector4Add(QP, Vector4Set(0.0f, KEpsilon, 0.0f, 0.0f));

	for (u32 Iteration = 0; Iteration < 10; ++Iteration)
	{
		GX0 = Vector4Add(QuaternionSquare(GX0), GQuaternion);
		GX1 = Vector4Add(QuaternionSquare(GX1), GQuaternion);
		GY0 = Vector4Add(QuaternionSquare(GY0), GQuaternion);
		GY1 = Vector4Add(QuaternionSquare(GY1), GQuaternion);
		GZ0 = Vector4Add(QuaternionSquare(GZ0), GQuaternion);
		GZ1 = Vector4Add(QuaternionSquare(GZ1), GQuaternion);
	}

	TVector3 Normal;
	Normal.X = Vector4Length(GX1) - Vector4Length(GX0);
	Normal.Y = Vector4Length(GY1) - Vector4Length(GY0);
	Normal.Z = Vector4Length(GZ1) - Vector4Length(GZ0);
	return Vector3Normalize(Normal);
}

FORCEINLINE static f32 ComputeDistance(TVector3 Position)
{
	TVector4 Q = { Position.Y, Position.Z, 0.0f, Position.X };
	TVector4 QP = { 0.0f, 0.0f, 0.0f, 1.0f };
	f32 Magnitude2;

	for (u32 Iteration = 0; Iteration < 10; ++Iteration)
	{
		QP = Vector4Scale(QuaternionMultiply(Q, QP), 2.0f);
		Q = Vector4Add(QuaternionSquare(Q), GQuaternion);

		Magnitude2 = Vector4Dot(Q, Q);
		if (Magnitude2 > KEscapeThreshold)
		{
			break;
		}
	}

	f32 Magnitude = sqrtf(Magnitude2);
	return 0.5f * Magnitude * logf(Magnitude) / Vector4Length(QP);
}

FORCEINLINE static b32 CastRay(TVector3 RayOrigin, TVector3 RayDirection, TVector3 *Position)
{
	f32 MarchedDistance = 1.0f;

	for (u32 Iteration = 0; Iteration < 128; ++Iteration)
	{
		*Position = Vector3Add(Vector3Scale(RayDirection, MarchedDistance), RayOrigin);

		f32 Distance = ComputeDistance(*Position);
		if (Distance <= KHitDistance)
		{
			return 1;
		}

		MarchedDistance += Distance;
		if (MarchedDistance >= KMaxDistance)
		{
			return 0;
		}
	}

	return 0;
}

TSetupInfo Setup(void)
{
    TSetupInfo Info =
    {
        .Name = "QuaternionJuliaSets",
        .WindowSize = 800
    };
    return Info;
}

void BeginFrame(void)
{
	TVector3 LookAtPosition = { 0.0f, 0.0f, 0.0f };
	TVector3 UpVector = { 0.0f, 1.0f, 0.0f };

	TVector3 ZAxis = Vector3Normalize(Vector3Subtract(GEyePosition, LookAtPosition));
	TVector3 XAxis = Vector3Normalize(Vector3Cross(UpVector, ZAxis));
	TVector3 YAxis = Vector3Normalize(Vector3Cross(ZAxis, XAxis));

	GViewTransform[0] = Vector3Set(XAxis.X, YAxis.X, ZAxis.X);
	GViewTransform[1] = Vector3Set(XAxis.Y, YAxis.Y, ZAxis.Y);
	GViewTransform[2] = Vector3Set(XAxis.Z, YAxis.Z, ZAxis.Z);
}

void RenderTile(u8 *Image, u32 BeginX, u32 BeginY, u32 EndX, u32 EndY)
{
    f32 ReciprocalWindowSize = 1.0f / GWindowSize;
	TVector3 RayOrigin = GEyePosition;

    for (u32 CurrentY = BeginY; CurrentY < EndY; ++CurrentY)
    {
        f32 Y = 2.0f * (CurrentY * ReciprocalWindowSize - 0.5f);

        for (u32 CurrentX = BeginX; CurrentX < EndX; ++CurrentX)
        {
            f32 X = 2.0f * (CurrentX * ReciprocalWindowSize - 0.5f);

			TVector3 RayDirection = { X, Y, -1.5f };
			RayDirection.X = Vector3Dot(RayDirection, GViewTransform[0]);
			RayDirection.Y = Vector3Dot(RayDirection, GViewTransform[1]);
			RayDirection.Z = Vector3Dot(RayDirection, GViewTransform[2]);
			RayDirection = Vector3Normalize(RayDirection);

			TVector3 Color = { 0 };
			TVector3 HitPosition;
			if (CastRay(RayOrigin, RayDirection, &HitPosition))
			{
				Color = ComputeNormalVector(HitPosition);
			}

			_mm_store_ss(&Color.X, _mm_max_ss(_mm_load_ss(&Color.X), _mm_setzero_ps()));
			_mm_store_ss(&Color.Y, _mm_max_ss(_mm_load_ss(&Color.Y), _mm_setzero_ps()));
			_mm_store_ss(&Color.Z, _mm_max_ss(_mm_load_ss(&Color.Z), _mm_setzero_ps()));

			_mm_store_ss(&Color.X, _mm_min_ss(_mm_load_ss(&Color.X), GF32x8_1_0.VL));
			_mm_store_ss(&Color.Y, _mm_min_ss(_mm_load_ss(&Color.Y), GF32x8_1_0.VL));
			_mm_store_ss(&Color.Z, _mm_min_ss(_mm_load_ss(&Color.Z), GF32x8_1_0.VL));

            u32 Index = (CurrentX + CurrentY * GWindowSize) * 4;
            Image[Index + 0] = (u8)(255.0f * Color.Z);
            Image[Index + 1] = (u8)(255.0f * Color.Y);
            Image[Index + 2] = (u8)(255.0f * Color.X);
            Image[Index + 3] = 0xff;
        }
    }
}
