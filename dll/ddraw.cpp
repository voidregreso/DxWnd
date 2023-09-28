#define _CRT_SECURE_NO_WARNINGS
#define INITGUID
//#define FULLHEXDUMP
//#define GETSCANLINETRACE // to have GetScanLine trace in debug tracing

//#define TRACEALL
//#define NOFASTBLT

#include <windows.h>
#include <ddraw.h>
#include "dxwnd.h"
#include "dxhook.h"
#include "ddrawi.h"
#include "dxwcore.hpp"
#include "stdio.h"
#include "hddraw.h"
#include "dxhelper.h"
#include "syslibs.h"

#ifdef CHECKMETHODPOINTERS
static void NullMethodPointer(char *s) {
    char msg[80];
    sprintf(msg, "NULL %s method", s);
    MessageBox(0, msg, "DxWnd", MB_OK);
}
#define CHECKPTR(p, s) if(!p) NullMethodPointer(s)
#else
#define CHECKPTR(p, s)
#endif

extern BOOL IsChangeDisplaySettingsHotPatched;
extern void TextureHandling(LPDIRECTDRAWSURFACE, int);
extern void SetMinimalCaps(int, LPDDCAPS, LPDDCAPS);
extern void SurfaceDump(LPDIRECTDRAWSURFACE, int);
ColorConversion_Type pColorConversion = NULL;
extern HRESULT WINAPI extQueryInterfaceDX(int, QueryInterface_Type, void *, REFIID, LPVOID *);

HDC hFlippedDC = NULL;
BOOL bFlippedDC = FALSE;
DWORD dwBackBufferCaps = 0;

#ifndef DXW_NOTRACES
#define TRAP 	MessageBox(NULL, ApiRef, "DxWnd trap", 0)
#else
#define TRAP
#endif

// DirectDraw API
HRESULT WINAPI extDirectDrawCreate(GUID FAR *, LPDIRECTDRAW FAR *, IUnknown FAR *);
HRESULT WINAPI extDirectDrawCreateEx(GUID FAR *, LPDIRECTDRAW FAR *, REFIID, IUnknown FAR *);
HRESULT WINAPI extDirectDrawEnumerateA(LPDDENUMCALLBACKA, LPVOID);
HRESULT WINAPI extDirectDrawEnumerateExA(LPDDENUMCALLBACKEXA, LPVOID, DWORD);
HRESULT WINAPI extDirectDrawEnumerateW(LPDDENUMCALLBACKW, LPVOID);
HRESULT WINAPI extDirectDrawEnumerateExW(LPDDENUMCALLBACKEXW, LPVOID, DWORD);
HRESULT WINAPI extDirectDrawCreateClipper(DWORD, LPDIRECTDRAWCLIPPER *, IUnknown *);
HRESULT WINAPI extDDGetSurfaceFromDC(HDC, LPDIRECTDRAWSURFACE *);

// DirectDraw
HRESULT WINAPI extQueryInterfaceD1(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceD2(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceD3(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceD4(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceD7(void *, REFIID, LPVOID *);
ULONG WINAPI extReleaseD1(LPDIRECTDRAW);
ULONG WINAPI extReleaseD2(LPDIRECTDRAW);
ULONG WINAPI extReleaseD3(LPDIRECTDRAW);
ULONG WINAPI extReleaseD4(LPDIRECTDRAW);
ULONG WINAPI extReleaseD7(LPDIRECTDRAW);

/*** IDirectDraw methods ***/
HRESULT WINAPI extCompact1(LPDIRECTDRAW);
HRESULT WINAPI extCompact2(LPDIRECTDRAW);
HRESULT WINAPI extCompact3(LPDIRECTDRAW);
HRESULT WINAPI extCompact4(LPDIRECTDRAW);
HRESULT WINAPI extCompact7(LPDIRECTDRAW);
HRESULT WINAPI extCreateClipper1(LPDIRECTDRAW, DWORD, LPDIRECTDRAWCLIPPER FAR *, IUnknown FAR *);
HRESULT WINAPI extCreateClipper2(LPDIRECTDRAW, DWORD, LPDIRECTDRAWCLIPPER FAR *, IUnknown FAR *);
HRESULT WINAPI extCreateClipper3(LPDIRECTDRAW, DWORD, LPDIRECTDRAWCLIPPER FAR *, IUnknown FAR *);
HRESULT WINAPI extCreateClipper4(LPDIRECTDRAW, DWORD, LPDIRECTDRAWCLIPPER FAR *, IUnknown FAR *);
HRESULT WINAPI extCreateClipper7(LPDIRECTDRAW, DWORD, LPDIRECTDRAWCLIPPER FAR *, IUnknown FAR *);
HRESULT WINAPI extCreatePalette1(LPDIRECTDRAW, DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown *);
HRESULT WINAPI extCreatePalette2(LPDIRECTDRAW, DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown *);
HRESULT WINAPI extCreatePalette3(LPDIRECTDRAW, DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown *);
HRESULT WINAPI extCreatePalette4(LPDIRECTDRAW, DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown *);
HRESULT WINAPI extCreatePalette7(LPDIRECTDRAW, DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown *);
HRESULT WINAPI extCreateSurface1(LPDIRECTDRAW, DDSURFACEDESC *, LPDIRECTDRAWSURFACE *, void *);
HRESULT WINAPI extCreateSurface2(LPDIRECTDRAW, DDSURFACEDESC *, LPDIRECTDRAWSURFACE *, void *);
HRESULT WINAPI extCreateSurface3(LPDIRECTDRAW, DDSURFACEDESC *, LPDIRECTDRAWSURFACE *, void *);
HRESULT WINAPI extCreateSurface4(LPDIRECTDRAW, DDSURFACEDESC2 *, LPDIRECTDRAWSURFACE *, void *);
HRESULT WINAPI extCreateSurface7(LPDIRECTDRAW, DDSURFACEDESC2 *, LPDIRECTDRAWSURFACE *, void *);
HRESULT WINAPI extDuplicateSurface1(LPDIRECTDRAW, LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE FAR *);
HRESULT WINAPI extDuplicateSurface2(LPDIRECTDRAW, LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE FAR *);
HRESULT WINAPI extDuplicateSurface3(LPDIRECTDRAW, LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE FAR *);
HRESULT WINAPI extDuplicateSurface4(LPDIRECTDRAW, LPDIRECTDRAWSURFACE4, LPDIRECTDRAWSURFACE4 FAR *);
HRESULT WINAPI extDuplicateSurface7(LPDIRECTDRAW, LPDIRECTDRAWSURFACE7, LPDIRECTDRAWSURFACE7 FAR *);
HRESULT WINAPI extEnumSurfaces1(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extEnumSurfaces2(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extEnumSurfaces3(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extEnumSurfaces4(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMSURFACESCALLBACK2);
HRESULT WINAPI extEnumSurfaces7(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMSURFACESCALLBACK7);
HRESULT WINAPI extFlipToGDISurface1(LPDIRECTDRAW);
HRESULT WINAPI extFlipToGDISurface2(LPDIRECTDRAW);
HRESULT WINAPI extFlipToGDISurface3(LPDIRECTDRAW);
HRESULT WINAPI extFlipToGDISurface4(LPDIRECTDRAW);
HRESULT WINAPI extFlipToGDISurface7(LPDIRECTDRAW);
HRESULT WINAPI extGetFourCCCodes1(LPDIRECTDRAW, LPDWORD, LPDWORD);
HRESULT WINAPI extGetFourCCCodes2(LPDIRECTDRAW, LPDWORD, LPDWORD);
HRESULT WINAPI extGetFourCCCodes3(LPDIRECTDRAW, LPDWORD, LPDWORD);
HRESULT WINAPI extGetFourCCCodes4(LPDIRECTDRAW, LPDWORD, LPDWORD);
HRESULT WINAPI extGetFourCCCodes7(LPDIRECTDRAW, LPDWORD, LPDWORD);
HRESULT WINAPI extGetDisplayMode1(LPDIRECTDRAW, LPDDSURFACEDESC);
HRESULT WINAPI extGetDisplayMode2(LPDIRECTDRAW, LPDDSURFACEDESC);
HRESULT WINAPI extGetDisplayMode3(LPDIRECTDRAW, LPDDSURFACEDESC);
HRESULT WINAPI extGetDisplayMode4(LPDIRECTDRAW, LPDDSURFACEDESC2);
HRESULT WINAPI extGetDisplayMode7(LPDIRECTDRAW, LPDDSURFACEDESC2);
HRESULT WINAPI extGetGDISurface1(LPDIRECTDRAW, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extGetGDISurface2(LPDIRECTDRAW, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extGetGDISurface3(LPDIRECTDRAW, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extGetGDISurface4(LPDIRECTDRAW, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extGetGDISurface7(LPDIRECTDRAW, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extEnumDisplayModes1(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK);
HRESULT WINAPI extEnumDisplayModes2(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK);
HRESULT WINAPI extEnumDisplayModes3(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK);
HRESULT WINAPI extEnumDisplayModes4(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2);
HRESULT WINAPI extEnumDisplayModes7(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2);
HRESULT WINAPI extInitialize1(LPDIRECTDRAW, FAR GUID *);
HRESULT WINAPI extInitialize2(LPDIRECTDRAW, FAR GUID *);
HRESULT WINAPI extInitialize3(LPDIRECTDRAW, FAR GUID *);
HRESULT WINAPI extInitialize4(LPDIRECTDRAW, FAR GUID *);
HRESULT WINAPI extInitialize7(LPDIRECTDRAW, FAR GUID *);
HRESULT WINAPI extRestoreDisplayMode1(LPDIRECTDRAW);
HRESULT WINAPI extRestoreDisplayMode2(LPDIRECTDRAW);
HRESULT WINAPI extRestoreDisplayMode3(LPDIRECTDRAW);
HRESULT WINAPI extRestoreDisplayMode4(LPDIRECTDRAW);
HRESULT WINAPI extRestoreDisplayMode7(LPDIRECTDRAW);
HRESULT WINAPI extSetCooperativeLevel1(LPDIRECTDRAW, HWND, DWORD);
HRESULT WINAPI extSetCooperativeLevel2(LPDIRECTDRAW, HWND, DWORD);
HRESULT WINAPI extSetCooperativeLevel3(LPDIRECTDRAW, HWND, DWORD);
HRESULT WINAPI extSetCooperativeLevel4(LPDIRECTDRAW, HWND, DWORD);
HRESULT WINAPI extSetCooperativeLevel7(LPDIRECTDRAW, HWND, DWORD);
HRESULT WINAPI extSetDisplayMode1(LPDIRECTDRAW, DWORD, DWORD, DWORD);
HRESULT WINAPI extSetDisplayMode2(LPDIRECTDRAW, DWORD, DWORD, DWORD, DWORD, DWORD);
HRESULT WINAPI extSetDisplayMode3(LPDIRECTDRAW, DWORD, DWORD, DWORD, DWORD, DWORD);
HRESULT WINAPI extSetDisplayMode4(LPDIRECTDRAW, DWORD, DWORD, DWORD, DWORD, DWORD);
HRESULT WINAPI extSetDisplayMode7(LPDIRECTDRAW, DWORD, DWORD, DWORD, DWORD, DWORD);
HRESULT WINAPI extWaitForVerticalBlank1(LPDIRECTDRAW, DWORD, HANDLE);
HRESULT WINAPI extWaitForVerticalBlank2(LPDIRECTDRAW, DWORD, HANDLE);
HRESULT WINAPI extWaitForVerticalBlank3(LPDIRECTDRAW, DWORD, HANDLE);
HRESULT WINAPI extWaitForVerticalBlank4(LPDIRECTDRAW, DWORD, HANDLE);
HRESULT WINAPI extWaitForVerticalBlank7(LPDIRECTDRAW, DWORD, HANDLE);
/*** Added in the V2 Interface ***/
HRESULT WINAPI extGetAvailableVidMem2(LPDIRECTDRAW, LPDDSCAPS, LPDWORD, LPDWORD);
HRESULT WINAPI extGetAvailableVidMem3(LPDIRECTDRAW, LPDDSCAPS, LPDWORD, LPDWORD);
HRESULT WINAPI extGetAvailableVidMem4(LPDIRECTDRAW, LPDDSCAPS2, LPDWORD, LPDWORD);
HRESULT WINAPI extGetAvailableVidMem7(LPDIRECTDRAW, LPDDSCAPS2, LPDWORD, LPDWORD);
/*** Added in the V4 Interface ***/
HRESULT WINAPI extTestCooperativeLevel4(LPDIRECTDRAW);
HRESULT WINAPI extTestCooperativeLevel7(LPDIRECTDRAW);
HRESULT WINAPI extGetSurfaceFromDC4(LPDIRECTDRAW, HDC, LPDIRECTDRAWSURFACE4 *);
HRESULT WINAPI extGetSurfaceFromDC7(LPDIRECTDRAW, HDC, LPDIRECTDRAWSURFACE4 *);
//    STDMETHOD(StartModeTest)(THIS_ LPSIZE, DWORD, DWORD ) PURE;
//    STDMETHOD(EvaluateMode)(THIS_ DWORD, DWORD * ) PURE;
HRESULT WINAPI extGetCaps1D(LPDIRECTDRAW, LPDDCAPS, LPDDCAPS);
HRESULT WINAPI extGetCaps2D(LPDIRECTDRAW, LPDDCAPS, LPDDCAPS);
HRESULT WINAPI extGetCaps3D(LPDIRECTDRAW, LPDDCAPS, LPDDCAPS);
HRESULT WINAPI extGetCaps4D(LPDIRECTDRAW, LPDDCAPS, LPDDCAPS);
HRESULT WINAPI extGetCaps7D(LPDIRECTDRAW, LPDDCAPS, LPDDCAPS);
HRESULT WINAPI extGetScanLine1(LPDIRECTDRAW, LPDWORD);
HRESULT WINAPI extGetScanLine2(LPDIRECTDRAW, LPDWORD);
HRESULT WINAPI extGetScanLine3(LPDIRECTDRAW, LPDWORD);
HRESULT WINAPI extGetScanLine4(LPDIRECTDRAW, LPDWORD);
HRESULT WINAPI extGetScanLine7(LPDIRECTDRAW, LPDWORD);
/*** Added in the V7 Interface ??? ***/
HRESULT WINAPI extStartModeTest(LPDIRECTDRAW, LPSIZE, DWORD, DWORD);
HRESULT WINAPI extEvaluateMode(LPDIRECTDRAW, DWORD, DWORD *);
#ifdef TRACEALL
HRESULT WINAPI extGetMonitorFrequency1(LPDIRECTDRAW, LPDWORD);
HRESULT WINAPI extGetMonitorFrequency2(LPDIRECTDRAW, LPDWORD);
HRESULT WINAPI extGetMonitorFrequency3(LPDIRECTDRAW, LPDWORD);
HRESULT WINAPI extGetMonitorFrequency4(LPDIRECTDRAW, LPDWORD);
HRESULT WINAPI extGetMonitorFrequency7(LPDIRECTDRAW, LPDWORD);
HRESULT WINAPI extGetVerticalBlankStatus1(LPDIRECTDRAW, LPBOOL);
HRESULT WINAPI extGetVerticalBlankStatus2(LPDIRECTDRAW, LPBOOL);
HRESULT WINAPI extGetVerticalBlankStatus3(LPDIRECTDRAW, LPBOOL);
HRESULT WINAPI extGetVerticalBlankStatus4(LPDIRECTDRAW, LPBOOL);
HRESULT WINAPI extGetVerticalBlankStatus7(LPDIRECTDRAW, LPBOOL);
HRESULT WINAPI extRestoreAllSurfaces4(LPDIRECTDRAW);
HRESULT WINAPI extRestoreAllSurfaces7(LPDIRECTDRAW);
HRESULT WINAPI extGetDeviceIdentifier4(LPDIRECTDRAW, LPDDDEVICEIDENTIFIER, DWORD);
HRESULT WINAPI extGetDeviceIdentifier7(LPDIRECTDRAW, LPDDDEVICEIDENTIFIER, DWORD);
#endif

// DirectDrawSurface
HRESULT WINAPI extQueryInterfaceS1(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceS2(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceS3(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceS4(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceS7(void *, REFIID, LPVOID *);
HRESULT WINAPI extReleaseS1(LPDIRECTDRAWSURFACE);
HRESULT WINAPI extReleaseS2(LPDIRECTDRAWSURFACE);
HRESULT WINAPI extReleaseS3(LPDIRECTDRAWSURFACE);
HRESULT WINAPI extReleaseS4(LPDIRECTDRAWSURFACE);
HRESULT WINAPI extReleaseS7(LPDIRECTDRAWSURFACE);

/*** IDirectDrawSurface methods ***/
HRESULT WINAPI extAddAttachedSurface1(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extAddAttachedSurface2(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extAddAttachedSurface3(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extAddAttachedSurface4(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extAddAttachedSurface7(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extBlt1(LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
HRESULT WINAPI extBlt2(LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
HRESULT WINAPI extBlt3(LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
HRESULT WINAPI extBlt4(LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
HRESULT WINAPI extBlt7(LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
HRESULT WINAPI extBltBatch1(LPDIRECTDRAWSURFACE, LPDDBLTBATCH, DWORD, DWORD);
HRESULT WINAPI extBltBatch2(LPDIRECTDRAWSURFACE, LPDDBLTBATCH, DWORD, DWORD);
HRESULT WINAPI extBltBatch3(LPDIRECTDRAWSURFACE, LPDDBLTBATCH, DWORD, DWORD);
HRESULT WINAPI extBltBatch4(LPDIRECTDRAWSURFACE, LPDDBLTBATCH, DWORD, DWORD);
HRESULT WINAPI extBltBatch7(LPDIRECTDRAWSURFACE, LPDDBLTBATCH, DWORD, DWORD);
HRESULT WINAPI extBltFast1(LPDIRECTDRAWSURFACE, DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD);
HRESULT WINAPI extBltFast2(LPDIRECTDRAWSURFACE, DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD);
HRESULT WINAPI extBltFast3(LPDIRECTDRAWSURFACE, DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD);
HRESULT WINAPI extBltFast4(LPDIRECTDRAWSURFACE, DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD);
HRESULT WINAPI extBltFast7(LPDIRECTDRAWSURFACE, DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD);
HRESULT WINAPI extDeleteAttachedSurface1(LPDIRECTDRAWSURFACE,  DWORD, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extDeleteAttachedSurface2(LPDIRECTDRAWSURFACE,  DWORD, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extDeleteAttachedSurface3(LPDIRECTDRAWSURFACE,  DWORD, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extDeleteAttachedSurface4(LPDIRECTDRAWSURFACE,  DWORD, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extDeleteAttachedSurface7(LPDIRECTDRAWSURFACE,  DWORD, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extEnumAttachedSurfaces1(LPDIRECTDRAWSURFACE, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extEnumAttachedSurfaces2(LPDIRECTDRAWSURFACE, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extEnumAttachedSurfaces3(LPDIRECTDRAWSURFACE, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extEnumAttachedSurfaces4(LPDIRECTDRAWSURFACE, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extEnumAttachedSurfaces7(LPDIRECTDRAWSURFACE, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extFlip1(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extFlip2(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extFlip3(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extFlip4(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extFlip7(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extGetAttachedSurface1(LPDIRECTDRAWSURFACE, LPDDSCAPS, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extGetAttachedSurface2(LPDIRECTDRAWSURFACE, LPDDSCAPS, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extGetAttachedSurface3(LPDIRECTDRAWSURFACE, LPDDSCAPS, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extGetAttachedSurface4(LPDIRECTDRAWSURFACE, LPDDSCAPS2, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extGetAttachedSurface7(LPDIRECTDRAWSURFACE, LPDDSCAPS2, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extGetBltStatus1(LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extGetBltStatus2(LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extGetBltStatus3(LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extGetBltStatus4(LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extGetBltStatus7(LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extGetCaps1S(LPDIRECTDRAWSURFACE, LPDDSCAPS);
HRESULT WINAPI extGetCaps2S(LPDIRECTDRAWSURFACE, LPDDSCAPS);
HRESULT WINAPI extGetCaps3S(LPDIRECTDRAWSURFACE, LPDDSCAPS);
HRESULT WINAPI extGetCaps4S(LPDIRECTDRAWSURFACE, LPDDSCAPS2);
HRESULT WINAPI extGetCaps7S(LPDIRECTDRAWSURFACE, LPDDSCAPS2);
HRESULT WINAPI extGetClipper1(LPDIRECTDRAWSURFACE, LPDIRECTDRAWCLIPPER *);
HRESULT WINAPI extGetClipper2(LPDIRECTDRAWSURFACE, LPDIRECTDRAWCLIPPER *);
HRESULT WINAPI extGetClipper3(LPDIRECTDRAWSURFACE, LPDIRECTDRAWCLIPPER *);
HRESULT WINAPI extGetClipper4(LPDIRECTDRAWSURFACE, LPDIRECTDRAWCLIPPER *);
HRESULT WINAPI extGetClipper7(LPDIRECTDRAWSURFACE, LPDIRECTDRAWCLIPPER *);
HRESULT WINAPI extGetColorKey1(LPDIRECTDRAWSURFACE, DWORD, LPDDCOLORKEY);
HRESULT WINAPI extGetColorKey2(LPDIRECTDRAWSURFACE, DWORD, LPDDCOLORKEY);
HRESULT WINAPI extGetColorKey3(LPDIRECTDRAWSURFACE, DWORD, LPDDCOLORKEY);
HRESULT WINAPI extGetColorKey4(LPDIRECTDRAWSURFACE, DWORD, LPDDCOLORKEY);
HRESULT WINAPI extGetColorKey7(LPDIRECTDRAWSURFACE, DWORD, LPDDCOLORKEY);
HRESULT WINAPI extGetDC1(LPDIRECTDRAWSURFACE, HDC FAR *);
HRESULT WINAPI extGetDC2(LPDIRECTDRAWSURFACE, HDC FAR *);
HRESULT WINAPI extGetDC3(LPDIRECTDRAWSURFACE, HDC FAR *);
HRESULT WINAPI extGetDC4(LPDIRECTDRAWSURFACE, HDC FAR *);
HRESULT WINAPI extGetDC7(LPDIRECTDRAWSURFACE, HDC FAR *);
HRESULT WINAPI extGetFlipStatus1(LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extGetFlipStatus2(LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extGetFlipStatus3(LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extGetFlipStatus4(LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extGetFlipStatus7(LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extGetPalette1(LPDIRECTDRAWSURFACE, LPDIRECTDRAWPALETTE *);
HRESULT WINAPI extGetPalette2(LPDIRECTDRAWSURFACE, LPDIRECTDRAWPALETTE *);
HRESULT WINAPI extGetPalette3(LPDIRECTDRAWSURFACE, LPDIRECTDRAWPALETTE *);
HRESULT WINAPI extGetPalette4(LPDIRECTDRAWSURFACE, LPDIRECTDRAWPALETTE *);
HRESULT WINAPI extGetPalette7(LPDIRECTDRAWSURFACE, LPDIRECTDRAWPALETTE *);
HRESULT WINAPI extGetPixelFormat1(LPDIRECTDRAWSURFACE, LPDDPIXELFORMAT);
HRESULT WINAPI extGetPixelFormat2(LPDIRECTDRAWSURFACE, LPDDPIXELFORMAT);
HRESULT WINAPI extGetPixelFormat3(LPDIRECTDRAWSURFACE, LPDDPIXELFORMAT);
HRESULT WINAPI extGetPixelFormat4(LPDIRECTDRAWSURFACE, LPDDPIXELFORMAT);
HRESULT WINAPI extGetPixelFormat7(LPDIRECTDRAWSURFACE, LPDDPIXELFORMAT);
HRESULT WINAPI extGetSurfaceDesc1(LPDIRECTDRAWSURFACE lpdds, LPDDSURFACEDESC lpddsd);
HRESULT WINAPI extGetSurfaceDesc2(LPDIRECTDRAWSURFACE lpdds, LPDDSURFACEDESC lpddsd);
HRESULT WINAPI extGetSurfaceDesc3(LPDIRECTDRAWSURFACE lpdds, LPDDSURFACEDESC lpddsd);
HRESULT WINAPI extGetSurfaceDesc4(LPDIRECTDRAWSURFACE2 lpdds, LPDDSURFACEDESC2 lpddsd);
HRESULT WINAPI extGetSurfaceDesc7(LPDIRECTDRAWSURFACE2 lpdds, LPDDSURFACEDESC2 lpddsd);
HRESULT WINAPI extLock1(LPDIRECTDRAWSURFACE, LPRECT, LPDDSURFACEDESC, DWORD, HANDLE);
HRESULT WINAPI extLock2(LPDIRECTDRAWSURFACE, LPRECT, LPDDSURFACEDESC, DWORD, HANDLE);
HRESULT WINAPI extLock3(LPDIRECTDRAWSURFACE, LPRECT, LPDDSURFACEDESC, DWORD, HANDLE);
HRESULT WINAPI extLock4(LPDIRECTDRAWSURFACE, LPRECT, LPDDSURFACEDESC, DWORD, HANDLE);
HRESULT WINAPI extLock7(LPDIRECTDRAWSURFACE, LPRECT, LPDDSURFACEDESC, DWORD, HANDLE);
HRESULT WINAPI extReleaseDC1(LPDIRECTDRAWSURFACE, HDC);
HRESULT WINAPI extReleaseDC2(LPDIRECTDRAWSURFACE, HDC);
HRESULT WINAPI extReleaseDC3(LPDIRECTDRAWSURFACE, HDC);
HRESULT WINAPI extReleaseDC4(LPDIRECTDRAWSURFACE, HDC);
HRESULT WINAPI extReleaseDC7(LPDIRECTDRAWSURFACE, HDC);
HRESULT WINAPI extSetClipper1(LPDIRECTDRAWSURFACE, LPDIRECTDRAWCLIPPER);
HRESULT WINAPI extSetClipper2(LPDIRECTDRAWSURFACE, LPDIRECTDRAWCLIPPER);
HRESULT WINAPI extSetClipper3(LPDIRECTDRAWSURFACE, LPDIRECTDRAWCLIPPER);
HRESULT WINAPI extSetClipper4(LPDIRECTDRAWSURFACE, LPDIRECTDRAWCLIPPER);
HRESULT WINAPI extSetClipper7(LPDIRECTDRAWSURFACE, LPDIRECTDRAWCLIPPER);
HRESULT WINAPI extSetColorKey1(LPDIRECTDRAWSURFACE, DWORD, LPDDCOLORKEY);
HRESULT WINAPI extSetColorKey2(LPDIRECTDRAWSURFACE, DWORD, LPDDCOLORKEY);
HRESULT WINAPI extSetColorKey3(LPDIRECTDRAWSURFACE, DWORD, LPDDCOLORKEY);
HRESULT WINAPI extSetColorKey4(LPDIRECTDRAWSURFACE, DWORD, LPDDCOLORKEY);
HRESULT WINAPI extSetColorKey7(LPDIRECTDRAWSURFACE, DWORD, LPDDCOLORKEY);
HRESULT WINAPI extSetPalette1(LPDIRECTDRAWSURFACE, LPDIRECTDRAWPALETTE);
HRESULT WINAPI extSetPalette2(LPDIRECTDRAWSURFACE, LPDIRECTDRAWPALETTE);
HRESULT WINAPI extSetPalette3(LPDIRECTDRAWSURFACE, LPDIRECTDRAWPALETTE);
HRESULT WINAPI extSetPalette4(LPDIRECTDRAWSURFACE, LPDIRECTDRAWPALETTE);
HRESULT WINAPI extSetPalette7(LPDIRECTDRAWSURFACE, LPDIRECTDRAWPALETTE);
HRESULT WINAPI extUnlock1(LPDIRECTDRAWSURFACE, LPVOID);
HRESULT WINAPI extUnlock2(LPDIRECTDRAWSURFACE, LPVOID);
HRESULT WINAPI extUnlock3(LPDIRECTDRAWSURFACE, LPVOID);
HRESULT WINAPI extUnlock4(LPDIRECTDRAWSURFACE, LPRECT);
HRESULT WINAPI extUnlock7(LPDIRECTDRAWSURFACE, LPRECT);
HRESULT WINAPI extEnumOverlayZOrders1(LPDIRECTDRAWSURFACE, DWORD, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extEnumOverlayZOrders2(LPDIRECTDRAWSURFACE, DWORD, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extEnumOverlayZOrders3(LPDIRECTDRAWSURFACE, DWORD, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extEnumOverlayZOrders4(LPDIRECTDRAWSURFACE, DWORD, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extEnumOverlayZOrders7(LPDIRECTDRAWSURFACE, DWORD, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extAddOverlayDirtyRect1(LPDIRECTDRAWSURFACE, LPRECT);
HRESULT WINAPI extAddOverlayDirtyRect2(LPDIRECTDRAWSURFACE, LPRECT);
HRESULT WINAPI extAddOverlayDirtyRect3(LPDIRECTDRAWSURFACE, LPRECT);
HRESULT WINAPI extAddOverlayDirtyRect4(LPDIRECTDRAWSURFACE, LPRECT);
HRESULT WINAPI extAddOverlayDirtyRect7(LPDIRECTDRAWSURFACE, LPRECT);
HRESULT WINAPI extGetOverlayPosition1(LPDIRECTDRAWSURFACE, LPLONG, LPLONG);
HRESULT WINAPI extGetOverlayPosition2(LPDIRECTDRAWSURFACE, LPLONG, LPLONG);
HRESULT WINAPI extGetOverlayPosition3(LPDIRECTDRAWSURFACE, LPLONG, LPLONG);
HRESULT WINAPI extGetOverlayPosition4(LPDIRECTDRAWSURFACE, LPLONG, LPLONG);
HRESULT WINAPI extGetOverlayPosition7(LPDIRECTDRAWSURFACE, LPLONG, LPLONG);
HRESULT WINAPI extSetOverlayPosition1(LPDIRECTDRAWSURFACE, LONG, LONG);
HRESULT WINAPI extSetOverlayPosition2(LPDIRECTDRAWSURFACE, LONG, LONG);
HRESULT WINAPI extSetOverlayPosition3(LPDIRECTDRAWSURFACE, LONG, LONG);
HRESULT WINAPI extSetOverlayPosition4(LPDIRECTDRAWSURFACE, LONG, LONG);
HRESULT WINAPI extSetOverlayPosition7(LPDIRECTDRAWSURFACE, LONG, LONG);

extern HRESULT WINAPI extCreateSurface(int, CreateSurface_Type, LPDIRECTDRAW, DDSURFACEDESC2 *, LPDIRECTDRAWSURFACE *, void *);
HRESULT WINAPI extSetSurfaceDesc3(LPDIRECTDRAWSURFACE, LPDDSURFACEDESC, DWORD);
HRESULT WINAPI extSetSurfaceDesc4(LPDIRECTDRAWSURFACE, LPDDSURFACEDESC, DWORD);
HRESULT WINAPI extSetSurfaceDesc7(LPDIRECTDRAWSURFACE, LPDDSURFACEDESC, DWORD);

// DirectDrawClipper
HRESULT WINAPI extReleaseC(LPDIRECTDRAWCLIPPER);
HRESULT WINAPI extGetClipList(LPDIRECTDRAWCLIPPER, LPRECT, LPRGNDATA, LPDWORD);
HRESULT WINAPI extSetClipList(LPDIRECTDRAWCLIPPER, LPRGNDATA, DWORD);
HRESULT WINAPI extSetHWnd(LPDIRECTDRAWCLIPPER, DWORD, HWND);

// DirectDrawPalette
HRESULT WINAPI extReleaseP(LPDIRECTDRAWPALETTE);
//    STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW, DWORD, LPPALETTEENTRY) PURE;
HRESULT WINAPI extSetEntries(LPDIRECTDRAWPALETTE, DWORD, DWORD, DWORD, LPPALETTEENTRY);

// GammaRamp
HRESULT WINAPI extDDSetGammaRamp(LPDIRECTDRAWSURFACE, DWORD, LPDDGAMMARAMP);
HRESULT WINAPI extDDGetGammaRamp(LPDIRECTDRAWSURFACE, DWORD, LPDDGAMMARAMP);


HDC WINAPI extGDIGetDC(HWND);
HDC WINAPI extGDIGetWindowDC(HWND);
int WINAPI extGDIReleaseDC(HWND, HDC);

/* DirectDraw APIs */
DirectDrawCreate_Type pDirectDrawCreate = NULL;
DirectDrawCreateEx_Type pDirectDrawCreateEx = NULL;
DirectDrawEnumerateA_Type pDirectDrawEnumerateA = NULL;
DirectDrawEnumerateW_Type pDirectDrawEnumerateW = NULL;
DirectDrawEnumerateExA_Type pDirectDrawEnumerateExA = NULL;
DirectDrawEnumerateExW_Type pDirectDrawEnumerateExW = NULL;
DirectDrawCreateClipper_Type pDirectDrawCreateClipper = NULL;
HandleDDThreadLock_Type pAcquireDDThreadLock = NULL;
HandleDDThreadLock_Type pReleaseDDThreadLock = NULL;
StartModeTest_Type pStartModeTest = NULL;
EvaluateMode_Type pEvaluateMode = NULL;
DDGetSurfaceFromDC_Type pDDGetSurfaceFromDC = NULL;

/* DirectDraw hook pointers */
QueryInterface_Type pQueryInterfaceD1, pQueryInterfaceD2, pQueryInterfaceD3, pQueryInterfaceD4, pQueryInterfaceD7;
AddRefD_Type pAddRefD;
ReleaseD_Type pReleaseD1, pReleaseD2, pReleaseD3, pReleaseD4, pReleaseD7;
Compact_Type pCompact1, pCompact2, pCompact3, pCompact4, pCompact7;
CreateClipper_Type pCreateClipper1, pCreateClipper2, pCreateClipper3, pCreateClipper4, pCreateClipper7;
CreatePalette_Type pCreatePalette1, pCreatePalette2, pCreatePalette3, pCreatePalette4, pCreatePalette7;
CreateSurface1_Type pCreateSurface1, pCreateSurface2, pCreateSurface3;
CreateSurface2_Type pCreateSurface4, pCreateSurface7;
DuplicateSurface_Type pDuplicateSurface1, pDuplicateSurface2, pDuplicateSurface3;
DuplicateSurface4_Type pDuplicateSurface4;
DuplicateSurface7_Type pDuplicateSurface7;
EnumDisplayModes1_Type pEnumDisplayModes1, pEnumDisplayModes2, pEnumDisplayModes3;
EnumDisplayModes4_Type pEnumDisplayModes4, pEnumDisplayModes7;
EnumSurfaces1_Type pEnumSurfaces1, pEnumSurfaces2, pEnumSurfaces3;
EnumSurfaces4_Type pEnumSurfaces4;
EnumSurfaces7_Type pEnumSurfaces7;
FlipToGDISurface_Type pFlipToGDISurface1, pFlipToGDISurface2, pFlipToGDISurface3, pFlipToGDISurface4, pFlipToGDISurface7;
GetCapsD_Type pGetCaps1D, pGetCaps2D, pGetCaps3D, pGetCaps4D, pGetCaps7D;
GetDisplayMode_Type pGetDisplayMode1, pGetDisplayMode2, pGetDisplayMode3;
GetDisplayMode4_Type pGetDisplayMode4, pGetDisplayMode7;
GetFourCCCodes_Type pGetFourCCCodes1, pGetFourCCCodes2, pGetFourCCCodes3, pGetFourCCCodes4, pGetFourCCCodes7;
GetGDISurface_Type pGetGDISurface1, pGetGDISurface2, pGetGDISurface3, pGetGDISurface4, pGetGDISurface7;
GetMonitorFrequency_Type pGetMonitorFrequency;
GetScanLine_Type pGetScanLine;
GetVerticalBlankStatus_Type pGetVerticalBlankStatus;
Initialize_Type pInitialize1, pInitialize2, pInitialize3, pInitialize4, pInitialize7;
RestoreDisplayMode_Type pRestoreDisplayMode;
SetCooperativeLevel_Type pSetCooperativeLevel1, pSetCooperativeLevel2, pSetCooperativeLevel3, pSetCooperativeLevel4, pSetCooperativeLevel7;
SetDisplayMode1_Type pSetDisplayMode1;
SetDisplayMode2_Type pSetDisplayMode2, pSetDisplayMode3, pSetDisplayMode4, pSetDisplayMode7;
WaitForVerticalBlank_Type pWaitForVerticalBlank1, pWaitForVerticalBlank2, pWaitForVerticalBlank3, pWaitForVerticalBlank4, pWaitForVerticalBlank7;
GetSurfaceFromDC_Type pGetSurfaceFromDC;
GetAvailableVidMem_Type pGetAvailableVidMem2, pGetAvailableVidMem3;
GetAvailableVidMem4_Type pGetAvailableVidMem4, pGetAvailableVidMem7;
TestCooperativeLevel_Type pTestCooperativeLevel4, pTestCooperativeLevel7;
GetDeviceIdentifier_Type pGetDeviceIdentifier;
RestoreDisplayMode_Type pRestoreDisplayMode1, pRestoreDisplayMode2, pRestoreDisplayMode3, pRestoreDisplayMode4, pRestoreDisplayMode7;
GetScanLine_Type pGetScanLine1, pGetScanLine2, pGetScanLine3, pGetScanLine4, pGetScanLine7;
GetSurfaceFromDC_Type pGetSurfaceFromDC4, pGetSurfaceFromDC7;
#ifdef TRACEALL
GetMonitorFrequency_Type pGetMonitorFrequency1, pGetMonitorFrequency2, pGetMonitorFrequency3, pGetMonitorFrequency4, pGetMonitorFrequency7;
GetVerticalBlankStatus_Type pGetVerticalBlankStatus1, pGetVerticalBlankStatus2, pGetVerticalBlankStatus3, pGetVerticalBlankStatus4, pGetVerticalBlankStatus7;
RestoreAllSurfaces_Type pRestoreAllSurfaces4, pRestoreAllSurfaces7;
GetDeviceIdentifier_Type pGetDeviceIdentifier4, pGetDeviceIdentifier7;
#endif

/* DirectDrawSurface hook pointers */
QueryInterface_Type pQueryInterfaceS1, pQueryInterfaceS2, pQueryInterfaceS3, pQueryInterfaceS4, pQueryInterfaceS7;
ReleaseS_Type pReleaseS1, pReleaseS2, pReleaseS3, pReleaseS4, pReleaseS7;
AddAttachedSurface_Type pAddAttachedSurface1, pAddAttachedSurface2, pAddAttachedSurface3, pAddAttachedSurface4, pAddAttachedSurface7;
AddOverlayDirtyRect_Type pAddOverlayDirtyRect;
Blt_Type pBlt1, pBlt2, pBlt3, pBlt4, pBlt7;
BltBatch_Type pBltBatch1, pBltBatch2, pBltBatch3, pBltBatch4, pBltBatch7;
BltFast_Type pBltFast1, pBltFast2, pBltFast3, pBltFast4, pBltFast7;
DeleteAttachedSurface_Type pDeleteAttachedSurface1, pDeleteAttachedSurface2, pDeleteAttachedSurface3, pDeleteAttachedSurface4, pDeleteAttachedSurface7;
EnumAttachedSurfaces_Type pEnumAttachedSurfaces1, pEnumAttachedSurfaces2, pEnumAttachedSurfaces3, pEnumAttachedSurfaces4, pEnumAttachedSurfaces7;
EnumOverlayZOrders_Type pEnumOverlayZOrders;
Flip_Type pFlip1, pFlip2, pFlip3, pFlip4, pFlip7;
GetAttachedSurface_Type pGetAttachedSurface1, pGetAttachedSurface2, pGetAttachedSurface3, pGetAttachedSurface4, pGetAttachedSurface7;
#ifdef TRACEALL
GetBltStatus_Type pGetBltStatus1, pGetBltStatus2, pGetBltStatus3, pGetBltStatus4, pGetBltStatus7;
#endif
GetCapsS_Type pGetCaps1S, pGetCaps2S, pGetCaps3S, pGetCaps4S, pGetCaps7S;
GetColorKey_Type pGetColorKey1, pGetColorKey2, pGetColorKey3, pGetColorKey4, pGetColorKey7;
GetDC_Type pGetDC1, pGetDC2, pGetDC3, pGetDC4, pGetDC7;
#ifdef TRACEALL
GetFlipStatus_Type pGetFlipStatus1, pGetFlipStatus2, pGetFlipStatus3, pGetFlipStatus4, pGetFlipStatus7;
#endif
GetOverlayPosition_Type pGetOverlayPosition1, pGetOverlayPosition2, pGetOverlayPosition3, pGetOverlayPosition4, pGetOverlayPosition7;
SetOverlayPosition_Type pSetOverlayPosition1, pSetOverlayPosition2, pSetOverlayPosition3, pSetOverlayPosition4, pSetOverlayPosition7;
GetPalette_Type pGetPalette1, pGetPalette2, pGetPalette3, pGetPalette4, pGetPalette7;
GetPixelFormat_Type pGetPixelFormat1, pGetPixelFormat2, pGetPixelFormat3, pGetPixelFormat4, pGetPixelFormat7;
GetClipper_Type pGetClipper1, pGetClipper2, pGetClipper3, pGetClipper4, pGetClipper7;
GetSurfaceDesc_Type pGetSurfaceDesc1, pGetSurfaceDesc2, pGetSurfaceDesc3;
GetSurfaceDesc2_Type pGetSurfaceDesc4, pGetSurfaceDesc7;
Lock_Type pLock1, pLock2, pLock3, pLock4, pLock7;
EnumOverlayZOrders_Type pEnumOverlayZOrders1, pEnumOverlayZOrders2, pEnumOverlayZOrders3, pEnumOverlayZOrders4, pEnumOverlayZOrders7;
AddOverlayDirtyRect_Type pAddOverlayDirtyRect1, pAddOverlayDirtyRect2, pAddOverlayDirtyRect3, pAddOverlayDirtyRect4, pAddOverlayDirtyRect7;

//Initialize
IsLost_Type pIsLost;
ReleaseDC_Type pReleaseDC1, pReleaseDC2, pReleaseDC3, pReleaseDC4, pReleaseDC7;
Restore_Type pRestore;
SetClipper_Type pSetClipper1, pSetClipper2, pSetClipper3, pSetClipper4, pSetClipper7;
SetColorKey_Type pSetColorKey1, pSetColorKey2, pSetColorKey3, pSetColorKey4, pSetColorKey7;
SetOverlayPosition_Type pSetOverlayPosition;
SetPalette_Type pSetPalette1, pSetPalette2, pSetPalette3, pSetPalette4, pSetPalette7;
Unlock1_Type pUnlock1, pUnlock2, pUnlock3;
Unlock4_Type pUnlock4, pUnlock7;
UpdateOverlay_Type pUpdateOverlay;
UpdateOverlayDisplay_Type pUpdateOverlayDisplay;
UpdateOverlayZOrder_Type pUpdateOverlayZOrder;
SetSurfaceDesc_Type pSetSurfaceDesc3, pSetSurfaceDesc4, pSetSurfaceDesc7;

/* DirectDrawClipper hook pointers */
QueryInterface_Type pQueryInterfaceC;
AddRefC_Type pAddRefC;
ReleaseC_Type pReleaseC;
GetClipList_Type pGetClipList;
SetClipList_Type pSetClipList;
GetHWnd_Type pGetHWnd;
InitializeC_Type pInitializeC;
IsClipListChanged_Type pIsClipListChanged;
SetHWnd_Type pSetHWnd;

/* DirectDrawPalette hook pointers */
QueryInterfaceP_Type pQueryInterfaceP;
AddRefP_Type pAddRefP;
ReleaseP_Type pReleaseP;
/*** IDirectDrawPalette methods ***/
GetCapsP_Type pGetCapsP;
GetEntries_Type pGetEntries;
//    STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW, DWORD, LPPALETTEENTRY) PURE;
SetEntries_Type pSetEntries;

// GammaRamp
GammaRamp_Type pDDGetGammaRamp;
GammaRamp_Type pDDSetGammaRamp;

// ddraw global variables, constants & so on

extern PALETTEENTRY DefaultSystemPalette[256];
LPDIRECTDRAWSURFACE lpDDSEmu_Prim = NULL;
LPDIRECTDRAWSURFACE lpDDSEmu_Back = NULL;
LPDIRECTDRAWSURFACE lpDDZBuffer = NULL; // BEWARE! Likely, this global is useless ....
int ZBufferSize;
// v2.1.87: lpPrimaryDD is the DIRECTDRAW object to which the primary surface and all
// the service objects (emulated backbuffer, emulater primary, ....) are attached.
LPDIRECTDRAW lpPrimaryDD = NULL;
int iPrimarySurfaceVersion;
int iDirectDrawVersion;
LPDIRECTDRAWPALETTE lpDDP = NULL;
LPDIRECTDRAWCLIPPER lpddC = NULL;
int iDDPExtraRefCounter = 0;
// v2.02.37: globals to store requested main surface capabilities
DDSURFACEDESC2 DDSD_Prim;
DWORD PaletteEntries[256];
DWORD *Palette16BPP = NULL;
void *EmuScreenBuffer = NULL; // to implement pitch bug fix
DWORD rPitch = 0;
LPVOID rSurface = NULL;
void SetPixFmt(LPDDSURFACEDESC2);
void GetPixFmt(LPDDSURFACEDESC2);

static HookEntryEx_Type ddHooks[] = {
    {HOOK_HOT_CANDIDATE, 0x08, "DirectDrawCreate", (FARPROC)NULL, (FARPROC *) &pDirectDrawCreate, (FARPROC)extDirectDrawCreate},
    {HOOK_HOT_CANDIDATE, 0x0A, "DirectDrawCreateEx", (FARPROC)NULL, (FARPROC *) &pDirectDrawCreateEx, (FARPROC)extDirectDrawCreateEx},
    {HOOK_HOT_CANDIDATE, 0x0B, "DirectDrawEnumerateA", (FARPROC)NULL, (FARPROC *) &pDirectDrawEnumerateA, (FARPROC)extDirectDrawEnumerateA},
    {HOOK_HOT_CANDIDATE, 0x0C, "DirectDrawEnumerateExA", (FARPROC)NULL, (FARPROC *) &pDirectDrawEnumerateExA, (FARPROC)extDirectDrawEnumerateExA},
    {HOOK_HOT_CANDIDATE, 0x09, "DirectDrawCreateClipper", (FARPROC)NULL, (FARPROC *) &pDirectDrawCreateClipper, (FARPROC)extDirectDrawCreateClipper},
    {HOOK_HOT_CANDIDATE, 0x01, "AcquireDDThreadLock", (FARPROC)NULL, (FARPROC *) &pAcquireDDThreadLock, (FARPROC)NULL},
    {HOOK_HOT_CANDIDATE, 0x15, "ReleaseDDThreadLock", (FARPROC)NULL, (FARPROC *) &pReleaseDDThreadLock, (FARPROC)NULL},
    {HOOK_IAT_CANDIDATE, 0x0E, "DirectDrawEnumerateW", (FARPROC)NULL, (FARPROC *) &pDirectDrawEnumerateW, (FARPROC)extDirectDrawCreate},
    {HOOK_IAT_CANDIDATE, 0x0D, "DirectDrawEnumerateExW", (FARPROC)NULL, (FARPROC *) &pDirectDrawEnumerateExW, (FARPROC)extDirectDrawCreate},
    {HOOK_IAT_CANDIDATE, 0x0D, "GetSurfaceFromDC", (FARPROC)NULL, (FARPROC *) &pDDGetSurfaceFromDC, (FARPROC)extDDGetSurfaceFromDC},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

FARPROC Remap_ddraw_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if (dxw.dwTargetDDVersion == HOOKDDRAWNONE) return NULL;
    if (addr = RemapLibraryEx(proc, hModule, ddHooks)) return addr;
    return NULL;
}

extern HRESULT WINAPI sBlt(int, Blt_Type pBlt, char *, LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX, BOOL);

/* ------------------------------------------------------------------------------ */
// auxiliary (static) functions
/* ------------------------------------------------------------------------------ */

DWORD gdwRefreshRate;
#define MAXREFRESHDELAYCOUNT 20
int iRefreshDelays[MAXREFRESHDELAYCOUNT] = {16, 17};
int iRefreshDelayCount = 2;

static HRESULT myGetDisplayMode(int dxversion, LPDIRECTDRAW lpdd, LPDDSURFACEDESC lpdds) {
    HRESULT res;
    switch(dxversion) {
    default:
    case 1:
        res = (*pGetDisplayMode1)(lpdd, lpdds);
        break;
    case 2:
        res = (*pGetDisplayMode2)(lpdd, lpdds);
        break;
    case 3:
        res = (*pGetDisplayMode3)(lpdd, lpdds);
        break;
    case 4:
        res = (*pGetDisplayMode4)(lpdd, (LPDDSURFACEDESC2)lpdds);
        break;
    case 7:
        res = (*pGetDisplayMode7)(lpdd, (LPDDSURFACEDESC2)lpdds);
        break;
    }
    return res;
}

void SetVSyncDelays(int dxversion, LPDIRECTDRAW lpdd) {
    DDSURFACEDESC2 ddsdRefreshRate;
    HRESULT res;
    memset(&ddsdRefreshRate, 0, sizeof(ddsdRefreshRate));
    ddsdRefreshRate.dwSize = sizeof(DDSURFACEDESC);
    res = myGetDisplayMode(dxversion, lpdd, (LPDDSURFACEDESC)&ddsdRefreshRate);
    if(res) return;
    dxw.SetVSyncDelays(ddsdRefreshRate.dwRefreshRate);
}

static DWORD GetBltWaitFlags(DWORD dwBltFlags) {
    if(dxw.dwFlags8 & FORCEWAIT) {
        dwBltFlags |= DDBLT_WAIT;
        dwBltFlags &= ~DDBLT_DONOTWAIT;
    }
    if(dxw.dwFlags8 & FORCENOWAIT) {
        dwBltFlags |= DDBLT_DONOTWAIT;
        dwBltFlags &= ~DDBLT_WAIT;
    }
    return dwBltFlags;
}

static DWORD GetFlipWaitFlags(DWORD dwFlipFlags) {
    if(dxw.dwFlags8 & FORCEWAIT) {
        dwFlipFlags |= DDFLIP_WAIT;
        dwFlipFlags &= ~DDFLIP_DONOTWAIT;
    }
    if(dxw.dwFlags8 & FORCENOWAIT) {
        dwFlipFlags |= DDFLIP_DONOTWAIT;
        dwFlipFlags &= ~DDFLIP_WAIT;
    }
    return dwFlipFlags;
}

static DWORD GetBltFastWaitFlags(DWORD dwBltFastFlags) {
    if(dxw.dwFlags8 & FORCEWAIT) {
        dwBltFastFlags |= DDBLTFAST_WAIT;
        dwBltFastFlags &= ~DDBLTFAST_DONOTWAIT;
    }
    if(dxw.dwFlags8 & FORCENOWAIT) {
        dwBltFastFlags |= DDBLTFAST_DONOTWAIT;
        dwBltFastFlags &= ~DDBLTFAST_WAIT;
    }
    return dwBltFastFlags;
}


static DWORD SetPrimaryCaps(DWORD dwCaps) {
    dwCaps |= (DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_FRONTBUFFER | DDSCAPS_VIDEOMEMORY | DDSCAPS_VISIBLE); // primary surfaces must be this way
    dwCaps &= ~(DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN); // primary surfaces can't be this way
    return dwCaps;
}

static DWORD SetBackBufferCaps(DWORD dwCaps) {
    // v2.03.11: added DDSCAPS_FLIP capability to backbuffer surface: "Ignition" checks for it before Flip-ping to primary
    dwCaps |= (DDSCAPS_BACKBUFFER | DDSCAPS_VIDEOMEMORY | DDSCAPS_FLIP | DDSCAPS_LOCALVIDMEM); // you never know....
    dwCaps &= ~(DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN); // backbuffer surfaces can't be this way
    return dwCaps;
}

static DWORD SetZBufferCaps(DWORD dwCaps) {
    // beware! the ZBUFFER surface could have never been registered!
    // in this case better adapt to the primary/backbuffer ones, since a ZBUFFER with no good caps won't work.
    // input caps not null or simply DDSCAPS_ZBUFFER, supposedly good, don't touch ....
    if(dwCaps & ~DDSCAPS_ZBUFFER) return dwCaps;
    // ... else try to cope with primary surface caps
    dwCaps = dxwss.GetCaps(dxwss.GetPrimarySurface());
    dwCaps &= ~(DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_BACKBUFFER | DDSCAPS_3DDEVICE | DDSCAPS_COMPLEX);
    dwCaps |= DDSCAPS_ZBUFFER;
    return dwCaps;
}

static void Stopper(char *s, int line) {
    char sMsg[81];
    sprintf(sMsg, "break: \"%s\"", s);
    MessageBox(0, sMsg, "break", MB_OK | MB_ICONEXCLAMATION);
}

//#define STOPPER_TEST // comment out to eliminate
#ifdef STOPPER_TEST
#define STOPPER(s) Stopper(s, __LINE__)
#else
#define STOPPER(s)
#endif

#ifndef DXW_NOTRACES

char *DumpPixelFormat(LPDDSURFACEDESC2 lpddsd) {
    return ExplainPixelFormat(&(lpddsd->ddpfPixelFormat));
}

CHAR *LogSurfaceAttributes(LPDDSURFACEDESC2 lpddsd, char *label, int line) {
    static char sInfo[1024];
    sprintf(sInfo, "SurfaceDesc: %s Flags=%#x(%s)",
            label,
            lpddsd->dwFlags, ExplainFlags(lpddsd->dwFlags));
    if (lpddsd->dwFlags & DDSD_BACKBUFFERCOUNT) sprintf(sInfo, "%s BackBufferCount=%d", sInfo, lpddsd->dwBackBufferCount);
    if (lpddsd->dwFlags & DDSD_WIDTH) sprintf(sInfo, "%s Width=%d", sInfo, lpddsd->dwWidth);
    if (lpddsd->dwFlags & DDSD_HEIGHT) sprintf(sInfo, "%s Height=%d", sInfo, lpddsd->dwHeight);
    if (lpddsd->dwFlags & DDSD_PITCH) sprintf(sInfo, "%s Pitch=%d", sInfo, lpddsd->lPitch);
    if (lpddsd->dwFlags & DDSD_MIPMAPCOUNT) sprintf(sInfo, "%s MipMapCount=%d", sInfo, lpddsd->dwMipMapCount);
    if (lpddsd->dwFlags & DDSD_CAPS) {
        sprintf(sInfo, "%s Caps=%#x(%s)", sInfo, lpddsd->ddsCaps.dwCaps, ExplainDDSCaps(lpddsd->ddsCaps.dwCaps));
        if(lpddsd->dwSize == sizeof(DDSURFACEDESC2)) {
            LPDDSURFACEDESC2 lpddsd2 = lpddsd;
            sprintf(sInfo, "%s Caps2=%#x(%s)", sInfo, lpddsd2->ddsCaps.dwCaps2, ExplainDDSCaps2(lpddsd2->ddsCaps.dwCaps2));
            sprintf(sInfo, "%s Caps3=%#x(%s)", sInfo, lpddsd2->ddsCaps.dwCaps3, ExplainDDSCaps3(lpddsd2->ddsCaps.dwCaps3));
            sprintf(sInfo, "%s Caps4=%#x(%s)", sInfo, lpddsd2->ddsCaps.dwCaps4, ExplainDDSCaps4(lpddsd2->ddsCaps.dwCaps4));
        }
    }
    if (lpddsd->dwFlags & DDSD_CKDESTBLT ) sprintf(sInfo, "%s CKDestBlt=(%#x,%#x)", sInfo, lpddsd->ddckCKDestBlt.dwColorSpaceLowValue, lpddsd->ddckCKDestBlt.dwColorSpaceHighValue);
    if (lpddsd->dwFlags & DDSD_CKDESTOVERLAY ) sprintf(sInfo, "%s CKDestOverlay=(%#x,%#x)", sInfo, lpddsd->ddckCKDestOverlay.dwColorSpaceLowValue, lpddsd->ddckCKDestOverlay.dwColorSpaceHighValue);
    if (lpddsd->dwFlags & DDSD_CKSRCBLT ) sprintf(sInfo, "%s CKSrcBlt=(%#x,%#x)", sInfo, lpddsd->ddckCKSrcBlt.dwColorSpaceLowValue, lpddsd->ddckCKSrcBlt.dwColorSpaceHighValue);
    if (lpddsd->dwFlags & DDSD_CKSRCOVERLAY ) sprintf(sInfo, "%s CKSrcOverlay=(%#x,%#x)", sInfo, lpddsd->ddckCKSrcOverlay.dwColorSpaceLowValue, lpddsd->ddckCKSrcOverlay.dwColorSpaceHighValue);
    if (lpddsd->dwFlags & DDSD_PIXELFORMAT ) sprintf(sInfo, "%s %s", sInfo, DumpPixelFormat((LPDDSURFACEDESC2)lpddsd));
    if (lpddsd->dwFlags & DDSD_LPSURFACE) sprintf(sInfo, "%s Surface=%#x", sInfo, lpddsd->lpSurface);
    if ((lpddsd->dwFlags & DDSD_ZBUFFERBITDEPTH) && (lpddsd->dwSize == sizeof(DDSURFACEDESC))) sprintf(sInfo, "%s ZBufferBitDepth=%d", sInfo, ((LPDDSURFACEDESC)lpddsd)->dwZBufferBitDepth);
    if (lpddsd->dwFlags & DDSD_ALPHABITDEPTH) sprintf(sInfo, "%s AlphaBitDepth=%d", sInfo, lpddsd->dwAlphaBitDepth);
    if (lpddsd->dwReserved) sprintf(sInfo, "%s Reserved=%#x", sInfo, lpddsd->dwReserved);
    if (lpddsd->dwFlags & DDSD_REFRESHRATE) sprintf(sInfo, "%s RefreshRate=%d", sInfo, lpddsd->dwRefreshRate);
    if (lpddsd->dwFlags & DDSD_LINEARSIZE) sprintf(sInfo, "%s LinearSize=%d", sInfo, lpddsd->dwLinearSize);
    if (lpddsd->dwSize == sizeof(DDSURFACEDESC2)) {
        if (lpddsd->dwFlags & DDSD_TEXTURESTAGE) sprintf(sInfo, "%s TextureStage=%#x", sInfo, ((LPDDSURFACEDESC2)lpddsd)->dwTextureStage);
        if (lpddsd->dwFlags & DDSD_FVF) sprintf(sInfo, "%s FVF=%#x", sInfo, ((LPDDSURFACEDESC2)lpddsd)->dwFVF);
    }
    return sInfo;
}

void DumpPixFmt(LPDDSURFACEDESC2 lpddsd) {
    OutTrace("PixelFormat: %s\n", ExplainPixelFormat(&(lpddsd->ddpfPixelFormat)));
}

void DumpCaps(LPDDSURFACEDESC2 lpddsd) {
    OutTrace("> caps  = %08.8x(%s)\n", lpddsd->ddsCaps.dwCaps, ExplainDDSCaps(lpddsd->ddsCaps.dwCaps));
    if(lpddsd->dwSize == sizeof(DDSURFACEDESC2)) {
        OutTrace("> caps2 = %08.8x(%s)\n", lpddsd->ddsCaps.dwCaps2, ExplainDDSCaps2(lpddsd->ddsCaps.dwCaps2));
        OutTrace("> caps3 = %08.8x(%s)\n", lpddsd->ddsCaps.dwCaps3, ExplainDDSCaps3(lpddsd->ddsCaps.dwCaps3));
        OutTrace("> caps4 = %08.8x(%s)\n", lpddsd->ddsCaps.dwCaps4, ExplainDDSCaps4(lpddsd->ddsCaps.dwCaps4));
    }
}
#endif

#define MARKBLITCOLOR32 0x00FFFF00
#define MARKBLITCOLOR16 0x0FF0

static void MarkRect32(DWORD *dest, LPRECT lpdestrect, int destpitch) {
    int w, h;
    int x, y;
    DWORD *dest0;
    destpitch >>= 2;
    x = lpdestrect->left;
    y = lpdestrect->top;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    dest0 = dest += x + (y * destpitch);
    for(x = 0; x < w; x ++) *(dest ++) = MARKBLITCOLOR32;
    if (h < 3) return;
    for(y = 1; y < h - 1; y ++) {
        dest = dest0 + (y * destpitch);
        *dest = MARKBLITCOLOR32;
        dest += w - 1;
        *dest = MARKBLITCOLOR32;
    }
    dest = dest0 + y * destpitch;
    for(x = 0; x < w; x ++) *(dest ++) = MARKBLITCOLOR32;
}

static void MarkRect16(SHORT *dest, LPRECT lpdestrect, int destpitch) {
    int w, h;
    int x, y;
    SHORT *dest0;
    destpitch >>= 1;
    x = lpdestrect->left;
    y = lpdestrect->top;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    dest0 = dest += x + (y * destpitch);
    for(x = 0; x < w; x ++) *(dest ++) = MARKBLITCOLOR16;
    if (h < 3) return;
    for(y = 1; y < h - 1; y ++) {
        dest = dest0 + (y * destpitch);
        *dest = MARKBLITCOLOR16;
        dest += w - 1;
        *dest = MARKBLITCOLOR16;
        dest += destpitch + 1;
    }
    dest = dest0 + y * destpitch;
    for(x = 0; x < w; x ++) *(dest ++) = MARKBLITCOLOR16;
}

/* ------------------------------------------------------------------------------ */
// auxiliary (static) functions for palette handling
/* ------------------------------------------------------------------------------ */

BOOL isPaletteUpdated;

// v2.05.53: revised palette handling
void mySetPalette(int dwstart, int dwcount, LPPALETTEENTRY lpentries) {
    int i;
    extern DXWNDSTATUS *pStatus;
    int nStatCols = 0;
    // if has reserved palette entries, recover them
    if(dxw.ReservedPaletteEntries) {
        int nPalEntries;
        PALETTEENTRY SysPalEntry[256];
        LPPALETTEENTRY lpEntry;
        extern GetSystemPaletteEntries_Type pGDIGetSystemPaletteEntries;
        extern GetDeviceCaps_Type pGDIGetDeviceCaps;
        // fix the first nStatCols/2 and the last nStatCols/2 entries
        if(dxw.ActualPixelFormat.dwRGBBitCount == 8) {
            // The number of static colours should be 20, but inquire it anyway
            dxw.ReservedPaletteEntries = (*pGDIGetDeviceCaps)(GetDC(0), NUMRESERVED);
            nPalEntries = (*pGDIGetSystemPaletteEntries)(GetDC(0), 0, 256, SysPalEntry);
        } else {
            nPalEntries = 256;
            memcpy(SysPalEntry, DefaultSystemPalette, sizeof(SysPalEntry));
        }
        nStatCols = dxw.ReservedPaletteEntries >> 1; // divide by 2
        for (i = 0; i < 256; i++) {
            // skip intermediate un-reserved palette entries
            if(i == nStatCols) i = 256 - nStatCols;
            lpEntry = &lpentries[i];
            if(lpEntry->peFlags == 0) {
                lpEntry->peRed = SysPalEntry[i].peRed;
                lpEntry->peGreen = SysPalEntry[i].peGreen;
                lpEntry->peBlue = SysPalEntry[i].peBlue;
            }
        }
    }
    // copy the palette entries on the current system palette
    for(int idx = 0; idx < dwcount; idx++) {
        int i = dwstart + idx;
        if ((i < nStatCols) ||
                (i > (256 - nStatCols)))
            pStatus->Palette[i] = DefaultSystemPalette[i];
        else
            pStatus->Palette[i] = lpentries[i];
    }
    for(i = 0; i < dwcount; i ++) {
        PALETTEENTRY PalColor;
        PalColor = lpentries[i];
        if (dxw.dwFlags3 & BLACKWHITE) {
            // (http://www.codeproject.com/Articles/66253/Converting-Colors-to-Gray-Shades):
            // gray = (red * 0.30) + (green * 0.59) + (blue * 0.11)
            DWORD grayscale;
            //grayscale = ((DWORD)lpentries[i].peRed + (DWORD)lpentries[i].peGreen + (DWORD)lpentries[i].peBlue) / 3;
            grayscale = (((DWORD)PalColor.peRed * 30) + ((DWORD)PalColor.peGreen * 59) + ((DWORD)PalColor.peBlue) * 11) / 100;
            PalColor.peRed = PalColor.peGreen = PalColor.peBlue = (BYTE)grayscale;
        }
#ifdef REDUCECOLORDEPTH
        if (REDUCECOLORDEPTH) {
#define COLORSHIFT 5
            PalColor.peRed   = (PalColor.peRed   >> COLORSHIFT) << COLORSHIFT;
            PalColor.peGreen = (PalColor.peGreen >> COLORSHIFT) << COLORSHIFT;
            PalColor.peBlue  = (PalColor.peBlue  >> COLORSHIFT) << COLORSHIFT;
        }
#endif
        switch (dxw.ActualPixelFormat.dwRGBBitCount) {
        case 32:
            PaletteEntries[i + dwstart] =
                (((DWORD)PalColor.peRed) << 16) + (((DWORD)PalColor.peGreen) << 8) + ((DWORD)PalColor.peBlue);
            break;
        case 16:
            PaletteEntries[i + dwstart] = (dxw.ActualPixelFormat.dwGBitMask == 0x03E0) ?
                                          (((DWORD)PalColor.peRed & 0xF8) << 8) + (((DWORD)PalColor.peGreen & 0xFC) << 3) + (((DWORD)PalColor.peBlue & 0xF8) >> 3)
                                          :
                                          (((DWORD)PalColor.peRed & 0xF8) << 8) + (((DWORD)PalColor.peGreen & 0xF8) << 3) + (((DWORD)PalColor.peBlue & 0xF8) >> 3);
            break;
        case 8: // it may happen in unemulated modes
            PaletteEntries[i + dwstart] =
                (((DWORD)PalColor.peRed) << 16) + (((DWORD)PalColor.peGreen) << 8) + ((DWORD)PalColor.peBlue);
            break;
        default:
            OutTraceDW("ASSERT: unsupported Color BPP=%d\n", dxw.ActualPixelFormat.dwRGBBitCount);
            break;
        }
    }
    isPaletteUpdated = TRUE;
}

void InitDDScreenParameters(int dxversion, LPDIRECTDRAW lpdd) {
    HRESULT res;
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(DDSURFACEDESC);
    res = myGetDisplayMode(dxversion, lpdd, (LPDDSURFACEDESC)&ddsd);
    if(res) {
        OutTraceE("GetDisplayMode: ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        return;
    }
    OutTraceDW("InitDDScreenParameters: Actual %s\n", DumpPixelFormat((LPDDSURFACEDESC2)&ddsd));
    dxw.ActualPixelFormat = ddsd.ddpfPixelFormat;
    if(dxw.VirtualPixelFormat.dwRGBBitCount == 0) dxw.VirtualPixelFormat = ddsd.ddpfPixelFormat;
    SetBltTransformations();
    return;
}

void InitScreenParameters(int dxversion) {
    extern void FixPixelFormat(int, DDPIXELFORMAT *);
    DEVMODE CurrDevMode;
    static int DoOnce = FALSE;
    DWORD dwVJoyStatus;
    if(DoOnce) return;
    DoOnce = TRUE;
    // v2.04.78: set screen initial resolution
    if(dxw.dwFlags7 & INITIALRES)
        dxw.SetScreenSize(dxw.iMaxW, dxw.iMaxH);
    else
        dxw.SetScreenSize(); // 800 x 600 by default
    GetHookInfo()->Height = (short)dxw.GetScreenHeight();
    GetHookInfo()->Width = (short)dxw.GetScreenWidth();
    GetHookInfo()->ColorDepth = 0; // unknown
    GetHookInfo()->DXVersion = 0; // unknown
    GetHookInfo()->isLogging = (dxw.dwTFlags & OUTTRACE);
    dwVJoyStatus = GetHookInfo()->VJoyStatus;
    dwVJoyStatus &= ~VJOYPRESENT;
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) dwVJoyStatus |= VJOYPRESENT;
    GetHookInfo()->VJoyStatus = dwVJoyStatus;
    if(!(*pEnumDisplaySettings)(NULL, ENUM_CURRENT_SETTINGS, &CurrDevMode)) {
        OutTraceE("EnumDisplaySettings: ERROR err=%d at %d\n", GetLastError(), __LINE__);
        return;
    }
    memset(&dxw.ActualPixelFormat, 0, sizeof(DDPIXELFORMAT));
    // initialize to default null values, but dwRGBBitCount
    dxw.ActualPixelFormat.dwRGBBitCount = CurrDevMode.dmBitsPerPel;
    dxw.VirtualPixelFormat.dwRGBBitCount = CurrDevMode.dmBitsPerPel; // until set differently
    if(dxw.dwFlags2 & INIT8BPP) FixPixelFormat(8, &dxw.VirtualPixelFormat);
    if(dxw.dwFlags2 & INIT16BPP) FixPixelFormat(16, &dxw.VirtualPixelFormat);
    if(dxw.dwFlags7 & INIT24BPP) FixPixelFormat(24, &dxw.VirtualPixelFormat);
    if(dxw.dwFlags7 & INIT32BPP) FixPixelFormat(32, &dxw.VirtualPixelFormat);
    OutTraceDW("InitScreenParameters: dxversion=%d RGBBitCount=%d\n", dxversion, CurrDevMode.dmBitsPerPel);
    SetBltTransformations();
    return;
}

void FixPixelFormat(int ColorDepth, DDPIXELFORMAT *pf) {
    pf->dwFlags = DDPF_RGB;
    GetHookInfo()->ColorDepth = ColorDepth; // v2.05.53
    switch(ColorDepth) {
    case 8:
        pf->dwFlags |= DDPF_PALETTEINDEXED8;
        pf->dwRGBBitCount = 8;
        pf->dwRBitMask = 0;
        pf->dwGBitMask = 0;
        pf->dwBBitMask = 0;
        pf->dwRGBAlphaBitMask = 0x0000;
        break;
    case 15: // v2.02.53: Hesperian Wars - so far the only game setting color depth to 15 BPP!
    case 16:
        pf->dwRGBBitCount = 16;
        if (dxw.dwFlags1 & USERGB565) {
            pf->dwRBitMask = 0xf800;
            pf->dwGBitMask = 0x07e0;
            pf->dwBBitMask = 0x001f;
            pf->dwRGBAlphaBitMask = 0x0000;
        } else {
            if(!(dxw.dwFlags4 & NOALPHACHANNEL) && (ColorDepth == 16)) pf->dwFlags |= DDPF_ALPHAPIXELS; // v2.02.33,40,53
            pf->dwRBitMask = 0x7c00;
            pf->dwGBitMask = 0x03e0;
            pf->dwBBitMask = 0x001f;
            pf->dwRGBAlphaBitMask = 0x8000;
        }
        break;
    case 24:
        pf->dwRGBBitCount = 24;
        pf->dwRBitMask = 0x00FF0000;
        pf->dwGBitMask = 0x0000FF00;
        pf->dwBBitMask = 0x000000FF;
        pf->dwRGBAlphaBitMask = 0x00000000;
        break;
    case 32:
        if(!(dxw.dwFlags4 & NOALPHACHANNEL)) pf->dwFlags |= DDPF_ALPHAPIXELS; // v2.02.33
        pf->dwRGBBitCount = 32;
        pf->dwRBitMask = 0x00FF0000;
        pf->dwGBitMask = 0x0000FF00;
        pf->dwBBitMask = 0x000000FF;
        pf->dwRGBAlphaBitMask = 0xFF000000;
        break;
    }
}

static void ddSetCompatibility() {
    typedef HRESULT (WINAPI * SetAppCompatData_Type)(DWORD, DWORD);
    SetAppCompatData_Type pSetAppCompatData;
    HRESULT res;
    HINSTANCE hinst;
    hinst = (*pLoadLibraryA)("ddraw.dll");
    if(!hinst)
        OutTraceDW("LoadLibrary ddraw.dll ERROR err=%d at %d\n", GetLastError(), __LINE__);
    pSetAppCompatData = (SetAppCompatData_Type)(*pGetProcAddress)(hinst, "SetAppCompatData");
    if(pSetAppCompatData) {
        if (dxw.dwFlags2 & SETCOMPATIBILITY) {
            // v2.03.27: set DWM Off for both primary Lock and Blt. Good for Age of Empires intro movie!
            res = (*pSetAppCompatData)(1, 0);
            res = (*pSetAppCompatData)(2, 0);
            OutTraceDW("HookDirectDraw: SetAppCompatData(2,0) ret=%#x(%s)\n", res, ExplainDDError(res));
        }
        if (dxw.dwFlags6 & DISABLEMAXWINMODE) {
            res = (*pSetAppCompatData)(12, 0);
            OutTraceDW("HookDirectDraw: SetAppCompatData(12,0) ret=%#x(%s)\n", res, ExplainDDError(res));
        }
    } else
        OutTraceDW("HookDirectDraw: missing SetAppCompatData call\n");
    (*pFreeLibrary)(hinst);
}

static void BypassGOGDDrawRedirector() {
    // this procedure bypasses the GOG proxy ddraw.dll that sometimes is found in the GOG game folders.
    // This way, there will be no more the need to rename or delete this file.
    char sSysLibraryPath[MAX_PATH + 10];
    HMODULE hinst;
    GetSystemDirectory(sSysLibraryPath, MAX_PATH);
    strcat(sSysLibraryPath, "\\ddraw.dll");
    hinst = (*pLoadLibraryA)(sSysLibraryPath);
    pDirectDrawEnumerateA = (DirectDrawEnumerateA_Type)GetProcAddress(hinst, "DirectDrawEnumerateA");
    pDirectDrawEnumerateExA = (DirectDrawEnumerateExA_Type)GetProcAddress(hinst, "DirectDrawEnumerateExA");
    pDirectDrawEnumerateW = (DirectDrawEnumerateW_Type)GetProcAddress(hinst, "DirectDrawEnumerateW");
    pDirectDrawEnumerateExW = (DirectDrawEnumerateExW_Type)GetProcAddress(hinst, "DirectDrawEnumerateExW");
    pDirectDrawCreate = (DirectDrawCreate_Type)GetProcAddress(hinst, "DirectDrawCreate");
    pDirectDrawCreateEx = (DirectDrawCreateEx_Type)GetProcAddress(hinst, "DirectDrawCreateEx");
}

int HookDirectDraw(HMODULE module, int version) {
    if(dxw.dwTargetDDVersion == HOOKDDRAWNONE) return 0;
    if ((dxw.dwFlags2 & SETCOMPATIBILITY) ||
            (dxw.dwFlags6 & DISABLEMAXWINMODE)) {
        static BOOL AlreadyDone = FALSE;
        if(!AlreadyDone) {
            ddSetCompatibility();
            AlreadyDone = TRUE;
        }
    }
    if(dxw.dwFlags6 & BYPASSGOGLIBS) {
        BypassGOGDDrawRedirector();
        return TRUE;
    }
    if(dxw.dwFlags4 & HOTPATCH) {
        // hot-patch all APIs and that's all folks!
        HookLibraryEx(module, ddHooks, "ddraw.dll");
        return TRUE;
    }
    //const GUID dd7 = {0x15e65ec0,0x3b9c,0x11d2,0xb9,0x2f,0x00,0x60,0x97,0x97,0xea,0x5b};
    HMODULE hinst;
    OutDebugDW("HookDirectDraw version=%d\n", version); //GHO
    switch(version) {
    case 0: // automatic
        HookLibraryEx(module, ddHooks, "ddraw.dll");
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        HookLibraryEx(module, ddHooks, "ddraw.dll");
        if(!pDirectDrawCreate) { // required for IAT patching
            hinst = (*pLoadLibraryA)("ddraw.dll");
            if(!hinst)
                OutTraceE("LoadLibrary ddraw.dll ERROR err=%d at %d\n", GetLastError(), __LINE__);
            else {
                pDirectDrawCreate = (DirectDrawCreate_Type)GetProcAddress(hinst, "DirectDrawCreate");
                pDirectDrawEnumerateA = (DirectDrawEnumerateA_Type)GetProcAddress(hinst, "DirectDrawEnumerateA");
            }
        }
        if(pDirectDrawCreate) {
            LPDIRECTDRAW lpdd;
            BOOL res;
            res = extDirectDrawCreate(0, &lpdd, 0);
            _if(res) OutTraceE("DirectDrawCreate: ERROR res=%#x(%s)\n", res, ExplainDDError(res));
            lpdd->Release();
        }
        break;
    case 7:
        HookLibraryEx(module, ddHooks, "ddraw.dll");
        if(!pDirectDrawCreate) { // required for IAT patching in "Crimson skies"
            hinst = (*pLoadLibraryA)("ddraw.dll");
            if(!hinst)
                OutTraceE("LoadLibrary ddraw.dll ERROR err=%d at %d\n", GetLastError(), __LINE__);
            else {
                pDirectDrawEnumerateA = (DirectDrawEnumerateA_Type)GetProcAddress(hinst, "DirectDrawEnumerateA");
                pDirectDrawEnumerateExA = (DirectDrawEnumerateExA_Type)GetProcAddress(hinst, "DirectDrawEnumerateExA");
                pDirectDrawCreate = (DirectDrawCreate_Type)GetProcAddress(hinst, "DirectDrawCreate");
                pDirectDrawCreateEx = (DirectDrawCreateEx_Type)GetProcAddress(hinst, "DirectDrawCreateEx");
            }
        }
        if(pDirectDrawCreate) {
            LPDIRECTDRAW lpdd;
            BOOL res;
            res = extDirectDrawCreate(0, &lpdd, 0);
            _if(res) OutTraceE("DirectDrawCreate: ERROR res=%#x(%s)\n", res, ExplainDDError(res));
            lpdd->Release();
        }
        break;
    }
    if(dxw.dwFlags12 & DIRECTXREPLACE) {
        //if(IsHookedBlock(ddHooks)){ -- better force operation in any case?
        char path[MAX_PATH];
        sprintf(path, "%salt.dll\\DDraw.dll", GetDxWndPath());
        PinLibraryEx(ddHooks, path);
        OutTrace("HookDirectDraw: alt.dll loaded path=%s\n", path);
        //}
    }
    if(pDirectDrawCreate || pDirectDrawCreateEx) return 1;
    return 0;
}

void HookDDraw(HMODULE module) {
    HookDirectDraw(module, dxw.dwTargetDDVersion);
}

CreatePalette_Type pCreatePaletteMethod(int dxversion) {
    CreatePalette_Type pCreatePalette;
    switch(dxversion) {
    case 1:
        pCreatePalette = pCreatePalette1;
        break;
    case 2:
        pCreatePalette = pCreatePalette2;
        break;
    case 3:
        pCreatePalette = pCreatePalette3;
        break;
    case 4:
        pCreatePalette = pCreatePalette4;
        break;
    case 7:
        pCreatePalette = pCreatePalette7;
        break;
    }
    CHECKPTR(pCreatePalette, "CreatePalette");
    return pCreatePalette;
}

SetPalette_Type pSetPaletteMethod(int dxversion) {
    SetPalette_Type pSetPalette;
    switch(dxversion) {
    case 1:
        pSetPalette = pSetPalette1;
        break;
    case 2:
        pSetPalette = pSetPalette2;
        break;
    case 3:
        pSetPalette = pSetPalette3;
        break;
    case 4:
        pSetPalette = pSetPalette4;
        break;
    case 7:
        pSetPalette = pSetPalette7;
        break;
    }
    CHECKPTR(pSetPalette, "SetPalette");
    return pSetPalette;
}

ReleaseS_Type pReleaseSMethod(int dxversion) {
    ReleaseS_Type pReleaseS;
    switch(dxversion) {
    case 1:
        pReleaseS = pReleaseS1;
        break;
    case 2:
        pReleaseS = pReleaseS2;
        break;
    case 3:
        pReleaseS = pReleaseS3;
        break;
    case 4:
        pReleaseS = pReleaseS4;
        break;
    case 7:
        pReleaseS = pReleaseS7;
        break;
    }
    CHECKPTR(pReleaseS, "Surface::Release");
    return pReleaseS;
}

ReleaseDC_Type pReleaseDCMethod() {
    ReleaseDC_Type pReleaseDC;
    switch(iPrimarySurfaceVersion) {
    case 1:
        pReleaseDC = pReleaseDC1;
        break;
    case 2:
        pReleaseDC = pReleaseDC2;
        break;
    case 3:
        pReleaseDC = pReleaseDC3;
        break;
    case 4:
        pReleaseDC = pReleaseDC4;
        break;
    case 7:
        pReleaseDC = pReleaseDC7;
        break;
    }
    CHECKPTR(pReleaseDC, "ReleaseDC");
    return pReleaseDC;
}

SetClipper_Type pSetClipperMethod(int dxversion) {
    SetClipper_Type pSetClipper;
    switch(dxversion) {
    case 1:
        pSetClipper = pSetClipper1;
        break;
    case 2:
        pSetClipper = pSetClipper2;
        break;
    case 3:
        pSetClipper = pSetClipper3;
        break;
    case 4:
        pSetClipper = pSetClipper4;
        break;
    case 7:
        pSetClipper = pSetClipper7;
        break;
    }
    CHECKPTR(pSetClipper, "SetClipper");
    return pSetClipper;
}

Blt_Type pGetBltMethod(int dxversion) {
    Blt_Type pBlt;
    switch(dxversion) {
    case 1:
        pBlt = pBlt1;
        break;
    case 2:
        pBlt = pBlt2;
        break;
    case 3:
        pBlt = pBlt3;
        break;
    case 4:
        pBlt = pBlt4;
        break;
    case 7:
        pBlt = pBlt7;
        break;
    }
    CHECKPTR(pBlt, "Blt");
    return pBlt;
}

Blt_Type pBltMethod() {
    Blt_Type pBlt;
    switch(iPrimarySurfaceVersion) {
    case 1:
        pBlt = pBlt1;
        break;
    case 2:
        pBlt = pBlt2;
        break;
    case 3:
        pBlt = pBlt3;
        break;
    case 4:
        pBlt = pBlt4;
        break;
    case 7:
        pBlt = pBlt7;
        break;
    }
    CHECKPTR(pBlt, "Blt");
    return pBlt;
}

GetDC_Type pGetDCMethod() {
    GetDC_Type pGetDC;
    switch(iPrimarySurfaceVersion) {
    case 1:
        pGetDC = pGetDC1;
        break;
    case 2:
        pGetDC = pGetDC2;
        break;
    case 3:
        pGetDC = pGetDC3;
        break;
    case 4:
        pGetDC = pGetDC4;
        break;
    case 7:
        pGetDC = pGetDC7;
        break;
    }
    CHECKPTR(pGetDC, "GetDC");
    return pGetDC;
}

Unlock4_Type pUnlockMethod(int dxversion) {
    Unlock4_Type pUnlock;
    switch(dxversion) {
    case 1:
        pUnlock = (Unlock4_Type)pUnlock1;
        break;
    case 2:
        pUnlock = (Unlock4_Type)pUnlock2;
        break;
    case 3:
        pUnlock = (Unlock4_Type)pUnlock3;
        break;
    case 4:
        pUnlock = (Unlock4_Type)pUnlock4;
        break;
    case 7:
        pUnlock = (Unlock4_Type)pUnlock7;
        break;
    }
    CHECKPTR(pUnlock, "Unlock");
    return pUnlock;
}

Lock_Type pLockMethod(int dxversion) {
    Lock_Type pLock;
    switch(dxversion) {
    case 1:
        pLock = pLock1;
        break;
    case 2:
        pLock = pLock2;
        break;
    case 3:
        pLock = pLock3;
        break;
    case 4:
        pLock = pLock4;
        break;
    case 7:
        pLock = pLock7;
        break;
    }
    CHECKPTR(pLock, "Lock");
    return pLock;
}

CreateSurface2_Type pCreateSurfaceMethod(int dxversion) {
    CreateSurface2_Type pCreateSurface;
    switch(dxversion) {
    case 1:
        pCreateSurface = (CreateSurface2_Type)pCreateSurface1;
        break;
    case 2:
        pCreateSurface = (CreateSurface2_Type)pCreateSurface2;
        break;
    case 3:
        pCreateSurface = (CreateSurface2_Type)pCreateSurface3;
        break;
    case 4:
        pCreateSurface = (CreateSurface2_Type)pCreateSurface4;
        break;
    case 7:
        pCreateSurface = (CreateSurface2_Type)pCreateSurface7;
        break;
    }
    CHECKPTR(pCreateSurface, "CreateSurface");
    return pCreateSurface;
}

GetSurfaceDesc2_Type pGetSurfaceDescMethod() {
    GetSurfaceDesc2_Type pGetSurfaceDesc;
    pGetSurfaceDesc = NULL;
    switch(iPrimarySurfaceVersion) {
    case 1:
        pGetSurfaceDesc = (GetSurfaceDesc2_Type)pGetSurfaceDesc1;
        break;
    case 2:
        pGetSurfaceDesc = (GetSurfaceDesc2_Type)pGetSurfaceDesc2;
        break;
    case 3:
        pGetSurfaceDesc = (GetSurfaceDesc2_Type)pGetSurfaceDesc3;
        break;
    case 4:
        pGetSurfaceDesc = (GetSurfaceDesc2_Type)pGetSurfaceDesc4;
        break;
    case 7:
        pGetSurfaceDesc = (GetSurfaceDesc2_Type)pGetSurfaceDesc7;
        break;
    }
    CHECKPTR(pGetSurfaceDesc, "GetSurfaceDesc");
    return pGetSurfaceDesc;
}

GetGDISurface_Type pGetGDISurfaceMethod(int dxversion) {
    GetGDISurface_Type pGetGDISurface;
    switch(dxversion) {
    default:
    case 1:
        pGetGDISurface = pGetGDISurface1;
        break;
    case 2:
        pGetGDISurface = pGetGDISurface2;
        break;
    case 3:
        pGetGDISurface = pGetGDISurface3;
        break;
    case 4:
        pGetGDISurface = pGetGDISurface4;
        break;
    case 7:
        pGetGDISurface = pGetGDISurface7;
        break;
    }
    CHECKPTR(pGetGDISurface, "GetGDISurface");
    return pGetGDISurface;
}

#ifndef DXW_NOTRACES
void DescribeSurface(LPDIRECTDRAWSURFACE lpdds, int dxversion, char *label, int line) {
    DDSURFACEDESC2 ddsd;
    HRESULT res;
    GetSurfaceDesc2_Type pGetSurfaceDesc = NULL;
    int dwSize = (dxversion < 4) ? sizeof(DDSURFACEDESC) : sizeof(DDSURFACEDESC2);
    memset(&ddsd, 0, dwSize);
    ddsd.dwSize = dwSize;
    pGetSurfaceDesc = pGetSurfaceDescMethod();
    if(pGetSurfaceDesc == NULL) return;
    res = (*pGetSurfaceDescMethod())((LPDIRECTDRAWSURFACE2)lpdds, &ddsd);
    if(res)return;
    OutTraceDW("Surface %s: ddsd=%#x dxversion=%d %s\n",
               label, lpdds, dxversion, LogSurfaceAttributes(&ddsd, label, line));
}
#endif

void RegisterPixelFormat(int dxversion, LPDIRECTDRAWSURFACE lpdds) {
    DDSURFACEDESC2 ddsdpix;
    memset((void *)&ddsdpix, 0, sizeof(DDSURFACEDESC2));
    ddsdpix.dwSize = (dxversion < 4) ? sizeof(DDSURFACEDESC) : sizeof(DDSURFACEDESC2);
    ddsdpix.dwFlags = DDSD_PIXELFORMAT;
    switch(dxversion) {
    case 1:
        if(pGetSurfaceDesc1)(*pGetSurfaceDesc1)((LPDIRECTDRAWSURFACE)lpdds, (LPDDSURFACEDESC)&ddsdpix);
        break;
    case 2:
        if(pGetSurfaceDesc2)(*pGetSurfaceDesc2)((LPDIRECTDRAWSURFACE)lpdds, (LPDDSURFACEDESC)&ddsdpix);
        break;
    case 3:
        if(pGetSurfaceDesc3)(*pGetSurfaceDesc3)((LPDIRECTDRAWSURFACE)lpdds, (LPDDSURFACEDESC)&ddsdpix);
        break;
    case 4:
        if(pGetSurfaceDesc4)(*pGetSurfaceDesc4)((LPDIRECTDRAWSURFACE2)lpdds, &ddsdpix);
        break;
    case 7:
        if(pGetSurfaceDesc7)(*pGetSurfaceDesc7)((LPDIRECTDRAWSURFACE2)lpdds, &ddsdpix);
        break;
    }
    GetHookInfo()->pfd = ddsdpix.ddpfPixelFormat; // v2.02.88
    OutDebugDW("RegisterPixelFormat: lpdds=%#x %s\n", lpdds, DumpPixelFormat(&ddsdpix));
}

/* ------------------------------------------------------------------ */

// SetPixFmt: builds a pixel format descriptor when no one is specified, starting from the color depth, the current
// desktop pixel format (when the color depth is the same) or the config flags

void SetPixFmt(LPDDSURFACEDESC2 lpdd) {
    OutTraceDW("SetPixFmt: BPP=%d Use565=%d NoAlpha=%d\n",
               dxw.VirtualPixelFormat.dwRGBBitCount,
               dxw.dwFlags1 & USERGB565 ? 1 : 0,
               dxw.dwFlags4 & NOALPHACHANNEL ? 1 : 0);
    memset(&lpdd->ddpfPixelFormat, 0, sizeof(DDPIXELFORMAT));
    lpdd->ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    switch (dxw.VirtualPixelFormat.dwRGBBitCount) {
    case 8:
    case 15:
    case 16:
    case 24:
    case 32:
        FixPixelFormat(dxw.VirtualPixelFormat.dwRGBBitCount, &lpdd->ddpfPixelFormat);
        break;
    default:
        OutTraceE("CreateSurface ERROR: Unsupported resolution ColorBPP=%d\n", dxw.VirtualPixelFormat.dwRGBBitCount);
        break;
    }
    // remember current virtual settings
    dxw.VirtualPixelFormat = lpdd->ddpfPixelFormat;
    OutTraceDW("SetPixFmt: %s\n", DumpPixelFormat(lpdd));
}

// retrieves the stored pixel format

void GetPixFmt(LPDDSURFACEDESC2 lpdd) {
    lpdd->ddpfPixelFormat = dxw.VirtualPixelFormat;
    OutTraceDW("GetPixFmt: %s\n", DumpPixelFormat(lpdd));
}

/* ------------------------------------------------------------------ */
// hook query functions that determines the object versioning ....
/* ------------------------------------------------------------------ */

int Set_dwSize_From_Surface() {
    int size;
    switch(iPrimarySurfaceVersion) {
    case 1:
        size = sizeof(DDSURFACEDESC);
        break;
    case 2:
        size = sizeof(DDSURFACEDESC);
        break;
    case 3:
        size = sizeof(DDSURFACEDESC);
        break;
    case 4:
        size = sizeof(DDSURFACEDESC2);
        break;
    case 7:
        size = sizeof(DDSURFACEDESC2);
        break;
    }
    return size;
}

void HookDDSession(LPDIRECTDRAW *lplpdd, int dxversion) {
    OutTraceDW("Hooking directdraw session dd=%#x dxversion=%d thread_id=%#x\n",
               *lplpdd, dxversion, GetCurrentThreadId());
    iDirectDrawVersion = dxversion; // save for ddraw session operations
    switch(dxversion) {
    case 1:
        SetHook((void *)(**(DWORD **)lplpdd), extQueryInterfaceD1, (void **)&pQueryInterfaceD1, "QueryInterface(D1)");
        SetHook((void *)(**(DWORD **)lplpdd +  8), extReleaseD1, (void **)&pReleaseD1, "Release(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 12), extCompact1, (void **)&pCompact1, "Compact(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 16), extCreateClipper1, (void **)&pCreateClipper1, "CreateClipper(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 20), extCreatePalette1, (void **)&pCreatePalette1, "CreatePalette(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 24), extCreateSurface1, (void **)&pCreateSurface1, "CreateSurface(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 28), extDuplicateSurface1, (void **)&pDuplicateSurface1, "DuplicateSurface(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 32), extEnumDisplayModes1, (void **)&pEnumDisplayModes1, "EnumDisplayModes(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 36), extEnumSurfaces1, (void **)&pEnumSurfaces1, "EnumSurfaces(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 40), extFlipToGDISurface1, (void **)&pFlipToGDISurface1, "FlipToGDISurface(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 44), extGetCaps1D, (void **)&pGetCaps1D, "GetCaps(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 48), extGetDisplayMode1, (void **)&pGetDisplayMode1, "GetDisplayMode(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 52), extGetFourCCCodes1, (void **)&pGetFourCCCodes1, "GetFourCCCodes(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 56), extGetGDISurface1, (void **)&pGetGDISurface1, "GetGDISurface(D1)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdd + 60), extGetMonitorFrequency1, (void **)&pGetMonitorFrequency1, "GetMonitorFrequency(D1)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd + 64), extGetScanLine1, (void **)&pGetScanLine1, "GetScanLine(D1)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdd + 68), extGetVerticalBlankStatus1, (void **)&pGetVerticalBlankStatus1, "GetVerticalBlankStatus(D1)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd + 72), extInitialize1, (void **)&pInitialize1, "Initialize(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 76), extRestoreDisplayMode1, (void **)&pRestoreDisplayMode1, "RestoreDisplayMode(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 80), extSetCooperativeLevel1, (void **)&pSetCooperativeLevel1, "SetCooperativeLevel(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 84), extSetDisplayMode1, (void **)&pSetDisplayMode1, "SetDisplayMode(D1)");
        SetHook((void *)(**(DWORD **)lplpdd + 88), extWaitForVerticalBlank1, (void **)&pWaitForVerticalBlank1, "WaitForVerticalBlank(D1)");
        break;
    case 2:
        SetHook((void *)(**(DWORD **)lplpdd), extQueryInterfaceD2, (void **)&pQueryInterfaceD2, "QueryInterface(D2)");
        SetHook((void *)(**(DWORD **)lplpdd +  8), extReleaseD2, (void **)&pReleaseD2, "Release(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 12), extCompact2, (void **)&pCompact2, "Compact(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 16), extCreateClipper2, (void **)&pCreateClipper2, "CreateClipper(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 20), extCreatePalette2, (void **)&pCreatePalette2, "CreatePalette(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 24), extCreateSurface2, (void **)&pCreateSurface2, "CreateSurface(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 28), extDuplicateSurface2, (void **)&pDuplicateSurface2, "DuplicateSurface(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 32), extEnumDisplayModes2, (void **)&pEnumDisplayModes2, "EnumDisplayModes(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 36), extEnumSurfaces2, (void **)&pEnumSurfaces2, "EnumSurfaces(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 40), extFlipToGDISurface2, (void **)&pFlipToGDISurface2, "FlipToGDISurface(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 44), extGetCaps2D, (void **)&pGetCaps2D, "GetCaps(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 48), extGetDisplayMode2, (void **)&pGetDisplayMode2, "GetDisplayMode(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 52), extGetFourCCCodes2, (void **)&pGetFourCCCodes2, "GetFourCCCodes(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 56), extGetGDISurface2, (void **)&pGetGDISurface2, "GetGDISurface(D2)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdd + 60), extGetMonitorFrequency2, (void **)&pGetMonitorFrequency2, "GetMonitorFrequency(D2)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd + 64), extGetScanLine2, (void **)&pGetScanLine2, "GetScanLine(D2)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdd + 68), extGetVerticalBlankStatus2, (void **)&pGetVerticalBlankStatus2, "GetVerticalBlankStatus(D2)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd + 72), extInitialize2, (void **)&pInitialize2, "Initialize(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 76), extRestoreDisplayMode2, (void **)&pRestoreDisplayMode2, "RestoreDisplayMode(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 80), extSetCooperativeLevel2, (void **)&pSetCooperativeLevel2, "SetCooperativeLevel(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 84), extSetDisplayMode2, (void **)&pSetDisplayMode2, "SetDisplayMode(D2)");
        SetHook((void *)(**(DWORD **)lplpdd + 88), extWaitForVerticalBlank2, (void **)&pWaitForVerticalBlank2, "WaitForVerticalBlank(D2)");
        // added in v2 interface
        SetHook((void *)(**(DWORD **)lplpdd + 92), extGetAvailableVidMem2, (void **)&pGetAvailableVidMem2, "GetAvailableVidMem(D2)");
        break;
    case 3:
        SetHook((void *)(**(DWORD **)lplpdd), extQueryInterfaceD3, (void **)&pQueryInterfaceD3, "QueryInterface(D3)");
        SetHook((void *)(**(DWORD **)lplpdd +  8), extReleaseD3, (void **)&pReleaseD3, "Release(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 12), extCompact3, (void **)&pCompact3, "Compact(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 16), extCreateClipper3, (void **)&pCreateClipper3, "CreateClipper(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 20), extCreatePalette3, (void **)&pCreatePalette3, "CreatePalette(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 24), extCreateSurface3, (void **)&pCreateSurface3, "CreateSurface(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 28), extDuplicateSurface3, (void **)&pDuplicateSurface3, "DuplicateSurface(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 32), extEnumDisplayModes3, (void **)&pEnumDisplayModes3, "EnumDisplayModes(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 36), extEnumSurfaces3, (void **)&pEnumSurfaces3, "EnumSurfaces(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 40), extFlipToGDISurface3, (void **)&pFlipToGDISurface3, "FlipToGDISurface(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 44), extGetCaps3D, (void **)&pGetCaps3D, "GetCaps(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 48), extGetDisplayMode3, (void **)&pGetDisplayMode3, "GetDisplayMode(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 52), extGetFourCCCodes3, (void **)&pGetFourCCCodes3, "GetFourCCCodes(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 56), extGetGDISurface3, (void **)&pGetGDISurface3, "GetGDISurface(D3)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdd + 60), extGetMonitorFrequency3, (void **)&pGetMonitorFrequency3, "GetMonitorFrequency(D3)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd + 64), extGetScanLine3, (void **)&pGetScanLine3, "GetScanLine(D3)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdd + 68), extGetVerticalBlankStatus3, (void **)&pGetVerticalBlankStatus3, "GetVerticalBlankStatus(D3)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd + 72), extInitialize3, (void **)&pInitialize3, "Initialize(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 76), extRestoreDisplayMode3, (void **)&pRestoreDisplayMode3, "RestoreDisplayMode(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 80), extSetCooperativeLevel3, (void **)&pSetCooperativeLevel3, "SetCooperativeLevel(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 84), extSetDisplayMode3, (void **)&pSetDisplayMode3, "SetDisplayMode(D3)");
        SetHook((void *)(**(DWORD **)lplpdd + 88), extWaitForVerticalBlank3, (void **)&pWaitForVerticalBlank3, "WaitForVerticalBlank(D3)");
        // added in v2 interface
        SetHook((void *)(**(DWORD **)lplpdd + 92), extGetAvailableVidMem3, (void **)&pGetAvailableVidMem3, "GetAvailableVidMem(D3)");
        break;
    case 4:
        SetHook((void *)(**(DWORD **)lplpdd), extQueryInterfaceD4, (void **)&pQueryInterfaceD4, "QueryInterface(D4)");
        SetHook((void *)(**(DWORD **)lplpdd +  8), extReleaseD4, (void **)&pReleaseD4, "Release(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 12), extCompact4, (void **)&pCompact4, "Compact(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 16), extCreateClipper4, (void **)&pCreateClipper4, "CreateClipper(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 20), extCreatePalette4, (void **)&pCreatePalette4, "CreatePalette(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 24), extCreateSurface4, (void **)&pCreateSurface4, "CreateSurface(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 28), extDuplicateSurface4, (void **)&pDuplicateSurface4, "DuplicateSurface(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 32), extEnumDisplayModes4, (void **)&pEnumDisplayModes4, "EnumDisplayModes(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 36), extEnumSurfaces4, (void **)&pEnumSurfaces4, "EnumSurfaces(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 40), extFlipToGDISurface4, (void **)&pFlipToGDISurface4, "FlipToGDISurface(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 44), extGetCaps4D, (void **)&pGetCaps4D, "GetCaps(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 48), extGetDisplayMode4, (void **)&pGetDisplayMode4, "GetDisplayMode(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 52), extGetFourCCCodes4, (void **)&pGetFourCCCodes4, "GetFourCCCodes(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 56), extGetGDISurface4, (void **)&pGetGDISurface4, "GetGDISurface(D4)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdd + 60), extGetMonitorFrequency4, (void **)&pGetMonitorFrequency4, "GetMonitorFrequency(D4)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd + 64), extGetScanLine4, (void **)&pGetScanLine4, "GetScanLine(D4)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdd + 68), extGetVerticalBlankStatus4, (void **)&pGetVerticalBlankStatus4, "GetVerticalBlankStatus(D4)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd + 72), extInitialize4, (void **)&pInitialize4, "Initialize(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 76), extRestoreDisplayMode4, (void **)&pRestoreDisplayMode4, "RestoreDisplayMode(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 80), extSetCooperativeLevel4, (void **)&pSetCooperativeLevel4, "SetCooperativeLevel(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 84), extSetDisplayMode4, (void **)&pSetDisplayMode4, "SetDisplayMode(D4)");
        SetHook((void *)(**(DWORD **)lplpdd + 88), extWaitForVerticalBlank4, (void **)&pWaitForVerticalBlank4, "WaitForVerticalBlank(D4)");
        // added in v2 interface
        SetHook((void *)(**(DWORD **)lplpdd + 92), extGetAvailableVidMem4, (void **)&pGetAvailableVidMem4, "GetAvailableVidMem(D4)");
        // added in v4 interface
        SetHook((void *)(**(DWORD **)lplpdd + 96), extGetSurfaceFromDC4, (void **)&pGetSurfaceFromDC4, "GetSurfaceFromDC(D4)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdd + 100), extRestoreAllSurfaces4, (void **)&pRestoreAllSurfaces4, "RestoreAllSurfaces(D4)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd + 104), extTestCooperativeLevel4, (void **)&pTestCooperativeLevel4, "TestCooperativeLevel(D4)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdd + 108), extGetDeviceIdentifier4, (void **)&pGetDeviceIdentifier4, "GetDeviceIdentifier(D4)");
#endif
        break;
    case 7:
        SetHook((void *)(**(DWORD **)lplpdd), extQueryInterfaceD7, (void **)&pQueryInterfaceD7, "QueryInterface(D7)");
        SetHook((void *)(**(DWORD **)lplpdd +  8), extReleaseD7, (void **)&pReleaseD7, "Release(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 12), extCompact7, (void **)&pCompact7, "Compact(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 16), extCreateClipper7, (void **)&pCreateClipper7, "CreateClipper(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 20), extCreatePalette7, (void **)&pCreatePalette7, "CreatePalette(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 24), extCreateSurface7, (void **)&pCreateSurface7, "CreateSurface(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 28), extDuplicateSurface7, (void **)&pDuplicateSurface7, "DuplicateSurface(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 32), extEnumDisplayModes7, (void **)&pEnumDisplayModes7, "EnumDisplayModes(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 36), extEnumSurfaces7, (void **)&pEnumSurfaces7, "EnumSurfaces(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 40), extFlipToGDISurface7, (void **)&pFlipToGDISurface7, "FlipToGDISurface(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 44), extGetCaps7D, (void **)&pGetCaps7D, "GetCaps(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 48), extGetDisplayMode7, (void **)&pGetDisplayMode7, "GetDisplayMode(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 52), extGetFourCCCodes7, (void **)&pGetFourCCCodes7, "GetFourCCCodes(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 56), extGetGDISurface7, (void **)&pGetGDISurface7, "GetGDISurface(D7)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdd + 60), extGetMonitorFrequency7, (void **)&pGetMonitorFrequency7, "GetMonitorFrequency(D7)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd + 64), extGetScanLine7, (void **)&pGetScanLine7, "GetScanLine(D7)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdd + 68), extGetVerticalBlankStatus7, (void **)&pGetVerticalBlankStatus7, "GetVerticalBlankStatus(D7)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd + 72), extInitialize7, (void **)&pInitialize7, "Initialize(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 76), extRestoreDisplayMode7, (void **)&pRestoreDisplayMode7, "RestoreDisplayMode(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 80), extSetCooperativeLevel7, (void **)&pSetCooperativeLevel7, "SetCooperativeLevel(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 84), extSetDisplayMode7, (void **)&pSetDisplayMode7, "SetDisplayMode(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 88), extWaitForVerticalBlank7, (void **)&pWaitForVerticalBlank7, "WaitForVerticalBlank(D7)");
        // added in v2 interface
        SetHook((void *)(**(DWORD **)lplpdd + 92), extGetAvailableVidMem7, (void **)&pGetAvailableVidMem7, "GetAvailableVidMem(D7)");
        // added in v4 interface
        SetHook((void *)(**(DWORD **)lplpdd + 96), extGetSurfaceFromDC7, (void **)&pGetSurfaceFromDC7, "GetSurfaceFromDC(D7)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdd + 100), extRestoreAllSurfaces7, (void **)&pRestoreAllSurfaces7, "RestoreAllSurfaces(D7)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd + 104), extTestCooperativeLevel7, (void **)&pTestCooperativeLevel7, "TestCooperativeLevel(D7)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdd + 108), extGetDeviceIdentifier7, (void **)&pGetDeviceIdentifier7, "GetDeviceIdentifier(D7)");
#endif
        // added in v7 interface
        SetHook((void *)(**(DWORD **)lplpdd + 112), extStartModeTest, (void **)&pStartModeTest, "StartModeTest(D7)");
        SetHook((void *)(**(DWORD **)lplpdd + 116), extEvaluateMode, (void **)&pEvaluateMode, "EvaluateMode(D7)");
        break;
    }
}

void HookDDClipper(LPDIRECTDRAWCLIPPER FAR *lplpDDClipper) {
    OutTraceDW("Hooking directdraw clipper dd=%#x\n", *lplpDDClipper);
    SetHook((void *)(**(DWORD **)lplpDDClipper + 8), extReleaseC, (void **)&pReleaseC, "Release(C)");
    SetHook((void *)(**(DWORD **)lplpDDClipper + 12), extGetClipList, (void **)&pGetClipList, "GetClipList(C)");
    SetHook((void *)(**(DWORD **)lplpDDClipper + 28), extSetClipList, (void **)&pSetClipList, "SetClipList(C)");
    SetHook((void *)(**(DWORD **)lplpDDClipper + 32), extSetHWnd, (void **)&pSetHWnd, "SetHWnd(C)");
}

static void HookDDPalette(LPDIRECTDRAWPALETTE FAR *lplpDDPalette) {
    OutTraceDW("Hooking directdraw palette dd=%#x\n", *lplpDDPalette);
    /*** IDirectDrawPalette methods ***/
    SetHook((void *)(**(DWORD **)lplpDDPalette + 8), extReleaseP, (void **)&pReleaseP, "Release(P)");
    SetHook((void *)(**(DWORD **)lplpDDPalette + 24), extSetEntries, (void **)&pSetEntries, "SetEntries(P)");
}

void HookDDSurface(LPDIRECTDRAWSURFACE *lplpdds, int dxversion, BOOL isPrim) {
    if(iPrimarySurfaceVersion == 0) iPrimarySurfaceVersion = dxversion; // v2.03.60
    OutTraceDW("Hooking surface dds=%#x dxversion=%d prim=%#x thread_id=%#x\n",
               *lplpdds, dxversion, isPrim, GetCurrentThreadId());
    switch(dxversion) {
    case 1:
        SetHook((void *)(**(DWORD **)lplpdds), extQueryInterfaceS1, (void **)&pQueryInterfaceS1, "QueryInterface(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 8), extReleaseS1, (void **)&pReleaseS1, "Release(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 12), extAddAttachedSurface1, (void **)&pAddAttachedSurface1, "AddAttachedSurface(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 16), extAddOverlayDirtyRect1, (void **)&pAddOverlayDirtyRect1, "AddOverlayDirtyRect(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 20), extBlt1, (void **)&pBlt1, "Blt(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 24), extBltBatch1, (void **)&pBltBatch1, "BltBatch(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 28), extBltFast1, (void **)&pBltFast1, "BltFast(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 32), extDeleteAttachedSurface1, (void **)&pDeleteAttachedSurface1, "DeleteAttachedSurface(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 36), extEnumAttachedSurfaces1, (void **)&pEnumAttachedSurfaces1, "EnumAttachedSurfaces(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 40), extEnumOverlayZOrders1, (void **)&pEnumOverlayZOrders1, "EnumOverlayZOrders(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 44), extFlip1, (void **)&pFlip1, "Flip(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 48), extGetAttachedSurface1, (void **)&pGetAttachedSurface1, "GetAttachedSurface(S1)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdds + 52), extGetBltStatus1, (void **)&pGetBltStatus1, "GetBltStatus(S1)");
#endif
        SetHook((void *)(**(DWORD **)lplpdds + 56), extGetCaps1S, (void **)&pGetCaps1S, "GetCaps(S1)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdds + 60), extGetClipper1, (void **)&pGetClipper1, "GetClipper(S1)");
#endif
        SetHook((void *)(**(DWORD **)lplpdds + 64), extGetColorKey1, (void **)&pGetColorKey1, "GetColorKey(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 68), extGetDC1, (void **)&pGetDC1, "GetDC(S1)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdds + 72), extGetFlipStatus1, (void **)&pGetFlipStatus1, "GetFlipStatus(S1)");
#endif
        SetHook((void *)(**(DWORD **)lplpdds + 76), extGetOverlayPosition1, (void **)&pGetOverlayPosition1, "GetOverlayPosition(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 80), extGetPalette1, (void **)&pGetPalette1, "GetPalette(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 84), extGetPixelFormat1, (void **)&pGetPixelFormat1, "GetPixelFormat(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 88), extGetSurfaceDesc1, (void **)&pGetSurfaceDesc1, "GetSurfaceDesc(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 100), extLock1, (void **)&pLock1, "Lock(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 128), extUnlock1, (void **)&pUnlock1, "Unlock(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 104), extReleaseDC1, (void **)&pReleaseDC1, "ReleaseDC(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 120), extSetOverlayPosition1, (void **)&pSetOverlayPosition1, "SetOverlayPosition(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 124), extSetPalette1, (void **)&pSetPalette1, "SetPalette(S1)");
        break;
    case 2:
        SetHook((void *)(**(DWORD **)lplpdds), extQueryInterfaceS2, (void **)&pQueryInterfaceS2, "QueryInterface(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 8), extReleaseS2, (void **)&pReleaseS2, "Release(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 12), extAddAttachedSurface2, (void **)&pAddAttachedSurface2, "AddAttachedSurface(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 16), extAddOverlayDirtyRect2, (void **)&pAddOverlayDirtyRect2, "AddOverlayDirtyRect(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 20), extBlt2, (void **)&pBlt2, "Blt(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 24), extBltBatch2, (void **)&pBltBatch2, "BltBatch(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 28), extBltFast2, (void **)&pBltFast2, "BltFast(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 32), extDeleteAttachedSurface2, (void **)&pDeleteAttachedSurface2, "DeleteAttachedSurface(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 36), extEnumAttachedSurfaces2, (void **)&pEnumAttachedSurfaces2, "EnumAttachedSurfaces(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 40), extEnumOverlayZOrders2, (void **)&pEnumOverlayZOrders2, "EnumOverlayZOrders(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 44), extFlip2, (void **)&pFlip2, "Flip(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 48), extGetAttachedSurface2, (void **)&pGetAttachedSurface2, "GetAttachedSurface(S2)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdds + 52), extGetBltStatus2, (void **)&pGetBltStatus2, "GetBltStatus(S2)");
#endif
        SetHook((void *)(**(DWORD **)lplpdds + 56), extGetCaps2S, (void **)&pGetCaps2S, "GetCaps(S2)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdds + 60), extGetClipper2, (void **)&pGetClipper2, "GetClipper(S2)");
#endif
        SetHook((void *)(**(DWORD **)lplpdds + 64), extGetColorKey2, (void **)&pGetColorKey2, "GetColorKey(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 68), extGetDC2, (void **)&pGetDC2, "GetDC(S2)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdds + 72), extGetFlipStatus2, (void **)&pGetFlipStatus2, "GetFlipStatus(S2)");
#endif
        SetHook((void *)(**(DWORD **)lplpdds + 76), extGetOverlayPosition2, (void **)&pGetOverlayPosition2, "GetOverlayPosition(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 80), extGetPalette2, (void **)&pGetPalette2, "GetPalette(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 84), extGetPixelFormat2, (void **)&pGetPixelFormat2, "GetPixelFormat(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 88), extGetSurfaceDesc2, (void **)&pGetSurfaceDesc2, "GetSurfaceDesc(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 100), extLock2, (void **)&pLock2, "Lock(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 128), extUnlock2, (void **)&pUnlock2, "Unlock(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 104), extReleaseDC2, (void **)&pReleaseDC2, "ReleaseDC(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 120), extSetOverlayPosition2, (void **)&pSetOverlayPosition2, "SetOverlayPosition(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 124), extSetPalette2, (void **)&pSetPalette2, "SetPalette(S2)");
        break;
    case 3:
        SetHook((void *)(**(DWORD **)lplpdds), extQueryInterfaceS3, (void **)&pQueryInterfaceS3, "QueryInterface(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 8), extReleaseS3, (void **)&pReleaseS3, "Release(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 12), extAddAttachedSurface3, (void **)&pAddAttachedSurface3, "AddAttachedSurface(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 16), extAddOverlayDirtyRect3, (void **)&pAddOverlayDirtyRect3, "AddOverlayDirtyRect(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 20), extBlt3, (void **)&pBlt3, "Blt(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 24), extBltBatch3, (void **)&pBltBatch3, "BltBatch(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 28), extBltFast3, (void **)&pBltFast3, "BltFast(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 32), extDeleteAttachedSurface3, (void **)&pDeleteAttachedSurface3, "DeleteAttachedSurface(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 36), extEnumAttachedSurfaces3, (void **)&pEnumAttachedSurfaces3, "EnumAttachedSurfaces(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 40), extEnumOverlayZOrders3, (void **)&pEnumOverlayZOrders3, "EnumOverlayZOrders(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 44), extFlip3, (void **)&pFlip3, "Flip(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 48), extGetAttachedSurface3, (void **)&pGetAttachedSurface3, "GetAttachedSurface(S3)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdds + 52), extGetBltStatus3, (void **)&pGetBltStatus3, "GetBltStatus(S3)");
#endif
        SetHook((void *)(**(DWORD **)lplpdds + 56), extGetCaps3S, (void **)&pGetCaps3S, "GetCaps(S3)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdds + 60), extGetClipper3, (void **)&pGetClipper3, "GetClipper(S3)");
#endif
        SetHook((void *)(**(DWORD **)lplpdds + 64), extGetColorKey3, (void **)&pGetColorKey3, "GetColorKey(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 68), extGetDC3, (void **)&pGetDC3, "GetDC(S3)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdds + 72), extGetFlipStatus3, (void **)&pGetFlipStatus3, "GetFlipStatus(S3)");
#endif
        SetHook((void *)(**(DWORD **)lplpdds + 76), extGetOverlayPosition3, (void **)&pGetOverlayPosition3, "GetOverlayPosition(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 80), extGetPalette3, (void **)&pGetPalette3, "GetPalette(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 84), extGetPixelFormat3, (void **)&pGetPixelFormat3, "GetPixelFormat(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 88), extGetSurfaceDesc3, (void **)&pGetSurfaceDesc3, "GetSurfaceDesc(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 100), extLock3, (void **)&pLock3, "Lock(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 128), extUnlock3, (void **)&pUnlock3, "Unlock(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 104), extReleaseDC3, (void **)&pReleaseDC3, "ReleaseDC(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 120), extSetOverlayPosition3, (void **)&pSetOverlayPosition3, "SetOverlayPosition(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 124), extSetPalette3, (void **)&pSetPalette3, "SetPalette(S3)");
        // added in interface version 3
        SetHook((void *)(**(DWORD **)lplpdds + 156), extSetSurfaceDesc3, (void **)&pSetSurfaceDesc3, "SetSurfaceDesc(S3)");
        break;
    case 4:
        SetHook((void *)(**(DWORD **)lplpdds), extQueryInterfaceS4, (void **)&pQueryInterfaceS4, "QueryInterface(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 8), extReleaseS4, (void **)&pReleaseS4, "Release(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 12), extAddAttachedSurface4, (void **)&pAddAttachedSurface4, "AddAttachedSurface(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 16), extAddOverlayDirtyRect4, (void **)&pAddOverlayDirtyRect4, "AddOverlayDirtyRect(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 20), extBlt4, (void **)&pBlt4, "Blt(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 24), extBltBatch4, (void **)&pBltBatch4, "BltBatch(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 28), extBltFast4, (void **)&pBltFast4, "BltFast(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 32), extDeleteAttachedSurface4, (void **)&pDeleteAttachedSurface4, "DeleteAttachedSurface(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 36), extEnumAttachedSurfaces4, (void **)&pEnumAttachedSurfaces4, "EnumAttachedSurfaces(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 40), extEnumOverlayZOrders4, (void **)&pEnumOverlayZOrders4, "EnumOverlayZOrders(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 44), extFlip4, (void **)&pFlip4, "Flip(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 48), extGetAttachedSurface4, (void **)&pGetAttachedSurface4, "GetAttachedSurface(S4)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdds + 52), extGetBltStatus4, (void **)&pGetBltStatus4, "GetBltStatus(S4)");
#endif
        SetHook((void *)(**(DWORD **)lplpdds + 56), extGetCaps4S, (void **)&pGetCaps4S, "GetCaps(S4)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdds + 60), extGetClipper4, (void **)&pGetClipper4, "GetClipper(S4)");
#endif
        SetHook((void *)(**(DWORD **)lplpdds + 64), extGetColorKey4, (void **)&pGetColorKey4, "GetColorKey(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 68), extGetDC4, (void **)&pGetDC4, "GetDC(S4)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdds + 72), extGetFlipStatus4, (void **)&pGetFlipStatus4, "GetFlipStatus(S4)");
#endif
        SetHook((void *)(**(DWORD **)lplpdds + 76), extGetOverlayPosition4, (void **)&pGetOverlayPosition4, "GetOverlayPosition(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 80), extGetPalette4, (void **)&pGetPalette4, "GetPalette(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 84), extGetPixelFormat4, (void **)&pGetPixelFormat4, "GetPixelFormat(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 88), extGetSurfaceDesc4, (void **)&pGetSurfaceDesc4, "GetSurfaceDesc(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 100), extLock4, (void **)&pLock4, "Lock(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 128), extUnlock4, (void **)&pUnlock4, "Unlock(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 104), extReleaseDC4, (void **)&pReleaseDC4, "ReleaseDC(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 120), extSetOverlayPosition4, (void **)&pSetOverlayPosition4, "SetOverlayPosition(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 124), extSetPalette4, (void **)&pSetPalette4, "SetPalette(S4)");
        // added in interface version 3
        SetHook((void *)(**(DWORD **)lplpdds + 156), extSetSurfaceDesc4, (void **)&pSetSurfaceDesc4, "SetSurfaceDesc(S4)");
        break;
    case 7:
        SetHook((void *)(**(DWORD **)lplpdds), extQueryInterfaceS7, (void **)&pQueryInterfaceS7, "QueryInterface(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 8), extReleaseS7, (void **)&pReleaseS7, "Release(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 12), extAddAttachedSurface7, (void **)&pAddAttachedSurface7, "AddAttachedSurface(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 16), extAddOverlayDirtyRect7, (void **)&pAddOverlayDirtyRect7, "AddOverlayDirtyRect(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 20), extBlt7, (void **)&pBlt7, "Blt(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 24), extBltBatch7, (void **)&pBltBatch7, "BltBatch(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 28), extBltFast7, (void **)&pBltFast7, "BltFast(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 32), extDeleteAttachedSurface7, (void **)&pDeleteAttachedSurface7, "DeleteAttachedSurface(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 36), extEnumAttachedSurfaces7, (void **)&pEnumAttachedSurfaces7, "EnumAttachedSurfaces(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 40), extEnumOverlayZOrders7, (void **)&pEnumOverlayZOrders7, "EnumOverlayZOrders(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 44), extFlip7, (void **)&pFlip7, "Flip(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 48), extGetAttachedSurface7, (void **)&pGetAttachedSurface7, "GetAttachedSurface(S7)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdds + 52), extGetBltStatus7, (void **)&pGetBltStatus7, "GetBltStatus(S7)");
#endif
        SetHook((void *)(**(DWORD **)lplpdds + 56), extGetCaps7S, (void **)&pGetCaps7S, "GetCaps(S7)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdds + 60), extGetClipper7, (void **)&pGetClipper7, "GetClipper(S7)");
#endif
        SetHook((void *)(**(DWORD **)lplpdds + 64), extGetColorKey7, (void **)&pGetColorKey7, "GetColorKey(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 68), extGetDC7, (void **)&pGetDC7, "GetDC(S7)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lplpdds + 72), extGetFlipStatus7, (void **)&pGetFlipStatus7, "GetFlipStatus(S7)");
#endif
        SetHook((void *)(**(DWORD **)lplpdds + 76), extGetOverlayPosition7, (void **)&pGetOverlayPosition7, "GetOverlayPosition(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 80), extGetPalette7, (void **)&pGetPalette7, "GetPalette(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 84), extGetPixelFormat7, (void **)&pGetPixelFormat7, "GetPixelFormat(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 88), extGetSurfaceDesc7, (void **)&pGetSurfaceDesc7, "GetSurfaceDesc(S7)");
        // 92:    STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW, LPDDSURFACEDESC) PURE;
        // 96:	  STDMETHOD(IsLost)(THIS) PURE;
        SetHook((void *)(**(DWORD **)lplpdds + 100), extLock7, (void **)&pLock7, "Lock(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 104), extReleaseDC7, (void **)&pReleaseDC7, "ReleaseDC(S7)");
        // 108:    STDMETHOD(Restore)(THIS) PURE;
        // 112:	   STDMETHOD(SetClipper)(THIS_ LPDIRECTDRAWCLIPPER) PURE;
        // 116:	   STDMETHOD(SetColorKey)(THIS_ DWORD, LPDDCOLORKEY) PURE;
        SetHook((void *)(**(DWORD **)lplpdds + 120), extSetOverlayPosition7, (void **)&pSetOverlayPosition7, "SetOverlayPosition(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 124), extSetPalette7, (void **)&pSetPalette7, "SetPalette(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 128), extUnlock7, (void **)&pUnlock7, "Unlock(S7)");
        // 132:    STDMETHOD(UpdateOverlay)(THIS_ LPRECT, LPDIRECTDRAWSURFACE,LPRECT,DWORD, LPDDOVERLAYFX) PURE;
        // 136:    STDMETHOD(UpdateOverlayDisplay)(THIS_ DWORD) PURE;
        // 140:	   STDMETHOD(UpdateOverlayZOrder)(THIS_ DWORD, LPDIRECTDRAWSURFACE) PURE;
        /*** Added in the v2 interface ***/
        // 144:    STDMETHOD(GetDDInterface)(THIS_ LPVOID FAR *) PURE;
        // 148:    STDMETHOD(PageLock)(THIS_ DWORD) PURE;
        // 152:    STDMETHOD(PageUnlock)(THIS_ DWORD) PURE;
        // added in interface version 3
        SetHook((void *)(**(DWORD **)lplpdds + 156), extSetSurfaceDesc7, (void **)&pSetSurfaceDesc7, "SetSurfaceDesc(S7)");
        /*** Added in the v4 interface ***/
        // 160:    STDMETHOD(SetPrivateData)(THIS_ REFGUID, LPVOID, DWORD, DWORD) PURE;
        // 164:    STDMETHOD(GetPrivateData)(THIS_ REFGUID, LPVOID, LPDWORD) PURE;
        // 168:    STDMETHOD(FreePrivateData)(THIS_ REFGUID) PURE;
        // 172:    STDMETHOD(GetUniquenessValue)(THIS_ LPDWORD) PURE;
        // 176:    STDMETHOD(ChangeUniquenessValue)(THIS) PURE;
        break;
    }
    if (!isPrim) return;
    switch(dxversion) {
    case 1:
        SetHook((void *)(**(DWORD **)lplpdds + 112), extSetClipper1, (void **)&pSetClipper1, "SetClipper(S1)");
        SetHook((void *)(**(DWORD **)lplpdds + 116), extSetColorKey1, (void **)&pSetColorKey1, "SetColorKey(S1)");
        break;
    case 2:
        SetHook((void *)(**(DWORD **)lplpdds + 112), extSetClipper2, (void **)&pSetClipper2, "SetClipper(S2)");
        SetHook((void *)(**(DWORD **)lplpdds + 116), extSetColorKey2, (void **)&pSetColorKey2, "SetColorKey(S2)");
        break;
    case 3:
        SetHook((void *)(**(DWORD **)lplpdds + 112), extSetClipper3, (void **)&pSetClipper3, "SetClipper(S3)");
        SetHook((void *)(**(DWORD **)lplpdds + 116), extSetColorKey3, (void **)&pSetColorKey3, "SetColorKey(S3)");
        break;
    case 4:
        SetHook((void *)(**(DWORD **)lplpdds + 112), extSetClipper4, (void **)&pSetClipper4, "SetClipper(S4)");
        SetHook((void *)(**(DWORD **)lplpdds + 116), extSetColorKey4, (void **)&pSetColorKey4, "SetColorKey(S4)");
        break;
    case 7:
        SetHook((void *)(**(DWORD **)lplpdds + 112), extSetClipper7, (void **)&pSetClipper7, "SetClipper(S7)");
        SetHook((void *)(**(DWORD **)lplpdds + 116), extSetColorKey7, (void **)&pSetColorKey7, "SetColorKey(S7)");
        break;
    }
}


void HookGammaControl(LPVOID *obp) {
    // IDirectDrawGammaControl::GetGammaRamp
    SetHook((void *)(**(DWORD **)obp + 12), extDDGetGammaRamp, (void **)&pDDGetGammaRamp, "GetGammaRamp(G)");
    // IDirectDrawGammaControl::SetGammaRamp
    SetHook((void *)(**(DWORD **)obp + 16), extDDSetGammaRamp, (void **)&pDDSetGammaRamp, "SetGammaRamp(G)");
}

/* ------------------------------------------------------------------------------ */
// CleanRect:
// takes care of a corrupted RECT struct where some elements are not valid pointers.
// In this case, the whole RECT * variable is set to NULL, a value that is interpreted
// by directdraw functions as the whole surface area.
/* ------------------------------------------------------------------------------ */

static void CleanRect(RECT **lprect, int line) {
    __try {
        // normally unharmful statements
        if(*lprect) {
            int i;
            i = (*lprect)->bottom;
            i = (*lprect)->top;
            i = (*lprect)->left;
            i = (*lprect)->right;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        OutTraceE("Rectangle exception caught at %d: invalid RECT\n", __LINE__);
        if(IsAssertEnabled) MessageBox(0, "Rectangle exception", "CleanRect", MB_OK | MB_ICONEXCLAMATION);
        *lprect = NULL;
    }
}

static void MaskCapsD(LPDDCAPS c1, LPDDCAPS c2) {
    FILE *capfile;
    //char sBuf[80+1];
    DWORD val;
#ifndef DXW_NOTRACES
    if(IsDebugDW) {
        char CapsBuf[3 * sizeof(DDCAPS_DX7) + 20];
        int i;
        unsigned char *p;
        if(c1) {
            p = (unsigned char *)c1;
            for(i = 0; i < sizeof(DDCAPS_DX7); i++) sprintf(&CapsBuf[3 * i], "%02.2X,", p[i]);
            OutTrace("CapsDump(c1:HW)=%s\n", CapsBuf);
        }
        if(c2) {
            p = (unsigned char *)c2;
            for(i = 0; i < sizeof(DDCAPS_DX7); i++) sprintf(&CapsBuf[3 * i], "%02.2X,", p[i]);
            OutTrace("CapsDump(c2:SW)=%s\n", CapsBuf);
        }
    }
#endif // DXW_NOTRACES
    capfile = fopen("dxwnd.caps.txt", "r");
    if(!capfile) return;
    int HwSwCap;
    int WordOffset;
    char Operator;
    LPDDCAPS Caps;
    LPDWORD lpDWord;
    while(TRUE) {
        if(fscanf(capfile, "C%d.%#x[%c]=%#x\n", &HwSwCap, &WordOffset, &Operator, &val) != 4) break;
        OutDebugDW("MaskCaps: C%d.%#x[%c].x%08.8X\n", HwSwCap, WordOffset, Operator, val);
        Caps = (HwSwCap == 1) ? c1 : c2;
        if(!Caps) continue;
        lpDWord = &(Caps->dwSize) + WordOffset;
        switch(Operator) {
        case '&':
            *lpDWord &= val;
            break;
        case '|':
            *lpDWord |= val;
            break;
        case '-':
            *lpDWord &= (~val);
            break;
        case '=':
        default:
            *lpDWord = val;
            break;
        }
    }
    fclose(capfile);
#ifndef DXW_NOTRACES
    if(c1) {
        OutTrace("MaskCaps(D-HW): caps=%#x(%s) caps2=%#x(%s) fxcaps=%#x(%s) fxalphacaps=%#x(%s) keycaps=%#x(%s)\n",
                 c1->dwCaps, ExplainDDDCaps(c1->dwCaps),
                 c1->dwCaps2, ExplainDDDCaps2(c1->dwCaps2),
                 c1->dwFXCaps, ExplainDDFXCaps(c1->dwFXCaps),
                 c1->dwFXAlphaCaps, ExplainDDFXALPHACaps(c1->dwFXAlphaCaps),
                 c1->dwCKeyCaps, ExplainDDCKeyCaps(c1->dwCKeyCaps));
    }
    if(c2) {
        OutTrace("MaskCaps(D-SW): caps=%#x(%s) caps2=%#x(%s) fxcaps=%#x(%s) fxalphacaps=%#x(%s) keycaps=%#x(%s)\n",
                 c2->dwCaps, ExplainDDDCaps(c2->dwCaps),
                 c2->dwCaps2, ExplainDDDCaps2(c2->dwCaps2),
                 c2->dwFXCaps, ExplainDDFXCaps(c2->dwFXCaps),
                 c2->dwFXAlphaCaps, ExplainDDFXALPHACaps(c2->dwFXAlphaCaps),
                 c2->dwCKeyCaps, ExplainDDCKeyCaps(c2->dwCKeyCaps));
    }
#endif // DXW_NOTRACES
}

/* ------------------------------------------------------------------------------ */
// directdraw method hooks
/* ------------------------------------------------------------------------------ */

#ifndef DXW_NOTRACES
static void TraceCapsD(char *sLabel, LPDDCAPS c) {
    OutTraceDDRAW(
        "GetCaps(%s): caps=%#x(%s) caps2=%#x(%s) palcaps=%#x(%s) "
        "fxcaps=%#x(%s) fxalphacaps=%#x(%s) keycaps=%#x(%s)\n",
        sLabel,
        c->dwCaps, ExplainDDDCaps(c->dwCaps),
        c->dwCaps2, ExplainDDDCaps2(c->dwCaps2),
        c->dwPalCaps, ExplainDDPalCaps(c->dwPalCaps),
        c->dwFXCaps, ExplainDDFXCaps(c->dwFXCaps),
        c->dwFXAlphaCaps, ExplainDDFXALPHACaps(c->dwFXAlphaCaps),
        c->dwCKeyCaps, ExplainDDCKeyCaps(c->dwCKeyCaps));
    OutTraceDDRAW("GetCaps(%s): VidMemTotal=%#x VidMemFree=%#x ZBufferBitDepths=%#x(%s)\n",
                  sLabel, c->dwVidMemTotal, c->dwVidMemFree, c->dwZBufferBitDepths, ExplainZBufferBitDepths(c->dwZBufferBitDepths));
    OutTraceDDRAW("GetCaps(%s): MaxVisibleOverlays=%#x CurrVisibleOverlays=%#x\n",
                  sLabel, c->dwMaxVisibleOverlays, c->dwCurrVisibleOverlays);
    OutHexDDRAW((unsigned char *)c, c->dwSize);
}
#endif

static BOOL HandleCapsD(char *sLabel, LPDDCAPS c) {
    BOOL updated = FALSE;
    const DWORD dwMaxMem  = 0x70000000;
    const DWORD dwHugeMem = 0xFFFFFFFF;
    if(dxw.bHintActive) {
        if(c->dwVidMemTotal > dwMaxMem) ShowHint(HINT_LIMITMEM);
        if(c->dwVidMemFree  > dwMaxMem) ShowHint(HINT_LIMITMEM);
    }
    if(dxw.dwFlags2 & LIMITRESOURCES) { // check for memory value overflow
        if(c->dwVidMemTotal > dwMaxMem) {
            c->dwVidMemTotal = dwMaxMem;
            updated = TRUE;
        }
        if(c->dwVidMemFree  > dwMaxMem) {
            c->dwVidMemFree  = dwMaxMem;
            updated = TRUE;
        }
    }
    if(dxw.dwDFlags & STRESSRESOURCES) {
        c->dwVidMemTotal = dwHugeMem;
        c->dwVidMemFree  = dwHugeMem;
        updated = TRUE;
    }
    if(dxw.dwFlags6 & SETZBUFFERBITDEPTHS) {
        // From MSDN https://msdn.microsoft.com/en-us/library/windows/desktop/gg426101%28v=vs.85%29.aspx :
        // dwZBufferBitDepths
        // DDBD_8, DDBD_16, DDBD_24, or DDBD_32. (Indicate 8, 16, 24, or 32 bits per pixel.)
        // This member is obsolete for DirectX 6.0 and later.
        // Use the IDirect3D7::EnumZBufferFormats to retrieve information about supported depth buffer formats.
        // v2.03.35: needed to bypass the "Banzai Bug" initial controls
        if (c->dwZBufferBitDepths == 0) {
            c->dwZBufferBitDepths = (DDBD_8 | DDBD_16 | DDBD_24 | DDBD_32);
            OutTraceDW("GetCaps(%s): FIXED ZBufferBitDepths=%#x(%s)\n",
                       sLabel, c->dwZBufferBitDepths, ExplainZBufferBitDepths(c->dwZBufferBitDepths));
            updated = TRUE;
        }
    }
    // v2.04.67: needed (either 16 or 24 bit) to bypass "3D Slot Car Racing" bogus controls.
    if(dxw.dwFlags3 & (SETZBUFFER16BIT | SETZBUFFER24BIT)) {
        if(dxw.dwFlags3 & SETZBUFFER16BIT) c->dwZBufferBitDepths = DDBD_16;
        if(dxw.dwFlags3 & SETZBUFFER24BIT) c->dwZBufferBitDepths = DDBD_24;
        OutTraceDW("GetCaps(%s): FIXED ZBufferBitDepths=%#x(%s)\n",
                   sLabel, c->dwZBufferBitDepths, ExplainZBufferBitDepths(c->dwZBufferBitDepths));
        updated = TRUE;
    }
#ifndef DXW_NOTRACES
    if((dxw.dwDFlags & STRESSRESOURCES) || (dxw.dwFlags2 & LIMITRESOURCES))
        OutTraceDW("GetCaps(%s): FIXED VidMemTotal=%#x VidMemFree=%#x\n", sLabel, c->dwVidMemTotal, c->dwVidMemFree);
#endif // DXW_NOTRACES
    return updated;
}

static HRESULT WINAPI extGetCapsD(int dxversion, GetCapsD_Type pGetCapsD, LPDIRECTDRAW lpdd, LPDDCAPS c1, LPDDCAPS c2) {
    HRESULT res;
    BOOL c1Updated, c2Updated;
    c1Updated = c2Updated = FALSE;
    OutTraceDDRAW("GetCaps(D%d): lpdd=%#x %s %s\n", dxversion, lpdd, c1 ? "c1" : "NULL", c2 ? "c2" : "NULL");
    res = (*pGetCapsD)(lpdd, c1, c2);
    if(res) {
        OutTraceE("GetCaps(D): ERROR res=%#x(%s)\n", res, ExplainDDError(res));
        return res;
    }
    if (c1) {
        TraceCapsD("D-HW", c1);
        c1Updated = HandleCapsD("D-HW", c1);
    }
    if (c2)  {
        TraceCapsD("D-SW", c2);
        c2Updated = HandleCapsD("D-HW", c2);
    }
    if((dxw.dwFlags3 & FORCESHEL) && c1) {
        DDCAPS_DX7 swcaps; // DDCAPS_DX7 because it is the bigger in size
        int size;
        size = c1->dwSize;
        if (!c2) {
            memset(&swcaps, 0, sizeof(DDCAPS_DX7));
            swcaps.dwSize = size;
            c2 = &swcaps;
            res = (*pGetCapsD)(lpdd, NULL, c2);
        }
        DWORD dwVidMemTotal = c1->dwVidMemTotal;
        DWORD dwVidMemFree = c1->dwVidMemFree;
        memcpy((void *)c1, (void *)c2, size);
        if(c1->dwVidMemTotal == 0) c1->dwVidMemTotal = dwVidMemTotal;
        if(c1->dwVidMemFree  == 0) c1->dwVidMemFree = dwVidMemFree;
        if(c1->dwVidMemTotal == 0) c1->dwVidMemTotal = 0x20000000; // 500 MB
        if(c1->dwVidMemFree  == 0) c1->dwVidMemFree = 0x20000000; // 500 MB
        if (c1) HandleCapsD("D-HW(fixed)", c1);
        if (c2) HandleCapsD("D-SW(fixed)", c2);
        c1Updated = TRUE;
    }
    // v2.04.82: fixed mismatch between dwCaps vs. ddsCaps.dwCaps and dwCaps2 vs. ddsCaps.dwCaps2
    if((dxw.dwFlags8 & NOHALDEVICE) && c1) {
        OutTraceDW("GetCaps(D): NOHALDEVICE\n");
        c1->dwCaps = DDCAPS_NOHARDWARE;
        c1->dwCaps2 = DDCAPS2_CANRENDERWINDOWED;
        c1->dwPalCaps = 0;
        c1->dwFXCaps = 0;
        c1->dwFXAlphaCaps = 0;
        c1->dwCKeyCaps = 0;
        c1->dwVidMemTotal = c1->dwVidMemFree = 0;
        c1->dwZBufferBitDepths = 0;
        c1->dwMaxVisibleOverlays = c1->dwCurrVisibleOverlays = 0;
        HandleCapsD("D-HW(NoHAL)", c1);
        c1Updated = TRUE;
    }
    if((dxw.dwFlags3 & MINIMALCAPS)) {
        SetMinimalCaps(dxversion, c1, c2);
        c1Updated = c2Updated = TRUE;
    }
    if(dxw.dwDFlags & CAPMASK) {
        MaskCapsD(c1, c2);
        c1Updated = c2Updated = TRUE;
    }
#define OVERLAYLAYERCAPS \
	(DDCAPS_OVERLAY|DDCAPS_OVERLAYCANTCLIP|\
	DDCAPS_OVERLAYFOURCC|DDCAPS_OVERLAYSTRETCH)
#define OVERLAYKEYCAPS \
	(DDCKEYCAPS_DESTOVERLAY|DDCKEYCAPS_DESTOVERLAYYUV|\
	DDCKEYCAPS_SRCOVERLAY|DDCKEYCAPS_SRCOVERLAYYUV|\
	DDCKEYCAPS_SRCOVERLAYCLRSPACE|DDCKEYCAPS_SRCOVERLAYCLRSPACEYUV)
    if(dxw.dwFlags7 & SUPPRESSOVERLAY) {
        if(c1) {
            c1->dwCaps &= ~OVERLAYLAYERCAPS;
            c1->dwCKeyCaps &= ~OVERLAYKEYCAPS;
            c1->dwMaxVisibleOverlays = c1->dwCurrVisibleOverlays = 0;
            c1Updated = TRUE;
        }
        if(c2) {
            c2->dwCaps &= ~OVERLAYLAYERCAPS;
            c2->dwCKeyCaps &= ~OVERLAYKEYCAPS;
            c2->dwMaxVisibleOverlays = c2->dwCurrVisibleOverlays = 0;
            c2Updated = TRUE;
        }
    }
    // v2.04.82: SUPPRESSFOURCCBLT flag
#define FOURCCCAPS (DDCAPS_BLTFOURCC | DDCAPS_OVERLAYFOURCC)
#define FOURCCCAPS2 DDCAPS2_COPYFOURCC
    if(dxw.dwFlags10 & SUPPRESSFOURCCBLT) {
        if(c1) {
            c1->dwCaps &= ~FOURCCCAPS;
            c1->dwCaps2 &= ~FOURCCCAPS2;
            c1Updated = TRUE;
        }
        if(c2) {
            c2->dwCaps &= ~FOURCCCAPS;
            c2->dwCaps2 &= ~FOURCCCAPS2;
            c2Updated = TRUE;
        }
    }
#ifndef DXW_NOTRACES
    if (c1 && c1Updated) TraceCapsD("Upd.D-HW", c1);
    if (c2 && c2Updated) TraceCapsD("Upd.D-SW", c2);
#endif // DXW_NOTRACES
    return res;
}

HRESULT WINAPI extGetCaps1D(LPDIRECTDRAW lpdd, LPDDCAPS c1, LPDDCAPS c2) {
    return extGetCapsD(1, pGetCaps1D, lpdd, c1, c2);
}
HRESULT WINAPI extGetCaps2D(LPDIRECTDRAW lpdd, LPDDCAPS c1, LPDDCAPS c2) {
    return extGetCapsD(2, pGetCaps2D, lpdd, c1, c2);
}
HRESULT WINAPI extGetCaps3D(LPDIRECTDRAW lpdd, LPDDCAPS c1, LPDDCAPS c2) {
    return extGetCapsD(3, pGetCaps3D, lpdd, c1, c2);
}
HRESULT WINAPI extGetCaps4D(LPDIRECTDRAW lpdd, LPDDCAPS c1, LPDDCAPS c2) {
    return extGetCapsD(4, pGetCaps4D, lpdd, c1, c2);
}
HRESULT WINAPI extGetCaps7D(LPDIRECTDRAW lpdd, LPDDCAPS_DX7 c1, LPDDCAPS_DX7 c2) {
    return extGetCapsD(7, pGetCaps7D, lpdd, (LPDDCAPS)c1, (LPDDCAPS)c2);
}

HRESULT WINAPI extDirectDrawCreate(GUID FAR *lpguid, LPDIRECTDRAW FAR *lplpdd, IUnknown FAR *pu) {
    HRESULT res;
    ApiName("DirectDrawCreate");
    GUID FAR *lpPrivGuid = lpguid;
    OutTraceDDRAW("%s: guid=%#x(%s) pu=%#x\n", ApiRef, lpguid, ExplainGUID(lpguid), pu);
    if(!pDirectDrawCreate) { // not hooked yet....
        HINSTANCE hinst;
        hinst = (*pLoadLibraryA)("ddraw.dll");
        if(!hinst)
            OutTraceE("%s: LoadLibrary ddraw.dll ERROR err=%d at %d\n", ApiRef, GetLastError(), __LINE__);
        pDirectDrawCreate =
            (DirectDrawCreate_Type)GetProcAddress(hinst, "DirectDrawCreate");
        if(pDirectDrawCreate)
            HookAPI(hinst, "ddraw.dll", pDirectDrawCreate, "DirectDrawCreate", extDirectDrawCreate); // v2.02.52
        else {
            char sMsg[81];
            sprintf_s(sMsg, 80, "%s hook failed: error=%d\n", ApiRef, GetLastError());
            OutTraceDW(sMsg);
            if(IsAssertEnabled) MessageBox(0, sMsg, "Hook", MB_OK | MB_ICONEXCLAMATION);
            return DDERR_GENERIC; // is there a better one?
        }
    }
    if(dxw.dwFlags3 & FORCESHEL) lpPrivGuid = (GUID FAR *)DDCREATE_EMULATIONONLY;
    if(dxw.dwFlags11 & FORCESHAL) lpPrivGuid = (GUID FAR *)DDCREATE_HARDWAREONLY;
    if(dxw.dwFlags11 & FORCESNULL) lpPrivGuid = (GUID FAR *)NULL;
    if(!dxw.Windowize && (dxw.MonitorId != -1)) {
        lpPrivGuid = dxw.GetForcedMonitorGUID();
        OutTraceDW("%s: forcing sguid=%s\n", ApiRef, sGUID(lpPrivGuid));
    }
    res = (*pDirectDrawCreate)(lpPrivGuid, lplpdd, pu);
    if(res) {
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    if(dxw.dwFlags3 & COLORFIX) (*((DDRAWI_DIRECTDRAW_INT **)lplpdd))->lpLcl->dwAppHackFlags |= 0x800;
    dxw.dwDDVersion = 1;
    char *mode;
    switch ((DWORD)lpPrivGuid) {
    case 0:
        mode = "NULL";
        break;
    case DDCREATE_HARDWAREONLY:
        mode = "DDCREATE_HARDWAREONLY";
        break;
    case DDCREATE_EMULATIONONLY:
        mode = "DDCREATE_EMULATIONONLY";
        break;
    default:
        switch (*(DWORD *)lpPrivGuid) {
        case 0x6C14DB80:
            dxw.dwDDVersion = 1;
            mode = "IID_IDirectDraw";
            break;
        case 0xB3A6F3E0:
            dxw.dwDDVersion = 2;
            mode = "IID_IDirectDraw2";
            break;
        case 0x618f8ad4:
            dxw.dwDDVersion = 3;
            mode = "IID_IDirectDraw3";
            break;
        case 0x9c59509a:
            dxw.dwDDVersion = 4;
            mode = "IID_IDirectDraw4";
            break;
        case 0x15e65ec0:
            dxw.dwDDVersion = 7;
            mode = "IID_IDirectDraw7";
            break;
        default:
            mode = "unknown";
            break;
        }
        break;
    }
    OutTraceDDRAW("%s: lpdd=%#x guid=%s DDVersion=%d\n", ApiRef, *lplpdd, mode, dxw.dwDDVersion);
    HookDDSession(lplpdd, dxw.dwDDVersion);
    // beware: old ddraw games seem to reuse a closed ddraw handle. The theory is that maybe whenever
    // you created a ddraw session, you got more than one reference count, so that releasing it once
    // did not destroy the object.... but why? when (I mean, in which situations? Maybe not always...)
    // and for which releases? The fix here assumes that this fact is true and holds always for ddraw 1
    // sessions.
    // update: this is no good for "Jet Moto" that created a ddraw session and expects to Release it
    // with a refcount zero.
    //if(dxw.dwDDVersion==1) (*lplpdd)->AddRef(); // seems to fix problems in "Warhammer 40K Rites Of War"
    memset(&dxw.DXCapsD1, 0, sizeof(dxw.DXCapsD1));
    dxw.DXCapsD1.dwSize = sizeof(dxw.DXCapsD1);
    memset(&dxw.DXCapsD2, 0, sizeof(dxw.DXCapsD2));
    dxw.DXCapsD2.dwSize = sizeof(dxw.DXCapsD2);
    res = ((LPDIRECTDRAW)(*lplpdd))->GetCaps(&dxw.DXCapsD1, &dxw.DXCapsD2); // logging already inside extGetCapsD
    if(lpPrimaryDD == NULL) lpPrimaryDD = *lplpdd; // do not override the value set when creating the primary surface!
    bFlippedDC = FALSE; // v02.03.30
    return DD_OK;
}

HRESULT WINAPI extDirectDrawCreateEx(GUID FAR *lpguid,
                                     LPDIRECTDRAW FAR *lplpdd, REFIID iid, IUnknown FAR *pu) {
    HRESULT res;
    ApiName("DirectDrawCreateEx");
    GUID FAR *lpPrivGuid = lpguid;
    OutTraceDDRAW("%s: guid=%#x(%s) refiid=%#x\n", ApiRef, lpguid, ExplainGUID(lpguid), iid);
    // v2.1.70: auto-hooking (just in case...)
    if(!pDirectDrawCreateEx) { // not hooked yet....
        HINSTANCE hinst;
        hinst = (*pLoadLibraryA)("ddraw.dll");
        if(!hinst)
            OutTraceE("%s: LoadLibrary ddraw.dll ERROR err=%d at %d\n", ApiRef, GetLastError(), __LINE__);
        pDirectDrawCreateEx =
            (DirectDrawCreateEx_Type)GetProcAddress(hinst, "DirectDrawCreateEx");
        if(pDirectDrawCreateEx)
            HookAPI(hinst, "ddraw.dll", pDirectDrawCreateEx, "DirectDrawCreateEx", extDirectDrawCreateEx); // v2.02.52
        else {
            char sMsg[81];
            sprintf_s(sMsg, 80, "%s hook failed: error=%d\n", ApiRef, GetLastError());
            OutTraceDW(sMsg);
            if(IsAssertEnabled) MessageBox(0, sMsg, "Hook", MB_OK | MB_ICONEXCLAMATION);
            return DDERR_GENERIC; // is there a better one?
        }
    }
    if(dxw.dwFlags3 & FORCESHEL) lpPrivGuid = (GUID FAR *)DDCREATE_EMULATIONONLY;
    if(dxw.dwFlags11 & FORCESHAL) lpPrivGuid = (GUID FAR *)DDCREATE_HARDWAREONLY;
    if(dxw.dwFlags11 & FORCESNULL) lpPrivGuid = (GUID FAR *)NULL;
    if(!dxw.Windowize && (dxw.MonitorId != -1)) {
        lpPrivGuid = dxw.GetForcedMonitorGUID();
        OutTraceDW("%s: forcing sguid=%s\n", ApiRef, sGUID(lpPrivGuid));
    }
    res = (*pDirectDrawCreateEx)(lpPrivGuid, lplpdd, iid, pu);
    if (res) {
        OutTraceDW("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    if(dxw.dwFlags3 & COLORFIX) (*((DDRAWI_DIRECTDRAW_INT **)lplpdd))->lpLcl->dwAppHackFlags |= 0x800;
    dxw.dwDDVersion = 7;
    char *mode;
    switch ((DWORD)lpPrivGuid) {
    case 0:
        mode = "NULL";
        break;
    case DDCREATE_HARDWAREONLY:
        mode = "DDCREATE_HARDWAREONLY";
        break;
    case DDCREATE_EMULATIONONLY:
        mode = "DDCREATE_EMULATIONONLY";
        break;
    default:
        switch (*(DWORD *)lpPrivGuid) {
        case 0x6C14DB80:
            dxw.dwDDVersion = 1;
            mode = "IID_IDirectDraw";
            break;
        case 0xB3A6F3E0:
            dxw.dwDDVersion = 2;
            mode = "IID_IDirectDraw2";
            break;
        case 0x618f8ad4:
            dxw.dwDDVersion = 3;
            mode = "IID_IDirectDraw3";
            break;
        case 0x9c59509a:
            dxw.dwDDVersion = 4;
            mode = "IID_IDirectDraw4";
            break;
        case 0x15e65ec0:
            dxw.dwDDVersion = 7;
            mode = "IID_IDirectDraw7";
            break;
        default:
            mode = "unknown";
            break;
        }
        break;
    }
    OutTraceDDRAW("%s: lpdd=%#x guid=%s DDVersion=%d\n", ApiRef, *lplpdd, mode, dxw.dwDDVersion);
    HookDDSession(lplpdd, dxw.dwDDVersion);
    if(IsDebugDW) {
        DDCAPS DriverCaps, EmulCaps;
        memset(&DriverCaps, 0, sizeof(DriverCaps));
        DriverCaps.dwSize = sizeof(DriverCaps);
        memset(&EmulCaps, 0, sizeof(EmulCaps));
        EmulCaps.dwSize = sizeof(EmulCaps);
        (LPDIRECTDRAW)(*lplpdd)->GetCaps(&DriverCaps, &EmulCaps);
        //OutTrace("DirectDrawCreate: drivercaps=%#x(%s) emulcaps=%#x(%s)\n", DriverCaps.ddsCaps, "???", EmulCaps.ddsCaps, "???");
    }
    if(lpPrimaryDD == NULL) lpPrimaryDD = *lplpdd; // do not override the value set when creating the primary surface!
    bFlippedDC = FALSE; // v02.03.30: fix for Powerslide
    return DD_OK;
}

static HRESULT WINAPI extInitialize(int dxversion, Initialize_Type pInitialize, LPDIRECTDRAW lpdd, GUID FAR *lpguid) {
    HRESULT res;
    ApiName("Initialize");
    GUID FAR *lpPrivGuid = lpguid;
    OutTraceDDRAW("%s(%d): lpdd=%#x guid=%#x(%s)\n", ApiRef, dxversion, lpdd, lpguid, ExplainGUID(lpguid));
    if((dxw.dwFlags3 & FORCESHEL) && (lpguid == NULL)) lpPrivGuid = (GUID FAR *)DDCREATE_EMULATIONONLY;
    res = (*pInitialize)(lpdd, lpPrivGuid);
    if(res == DDERR_ALREADYINITIALIZED) {
        OutTrace("%s: SKIP DDERR_ALREADYINITIALIZED error\n", ApiRef);
        res = DD_OK; // v2.03.18: this error could be caused by the CoCreateInstance implementation
    }
    if(dxw.dwFlags3 & COLORFIX) (((DDRAWI_DIRECTDRAW_INT *)lpdd))->lpLcl->dwAppHackFlags |= 0x800;
    _if(res) OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extInitialize1(LPDIRECTDRAW lpdd, GUID FAR *lpguid) {
    return extInitialize(1, pInitialize1, lpdd, lpguid);
}
HRESULT WINAPI extInitialize2(LPDIRECTDRAW lpdd, GUID FAR *lpguid) {
    return extInitialize(2, pInitialize2, lpdd, lpguid);
}
HRESULT WINAPI extInitialize3(LPDIRECTDRAW lpdd, GUID FAR *lpguid) {
    return extInitialize(3, pInitialize3, lpdd, lpguid);
}
HRESULT WINAPI extInitialize4(LPDIRECTDRAW lpdd, GUID FAR *lpguid) {
    return extInitialize(4, pInitialize4, lpdd, lpguid);
}
HRESULT WINAPI extInitialize7(LPDIRECTDRAW lpdd, GUID FAR *lpguid) {
    return extInitialize(7, pInitialize7, lpdd, lpguid);
}

HRESULT WINAPI extGetScanLine(int dxversion, GetScanLine_Type pGetScanLine, LPDIRECTDRAW lpdd, LPDWORD lpdwScanLine) {
    HRESULT res;
    ApiName("GetScanLine");
    res = pGetScanLine(lpdd, lpdwScanLine);
#ifdef GETSCANLINETRACE
    OutDebugDW("%s(%d): lpdd=%#x res=%#x line=%d\n", ApiRef, dxversion, lpdd, res, *lpdwScanLine);
#endif
    if(!res && dxw.Windowize && dxw.dwRealScreenHeight) {
        *lpdwScanLine = (*lpdwScanLine * dxw.GetScreenHeight()) / dxw.dwRealScreenHeight;
#ifdef GETSCANLINETRACE
        OutDebugDW("%s(%d): FIXED line=%d\n", ApiRef, dxversion, *lpdwScanLine);
#endif
    }
    return res;
}

HRESULT WINAPI extGetScanLine1(LPDIRECTDRAW lpdd, LPDWORD lpdwScanLine) {
    return extGetScanLine(1, pGetScanLine1, lpdd, lpdwScanLine);
}
HRESULT WINAPI extGetScanLine2(LPDIRECTDRAW lpdd, LPDWORD lpdwScanLine) {
    return extGetScanLine(2, pGetScanLine2, lpdd, lpdwScanLine);
}
HRESULT WINAPI extGetScanLine3(LPDIRECTDRAW lpdd, LPDWORD lpdwScanLine) {
    return extGetScanLine(3, pGetScanLine3, lpdd, lpdwScanLine);
}
HRESULT WINAPI extGetScanLine4(LPDIRECTDRAW lpdd, LPDWORD lpdwScanLine) {
    return extGetScanLine(4, pGetScanLine4, lpdd, lpdwScanLine);
}
HRESULT WINAPI extGetScanLine7(LPDIRECTDRAW lpdd, LPDWORD lpdwScanLine) {
    return extGetScanLine(7, pGetScanLine7, lpdd, lpdwScanLine);
}

HRESULT WINAPI extQueryInterfaceD1(void *lpdd, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceDX(1, pQueryInterfaceD1, lpdd, riid, obp);
}
HRESULT WINAPI extQueryInterfaceD2(void *lpdd, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceDX(2, pQueryInterfaceD2, lpdd, riid, obp);
}
HRESULT WINAPI extQueryInterfaceD3(void *lpdd, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceDX(3, pQueryInterfaceD3, lpdd, riid, obp);
}
HRESULT WINAPI extQueryInterfaceD4(void *lpdd, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceDX(4, pQueryInterfaceD4, lpdd, riid, obp);
}
HRESULT WINAPI extQueryInterfaceD7(void *lpdd, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceDX(7, pQueryInterfaceD7, lpdd, riid, obp);
}

HRESULT WINAPI extQueryInterfaceS1(void *lpdds, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceDX(1, pQueryInterfaceS1, lpdds, riid, obp);
}
HRESULT WINAPI extQueryInterfaceS2(void *lpdds, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceDX(2, pQueryInterfaceS2, lpdds, riid, obp);
}
HRESULT WINAPI extQueryInterfaceS3(void *lpdds, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceDX(3, pQueryInterfaceS3, lpdds, riid, obp);
}
HRESULT WINAPI extQueryInterfaceS4(void *lpdds, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceDX(4, pQueryInterfaceS4, lpdds, riid, obp);
}
HRESULT WINAPI extQueryInterfaceS7(void *lpdds, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceDX(7, pQueryInterfaceS7, lpdds, riid, obp);
}

HRESULT WINAPI extSetDisplayMode(int dxversion, LPDIRECTDRAW lpdd,
                                 DWORD dwwidth, DWORD dwheight, DWORD dwbpp, DWORD dwrefreshrate, DWORD dwflags) {
    DDSURFACEDESC2 ddsd;
    ApiName("SetDisplayMode");
    HRESULT res = 0;
    if(IsTraceDDRAW) {
        char sInfo[81];
        strcpy(sInfo, "");
        if (dxversion >= 2) sprintf(sInfo, " dwRefresh=%i dwFlags=%#x", dwrefreshrate, dwflags);
        OutTrace("%s(%d): dwWidth=%i dwHeight=%i dwBPP=%i%s\n",
                 ApiRef, dxversion, dwwidth, dwheight, dwbpp, sInfo);
    }
    // save for later use in RestoreDisplayMode wrapper
    if(dxw.IsEmulated) {
        dxw.dxwLastDisplayWidth = dxw.GetScreenWidth();
        dxw.dxwLastDisplayHeight = dxw.GetScreenHeight();
        dxw.dxwLastDisplayBPP = dxw.VirtualPixelFormat.dwRGBBitCount;
    }
    // binkplayer fix
    if((int)dwwidth < 0) dwwidth = dxw.GetScreenWidth();
    if((int)dwheight < 0) dwheight = dxw.GetScreenHeight();
    // v2.03.90: add LOCKCOLORDEPTH, easiest way to manage "Ahlgrens bisplet"!
    if(dxw.dwFlags7 & LOCKCOLORDEPTH) {
        OutTraceDW("%s: LOCKCOLORDEPTH bpp=%d->%d\n", ApiRef, dwbpp, dxw.ActualPixelFormat.dwRGBBitCount);
        dwbpp = dxw.ActualPixelFormat.dwRGBBitCount;
    }
    if((dxw.dwFlags1 & LOCK24BITDEPTH) && (dwbpp == 24)) {
        OutTraceDW("%s: SKIP24BPP bpp=24->%d\n", ApiRef, dxw.ActualPixelFormat.dwRGBBitCount);
        dwbpp = dxw.ActualPixelFormat.dwRGBBitCount;
    }
    dxw.SetScreenSize(dwwidth, dwheight);
    GetHookInfo()->Height = (short)dxw.GetScreenHeight();
    GetHookInfo()->Width = (short)dxw.GetScreenWidth();
    GetHookInfo()->ColorDepth = (short)dwbpp;
    if(dxw.Windowize) dxw.AdjustWindowFrame(dxw.GethWnd(), dwwidth, dwheight);
    if(dxw.IsEmulated) {
        // in emulation mode, let SetPixFmt decide upon the PixelFormat
        dxw.VirtualPixelFormat.dwRGBBitCount = dwbpp;
        memset(&ddsd, 0, sizeof(ddsd));
        ddsd.dwSize = sizeof(ddsd);
        SetPixFmt(&ddsd);
        SetBltTransformations();
        if(dxw.Windowize) {
            OutTraceDW("%s: mode=EMULATE %s ret=OK\n", ApiRef, DumpPixelFormat(&ddsd));
            SetVSyncDelays(dxversion, lpdd);
            return DD_OK;
        }
    } else {
        OutTraceDW("%s: mode=STANDARD BPP=%d\n", ApiRef, dwbpp);
        dxw.VirtualPixelFormat.dwRGBBitCount = dwbpp;
        dxw.ActualPixelFormat.dwRGBBitCount = dwbpp;
    }
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_REFRESHRATE;
    ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
    switch(dxversion) {
    default:
    case 1:
        ddsd.dwSize = sizeof(LPDDSURFACEDESC);
        res = (*pGetDisplayMode1)(lpdd, (LPDDSURFACEDESC)&ddsd);
        break;
    case 2:
        ddsd.dwSize = sizeof(LPDDSURFACEDESC);
        res = (*pGetDisplayMode2)(lpdd, (LPDDSURFACEDESC)&ddsd);
        break;
    case 3:
        ddsd.dwSize = sizeof(LPDDSURFACEDESC);
        res = (*pGetDisplayMode3)(lpdd, (LPDDSURFACEDESC)&ddsd);
        break;
    case 4:
        ddsd.dwSize = sizeof(LPDDSURFACEDESC2);
        res = (*pGetDisplayMode4)(lpdd, &ddsd);
        break;
    case 7:
        ddsd.dwSize = sizeof(LPDDSURFACEDESC2);
        res = (*pGetDisplayMode7)(lpdd, &ddsd);
        break;
    }
    if(res) {
        OutTraceE("%s: GetDisplayMode err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        // fix for Win10 platform returning DDERR_INVALIDPARAMS
        RECT desktop;
        ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
        (*pGetClientRect)((*pGetDesktopWindow)(), &desktop);
        ddsd.dwWidth = desktop.right;
        ddsd.dwHeight = desktop.bottom;
    }
    OutDebugDW("%s: detected screen size=(%dx%d)\n", ApiRef, ddsd.dwWidth, ddsd.dwHeight);
    if(dxw.LockedRes) {
        if(!IsChangeDisplaySettingsHotPatched) {
            dwwidth = ddsd.dwWidth;
            dwheight = ddsd.dwHeight;
        }
        OutTraceDW("%s: fixing colordepth current=%d required=%d size=(%dx%d)\n",
                   ApiRef, ddsd.ddpfPixelFormat.dwRGBBitCount, dwbpp, dwwidth, dwheight);
    }
    if(dxw.IsEmulated)
        dwbpp = ddsd.ddpfPixelFormat.dwRGBBitCount;
    // v2.04.79: change display mode only when necessary
    if(!(dxw.LockedRes && (ddsd.ddpfPixelFormat.dwRGBBitCount == dwbpp))) {
        switch(dxversion) {
        default:
        case 1:
            res = (*pSetDisplayMode1)(lpdd, dwwidth, dwheight, dwbpp);
            break;
        case 2:
            res = (*pSetDisplayMode2)(lpdd, dwwidth, dwheight, dwbpp, ddsd.dwRefreshRate, 0);
            break;
        case 3:
            res = (*pSetDisplayMode3)(lpdd, dwwidth, dwheight, dwbpp, ddsd.dwRefreshRate, 0);
            break;
        case 4:
            res = (*pSetDisplayMode4)(lpdd, dwwidth, dwheight, dwbpp, ddsd.dwRefreshRate, 0);
            break;
        case 7:
            res = (*pSetDisplayMode7)(lpdd, dwwidth, dwheight, dwbpp, ddsd.dwRefreshRate, 0);
            break;
        }
        _if(res) OutTraceE("%s: error=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    }
    SetVSyncDelays(dxversion, lpdd);
    // set a default palette ???
    if(dxw.VirtualPixelFormat.dwRGBBitCount == 8) mySetPalette(0, 256, DefaultSystemPalette);
    if(dxw.bAutoScale) dxw.AutoScale();
    return DD_OK;
}

HRESULT WINAPI extSetDisplayMode1(LPDIRECTDRAW lpdd, DWORD dwwidth, DWORD dwheight, DWORD dwbpp) {
    return extSetDisplayMode(1, lpdd, dwwidth, dwheight, dwbpp, 0, 0);
}
HRESULT WINAPI extSetDisplayMode2(LPDIRECTDRAW lpdd, DWORD dwwidth, DWORD dwheight, DWORD dwbpp, DWORD dwrefreshrate, DWORD dwflags) {
    return extSetDisplayMode(2, lpdd, dwwidth, dwheight, dwbpp, dwrefreshrate, dwflags);
}
HRESULT WINAPI extSetDisplayMode3(LPDIRECTDRAW lpdd, DWORD dwwidth, DWORD dwheight, DWORD dwbpp, DWORD dwrefreshrate, DWORD dwflags) {
    return extSetDisplayMode(3, lpdd, dwwidth, dwheight, dwbpp, dwrefreshrate, dwflags);
}
HRESULT WINAPI extSetDisplayMode4(LPDIRECTDRAW lpdd, DWORD dwwidth, DWORD dwheight, DWORD dwbpp, DWORD dwrefreshrate, DWORD dwflags) {
    return extSetDisplayMode(4, lpdd, dwwidth, dwheight, dwbpp, dwrefreshrate, dwflags);
}
HRESULT WINAPI extSetDisplayMode7(LPDIRECTDRAW lpdd, DWORD dwwidth, DWORD dwheight, DWORD dwbpp, DWORD dwrefreshrate, DWORD dwflags) {
    return extSetDisplayMode(7, lpdd, dwwidth, dwheight, dwbpp, dwrefreshrate, dwflags);
}

static HRESULT WINAPI extRestoreDisplayMode(int dxversion, RestoreDisplayMode_Type pRestoreDisplayMode, LPDIRECTDRAW lpdd) {
    ApiName("RestoreDisplayMode");
    DWORD dwwidth, dwheight, dwRGBBitCount;
    OutTraceDW("%s(%d): lpdd=%#x\n", ApiRef, dxversion, lpdd);
    // v2.04.32: fully revised implementation for programs where RestoreDisplayMode is followed by more
    // improves video management for "Rollcage Stage 1" and "Mig Alley"
    if(dxw.IsEmulated) {
        DDSURFACEDESC2 ddsd;
        // in emulation mode, let SetPixFmt decide upon the PixelFormat
        dxw.VirtualPixelFormat.dwRGBBitCount = dxw.dwDefaultColorDepth;
        // after a SetDisplayMode, recover previous values
        if(dxw.dxwLastDisplayWidth) {
            // restore previous values
            dwwidth =  dxw.dxwLastDisplayWidth;
            dwheight = dxw.dxwLastDisplayHeight;
            dwRGBBitCount = dxw.dxwLastDisplayBPP;
        } else {
            OutTraceE("%s: impossible case!!!\n", ApiRef);
            // impossible case ???
            dwwidth =  dxw.dwDefaultScreenWidth;
            dwheight = dxw.dwDefaultScreenHeight;
            dwRGBBitCount = dxw.dwDefaultColorDepth;
        }
        OutTraceDW("%s: restore %dx%d %dBPP\n", ApiRef, dwwidth, dwheight, dwRGBBitCount);
        dxw.SetScreenSize(dwwidth, dwheight);
        dxw.VirtualPixelFormat.dwRGBBitCount = dwRGBBitCount;
        GetHookInfo()->Height = (short)dxw.GetScreenHeight();
        GetHookInfo()->Width = (short)dxw.GetScreenWidth();
        GetHookInfo()->ColorDepth = (short)dxw.VirtualPixelFormat.dwRGBBitCount;
        if(dxw.Windowize) dxw.AdjustWindowFrame(dxw.GethWnd(), dwwidth, dwheight);
        memset(&ddsd, 0, sizeof(ddsd));
        ddsd.dwSize = sizeof(ddsd);
        SetPixFmt(&ddsd);
        SetBltTransformations();
        if(dxw.Windowize) {
            OutTraceDW("%s: mode=EMULATE %s ret=OK\n", ApiRef, DumpPixelFormat(&ddsd));
            SetVSyncDelays(dxversion, lpdd);
            return DD_OK;
        }
    }
    return (*pRestoreDisplayMode)(lpdd);
}

HRESULT WINAPI extRestoreDisplayMode1(LPDIRECTDRAW lpdd) {
    return extRestoreDisplayMode(1, pRestoreDisplayMode1, lpdd);
}
HRESULT WINAPI extRestoreDisplayMode2(LPDIRECTDRAW lpdd) {
    return extRestoreDisplayMode(2, pRestoreDisplayMode2, lpdd);
}
HRESULT WINAPI extRestoreDisplayMode3(LPDIRECTDRAW lpdd) {
    return extRestoreDisplayMode(3, pRestoreDisplayMode3, lpdd);
}
HRESULT WINAPI extRestoreDisplayMode4(LPDIRECTDRAW lpdd) {
    return extRestoreDisplayMode(4, pRestoreDisplayMode4, lpdd);
}
HRESULT WINAPI extRestoreDisplayMode7(LPDIRECTDRAW lpdd) {
    return extRestoreDisplayMode(7, pRestoreDisplayMode7, lpdd);
}

HRESULT WINAPI extGetDisplayMode(int dxversion, GetDisplayMode_Type pGetDisplayMode, LPDIRECTDRAW lpdd, LPDDSURFACEDESC lpddsd) {
    ApiName("GetDisplayMode");
    OutTraceDDRAW("%s(D%d): lpdd=%#x lpddsd=%#x\n", ApiRef, dxversion, lpdd, lpddsd);
    (*pGetDisplayMode)(lpdd, lpddsd);
    if(dxw.IsEmulated) {
        GetPixFmt((LPDDSURFACEDESC2)lpddsd);
        if(!lpddsd->ddpfPixelFormat.dwFlags) SetPixFmt((LPDDSURFACEDESC2)lpddsd);
    }
    dxw.dwRealScreenWidth = lpddsd->dwWidth;
    dxw.dwRealScreenHeight = lpddsd->dwHeight;
    if(dxw.Windowize) {
        lpddsd->dwWidth = dxw.GetScreenWidth();
        lpddsd->dwHeight = dxw.GetScreenHeight();
    }
    // v2.1.96: fake screen color depth
    // v2.03.96: moved out from dxw.Windowize condition
    if((dxw.dwFlags2 & (INIT8BPP | INIT16BPP)) || (dxw.dwFlags7 & (INIT24BPP | INIT32BPP))) { // v2.02.32 fix
        if(dxw.dwFlags2 & INIT8BPP) FixPixelFormat(8, &lpddsd->ddpfPixelFormat);
        if(dxw.dwFlags2 & INIT16BPP) FixPixelFormat(16, &lpddsd->ddpfPixelFormat);
        if(dxw.dwFlags7 & INIT24BPP) FixPixelFormat(24, &lpddsd->ddpfPixelFormat);
        if(dxw.dwFlags7 & INIT32BPP) FixPixelFormat(32, &lpddsd->ddpfPixelFormat);
        OutTraceDW("%s: fix RGBBitCount=%d\n", ApiRef, lpddsd->ddpfPixelFormat.dwRGBBitCount);
    }
    //OutTraceDW("GetDisplayMode: returning WxH=(%dx%d) PixelFormat Flags=%#x(%s) RGBBitCount=%d RGBAmask=(%#x,%#x,%#x,%#x) Caps=%#x(%s)\n",
    //	lpddsd->dwWidth, lpddsd->dwHeight,
    //	lpddsd->ddpfPixelFormat.dwFlags, ExplainPixelFormatFlags(lpddsd->ddpfPixelFormat.dwFlags),
    //	lpddsd->ddpfPixelFormat.dwRGBBitCount,
    //	lpddsd->ddpfPixelFormat.dwRBitMask, lpddsd->ddpfPixelFormat.dwGBitMask, lpddsd->ddpfPixelFormat.dwBBitMask,
    //	lpddsd->ddpfPixelFormat.dwRGBAlphaBitMask,
    //	lpddsd->ddsCaps.dwCaps, ExplainDDSCaps(lpddsd->ddsCaps.dwCaps));
    OutTraceDDRAW("%s: returning size=(%dx%d) %s\n",
                  ApiRef, lpddsd->dwWidth, lpddsd->dwHeight, DumpPixelFormat((LPDDSURFACEDESC2)lpddsd));
    return DD_OK;
}

HRESULT WINAPI extGetDisplayMode1(LPDIRECTDRAW lpdd, LPDDSURFACEDESC lpddsd) {
    return extGetDisplayMode(1, pGetDisplayMode1, lpdd, lpddsd);
}
HRESULT WINAPI extGetDisplayMode2(LPDIRECTDRAW lpdd, LPDDSURFACEDESC lpddsd) {
    return extGetDisplayMode(2, pGetDisplayMode2, lpdd, lpddsd);
}
HRESULT WINAPI extGetDisplayMode3(LPDIRECTDRAW lpdd, LPDDSURFACEDESC lpddsd) {
    return extGetDisplayMode(3, pGetDisplayMode3, lpdd, lpddsd);
}
HRESULT WINAPI extGetDisplayMode4(LPDIRECTDRAW lpdd, LPDDSURFACEDESC2 lpddsd) {
    return extGetDisplayMode(4, (GetDisplayMode_Type)pGetDisplayMode4, lpdd, (LPDDSURFACEDESC)lpddsd);
}
HRESULT WINAPI extGetDisplayMode7(LPDIRECTDRAW lpdd, LPDDSURFACEDESC2 lpddsd) {
    return extGetDisplayMode(7, (GetDisplayMode_Type)pGetDisplayMode7, lpdd, (LPDDSURFACEDESC)lpddsd);
}

HRESULT WINAPI extSetCooperativeLevel(int dxversion, SetCooperativeLevel_Type pSetCooperativeLevel, LPDIRECTDRAW lpdd, HWND hwnd, DWORD dwflags) {
    HRESULT res;
    ApiName("SetCooperativeLevel");
    BOOL bFixFrame = FALSE;
    OutTraceDDRAW("%s(D%d): lpdd=%#x hwnd=%#x dwFlags=%#x(%s)\n",
                  ApiRef, dxversion, lpdd, hwnd, dwflags, ExplainCoopFlags(dwflags));
    // when SetCooperativeLevel is called for the only purpose of setting the FPU policy,
    // just proxy the call. Seen in "G-Darius".
    switch(dwflags) {
    case DDSCL_FPUSETUP:
    case DDSCL_FPUPRESERVE:
        res = (*pSetCooperativeLevel)(lpdd, hwnd, dwflags);
        if(res)
            OutTraceE("%s: ERROR bypass err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return DD_OK;
        break;
    }
    DDSURFACEDESC2 ddsd;
    switch(dxversion) {
    default:
    case 1:
        ddsd.dwSize = sizeof(DDSURFACEDESC);
        res = (*pGetDisplayMode1)(lpdd, (LPDDSURFACEDESC)&ddsd);
        break;
    case 2:
        ddsd.dwSize = sizeof(DDSURFACEDESC);
        res = (*pGetDisplayMode2)(lpdd, (LPDDSURFACEDESC)&ddsd);
        break;
    case 3:
        ddsd.dwSize = sizeof(DDSURFACEDESC);
        res = (*pGetDisplayMode3)(lpdd, (LPDDSURFACEDESC)&ddsd);
        break;
    case 4:
        ddsd.dwSize = sizeof(DDSURFACEDESC2);
        res = (*pGetDisplayMode4)(lpdd, &ddsd);
        break;
    case 7:
        ddsd.dwSize = sizeof(DDSURFACEDESC2);
        res = (*pGetDisplayMode7)(lpdd, &ddsd);
        break;
    }
    if(res) {
        OutTraceE("%s: GetDisplayMode ERROR res=%#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
        return res;
    }
    OutTraceDW("%s: Actual size=(%dx%d)%s\n", ApiRef, ddsd.dwWidth, ddsd.dwHeight, DumpPixelFormat((LPDDSURFACEDESC2)&ddsd));
    dxw.ActualPixelFormat = ddsd.ddpfPixelFormat;
    if(dxw.VirtualPixelFormat.dwRGBBitCount == 0) dxw.VirtualPixelFormat = ddsd.ddpfPixelFormat;
    SetBltTransformations();
    if(dxw.Windowize || (dxw.dwFlags7 & NODDEXCLUSIVEMODE)) {
        if (dwflags & DDSCL_FULLSCREEN) {
            if (dwflags & DDSCL_EXCLUSIVE) dxw.ReservedPaletteEntries = 0;
            // v2.04.01: CREATEDESKTOP option for games that despite all efforts don't have
            // a valid main window, so we build one.
            // Fixes (somehow) "Man TT Super Bike".
            if(dxw.Windowize && (dxw.GethWnd() == NULL) && (dxw.dwFlags6 & CREATEDESKTOP)) {
                hwnd = dxw.CreateVirtualDesktop();
                dxw.SethWnd(hwnd);
            }
            // v2.01.82 fix:
            // WARN: Tomb Raider 4 demo is setting cooperative level against hwnd 0 (desktop)
            // so in this case better use the registered hWnd value. Same as GP500, who uses
            // the desktop window handle.
            // v2.02.31 fix:
            // Hooligans - Storm over Europe wants to set cooperative level NORMAL to hwnd 0
            // that is legitimate, but setting against desktop window gives an error code - so
            // the zero hwnd redirection had to be moved within the FULLSCREEN if case.
            if(dxw.IsRealDesktop(hwnd)) {
                OutTraceDW("%s: desktop hwnd=%#x -> %#x\n", ApiRef, hwnd, dxw.GethWnd());
                hwnd = dxw.GethWnd();
            }
            // v2.03.40 fix:
            // WARN: "Reah" creates a main window undetected. Setting a FULLSCREEN cooperative level
            // against it is a good enough proof that this is the real main window!
            if(hwnd != dxw.GethWnd()) {
                OutTraceDW("%s: setting new main hwnd=%#x -> %#x\n", ApiRef, dxw.GethWnd(), hwnd);
                dxw.SethWnd(hwnd);
            }
            dxw.SetFullScreen(TRUE);
            // v2.03.41: added suppression of DDSCL_SETDEVICEWINDOW DDSCL_CREATEDEVICEWINDOW DDSCL_SETFOCUSWINDOW used by "PBA Bowling 2"
            dwflags &= ~(DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWMODEX | DDSCL_SETDEVICEWINDOW | DDSCL_CREATEDEVICEWINDOW | DDSCL_SETFOCUSWINDOW);
            dwflags |= DDSCL_NORMAL;
            bFixFrame = TRUE;
        } else {
            // v2.03.77: the game "Portugal 1111" calls SetCooperativeLevel with hwnd=0.
            // in such a case, you can get the window size, so better leave the previous
            // FullScreen setting unchanged. This is a wise politic also in case of any
            // sort of GetClientRect error code.
            if(hwnd) {
                RECT client;
                BOOL ret;
                ret = (*pGetClientRect)(hwnd, &client);
                // v2.02.11:
                // Non fullscreen cooperative mode means windowed, unless the window occupies the whole desktop area
                if (ret) {
                    dxw.SetFullScreen(client.right == dxw.iSizX && client.bottom == dxw.iSizY);
                    OutTraceDW("%s: NORMAL hwnd=%#x size=(%dx%d) fullscreen=%#x\n",
                               ApiRef, hwnd, client.right, client.bottom, dxw.IsFullScreen());
                } else {
                    OutTraceDW("%s: pGetClientRect ERROR err=%d fullscreen=%#x\n",
                               ApiRef, GetLastError(), dxw.IsFullScreen());
                }
            }
        }
    } else {
        if(dxw.dwFlags9 & LOCKFULLSCREENCOOP) {
            if(!(dwflags & DDSCL_FULLSCREEN)) return DDERR_EXCLUSIVEMODEALREADYSET;
        }
    }
    OutTraceDW("%s: FIXED dwFlags=%#x(%s)\n", ApiRef, dwflags, ExplainCoopFlags(dwflags));
    res = (*pSetCooperativeLevel)(lpdd, hwnd, dwflags);
    if(res) {
        switch(res) {
        case DDERR_EXCLUSIVEMODEALREADYSET: // v2.04.15 - "Sid Meier's Alpha Centauri" in non-windowed mode
            OutTraceDW("%s: bypass DDERR_EXCLUSIVEMODEALREADYSET\n", ApiRef);
            res = DD_OK;
            break;
        case DDERR_INVALIDPARAMS:
            //hwnd = GetForegroundWindow();
            PostMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
            Sleep(1000);
            res = (*pSetCooperativeLevel)(lpdd, hwnd, dwflags);
            break;
        }
        if(res) {
            OutTraceE("%s: ERROR lpdd=%#x hwnd=%#x Flags=%#x err=%#x(%s) at %d\n",
                      ApiRef, lpdd, hwnd, dwflags, res, ExplainDDError(res), __LINE__);
        }
    }
    if(bFixFrame) {
        // v2.05.01: fix - frame recovery should take physical size and not virtual resolution
        // fixes "Heavy Gear 2" window recovery after minimize
        dxw.AdjustWindowFrame(hwnd, dxw.iSizX, dxw.iSizY);
        dxw.FixWindowFrame(hwnd); // v2.04.44: checks inside ...
    }
    GetHookInfo()->IsFullScreen = dxw.IsFullScreen();
    // WARN: GP500 was setting cooperative level against the desktop! This can be partially
    // intercepted by hooking the GetDesktopWindow() call, but in windowed mode this can't be
    // done, so better repeat the check here.
    if ((res == DD_OK) && (hwnd != NULL)) {
        if (hwnd == (*pGetDesktopWindow)())
            OutTraceE("%s: attempt to work on desktop window\n", ApiRef);
        else
            dxw.SethWnd(hwnd); // save the good one
    }
    return res;
}

HRESULT WINAPI extSetCooperativeLevel1(LPDIRECTDRAW lpdd, HWND hwnd, DWORD dwflags) {
    return extSetCooperativeLevel(1, pSetCooperativeLevel1, lpdd, hwnd, dwflags);
}
HRESULT WINAPI extSetCooperativeLevel2(LPDIRECTDRAW lpdd, HWND hwnd, DWORD dwflags) {
    return extSetCooperativeLevel(2, pSetCooperativeLevel2, lpdd, hwnd, dwflags);
}
HRESULT WINAPI extSetCooperativeLevel3(LPDIRECTDRAW lpdd, HWND hwnd, DWORD dwflags) {
    return extSetCooperativeLevel(3, pSetCooperativeLevel3, lpdd, hwnd, dwflags);
}
HRESULT WINAPI extSetCooperativeLevel4(LPDIRECTDRAW lpdd, HWND hwnd, DWORD dwflags) {
    return extSetCooperativeLevel(4, pSetCooperativeLevel4, lpdd, hwnd, dwflags);
}
HRESULT WINAPI extSetCooperativeLevel7(LPDIRECTDRAW lpdd, HWND hwnd, DWORD dwflags) {
    return extSetCooperativeLevel(7, pSetCooperativeLevel7, lpdd, hwnd, dwflags);
}

void FixSurfaceCapsInner(LPDDSURFACEDESC2 lpddsd, int dxversion) {
    // rules of thumb:
    // 0) if supported (HANDLEFOURCC set) don't touch surfaces with FOURCC codecs like YUY2
    // 1) textures should be left untouched (switching to SYSTEMMEMORY when forcing HEL may even fail!)
    // 1.1) textures with no DDSD_PIXELFORMAT specification should have one when virtual color depth is not equal to real color depth?
    // 2) if a pixel format is specified, if DDSCAPS_SYSTEMMEMORY add DDSCAPS_OFFSCREENPLAY (if pixel formats are different?), otherwise do not touch anything.
    // 3) if the surface is used as a buffer (DDSD_WIDTH set, DDSD_HEIGHT unset) do not touch anything.
    // 4) zbuffer surfaces (DDSCAPS_ZBUFFER set) must have DDSCAPS_SYSTEMMEMORY
    // 5) DDSCAPS_3DDEVICE surfaces should have a defined pixel format
    // 6) in all remaining cases, adjust the pixel format and ensure you have DDSCAPS_SYSTEMMEMORY|DDSCAPS_OFFSCREENPLAIN
    if(!(lpddsd->dwFlags & DDSD_CAPS)) lpddsd->ddsCaps.dwCaps = 0;
    if((dxw.dwFlags10 & HANDLEFOURCC) && (lpddsd->ddpfPixelFormat.dwFlags & DDPF_FOURCC)) {
        OutTrace("FixSurfaceCaps: mapping FOURCC surface\n");
        return;
    }
    if(dxw.dwFlags10 & NOCOMPLEXMIPMAPS) {
        // tweak so far for "Extreme G2" only: older ddraw interfaces, so as Wine and gdVoodoo2 as well,
        // can create surfaces with MIPMAP features and COMPLEX+TEXTURE+MIPMAP capabilities, while the
        // current ddraw returns a DDERR_INVLIDPARAMS error code.
        // To get the best possible rendering without replacing ddraw implementation, you can turn these
        // capabilities off.
#define DDSCAPS_MARKER (DDSCAPS_MIPMAP|DDSCAPS_COMPLEX|DDSCAPS_TEXTURE)
        if ((lpddsd->dwFlags & DDSD_MIPMAPCOUNT) &&
                ((lpddsd->ddsCaps.dwCaps & DDSCAPS_MARKER) == DDSCAPS_MARKER)
           ) {
            lpddsd->dwFlags &= ~DDSD_MIPMAPCOUNT;
            lpddsd->dwMipMapCount = 0;
            lpddsd->ddsCaps.dwCaps &= ~(DDSCAPS_MIPMAP | DDSCAPS_COMPLEX);
        }
    }
    // DDSCAPS_TEXTURE surfaces must be left untouched, unless you set FORCESHEL: in this case switch VIDEOMEMORY to SYSTEMMEMORY
    if((lpddsd->dwFlags & DDSD_CAPS) && (lpddsd->ddsCaps.dwCaps & DDSCAPS_TEXTURE)) {
        if (dxw.dwFlags3 & FORCESHEL) {
            lpddsd->ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
            lpddsd->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
        }
        if((dxw.dwFlags5 & TEXTUREFORMAT) && !(lpddsd->dwFlags & DDSD_PIXELFORMAT)) {
            // TEXTURE: enforce PIXELFORMAT on MEMORY
            lpddsd->dwFlags |= DDSD_PIXELFORMAT;
            GetPixFmt(lpddsd);
            // trytry
            // lpddsd->ddsCaps.dwCaps |= DDSCAPS_OFFSCREENPLAIN; // OFFSCREENPLAIN to set a different pixel format
        }
        if((lpddsd->dwFlags & DDSD_PIXELFORMAT) &&
                (lpddsd->ddpfPixelFormat.dwFlags & DDPF_FOURCC) &&
                (dxw.dwFlags7 & CLEARTEXTUREFOURCC)) {
            lpddsd->ddpfPixelFormat = dxw.VirtualPixelFormat;
            lpddsd->ddpfPixelFormat.dwFourCC = 0;
            lpddsd->ddpfPixelFormat.dwFlags &= ~DDPF_FOURCC;
        }
        return;
    }
    if((lpddsd->dwFlags & DDSD_CAPS) && (lpddsd->ddsCaps.dwCaps & DDSCAPS_ZBUFFER)) { // z-buffer surface - set to memory
        if(dxw.dwFlags8 & OFFSCREENZBUFFER) return;
        lpddsd->ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
        if (dxw.dwFlags8 & ALLOWSYSMEMON3DDEV) lpddsd->ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_ZBUFFER;
        return;
    }
    // this is valid just in case the above block eliminated TEXTURE surfaces....
    if (lpddsd->dwFlags & DDSD_PIXELFORMAT) { // pixel format defined
        // trytry
        //if (lpddsd->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) lpddsd->ddsCaps.dwCaps |= DDSCAPS_OFFSCREENPLAIN; // to allow for pixel format conversion (Quest for Glory 5 - GOG version)
        lpddsd->ddsCaps.dwCaps |= DDSCAPS_OFFSCREENPLAIN;
        return;
    }
    // buffer surface
    if ((lpddsd->dwFlags & (DDSD_WIDTH | DDSD_HEIGHT)) == DDSD_WIDTH) return;
    // 3DDEVICE no TEXTURE: enforce PIXELFORMAT
    // v2.02.90: added for "Zoo Tycoon" textures
    // v2.03.48 - there are two situations
    // "Arx fatalis" asks for DDSCAPS_3DDEVICE+DDSCAPS_OFFSCREENPLAIN capability and needs no DDSCAPS_SYSTEMMEMORY capability
    // "Bunnies must die" asks for DDSCAPS_3DDEVICE+DDSCAPS_OFFSCREENPLAIN+DDSCAPS_VIDEOMEMORY capability and requires DDSCAPS_SYSTEMMEMORY capability
    // we try to manage them by checking for the DDSCAPS_VIDEOMEMORY capability ...
    if((lpddsd->dwFlags & DDSD_CAPS) && (lpddsd->ddsCaps.dwCaps & DDSCAPS_3DDEVICE)) {
        lpddsd->dwFlags |= DDSD_PIXELFORMAT;
        GetPixFmt(lpddsd);
        if(dxw.dwFlags8 & OFFSCREENZBUFFER) {
            // add OFFSCREENPLAIN to set a different pixel format
            lpddsd->ddsCaps.dwCaps |= DDSCAPS_OFFSCREENPLAIN;
            return;
        }
        if(lpddsd->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
            lpddsd->ddsCaps.dwCaps = (DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_SYSTEMMEMORY); // good for "Bunnies must die", NO "Arx Fatalis"
        else
            lpddsd->ddsCaps.dwCaps = (DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE); // good for "Arx Fatalis", NO "Bunnies must die"
        return;
    }
    // default case: adjust pixel format
    OutDebugDW("FixSurfaceCaps: suppress DDSCAPS_VIDEOMEMORY case\n");
    lpddsd->dwFlags |= (DDSD_CAPS | DDSD_PIXELFORMAT);
    if(!(dxw.dwFlags8 & OFFSCREENZBUFFER)) {
        lpddsd->ddsCaps.dwCaps &= ~(DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM); // v2.02.43
        lpddsd->ddsCaps.dwCaps |= (DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN);
    }
    if(!(dxw.dwFlags3 & NOPIXELFORMAT)) GetPixFmt(lpddsd);
    return;
}

void FixSurfaceCaps(LPDDSURFACEDESC2 lpddsd, int dxversion) {
    OutTraceDW("FixSurfaceCaps:  INPUT Flags=%#x(%s) Caps=%#x(%s)\n",
               lpddsd->dwFlags, ExplainFlags(lpddsd->dwFlags), lpddsd->ddsCaps.dwCaps, ExplainDDSCaps(lpddsd->ddsCaps.dwCaps));
    FixSurfaceCapsInner(lpddsd, dxversion);
    OutTraceDW("FixSurfaceCaps: OUTPUT Flags=%#x(%s) Caps=%#x(%s)\n",
               lpddsd->dwFlags, ExplainFlags(lpddsd->dwFlags), lpddsd->ddsCaps.dwCaps, ExplainDDSCaps(lpddsd->ddsCaps.dwCaps));
}

HRESULT WINAPI extCreateSurface1(LPDIRECTDRAW lpdd, DDSURFACEDESC *lpddsd, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    return extCreateSurface(1, (CreateSurface_Type)pCreateSurface1, lpdd, (DDSURFACEDESC2 *)lpddsd, lplpdds, pu);
}
HRESULT WINAPI extCreateSurface2(LPDIRECTDRAW lpdd, DDSURFACEDESC *lpddsd, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    return extCreateSurface(2, (CreateSurface_Type)pCreateSurface2, lpdd, (DDSURFACEDESC2 *)lpddsd, lplpdds, pu);
}
HRESULT WINAPI extCreateSurface3(LPDIRECTDRAW lpdd, DDSURFACEDESC *lpddsd, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    return extCreateSurface(3, (CreateSurface_Type)pCreateSurface3, lpdd, (DDSURFACEDESC2 *)lpddsd, lplpdds, pu);
}
HRESULT WINAPI extCreateSurface4(LPDIRECTDRAW lpdd, DDSURFACEDESC2 *lpddsd, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    return extCreateSurface(4, (CreateSurface_Type)pCreateSurface4, lpdd, (DDSURFACEDESC2 *)lpddsd, lplpdds, pu);
}
HRESULT WINAPI extCreateSurface7(LPDIRECTDRAW lpdd, DDSURFACEDESC2 *lpddsd, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    return extCreateSurface(7, (CreateSurface_Type)pCreateSurface7, lpdd, (DDSURFACEDESC2 *)lpddsd, lplpdds, pu);
}


HRESULT WINAPI extGetAttachedSurface(int dxversion, GetAttachedSurface_Type pGetAttachedSurface,
                                     LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS2 lpddsc, LPDIRECTDRAWSURFACE *lplpddas) {
    HRESULT res;
    ApiName("GetAttachedSurface");
    BOOL IsPrim;
    BOOL IsBack;
    IsPrim = dxwss.IsAPrimarySurface(lpdds);
    IsBack = dxwss.IsABackBufferSurface(lpdds);
    if(IsTraceDDRAW) {
        char sCaps2[60];
        strcpy(sCaps2, "");
        if(dxversion >= 4) sprintf(sCaps2, " caps2=%#x caps3=%#x caps4=%#x", lpddsc->dwCaps2, lpddsc->dwCaps3, lpddsc->dwCaps4);
        OutTrace("%s(%d): lpdds=%#x%s caps=%#x(%s)%s\n",
                 ApiRef, dxversion, lpdds, (IsPrim ? "(PRIM)" : (IsBack ? "(BACK)" : "")),
                 lpddsc->dwCaps, ExplainDDSCaps(lpddsc->dwCaps), sCaps2);
    }
    if(dxw.IsEmulated) {
        // v2.1.81: fix to make "Silver" working: if the primary surface was created with
        // backbuffercount == 2, the game expects some more surface to be attached to
        // the attached backbuffer. Since there would be no use for it, just return
        // the attached backbuffer itself. Makes Silver working, anyway....
        // beware: "Snowboard Racer" fails if you return an attached surface anyhow! There,
        // the primary surface was created with back buffer count == 1.
        // v2.2.62 fix: a check to implement doublebuffer emulation only in case of DDSCAPS_BACKBUFFER
        // requests. A call to GetAttachedSurface can be made to retrieve DDSCAPS_ZBUFFER surfaces, and in
        // this case the BackBuffer surface can't be returned.
        // v2.03.56.fix1: the double buffer trick for Silver works in generic emulation mode, but the
        // surface is requested also as a DDSCAPS_VIDEOMEMORY one.
        // v2.04.60: the doublebuffer trick must be done also for DDSCAPS_FLIP capability - used by "Honour and Freedom".
        if (IsBack && (DDSD_Prim.dwBackBufferCount > 1) && (lpddsc->dwCaps & (DDSCAPS_BACKBUFFER | DDSCAPS_VIDEOMEMORY | DDSCAPS_FLIP))) {
            *lplpddas = lpdds;
            OutTraceDW("%s(%d): DOUBLEBUFFER attached to BACK=%#x\n", ApiRef, dxversion, lpdds);
            return DD_OK;
        }
        // in "Tomb Raider III" GOG release, the primary surface is queryed and has no attached
        // backbuffer, but a backbuffer does exist and has to be retrieved by GetBackBufferSurface.
        // on primary surface check for the backbuffer surface coming from either an explicit
        // AddAttachedSurface, or a primary complex surface creation otherwise.
        // Best criteria to do this is checking for the registered virtual surface capabilities
        if(IsPrim) {
            // most frequent case: DDSCAPS_BACKBUFFER
            // v2.02.42 noticed DDSCAPS_FLIP for "Empire Earth"
            // v2.04.27 noticed DDSCAPS_3DDEVICE for "Frogger 2"
            LPDIRECTDRAWSURFACE lpddsback = dxwss.GetBackBufferSurface();
            if (lpddsback) {
                DWORD BackBufferCaps = dxwss.GetCaps(lpddsback);
                if(BackBufferCaps & lpddsc->dwCaps) {
                    *lplpddas = lpddsback;
                    OutTraceDW("%s(%d): emulate BACKBUFFER attach lpddas=%#x\n", ApiRef, dxversion, *lplpddas);
                    return DD_OK;
                }
            }
        }
        if((lpddsc->dwCaps & DDSCAPS_ZBUFFER) && (
                    (dxw.dwFlags8 & NOHALDEVICE) ||
                    (dxw.dwFlags10 & NOZBUFATTACH)
                )) {
            // tested with "Grand Prix World": if a ZBUFFER is returned, CreateDevice fails!
            // v2.04.83: NOZBUFATTACH tested with "Silkolene Honda Motocross GP":
            // this is different from NOHALDEVICE because the devica should have HAL caps!!
            *lplpddas = NULL;
            OutTraceDW("%s(%d): no attached ZBUFFER\n", ApiRef, dxversion);
            return DDERR_NOTFOUND;
        }
        // attempt to fix S.C.A.R.S. black screen bug ... failed
        //if(lpddsc->dwCaps == (DDSCAPS_TEXTURE|DDSCAPS_MIPMAP)) lpddsc->dwCaps = DDSCAPS_TEXTURE;
    } else {
        // Virtual primary surfaces are created with no DDSCAPS_3DDEVICE caps, so don't look for it ....
        if(IsPrim && (lpddsc->dwCaps & (DDSCAPS_BACKBUFFER | DDSCAPS_FLIP)))
            lpddsc->dwCaps &= ~DDSCAPS_3DDEVICE;
    }
    // proxy the call...
    res = (*pGetAttachedSurface)(lpdds, (LPDDSCAPS)lpddsc, lplpddas);
    if(res) {
        // When necessary, simulate a backbuffer attached to primary surface
        // v2.04.95: both when dwBackBufferCount is greater than 0 .or. when the primary surface has DDSCAPS_FLIP capability
        // if (IsPrim && ((DDSD_Prim.dwBackBufferCount > 0) || (lpddsc->dwCaps & (DDSCAPS_BACKBUFFER|DDSCAPS_FLIP)))){
        // v2.05.20: roll back the fix from v2.04.95: both conditions must be true (.AND.), there is a backbuffer and it has
        // BACKBUFFER or FLIP capability. Recovers "Star Wars Rogue Squadron 3D" crash.
        if (IsPrim && (DDSD_Prim.dwBackBufferCount > 0) && (lpddsc->dwCaps & (DDSCAPS_BACKBUFFER | DDSCAPS_FLIP))) {
            LPDIRECTDRAWSURFACE lpBackBuffer;
            lpBackBuffer = dxwss.GetBackBufferSurface();
            if(lpBackBuffer) {
                *lplpddas = lpBackBuffer;
                OutTraceDW("%s(%d): SIMULATE BACKBUF attach to PRIM=%#x\n", ApiRef, dxversion, lpdds);
                return DD_OK;
            } else
                OutTraceDW("%s(%d): NO BACKBUF\n", ApiRef, dxversion);
        }
        // Warning: AVOID simulating a ZBUFFER attach to any surface that has not it really attached
        // otherwise the program steps over but crashes afterwards, as soon as it tries to use the ZBUFFER.
        // Better return the error (most programs overcome the problem) or change caps policy so that the
        // ZBUFFER becomes attached.
        // Ref: "Arx Fatalis", "TOCA Touring Car Championship"
        // non severe error, log al lower priority
        OutTraceDDRAW("%s(%d): ERROR res=%#x(%s) at %d\n", ApiRef, dxversion, res, ExplainDDError(res), __LINE__);
    } else {
        OutTraceDDRAW("%s(%d): attached=%#x\n", ApiRef, dxversion, *lplpddas);
        if(lpddsc->dwCaps & DDSCAPS_ZBUFFER) {
            char *sMode;
            // Martian Gothic and others: if the ZBUFFER is unknown, then you must register virtual capabilities
            // compatible with those of the original surface!
            DWORD dwCaps;
            lpDDZBuffer = *lplpddas;
            dwCaps = dxwss.GetCaps(*lplpddas);
            // v2.04.17 - beware: the caps db is not properly cleaned on surface destruction, so it may happen
            // that you get a fake-known case where the caps are referred to some other surface type.
            // Happened in "Star Treck: Armada" where the ZBUFFER collects an old PRIMARY caps entry.
            if(!dwCaps || !(dwCaps & DDSCAPS_ZBUFFER)) {
                dwCaps = dxwss.GetCaps(lpdds);
                if(dwCaps) {
                    dwCaps &= ~(DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_BACKBUFFER | DDSCAPS_3DDEVICE | DDSCAPS_COMPLEX);
                    dwCaps |= DDSCAPS_ZBUFFER;
                    //if(dwCaps & DDSCAPS_VIDEOMEMORY) dwCaps|=DDSCAPS_LOCALVIDMEM;
                    dxwss.PushZBufferSurface(*lplpddas, dxversion, dwCaps);
                    sMode = "derived";
                } else sMode = "unknown";
            } else
                sMode = "known";
            OutTraceDW("%s(%d): ZBUFFER caps=%#x(%s) (%s)\n", ApiRef, dxversion, dwCaps, ExplainDDSCaps(dwCaps), sMode);
        }
    }
    return res;
}

HRESULT WINAPI extGetAttachedSurface1(LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS lpddsc, LPDIRECTDRAWSURFACE *lplpddas) {
    return extGetAttachedSurface(1, pGetAttachedSurface1, lpdds, (LPDDSCAPS2)lpddsc, lplpddas);
}
HRESULT WINAPI extGetAttachedSurface2(LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS lpddsc, LPDIRECTDRAWSURFACE *lplpddas) {
    return extGetAttachedSurface(2, pGetAttachedSurface2, lpdds, (LPDDSCAPS2)lpddsc, lplpddas);
}
HRESULT WINAPI extGetAttachedSurface3(LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS lpddsc, LPDIRECTDRAWSURFACE *lplpddas) {
    return extGetAttachedSurface(3, pGetAttachedSurface3, lpdds, (LPDDSCAPS2)lpddsc, lplpddas);
}
HRESULT WINAPI extGetAttachedSurface4(LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS2 lpddsc, LPDIRECTDRAWSURFACE *lplpddas) {
    return extGetAttachedSurface(4, pGetAttachedSurface4, lpdds, lpddsc, lplpddas);
}
HRESULT WINAPI extGetAttachedSurface7(LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS2 lpddsc, LPDIRECTDRAWSURFACE *lplpddas) {
    return extGetAttachedSurface(7, pGetAttachedSurface7, lpdds, lpddsc, lplpddas);
}

#ifdef TRACEALL
HRESULT WINAPI extGetBltStatus(int dxversion, GetBltStatus_Type pGetBltStatus, LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags) {
    // v2.05.18: caught on "Emperor: Battle for Dune" GAME.EXE intro video !!!!
    HRESULT res;
    ApiName("GetBltStatus");
    if(IsTraceDDRAW) {
        OutTraceDDRAW("%s(%d): flags=%#x(%s)\n",
                      ApiRef, dxversion,
                      dwFlags,
                      (dwFlags == DDGBS_CANBLT) ? "CANBLT" : (dwFlags == DDGBS_ISBLTDONE ? "ISBLTDONE" : "invalid"));
    }
    res = (*pGetBltStatus)(lpdds, dwFlags);
    OutTraceDDRAW("%s(%d): ret=%#x(%s)\n", ApiRef, dxversion, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extGetBltStatus1(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags) {
    return extGetBltStatus(1, pGetBltStatus1, lpdds, dwFlags);
}
HRESULT WINAPI extGetBltStatus2(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags) {
    return extGetBltStatus(2, pGetBltStatus2, lpdds, dwFlags);
}
HRESULT WINAPI extGetBltStatus3(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags) {
    return extGetBltStatus(3, pGetBltStatus3, lpdds, dwFlags);
}
HRESULT WINAPI extGetBltStatus4(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags) {
    return extGetBltStatus(4, pGetBltStatus4, lpdds, dwFlags);
}
HRESULT WINAPI extGetBltStatus7(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags) {
    return extGetBltStatus(7, pGetBltStatus7, lpdds, dwFlags);
}

#endif

#ifndef DXW_NOTRACES
void BlitError(HRESULT res, LPRECT lps, LPRECT lpd, int line) {
    char sInfo[512];
    sprintf(sInfo, "Blt: ERROR %#x(%s) at %d", res, ExplainDDError(res), line);
    if (res == DDERR_INVALIDRECT) {
        if (lps)
            sprintf(sInfo, "%s src=(%d,%d)-(%d,%d)", sInfo, lps->left, lps->top, lps->right, lps->bottom);
        else
            sprintf(sInfo, "%s src=(NULL)", sInfo);
        if (lpd)
            sprintf(sInfo, "%s dest=(%d,%d)-(%d,%d)", sInfo, lpd->left, lpd->top, lpd->right, lpd->bottom);
        else
            sprintf(sInfo, "%s dest=(NULL)", sInfo);
    }
    strcat(sInfo, "\n");
    OutTrace(sInfo);
    return;
}

void BlitTrace(char *label, LPRECT lps, LPRECT lpd, int line) {
    char sInfo[512];
    sprintf(sInfo, "[%s]", label);
    if (lps)
        sprintf(sInfo, "%s src=(%d,%d)-(%d,%d)", sInfo, lps->left, lps->top, lps->right, lps->bottom);
    else
        sprintf(sInfo, "%s src=(NULL)", sInfo);
    if (lpd)
        sprintf(sInfo, "%s dest=(%d,%d)-(%d,%d)", sInfo, lpd->left, lpd->top, lpd->right, lpd->bottom);
    else
        sprintf(sInfo, "%s dest=(NULL)", sInfo);
    sprintf(sInfo, "%s at %d\n", sInfo, line);
    OutTrace(sInfo);
    return;
}
#else
void BlitError(HRESULT res, LPRECT lps, LPRECT lpd, int line) {}
void BlitTrace(char *label, LPRECT lps, LPRECT lpd, int line) {}
#endif

HRESULT WINAPI PrimaryBlt(int dxversion, Blt_Type pBlt, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    HRESULT res;
    res = (*pBlt)(lpdds, lpdestrect, lpddssrc, lpsrcrect, GetBltWaitFlags(dwflags), lpddbltfx);
    if(res == DDERR_SURFACELOST) {
        if(lpdds->IsLost()) {
            OutTraceDW("RECOVERY of lost surface lpdds=%#x\n", lpdds);
            lpdds->Restore();
            if(res)
                OutTraceE("PrimaryBlt: Restore ERROR lpdds=%#x err=%#x(%s) at %d\n", lpdds, res, ExplainDDError(res), __LINE__);
        }
    }
    return res;
}

HRESULT WINAPI PrimaryFastBlt(int dxversion, Blt_Type pBlt, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    HRESULT res;
    RECT client;
    int iXOffset, iYOffset; // offsets to center surface area to window
    (*pGetClientRect)(dxw.GethWnd(), &client);
    iXOffset = (client.right - dxw.GetScreenWidth()) >> 1;
    iYOffset = (client.bottom - dxw.GetScreenHeight()) >> 1;
    if(dxw.dwFlags3 & FORCECLIPPER) {
        RECT destrect;
        destrect.left = iXOffset + lpdestrect->left;
        destrect.right = iXOffset + lpdestrect->right;
        destrect.top = iYOffset + lpdestrect->top;
        destrect.bottom = iYOffset + lpdestrect->bottom;
        res = (*pBlt)(lpdds, &destrect, lpddssrc, lpsrcrect, GetBltWaitFlags(dwflags), 0);
    } else {
        BltFast_Type pBltFast;
        switch(dxversion) {
        case 1:
            pBltFast = pBltFast1;
            break;
        case 2:
            pBltFast = pBltFast2;
            break;
        case 3:
            pBltFast = pBltFast3;
            break;
        case 4:
            pBltFast = pBltFast4;
            break;
        case 7:
            pBltFast = pBltFast7;
            break;
        }
        res = (*pBltFast)(lpdds, iXOffset + lpdestrect->left, iYOffset + lpdestrect->top, lpddssrc, lpsrcrect, GetFlipWaitFlags(DDFLIP_WAIT));
    }
    if(res == DDERR_SURFACELOST) {
        if(lpdds->IsLost()) {
            OutTraceDW("RECOVERY of lost surface lpdds=%#x\n", lpdds);
            res = lpdds->Restore();
            if(res)
                OutTraceE("PrimaryBlt: Restore ERROR lpdds=%#x err=%#x(%s) at %d\n", lpdds, res, ExplainDDError(res), __LINE__);
        }
    }
    return res;
}

HRESULT WINAPI PrimaryStretchBlt(int dxversion, Blt_Type pBlt, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwFlags, LPDDBLTFX lpddbltfx) {
    HRESULT res;
    DDSURFACEDESC2 ddsd;
    RECT TmpRect;
    LPDIRECTDRAWSURFACE lpddsTmp;
    LPDIRECTDRAWSURFACE lpddsBak;
    DDSCAPS caps;
    CreateSurface1_Type pCreateSurface;
    CreateClipper_Type pCreateClipper;
    SetClipper_Type pSetClipper;
    BltFast_Type pBltFast;
    int dwSize;
    static BOOL bUseFastBlt = !(dxw.dwFlags3 & FORCECLIPPER);
    if (dwFlags & DDBLT_COLORFILL) {
        res = (*pBlt)(lpdds, lpdestrect, lpddssrc, lpsrcrect, dwFlags, lpddbltfx);
        _if(res) OutTraceE("PrimaryStretchBlt: Blt ERROR %#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        // v2.04.23: to do - works in all modes but emulated buffer, don't know why. Fake a positive retcode anyway.
        return DD_OK;
    }
    switch(iPrimarySurfaceVersion) {
    default:
    case 1:
        pBltFast = pBltFast1;
        pCreateSurface = pCreateSurface1;
        dwSize = sizeof(DDSURFACEDESC);
        break;
    case 2:
        pBltFast = pBltFast2;
        pCreateSurface = (CreateSurface1_Type)pCreateSurface2;
        dwSize = sizeof(DDSURFACEDESC);
        break;
    case 3:
        pBltFast = pBltFast3;
        pCreateSurface = (CreateSurface1_Type)pCreateSurface3;
        dwSize = sizeof(DDSURFACEDESC);
        break;
    case 4:
        pBltFast = pBltFast4;
        pCreateSurface = (CreateSurface1_Type)pCreateSurface4;
        dwSize = sizeof(DDSURFACEDESC2);
        break;
    case 7:
        pBltFast = pBltFast7;
        pCreateSurface = (CreateSurface1_Type)pCreateSurface7;
        dwSize = sizeof(DDSURFACEDESC2);
        break;
    }
    caps.dwCaps = DDSCAPS_BACKBUFFER;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = dwSize;
    if(lpddssrc == NULL) {
        HRESULT res2;
        // blit from backbuffer
        res2 = lpdds->GetAttachedSurface(&caps, &(LPDIRECTDRAWSURFACE)lpddsBak);
        if(res2) {
            OutTraceE("PrimaryStretchBlt: skip - lpddsBak=%#x at %d\n", lpddsBak, __LINE__);
            return DD_OK; // fake a success ...
        }
        lpddsBak->GetSurfaceDesc((LPDDSURFACEDESC)&ddsd);
    } else {
        // blit from surface
        lpddssrc->GetSurfaceDesc((LPDDSURFACEDESC)&ddsd);
    }
    TmpRect.left = TmpRect.top = 0;
    TmpRect.bottom = ddsd.dwHeight = lpdestrect->bottom - lpdestrect->top;
    TmpRect.right  = ddsd.dwWidth  = lpdestrect->right  - lpdestrect->left;
    if((TmpRect.bottom == 0) || (TmpRect.right == 0)) return DD_OK; // avoid blitting to null areas (Fifa 2000 D3D)
    ddsd.dwFlags = (DDSD_HEIGHT | DDSD_WIDTH | DDSD_CAPS);
    // capabilities must cope with primary / backbuffer surface capabilities to get speedy operations
    ddsd.ddsCaps.dwCaps = dwBackBufferCaps;
    // v2.04.23: this field gets overwritten by GetSurfaceDesc, so the assignment must be repeated
    ddsd.dwSize = dwSize;
    res = (*pCreateSurface)(lpPrimaryDD, (LPDDSURFACEDESC)&ddsd, &lpddsTmp, NULL);
    if(res) {
        OutTraceE("PrimaryStretchBlt: CreateSurface ERROR %#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        OutDebugDW("PrimaryStretchBlt: CreateSurface %s\n", LogSurfaceAttributes(&ddsd, "[Gateway]", __LINE__));
        return res;
    }
    // stretch-blit to target size on OFFSCREENPLAIN temp surface
    res = (*pBlt)(lpddsTmp, &TmpRect, lpddssrc, lpsrcrect, GetBltWaitFlags(dwFlags), lpddbltfx);
    if(res)
        OutTraceE("PrimaryStretchBlt: Blt ERROR %#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else {
        while(TRUE) {
            if(bUseFastBlt)
                res = (*pBltFast)(lpdds, lpdestrect->left, lpdestrect->top, lpddsTmp, &TmpRect, GetBltFastWaitFlags(DDBLTFAST_WAIT));
            else
                res = (*pBlt)(lpdds, lpdestrect, lpddsTmp, &TmpRect, GetBltWaitFlags(DDBLT_WAIT), 0);
            if(res == DDERR_INVALIDRECT) {
                OutTraceDW("PrimaryStretchBlt: adding clipper ddversion=%#x at %d\n", iDirectDrawVersion, __LINE__);
                bUseFastBlt = FALSE;
                switch(iDirectDrawVersion) {
                case 1:
                    pCreateClipper = pCreateClipper1;
                    break;
                case 2:
                    pCreateClipper = pCreateClipper2;
                    break;
                case 3:
                    pCreateClipper = pCreateClipper3;
                    break;
                case 4:
                    pCreateClipper = pCreateClipper4;
                    break;
                case 7:
                    pCreateClipper = pCreateClipper7;
                    break;
                }
                switch(iPrimarySurfaceVersion) {
                case 1:
                    pSetClipper = pSetClipper1;
                    break;
                case 2:
                    pSetClipper = pSetClipper2;
                    break;
                case 3:
                    pSetClipper = pSetClipper3;
                    break;
                case 4:
                    pSetClipper = pSetClipper4;
                    break;
                case 7:
                    pSetClipper = pSetClipper7;
                    break;
                }
                if(!lpddC) {
                    res = (*pCreateClipper)(lpPrimaryDD, 0, &lpddC, NULL);
                    _if(res) OutTraceE("CreateClipper ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
                }
                //res=(*pSetHWnd)(lpddC, 0, dxw.GethWnd()); // no good, cliper object could be unhooked yet! "Settlers III" would crash!
                res = lpddC->SetHWnd(0, dxw.GethWnd());
                _if(res) OutTraceE("SetHWnd ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
                res = (*pSetClipper)(lpdds, lpddC);
                _if(res) OutTraceE("SetClipper ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
                OutTraceDW("PrimaryStretchBlt: added clipper lpddc=%#x at %d\n", lpddC, __LINE__);
                res = (*pBlt)(lpdds, lpdestrect, lpddsTmp, &TmpRect, GetBltWaitFlags(DDBLT_WAIT), lpddbltfx);
            }
            _if(res) OutTraceE("PrimaryStretchBlt: %s ERROR %#x(%s) at %d\n", bUseFastBlt ? "BltFast" : "Blt", res, ExplainDDError(res), __LINE__);
            if(res == DDERR_SURFACELOST) {
                OutTraceDW("PrimaryStretchBlt: performing IsLost on src=%#x dst=%#x\n", lpddsTmp, lpdds);
                if(lpdds->IsLost()) {
                    OutTraceDW("RECOVERY of lost surface lpdds=%#x\n", lpdds);
                    res = lpdds->Restore();
                    if(res)
                        OutTraceE("PrimaryStretchBlt: Restore ERROR lpdds=%#x err=%#x(%s) at %d\n", lpdds, res, ExplainDDError(res), __LINE__);
                }
                if(lpddsTmp->IsLost()) {
                    OutTraceDW("RECOVERY of lost surface lpdds=%#x\n", lpddsTmp);
                    res = lpddsTmp->Restore();
                    if(res)
                        OutTraceE("PrimaryStretchBlt: Restore ERROR lpdds=%#x err=%#x(%s) at %d\n", lpddsTmp, res, ExplainDDError(res), __LINE__);
                }
                if(!res) {
                    OutTraceDW("PrimaryStretchBlt: surface sucessfully recovered, retry\n");
                    continue;
                }
            }
            break;
        }
    }
    (*pReleaseSMethod(dxversion))(lpddsTmp);
    return res;
}

void *LoadFilter(char *apiname) {
    HMODULE filterlib;
    char sSourcePath[MAX_PATH + 1];
    SetDllDirectory(GetDxWndPath());
    sprintf_s(sSourcePath, MAX_PATH, "%s\\filter.dll", GetDxWndPath());
    filterlib = (*pLoadLibraryA)(sSourcePath);
    if(!filterlib) {
        OutTraceDW("DXWND: Load lib=\"%s\" failed err=%d\n", sSourcePath, GetLastError());
        return NULL;
    }
    return (*pGetProcAddress)(filterlib, apiname);
}

HRESULT WINAPI PrimaryBilinearBlt(int dxversion, Blt_Type pBlt, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    HRESULT res;
    typedef void (WINAPI * Resize_HQ_Type)( unsigned char *, RECT *, int, unsigned char *, RECT *, int);
    static Resize_HQ_Type pResize_HQ = NULL;
    /* to be implemented .... */
    DDSURFACEDESC2 ddsd;
    RECT TmpRect, SrcRect;
    LPDIRECTDRAWSURFACE lpddsTmp;
    LPDIRECTDRAWSURFACE lpddsBak;
    LPDIRECTDRAWSURFACE lpddsCopy = NULL;
    DDSCAPS caps;
    BYTE *bSourceBuf, *bDestBuf;
    LONG dwWidth, dwHeight;
    int SrcPitch, DestPitch;
    CreateSurface1_Type pCreateSurface;
    BltFast_Type pBltFast;
    int dwSize;
    switch(iPrimarySurfaceVersion) { // v2.04.03.fx1 fix: must use iPrimarySurfaceVersion, not dxversion!
    default:
    case 1:
        pBltFast = pBltFast1;
        pCreateSurface = pCreateSurface1;
        dwSize = sizeof(DDSURFACEDESC);
        break;
    case 2:
        pBltFast = pBltFast2;
        pCreateSurface = (CreateSurface1_Type)pCreateSurface2;
        dwSize = sizeof(DDSURFACEDESC);
        break;
    case 3:
        pBltFast = pBltFast3;
        pCreateSurface = (CreateSurface1_Type)pCreateSurface3;
        dwSize = sizeof(DDSURFACEDESC);
        break;
    case 4:
        pBltFast = pBltFast4;
        pCreateSurface = (CreateSurface1_Type)pCreateSurface4;
        dwSize = sizeof(DDSURFACEDESC2);
        break;
    case 7:
        pBltFast = pBltFast7;
        pCreateSurface = (CreateSurface1_Type)pCreateSurface7;
        dwSize = sizeof(DDSURFACEDESC2);
        break;
    }
    caps.dwCaps = DDSCAPS_BACKBUFFER;
    memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = dwSize;
    if(lpddssrc == NULL) {
        HRESULT res2;
        // blit from backbuffer
        res2 = lpdds->GetAttachedSurface(&caps, &lpddsBak);
        if(res2) {
            OutTraceE("PrimaryBilinearBlt: skip - lpddsBak=%#x at %d\n", lpddsBak, __LINE__);
            return DD_OK; // fake a success ...
        }
        lpddsBak->GetSurfaceDesc((LPDDSURFACEDESC)&ddsd);
    } else {
        // blit from surface
        lpddssrc->GetSurfaceDesc((LPDDSURFACEDESC)&ddsd);
    }
    // assign source RECT values anyway....
    if(!lpsrcrect) {
        lpsrcrect = &SrcRect;
        lpsrcrect->left = lpsrcrect->top = 0;
        lpsrcrect->right = dxw.GetScreenWidth();
        lpsrcrect->bottom = dxw.GetScreenHeight();
    }
    dwWidth = lpdestrect->right  - lpdestrect->left;
    dwHeight = lpdestrect->bottom - lpdestrect->top;
    TmpRect.left = TmpRect.top = 0;
    TmpRect.bottom = ddsd.dwHeight = dwHeight;
    TmpRect.right  = ddsd.dwWidth  = dwWidth;
    ddsd.dwFlags = (DDSD_HEIGHT | DDSD_WIDTH | DDSD_CAPS);
    // capabilities must cope with primary / backbuffer surface capabilities to get speedy operations
    ddsd.ddsCaps.dwCaps = dwBackBufferCaps;
    res = (*pCreateSurface)(lpPrimaryDD, (LPDDSURFACEDESC)&ddsd, &lpddsTmp, NULL);
    _if(res) OutTraceE("PrimaryBilinearBlt: CreateSurface ERROR %#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    // get informations
    memset(&ddsd, 0, dwSize);
    ddsd.dwSize = dwSize;
    ddsd.dwFlags = DDSD_LPSURFACE | DDSD_PITCH;
    res = (*pLockMethod(dxversion))(lpddssrc, 0, (LPDDSURFACEDESC)&ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_READONLY, 0);
    if(res) {
        OutTraceE("PrimaryBilinearBlt: Lock ERROR %#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        return DD_OK;
    }
    bSourceBuf = (BYTE *)ddsd.lpSurface;
    SrcPitch = ddsd.lPitch;
    memset(&ddsd, 0, dwSize);
    ddsd.dwSize = dwSize;
    ddsd.dwFlags = DDSD_LPSURFACE | DDSD_PITCH;
    res = (*pLockMethod(dxversion))(lpddsTmp, 0, (LPDDSURFACEDESC)&ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WRITEONLY | DDLOCK_WAIT, 0);
    if(res) {
        OutTraceE("PrimaryBilinearBlt: Lock ERROR %#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        return DD_OK;
    }
    bDestBuf = (BYTE *)ddsd.lpSurface;
    DestPitch = ddsd.lPitch;
    // do the filtering
    if(!pResize_HQ) {
        char *filter;
        HMODULE filterlib;
        switch(ddsd.ddpfPixelFormat.dwGBitMask) {
        default:
        case 0x00FF00:
            filter = "Resize_HQ_4ch";
            break;
        case 0x0007E0: // RGB565
            filter = "Resize_HQ_2ch565";
            break;
        case 0x0003E0: // RGB555
            filter = "Resize_HQ_2ch555";
            break;
        }
        filterlib = (*pLoadLibraryA)("filter.dll");
        if(!filterlib) {
            char sMsg[80 + 1];
            sprintf(sMsg, "DXWND: ERROR can't load lib=\"filter.dll\" err=%#x\n", GetLastError());
            OutTraceE(sMsg);
            MessageBox(0, sMsg, "ERROR", MB_OK | MB_ICONEXCLAMATION);
            exit(0);
        }
        pResize_HQ = (Resize_HQ_Type)(*pGetProcAddress)(filterlib, filter);
        if(!pResize_HQ) {
            char sMsg[80 + 1];
            sprintf(sMsg, "DXWND: ERROR can't load name=\"%s\"\n", filter);
            OutTraceE(sMsg);
            MessageBox(0, sMsg, "ERROR", MB_OK | MB_ICONEXCLAMATION);
            exit(0);
        }
    }
    (*pResize_HQ)(bSourceBuf, lpsrcrect, SrcPitch, bDestBuf, lpdestrect, DestPitch);
    // fast-blit to primary
    (*pUnlockMethod(dxversion))(lpddssrc, NULL);
    (*pUnlockMethod(dxversion))(lpddsTmp, NULL);
    if(dxw.dwFlags3 & FORCECLIPPER) {
        res = (*pBlt)(lpdds, lpdestrect, lpddsTmp, &TmpRect, DDBLT_WAIT, 0);
        _if(res) OutTraceE("PrimaryBilinearBlt: Blt ERROR %#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    } else {
        res = (*pBltFast)(lpdds, lpdestrect->left, lpdestrect->top, lpddsTmp, &TmpRect, DDBLTFAST_WAIT);
        _if(res) OutTraceE("PrimaryBilinearBlt: BltFast ERROR %#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    }
    (*pReleaseSMethod(dxversion))(lpddsTmp);
    if(lpddsCopy) (*pReleaseSMethod(dxversion))(lpddsCopy);
    return res;
}

HRESULT WINAPI PrimaryNoBlt(int dxversion, Blt_Type pBlt, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    return DD_OK;
}

typedef HRESULT (WINAPI *ColorConversion_Type)(int, LPDIRECTDRAWSURFACE, RECT, LPDIRECTDRAWSURFACE *);

HRESULT WINAPI ColorConversionEmulated(int dxversion, LPDIRECTDRAWSURFACE lpdds, RECT emurect, LPDIRECTDRAWSURFACE *lpddssource) {
    HRESULT res, res1, res2;
    Blt_Type pBlt;
    RECT srcrect, destrect;
    pBlt = pGetBltMethod(dxversion);
    // v2.04.01.fx6: copy emurect since pEmuBlt will alter values!
    srcrect = destrect = emurect;
    LONG srcpitch, destpitch;
    LPVOID srcsurface, dstsurface;
    DWORD dstGBitMask, srcGBitMask;
    DDSURFACEDESC2 ddsdback;
    DDSURFACEDESC2 ddsd;
    memset(&ddsdback, 0, sizeof(DDSURFACEDESC2));
    ddsdback.dwSize = Set_dwSize_From_Surface();
    ddsdback.dwFlags = DDSD_LPSURFACE | DDSD_PITCH;
    if(res = (*pLockMethod(dxversion))(lpDDSEmu_Back, 0, (LPDDSURFACEDESC)&ddsdback, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WRITEONLY | DDLOCK_WAIT, 0)) {
        OutTraceE("EmuBlt: Lock ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        if(res == DDERR_INVALIDPARAMS)
            OutTraceE("EmuBlt: lpDDSEmu_Back=%#x swSize=%d\n", lpDDSEmu_Back, ddsdback.dwSize);
        return res;
    }
    destpitch = ddsdback.lPitch;
    dstsurface = ddsdback.lpSurface;
    dstGBitMask = ddsdback.ddpfPixelFormat.dwGBitMask;
    //if(dxw.dwDFlags & DUMPBLITDELTAS) SurfaceDiffPush(dxversion, (DDSURFACEDESC *)&ddsdback);
    memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = Set_dwSize_From_Surface();
    ddsd.dwFlags = DDSD_LPSURFACE | DDSD_PITCH;
    if(res = (*pLockMethod(dxversion))(lpdds, 0, (LPDDSURFACEDESC)&ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_READONLY, 0)) {
        OutTraceE("EmuBlt: Lock ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        if(res == DDERR_INVALIDPARAMS)
            OutTraceE("EmuBlt: lpdds=%#x swSize=%d\n", lpdds, ddsd.dwSize);
        (*pUnlockMethod(dxversion))(lpDDSEmu_Back, NULL);
        (*pUnlockMethod(dxversion))(lpdds, NULL);
        return DD_OK;
    }
    srcsurface = ddsd.lpSurface;
    srcpitch = ddsd.lPitch;
    srcGBitMask = ddsd.ddpfPixelFormat.dwGBitMask;
    // v2.04.33: try/catch added to neutralize "Emperor of the Fading Suns" possible exceptions
    __try {
        //res=(*pEmuBlt)(dxversion, pBlt, lpDDSEmu_Back, &destrect, lpdds, &srcrect, DDBLT_WAIT, 0);
        (*pEmuBlt)(dstsurface, &destrect, destpitch, dstGBitMask, srcsurface, &srcrect, srcpitch, srcGBitMask);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
    }
    if(GetHookInfo()->GammaControl && dxw.bEnableGammaControl)
        (*pGammaCorrection)(dstsurface, &destrect, destpitch, dstGBitMask);
    if(dxw.dwDFlags & MARKBLIT) {
        if(dxw.FilterXScalingFactor) {
            destrect.left	*= dxw.FilterXScalingFactor;
            destrect.right	*= dxw.FilterXScalingFactor;
        }
        if(dxw.FilterYScalingFactor) {
            destrect.top	*= dxw.FilterYScalingFactor;
            destrect.bottom *= dxw.FilterYScalingFactor;
        }
        switch(dxw.ActualPixelFormat.dwRGBBitCount) {
        case 16:
            MarkRect16((SHORT *)dstsurface, &destrect, destpitch);
            break;
        case 32:
            MarkRect32((DWORD *)dstsurface, &destrect, destpitch);
            break;
        }
    }
    //if(dxw.dwDFlags & DUMPBLITDELTAS) SurfaceDiffDump(dxversion, (DDSURFACEDESC *)&ddsdback);
    res1 = (*pUnlockMethod(dxversion))(lpDDSEmu_Back, &destrect);
    if(res1 == DDERR_SURFACEBUSY) {
        (*pUnlockMethod(dxversion))(lpdds, NULL);
        res1 = DD_OK;
    }
#ifndef DXW_NOTRACES
    if (res1) OutTraceE("EmuBlt: Unlock ERROR dds=%#x res=%#x(%s) at %d\n", lpDDSEmu_Back, res, ExplainDDError(res), __LINE__);
#endif // DXW_NOTRACES
    res2 = (*pUnlockMethod(dxversion))(lpdds, &srcrect);
    if(res2 == DDERR_SURFACEBUSY) {
        (*pUnlockMethod(dxversion))(lpDDSEmu_Back, NULL);
        res2 = DD_OK;
    }
#ifndef DXW_NOTRACES
    if (res2) OutTraceE("EmuBlt: Unlock ERROR dds=%#x res=%#x(%s) at %d\n", lpdds, res, ExplainDDError(res), __LINE__);
#endif // DXW_NOTRACES
    if(res) {
        BlitError(res, &emurect, &emurect, __LINE__);
        if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = 0;
    }
    *lpddssource = lpDDSEmu_Back;
    return res;
}

HRESULT WINAPI ColorConversionGDI(int version, LPDIRECTDRAWSURFACE lpdds, RECT emurect, LPDIRECTDRAWSURFACE *lpddssource) {
    HRESULT res;
    GetDC_Type pGetDC = pGetDCMethod();
    ReleaseDC_Type pReleaseDC = pReleaseDCMethod();
    do {
        HDC hdc_source, hdc_dest;
        res = (*pGetDC)(lpdds, &hdc_source);
        if(res) {
            OutTraceE("GetDC ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
            break;
        }
        res = (*pGetDC)(lpDDSEmu_Back, &hdc_dest);
        if(res) {
            OutTraceE("GetDC ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
            break;
        }
        if(!BitBlt(hdc_dest, 0, 0, dxw.GetScreenWidth(), dxw.GetScreenHeight(), hdc_source, 0, 0, SRCCOPY))
            OutTraceE("BitBlt ERROR: err=%d at %d\n", GetLastError(), __LINE__);
        res = (*pReleaseDC)(lpdds, hdc_source);
        if(res) {
            OutTraceE("ReleaseDC ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
            break;
        }
        res = (*pReleaseDC)(lpDDSEmu_Back, hdc_dest);
        if(res) {
            OutTraceE("ReleaseDC ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
            break;
        }
    } while(FALSE);
    *lpddssource = lpDDSEmu_Back;
    return res;
}

HRESULT WINAPI ColorConversionDDRAW(int dxversion, LPDIRECTDRAWSURFACE lpdds, RECT emurect, LPDIRECTDRAWSURFACE *lpddssource) {
    *lpddssource = lpdds;
    return DD_OK;
}

HRESULT WINAPI extFlip(int dxversion, Flip_Type pFlip, LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddssrc, DWORD dwflags) {
    BOOL IsPrim;
    HRESULT res;
    ApiName("Flip");
    DDSURFACEDESC2 ddsd;
    LPDIRECTDRAWSURFACE lpddsTmp;
    LPDIRECTDRAWSURFACE lpDDSBack;
    DWORD dwBltFlags;
    BOOL bEmulatedFlip;
    bEmulatedFlip = dxw.Windowize || dxw.IsEmulated;
    IsPrim = dxwss.IsAPrimarySurface(lpdds);
    OutTraceDDRAW("%s(%d): %s lpdds=%#x%s, src=%#x, flags=%#x(%s)\n",
                  ApiRef, dxversion, bEmulatedFlip ? "EMULATED" : "GENUINE",
                  lpdds, IsPrim ? "(PRIM)" : "", lpddssrc, dwflags, ExplainFlipFlags(dwflags));
    Blt_Type pBlt;
    pBlt = pGetBltMethod(dxversion);
    // Flip-ping to non primary surface ....
    if (!IsPrim) {
        if(lpddssrc)
            res = (*pFlip)(lpdds, lpddssrc, dwflags);
        else {
            LPDIRECTDRAWSURFACE lpddsAttached;
            DDSCAPS2 ddsc;
            DDSURFACEDESC2 sd;
            sd.dwSize = Set_dwSize_From_Surface();
            res = lpdds->GetSurfaceDesc((DDSURFACEDESC *)&sd);
#ifndef DXW_NOTRACES
            if (res) OutTraceDW("%s: GetSurfaceDesc res=%#x at %d\n", ApiRef, res, __LINE__);
#endif // DXW_NOTRACES
            if((dxw.dwFlags10 & HANDLEFOURCC) && (sd.ddpfPixelFormat.dwFlags & DDPF_FOURCC)) {
                extern int gFlippedToScreen;
                LPDIRECTDRAWSURFACE lpSource;
                RECT Rect = dxw.GetUnmappedScreenRect();
                PAINTSTRUCT Paint;
                (*pBeginPaint)(dxw.GethWnd(), &Paint);
#if 0
                res = (*pFlip)(lpdds, lpddssrc, dwflags);
                if(res)	OutTraceE("Flip: FOURCC Flip ERROR %#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
                lpSource = lpdds;
#else
                memset(&ddsc, 0, sizeof(ddsc));
                ddsc.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_VIDEOMEMORY;
                //ddsc.dwCaps = DDSCAPS_FLIP;
                res = lpdds->GetAttachedSurface((LPDDSCAPS)&ddsc, &lpddsAttached);
                _if(res) OutTraceE("%s: FOURCC GetAttached ERROR %#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
                lpSource = lpddsAttached;
#endif
#if 1
                res = (*pBlt)(lpDDSEmu_Prim, &Rect, lpSource, NULL, GetBltWaitFlags(DDBLT_WAIT), 0);
                _if(res) OutTraceE("%s: FOURCC Blt ERROR %#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
#else
                res = (*pBlt)(lpDDSEmu_Back, NULL, lpSource, NULL, GetBltWaitFlags(DDBLT_WAIT), 0);
                _if(res) OutTraceE("Flip: FOURCC Blt ERROR %#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
                res = (*pBlt)(lpDDSEmu_Prim, &Rect, lpDDSEmu_Back, NULL, GetBltWaitFlags(DDBLT_WAIT), 0);
                _if(res) OutTraceE("Flip: FOURCC Blt ERROR %#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
#endif
                (*pEndPaint)(dxw.GethWnd(), &Paint);
                gFlippedToScreen = 2;
                OutTraceDW("%s: FOURCC OK\n", ApiRef);
                return DD_OK;
            }
            // replace these CAPS (good for seven kingdoms II) with same as lpdds surface
            ddsc.dwCaps = sd.ddsCaps.dwCaps;
            ddsc.dwCaps2 = 0;
            ddsc.dwCaps3 = 0;
            ddsc.dwCaps4 = 0;
            res = lpdds->GetAttachedSurface((LPDDSCAPS)&ddsc, &lpddsAttached);
            if(res) {
                OutTraceE("%s: GetAttachedSurface ERROR %#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
                return res;
            }
            res = sBlt(dxversion, pBlt, ApiRef, lpdds, NULL, lpddsAttached, NULL, DDBLT_WAIT, 0, TRUE);
            if(res) {
                OutTraceE("%s: Blt ERROR %#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
                return res;
            }
        }
        _if(res) OutTraceE("%s: ERROR %#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
        return res;
    }
    // from here on, Flip-ping to primary surface ....
    // not bEmulatedFlip means no emulation and no window mode (see above), that is ddraw used in native mode
    // in these circumstances a direct Flip should be possible.
    // beware: if no backbuffer surface was esplicitly created ("The Sting!", "Need for Speed 5") this is
    // the only working way to perform a Flip from NULL surface operation.
    // Fixes arqarq complains about these games in non-windowed & non-emulated mode
    if(!bEmulatedFlip) {
        dwBltFlags = GetFlipWaitFlags(dwflags);
        res = (*pFlip)(lpdds, lpddssrc, dwBltFlags);
        _if(res) OutTraceE("%s: ERROR %#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
#ifndef DXW_NOTRACES
        if((dxw.dwDFlags & DUMPSURFACES) && dxw.bCustomKeyToggle) SurfaceDump(lpdds, dxversion);
        if((dxw.dwDFlags2 & DUMPBLITSRC) && dxw.bCustomKeyToggle) SurfaceDump(lpddssrc, dxversion);
#endif // DXW_NOTRACES
        return res;
    }
    // emulation to primary surface Flip - you can't flip to window surfaces,
    // so you have to replace it with Blt operations.
    // v2.03.98: moved closer to actual video operations to avoid the risk of losing the end of vertical blank period
    // while doing calculations
    //if((dwflags & DDFLIP_WAIT) || (dxw.dwFlags1 & SAVELOAD)) lpPrimaryDD->WaitForVerticalBlank(DDWAITVB_BLOCKEND , 0);
    lpDDSBack = dxwss.GetBackBufferSurface();
    // from ddraw.h:
    // On IDirectDrawSurface7 and higher interfaces, the default is DDFLIP_WAIT. If you wish
    // to override the default and use time when the accelerator is busy (as denoted by
    // the DDERR_WASSTILLDRAWING return code) then use DDFLIP_DONOTWAIT.
    if(dxversion >= 7)
        dwBltFlags = (dwflags & DDFLIP_DONOTWAIT) ? DDBLT_DONOTWAIT : DDBLT_WAIT;
    else
        dwBltFlags = (dwflags & DDFLIP_WAIT) ? DDBLT_WAIT : 0;
    dwBltFlags = GetBltWaitFlags(dwBltFlags);
    if(dxw.dwFlags4 & NOFLIPEMULATION) {
        HRESULT res2;
        // create a temporary working surface
        memset(&ddsd, 0, sizeof(ddsd));
        ddsd.dwSize = (dxversion < 4) ? sizeof(DDSURFACEDESC) : sizeof(DDSURFACEDESC2); // v2.03.60 insidious bug fix !!!
        // v2.02.80: the BackBuffer may not exist? see "HellCopter"
        if(lpDDSBack) {
            (*pGetSurfaceDescMethod())((LPDIRECTDRAWSURFACE2)lpDDSBack, &ddsd);
            ddsd.dwFlags &= ~DDSD_PITCH;
        } else {
            ddsd.dwFlags = (DDSD_HEIGHT | DDSD_WIDTH | DDSD_CAPS);
            ddsd.ddsCaps.dwCaps = (DDSCAPS_OFFSCREENPLAIN);
            ddsd.dwHeight = dxw.GetScreenHeight();
            ddsd.dwWidth = dxw.GetScreenWidth();
        }
        // v2.04.09 fix: dxversion replaced with iPrimarySurfaceVersion - fixes "Gruntz" crash
        res2 = (*pCreateSurfaceMethod(iPrimarySurfaceVersion))(lpPrimaryDD, &ddsd, &lpddsTmp, NULL);
        if(res2) {
            OutTraceE("%s: CreateSurface ERROR %#x(%s) at %d\n", ApiRef, res2, ExplainDDError(res2), __LINE__);
            OutTraceE("> Size=%d lpPrimaryDD=%#x lpDDSBack=%#x %s\n",
                      ddsd.dwSize, lpPrimaryDD, lpDDSBack, LogSurfaceAttributes(&ddsd, "[FlipBuf]", __LINE__));
        }
        //OutTrace("DEBUG: copied surface size=(%dx%d)\n", ddsd.dwWidth, ddsd.dwHeight);
        // copy front buffer
        if(dxw.IsEmulated) {
            LPDIRECTDRAWSURFACE lpDDSSource;
            // in emulated mode, the primary surface is virtual and you can pick it all
            // needed for "Gruntz"
            // v2.04.81: some renderer don't make the primary buffer accessible for reading, so
            // it is necessary to initialize the temporary surface with the backbuffer.
            // Fixes the NOFLIPEMULATION processing with gdi & SDL renderers.
            lpDDSSource = (dxw.Renderer->flags & DXWRF_USEBACKBUFFER) ? lpDDSBack : lpdds;
            res2 = (*pBlt)(lpddsTmp, NULL, lpDDSSource, NULL, DDBLT_WAIT, NULL);
            if(res2) BlitError(res2, NULL, NULL,  __LINE__);
        } else {
            // in no-emulated mode, the primary surface is the whole screen, so you have to pick...
            // needed for "Black Thorn"
            RECT clip;
            //if(dxw.RendererId == DXW_RENDERER_EMULATEBUFFER)
            if(dxw.Renderer->flags & DXWRF_BUILDREALSURFACES)
                clip = dxw.GetScreenRect();
            else
                clip = dxw.GetUnmappedScreenRect();
            res2 = (*pBlt)(lpddsTmp, NULL, lpdds, &clip, DDBLT_WAIT, NULL);
            if(res2) BlitError(res2, &clip, NULL,  __LINE__);
        }
    }
    if(lpddssrc) {
        //res=lpdds->Blt(0, lpddssrc, 0, DDBLT_WAIT, 0);
        res = sBlt(dxversion, pBlt, ApiRef, lpdds, NULL, lpddssrc, NULL, dwBltFlags, 0, TRUE);
    } else {
        if(!lpDDSBack) {
            OutTraceE("%s: no backbuffer\n", ApiRef);
            return DDERR_INVALIDPARAMS;
        }
        // v2.05.19: always define a source RECT since the screen resolution could be
        // smaller than current backbuffer size (ref. "Flight Unlimited II & III)
        RECT NullArea = dxw.GetScreenRect();
        res = sBlt(dxversion, pBlt, ApiRef, lpdds, NULL, lpDDSBack, &NullArea, dwBltFlags, 0, TRUE);
        lpddssrc = lpDDSBack;
    }
    if(dxw.dwFlags4 & NOFLIPEMULATION) {
        HRESULT res2;
        // restore flipped backbuffer and delete temporary surface
        res2 = (*pBlt)(lpddssrc, NULL, lpddsTmp, NULL, DDBLT_WAIT, NULL);
#ifndef DXW_NOTRACES
        if(res2) OutTraceE("%s: Blt ERROR %#x(%s) at %d\n", ApiRef, res2, ExplainDDError(res2), __LINE__);
#endif // DXW_NOTRACES
        (*pReleaseSMethod(dxversion))(lpddsTmp);
    }
    _if(res) OutTraceE("%s: Blt ERROR %#x(%s)\n", ApiRef, res, ExplainDDError(res));
#ifndef DXW_NOTRACES
    if((dxw.dwDFlags & DUMPSURFACES) && dxw.bCustomKeyToggle) SurfaceDump(lpdds, dxversion);
    if((dxw.dwDFlags2 & DUMPBLITSRC) && dxw.bCustomKeyToggle) SurfaceDump(lpddssrc, dxversion);
#endif // DXW_NOTRACES
    return res;
}

HRESULT WINAPI extFlip1(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddssrc, DWORD dwflags) {
    return extFlip(1, pFlip1, lpdds, lpddssrc, dwflags);
}
HRESULT WINAPI extFlip2(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddssrc, DWORD dwflags) {
    return extFlip(2, pFlip2, lpdds, lpddssrc, dwflags);
}
HRESULT WINAPI extFlip3(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddssrc, DWORD dwflags) {
    return extFlip(3, pFlip3, lpdds, lpddssrc, dwflags);
}
HRESULT WINAPI extFlip4(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddssrc, DWORD dwflags) {
    return extFlip(4, pFlip4, lpdds, lpddssrc, dwflags);
}
HRESULT WINAPI extFlip7(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddssrc, DWORD dwflags) {
    return extFlip(7, pFlip7, lpdds, lpddssrc, dwflags);
}

HRESULT WINAPI extBlt(int dxversion, Blt_Type pBlt, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect,
                      LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    HRESULT res;
    if ((dxw.dwDFlags & FULLRECTBLT) && dxwss.IsAPrimarySurface(lpdds)) {
        lpsrcrect = NULL;
        lpdestrect = NULL;
    }
    res = sBlt(dxversion, pBlt, "Blt", lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx, FALSE);
#ifndef DXW_NOTRACES
    if((dxw.dwDFlags & DUMPSURFACES) && dxw.bCustomKeyToggle) SurfaceDump(lpdds, dxversion);
    if((dxw.dwDFlags2 & DUMPBLITSRC) && dxw.bCustomKeyToggle) SurfaceDump(lpddssrc, dxversion);
#endif // DXW_NOTRACES
    return res;
}

HRESULT WINAPI extBlt1(LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    return extBlt(1, pBlt1, lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
}
HRESULT WINAPI extBlt2(LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    return extBlt(2, pBlt2, lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
}
HRESULT WINAPI extBlt3(LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    return extBlt(3, pBlt3, lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
}
HRESULT WINAPI extBlt4(LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    return extBlt(4, pBlt4, lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
}
HRESULT WINAPI extBlt7(LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    return extBlt(7, pBlt7, lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
}

HRESULT WINAPI extBltBatch(int dxversion, BltBatch_Type pBltBatch, LPDIRECTDRAWSURFACE lpdds, LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags) {
    HRESULT res;
    ApiName("BltBatch");
    OutTraceDDRAW("%s(%d): lpdds=%#x count=%d flags=%#x\n", ApiRef, dxversion, dwCount, dwFlags);
    for(DWORD i = 0; i < dwCount; i++) {
        OutTraceDDRAW(">[%d] lpddssrc=%#x\n", i, lpDDBltBatch[i].lpDDSSrc);
        OutTraceDDRAW(">[%d] src=%#x\n", i, lpDDBltBatch[i].lprSrc);
        OutTraceDDRAW(">[%d] dst=%#x\n", i, lpDDBltBatch[i].lprDest);
        OutTraceDDRAW(">[%d] flags=%#x\n", i, lpDDBltBatch[i].dwFlags);
        OutTraceDDRAW(">[%d] bltfx=%#x\n", i, lpDDBltBatch[i].lpDDBltFx);
    }
    TRAP;
    res = (*pBltBatch)(lpdds, lpDDBltBatch, dwCount, dwFlags);
    _if(res) OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extBltBatch1(LPDIRECTDRAWSURFACE lpdds, LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags) {
    return extBltBatch(1, pBltBatch1, lpdds, lpDDBltBatch, dwCount, dwFlags);
}
HRESULT WINAPI extBltBatch2(LPDIRECTDRAWSURFACE lpdds, LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags) {
    return extBltBatch(2, pBltBatch2, lpdds, lpDDBltBatch, dwCount, dwFlags);
}
HRESULT WINAPI extBltBatch3(LPDIRECTDRAWSURFACE lpdds, LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags) {
    return extBltBatch(3, pBltBatch3, lpdds, lpDDBltBatch, dwCount, dwFlags);
}
HRESULT WINAPI extBltBatch4(LPDIRECTDRAWSURFACE lpdds, LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags) {
    return extBltBatch(4, pBltBatch4, lpdds, lpDDBltBatch, dwCount, dwFlags);
}
HRESULT WINAPI extBltBatch7(LPDIRECTDRAWSURFACE lpdds, LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags) {
    return extBltBatch(7, pBltBatch7, lpdds, lpDDBltBatch, dwCount, dwFlags);
}

static HRESULT WINAPI extBltFast(int dxversion, Blt_Type pBlt, BltFast_Type pBltFast,
                                 LPDIRECTDRAWSURFACE lpdds, DWORD dwx, DWORD dwy,
                                 LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwtrans) {
    // BltFast is supported just on screen surfaces, so it has to be replaced
    // by ordinary Blt operation in emulation mode.
    // Mind that screen surface doesn't necessarily mean PRIMARY surfaces!
    RECT srcrect, destrect;
    DWORD flags;
    DDSURFACEDESC2 ddsd;
    HRESULT res;
    BOOL ToPrim, FromPrim;
    ApiName("BltFast");
    ToPrim = dxwss.IsAPrimarySurface(lpdds);
    FromPrim = dxwss.IsAPrimarySurface(lpddssrc);
    CleanRect(&lpsrcrect, __LINE__);
    // set flags for possible Blt (non-fast) operation
    flags = 0;
    if(dwtrans & DDBLTFAST_WAIT) flags |= DDBLT_WAIT;
    if(dwtrans & DDBLTFAST_DONOTWAIT) flags |= DDBLT_DONOTWAIT; // v2.05.25 fix
    if(dwtrans & DDBLTFAST_DESTCOLORKEY) flags |= DDBLT_KEYDEST;
    if(dwtrans & DDBLTFAST_SRCCOLORKEY) flags |= DDBLT_KEYSRC;
    if(IsTraceDDRAW) {
        char sRect[81];
        if (lpsrcrect) sprintf(sRect, "(%d,%d)-(%d,%d)", lpsrcrect->left, lpsrcrect->top, lpsrcrect->right, lpsrcrect->bottom);
        else strcpy(sRect, "(NULL)");
        OutTrace("%s(%d): dest=%#x%s src=%#x%s dwTrans=%#x(%s) (x,y)=(%d,%d) srcrect=%s\n",
                 ApiRef, dxversion, lpdds, ToPrim ? "(PRIM)" : "",
                 lpddssrc, FromPrim ? "(PRIM)" : "", dwtrans,
                 ExplainBltFastFlags(dwtrans), dwx, dwy, sRect);
    }
    // "Oddworld Abe Oddysee" patch: don't emulate the strange BltFast operation on single video lines
    if(FromPrim && ToPrim) {
#ifdef NOFASTBLT
        RECT drect;
        drect.top = dwy;
        drect.left = dwx;
        if (lpsrcrect) drect.bottom = dwy + (lpsrcrect->bottom - lpsrcrect->top);
        if (lpsrcrect) drect.right = dwx  + (lpsrcrect->right - lpsrcrect->left);
        res = pBlt(lpdds, &drect, lpddssrc, lpsrcrect, flags, 0);
        _if(res) OutTraceE("%s: Blt ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
#else
        res = pBltFast(lpdds, dwx, dwy, lpddssrc, lpsrcrect, dwtrans);
        _if(res) OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
#endif
        if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
        return res;
    }
    // between not-virtual surfaces, try the actual method first, it may work in some corcumstances....
    // when ret is DDERR_UNSUPPORTED try the emulated path.
    if(!(ToPrim || FromPrim)) {
        if(dxw.dwFlags5 & MESSAGEPUMP) dxw.MessagePump();
#ifdef NOFASTBLT
        RECT drect;
        drect.top = dwy;
        drect.left = dwx;
        if (lpsrcrect) drect.bottom = dwy + (lpsrcrect->bottom - lpsrcrect->top);
        if (lpsrcrect) drect.right = dwx  + (lpsrcrect->right - lpsrcrect->left);
        res = (*pBlt)(lpdds, &drect, lpddssrc, lpsrcrect, flags, 0);
#else
        res = pBltFast(lpdds, dwx, dwy, lpddssrc, lpsrcrect, dwtrans);
#endif
        if(res != DDERR_UNSUPPORTED) {
            _if(res) OutTraceE("%s ERROR: res=%#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
            if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
            return res;
        }
    }
    // consistency checks ....
    // v2.04.51: fixed criteria to avoid "Kiss Pinball" tables to show partially
    if (lpsrcrect) {
        // valid rectangle, situation found for instance in "Warlords Battlecry II demo".
        if((lpsrcrect->left >= lpsrcrect->right) || (lpsrcrect->top >= lpsrcrect->bottom)) {
            OutTraceDW("%s: ASSERT bad rect at %d\n", ApiRef, __LINE__);
            return 0;
        }
        // "Road Rash" fix: if the passed src rect is invalid, let's trim it.
        DDSURFACEDESC2 ddsd;
        memset(&ddsd, 0, sizeof(ddsd));
        ddsd.dwSize = (dxversion < 4) ? sizeof(DDSURFACEDESC) : sizeof(DDSURFACEDESC2);
        (*pGetSurfaceDescMethod())((LPDIRECTDRAWSURFACE2)lpddssrc, &ddsd);
        if(lpsrcrect->right > (LONG)ddsd.dwWidth) lpsrcrect->right = ddsd.dwWidth;
        if(lpsrcrect->bottom > (LONG)ddsd.dwHeight) lpsrcrect->bottom = ddsd.dwHeight;
    }
    if ((dxw.dwDFlags & FULLRECTBLT) && ToPrim) {
        res = sBlt(dxversion, pBlt, ApiRef, lpdds, NULL, lpddssrc, lpsrcrect, flags, NULL, FALSE);
#ifndef DXW_NOTRACES
        if(res)
            OutTraceE("%s: FULLRECBLT res=%#x\n", ApiRef, res);
        else
            OutTraceDDRAW("%s: FULLRECBLT res=DD_OK\n", ApiRef);
#endif // DXW_NOTRACES
        return res;
    }
    destrect.left = dwx;
    destrect.top = dwy;
    if(lpsrcrect) {
        destrect.right = destrect.left + lpsrcrect->right - lpsrcrect->left;
        destrect.bottom = destrect.top + lpsrcrect->bottom - lpsrcrect->top;
        // avoid altering pointed values....
        srcrect = *lpsrcrect;
        res = sBlt(dxversion, pBlt, ApiRef, lpdds, &destrect, lpddssrc, &srcrect, flags, NULL, FALSE);
    } else {
        // does it EVER goes through here? NULL is not a valid rect value for BltFast call....
        // yes, forced in FULLRECTBLT mode!
        // yes, when BltFast on DDBLTFAST_SRCCOLORKEY!! (Pax Corpus)
        // yes, in RoadRash too.
        if(lpddssrc) {
            ddsd.dwSize = Set_dwSize_From_Surface();
            ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT;
            res = lpddssrc->GetSurfaceDesc((LPDDSURFACEDESC)&ddsd);
            if (res) {
                OutTraceE("%s: GetSurfaceDesc ERROR %#x at %d\n", ApiRef, res, __LINE__);
                return DD_OK;
            }
            destrect.right = destrect.left + ddsd.dwWidth;
            destrect.bottom = destrect.top + ddsd.dwHeight;
            res = sBlt(dxversion, pBlt, ApiRef, lpdds, &destrect, lpddssrc, NULL, flags, NULL, FALSE);
        } else {
            //ret=sBlt("BltFast", lpdds, NULL, lpddssrc, NULL, flags, NULL, FALSE);
            OutTraceDW("%s FAKE res=DD_OK at %d\n", ApiRef, __LINE__);
            res = DD_OK;
        }
    }
#ifndef DXW_NOTRACES
    if(res)
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else
        OutTraceDDRAW("%s: res=DD_OK\n", ApiRef);
    if((dxw.dwDFlags & DUMPSURFACES) && dxw.bCustomKeyToggle) SurfaceDump(lpdds, dxversion);
    if((dxw.dwDFlags2 & DUMPBLITSRC) && dxw.bCustomKeyToggle) SurfaceDump(lpddssrc, dxversion);
#endif // DXW_NOTRACES
    return res;
}

HRESULT WINAPI extBltFast1(LPDIRECTDRAWSURFACE lpdds, DWORD dwx, DWORD dwy, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwtrans) {
    return extBltFast(1, pBlt1, pBltFast1, lpdds, dwx, dwy, lpddssrc, lpsrcrect, dwtrans);
}
HRESULT WINAPI extBltFast2(LPDIRECTDRAWSURFACE lpdds, DWORD dwx, DWORD dwy, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwtrans) {
    return extBltFast(2, pBlt2, pBltFast2, lpdds, dwx, dwy, lpddssrc, lpsrcrect, dwtrans);
}
HRESULT WINAPI extBltFast3(LPDIRECTDRAWSURFACE lpdds, DWORD dwx, DWORD dwy, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwtrans) {
    return extBltFast(3, pBlt3, pBltFast3, lpdds, dwx, dwy, lpddssrc, lpsrcrect, dwtrans);
}
HRESULT WINAPI extBltFast4(LPDIRECTDRAWSURFACE lpdds, DWORD dwx, DWORD dwy, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwtrans) {
    return extBltFast(4, pBlt4, pBltFast4, lpdds, dwx, dwy, lpddssrc, lpsrcrect, dwtrans);
}
HRESULT WINAPI extBltFast7(LPDIRECTDRAWSURFACE lpdds, DWORD dwx, DWORD dwy, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwtrans) {
    return extBltFast(7, pBlt7, pBltFast7, lpdds, dwx, dwy, lpddssrc, lpsrcrect, dwtrans);
}

HRESULT WINAPI extWaitForVerticalBlank(WaitForVerticalBlank_Type pWaitForVerticalBlank, LPDIRECTDRAW lpdd, DWORD dwflags, HANDLE hevent) {
    HRESULT res;
    ApiName("WaitForVerticalBlank");
#ifdef TRACEALL
    OutTraceDDRAW("%s: lpdd=%#x flags=%#x hevent=%#x\n", ApiRef, lpdd, dwflags, hevent);
#endif
    if(dxw.dwFlags8 & FORCENOVSYNC) return DD_OK;
    if(dxw.dwFlags1 & SAVELOAD) {
        dxw.VSyncWait();
        return DD_OK;
    }
    if(dxw.dwFlags8 & VSYNCSCANLINE) {
        dxw.VSyncWaitLine(((DWORD)dxw.ScanLine * 100) / dxw.GetScreenHeight());
        return DD_OK;
    }
    res = (*pWaitForVerticalBlank)(lpdd, dwflags, hevent);
#ifdef TRACEALL
    OutTraceDDRAW("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
#endif
    return res;
}

HRESULT WINAPI extWaitForVerticalBlank1(LPDIRECTDRAW lpdd, DWORD dwflags, HANDLE hevent) {
    return extWaitForVerticalBlank(pWaitForVerticalBlank1, lpdd, dwflags, hevent);
}
HRESULT WINAPI extWaitForVerticalBlank2(LPDIRECTDRAW lpdd, DWORD dwflags, HANDLE hevent) {
    return extWaitForVerticalBlank(pWaitForVerticalBlank2, lpdd, dwflags, hevent);
}
HRESULT WINAPI extWaitForVerticalBlank3(LPDIRECTDRAW lpdd, DWORD dwflags, HANDLE hevent) {
    return extWaitForVerticalBlank(pWaitForVerticalBlank3, lpdd, dwflags, hevent);
}
HRESULT WINAPI extWaitForVerticalBlank4(LPDIRECTDRAW lpdd, DWORD dwflags, HANDLE hevent) {
    return extWaitForVerticalBlank(pWaitForVerticalBlank4, lpdd, dwflags, hevent);
}
HRESULT WINAPI extWaitForVerticalBlank7(LPDIRECTDRAW lpdd, DWORD dwflags, HANDLE hevent) {
    return extWaitForVerticalBlank(pWaitForVerticalBlank7, lpdd, dwflags, hevent);
}

#define DDPCAPS_INITIALIZE_LEGACY 0x00000008l

HRESULT WINAPI extCreatePalette(int version, CreatePalette_Type pCreatePalette,
                                LPDIRECTDRAW lpdd, DWORD dwflags, LPPALETTEENTRY lpddpa, LPDIRECTDRAWPALETTE *lplpddp, IUnknown *pu) {
    HRESULT res;
    ApiName("CreatePalette");
    OutTraceDDRAW("%s: lpdd=%#x dwFlags=%#x(%s)\n", ApiRef, lpdd, dwflags, ExplainCreatePaletteFlags(dwflags));
    if(IsDebugDW && (dwflags & DDPCAPS_8BIT)) dxw.DumpPalette(256, lpddpa);
    if(dxw.IsEmulated) dwflags &= ~DDPCAPS_PRIMARYSURFACE;
    if(dwflags & DDPCAPS_ALLOW256) dxw.ReservedPaletteEntries = 0; // v2.05.53
    res = (*pCreatePalette)(lpdd, dwflags, lpddpa, lplpddp, pu);
    if (res) {
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceDDRAW("%s: OK lpddp=%#x\n", ApiRef, *lplpddp);
    HookDDPalette(lplpddp);
    return DD_OK;
}

HRESULT WINAPI extCreatePalette1(LPDIRECTDRAW lpdd, DWORD dwflags, LPPALETTEENTRY lpddpa, LPDIRECTDRAWPALETTE *lplpddp, IUnknown *pu) {
    return extCreatePalette(1, (CreatePalette_Type)pCreatePalette1, lpdd, dwflags, lpddpa, lplpddp, pu);
}
HRESULT WINAPI extCreatePalette2(LPDIRECTDRAW lpdd, DWORD dwflags, LPPALETTEENTRY lpddpa, LPDIRECTDRAWPALETTE *lplpddp, IUnknown *pu) {
    return extCreatePalette(2, (CreatePalette_Type)pCreatePalette2, lpdd, dwflags, lpddpa, lplpddp, pu);
}
HRESULT WINAPI extCreatePalette3(LPDIRECTDRAW lpdd, DWORD dwflags, LPPALETTEENTRY lpddpa, LPDIRECTDRAWPALETTE *lplpddp, IUnknown *pu) {
    return extCreatePalette(3, (CreatePalette_Type)pCreatePalette3, lpdd, dwflags, lpddpa, lplpddp, pu);
}
HRESULT WINAPI extCreatePalette4(LPDIRECTDRAW lpdd, DWORD dwflags, LPPALETTEENTRY lpddpa, LPDIRECTDRAWPALETTE *lplpddp, IUnknown *pu) {
    return extCreatePalette(4, (CreatePalette_Type)pCreatePalette4, lpdd, dwflags, lpddpa, lplpddp, pu);
}
HRESULT WINAPI extCreatePalette7(LPDIRECTDRAW lpdd, DWORD dwflags, LPPALETTEENTRY lpddpa, LPDIRECTDRAWPALETTE *lplpddp, IUnknown *pu) {
    return extCreatePalette(7, (CreatePalette_Type)pCreatePalette7, lpdd, dwflags, lpddpa, lplpddp, pu);
}

HRESULT WINAPI extGetPalette(int dxversion, GetPalette_Type pGetPalette, LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWPALETTE *lplpddp) {
    HRESULT res;
    ApiName("GetPalette");
    BOOL isPrim, isBack;
    isPrim = dxwss.IsAPrimarySurface(lpdds);
    isBack = dxwss.IsABackBufferSurface(lpdds);
    OutTraceDDRAW("%s(%d): lpdds=%#x%s%s\n", ApiRef, dxversion, lpdds, isPrim ? "(PRIM)" : "", isBack ? "(BACK)" : "");
    if((dxw.IsEmulated) && (isPrim || isBack)) {
        // v2.05.15: sleepwalker fix ...
        if((dxw.VirtualPixelFormat.dwRGBBitCount != 8) || (lpDDP == NULL)) {
            OutTraceDW("%s: RGB=%d lpDDP=%#x simulate err=DDERR_NOPALETTEATTACHED lpDDP=NULL\n",
                       ApiRef, dxw.VirtualPixelFormat.dwRGBBitCount, lpDDP);
            res = DDERR_NOPALETTEATTACHED;
            *lplpddp = 0;
            return res;
        }
    }
    res = DD_OK;
    lpdds->AddRef();
    if(lpdds->Release()) res = (*pGetPalette)(lpdds, lplpddp);
    // v2.03.07: in "Die Hard Trilogy" the backbuffer surface is queryed for the palette
    // v2.03.08: in "Viper Racing" lpDDP is still NULL (how could it be?)
    // v2.04.25: the reference counter for palette attach can't be incremented more than once
    if((dxw.IsEmulated) && (res == DDERR_NOPALETTEATTACHED) && (isPrim || isBack) && lpDDP) {
        OutTraceDW("%s: retrieve PRIMARY palette for emulated surface lpDDP=%#x\n", ApiRef, lpDDP);
        *lplpddp = lpDDP;
        lpDDP->AddRef();
        iDDPExtraRefCounter = 1;
        res = DD_OK;
    }
    if (res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else
        OutTraceDDRAW("%s: OK lplpddp=%#x\n", ApiRef, *lplpddp);
    return res;
}

HRESULT WINAPI extGetPalette1(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWPALETTE *lplpddp) {
    return extGetPalette(1, pGetPalette1, lpdds, lplpddp);
}
HRESULT WINAPI extGetPalette2(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWPALETTE *lplpddp) {
    return extGetPalette(2, pGetPalette2, lpdds, lplpddp);
}
HRESULT WINAPI extGetPalette3(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWPALETTE *lplpddp) {
    return extGetPalette(3, pGetPalette3, lpdds, lplpddp);
}
HRESULT WINAPI extGetPalette4(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWPALETTE *lplpddp) {
    return extGetPalette(4, pGetPalette4, lpdds, lplpddp);
}
HRESULT WINAPI extGetPalette7(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWPALETTE *lplpddp) {
    return extGetPalette(7, pGetPalette7, lpdds, lplpddp);
}

HRESULT WINAPI extSetPalette(int dxversion, SetPalette_Type pSetPalette, LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWPALETTE lpddp) {
    PALETTEENTRY *lpentries;
    BOOL isPrim;
    HRESULT res;
    ApiName("SetPalette");
    isPrim = dxwss.IsAPrimarySurface(lpdds);
    OutTraceDDRAW("%s(%d): lpdds=%#x%s lpddp=%#x\n", ApiRef, dxversion, lpdds, isPrim ? "(PRIM)" : "", lpddp);
    if(dxw.dwDFlags2 & FIXRANDOMPALETTE) {
        lpentries = (LPPALETTEENTRY)PaletteEntries;
        for(int i = 0; i < 256; i++) {
            lpentries->peRed = rand() % 0x100;
            lpentries->peGreen = rand() % 0x100;
            lpentries->peBlue = rand() % 0x100;
            lpentries++;
        }
        lpentries = (LPPALETTEENTRY)PaletteEntries;
        mySetPalette(0, 256, lpentries);
        return DD_OK;
    }
    if((dxw.dwFlags9 & SAFEPALETTEUSAGE) && (lpddp == 0)) {
        // avoid clearing palette
        lpentries = (LPPALETTEENTRY)PaletteEntries;
        memset(lpentries, 0, sizeof(PaletteEntries));
        mySetPalette(0, 256, lpentries);
        return DD_OK;
    }
    __try {
        res = (*pSetPalette)(lpdds, lpddp);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        return DD_OK;
    }
    if(res) {
        OutTraceE("%s: ERROR res=%#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
        // from MSDN, about lpddp:
        // A pointer to the IDirectDrawPalette interface for the palette object to be used with this surface.
        // If NULL, the current palette is detached.
        // if(lpddp) lpddp->AddRef(); // to allow final Release() without crash - fixes "Heavy Gear" crash at end of mission
        // v2.04.25: "Speedboat Attack" has a real bug in palette handling at the end of a race, the lpddp
        // palette descriptor gets corrupted, so we don't have the usual error for no palette capability but
        // a more severe error (DDERR_INVALIDPARAMS). In this situation (and SAFEPALETTEUSAGE mode) any operation
        // should be safely skipped. BTW the game shows correct colors anyway.
        // v2.04.90: same thing happens in korean game "Deicide 3", but at the beginning of the game.
        if((res == DDERR_INVALIDPARAMS) && (dxw.dwFlags9 & SAFEPALETTEUSAGE))
            return DD_OK;
    } else OutTraceDDRAW("%s: OK\n", ApiRef);
    res = DD_OK;
    if((dxw.IsEmulated) && isPrim) {
        OutTraceDW("%s: register PRIMARY palette lpDDP=%#x\n", ApiRef, lpddp);
        lpDDP = lpddp;
        LPDIRECTDRAWSURFACE lpDDSBack;
        if(lpddp) {
            HRESULT res2;
            lpentries = (LPPALETTEENTRY)PaletteEntries;
            res2 = lpddp->GetEntries(0, 0, 256, lpentries);
#ifndef DXW_NOTRACES
            if(res2) OutTraceE("%s: GetEntries ERROR res=%#x(%s)\n", ApiRef, res2, ExplainDDError(res2));
#endif // DXW_NOTRACES
            mySetPalette(0, 256, lpentries); // v2.02.76: necessary for "Requiem Avenging Angel" in SURFACEEMULATION mode
        }
        // Apply palette to backbuffer surface. This is necessary on some games: "Duckman private dick", "Total Soccer 2000", ...
        lpDDSBack = dxwss.GetBackBufferSurface();
        if (lpDDSBack) {
            OutTraceDW("%s: apply PRIMARY palette lpDDP=%#x to DDSBack=%#x\n", ApiRef, lpddp, lpDDSBack);
            res = (*pSetPalette)(lpDDSBack, lpddp);
            _if(res) OutTraceE("%s: ERROR res=%#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
        }
        res = DD_OK;
    }
    return res;
}

HRESULT WINAPI extSetPalette1(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWPALETTE lpddp) {
    return extSetPalette(1, pSetPalette1, lpdds, lpddp);
}
HRESULT WINAPI extSetPalette2(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWPALETTE lpddp) {
    return extSetPalette(2, pSetPalette2, lpdds, lpddp);
}
HRESULT WINAPI extSetPalette3(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWPALETTE lpddp) {
    return extSetPalette(3, pSetPalette3, lpdds, lpddp);
}
HRESULT WINAPI extSetPalette4(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWPALETTE lpddp) {
    return extSetPalette(4, pSetPalette4, lpdds, lpddp);
}
HRESULT WINAPI extSetPalette7(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWPALETTE lpddp) {
    return extSetPalette(7, pSetPalette7, lpdds, lpddp);
}

HRESULT WINAPI extSetEntries(LPDIRECTDRAWPALETTE lpddp, DWORD dwflags, DWORD dwstart, DWORD dwcount, LPPALETTEENTRY lpentries) {
    HRESULT res;
    ApiName("SetEntries");
    UINT nStatCols = (dxw.ReservedPaletteEntries >> 1);	// v2.05.53: revised palette handling
    OutTraceDDRAW("%s: lpddp=%#x dwFlags=%#x, start=%d, count=%d entries=%#x\n", //GHO: added trace infos
                  ApiRef, lpddp, dwflags, dwstart, dwcount, lpentries);
    if(IsDebugDW) dxw.DumpPalette(dwcount, &lpentries[dwstart]);
    // v2.05.53: check palette overflow
    if(dwcount > 256) {
        OutTraceDW("%s: trim palette count lpddp=%#x count=%d->256\n", ApiRef, lpddp, dwcount);
        dwcount = 256;
    }
    // v2.05.53: palette revised
    if((dxw.IsEmulated) && (lpDDP == lpddp)) {
        OutTraceDW("%s: update PRIMARY palette lpDDP=%#x\n", ApiRef, lpddp);
        if (nStatCols > dwstart) dwstart = nStatCols;
        if (nStatCols + dwstart + dwcount > 256) {
            OutTraceDW("%s: ASSERT reserved=%d+start=%d+count=%d > 256\n",
                       ApiRef, dxw.ReservedPaletteEntries, dwstart, dwcount);
            dwcount = 256 - dxw.ReservedPaletteEntries;
            dwstart = nStatCols;
        }
        mySetPalette(dwstart, dwcount, lpentries);
        // GHO: needed for fixed rect and variable palette animations,
        // e.g. dungeon keeper loading screen, Warcraft II splash, ...
        // GHO: but refreshing cause flickering when GDI was used without updating the primary surface
        // e.g. Tomb Raider 2 intro titles, Virtua Fighter PC, ...
        // v2.03.10: do not blit also in case of GDI mode
        if ((dxw.IsEmulated) && !(dxw.dwFlags2 & NOPALETTEUPDATE) && !(dxw.Renderer->flags & DXWRF_PALETTEREFRESH)) dxw.ScreenRefresh(TRUE);
    }
    // this part (setentry against all surfaces, including virtual primary) can be necessary whenever the game uses mixed access to the
    // screen (like ddraw & GDI) and is necessary on "Road Rash".
    if(dxw.dwDFlags2 & EXPERIMENTAL) {
        for(UINT i = 0; i < dwcount; i++) {
            LPPALETTEENTRY lppal = &lpentries[i];
            if(lppal->peFlags == PC_EXPLICIT)
                lppal->peFlags = PC_NOCOLLAPSE;
        }
    }
    res = (*pSetEntries)(lpddp, dwflags, dwstart, dwcount, lpentries);
#ifndef DXW_NOTRACES
    if(res) OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else OutTraceDDRAW("%s: OK\n", ApiRef);
#endif // DXW_NOTRACES
    return res;
}

static HRESULT WINAPI extSetClipper(int dxversion, SetClipper_Type pSetClipper, LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWCLIPPER lpddc) {
    HRESULT res;
    ApiName("SetClipper");
    BOOL isPrim;
    isPrim = dxwss.IsAPrimarySurface(lpdds);
    OutTraceDDRAW("%s(%d): lpdds=%#x%s lpddc=%#x\n", ApiRef, dxversion, lpdds, dxwss.ExplainSurfaceRole(lpdds), lpddc);
    // v2.1.84: SUPPRESSCLIPPING flag - improves "Monopoly Edition 3D" where continuous
    // clipping ON & OFF affects blitting on primary surface.
    // Needed also to avoid "New Yourk Racer" intro movie clipping.
    if(dxw.dwFlags1 & SUPPRESSCLIPPING) return 0;
    // v2.05.52 fix: found in "Caponata" (Jap funny game)
    // in emulation mode (and also in emulated buffer mode) you can't set clippers to a backbuffer
    // using a reference window because the window size is deceiving (it is supposed to be the same size
    // of the surface, but it isn't!) and could change size. This is ok only in case of the real primary
    // surface, in all other cases better use a RECT as a reference.
    if(dxw.IsEmulated || (dxw.Renderer->flags & DXWRF_BUILDLOCKBUFFER)) {
        if(dxwss.IsABackBufferSurface(lpdds)) {
            HWND hwnd = 0;
            lpddc->GetHWnd(&hwnd);
            if(hwnd) {
                RGNDATA *rgn = (RGNDATA *)malloc(sizeof(RGNDATA) + sizeof(RECT));
                rgn->rdh.dwSize = sizeof(RGNDATAHEADER);
                rgn->rdh.iType = RDH_RECTANGLES;
                rgn->rdh.nCount = 1;
                rgn->rdh.nRgnSize = 0;
                rgn->rdh.rcBound = dxw.GetScreenRect();
                RECT *r = (RECT *)rgn->Buffer;
                *r = dxw.GetScreenRect();
                //lpddc->SetHWnd(0, 0);
                res = (*pSetHWnd)(lpddc, 0, 0);
                //lpddc->SetClipList(rgn, 0);
                res = (*pSetClipList)(lpddc, rgn, 0);
                free(rgn);
            }
        }
        if (dxw.dwFlags3 & FORCECLIPPER) {
            // v2.03.48: FORCECLIPPER ensures that a valid clipper is issued on the real primary surface
            // and on the main game window. Then, you won't be able to BltFast any longer....
            if (isPrim && lpDDSEmu_Prim) {
                OutTraceDW("%s: redirect surface to primary hwnd=%#x lpdds=%#x%s->%#x\n",
                           ApiRef, dxw.GethWnd(), lpdds, isPrim ? "(PRIM)" : "", lpDDSEmu_Prim);
                res = (*pSetHWnd)(lpddc, 0, dxw.GethWnd());
                //res=lpddc->SetHWnd(0, dxw.GethWnd());
                OutTraceE("%s: SetHWnd ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
                res = (*pSetClipper)(lpDDSEmu_Prim, lpddc);
                _if(res) OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
                return res;
            }
        } else {
            if (isPrim && lpDDSEmu_Prim) {
                OutTraceDW("%s: skip primary lpdds=%#x\n", ApiRef, lpdds);
                return 0;
            }
        }
    }
    res = (*pSetClipper)(lpdds, lpddc);
    _if(res) OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extSetClipper1(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWCLIPPER lpddc) {
    return extSetClipper(1, pSetClipper1, lpdds, lpddc);
}
HRESULT WINAPI extSetClipper2(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWCLIPPER lpddc) {
    return extSetClipper(2, pSetClipper2, lpdds, lpddc);
}
HRESULT WINAPI extSetClipper3(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWCLIPPER lpddc) {
    return extSetClipper(3, pSetClipper3, lpdds, lpddc);
}
HRESULT WINAPI extSetClipper4(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWCLIPPER lpddc) {
    return extSetClipper(4, pSetClipper4, lpdds, lpddc);
}
HRESULT WINAPI extSetClipper7(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWCLIPPER lpddc) {
    return extSetClipper(7, pSetClipper7, lpdds, lpddc);
}

DDSURFACEDESC SaveSurfaceDesc;
LPDIRECTDRAWSURFACE SaveSurface = NULL;
LPRECT SaveLockedlpRect = NULL;
RECT SaveLockedRect;
LPDIRECTDRAWSURFACE SaveLockedSurface = NULL;
LPVOID SafeData;

static void PushLockedRect(LPDIRECTDRAWSURFACE lpdds, LPRECT lprect) {
    SaveLockedSurface = lpdds;
    SaveLockedlpRect = lprect;
    if(SaveLockedlpRect) SaveLockedRect = *lprect;
}

static LPRECT PopLockedRect(LPDIRECTDRAWSURFACE lpdds, LPRECT lprect) {
    if(lpdds == SaveLockedSurface) {
        SaveLockedSurface = NULL;
        return SaveLockedlpRect;
    } else
        return lprect;
}

LPDIRECTDRAWSURFACE2 lpDDSBuffer = NULL;

static HRESULT WINAPI extLock(int dxversion, Lock_Type pLock, LPDIRECTDRAWSURFACE lpdds, LPRECT lprect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD flags, HANDLE hEvent) {
    HRESULT res;
    CleanRect(&lprect, __LINE__);
    if(IsTraceDDRAW) {
        BOOL IsPrim = dxwss.IsAPrimarySurface(lpdds);
        char sRect[81];
        if (lprect) sprintf_s(sRect, 80, "(%d,%d)-(%d,%d)", lprect->left, lprect->top, lprect->right, lprect->bottom);
        else strcpy(sRect, "(NULL)");
        OutTrace("Lock(%d): lpdds=%#x%s flags=%#x(%s) lpDDSurfaceDesc=%#x rect=%s\n",
                 dxversion, lpdds, (IsPrim ? "(PRIM)" : ""), flags, ExplainLockFlags(flags), lpDDSurfaceDesc, sRect);
    }
    // don't mention Win16 features ... found in "Star Wars X-Wing" Win95 port XWING95.EXE
    // v2.05.40: don't clear the flag in ddraw7 Lock. Fixes "Battle realms"
    // to be checked ...
    //if(dxversion < 7) flags &= ~DDLOCK_NOSYSLOCK;
    PushLockedRect(lpdds, lprect);
    if(!dxw.IsEmulated) { // direct mode
        RECT client;
        POINT upleft = {0, 0};
        // V2.02.43: Empire Earth does some test Lock operations apparently before the primary surface is created
        if(lpPrimaryDD) {
            if(dxwss.IsAPrimarySurface(lpdds)) {
                if(dxw.Renderer->flags & DXWRF_BUILDLOCKSURFACE) {
                    DDSURFACEDESC2 ddsd;
                    memset(&ddsd, 0, sizeof(ddsd));
                    ddsd.dwSize   = (dxversion < 4) ? sizeof(DDSURFACEDESC) : sizeof(DDSURFACEDESC2);
                    ddsd.dwFlags  = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
                    ddsd.dwWidth  = dxw.GetScreenWidth();
                    ddsd.dwHeight = dxw.GetScreenHeight();
                    ddsd.ddsCaps.dwCaps = 0;
                    OutDebugDW("Lock: %s\n", LogSurfaceAttributes(&ddsd, "[Dir FixBuf]", __LINE__));
                    res = (*pCreateSurfaceMethod(dxversion))(lpPrimaryDD, &ddsd, (LPDIRECTDRAWSURFACE *)&lpDDSBuffer, 0);
                    if(res) {
                        OutTraceE("Lock: CreateSurface ERROR on DDSBuffer res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
                        return res;
                    }
                    lpdds = (LPDIRECTDRAWSURFACE)lpDDSBuffer;
                } else {
                    // v2.04.79: DXW_RENDERER_EMULATEBUFFER = "primary buffer" doesn't need (and can't)
                    // update the target RECT coordinates. The logic is still good for DXW_RENDERER_NONE.
                    // Generalized using Renderer->flags DXWRF_BUILDLOCKBUFFER criteria
                    if(!(dxw.Renderer->flags & DXWRF_BUILDLOCKBUFFER)) {
                        // since it can't scale, at least the updated rect is centered into the window.
                        HWND hwnd;
                        hwnd = dxw.GethWnd();
                        (*pGetClientRect)(hwnd, &client);
                        (*pClientToScreen)(hwnd, &upleft);
                        if (!lprect) lprect = &client;
                        OffsetRect(lprect,
                                   upleft.x + (client.right - dxw.GetScreenWidth()) / 2,
                                   upleft.y + (client.bottom - dxw.GetScreenHeight()) / 2);
                        OutTraceDW("Lock: NULL rect remapped to (%d,%d)-(%d,%d)\n",
                                   lprect->left, lprect->top, lprect->right, lprect->bottom);
                    }
                }
            }
        }
    }
    res = (*pLock)(lpdds, lprect, lpDDSurfaceDesc, flags, hEvent);
    if(res == DDERR_SURFACEBUSY) { // v70: fix for "Ancient Evil"
        (*pUnlockMethod(dxversion))(lpdds, NULL);
        res = (*pLock)(lpdds, lprect, lpDDSurfaceDesc, flags, hEvent);
        OutTraceDW("Lock SURFACEBUSY RETRY: ret=%#x(%s)\n", res, ExplainDDError(res));
    }
    if(res == DDERR_SURFACELOST) {
        lpdds->Restore();
        res = (*pLock)(lpdds, lprect, lpDDSurfaceDesc, flags, hEvent);
        OutTraceDW("Lock SURFACELOST RETRY: ret=%#x(%s)\n", res, ExplainDDError(res));
    }
    _if(res) OutTraceE("Lock ERROR: ret=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    OutDebugDW("Lock: lPitch=%d lpSurface=%#x %s\n",
               lpDDSurfaceDesc->lPitch, lpDDSurfaceDesc->lpSurface, LogSurfaceAttributes((LPDDSURFACEDESC2)lpDDSurfaceDesc, "[Locked]", __LINE__));
    // v2.03.60: necessary for "Mech Commander 2"
    if(dxw.dwFlags6 & SETZBUFFERBITDEPTHS)
        lpDDSurfaceDesc->dwZBufferBitDepth = dxw.VirtualPixelFormat.dwRGBBitCount;
    if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    if((dxw.dwFlags6 & FIXPITCH) || (dxw.dwDFlags & MARKLOCK)) {
        SaveSurfaceDesc = *lpDDSurfaceDesc;
        SaveSurface = lpdds;
    }
    if((dxw.dwFlags10 & SAFEPRIMLOCK) && dxwss.IsAPrimarySurface(lpdds)) {
        // ref. "Signus the Artefact Wars". The memory overflow here is not small ...
        int size =
            lpDDSurfaceDesc->dwHeight *
            lpDDSurfaceDesc->dwWidth *
            (lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount / 8);
        SafeData = malloc(size * 2); // double the minimum necessary size !!
        memcpy(SafeData, lpDDSurfaceDesc->lpSurface, size);
        SaveSurfaceDesc = *lpDDSurfaceDesc;
        lpDDSurfaceDesc->lpSurface = SafeData;
    }
    return res;
}

HRESULT WINAPI extLock1(LPDIRECTDRAWSURFACE lpdds, LPRECT lprect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD flags, HANDLE hEvent) {
    return extLock(1, pLock1, lpdds, lprect, lpDDSurfaceDesc, flags, hEvent);
}
HRESULT WINAPI extLock2(LPDIRECTDRAWSURFACE lpdds, LPRECT lprect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD flags, HANDLE hEvent) {
    return extLock(2, pLock2, lpdds, lprect, lpDDSurfaceDesc, flags, hEvent);
}
HRESULT WINAPI extLock3(LPDIRECTDRAWSURFACE lpdds, LPRECT lprect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD flags, HANDLE hEvent) {
    return extLock(3, pLock3, lpdds, lprect, lpDDSurfaceDesc, flags, hEvent);
}
HRESULT WINAPI extLock4(LPDIRECTDRAWSURFACE lpdds, LPRECT lprect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD flags, HANDLE hEvent) {
    return extLock(4, pLock4, lpdds, lprect, lpDDSurfaceDesc, flags, hEvent);
}
HRESULT WINAPI extLock7(LPDIRECTDRAWSURFACE lpdds, LPRECT lprect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD flags, HANDLE hEvent) {
    return extLock(7, pLock7, lpdds, lprect, lpDDSurfaceDesc, flags, hEvent);
}

#define MARK16COLOR 0x7C1F
#define MARK32COLOR 0x00FF00FF

static HRESULT WINAPI extUnlock(int dxversion, Unlock4_Type pUnlock, LPDIRECTDRAWSURFACE lpdds, LPRECT lprect) {
    HRESULT res;
    ApiName("Unlock");
    BOOL IsPrim;
    BOOL IsBack;
    Blt_Type pBlt;
    RECT rect;
    IsPrim = dxwss.IsAPrimarySurface(lpdds);
    IsBack = dxwss.IsABackBufferSurface(lpdds);
    if(IsTraceDDRAW) {
        char sRect[81];
        switch(dxversion) {
        case 1:
        case 2:
        case 3:
            sprintf_s(sRect, 80, "lpvoid=%#x", lprect);
            break;
        case 4:
        case 7:
            if (lprect) sprintf_s(sRect, 80, "rect=(%d,%d)-(%d,%d)", lprect->left, lprect->top, lprect->right, lprect->bottom);
            else strcpy(sRect, "rect=(NULL)");
            break;
        }
        OutTrace("%s(%d): lpdds=%#x%s %s\n", ApiRef, dxversion, lpdds, (IsPrim ? "(PRIM)" : (IsBack ? "(BACK)" : "")), sRect);
    }
    if ((dxw.dwFlags8 & ZBUFFERHARDCLEAN) && dxwss.IsAZBufferSurface(lpdds)) {
        DWORD dwSize = (dxversion < 4) ? sizeof(DDSURFACEDESC) : sizeof(DDSURFACEDESC2);
        DDSURFACEDESC2 ddsd;
        memset(&ddsd, 0, dwSize);
        ddsd.dwSize = dwSize;
        res = (*pUnlock)(lpdds, NULL);
        _if(res) OutTraceDW("%s ZBUFFER: Unlock err=%#x at %d\n", ApiRef, res, __LINE__);
        res = (*pLockMethod(dxversion))(lpdds, NULL, (LPDDSURFACEDESC)&ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WRITEONLY, 0);
        _if(res) OutTraceDW("%s ZBUFFER: Lock err=%#x at %d\n", ApiRef, res, __LINE__);
        memset(ddsd.lpSurface, 0xFF, ZBufferSize);
        res = (*pUnlock)(lpdds, lprect);
        _if(res) OutTraceDW("%s ZBUFFER: Unlock err=%#x at %d\n", ApiRef, res, __LINE__);
        return res;
    }
    switch(dxversion) {
    case 4:
        // v2.02.92: found in Fifa 2000: lpRect is completely ignored, receiving bogus values like (-1, -1, -1, -1}
        // or {0, 0, 0, 0}, or {-109119151, -109119151, -109119151, -109119151}.
        // better use the Lock-ed rect
        // v2.04.21: the fix is proper only when lprect is not NULL, when NULL better leave it unaltered.
        // Fixing the lprect value unconditionally breaks "Shadow Watch" text blitting on game menus.
        if(lprect) lprect = PopLockedRect(lpdds, lprect);
        break;
    case 7:
        if(lprect) {
            rect = *lprect;
            lprect = &rect;
            // v2.03.20: apparently, it seems that in ddraw 7 you can set an empty rectangle to mean the whole area ....
            // this fixes the black screen otherwise present in "Arcanum".
            if(IsPrim && ((lprect->right - lprect->left) == 0) && ((lprect->bottom - lprect->top) == 0)) lprect = NULL;
        }
        break;
    }
    if((dxw.dwFlags6 & FIXPITCH) && !(IsPrim || IsBack) && (lpdds == SaveSurface)) {
        OutTraceDW("DEBUG: fixing lpdds=%#x size=(%dx%d) surface=%#x pitch=%d bpp=%d\n",
                   lpdds, SaveSurfaceDesc.dwWidth, SaveSurfaceDesc.dwHeight, SaveSurfaceDesc.lpSurface, SaveSurfaceDesc.lPitch, SaveSurfaceDesc.ddpfPixelFormat.dwRGBBitCount);
        int y;
        LONG lVirtualPitch;
        LPVOID p1, p2;
        lVirtualPitch = (((SaveSurfaceDesc.dwWidth + 1) >> 1) * SaveSurfaceDesc.ddpfPixelFormat.dwRGBBitCount) >> 2;
        if(lVirtualPitch != SaveSurfaceDesc.lPitch) { // do just if necessary ...
            for (y = SaveSurfaceDesc.dwHeight - 1; y; y--) { // for each scan line ...
                p1 = (LPVOID)((DWORD)SaveSurfaceDesc.lpSurface + (y * lVirtualPitch));
                p2 = (LPVOID)((DWORD)SaveSurfaceDesc.lpSurface + (y * SaveSurfaceDesc.lPitch));
                memcpy(p2, p1, SaveSurfaceDesc.lPitch); // copy line to correct offset
            }
        }
    }
    if((dxw.dwFlags10 & SAFEPRIMLOCK) && IsPrim) {
        int size =
            SaveSurfaceDesc.dwHeight *
            SaveSurfaceDesc.dwWidth *
            (SaveSurfaceDesc.ddpfPixelFormat.dwRGBBitCount / 8);
        memcpy(SaveSurfaceDesc.lpSurface, SafeData, size);
        free(SafeData);
    }
    if(dxw.Renderer->flags & DXWRF_BUILDLOCKSURFACE) {
        if(dxwss.IsAPrimarySurface(lpdds) && lpDDSBuffer) {
            pBlt = pGetBltMethod(dxversion);
            RECT client = dxw.GetUnmappedScreenRect();
            res = (*pUnlock)((LPDIRECTDRAWSURFACE)lpDDSBuffer, lprect);
            _if(res) OutTraceE("%s: Unlock err %#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
            res = (*pBlt)(lpdds, &client, (LPDIRECTDRAWSURFACE)lpDDSBuffer, NULL, DDBLT_WAIT, 0);
            _if(res) OutTraceE("%s: Blt err %#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
            if(res == DDERR_INVALIDRECT) BlitError(res, NULL, lprect, __LINE__);
            res = (*pReleaseSMethod(dxversion))((LPDIRECTDRAWSURFACE)lpDDSBuffer);
            _if(res) OutTraceE("%s: Release err %#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
            lpDDSBuffer = NULL;
        }
    }
    if((dxw.dwDFlags & MARKLOCK) && !(IsPrim || IsBack) && (lpdds == SaveSurface)) {
        switch(SaveSurfaceDesc.ddpfPixelFormat.dwRGBBitCount) {
        case 16:
            for (UINT y = 0; y < SaveSurfaceDesc.dwHeight; y++) { // for each scan line ...
                USHORT *p;
                p = (USHORT *)((DWORD)SaveSurfaceDesc.lpSurface + (y * SaveSurfaceDesc.lPitch));
                p[0] = MARK16COLOR;
                p[SaveSurfaceDesc.dwWidth - 1] = MARK16COLOR;
                if((y == 0) || (y == (SaveSurfaceDesc.dwHeight - 1)))
                    for (UINT x = 0; x < SaveSurfaceDesc.dwWidth; x++) p[x] = MARK16COLOR;
            }
            break;
        case 32:
            for (UINT y = 0; y < SaveSurfaceDesc.dwHeight; y++) { // for each scan line ...
                ULONG *p;
                p = (ULONG *)((DWORD)SaveSurfaceDesc.lpSurface + (y * SaveSurfaceDesc.lPitch));
                p[0] = MARK32COLOR;
                p[SaveSurfaceDesc.dwWidth - 1] = MARK32COLOR;
                if((y == 0) || (y == (SaveSurfaceDesc.dwHeight - 1)))
                    for (UINT x = 0; x < SaveSurfaceDesc.dwWidth; x++) p[x] = MARK32COLOR;
            }
            break;
        }
    }
    res = (*pUnlock)(lpdds, lprect);
    if(res == DDERR_NOTLOCKED) res = DD_OK; // ignore not locked error
    if (IsPrim && res == DD_OK) {
        pBlt = pGetBltMethod(dxversion);
        if(dxversion < 4) lprect = NULL; // v2.03.60
        res = sBlt(dxversion, pBlt, ApiRef, lpdds, lprect, lpdds, lprect, NULL, 0, FALSE);
    }
    _if(res) OutTraceE("%s ERROR res=%#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
    if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    if((dxw.dwFlags5 & TEXTUREMASK) && (!IsPrim)) {
        // Texture Handling on Unlock
        TextureHandling(lpdds, dxversion);
    }
    OutDebugDW("%s OK\n", ApiRef);
#ifdef LOCKDIFFDUMP2
    if(!(IsBack | IsPrim)) SurfaceDiffDump(dxversion, lpdds);
#endif
#ifndef DXW_NOTRACES
    if((dxw.dwDFlags & DUMPSURFACES) && dxw.bCustomKeyToggle) SurfaceDump(lpdds, dxversion);
#endif // DXW_NOTRACES
    return res;
}

HRESULT WINAPI extUnlock1(LPDIRECTDRAWSURFACE lpdds, LPVOID lpvoid) {
    return extUnlock(1, (Unlock4_Type)pUnlock1, lpdds, (LPRECT)lpvoid);
}
HRESULT WINAPI extUnlock2(LPDIRECTDRAWSURFACE lpdds, LPVOID lpvoid) {
    return extUnlock(2, (Unlock4_Type)pUnlock2, lpdds, (LPRECT)lpvoid);
}
HRESULT WINAPI extUnlock3(LPDIRECTDRAWSURFACE lpdds, LPVOID lpvoid) {
    return extUnlock(3, (Unlock4_Type)pUnlock3, lpdds, (LPRECT)lpvoid);
}
HRESULT WINAPI extUnlock4(LPDIRECTDRAWSURFACE lpdds, LPRECT lprect) {
    return extUnlock(4, pUnlock4, lpdds, lprect);
}
HRESULT WINAPI extUnlock7(LPDIRECTDRAWSURFACE lpdds, LPRECT lprect) {
    return extUnlock(7, pUnlock7, lpdds, lprect);
}

/*  to do: instead of calling GDI GetDC, try to map GetDC with Lock and
    ReleaseDC with Unlock, returning the surface memory ptr (???) as HDC
    and avoiding the consistency check performed by surface::GetDC (why
    should it bother if the screen is 32BPP and the surface is not??? */

static HRESULT WINAPI extGetDC(int dxversion, GetDC_Type pGetDC, LPDIRECTDRAWSURFACE lpdds, HDC FAR *pHDC) {
    HRESULT res;
    BOOL IsPrim;
    static HDC LastDC = 0;
    IsPrim = dxwss.IsAPrimarySurface(lpdds);
    OutTraceDDRAW("GetDC: lpdss=%#x%s\n", lpdds, IsPrim ? "(PRIM)" : "");
    // In non-emulated mode, better return the window DC rather than the actual primary surfce DC,
    // because that would refer to the whole desktop
    if (!(dxw.IsEmulated) && IsPrim) {
        *pHDC = (*pGDIGetDC)(dxw.GethWnd());
        return DD_OK;
    }
    res = (*pGetDC)(lpdds, pHDC);
    // v2.05.35: save for possible later use in case of
    // DDERR_DCALREADYCREATED error.
    if(dxw.dwFlags12 & FIXDCALREADYCREATED) {
        if(res == DD_OK) LastDC = *pHDC;
        if(res == DDERR_DCALREADYCREATED) {
            OutTraceDW("GetDC: fixing DDERR_DCALREADYCREATED error\n");
            *pHDC = LastDC;
            res = DD_OK;
            OutTraceDDRAW("GetDC: res=%#x(%s) hdc=%#x\n", res, ExplainDDError(res), *pHDC);
            return res;
        }
    }
    if (res == DDERR_CANTCREATEDC &&
            (dxw.IsEmulated) &&
            dxw.VirtualPixelFormat.dwRGBBitCount == 8) {
        // for 8BPP palettized surfaces, connect them to either the ddraw emulated palette or the GDI emulated palette
        OutTraceDW("GetDC: adding 8BPP palette to surface lpdds=%#x\n", lpdds);
        if(lpDDP == NULL) {
            // should link here to the GDI palette? See Hyperblade....
            dxw.palNumEntries = 256;
            res = (*pCreatePaletteMethod(dxversion))(lpPrimaryDD, DDPCAPS_ALLOW256 | DDPCAPS_8BIT | DDPCAPS_INITIALIZE, dxw.palPalEntry, &lpDDP, NULL);
            if (res) {
                OutTraceE("GetDC: CreatePalette ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
                return res;
            }
        }
        res = (*pSetPaletteMethod(dxversion))(lpdds, lpDDP);
        if (res) {
            OutTraceE("GetDC: SetPalette ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
            return res;
        }
        iDDPExtraRefCounter = 1;
        // retry ....
        res = (*pGetDC)(lpdds, pHDC);
    }
    OutTraceDDRAW("GetDC: res=%#x(%s) hdc=%#x\n", res, ExplainDDError(res), *pHDC);
    return res;
}

HRESULT WINAPI extGetDC1(LPDIRECTDRAWSURFACE lpdds, HDC FAR *pHDC) {
    return extGetDC(1, pGetDC1, lpdds, pHDC);
}
HRESULT WINAPI extGetDC2(LPDIRECTDRAWSURFACE lpdds, HDC FAR *pHDC) {
    return extGetDC(2, pGetDC2, lpdds, pHDC);
}
HRESULT WINAPI extGetDC3(LPDIRECTDRAWSURFACE lpdds, HDC FAR *pHDC) {
    return extGetDC(3, pGetDC3, lpdds, pHDC);
}
HRESULT WINAPI extGetDC4(LPDIRECTDRAWSURFACE lpdds, HDC FAR *pHDC) {
    return extGetDC(4, pGetDC4, lpdds, pHDC);
}
HRESULT WINAPI extGetDC7(LPDIRECTDRAWSURFACE lpdds, HDC FAR *pHDC) {
    return extGetDC(7, pGetDC7, lpdds, pHDC);
}

#ifdef TRACEALL
HRESULT WINAPI extGetFlipStatus(int dxversion, GetFlipStatus_Type pGetFlipStatus, LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags) {
    HRESULT res;
    ApiName("GetFlipStatus");
    if(IsTraceDDRAW) {
        OutTraceDDRAW("%s(%d): flags=%#x(%s)\n",
                      ApiRef, dxversion,
                      dwFlags,
                      (dwFlags == DDGBS_CANBLT) ? "CANBLT" : (dwFlags == DDGBS_ISBLTDONE ? "ISBLTDONE" : "invalid"));
    }
    res = (*pGetFlipStatus)(lpdds, dwFlags);
    OutTraceDDRAW("%s(%d): ret=%#x(%s)\n", ApiRef, dxversion, res, ExplainDDError(res));
    // v2.05.23: deleted "GetFlipStatus" TRAP macro:
    // this ddraw method was found in "Age of Wonders Shadow Magic"
    return res;
}

HRESULT WINAPI extGetFlipStatus1(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags) {
    return extGetFlipStatus(1, pGetFlipStatus1, lpdds, dwFlags);
}
HRESULT WINAPI extGetFlipStatus2(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags) {
    return extGetFlipStatus(2, pGetFlipStatus2, lpdds, dwFlags);
}
HRESULT WINAPI extGetFlipStatus3(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags) {
    return extGetFlipStatus(3, pGetFlipStatus3, lpdds, dwFlags);
}
HRESULT WINAPI extGetFlipStatus4(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags) {
    return extGetFlipStatus(4, pGetFlipStatus4, lpdds, dwFlags);
}
HRESULT WINAPI extGetFlipStatus7(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags) {
    return extGetFlipStatus(7, pGetFlipStatus7, lpdds, dwFlags);
}
#endif

HRESULT WINAPI extReleaseDC(int dxversion, ReleaseDC_Type pReleaseDC, LPDIRECTDRAWSURFACE lpdds, HDC FAR hdc) {
    HRESULT res;
    ApiName("ReleaseDC");
    BOOL IsPrim;
    IsPrim = dxwss.IsAPrimarySurface(lpdds);
    OutTraceDDRAW("%s: lpdss=%#x%s hdc=%#x\n", ApiRef, lpdds, IsPrim ? "(PRIM)" : "", hdc);
    res = (*pReleaseDC)(lpdds, hdc);
    if((IsPrim) && (dxw.IsEmulated)) {
        Blt_Type pBlt;
        pBlt = pGetBltMethod(dxversion);
        sBlt(dxversion, pBlt, ApiRef, lpdds, NULL, lpdds, NULL, 0, NULL, FALSE);
    }
    _if(res) OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    return res;
}

HRESULT WINAPI extReleaseDC1(LPDIRECTDRAWSURFACE lpdds, HDC FAR hdc) {
    return extReleaseDC(1, pReleaseDC1, lpdds, hdc);
}
HRESULT WINAPI extReleaseDC2(LPDIRECTDRAWSURFACE lpdds, HDC FAR hdc) {
    return extReleaseDC(2, pReleaseDC2, lpdds, hdc);
}
HRESULT WINAPI extReleaseDC3(LPDIRECTDRAWSURFACE lpdds, HDC FAR hdc) {
    return extReleaseDC(3, pReleaseDC3, lpdds, hdc);
}
HRESULT WINAPI extReleaseDC4(LPDIRECTDRAWSURFACE lpdds, HDC FAR hdc) {
    return extReleaseDC(4, pReleaseDC4, lpdds, hdc);
}
HRESULT WINAPI extReleaseDC7(LPDIRECTDRAWSURFACE lpdds, HDC FAR hdc) {
    return extReleaseDC(7, pReleaseDC7, lpdds, hdc);
}


HRESULT WINAPI extFlipToGDISurface(FlipToGDISurface_Type pFlipToGDISurface, LPDIRECTDRAW lpdd) {
    // HRESULT res;
    OutTraceDDRAW("FlipToGDISurface: lpdd=%#x\n", lpdd);
    // to revise: so far, it seems the best thing to do is NOTHING, just return 0.
    // res=(*pFlipToGDISurface)(lpdd);
    // _if(res) OutTraceE("FlipToGDISurface: ERROR res=%#x(%s), skipping\n", res, ExplainDDError(res));
    // pretend you flipped anyway....
    if(dxw.dwFlags6 & SHAREDDC) bFlippedDC = TRUE;
    return DD_OK;
}

HRESULT WINAPI extFlipToGDISurface1(LPDIRECTDRAW lpdd) {
    return extFlipToGDISurface(pFlipToGDISurface1, lpdd);
}
HRESULT WINAPI extFlipToGDISurface2(LPDIRECTDRAW lpdd) {
    return extFlipToGDISurface(pFlipToGDISurface2, lpdd);
}
HRESULT WINAPI extFlipToGDISurface3(LPDIRECTDRAW lpdd) {
    return extFlipToGDISurface(pFlipToGDISurface3, lpdd);
}
HRESULT WINAPI extFlipToGDISurface4(LPDIRECTDRAW lpdd) {
    return extFlipToGDISurface(pFlipToGDISurface4, lpdd);
}
HRESULT WINAPI extFlipToGDISurface7(LPDIRECTDRAW lpdd) {
    return extFlipToGDISurface(pFlipToGDISurface7, lpdd);
}

static HRESULT WINAPI extGetFourCCCodes(int dxversion, GetFourCCCodes_Type pGetFourCCCodes, LPDIRECTDRAW lpdd, LPDWORD lpNumCodes, LPDWORD lpCodes) {
    HRESULT res;
    ApiName("GetFourCCCodes");
    OutTraceDDRAW("%s(%d): lpdd=%#x\n", ApiRef, dxversion, lpdd);
    if(dxw.dwFlags10 & SUPPRESSFOURCCBLT) {
        OutTraceDW("%s: SUPPRESSFOURCCBLT\n", ApiRef);
        *lpNumCodes = 0;
        return DD_OK;
    }
    res = (*pGetFourCCCodes)(lpdd, lpNumCodes, lpCodes);
    if(res)
        OutTraceE("%s ERROR: ret=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else {
        OutTraceDDRAW("%s: numcodes=%d\n", ApiRef, *lpNumCodes);
        if(lpCodes) for(DWORD i = 0; i < *lpNumCodes; i++) {
                DWORD fcc = lpCodes[i];
                char *c = (char *)&fcc;
                OutTraceDDRAW("> fourcc[%d]=%#x(%s)\n", i, fcc, sFourCC(fcc));
            }
    }
    return res;
}

HRESULT WINAPI extGetFourCCCodes1(LPDIRECTDRAW lpdd, LPDWORD lpNumCodes, LPDWORD lpCodes) {
    return extGetFourCCCodes(1, pGetFourCCCodes1, lpdd, lpNumCodes, lpCodes);
}
HRESULT WINAPI extGetFourCCCodes2(LPDIRECTDRAW lpdd, LPDWORD lpNumCodes, LPDWORD lpCodes) {
    return extGetFourCCCodes(2, pGetFourCCCodes2, lpdd, lpNumCodes, lpCodes);
}
HRESULT WINAPI extGetFourCCCodes3(LPDIRECTDRAW lpdd, LPDWORD lpNumCodes, LPDWORD lpCodes) {
    return extGetFourCCCodes(3, pGetFourCCCodes3, lpdd, lpNumCodes, lpCodes);
}
HRESULT WINAPI extGetFourCCCodes4(LPDIRECTDRAW lpdd, LPDWORD lpNumCodes, LPDWORD lpCodes) {
    return extGetFourCCCodes(4, pGetFourCCCodes4, lpdd, lpNumCodes, lpCodes);
}
HRESULT WINAPI extGetFourCCCodes7(LPDIRECTDRAW lpdd, LPDWORD lpNumCodes, LPDWORD lpCodes) {
    return extGetFourCCCodes(7, pGetFourCCCodes7, lpdd, lpNumCodes, lpCodes);
}

HRESULT WINAPI extGetGDISurface(int dxversion, GetGDISurface_Type pGetGDISurface, LPDIRECTDRAW lpdd, LPDIRECTDRAWSURFACE *w) {
    HRESULT res;
    ApiName("GetGDISurface");
    OutTraceDDRAW("%s(%d): lpdd=%#x\n", ApiRef, dxversion, lpdd);
    // v2.02.31:
    // in EMULATED mode, should not return the actual ddraw primary surface, but the virtual one.
    if(dxw.IsEmulated) {
        LPDIRECTDRAWSURFACE lpGDI;
        lpGDI = dxwss.GetPrimarySurface();
        OutTraceDW("%s: EMULATED lpdd=%#x w=%#x\n", ApiRef, lpdd, lpGDI);
        if(lpGDI) {
            lpGDI->AddRef();
            *w = lpGDI;
            return DD_OK;
        } else
            return DDERR_NOTFOUND;
    }
    res = (*pGetGDISurface)(lpdd, w);
    if (res)
        OutTraceE("%s: ERROR lpdd=%#x res=%#x(%s)\n", ApiRef, lpdd, res, ExplainDDError(res));
    else
        OutTraceDDRAW("%s: lpdd=%#x w=%#x\n", ApiRef, lpdd, *w);
    return res;
}

HRESULT WINAPI extGetGDISurface1(LPDIRECTDRAW lpdd, LPDIRECTDRAWSURFACE *w) {
    return extGetGDISurface(1, pGetGDISurface1, lpdd, w);
}
HRESULT WINAPI extGetGDISurface2(LPDIRECTDRAW lpdd, LPDIRECTDRAWSURFACE *w) {
    return extGetGDISurface(2, pGetGDISurface2, lpdd, w);
}
HRESULT WINAPI extGetGDISurface3(LPDIRECTDRAW lpdd, LPDIRECTDRAWSURFACE *w) {
    return extGetGDISurface(3, pGetGDISurface3, lpdd, w);
}
HRESULT WINAPI extGetGDISurface4(LPDIRECTDRAW lpdd, LPDIRECTDRAWSURFACE *w) {
    return extGetGDISurface(4, pGetGDISurface4, lpdd, w);
}
HRESULT WINAPI extGetGDISurface7(LPDIRECTDRAW lpdd, LPDIRECTDRAWSURFACE *w) {
    return extGetGDISurface(7, pGetGDISurface7, lpdd, w);
}

// debug function to dump all video modes queried by the DirectDrav::EnumDisplayModes method

HRESULT WINAPI EnumModesCallbackDumper(LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext) {
    OutTrace("EnumModesCallback:\n");
    OutTrace("\tdwSize=%d\n", lpDDSurfaceDesc->dwSize);
    OutTrace("\tddpfPixelFormat depth=%d rgba=(%X-%X-%X-%X)\n",
             lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount,
             lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask,
             lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask,
             lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask,
             lpDDSurfaceDesc->ddpfPixelFormat.dwRGBAlphaBitMask);
    OutTrace("\tdwFlags=%#x(%s)\n", lpDDSurfaceDesc->dwFlags, ExplainFlags(lpDDSurfaceDesc->dwFlags));
    OutTrace("\tdwWidth x dwHeight=(%d,%d)\n", lpDDSurfaceDesc->dwWidth, lpDDSurfaceDesc->dwHeight);
    OutTrace("\tlPitch=%d\n", lpDDSurfaceDesc->lPitch);
    //OutTrace("\tdwBackBufferCount=%d\n", lpDDSurfaceDesc->dwBackBufferCount);
    OutTrace("\tdwRefreshRate=%d\n", lpDDSurfaceDesc->dwRefreshRate);
    //OutTrace("\tlpSurface=%#x\n", lpDDSurfaceDesc->lpSurface);
    //OutTrace("\tCaps=%#x(%s)\n", lpDDSurfaceDesc->ddsCaps.dwCaps, ExplainDDSCaps(lpDDSurfaceDesc->ddsCaps.dwCaps));
    //if ((NewContext_Type *)lpContext->dxversion >= 4) {
    //	OutTrace("\tddpfPixelFormat %s\n", DumpPixelFormat((LPDDSURFACEDESC2)lpDDSurfaceDesc));
    //}
#ifdef FULLHEXDUMP
    OutTrace("DDSurfaceDesc=");
    OutTraceHex((BYTE *)lpDDSurfaceDesc, sizeof(DDSURFACEDESC));
#endif
    return DDENUMRET_OK;
}

typedef HRESULT (WINAPI *EnumModesCallback_Type)(LPDDSURFACEDESC, LPVOID);
typedef struct {
    LPVOID lpContext;
    EnumModesCallback_Type lpCallback;
    DWORD dwWidth;
    DWORD dwHeight;
    int dxversion;
    BOOL bFirst;
} NewContext_Type;

static BOOL CheckResolutionLimit(LPDDSURFACEDESC lpDDSurfaceDesc) {
    if(dxw.dwFlags4 & LIMITSCREENRES) {
#define HUGE 100000
        DWORD maxw, maxh;
        maxw = HUGE;
        maxh = HUGE;
        switch(dxw.MaxScreenRes) {
        case DXW_LIMIT_320x200:
            maxw = 320;
            maxh = 200;
            break;
        case DXW_LIMIT_400x300:
            maxw = 400;
            maxh = 300;
            break; // v2.04.78 fix
        case DXW_LIMIT_640x480:
            maxw = 640;
            maxh = 480;
            break;
        case DXW_LIMIT_800x600:
            maxw = 800;
            maxh = 600;
            break;
        case DXW_LIMIT_1024x768:
            maxw = 1024;
            maxh = 768;
            break;
        case DXW_LIMIT_1280x960:
            maxw = 1280;
            maxh = 960;
            break;
        }
        if((lpDDSurfaceDesc->dwWidth > maxw) || (lpDDSurfaceDesc->dwHeight > maxh)) {
            OutTraceDW("EnumDisplaySettings: hide device mode=(%d,%d)\n", maxw, maxh);
            return TRUE;
        }
    }
    if(dxw.dwFlags7 & MAXIMUMRES) {
        if((lpDDSurfaceDesc->dwWidth > (DWORD)dxw.iMaxW) || (lpDDSurfaceDesc->dwHeight > (DWORD)dxw.iMaxH)) {
            OutTraceDW("EnumDisplaySettings: hide device mode=(%d,%d)\n", dxw.iMaxW, dxw.iMaxH);
            return TRUE;
        }
    }
    return FALSE;
}

HRESULT WINAPI myEnumModesFilterDirect(LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext) {
    HRESULT res;
    // trick to perform the call just once per each color depth ....
    if(((NewContext_Type *)lpContext)->bFirst) {
        ((NewContext_Type *)lpContext)->bFirst = FALSE;
        ((NewContext_Type *)lpContext)->dwHeight = lpDDSurfaceDesc->dwHeight;
        ((NewContext_Type *)lpContext)->dwWidth = lpDDSurfaceDesc->dwWidth;
    }
    if ((((NewContext_Type *)lpContext)->dwHeight != lpDDSurfaceDesc->dwHeight) ||
            (((NewContext_Type *)lpContext)->dwWidth != lpDDSurfaceDesc->dwWidth)) {
        // skip other resolutions ...
        return DDENUMRET_OK;
    }
    if (IsDebugDW) EnumModesCallbackDumper(lpDDSurfaceDesc, NULL);
    // tricky part: for each color depth related to current video resolution, fake each of the
    // supported resolutions
    for (int ResIdx = 0; SupportedRes[ResIdx].h; ResIdx++) {
        lpDDSurfaceDesc->dwHeight = SupportedRes[ResIdx].h;
        lpDDSurfaceDesc->dwWidth = SupportedRes[ResIdx].w;
        res = (*((NewContext_Type *)lpContext)->lpCallback)(lpDDSurfaceDesc, ((NewContext_Type *)lpContext)->lpContext);
        OutTraceDW("EnumDisplayModes(D): proposed size[%d]=(%d,%d) res=%#x\n", ResIdx, SupportedRes[ResIdx].w, SupportedRes[ResIdx].h, res);
        if(res == DDENUMRET_CANCEL) break;
    }
    return res;
}

HRESULT WINAPI myEnumModesFilterNative(LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext) {
    HRESULT res;
    if (IsDebugDW) EnumModesCallbackDumper(lpDDSurfaceDesc, NULL);
    if (dxw.dwFlags1 & PREVENTMAXIMIZE) {
        // if PREVENTMAXIMIZE is set, don't let the caller know about forbidden screen settings.
        if((lpDDSurfaceDesc->dwHeight > dxw.GetScreenHeight()) ||
                (lpDDSurfaceDesc->dwWidth > dxw.GetScreenWidth())) {
            OutTraceDW("EnumDisplayModes: skipping screen size=(%d,%d)\n", lpDDSurfaceDesc->dwHeight, lpDDSurfaceDesc->dwWidth);
            return DDENUMRET_OK;
        }
    }
    if(CheckResolutionLimit(lpDDSurfaceDesc)) return DDENUMRET_OK;
    res = (*((NewContext_Type *)lpContext)->lpCallback)(lpDDSurfaceDesc, ((NewContext_Type *)lpContext)->lpContext);
    OutTraceDW("EnumDisplayModes(D): native size=(%d,%d) bpp=%d res=%#x\n", lpDDSurfaceDesc->dwWidth, lpDDSurfaceDesc->dwHeight, lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount, res);
    return res;
}

HRESULT WINAPI extEnumDisplayModes(int dxversion, EnumDisplayModes1_Type pEnumDisplayModes, LPDIRECTDRAW lpdd, DWORD dwflags, LPDDSURFACEDESC lpddsd, LPVOID lpContext, LPDDENUMMODESCALLBACK cb) {
    HRESULT res;
    ApiName("EnumDisplayModes");
    NewContext_Type NewContext;
    OutTraceDDRAW("%s(D%d): lpdd=%#x flags=%#x lpddsd=%#x callback=%#x\n", ApiRef, dxversion, lpdd, dwflags, lpddsd, cb);
#ifndef DXW_NOTRACES
    if(lpddsd) OutTraceDDRAW("%s(D): %s\n", ApiRef, LogSurfaceAttributes((LPDDSURFACEDESC2)lpddsd, ApiRef, __LINE__));
#endif // DXW_NOTRACES
    if ((dxw.dwFlags4 & NATIVERES) ||
            (!(dxw.IsEmulated) && !dxw.Windowize)) { // v2.04.14 - make nonemulated nonwindowed mode work ...
        NewContext.dwWidth = 0;
        NewContext.dwHeight = 0;
        NewContext.lpContext = lpContext;
        NewContext.lpCallback = cb;
        NewContext.dxversion = dxversion;
        res = (*pEnumDisplayModes)(lpdd, dwflags, lpddsd, &NewContext, myEnumModesFilterNative);
        _if(res) OutTraceE("%s(D): ERROR res=%#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
        return res;
    }
    // note: extEnumDisplayModes serves both the EnumDisplayModes and EnumDisplayModes2 interfaces:
    // they differ for the lpddsd argument that should point to either DDSURFACEDESC or DDSURFACEDESC2
    // structures, but unification is possible if the lpddsd->dwSize is properly set and is left untouched.
    if((dxw.IsEmulated)) {
        int ResIdx, DepthIdx;
        DDSURFACEDESC2 EmuDesc;
        DWORD dwSize;
        EmuDesc.dwRefreshRate = 0;
        EmuDesc.ddpfPixelFormat.dwFlags = DDPF_RGB;
        if (lpddsd) dwSize = lpddsd->dwSize; // sizeof either DDSURFACEDESC or DDSURFACEDESC2 !!!
        else dwSize = (dxversion < 4) ? sizeof(DDSURFACEDESC) : sizeof(DDSURFACEDESC2);
        memset(&EmuDesc, 0, dwSize);
        EmuDesc.dwSize = dwSize;
        EmuDesc.dwFlags = DDSD_PIXELFORMAT | DDSD_REFRESHRATE | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH;
        res = DD_OK;
        for (ResIdx = 0; SupportedRes[ResIdx].h; ResIdx++) {
            EmuDesc.dwHeight = SupportedRes[ResIdx].h;
            EmuDesc.dwWidth = SupportedRes[ResIdx].w;
            if(CheckResolutionLimit((LPDDSURFACEDESC)&EmuDesc)) break;
            EmuDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
            EmuDesc.ddpfPixelFormat.dwFlags = DDPF_RGB;
            for (DepthIdx = 0; SupportedDepths[DepthIdx]; DepthIdx++) {
                // v2.03.24: if lpddsd is not null, compatible color depth must be selected. Skip incompatible ones.
                // Fixes "Total Annihilation Kingdoms" crash
                if(lpddsd && (lpddsd->dwFlags & DDSD_PIXELFORMAT) && (lpddsd->ddpfPixelFormat.dwRGBBitCount != SupportedDepths[DepthIdx])) continue;
                EmuDesc.ddpfPixelFormat.dwRGBBitCount = SupportedDepths[DepthIdx];
                EmuDesc.lPitch = SupportedRes[ResIdx].w * SupportedDepths[DepthIdx] / 8;
                FixPixelFormat(EmuDesc.ddpfPixelFormat.dwRGBBitCount, &(EmuDesc.ddpfPixelFormat));
                if(IsDebugDW) EnumModesCallbackDumper((LPDDSURFACEDESC)&EmuDesc, lpContext);
                res = (*cb)((LPDDSURFACEDESC)&EmuDesc, lpContext);
                if(res == DDENUMRET_CANCEL) break;
            }
            if(res == DDENUMRET_CANCEL) break;
        }
        res = DD_OK;
    } else {
        NewContext_Type NewContext;
        NewContext.lpContext = lpContext;
        NewContext.lpCallback = cb;
        NewContext.bFirst = TRUE;
        res = (*pEnumDisplayModes)(lpdd, dwflags, lpddsd, &NewContext, myEnumModesFilterDirect);
    }
    _if(res) OutTraceE("%s(D): ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extEnumDisplayModes1(LPDIRECTDRAW lpdd, DWORD dwflags, LPDDSURFACEDESC lpddsd, LPVOID lpContext, LPDDENUMMODESCALLBACK cb) {
    return extEnumDisplayModes(1, pEnumDisplayModes1, lpdd, dwflags, lpddsd, lpContext, cb);
}
HRESULT WINAPI extEnumDisplayModes2(LPDIRECTDRAW lpdd, DWORD dwflags, LPDDSURFACEDESC lpddsd, LPVOID lpContext, LPDDENUMMODESCALLBACK cb) {
    return extEnumDisplayModes(2, pEnumDisplayModes2, lpdd, dwflags, lpddsd, lpContext, cb);
}
HRESULT WINAPI extEnumDisplayModes3(LPDIRECTDRAW lpdd, DWORD dwflags, LPDDSURFACEDESC lpddsd, LPVOID lpContext, LPDDENUMMODESCALLBACK cb) {
    return extEnumDisplayModes(3, pEnumDisplayModes3, lpdd, dwflags, lpddsd, lpContext, cb);
}
HRESULT WINAPI extEnumDisplayModes4(LPDIRECTDRAW lpdd, DWORD dwflags, LPDDSURFACEDESC2 lpddsd, LPVOID lpContext, LPDDENUMMODESCALLBACK2 cb) {
    return extEnumDisplayModes(4, (EnumDisplayModes1_Type)pEnumDisplayModes4, lpdd, dwflags, (LPDDSURFACEDESC)lpddsd, lpContext, (LPDDENUMMODESCALLBACK)cb);
}
HRESULT WINAPI extEnumDisplayModes7(LPDIRECTDRAW lpdd, DWORD dwflags, LPDDSURFACEDESC2 lpddsd, LPVOID lpContext, LPDDENUMMODESCALLBACK2 cb) {
    return extEnumDisplayModes(7, (EnumDisplayModes1_Type)pEnumDisplayModes7, lpdd, dwflags, (LPDDSURFACEDESC)lpddsd, lpContext, (LPDDENUMMODESCALLBACK)cb);
}

HRESULT WINAPI extGetPixelFormat(GetPixelFormat_Type pGetPixelFormat, LPDIRECTDRAWSURFACE lpdds, LPDDPIXELFORMAT p) {
    HRESULT res;
    ApiName("GetPixelFormat");
    BOOL IsPrim;
    IsPrim = dxwss.IsAPrimarySurface(lpdds);
    OutTraceDDRAW("%s: lpdds=%#x%s\n", ApiRef, lpdds, IsPrim ? "(PRIM)" : "");
    res = (*pGetPixelFormat)(lpdds, p);
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else {
        OutTraceDDRAW("%s: Flags=%#x(%s) FourCC=%#x BitCount=%d RGBA=(%#x,%#x,%#x,%#x)\n",
                      ApiRef, p->dwFlags, ExplainPixelFormatFlags(p->dwFlags), p->dwFourCC, p->dwRGBBitCount,
                      p->dwRBitMask, p->dwGBitMask, p->dwBBitMask, p->dwRGBAlphaBitMask );
    }
    // fix: virtual pixel definition is helpful not only on primary surfaces, but more in general
    // on every surface that returns an error. It fixes "Arx Fatalis" crash.
    if ((dxw.IsEmulated) && res) {
        p->dwFlags = dxw.VirtualPixelFormat.dwFlags;
        p->dwRGBBitCount = dxw.VirtualPixelFormat.dwRGBBitCount;
        p->dwRBitMask = dxw.VirtualPixelFormat.dwRBitMask;
        p->dwGBitMask = dxw.VirtualPixelFormat.dwGBitMask;
        p->dwBBitMask = dxw.VirtualPixelFormat.dwBBitMask;
        p->dwRGBAlphaBitMask = dxw.VirtualPixelFormat.dwRGBAlphaBitMask;
        OutTraceDW("%s: EMULATED BitCount=%d RGBA=(%#x,%#x,%#x,%#x)\n",
                   ApiRef, p->dwRGBBitCount, p->dwRBitMask, p->dwGBitMask, p->dwBBitMask, p->dwRGBAlphaBitMask );
        res = DD_OK;
    }
    return res;
}

HRESULT WINAPI extGetPixelFormat1(LPDIRECTDRAWSURFACE lpdds, LPDDPIXELFORMAT p) {
    return extGetPixelFormat(pGetPixelFormat1, lpdds, p);
}
HRESULT WINAPI extGetPixelFormat2(LPDIRECTDRAWSURFACE lpdds, LPDDPIXELFORMAT p) {
    return extGetPixelFormat(pGetPixelFormat2, lpdds, p);
}
HRESULT WINAPI extGetPixelFormat3(LPDIRECTDRAWSURFACE lpdds, LPDDPIXELFORMAT p) {
    return extGetPixelFormat(pGetPixelFormat3, lpdds, p);
}
HRESULT WINAPI extGetPixelFormat4(LPDIRECTDRAWSURFACE lpdds, LPDDPIXELFORMAT p) {
    return extGetPixelFormat(pGetPixelFormat4, lpdds, p);
}
HRESULT WINAPI extGetPixelFormat7(LPDIRECTDRAWSURFACE lpdds, LPDDPIXELFORMAT p) {
    return extGetPixelFormat(pGetPixelFormat7, lpdds, p);
}

HRESULT WINAPI extTestCooperativeLevel(TestCooperativeLevel_Type pTestCooperativeLevel, LPDIRECTDRAW lpdd) {
    HRESULT res;
    ApiName("TestCooperativeLevel");
    res = (*pTestCooperativeLevel)(lpdd);
    OutDebugDW("%s: lpdd=%#x res=%#x(%s)\n", ApiRef, lpdd, res, ExplainDDError(res));
    if(res == DDERR_WRONGMODE) {
        res = ((LPDIRECTDRAW7)lpdd)->RestoreAllSurfaces();
        _if(res) OutTraceE("%s: RestoreAllSurfaces ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    }
    if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    return res;
}

HRESULT WINAPI extTestCooperativeLevel4(LPDIRECTDRAW lpdd) {
    return extTestCooperativeLevel(pTestCooperativeLevel4, lpdd);
}
HRESULT WINAPI extTestCooperativeLevel7(LPDIRECTDRAW lpdd) {
    return extTestCooperativeLevel(pTestCooperativeLevel7, lpdd);
}

HRESULT WINAPI extReleaseS(int dxversion, ReleaseS_Type pReleaseS, LPDIRECTDRAWSURFACE lpdds) {
    HRESULT res;
    ApiName("Release");
    BOOL IsPrim;
    BOOL IsBack;
    IsPrim = dxwss.IsAPrimarySurface(lpdds);
    IsBack = dxwss.IsABackBufferSurface(lpdds);
    OutTraceDDRAW("%s(S%d): lpdds=%#x%s\n", ApiRef, dxversion, lpdds, dxwss.ExplainSurfaceRole(lpdds));
    // fix for "Tetris World" .... ???
    // fix for "Team 47 GoMan"
    // fix for "Drift: When Worlds Collide", "Midnight GT Primary Racer", "Team 47 Goman"
    if (IsBack && (dxw.dwFlags6 & SUPPRESSRELEASE)) {
        OutTraceDDRAW("%s(S): SUPPRESS lpdds=%#x ref=0\n", ApiRef, lpdds);
        dxw.lpZombieSurface = lpdds; // v2.05.48: register lpZombieSurface for late release in QueryInterface
        return 0;
    }
    __try { // try/except useful when using proxy dll
        res = (*pReleaseS)(lpdds);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        OutTraceE("%s(S): EXCEPTION lpdds=%#x%s res=0\n", ApiRef, lpdds, dxwss.ExplainSurfaceRole(lpdds));
        res = 0;
    }
    OutTraceDDRAW("%s(S): lpdds=%#x%s ref=%d\n", ApiRef, lpdds, dxwss.ExplainSurfaceRole(lpdds), res);
    if (res == 0) { // common precondition
        // erase surface from primary or backbuffer list
        // v2-03-20: d3d 1-7 play a dirty trick: it Release the surface passed as argument until refcount == 0,
        // but then rebuilds it with the original attributes! So, better not forget them.
        if(IsPrim || IsBack) dxwss.UnrefSurface(lpdds);
        // when releasing primary surface, erase clipping region
        // v2.04.14: commented out, better not to! The game could close and reopen a new primary surface.
        // fixes "Settlers III" clipping problems
        // if(IsPrim && (dxw.dwFlags1 & CLIPCURSOR)) dxw.EraseClipCursor();
        // clear service surface pointers
        if (dxw.IsEmulated) {
            if(lpdds == lpDDSEmu_Prim) {
                OutTraceDW("%s(S): Clearing lpDDSEmu_Prim pointer\n", ApiRef);
                lpDDSEmu_Prim = NULL;
            }
            if(lpdds == lpDDSEmu_Back) {
                OutTraceDW("%s(S): Clearing lpDDSEmu_Back pointer\n", ApiRef);
                lpDDSEmu_Back = NULL;
            }
        }
    }
    if(dxw.dwFlags4 & RETURNNULLREF) return 0;
    return res;
}

HRESULT WINAPI extReleaseS1(LPDIRECTDRAWSURFACE lpdds) {
    return extReleaseS(1, pReleaseS1, lpdds);
}
HRESULT WINAPI extReleaseS2(LPDIRECTDRAWSURFACE lpdds) {
    return extReleaseS(2, pReleaseS2, lpdds);
}
HRESULT WINAPI extReleaseS3(LPDIRECTDRAWSURFACE lpdds) {
    return extReleaseS(3, pReleaseS3, lpdds);
}
HRESULT WINAPI extReleaseS4(LPDIRECTDRAWSURFACE lpdds) {
    return extReleaseS(4, pReleaseS4, lpdds);
}
HRESULT WINAPI extReleaseS7(LPDIRECTDRAWSURFACE lpdds) {
    return extReleaseS(7, pReleaseS7, lpdds);
}

static HRESULT WINAPI extSetColorKey(SetColorKey_Type pSetColorKey, LPDIRECTDRAWSURFACE lpdds, DWORD flags, LPDDCOLORKEY lpDDColorKey) {
    HRESULT res;
    ApiName("SetColorKey");
    BOOL IsPrim;
    IsPrim = dxwss.IsAPrimarySurface(lpdds);
#ifndef DXW_NOTRACES
    if(IsTraceDDRAW) {
        char sInfo[81];
        if (lpDDColorKey) sprintf(sInfo, "(L:%#x,H:%#x)", lpDDColorKey->dwColorSpaceLowValue, lpDDColorKey->dwColorSpaceHighValue);
        else strcpy(sInfo, "(NULL)");
        OutTrace("%s: lpdds=%#x%s flags=%#x(%s) colors=%s\n",
                 ApiRef, lpdds, (IsPrim ? "(PRIM)" : ""), flags, ExplainColorKeyFlag(flags), sInfo);
    }
#endif
    res = (*pSetColorKey)(lpdds, flags, lpDDColorKey);
    _if(res) OutTraceE("%s: ERROR flags=%#x lpdds=%#x res=%#x(%s)\n",
                       ApiRef, flags, lpdds, res, ExplainDDError(res));
    if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    return res;
}

HRESULT WINAPI extSetColorKey1(LPDIRECTDRAWSURFACE lpdds, DWORD flags, LPDDCOLORKEY lpDDColorKey) {
    return extSetColorKey(pSetColorKey1, lpdds, flags, lpDDColorKey);
}
HRESULT WINAPI extSetColorKey2(LPDIRECTDRAWSURFACE lpdds, DWORD flags, LPDDCOLORKEY lpDDColorKey) {
    return extSetColorKey(pSetColorKey2, lpdds, flags, lpDDColorKey);
}
HRESULT WINAPI extSetColorKey3(LPDIRECTDRAWSURFACE lpdds, DWORD flags, LPDDCOLORKEY lpDDColorKey) {
    return extSetColorKey(pSetColorKey3, lpdds, flags, lpDDColorKey);
}
HRESULT WINAPI extSetColorKey4(LPDIRECTDRAWSURFACE lpdds, DWORD flags, LPDDCOLORKEY lpDDColorKey) {
    return extSetColorKey(pSetColorKey4, lpdds, flags, lpDDColorKey);
}
HRESULT WINAPI extSetColorKey7(LPDIRECTDRAWSURFACE lpdds, DWORD flags, LPDDCOLORKEY lpDDColorKey) {
    return extSetColorKey(pSetColorKey7, lpdds, flags, lpDDColorKey);
}

static HRESULT WINAPI extGetColorKey(int dxversion, GetColorKey_Type pGetColorKey, LPDIRECTDRAWSURFACE lpdds, DWORD flags, LPDDCOLORKEY lpDDColorKey) {
    HRESULT res;
    ApiName("GetColorKey");
    BOOL IsPrim;
    IsPrim = dxwss.IsAPrimarySurface(lpdds);
    OutTraceDDRAW("%s(%d): lpdds=%#x%s flags=%#x(%s)\n",
                  ApiRef, dxversion, lpdds, (IsPrim ? "(PRIM)" : ""), flags, ExplainColorKeyFlag(flags));
    res = (*pGetColorKey)(lpdds, flags, lpDDColorKey);
    if(res)
        // non severe error, log al lower priority
        OutTraceDDRAW("%s: ERROR lpdds=%#x flags=%#x res=%#x(%s)\n", ApiRef, lpdds, flags, res, ExplainDDError(res));
    else
        OutTraceDDRAW("%s: colors=(L:%#x,H:%#x)\n",
                      ApiRef, lpDDColorKey->dwColorSpaceLowValue, lpDDColorKey->dwColorSpaceHighValue);
    if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    return res;
}

HRESULT WINAPI extGetColorKey1(LPDIRECTDRAWSURFACE lpdds, DWORD flags, LPDDCOLORKEY lpDDColorKey) {
    return extGetColorKey(1, pGetColorKey1, lpdds, flags, lpDDColorKey);
}
HRESULT WINAPI extGetColorKey2(LPDIRECTDRAWSURFACE lpdds, DWORD flags, LPDDCOLORKEY lpDDColorKey) {
    return extGetColorKey(2, pGetColorKey2, lpdds, flags, lpDDColorKey);
}
HRESULT WINAPI extGetColorKey3(LPDIRECTDRAWSURFACE lpdds, DWORD flags, LPDDCOLORKEY lpDDColorKey) {
    return extGetColorKey(3, pGetColorKey3, lpdds, flags, lpDDColorKey);
}
HRESULT WINAPI extGetColorKey4(LPDIRECTDRAWSURFACE lpdds, DWORD flags, LPDDCOLORKEY lpDDColorKey) {
    return extGetColorKey(4, pGetColorKey4, lpdds, flags, lpDDColorKey);
}
HRESULT WINAPI extGetColorKey7(LPDIRECTDRAWSURFACE lpdds, DWORD flags, LPDDCOLORKEY lpDDColorKey) {
    return extGetColorKey(7, pGetColorKey7, lpdds, flags, lpDDColorKey);
}

static HRESULT WINAPI extEnumAttachedSurfaces(int dxversion, EnumAttachedSurfaces_Type pEnumAttachedSurfaces, LPDIRECTDRAWSURFACE lpdds, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback) {
    HRESULT res;
    ApiName("EnumAttachedSurfaces");
    BOOL IsPrim, IsBack;
    IsPrim = dxwss.IsAPrimarySurface(lpdds);
    IsBack = dxwss.IsABackBufferSurface(lpdds);
    OutTraceDDRAW("%s(%d): lpdds=%#x%s Context=%#x Callback=%#x\n",
                  ApiRef, dxversion, lpdds, (IsPrim ? "(PRIM)" : ""), lpContext, lpEnumSurfacesCallback);
    if (IsPrim) {
        LPDIRECTDRAWSURFACE lpDDSBack;
        // A Primary surface has not backbuffer attached surfaces actually,
        // so don't rely on ddraw and call the callback function directly.
        // Needed to make Nox working.
        DDSURFACEDESC2 ddsd;
        // first, call hooked function
        res = (*pEnumAttachedSurfaces)(lpdds, lpContext, lpEnumSurfacesCallback);
        _if(res) OutTraceE("%s: ERROR %#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
        if(lpDDSBack = dxwss.GetBackBufferSurface()) {
            ddsd.dwSize = Set_dwSize_From_Surface();
            res = lpDDSBack->GetSurfaceDesc((LPDDSURFACEDESC)&ddsd);
            if(res) {
                OutTraceE("%s: GetSurfaceDesc ERROR %#x(%s)\n",
                          ApiRef, res, ExplainDDError(res));
                return res;
            }
            res = (lpEnumSurfacesCallback)(lpDDSBack, (LPDDSURFACEDESC)&ddsd, lpContext);
            OutTraceDW("%s: Callback on DDSBack=%#x res=%#x(%s)\n",
                       ApiRef, lpDDSBack, res, res ? "DDENUMRET_OK" : "DDENUMRET_CANCEL");
        }
        return DD_OK; // for Black Dahlia
    }
    if(IsBack) {
        LPDIRECTDRAWSURFACE lpDDSPrim;
        // A Primary surface has not backbuffer attached surfaces actually,
        // so don't rely on ddraw and call the callback function directly.
        // Needed to make Nox working.
        DDSURFACEDESC2 ddsd;
        // first, call hooked function
        res = (*pEnumAttachedSurfaces)(lpdds, lpContext, lpEnumSurfacesCallback);
        _if(res) OutTraceE("%s: ERROR %#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
        if(lpDDSPrim = dxwss.GetPrimarySurface()) {
            ddsd.dwSize = Set_dwSize_From_Surface();
            res = lpDDSPrim->GetSurfaceDesc((LPDDSURFACEDESC)&ddsd);
            if(res) {
                OutTraceE("%s: GetSurfaceDesc ERROR %#x(%s)\n",
                          ApiRef, res, ExplainDDError(res));
                return res;
            }
            res = (lpEnumSurfacesCallback)(lpDDSPrim, (LPDDSURFACEDESC)&ddsd, lpContext);
            OutTraceDW("%s: Callback on DDSPrim=%#x res=%#x(%s)\n",
                       ApiRef, lpDDSPrim, res, res ? "DDENUMRET_OK" : "DDENUMRET_CANCEL");
        }
        return DD_OK; // for GTA (first episode, window version "gtawin.exe")
    }
    res = (*pEnumAttachedSurfaces)(lpdds, lpContext, lpEnumSurfacesCallback);
    _if(res) OutTraceE("%s: ERROR %#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
    return res;
}

HRESULT WINAPI extEnumAttachedSurfaces1(LPDIRECTDRAWSURFACE lpdds, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback) {
    return extEnumAttachedSurfaces(1, pEnumAttachedSurfaces1, lpdds, lpContext, lpEnumSurfacesCallback);
}
HRESULT WINAPI extEnumAttachedSurfaces2(LPDIRECTDRAWSURFACE lpdds, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback) {
    return extEnumAttachedSurfaces(2, pEnumAttachedSurfaces2, lpdds, lpContext, lpEnumSurfacesCallback);
}
HRESULT WINAPI extEnumAttachedSurfaces3(LPDIRECTDRAWSURFACE lpdds, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback) {
    return extEnumAttachedSurfaces(3, pEnumAttachedSurfaces3, lpdds, lpContext, lpEnumSurfacesCallback);
}
HRESULT WINAPI extEnumAttachedSurfaces4(LPDIRECTDRAWSURFACE lpdds, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback) {
    return extEnumAttachedSurfaces(4, pEnumAttachedSurfaces4, lpdds, lpContext, lpEnumSurfacesCallback);
}
HRESULT WINAPI extEnumAttachedSurfaces7(LPDIRECTDRAWSURFACE lpdds, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback) {
    return extEnumAttachedSurfaces(7, pEnumAttachedSurfaces7, lpdds, lpContext, lpEnumSurfacesCallback);
}

static HRESULT WINAPI extAddAttachedSurface(int dxversion, AddAttachedSurface_Type pAddAttachedSurface, LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddsadd) {
    HRESULT res;
    ApiName("AddAttachedSurface");
    BOOL IsPrim, IsBack;
    DDSURFACEDESC2 sd;
    // You can add backbuffers to primary surfaces to join the flipping chain, but you can't do that
    // to an emulated primary surface, and you receive a DDERR_CANNOTATTACHSURFACE error code.
    // In that case, it's worth to try to emulate the attach, and since the Flip method is emulated,
    // just remember this for further handling in the Flip operation.
    // But beware: this holds to BACKBUFFER surfaces only, and NOT for attached ZBUFFERS or similar!
    // If the surface was created with DDSCAPS_BACKBUFFER/ZBUFFER capabilities, it should be in the
    // surface stack already, so better avoid setting unknown version and capabilities here...
    // Note: should DxWnd trace the difference between a potential BACK/ZBUFFER surface and an active one?
    // The program can create a surface with backbuffer/zbuffer capabilities, but assign the effective
    // role afterwards, here while performing the attach to the master surface.
    IsPrim = dxwss.IsAPrimarySurface(lpdds);
    IsBack = dxwss.IsABackBufferSurface(lpdds);
    OutTraceDDRAW("%s: lpdds=%#x%s lpddsadd=%#x%s\n", ApiRef, lpdds, IsPrim ? "(PRIM)" : (IsBack ? "(BACK)" : ""), lpddsadd, (lpddsadd == lpDDZBuffer) ? "(ZBUF)" : "");
    // get info about the candidate attached surface
    sd.dwSize = Set_dwSize_From_Surface();
    res = lpddsadd->GetSurfaceDesc((DDSURFACEDESC *)&sd);
    if (res) {
        OutTraceE("%s: GetSurfaceDesc ERROR res=%#x at %d\n", ApiRef, res, __LINE__);
        memset(&sd, 0, sizeof(sd));
    } else {
        OutTraceDW("%s: GetSurfaceDesc dwCaps=%#x(%s)\n",
                   ApiRef, sd.ddsCaps.dwCaps, ExplainDDSCaps(sd.ddsCaps.dwCaps));
    }
    //if(!lpddsadd) return DDERR_CANNOTATTACHSURFACE; // to avoid a crash...
    res = (*pAddAttachedSurface)(lpdds, lpddsadd);
    if (res) {
        OutTraceE("%s: ERROR %#x(%s) at %d\n", ApiRef, res, ExplainDDError(res), __LINE__);
        if (IsPrim) {
            if (sd.ddsCaps.dwCaps & DDSCAPS_BACKBUFFER) {
                if ((dxw.IsEmulated) && (res == DDERR_CANNOTATTACHSURFACE) ||
                        (res == DDERR_NOEXCLUSIVEMODE)) {
                    OutTraceDW("%s: emulating BACKBUFFER attach on PRIMARY\n", ApiRef);
                    res = DD_OK;
                }
            }
        }
        //else if (IsBack) {
        else {
            // v2.02.13: emulate ZBUFFER attach to backbuffer/plain surface: do nothing and return OK
            // this trick makes at least "Nocturne" work also in emulated mode when hardware acceleration
            // is set in the game "Options" menu.
            if (sd.ddsCaps.dwCaps & DDSCAPS_ZBUFFER) { // DDSCAPS_BACKBUFFER for double buffering ???
                if ((dxw.IsEmulated) && (res == DDERR_CANNOTATTACHSURFACE)) {
                    OutTraceDW("%s: emulating ZBUFFER attach on %s surface\n", ApiRef, IsBack ? "BACKBUFFER" : "PLAIN");
                    res = DD_OK;
                }
            }
        }
    }
    // v2.04.24: when attaching a generic ZBUFFER somewhere, try inheriting the attachee capabilities
    // Note: this piece of code could become useless when we will predict the actual ZBUFFER capabilities
    // on the CreateSurface event
    // This makes "Wipeout XL" working (again?)
    SurfaceDB_Type *s;
    s = dxwss.GetSurface(lpddsadd);
    if(s && (s->dwCaps & DDSCAPS_ZBUFFER)) {
        if(s->dwCaps == DDSCAPS_ZBUFFER) { // if generic ZBUFFER with no further specifications
            s = dxwss.GetSurface(lpdds);
            if(s) {
                DWORD dwCaps;
                dwCaps = s->dwCaps;
                dwCaps &= ~(DDSCAPS_PRIMARYSURFACE | DDSCAPS_BACKBUFFER | DDSCAPS_FLIP); // strip away incompatible caps
                dwCaps |= DDSCAPS_ZBUFFER; // add ZBUFFER capability
                dxwss.PushZBufferSurface(lpddsadd, dxversion, dwCaps);
                OutTraceDW("%s: set ZBUFFER caps=%#x(%s)\n", ApiRef, dwCaps, ExplainDDSCaps(dwCaps));
            }
        }
        // v2.05.02: when there's no defined backbuffer surface, attaching a ZBUFFER to a plain surface
        // somehow promotes it as a potential backbuffer. At least, this happens (or seems to happen) in
        // "Conquest: Frontier Wars". Added a potentially useless check on dxversion == 7 to limit the scope.
        if(!dxwss.GetBackBufferSurface() && (dxversion == 7)) {
            SurfaceDB_Type *sb = dxwss.GetSurface(lpdds);
            DWORD sbCaps = 0;
            if(sb) sbCaps = sb->dwCaps;
            //sbCaps |= DDSCAPS_BACKBUFFER;
            dxwss.PushBackBufferSurface(lpdds, dxversion, sbCaps);
        }
    }
    // v2.04.98: custom fix for "Alchemist": you can add a VIDEOMEMORY+OFFSCREENPLAIN surface to
    // the primary surface and this will become a flippable backbuffer surface.
    if(IsPrim && !s) {
        DWORD dwCaps = sd.ddsCaps.dwCaps;
        // revert emulation fixes
        dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
        dwCaps |= ~DDSCAPS_VIDEOMEMORY;
        OutTraceDW("%s: promote OFFSCREEN to BACKBUFFER\n", ApiRef);
        dxwss.PushBackBufferSurface(lpddsadd, dxversion, dwCaps);
    }
    _if(res) OutTraceE("%s: ERROR %#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extAddAttachedSurface1(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddsadd) {
    return extAddAttachedSurface(1, pAddAttachedSurface1, lpdds, lpddsadd);
}
HRESULT WINAPI extAddAttachedSurface2(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddsadd) {
    return extAddAttachedSurface(2, pAddAttachedSurface2, lpdds, lpddsadd);
}
HRESULT WINAPI extAddAttachedSurface3(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddsadd) {
    return extAddAttachedSurface(3, pAddAttachedSurface3, lpdds, lpddsadd);
}
HRESULT WINAPI extAddAttachedSurface4(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddsadd) {
    return extAddAttachedSurface(4, pAddAttachedSurface4, lpdds, lpddsadd);
}
HRESULT WINAPI extAddAttachedSurface7(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddsadd) {
    return extAddAttachedSurface(7, pAddAttachedSurface7, lpdds, lpddsadd);
}

static HRESULT WINAPI extDeleteAttachedSurface(DeleteAttachedSurface_Type pDeleteAttachedSurface, LPDIRECTDRAWSURFACE lpdds,  DWORD dwflags, LPDIRECTDRAWSURFACE lpddsdel) {
    HRESULT res;
    ApiName("DeleteAttachedSurface");
    OutTraceDDRAW("%s: lpdds=%#x flags=%#x lpddsdel=%#x\n", ApiRef, lpdds, dwflags, lpddsdel);
    res = (*pDeleteAttachedSurface)(lpdds, dwflags, lpddsdel);
    _if(res) OutTraceE("%s: ERROR %#x(%s)\n", ApiRef, res, ExplainDDError(res));
    if (res && dxwss.IsABackBufferSurface(lpddsdel)) {
        OutTraceDW("%s: emulating BACKBUFFER detach lpdds=%#x\n", ApiRef, lpddsdel);
        // v2.05.52: added decrement of reference counter (to be tested!!)
        lpddsdel->Release();
        res = DD_OK;
    }
    // v2.05.52: if it was a ZBUFFER emulated attach, keep pretending it is still attached
    if(res == DDERR_SURFACENOTATTACHED) {
        SurfaceDB_Type *s;
        s = dxwss.GetSurface(lpddsdel);
        if(s && (s->dwCaps & DDSCAPS_ZBUFFER)) {
            OutTraceDW("%s: emulating ZBUFFER detach lpdds=%#x\n", ApiRef, lpddsdel);
            lpddsdel->Release();
            res = DD_OK;
        }
    }
    if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    return res;
}

HRESULT WINAPI extDeleteAttachedSurface1(LPDIRECTDRAWSURFACE lpdds,  DWORD dwflags, LPDIRECTDRAWSURFACE lpddsdel) {
    return extDeleteAttachedSurface(pDeleteAttachedSurface1, lpdds,  dwflags, lpddsdel);
}
HRESULT WINAPI extDeleteAttachedSurface2(LPDIRECTDRAWSURFACE lpdds,  DWORD dwflags, LPDIRECTDRAWSURFACE lpddsdel) {
    return extDeleteAttachedSurface(pDeleteAttachedSurface2, lpdds,  dwflags, lpddsdel);
}
HRESULT WINAPI extDeleteAttachedSurface3(LPDIRECTDRAWSURFACE lpdds,  DWORD dwflags, LPDIRECTDRAWSURFACE lpddsdel) {
    return extDeleteAttachedSurface(pDeleteAttachedSurface3, lpdds,  dwflags, lpddsdel);
}
HRESULT WINAPI extDeleteAttachedSurface4(LPDIRECTDRAWSURFACE lpdds,  DWORD dwflags, LPDIRECTDRAWSURFACE lpddsdel) {
    return extDeleteAttachedSurface(pDeleteAttachedSurface4, lpdds,  dwflags, lpddsdel);
}
HRESULT WINAPI extDeleteAttachedSurface7(LPDIRECTDRAWSURFACE lpdds,  DWORD dwflags, LPDIRECTDRAWSURFACE lpddsdel) {
    return extDeleteAttachedSurface(pDeleteAttachedSurface7, lpdds,  dwflags, lpddsdel);
}

HRESULT WINAPI cbDump(LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext) {
    OutTraceDW("EnumDisplayModes: CALLBACK lpdds=%#x Context=%#x Caps=%#x(%s)\n",
               lpDDSurfaceDesc, lpContext,
               lpDDSurfaceDesc->ddsCaps.dwCaps, ExplainDDSCaps(lpDDSurfaceDesc->ddsCaps.dwCaps));
    return 1;
}

// BEWARE!!!
// in order to meet Direct3D expectations, it seems that we require this policy:
// when a ZBUFFER surface is created, its dwCaps value must be saved and restored identically on GetSurfaceDesc and GetCaps
// A surface is known to be virtually a primary or backbuffer only by looking at the surfaces stack dxwss
// On the contrary, a surface is known to be a ZBUFFER only after retrieving its capabilities with pGetCaps or pGetSurfaceDesc
// hence the IsZBuf flag is set after the query and can't be reflected in the first log line!
// The same comment / logic is valid also for extGetSurfaceDesc wrapper

static HRESULT WINAPI extGetCapsS(int dxInterface, GetCapsS_Type pGetCapsS, LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS caps) {
    HRESULT res;
    BOOL IsFixed = FALSE;
    int role;
    SurfaceDB_Type *ps;
    char *sLabel;
    ps = dxwss.GetSurface(lpdds);
    // BEWARE: ZBUFFER surfaces could be created automatically so that they could exist without
    // being regisered in surface stack. You must query the actual surface capabilities.
    res = (*pGetCapsS)(lpdds, caps);
    if(res)
        OutTraceE("GetCaps(S%d): ERROR lpdds=%#x err=%#x(%s)\n", dxInterface, lpdds, res, ExplainDDError(res));
    else
        OutTraceDDRAW("GetCaps(S%d): lpdds=%#x caps=%#x(%s)\n", dxInterface, lpdds, caps->dwCaps, ExplainDDSCaps(caps->dwCaps));
    sLabel = "";
    role = SURFACE_ROLE_UNKNOWN;
    if (ps) {
        role = ps->uRole;
        switch(role) {
        case SURFACE_ROLE_PRIMARY:
            sLabel = "(PRIM)";
            break;
        case SURFACE_ROLE_BACKBUFFER:
            sLabel = "(BACK)";
            break;
        case SURFACE_ROLE_ZBUFFER:
            sLabel = "(ZBUF)";
            break;
        case SURFACE_ROLE_3DREF:
            sLabel = "(3DREF)";
            break;
        }
    } else {
        if (caps->dwCaps & DDSCAPS_ZBUFFER) {
            sLabel = "(ZBUF)";
            role = SURFACE_ROLE_ZBUFFER;
        }
    }
    if(!(dxw.IsEmulated || dxw.Windowize)) return res;
    switch(role) {
    case SURFACE_ROLE_PRIMARY:
        IsFixed = TRUE;
        caps->dwCaps = SetPrimaryCaps(dxwss.GetCaps(lpdds));
        break;
    case SURFACE_ROLE_BACKBUFFER:
        IsFixed = TRUE;
        caps->dwCaps = SetBackBufferCaps(dxwss.GetCaps(lpdds));
        break;
    case SURFACE_ROLE_ZBUFFER:
        IsFixed = TRUE;
        if(dxw.IsEmulated) caps->dwCaps = SetZBufferCaps(dxwss.GetCaps(lpdds));
        break;
    case SURFACE_ROLE_3DREF:
        IsFixed = TRUE;
        if(dxw.IsEmulated) caps->dwCaps = dxwss.GetCaps(lpdds);
        break;
    }
    if(IsFixed)
        OutTraceDW("GetCaps(S%d): lpdds=%#x FIXED %s caps=%#x(%s)\n", dxInterface, lpdds, sLabel, caps->dwCaps, ExplainDDSCaps(caps->dwCaps));
    OutHexDDRAW((unsigned char *)caps, sizeof(DDSCAPS));
    return res;
}

HRESULT WINAPI extGetCaps1S(LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS caps) {
    return extGetCapsS(1, pGetCaps1S, lpdds, caps);
}
HRESULT WINAPI extGetCaps2S(LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS caps) {
    return extGetCapsS(2, pGetCaps2S, lpdds, caps);
}
HRESULT WINAPI extGetCaps3S(LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS caps) {
    return extGetCapsS(3, pGetCaps3S, lpdds, caps);
}
HRESULT WINAPI extGetCaps4S(LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS2 caps) {
    return extGetCapsS(4, (GetCapsS_Type)pGetCaps4S, lpdds, (LPDDSCAPS)caps);
}
HRESULT WINAPI extGetCaps7S(LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS2 caps) {
    return extGetCapsS(7, (GetCapsS_Type)pGetCaps7S, lpdds, (LPDDSCAPS)caps);
}

#ifdef TRACEALL
HRESULT WINAPI extGetClipper(int dxversion, GetClipper_Type pGetClipper, LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWCLIPPER *lplpddc) {
    HRESULT res;
    ApiName("GetClipper");
    OutTraceDDRAW("%s(%d): lpdds=%#x\n", ApiRef, dxversion, lpdds);
    res = (*pGetClipper)(lpdds, lplpddc);
    if(res) {
        OutTraceE("%s(%d): ERROR err=%#x(%s)\n", ApiRef, dxversion, res, ExplainDDError(res));
        return res;
    }
    OutTraceDDRAW("%s(%d): lpddc=%#x\n", ApiRef, dxversion, *lplpddc);
    return res;
}

HRESULT WINAPI extGetClipper1(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWCLIPPER *lpddc) {
    return extGetClipper(1, pGetClipper1, lpdds, lpddc);
}
HRESULT WINAPI extGetClipper2(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWCLIPPER *lpddc) {
    return extGetClipper(2, pGetClipper2, lpdds, lpddc);
}
HRESULT WINAPI extGetClipper3(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWCLIPPER *lpddc) {
    return extGetClipper(3, pGetClipper3, lpdds, lpddc);
}
HRESULT WINAPI extGetClipper4(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWCLIPPER *lpddc) {
    return extGetClipper(4, pGetClipper4, lpdds, lpddc);
}
HRESULT WINAPI extGetClipper7(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWCLIPPER *lpddc) {
    return extGetClipper(7, pGetClipper7, lpdds, lpddc);
}
#endif

static HRESULT WINAPI extGetSurfaceDesc(int dxversion, GetSurfaceDesc_Type pGetSurfaceDesc, LPDIRECTDRAWSURFACE lpdds, LPDDSURFACEDESC lpddsd) {
    HRESULT res;
    ApiName("GetSurfaceDesc");
    BOOL IsFixed = FALSE;
    SurfaceDB_Type *ps;
    int role;
    char *sLabel;
    int prevsize;
    OutTraceDDRAW("%s(%d): lpdds=%#x lpddsd=%#x\n",  ApiRef, dxversion, lpdds, lpddsd);
    if (!pGetSurfaceDesc) {
        OutTraceE("%s: ERROR no hooked function\n", ApiRef);
        return DDERR_INVALIDPARAMS;
    }
    ps = dxwss.GetSurface(lpdds);
    // v2.05.22: do not assume lpddsd is a pointer to safe place, ddraw can handle this mistake,
    // dxwnd must do that too. Fixes a bad pointer fying into "Dragon Riders Chronicles of Pern"
    __try {
        prevsize = lpddsd->dwSize;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        OutTraceE("%s: ERROR invalid lpddsd pointer\n", ApiRef);
        return DDERR_INVALIDPARAMS;
    }
    switch(dxversion) {
    case 1:
    case 2:
    case 3:
        lpddsd->dwSize = sizeof(DDSURFACEDESC);
        break;
    case 4:
    case 7:
        lpddsd->dwSize = sizeof(DDSURFACEDESC2);
        break;
    }
#ifndef DXW_NOTRACES
    if(prevsize != lpddsd->dwSize) OutTraceDW("%s(%d): FIXED dwSize=%d->%d\n", ApiRef, dxversion, prevsize, lpddsd->dwSize);
#endif // DXW_NOTRACES
    res = (*pGetSurfaceDesc)(lpdds, lpddsd);
    if(res) {
        OutTraceE("%s(%d): ERROR err=%#x(%s)\n", ApiRef, dxversion, res, ExplainDDError(res));
        return res;
    }
    sLabel = "";
    role = SURFACE_ROLE_UNKNOWN;
    if (ps) {
        role = ps->uRole;
        switch(role) {
        case SURFACE_ROLE_PRIMARY:
            sLabel = "(PRIM)";
            break;
        case SURFACE_ROLE_BACKBUFFER:
            sLabel = "(BACK)";
            break;
        case SURFACE_ROLE_ZBUFFER:
            sLabel = "(ZBUF)";
            break;
        case SURFACE_ROLE_3DREF:
            sLabel = "(3DREF)";
            break;
        }
    } else {
        if (lpddsd->ddsCaps.dwCaps & DDSCAPS_ZBUFFER) {
            sLabel = "(ZBUF)";
            role = SURFACE_ROLE_ZBUFFER;
        }
    }
    OutTraceDDRAW("%s(%d): lpdds=%#x%s %s\n",
                  ApiRef, dxversion, lpdds, sLabel, LogSurfaceAttributes((LPDDSURFACEDESC2)lpddsd, ApiRef, __LINE__));
    if(!(dxw.IsEmulated || dxw.Windowize)) return res;
    switch(role) {
    case SURFACE_ROLE_PRIMARY:
        IsFixed = TRUE;
        lpddsd->ddsCaps.dwCaps = SetPrimaryCaps(dxwss.GetCaps(lpdds));
        lpddsd->dwBackBufferCount = DDSD_Prim.dwBackBufferCount;
        lpddsd->dwHeight = dxw.GetScreenHeight();
        lpddsd->dwWidth = dxw.GetScreenWidth();
        break;
    case SURFACE_ROLE_BACKBUFFER:
        IsFixed = TRUE;
        lpddsd->ddsCaps.dwCaps = SetBackBufferCaps(dxwss.GetCaps(lpdds));
        break;
    case SURFACE_ROLE_ZBUFFER:
        IsFixed = TRUE;
        lpddsd->ddsCaps.dwCaps = SetZBufferCaps(dxwss.GetCaps(lpdds));
        break;
    case SURFACE_ROLE_3DREF:
        IsFixed = TRUE;
        lpddsd->ddsCaps.dwCaps = dxwss.GetCaps(lpdds);
        break;
    }
    if(IsFixed) {
        OutTraceDW("%s: FIXED lpdds=%#x %s\n",
                   ApiRef, lpdds, LogSurfaceAttributes((LPDDSURFACEDESC2)lpddsd, sLabel, __LINE__));
    }
    OutHexDDRAW((unsigned char *)lpddsd, sizeof(DDSURFACEDESC));
    return DD_OK;
}

// Beware: despite the surface version, some game (The Sims!!!) intentionally uses a different dwSize, so that
// you shouldn't reset the value

HRESULT WINAPI extGetSurfaceDesc1(LPDIRECTDRAWSURFACE lpdds, LPDDSURFACEDESC lpddsd) {
    return extGetSurfaceDesc(1, pGetSurfaceDesc1, lpdds, lpddsd);
}
HRESULT WINAPI extGetSurfaceDesc2(LPDIRECTDRAWSURFACE lpdds, LPDDSURFACEDESC lpddsd) {
    return extGetSurfaceDesc(2, pGetSurfaceDesc2, lpdds, lpddsd);
}
HRESULT WINAPI extGetSurfaceDesc3(LPDIRECTDRAWSURFACE lpdds, LPDDSURFACEDESC lpddsd) {
    return extGetSurfaceDesc(3, pGetSurfaceDesc3, lpdds, lpddsd);
}
HRESULT WINAPI extGetSurfaceDesc4(LPDIRECTDRAWSURFACE2 lpdds, LPDDSURFACEDESC2 lpddsd) {
    return extGetSurfaceDesc(4, (GetSurfaceDesc_Type)pGetSurfaceDesc4, (LPDIRECTDRAWSURFACE)lpdds, (LPDDSURFACEDESC)lpddsd);
}
HRESULT WINAPI extGetSurfaceDesc7(LPDIRECTDRAWSURFACE2 lpdds, LPDDSURFACEDESC2 lpddsd) {
    return extGetSurfaceDesc(7, (GetSurfaceDesc_Type)pGetSurfaceDesc7, (LPDIRECTDRAWSURFACE)lpdds, (LPDDSURFACEDESC)lpddsd);
}

static ULONG WINAPI extReleaseD(int dxversion, ReleaseD_Type pReleaseD, LPDIRECTDRAW lpdd) {
    ULONG ActualRef;
    LONG VirtualRef;
    OutTraceDDRAW("Release(D%d): lpdd=%#x\n", dxversion, lpdd);
    if((ReleaseD_Type)extReleaseD == pReleaseD) {
        OutTraceE("Release(D) ERROR: bad hooker pReleaseD=%#x\n", pReleaseD);
        return 0;
    }
    ActualRef = (*pReleaseD)(lpdd);
    VirtualRef = (LONG)ActualRef;
    OutTraceDW("Release(D): lpdd=%#x service_lpdd=%#x ref=%d\n", lpdd, lpPrimaryDD, ActualRef);
    if (lpdd == lpPrimaryDD) { // v2.1.87: fix for Dungeon Keeper II
        if(dxw.dwFlags4 & FIXREFCOUNTER) {
            // v2.02.41: fix the ref counter to sumulate the unwindowed original situation
            --VirtualRef; // why ????
            if(dxwss.GetBackBufferSurface()) --VirtualRef;
            if(dxw.IsEmulated) {
                if(lpDDSEmu_Prim) --VirtualRef;
                if(lpDDSEmu_Back) --VirtualRef;
                if(lpDDZBuffer) --VirtualRef; // is it correct ? Inserted to fix "Microsoft International Soccer 2000" in hw mode
            }
            if(VirtualRef < 0) VirtualRef = 0;
            OutTraceDW("Release(D): fixed ref counter %d->%d\n", ActualRef, VirtualRef);
        }
        if((dxversion < 4) && (ActualRef == 0)) {
            // directdraw old versions automatically free all linked objects when the parent session is closed.
            OutTraceDW("Release(D): RefCount=0 - service object RESET condition\n");
            lpDDSEmu_Prim = NULL;
            lpDDSEmu_Back = NULL;
            lpDDP = NULL;
            iDDPExtraRefCounter = 0;
        }
        if(ActualRef == 0) lpPrimaryDD = NULL; // v2.03.61
    }
    // when lpdd session is closed (ref==0) the system restores the default color depth
    // so if FORCE16BPP is set, dxwnd must restore the 16BPP value
    //extern void SwitchTo16BPP();
    //if((ActualRef==0) && (dxw.dwFlags3 & FORCE16BPP)) SwitchTo16BPP();
    if(dxw.dwFlags4 & RETURNNULLREF) VirtualRef = 0;
    OutTraceDDRAW("Release(D): lpdd=%#x ref=%#x\n", lpdd, VirtualRef);
    return (ULONG)VirtualRef;
}

ULONG WINAPI extReleaseD1(LPDIRECTDRAW lpdd) {
    return extReleaseD(1, pReleaseD1, lpdd);
}
ULONG WINAPI extReleaseD2(LPDIRECTDRAW lpdd) {
    return extReleaseD(2, pReleaseD2, lpdd);
}
ULONG WINAPI extReleaseD3(LPDIRECTDRAW lpdd) {
    return extReleaseD(3, pReleaseD3, lpdd);
}
ULONG WINAPI extReleaseD4(LPDIRECTDRAW lpdd) {
    return extReleaseD(4, pReleaseD4, lpdd);
}
ULONG WINAPI extReleaseD7(LPDIRECTDRAW lpdd) {
    return extReleaseD(7, pReleaseD7, lpdd);
}

static HRESULT WINAPI extCreateClipper(int dxversion, CreateClipper_Type pCreateClipper, LPDIRECTDRAW lpdd, DWORD dwflags,
                                       LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter) {
    HRESULT res;
    ApiName("CreateClipper");
    OutTraceDDRAW("%s(%d): lpdd=%#x flags=%#x\n", ApiRef, dxversion, lpdd, dwflags);
    res = (*pCreateClipper)(lpdd, dwflags, lplpDDClipper, pUnkOuter);
    if(res) {
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, lpdd, res, ExplainDDError(res));
        return res;
    }
    OutTraceDDRAW("%s: OK lpddclipper=%#x\n", ApiRef, *lplpDDClipper);
    HookDDClipper(lplpDDClipper); // there is a single Clipper intrface!
    return res;
}

HRESULT WINAPI extCreateClipper1(LPDIRECTDRAW lpdd, DWORD dwflags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter) {
    return extCreateClipper(1, pCreateClipper1, lpdd, dwflags, lplpDDClipper, pUnkOuter);
}
HRESULT WINAPI extCreateClipper2(LPDIRECTDRAW lpdd, DWORD dwflags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter) {
    return extCreateClipper(2, pCreateClipper2, lpdd, dwflags, lplpDDClipper, pUnkOuter);
}
HRESULT WINAPI extCreateClipper3(LPDIRECTDRAW lpdd, DWORD dwflags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter) {
    return extCreateClipper(3, pCreateClipper3, lpdd, dwflags, lplpDDClipper, pUnkOuter);
}
HRESULT WINAPI extCreateClipper4(LPDIRECTDRAW lpdd, DWORD dwflags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter) {
    return extCreateClipper(4, pCreateClipper4, lpdd, dwflags, lplpDDClipper, pUnkOuter);
}
HRESULT WINAPI extCreateClipper7(LPDIRECTDRAW lpdd, DWORD dwflags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter) {
    return extCreateClipper(7, pCreateClipper7, lpdd, dwflags, lplpDDClipper, pUnkOuter);
}

HRESULT WINAPI extReleaseC(LPDIRECTDRAWCLIPPER lpddClip) {
    ULONG ref;
    ref = (*pReleaseC)(lpddClip);
    OutTraceDDRAW("Release(C): PROXED lpddClip=%#x ref=%#x\n", lpddClip, ref);
    return ref;
}

HRESULT WINAPI extGetClipList(LPDIRECTDRAWCLIPPER lpddClip, LPRECT lpRect, LPRGNDATA lpRgnData, LPDWORD lpw) {
    HRESULT res;
    ApiName("GetClipList");
    // returned clip region (both RgnData and single RECT array) should be relocated in window mode
    // an easy way to do that, though not accurate, is to consider the clip region as the whole virtual screen
    // a better way is to use the dxw.UnmapWindow(RECT *) method
    // this makes "Full Pipe" working without clipping problems.
#ifndef DXW_NOTRACES
    if(IsTraceDDRAW) {
        char sInfo[81];
        if (lpRect) sprintf(sInfo, "rect=(%d,%d)-(%d,%d) ", lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
        else sprintf(sInfo, "rect=(NULL) ");
        OutTrace("%s(C): lpddClip=%#x %s\n", ApiRef, lpddClip, sInfo);
    }
#endif // DXW_NOTRACES
    res = (*pGetClipList)(lpddClip, lpRect, lpRgnData, lpw);
    if(res) {
        OutTraceE("%s(C): ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    if(lpRgnData) {
        OutTraceDW("%s(C): w=%#x rgndataheader{size=%d type=%#x count=%d RgnSize=%d bound=(%d,%d)-(%d,%d)}\n",
                   ApiRef, *lpw, lpRgnData->rdh.dwSize, lpRgnData->rdh.iType, lpRgnData->rdh.nCount, lpRgnData->rdh.nRgnSize,
                   lpRgnData->rdh.rcBound.left, lpRgnData->rdh.rcBound.top, lpRgnData->rdh.rcBound.right, lpRgnData->rdh.rcBound.bottom);
        if(dxw.Windowize && (dxw.dwFlags1 & CLIENTREMAPPING)) {
            dxw.UnmapClient(&lpRgnData->rdh.rcBound);
            if(dxw.dwFlags5 & MAXCLIPPER) lpRgnData->rdh.rcBound = dxw.GetScreenRect();
            OutTraceDW("%s(C): w=%#x rgndataheader{size=%d type=%#x count=%d RgnSize=%d REMAPPED bound=(%d,%d)-(%d,%d)}\n",
                       ApiRef, *lpw, lpRgnData->rdh.dwSize, lpRgnData->rdh.iType, lpRgnData->rdh.nCount, lpRgnData->rdh.nRgnSize,
                       lpRgnData->rdh.rcBound.left, lpRgnData->rdh.rcBound.top, lpRgnData->rdh.rcBound.right, lpRgnData->rdh.rcBound.bottom);
        }
        RECT *rgns;
        rgns = (RECT *)lpRgnData->Buffer;
        if(IsDebugDW) {
            for(DWORD i = 0; i < lpRgnData->rdh.nCount; i++) {
                OutTrace("> rect[%d]=(%d,%d)-(%d,%d)\n",
                         i, rgns[i].left, rgns[i].top, rgns[i].right, rgns[i].bottom);
            }
        }
        if(dxw.Windowize && (dxw.dwFlags1 & CLIENTREMAPPING)) {
            for(DWORD i = 0; i < lpRgnData->rdh.nCount; i++) {
                dxw.UnmapClient(&rgns[i]);
                if(dxw.dwFlags5 & MAXCLIPPER) rgns[i] = dxw.GetScreenRect();
                if(IsDebugDW) {
                    OutTrace("> REMAPPED rect[%d]=(%d,%d)-(%d,%d)\n",
                             i, rgns[i].left, rgns[i].top, rgns[i].right, rgns[i].bottom);
                }
            }
        }
    } else
        OutTraceDW("%s(C): w=%#x\n", ApiRef, *lpw);
    return DD_OK;
}

HRESULT WINAPI extSetClipList(LPDIRECTDRAWCLIPPER lpddClip, LPRGNDATA lpRgnData, DWORD dwFlags) {
    HRESULT res;
    ApiName("SetClipList");
    OutTraceP("%s(C): lpddClip=%#x lpcliplist=%#x flags=%#x\n", ApiRef, lpddClip, lpRgnData, dwFlags);
    if(lpRgnData) {
        OutTraceDW("%s(C): rgndataheader{size=%d type=%#x count=%d RgnSize=%d bound=(%d,%d)-(%d,%d)}\n",
                   ApiRef, lpRgnData->rdh.dwSize, lpRgnData->rdh.iType, lpRgnData->rdh.nCount, lpRgnData->rdh.nRgnSize,
                   lpRgnData->rdh.rcBound.left, lpRgnData->rdh.rcBound.top, lpRgnData->rdh.rcBound.right, lpRgnData->rdh.rcBound.bottom);
        if(dxw.Windowize && (dxw.dwFlags1 & CLIENTREMAPPING)) {
            dxw.MapClient(&lpRgnData->rdh.rcBound);
            OutTraceDW("%s(C): rgndataheader{size=%d type=%#x count=%d RgnSize=%d REMAPPED bound=(%d,%d)-(%d,%d)}\n",
                       ApiRef, lpRgnData->rdh.dwSize, lpRgnData->rdh.iType, lpRgnData->rdh.nCount, lpRgnData->rdh.nRgnSize,
                       lpRgnData->rdh.rcBound.left, lpRgnData->rdh.rcBound.top, lpRgnData->rdh.rcBound.right, lpRgnData->rdh.rcBound.bottom);
        }
        RECT *rgns;
        rgns = (RECT *)lpRgnData->Buffer;
        if(IsDebugDW) {
            for(DWORD i = 0; i < lpRgnData->rdh.nCount; i++) {
                OutTrace("> rect[%d]=(%d,%d)-(%d,%d)\n",
                         i, rgns[i].left, rgns[i].top, rgns[i].right, rgns[i].bottom);
            }
        }
        if(dxw.Windowize && (dxw.dwFlags1 & CLIENTREMAPPING)) {
            for(DWORD i = 0; i < lpRgnData->rdh.nCount; i++) {
                dxw.MapClient(&rgns[i]);
                if(IsDebugDW) {
                    OutTrace("> REMAPPED rect[%d]=(%d,%d)-(%d,%d)\n",
                             i, rgns[i].left, rgns[i].top, rgns[i].right, rgns[i].bottom);
                }
            }
        }
    }
    res = (*pSetClipList)(lpddClip, lpRgnData, dwFlags);
#ifndef DXW_NOTRACES
    if(res) OutTraceP("%s(C): ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
#endif // DXW_NOTRACES
    return res;
}

HRESULT WINAPI extSetHWnd(LPDIRECTDRAWCLIPPER lpddClip, DWORD dwFlags, HWND hwnd) {
    HRESULT res;
    OutTraceP("SetHWnd(C): lpddClip=%#x flags=%#x hwnd=%#x\n", lpddClip, dwFlags, hwnd);
    res = (*pSetHWnd)(lpddClip, dwFlags, hwnd);
#ifndef DXW_NOTRACES
    if(res) OutTraceP("SetHWnd(C): ERROR err=%#x(%s)\n", res, ExplainDDError(res));
#endif // DXW_NOTRACES
    return res;
}

HRESULT WINAPI extReleaseP(LPDIRECTDRAWPALETTE lpddPalette) {
    ULONG ref;
    OutTraceDDRAW("Release(P): lpddPalette=%#x\n", lpddPalette);
    if(dxw.dwFlags9 & SAFEPALETTEUSAGE) {
        OutTraceDW("Release(P): SUPPRESS ref=0\n");
        return 0;
    }
    __try { // try/except useful when using proxy dll
        ref = (*pReleaseP)(lpddPalette);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        OutTraceE("Release(P): EXCEPTION lpdds=%#x%s res=0\n", lpddPalette);
        ref = 0;
    }
    OutTraceDDRAW("Release(P): lpddPalette=%#x ref=%#x\n", lpddPalette, ref);
    if (lpddPalette == lpDDP) {
        if(dxw.dwFlags4 & FIXREFCOUNTER) {
            OutTraceDW("Release(P): lpDDP ref=%d extrarefcount=%d\n", ref, iDDPExtraRefCounter);
            if((ULONG)iDDPExtraRefCounter >= ref) {
                while(ref) ref = (*pReleaseP)(lpddPalette);
                lpDDP = NULL;
                iDDPExtraRefCounter = 0;
                return 0;
            }
            ref -= iDDPExtraRefCounter;
        }
        if (ref <= 0) {
            ref = 0;
            OutTraceDW("Release(P): clearing lpDDP=%#x->NULL\n", lpDDP);
            lpDDP = NULL;
        }
        if(dxw.dwFlags4 & RETURNNULLREF) ref = 0;
    }
    return ref;
}

static BOOL IsMatchingGuid(LPGUID lpGuid) {
    BOOL matching = FALSE;
    LPGUID lpTargetGuid = dxw.GetForcedMonitorGUID();
    if(!lpTargetGuid) return TRUE; // no target GUID must match everything
    if(!lpGuid) return TRUE; // v2.05.43 fix: default monitor's GUID must match everything
    if((DWORD)lpGuid & 0xFFFF0000)
        matching = IsEqualGUID(*lpGuid, *dxw.GetForcedMonitorGUID());
    return matching;
}

BOOL FAR PASCAL DDEnumerateCallbackFilterW(GUID FAR *lpGuid, LPWSTR lpDriverDescription, LPWSTR lpDriverName, LPVOID lpContext) {
    BOOL res;
    typedef struct {
        LPDDENUMCALLBACKW lpCallback;
        LPVOID lpContext;
    } Context_Type;
    Context_Type *p = (Context_Type *)lpContext;
    OutTraceDW("DDEnumerateCallback: guid=%s DriverDescription=\"%ls\" DriverName=\"%ls\" Context=%#x\n",
               sGUID(lpGuid), lpDriverDescription, lpDriverName, p->lpContext);
    if(dxw.MonitorId != -1) {
        if(IsMatchingGuid(lpGuid)) {
            if(!dxw.Windowize && (dxw.dwFlags2 & HIDEMULTIMONITOR)) {
                // let's pretend this is the default and primary device
                if(!dxw.lpFakePrimaryMonitorGUID) {
                    dxw.lpFakePrimaryMonitorGUID = (LPGUID)malloc(sizeof(GUID));
                    memcpy((void *)dxw.lpFakePrimaryMonitorGUID, (void *)lpGuid, sizeof(GUID));
                }
                res = (*p->lpCallback)(0, L"Primary Display Driver", L"display", p->lpContext);
            } else
                res = (*p->lpCallback)(lpGuid, lpDriverDescription, lpDriverName, p->lpContext);
        } else {
            OutTraceDW("DDEnumerateCallback: SKIP DriverName=\"%ls\"\n", lpDriverName);
            res = TRUE;
        }
    } else {
        if((lpGuid == NULL) || !(dxw.dwFlags2 & HIDEMULTIMONITOR)) res = (*p->lpCallback)(lpGuid, lpDriverDescription, lpDriverName, p->lpContext);
        if((lpGuid == NULL) && (dxw.dwFlags2 & HIDEMULTIMONITOR)) res = FALSE;
    }
    OutTraceDW("DDEnumerateCallback: res=%#x(%s)\n", res, res ? "continue" : "break");
    return res;
}

BOOL FAR PASCAL DDEnumerateCallbackExFilterW(GUID FAR *lpGuid, LPWSTR lpDriverDescription, LPWSTR lpDriverName, LPVOID lpContext, HMONITOR hm) {
    BOOL res;
    typedef struct {
        LPDDENUMCALLBACKEXW lpCallback;
        LPVOID lpContext;
    } Context_Type;
    Context_Type *p = (Context_Type *)lpContext;
    OutTraceDW("DDEnumerateCallbackEx: guid=%s DriverDescription=\"%ls\" DriverName=\"%ls\" Context=%#x hm=%#x\n",
               sGUID(lpGuid), lpDriverDescription, lpDriverName, lpContext, hm);
    res = TRUE;
    // single monitor handling
    if(dxw.MonitorId != -1) {
        if(IsMatchingGuid(lpGuid)) {
            if(!dxw.Windowize && (dxw.dwFlags2 & HIDEMULTIMONITOR)) {
                // let's pretend this is the default and primary device
                if(!dxw.lpFakePrimaryMonitorGUID) {
                    dxw.lpFakePrimaryMonitorGUID = (LPGUID)malloc(sizeof(GUID));
                    memcpy((void *)dxw.lpFakePrimaryMonitorGUID, (void *)lpGuid, sizeof(GUID));
                }
                res = (*p->lpCallback)(0, L"Primary Display Driver", L"display", p->lpContext, hm);
            }
        } else {
            OutTraceDW("DDEnumerateCallbackEx: SKIP DriverName=\"%ls\"\n", lpDriverName);
            res = TRUE;
        }
    } else {
        if((lpGuid == NULL) || !(dxw.dwFlags2 & HIDEMULTIMONITOR)) res = (*p->lpCallback)(lpGuid, lpDriverDescription, lpDriverName, p->lpContext, hm);
        if((lpGuid == NULL) && (dxw.dwFlags2 & HIDEMULTIMONITOR)) res = FALSE;
    }
    OutTraceDW("DDEnumerateCallbackEx: res=%#x(%s)\n", res, res ? "continue" : "break");
    return res;
}

BOOL FAR PASCAL DDEnumerateCallbackFilterA(GUID FAR *lpGuid, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext) {
    BOOL res;
    typedef struct {
        LPDDENUMCALLBACK lpCallback;
        LPVOID lpContext;
    } Context_Type;
    Context_Type *p = (Context_Type *)lpContext;
    OutTraceDW("DDEnumerateCallback: guid=%s DriverDescription=\"%s\" DriverName=\"%s\" Context=%#x\n",
               sGUID(lpGuid), lpDriverDescription, lpDriverName, p->lpContext);
    res = TRUE;
    if(dxw.MonitorId != -1) {
        if(IsMatchingGuid(lpGuid)) {
            if(!dxw.Windowize && (dxw.dwFlags2 & HIDEMULTIMONITOR)) {
                // let's pretend this is the default and primary device
                if(!dxw.lpFakePrimaryMonitorGUID) {
                    dxw.lpFakePrimaryMonitorGUID = (LPGUID)malloc(sizeof(GUID));
                    memcpy((void *)dxw.lpFakePrimaryMonitorGUID, (void *)lpGuid, sizeof(GUID));
                }
                res = (*p->lpCallback)(0, "Primary Display Driver", "display", p->lpContext);
            } else
                res = (*p->lpCallback)(lpGuid, lpDriverDescription, lpDriverName, p->lpContext);
        } else {
            OutTraceDW("DDEnumerateCallback: SKIP DriverName=\"%s\"\n", lpDriverName);
            res = TRUE;
        }
    } else {
        if((lpGuid == NULL) || !(dxw.dwFlags2 & HIDEMULTIMONITOR)) res = (*p->lpCallback)(lpGuid, lpDriverDescription, lpDriverName, p->lpContext);
        if((lpGuid == NULL) && (dxw.dwFlags2 & HIDEMULTIMONITOR)) res = FALSE;
    }
    OutTraceDW("DDEnumerateCallback: res=%#x(%s)\n", res, res ? "continue" : "break");
    return res;
}

BOOL FAR PASCAL DDEnumerateCallbackExFilterA(GUID FAR *lpGuid, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm) {
    BOOL res;
    typedef struct {
        LPDDENUMCALLBACKEX lpCallback;
        LPVOID lpContext;
    } Context_Type;
    Context_Type *p = (Context_Type *)lpContext;
    OutTraceDW("DDEnumerateCallbackEx: guid=%s DriverDescription=\"%s\" DriverName=\"%s\" Context=%#x hm=%#x\n",
               sGUID(lpGuid), lpDriverDescription, lpDriverName, lpContext, hm);
    res = TRUE;
    if(dxw.MonitorId != -1) {
        if(IsMatchingGuid(lpGuid)) {
            if(!dxw.Windowize && (dxw.dwFlags2 & HIDEMULTIMONITOR)) {
                // let's pretend this is the default and primary device
                if(!dxw.lpFakePrimaryMonitorGUID) {
                    dxw.lpFakePrimaryMonitorGUID = (LPGUID)malloc(sizeof(GUID));
                    memcpy((void *)dxw.lpFakePrimaryMonitorGUID, (void *)lpGuid, sizeof(GUID));
                }
                res = (*p->lpCallback)(0, "Primary Display Driver", "display", p->lpContext, hm);
            } else
                res = (*p->lpCallback)(lpGuid, lpDriverDescription, lpDriverName, p->lpContext, hm);
        } else {
            OutTraceDW("DDEnumerateCallbackEx: SKIP DriverName=\"%s\"\n", lpDriverName);
            res = TRUE;
        }
    } else {
        if((lpGuid == NULL) || !(dxw.dwFlags2 & HIDEMULTIMONITOR)) res = (*p->lpCallback)(lpGuid, lpDriverDescription, lpDriverName, p->lpContext, hm);
        if((lpGuid == NULL) && (dxw.dwFlags2 & HIDEMULTIMONITOR)) res = FALSE;
    }
    OutTraceDW("DDEnumerateCallbackEx: res=%#x(%s)\n", res, res ? "continue" : "break");
    return res;
}

HRESULT WINAPI extDirectDrawEnumerateA(LPDDENUMCALLBACKA lpCallback, LPVOID lpContext) {
    HRESULT res;
    ApiName("DirectDrawEnumerateA");
    OutTraceDDRAW("%s: lpCallback=%#x lpContext=%#x\n", ApiRef, lpCallback, lpContext);
    if((dxw.dwFlags2 & HIDEMULTIMONITOR) || (dxw.dwTFlags & OUTDEBUG) || (dxw.MonitorId != -1)) {
        struct {
            LPDDENUMCALLBACKA lpCallback;
            LPVOID lpContext;
        } myContext;
        myContext.lpCallback = lpCallback;
        myContext.lpContext = lpContext;
        res = (*pDirectDrawEnumerateA)(DDEnumerateCallbackFilterA, (LPVOID)&myContext);
    } else
        res = (*pDirectDrawEnumerateA)(lpCallback, lpContext);
    _if(res) OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extDirectDrawEnumerateW(LPDDENUMCALLBACKW lpCallback, LPVOID lpContext) {
    HRESULT res;
    ApiName("DirectDrawEnumerateW");
    OutTraceDDRAW("%s: lpCallback=%#x lpContext=%#x\n", ApiRef, lpCallback, lpContext);
    if((dxw.dwFlags2 & HIDEMULTIMONITOR) || (dxw.dwTFlags & OUTDEBUG) || (dxw.MonitorId != -1)) {
        struct {
            LPDDENUMCALLBACKW lpCallback;
            LPVOID lpContext;
        } myContext;
        myContext.lpCallback = lpCallback;
        myContext.lpContext = lpContext;
        res = (*pDirectDrawEnumerateW)(DDEnumerateCallbackFilterW, (LPVOID)&myContext);
    } else
        res = (*pDirectDrawEnumerateW)(lpCallback, lpContext);
    _if(res) OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extDirectDrawEnumerateExA(LPDDENUMCALLBACKEXA lpCallback, LPVOID lpContext, DWORD dwFlags) {
    HRESULT res;
    ApiName("DirectDrawEnumerateExA");
    OutTraceDDRAW("%s: lpCallback=%#x lpContext=%#x Flags=%#x(%s)\n",
                  ApiRef, lpCallback, lpContext, dwFlags, ExplainDDEnumerateFlags(dwFlags));
    if((dxw.dwFlags2 & HIDEMULTIMONITOR) || (dxw.dwTFlags & OUTDEBUG) || (dxw.MonitorId != -1)) {
        struct {
            LPDDENUMCALLBACKEXA lpCallback;
            LPVOID lpContext;
        } myContext;
        myContext.lpCallback = lpCallback;
        myContext.lpContext = lpContext;
        if(dxw.Windowize && (dxw.dwFlags2 & HIDEMULTIMONITOR))
            dwFlags &= ~(DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_DETACHEDSECONDARYDEVICES | DDENUM_NONDISPLAYDEVICES);
        res = (*pDirectDrawEnumerateExA)(DDEnumerateCallbackExFilterA, (LPVOID)&myContext, dwFlags);
    } else
        res = (*pDirectDrawEnumerateExA)(lpCallback, lpContext, dwFlags);
    _if(res) OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    return res;
}

HRESULT WINAPI extDirectDrawEnumerateExW(LPDDENUMCALLBACKEXW lpCallback, LPVOID lpContext, DWORD dwFlags) {
    HRESULT res;
    ApiName("DirectDrawEnumerateExW");
    OutTraceDDRAW("%s: lpCallback=%#x lpContext=%#x Flags=%#x(%s)\n",
                  ApiRef, lpCallback, lpContext, dwFlags, ExplainDDEnumerateFlags(dwFlags));
    if((dxw.dwFlags2 & HIDEMULTIMONITOR) || (dxw.dwTFlags & OUTDEBUG) || (dxw.MonitorId != -1)) {
        struct {
            LPDDENUMCALLBACKEXW lpCallback;
            LPVOID lpContext;
        } myContext;
        myContext.lpCallback = lpCallback;
        myContext.lpContext = lpContext;
        if(dxw.Windowize && (dxw.dwFlags2 & HIDEMULTIMONITOR))
            dwFlags &= ~(DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_DETACHEDSECONDARYDEVICES | DDENUM_NONDISPLAYDEVICES);
        res = (*pDirectDrawEnumerateExW)(DDEnumerateCallbackExFilterW, (LPVOID)&myContext, dwFlags);
    } else
        res = (*pDirectDrawEnumerateExW)(lpCallback, lpContext, dwFlags);
    _if(res) OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    return res;
}

HRESULT WINAPI extDDGetGammaRamp(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags, LPDDGAMMARAMP lpgr) {
    HRESULT ret;
    ApiName("GetGammaRamp");
    OutTraceDDRAW("%s: dds=%#x dwFlags=%#x\n", ApiRef, lpdds, dwFlags);
    if(dxwss.IsAPrimarySurface(lpdds)) lpdds = lpDDSEmu_Prim;
    ret = (*pDDGetGammaRamp)(lpdds, dwFlags, lpgr);
    if(ret) {
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, ret, ExplainDDError(ret));
        if(dxw.dwFlags1 & SUPPRESSDXERRORS) {
            // clear the error code, and provide a reasonable gamma ramp array
            for(int i = 0; i < 256; i++) lpgr->red[i] = lpgr->green[i] = lpgr->blue[i] = (i * 0x100);
            ret = 0;
        }
    } else {
        if(IsDebugDDRAW) {
            OutTrace("%s: RGB=", ApiRef);
            for(int i = 0; i < 256; i++) OutTrace("(%#x,%#x,%#x)", lpgr->red[i], lpgr->green[i], lpgr->blue[i]);
            OutTrace("\n");
        }
    }
    return ret;
}

HRESULT WINAPI extDDSetGammaRamp(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags, LPDDGAMMARAMP lpgr) {
    HRESULT res;
    ApiName("SetGammaRamp");
    OutTraceDDRAW("%s: dds=%#x dwFlags=%#x\n", ApiRef, lpdds, dwFlags);
    if(IsDebugDDRAW) {
        OutTrace("%s: RGB=", ApiRef);
        for(int i = 0; i < 256; i++) OutTrace("(%#x,%#x,%#x)", lpgr->red[i], lpgr->green[i], lpgr->blue[i]);
        OutTrace("\n");
    }
    if (dxw.dwFlags2 & DISABLEGAMMARAMP) return DD_OK;
    res = (*pDDSetGammaRamp)(lpdds, dwFlags, lpgr);
    _if(res) OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    return res;
}

static HRESULT WINAPI extGetAvailableVidMem(int dxversion, GetAvailableVidMem4_Type pGetAvailableVidMem, LPDIRECTDRAW lpdd, LPDDSCAPS2 lpDDSCaps, LPDWORD lpdwTotal, LPDWORD lpdwFree) {
    HRESULT res;
    ApiName("GetAvailableVidMem");
    //const DWORD dwMaxMem = 0x7FFFF000;
    // v03.01.01: limit to smaller value to allow "Breath of Fire IV" card detection
    const DWORD dwMaxMem = 0x70000000;
    const DWORD dwHugeMem = 0xF0000000;
    OutTraceDDRAW("%s(D%d): lpdd=%#x\n", ApiRef, dxversion, lpdd);
    res = (*pGetAvailableVidMem)(lpdd, lpDDSCaps, lpdwTotal, lpdwFree);
    if(res) {
        if((dxw.dwFlags3 & FORCESHEL) && (res == DDERR_NODIRECTDRAWHW)) {
            // fake some video memory....
            OutTraceDW("%s(D): FORCESHEL mode Total=Free=%#x\n", ApiRef, dwMaxMem);
            if(lpdwTotal) *lpdwTotal = dwMaxMem;
            if(lpdwFree) *lpdwFree = dwMaxMem;
            return DD_OK;
        }
        OutTraceE("%s(D): ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    if(dxversion < 4) { // fix: should include also dxversion 3!!!
        OutTraceDW("%s(D%d): DDSCaps=%#x(%s) Total=%#x Free=%#x\n",
                   ApiRef, dxversion,
                   lpDDSCaps->dwCaps, ExplainDDSCaps(lpDDSCaps->dwCaps),
                   lpdwTotal ? *lpdwTotal : 0, lpdwFree ? *lpdwFree : 0);
    } else {
        OutTraceDW("%s(D%d): DDSCaps=%#x(%s).%#x.%#x.%#x volumedepth=%d Total=%#x Free=%#x\n",
                   ApiRef, dxversion,
                   lpDDSCaps->dwCaps, ExplainDDSCaps(lpDDSCaps->dwCaps),
                   lpDDSCaps->dwCaps2, lpDDSCaps->dwCaps3, lpDDSCaps->dwCaps4, lpDDSCaps->dwVolumeDepth,
                   lpdwTotal ? *lpdwTotal : 0, lpdwFree ? *lpdwFree : 0);
    }
    if(!(dxw.dwFlags2 & LIMITRESOURCES)) return res;
    // may need hints ....
    if(dxw.bHintActive) {
        if(lpdwTotal && (*lpdwTotal > dwMaxMem)) ShowHint(HINT_LIMITMEM);
        if(lpdwFree  && (*lpdwFree  > dwMaxMem)) ShowHint(HINT_LIMITMEM);
    }
    // simulate a value overflow condition
    if(dxw.dwDFlags & STRESSRESOURCES) {
        if(lpdwTotal) *lpdwTotal = dwHugeMem;
        if(lpdwFree) *lpdwFree = dwHugeMem;
        return DD_OK;
    }
    // check for memory value overflow - see "Mageslayer" and "Take no Prisoners"
    DWORD dwLocalTotal;
    if(lpdwTotal == NULL) {
        lpdwTotal = &dwLocalTotal; // point to usable memory....
        res = (*pGetAvailableVidMem)(lpdd, lpDDSCaps, lpdwTotal, lpdwFree); // do it again to get total memory
    }
    if(*lpdwTotal > dwMaxMem) {
        if(lpdwFree != NULL) {
            DWORD dwDiff = *lpdwTotal - *lpdwFree;
            if(dwDiff > dwMaxMem)
                *lpdwFree = dwMaxMem;
            else
                *lpdwFree = dwMaxMem - dwDiff;
        }
        *lpdwTotal = dwMaxMem;
        // v2.05.01: fixed possible crash in log message
        OutTraceDW("%s(D): FIXED Total=%#x Free=%#x\n", ApiRef, *lpdwTotal, lpdwFree ? *lpdwFree : 0);
    }
    return res;
}

HRESULT WINAPI extGetAvailableVidMem2(LPDIRECTDRAW lpdd, LPDDSCAPS lpDDSCaps, LPDWORD lpdwTotal, LPDWORD lpdwFree) {
    return extGetAvailableVidMem(2, (GetAvailableVidMem4_Type)pGetAvailableVidMem2, lpdd, (LPDDSCAPS2)lpDDSCaps, lpdwTotal, lpdwFree);
}
HRESULT WINAPI extGetAvailableVidMem3(LPDIRECTDRAW lpdd, LPDDSCAPS lpDDSCaps, LPDWORD lpdwTotal, LPDWORD lpdwFree) {
    return extGetAvailableVidMem(3, (GetAvailableVidMem4_Type)pGetAvailableVidMem3, lpdd, (LPDDSCAPS2)lpDDSCaps, lpdwTotal, lpdwFree);
}
HRESULT WINAPI extGetAvailableVidMem4(LPDIRECTDRAW lpdd, LPDDSCAPS2 lpDDSCaps, LPDWORD lpdwTotal, LPDWORD lpdwFree) {
    return extGetAvailableVidMem(4, pGetAvailableVidMem4, lpdd, lpDDSCaps, lpdwTotal, lpdwFree);
}
HRESULT WINAPI extGetAvailableVidMem7(LPDIRECTDRAW lpdd, LPDDSCAPS2 lpDDSCaps, LPDWORD lpdwTotal, LPDWORD lpdwFree) {
    return extGetAvailableVidMem(7, pGetAvailableVidMem7, lpdd, lpDDSCaps, lpdwTotal, lpdwFree);
}

HRESULT WINAPI extSetSurfaceDesc(int dxversion, SetSurfaceDesc_Type pSetSurfaceDesc, LPDIRECTDRAWSURFACE lpdds, LPDDSURFACEDESC lpDDsd2, DWORD dwFlags) {
    HRESULT res;
    ApiName("SetSurfaceDesc");
    if(IsTraceDDRAW) {
        SurfaceDB_Type *ps;
        char *sLabel;
        ps = dxwss.GetSurface(lpdds);
        sLabel = "";
        if (ps) {
            switch(ps->uRole) {
            case SURFACE_ROLE_PRIMARY:
                sLabel = "(PRIM)";
                break;
            case SURFACE_ROLE_BACKBUFFER:
                sLabel = "(BACK)";
                break;
            case SURFACE_ROLE_ZBUFFER:
                sLabel = "(ZBUF)";
                break;
            case SURFACE_ROLE_3DREF:
                sLabel = "(3DREF)";
                break;
            }
        }
        OutTraceDDRAW("%s(%d): lpdds=%#x%s flags=%#x %s\n",
                      ApiRef, dxversion, lpdds, sLabel, dwFlags,
                      LogSurfaceAttributes((LPDDSURFACEDESC2)lpDDsd2, "SetSurfaceDesc", __LINE__));
    }
    // trapped in "Need for Speed Porsche Unleashed". The method bypass causes rendering
    // problems,better leave it as it is ...
    res = (*pSetSurfaceDesc)(lpdds, lpDDsd2, dwFlags);
    _if(res) OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extSetSurfaceDesc3(LPDIRECTDRAWSURFACE lpdds, LPDDSURFACEDESC lpDDsd2, DWORD dwFlags) {
    return extSetSurfaceDesc(3, pSetSurfaceDesc3, lpdds, lpDDsd2, dwFlags);
}
HRESULT WINAPI extSetSurfaceDesc4(LPDIRECTDRAWSURFACE lpdds, LPDDSURFACEDESC lpDDsd2, DWORD dwFlags) {
    return extSetSurfaceDesc(4, pSetSurfaceDesc4, lpdds, lpDDsd2, dwFlags);
}
HRESULT WINAPI extSetSurfaceDesc7(LPDIRECTDRAWSURFACE lpdds, LPDDSURFACEDESC lpDDsd2, DWORD dwFlags) {
    return extSetSurfaceDesc(7, pSetSurfaceDesc7, lpdds, lpDDsd2, dwFlags);
}

HRESULT WINAPI extDirectDrawCreateClipper(DWORD dwFlags, LPDIRECTDRAWCLIPPER *lplpDDClipper, IUnknown *pUnkOuter) {
    HRESULT res;
    ApiName("DirectDrawCreateClipper");
    OutTraceDW("%s: flags=%#x\n", ApiRef, dwFlags);
    res = (*pDirectDrawCreateClipper)(dwFlags, lplpDDClipper, pUnkOuter);
    if(res) {
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceDW("%s: OK lpclipper=%#x\n", ApiRef, *lplpDDClipper);
    HookDDClipper(lplpDDClipper);
    return res;
}

HRESULT WINAPI extEnumOverlayZOrders(int dxversion, EnumOverlayZOrders_Type pEnumOverlayZOrders, LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpfnCallback) {
    HRESULT res;
    ApiName("EnumOverlayZOrders");
    OutTrace("%s(%d): lpdds=%#x flags=%#x(%s)\n", ApiRef, dxversion, lpdds, dwFlags, dwFlags ? "FRONTTOBACK" : "BACKTOFRONT");
    res = (*pEnumOverlayZOrders)(lpdds, dwFlags, lpContext, lpfnCallback);
    OutTrace("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extEnumOverlayZOrders1(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpfnCallback) {
    return extEnumOverlayZOrders(1, pEnumOverlayZOrders1, lpdds, dwFlags, lpContext, lpfnCallback);
}
HRESULT WINAPI extEnumOverlayZOrders2(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpfnCallback) {
    return extEnumOverlayZOrders(2, pEnumOverlayZOrders2, lpdds, dwFlags, lpContext, lpfnCallback);
}
HRESULT WINAPI extEnumOverlayZOrders3(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpfnCallback) {
    return extEnumOverlayZOrders(3, pEnumOverlayZOrders3, lpdds, dwFlags, lpContext, lpfnCallback);
}
HRESULT WINAPI extEnumOverlayZOrders4(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpfnCallback) {
    return extEnumOverlayZOrders(4, pEnumOverlayZOrders4, lpdds, dwFlags, lpContext, lpfnCallback);
}
HRESULT WINAPI extEnumOverlayZOrders7(LPDIRECTDRAWSURFACE lpdds, DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpfnCallback) {
    return extEnumOverlayZOrders(7, pEnumOverlayZOrders7, lpdds, dwFlags, lpContext, lpfnCallback);
}

HRESULT WINAPI extAddOverlayDirtyRect(int dxversion, AddOverlayDirtyRect_Type pAddOverlayDirtyRect, LPDIRECTDRAWSURFACE lpdds, LPRECT lpRect) {
    HRESULT res;
    ApiName("AddOverlayDirtyRect");
    char sInfo[128];
    if (lpRect)
        sprintf(sInfo, "(%d,%d)-(%d,%d)", lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
    else
        sprintf(sInfo, "(NULL)");
    OutTraceDDRAW("%s(%d): lpdds=%#x rect=%s\n", ApiRef, dxversion, lpdds, sInfo);
    res = (*pAddOverlayDirtyRect)(lpdds, lpRect);
#ifndef DXW_NOTRACES
    if(res) OutTraceDDRAW("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
#endif // DXW_NOTRACES
    return res;
}

HRESULT WINAPI extAddOverlayDirtyRect1(LPDIRECTDRAWSURFACE lpdds, LPRECT lpRect) {
    return extAddOverlayDirtyRect(1, pAddOverlayDirtyRect1, lpdds, lpRect);
}
HRESULT WINAPI extAddOverlayDirtyRect2(LPDIRECTDRAWSURFACE lpdds, LPRECT lpRect) {
    return extAddOverlayDirtyRect(2, pAddOverlayDirtyRect2, lpdds, lpRect);
}
HRESULT WINAPI extAddOverlayDirtyRect3(LPDIRECTDRAWSURFACE lpdds, LPRECT lpRect) {
    return extAddOverlayDirtyRect(3, pAddOverlayDirtyRect3, lpdds, lpRect);
}
HRESULT WINAPI extAddOverlayDirtyRect4(LPDIRECTDRAWSURFACE lpdds, LPRECT lpRect) {
    return extAddOverlayDirtyRect(4, pAddOverlayDirtyRect4, lpdds, lpRect);
}
HRESULT WINAPI extAddOverlayDirtyRect7(LPDIRECTDRAWSURFACE lpdds, LPRECT lpRect) {
    return extAddOverlayDirtyRect(7, pAddOverlayDirtyRect7, lpdds, lpRect);
}

static HRESULT WINAPI extCompact(int dxversion, Compact_Type pCompact, LPDIRECTDRAW lpdd) {
    HRESULT res = DD_OK;
    if(pCompact) res = (*pCompact)(lpdd);
    OutTraceDDRAW("Compact(%d): lpdd=%#x res=%#x(%s)\n", dxversion, lpdd, res, ExplainDDError(res));
    // The method is currently unimplemented, always return DD_OK. Fixes "Hyper Crazy Climber"
    return DD_OK;
}

HRESULT WINAPI extCompact1(LPDIRECTDRAW lpdd) {
    return extCompact(1, pCompact1, lpdd);
}
HRESULT WINAPI extCompact2(LPDIRECTDRAW lpdd) {
    return extCompact(2, pCompact2, lpdd);
}
HRESULT WINAPI extCompact3(LPDIRECTDRAW lpdd) {
    return extCompact(3, pCompact3, lpdd);
}
HRESULT WINAPI extCompact4(LPDIRECTDRAW lpdd) {
    return extCompact(4, pCompact4, lpdd);
}
HRESULT WINAPI extCompact7(LPDIRECTDRAW lpdd) {
    return extCompact(7, pCompact7, lpdd);
}

HRESULT WINAPI extDuplicateSurface(int dxversion, DuplicateSurface_Type pDuplicateSurface, LPDIRECTDRAW lpdd, LPDIRECTDRAWSURFACE lpddssrc, LPDIRECTDRAWSURFACE FAR *lpddsdest) {
    HRESULT res;
    ApiName("DuplicateSurface");
    OutTrace("%s(%d): lpdd=%#x lpddsrc=%#x\n", ApiRef, dxversion, lpdd, lpddssrc);
    res = (*pDuplicateSurface)(lpdd, lpddssrc, lpddsdest);
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else
        OutTraceDDRAW("%s: lpddsdest=%#x\n", ApiRef, *lpddsdest);
    return res;
}

HRESULT WINAPI extDuplicateSurface1(LPDIRECTDRAW lpdd, LPDIRECTDRAWSURFACE lpddssrc, LPDIRECTDRAWSURFACE FAR *lpddsdest) {
    return extDuplicateSurface(1, pDuplicateSurface1, lpdd, lpddssrc, lpddsdest);
}
HRESULT WINAPI extDuplicateSurface2(LPDIRECTDRAW lpdd, LPDIRECTDRAWSURFACE lpddssrc, LPDIRECTDRAWSURFACE FAR *lpddsdest) {
    return extDuplicateSurface(2, pDuplicateSurface2, lpdd, lpddssrc, lpddsdest);
}
HRESULT WINAPI extDuplicateSurface3(LPDIRECTDRAW lpdd, LPDIRECTDRAWSURFACE lpddssrc, LPDIRECTDRAWSURFACE FAR *lpddsdest) {
    return extDuplicateSurface(3, pDuplicateSurface3, lpdd, lpddssrc, lpddsdest);
}
HRESULT WINAPI extDuplicateSurface4(LPDIRECTDRAW lpdd, LPDIRECTDRAWSURFACE4 lpddssrc, LPDIRECTDRAWSURFACE4 FAR *lpddsdest) {
    return extDuplicateSurface(4, (DuplicateSurface_Type)pDuplicateSurface4, lpdd, (LPDIRECTDRAWSURFACE)lpddssrc, (LPDIRECTDRAWSURFACE *)lpddsdest);
}
HRESULT WINAPI extDuplicateSurface7(LPDIRECTDRAW lpdd, LPDIRECTDRAWSURFACE7 lpddssrc, LPDIRECTDRAWSURFACE7 FAR *lpddsdest) {
    return extDuplicateSurface(7, (DuplicateSurface_Type)pDuplicateSurface7, lpdd, (LPDIRECTDRAWSURFACE)lpddssrc, (LPDIRECTDRAWSURFACE *)lpddsdest);
}

static HRESULT WINAPI extSetOverlayPosition(int dxversion, SetOverlayPosition_Type pSetOverlayPosition, LPDIRECTDRAWSURFACE lpdds, LONG lX, LONG lY) {
    HRESULT res;
    ApiName("SetOverlayPosition");
    OutTraceDDRAW("%s(%d): lpdds=%#x pos=(%ld,%ld)\n", ApiRef, dxversion, lpdds, lX, lY);
    res = (*pSetOverlayPosition)(lpdds, lX, lY);
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extSetOverlayPosition1(LPDIRECTDRAWSURFACE lpdds, LONG lX, LONG lY) {
    return extSetOverlayPosition(1, pSetOverlayPosition1, lpdds, lX, lY);
}
HRESULT WINAPI extSetOverlayPosition2(LPDIRECTDRAWSURFACE lpdds, LONG lX, LONG lY) {
    return extSetOverlayPosition(2, pSetOverlayPosition2, lpdds, lX, lY);
}
HRESULT WINAPI extSetOverlayPosition3(LPDIRECTDRAWSURFACE lpdds, LONG lX, LONG lY) {
    return extSetOverlayPosition(3, pSetOverlayPosition3, lpdds, lX, lY);
}
HRESULT WINAPI extSetOverlayPosition4(LPDIRECTDRAWSURFACE lpdds, LONG lX, LONG lY) {
    return extSetOverlayPosition(4, pSetOverlayPosition4, lpdds, lX, lY);
}
HRESULT WINAPI extSetOverlayPosition7(LPDIRECTDRAWSURFACE lpdds, LONG lX, LONG lY) {
    return extSetOverlayPosition(7, pSetOverlayPosition7, lpdds, lX, lY);
}

static HRESULT WINAPI extGetOverlayPosition(int dxversion, GetOverlayPosition_Type pGetOverlayPosition, LPDIRECTDRAWSURFACE lpdds, LPLONG lX, LPLONG lY) {
    HRESULT res;
    ApiName("GetOverlayPosition");
    OutTraceDDRAW("%s(%d): lpdds=%#x\n", ApiRef, dxversion, lpdds);
    res = (*pGetOverlayPosition)(lpdds, lX, lY);
#ifndef DXW_NOTRACES
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else
        OutTraceDDRAW("%s: pos=(%ld,%ld)\n", ApiRef, *lX, *lY);
#endif // DXW_NOTRACES
    return res;
}

HRESULT WINAPI extGetOverlayPosition1(LPDIRECTDRAWSURFACE lpdds, LPLONG lX, LPLONG lY) {
    return extGetOverlayPosition(1, pGetOverlayPosition1, lpdds, lX, lY);
}
HRESULT WINAPI extGetOverlayPosition2(LPDIRECTDRAWSURFACE lpdds, LPLONG lX, LPLONG lY) {
    return extGetOverlayPosition(2, pGetOverlayPosition2, lpdds, lX, lY);
}
HRESULT WINAPI extGetOverlayPosition3(LPDIRECTDRAWSURFACE lpdds, LPLONG lX, LPLONG lY) {
    return extGetOverlayPosition(3, pGetOverlayPosition3, lpdds, lX, lY);
}
HRESULT WINAPI extGetOverlayPosition4(LPDIRECTDRAWSURFACE lpdds, LPLONG lX, LPLONG lY) {
    return extGetOverlayPosition(4, pGetOverlayPosition4, lpdds, lX, lY);
}
HRESULT WINAPI extGetOverlayPosition7(LPDIRECTDRAWSURFACE lpdds, LPLONG lX, LPLONG lY) {
    return extGetOverlayPosition(7, pGetOverlayPosition7, lpdds, lX, lY);
}

HRESULT WINAPI extStartModeTest(LPDIRECTDRAW lpdds, LPSIZE lpModesToTest, DWORD dwNumEntries, DWORD dwFlags) {
    HRESULT res;
#ifndef DXW_NOTRACES
    if(IsTraceDDRAW) {
        char sBuf[80];
        OutTrace("StartModeTest(7): lpdds=%#x flags=%#x num=%d modes=", lpdds, dwFlags, dwNumEntries);
        for(DWORD i = 0; i < dwNumEntries; i++) {
            sprintf(sBuf, "(%d x %d)", lpModesToTest[i].cx, lpModesToTest[i].cy);
            OutTrace(sBuf);
        }
    }
#endif // DXW_NOTRACES
    res = (*pStartModeTest)(lpdds, lpModesToTest, dwNumEntries, dwFlags);
    OutTraceDDRAW("StartModeTest: res=%#x\n", res);
    return res;
}

HRESULT WINAPI extEvaluateMode(LPDIRECTDRAW lpdds, DWORD dwFlags, DWORD *pSecondsUntilTimeout) {
    HRESULT res;
    OutTraceDDRAW("EvaluateMode(7): lpdds=%#x flags=%#x\n", lpdds, dwFlags);
    res = (*pEvaluateMode)(lpdds, dwFlags, pSecondsUntilTimeout);
    OutTraceDDRAW("EvaluateMode: res=%#x timeout=%d\n", res, *pSecondsUntilTimeout);
    return res;
}

HRESULT WINAPI extDDGetSurfaceFromDC(HDC hdc, LPDIRECTDRAWSURFACE *lplpdds) {
    HRESULT res;
    ApiName("GetSurfaceFromDC");
    OutTraceDDRAW("%s: hdc=%#x\n", ApiRef, hdc);
    res = (*pDDGetSurfaceFromDC)(hdc, lplpdds);
#ifndef DXW_NOTRACES
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else
        OutTraceDDRAW("%s: lpdds=%#x\n", ApiRef, *lplpdds);
#endif // DXW_NOTRACES
    return res;
}

HRESULT WINAPI extGetSurfaceFromDC(int dxversion, GetSurfaceFromDC_Type pGetSurfaceFromDC, LPDIRECTDRAW lpdd, HDC hdc, LPDIRECTDRAWSURFACE4 *lplpdds) {
    HRESULT res;
    ApiName("GetSurfaceFromDC");
    OutTraceDDRAW("%s(%d): lpdd=%#x hdc=%#x\n", ApiRef, dxversion, lpdd, hdc);
    res = (*pGetSurfaceFromDC)(lpdd, hdc, lplpdds);
#ifndef DXW_NOTRACES
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else
        OutTraceDDRAW("%s: lpdds=%#x\n", ApiRef, *lplpdds);
#endif // DXW_NOTRACES
    return res;
}

HRESULT WINAPI extGetSurfaceFromDC4(LPDIRECTDRAW lpdd, HDC hdc, LPDIRECTDRAWSURFACE4 *lplpdds) {
    return extGetSurfaceFromDC(4, pGetSurfaceFromDC4, lpdd, hdc, lplpdds);
}
HRESULT WINAPI extGetSurfaceFromDC7(LPDIRECTDRAW lpdd, HDC hdc, LPDIRECTDRAWSURFACE4 *lplpdds) {
    return extGetSurfaceFromDC(7, pGetSurfaceFromDC7, lpdd, hdc, lplpdds);
}

HRESULT WINAPI extEnumSurfaces7(LPDIRECTDRAW lpdd, DWORD dwFlags, LPDDSURFACEDESC2 lpddsd, LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpEnumSurfaceCallback) {
    HRESULT res;
    OutTraceDDRAW("EnumSurfaces(7): flags=%#x\n", dwFlags);
    //MessageBox(NULL, "EnumSurfaces7", "", 0);
    res = (*pEnumSurfaces7)(lpdd, dwFlags, lpddsd, lpContext, lpEnumSurfaceCallback);
    return res;
}

HRESULT WINAPI extEnumSurfaces4(LPDIRECTDRAW lpdd, DWORD dwFlags, LPDDSURFACEDESC2 lpddsd, LPVOID lpContext, LPDDENUMSURFACESCALLBACK2 lpEnumSurfaceCallback) {
    HRESULT res;
    OutTraceDDRAW("EnumSurfaces(4): flags=%#x\n", dwFlags);
    //MessageBox(NULL, "EnumSurfaces4", "", 0);
    res = (*pEnumSurfaces4)(lpdd, dwFlags, lpddsd, lpContext, lpEnumSurfaceCallback);
    return res;
}

HRESULT WINAPI extEnumSurfaces(int dxversion, EnumSurfaces1_Type pEnumSurfaces, LPDIRECTDRAW lpdd, DWORD dwFlags, LPDDSURFACEDESC lpddsd, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfaceCallback) {
    HRESULT res;
    OutTraceDDRAW("EnumSurfaces(%d): flags=%#x\n", dxversion, dwFlags);
    //MessageBox(NULL, "EnumSurfaces", "", 0);
    res = (*pEnumSurfaces)(lpdd, dwFlags, lpddsd, lpContext, lpEnumSurfaceCallback);
    return res;
}

HRESULT WINAPI extEnumSurfaces1(LPDIRECTDRAW lpdd, DWORD dwFlags, LPDDSURFACEDESC lpddsd, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfaceCallback) {
    return extEnumSurfaces(1, pEnumSurfaces1, lpdd, dwFlags, lpddsd, lpContext, lpEnumSurfaceCallback);
}
HRESULT WINAPI extEnumSurfaces2(LPDIRECTDRAW lpdd, DWORD dwFlags, LPDDSURFACEDESC lpddsd, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfaceCallback) {
    return extEnumSurfaces(2, pEnumSurfaces2, lpdd, dwFlags, lpddsd, lpContext, lpEnumSurfaceCallback);
}
HRESULT WINAPI extEnumSurfaces3(LPDIRECTDRAW lpdd, DWORD dwFlags, LPDDSURFACEDESC lpddsd, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfaceCallback) {
    return extEnumSurfaces(3, pEnumSurfaces3, lpdd, dwFlags, lpddsd, lpContext, lpEnumSurfaceCallback);
}

#ifdef TRACEALL
HRESULT WINAPI extGetMonitorFrequency(int dxversion, GetMonitorFrequency_Type pGetMonitorFrequency, LPDIRECTDRAW lpdd, LPDWORD freq) {
    HRESULT res;
    ApiName("GetMonitorFrequency");
    OutTraceDDRAW("%s(%d): lpdd=%#x\n", ApiRef, dxversion, lpdd);
    //MessageBox(NULL, "EnumSurfaces", "", 0);
    res = (*pGetMonitorFrequency)(lpdd, freq);
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else
        OutTraceDDRAW("%s: freq=%d\n", ApiRef, *freq);
    return res;
}

HRESULT WINAPI extGetMonitorFrequency1(LPDIRECTDRAW lpdd, LPDWORD freq) {
    return extGetMonitorFrequency(1, pGetMonitorFrequency1, lpdd, freq);
}
HRESULT WINAPI extGetMonitorFrequency2(LPDIRECTDRAW lpdd, LPDWORD freq) {
    return extGetMonitorFrequency(2, pGetMonitorFrequency2, lpdd, freq);
}
HRESULT WINAPI extGetMonitorFrequency3(LPDIRECTDRAW lpdd, LPDWORD freq) {
    return extGetMonitorFrequency(3, pGetMonitorFrequency3, lpdd, freq);
}
HRESULT WINAPI extGetMonitorFrequency4(LPDIRECTDRAW lpdd, LPDWORD freq) {
    return extGetMonitorFrequency(4, pGetMonitorFrequency4, lpdd, freq);
}
HRESULT WINAPI extGetMonitorFrequency7(LPDIRECTDRAW lpdd, LPDWORD freq) {
    return extGetMonitorFrequency(7, pGetMonitorFrequency7, lpdd, freq);
}

HRESULT WINAPI extGetVerticalBlankStatus(int dxversion, GetVerticalBlankStatus_Type pGetVerticalBlankStatus, LPDIRECTDRAW lpdd, LPBOOL status) {
    HRESULT res;
    ApiName("GetVerticalBlankStatus");
    OutTraceDDRAW("%s(%d): lpdd=%#x\n", ApiRef, dxversion, lpdd);
    //MessageBox(NULL, "EnumSurfaces", "", 0);
    res = (*pGetVerticalBlankStatus)(lpdd, status);
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else
        OutTraceDDRAW("%s: status=%d\n", ApiRef, *status);
    return res;
}

HRESULT WINAPI extGetVerticalBlankStatus1(LPDIRECTDRAW lpdd, LPBOOL status) {
    return extGetVerticalBlankStatus(1, pGetVerticalBlankStatus1, lpdd, status);
}
HRESULT WINAPI extGetVerticalBlankStatus2(LPDIRECTDRAW lpdd, LPBOOL status) {
    return extGetVerticalBlankStatus(2, pGetVerticalBlankStatus2, lpdd, status);
}
HRESULT WINAPI extGetVerticalBlankStatus3(LPDIRECTDRAW lpdd, LPBOOL status) {
    return extGetVerticalBlankStatus(3, pGetVerticalBlankStatus3, lpdd, status);
}
HRESULT WINAPI extGetVerticalBlankStatus4(LPDIRECTDRAW lpdd, LPBOOL status) {
    return extGetVerticalBlankStatus(4, pGetVerticalBlankStatus4, lpdd, status);
}
HRESULT WINAPI extGetVerticalBlankStatus7(LPDIRECTDRAW lpdd, LPBOOL status) {
    return extGetVerticalBlankStatus(7, pGetVerticalBlankStatus7, lpdd, status);
}

HRESULT WINAPI extRestoreAllSurfaces(char *api, RestoreAllSurfaces_Type pRestoreAllSurfaces, LPDIRECTDRAW lpdd) {
    HRESULT res;
    OutTraceDDRAW("%s: lpdd=%#x\n", api, lpdd);
    res = (*pRestoreAllSurfaces)(lpdd);
    OutTraceDDRAW("%s: res=%#x(%s)\n", api, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extRestoreAllSurfaces4(LPDIRECTDRAW lpdd) {
    return extRestoreAllSurfaces("RestoreAllSurfaces(4)", pRestoreAllSurfaces4, lpdd);
}
HRESULT WINAPI extRestoreAllSurfaces7(LPDIRECTDRAW lpdd) {
    return extRestoreAllSurfaces("RestoreAllSurfaces(7)", pRestoreAllSurfaces7, lpdd);
}

HRESULT WINAPI extGetDeviceIdentifier(char *api, GetDeviceIdentifier_Type pGetDeviceIdentifier, LPDIRECTDRAW lpdd, LPDDDEVICEIDENTIFIER lpdid, DWORD dwFlags) {
    HRESULT res;
    OutTraceDDRAW("%s: lpdd=%#x flags=%#x\n", api, lpdd, dwFlags);
    res = (*pGetDeviceIdentifier)(lpdd, lpdid, dwFlags);
    // traces here ...
    OutTraceDDRAW("%s: res=%#x(%s)\n", api, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extGetDeviceIdentifier4(LPDIRECTDRAW lpdd, LPDDDEVICEIDENTIFIER lpdid, DWORD dwFlags) {
    return extGetDeviceIdentifier("GetDeviceIdentifier(4)", pGetDeviceIdentifier4, lpdd, lpdid, dwFlags);
}
HRESULT WINAPI extGetDeviceIdentifier7(LPDIRECTDRAW lpdd, LPDDDEVICEIDENTIFIER lpdid, DWORD dwFlags) {
    return extGetDeviceIdentifier("GetDeviceIdentifier(7)", pGetDeviceIdentifier7, lpdd, lpdid, dwFlags);
}
#endif
