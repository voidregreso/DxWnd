#include <windows.h>
#include "syslibs.h"

#define DDSQLEN 0x40
#define MAXFONTS 0x40

#define WS_OVERLAPPEDTHIN   (WS_DLGFRAME|WS_BORDER)

// uncomment DXW_NOTRACES definition to build a "light" dxwnd.dll with no trace messages
//#define DXW_NOTRACES

// callbacks:
// DDrawInitializer (creates the real surfaces in emulation mode, void otherwise?
// GetScreenshot

typedef struct {
    DWORD dwTimerType;
    union {
        struct {
            UINT uTimerId;
            UINT uDelay;
            UINT uResolution;
            LPTIMECALLBACK lpTimeProc;
            DWORD_PTR dwUser;
            UINT fuEvent;
        };
        struct {
            HWND hWnd;
            UINT_PTR nIDEvent;
            UINT uElapse;
            TIMERPROC lpTimerFunc;
        };
    } t;
} TimerEvent_Type;

typedef struct {
    LPDIRECTDRAWSURFACE lpdds;
    USHORT	uRef;
    USHORT	uRole;
    USHORT	uVersion;
    DWORD	dwCaps;
} SurfaceDB_Type;

typedef struct {
    LPDIRECTDRAWSURFACE lpdds;
    DWORD	dwCaps;
} CapsDB_Type;

typedef enum {
    SURFACE_ROLE_PRIMARY = 0,
    SURFACE_ROLE_BACKBUFFER,
    SURFACE_ROLE_ZBUFFER,
    SURFACE_ROLE_3DREF,
    SURFACE_ROLE_UNKNOWN
} Enum_Surface_Role_Type;

typedef enum {
    WSTYLE_DEFAULT = 0,
    WSTYLE_MODALSTYLE,
    WSTYLE_THICKFRAME,
    WSTYLE_THINFRAME
} Enum_Window_Style_Type;

typedef struct {
    HFONT font;
    HFONT scaled;
} FontDB_Type;

extern dxw_Filter_Type dxwFilters[];
extern dxw_Renderer_Type dxwRenderers[];

class dxwCore {
    // Construction/destruction
public:
    dxwCore();
    virtual ~dxwCore();

    // Operations
public: // methods
    void InitTarget(TARGETMAP *);
    void InitScreenResolutions(void);
    void InitPos(TARGETMAP *);
    void SethWnd(HWND hwnd);
    void SethWnd(HWND hwnd, HWND hwndfps) {
        hWnd = hwnd;
        hWndFPS = hwndfps;
    }
    void AdjustWindowPos(HWND, DWORD, DWORD);
    void SuppressIMEWindow();
    void DisableDWM(HWND);
    void InitWindowPos(int, int, int, int);
    HWND GethWnd(void) {
        return hWnd;
    }
    void SetScreenSize(void);
    void SetScreenSize(int x, int y);
    DWORD GetScreenWidth(void) {
        return dwScreenWidth;
    }
    DWORD GetScreenHeight(void) {
        return dwScreenHeight;
    }
    void SetFullScreen(BOOL);
    BOOL IsFullScreen();
    BOOL IsToRemap(HDC);
    BOOL IsDesktop(HWND);
    BOOL IsRealDesktop(HWND);
    POINT FixCursorPos(POINT);
    void FixCursorClipper(LPPOINT);
    void FixNCHITCursorPos(LPPOINT);
    void SetClipCursor(void);
    void EraseClipCursor(void);
    void InitializeClipCursorState(void);
    BOOL IsClipCursorActive(void);
    BOOL IsValidMainWindow();
    RECT MapWindowRect(LPRECT);
    RECT MapWindowRect(void);
    RECT MapClientRect(LPRECT);
    void MapClient(LPPOINT);
    void MapClient(LPRECT);
    void MapClient(int *, int *, int *, int *);
    void MapClient(short *, short *, short *, short *);
    void MapClient(int *, int *);
    void MapClient(USHORT *, USHORT *); // for SDL calls
    void MapClient(unsigned long *, unsigned long *);
    void MapClient(float *, float *); // for OpenGL calls
    void UnmapClient(LPPOINT);
    void UnmapClient(int *, int *);
    void UnmapClient(int *, int *, int *, int *);
    void UnmapClient(LPRECT);
    void UnmapClient(float *, float *); // for OpenGL calls
    void UnmapClient(double *, double *); // for OpenGL calls
    void MapWindow(LPPOINT);
    void MapWindow(LPRECT);
    void MapWindow(int *, int *, int *, int *);
    void MapWindow(int *, int *);
    void UnmapWindow(LPRECT);
    void UnmapWindow(LPPOINT);
    void UnmapWindow(int *, int *, int *, int *);
    void FixWorkarea(LPRECT);
    void UpdateDesktopCoordinates();
    HRGN MapRegion(char *, HRGN);
    HRGN UnmapRegion(char *, HRGN);
    RECT GetScreenRect(void);
    RECT GetUnmappedScreenRect();
    RECT GetWindowRect(RECT);
    RECT GetClientRect(RECT);
    POINT GetFrameOffset();
    POINT AddCoordinates(POINT, POINT);
    RECT AddCoordinates(RECT, POINT);
    POINT SubCoordinates(POINT, POINT);
    POINT ClientOffset(HWND);
    void AutoScale();
    void ScreenRefresh(void);
    void ScreenRefresh(BOOL);
    BOOL HandleFPS(void);
    void HandleDIB(void);
    void SuspendFPS();
    void ResumeFPS();
    BOOL ishWndFPS(HWND);
    DWORD GetTickCount(void);
    void GetSystemTime(LPSYSTEMTIME);
    void GetSystemTimeAsFileTime(LPFILETIME);
    DWORD StretchTime(DWORD);
    DWORD StretchCounter(DWORD);
    LARGE_INTEGER StretchCounter(LARGE_INTEGER);
    void ShowOverlay();
    void ShowOverlay(HDC);
    void ShowOverlay(HDC, int, int);
    void ShowOverlay(LPDIRECTDRAWSURFACE);
    char *GetTSCaption(void);
    char *GetTSCaption(int);
    void DoSlow(int);
    void ShowBanner(HWND);
    void HideDesktop(HWND);
    POINT ScreenToClient(POINT);
    int GetDLLIndex(char *);
    void SetDLLFlags(int, DWORD);
    void FixStyle(char *, HWND, WPARAM, LPARAM);
    void FixWindowFrame(HWND);
    void HookWindowProc(HWND);
    void AdjustWindowFrame(HWND, DWORD, DWORD);
    void FixWindow(HWND, DWORD, DWORD, int, int, int, int);
    DWORD FixWinStyle(DWORD);
    DWORD FixWinExStyle(DWORD);
    HDC AcquireEmulatedDC(HWND);
    BOOL ReleaseEmulatedDC(HWND);
    BOOL IsVirtual(HDC);
    void ResetEmulatedDC();
    int VirtualOffsetX, VirtualOffsetY, VirtualExtentX, VirtualExtentY;
    void DumpPalette(DWORD, LPPALETTEENTRY);
#ifndef DXW_NOTRACE
    void DumpSysPalette(void);
#endif // DXW_NOTRACES
    void PushTimer(UINT, UINT, UINT, LPTIMECALLBACK, DWORD_PTR, UINT);
    void PushTimer(HWND, UINT_PTR, UINT, TIMERPROC);
    void PopTimer(UINT);
    void PopTimer(HWND, UINT_PTR);
    void RenewTimers();
    void SuspendTimeStretch(BOOL);
    LARGE_INTEGER StretchLargeCounter(LARGE_INTEGER);
    UINT MapKeysConfig(UINT, LPARAM, WPARAM);
    void MapKeysInit();
    void SetVSyncDelays(UINT);
    void VSyncWait();
    void VSyncWaitLine(DWORD);
    void DumpDesktopStatus();
    void ToggleFreezedTime();
    int PickMonitorId(void);
    void CalculateWindowPos(HWND, DWORD, DWORD, LPWINDOWPOS);
    void MessagePump(void);
    void Mark(HDC, BOOL, COLORREF, int, int, int, int);
    void Mark(HDC, BOOL, COLORREF, RECT);
    HWND CreateVirtualDesktop(void);
    void ScaleRelMouse(char *, LPPOINT);
    UINT SetDIBColors(HDC, UINT, UINT, const RGBQUAD *);
    UINT GetDIBColors(HDC, UINT, UINT, const RGBQUAD *);
    LPVOID EmulateDIB(LPVOID, BITMAPINFO *, UINT);
    char FakeHDDrive;
    char FakeCDDrive;
    DWORD dwCurrentFolderType;
    DWORD dwFindFileMapping;
    BOOL bIsRootFolder;
    // needs to have two separate current cd index values because the CD switch will happen separately
    // for audio and data tracks
    short AudioCDIndex;
    short DataCDIndex;
    DWORD dwCDOverlayTimeStamp;
    void ShowCDChanger();
    void ScreenShot();
    void Project(HDC);
    void Project(void);
    LPGUID GetForcedMonitorGUID(void);
    void InitGammaRamp(void);

public: // simple data variables
    int MonitorId;
    BOOL Windowize;
    // v2.04.86: LockedRes is an almost duplicate of Windowize, with the difference that it always keeps its TRUE value, while
    // Windowize is temporarily made to FALSE during dialog box creations to avoid improper coordinate scaling. In practice,
    // Windowize means you should scale coordinates, LockedRes means that the desktop size has to stay steady fixed all time.
    BOOL LockedRes;
    BOOL IsVisible;
    BOOL IsEmulated;
    BOOL MustFixCoordinates;
    DDPIXELFORMAT ActualPixelFormat;
    DDPIXELFORMAT VirtualPixelFormat;
    UINT ReservedPaletteEntries;
    DWORD dwDDVersion;
    DWORD dwTargetDDVersion;
    DWORD MaxDdrawInterface;
    DWORD dwIndex;
    DWORD dwFlags1;
    DWORD dwFlags2;
    DWORD dwFlags3;
    DWORD dwFlags4;
    DWORD dwFlags5;
    DWORD dwFlags6;
    DWORD dwFlags7;
    DWORD dwFlags8;
    DWORD dwFlags9;
    DWORD dwFlags10;
    DWORD dwFlags11;
    DWORD dwFlags12;
    DWORD dwFlags13;
    DWORD dwFlags14;
    DWORD dwFlags15;
    DWORD dwFlags16;
    DWORD dwDFlags;
    DWORD dwDFlags2;
    DWORD dwTFlags;
    HWND hParentWnd;
    HWND hChildWnd;
    BOOL bActive;
    BOOL bDInputAbs;
    DWORD MaxFPS;
    DWORD SwapEffect;
    char *gsModules;
    int TimeShift;
    long iPosX;
    long iPosY;
    long iSizX;
    long iSizY;
    long iPos0X;
    long iPos0Y;
    long iSiz0X;
    long iSiz0Y;
    DWORD iMaxW;
    DWORD iMaxH;
    int iRatioX;
    int iRatioY;
    char *CustomOpenGLLib;
    WORD palVersion;
    WORD palNumEntries;
    PALETTEENTRY palPalEntry[256];
    short FakeVersionId;
    short Coordinates;
    short MaxScreenRes;
    short SlowRatio;
    short ScanLine;
    HDC RealHDC;
    HDC VirtualHDC;
    int GDIEmulationMode;
    BOOL TimeFreeze;
    RECT VirtualDesktop;
    BOOL bHintActive;
    BOOL bAutoScale;
    DWORD dwRealScreenWidth;
    DWORD dwRealScreenHeight;
    _DDCAPS_DX7 DXCapsD1, DXCapsD2;
    LONG dwDefaultScreenWidth;
    LONG dwDefaultScreenHeight;
    LONG dwDefaultColorDepth;
    LONG dxwLastDisplayWidth;
    LONG dxwLastDisplayHeight;
    LONG dxwLastDisplayBPP;
    short FilterId;
    short RendererId;
    dxw_Renderer_Type *Renderer;
    int	FilterXScalingFactor;
    int	FilterYScalingFactor;
    int WindowStyle;
    LPVOID pInitialRamp;
    LPVOID pCurrentRamp;
    BOOL bCustomKeyToggle;
    BOOL bEnableGammaControl;
    short FixedVolume;
    BOOL bTFlags[24 * 2]; // 24 flags doubled for debug flags
    int MaxCDVolume;
    UINT Language;
    UINT CodePage;
    UINT Country;
    DWORD VirtualCDAudioDeviceId;
    BOOL bIsSelectClipRgnRecursed;
    LPGUID lpFakePrimaryMonitorGUID;
    LPDIRECTDRAWSURFACE lpZombieSurface;

    // Implementation
protected:
    LONG dwScreenWidth;
    LONG dwScreenHeight;
    BOOL FullScreen;
    HWND hWnd, hWndFPS;
    HBITMAP VirtualPic;
    RECT VirtualPicRect;

private:
    void GetMonitorWorkarea(LPRECT, BOOL);
    BOOL MustShowOverlay;
    void ShowTextOverlay(HDC, int, int);
    void ShowCDChangerIcon(HDC, int, int);
    TimerEvent_Type TimerEvent;
    DWORD gdwRefreshRate;
#define MAXREFRESHDELAYCOUNT 20
    int iRefreshDelays[MAXREFRESHDELAYCOUNT];
    int iRefreshDelayCount;
    BOOL ClipCursorToggleState;
    DWORD dwFPSFlags;
    int SavedTimeShift;
    int TimeShiftStatus;
    DWORD TimeShiftRedeemTime;
    RGBQUAD *DIBRGBQuadEntries;
};

class dxwSStack {
public:
    dxwSStack();
    virtual ~dxwSStack();

public:
    char *ExplainSurfaceRole(LPDIRECTDRAWSURFACE);
    void ClearSurfaceList();
    void PushPrimarySurface(LPDIRECTDRAWSURFACE, int, DWORD);
    BOOL IsAPrimarySurface(LPDIRECTDRAWSURFACE);
    void UnrefSurface(LPDIRECTDRAWSURFACE);
    LPDIRECTDRAWSURFACE GetPrimarySurface(void);
    void PushBackBufferSurface(LPDIRECTDRAWSURFACE, int, DWORD);
    BOOL IsABackBufferSurface(LPDIRECTDRAWSURFACE);
    LPDIRECTDRAWSURFACE GetZBufferSurface(void);
    void PushZBufferSurface(LPDIRECTDRAWSURFACE, int, DWORD);
    BOOL IsAZBufferSurface(LPDIRECTDRAWSURFACE);
    LPDIRECTDRAWSURFACE GetBackBufferSurface(void);
    void Push3DRefSurface(LPDIRECTDRAWSURFACE, int, DWORD);
    BOOL IsA3DRefSurface(LPDIRECTDRAWSURFACE);
    LPDIRECTDRAWSURFACE Get3DRefBufferSurface(void);
    void PopSurface(LPDIRECTDRAWSURFACE);
    DWORD DuplicateSurface(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE, int);
    DWORD GetCaps(LPDIRECTDRAWSURFACE);
    SurfaceDB_Type *GetSurface(LPDIRECTDRAWSURFACE);

protected:
    SurfaceDB_Type SurfaceDB[DDSQLEN + 1];
    LPDIRECTDRAWSURFACE lpDDSPrimary;
    LPDIRECTDRAWSURFACE lpDDSBackBuffer;
    LPDIRECTDRAWSURFACE lpDDSZBuffer;
    LPDIRECTDRAWSURFACE lpDDS3DRef;

private:
    void PushSurface(LPDIRECTDRAWSURFACE, USHORT, USHORT, DWORD);
    LPDIRECTDRAWSURFACE GetSurfaceByRole(USHORT);
    void SetSurfaceEntry(LPDIRECTDRAWSURFACE, USHORT, USHORT);
};

typedef struct {
    HWND hwnd;
    WNDPROC wndproc;
    int w;
    int h;
} wndstack_entry;

class dxwWStack {
public:
    dxwWStack();
    virtual ~dxwWStack();

public:
    void Put(HWND, WNDPROC, int, int);
    void PutProc(HWND, WNDPROC);
    void PutSize(HWND, int, int);
    BOOL GetSize(HWND, int *, int *);
    WNDPROC GetProc(HWND);

private:
    wndstack_entry *WhndStack;
    int WhndTOS;
    int WhndSize;
};

class dxwFStack {
public:
    dxwFStack();
    virtual ~dxwFStack();

public:
    void Push(HFONT, HFONT);
    HFONT GetScaledFont(HFONT);
    HFONT DeleteFont(HFONT);

protected:
    FontDB_Type *FontDB;
    DWORD StackSize;
};

extern dxwCore dxw;
extern dxwSStack dxwss;
extern dxwWStack dxwws;
extern dxwFStack fontdb;

typedef enum {
    TIMESHIFT_OFF = 0,
    TIMESHIFT_ON,
    TIMESHIFT_REDEEM
};

typedef enum {
    SYSLIBIDX_KERNEL32 = 0,
    SYSLIBIDX_USER32,
    SYSLIBIDX_GDI32,
    SYSLIBIDX_IMELIB,
    SYSLIBIDX_ADVAPI32,
    SYSLIBIDX_OLE32,
    SYSLIBIDX_OLEAUT32, // no hook
    SYSLIBIDX_UXTHEME, // no hook
    SYSLIBIDX_SETUPAPI, // no hook
    SYSLIBIDX_DWMAPI, // no hook
    SYSLIBIDX_DIRECTDRAW,
    SYSLIBIDX_DIRECT3D8,
    SYSLIBIDX_DIRECT3D9,
    SYSLIBIDX_DIRECT3D9_30,
    SYSLIBIDX_DIRECT3D10,
    SYSLIBIDX_DIRECT3D10_1,
    SYSLIBIDX_DIRECT3D11,
    SYSLIBIDX_OPENGL,
    SYSLIBIDX_MSVFW,
    SYSLIBIDX_DSOUND,
    SYSLIBIDX_WINMM,
    SYSLIBIDX_IMM32,
    SYSLIBIDX_DINPUT,
    SYSLIBIDX_DINPUT8,
    SYSLIBIDX_WINTRUST,
    SYSLIBIDX_DIRECT3D,
    SYSLIBIDX_DIRECT3D700,
    SYSLIBIDX_IMAGEHLP,
    SYSLIBIDX_COMDLG32,
    SYSLIBIDX_COMCTL32,
    SYSLIBIDX_AVIFIL32,
    SYSLIBIDX_GLIDE,
    SYSLIBIDX_GLIDE2,
    SYSLIBIDX_GLIDE3,
    SYSLIBIDX_SDL,
    SYSLIBIDX_SDL2,
    SYSLIBIDX_SMACKW32,
    SYSLIBIDX_SMKWAI32,
    SYSLIBIDX_BINKW32,
    SYSLIBIDX_WING32,
    SYSLIBIDX_DPLAYX,
    SYSLIBIDX_WSOCK32,
    SYSLIBIDX_VERSION,
    SYSLIBIDX_WINSPOOL,
    SYSLIBIDX_ZLIB,
    SYSLIBIDX_XINPUT11,
    SYSLIBIDX_XINPUT12,
    SYSLIBIDX_XINPUT13,
    SYSLIBIDX_XINPUT14,
    SYSLIBIDX_XINPUT910,
    SYSLIBIDX_NTDLL,
    SYSLIBIDX_DDRAWEX,
    SYSLIBIDX_DXGI,
    SYSLIBIDX_EARPDS,
    SYSLIBIDX_EARIDS,
    SYSLIBIDX_MSVCRT,
    SYSLIBIDX_KERNELBASE,
    SYSLIBIDX_DSETUP
}
enum_syslibraries;

typedef void (*DxwHookFunction_Type)(HMODULE);
typedef void (*DxwInitFunction_Type)(void);
typedef FARPROC (*RxwRemapFunction_Type)(LPCSTR, HMODULE);

typedef struct {
    char *name;
    BOOL prefixed;
    HMODULE hmodule;
    DWORD flags;
    DxwHookFunction_Type hookf;
    RxwRemapFunction_Type remapf;
} dxwLibsHookTable_Type;

#define DXWHOOK_NULL    0x00000000 // unknown module (or pretending to be such) to be hooked if "Hook all DLL" is set
#define DXWHOOK_HOOK    0x00000001 // the module has its own hook plugin for IAT and GetProcAddress mapping
#define DXWHOOK_EXTEND  0x00000002 // the module needs to extend hooking on all its dependencies
#define DXWHOOK_SKIP    0x00000004 // the module should be ignored
#define DXWHOOK_REPLACE 0x00000008 // the module could have a replacement dll

extern dxwLibsHookTable_Type SysLibsTable[];

enum {
    HINT_HINT = 0,
    HINT_DDRAW,		HINT_D3D8,		HINT_D3D9,		HINT_D3D10,
    HINT_D3D11,		HINT_OPENGL,	HINT_DSOUND,	HINT_DINPUT,
    HINT_DINPUT8,	HINT_MOVIES,	HINT_D3D,		HINT_IHLP,
    HINT_FAKEOS,	HINT_OBFUSCATED, HINT_SAFEDISC,	HINT_SECUROM,
    HINT_LIMITMEM,	HINT_HOOKUPDATE, HINT_XINPUT,
    HINT_LAST
};

enum {
    DXVK_NONE = 0,
    DXVK_CLIPTOGGLE,
    DXVK_REFRESH,
    DXVK_LOGTOGGLE,
    DXVK_PLOCKTOGGLE,
    DXVK_FPSTOGGLE,
    DXVK_TIMEFAST,
    DXVK_TIMESLOW,
    DXVK_TIMETOGGLE,
    DXVK_ALTF4,
    DXVK_PRINTSCREEN,
    DXVK_CORNERIZE,
    DXVK_FREEZETIME,
    DXVK_FULLSCREEN,
    DXVK_FAKEWORKAREA,
    DXVK_FAKEDESKTOP,
    DXVK_CUSTOM,
    DXVK_CDPREV,
    DXVK_CDNEXT,
    DXVK_SIZE
};

enum {
    GDIMODE_NONE = 0,
    GDIMODE_STRETCHED,
    GDIMODE_EMULATED,
    GDIMODE_SHAREDDC
};

typedef enum {
    DXTRACE_DXWND = 0,
    DXTRACE_DDRAW,
    DXTRACE_D3D,
    DXTRACE_WIN,
    DXTRACE_CURSOR,
    DXTRACE_IAT,
    DXTRACE_REGISTRY,
    DXTRACE_HOOK,
    DXTRACE_WING32,
    DXTRACE_OPENGL,
    DXTRACE_SDL,
    DXTRACE_TIME,
    DXTRACE_SOUND,
    DXTRACE_INPUT,
    DXTRACE_SYS,
    DXTRACE_LOCALE,
    DXTRACE_FPS,
    DXTRACE_DEBUG // must be last one
};

enum {
    DXW_NO_FAKE = 0,
    DXW_FAKE_HD,
    DXW_FAKE_CD
};

// defines below to condition debug message handling
#ifndef DXW_NOTRACES
#define ApiName(s) char *api = s
#define ApiArg char *api
#define ApiRef api
#define _if(s) if(s)
#else
#define ApiName(s)
#define ApiArg char *api
#define ApiRef NULL
#define _if(s)
#endif

#ifdef DXW_NOTRACES
#define IsTraceHex FALSE
// t.b.d
#define OutTrace(f, ...)
#define IsTraceDW FALSE
#define IsTraceDDRAW FALSE
#define IsTraceD3D FALSE
#define IsTraceW FALSE
#define IsTraceC FALSE
#define IsTraceIAT FALSE
#define IsTraceR FALSE
#define IsTraceH FALSE
#define IsTraceWG FALSE
#define IsTraceOGL FALSE
#define IsTraceSDL FALSE
#define IsTraceT FALSE
#define IsTraceSND FALSE
#define IsTraceIN FALSE
#define IsTraceSYS FALSE
#define IsTraceLOC FALSE
#define IsTraceFPS FALSE
// -- debug
#define IsDebugDW FALSE
#define IsDebugDDRAW FALSE
#define IsDebugD3D FALSE
#define IsDebugWIN FALSE
#define IsDebugC FALSE
#define IsDebugIAT FALSE
#define IsDebugR FALSE
#define IsDebugH FALSE
#define IsDebugWG FALSE
#define IsDebugOGL FALSE
#define IsDebugSDL FALSE
#define IsDebugT FALSE
#define IsDebugSND FALSE
#define IsDebugIN FALSE
#define IsDebugSYS FALSE
#define IsDebugLOC FALSE
#define IsDebugFPS FALSE
// --- macros
#define OutTraceDW(f, ...)
#define OutTraceDDRAW(f, ...)
#define OutTraceD3D(f, ...)
#define OutTraceW(f, ...)
#define OutTraceC(f, ...)
#define OutTraceIAT(f, ...)
#define OutTraceR(f, ...)
#define OutTraceH(f, ...)
#define OutTraceWG(f, ...)
#define OutTraceOGL(f, ...)
#define OutTraceSDL(f, ...)
#define OutTraceT(f, ...)
#define OutTraceSND(f, ...)
#define OutTraceIN(f, ...)
#define OutTraceSYS(f, ...)
#define OutTraceLOC(f, ...)
#define OutTraceFPS(f, ...)
// -- debug macros
#define OutDebugDW(f, ...)
#define OutDebugDDRAW(f, ...)
#define OutDebugD3D(f, ...)
#define OutDebugW(f, ...)
#define OutDebugC(f, ...)
#define OutDebugIAT(f, ...)
#define OutDebugR(f, ...)
#define OutDebugH(f, ...)
#define OutDebugWG(f, ...)
#define OutDebugOGL(f, ...)
#define OutDebugSDL(f, ...)
#define OutDebugT(f, ...)
#define OutDebugSND(f, ...)
#define OutDebugIN(f, ...)
#define OutDebugSYS(f, ...)
#define OutDebugLOC(f, ...)
#define OutDebugFPS(f, ...)
// -- hexdump macros
#define OutHexDW(p, l)
#define OutHexDDRAW(p, l)
#define OutHexD3D(p, l)
#define OutHexW(p, l)
#define OutHexC(p, l)
#define OutHexIAT(p, l)
#define OutHexR(p, l)
#define OutHexH(p, l)
#define OutHexWG(p, l)
#define OutHexOGL(p, l)
#define OutHexSDL(p, l)
#define OutHexT(p, l)
#define OutHexSND(p, l)
#define OutHexIN(p, l)
#define OutHexSYS(p, l)
#define OutHexLOC(p, l)
// -- options
#define IsTraceE FALSE
#define IsTraceP FALSE
#define OutTraceP(f, ...)
#define OutTraceE(f, ...)
#define IsAssertEnabled FALSE
#define OutTraceB(f, ...)
#define STEP
// -- tracing subroutines
#define DescribeSurface(a, b, c, d)
#define TraceCapsD(a, b)
#define DumpPixFmt(a)
#else
#define IsTraceHex (dxw.dwTFlags & OUTHEXTRACE)
// -- trace info
#define IsTraceDW (dxw.bTFlags[DXTRACE_DXWND])
#define IsTraceDDRAW (dxw.bTFlags[DXTRACE_DDRAW])
#define IsTraceD3D (dxw.bTFlags[DXTRACE_D3D])
#define IsTraceW (dxw.bTFlags[DXTRACE_WIN])
#define IsTraceC (dxw.bTFlags[DXTRACE_CURSOR])
#define IsTraceIAT (dxw.bTFlags[DXTRACE_IAT])
#define IsTraceR (dxw.bTFlags[DXTRACE_REGISTRY])
#define IsTraceH (dxw.bTFlags[DXTRACE_HOOK])
#define IsTraceWG (dxw.bTFlags[DXTRACE_WING32])
#define IsTraceOGL (dxw.bTFlags[DXTRACE_OPENGL])
#define IsTraceSDL (dxw.bTFlags[DXTRACE_SDL])
#define IsTraceT (dxw.bTFlags[DXTRACE_TIME])
#define IsTraceSND (dxw.bTFlags[DXTRACE_SOUND])
#define IsTraceIN (dxw.bTFlags[DXTRACE_INPUT])
#define IsTraceSYS (dxw.bTFlags[DXTRACE_SYS])
#define IsTraceLOC (dxw.bTFlags[DXTRACE_LOCALE])
#define IsTraceFPS (dxw.bTFlags[DXTRACE_FPS])
// -- debug
#define IsDebugDW (dxw.bTFlags[DXTRACE_DXWND+DXTRACE_DEBUG])
#define IsDebugDDRAW (dxw.bTFlags[DXTRACE_DDRAW+DXTRACE_DEBUG])
#define IsDebugD3D (dxw.bTFlags[DXTRACE_D3D+DXTRACE_DEBUG])
#define IsDebugWIN (dxw.bTFlags[DXTRACE_WIN+DXTRACE_DEBUG])
#define IsDebugC (dxw.bTFlags[DXTRACE_CURSOR+DXTRACE_DEBUG])
#define IsDebugIAT (dxw.bTFlags[DXTRACE_IAT+DXTRACE_DEBUG])
#define IsDebugR (dxw.bTFlags[DXTRACE_REGISTRY+DXTRACE_DEBUG])
#define IsDebugH (dxw.bTFlags[DXTRACE_HOOK+DXTRACE_DEBUG])
#define IsDebugWG (dxw.bTFlags[DXTRACE_WING32+DXTRACE_DEBUG])
#define IsDebugOGL (dxw.bTFlags[DXTRACE_OPENGL+DXTRACE_DEBUG])
#define IsDebugSDL (dxw.bTFlags[DXTRACE_SDL+DXTRACE_DEBUG])
#define IsDebugT (dxw.bTFlags[DXTRACE_TIME+DXTRACE_DEBUG])
#define IsDebugSND (dxw.bTFlags[DXTRACE_SOUND+DXTRACE_DEBUG])
#define IsDebugIN (dxw.bTFlags[DXTRACE_INPUT+DXTRACE_DEBUG])
#define IsDebugSYS (dxw.bTFlags[DXTRACE_SYS+DXTRACE_DEBUG])
#define IsDebugLOC (dxw.bTFlags[DXTRACE_LOCALE+DXTRACE_DEBUG])
#define IsDebugFPS (dxw.bTFlags[DXTRACE_FPS+DXTRACE_DEBUG])
// --- macros
#define OutTraceDW(f, ...) if(IsTraceDW) OutTrace(f, __VA_ARGS__)
#define OutTraceDDRAW(f, ...) if(IsTraceDDRAW) OutTrace(f, __VA_ARGS__)
#define OutTraceD3D(f, ...) if(IsTraceD3D) OutTrace(f, __VA_ARGS__)
#define OutTraceW(f, ...) if(IsTraceW) OutTrace(f, __VA_ARGS__)
#define OutTraceC(f, ...) if(IsTraceC) OutTrace(f, __VA_ARGS__)
#define OutTraceIAT(f, ...) if(IsTraceIAT) OutTrace(f, __VA_ARGS__)
#define OutTraceR(f, ...) if(IsTraceR) OutTrace(f, __VA_ARGS__)
#define OutTraceH(f, ...) if(IsTraceH) OutTrace(f, __VA_ARGS__)
#define OutTraceWG(f, ...) if(IsTraceWG) OutTrace(f, __VA_ARGS__)
#define OutTraceOGL(f, ...) if(IsTraceOGL) OutTrace(f, __VA_ARGS__)
#define OutTraceSDL(f, ...) if(IsTraceSDL) OutTrace(f, __VA_ARGS__)
#define OutTraceT(f, ...) if(IsTraceT) OutTrace(f, __VA_ARGS__)
#define OutTraceSND(f, ...) if(IsTraceSND) OutTrace(f, __VA_ARGS__)
#define OutTraceIN(f, ...) if(IsTraceIN) OutTrace(f, __VA_ARGS__)
#define OutTraceSYS(f, ...) if(IsTraceSYS) OutTrace(f, __VA_ARGS__)
#define OutTraceLOC(f, ...) if(IsTraceLOC) OutTrace(f, __VA_ARGS__)
#define OutTraceFPS(f, ...) if(IsTraceFPS) OutTrace(f, __VA_ARGS__)
// -- debug macros
#define OutDebugDW(f, ...) if(IsDebugDW) OutTrace(f, __VA_ARGS__)
#define OutDebugDDRAW(f, ...) if(IsDebugDDRAW) OutTrace(f, __VA_ARGS__)
#define OutDebugD3D(f, ...) if(IsDebugD3D) OutTrace(f, __VA_ARGS__)
#define OutDebugW(f, ...) if(IsDebugWIN) OutTrace(f, __VA_ARGS__)
#define OutDebugC(f, ...) if(IsDebugC) OutTrace(f, __VA_ARGS__)
#define OutDebugIAT(f, ...) if(IsDebugIAT) OutTrace(f, __VA_ARGS__)
#define OutDebugR(f, ...) if(IsDebugR) OutTrace(f, __VA_ARGS__)
#define OutDebugH(f, ...) if(IsDebugH) OutTrace(f, __VA_ARGS__)
#define OutDebugWG(f, ...) if(IsDebugWG) OutTrace(f, __VA_ARGS__)
#define OutDebugOGL(f, ...) if(IsDebugOGL) OutTrace(f, __VA_ARGS__)
#define OutDebugSDL(f, ...) if(IsDebugSDL) OutTrace(f, __VA_ARGS__)
#define OutDebugT(f, ...) if(IsDebugT) OutTrace(f, __VA_ARGS__)
#define OutDebugSND(f, ...) if(IsDebugSND) OutTrace(f, __VA_ARGS__)
#define OutDebugIN(f, ...) if(IsDebugIN) OutTrace(f, __VA_ARGS__)
#define OutDebugSYS(f, ...) if(IsDebugSYS) OutTrace(f, __VA_ARGS__)
#define OutDebugLOC(f, ...) if(IsDebugLOC) OutTrace(f, __VA_ARGS__)
#define OutDebugFPS(f, ...) if(IsDebugFPS) OutTrace(f, __VA_ARGS__)
// -- hexdump macros
#define OutHexDW(p, l) if(IsDebugDW && IsTraceHex) HexTrace(p, l)
#define OutHexDDRAW(p, l) if(IsDebugDDRAW && IsTraceHex) HexTrace(p, l)
#define OutHexD3D(p, l) if(IsDebugD3D && IsTraceHex) HexTrace(p, l)
#define OutHexW(p, l) if(IsDebugWIN && IsTraceHex) HexTrace(p, l)
#define OutHexC(p, l) if(IsDebugC && IsTraceHex) HexTrace(p, l)
#define OutHexIAT(p, l) if(IsDebugIAT && IsTraceHex) HexTrace(p, l)
#define OutHexR(p, l) if(IsDebugR && IsTraceHex) HexTrace(p, l)
#define OutHexH(p, l) if(IsDebugH && IsTraceHex) HexTrace(p, l)
#define OutHexWG(p, l) if(IsDebugWG && IsTraceHex) HexTrace(p, l)
#define OutHexOGL(p, l) if(IsDebugOGL && IsTraceHex) HexTrace(p, l)
#define OutHexSDL(p, l) if(IsDebugSDL && IsTraceHex) HexTrace(p, l)
#define OutHexT(p, l) if(IsDebugT && IsTraceHex) HexTrace(p, l)
#define OutHexSND(p, l) if(IsDebugSND && IsTraceHex) HexTrace(p, l)
#define OutHexIN(p, l) if(IsDebugIN && IsTraceHex) HexTrace(p, l)
#define OutHexSYS(p, l) if(IsDebugSYS && IsTraceHex) HexTrace(p, l)
#define OutHexLOC(p, l) if(IsDebugLOC && IsTraceHex) HexTrace(p, l)
// W.I.P.
// -- options
#define IsTraceE (TRUE)
#define IsTraceP (TRUE)
#define OutTraceP(f, ...) OutTrace(f, __VA_ARGS__)
#define OutTraceE(f, ...) OutTrace(f, __VA_ARGS__)
#define IsAssertEnabled (dxw.dwTFlags & ASSERTDIALOG)
#define STEP OutTrace("STEP at %s:%d\n", __FILE__, __LINE__)
// -- tracing subroutines
extern void DescribeSurface(LPDIRECTDRAWSURFACE, int, char *, int);
extern void DumpPixFmt(LPDDSURFACEDESC2);
extern void DumpCaps(LPDDSURFACEDESC2);
void OutTrace(const char *, ...);
void OutTraceHex(BYTE *, int);
#endif // DXW_NOTRACES

typedef HRESULT (WINAPI *ColorConversion_Type)(int, LPDIRECTDRAWSURFACE, RECT, LPDIRECTDRAWSURFACE *);
typedef UINT (WINAPI *GetPrivateProfileIntA_Type)(LPCTSTR, LPCTSTR, INT, LPCTSTR);
typedef DWORD (WINAPI *GetPrivateProfileStringA_Type)(LPCSTR, LPCSTR, LPCSTR, LPSTR, DWORD, LPCSTR);
extern ColorConversion_Type pColorConversion;
extern void ToggleAcquiredDevices(BOOL);
extern GetWindowLong_Type pGetWindowLong;
extern SetWindowLong_Type pSetWindowLong;
extern GetPrivateProfileIntA_Type pGetPrivateProfileIntA;
extern GetPrivateProfileStringA_Type pGetPrivateProfileStringA;
