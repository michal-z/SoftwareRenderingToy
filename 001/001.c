#include "Common.h"


void Setup(TSetupInfo *InfoOut)
{
    InfoOut->WindowSize = 800;
}

void Render(u8 *PixelPtr, u32 BeginX, u32 BeginY, u32 EndX, u32 EndY)
{
    for (u32 CurrentY = BeginY; CurrentY < EndY; ++CurrentY)
    {
        for (u32 CurrentX = BeginX; CurrentX < EndX; ++CurrentX)
        {
            u32 Index = (CurrentX + CurrentY * 800) * 4;
            PixelPtr[Index + 0] = 0xff;
            PixelPtr[Index + 1] = 0xff;
            PixelPtr[Index + 2] = 0xff;
            PixelPtr[Index + 3] = 0xff;
        }
    }
}
