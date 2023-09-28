#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include "dxdiag.h"
#include "ddraw.h"
#include "proxy.h"

// DirectDraw COM methods
typedef ULONG	(WINAPI *AddRefD_Type)(LPDIRECTDRAW);
typedef ULONG	(WINAPI *ReleaseD_Type)(LPDIRECTDRAW);
typedef HRESULT (WINAPI *QueryInterface_Type)(void *, REFIID, LPVOID *);
typedef ULONG	(WINAPI *Compact_Type)(LPDIRECTDRAW);
typedef HRESULT (WINAPI *CreateClipper_Type)(LPDIRECTDRAW, DWORD, LPDIRECTDRAWCLIPPER FAR *, IUnknown FAR *);
typedef HRESULT (WINAPI *CreatePalette_Type)(LPDIRECTDRAW, DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown *);
typedef HRESULT (WINAPI *SetCooperativeLevel_Type)(LPDIRECTDRAW, HWND, DWORD);
typedef HRESULT (WINAPI *CreateSurface1_Type)(LPDIRECTDRAW, DDSURFACEDESC *, LPDIRECTDRAWSURFACE *, void *);
typedef HRESULT (WINAPI *CreateSurface4_Type)(LPDIRECTDRAW, DDSURFACEDESC2 *, LPDIRECTDRAWSURFACE4 *, void *);
typedef HRESULT (WINAPI *CreateSurface7_Type)(LPDIRECTDRAW, DDSURFACEDESC2 *, LPDIRECTDRAWSURFACE7 *, void *);
typedef HRESULT (WINAPI *DuplicateSurface_Type)(LPDIRECTDRAW, LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE *);
typedef HRESULT (WINAPI *EnumDisplayModes1_Type)(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK);
typedef HRESULT (WINAPI *EnumDisplayModes4_Type)(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2);
typedef HRESULT (WINAPI *EnumSurfaces1_Type)(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMSURFACESCALLBACK);
typedef HRESULT (WINAPI *EnumSurfaces4_Type)(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMSURFACESCALLBACK2);
typedef HRESULT (WINAPI *FlipToGDISurface_Type)(LPDIRECTDRAW);
typedef HRESULT (WINAPI *GetDisplayMode_Type)(LPDIRECTDRAW, LPDDSURFACEDESC);

AddRefD_Type pAddRefD1, pAddRefD2, pAddRefD3, pAddRefD4, pAddRefD7;
ReleaseD_Type pReleaseD1, pReleaseD2, pReleaseD3, pReleaseD4, pReleaseD7;
QueryInterface_Type pQueryInterfaceD = NULL;
Compact_Type pCompact = NULL;
CreateClipper_Type pCreateClipper = NULL;
CreatePalette_Type pCreatePalette = NULL;
SetCooperativeLevel_Type pSetCooperativeLevel1, pSetCooperativeLevel2, pSetCooperativeLevel3, pSetCooperativeLevel4, pSetCooperativeLevel7;
CreateSurface1_Type pCreateSurface1 = NULL;
CreateSurface1_Type pCreateSurface2 = NULL;
CreateSurface1_Type pCreateSurface3 = NULL;
CreateSurface4_Type pCreateSurface4 = NULL;
CreateSurface7_Type pCreateSurface7 = NULL;
DuplicateSurface_Type pDuplicateSurface = NULL;
FlipToGDISurface_Type pFlipToGDISurface = NULL;
GetDisplayMode_Type pGetDisplayMode = NULL;

// DirectDraw redirectors
ULONG	WINAPI extAddRefD1(LPDIRECTDRAW);
ULONG	WINAPI extAddRefD2(LPDIRECTDRAW);
ULONG	WINAPI extAddRefD3(LPDIRECTDRAW);
ULONG	WINAPI extAddRefD4(LPDIRECTDRAW);
ULONG	WINAPI extAddRefD7(LPDIRECTDRAW);
ULONG	WINAPI extReleaseD1(LPDIRECTDRAW);
ULONG	WINAPI extReleaseD2(LPDIRECTDRAW);
ULONG	WINAPI extReleaseD3(LPDIRECTDRAW);
ULONG	WINAPI extReleaseD4(LPDIRECTDRAW);
ULONG	WINAPI extReleaseD7(LPDIRECTDRAW);
HRESULT WINAPI extQueryInterfaceD(void *, REFIID, LPVOID *);
ULONG	WINAPI extCompact(LPDIRECTDRAW);
HRESULT WINAPI extCreateClipper(LPDIRECTDRAW, DWORD, LPDIRECTDRAWCLIPPER FAR *, IUnknown FAR *);
HRESULT WINAPI extCreatePalette(LPDIRECTDRAW, DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown *);
HRESULT WINAPI extSetCooperativeLevel1(LPDIRECTDRAW, HWND, DWORD);
HRESULT WINAPI extSetCooperativeLevel2(LPDIRECTDRAW, HWND, DWORD);
HRESULT WINAPI extSetCooperativeLevel3(LPDIRECTDRAW, HWND, DWORD);
HRESULT WINAPI extSetCooperativeLevel4(LPDIRECTDRAW, HWND, DWORD);
HRESULT WINAPI extSetCooperativeLevel7(LPDIRECTDRAW, HWND, DWORD);
HRESULT WINAPI extCreateSurface1(LPDIRECTDRAW, DDSURFACEDESC *, LPDIRECTDRAWSURFACE *, void *);
HRESULT WINAPI extCreateSurface2(LPDIRECTDRAW, DDSURFACEDESC *, LPDIRECTDRAWSURFACE *, void *);
HRESULT WINAPI extCreateSurface3(LPDIRECTDRAW, DDSURFACEDESC *, LPDIRECTDRAWSURFACE *, void *);
HRESULT WINAPI extCreateSurface4(LPDIRECTDRAW, DDSURFACEDESC2 *, LPDIRECTDRAWSURFACE4 *, void *);
HRESULT WINAPI extCreateSurface7(LPDIRECTDRAW, DDSURFACEDESC2 *, LPDIRECTDRAWSURFACE7 *, void *);
HRESULT WINAPI extDuplicateSurface(LPDIRECTDRAW, LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extFlipToGDISurface(LPDIRECTDRAW);
HRESULT WINAPI extGetDisplayMode(LPDIRECTDRAW, LPDDSURFACEDESC);
HRESULT WINAPI extGetGDISurface(LPDIRECTDRAW, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extEnumDisplayModes1(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK);
HRESULT WINAPI extEnumDisplayModes4(LPDIRECTDRAW, DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2);
HRESULT WINAPI extInitialize(LPDIRECTDRAW, FAR GUID *);
HRESULT WINAPI extSetDisplayMode1(LPDIRECTDRAW, DWORD, DWORD, DWORD);
HRESULT WINAPI extSetDisplayMode2(LPDIRECTDRAW, DWORD, DWORD, DWORD, DWORD, DWORD);
HRESULT WINAPI extWaitForVerticalBlank(LPDIRECTDRAW, DWORD, HANDLE);

/*
	// IDIrectDraw::EnumDisplayModes
	switch(dxVersion) {
	case 1:
	case 2:
		SetHook((void *)(**(DWORD **)lplpdd + 32), extEnumDisplayModes1Proxy, (void **)&pEnumDisplayModes1, "EnumDisplayModes(D1)");
		break;
	case 4:
	case 7:
		SetHook((void *)(**(DWORD **)lplpdd + 32), extEnumDisplayModes4Proxy, (void **)&pEnumDisplayModes4, "EnumDisplayModes(D4)");
		break;
	}
	// IDIrectDraw::EnumSurfaces
	if (dxVersion < 4)
		SetHook((void *)(**(DWORD **)lplpdd + 36), extEnumSurfacesProxy1, (void **)&pEnumSurfaces1, "EnumSurfaces(D1)");
	else
		SetHook((void *)(**(DWORD **)lplpdd + 36), extEnumSurfacesProxy4, (void **)&pEnumSurfaces4, "EnumSurfaces(D4)");
	// IDIrectDraw::GetCaps
	SetHook((void *)(**(DWORD **)lplpdd + 44), extGetCapsD, (void **)&pGetCapsD, "GetCaps(D)");
	// IDIrectDraw::GetFourCCCodes
	SetHook((void *)(**(DWORD **)lplpdd + 52), extGetFourCCCodesProxy, (void **)&pGetFourCCCodes, "GetFourCCCodes(D)");
	// IDIrectDraw::GetGDISurface
	SetHook((void *)(**(DWORD **)lplpdd + 56), extGetGDISurfaceProxy, (void **)&pGetGDISurface, "GetGDISurface(D)");
	// IDIrectDraw::GetMonitorFrequency
	SetHook((void *)(**(DWORD **)lplpdd + 60), extGetMonitorFrequencyProxy, (void **)&pGetMonitorFrequency, "GetMonitorFrequency(D)");
	// IDIrectDraw::GetScanLine
	SetHook((void *)(**(DWORD **)lplpdd + 64), extGetScanLineProxy, (void **)&pGetScanLine, "GetScanLine(D)");
	// IDIrectDraw::GetVerticalBlankStatus
	SetHook((void *)(**(DWORD **)lplpdd + 68), extGetVerticalBlankStatusProxy, (void **)&pGetVerticalBlankStatus, "GetVerticalBlankStatus(D)");
	// IDIrectDraw::Initialize
	SetHook((void *)(**(DWORD **)lplpdd + 72), extInitializeProxy, (void **)&pInitialize, "Initialize(D)");
	// IDIrectDraw::RestoreDisplayMode
	SetHook((void *)(**(DWORD **)lplpdd + 76), extRestoreDisplayModeProxy, (void **)&pRestoreDisplayMode, "RestoreDisplayMode(D)");
	if (dxVersion >= 2){
		// IDIrectDraw::GetAvailableVidMem
		SetHook((void *)(**(DWORD **)lplpdd + 92), extGetAvailableVidMemProxy, (void **)&pGetAvailableVidMem, "GetAvailableVidMem(D)");
	}
	if (dxVersion >= 4){
		// IDIrectDraw::GetSurfaceFromDC
		SetHook((void *)(**(DWORD **)lplpdd + 96), extGetSurfaceFromDCProxy, (void **)&pGetSurfaceFromDC, "GetSurfaceFromDC(D)");
		// IDIrectDraw::RestoreAllSurfaces
		SetHook((void *)(**(DWORD **)lplpdd + 100), extRestoreAllSurfacesProxy, (void **)&pRestoreAllSurfaces, "RestoreAllSurfaces(D)");
		// IDIrectDraw::GetDeviceIdentifier
		SetHook((void *)(**(DWORD **)lplpdd + 108), extGetDeviceIdentifierProxy, (void **)&pGetDeviceIdentifier, "GetDeviceIdentifier(D)");
	}
	// IDIrectDraw::SetCooperativeLevel
	SetHook((void *)(**(DWORD **)lplpdd + 80), extSetCooperativeLevelProxy, (void **)&pSetCooperativeLevel, "SetCooperativeLevel(D)");
	// IDIrectDraw::SetDisplayMode
	if (dxVersion > 1){
		SetHook((void *)(**(DWORD **)lplpdd + 84), extSetDisplayMode2Proxy, (void **)&pSetDisplayMode2, "SetDisplayMode(D2)");
	}
	else {
		SetHook((void *)(**(DWORD **)lplpdd + 84), extSetDisplayMode1Proxy, (void **)&pSetDisplayMode1, "SetDisplayMode(D1)");
	}
    #if 0
	// IDIrectDraw::WaitForVerticalBlank
	SetHook((void *)(**(DWORD **)lplpdd + 88), extWaitForVerticalBlankProxy, (void **)&pWaitForVerticalBlank, "WaitForVerticalBlank(D)");
    #endif
	if (dxVersion >= 4){
		// IDIrectDraw::TestCooperativeLevel
		SetHook((void *)(**(DWORD **)lplpdd + 104), extTestCooperativeLevelProxy, (void **)&pTestCooperativeLevel, "TestCooperativeLevel(D)");
	}
    }
*/

void HookDDSession(LPDIRECTDRAW *lplpdd, int dxversion) {
    // IDIrectDraw::QueryInterface
    if(bLogAll || bLogQueryInterface)
        SetHook((void *)(**(DWORD **)lplpdd + 0), extQueryInterfaceD, (void **)&pQueryInterfaceD, "QueryInterface(D)");
#ifdef COMPLETETRACE
    // IDIrectDraw::Compact
    //SetHook((void *)(**(DWORD **)lplpdd + 12), extCompact, (void **)&pCompact, "Compact(D)");
    // IDIrectDraw::CreateClipper
    SetHook((void *)(**(DWORD **)lplpdd + 16), extCreateClipper, (void **)&pCreateClipper, "CreateClipper(D)");
    // IDIrectDraw::CreatePalette
    //SetHook((void *)(**(DWORD **)lplpdd + 20), extCreatePalette, (void **)&pCreatePalette, "CreatePalette(D)");
#endif
    if(bLogRelease) {
        switch(dxversion) {
        case 1:
            SetHook((void *)(**(DWORD **)lplpdd + 4), extAddRefD1, (void **)&pAddRefD1, "AddRef(D1)");
            SetHook((void *)(**(DWORD **)lplpdd + 8), extReleaseD1, (void **)&pReleaseD1, "Release(D1)");
            break;
        case 2:
            SetHook((void *)(**(DWORD **)lplpdd + 4), extAddRefD2, (void **)&pAddRefD2, "AddRef(D2)");
            SetHook((void *)(**(DWORD **)lplpdd + 8), extReleaseD2, (void **)&pReleaseD2, "Release(D2)");
            break;
        case 3:
            SetHook((void *)(**(DWORD **)lplpdd + 4), extAddRefD3, (void **)&pAddRefD3, "AddRef(D3)");
            SetHook((void *)(**(DWORD **)lplpdd + 8), extReleaseD3, (void **)&pReleaseD3, "Release(D3)");
            break;
        case 4:
            SetHook((void *)(**(DWORD **)lplpdd + 4), extAddRefD4, (void **)&pAddRefD4, "AddRef(D4)");
            SetHook((void *)(**(DWORD **)lplpdd + 8), extReleaseD4, (void **)&pReleaseD4, "Release(D4)");
            break;
        case 7:
            SetHook((void *)(**(DWORD **)lplpdd + 4), extAddRefD7, (void **)&pAddRefD7, "AddRef(D7)");
            SetHook((void *)(**(DWORD **)lplpdd + 8), extReleaseD7, (void **)&pReleaseD7, "Release(D7)");
            break;
        default:
            trace("*** unknown interface ***\n");
            break;
        }
    }
    switch(dxversion) {
    case 1:
        SetHook((void *)(**(DWORD **)lplpdd + 24), extCreateSurface1, (void **)&pCreateSurface1, "CreateSurface(S1)");
        SetHook((void *)(**(DWORD **)lplpdd + 80), extSetCooperativeLevel1, (void **)&pSetCooperativeLevel1, "SetCooperativeLevel(D1)");
        break;
    case 2:
        SetHook((void *)(**(DWORD **)lplpdd + 24), extCreateSurface2, (void **)&pCreateSurface2, "CreateSurface(S2)");
        SetHook((void *)(**(DWORD **)lplpdd + 80), extSetCooperativeLevel2, (void **)&pSetCooperativeLevel2, "SetCooperativeLevel(D2)");
        break;
    case 3:
        SetHook((void *)(**(DWORD **)lplpdd + 24), extCreateSurface3, (void **)&pCreateSurface3, "CreateSurface(S3)");
        SetHook((void *)(**(DWORD **)lplpdd + 80), extSetCooperativeLevel3, (void **)&pSetCooperativeLevel3, "SetCooperativeLevel(D3)");
        break;
    case 4:
        SetHook((void *)(**(DWORD **)lplpdd + 24), extCreateSurface4, (void **)&pCreateSurface4, "CreateSurface(S4)");
        SetHook((void *)(**(DWORD **)lplpdd + 80), extSetCooperativeLevel4, (void **)&pSetCooperativeLevel4, "SetCooperativeLevel(D4)");
        break;
    case 7:
        SetHook((void *)(**(DWORD **)lplpdd + 24), extCreateSurface7, (void **)&pCreateSurface7, "CreateSurface(S7)");
        SetHook((void *)(**(DWORD **)lplpdd + 80), extSetCooperativeLevel7, (void **)&pSetCooperativeLevel7, "SetCooperativeLevel(D7)");
        break;
    default:
        // unknown, no hook!!
        break;
    }
#ifdef COMPLETETRACE
    // IDIrectDraw::DuplicateSurface
    SetHook((void *)(**(DWORD **)lplpdd + 28), extDuplicateSurface, (void **)&pDuplicateSurface, "DuplicateSurface(D)");
    // IDIrectDraw::FlipToGDISurface
    SetHook((void *)(**(DWORD **)lplpdd + 40), extFlipToGDISurface, (void **)&pFlipToGDISurface, "FlipToGDISurface(D)");
    // IDIrectDraw::GetDisplayMode
    SetHook((void *)(**(DWORD **)lplpdd + 48), extGetDisplayMode, (void **)&pGetDisplayMode, "GetDisplayMode(D)");
#endif
    // IDIrectDraw::SetCooperativeLevel
}


HRESULT WINAPI extQueryInterfaceD(void *lpdd, REFIID riid, LPVOID *obp) {
    HRESULT res;
    int dwLocalDDVersion;
    trace(">> DirectDraw::QueryInterface(lpdd=%x refiid=%s)\n", lpdd, sGUID((GUID *)&riid));
    res = (*pQueryInterfaceD)(lpdd, riid, obp);
    trace("<< DirectDraw::QueryInterface(obp=%x) res=%x(%s)\n", *obp, res, ExplainDDError(res));
    dwLocalDDVersion = GUIDDDrawVersion((GUID *)&riid, NULL);
    HookDDSession((LPDIRECTDRAW *)obp, dwLocalDDVersion);
    return res;
}

static ULONG WINAPI extAddRefD(int dxversion, AddRefD_Type pAddRefD, LPDIRECTDRAW lpdd) {
    ULONG res;
    trace(">> DirectDraw(%d)::AddRef(lpdds=%x)\n", dxversion, lpdd);
    res = (*pAddRefD)(lpdd);
    trace("<< DirectDraw(%d)::AddRef res(refcount)=%x\n", dxversion, res);
    return res;
}

ULONG WINAPI extAddRefD1(LPDIRECTDRAW lpdd) {
    return extAddRefD(1, pAddRefD1, lpdd);
}
ULONG WINAPI extAddRefD2(LPDIRECTDRAW lpdd) {
    return extAddRefD(2, pAddRefD2, lpdd);
}
ULONG WINAPI extAddRefD3(LPDIRECTDRAW lpdd) {
    return extAddRefD(3, pAddRefD3, lpdd);
}
ULONG WINAPI extAddRefD4(LPDIRECTDRAW lpdd) {
    return extAddRefD(4, pAddRefD4, lpdd);
}
ULONG WINAPI extAddRefD7(LPDIRECTDRAW lpdd) {
    return extAddRefD(7, pAddRefD7, lpdd);
}

static ULONG WINAPI extReleaseD(int dxversion, ReleaseD_Type pReleaseD, LPDIRECTDRAW lpdd) {
    HRESULT res;
    trace(">> DirectDraw(%d)::Release(lpdds=%x)\n", dxversion, lpdd);
    res = (*pReleaseD)(lpdd);
    trace("<< DirectDraw(%d)::Release res(refcount)=%x\n", dxversion, res);
    return res;
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

ULONG WINAPI extCompact(LPDIRECTDRAW lpdd) {
    ULONG res;
    trace(">> DirectDraw::Compact(lpdds=%x)\n", lpdd);
    res = (*pCompact)(lpdd);
    trace("<< DirectDraw::Compact res(refcount)=%x\n", res);
    return res;
}

HRESULT WINAPI extCreateClipper(LPDIRECTDRAW lpdd, DWORD dwflags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter) {
    HRESULT res;
    trace(">> DirectDraw::CreateClipper(lpdd=%x flags=%x)\n", lpdd, dwflags);
    res = (*pCreateClipper)(lpdd, dwflags, lplpDDClipper, pUnkOuter);
    trace("<< DirectDraw::CreateClipper(clipper=%x) res=%x(%s)\n", *lplpDDClipper, res, ExplainDDError(res));
    if(!res) HookDDClipper(lplpDDClipper);
    return res;
}

#define DDPCAPS_INITIALIZE_LEGACY 0x00000008l
char *ExplainCreatePaletteFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DDPCAPS_");
    if (c & DDPCAPS_4BIT) strcat(eb, "4BIT+");
    if (c & DDPCAPS_8BITENTRIES) strcat(eb, "8BITENTRIES+");
    if (c & DDPCAPS_8BIT) strcat(eb, "8BIT+");
    //if (c & DDPCAPS_INITIALIZE) strcat(eb, "INITIALIZE+");
    // DDPCAPS_INITIALIZE is obsolete and redefined to 0x0, but that is not the legacy value embedded in assembly!
    if (c & DDPCAPS_INITIALIZE_LEGACY) strcat(eb, "INITIALIZE+");
    if (c & DDPCAPS_PRIMARYSURFACE) strcat(eb, "PRIMARYSURFACE+");
    if (c & DDPCAPS_PRIMARYSURFACELEFT) strcat(eb, "PRIMARYSURFACELEFT+");
    if (c & DDPCAPS_ALLOW256) strcat(eb, "ALLOW256+");
    if (c & DDPCAPS_VSYNC) strcat(eb, "VSYNC+");
    if (c & DDPCAPS_1BIT) strcat(eb, "1BIT+");
    if (c & DDPCAPS_2BIT) strcat(eb, "2BIT+");
    if (c & DDPCAPS_ALPHA) strcat(eb, "ALPHA+");
    l = strlen(eb);
    if (l > strlen("DDPCAPS_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

HRESULT WINAPI extCreatePalette(LPDIRECTDRAW lpdd, DWORD dwflags, LPPALETTEENTRY lpddpa, LPDIRECTDRAWPALETTE *lplpddp, IUnknown *pu) {
    HRESULT res;
    trace(">> DirectDraw::CreatePalette(lpdd=%x flags=%x(%s))\n", lpdd, dwflags, ExplainCreatePaletteFlags(dwflags));
    res = (*pCreatePalette)(lpdd, dwflags, lpddpa, lplpddp, pu);
    trace("<< DirectDraw::CreatePalette(palette=%x) res=%x(%s)\n", *lplpddp, res, ExplainDDError(res));
    if(!res) HookDDPalette(lplpddp);
    return res;
}

char *ExplainCoopFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DDSCL_");
    if (c & DDSCL_FULLSCREEN) strcat(eb, "FULLSCREEN+");
    if (c & DDSCL_ALLOWREBOOT) strcat(eb, "ALLOWREBOOT+");
    if (c & DDSCL_NOWINDOWCHANGES) strcat(eb, "NOWINDOWCHANGES+");
    if (c & DDSCL_NORMAL) strcat(eb, "NORMAL+");
    if (c & DDSCL_EXCLUSIVE) strcat(eb, "EXCLUSIVE+");
    if (c & DDSCL_ALLOWMODEX) strcat(eb, "ALLOWMODEX+");
    if (c & DDSCL_SETFOCUSWINDOW) strcat(eb, "SETFOCUSWINDOW+");
    if (c & DDSCL_SETDEVICEWINDOW) strcat(eb, "SETDEVICEWINDOW+");
    if (c & DDSCL_CREATEDEVICEWINDOW) strcat(eb, "CREATEDEVICEWINDOW+");
    if (c & DDSCL_MULTITHREADED) strcat(eb, "MULTITHREADED+");
    if (c & DDSCL_FPUSETUP) strcat(eb, "FPUSETUP+");
    if (c & DDSCL_FPUPRESERVE) strcat(eb, "FPUPRESERVE+");
    l = strlen(eb);
    if (l > strlen("DDSCL_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

static HRESULT WINAPI extSetCooperativeLevel(int dxversion, SetCooperativeLevel_Type pSetCooperativeLevel,
        LPDIRECTDRAW lpdd, HWND hwnd, DWORD dwflags) {
    HRESULT res;
    trace(">> DirectDraw(%d)::SetCooperativeLevel(lpdds=%x hwnd=%x flags=%x(%s))\n",
          dxversion, lpdd, hwnd, dwflags, ExplainCoopFlags(dwflags));
    res = (*pSetCooperativeLevel)(lpdd, hwnd, dwflags);
    trace("<< DirectDraw(%d)::SetCooperativeLevel res=%x(%s)\n", dxversion, res, ExplainDDError(res));
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

static HRESULT WINAPI extCreateSurface(int dxversion, CreateSurface1_Type pCreateSurface,
                                       LPDIRECTDRAW lpdd, DDSURFACEDESC2 *lpddsd, LPDIRECTDRAWSURFACE7 *lplpdds, void *x) {
    HRESULT res;
    trace(">> DirectDraw(%d)::CreateSurface(lpdd=%x ", dxversion, lpdd);
    LogSurfaceAttributes(TraceLog, (LPDDSURFACEDESC)lpddsd);
    trace(")\n");
    switch(dxversion) {
    case 1:
    case 2:
    case 3:
        res = (*pCreateSurface)(lpdd, (DDSURFACEDESC *)lpddsd, (LPDIRECTDRAWSURFACE *)lplpdds, x);
        break;
    case 4:
        res = (*(CreateSurface4_Type)pCreateSurface)(lpdd, (DDSURFACEDESC2 *)lpddsd, (LPDIRECTDRAWSURFACE4 *)lplpdds, x);
        break;
    case 7:
        res = (*(CreateSurface7_Type)pCreateSurface)(lpdd, (DDSURFACEDESC2 *)lpddsd, (LPDIRECTDRAWSURFACE7 *)lplpdds, x);
        break;
    }
    if (!res) {
        HookSurface((LPDIRECTDRAWSURFACE *)lplpdds, dxversion);
        dumpreff(*lplpdds);
    }
    trace("<< DirectDraw(%d)::CreateSurface(lpdds=%x) res=%x(%s)\n", dxversion, *lplpdds, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extCreateSurface1(LPDIRECTDRAW lpdd, DDSURFACEDESC *lpddsd, LPDIRECTDRAWSURFACE *lplpdds, void *x) {
    return extCreateSurface(1, pCreateSurface1, lpdd, (DDSURFACEDESC2 *)lpddsd, (LPDIRECTDRAWSURFACE7 *)lplpdds, x);
}
HRESULT WINAPI extCreateSurface2(LPDIRECTDRAW lpdd, DDSURFACEDESC *lpddsd, LPDIRECTDRAWSURFACE *lplpdds, void *x) {
    return extCreateSurface(2, pCreateSurface2, lpdd, (DDSURFACEDESC2 *)lpddsd, (LPDIRECTDRAWSURFACE7 *)lplpdds, x);
}
HRESULT WINAPI extCreateSurface3(LPDIRECTDRAW lpdd, DDSURFACEDESC *lpddsd, LPDIRECTDRAWSURFACE *lplpdds, void *x) {
    return extCreateSurface(3, pCreateSurface3, lpdd, (DDSURFACEDESC2 *)lpddsd, (LPDIRECTDRAWSURFACE7 *)lplpdds, x);
}
HRESULT WINAPI extCreateSurface4(LPDIRECTDRAW lpdd, DDSURFACEDESC2 *lpddsd, LPDIRECTDRAWSURFACE4 *lplpdds, void *x) {
    return extCreateSurface(4, (CreateSurface1_Type)pCreateSurface4, lpdd, (DDSURFACEDESC2 *)lpddsd, (LPDIRECTDRAWSURFACE7 *)lplpdds, x);
}
HRESULT WINAPI extCreateSurface7(LPDIRECTDRAW lpdd, DDSURFACEDESC2 *lpddsd, LPDIRECTDRAWSURFACE7 *lplpdds, void *x) {
    return extCreateSurface(7, (CreateSurface1_Type)pCreateSurface7, lpdd, (DDSURFACEDESC2 *)lpddsd, (LPDIRECTDRAWSURFACE7 *)lplpdds, x);
}

HRESULT WINAPI extDuplicateSurface(LPDIRECTDRAW lpdd, LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE *lplpdds) {
    HRESULT res;
    trace(">> DirectDraw::DuplicateSurface(lpdd=%x lpdds=%x)\n", lpdd, lpdds);
    res = (*pDuplicateSurface)(lpdd, lpdds, lplpdds);
    trace("<< DirectDraw::DuplicateSurface(lplpdds=%x) res=%x(%s)\n", *lplpdds, res, ExplainDDError(res));
    //if (!res) HookSurface((LPDIRECTDRAWSURFACE *)lplpdds, 7);
    return res;
}

HRESULT WINAPI extFlipToGDISurface(LPDIRECTDRAW lpdd) {
    HRESULT res;
    trace(">> DirectDraw::FlipToGDISurface(lpdd=%x)\n", lpdd);
    res = (*pFlipToGDISurface)(lpdd);
    trace("<< DirectDraw::FlipToGDISurface res=%x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extGetDisplayMode(LPDIRECTDRAW lpdd, LPDDSURFACEDESC lpddsd) {
    HRESULT res;
    trace(">> DirectDraw::GetDisplayMode(lpdd=%x)\n", lpdd);
    res = (*pGetDisplayMode)(lpdd, lpddsd);
    if(res)
        trace("<< DirectDraw::GetDisplayMode res=%x(%s)\n", res, ExplainDDError(res));
    else {
        trace("<< DirectDraw::GetDisplayMode(");
        LogSurfaceAttributes(TraceLog, (LPDDSURFACEDESC)lpddsd);
        trace(")\n");
    }
    return res;
}
