#include "Common.h"
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#define KName "software rendering"
#define KWindowSize 800
#define KTileSize 16
#define KNumTilesPerRow (KWindowSize / KTileSize)
#define KNumTiles (KNumTilesPerRow * KNumTilesPerRow)

static u32 GTileIndex;
static u8 *GDisplayPtr;

void Render(u8 *PixelPtr, u32 BeginX, u32 BeginY, u32 EndX, u32 EndY);

f64 GetTime(void)
{
    static LARGE_INTEGER StartCounter;
    static LARGE_INTEGER Frequency;
    if (StartCounter.QuadPart == 0)
    {
        QueryPerformanceFrequency(&Frequency);
        QueryPerformanceCounter(&StartCounter);
    }
    LARGE_INTEGER Counter;
    QueryPerformanceCounter(&Counter);
    return (Counter.QuadPart - StartCounter.QuadPart) / (f64)Frequency.QuadPart;
}

static void UpdateFrameStats(HWND Window, const char *Name, f64 *TimeOut, f32 *DeltaTimeOut)
{
    static f64 PreviousTime = -1.0;
    static f64 HeaderRefreshTime = 0.0;
    static u32 FrameCount = 0;

    if (PreviousTime < 0.0)
    {
        PreviousTime = GetTime();
        HeaderRefreshTime = PreviousTime;
    }

    *TimeOut = GetTime();
    *DeltaTimeOut = (f32)(*TimeOut - PreviousTime);
    PreviousTime = *TimeOut;

    if ((*TimeOut - HeaderRefreshTime) >= 1.0)
    {
        f64 FramesPerSecond = FrameCount / (*TimeOut - HeaderRefreshTime);
        f64 MilliSeconds = (1.0 / FramesPerSecond) * 1000.0;
        char Header[256];
        snprintf(Header, sizeof(Header), "[%.1f fps  %.3f ms] %s", FramesPerSecond, MilliSeconds, Name);
        SetWindowText(Window, Header);
        HeaderRefreshTime = *TimeOut;
        FrameCount = 0;
    }
    FrameCount++;
}

static LRESULT CALLBACK ProcessWindowMessage(HWND Window, UINT Message, WPARAM ParamW, LPARAM ParamL)
{
    switch (Message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (ParamW == VK_ESCAPE)
        {
            PostQuitMessage(0);
            return 0;
        }
        break;
    }
    return DefWindowProc(Window, Message, ParamW, ParamL);
}

static void CALLBACK RenderJob(PTP_CALLBACK_INSTANCE Instance, void *Context, PTP_WORK Work)
{
    (void)Instance; (void)Context; (void)Work;

    u8 *DisplayPtr = GDisplayPtr;

    for (;;)
    {
        u32 TileIndex = (u32)_InterlockedIncrement((volatile LONG *)&GTileIndex) - 1;
        if (TileIndex >= KNumTiles)
            break;

        u32 BeginX = (TileIndex % KNumTilesPerRow) * KTileSize;
        u32 BeginY = (TileIndex / KNumTilesPerRow) * KTileSize;
        u32 EndX = BeginX + KTileSize;
        u32 EndY = BeginY + KTileSize;

        Render(&DisplayPtr[(BeginX + BeginY * KWindowSize) * 4], BeginX, BeginY, EndX, EndY);
    }
}

i32 main(void)
{
    SetProcessDPIAware();

    WNDCLASS Winclass =
    {
        .lpfnWndProc = ProcessWindowMessage,
        .hInstance = GetModuleHandle(NULL),
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .lpszClassName = KName
    };
    if (!RegisterClass(&Winclass))
        assert(0);

    RECT Rect = { 0, 0, KWindowSize, KWindowSize };
    if (!AdjustWindowRect(&Rect, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, 0))
        assert(0);

    HWND Window = CreateWindowEx(
        0, KName, KName, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        Rect.right - Rect.left, Rect.bottom - Rect.top,
        NULL, NULL, NULL, 0);
    assert(Window);

    HDC WindowDc = GetDC(Window);
    assert(WindowDc);

    BITMAPINFO BitmapInfo =
    {
        .bmiHeader.biSize = sizeof(BITMAPINFOHEADER),
        .bmiHeader.biPlanes = 1,
        .bmiHeader.biBitCount = 32,
        .bmiHeader.biCompression = BI_RGB,
        .bmiHeader.biWidth = KWindowSize,
        .bmiHeader.biHeight = KWindowSize,
        .bmiHeader.biSizeImage = KWindowSize * KWindowSize
    };
    HBITMAP BitmapHandle = CreateDIBSection(WindowDc, &BitmapInfo, DIB_RGB_COLORS, (void **)&GDisplayPtr, NULL, 0);
    assert(BitmapHandle);

    HDC MemoryDc = CreateCompatibleDC(WindowDc);
    assert(MemoryDc);
    SelectObject(MemoryDc, BitmapHandle);

    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    u32 NumCores = (u32)SystemInfo.dwNumberOfProcessors;

    PTP_WORK JobHandle = CreateThreadpoolWork(RenderJob, NULL, NULL);
    assert(JobHandle);

    for (;;)
    {
        MSG Message = { 0 };
        if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
            DispatchMessage(&Message);
            if (Message.message == WM_QUIT)
                break;
        }
        else
        {
            f64 Time;
            f32 DeltaTime;
            UpdateFrameStats(Window, KName, &Time, &DeltaTime);

            GTileIndex = 0;

            for (u32 Index = 0; Index < NumCores - 1; ++Index)
                SubmitThreadpoolWork(JobHandle);

            RenderJob(NULL, NULL, NULL);

            WaitForThreadpoolWorkCallbacks(JobHandle, FALSE);

            BitBlt(WindowDc, 0, 0, KWindowSize, KWindowSize, MemoryDc, 0, 0, SRCCOPY);
        }
    }

    return 0;
}
