#define _CRT_SECURE_NO_WARNINGS

//#define BEST_QUALITY ANTIALIASED_QUALITY
#define BEST_QUALITY CLEARTYPE_NATURAL_QUALITY
//#define TRACEALL
//#define TRACEREGIONS
//#define TRACEBITMAPS
#ifdef TRACEALL
#define TRACEPALETTE
#define TRACEREGIONS
#define TRACEBITMAPS
#endif

#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "hddraw.h"
#include "dxhook.h"
#include "dxhelper.h"
#include "shareddc.hpp"

#include "stdio.h"

#ifndef DXW_NOTRACES
#define TraceError() OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError())
#define IfTraceError() if(!res) TraceError()
extern void DumpDibSection(char *, HDC, const BITMAPINFO *, UINT, VOID *);
extern void DumpBitmap(char *, HBITMAP);
extern void DumpHDC(HDC, int, int, int, int);
#else
#define TraceError()
#define IfTraceError()
#endif

static BOOL bGDIRecursionFlag = FALSE;

static void Stopper(char *s, int line) {
    char sMsg[81];
    sprintf_s(sMsg, 80, "break: \"%s\"", s);
    MessageBox(0, sMsg, "break", MB_OK | MB_ICONEXCLAMATION);
}

//#define STOPPER_TEST // comment out to eliminate
#ifdef STOPPER_TEST
#define STOPPER(s) Stopper(s, __LINE__)
#else
#define STOPPER(s)
#endif
#define _Warn(s) MessageBox(0, s, "to do", MB_ICONEXCLAMATION)

typedef UINT (WINAPI *GetDIBColorTable_Type)(HDC, UINT, UINT, RGBQUAD *);
GetDIBColorTable_Type pGetDIBColorTable = NULL;
UINT WINAPI extGetDIBColorTable(HDC, UINT, UINT, RGBQUAD *);
typedef UINT (WINAPI *SetDIBColorTable_Type)(HDC, UINT, UINT, const RGBQUAD *);
SetDIBColorTable_Type pSetDIBColorTable = NULL;
UINT WINAPI extSetDIBColorTable(HDC, UINT, UINT, const RGBQUAD *);
typedef BOOL (WINAPI *SwapBuffers_Type)(HDC);
SwapBuffers_Type pSwapBuffers;
BOOL WINAPI extSwapBuffers(HDC);
typedef int (WINAPI *GetMapMode_Type)(HDC);
GetMapMode_Type pGetMapMode;
int WINAPI extGetMapMode(HDC);

// -- NLS
typedef HGDIOBJ (WINAPI *GetStockObject_Type)(int);
GetStockObject_Type pGetStockObject;
HGDIOBJ WINAPI extGetStockObject(int);
typedef BOOL (WINAPI *GetCPInfo_Type)(UINT, LPCPINFO);
GetCPInfo_Type pGetCPInfo;
BOOL WINAPI extGetCPInfo(UINT, LPCPINFO);
typedef COLORREF (WINAPI *SetTextColor_Type)(HDC hdc, COLORREF crColor);
SetTextColor_Type pSetTextColor = NULL;
COLORREF WINAPI extSetTextColor(HDC hdc, COLORREF crColor);
//

#ifdef TRACEALL
typedef int (WINAPI *SetBkMode_Type)(HDC, int);
typedef COLORREF (WINAPI *SetBkColor_Type)(HDC, COLORREF);
SetBkMode_Type pSetBkMode = NULL;
SetBkColor_Type pSetBkColor = NULL;
int WINAPI extSetBkMode(HDC, int);
COLORREF WINAPI extSetBkColor(HDC, COLORREF);
typedef BOOL (WINAPI *ScaleWindowExtEx_Type)(HDC, int, int, int, int, LPSIZE);
ScaleWindowExtEx_Type pGDIScaleWindowExtEx;
extern BOOL WINAPI extScaleWindowExtEx(HDC, int, int, int, int, LPSIZE);
typedef HDC	(WINAPI *CreateICA_Type)(LPCTSTR, LPCTSTR, LPCTSTR, const DEVMODEA *);
CreateICA_Type pCreateICA;
HDC WINAPI extCreateICA(LPCTSTR, LPCTSTR, LPCTSTR, const DEVMODEA *);
typedef HDC	(WINAPI *CreateICW_Type)(LPCWSTR, LPCWSTR, LPCWSTR, const DEVMODEW *);
CreateICW_Type pCreateICW;
HDC WINAPI extCreateICW(LPCWSTR, LPCWSTR, LPCWSTR, const DEVMODEW *);
typedef int (WINAPI *GetObject_Type)(HANDLE, int, LPVOID);
GetObject_Type pGetObjectA, pGetObjectW;
int WINAPI extGetObjectA(HANDLE, int, LPVOID);
int WINAPI extGetObjectW(HANDLE, int, LPVOID);
typedef HGDIOBJ (WINAPI *GetCurrentObject_Type)(HDC, UINT);
GetCurrentObject_Type pGetCurrentObject;
HGDIOBJ WINAPI extGetCurrentObject(HDC, UINT);
#endif // TRACEALL

#ifdef TRACEREGIONS
typedef BOOL (WINAPI *PtInRegion_Type)(HRGN, int, int);
typedef int (WINAPI *CombineRgn_Type)(HRGN, HRGN, HRGN, int);
typedef HRGN (WINAPI *CreateEllipticRgn_Type)(int, int, int, int);
typedef HRGN (WINAPI *CreateEllipticRgnIndirect_Type)(const RECT *);
typedef HRGN (WINAPI *CreateRectRgn_Type)(int, int, int, int);
typedef HRGN (WINAPI *CreateRectRgnIndirect_Type)(const RECT *);
typedef HRGN (WINAPI *CreateRoundRectRgn_Type)(int, int, int, int, int, int);
typedef HRGN (WINAPI *CreatePolygonRgn_Type)(const POINT *, int, int);
typedef HRGN (WINAPI *CreatePolyPolygonRgn_Type)(const POINT *, CONST int *, int, int);
typedef BOOL (WINAPI *SetRectRgn_Type)(HRGN, int, int, int, int);
typedef int (WINAPI *OffsetRgn_Type)(HRGN, int, int);
typedef int (WINAPI *GetRgnBox_Type)(HRGN, LPRECT);
typedef int (WINAPI *GetMetaRgn_Type)(HDC, HRGN);
typedef int (WINAPI *SetMetaRgn_Type)(HDC);
typedef int (WINAPI *GetRandomRgn_Type)(HDC, HRGN, INT);

PtInRegion_Type pPtInRegion;
CombineRgn_Type pCombineRgn;
CreateEllipticRgn_Type pCreateEllipticRgn;
CreateEllipticRgnIndirect_Type pCreateEllipticRgnIndirect;
CreateRectRgn_Type pCreateRectRgn;
CreateRectRgnIndirect_Type pCreateRectRgnIndirect;
CreateRoundRectRgn_Type pCreateRoundRectRgn;
CreatePolygonRgn_Type pCreatePolygonRgn;
CreatePolyPolygonRgn_Type pCreatePolyPolygonRgn;
SetRectRgn_Type pSetRectRgn;
OffsetRgn_Type pOffsetRgn;
GetRgnBox_Type pGetRgnBox;
GetMetaRgn_Type pGetMetaRgn;
SetMetaRgn_Type pSetMetaRgn;
GetRandomRgn_Type pGetRandomRgn;

BOOL WINAPI extPtInRegion(HRGN, int, int);
int WINAPI extCombineRgn(HRGN, HRGN, HRGN, int);
HRGN WINAPI extCreateEllipticRgn(int, int, int, int);
HRGN WINAPI extCreateEllipticRgnIndirect(const RECT *);
HRGN WINAPI extCreateRectRgn(int, int, int, int);
HRGN WINAPI extCreateRectRgnIndirect(const RECT *);
HRGN WINAPI extCreateRoundRectRgn(int, int, int, int, int, int);
HRGN WINAPI extCreatePolygonRgn(const POINT *, int, int);
HRGN WINAPI extCreatePolyPolygonRgn(const POINT *, CONST int *, int, int);
BOOL WINAPI extSetRectRgn(HRGN, int, int, int, int);
int WINAPI extOffsetRgn(HRGN, int, int);
int WINAPI extGetRgnBox(HRGN, LPRECT);
int WINAPI extGetMetaRgn(HDC, HRGN);
int WINAPI extSetMetaRgn(HDC);
int WINAPI extGetRandomRgn(HDC, HRGN, INT);
#endif // TRACEREGIONS

#ifdef TRACEBITMAPS
typedef HBITMAP (WINAPI *CreateBitmap_Type)(int, int, UINT, UINT, CONST VOID *);
CreateBitmap_Type pCreateBitmap;
HBITMAP WINAPI extCreateBitmap(int, int, UINT, UINT, CONST VOID *);
#endif

typedef BOOL (WINAPI *PtVisible_Type)(HDC, int, int);
PtVisible_Type pPtVisible;
BOOL WINAPI extPtVisible(HDC, int, int);
typedef BOOL (WINAPI *RectVisible_Type)(HDC, const RECT *);
RectVisible_Type pRectVisible;
BOOL WINAPI extRectVisible(HDC, const RECT *);
typedef BOOL (WINAPI *SelectClipPath_Type)(HDC, int);
SelectClipPath_Type pSelectClipPath;
BOOL WINAPI extSelectClipPath(HDC, int);

typedef BOOL (WINAPI *GetWindowExtEx_Type)(HDC, LPSIZE);
GetWindowExtEx_Type pGetWindowExtEx;
BOOL WINAPI extGetWindowExtEx(HDC, LPSIZE);

#ifdef TRACEFONTS
typedef int	(WINAPI *EnumFontsA_Type)(HDC, LPCSTR, FONTENUMPROC, LPARAM);

EnumFontsA_Type pEnumFontsA;

int WINAPI extEnumFontsA(HDC, LPCSTR, FONTENUMPROC, LPARAM);
#endif // TRACEFONTS

#ifdef TRACEPALETTE
typedef BOOL (WINAPI *ResizePalette_Type)(HPALETTE, UINT);

ResizePalette_Type pResizePalette = NULL;

BOOL WINAPI extResizePalette(HPALETTE, UINT);
#endif // TRACEPALETTE

// to do (?): for virtual CD/HD path translation
// CreateMetaFileA / W  (*)
// CreateEnhMetaFileA / W  (*)
// GetEnhMetaFileA / W
// GetMetaFileA / W
// (*) in "Spearhead"

static HookEntryEx_Type Hooks[] = {
    //{HOOK_IAT_CANDIDATE, 0, "DPtoLP", (FARPROC)DPtoLP, (FARPROC *)&pDPtoLP, (FARPROC)extDPtoLP},

    {HOOK_IAT_CANDIDATE, 0, "GetDeviceCaps", (FARPROC)GetDeviceCaps, (FARPROC *) &pGDIGetDeviceCaps, (FARPROC)extGetDeviceCaps},
    {HOOK_IAT_CANDIDATE, 0, "SaveDC", (FARPROC)SaveDC, (FARPROC *) &pGDISaveDC, (FARPROC)extGDISaveDC},
    {HOOK_IAT_CANDIDATE, 0, "RestoreDC", (FARPROC)RestoreDC, (FARPROC *) &pGDIRestoreDC, (FARPROC)extGDIRestoreDC},
    {HOOK_HOT_CANDIDATE, 0, "AnimatePalette", (FARPROC)AnimatePalette, (FARPROC *) &pAnimatePalette, (FARPROC)extAnimatePalette},
    {HOOK_HOT_CANDIDATE, 0, "CreatePalette", (FARPROC)CreatePalette, (FARPROC *) &pGDICreatePalette, (FARPROC)extGDICreatePalette},
    {HOOK_HOT_CANDIDATE, 0, "SelectPalette", (FARPROC)SelectPalette, (FARPROC *) &pGDISelectPalette, (FARPROC)extSelectPalette},
    {HOOK_HOT_CANDIDATE, 0, "RealizePalette", (FARPROC)RealizePalette, (FARPROC *) &pGDIRealizePalette, (FARPROC)extRealizePalette},
    {HOOK_HOT_CANDIDATE, 0, "GetSystemPaletteEntries", (FARPROC)GetSystemPaletteEntries, (FARPROC *) &pGDIGetSystemPaletteEntries, (FARPROC)extGetSystemPaletteEntries},
    {HOOK_HOT_CANDIDATE, 0, "SetSystemPaletteUse", (FARPROC)SetSystemPaletteUse, (FARPROC *) &pSetSystemPaletteUse, (FARPROC)extSetSystemPaletteUse},
    {HOOK_HOT_CANDIDATE, 0, "SetPixelFormat", (FARPROC)NULL, (FARPROC *) &pGDISetPixelFormat, (FARPROC)extGDISetPixelFormat},
    {HOOK_IAT_CANDIDATE, 0, "GetPixelFormat", (FARPROC)NULL, (FARPROC *) &pGDIGetPixelFormat, (FARPROC)extGDIGetPixelFormat},
    {HOOK_IAT_CANDIDATE, 0, "ChoosePixelFormat", (FARPROC)NULL, (FARPROC *) &pChoosePixelFormat, (FARPROC)extChoosePixelFormat},
    {HOOK_IAT_CANDIDATE, 0, "DescribePixelFormat", (FARPROC)NULL, (FARPROC *) &pDescribePixelFormat, (FARPROC)extDescribePixelFormat},
    {HOOK_HOT_CANDIDATE, 0, "GetPaletteEntries", (FARPROC)GetPaletteEntries, (FARPROC *) &pGetPaletteEntries, (FARPROC)extGetPaletteEntries},
    {HOOK_HOT_CANDIDATE, 0, "SetPaletteEntries", (FARPROC)SetPaletteEntries, (FARPROC *) &pSetPaletteEntries, (FARPROC)extSetPaletteEntries},
    {HOOK_HOT_CANDIDATE, 0, "GetSystemPaletteUse", (FARPROC)GetSystemPaletteUse, (FARPROC *) &pGetSystemPaletteUse, (FARPROC)extGetSystemPaletteUse},
    {HOOK_HOT_CANDIDATE, 0, "SetROP2", (FARPROC)SetROP2, (FARPROC *) &pSetROP2, (FARPROC)extSetROP2}, // Titanic
    {HOOK_HOT_CANDIDATE, 0, "GetDIBColorTable", (FARPROC)GetDIBColorTable, (FARPROC *) &pGetDIBColorTable, (FARPROC)extGetDIBColorTable},
    {HOOK_HOT_CANDIDATE, 0, "SetDIBColorTable", (FARPROC)SetDIBColorTable, (FARPROC *) &pSetDIBColorTable, (FARPROC)extSetDIBColorTable},
#ifdef TRACEPALETTE
    {HOOK_IAT_CANDIDATE, 0, "ResizePalette", (FARPROC)ResizePalette, (FARPROC *) &pResizePalette, (FARPROC)extResizePalette},
#endif
    //
    // regions related functions unrelated with DC handles
    //
#ifdef TRACEREGIONS
    {HOOK_IAT_CANDIDATE, 0, "CombineRgn", (FARPROC)CombineRgn, (FARPROC *) &pCombineRgn, (FARPROC)extCombineRgn},
    // commented out since they alter text on screen...... (see Imperialism II difficulty level menu)
    // v2.03.47 - restored: needed for "688(I) Hunter Killer" periscope ....
    {HOOK_IAT_CANDIDATE, 0, "CreateEllipticRgn", (FARPROC)CreateEllipticRgn, (FARPROC *) &pCreateEllipticRgn, (FARPROC)extCreateEllipticRgn},
    {HOOK_IAT_CANDIDATE, 0, "CreateEllipticRgnIndirect", (FARPROC)CreateEllipticRgnIndirect, (FARPROC *) &pCreateEllipticRgnIndirect, (FARPROC)extCreateEllipticRgnIndirect},
    {HOOK_IAT_CANDIDATE, 0, "CreateRectRgn", (FARPROC)CreateRectRgn, (FARPROC *) &pCreateRectRgn, (FARPROC)extCreateRectRgn},
    {HOOK_IAT_CANDIDATE, 0, "CreateRectRgnIndirect", (FARPROC)CreateRectRgnIndirect, (FARPROC *) &pCreateRectRgnIndirect, (FARPROC)extCreateRectRgnIndirect},
    {HOOK_IAT_CANDIDATE, 0, "CreateRoundRectRgn", (FARPROC)CreateRoundRectRgn, (FARPROC *) &pCreateRoundRectRgn, (FARPROC)extCreateRoundRectRgn},
    {HOOK_IAT_CANDIDATE, 0, "CreatePolygonRgn", (FARPROC)CreatePolygonRgn, (FARPROC *) &pCreatePolygonRgn, (FARPROC)extCreatePolygonRgn},
    {HOOK_IAT_CANDIDATE, 0, "CreatePolyPolygonRgn", (FARPROC)CreatePolyPolygonRgn, (FARPROC *) &pCreatePolyPolygonRgn, (FARPROC)extCreatePolyPolygonRgn},
    {HOOK_IAT_CANDIDATE, 0, "PtInRegion", (FARPROC)PtInRegion, (FARPROC *) &pPtInRegion, (FARPROC)extPtInRegion},
    {HOOK_IAT_CANDIDATE, 0, "SetRectRgn", (FARPROC)SetRectRgn, (FARPROC *) &pSetRectRgn, (FARPROC)extSetRectRgn},
    {HOOK_IAT_CANDIDATE, 0, "GetRgnBox", (FARPROC)GetRgnBox, (FARPROC *) &pGetRgnBox, (FARPROC)extGetRgnBox},
    {HOOK_IAT_CANDIDATE, 0, "GetRandomRgn", (FARPROC)GetRandomRgn, (FARPROC *) &pGetRandomRgn, (FARPROC)extGetRandomRgn},
    {HOOK_IAT_CANDIDATE, 0, "GetMetaRgn", (FARPROC)GetMetaRgn, (FARPROC *) &pGetMetaRgn, (FARPROC)extGetMetaRgn},
    {HOOK_IAT_CANDIDATE, 0, "SetMetaRgn", (FARPROC)SetMetaRgn, (FARPROC *) &pSetMetaRgn, (FARPROC)extSetMetaRgn},
#endif // TRACEREGIONS
#ifdef TRACEALL
    {HOOK_HOT_CANDIDATE, 0, "SetBkColor", (FARPROC)SetBkColor, (FARPROC *) &pSetBkColor, (FARPROC)extSetBkColor},
    {HOOK_HOT_CANDIDATE, 0, "SetBkMode", (FARPROC)SetBkMode, (FARPROC *) &pSetBkMode, (FARPROC)extSetBkMode},
    {HOOK_IAT_CANDIDATE, 0, "ScaleWindowExtEx", (FARPROC)ScaleWindowExtEx, (FARPROC *) &pGDIScaleWindowExtEx, (FARPROC)extScaleWindowExtEx},
    {HOOK_HOT_CANDIDATE, 0, "CreateICA", (FARPROC)CreateICA, (FARPROC *) &pCreateICA, (FARPROC)extCreateICA}, // Riven, Everquest
    {HOOK_HOT_CANDIDATE, 0, "CreateICW", (FARPROC)CreateICW, (FARPROC *) &pCreateICW, (FARPROC)extCreateICW}, // unseen ...
    {HOOK_HOT_CANDIDATE, 0, "GetObjectA", (FARPROC)GetObjectA, (FARPROC *) &pGetObjectA, (FARPROC)extGetObjectA}, //
    {HOOK_HOT_CANDIDATE, 0, "GetObjectW", (FARPROC)GetObjectW, (FARPROC *) &pGetObjectW, (FARPROC)extGetObjectW}, //
    {HOOK_HOT_CANDIDATE, 0, "GetCurrentObject", (FARPROC)GetCurrentObject, (FARPROC *) &pGetCurrentObject, (FARPROC)extGetCurrentObject}, //
#endif // TRACEALL
#ifdef TRACEBITMAPS
    {HOOK_IAT_CANDIDATE, 0, "CreateBitmap", (FARPROC)CreateBitmap, (FARPROC *) &pCreateBitmap, (FARPROC)extCreateBitmap},
#endif // TRACEBITMAPS
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type RemapHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "SetViewportOrgEx", (FARPROC)SetViewportOrgEx, (FARPROC *) &pSetViewportOrgEx, (FARPROC)extSetViewportOrgEx}, // needed in ShowBanner
    {HOOK_IAT_CANDIDATE, 0, "SetViewportExtEx", (FARPROC)SetViewportExtEx, (FARPROC *) &pSetViewportExtEx, (FARPROC)extSetViewportExtEx},
    {HOOK_IAT_CANDIDATE, 0, "GetViewportOrgEx", (FARPROC)GetViewportOrgEx, (FARPROC *) &pGetViewportOrgEx, (FARPROC)extGetViewportOrgEx},
    {HOOK_IAT_CANDIDATE, 0, "GetViewportExtEx", (FARPROC)GetViewportExtEx, (FARPROC *) &pGetViewportExtEx, (FARPROC)extGetViewportExtEx},
    {HOOK_IAT_CANDIDATE, 0, "GetWindowOrgEx", (FARPROC)GetWindowOrgEx, (FARPROC *) &pGetWindowOrgEx, (FARPROC)extGetWindowOrgEx},
    {HOOK_IAT_CANDIDATE, 0, "SetWindowOrgEx", (FARPROC)SetWindowOrgEx, (FARPROC *) &pSetWindowOrgEx, (FARPROC)extSetWindowOrgEx},
    {HOOK_HOT_CANDIDATE, 0, "GetWindowExtEx", (FARPROC)GetWindowExtEx, (FARPROC *) &pGetWindowExtEx, (FARPROC)extGetWindowExtEx},
    {HOOK_IAT_CANDIDATE, 0, "SetWindowExtEx", (FARPROC)SetWindowExtEx, (FARPROC *) &pSetWindowExtEx, (FARPROC)extSetWindowExtEx},
    {HOOK_IAT_CANDIDATE, 0, "GetCurrentPositionEx", (FARPROC)GetCurrentPositionEx, (FARPROC *) &pGetCurrentPositionEx, (FARPROC)extGetCurrentPositionEx},
    //{HOOK_IAT_CANDIDATE, 0, "GetRegionData", (FARPROC)NULL, (FARPROC *)&pGetRegionData, (FARPROC)extGetRegionData},
    {HOOK_IAT_CANDIDATE, 0, "CreateCompatibleDC", (FARPROC)CreateCompatibleDC, (FARPROC *) &pGDICreateCompatibleDC, (FARPROC)extGDICreateCompatibleDC}, /* to check */
    //TODO {HOOK_IAT_CANDIDATE, 0, "DrawEscape", (FARPROC)DrawEscape, (FARPROC *)&pDrawEscape, (FARPROC)extDrawEscape}, /* to check */
    {HOOK_IAT_CANDIDATE, 0, "GetDCOrgEx", (FARPROC)GetDCOrgEx, (FARPROC *) &pGetDCOrgEx, (FARPROC)extGetDCOrgEx},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type SyscallHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "StretchDIBits", (FARPROC)StretchDIBits, (FARPROC *) &pStretchDIBits, (FARPROC)extStretchDIBits},
    {HOOK_HOT_CANDIDATE, 0, "SetDIBits", (FARPROC)SetDIBits, (FARPROC *) &pSetDIBits, (FARPROC)extSetDIBits},
    {HOOK_HOT_CANDIDATE, 0, "GetDIBits", (FARPROC)GetDIBits, (FARPROC *) &pGetDIBits, (FARPROC)extGetDIBits},
    {HOOK_IAT_CANDIDATE, 0, "CreateCompatibleBitmap", (FARPROC)CreateCompatibleBitmap, (FARPROC *) &pCreateCompatibleBitmap, (FARPROC)extCreateCompatibleBitmap},
    {HOOK_IAT_CANDIDATE, 0, "CreateDIBitmap", (FARPROC)NULL, (FARPROC *) &pCreateDIBitmap, (FARPROC)extCreateDIBitmap},
    {HOOK_HOT_CANDIDATE, 0, "CreateDIBSection", (FARPROC)CreateDIBSection, (FARPROC *) &pCreateDIBSection, (FARPROC)extCreateDIBSection},
    {HOOK_IAT_CANDIDATE, 0, "CreateDiscardableBitmap", (FARPROC)NULL, (FARPROC *) &pCreateDiscardableBitmap, (FARPROC)extCreateDiscardableBitmap},
    {HOOK_IAT_CANDIDATE, 0, "ExtFloodFill", (FARPROC)NULL, (FARPROC *) &pExtFloodFill, (FARPROC)extExtFloodFill},
    {HOOK_IAT_CANDIDATE, 0, "GdiAlphaBlend", (FARPROC)NULL, (FARPROC *) &pGdiAlphaBlend, (FARPROC)extGdiAlphaBlend},
    {HOOK_IAT_CANDIDATE, 0, "GdiGradientFill", (FARPROC)NULL, (FARPROC *) &pGdiGradientFill, (FARPROC)extGdiGradientFill},
    {HOOK_IAT_CANDIDATE, 0, "GdiTransparentBlt", (FARPROC)NULL, (FARPROC *) &pGdiTransparentBlt, (FARPROC)extGdiTransparentBlt},
    {HOOK_IAT_CANDIDATE, 0, "Pie", (FARPROC)NULL, (FARPROC *) &pPie, (FARPROC)extPie},
    {HOOK_IAT_CANDIDATE, 0, "AngleArc", (FARPROC)NULL, (FARPROC *) &pAngleArc, (FARPROC)extAngleArc},
    {HOOK_IAT_CANDIDATE, 0, "PolyPolyline", (FARPROC)NULL, (FARPROC *) &pPolyPolyline, (FARPROC)extPolyPolyline},
    {HOOK_IAT_CANDIDATE, 0, "FillRgn", (FARPROC)NULL, (FARPROC *) &pFillRgn, (FARPROC)extFillRgn},
    {HOOK_IAT_CANDIDATE, 0, "FrameRgn", (FARPROC)NULL, (FARPROC *) &pFrameRgn, (FARPROC)extFrameRgn},
    {HOOK_IAT_CANDIDATE, 0, "InvertRgn", (FARPROC)NULL, (FARPROC *) &pInvertRgn, (FARPROC)extInvertRgn},
    {HOOK_IAT_CANDIDATE, 0, "PaintRgn", (FARPROC)NULL, (FARPROC *) &pPaintRgn, (FARPROC)extPaintRgn},
    {HOOK_IAT_CANDIDATE, 0, "SetMapMode", (FARPROC)NULL, (FARPROC *) &pSetMapMode, (FARPROC)extSetMapMode},
    {HOOK_IAT_CANDIDATE, 0, "GetMapMode", (FARPROC)NULL, (FARPROC *) &pGetMapMode, (FARPROC)extGetMapMode},
    {HOOK_IAT_CANDIDATE, 0, "SetDIBitsToDevice", (FARPROC)SetDIBitsToDevice, (FARPROC *) &pSetDIBitsToDevice, (FARPROC)extSetDIBitsToDevice}, // does the stretching
    {HOOK_IAT_CANDIDATE, 0, "Polyline", (FARPROC)Polyline, (FARPROC *) &pPolyline, (FARPROC)extPolyline},
    {HOOK_IAT_CANDIDATE, 0, "BitBlt", (FARPROC)BitBlt, (FARPROC *) &pGDIBitBlt, (FARPROC)extGDIBitBlt},
    {HOOK_IAT_CANDIDATE, 0, "StretchBlt", (FARPROC)StretchBlt, (FARPROC *) &pGDIStretchBlt, (FARPROC)extGDIStretchBlt},
    {HOOK_IAT_CANDIDATE, 0, "PatBlt", (FARPROC)PatBlt, (FARPROC *) &pGDIPatBlt, (FARPROC)extGDIPatBlt},
    {HOOK_IAT_CANDIDATE, 0, "MaskBlt", (FARPROC)MaskBlt, (FARPROC *) &pMaskBlt, (FARPROC)extMaskBlt},
    {HOOK_IAT_CANDIDATE, 0, "TextOutA", (FARPROC)TextOutA, (FARPROC *) &pGDITextOutA, (FARPROC)extTextOutA},
    {HOOK_IAT_CANDIDATE, 0, "TextOutW", (FARPROC)TextOutW, (FARPROC *) &pGDITextOutW, (FARPROC)extTextOutW},
    {HOOK_IAT_CANDIDATE, 0, "Rectangle", (FARPROC)Rectangle, (FARPROC *) &pGDIRectangle, (FARPROC)extRectangle},
    {HOOK_IAT_CANDIDATE, 0, "RoundRect", (FARPROC)RoundRect, (FARPROC *) &pRoundRect, (FARPROC)extRoundRect},
    {HOOK_IAT_CANDIDATE, 0, "Polygon", (FARPROC)Polygon, (FARPROC *) &pPolygon, (FARPROC)extPolygon},
    {HOOK_IAT_CANDIDATE, 0, "PolyPolygon", (FARPROC)PolyPolygon, (FARPROC *) &pPolyPolygon, (FARPROC)extPolyPolygon},
    {HOOK_IAT_CANDIDATE, 0, "PolyBezier", (FARPROC)PolyBezier, (FARPROC *) &pPolyBezier, (FARPROC)extPolyBezier},
    {HOOK_IAT_CANDIDATE, 0, "PolyBezierTo", (FARPROC)PolyBezierTo, (FARPROC *) &pPolyBezierTo, (FARPROC)extPolyBezierTo},
    {HOOK_IAT_CANDIDATE, 0, "PolylineTo", (FARPROC)PolylineTo, (FARPROC *) &pPolylineTo, (FARPROC)extPolylineTo},
    {HOOK_IAT_CANDIDATE, 0, "PolyDraw", (FARPROC)PolyDraw, (FARPROC *) &pPolyDraw, (FARPROC)extPolyDraw},
    {HOOK_IAT_CANDIDATE, 0, "GetPixel", (FARPROC)GetPixel, (FARPROC *) &pGetPixel, (FARPROC)extGetPixel},
    {HOOK_IAT_CANDIDATE, 0, "PlgBlt", (FARPROC)PlgBlt, (FARPROC *) &pPlgBlt, (FARPROC)extPlgBlt},
    {HOOK_IAT_CANDIDATE, 0, "SetPixel", (FARPROC)SetPixel, (FARPROC *) &pSetPixel, (FARPROC)extSetPixel},
    {HOOK_IAT_CANDIDATE, 0, "SetPixelV", (FARPROC)SetPixelV, (FARPROC *) &pSetPixelV, (FARPROC)extSetPixelV},
    {HOOK_IAT_CANDIDATE, 0, "Chord", (FARPROC)Chord, (FARPROC *) &pChord, (FARPROC)extChord},
    {HOOK_IAT_CANDIDATE, 0, "Ellipse", (FARPROC)Ellipse, (FARPROC *) &pEllipse, (FARPROC)extEllipse},
    {HOOK_IAT_CANDIDATE, 0, "ExtTextOutA", (FARPROC)ExtTextOutA, (FARPROC *) &pExtTextOutA, (FARPROC)extExtTextOutA},
    {HOOK_IAT_CANDIDATE, 0, "ExtTextOutW", (FARPROC)ExtTextOutW, (FARPROC *) &pExtTextOutW, (FARPROC)extExtTextOutW},
    {HOOK_IAT_CANDIDATE, 0, "PolyTextOutA", (FARPROC)PolyTextOutA, (FARPROC *) &pPolyTextOutA, (FARPROC)extPolyTextOutA},
    {HOOK_IAT_CANDIDATE, 0, "PolyTextOutW", (FARPROC)PolyTextOutA, (FARPROC *) &pPolyTextOutA, (FARPROC)extPolyTextOutA},
    {HOOK_IAT_CANDIDATE, 0, "ArcTo", (FARPROC)ArcTo, (FARPROC *) &pArcTo, (FARPROC)extArcTo},
    {HOOK_IAT_CANDIDATE, 0, "LineTo", (FARPROC)LineTo, (FARPROC *) &pLineTo, (FARPROC)extLineTo},
    {HOOK_IAT_CANDIDATE, 0, "Arc", (FARPROC)Arc, (FARPROC *) &pArc, (FARPROC)extArc},
    {HOOK_IAT_CANDIDATE, 0, "MoveToEx", (FARPROC)MoveToEx, (FARPROC *) &pMoveToEx, (FARPROC)extMoveToEx},
    //{HOOK_IAT_CANDIDATE, 0, "DeleteDC", (FARPROC)DeleteDC, (FARPROC *)&pGDIDeleteDC, (FARPROC)extGDIDeleteDC}, // for tracing only! (commented: crashes Dylan Dog HLP!!)
    {HOOK_IAT_CANDIDATE, 0, "PlayEnhMetaFile", (FARPROC)PlayEnhMetaFile, (FARPROC *) &pPlayEnhMetaFile, (FARPROC)extPlayEnhMetaFile},
    {HOOK_HOT_CANDIDATE, 0, "SetTextColor", (FARPROC)SetTextColor, (FARPROC *) &pSetTextColor, (FARPROC)extSetTextColor},
    //
    // clip region related functions
    //
    {HOOK_HOT_CANDIDATE, 0, "GetClipRgn", (FARPROC)GetClipRgn, (FARPROC *) &pGetClipRgn, (FARPROC)extGetClipRgn},
    {HOOK_HOT_CANDIDATE, 0, "SelectClipRgn", (FARPROC)SelectClipRgn, (FARPROC *) &pSelectClipRgn, (FARPROC)extSelectClipRgn}, // Sid Meier's Alpha Centaury
    {HOOK_HOT_CANDIDATE, 0, "ExtSelectClipRgn", (FARPROC)ExtSelectClipRgn, (FARPROC *) &pExtSelectClipRgn, (FARPROC)extExtSelectClipRgn},
    {HOOK_HOT_CANDIDATE, 0, "OffsetClipRgn", (FARPROC)OffsetClipRgn, (FARPROC *) &pOffsetClipRgn, (FARPROC)extOffsetClipRgn},
    {HOOK_HOT_CANDIDATE, 0, "IntersectClipRect", (FARPROC)IntersectClipRect, (FARPROC *) &pIntersectClipRect, (FARPROC)extIntersectClipRect}, // Riven !!
    {HOOK_HOT_CANDIDATE, 0, "ExcludeClipRect", (FARPROC)ExcludeClipRect, (FARPROC *) &pExcludeClipRect, (FARPROC)extExcludeClipRect},
    {HOOK_HOT_CANDIDATE, 0, "GetClipBox", (FARPROC)GetClipBox, (FARPROC *) &pGDIGetClipBox, (FARPROC)extGetClipBox},
    {HOOK_HOT_CANDIDATE, 0, "PtVisible", (FARPROC)PtVisible, (FARPROC *) &pPtVisible, (FARPROC)extPtVisible},
    {HOOK_HOT_CANDIDATE, 0, "RectVisible", (FARPROC)RectVisible, (FARPROC *) &pRectVisible, (FARPROC)extRectVisible},
    {HOOK_HOT_CANDIDATE, 0, "SelectClipPath", (FARPROC)SelectClipPath, (FARPROC *) &pSelectClipPath, (FARPROC)extSelectClipPath},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type CreateDCHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "CreateDCA", (FARPROC)CreateDCA, (FARPROC *) &pGDICreateDCA, (FARPROC)extGDICreateDCA},
    {HOOK_IAT_CANDIDATE, 0, "CreateDCW", (FARPROC)CreateDCW, (FARPROC *) &pGDICreateDCW, (FARPROC)extGDICreateDCW},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type EmulateHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "GetObjectType", (FARPROC)GetObjectType, (FARPROC *) &pGetObjectType, (FARPROC)extGetObjectType},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type TextHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "CreateFontA", (FARPROC)CreateFont, (FARPROC *) &pGDICreateFontA, (FARPROC)extCreateFontA},
    {HOOK_HOT_CANDIDATE, 0, "CreateFontIndirectA", (FARPROC)CreateFontIndirectA, (FARPROC *) &pGDICreateFontIndirectA, (FARPROC)extCreateFontIndirectA},
    {HOOK_IAT_CANDIDATE, 0, "GetTextExtentPointA", (FARPROC)NULL, (FARPROC *) &pGetTextExtentPointA, (FARPROC)extGetTextExtentPointA},
    {HOOK_IAT_CANDIDATE, 0, "GetTextExtentPoint32A", (FARPROC)NULL, (FARPROC *) &pGetTextExtentPoint32A, (FARPROC)extGetTextExtentPoint32A},
#ifdef TRACEFONTS
    {HOOK_HOT_CANDIDATE, 0, "EnumFontsA", (FARPROC)EnumFontsA, (FARPROC *) &pEnumFontsA, (FARPROC)extEnumFontsA}, // Titanic
#endif
    {HOOK_HOT_CANDIDATE, 0, "SelectObject", (FARPROC)SelectObject, (FARPROC *) &pSelectObject, (FARPROC)extSelectObject}, // font scaling ....
    {HOOK_HOT_CANDIDATE, 0, "DeleteObject", (FARPROC)DeleteObject, (FARPROC *) &pDeleteObject, (FARPROC)extDeleteObject}, // font scaling ....
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type TextSizeHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "CreateFontA", (FARPROC)CreateFont, (FARPROC *) &pGDICreateFontA, (FARPROC)extCreateFontA},
    {HOOK_HOT_CANDIDATE, 0, "CreateFontIndirectA", (FARPROC)CreateFontIndirectA, (FARPROC *) &pGDICreateFontIndirectA, (FARPROC)extCreateFontIndirectA},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type GammaHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "SetDeviceGammaRamp", (FARPROC)SetDeviceGammaRamp, (FARPROC *) &pGDISetDeviceGammaRamp, (FARPROC)extSetDeviceGammaRamp},
    {HOOK_IAT_CANDIDATE, 0, "GetDeviceGammaRamp", (FARPROC)GetDeviceGammaRamp, (FARPROC *) &pGDIGetDeviceGammaRamp, (FARPROC)extGetDeviceGammaRamp},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type FontHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "CreateScalableFontResourceA", (FARPROC)CreateScalableFontResourceA, (FARPROC *) &pCreateScalableFontResourceA, (FARPROC)extCreateScalableFontResourceA},
    {HOOK_IAT_CANDIDATE, 0, "CreateScalableFontResourceW", (FARPROC)CreateScalableFontResourceW, (FARPROC *) &pCreateScalableFontResourceW, (FARPROC)extCreateScalableFontResourceW},
    {HOOK_IAT_CANDIDATE, 0, "AddFontResourceA", (FARPROC)AddFontResourceA, (FARPROC *) &pAddFontResourceA, (FARPROC)extAddFontResourceA},
    {HOOK_IAT_CANDIDATE, 0, "AddFontResourceW", (FARPROC)AddFontResourceW, (FARPROC *) &pAddFontResourceW, (FARPROC)extAddFontResourceW},
    // v2.04.05: Used by "Warhammer: Shadow of the Horned Rat"
    {HOOK_IAT_CANDIDATE, 0, "RemoveFontResourceA", (FARPROC)RemoveFontResourceA, (FARPROC *) &pRemoveFontResourceA, (FARPROC)extRemoveFontResourceA},
    {HOOK_IAT_CANDIDATE, 0, "RemoveFontResourceW", (FARPROC)RemoveFontResourceW, (FARPROC *) &pRemoveFontResourceW, (FARPROC)extRemoveFontResourceW},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type NlsHooks[] = {
    // commented: interferes with "Swat" ...
    //	{HOOK_HOT_REQUIRED, 0, "GetStockObject", (FARPROC)GetStockObject, (FARPROC *)&pGetStockObject, (FARPROC)extGetStockObject},
    {HOOK_HOT_REQUIRED, 0, "GetCPInfo", (FARPROC)GetCPInfo, (FARPROC *) &pGetCPInfo, (FARPROC)extGetCPInfo},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type ProjectHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "SwapBuffers", (FARPROC)SwapBuffers, (FARPROC *) &pSwapBuffers, (FARPROC)extSwapBuffers},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

extern HRESULT WINAPI extDirectDrawCreate(GUID FAR *, LPDIRECTDRAW FAR *, IUnknown FAR *);
extern HRESULT WINAPI extDirectDrawCreateEx(GUID FAR *, LPDIRECTDRAW FAR *, REFIID, IUnknown FAR *);

static char *libname = "gdi32.dll";

void HookGDI32Init() {
    HookLibInitEx(Hooks);
    HookLibInitEx(RemapHooks);
    HookLibInitEx(SyscallHooks);
    HookLibInitEx(EmulateHooks);
    HookLibInitEx(TextHooks);
    HookLibInitEx(GammaHooks);
}

void HookGDI32(HMODULE module) {
    HookLibraryEx(module, Hooks, libname);
    if (dxw.GDIEmulationMode != GDIMODE_NONE) HookLibraryEx(module, SyscallHooks, libname);
    if (dxw.dwFlags1 & CLIENTREMAPPING)		HookLibraryEx(module, RemapHooks, libname);
    if (dxw.dwFlags3 & GDIEMULATEDC)		HookLibraryEx(module, EmulateHooks, libname);
    if (dxw.dwFlags1 & FIXTEXTOUT)			HookLibraryEx(module, TextHooks, libname);
    if ((dxw.dwFlags11 & SHRINKFONTWIDTH) || (dxw.dwFlags8 & QUALITYFONTS))
        HookLibraryEx(module, TextSizeHooks, libname);
    if (dxw.dwFlags2 & DISABLEGAMMARAMP)	HookLibraryEx(module, GammaHooks, libname);
    // v2.02.33 - for "Stratego" compatibility option
    if(dxw.dwFlags3 & FONTBYPASS)			HookLibraryEx(module, FontHooks, libname);
    if(dxw.dwFlags10 & CREATEDCHOOK)		HookLibraryEx(module, CreateDCHooks, libname);
    if(dxw.dwFlags11 & CUSTOMLOCALE)		HookLibraryEx(module, NlsHooks, libname);
    if(dxw.dwFlags12 & PROJECTBUFFER)			HookLibraryEx(module, ProjectHooks, libname);
}

FARPROC Remap_GDI32_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if(addr = RemapLibraryEx(proc, hModule, Hooks)) return addr;
    if (dxw.GDIEmulationMode != GDIMODE_NONE) if(addr = RemapLibraryEx(proc, hModule, SyscallHooks)) return addr;
    if (dxw.dwFlags1 & CLIENTREMAPPING)		if(addr = RemapLibraryEx(proc, hModule, RemapHooks)) return addr;
    if (dxw.dwFlags3 & GDIEMULATEDC)		if (addr = RemapLibraryEx(proc, hModule, EmulateHooks)) return addr;
    if (dxw.dwFlags1 & FIXTEXTOUT)			if(addr = RemapLibraryEx(proc, hModule, TextHooks)) return addr;
    if ((dxw.dwFlags11 & SHRINKFONTWIDTH) || (dxw.dwFlags8 & QUALITYFONTS))
        if(addr = RemapLibraryEx(proc, hModule, TextSizeHooks)) return addr;
    if (dxw.dwFlags2 & DISABLEGAMMARAMP)	if(addr = RemapLibraryEx(proc, hModule, GammaHooks)) return addr;
    // v2.02.33 - for "Stratego" compatibility option
    if (dxw.dwFlags3 & FONTBYPASS)			if(addr = RemapLibraryEx(proc, hModule, FontHooks)) return addr;
    if (dxw.dwFlags10 & CREATEDCHOOK)		if(addr = RemapLibraryEx(proc, hModule, CreateDCHooks)) return addr;
    if (dxw.dwFlags11 & CUSTOMLOCALE)		if(addr = RemapLibraryEx(proc, hModule, NlsHooks)) return addr;
    if (dxw.dwFlags12 & PROJECTBUFFER)		if(addr = RemapLibraryEx(proc, hModule, ProjectHooks)) return addr;
    return NULL;
}

//--------------------------------------------------------------------------------------------
//
// extern and common functions
//
//--------------------------------------------------------------------------------------------

extern DWORD PaletteEntries[256];
extern Unlock4_Type pUnlockMethod(int);
extern HRESULT WINAPI sBlt(int, Blt_Type, char *, LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX, BOOL);

extern GetDC_Type pGetDC;
extern ReleaseDC_Type pReleaseDC1;

#ifndef DXW_NOTRACES
static char *ExplainDIBUsage(UINT u) {
    char *p;
    switch(u) {
    case DIB_PAL_COLORS:
        p = "DIB_PAL_COLORS";
        break;
    case DIB_RGB_COLORS:
        p = "DIB_RGB_COLORS";
        break;
    default:
        p = "invalid";
        break;
    }
    return p;
}

static char *sBMIDump(BITMAPINFO *pbmi) {
    static char s[256];
    sprintf(s, "colors RGBX=(%#x,%#x,%#x,%#x) header bitc=%d size=(%dx%d) compr=%#x",
            pbmi->bmiColors->rgbRed, pbmi->bmiColors->rgbGreen, pbmi->bmiColors->rgbBlue, pbmi->bmiColors->rgbReserved,
            pbmi->bmiHeader.biBitCount, pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight, pbmi->bmiHeader.biCompression);
    return s;
}

static void TraceBITMAPINFOHEADER(char *fName, BITMAPINFOHEADER *bmi) {
    OutTrace("%s: BitmapInfo {Size=%d dim=(%dx%d) Planes=%d bitcount=%d Compression=%#x SizeImage=%d PelsPerMeter=%dx%d colors=U%d:I%d}\n",
             fName, bmi->biSize, bmi->biWidth, bmi->biHeight, bmi->biPlanes, bmi->biBitCount, bmi->biCompression,
             bmi->biSizeImage, bmi->biXPelsPerMeter, bmi->biYPelsPerMeter, bmi->biClrUsed, bmi->biClrImportant);
    if(bmi->biSize > sizeof(BITMAPINFOHEADER)) {
        BITMAPV4HEADER *bm4 = (BITMAPV4HEADER *)bmi;
        OutTrace("%s: BitmapInfoV4 {RGBA mask=%#x:%#x:%#x:%#x cstype=%#x gamma RGB=%#x:%#x:%#x}\n",
                 fName, bm4->bV4RedMask, bm4->bV4GreenMask, bm4->bV4BlueMask, bm4->bV4AlphaMask,
                 bm4->bV4CSType, bm4->bV4GammaRed, bm4->bV4GammaGreen, bm4->bV4GammaBlue);
    }
    if(bmi->biSize > sizeof(BITMAPV4HEADER)) {
        BITMAPV5HEADER *bm5 = (BITMAPV5HEADER *)bmi;
        OutTrace("%s: BitmapInfoV5 {intent=%#x profiledata=%#x profilesize=%#x resvd=%#x}\n",
                 fName, bm5->bV5Intent, bm5->bV5ProfileData, bm5->bV5ProfileSize, bm5->bV5Reserved);
    }
}

static char *ExplainGetRgnCode(int code) {
    char *s;
    switch(code) {
    case 0:
        s = "no region";
        break;
    case 1:
        s = "success";
        break;
    case -1:
        s = "error";
        break;
    default:
        s = "unknown";
        break;
    }
    return s;
}

static char *ExplainRgnMode(int mode) {
    char *s;
    switch(mode) {
    case RGN_AND:
        s = "AND";
        break;
    case RGN_COPY:
        s = "COPY";
        break;
    case RGN_DIFF:
        s = "DIFF";
        break;
    case RGN_OR:
        s = "OR";
        break;
    case RGN_XOR:
        s = "XOR";
        break;
    default:
        s = "unknown";
        break;
    }
    return s;
}

static char *sGradientMode(ULONG mode) {
    char *s;
    switch(mode) {
    case GRADIENT_FILL_RECT_H:
        s = "RECT_H";
        break;
    case GRADIENT_FILL_RECT_V:
        s = "RECT_V";
        break;
    case GRADIENT_FILL_TRIANGLE:
        s = "TRIANGLE";
        break;
    default:
        s = "???";
        break;
    }
    return s;
}

char *objname(int f) {
    char *s;
    static char *objnames[] = {
        "WHITE_BRUSH", "LTGRAY_BRUSH", "GRAY_BRUSH", "DKGRAY_BRUSH", "BLACK_BRUSH",
        "NULL_BRUSH", "WHITE_PEN", "BLACK_PEN", "NULL_PEN", "", "OEM_FIXED_FONT",
        "ANSI_FIXED_FONT", "ANSI_VAR_FONT", "SYSTEM_FONT", "DEVICE_DEFAULT_FONT",
        "DEFAULT_PALETTE", "SYSTEM_FIXED_FONT", "DEFAULT_GUI_FONT", "DC_BRUSH", "DC_PEN"
    };
    s = "???";
    if(f <= STOCK_LAST) s = objnames[f];
    return s;
}

static void dxwDumpRgn(char *api, HRGN hrgn) {
    LPRGNDATA lpRgnData;
    size_t size;
    DWORD nCount;
    LPRECT lpRect;
    if(hrgn == 0) return; // don't dump NULL region
    size = GetRegionData(hrgn, 0, NULL);
    lpRgnData = (LPRGNDATA)malloc(size);
    GetRegionData(hrgn, size, lpRgnData);
    OutTraceSYS("%s: region\n", ApiRef);
    OutTraceSYS("> dwSize=%d\n", lpRgnData->rdh.dwSize);
    OutTraceSYS("> iType=%d\n", lpRgnData->rdh.iType);
    OutTraceSYS("> nCount=%d\n", lpRgnData->rdh.nCount);
    OutTraceSYS("> nRgnSize=%d\n", lpRgnData->rdh.nRgnSize);
    OutTraceSYS("> rcBound=(%d,%d)-(%d,%d)\n",
                lpRgnData->rdh.rcBound.left,
                lpRgnData->rdh.rcBound.top,
                lpRgnData->rdh.rcBound.right,
                lpRgnData->rdh.rcBound.bottom
               );
    lpRect = (LPRECT)(&lpRgnData->Buffer);
    nCount = lpRgnData->rdh.nCount;
    //dxw.MapClient(&(lpRgnData->rdh.rcBound));
    for(DWORD i = 0; i < nCount; i++) {
        OutTraceSYS("> rect[%d]=(%d,%d)-(%d,%d)\n",
                    i,
                    lpRect->left,
                    lpRect->top,
                    lpRect->right,
                    lpRect->bottom);
        lpRect++;
    }
}
#else
#define dxwDumpRgn(a, h)
#endif

//--------------------------------------------------------------------------------------------
//
// API hookers
//
//--------------------------------------------------------------------------------------------

int WINAPI extGetDeviceCaps(HDC hdc, int nindex) {
    DWORD res;
    ApiName("GetDeviceCaps");
    res = (*pGDIGetDeviceCaps)(hdc, nindex);
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        char sInfo[1024];
        sprintf(sInfo, "%s: hdc=%#x index=%#x(%s)", ApiRef, hdc, nindex, ExplainDeviceCaps(nindex));
        switch(nindex) {
        case RASTERCAPS:
            sprintf(sInfo, "%s res=0x%04x(%s)\n", sInfo, res, ExplainRasterCaps(res));
            break;
        case BITSPIXEL:
        case COLORRES:
        case VERTRES:
        case HORZRES:
        case SIZEPALETTE:
        case NUMRESERVED:
            sprintf(sInfo, "%s res=%d\n", sInfo, res);
            break;
        default:
            sprintf(sInfo, "%s res=0x%04x\n", sInfo, res);
            break;
        }
        OutTrace(sInfo);
    }
#endif
    switch(nindex) {
    case VERTRES:
        if(dxw.Windowize) {
            if(dxw.IsDesktop(WindowFromDC(hdc)))
                res = dxw.GetScreenHeight();
            else {
                if(OBJ_DC == (*pGetObjectType)(hdc)) {
                    switch(dxw.GDIEmulationMode) {
                    case GDIMODE_SHAREDDC:
                    case GDIMODE_EMULATED:
                    default:
                        break;
                    case GDIMODE_STRETCHED:
                        int dummy = 0;
                        dxw.UnmapClient(&dummy, (int *)&res);
                        break;
                    }
                }
            }
            OutTraceDW("%s: fix(1) VERTRES cap=%d\n", ApiRef, res);
        }
        break;
    case HORZRES:
        if(dxw.Windowize) {
            if(dxw.IsDesktop(WindowFromDC(hdc)))
                res = dxw.GetScreenWidth();
            else {
                if(OBJ_DC == (*pGetObjectType)(hdc)) {
                    switch(dxw.GDIEmulationMode) {
                    case GDIMODE_SHAREDDC:
                    case GDIMODE_EMULATED:
                    default:
                        break;
                    case GDIMODE_STRETCHED:
                        int dummy = 0;
                        dxw.UnmapClient((int *)&res, &dummy);
                        break;
                    }
                }
            }
            OutTraceDW("%s: fix(2) HORZRES cap=%d\n", ApiRef, res);
        }
        break;
        if(dxw.Windowize)
            res = dxw.GetScreenWidth();
        break;
    // WARNING: in no-emu mode, the INIT8BPP and INIT16BPP flags expose capabilities that
    // are NOT implemented and may cause later troubles!
    case RASTERCAPS:
        if(dxw.dwFlags2 & INIT8BPP) {
            res |= RC_PALETTE; // v2.02.12
            OutTraceDW("%s: fix(3) RASTERCAPS setting RC_PALETTE cap=%#x\n", ApiRef, res);
        }
        break;
    case BITSPIXEL:
    case COLORRES:
        if((dxw.dwFlags2 & (INIT8BPP | INIT16BPP)) || (dxw.dwFlags7 & (INIT24BPP | INIT32BPP))) { // v2.02.32 fix
            if(dxw.dwFlags2 & INIT8BPP) res = 8;
            if(dxw.dwFlags2 & INIT16BPP) res = 16;
            if(dxw.dwFlags7 & INIT24BPP) res = 24;
            if(dxw.dwFlags7 & INIT32BPP) res = 32;
            OutTraceDW("%s: fix(4) BITSPIXEL/COLORRES cap=%d\n", ApiRef, res);
        }
        break;
    }
    if(dxw.IsEmulated) {
        switch(nindex) {
        case RASTERCAPS:
            if((dxw.VirtualPixelFormat.dwRGBBitCount == 8) || (dxw.dwFlags2 & INIT8BPP)) {
                res |= RC_PALETTE;
                OutTraceDW("%s: fix(5) RASTERCAPS setting RC_PALETTE cap=%#x(%s)\n", ApiRef, res, ExplainRasterCaps(res));
            }
            break;
        case BITSPIXEL:
        case COLORRES:
            int PrevRes;
            PrevRes = res;
            if(dxw.VirtualPixelFormat.dwRGBBitCount != 0) res = dxw.VirtualPixelFormat.dwRGBBitCount;
            if(dxw.dwFlags2 & INIT8BPP) res = 8;
            if(dxw.dwFlags2 & INIT16BPP) res = 16;
            if(dxw.dwFlags7 & INIT24BPP) res = 24;
            if(dxw.dwFlags7 & INIT32BPP) res = 32;
            _if(PrevRes != res) OutTraceDW("%s: fix(6) BITSPIXEL/COLORRES cap=%d\n", ApiRef, res);
            break;
        case SIZEPALETTE:
            res = 256;
            OutTraceDW("%s: fix(7) SIZEPALETTE cap=%#x\n", ApiRef, res);
            break;
        case NUMRESERVED:
            res = 0;
            OutTraceDW("%s: fix(8) NUMRESERVED cap=%#x\n", ApiRef, res);
            break;
            /*  v2.04.62 - removed
                case NUMCOLORS: // numcolors windows bug fix....
                if(res == -1) {
            	res = (dxw.VirtualPixelFormat.dwRGBBitCount == 8) ? 256 : 1;
            	OutTraceDW("GetDeviceCaps: fix(9) NUMCOLORS cap=%d\n", res);
                }
                break;
            */
        }
    }
    return res;
}

BOOL WINAPI extTextOutA(HDC hdc, int nXStart, int nYStart, LPCTSTR lpString, int cchString) {
    BOOL ret;
    ApiName("TextOutA");
    extern BOOL gFixed;
    OutTraceSYS("%s: hdc=%#x xy=(%d,%d) str=(%d)\"%.*s\"\n", ApiRef, hdc, nXStart, nYStart, cchString, cchString, lpString);
    if (!gFixed && dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pGDITextOutA)(sdc.GetHdc(), nXStart, nYStart, lpString, cchString);
            // update whole screen to avoid make calculations about text position & size
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&nXStart, &nYStart);
            OutTraceDW("%s: fixed dest=(%d,%d)\n", ApiRef, nXStart, nYStart);
            break;
        }
    }
    ret = (*pGDITextOutA)(hdc, nXStart, nYStart, lpString, cchString);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extTextOutW(HDC hdc, int nXStart, int nYStart, LPCWSTR lpString, int cchString) {
    BOOL ret;
    ApiName("TextOutW");
    extern BOOL gFixed;
    OutTraceSYS("%s: hdc=%#x xy=(%d,%d) str=(%d)\"%.*ls\"\n", ApiRef, hdc, nXStart, nYStart, cchString, cchString, lpString);
    if (!gFixed && dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pGDITextOutW)(sdc.GetHdc(), nXStart, nYStart, lpString, cchString);
            // update whole screen to avoid make calculations about text position & size
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&nXStart, &nYStart);
            OutTraceDW("%s: fixed dest=(%d,%d)\n", ApiRef, nXStart, nYStart);
            break;
        }
    }
    ret = (*pGDITextOutW)(hdc, nXStart, nYStart, lpString, cchString);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

#ifdef TRACEALL
BOOL WINAPI extScaleWindowExtEx(HDC hdc, int Xnum, int Xdenom, int Ynum, int Ydenom, LPSIZE lpSize) {
    BOOL ret;
    ApiName("ScaleWindowExtEx");
    OutTraceSYS("%s: hdc=%#x num=(%d,%d) denom=(%d,%d) lpSize=%d\n",
                ApiRef, hdc, Xnum, Ynum, Xdenom, Ydenom, lpSize);
    // MessageBox(0, "ScaleWindowExtEx", "to fix", MB_OK | MB_ICONEXCLAMATION);
    // call found in "Lego Marvel Superheroes" by Mchaidang
    ret = (*pGDIScaleWindowExtEx)(hdc, Xnum, Xdenom, Ynum, Ydenom, lpSize);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}
#endif // TRACEALL

BOOL WINAPI extRectangle(HDC hdc, int nLeftRect, int nTopRect, int nRightRect, int nBottomRect) {
    BOOL ret;
    ApiName("Rectangle");
    OutTraceSYS("%s: hdc=%#x xy=(%d,%d)-(%d,%d)\n", ApiRef, hdc, nLeftRect, nTopRect, nRightRect, nBottomRect);
    if (dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pGDIRectangle)(sdc.GetHdc(), nLeftRect, nTopRect, nRightRect, nBottomRect);
            sdc.PutPrimaryDC(hdc, TRUE, nLeftRect, nTopRect, nRightRect - nLeftRect, nBottomRect - nTopRect);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&nLeftRect, &nTopRect, &nRightRect, &nBottomRect);
            OutTraceDW("%s: fixed dest=(%d,%d)-(%d,%d)\n", ApiRef, nLeftRect, nTopRect, nRightRect, nBottomRect);
            break;
        }
    }
    ret = (*pGDIRectangle)(hdc, nLeftRect, nTopRect, nRightRect, nBottomRect);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

int WINAPI extGDISaveDC(HDC hdc) {
    int ret;
    ret = (*pGDISaveDC)(hdc);
    OutTraceSYS("GDI.SaveDC: hdc=%#x ret=%#x\n", hdc, ret);
    return ret;
}

BOOL WINAPI extGDIRestoreDC(HDC hdc, int nSavedDC) {
    BOOL ret;
    ret = (*pGDIRestoreDC)(hdc, nSavedDC);
    OutTraceSYS("GDI.RestoreDC: hdc=%#x nSavedDC=%#x ret=%#x\n", hdc, nSavedDC, ret);
    return ret;
}

/* --------------------------------------------------------------------------- */
/*     Palette handling                                                        */
/* --------------------------------------------------------------------------- */

extern HDC hFlippedDC;
extern BOOL bFlippedDC;
extern void mySetPalette(int, int, LPPALETTEENTRY);

// v2.1.75: Hooking for GDI32 CreatePalette, SelectPalette, RealizePalette:
// maps the GDI palette to the buffered DirectDraw one. This fixes the screen
// output for "Dementia" (a.k.a. "Armed & Delirious").

// In emulated mode (when color depyth is 8BPP ?) it may happen that the game
// expects to get the requested system palette entries, while the 32BPP screen
// returns 0. "Mission Force Cyberstorm" is one of these. Returning the same
// value as nEntries, even though lppe is untouched, fixes the problem.

PALETTEENTRY DefaultSystemPalette[256] = { // default palette, captured on my PC with video mode set to 8BPP
    {0x00, 0x00, 0x00, 0x00}, {0x80, 0x00, 0x00, 0x00}, {0x00, 0x80, 0x00, 0x00}, {0x80, 0x80, 0x00, 0x00},
    {0x00, 0x00, 0x80, 0x00}, {0x80, 0x00, 0x80, 0x00}, {0x00, 0x80, 0x80, 0x00}, {0xc0, 0xc0, 0xc0, 0x00},
    {0xa0, 0xa0, 0xa0, 0x00}, {0xf0, 0xf0, 0xf0, 0x00}, {0xc0, 0xdc, 0xc0, 0x00}, {0xa6, 0xca, 0xf0, 0x00},
    {0x04, 0x04, 0x04, 0x00}, {0x08, 0x08, 0x08, 0x00}, {0x0c, 0x0c, 0x0c, 0x00}, {0x11, 0x11, 0x11, 0x00},
    {0x16, 0x16, 0x16, 0x00}, {0x1c, 0x1c, 0x1c, 0x00}, {0x22, 0x22, 0x22, 0x00}, {0x29, 0x29, 0x29, 0x00},
    {0x55, 0x55, 0x55, 0x00}, {0x4d, 0x4d, 0x4d, 0x00}, {0x42, 0x42, 0x42, 0x00}, {0x39, 0x39, 0x39, 0x00},
    {0xff, 0x7c, 0x80, 0x00}, {0xff, 0x50, 0x50, 0x00}, {0xd6, 0x00, 0x93, 0x00}, {0xcc, 0xec, 0xff, 0x00},
    {0xef, 0xd6, 0xc6, 0x00}, {0xe7, 0xe7, 0xd6, 0x00}, {0xad, 0xa9, 0x90, 0x00}, {0x33, 0x00, 0x00, 0x00},
    {0x66, 0x00, 0x00, 0x00}, {0x99, 0x00, 0x00, 0x00}, {0xcc, 0x00, 0x00, 0x00}, {0x00, 0x33, 0x00, 0x00},
    {0x33, 0x33, 0x00, 0x00}, {0x66, 0x33, 0x00, 0x00}, {0x99, 0x33, 0x00, 0x00}, {0xcc, 0x33, 0x00, 0x00},
    {0xff, 0x33, 0x00, 0x00}, {0x00, 0x66, 0x00, 0x00}, {0x33, 0x66, 0x00, 0x00}, {0x66, 0x66, 0x00, 0x00},
    {0x99, 0x66, 0x00, 0x00}, {0xcc, 0x66, 0x00, 0x00}, {0xff, 0x66, 0x00, 0x00}, {0x00, 0x99, 0x00, 0x00},
    {0x33, 0x99, 0x00, 0x00}, {0x66, 0x99, 0x00, 0x00}, {0x99, 0x99, 0x00, 0x00}, {0xcc, 0x99, 0x00, 0x00},
    {0xff, 0x99, 0x00, 0x00}, {0x00, 0xcc, 0x00, 0x00}, {0x33, 0xcc, 0x00, 0x00}, {0x66, 0xcc, 0x00, 0x00},
    {0x99, 0xcc, 0x00, 0x00}, {0xcc, 0xcc, 0x00, 0x00}, {0xff, 0xcc, 0x00, 0x00}, {0x66, 0xff, 0x00, 0x00},
    {0x99, 0xff, 0x00, 0x00}, {0xcc, 0xff, 0x00, 0x00}, {0x00, 0x00, 0x33, 0x00}, {0x33, 0x00, 0x33, 0x00},
    {0x66, 0x00, 0x33, 0x00}, {0x99, 0x00, 0x33, 0x00}, {0xcc, 0x00, 0x33, 0x00}, {0xff, 0x00, 0x33, 0x00},
    {0x00, 0x33, 0x33, 0x00}, {0x33, 0x33, 0x33, 0x00}, {0x66, 0x33, 0x33, 0x00}, {0x99, 0x33, 0x33, 0x00},
    {0xcc, 0x33, 0x33, 0x00}, {0xff, 0x33, 0x33, 0x00}, {0x00, 0x66, 0x33, 0x00}, {0x33, 0x66, 0x33, 0x00},
    {0x66, 0x66, 0x33, 0x00}, {0x99, 0x66, 0x33, 0x00}, {0xcc, 0x66, 0x33, 0x00}, {0xff, 0x66, 0x33, 0x00},
    {0x00, 0x99, 0x33, 0x00}, {0x33, 0x99, 0x33, 0x00}, {0x66, 0x99, 0x33, 0x00}, {0x99, 0x99, 0x33, 0x00},
    {0xcc, 0x99, 0x33, 0x00}, {0xff, 0x99, 0x33, 0x00}, {0x00, 0xcc, 0x33, 0x00}, {0x33, 0xcc, 0x33, 0x00},
    {0x66, 0xcc, 0x33, 0x00}, {0x99, 0xcc, 0x33, 0x00}, {0xcc, 0xcc, 0x33, 0x00}, {0xff, 0xcc, 0x33, 0x00},
    {0x33, 0xff, 0x33, 0x00}, {0x66, 0xff, 0x33, 0x00}, {0x99, 0xff, 0x33, 0x00}, {0xcc, 0xff, 0x33, 0x00},
    {0xff, 0xff, 0x33, 0x00}, {0x00, 0x00, 0x66, 0x00}, {0x33, 0x00, 0x66, 0x00}, {0x66, 0x00, 0x66, 0x00},
    {0x99, 0x00, 0x66, 0x00}, {0xcc, 0x00, 0x66, 0x00}, {0xff, 0x00, 0x66, 0x00}, {0x00, 0x33, 0x66, 0x00},
    {0x33, 0x33, 0x66, 0x00}, {0x66, 0x33, 0x66, 0x00}, {0x99, 0x33, 0x66, 0x00}, {0xcc, 0x33, 0x66, 0x00},
    {0xff, 0x33, 0x66, 0x00}, {0x00, 0x66, 0x66, 0x00}, {0x33, 0x66, 0x66, 0x00}, {0x66, 0x66, 0x66, 0x00},
    {0x99, 0x66, 0x66, 0x00}, {0xcc, 0x66, 0x66, 0x00}, {0x00, 0x99, 0x66, 0x00}, {0x33, 0x99, 0x66, 0x00},
    {0x66, 0x99, 0x66, 0x00}, {0x99, 0x99, 0x66, 0x00}, {0xcc, 0x99, 0x66, 0x00}, {0xff, 0x99, 0x66, 0x00},
    {0x00, 0xcc, 0x66, 0x00}, {0x33, 0xcc, 0x66, 0x00}, {0x99, 0xcc, 0x66, 0x00}, {0xcc, 0xcc, 0x66, 0x00},
    {0xff, 0xcc, 0x66, 0x00}, {0x00, 0xff, 0x66, 0x00}, {0x33, 0xff, 0x66, 0x00}, {0x99, 0xff, 0x66, 0x00},
    {0xcc, 0xff, 0x66, 0x00}, {0xff, 0x00, 0xcc, 0x00}, {0xcc, 0x00, 0xff, 0x00}, {0x00, 0x99, 0x99, 0x00},
    {0x99, 0x33, 0x99, 0x00}, {0x99, 0x00, 0x99, 0x00}, {0xcc, 0x00, 0x99, 0x00}, {0x00, 0x00, 0x99, 0x00},
    {0x33, 0x33, 0x99, 0x00}, {0x66, 0x00, 0x99, 0x00}, {0xcc, 0x33, 0x99, 0x00}, {0xff, 0x00, 0x99, 0x00},
    {0x00, 0x66, 0x99, 0x00}, {0x33, 0x66, 0x99, 0x00}, {0x66, 0x33, 0x99, 0x00}, {0x99, 0x66, 0x99, 0x00},
    {0xcc, 0x66, 0x99, 0x00}, {0xff, 0x33, 0x99, 0x00}, {0x33, 0x99, 0x99, 0x00}, {0x66, 0x99, 0x99, 0x00},
    {0x99, 0x99, 0x99, 0x00}, {0xcc, 0x99, 0x99, 0x00}, {0xff, 0x99, 0x99, 0x00}, {0x00, 0xcc, 0x99, 0x00},
    {0x33, 0xcc, 0x99, 0x00}, {0x66, 0xcc, 0x66, 0x00}, {0x99, 0xcc, 0x99, 0x00}, {0xcc, 0xcc, 0x99, 0x00},
    {0xff, 0xcc, 0x99, 0x00}, {0x00, 0xff, 0x99, 0x00}, {0x33, 0xff, 0x99, 0x00}, {0x66, 0xcc, 0x99, 0x00},
    {0x99, 0xff, 0x99, 0x00}, {0xcc, 0xff, 0x99, 0x00}, {0xff, 0xff, 0x99, 0x00}, {0x00, 0x00, 0xcc, 0x00},
    {0x33, 0x00, 0x99, 0x00}, {0x66, 0x00, 0xcc, 0x00}, {0x99, 0x00, 0xcc, 0x00}, {0xcc, 0x00, 0xcc, 0x00},
    {0x00, 0x33, 0x99, 0x00}, {0x33, 0x33, 0xcc, 0x00}, {0x66, 0x33, 0xcc, 0x00}, {0x99, 0x33, 0xcc, 0x00},
    {0xcc, 0x33, 0xcc, 0x00}, {0xff, 0x33, 0xcc, 0x00}, {0x00, 0x66, 0xcc, 0x00}, {0x33, 0x66, 0xcc, 0x00},
    {0x66, 0x66, 0x99, 0x00}, {0x99, 0x66, 0xcc, 0x00}, {0xcc, 0x66, 0xcc, 0x00}, {0xff, 0x66, 0x99, 0x00},
    {0x00, 0x99, 0xcc, 0x00}, {0x33, 0x99, 0xcc, 0x00}, {0x66, 0x99, 0xcc, 0x00}, {0x99, 0x99, 0xcc, 0x00},
    {0xcc, 0x99, 0xcc, 0x00}, {0xff, 0x99, 0xcc, 0x00}, {0x00, 0xcc, 0xcc, 0x00}, {0x33, 0xcc, 0xcc, 0x00},
    {0x66, 0xcc, 0xcc, 0x00}, {0x99, 0xcc, 0xcc, 0x00}, {0xcc, 0xcc, 0xcc, 0x00}, {0xff, 0xcc, 0xcc, 0x00},
    {0x00, 0xff, 0xcc, 0x00}, {0x33, 0xff, 0xcc, 0x00}, {0x66, 0xff, 0x99, 0x00}, {0x99, 0xff, 0xcc, 0x00},
    {0xcc, 0xff, 0xcc, 0x00}, {0xff, 0xff, 0xcc, 0x00}, {0x33, 0x00, 0xcc, 0x00}, {0x66, 0x00, 0xff, 0x00},
    {0x99, 0x00, 0xff, 0x00}, {0x00, 0x33, 0xcc, 0x00}, {0x33, 0x33, 0xff, 0x00}, {0x66, 0x33, 0xff, 0x00},
    {0x99, 0x33, 0xff, 0x00}, {0xcc, 0x33, 0xff, 0x00}, {0xff, 0x33, 0xff, 0x00}, {0x00, 0x66, 0xff, 0x00},
    {0x33, 0x66, 0xff, 0x00}, {0x66, 0x66, 0xcc, 0x00}, {0x99, 0x66, 0xff, 0x00}, {0xcc, 0x66, 0xff, 0x00},
    {0xff, 0x66, 0xcc, 0x00}, {0x00, 0x99, 0xff, 0x00}, {0x33, 0x99, 0xff, 0x00}, {0x66, 0x99, 0xff, 0x00},
    {0x99, 0x99, 0xff, 0x00}, {0xcc, 0x99, 0xff, 0x00}, {0xff, 0x99, 0xff, 0x00}, {0x00, 0xcc, 0xff, 0x00},
    {0x33, 0xcc, 0xff, 0x00}, {0x66, 0xcc, 0xff, 0x00}, {0x99, 0xcc, 0xff, 0x00}, {0xcc, 0xcc, 0xff, 0x00},
    {0xff, 0xcc, 0xff, 0x00}, {0x33, 0xff, 0xff, 0x00}, {0x66, 0xff, 0xcc, 0x00}, {0x99, 0xff, 0xff, 0x00},
    {0xcc, 0xff, 0xff, 0x00}, {0xff, 0x66, 0x66, 0x00}, {0x66, 0xff, 0x66, 0x00}, {0xff, 0xff, 0x66, 0x00},
    {0x66, 0x66, 0xff, 0x00}, {0xff, 0x66, 0xff, 0x00}, {0x66, 0xff, 0xff, 0x00}, {0xa5, 0x00, 0x21, 0x00},
    {0x5f, 0x5f, 0x5f, 0x00}, {0x77, 0x77, 0x77, 0x00}, {0x86, 0x86, 0x86, 0x00}, {0x96, 0x96, 0x96, 0x00},
    {0xcb, 0xcb, 0xcb, 0x00}, {0xb2, 0xb2, 0xb2, 0x00}, {0xd7, 0xd7, 0xd7, 0x00}, {0xdd, 0xdd, 0xdd, 0x00},
    {0xe3, 0xe3, 0xe3, 0x00}, {0xea, 0xea, 0xea, 0x00}, {0xff, 0xfb, 0xf0, 0x00}, {0xa0, 0xa0, 0xa4, 0x00},
    {0x80, 0x80, 0x80, 0x00}, {0xff, 0x00, 0x00, 0x00}, {0x00, 0xff, 0x00, 0x00}, {0xff, 0xff, 0x00, 0x00},
    {0x00, 0x00, 0xff, 0x00}, {0xff, 0x00, 0xff, 0x00}, {0x00, 0xff, 0xff, 0x00}, {0xff, 0xff, 0xff, 0x00}
};

HPALETTE WINAPI extGDICreatePalette(CONST LOGPALETTE *plpal) {
    HPALETTE ret;
    ApiName("GDI.CreatePalette");
    OutTraceSYS("%s: plpal=%#x version=%#x NumEntries=%d\n", ApiRef, plpal, plpal->palVersion, plpal->palNumEntries);
    if(IsDebugSYS) dxw.DumpPalette(plpal->palNumEntries, (LPPALETTEENTRY)plpal->palPalEntry);
    ret = (*pGDICreatePalette)(plpal);
    OutTraceSYS("%s: hPalette=%#x\n", ApiRef, ret);
    return ret;
}

HPALETTE hDesktopPalette = NULL;

HPALETTE WINAPI extSelectPalette(HDC hdc, HPALETTE hpal, BOOL bForceBackground) {
    HPALETTE ret;
    ApiName("GDI.SelectPalette");
    OutTraceSYS("%s: hdc=%#x hpal=%#x ForceBackground=%#x\n", ApiRef, hdc, hpal, bForceBackground);
    if(hdc == dxw.RealHDC) hdc = dxw.VirtualHDC;
    //if((dxw.IsEmulated)  && (dxw.dwFlags6 & SYNCPALETTE) && bFlippedDC){
    if((dxw.IsEmulated)  && (dxw.dwFlags6 & SYNCPALETTE)) {
        hDesktopPalette = hpal;
        if(hFlippedDC) {
            hdc = hFlippedDC;
            ret = (*pGDISelectPalette)(hdc, hpal, bForceBackground);
        } else {
            extern GetDC_Type pGetDCMethod();
            extern ReleaseDC_Type pReleaseDCMethod();
            LPDIRECTDRAWSURFACE lpDDSPrim;
            lpDDSPrim = dxwss.GetPrimarySurface();
            // v2.04.24: check for lpDDSPrim in case you're calling SelectPalette before a ddraw session is created
            // and a valid value for pGetDCMethod()/pReleaseDCMethod() is returned. Fixes "Yu-No"
            if(lpDDSPrim) {
                (*pGetDCMethod())(lpDDSPrim, &hdc);
                ret = (*pGDISelectPalette)(hdc, hpal, bForceBackground);
                (*pReleaseDCMethod())(lpDDSPrim, hdc);
            } else
                ret = (*pGDISelectPalette)(hdc, hpal, bForceBackground);
        }
    } else
        ret = (*pGDISelectPalette)(hdc, hpal, bForceBackground);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
#ifndef DXW_NOTRACE
    if((dxw.ActualPixelFormat.dwRGBBitCount == 8) && IsDebugSYS) dxw.DumpSysPalette();
#endif // DXW_NOTRACE
    return ret;
}

BOOL WINAPI extAnimatePalette(HPALETTE hpal, UINT iStartIndex, UINT cEntries, const PALETTEENTRY *ppe) {
    // Invoked by "Pharaoh's Ascent 1.4"
    // Used by "Yu No"
    BOOL ret;
    ApiName("GDI.AnimatePalette");
    OutTraceSYS("%s: hpal=%#x startindex=%d entries=%d\n", ApiRef, hpal, iStartIndex, cEntries);
    if(IsDebugSYS) dxw.DumpPalette(cEntries, (LPPALETTEENTRY)ppe + iStartIndex);
    ret = (*pAnimatePalette)(hpal, iStartIndex, cEntries, ppe);
    if((dxw.IsEmulated) && (dxw.dwFlags6 & SYNCPALETTE)) {
        PALETTEENTRY PalEntries[256];
        UINT cEntries;
        cEntries = (*pGetPaletteEntries)(hDesktopPalette, 0, 256, PalEntries);
        mySetPalette(dxw.ReservedPaletteEntries >> 1, cEntries, PalEntries);
        if(IsDebugDW && cEntries) dxw.DumpPalette(cEntries, PalEntries);
        ret = cEntries;
        HRESULT res;
        extern LPDIRECTDRAWPALETTE lpDDP;
        extern SetEntries_Type pSetEntries;
        if(lpDDP && pSetEntries) res = (*pSetEntries)(lpDDP, 0, 0, 256, PalEntries);
    }
    if(!ret) {
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
        ret = TRUE;
    }
#ifndef DXW_NOTRACE
    if((dxw.ActualPixelFormat.dwRGBBitCount == 8) && IsDebugSYS) dxw.DumpSysPalette();
#endif // DXW_NOTRACE
    return ret;
}

UINT WINAPI extRealizePalette(HDC hdc) {
    UINT ret;
    ApiName("GDI.RealizePalette");
    OutTraceSYS("%s: hdc=%#x\n", ApiRef, hdc);
    if((dxw.IsEmulated) && (dxw.dwFlags6 & SYNCPALETTE)) {
        PALETTEENTRY PalEntries[256];
        UINT cEntries;
        if(bFlippedDC) hdc = hFlippedDC;
        ret = (*pGDIRealizePalette)(hdc);
        OutTraceDW("%s: RealizePalette on hdc=%#x ret=%d\n", ApiRef, hdc, ret);
        cEntries = (*pGetPaletteEntries)(hDesktopPalette, 0, 256, PalEntries);
        OutTraceDW("%s: GetPaletteEntries on hdc=%#x ret=%d\n", ApiRef, hdc, cEntries);
        mySetPalette(dxw.ReservedPaletteEntries >> 1, cEntries, PalEntries);
        if(IsDebugDW && cEntries) dxw.DumpPalette(cEntries, PalEntries);
        ret = cEntries;
        HRESULT res;
        extern LPDIRECTDRAWPALETTE lpDDP;
        extern SetEntries_Type pSetEntries;
        if(lpDDP && pSetEntries) res = (*pSetEntries)(lpDDP, 0, 0, 256, PalEntries);
    } else
        ret = (*pGDIRealizePalette)(hdc);
    OutTraceSYS("%s: hdc=%#x nEntries=%d\n", ApiRef, hdc, ret);
    // refresh on resize conditioned to configuration flag. It should be possible, though, to predict whether
    // a window refresh is needed or not given the conditions. To be done ....
    if(dxw.IsFullScreen() && (dxw.dwFlags3 & REFRESHONREALIZE)) {
        HWND hwnd = WindowFromDC(hdc);
        if(!dxw.IsRealDesktop(hwnd))(*pInvalidateRect)(hwnd, NULL, FALSE);
    }
    return ret;
}

UINT WINAPI extGetSystemPaletteEntries(HDC hdc, UINT iStartIndex, UINT nEntries, LPPALETTEENTRY lppe) {
    int ret;
    ApiName("GetSystemPaletteEntries");
    OutTraceSYS("%s: hdc=%#x start=%d num=%d\n", ApiRef, hdc, iStartIndex, nEntries);
    ret = (*pGDIGetSystemPaletteEntries)(hdc, iStartIndex, nEntries, lppe);
    OutTraceSYS("%s: ret=%d\n", ApiRef, ret);
    if((ret == 0) && (dxw.IsEmulated) && (dxw.dwFlags6 & SYNCPALETTE)) {
        // use static default data...
        for(UINT idx = 0; idx < nEntries; idx++) lppe[idx] = DefaultSystemPalette[iStartIndex + idx];
        ret = nEntries;
        OutTraceDW("%s: FIXED ret=%d\n", ApiRef, ret);
    }
    if(IsDebugDW) dxw.DumpPalette(nEntries, lppe);
    return ret;
}

UINT WINAPI extSetSystemPaletteUse(HDC hdc, UINT uUsage) {
    //BOOL res;
    OutTraceSYS("GDI.SetSystemPaletteUse: hdc=%#x Usage=%#x(%s)\n", hdc, uUsage, ExplainPaletteUse(uUsage));
    return SYSPAL_NOSTATIC256;
}

UINT WINAPI extGetPaletteEntries(HPALETTE hpal, UINT iStartIndex, UINT nEntries, LPPALETTEENTRY lppe) {
    UINT res;
    ApiName("GDI.GetPaletteEntries");
    OutTraceSYS("%s: hpal=%#x iStartIndex=%d nEntries=%d\n", ApiRef, hpal, iStartIndex, nEntries);
    res = (*pGetPaletteEntries)(hpal, iStartIndex, nEntries, lppe);
    OutTraceSYS("%s: res-nEntries=%d\n", ApiRef, res);
    if((res < nEntries) && (dxw.dwFlags6 & SYNCPALETTE)) {
        res = nEntries;
        OutTraceDW("%s: faking missing entries=%d\n", ApiRef, res);
    }
    // GDI Palette applied to ddraw: needed to color the gameplay 3D screen of "Hyperblade".
    if ((dxw.IsEmulated) && (dxw.dwFlags6 & SYNCPALETTE)) mySetPalette(0, nEntries, lppe);
    if(IsDebugDW && res) dxw.DumpPalette(res, lppe);
    return res;
}

UINT WINAPI extGetSystemPaletteUse(HDC hdc) {
    UINT res;
    ApiName("GDI.GetSystemPaletteUse");
    OutTraceSYS("%s: hdc=%#x\n", ApiRef, hdc);
    res = (*pGetSystemPaletteUse)(hdc);
    if((res == SYSPAL_ERROR) && (dxw.dwFlags6 & SYNCPALETTE)) res = SYSPAL_NOSTATIC;
    OutTraceSYS("%s: res=%#x(%s)\n", ApiRef, res, ExplainPaletteUse(res));
    return res;
}

HDC WINAPI extGDICreateDCA(LPSTR lpszDriver, LPSTR lpszDevice, LPSTR lpszOutput, CONST DEVMODE *lpdvmInit) {
    HDC WinHDC, RetHDC;
    ApiName("GDI.CreateDCA");
    OutTraceSYS("%s: Driver=%s Device=%s Output=%s InitData=%#x\n", ApiRef,
                lpszDriver ? lpszDriver : "(NULL)", lpszDevice ? lpszDevice : "(NULL)", lpszOutput ? lpszOutput : "(NULL)", lpdvmInit);
    if ((!lpszDriver || !strncmp(lpszDriver, "DISPLAY", 7)) && dxw.GethWnd()) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_NONE:
        case GDIMODE_STRETCHED:
        case GDIMODE_EMULATED:
        case GDIMODE_SHAREDDC:
        default:
            OutTraceDW("%s: returning window surface DC\n", ApiRef);
            WinHDC = (*pGDIGetDC)(dxw.GethWnd());
            RetHDC = (*pGDICreateCompatibleDC)(WinHDC);
            (*pGDIReleaseDC)(dxw.GethWnd(), WinHDC);
            break;
        }
    } else
        RetHDC = (*pGDICreateDCA)(lpszDriver, lpszDevice, lpszOutput, lpdvmInit);
    if(RetHDC)
        OutTraceSYS("%s: returning HDC=%#x\n", ApiRef, RetHDC);
    else
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return RetHDC;
}

HDC WINAPI extGDICreateDCW(LPWSTR lpszDriver, LPWSTR lpszDevice, LPWSTR lpszOutput, CONST DEVMODE *lpdvmInit) {
    HDC WinHDC, RetHDC;
    ApiName("GDI.CreateDCW");
    OutTraceSYS("%s: Driver=%ls Device=%ls Output=%ls InitData=%#x\n", ApiRef,
                lpszDriver ? lpszDriver : L"(NULL)", lpszDevice ? lpszDevice : L"(NULL)", lpszOutput ? lpszOutput : L"(NULL)", lpdvmInit);
    if ((!lpszDriver || !wcsncmp(lpszDriver, L"DISPLAY", 7)) && dxw.GethWnd()) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_NONE:
        case GDIMODE_STRETCHED:
        case GDIMODE_EMULATED:
        case GDIMODE_SHAREDDC:
        default:
            OutTraceDW("%s: returning window surface DC\n", ApiRef);
            WinHDC = (*pGDIGetDC)(dxw.GethWnd());
            RetHDC = (*pGDICreateCompatibleDC)(WinHDC);
            (*pGDIReleaseDC)(dxw.GethWnd(), WinHDC);
            break;
        }
    } else
        RetHDC = (*pGDICreateDCW)(lpszDriver, lpszDevice, lpszOutput, lpdvmInit);
    if(RetHDC)
        OutTraceSYS("%s: returning HDC=%#x\n", ApiRef, RetHDC);
    else
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return RetHDC;
}

#ifdef TRACEALL
HDC WINAPI extCreateICA(LPCTSTR lpszDriver, LPCTSTR lpszDevice, LPCTSTR lpszOutput, const DEVMODEA *lpdvmInit) {
    HDC RetHDC;
    ApiName("GDI.CreateICA");
    OutTraceSYS("%s: Driver=%s Device=%s Output=%s InitData=%#x\n", ApiRef,
                lpszDriver ? lpszDriver : "(NULL)", lpszDevice ? lpszDevice : "(NULL)", lpszOutput ? lpszOutput : "(NULL)", lpdvmInit);
    RetHDC = (*pCreateICA)(lpszDriver, lpszDevice, lpszOutput, lpdvmInit);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, RetHDC);
    return RetHDC;
}
#endif // TRACEALL

#ifdef TRACEALL
HDC WINAPI extCreateICW(LPCWSTR lpszDriver, LPCWSTR lpszDevice, LPCWSTR lpszOutput, const DEVMODEW *lpdvmInit) {
    HDC RetHDC;
    ApiName("GDI.CreateICW");
    OutTraceSYS("%s: Driver=%ls Device=%ls Output=%ls InitData=%#x\n", ApiRef,
                lpszDriver ? lpszDriver : L"(NULL)", lpszDevice ? lpszDevice : L"(NULL)", lpszOutput ? lpszOutput : L"(NULL)", lpdvmInit);
    RetHDC = (*pCreateICW)(lpszDriver, lpszDevice, lpszOutput, lpdvmInit);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, RetHDC);
    return RetHDC;
}
#endif // TRACEALL

HDC WINAPI extGDICreateCompatibleDC(HDC hdc) {
    // v2.03.75: fixed dc leakage that crashed "Mechwarrior 3"
    HDC RetHdc;
    ApiName("GDI.CreateCompatibleDC");
    DWORD LastError;
    BOOL bSwitchedToMainWin = FALSE;
    OutTraceSYS("%s: hdc=%#x\n", ApiRef, hdc);
    // n.b. useful? hdc == 0 doesn't imply the desktop but rather the current window
    // from MSDN:
    // hdc[in] A handle to an existing DC. If this handle is NULL the function creates
    // a memory DC compatible with the application's current screen.
    if(hdc == 0 || (WindowFromDC(hdc) == 0)) { // v2.03.99: Star Trek Armada
        hdc = (*pGDIGetDC)(dxw.GethWnd()); // potential DC leakage
        bSwitchedToMainWin = TRUE;
        OutTraceDW("%s: duplicating win HDC hWnd=%#x\n", ApiRef, dxw.GethWnd());
    }
    // eliminated error message for errorcode 0.
    SetLastError(0);
    RetHdc = (*pGDICreateCompatibleDC)(hdc);
    if(bSwitchedToMainWin) (*pGDIReleaseDC)(dxw.GethWnd(), hdc); // fixed DC leakage
    LastError = GetLastError();
    if(LastError == 0)
        OutTraceSYS("%s: returning HDC=%#x\n", ApiRef, RetHdc);
    else
        OutTraceE("%s: ERROR err=%d\n", ApiRef, LastError);
    return RetHdc;
}

/*-------------------------------------------*/

BOOL WINAPI extGDIBitBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop) {
    BOOL res;
    ApiName("GDI.BitBlt");
    BOOL IsToScreen;
    BOOL IsFromScreen;
    BOOL IsDCLeakageSrc = FALSE;
    BOOL IsDCLeakageDest = FALSE;
    int Flux;
    OutTraceSYS("%s: HDC=%#x(type=%s) nXDest=%d nYDest=%d nWidth=%d nHeight=%d hdcSrc=%#x(type=%s) nXSrc=%d nYSrc=%d dwRop=%#x(%s)\n",
                ApiRef, hdcDest, GetObjectTypeStr(hdcDest), nXDest, nYDest, nWidth, nHeight,
                hdcSrc, GetObjectTypeStr(hdcSrc), nXSrc, nYSrc, dwRop, ExplainROP(dwRop));
    OutDebugSYS("%s: DEBUG FullScreen=%#x target hdctype=%#x(%s) hwnd=%#x\n",
                ApiRef, dxw.IsFullScreen(), (*pGetObjectType)(hdcDest), ExplainDCType((*pGetObjectType)(hdcDest)), WindowFromDC(hdcDest));
    // beware: HDC could refer to screen DC that are written directly on screen, or memory DC that will be scaled to
    // the screen surface later on, on ReleaseDC or ddraw Blit / Flip operation. Scaling of rect coordinates is
    // needed only in the first case, and must be avoided on the second, otherwise the image would be scaled twice!
    if(hdcDest == NULL) {
        // happens in Reah, hdc is NULL despite the fact that BeginPaint returns a valid DC. Too bad, we recover here ...
        hdcDest = (*pGDIGetDC)(dxw.GethWnd());
        OutDebugDW("%s: DEBUG hdc dest=NULL->%#x\n", ApiRef, hdcDest);
        IsDCLeakageDest = TRUE;
    }
    if(hdcSrc == NULL) {
        hdcSrc = (*pGDIGetDC)(dxw.GethWnd());
        OutDebugDW("%s: DEBUG hdc src=NULL->%#x\n", ApiRef, hdcSrc);
        IsDCLeakageSrc = TRUE;
    }
    if((dxw.dwFlags10 & CHAOSOVERLORDSFIX) && (dwRop == SRCAND)) dwRop = SRCINVERT; // ok
    IsToScreen = (OBJ_DC == (*pGetObjectType)(hdcDest));
    IsFromScreen = (OBJ_DC == (*pGetObjectType)(hdcSrc));
    Flux = (IsToScreen ? 1 : 0) + (IsFromScreen ? 2 : 0);
    OutDebugDW("%s: flux=%d\n", ApiRef, Flux);
    // v2.04.30 fixed handlers leakage
    if (IsToScreen && (dxw.dwDFlags & NOGDIBLT)) {
        if(IsDCLeakageSrc) (*pGDIReleaseDC)(dxw.GethWnd(), hdcSrc);
        if(IsDCLeakageDest) (*pGDIReleaseDC)(dxw.GethWnd(), hdcDest);
        return TRUE;
    }
#ifndef DXW_NOTRACES
    if(IsToScreen && (dxw.dwDFlags & DUMPDEVCONTEXT) && dxw.bCustomKeyToggle) DumpHDC(hdcSrc, nXSrc, nYSrc, nWidth, nHeight);
#endif // DXW_NOTRACES
    if(IsToScreen) dxw.HandleFPS(); // handle refresh delays
    if(dxw.IsFullScreen()) {
        //int nWSrc, nHSrc,
        int nWDest, nHDest;
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            switch(Flux) {
            case 0: // memory to memory
                res = (*pGDIBitBlt)(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
                break;
            case 1: // memory to screen
            case 3: // screen to screen
                sdc.SetOrigin(nXSrc, nYSrc);
                sdc.GetPrimaryDC(hdcDest, hdcSrc);
                res = (*pGDIBitBlt)(sdc.GetHdc(), nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
                sdc.PutPrimaryDC(hdcDest, TRUE, nXDest, nYDest, nWidth, nHeight);
                break;
            case 2: // screen to memory
                sdc.GetPrimaryDC(hdcSrc);
                res = (*pGDIBitBlt)(hdcDest, nXDest, nYDest, nWidth, nHeight, sdc.GetHdc(), nXSrc, nYSrc, dwRop);
                sdc.PutPrimaryDC(hdcSrc, TRUE, nXDest, nYDest, nWidth, nHeight);
                break;
            }
            break;
        case GDIMODE_STRETCHED:
            nWDest = nWidth;
            nHDest = nHeight;
            switch(Flux) {
            case 1: // memory to screen
                // v1.03.58: BitBlt can blitfrom negative coordinates, StretchBlt can't!
                if(nXDest < 0) {
                    int nXshift = -nXDest;
                    nXDest = 0;
                    nXSrc += nXshift;
                    nWidth -= nXshift;
                    nWDest -= nXshift;
                }
                if(nYDest < 0) {
                    int nYshift = -nYDest;
                    nYDest = 0;
                    nYSrc += nYshift;
                    nHeight -= nYshift;
                    nHDest -= nYshift;
                }
                dxw.MapClient(&nXDest, &nYDest, &nWDest, &nHDest);
                OutDebugDW("%s: FIXED dest pos=(%d,%d) size=(%d,%d)\n", ApiRef, nXDest, nYDest, nWDest, nHDest);
                res = (*pGDIStretchBlt)(hdcDest, nXDest, nYDest, nWDest, nHDest, hdcSrc, nXSrc, nYSrc, nWidth, nHeight, dwRop);
                break;
            case 2: // screen to memory
                dxw.MapClient(&nXSrc, &nYSrc, &nWidth, &nHeight);
                res = (*pGDIStretchBlt)(hdcDest, nXDest, nYDest, nWDest, nHDest, hdcSrc, nXSrc, nYSrc, nWidth, nHeight, dwRop);
                break;
            default:
                // v2.04.32: avoid StretchBlt for intra-memory or intra-video operations!
                // Fixes "Avernum 3" transparency problems
                res = (*pGDIBitBlt)(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
                break;
            }
            OutDebugDW("%s: DEBUG DC dest=(%d,%d) size=(%d,%d)\n", ApiRef, nXDest, nYDest, nWDest, nHDest);
            break;
        case GDIMODE_EMULATED:
            if (hdcDest == dxw.RealHDC)
                hdcDest = dxw.VirtualHDC;
            res = (*pGDIBitBlt)(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
            OutDebugDW("%s: DEBUG emulated hdc dest=%#x->%#x\n", ApiRef, dxw.RealHDC, hdcDest);
            break;
        default:
            res = (*pGDIBitBlt)(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
            break;
        }
    } else
        res = (*pGDIBitBlt)(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
    if(IsDCLeakageSrc) (*pGDIReleaseDC)(dxw.GethWnd(), hdcSrc);
    if(IsDCLeakageDest) (*pGDIReleaseDC)(dxw.GethWnd(), hdcDest);
    if(res && IsToScreen) {
        dxw.ShowOverlay(hdcDest);
        if(dxw.dwDFlags & MARKGDI32) dxw.Mark(hdcDest, FALSE, RGB(0, 0, 255), nXDest, nYDest, nWidth, nHeight);
    }
    _if(!res) OutTraceE("%s: ERROR err=%d at %d\n", ApiRef, GetLastError(), __LINE__);
    return res;
}

BOOL WINAPI extGDIStretchBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight,
                             HDC hdcSrc, int nXSrc, int nYSrc, int nWSrc, int nHSrc, DWORD dwRop) {
    BOOL res;
    BOOL IsToScreen;
    BOOL IsFromScreen;
    BOOL IsDCLeakageSrc = FALSE;
    BOOL IsDCLeakageDest = FALSE;
    int Flux;
    OutTraceSYS("GDI.StretchBlt: HDC=%#x nXDest=%d nYDest=%d nWidth=%d nHeight=%d hdcSrc=%#x nXSrc=%d nYSrc=%d nWSrc=%d nHSrc=%d dwRop=%#x(%s)\n",
                hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, nWSrc, nHSrc, dwRop, ExplainROP(dwRop));
    OutDebugSYS("GDI.StretchBlt: DEBUG FullScreen=%#x target hdctype=%#x(%s) hwnd=%#x\n",
                dxw.IsFullScreen(), (*pGetObjectType)(hdcDest), ExplainDCType((*pGetObjectType)(hdcDest)), WindowFromDC(hdcDest));
    if(dxw.GDIEmulationMode == GDIMODE_EMULATED) {
        if (hdcDest == dxw.RealHDC) hdcDest = dxw.VirtualHDC;
        OutDebugDW("GDI.StretchBlt: DEBUG emulated hdc dest=%#x->%#x\n", dxw.RealHDC, hdcDest);
    }
    if(hdcDest == NULL) {
        // happens in Reah, hdc is NULL despite the fact that BeginPaint returns a valid DC. Too bad, we recover here ...
        hdcDest = (*pGDIGetDC)(dxw.GethWnd());
        OutDebugDW("GDI.StretchBlt: DEBUG hdc dest=NULL->%#x\n", hdcDest);
        IsDCLeakageDest = TRUE;
    }
    if(hdcSrc == NULL) {
        hdcSrc = (*pGDIGetDC)(dxw.GethWnd());
        OutDebugDW("GDI.StretchBlt: DEBUG hdc src=NULL->%#x\n", hdcSrc);
        IsDCLeakageSrc = TRUE;
    }
    IsToScreen = (OBJ_DC == (*pGetObjectType)(hdcDest));
    IsFromScreen = (OBJ_DC == (*pGetObjectType)(hdcSrc));
    Flux = (IsToScreen ? 1 : 0) + (IsFromScreen ? 2 : 0);
    if (IsToScreen && (dxw.dwDFlags & NOGDIBLT)) return TRUE;
#ifndef DXW_NOTRACES
    if(IsToScreen && (dxw.dwDFlags & DUMPDEVCONTEXT) && dxw.bCustomKeyToggle) DumpHDC(hdcSrc, nXSrc, nYSrc, nWidth, nHeight);
#endif // DXW_NOTRACES
    if(IsToScreen) dxw.HandleFPS(); // handle refresh delays
    if(dxw.IsToRemap(hdcDest) && (hdcDest != hdcSrc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            switch(Flux) {
            case 0: // memory to memory
                res = (*pGDIStretchBlt)(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, nWSrc, nHSrc, dwRop);
                break;
            case 1: // memory to screen
            case 3: // screen to screen
                sdc.GetPrimaryDC(hdcDest, hdcSrc);
                sdc.SetOrigin(nXSrc, nYSrc);
                res = (*pGDIStretchBlt)(sdc.GetHdc(), nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, nWSrc, nHSrc, dwRop);
                sdc.PutPrimaryDC(hdcDest, TRUE, nXDest, nYDest, nWidth, nHeight);
                break;
            case 2: // screen to memory using virtual screen
                sdc.GetPrimaryDC(hdcSrc);
                res = (*pGDIStretchBlt)(hdcDest, nXDest, nYDest, nWidth, nHeight, sdc.GetHdc(), nXSrc, nYSrc, nWSrc, nHSrc, dwRop);
                sdc.PutPrimaryDC(hdcSrc, TRUE, nXSrc, nYSrc, nWSrc, nHSrc);
                break;
            }
            break;
        case GDIMODE_STRETCHED: {
            int nWDest, nHDest;
            nWDest = nWidth;
            nHDest = nHeight;
            switch(Flux) {
            case 1: // memory to screen
                dxw.MapClient(&nXDest, &nYDest, &nWDest, &nHDest);
                break;
            case 2: // screen to memory
                dxw.MapClient(&nXSrc, &nYSrc, &nWidth, &nHeight);
                break;
            default:
                break;
            }
            res = (*pGDIStretchBlt)(hdcDest, nXDest, nYDest, nWDest, nHDest, hdcSrc, nXSrc, nYSrc, nWSrc, nHSrc, dwRop);
            OutDebugDW("GDI.StretchBlt: DEBUG DC dest=(%d,%d) size=(%d,%d)\n", nXDest, nYDest, nWDest, nHDest);
        }
        break;
        case GDIMODE_EMULATED:
        default:
            res = (*pGDIStretchBlt)(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, nWSrc, nHSrc, dwRop);
            break;
        }
    } else
        res = (*pGDIStretchBlt)(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, nWSrc, nHSrc, dwRop);
    if(IsDCLeakageSrc) (*pGDIReleaseDC)(dxw.GethWnd(), hdcSrc);
    if(IsDCLeakageDest) (*pGDIReleaseDC)(dxw.GethWnd(), hdcDest);
    if(res && IsToScreen) {
        dxw.ShowOverlay(hdcDest);
        if(dxw.dwDFlags & MARKGDI32) dxw.Mark(hdcDest, FALSE, RGB(0, 250, 250), nXDest, nYDest, nWidth, nHeight);
    }
    _if(!res) OutTraceE("GDI.StretchBlt: ERROR err=%d at %d\n", GetLastError(), __LINE__);
    return res;
}

BOOL WINAPI extGDIPatBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, DWORD dwRop) {
    BOOL res;
    BOOL IsToScreen;
    BOOL IsDCLeakageDest = FALSE;
    OutTraceSYS("GDI.PatBlt: HDC=%#x nXDest=%d nYDest=%d nWidth=%d nHeight=%d dwRop=%#x(%s)\n",
                hdcDest, nXDest, nYDest, nWidth, nHeight, dwRop, ExplainROP(dwRop));
    OutDebugSYS("GDI.PatBlt: DEBUG FullScreen=%#x target hdctype=%#x(%s) hwnd=%#x\n",
                dxw.IsFullScreen(), (*pGetObjectType)(hdcDest), ExplainDCType((*pGetObjectType)(hdcDest)), WindowFromDC(hdcDest));
    if(dxw.GDIEmulationMode == GDIMODE_EMULATED) {
        if (hdcDest == dxw.RealHDC) hdcDest = dxw.VirtualHDC;
        OutDebugDW("GDI.PatBlt: DEBUG emulated hdc dest=%#x->%#x\n", dxw.RealHDC, hdcDest);
    }
    if(hdcDest == NULL) {
        // happens in Reah, hdc is NULL despite the fact that BeginPaint returns a valid DC. Too bad, we recover here ...
        hdcDest = (*pGDIGetDC)(dxw.GethWnd());
        OutDebugDW("GDI.PatBlt: DEBUG hdc dest=NULL->%#x\n", hdcDest);
        IsDCLeakageDest = TRUE;
    }
    IsToScreen = (OBJ_DC == (*pGetObjectType)(hdcDest));
    if (IsToScreen && (dxw.dwDFlags & NOGDIBLT)) return TRUE;
    if(dxw.IsToRemap(hdcDest)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdcDest);
            res = (*pGDIPatBlt)(sdc.GetHdc(),  nXDest, nYDest, nWidth, nHeight, dwRop);
            sdc.PutPrimaryDC(hdcDest, TRUE, nXDest, nYDest, nWidth, nHeight);
            return res;
            break;
        case GDIMODE_STRETCHED: {
            int nWDest, nHDest;
            nWDest = nWidth;
            nHDest = nHeight;
            dxw.MapClient(&nXDest, &nYDest, &nWDest, &nHDest);
            res = (*pGDIPatBlt)(hdcDest, nXDest, nYDest, nWDest, nHDest, dwRop);
            OutDebugDW("GDI.PatBlt: DEBUG DC dest=(%d,%d) size=(%d,%d)\n", nXDest, nYDest, nWDest, nHDest);
        }
        break;
        case GDIMODE_EMULATED:
        default:
            res = (*pGDIPatBlt)(hdcDest,  nXDest, nYDest, nWidth, nHeight, dwRop);
            break;
        }
    } else
        res = (*pGDIPatBlt)(hdcDest,  nXDest, nYDest, nWidth, nHeight, dwRop);
    if(IsDCLeakageDest) (*pGDIReleaseDC)(dxw.GethWnd(), hdcDest);
    if(res && IsToScreen) {
        dxw.ShowOverlay(hdcDest);
        if(dxw.dwDFlags & MARKGDI32) dxw.Mark(hdcDest, FALSE, RGB(255, 255, 0), nXDest, nYDest, nWidth, nHeight);
    }
    _if(!res) OutTraceE("GDI.PatBlt: ERROR err=%d at %d\n", GetLastError(), __LINE__);
    return res;
}

// NOTE: when creating a font with FIXTEXTOUT option, you have to build two fonts actually, because
// the scaled font should be used only with video DC, while memory DC should use the original sized
// font since the whole DC will be scaled later, eventually.

HFONT WINAPI extCreateFontA(int nHeight, int nWidth, int nEscapement, int nOrientation, int fnWeight,
                            DWORD fdwItalic, DWORD fdwUnderline, DWORD fdwStrikeOut, DWORD fdwCharSet,
                            DWORD fdwOutputPrecision, DWORD fdwClipPrecision, DWORD fdwQuality,
                            DWORD fdwPitchAndFamily, LPCTSTR lpszFace) {
    ApiName("CreateFontA");
    HFONT HFont, HScaled;
    OutTraceSYS("%s: h=%d w=%d face=\"%s\"\n", ApiRef, nHeight, nWidth, lpszFace); // v2.05.35 fix
    if(IsDebugSYS) {
        OutTrace("> Escapement: %d\n", nEscapement);
        OutTrace("> Orientation: %d\n", nOrientation);
        OutTrace("> fnWeight: %d\n", fnWeight);
        OutTrace("> fdwItalic: %d\n", fdwItalic);
        OutTrace("> fdwUnderline: %d\n", fdwUnderline);
        OutTrace("> fdwStrikeOut: %d\n", fdwStrikeOut);
        OutTrace("> fdwCharSet: %d\n", fdwCharSet);
        OutTrace("> fdwOutputPrecision: %d\n", fdwOutputPrecision);
        OutTrace("> fdwClipPrecision: %d\n", fdwClipPrecision);
        OutTrace("> fdwQuality: %d\n", fdwQuality);
        OutTrace("> fdwPitchAndFamily: %d\n", fdwPitchAndFamily);
    }
    if(dxw.dwFlags8 & QUALITYFONTS) fdwQuality = BEST_QUALITY;
    if(dxw.dwFlags11 & SHRINKFONTWIDTH) {
        nWidth = (nWidth * 9) / 10;
        OutTraceDW("%s: shrink font size width=%d\n", ApiRef, nWidth);
    }
    HFont = (*pGDICreateFontA)(nHeight, nWidth, nEscapement, nOrientation, fnWeight,
                               fdwItalic, fdwUnderline, fdwStrikeOut, fdwCharSet,
                               fdwOutputPrecision, fdwClipPrecision, fdwQuality,
                               fdwPitchAndFamily, lpszFace);
    if((dxw.dwFlags1 & FIXTEXTOUT) && HFont) {
        if(nHeight > 0) dxw.MapClient(&nWidth, &nHeight);
        else {
            nHeight = -nHeight;
            dxw.MapClient(&nWidth, &nHeight);
            nHeight = -nHeight;
        }
        HScaled = (*pGDICreateFontA)(nHeight, nWidth, nEscapement, nOrientation, fnWeight,
                                     fdwItalic, fdwUnderline, fdwStrikeOut, fdwCharSet,
                                     fdwOutputPrecision, fdwClipPrecision, fdwQuality,
                                     fdwPitchAndFamily, lpszFace);
        if(HScaled) {
            OutTraceSYS("%s: associate font=%#x scaled=%#x\n", ApiRef, HFont, HScaled);
            fontdb.Push(HFont, HScaled);
        } else
            OutTraceE("%s: ERROR scaled font err=%d\n", ApiRef, GetLastError());
    }
    if(HFont)
        OutTraceSYS("%s: hfont=%#x\n", ApiRef, HFont);
    else
        OutTraceSYS("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return HFont;
}

// CreateFontIndirect hook routine to avoid font aliasing that prevents reverse blitting working on palettized surfaces
// NONANTIALIASED_QUALITY no longer necessary, since reverse blitting is no longer used

HFONT WINAPI extCreateFontIndirectA(const LOGFONT *lplf) {
    ApiName("CreateFontIndirectA");
    HFONT HFont, HScaled;
    LOGFONT lf;
    OutTraceSYS("%s: h=%d w=%d face=\"%s\"\n", ApiRef, lplf->lfHeight, lplf->lfWidth, lplf->lfFaceName);
    if(IsDebugSYS) {
        OutTrace("> Escapement: %d\n", lplf->lfEscapement);
        OutTrace("> Orientation: %d\n", lplf->lfOrientation);
        OutTrace("> fnWeight: %d\n", lplf->lfWeight);
        OutTrace("> fdwItalic: %d\n", lplf->lfItalic);
        OutTrace("> fdwUnderline: %d\n", lplf->lfUnderline);
        OutTrace("> fdwStrikeOut: %d\n", lplf->lfStrikeOut);
        OutTrace("> fdwCharSet: %d\n", lplf->lfCharSet);
        OutTrace("> fdwOutputPrecision: %d\n", lplf->lfOutPrecision);
        OutTrace("> fdwClipPrecision: %d\n", lplf->lfClipPrecision);
        OutTrace("> fdwQuality: %d\n", lplf->lfQuality);
        OutTrace("> fdwPitchAndFamily: %d\n", lplf->lfPitchAndFamily);
    }
    memcpy((char *)&lf, (char *)lplf, sizeof(LOGFONT));
    if(dxw.dwFlags8 & QUALITYFONTS) lf.lfQuality = BEST_QUALITY;
    if(dxw.dwFlags11 & SHRINKFONTWIDTH) {
        lf.lfWidth = (lf.lfWidth * 9) / 10;
        OutTraceDW("%s: shrink font size width=%d\n", ApiRef, lf.lfWidth);
    }
    HFont = (*pGDICreateFontIndirectA)(&lf);
    if(HFont && (dxw.dwFlags1 & FIXTEXTOUT)) { // v2.04.93: don't build a scaled font if the first failed.
        memcpy((char *)&lf, (char *)lplf, sizeof(LOGFONT));
        if(dxw.dwFlags8 & QUALITYFONTS) lf.lfQuality = BEST_QUALITY;
        if(lf.lfHeight > 0) dxw.MapClient((int *)&lf.lfWidth, (int *)&lf.lfHeight);
        else {
            lf.lfHeight = -lf.lfHeight;
            dxw.MapClient((int *)&lf.lfWidth, (int *)&lf.lfHeight);
            lf.lfHeight = -lf.lfHeight;
        }
        HScaled = ((*pGDICreateFontIndirectA)(&lf));
        if(HScaled) {
            OutTraceSYS("%s: associate font=%#x scaled=%#x\n", ApiRef, HFont, HScaled);
            fontdb.Push(HFont, HScaled);
        } else
            OutTraceE("%s: ERROR scaled font err=%d\n", ApiRef, GetLastError());
    }
    if(HFont)
        OutTraceSYS("CreateFontIndirect: hfont=%#x\n", HFont);
    else
        OutTraceE("CreateFontIndirect ERROR: err=%d\n", GetLastError());
    return HFont;
}

BOOL WINAPI extSetDeviceGammaRamp(HDC hDC, LPVOID lpRamp) {
    BOOL ret;
    OutTraceSYS("SetDeviceGammaRamp: hdc=%#x\n", hDC);
    if(dxw.dwFlags2 & DISABLEGAMMARAMP) {
        OutTraceSYS("SetDeviceGammaRamp: SUPPRESS\n");
        return TRUE;
    }
    if((dxw.dwFlags10 & FORCED3DGAMMARAMP) && (dxw.pInitialRamp == NULL)) dxw.InitGammaRamp();
    ret = (*pGDISetDeviceGammaRamp)(hDC, lpRamp);
    _if(!ret) OutTraceE("SetDeviceGammaRamp: ERROR err=%d\n", GetLastError());
    return ret;
}

BOOL WINAPI extGetDeviceGammaRamp(HDC hDC, LPVOID lpRamp) {
    BOOL ret;
    OutTraceSYS("GetDeviceGammaRamp: hdc=%#x\n", hDC);
    ret = (*pGDIGetDeviceGammaRamp)(hDC, lpRamp);
    _if(!ret) OutTraceE("GetDeviceGammaRamp: ERROR err=%d\n", GetLastError());
    return ret;
}

int WINAPI extGetClipBox(HDC hdc, LPRECT lprc) {
    // v2.02.31: needed in "Imperialism II" to avoid blit clipping
    int ret;
    ApiName("GetClipBox");
    OutTraceSYS("%s: hdc=%#x\n", ApiRef, hdc);
    if (dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pGDIGetClipBox)(sdc.GetHdc(), lprc);
            sdc.PutPrimaryDC(hdc, FALSE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            ret = (*pGDIGetClipBox)(hdc, lprc);
            OutTraceSYS("%s: scaling main win coordinates (%d,%d)-(%d,%d)\n",
                        ApiRef, lprc->left, lprc->top, lprc->right, lprc->bottom);
            dxw.UnmapClient(lprc);
            break;
        default:
            ret = (*pGDIGetClipBox)(hdc, lprc);
            break;
        }
    } else
        ret = (*pGDIGetClipBox)(hdc, lprc);
    OutTraceSYS("%s: ret=%#x(%s) rect=(%d,%d)-(%d,%d)\n",
                ApiRef, ret, ExplainRegionType(ret), lprc->left, lprc->top, lprc->right, lprc->bottom);
    return ret;
}

static int WINAPI HandleClipRect(
    HandleClipRect_Type pHandleClipRect,
    char *api,
    HDC hdc, int nLeftRect, int nTopRect, int nRightRect, int nBottomRect) {
    int ret;
    OutTraceSYS("%s: hdc=%#x rect=(%d,%d)-(%d,%d)\n", api, hdc, nLeftRect, nTopRect, nRightRect, nBottomRect);
    if (dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pHandleClipRect)(sdc.GetHdc(), nLeftRect, nTopRect, nRightRect, nBottomRect);
            sdc.PutPrimaryDC(hdc, FALSE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&nLeftRect, &nTopRect, &nRightRect, &nBottomRect);
            OutTraceSYS("%s: fixed rect=(%d,%d)-(%d,%d)\n", api, nLeftRect, nTopRect, nRightRect, nBottomRect);
            break;
        }
    }
    ret = (*pHandleClipRect)(hdc, nLeftRect, nTopRect, nRightRect, nBottomRect);
    OutTraceSYS("%s: ret=%#x(%s)\n", api, ret, ExplainRegionType(ret));
    return ret;
}

int WINAPI extExcludeClipRect(HDC hdc, int nLeftRect, int nTopRect, int nRightRect, int nBottomRect) {
    return HandleClipRect(pExcludeClipRect, "ExcludeClipRect", hdc, nLeftRect, nTopRect, nRightRect, nBottomRect);
}
int WINAPI extIntersectClipRect(HDC hdc, int nLeftRect, int nTopRect, int nRightRect, int nBottomRect) {
    return HandleClipRect(pIntersectClipRect, "IntersectClipRect", hdc, nLeftRect, nTopRect, nRightRect, nBottomRect);
}

BOOL WINAPI extPolyline(HDC hdc, const POINT *lppt, int cPoints) {
    // LOGTOBEFIXED
    BOOL ret;
    ApiName("Polyline");
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        int i;
        OutTrace("%s: hdc=%#x cPoints=%d pt=", ApiRef, hdc, cPoints);
        for(i = 0; i < cPoints; i++) OutTrace("(%d,%d) ", lppt[i].x, lppt[i].y);
        OutTrace("\n");
    }
#endif
    if (dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pPolyline)(sdc.GetHdc(), lppt, cPoints);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            for(int i = 0; i < cPoints; i++) dxw.MapClient((LPPOINT)&lppt[i]);
#ifndef DXW_NOTRACES
            if(IsTraceSYS) {
                OutTrace("%s: fixed cPoints=%d pt=", ApiRef, cPoints);
                for(int i = 0; i < cPoints; i++) OutTrace("(%d,%d) ", lppt[i].x, lppt[i].y);
                OutTrace("\n");
            }
#endif
            break;
        }
    }
    ret = (*pPolyline)(hdc, lppt, cPoints);
    _if(!ret)OutTraceE("%s: ERROR ret=%#x\n", ApiRef, ret);
    return ret;
}

BOOL WINAPI extLineTo(HDC hdc, int nXEnd, int nYEnd) {
    BOOL ret;
    ApiName("LineTo");
    OutTraceSYS("%s: hdc=%#x pt=(%d,%d)\n", ApiRef, hdc, nXEnd, nYEnd);
    if (dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pLineTo)(sdc.GetHdc(), nXEnd, nYEnd);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&nXEnd, &nYEnd);
            OutTraceSYS("%s: fixed pt=(%d,%d)\n", ApiRef, nXEnd, nYEnd);
            break;
        }
    }
    ret = (*pLineTo)(hdc, nXEnd, nYEnd);
    _if(!ret)OutTraceE("%s: ERROR ret=%#x\n", ApiRef, ret);
    return ret;
}

BOOL WINAPI extArcTo(HDC hdc, int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, int nXRadial1, int nYRadial1, int nXRadial2, int nYRadial2) {
    BOOL ret;
    ApiName("ArcTo");
    OutTraceSYS("%s: hdc=%#x rect=(%d,%d)(%d,%d) radial=(%d,%d)(%d,%d)\n",
                ApiRef, hdc, nLeftRect, nTopRect, nRightRect, nBottomRect, nXRadial1, nYRadial1, nXRadial2, nYRadial2);
    if (dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pArcTo)(sdc.GetHdc(), nLeftRect, nTopRect, nRightRect, nBottomRect, nXRadial1, nYRadial1, nXRadial2, nYRadial2);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&nLeftRect, &nTopRect, &nRightRect, &nBottomRect);
            dxw.MapClient(&nXRadial1, &nYRadial1, &nXRadial2, &nYRadial2);
            OutTraceSYS("%s: fixed rect=(%d,%d)(%d,%d) radial=(%d,%d)(%d,%d)\n",
                        ApiRef, nLeftRect, nTopRect, nRightRect, nBottomRect, nXRadial1, nYRadial1, nXRadial2, nYRadial2);
            break;
        }
    }
    ret = (*pArcTo)(hdc, nLeftRect, nTopRect, nRightRect, nBottomRect, nXRadial1, nYRadial1, nXRadial2, nYRadial2);
    _if(!ret)OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extMoveToEx(HDC hdc, int X, int Y, LPPOINT lpPoint) {
    BOOL ret;
    ApiName("MoveToEx");
    OutTraceSYS("%s: hdc=%#x pt=(%d,%d)\n", ApiRef, hdc, X, Y);
    if (dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pMoveToEx)(sdc.GetHdc(), X, Y, lpPoint);
            sdc.PutPrimaryDC(hdc, FALSE);
            return ret; // no need to update the screen!
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&X, &Y);
            OutTraceSYS("%s: fixed pt=(%d,%d)\n", ApiRef, X, Y);
            break;
        }
    }
    ret = (*pMoveToEx)(hdc, X, Y, lpPoint);
    _if(!ret)OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extPolyDraw(HDC hdc, const POINT *lpPoints, const BYTE *lpbTypes, int cCount) {
    BOOL ret;
    ApiName("PolyDraw");
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        OutTrace("%s: hdc=%#x cCount=%d\n", ApiRef, hdc, cCount);
        for(int i = 0; i < cCount; i++) OutTrace("> pt[%d]=(%d,%d)\n", i, lpPoints[i].x, lpPoints[i].y);
    }
#endif
    if (dxw.IsToRemap(hdc)) {
        int i, size;
        POINT *lpRemPoints;
        size = cCount * sizeof(POINT);
        lpRemPoints = (LPPOINT)malloc(size);
        memcpy(lpRemPoints, lpPoints, size);
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            free(lpRemPoints);
            sdc.GetPrimaryDC(hdc);
            ret = (*pPolyDraw)(sdc.GetHdc(), lpPoints, lpbTypes, cCount);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            for(i = 0; i < cCount; i++)
                dxw.MapClient(&lpRemPoints[i]);
#ifndef DXW_NOTRACES
            if(IsTraceSYS) {
                OutTrace("%s: fixed cCount=%d\n", ApiRef, cCount);
                for(int i = 0; i < cCount; i++) OutTrace("> pt[%d]=(%d,%d)\n", i, lpPoints[i].x, lpPoints[i].y);
            }
#endif
            break;
        case GDIMODE_EMULATED:
            if (dxw.IsVirtual(hdc)) {
                for(i = 0; i < cCount; i++) {
                    lpRemPoints[i].x += dxw.VirtualOffsetX;
                    lpRemPoints[i].y += dxw.VirtualOffsetY;
                }
#ifndef DXW_NOTRACES
                if(IsTraceSYS) {
                    OutTrace("%s: fixed cCount=%d\n", ApiRef, cCount);
                    for(int i = 0; i < cCount; i++) OutTrace("> pt[%d]=(%d,%d)\n", i, lpPoints[i].x, lpPoints[i].y);
                }
#endif
            }
            break;
        }
        ret = (*pPolyDraw)(hdc, lpRemPoints, lpbTypes, cCount);
        free(lpRemPoints);
    } else
        ret = (*pPolyDraw)(hdc, lpPoints, lpbTypes, cCount);
    _if(!ret)OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extPolylineTo(HDC hdc, const POINT *lpPoints, DWORD cCount) {
    BOOL ret;
    ApiName("PolylineTo");
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        OutTrace("%s: hdc=%#x cCount=%d\n", ApiRef, hdc, cCount);
        for(DWORD i = 0; i < cCount; i++) OutTrace("> pt[%d]=(%d,%d)\n", i, lpPoints[i].x, lpPoints[i].y);
    }
#endif
    if (dxw.IsToRemap(hdc)) {
        DWORD i, size;
        POINT *lpRemPoints;
        size = cCount * sizeof(POINT);
        lpRemPoints = (LPPOINT)malloc(size);
        memcpy(lpRemPoints, lpPoints, size);
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            free(lpRemPoints);
            sdc.GetPrimaryDC(hdc);
            ret = (*pPolylineTo)(sdc.GetHdc(), lpPoints, cCount);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            for(i = 0; i < cCount; i++)
                dxw.MapClient(&lpRemPoints[i]);
#ifndef DXW_NOTRACES
            if(IsTraceSYS) {
                OutTrace("%s: fixed cCount=%d\n", ApiRef, cCount);
                for(DWORD i = 0; i < cCount; i++) OutTrace("> pt[%d]=(%d,%d)\n", i, lpPoints[i].x, lpPoints[i].y);
            }
#endif
            break;
        case GDIMODE_EMULATED:
            if (dxw.IsVirtual(hdc)) {
                for(i = 0; i < cCount; i++) {
                    lpRemPoints[i].x += dxw.VirtualOffsetX;
                    lpRemPoints[i].y += dxw.VirtualOffsetY;
                }
#ifndef DXW_NOTRACES
                if(IsTraceSYS) {
                    OutTrace("%s: fixed cCount=%d\n", ApiRef, cCount);
                    for(DWORD i = 0; i < cCount; i++) OutTrace("> pt[%d]=(%d,%d)\n", i, lpPoints[i].x, lpPoints[i].y);
                }
#endif
            }
            break;
        }
        ret = (*pPolylineTo)(hdc, lpRemPoints, cCount);
        free(lpRemPoints);
    } else
        ret = (*pPolylineTo)(hdc, lpPoints, cCount);
    _if(!ret)OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extPolyBezierTo(HDC hdc, const POINT *lpPoints, DWORD cCount) {
    BOOL ret;
    ApiName("PolyBezierTo");
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        OutTrace("%s: hdc=%#x cCount=%d\n", ApiRef, hdc, cCount);
        for(DWORD i = 0; i < cCount; i++) OutTrace("> pt[%d]=(%d,%d)\n", i, lpPoints[i].x, lpPoints[i].y);
    }
#endif
    if (dxw.IsToRemap(hdc)) {
        DWORD i, size;
        POINT *lpRemPoints;
        size = cCount * sizeof(POINT);
        lpRemPoints = (LPPOINT)malloc(size);
        memcpy(lpRemPoints, lpPoints, size);
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            free(lpRemPoints);
            sdc.GetPrimaryDC(hdc);
            ret = (*pPolyBezierTo)(sdc.GetHdc(), lpPoints, cCount);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            for(i = 0; i < cCount; i++)
                dxw.MapClient(&lpRemPoints[i]);
#ifndef DXW_NOTRACES
            if(IsTraceSYS) {
                OutTrace("%s: fixed cCount=%d\n", ApiRef, cCount);
                for(i = 0; i < cCount; i++) OutTrace("> pt[%d]=(%d,%d)\n", i, &lpRemPoints[i].x, &lpRemPoints[i].y);
            }
#endif
            break;
        case GDIMODE_EMULATED:
            if (dxw.IsVirtual(hdc)) {
                for(i = 0; i < cCount; i++) {
                    lpRemPoints[i].x += dxw.VirtualOffsetX;
                    lpRemPoints[i].y += dxw.VirtualOffsetY;
                }
#ifndef DXW_NOTRACES
                if(IsTraceSYS) {
                    OutTrace("%s: fixed cCount=%d\n", ApiRef, cCount);
                    for(i = 0; i < cCount; i++) OutTrace("> pt[%d]=(%d,%d)\n", i, &lpRemPoints[i].x, &lpRemPoints[i].y);
                }
#endif
            }
            break;
        }
        ret = (*pPolyBezierTo)(hdc, lpRemPoints, cCount);
        free(lpRemPoints);
    } else
        ret = (*pPolyBezierTo)(hdc, lpPoints, cCount);
    _if(!ret)OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extPolyBezier(HDC hdc, const POINT *lpPoints, DWORD cCount) {
    BOOL ret;
    ApiName("PolyBezier");
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        DWORD i;
        OutTrace("%s: hdc=%#x cCount=%d\n", ApiRef, hdc, cCount);
        for(i = 0; i < cCount; i++) OutTrace("> pt[%d]=(%d,%d)\n", i, lpPoints[i].x, lpPoints[i].y);
    }
#endif
    if (dxw.IsToRemap(hdc)) {
        DWORD i, size;
        POINT *lpRemPoints;
        size = cCount * sizeof(POINT);
        lpRemPoints = (LPPOINT)malloc(size);
        memcpy(lpRemPoints, lpPoints, size);
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            free(lpRemPoints);
            sdc.GetPrimaryDC(hdc);
            ret = (*pPolyBezier)(sdc.GetHdc(), lpPoints, cCount);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            for(i = 0; i < cCount; i++)
                dxw.MapClient(&lpRemPoints[i]);
#ifndef DXW_NOTRACES
            if(IsTraceSYS) {
                OutTrace("%s: fixed cCount=%d\n", ApiRef, cCount);
                for(i = 0; i < cCount; i++) OutTrace("> pt[%d]=(%d,%d)\n", i, lpPoints[i].x, lpPoints[i].y);
            }
#endif
            break;
        case GDIMODE_EMULATED:
            if (dxw.IsVirtual(hdc)) {
                for(i = 0; i < cCount; i++) {
                    lpRemPoints[i].x += dxw.VirtualOffsetX;
                    lpRemPoints[i].y += dxw.VirtualOffsetY;
                }
#ifndef DXW_NOTRACES
                if(IsTraceSYS) {
                    OutTrace("%s: fixed cCount=%d\n", ApiRef, cCount);
                    for(i = 0; i < cCount; i++) OutTrace("> pt[%d]=(%d,%d)\n", i, lpPoints[i].x, lpPoints[i].y);
                }
#endif
            }
            break;
        }
        ret = (*pPolyBezier)(hdc, lpRemPoints, cCount);
        free(lpRemPoints);
    } else
        ret = (*pPolyBezier)(hdc, lpPoints, cCount);
    _if(!ret)OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

int WINAPI extStretchDIBits(HDC hdc, int XDest, int YDest, int nDestWidth, int nDestHeight, int XSrc, int YSrc, int nSrcWidth, int nSrcHeight,
                            const VOID *lpBits, const BITMAPINFO *lpBitsInfo, UINT iUsage, DWORD dwRop) {
    int ret;
    ApiName("StretchDIBits");
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        OutTraceSYS("%s: hdc=%#x dest=(%d,%d):(%dx%d) src=(%d,%d):(%dx%d) rop=%#x(%s)\n",
                    ApiRef, hdc, XDest, YDest, nDestWidth, nDestHeight, XSrc, YSrc, nSrcWidth, nSrcHeight, dwRop, ExplainROP(dwRop));
        TraceBITMAPINFOHEADER(ApiRef, (BITMAPINFOHEADER *) & (lpBitsInfo->bmiHeader));
    }
#endif
    if(dxw.IsToRemap(hdc)) {
        dxw.HandleDIB(); // handle refresh delays
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pStretchDIBits)(sdc.GetHdc(), XDest, YDest, nDestWidth, nDestHeight, XSrc, YSrc, nSrcWidth, nSrcHeight, lpBits, lpBitsInfo, iUsage, dwRop);
            sdc.PutPrimaryDC(hdc, TRUE, XDest, YDest, nDestWidth, nDestHeight);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&XDest, &YDest, &nDestWidth, &nDestHeight);
            OutTraceSYS("%s: fixed STRETCHED dest=(%d,%d):(%dx%d)\n", ApiRef, XDest, YDest, nDestWidth, nDestHeight);
            break;
        case GDIMODE_EMULATED:
            if(dxw.IsVirtual(hdc)) {
                XDest += dxw.VirtualOffsetX;
                YDest += dxw.VirtualOffsetY;
                OutTraceSYS("%s: fixed EMULATED dest=(%d,%d):(%dx%d)\n", ApiRef, XDest, YDest, nDestWidth, nDestHeight);
            }
            break;
        default:
            break;
        }
    }
    ret = (*pStretchDIBits)(hdc, XDest, YDest, nDestWidth, nDestHeight, XSrc, YSrc, nSrcWidth, nSrcHeight, lpBits, lpBitsInfo, iUsage, dwRop);
    if(dxw.dwDFlags & MARKGDI32) dxw.Mark(hdc, FALSE, RGB(255, 0, 255), XDest, YDest, nDestWidth, nDestHeight);
    _if(!ret || (ret == GDI_ERROR)) OutTraceE("%s: ERROR ret=%#x err=%d\n", ApiRef, ret, GetLastError());
    return ret;
}

int WINAPI extSetDIBits(HDC hdc, HBITMAP hbmp, UINT uStartScan, UINT cScanLines, const VOID *lpvBits, const BITMAPINFO *lpbmi, UINT fuColorUse) {
    int ret;
    ApiName("SetDIBits");
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        OutTrace("%s: hdc=%#x%s hbmp=%#x lines=(%d,%d) ColorUse=%#x(%s)\n",
                 ApiRef, hdc, dxw.IsToRemap(hdc) ? "(R)" : "", hbmp, uStartScan, cScanLines, fuColorUse, ExplainDIBUsage(fuColorUse));
        TraceBITMAPINFOHEADER("SetDIBits", (BITMAPINFOHEADER *) & (lpbmi->bmiHeader));
    }
#endif
    // v2.04.60: hdc of primary window strangely returns 0 as GetObjectType (error) in "Honour & Freedom"
    //if(dxw.IsToRemap(hdc) && !bGDIRecursionFlag){
    if((dxw.IsToRemap(hdc) || ((*pGetObjectType)(hdc) == 0 )) && !bGDIRecursionFlag) {
        HDC hTempDc;
        HBITMAP hbmPic;
        DWORD OrigWidth, OrigHeight;
        dxw.HandleDIB(); // handle refresh delays
        HGDIOBJ obj;
        BOOL ret2;
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC: // this will flicker !!!!
            sdc.GetPrimaryDC(hdc);
            ret = (*pSetDIBits)(sdc.GetHdc(), hbmp, uStartScan, cScanLines, lpvBits, lpbmi, fuColorUse);
            _if(!ret || (ret == GDI_ERROR)) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
            sdc.PutPrimaryDC(hdc, TRUE, 0, 0, lpbmi->bmiHeader.biWidth, lpbmi->bmiHeader.biHeight);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            // v2.04.60: reinserted logic for stretching DIBs for "Honour & Freedom" 16bit DIB
            OrigWidth = lpbmi->bmiHeader.biWidth;
            OrigHeight = lpbmi->bmiHeader.biHeight;
            if(lpbmi->bmiHeader.biHeight < 0) OrigHeight = -lpbmi->bmiHeader.biHeight;
            // blitting to primary surface !!!
            dxw.MapClient((int *)&OrigWidth, (int *)&OrigHeight);
            OutTraceSYS("%s: fixed size=(%dx%d)\n", ApiRef, OrigWidth, OrigHeight);
            hTempDc = CreateCompatibleDC(hdc);
            _if(!hTempDc) OutTraceE("%s: CreateCompatibleDC ERROR err=%d at=%d\n", ApiRef, GetLastError(), __LINE__);
            // tricky part: CreateCompatibleBitmap is needed to set the dc size, but it has to be performed
            // against hdc to set for color depth, then selected (through SelectObject) against the temporary
            // dc to assign the needed size and color space to the temporary dc.
            hbmPic = CreateCompatibleBitmap(hdc, OrigWidth, OrigHeight);
            _if(!hbmPic) ("%s: CreateCompatibleBitmap ERROR err=%d at=%d\n", ApiRef, GetLastError(), __LINE__);
            obj = (*pSelectObject)(hTempDc, hbmPic);
            _if(!obj) OutTraceE("%s SelectObject ERROR err=%d at=%d\n", ApiRef, GetLastError(), __LINE__);
            if((lpbmi->bmiHeader.biBitCount == 8) && (dxw.dwFlags2 & PALDIBEMULATION)) {
                VOID *lpvNewBits;
                BITMAPINFO NewBmi;
                memcpy(&NewBmi, lpbmi, sizeof(BITMAPINFO));
                lpvNewBits = dxw.EmulateDIB((LPVOID)lpvBits, &NewBmi, fuColorUse);
#ifndef DXW_NOTRACES
                if(dxw.dwDFlags & DUMPDIBSECTION)
                    DumpDibSection(ApiRef, hdc, &NewBmi, DIB_RGB_COLORS, lpvNewBits);
#endif // DXW_NOTRACES
                ret = (*pSetDIBitsToDevice)(hTempDc, 0, 0, OrigWidth, OrigHeight, 0, 0, uStartScan, cScanLines, lpvNewBits, &NewBmi, DIB_RGB_COLORS);
                _if(!ret) OutTraceE("%s: SetDIBitsToDevice ERROR err=%d\n", ApiRef, GetLastError());
                free(lpvNewBits);
            } else {
                ret = (*pSetDIBits)(hTempDc, hbmp, uStartScan, cScanLines, lpvBits, lpbmi, fuColorUse);
                _if(!ret) OutTraceE("%s: SetDIBits ERROR err=%d at=%d\n", ApiRef, GetLastError(), __LINE__);
            }
            bGDIRecursionFlag = FALSE;
#ifndef DXW_NOTRACES
            if((dxw.dwDFlags & DUMPDEVCONTEXT) && dxw.bCustomKeyToggle) DumpHDC(hTempDc, 0, 0, OrigWidth, OrigHeight);
#endif // DXW_NOTRACES
            // v2.02.94: set HALFTONE stretching. Fixes "Celtic Kings Rage of War"
            SetStretchBltMode(hdc, HALFTONE);
            ret = (*pGDIStretchBlt)(hdc, 0, 0, lpbmi->bmiHeader.biWidth, lpbmi->bmiHeader.biHeight, hTempDc, 0, 0, OrigWidth, OrigHeight, SRCCOPY);
            _if(!ret) OutTraceE("%s: StretchBlt ERROR err=%d at=%d\n", ApiRef, GetLastError(), __LINE__);
            ret2 = DeleteObject(hbmPic); // v2.02.32 - avoid resource leakage
            _if(!ret2) OutTraceE("%s: DeleteObject ERROR err=%d at=%d\n", ApiRef, GetLastError(), __LINE__);
            ret2 = DeleteDC(hTempDc);
            _if(!ret2) OutTraceE("%s: DeleteDC ERROR err=%d at=%d\n", ApiRef, GetLastError(), __LINE__);
            if(dxw.dwDFlags & MARKGDI32) dxw.Mark(hdc, FALSE, RGB(255, 255, 255),  0, 0, lpbmi->bmiHeader.biWidth, lpbmi->bmiHeader.biHeight);
            return ret;
            break;
        case GDIMODE_EMULATED:
#if 0
            if (dxw.IsVirtual(hdc)) {
                int X, Y;
                X = XDest + dxw.VirtualOffsetX;
                Y = YDest + dxw.VirtualOffsetY;
                OutTraceSYS("SetDIBits: virtual pos=(%d,%d)+(%d+%d)=(%d,%d)\n",
                            XDest, YDest, dxw.VirtualOffsetX, dxw.VirtualOffsetY, X, Y);
                ret = (*pSetDIBits)(sdc.GetHdc(), hbmp, uStartScan, cScanLines, lpvBits, lpbmi, fuColorUse);
                if(!ret || (ret == GDI_ERROR)) OutTraceE("SetDIBits: ERROR ret=%#x err=%d\n", ret, GetLastError());
                return ret;
            }
#endif
            break;
        default:
            break;
        }
    }
    ret = (*pSetDIBits)(hdc, hbmp, uStartScan, cScanLines, lpvBits, lpbmi, fuColorUse);
#ifndef DXW_NOTRACES
    if(dxw.dwDFlags & DUMPDIBSECTION) DumpDibSection(ApiRef, hdc, lpbmi, fuColorUse, (LPBYTE)lpvBits);
#endif // DXW_NOTRACES
    _if(!ret || (ret == GDI_ERROR)) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

static BYTE InvertPalette32(DWORD pixel, LPPALETTEENTRY Pal) {
    DWORD BestCost = (DWORD) - 1;
    BYTE BestByte = 0;
    BYTE bRed   = pixel & 0x000000FF;
    BYTE bGreen = pixel & 0x0000FF00 >> 8;
    BYTE bBlue  = pixel & 0x00FF0000 >> 16;
    //BYTE bBlue  = pixel & 0x000000FF;
    //BYTE bGreen = pixel & 0x0000FF00 >> 8;
    //BYTE bRed   = pixel & 0x00FF0000 >> 16;
    for (int i = 0; i < 255; i++) {
        int RCost = (bRed - (Pal[i]).peRed);
        if(RCost < 0) RCost = -RCost;
        int GCost = (bGreen - (Pal[i]).peGreen);
        if(GCost < 0) GCost = -GCost;
        int BCost = (bBlue - (Pal[i]).peBlue);
        if(RCost < 0) BCost = -BCost;
        DWORD Cost = (RCost * RCost) + (GCost * GCost) + (BCost * BCost);
        if(Cost < BestCost) {
            BestByte = i;
            BestCost = Cost;
        }
    }
    //OutTrace("Pixel=%#x byte=%#x cost=%d\n", pixel, BestByte, BestCost);
    return BestByte;
}

int WINAPI extGetDIBits(HDC hdc, HBITMAP hbmp, UINT uStartScan, UINT cScanLines, LPVOID lpvBits, LPBITMAPINFO lpbmi, UINT uUsage) {
    int ret;
    ApiName("GetDIBits");
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        OutTrace("%s: hdc=%#x hbmp=%#x lines=(%d,%d) lpvbits=%#x ColorUse=%#x(%s)\n", ApiRef, hdc, hbmp, uStartScan, cScanLines, lpvBits, uUsage, ExplainDIBUsage(uUsage));
        TraceBITMAPINFOHEADER(ApiRef, (BITMAPINFOHEADER *) & (lpbmi->bmiHeader));
    }
#endif
    if(dxw.IsToRemap(hdc) && !bGDIRecursionFlag) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC: // this will flicker !!!!
            sdc.GetPrimaryDC(hdc);
            ret = (*pGetDIBits)(sdc.GetHdc(), hbmp, uStartScan, cScanLines, lpvBits, lpbmi, uUsage);
            sdc.PutPrimaryDC(hdc, FALSE);
#ifndef DXW_NOTRACES
            if(dxw.dwDFlags & DUMPDIBSECTION) DumpDibSection("GetDIBits", hdc, lpbmi, uUsage, (LPBYTE)lpvBits);
#endif // DXW_NOTRACES
            return ret;
            break;
        case GDIMODE_EMULATED:
        default:
            break;
        }
    }
    ret = (*pGetDIBits)(hdc, hbmp, uStartScan, cScanLines, lpvBits, lpbmi, uUsage);
    _if(!ret || (ret == GDI_ERROR)) {
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
        return ret;
    }
#ifndef DXW_NOTRACES
    if(dxw.dwDFlags & DUMPDIBSECTION) DumpDibSection(ApiRef, hdc, lpbmi, uUsage, (LPBYTE)lpvBits);
    TraceBITMAPINFOHEADER(ApiRef, (BITMAPINFOHEADER *) & (lpbmi->bmiHeader));
#endif // DXW_NOTRACES
    if ((dxw.dwFlags12 & REVERTDIBPALETTE) &&
            (lpbmi->bmiHeader.biBitCount == 32)) {
        if(dxw.VirtualPixelFormat.dwRGBBitCount == 8) {
            if(lpvBits) {
                int w = lpbmi->bmiHeader.biWidth;
                int h = lpbmi->bmiHeader.biHeight;
                if (h < 0) h = -h;
                HPALETTE hpal;
                PALETTEENTRY DCPaletteEntries[256];
                // hellish trick: to get the DC palette change it twice, but the replacement must be successful,
                // so you must use a valid palette handle to be replaced: GetStockObject(DEFAULT_PALETTE) is ok.
                hpal = (*pGDISelectPalette)(hdc, (HPALETTE)GetStockObject(DEFAULT_PALETTE), 0);
                if(hpal) {
                    int nEntries;
                    (*pGDISelectPalette)(hdc, hpal, 0);
                    nEntries = (*pGetPaletteEntries)(hpal, 0, 256, DCPaletteEntries);
                }
                LPDWORD pw = (LPDWORD)lpvBits;
                LPBYTE pb = (LPBYTE)lpvBits;
                int iByteAlign = w - ((w >> 2) << 2);
                for(int y = 0; y < h; y++) {
                    for(int x = 0; x < w; x++)
                        *pb++ = InvertPalette32(*pw++, DCPaletteEntries);
                    pb += iByteAlign;
                }
            }
            lpbmi->bmiHeader.biBitCount = 8;
            lpbmi->bmiHeader.biSizeImage = lpbmi->bmiHeader.biSizeImage / 4;
        }
        if(dxw.VirtualPixelFormat.dwRGBBitCount == 16) {
            if(lpvBits)
                MessageBox(NULL, "TO DO: pixel conversion", "DxWnd", 0);
            lpbmi->bmiHeader.biBitCount = 16;
            lpbmi->bmiHeader.biSizeImage = lpbmi->bmiHeader.biSizeImage / 2;
        }
    }
    return ret;
}

int WINAPI extSetDIBitsToDevice(HDC hdc, int XDest, int YDest, DWORD dwWidth, DWORD dwHeight, int XSrc, int YSrc, UINT uStartScan, UINT cScanLines,
                                const VOID *lpvBits, const BITMAPINFO *lpbmi, UINT fuColorUse) {
    int ret;
    ApiName("SetDIBitsToDevice");
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        OutTrace("%s: hdc=%#x dest=(%d,%d)-(%dx%d) src=(%d,%d) lines=(%d,%d) bits=%#x ColorUse=%#x(%s)\n",
                 ApiRef, hdc, XDest, YDest, dwWidth, dwHeight, XSrc, YSrc, uStartScan, cScanLines, lpvBits, fuColorUse, ExplainDIBUsage(fuColorUse));
        TraceBITMAPINFOHEADER(ApiRef, (BITMAPINFOHEADER *) & (lpbmi->bmiHeader));
    }
    if(dxw.dwDFlags & DUMPDIBSECTION) DumpDibSection(ApiRef, hdc, lpbmi, fuColorUse, (LPBYTE)lpvBits);
#endif // DXW_NOTRACES
    bGDIRecursionFlag = TRUE; // beware: it seems that SetDIBitsToDevice calls SetDIBits internally
    if(dxw.IsToRemap(hdc)) {
        HDC hTempDc;
        HBITMAP hbmPic;
        DWORD OrigWidth, OrigHeight;
        int OrigXDest, OrigYDest;
        HGDIOBJ obj;
        BOOL ret2;
        OrigWidth = dwWidth;
        OrigHeight = dwHeight;
        OrigXDest = XDest;
        OrigYDest = YDest;
        dxw.HandleDIB(); // handle refresh delays
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pSetDIBitsToDevice)(sdc.GetHdc(), XDest, YDest, dwWidth, dwHeight, XSrc, YSrc, uStartScan, cScanLines, lpvBits, lpbmi, fuColorUse);
#ifndef DXW_NOTRACES
            if((dxw.dwDFlags & DUMPDEVCONTEXT) && dxw.bCustomKeyToggle) DumpHDC(sdc.GetHdc(), XDest, YDest, dwWidth, dwHeight);
#endif // DXW_NOTRACES
            sdc.PutPrimaryDC(hdc, TRUE, XDest, YDest, dwWidth, dwHeight);
            bGDIRecursionFlag = FALSE;
            return ret;
            break;
        case GDIMODE_STRETCHED:
            // blitting to primary surface !!!
            dxw.MapClient(&XDest, &YDest, (int *)&dwWidth, (int *)&dwHeight);
            OutTraceSYS("%s: fixed dest=(%d,%d)-(%dx%d)\n", ApiRef, XDest, YDest, dwWidth, dwHeight);
            hTempDc = CreateCompatibleDC(hdc);
            _if(!hTempDc) OutTraceE("%s: CreateCompatibleDC ERROR err=%d\n", ApiRef, GetLastError());
            // tricky part: CreateCompatibleBitmap is needed to set the dc size, but it has to be performed
            // against hdc to set for color depth, then selected (through SelectObject) against the temporary
            // dc to assign the needed size and color space to the temporary dc.
            hbmPic = CreateCompatibleBitmap(hdc, OrigWidth, OrigHeight);
            _if(!hbmPic) OutTraceE("%s: CreateCompatibleBitmap ERROR err=%d\n", ApiRef, GetLastError());
            obj = (*pSelectObject)(hTempDc, hbmPic);
            _if(!obj) OutTraceE("%s: SelectObject ERROR err=%d\n", ApiRef, GetLastError());
            if((lpbmi->bmiHeader.biBitCount == 8) && (dxw.dwFlags2 & PALDIBEMULATION)) {
                VOID *lpvNewBits;
                BITMAPINFO NewBmi;
                memcpy(&NewBmi, lpbmi, sizeof(BITMAPINFO));
                lpvNewBits = dxw.EmulateDIB((LPVOID)lpvBits, &NewBmi, fuColorUse);
#ifndef DXW_NOTRACES
                if(dxw.dwDFlags & DUMPDIBSECTION)
                    DumpDibSection(ApiRef, hdc, &NewBmi, DIB_RGB_COLORS, lpvNewBits);
#endif // DXW_NOTRACES
                ret = (*pSetDIBitsToDevice)(hTempDc, 0, 0, OrigWidth, OrigHeight, XSrc, YSrc, uStartScan, cScanLines, lpvNewBits, &NewBmi, DIB_RGB_COLORS);
                _if(!ret) OutTraceE("%s: SetDIBitsToDevice ERROR err=%d\n", ApiRef, GetLastError());
                free(lpvNewBits);
            } else {
                ret = (*pSetDIBitsToDevice)(hTempDc, 0, 0, OrigWidth, OrigHeight, XSrc, YSrc, uStartScan, cScanLines, lpvBits, lpbmi, fuColorUse);
                _if(!ret) OutTraceE("%s: SetDIBitsToDevice ERROR err=%d\n", ApiRef, GetLastError());
            }
            bGDIRecursionFlag = FALSE;
#ifndef DXW_NOTRACES
            if((dxw.dwDFlags & DUMPDEVCONTEXT) && dxw.bCustomKeyToggle) DumpHDC(hTempDc, 0, 0, OrigWidth, OrigHeight);
#endif // DXW_NOTRACES
            // v2.02.94: set HALFTONE stretching. Fixes "Celtic Kings Rage of War"
            SetStretchBltMode(hdc, HALFTONE);
            ret = (*pGDIStretchBlt)(hdc, XDest, YDest, dwWidth, dwHeight, hTempDc, 0, 0, OrigWidth, OrigHeight, SRCCOPY);
            _if(!ret) OutTraceE("%s: StretchBlt ERROR err=%d\n", ApiRef, GetLastError());
            ret2 = DeleteObject(hbmPic); // v2.02.32 - avoid resource leakage
            _if(!ret2) OutTraceE("%s: DeleteObject ERROR err=%d\n", ApiRef, GetLastError());
            ret2 = DeleteDC(hTempDc);
            _if(!ret2) OutTraceE("%s: DeleteDC ERROR err=%d\n", ApiRef, GetLastError());
            if(dxw.dwDFlags & MARKGDI32) dxw.Mark(hdc, FALSE, RGB(255, 255, 255),  XDest, YDest, dwWidth, dwHeight);
            return ret;
            break;
        case GDIMODE_EMULATED:
            int X, Y;
            X = XDest + dxw.VirtualOffsetX;
            Y = YDest + dxw.VirtualOffsetY;
            OutTraceSYS("%s: virtual pos=(%d,%d)+(%d+%d)=(%d,%d)\n",
                        ApiRef, XDest, YDest, dxw.VirtualOffsetX, dxw.VirtualOffsetY, X, Y);
            ret = (*pSetDIBitsToDevice)(hdc, X, Y, dwWidth, dwHeight, XSrc, YSrc, uStartScan, cScanLines, lpvBits, lpbmi, fuColorUse);
            bGDIRecursionFlag = FALSE;
#ifndef DXW_NOTRACES
            if((dxw.dwDFlags & DUMPDEVCONTEXT) && dxw.bCustomKeyToggle) DumpHDC(hdc, X, Y, dwWidth, dwHeight);
#endif // DXW_NOTRACES
            _if(!ret || (ret == GDI_ERROR)) OutTraceE("%s: ERROR ret=%#x err=%d\n", ApiRef, ret, GetLastError());
            if(dxw.dwDFlags & MARKGDI32) dxw.Mark(hdc, FALSE, RGB(255, 255, 255),  XDest, YDest, dwWidth, dwHeight);
            return ret;
        default:
#ifndef DXW_NOTRACES
            if((dxw.dwDFlags & DUMPDEVCONTEXT) && dxw.bCustomKeyToggle) DumpHDC(hdc, XDest, YDest, dwWidth, dwHeight);
#endif // DXW_NOTRACES
            break;
        }
    }
    ret = (*pSetDIBitsToDevice)(hdc, XDest, YDest, dwWidth, dwHeight, XSrc, YSrc, uStartScan, cScanLines, lpvBits, lpbmi, fuColorUse);
    bGDIRecursionFlag = FALSE;
    _if(!ret || (ret == GDI_ERROR)) OutTraceE("%s: ERROR ret=%#x err=%d\n", ApiRef, ret, GetLastError());
    if(dxw.dwDFlags & MARKGDI32) dxw.Mark(hdc, FALSE, RGB(255, 255, 255),  XDest, YDest, dwWidth, dwHeight);
    return ret;
}

UINT WINAPI extGetDIBColorTable(HDC hdc, UINT uStartIndex, UINT cEntries, RGBQUAD *pColors) {
    UINT ret;
    ApiName("GetDIBColorTable");
    OutTraceSYS("%s: hdc=%#x start=%d entries=%d\n", ApiRef, hdc, uStartIndex, cEntries);
    if(dxw.dwFlags2 & PALDIBEMULATION)
        ret = dxw.GetDIBColors(hdc, uStartIndex, cEntries, pColors);
    else
        ret = (*pGetDIBColorTable)(hdc, uStartIndex, cEntries, pColors);
    OutTraceSYS("%s: ret=%d\n", ApiRef, ret);
    if(IsDebugDW) dxw.DumpPalette(cEntries, (PALETTEENTRY *)pColors);
    return ret;
}

UINT WINAPI extSetDIBColorTable(HDC hdc, UINT uStartIndex, UINT cEntries, const RGBQUAD *pColors) {
    UINT ret;
    ApiName("SetDIBColorTable");
    OutTraceSYS("%s: hdc=%#x start=%d entries=%d\n", ApiRef, hdc, uStartIndex, cEntries);
    if(IsDebugDW) dxw.DumpPalette(cEntries, (PALETTEENTRY *)pColors);
    if(dxw.dwFlags2 & PALDIBEMULATION)
        ret = dxw.SetDIBColors(hdc, uStartIndex, cEntries, pColors);
    else
        ret = (*pSetDIBColorTable)(hdc, uStartIndex, cEntries, pColors);
    OutTraceSYS("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

HBITMAP WINAPI extCreateCompatibleBitmap(HDC hdc, int nWidth, int nHeight) {
    HBITMAP ret;
    ApiName("CreateCompatibleBitmap");
    OutTraceSYS("%s: hdc=%#x size=(%d,%d)\n", ApiRef, hdc, nWidth, nHeight);
    if (dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pCreateCompatibleBitmap)(sdc.GetHdc(), nWidth, nHeight);
            sdc.PutPrimaryDC(hdc, FALSE);
#ifndef DXW_NOTRACES
            if(ret)
                OutTraceSYS("%s: hbitmap=%#x\n", ApiRef, ret);
            else
                OutTraceE("%s: ERROR ret=%#x err=%d\n", ApiRef, ret, GetLastError());
#endif // DXW_NOTRACES
            return ret;
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&nWidth, &nHeight);
            OutTraceSYS("%s: fixed size=(%d,%d)\n", ApiRef, nWidth, nHeight);
            break;
        default:
            break;
        }
    }
    ret = (*pCreateCompatibleBitmap)(hdc, nWidth, nHeight);
#ifndef DXW_NOTRACES
    if(ret)
        OutTraceSYS("%s: hbitmap=%#x\n", ApiRef, ret);
    else
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
#endif // DXW_NOTRACES
    return ret;
}

COLORREF WINAPI extSetPixel(HDC hdc, int X, int Y, COLORREF crColor) {
    COLORREF ret;
    ApiName("SetPixel");
    OutTraceSYS("%s: hdc=%#x color=%#x point=(%d,%d)\n", ApiRef, hdc, crColor, X, Y);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pSetPixel)(sdc.GetHdc(), X, Y, crColor);
            sdc.PutPrimaryDC(hdc, TRUE, X, Y, 1, 1); // ????
            return ret; // this returns a COLORREF type
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&X, &Y);
            OutTraceSYS("%s: fixed pos=(%d,%d)\n", ApiRef, X, Y);
            break;
        }
    }
    ret = (*pSetPixel)(hdc, X, Y, crColor);
    // both 0x00000000 and 0xFFFFFFFF are legitimate colors and therefore valid return codes...
    //if(ret==GDI_ERROR) OutTraceE("SetPixel: ERROR ret=%#x err=%d\n", ret, GetLastError());
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

BOOL WINAPI extSetPixelV(HDC hdc, int X, int Y, COLORREF crColor) {
    BOOL ret;
    ApiName("SetPixelV");
    OutTraceSYS("%s: hdc=%#x color=%#x point=(%d,%d)\n", ApiRef, hdc, crColor, X, Y);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pSetPixelV)(sdc.GetHdc(), X, Y, crColor);
            sdc.PutPrimaryDC(hdc, TRUE, X, Y, 1, 1); // ????
            return ret;
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&X, &Y);
            OutTraceSYS("%s: fixed pos=(%d,%d)\n", ApiRef, X, Y);
            break;
        }
    }
    ret = (*pSetPixelV)(hdc, X, Y, crColor);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

BOOL WINAPI extEllipse(HDC hdc, int nLeftRect, int nTopRect, int nRightRect, int nBottomRect) {
    int ret;
    ApiName("Ellipse");
    OutTraceSYS("%s: hdc=%#x rect=(%d,%d)-(%d,%d)\n", ApiRef, hdc, nLeftRect, nTopRect, nRightRect, nBottomRect);
    if (dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pEllipse)(sdc.GetHdc(), nLeftRect, nTopRect, nRightRect, nBottomRect);
            sdc.PutPrimaryDC(hdc, TRUE, nLeftRect, nTopRect, nRightRect - nLeftRect, nBottomRect - nTopRect);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&nLeftRect, &nTopRect, &nRightRect, &nBottomRect);
            OutTraceSYS("%s: fixed dest=(%d,%d)-(%d,%d)\n", ApiRef, nLeftRect, nTopRect, nRightRect, nBottomRect);
            break;
        }
    }
    ret = (*pEllipse)(hdc, nLeftRect, nTopRect, nRightRect, nBottomRect);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extPolygon(HDC hdc, const POINT *lpPoints, int cCount) {
    BOOL ret;
    ApiName("Polygon");
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        int i;
        OutTrace("%s: hdc=%#x cCount=%d\n", ApiRef, hdc, cCount);
        if(IsDebugSYS) {
            for(i = 0; i < cCount; i++) OutTrace("> pt[%d]=(%d,%d)\n", i, lpPoints[i].x, lpPoints[i].y);
        }
    }
#endif
    if (dxw.IsToRemap(hdc)) {
        int i, size;
        POINT *lpRemPoints;
        size = cCount * sizeof(POINT);
        lpRemPoints = (LPPOINT)malloc(size);
        memcpy(lpRemPoints, lpPoints, size);
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            free(lpRemPoints);
            sdc.GetPrimaryDC(hdc);
            ret = (*pPolygon)(sdc.GetHdc(), lpPoints, cCount);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            for(i = 0; i < cCount; i++)
                dxw.MapClient(&lpRemPoints[i]);
#ifndef DXW_NOTRACES
            if(IsTraceSYS) {
                OutTrace("%s: fixed cCount=%d\n", ApiRef, cCount);
                if(IsDebugSYS) {
                    for(i = 0; i < cCount; i++) OutTrace("> pt[%d]=(%d,%d)\n", i, lpPoints[i].x, lpPoints[i].y);
                }
            }
#endif
            break;
        case GDIMODE_EMULATED:
            if (dxw.IsVirtual(hdc)) {
                for(i = 0; i < cCount; i++) {
                    lpRemPoints[i].x += dxw.VirtualOffsetX;
                    lpRemPoints[i].y += dxw.VirtualOffsetY;
                }
#ifndef DXW_NOTRACES
                if(IsTraceSYS) {
                    OutTrace("%s: fixed cCount=%d\n", ApiRef, cCount);
                    if(IsDebugSYS) {
                        for(i = 0; i < cCount; i++) OutTrace("> pt[%d]=(%d,%d)\n", i, lpPoints[i].x, lpPoints[i].y);
                    }
                }
#endif
            }
            break;
        }
        ret = (*pPolygon)(hdc, lpRemPoints, cCount);
        free(lpRemPoints);
    } else
        ret = (*pPolygon)(hdc, lpPoints, cCount);
    _if(!ret)OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extArc(HDC hdc, int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, int nXStartArc, int nYStartArc, int nXEndArc, int nYEndArc) {
    int ret;
    ApiName("Arc");
    OutTraceSYS("%s: hdc=%#x rect=(%d,%d)-(%d,%d) start=(%d,%d) end=(%d,%d)\n",
                ApiRef, hdc, nLeftRect, nTopRect, nRightRect, nBottomRect, nXStartArc, nYStartArc, nXEndArc, nYEndArc);
    if (dxw.IsToRemap(hdc)) {
        dxw.MapClient(&nLeftRect, &nTopRect, &nRightRect, &nBottomRect);
        dxw.MapClient(&nXStartArc, &nYStartArc, &nXEndArc, &nYEndArc);
        OutTraceSYS("%s: fixed rect=(%d,%d)-(%d,%d) start=(%d,%d) end=(%d,%d)\n",
                    ApiRef, nLeftRect, nTopRect, nRightRect, nBottomRect, nXStartArc, nYStartArc, nXEndArc, nYEndArc);
    }
    ret = (*pArc)(hdc, nLeftRect, nTopRect, nRightRect, nBottomRect, nXStartArc, nYStartArc, nXEndArc, nYEndArc);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extMaskBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc,
                       int nXSrc, int nYSrc, HBITMAP hbmMask, int xMask, int yMask, DWORD dwRop) {
    // found in "Spearhead"
    BOOL ret;
    ApiName("MaskBlt");
#ifndef DXW_NOTRACES
    OutTraceSYS("%s: hdcDest=%#x pos=(%d,%d) size=(%dx%d) hdcSrc=%#x pos=(%d,%d) hbmMask=%#x Mask=(%d,%d) dwRop=%#x\n",
                ApiRef, hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, hbmMask, xMask, yMask, dwRop);
#endif
    if (dxw.IsToRemap(hdcDest)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdcDest);
            ret = (*pMaskBlt)(sdc.GetHdc(), nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, hbmMask, xMask, yMask, dwRop);
            sdc.PutPrimaryDC(hdcDest, TRUE, nXDest, nYDest, nWidth, nHeight);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&nXDest, &nYDest, &nWidth, &nHeight);
            OutTraceSYS("%s: fixed pos=(%d,%d) size=(%dx%d)\n", ApiRef, nXDest, nYDest, nWidth, nHeight);
            break;
        case GDIMODE_EMULATED:
            // to be implemented
            break;
        default:
            break;
        }
    }
    ret = (*pMaskBlt)(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, hbmMask, xMask, yMask, dwRop);
    _if(!ret) OutTraceE("%s: ERROR rerr=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extSetViewportOrgEx(HDC hdc, int X, int Y, LPPOINT lpPoint) {
    BOOL ret;
    ApiName("SetViewportOrgEx");
    OutTraceSYS("%s: hdc=%#x pos=(%d,%d)\n", ApiRef, hdc, X, Y);
    if(dxw.IsToRemap(hdc)) {
        dxw.VirtualOffsetX = X;
        dxw.VirtualOffsetY = Y;
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_EMULATED:
            if(dxw.IsVirtual(hdc)) {
                OutTraceSYS("%s: virtual hdc\n", ApiRef);
                if(lpPoint) {
                    lpPoint->x = dxw.VirtualOffsetX;
                    lpPoint->y = dxw.VirtualOffsetY;
                }
                ret = TRUE;
            } else {
                ret = (*pSetViewportOrgEx)(hdc, X, Y, lpPoint);
                if(ret && lpPoint) {
                    OutTraceSYS("%s: previous ViewPort=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
                    dxw.UnmapClient(lpPoint);
                    OutTraceSYS("%s: fixed previous ViewPort=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
                }
            }
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&X, &Y);
            OutTraceSYS("%s: fixed pos=(%d,%d)\n", ApiRef, X, Y);
            ret = (*pSetViewportOrgEx)(hdc, X, Y, lpPoint);
            if(ret && lpPoint) {
                OutTraceSYS("%s: previous ViewPort=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
                dxw.UnmapClient(lpPoint);
                OutTraceSYS("%s: fixed previous ViewPort=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
            }
            break;
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pSetViewportOrgEx)(sdc.GetHdc(), X, Y, lpPoint);
            sdc.PutPrimaryDC(hdc, FALSE);
            break;
        default:
            ret = (*pSetViewportOrgEx)(hdc, X, Y, lpPoint);
            break;
        }
    } else
        ret = (*pSetViewportOrgEx)(hdc, X, Y, lpPoint);
    _if(ret && lpPoint) OutTraceSYS("%s: previous ViewPort=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extSetViewportExtEx(HDC hdc, int X, int Y, LPSIZE lpSize) {
    BOOL ret;
    ApiName("SetViewportExtEx");
    OutTraceSYS("%s: hdc=%#x pos=(%d,%d)\n", ApiRef, hdc, X, Y);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_EMULATED:
            if(dxw.IsVirtual(hdc)) {
                OutTraceSYS("%s: virtual hdc\n", ApiRef);
                if(lpSize) {
                    lpSize->cx = dxw.VirtualExtentX;
                    lpSize->cy = dxw.VirtualExtentY;
                }
                dxw.VirtualExtentX = X;
                dxw.VirtualExtentY = Y;
                ret = TRUE;
            } else
                ret = (*pSetViewportExtEx)(hdc, X, Y, lpSize);
            break;
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pSetViewportExtEx)(sdc.GetHdc(), X, Y, lpSize);
            sdc.PutPrimaryDC(hdc, FALSE);
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&X, &Y);
            OutTraceSYS("%s: fixed pos=(%d,%d)\n", ApiRef, X, Y);
            ret = (*pSetViewportExtEx)(hdc, X, Y, lpSize);
            if(ret && lpSize) {
                OutTraceSYS("%s: previous ViewPort=(%d,%d)\n", ApiRef, lpSize->cx, lpSize->cy);
                dxw.UnmapClient((LPPOINT)lpSize);
                OutTraceSYS("%s: fixed previous ViewPort=(%d,%d)\n", ApiRef, lpSize->cx, lpSize->cy);
            }
            break;
        default:
            ret = (*pSetViewportExtEx)(hdc, X, Y, lpSize);
            break;
        }
    } else
        ret = (*pSetViewportExtEx)(hdc, X, Y, lpSize);
    _if(ret && lpSize) OutTraceSYS("%s: previous ViewPort=(%d,%d)\n", ApiRef, lpSize->cx, lpSize->cy);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extGetViewportOrgEx(HDC hdc, LPPOINT lpPoint) {
    BOOL ret;
    ApiName("GetViewportOrgEx");
    OutTraceSYS("%s: hdc=%#x\n", ApiRef, hdc);
    if(dxw.IsVirtual(hdc)) {
        lpPoint->x = dxw.VirtualOffsetX;
        lpPoint->y = dxw.VirtualOffsetY;
        return TRUE;
    }
    ret = (*pGetViewportOrgEx)(hdc, lpPoint);
    if(ret) {
        OutTraceSYS("%s: origin=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
        if (dxw.IsToRemap(hdc)) {
            dxw.UnmapClient(lpPoint);
            OutTraceSYS("%s: fixed origin=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
        }
    }
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extGetViewportExtEx(HDC hdc, LPPOINT lpPoint) {
    BOOL ret;
    ApiName("GetViewportExtEx");
    OutTraceSYS("%s: hdc=%#x\n", ApiRef, hdc);
    if(dxw.IsVirtual(hdc)) {
        lpPoint->x = dxw.VirtualOffsetX;
        lpPoint->y = dxw.VirtualOffsetY;
        return TRUE;
    }
    ret = (*pGetViewportExtEx)(hdc, lpPoint);
    if(ret) {
        OutTraceSYS("%s: extent=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
        if (dxw.IsToRemap(hdc)) {
            dxw.UnmapClient(lpPoint);
            OutTraceSYS("%s: fixed extent=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
        }
    }
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extGetWindowOrgEx(HDC hdc, LPPOINT lpPoint) {
    BOOL ret;
    ApiName("GetWindowOrgEx");
    OutTraceSYS("%s: hdc=%#x\n", ApiRef, hdc);
    ret = (*pGetWindowOrgEx)(hdc, lpPoint);
    if(ret) {
        OutTraceSYS("%s: origin=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
        if (dxw.IsToRemap(hdc)) {
            dxw.UnmapClient(lpPoint);
            OutTraceSYS("%s: fixed origin=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
        }
    }
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extSetWindowOrgEx(HDC hdc, int X, int Y, LPPOINT lpPoint) {
    BOOL ret;
    ApiName("SetWindowOrgEx");
    OutTraceSYS("%s: hdc=%#x origin=(%d,%d)\n", ApiRef, hdc, X, Y);
    if (dxw.IsToRemap(hdc)) {
        dxw.MapClient(&X, &Y);
        OutTraceSYS("%s: fixed origin=(%d,%d)\n", ApiRef, X, Y);
    }
    ret = (*pSetWindowOrgEx)(hdc, X, Y, lpPoint);
    if(ret && lpPoint) {
        OutTraceSYS("%s: previous origin=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
        if (dxw.IsToRemap(hdc)) {
            dxw.UnmapClient(lpPoint);
            OutTraceSYS("%s: fixed previous origin=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
        }
    }
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extSetWindowExtEx(HDC hdc, int X, int Y, LPPOINT lpPoint) {
    BOOL ret;
    ApiName("SetWindowExtEx");
    OutTraceSYS("%s: hdc=%#x extent=(%d,%d)\n", ApiRef, hdc, X, Y);
    if (dxw.IsToRemap(hdc)) {
        dxw.MapClient(&X, &Y);
        OutTraceSYS("%s: fixed extent=(%d,%d)\n", ApiRef, X, Y);
    }
    ret = (*pSetWindowExtEx)(hdc, X, Y, lpPoint);
    if(ret && lpPoint) {
        OutTraceSYS("%s: previous extent=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
        if (dxw.IsToRemap(hdc)) {
            dxw.UnmapClient(lpPoint);
            OutTraceSYS("%s: fixed previous extent=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
        }
    }
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extGetWindowExtEx(HDC hdc, LPSIZE lpsize) {
    BOOL ret;
    ApiName("GetWindowExtEx");
    //if(hdc == NULL){
    //	lpsize->cx = dxw.GetScreenWidth();
    //	lpsize->cy = dxw.GetScreenHeight();
    //	OutTrace("%s: hdc=%#x returning fake size=(%d x %d)\n",
    //		ApiRef, hdc, lpsize->cx, lpsize->cy);
    //	return TRUE;
    //}
    ret = (*pGetWindowExtEx)(hdc, lpsize);
    if(ret) {
        OutTrace("%s: hdc=%#x extent=(%d,%d)\n",
                 ApiRef, hdc, lpsize->cx, lpsize->cy);
    } else {
        OutTraceE("%s: hdc=#x ERROR err=%d\n",
                  ApiRef, hdc, GetLastError());
    }
    return ret;
}

BOOL WINAPI extGetCurrentPositionEx(HDC hdc, LPPOINT lpPoint) {
    BOOL ret;
    ApiName("GetCurrentPositionEx");
    OutTraceSYS("%s: hdc=%#x\n", ApiRef, hdc);
    ret = (*pGetCurrentPositionEx)(hdc, lpPoint);
    if(ret) {
        OutTraceSYS("%s: pos=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
        if (dxw.IsToRemap(hdc)) {
            switch(dxw.GDIEmulationMode) {
            case GDIMODE_STRETCHED:
                dxw.UnmapClient(lpPoint);
                break;
            case GDIMODE_SHAREDDC:
                sdc.GetPrimaryDC(hdc);
                ret = (*pGetCurrentPositionEx)(sdc.GetHdc(), lpPoint);
                sdc.PutPrimaryDC(hdc, FALSE);
                return ret;
                break;
            case GDIMODE_EMULATED:
            default:
                break;
            }
            OutTraceSYS("%s: fixed pos=(%d,%d)\n", ApiRef, lpPoint->x, lpPoint->y);
        }
    }
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extCreateScalableFontResourceA(DWORD fdwHidden, LPCTSTR lpszFontRes, LPCTSTR lpszFontFile, LPCTSTR lpszCurrentPath) {
    BOOL res;
    ApiName("CreateScalableFontResourceA");
    OutTraceSYS("%s: hidden=%d FontRes=\"%s\" FontFile=\"%s\" CurrentPath=\"%s\"\n",
                ApiRef, fdwHidden, lpszFontRes, lpszFontFile, lpszCurrentPath);
    if(dxw.dwFlags3 & FONTBYPASS) return TRUE;
    res = (*pCreateScalableFontResourceA)(fdwHidden, lpszFontRes, lpszFontFile, lpszCurrentPath);
    _if(!res) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return res;
}

BOOL WINAPI extCreateScalableFontResourceW(DWORD fdwHidden, LPCWSTR lpszFontRes, LPCWSTR lpszFontFile, LPCWSTR lpszCurrentPath) {
    BOOL res;
    ApiName("CreateScalableFontResourceW");
    OutTraceSYS("%s: hidden=%d FontRes=\"%ls\" FontFile=\"%ls\" CurrentPath=\"%ls\"\n",
                ApiRef, fdwHidden, lpszFontRes, lpszFontFile, lpszCurrentPath);
    if(dxw.dwFlags3 & FONTBYPASS) return TRUE;
    res = (*pCreateScalableFontResourceW)(fdwHidden, lpszFontRes, lpszFontFile, lpszCurrentPath);
    _if(!res) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return res;
}

int WINAPI extAddFontResourceA(LPCTSTR lpszFontFile) {
    BOOL res;
    ApiName("AddFontResourceA");
    OutTraceSYS("%s: FontFile=\"%s\"\n", ApiRef, lpszFontFile);
    if(dxw.dwFlags3 & FONTBYPASS) {
        OutTraceDW("%s: SUPPRESS FontFile=\"%s\"\n", ApiRef, lpszFontFile);
        return TRUE;
    }
    res = (*pAddFontResourceA)(lpszFontFile);
    _if(!res) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return res;
}

int WINAPI extRemoveFontResourceA(LPCTSTR lpszFontFile) {
    BOOL res;
    ApiName("RemoveFontResourceA");
    OutTraceSYS("RemoveFontResource: FontFile=\"%s\"\n", lpszFontFile);
    if(dxw.dwFlags3 & FONTBYPASS) {
        OutTraceDW("%s: SUPPRESS FontFile=\"%s\"\n", ApiRef, lpszFontFile);
        return TRUE;
    }
    res = (*pRemoveFontResourceA)(lpszFontFile);
    _if(!res) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return res;
}

int WINAPI extAddFontResourceW(LPCWSTR lpszFontFile) {
    BOOL res;
    ApiName("AddFontResourceW");
    OutTraceSYS("%s: FontFile=\"%ls\"\n", ApiRef, lpszFontFile);
    if(dxw.dwFlags3 & FONTBYPASS) {
        OutTraceDW("%s: SUPPRESS FontFile=\"%ls\"\n", ApiRef, lpszFontFile);
        return TRUE;
    }
    res = (*pAddFontResourceW)(lpszFontFile);
    _if(!res) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return res;
}

int WINAPI extRemoveFontResourceW(LPCWSTR lpszFontFile) {
    BOOL res;
    ApiName("RemoveFontResourceW");
    OutTraceSYS("%s: FontFile=\"%ls\"\n", ApiRef, lpszFontFile);
    if(dxw.dwFlags3 & FONTBYPASS) {
        OutTraceDW("%s: SUPPRESS FontFile=\"%ls\"\n", ApiRef, lpszFontFile);
        return TRUE;
    }
    res = (*pRemoveFontResourceW)(lpszFontFile);
    _if(!res) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return res;
}

#ifdef TRACEFONTS
int WINAPI extEnumFontsA(HDC hdc, LPCSTR lpFaceName, FONTENUMPROC lpFontFunc, LPARAM lParam) {
    int res;
    ApiName("EnumFonts");
    OutTraceSYS("%s: hdc=%#x facename=\"%s\" fontfunc=%#x lparam=%#x\n", ApiRef, hdc, lpFaceName, lpFontFunc, lParam);
    res = (*pEnumFontsA)(hdc, lpFaceName, lpFontFunc, lParam);
    OutTraceSYS("%s: res=%#x\n", ApiRef, res);
    return res;
}
#endif

//BEWARE: SetPixelFormat must be issued on the same hdc used by OpenGL wglCreateContext, otherwise
// a failure err=2000 ERROR INVALID PIXEL FORMAT occurs!!
//
//Remarks: https://msdn.microsoft.com/en-us/library/ms537559(VS.85).aspx
//
// If hdc references a window, calling the SetPixelFormat function also changes the pixel format of the window.
// Setting the pixel format of a window more than once can lead to significant complications for the Window
// Manager and for multithread applications, so it is not allowed. An application can only set the pixel format
// of a window one time. Once a window's pixel format is set, it cannot be changed.

BOOL WINAPI extGDISetPixelFormat(HDC hdc, int iPixelFormat, const PIXELFORMATDESCRIPTOR *ppfd) {
    BOOL res;
    BOOL bRemappedDC = FALSE;
    static int iCounter = 0;
    static HDC hLastDC = 0;
    // v2.04.87: ppfd can be NULL (in "Neverwinter Nights Platinum")
    if(IsTraceSYS) {
        if(ppfd) {
            OutTrace("SetPixelFormat: hdc=%#x PixelFormat=%d ppfd={Flags=%#x PixelType=%#x(%s) ColorBits=%d RGBdepth=(%d,%d,%d) RGBshift=(%d,%d,%d)}\n",
                     hdc, iPixelFormat,
                     ppfd->dwFlags, ppfd->iPixelType, ppfd->iPixelType ? "PFD_TYPE_COLORINDEX" : "PFD_TYPE_RGBA", ppfd->cColorBits,
                     ppfd->cRedBits, ppfd->cGreenBits, ppfd->cBlueBits,
                     ppfd->cRedShift, ppfd->cGreenShift, ppfd->cBlueShift);
        } else {
            OutTrace("SetPixelFormat: hdc=%#x PixelFormat=%d ppfd=(NULL)\n",
                     hdc, iPixelFormat);
        }
    }
    if(dxw.IsEmulated && (dxw.dwFlags12 & NOSETPIXELFORMAT)) {
        OutTrace("SetPixelFormat: BYPASS\n");
        return TRUE;
    }
    //if(dxw.IsEmulated) {
    if(dxw.IsRealDesktop(WindowFromDC(hdc))) {
        HDC oldhdc = hdc;
        hdc = (*pGDIGetDC)(dxw.GethWnd()); // potential leakage
        bRemappedDC = TRUE;
        OutTraceSYS("SetPixelFormat: remapped desktop hdc=%#x->%#x hWnd=%#x\n", oldhdc, hdc, dxw.GethWnd());
    }
    if(iCounter && (hdc == hLastDC))
        res = TRUE; // avoid calling SetPixelFormat more than once on same hdc
    else {
        res = (*pGDISetPixelFormat)(hdc, iPixelFormat, ppfd);
        iCounter++;
        hLastDC = hdc;
    }
    if(bRemappedDC) (*pGDIReleaseDC)(dxw.GethWnd(), hdc); // fixed DC leakage
    if (ppfd) dxw.ActualPixelFormat.dwRGBBitCount = ppfd->cColorBits;
    _if(!res) OutTraceE("SetPixelFormat: ERROR err=%d at=%d\n", GetLastError(), __LINE__);
    return res;
}

int WINAPI extGDIGetPixelFormat(HDC hdc) {
    int res;
    BOOL bRemappedDC = FALSE;
    OutTraceSYS("GetPixelFormat: hdc=%#x\n", hdc);
    if(dxw.IsDesktop(WindowFromDC(hdc))) {
        HDC oldhdc = hdc;
        hdc = (*pGDIGetDC)(dxw.GethWnd()); // potential DC leakage
        bRemappedDC = TRUE;
        OutTraceSYS("GetPixelFormat: remapped desktop hdc=%#x->%#x hWnd=%#x\n", oldhdc, hdc, dxw.GethWnd());
    }
    res = (*pGDIGetPixelFormat)(hdc);
#ifndef DXW_NOTRACES
    if(!res) OutTraceE("GetPixelFormat: ERROR err=%d at=%d\n", GetLastError(), __LINE__);
    else OutTraceSYS("GetPixelFormat: res=%d\n", res);
#endif // DXW_NOTRACES
    if(bRemappedDC)(*pGDIReleaseDC)(dxw.GethWnd(), hdc); // fixed DC leakage
    return res;
}

static char *ExplainPFFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "PFD_");
    if (c & PFD_DOUBLEBUFFER) strcat(eb, "DOUBLEBUFFER+");
    if (c & PFD_STEREO) strcat(eb, "STEREO+");
    if (c & PFD_DRAW_TO_WINDOW) strcat(eb, "DRAW_TO_WINDOW+");
    if (c & PFD_DRAW_TO_BITMAP) strcat(eb, "DRAW_TO_BITMAP+");
    if (c & PFD_SUPPORT_GDI) strcat(eb, "SUPPORT_GDI+");
    if (c & PFD_SUPPORT_OPENGL) strcat(eb, "SUPPORT_OPENGL+");
    if (c & PFD_GENERIC_FORMAT) strcat(eb, "GENERIC_FORMAT+");
    if (c & PFD_NEED_PALETTE) strcat(eb, "NEED_PALETTE+");
    if (c & PFD_NEED_SYSTEM_PALETTE) strcat(eb, "NEED_SYSTEM_PALETTE+");
    if (c & PFD_SWAP_EXCHANGE) strcat(eb, "SWAP_EXCHANGE+");
    if (c & PFD_SWAP_COPY) strcat(eb, "SWAP_COPY+");
    if (c & PFD_SWAP_LAYER_BUFFERS) strcat(eb, "SWAP_LAYER_BUFFERS+");
    if (c & PFD_GENERIC_ACCELERATED) strcat(eb, "GENERIC_ACCELERATED+");
    if (c & PFD_SUPPORT_DIRECTDRAW) strcat(eb, "SUPPORT_DIRECTDRAW+");
    if (c & PFD_DIRECT3D_ACCELERATED) strcat(eb, "DIRECT3D_ACCELERATED+");
    if (c & PFD_SUPPORT_COMPOSITION) strcat(eb, "SUPPORT_COMPOSITION+");
    if (c & PFD_DEPTH_DONTCARE) strcat(eb, "DEPTH_DONTCARE+");
    if (c & PFD_DOUBLEBUFFER_DONTCARE) strcat(eb, "DOUBLEBUFFER_DONTCARE+");
    if (c & PFD_STEREO_DONTCARE) strcat(eb, "STEREO_DONTCARE+");
    l = strlen(eb);
    if (l > strlen("PFD_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

int WINAPI extChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR *ppfd) {
    int res;
    OutTraceSYS("ChoosePixelFormat: hdc=%#x Flags=%#x(%s) PixelType=%#x(%s) ColorBits=%d RGBdepth=(%d,%d,%d) RGBshift=(%d,%d,%d)\n",
                hdc,
                ppfd->dwFlags, ExplainPFFlags(ppfd->dwFlags),
                ppfd->iPixelType, ppfd->iPixelType ? "PFD_TYPE_COLORINDEX" : "PFD_TYPE_RGBA", ppfd->cColorBits,
                ppfd->cRedBits, ppfd->cGreenBits, ppfd->cBlueBits,
                ppfd->cRedShift, ppfd->cGreenShift, ppfd->cBlueShift);
    if(0) { // if necessary, add a flag here .....
        PIXELFORMATDESCRIPTOR myppfd;
        memcpy(&myppfd, ppfd, sizeof(PIXELFORMATDESCRIPTOR));
        myppfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        myppfd.dwFlags |= PFD_DRAW_TO_WINDOW;
        //myppfd.dwFlags |= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
        res = (*pChoosePixelFormat)(hdc, &myppfd);
    } else
        res = (*pChoosePixelFormat)(hdc, ppfd);
#ifndef DXW_NOTRACES
    if(!res) OutTraceE("ChoosePixelFormat: ERROR err=%d at=%d\n", GetLastError(), __LINE__);
    else OutTraceSYS("ChoosePixelFormat: res=%d\n", res);
#endif // DXW_NOTRACES
    return res;
}

int WINAPI extDescribePixelFormat(HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd) {
    int res;
    OutTraceSYS("DescribePixelFormat: hdc=%#x PixelFormat=%d Bytes=%d\n", hdc, iPixelFormat, nBytes);
    res = (*pDescribePixelFormat)(hdc, iPixelFormat, nBytes, ppfd);
    if(!res) {
        OutTraceE("DescribePixelFormat: ERROR err=%d at=%d\n", GetLastError(), __LINE__);
        return res;
    }
    if (ppfd && nBytes == sizeof(PIXELFORMATDESCRIPTOR)) {
        OutTraceSYS("DescribePixelFormat: res=%d Flags=%#x(%s) PixelType=%#x(%s) ColorBits=%d RGBdepth=(%d,%d,%d) RGBshift=(%d,%d,%d)\n",
                    res,
                    ppfd->dwFlags, ExplainPFFlags(ppfd->dwFlags), ppfd->iPixelType,
                    ppfd->iPixelType ? "PFD_TYPE_COLORINDEX" : "PFD_TYPE_RGBA", ppfd->cColorBits,
                    ppfd->cRedBits, ppfd->cGreenBits, ppfd->cBlueBits,
                    ppfd->cRedShift, ppfd->cGreenShift, ppfd->cBlueShift);
        if((hdc == 0) && dxw.IsFullScreen() && (ppfd->iPixelType == PFD_TYPE_RGBA)) {
            OutTraceSYS("DescribePixelFormat: emulating virtual desktop pixelformat bpp=%d\n", dxw.VirtualPixelFormat.dwRGBBitCount);
            switch(dxw.VirtualPixelFormat.dwRGBBitCount) {
            case 8:
                ppfd->cColorBits = 8;
                OutTrace("colorbits=%d to be fixed!\n", ppfd->cColorBits);
                break;
            case 16:
                ppfd->cColorBits = 16;
                switch(dxw.VirtualPixelFormat.dwGBitMask) {
                case 0x0007E0: // RGB565
                    ppfd->cColorBits = 16;
                    ppfd->cRedBits = 5;
                    ppfd->cRedShift = 0;
                    ppfd->cGreenBits = 6;
                    ppfd->cGreenShift = 5;
                    ppfd->cBlueBits = 5;
                    ppfd->cBlueShift = 11;
                    ppfd->cAlphaBits = 0;
                    ppfd->cAlphaShift = 0;
                    break;
                case 0x0003E0: // RGB555
                    ppfd->cColorBits = 15;
                    ppfd->cRedBits = 5;
                    ppfd->cRedShift = 0;
                    ppfd->cGreenBits = 5;
                    ppfd->cGreenShift = 5;
                    ppfd->cBlueBits = 5;
                    ppfd->cBlueShift = 10;
                    ppfd->cAlphaBits = 1;
                    ppfd->cAlphaShift = 15;
                    break;
                }
                break;
            case 24:
                _if (ppfd->cColorBits != 24) OutTrace("colorbits=%d to be fixed!\n", ppfd->cColorBits);
                break;
            case 32:
                _if (ppfd->cColorBits != 24) OutTrace("colorbits=%d to be fixed!\n", ppfd->cColorBits);
                break;
            }
        }
    } else
        OutTraceSYS("DescribePixelFormat: res=%d\n", res);
    return res;
}

DWORD WINAPI extGetObjectType(HGDIOBJ h) {
    DWORD res;
    res = (*pGetObjectType)(h);
    OutTraceSYS("GetObjectType: h=%#x type=%#x\n", h, res);
    if(h == dxw.VirtualHDC) {
        OutTraceSYS("GetObjectType: REMAP h=%#x type=%#x->%#x\n", h, res, OBJ_DC);
        res = OBJ_DC;
    }
    return res;
}

extern BOOL gFixed;

BOOL WINAPI extExtTextOutA(HDC hdc, int X, int Y, UINT fuOptions, const RECT *lprc, LPCSTR lpString, UINT cbCount, const INT *lpDx) {
    RECT rc;
    BOOL ret;
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        char sRect[81];
        if(lprc) sprintf(sRect, "(%d,%d)-(%d,%d)", lprc->left, lprc->top, lprc->right, lprc->bottom);
        else strcpy(sRect, "NULL");
        OutTrace("ExtTextOutA: hdc=%#x pos=(%d,%d) String=\"%s\" rect=%s\n", hdc, X, Y, lpString, sRect);
    }
#endif
    if (dxw.IsToRemap(hdc) && !gFixed) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pExtTextOutA)(sdc.GetHdc(), X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
            if(lprc) {
                rc = *lprc;
                sdc.PutPrimaryDC(hdc, TRUE, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
            } else
                sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&X, &Y);
            if(lprc) {
                rc = *lprc;
                dxw.MapClient(&rc);
            }
            OutTraceSYS("ExtTextOutA: fixed pos=(%d,%d)\n", X, Y);
            break;
        }
    }
    if(lprc)
        return (*pExtTextOutA)(hdc, X, Y, fuOptions, &rc, lpString, cbCount, lpDx);
    else
        return (*pExtTextOutA)(hdc, X, Y, fuOptions, NULL, lpString, cbCount, lpDx);
}

BOOL WINAPI extExtTextOutW(HDC hdc, int X, int Y, UINT fuOptions, const RECT *lprc, LPCWSTR lpString, UINT cbCount, const INT *lpDx) {
    RECT rc;
    BOOL ret;
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        char sRect[81];
        if(lprc) sprintf(sRect, "(%d,%d)-(%d,%d)", lprc->left, lprc->top, lprc->right, lprc->bottom);
        else strcpy(sRect, "NULL");
        OutTrace("ExtTextOutW: hdc=%#x pos=(%d,%d) String=\"%ls\" rect=%s\n", hdc, X, Y, lpString, sRect);
    }
#endif
    if (dxw.IsToRemap(hdc) && !gFixed) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pExtTextOutW)(sdc.GetHdc(), X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
            if(lprc) {
                rc = *lprc;
                sdc.PutPrimaryDC(hdc, TRUE, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
            } else
                sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&X, &Y);
            if(lprc) {
                rc = *lprc;
                dxw.MapClient(&rc);
            }
            OutTraceSYS("ExtTextOutW: fixed pos=(%d,%d)\n", X, Y);
            break;
        }
    }
    if(lprc)
        return (*pExtTextOutW)(hdc, X, Y, fuOptions, &rc, lpString, cbCount, lpDx);
    else
        return (*pExtTextOutW)(hdc, X, Y, fuOptions, NULL, lpString, cbCount, lpDx);
}

#ifdef TRACEPALETTE
BOOL WINAPI extResizePalette(HPALETTE hpal, UINT nEntries) {
    OutTrace("ResizePalette: hpal=%#x nEntries=%d\n", hpal, nEntries);
    return (*pResizePalette)(hpal, nEntries);
}
#endif

#ifndef DXW_NOTRACES
char *sBkMode(int m) {
    char *s;
    switch(m) {
    case TRANSPARENT:
        s = "TRANSPARENT";
        break;
    case OPAQUE:
        s = "OPAQUE";
        break;
    default:
        s = "invalid";
        break;
    }
    return s;
}
#endif

#ifdef TRACEALL
int WINAPI extSetBkMode(HDC hdc, int iBkMode) {
    int ret;
    ApiName("SetBkMode");
    OutTraceSYS("%s: hdc=%#x bkmode=%#x(%s)\n", ApiRef, hdc, iBkMode, sBkMode(iBkMode));
    ret = (*pSetBkMode)(hdc, iBkMode);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

COLORREF WINAPI extSetBkColor(HDC hdc, COLORREF crColor) {
    COLORREF ret;
    ApiName("SetBkColor");
    OutTraceSYS("%s: hdc=%#x color=%#x\n", ApiRef, hdc, crColor);
    ret = (*pSetBkColor)(hdc, crColor);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}
#endif

COLORREF WINAPI extSetTextColor(HDC hdc, COLORREF crColor) {
    COLORREF ret;
    ApiName("SetTextColor");
    OutTraceSYS("%s: hdc=%#x color=%#x\n", ApiRef, hdc, crColor);
    if(dxw.dwFlags12 & W98OPAQUEFONT) crColor &= 0x00FFFFFF; // strip alpha
    ret = (*pSetTextColor)(hdc, crColor);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

COLORREF WINAPI extGetPixel(HDC hdc, int nXPos, int nYPos) {
    COLORREF ret;
    ApiName("GetPixel");
    OutTraceSYS("%s: hdc=%#x\n", ApiRef, hdc);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pGetPixel)(sdc.GetHdc(), nXPos, nYPos);
            sdc.PutPrimaryDC(hdc, FALSE);
            return ret;
            break;
        default:
            // to do .....
            break;
        }
    }
    ret = (*pGetPixel)(hdc, nXPos, nYPos);
    if(ret == CLR_INVALID)
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    else
        OutTraceSYS("%s: color=%#x\n", ApiRef, ret);
    return ret;
}

BOOL WINAPI extPlgBlt(HDC hdcDest, const POINT *lpPoint, HDC hdcSrc, int nXSrc, int nYSrc, int nWidth, int nHeight, HBITMAP hbmMask, int xMask, int yMask) {
    // found in "Spearhead"
    MessageBox(0, "PlgBlt", "DxWnd", MB_OK);
    return (COLORREF)0;
}

BOOL WINAPI extChord(HDC hdc, int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, int nXRadial1, int nYRadial1, int nXRadial2, int nYRadial2) {
    // v2.04.96: found in "Fallen Haven"
    BOOL ret;
    ApiName("Chord");
    OutTraceSYS("%s: hdc=%#x rect=(%d,%d)-(%d,%d) rad1=(%d,%d) rad2=(%d,%d)\n",
                ApiRef, hdc, nLeftRect, nTopRect, nRightRect, nBottomRect, nXRadial1, nYRadial1, nXRadial2, nYRadial2);
    if (dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pChord)(sdc.GetHdc(), nLeftRect, nTopRect, nRightRect, nBottomRect, nXRadial1, nYRadial1, nXRadial2, nYRadial2);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&nLeftRect, &nTopRect, &nRightRect, &nBottomRect);
            dxw.MapClient(&nXRadial1, &nYRadial1);
            dxw.MapClient(&nXRadial2, &nYRadial2);
            OutTraceDW("%s: FIXED rect=(%d,%d)-(%d,%d) rad1=(%d,%d) rad2=(%d,%d)\n",
                       ApiRef, nLeftRect, nTopRect, nRightRect, nBottomRect, nXRadial1, nYRadial1, nXRadial2, nYRadial2);
            break;
        }
    }
    ret = (*pChord)(hdc, nLeftRect, nTopRect, nRightRect, nBottomRect, nXRadial1, nYRadial1, nXRadial2, nYRadial2);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extPolyTextOutA(HDC hdc, const POLYTEXTA *pptxt, int cStrings) {
    MessageBox(0, "PolyTextOutA", "DxWnd", MB_OK);
    return TRUE;
}

BOOL WINAPI extPolyTextOutW(HDC hdc, const POLYTEXTW *pptxt, int cStrings) {
    MessageBox(0, "PolyTextOutW", "DxWnd", MB_OK);
    return TRUE;
}

HBITMAP WINAPI extCreateDIBitmap(HDC hdc, BITMAPINFOHEADER *lpbmih, DWORD fdwInit, const VOID *lpbInit, const BITMAPINFO *lpbmi, UINT fuUsage) {
    HBITMAP ret;
    ApiName("CreateDIBitmap");
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        OutTrace("%s: hdc=%#x init=%#x%s data=%#x usage=%#x(%s)\n",
                 ApiRef, hdc, fdwInit, fdwInit == CBM_INIT ? "(CBM_INIT)" : "", lpbInit,
                 fuUsage, ExplainDIBUsage(fuUsage));
        if(fdwInit == CBM_INIT) TraceBITMAPINFOHEADER("CreateDIBitmap(lpbmih)", lpbmih);
        TraceBITMAPINFOHEADER("CreateDIBitmap(lpbmi)", (BITMAPINFOHEADER *) & (lpbmi->bmiHeader));
    }
#endif
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pCreateDIBitmap)(sdc.GetHdc(), lpbmih, fdwInit, lpbInit, lpbmi, fuUsage);
            _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
            sdc.PutPrimaryDC(hdc, FALSE);
            return ret;
            break;
        default:
            break;
        }
    }
    ret = (*pCreateDIBitmap)(hdc, lpbmih, fdwInit, lpbInit, lpbmi, fuUsage);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

HBITMAP WINAPI extCreateDIBSection(HDC hdc, const BITMAPINFO *pbmi, UINT iUsage, VOID **ppvBits, HANDLE hSection, DWORD dwOffset) {
    ApiName("CreateDIBSection");
    HBITMAP ret;
    OutTraceSYS("%s: hdc=%#x bmi={%s} usage=%s hsect=%#x offset=%#x\n", ApiRef, hdc, sBMIDump((BITMAPINFO *)pbmi), ExplainDIBUsage(iUsage), hSection, dwOffset);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pCreateDIBSection)(sdc.GetHdc(), pbmi, iUsage, ppvBits, hSection, dwOffset);
            sdc.PutPrimaryDC(hdc, FALSE);
            break;
        default:
            ret = (*pCreateDIBSection)(hdc, pbmi, iUsage, ppvBits, hSection, dwOffset);
            break;
        }
    } else
        ret = (*pCreateDIBSection)(hdc, pbmi, iUsage, ppvBits, hSection, dwOffset);
    if(!ret)
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    else {
        OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
        // beware: it is worth dumping the DIB section only when hSection is not NULL
        // which means the bitmap is connected to a shared memory file
        // v2.04.54: flag moved to debug flags, dump activated by toggle key
#ifndef DXW_NOTRACES
        if((dxw.dwDFlags & DUMPDIBSECTION) && hSection)
            DumpDibSection(ApiRef, hdc, pbmi, iUsage, *ppvBits);
#endif // DXW_NOTRACES
    }
    return ret;
}

HBITMAP WINAPI extCreateDiscardableBitmap(HDC hdc, int nWidth, int nHeight) {
    HBITMAP ret;
    ApiName("CreateDiscardableBitmap");
    OutTraceSYS("%s: hdc=%#x size=(%dx%d)\n", ApiRef, hdc, nWidth, nHeight);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pCreateDiscardableBitmap)(sdc.GetHdc(), nWidth, nHeight);
            sdc.PutPrimaryDC(hdc, FALSE);
            return ret;
            break;
        default:
            break;
        }
    }
    ret = (*pCreateDiscardableBitmap)(hdc, nWidth, nHeight);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extExtFloodFill(HDC hdc, int nXStart, int nYStart, COLORREF crColor, UINT fuFillType) {
    BOOL ret;
    ApiName("ExtFloodFill");
    OutTraceSYS("%s: hdc=%#x pos=(%d,%d)\n", ApiRef, hdc, nXStart, nYStart);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pExtFloodFill)(sdc.GetHdc(), nXStart, nYStart, crColor, fuFillType);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        default:
            break;
        }
    }
    ret = (*pExtFloodFill)(hdc, nXStart, nYStart, crColor, fuFillType);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extGdiAlphaBlend(HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest, HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, BLENDFUNCTION ftn) {
    BOOL ret;
    ApiName("GdiAlphaBlend");
    int Flux;
    BOOL IsToScreen, IsFromScreen;
    BOOL IsDCLeakageSrc = FALSE;
    BOOL IsDCLeakageDest = FALSE;
    // v2.04.05: call found in Rhem during savegame load
    OutTraceSYS("%s: dest {hdc=%#x pos=(%d,%d) size=(%dx%d)} source {hdc=%#x pos=(%d,%d) size=(%dx%d)} ftn=%#x\n",
                ApiRef, hdcDest, xoriginDest, yoriginDest, wDest, hDest, hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, ftn);
    if(hdcDest == NULL) {
        hdcDest = (*pGDIGetDC)(dxw.GethWnd());
        OutDebugDW("%s: DEBUG hdc dest=NULL->%#x\n", ApiRef, hdcDest);
        IsDCLeakageDest = TRUE;
    }
    if(hdcSrc == NULL) {
        hdcSrc = (*pGDIGetDC)(dxw.GethWnd());
        OutDebugDW("%s: DEBUG hdc src=NULL->%#x\n", ApiRef, hdcSrc);
        IsDCLeakageSrc = TRUE;
    }
    IsToScreen = (OBJ_DC == (*pGetObjectType)(hdcDest));
    IsFromScreen = (OBJ_DC == (*pGetObjectType)(hdcSrc));
    Flux = (IsToScreen ? 1 : 0) + (IsFromScreen ? 2 : 0);
    if (IsToScreen && (dxw.dwDFlags & NOGDIBLT)) return TRUE;
    //_Warn("GdiAlphaBlend");
    switch(dxw.GDIEmulationMode) {
    case GDIMODE_SHAREDDC:
        switch(Flux) {
        case 0: // memory to memory
            ret = (*pGdiAlphaBlend)(hdcSrc, xoriginDest, yoriginDest, wDest, hDest, hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, ftn);
            break;
        case 1: // memory to screen
        case 3: // screen to screen
            sdc.GetPrimaryDC(hdcDest);
            ret = (*pGdiAlphaBlend)(sdc.GetHdc(), xoriginDest, yoriginDest, wDest, hDest, hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, ftn);
            sdc.PutPrimaryDC(hdcDest, TRUE, xoriginDest, yoriginDest, wDest, hDest);
            break;
        case 2: // screen to memory using virtual screen
            sdc.GetPrimaryDC(hdcSrc);
            ret = (*pGdiAlphaBlend)(hdcDest, xoriginDest, yoriginDest, wDest, hDest, sdc.GetHdc(), xoriginSrc, yoriginSrc, wSrc, hSrc, ftn);
            sdc.PutPrimaryDC(hdcSrc, FALSE, xoriginSrc, yoriginSrc, wSrc, hSrc);
            break;
        }
        break;
    case GDIMODE_STRETCHED:
        switch(Flux) {
        case 1: // memory to screen
            dxw.MapClient(&xoriginDest, &yoriginDest, &wDest, &hDest);
            break;
        case 2: // screen to memory
            dxw.MapClient(&xoriginSrc, &yoriginSrc, &wSrc, &hSrc);
            break;
        default:
            break;
        }
    // fallthrough ....
    case GDIMODE_EMULATED:
    default:
        ret = (*pGdiAlphaBlend)(hdcDest, xoriginDest, yoriginDest, wDest, hDest, hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, ftn);
        break;
    }
    if(IsDCLeakageSrc) (*pGDIReleaseDC)(dxw.GethWnd(), hdcSrc);
    if(IsDCLeakageDest) (*pGDIReleaseDC)(dxw.GethWnd(), hdcDest);
    if(ret && IsToScreen) dxw.ShowOverlay(hdcDest);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extGdiGradientFill(HDC hdc, PTRIVERTEX pVertex, ULONG nVertex, PVOID pMesh, ULONG nMesh, ULONG ulMode) {
    // v2.04.06.fx2: found in RHEM when DirectX emulation is off (?).
    // temporary version - doesn't scale nor return error
    BOOL ret;
    ApiName("GdiGradientFill");
    OutTraceSYS("%s: HDC=%#x nVertex=%d nMesh=%d mode=%#x(%s)\n",
                ApiRef, hdc, nVertex, nMesh, ulMode, sGradientMode(ulMode));
    ret = (*pGdiGradientFill)(hdc, pVertex, nVertex, pMesh, nMesh, ulMode);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return TRUE;
}

BOOL WINAPI extGdiTransparentBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight,
                                 HDC hdcSrc, int nXSrc, int nYSrc, int nWSrc, int nHSrc, UINT crTransparent) {
    BOOL res;
    ApiName("GdiTransparentBlt");
    BOOL IsToScreen;
    BOOL IsFromScreen;
    BOOL IsDCLeakageSrc = FALSE;
    BOOL IsDCLeakageDest = FALSE;
    int Flux;
    OutTraceSYS("%s: HDC=%#x nXDest=%d nYDest=%d nWidth=%d nHeight=%d hdcSrc=%#x nXSrc=%d nYSrc=%d nWSrc=%d nHSrc=%d transp=%#x\n",
                ApiRef, hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, nWSrc, nHSrc, crTransparent);
    OutDebugSYS("%s: DEBUG FullScreen=%#x target hdctype=%#x(%s) hwnd=%#x\n",
                ApiRef, dxw.IsFullScreen(), (*pGetObjectType)(hdcDest), ExplainDCType((*pGetObjectType)(hdcDest)), WindowFromDC(hdcDest));
    if(dxw.GDIEmulationMode == GDIMODE_EMULATED) {
        if (hdcDest == dxw.RealHDC) hdcDest = dxw.VirtualHDC;
        OutDebugDW("%s: DEBUG emulated hdc dest=%#x->%#x\n", ApiRef, dxw.RealHDC, hdcDest);
    }
    if(hdcDest == NULL) {
        // happens in Reah, hdc is NULL despite the fact that BeginPaint returns a valid DC. Too bad, we recover here ...
        hdcDest = (*pGDIGetDC)(dxw.GethWnd());
        OutDebugDW("%s: DEBUG hdc dest=NULL->%#x\n", ApiRef, hdcDest);
        IsDCLeakageDest = TRUE;
    }
    if(hdcSrc == NULL) {
        hdcSrc = (*pGDIGetDC)(dxw.GethWnd());
        OutDebugDW("%s: DEBUG hdc src=NULL->%#x\n", ApiRef, hdcSrc);
        IsDCLeakageSrc = TRUE;
    }
    IsToScreen = (OBJ_DC == (*pGetObjectType)(hdcDest));
    IsFromScreen = (OBJ_DC == (*pGetObjectType)(hdcSrc));
    Flux = (IsToScreen ? 1 : 0) + (IsFromScreen ? 2 : 0);
    if (IsToScreen && (dxw.dwDFlags & NOGDIBLT)) return TRUE;
    if(dxw.IsToRemap(hdcDest) && (hdcDest != hdcSrc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            switch(Flux) {
            case 0: // memory to memory
                res = (*pGdiTransparentBlt)(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, nWSrc, nHSrc, crTransparent);
                break;
            case 1: // memory to screen
            case 3: // screen to screen
                sdc.GetPrimaryDC(hdcDest);
                res = (*pGdiTransparentBlt)(sdc.GetHdc(), nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, nWSrc, nHSrc, crTransparent);
                sdc.PutPrimaryDC(hdcDest, TRUE, nXDest, nYDest, nWidth, nHeight);
                break;
            case 2: // screen to memory using virtual screen
                sdc.GetPrimaryDC(hdcSrc);
                res = (*pGdiTransparentBlt)(hdcDest, nXDest, nYDest, nWidth, nHeight, sdc.GetHdc(), nXSrc, nYSrc, nWSrc, nHSrc, crTransparent);
                sdc.PutPrimaryDC(hdcSrc, FALSE, nXSrc, nYSrc, nWSrc, nHSrc);
                break;
            }
            break;
        case GDIMODE_STRETCHED: {
            int nWDest, nHDest;
            nWDest = nWidth;
            nHDest = nHeight;
            switch(Flux) {
            case 1: // memory to screen
                dxw.MapClient(&nXDest, &nYDest, &nWDest, &nHDest);
                break;
            case 2: // screen to memory
                dxw.MapClient(&nXSrc, &nYSrc, &nWidth, &nHeight);
                break;
            default:
                break;
            }
            res = (*pGdiTransparentBlt)(hdcDest, nXDest, nYDest, nWDest, nHDest, hdcSrc, nXSrc, nYSrc, nWSrc, nHSrc, crTransparent);
            OutDebugDW("%s: DEBUG DC dest=(%d,%d) size=(%d,%d)\n", ApiRef, nXDest, nYDest, nWDest, nHDest);
        }
        break;
        case GDIMODE_EMULATED:
        default:
            res = (*pGdiTransparentBlt)(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, nWSrc, nHSrc, crTransparent);
            break;
        }
    } else
        res = (*pGdiTransparentBlt)(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, nWSrc, nHSrc, crTransparent);
    if(IsDCLeakageSrc) (*pGDIReleaseDC)(dxw.GethWnd(), hdcSrc);
    if(IsDCLeakageDest) (*pGDIReleaseDC)(dxw.GethWnd(), hdcDest);
    if(res && IsToScreen) {
        dxw.ShowOverlay(hdcDest);
        if(dxw.dwDFlags & MARKGDI32) dxw.Mark(hdcDest, FALSE, RGB(255, 255, 255), nXDest, nYDest, nWidth, nHeight);
    }
    _if(!res) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return res;
}

BOOL WINAPI extPie(HDC hdc, int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, int nXRadial1, int nYRadial1, int nXRadial2, int nYRadial2) {
    BOOL ret;
    ApiName("Pie");
    OutTraceSYS("%s: hdc=%#x rect=(%d,%d)-(%d,%d)\n", ApiRef, hdc, nLeftRect, nTopRect, nRightRect, nBottomRect);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pPie)(sdc.GetHdc(), nLeftRect, nTopRect, nRightRect, nBottomRect, nXRadial1, nYRadial1, nXRadial2, nYRadial2);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        default:
            break;
        }
    }
    ret = (*pPie)(hdc, nLeftRect, nTopRect, nRightRect, nBottomRect, nXRadial1, nYRadial1, nXRadial2, nYRadial2);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extAngleArc(HDC hdc, int X, int Y, DWORD dwRadius, FLOAT eStartAngle, FLOAT eSweepAngle) {
    BOOL ret;
    ApiName("AngleArc");
    OutTraceSYS("%s: hdc=%#x pos=(%d,%d)\n", ApiRef, hdc, X, Y);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pAngleArc)(sdc.GetHdc(), X, Y, dwRadius, eStartAngle, eSweepAngle);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        default:
            break;
        }
    }
    ret = (*pAngleArc)(hdc, X, Y, dwRadius, eStartAngle, eSweepAngle);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extPolyPolyline(HDC hdc, const POINT *lppt, const DWORD *lpdwPolyPoints, DWORD cCount) {
    BOOL ret;
    ApiName("PolyPolyline");
    OutTraceSYS("%s: hdc=%#x\n", ApiRef, hdc);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pPolyPolyline)(sdc.GetHdc(), lppt, lpdwPolyPoints, cCount);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        default:
            break;
        }
    }
    ret = (*pPolyPolyline)(hdc, lppt, lpdwPolyPoints, cCount);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

int WINAPI extSelectClipRgn(HDC hdc, HRGN hrgn) {
    int ret;
    ApiName("SelectClipRgn");
    OutTraceSYS("%s: hdc=%#x hrgn=%#x\n", ApiRef, hdc, hrgn);
    dxw.bIsSelectClipRgnRecursed = TRUE;
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pSelectClipRgn)(sdc.GetHdc(), hrgn);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            // from MSDN https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-selectcliprgn
            // Only a copy of the selected region is used.
            // The region itself can be selected for any number of other device contexts or it can be deleted.
            if(hrgn) { // beware: hrgn can be NULL !!!
                HRGN hrgnScaled;
                dxw.bIsSelectClipRgnRecursed = FALSE;
                hrgnScaled = dxw.MapRegion(ApiRef, hrgn);
                ret = (*pSelectClipRgn)(hdc, hrgnScaled);
                (*pDeleteObject)(hrgnScaled);
                OutTraceSYS("%s: ret=%d(%s)\n", ApiRef, ret, ExplainRgnType(ret));
                return ret;
            }
        default:
            break;
        }
    }
    ret = (*pSelectClipRgn)(hdc, hrgn);
    dxw.bIsSelectClipRgnRecursed = FALSE;
    OutTraceSYS("%s: ret=%d(%s)\n", ApiRef, ret, ExplainRgnType(ret));
    return ret;
}

int WINAPI extExtSelectClipRgn(HDC hdc, HRGN hrgn, int fnMode) {
    int ret;
    ApiName("ExtSelectClipRgn");
    // recursion bypass, don't even log the operation !!
    if(dxw.bIsSelectClipRgnRecursed) return(*pExtSelectClipRgn)(hdc, hrgn, fnMode);
    OutTraceSYS("%s: hdc=%#x hrgn=%#x mode=%#x(%s)\n",
                ApiRef, hdc, hrgn, fnMode, ExplainRgnMode(fnMode));
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pExtSelectClipRgn)(sdc.GetHdc(), hrgn, fnMode);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            // from MSDN https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-extselectcliprgn
            // Only a copy of the selected region is used.
            // The region itself can be selected for any number of other device contexts or it can be deleted.
            if(hrgn) { // beware: hrgn can be NULL !!!
                HRGN hrgnScaled;
                hrgnScaled = dxw.MapRegion(ApiRef, hrgn);
                ret = (*pExtSelectClipRgn)(hdc, hrgnScaled, fnMode);
                (*pDeleteObject)(hrgnScaled);
                OutTraceSYS("%s: ret=%d(%s)\n", ApiRef, ret, ExplainRgnType(ret));
                return ret;
            }
        default:
            break;
        }
    }
    ret = (*pExtSelectClipRgn)(hdc, hrgn, fnMode);
    OutTraceSYS("%s: ret=%d(%s)\n", ApiRef, ret, ExplainRgnType(ret));
    return ret;
}

int WINAPI extGetClipRgn(HDC hdc, HRGN hrgn) {
    int ret;
    ApiName("GetClipRgn");
    OutTraceSYS("%s: hdc=%#x hrgn=%#x\n", ApiRef, hdc, hrgn);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pGetClipRgn)(sdc.GetHdc(), hrgn);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED:
            if(hrgn) { // beware: hrgn can be NULL !!!
                HRGN hrgnScaled;
                ret = (*pGetClipRgn)(hdc, hrgn);
                hrgnScaled = dxw.UnmapRegion(ApiRef, hrgn);
                CombineRgn(hrgn, hrgnScaled, NULL, RGN_COPY);
                (*pDeleteObject)(hrgnScaled);
                OutTraceSYS("%s: ret=%d(%s)\n", ApiRef, ret, ExplainGetRgnCode(ret));
                _if(ret == 1) dxwDumpRgn(ApiRef, hrgn);
                return ret;
            }
        default:
            break;
        }
    }
    ret = (*pGetClipRgn)(hdc, hrgn);
    OutTraceSYS("%s: ret=%d(%s)\n", ApiRef, ret, ExplainGetRgnCode(ret));
    _if(ret == 1) dxwDumpRgn(ApiRef, hrgn);
    return ret;
}

int WINAPI extOffsetClipRgn(HDC hdc, int nXOffset, int nYOffset) {
    int ret;
    ApiName("OffsetClipRgn");
    OutTraceSYS("%s: hdc=%#x offset(x,y)=(%d,%d)\n", ApiRef, nXOffset, nYOffset);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pOffsetClipRgn)(sdc.GetHdc(), nXOffset, nYOffset);
            sdc.PutPrimaryDC(hdc, TRUE);
            OutTraceSYS("%s: ret=%d(%s)\n", ApiRef, ret, ExplainRgnType(ret));
            return ret;
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&nXOffset, &nYOffset);
        default:
            break;
        }
    }
    ret = (*pOffsetClipRgn)(hdc, nXOffset, nYOffset);
    OutTraceSYS("%s: ret=%d(%s)\n", ApiRef, ret, ExplainRgnType(ret));
    return ret;
}

BOOL WINAPI extFillRgn(HDC hdc, HRGN hrgn, HBRUSH hbr) {
    BOOL ret;
    ApiName("FrameRgn");
    OutTraceSYS("%s: hdc=%#x\n", ApiRef, hdc);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pFillRgn)(sdc.GetHdc(), hrgn, hbr);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED: {
            HRGN hrgnScaled = dxw.MapRegion(ApiRef, hrgn);
            ret = (*pFillRgn)(hdc, hrgnScaled, hbr);
            (*pDeleteObject)(hrgnScaled);
            _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
            return ret;
        }
        default:
            break;
        }
    }
    ret = (*pFillRgn)(hdc, hrgn, hbr);
    _if(!ret) OutTraceE("%s ERROR: err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extFrameRgn(HDC hdc, HRGN hrgn, HBRUSH hbr, int nWidth, int nHeight) {
    BOOL ret;
    ApiName("FrameRgn");
    OutTraceSYS("%s: hdc=%#x\n", ApiRef, hdc);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pFrameRgn)(sdc.GetHdc(), hrgn, hbr, nWidth, nHeight);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED: {
            HRGN hrgnScaled = dxw.MapRegion(ApiRef, hrgn);
            ret = (*pFrameRgn)(hdc, hrgnScaled, hbr, nWidth, nHeight);
            (*pDeleteObject)(hrgnScaled);
            _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
            return ret;
        }
        break;
        default:
            break;
        }
    }
    ret = (*pFrameRgn)(hdc, hrgn, hbr, nWidth, nHeight);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extInvertRgn(HDC hdc, HRGN hrgn) {
    BOOL ret;
    ApiName("InvertRgn");
    OutTraceSYS("%s: hdc=%#x hrgn=%#x\n", ApiRef, hdc, hrgn);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pInvertRgn)(sdc.GetHdc(), hrgn);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED: {
            HRGN hrgnScaled = dxw.MapRegion(ApiRef, hrgn);
            ret = (*pInvertRgn)(hdc, hrgnScaled);
            (*pDeleteObject)(hrgnScaled);
            _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
            return ret;
        }
        break;
        default:
            break;
        }
    }
    ret = (*pInvertRgn)(hdc, hrgn);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extPaintRgn(HDC hdc, HRGN hrgn) {
    BOOL ret;
    ApiName("PaintRgn");
    OutTraceSYS("%s: hdc=%#x hrgn=%#x\n", ApiRef, hdc, hrgn);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pPaintRgn)(sdc.GetHdc(), hrgn);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        case GDIMODE_STRETCHED: {
            HRGN hrgnScaled = dxw.MapRegion(ApiRef, hrgn);
            ret = (*pPaintRgn)(hdc, hrgnScaled);
            (*pDeleteObject)(hrgnScaled);
            _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
            return ret;
        }
        break;
        default:
            break;
        }
    }
    ret = (*pPaintRgn)(hdc, hrgn);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

int WINAPI extSetMapMode(HDC hdc, int fnMapMode) {
    int ret;
    ApiName("SetMapMode");
    OutTraceSYS("%s: hdc=%#x MapMode=%#x\n", ApiRef, hdc, fnMapMode);
#if 0
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pSetMapMode)(sdc.GetHdc(), fnMapMode);
            sdc.PutPrimaryDC(hdc, FALSE);
            return ret;
            break;
        default:
            break;
        }
    }
#endif
    ret = (*pSetMapMode)(hdc, fnMapMode);
    OutTraceSYS("%s: ret=%d\n", ApiRef, ret);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

int WINAPI extGetMapMode(HDC hdc) {
    int ret;
    ApiName("GetMapMode");
    ret = (*pGetMapMode)(hdc);
    OutTraceSYS("%s: hdc=%#x res=%d\n", ApiRef, hdc, ret);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extRoundRect(HDC hdc, int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, int nWidth, int nHeight) {
    int ret;
    OutTraceSYS("RoundRect: hdc=%#x rect=(%d,%d)-(%d,%d) ellipse=(%dx%d)\n", hdc, nLeftRect, nTopRect, nRightRect, nBottomRect, nWidth, nHeight);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_STRETCHED:
            // v2.04.28: found in "iF-22"
            dxw.MapClient(&nLeftRect, &nTopRect, &nRightRect, &nBottomRect);
            dxw.MapClient(&nWidth, &nHeight);
            OutTraceSYS("RoundRect: FIXED rect=(%d,%d)-(%d,%d) ellipse=(%dx%d)\n", nLeftRect, nTopRect, nRightRect, nBottomRect, nWidth, nHeight);
            break;
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pRoundRect)(sdc.GetHdc(), nLeftRect, nTopRect, nRightRect, nBottomRect, nWidth, nHeight);
            sdc.PutPrimaryDC(hdc, TRUE, nLeftRect, nTopRect, nRightRect, nBottomRect);
            return ret;
            break;
        default:
            break;
        }
    }
    ret = (*pRoundRect)(hdc, nLeftRect, nTopRect, nRightRect, nBottomRect, nWidth, nHeight);
    _if(!ret) OutTraceE("RoundRect ERROR: err=%d\n", GetLastError());
    return ret;
}

BOOL WINAPI extPolyPolygon(HDC hdc, const POINT *lpPoints, const INT *lpPolyCounts, int nCount) {
    BOOL ret;
    OutTraceSYS("PolyPolygon: hdc=%#x\n", hdc);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            sdc.GetPrimaryDC(hdc);
            ret = (*pPolyPolygon)(sdc.GetHdc(), lpPoints, lpPolyCounts, nCount);
            sdc.PutPrimaryDC(hdc, TRUE);
            return ret;
            break;
        default:
            break;
        }
    }
    ret = (*pPolyPolygon)(hdc, lpPoints, lpPolyCounts, nCount);
    _if(!ret) OutTraceE("PolyPolygon ERROR: err=%d\n", GetLastError());
    return ret;
}

#if 0
BOOL WINAPI extDPtoLP(HDC hdc, LPPOINT lpPoints, int nCount) {
    BOOL ret;
    OutTrace("DPtoLP: hdc=%#x, nCount=%d\n", hdc, nCount);
    for(int i = 0; i < nCount; i++) OutTrace("point[%d]=(%d,%d)\n", i, lpPoints[i].x, lpPoints[i].y);
    ret = (*pDPtoLP)(hdc, lpPoints, nCount);
    for(int i = 0; i < nCount; i++) OutTrace("point[%d]=(%d,%d)\n", i, lpPoints[i].x, lpPoints[i].y);
    return ret;
}
#endif

BOOL WINAPI extPlayEnhMetaFile(HDC hdc, HENHMETAFILE hemf, const RECT *lpRect) {
    BOOL ret;
    MessageBox(0, "PlayEnhMetaFile", "dxwnd", MB_OK);
    ret = pPlayEnhMetaFile(hdc, hemf, lpRect);
    return ret;
}

BOOL WINAPI extGetDCOrgEx(HDC hdc, LPPOINT lpPoint) {
    BOOL ret;
    ret = pGetDCOrgEx(hdc, lpPoint);
    OutTraceSYS("GetDCOrgEx: hdc=%#x pt=(%d,%d)\n", hdc, lpPoint->x, lpPoint->y);
    if(ret && dxw.IsFullScreen()) { // ?? and dxw.isDesktop() ???
        dxw.UnmapClient(lpPoint);
        OutTraceSYS("GetDCOrgEx: fixed pt=(%d,%d)\n", lpPoint->x, lpPoint->y);
    }
    return ret;
}

UINT WINAPI extSetPaletteEntries(HPALETTE hpal, UINT iStart, UINT cEntries, const PALETTEENTRY *lppe) {
    UINT ret;
    OutTraceSYS("SetPaletteEntries: hpal=%#x start=%d entries=%d\n", hpal, iStart, cEntries);
    ret = (*pSetPaletteEntries)(hpal, iStart, cEntries, lppe);
    // the only purpose of hooking this call is the fact that in windowed mode a palette update
    // does not flush the HDC updates to the device like in fullscreen (Win98?) mode.
    // if(dxw.IsFullScreen()) (*pInvalidateRect)(dxw.GethWnd(), NULL, FALSE);
    OutTraceSYS("SetPaletteEntries: ret=%d\n", ret);
    return ret;
}

int WINAPI extSetROP2(HDC hdc, int fnDrawMode) {
    // In early OS the SetROP2 caused some sort of screen refresh, that does no longer happen in recent ones.
    // So wrapping the function and inserting a InvalidateRect right after that fixes the problem.
    // This trick fixes the black screens in "Titanic - an adventure out of time" on Win10.
    // N.b. Titanic calls SetTextColor, SetBkColor and SetROP2 in sequence, it might be possible that the
    // API to hook for refreshing is another, or even them all!
    int ret;
    OutTraceSYS("SetROP2: hdc=%#x drawmode=%d\n", hdc, fnDrawMode);
    ret = (*pSetROP2)(hdc, fnDrawMode);
    return ret;
}

// v2.04.05: GetTextExtentPointA hooker for "Warhammer Shadow of the Horned Rat"
BOOL WINAPI extGetTextExtentPointA(HDC hdc, LPCTSTR lpString, int cbString, LPSIZE lpSize) {
    BOOL ret;
    OutTraceSYS("GetTextExtentPointA: hdc=%#x string=\"%s\"(%d)\n", hdc, lpString, cbString);
    ret = (*pGetTextExtentPointA)(hdc, lpString, cbString, lpSize);
    if(!ret) {
        OutTraceSYS("GetTextExtentPointA ERROR: err=%d\n", GetLastError);
        return ret;
    }
    OutTraceSYS("GetTextExtentPointA: size=(%dx%d)\n", lpSize->cx, lpSize->cy);
    // beware: size scaling is appropriate only when referred to video DC
    switch(dxw.GDIEmulationMode) {
    case GDIMODE_STRETCHED:
        if(dxw.Windowize && (OBJ_DC == (*pGetObjectType)(hdc))) {
            dxw.UnmapClient((LPPOINT)lpSize);
            OutTraceSYS("GetTextExtentPointA: remapped size=(%dx%d)\n", lpSize->cx, lpSize->cy);
        }
        break;
    default:
        break;
    }
    return ret;
}

// v2.04.05: GetTextExtentPoint32A hooker for "Warhammer Shadow of the Horned Rat"
BOOL WINAPI extGetTextExtentPoint32A(HDC hdc, LPCTSTR lpString, int cbString, LPSIZE lpSize) {
    BOOL ret;
    OutTraceSYS("GetTextExtentPoint32A: hdc=%#x(%s) string=\"%s\"(%d)\n", hdc, GetObjectTypeStr(hdc), lpString, cbString);
    ret = (*pGetTextExtentPoint32A)(hdc, lpString, cbString, lpSize);
    if(!ret) {
        OutTraceSYS("GetTextExtentPoint32A ERROR: err=%d\n", GetLastError);
        return ret;
    }
    OutTraceSYS("GetTextExtentPoint32A: size=(%dx%d)\n", lpSize->cx, lpSize->cy);
    // beware: size scaling is appropriate only when referred to video DC
    switch(dxw.GDIEmulationMode) {
    case GDIMODE_STRETCHED:
        if(dxw.Windowize && (OBJ_DC == (*pGetObjectType)(hdc))) {
            dxw.UnmapClient((LPPOINT)lpSize);
            OutTraceSYS("GetTextExtentPoint32A: remapped size=(%dx%d)\n", lpSize->cx, lpSize->cy);
        }
        break;
    default:
        break;
    }
    return ret;
}

#if 0
LONG WINAPI extSetBitmapBits(HBITMAP hbmp, DWORD cBytes, VOID *lpBits) {
    LONG ret;
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        OutTrace("SetDIBits: hdc=%#x hbmp=%#x lines=(%d,%d) ColorUse=%#x(%s)\n", hdc, hbmp, uStartScan, cScanLines, fuColorUse, ExplainDIBUsage(fuColorUse));
        TraceBITMAPINFOHEADER("SetDIBits", (BITMAPINFOHEADER *) & (lpbmi->bmiHeader));
    }
#endif
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC: // this will flicker !!!!
            sdc.GetPrimaryDC(hdc);
            ret = (*pSetDIBits)(sdc.GetHdc(), hbmp, uStartScan, cScanLines, lpvBits, lpbmi, fuColorUse);
            if(!ret || (ret == GDI_ERROR)) OutTraceE("SetDIBits: ERROR err=%d\n", GetLastError());
            sdc.PutPrimaryDC(hdc, TRUE, 0, 0, lpbmi->bmiHeader.biWidth, lpbmi->bmiHeader.biHeight);
            return ret;
            break;
        case GDIMODE_STRETCHED:
        case GDIMODE_EMULATED:
        default:
            break;
        }
    }
    ret = (*pSetBitmapBits)(hbmp, cBytes, lpBits);
    if(!ret || (ret == GDI_ERROR)) OutTraceE("SetDIBits: ERROR err=%d\n", GetLastError());
    return ret;
}
#endif

HGDIOBJ WINAPI extSelectObject(HDC hdc, HGDIOBJ hgdiobj) {
    HGDIOBJ ret;
    ApiName("SelectObject");
    DWORD dwObjectType = GetObjectType(hgdiobj);
    OutTraceSYS("%s: hdc=%#x(%s) obj=%#x(%s)\n", ApiRef, hdc, GetObjectTypeStr(hdc), hgdiobj, GetObjectTypeStr((HDC)hgdiobj));
    switch(dwObjectType) {
    case OBJ_FONT: {
        if(GetObjectType(hdc) == OBJ_DC) {
            HGDIOBJ scaled;
            scaled = fontdb.GetScaledFont((HFONT)hgdiobj);
            if(scaled) {
                hgdiobj = scaled;
                OutTraceSYS("SelectObject: replaced font obj=%#x\n", hgdiobj);
            } else
                OutTraceE("SelectObject: unmatched font obj=%#x\n", hgdiobj);
        }
    }
    break;
#ifndef DXW_NOTRACES
    case OBJ_BITMAP:
        if(dxw.dwDFlags2 & DUMPBITMAPS) DumpBitmap(ApiRef, (HBITMAP)hgdiobj);
        break;
#endif // DXW_NOTRACES
    }
    ret = (*pSelectObject)(hdc, hgdiobj);
    return ret;
}

BOOL WINAPI extDeleteObject(HGDIOBJ hgdiobj) {
    BOOL ret;
    HGDIOBJ scaledobj;
    OutTraceSYS("DeleteObject: obj=%#x(%s)\n", hgdiobj, GetObjectTypeStr((HDC)hgdiobj));
    if(GetObjectType(hgdiobj) == OBJ_FONT) {
        scaledobj = fontdb.DeleteFont((HFONT)hgdiobj); // v2.04.94: moved up
        if(scaledobj) (*pDeleteObject)(scaledobj);
        OutTraceSYS("DeleteObject: deleted font obj=%#x scaled=%#x\n", hgdiobj, scaledobj);
    }
    ret = (*pDeleteObject)(hgdiobj);
    return ret;
}

// --- NLS

// v2.05.14: GetStockObject wrapper here almost duly copied from ntleas code, who in turn seems
// copied from Arianrhod's Locale Emulator project, no longer available at that git address

static HFONT GetFontFromFont(BYTE DefaultCharset, LPCSTR DefaultFontFace, HFONT Font) {
    LOGFONTW LogFont;
    if (GetObjectW(Font, sizeof(LogFont), &LogFont) == 0) return NULL;
    LogFont.lfCharSet = DefaultCharset;
    MultiByteToWideChar(CP_ACP, 0, DefaultFontFace, -1, LogFont.lfFaceName, LF_FACESIZE);
    OutTraceSYS("GetFontFromFont: facename=%ls size=(%dx%d)\n", LogFont.lfFaceName, LogFont.lfWidth, LogFont.lfHeight);
    return CreateFontIndirectW(&LogFont);
}

__inline BYTE ToCharSet(int lfcharset) {
    return lfcharset > 0 ? (BYTE)lfcharset : DEFAULT_CHARSET;
}

extern int CodePageToCharset(UINT);

HGDIOBJ WINAPI extGetStockObject(int fnObject) {
    HGDIOBJ ret;
    ApiName("GetStockObject");
    // this code is considered from LE :
    // https://github.com/Arianrhod/Arianrhod/blob/master/Source/LocaleEmulator/LocaleEmulator/Gdi32Hook.cpp
    static const int StockObjectIndex[] = {
        OEM_FIXED_FONT, ANSI_FIXED_FONT, ANSI_VAR_FONT,
        SYSTEM_FONT, DEVICE_DEFAULT_FONT, SYSTEM_FIXED_FONT, DEFAULT_GUI_FONT,
    };
    static HFONT fontstock[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static int lfcharset = 0;
    if(!lfcharset) {
        lfcharset = CodePageToCharset(dxw.CodePage);
        OutTraceSYS("%s: initialized charset=%d cp=%d\n", ApiRef, lfcharset, dxw.CodePage);
    }
    for (int i = 0; i < ARRAYSIZE(StockObjectIndex); ++i) {
        if (StockObjectIndex[i] == fnObject) {
            if (!fontstock[fnObject])
                //fontstock[fnObject] = GetFontFromFont(ToCharSet(settings.lfcharset), (LPCSTR)settings.lpFontFaceName, GetStockObjectJ(fnObject));
                //fontstock[fnObject] = GetFontFromFont(ToCharSet(lfcharset), (LPCSTR)"MingLiu regular", (HFONT)(*pGetStockObject)(fnObject));
                //fontstock[fnObject] = GetFontFromFont(ToCharSet(lfcharset), (LPCSTR)"Microsoft Sans Serif", (HFONT)(*pGetStockObject)(fnObject));
                fontstock[fnObject] = GetFontFromFont(ToCharSet(lfcharset), (LPCSTR)"", (HFONT)(*pGetStockObject)(fnObject));
            OutTraceSYS("%s: fnObject=%d(%s) remapped ret=%#x\n", ApiRef, fnObject, objname(fnObject), fontstock[fnObject]);
            return fontstock[fnObject];
        }
    }
    ret = (*pGetStockObject)(fnObject);
    OutTraceSYS("%s: fnObject=%d(%s) ret=%#x\n", ApiRef, fnObject, objname(fnObject), ret);
    return ret;
}

BOOL WINAPI extGetCPInfo(UINT CodePage, LPCPINFO lpCPInfo) {
    BOOL res;
    ApiName("GetCPInfo");
    res = (*pGetCPInfo)(dxw.CodePage, lpCPInfo);
    OutTraceSYS("%s: cp=%d -> %d\n", ApiRef, CodePage, dxw.CodePage);
    if(res) {
        OutTraceSYS("> DefaultChar=0x%X\n", lpCPInfo->DefaultChar);
        OutTraceSYS("> LeadByte=0x%X\n", lpCPInfo->LeadByte);
        OutTraceSYS("> MaxCharSize=%d\n", lpCPInfo->MaxCharSize);
    } else
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return res;
}

BOOL WINAPI extSwapBuffers(HDC hdc) {
    if(dxw.dwFlags12 & PROJECTBUFFER) dxw.Project(hdc);
    return (*pSwapBuffers)(hdc);
}

#ifdef TRACEREGIONS
BOOL WINAPI extPtInRegion(HRGN hrgn, int x, int y) {
    ApiName("PtInRegion");
    BOOL ret;
    OutTraceSYS("%s: hrgn=%#x xy=(%d, %d)\n", ApiRef, hrgn, x, y);
    ret = (*pPtInRegion)(hrgn, x, y);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

int WINAPI extCombineRgn(HRGN hrgnDest, HRGN hrgn1, HRGN hrgn2, int iMode) {
    int ret;
    ApiName("CombineRgn");
    OutTraceSYS("%s: hrgnDest=%#x hrgn1=%#x hrgn2=%#x mode=%d(%s)\n",
                ApiRef, hrgnDest, hrgn1, hrgn2, iMode, ExplainRgnMode(iMode));
    ret = (*pCombineRgn)(hrgnDest, hrgn1, hrgn2, iMode);
    OutTraceSYS("%s: ret=%d(%s)\n", ApiRef, ret, ExplainRgnType(ret));
    return ret;
}

HRGN WINAPI extCreateEllipticRgn(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect) {
    HRGN ret;
    ApiName("CreateEllipticRgn");
    OutTraceSYS("%s: rect=(%d,%d)-(%d,%d)\n",
                ApiRef, nLeftRect, nTopRect, nRightRect, nBottomRect);
    ret = (*pCreateEllipticRgn)(nLeftRect, nTopRect, nRightRect, nBottomRect);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

HRGN WINAPI extCreateEllipticRgnIndirect(const RECT *lprc) {
    HRGN ret;
    ApiName("CreateEllipticRgnIndirect");
    OutTraceSYS("%s: rect=(%d,%d)-(%d,%d)\n",
                ApiRef, lprc->left, lprc->top, lprc->right, lprc->bottom);
    ret = (*pCreateEllipticRgnIndirect)(lprc);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

HRGN WINAPI extCreateRectRgn(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect) {
    HRGN ret;
    ApiName("CreateRectRgn");
    OutTraceSYS("%s: rect=(%d,%d)-(%d,%d)\n",
                ApiRef, nLeftRect, nTopRect, nRightRect, nBottomRect);
    ret = (*pCreateRectRgn)(nLeftRect, nTopRect, nRightRect, nBottomRect);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

HRGN WINAPI extCreateRectRgnIndirect(const RECT *lprc) {
    HRGN ret;
    ApiName("CreateRectRgnIndirect");
    RECT rc;
    OutTraceSYS("%s: rect=(%d,%d)-(%d,%d)\n",
                ApiRef, lprc->left, lprc->top, lprc->right, lprc->bottom);
    ret = (*pCreateRectRgnIndirect)(&rc);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

HRGN WINAPI extCreateRoundRectRgn(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, int w, int h) {
    HRGN ret;
    ApiName("CreateRoundRectRgn");
    OutTraceSYS("%s: rect=(%d,%d)-(%d,%d) ellypse=(%d,%d)\n",
                ApiRef, nLeftRect, nTopRect, nRightRect, nBottomRect, w, h);
    ret = (*pCreateRoundRectRgn)(nLeftRect, nTopRect, nRightRect, nBottomRect, w, h);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

HRGN WINAPI extCreatePolygonRgn(const POINT *lpPoints, int cPoints, int fnPolyFillMode) {
    HRGN ret;
    ApiName("CreatePolygonRgn");
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        int i;
        OutTrace("%s: PolyFillMode=%#x cCount=%d pt=", ApiRef, fnPolyFillMode, cPoints);
        if(IsDebugSYS) {
            for(i = 0; i < cPoints; i++) OutTrace("> pt[%d]=(%d,%d) ", i, lpPoints[i].x, lpPoints[i].y);
        }
    }
#endif
    ret = (*pCreatePolygonRgn)(lpPoints, cPoints, fnPolyFillMode);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

HRGN WINAPI extCreatePolyPolygonRgn(const POINT *lpPoints, CONST int *lpCounts, int cCount, int fnPolyFillMode) {
    HRGN ret;
    ApiName("CreatePolygonRgn");
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        int i;
        OutTrace("%s: PolyFillMode=%#x cCount=%d pt=", ApiRef, fnPolyFillMode, cCount);
        if(IsDebugSYS) {
            int k = 0;
            for(i = 0; i < cCount; i++)
                for(int j = 0; j < lpCounts[i]; j++, k++)
                    OutTrace("> pt[%d-%d]=(%d,%d) ", i, j, lpPoints[k].x, lpPoints[k].y);
        }
    }
#endif
    ret = (*pCreatePolyPolygonRgn)(lpPoints, lpCounts, cCount, fnPolyFillMode);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extSetRectRgn(HRGN hrgn, int nLeftRect, int nTopRect, int nRightRect, int nBottomRect) {
    BOOL ret;
    ApiName("SetRectRgn");
    OutTraceSYS("%s: hrgn=%#x rect=(%d,%d)-(%d,%d)\n",
                ApiRef, hrgn, nLeftRect, nTopRect, nRightRect, nBottomRect);
    ret = (*pSetRectRgn)(hrgn, nLeftRect, nTopRect, nRightRect, nBottomRect);
    _if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

int WINAPI extOffsetRgn(HRGN hrgn, int nXOffset, int nYOffset) {
    int ret;
    ApiName("OffsetRgn");
    OutTraceSYS("%s: hrgn=%#x nXOffset=%d nYOffset=%d\n", ApiRef, hrgn, nXOffset, nYOffset);
    ret = (*pOffsetRgn)(hrgn, nXOffset, nYOffset);
    OutTraceSYS("%s: ret=%d(%s)\n", ApiRef, ret, ExplainRgnType(ret));
    return ret;
}

int WINAPI extGetRgnBox(HRGN hrgn, LPRECT lprc) {
    int ret;
    ApiName("GetRgnBox");
    OutTraceSYS("%s: hrgn=%#x\n", ApiRef, hrgn);
    ret = (*pGetRgnBox)(hrgn, lprc);
    OutTraceSYS("%s: ret=%d(%s) rect=(%d,%d)-(%d,%d)\n",
                ApiRef, ret, ExplainRegionType(ret), lprc->left, lprc->top, lprc->right, lprc->bottom);
    return ret;
}

int WINAPI extGetRandomRgn(HDC hdc, HRGN hrgn, INT i) {
    int ret;
    ApiName("GetRandomRgn");
    OutTraceSYS("%s: hdc=%#x hrgn=%#x i=%d\n", ApiRef, hdc, hrgn, i);
    MessageBox(0, "GetRandomRgn", "DxWnd", 0);
    ret = (*pGetRandomRgn)(hdc, hrgn, i);
    OutTraceSYS("%s: ret=%d(%s)\n", ApiRef, ret, ret ? (ret == 1 ? "OK" : "ERROR") : "NULL");
    return ret;
}

int WINAPI extGetMetaRgn(HDC hdc, HRGN hrgn) {
    int ret;
    ApiName("GetMetaRgn");
    OutTraceSYS("%s: hdc=%#x hrgn=%#x\n", ApiRef, hdc, hrgn);
    MessageBox(0, "GetMetaRgn", "DxWnd", 0);
    ret = (*pGetMetaRgn)(hdc, hrgn);
    OutTraceSYS("%s: ret=%d(%s)\n", ApiRef, ret, ret ? "OK" : "ERROR");
    return ret;
}

int WINAPI extSetMetaRgn(HDC hdc) {
    int ret;
    ApiName("SetMetaRgn");
    OutTraceSYS("%s: hdc=%#x\n", ApiRef, hdc);
    MessageBox(0, "SetMetaRgn", "DxWnd", 0);
    ret = (*pSetMetaRgn)(hdc);
    OutTraceSYS("%s: ret=%d(%s)\n", ApiRef, ret, ExplainRegionType(ret));
    return ret;
}
#endif

BOOL WINAPI extPtVisible(HDC hdc, int x, int y) {
    BOOL ret;
    ApiName("PtVisible");
    OutTraceSYS("%s: hdc=%#x pt=(%d,%d)\n", ApiRef, hdc, x, y);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            hdc = sdc.GetPrimaryDC(hdc);
            break;
        case GDIMODE_STRETCHED:
            dxw.MapClient(&x, &y);
        case GDIMODE_EMULATED:
        default:
            break;
        }
    }
    ret = (*pPtVisible)(hdc, x, y);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

BOOL WINAPI extRectVisible(HDC hdc, const RECT *lprect) {
    BOOL ret;
    ApiName("RectVisible");
    RECT rect;
    OutTraceSYS("%s: hdc=%#x rect=(%d,%d)-(%d,%d)\n",
                ApiRef, hdc, lprect->left, lprect->top, lprect->right, lprect->bottom);
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            hdc = sdc.GetPrimaryDC(hdc);
            break;
        case GDIMODE_STRETCHED:
            rect = *lprect; // work on a copy
            lprect = &rect;
            dxw.MapClient((LPRECT)lprect);
        case GDIMODE_EMULATED:
        default:
            break;
        }
    }
    ret = (*pRectVisible)(hdc, lprect);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

BOOL WINAPI extSelectClipPath(HDC hdc, int mode) {
    BOOL ret;
    ApiName("SelectClipPath");
    MessageBox(0, "SelectClipPath", "DxWnd", 0);
    // so far, just proxy: how to handle this ???
    OutTraceSYS("%s: hdc=%#x mode=%d(%s)\n", ApiRef, hdc, mode, ExplainRgnMode(mode));
    if(dxw.IsToRemap(hdc)) {
        switch(dxw.GDIEmulationMode) {
        case GDIMODE_SHAREDDC:
            hdc = sdc.GetPrimaryDC(hdc);
            break;
        case GDIMODE_STRETCHED: {
            int cpt;
            cpt = GetPath(hdc, NULL, NULL, 0);
            OutTraceSYS("%s: current path count=%d\n", ApiRef, cpt);
            // to do : scale current path ???
        }
        case GDIMODE_EMULATED:
        default:
            break;
        }
    }
    ret = (*pSelectClipPath)(hdc, mode);
    OutTraceSYS("%s: ret=%#x(%s)\n", ApiRef, ret, ret ? "OK" : "ERR");
    return ret;
}

#ifdef TRACEBITMAPS
HBITMAP WINAPI extCreateBitmap(int nWidth, int nHeight, UINT nPlanes, UINT nBitCount, CONST VOID *lpBits) {
    HBITMAP res;
    ApiName("CreateBitmap");
    OutTraceSYS("%s: size=(%d x %d) planes=%d bitc=%d bits=%#x\n",
                ApiRef, nWidth, nHeight, nPlanes, nBitCount, lpBits);
    res = (*pCreateBitmap)(nWidth, nHeight, nPlanes, nBitCount, lpBits);
    if(res) {
        OutTraceSYS("%s: hbitmap=%#x\n", ApiRef, res);
#ifndef DXW_NOTRACES
        if(dxw.dwDFlags2 & DUMPBITMAPS) DumpBitmap(ApiRef, res);
#endif // DXW_NOTRACES
    } else
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return res;
}
#endif // TRACEBITMAPS

#ifdef TRACEALL
int WINAPI extGetObject(ApiArg, GetObject_Type pGetObject, HANDLE h, int c, LPVOID pv) {
    int res;
    DWORD dwObjectType = GetObjectType(h);
    OutTraceSYS("%s: hobj=%#x(%s) c=%d\n", ApiRef, h, GetObjectTypeStr((HDC)h), c);
    res = (*pGetObjectW)(h, c, pv);
    if(res && IsTraceSYS) {
        OutTrace("%s: res=%d\n", ApiRef, res);
        if(pv && res) {
            switch(dwObjectType) {
            case OBJ_BITMAP:
                if(c >= sizeof(BITMAP)) {
                    LPBITMAP lpbmp = (LPBITMAP)pv;
                    OutTrace("> type=%d\n", lpbmp->bmType);
                    OutTrace("> size=(%d x %d)\n", lpbmp->bmWidth, lpbmp->bmHeight);
                    OutTrace("> widthbytes=%d\n", lpbmp->bmWidthBytes);
                    OutTrace("> planes=%d\n", lpbmp->bmPlanes);
                    OutTrace("> bpp=%d\n", lpbmp->bmBitsPixel);
                }
                break;
            case OBJ_PAL:
                OutTrace("> entries=%d\n", (WORD)pv);
                break;
            case OBJ_PEN:
                if(c == sizeof(LOGPEN)) {
                    LPLOGPEN lppen = (LPLOGPEN)pv;
                    OutTrace("> style=%d\n", lppen->lopnStyle);
                    OutTrace("> width=%d\n", lppen->lopnWidth);
                    OutTrace("> color=%#x\n", lppen->lopnColor);
                }
                if(c == sizeof(EXTLOGPEN)) {
                    LPEXTLOGPEN lppen = (LPEXTLOGPEN)pv;
                    OutTrace("> pen style=%d\n", lppen->elpPenStyle);
                    OutTrace("> width=%d\n", lppen->elpWidth);
                    OutTrace("> color=%#x\n", lppen->elpColor);
                    OutTrace("> brush style=%d\n", lppen->elpBrushStyle);
                    OutTrace("> hatch=%d\n", lppen->elpHatch);
                    OutTrace("> numentries=%d\n", lppen->elpNumEntries);
                }
                break;
            case OBJ_BRUSH:
                if(c == sizeof(LOGBRUSH)) {
                    LPLOGBRUSH lpbrush = (LPLOGBRUSH)pv;
                    OutTrace("> style=%d\n", lpbrush->lbStyle);
                    OutTrace("> hatch=%d\n", lpbrush->lbHatch);
                    OutTrace("> color=%#x\n", lpbrush->lbColor);
                }
                break;
            case OBJ_FONT:
                if(c == sizeof(LOGFONTA)) {
                    LPLOGFONTA lplf = (LPLOGFONTA)pv;
                    OutTrace("> FaceName: \"%s\"\n", lplf->lfFaceName);
                    OutTrace("> size(w,h): (%d,%d)\n", lplf->lfWidth, lplf->lfHeight);
                    OutTrace("> Escapement: %d\n", lplf->lfEscapement);
                    OutTrace("> Orientation: %d\n", lplf->lfOrientation);
                    OutTrace("> fnWeight: %d\n", lplf->lfWeight);
                    OutTrace("> fdwItalic: %d\n", lplf->lfItalic);
                    OutTrace("> fdwUnderline: %d\n", lplf->lfUnderline);
                    OutTrace("> fdwStrikeOut: %d\n", lplf->lfStrikeOut);
                    OutTrace("> fdwCharSet: %d\n", lplf->lfCharSet);
                    OutTrace("> fdwOutputPrecision: %d\n", lplf->lfOutPrecision);
                    OutTrace("> fdwClipPrecision: %d\n", lplf->lfClipPrecision);
                    OutTrace("> fdwQuality: %d\n", lplf->lfQuality);
                    OutTrace("> fdwPitchAndFamily: %d\n", lplf->lfPitchAndFamily);
                }
                if(c == sizeof(LOGFONTW)) {
                    LPLOGFONTW lplf = (LPLOGFONTW)pv;
                    OutTrace("> FaceName: \"%ls\"\n", lplf->lfFaceName);
                    OutTrace("> size(w,h): (%d,%d)\n", lplf->lfWidth, lplf->lfHeight);
                    OutTrace("> Escapement: %d\n", lplf->lfEscapement);
                    OutTrace("> Orientation: %d\n", lplf->lfOrientation);
                    OutTrace("> fnWeight: %d\n", lplf->lfWeight);
                    OutTrace("> fdwItalic: %d\n", lplf->lfItalic);
                    OutTrace("> fdwUnderline: %d\n", lplf->lfUnderline);
                    OutTrace("> fdwStrikeOut: %d\n", lplf->lfStrikeOut);
                    OutTrace("> fdwCharSet: %d\n", lplf->lfCharSet);
                    OutTrace("> fdwOutputPrecision: %d\n", lplf->lfOutPrecision);
                    OutTrace("> fdwClipPrecision: %d\n", lplf->lfClipPrecision);
                    OutTrace("> fdwQuality: %d\n", lplf->lfQuality);
                    OutTrace("> fdwPitchAndFamily: %d\n", lplf->lfPitchAndFamily);
                }
                break;
            default:
                break;
            }
        }
    } else
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return res;
}

int WINAPI extGetObjectA(HANDLE h, int c, LPVOID pv) {
    return extGetObject("GetObjectA", pGetObjectA, h, c, pv);
}
int WINAPI extGetObjectW(HANDLE h, int c, LPVOID pv) {
    return extGetObject("GetObjectW", pGetObjectW, h, c, pv);
}

HGDIOBJ WINAPI extGetCurrentObject(HDC hdc, UINT type) {
    HGDIOBJ res;
    ApiName("GetCurrentObject");
    OutTrace("%s: hdc=%#x type=%d(%s)\n", ApiRef, hdc, type, ExplainDCType(type));
    res = (*pGetCurrentObject)(hdc, type);
    OutTrace("%s: res=%#x\n", ApiRef, res);
    return res;
}
#endif // TRACEALL
