#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include "dxdiag.h"
#include "ddraw.h"
#include "proxy.h"

// DirectDrawPalette
typedef HRESULT (WINAPI *QueryInterfaceCC_Type)(LPDIRECTDRAWCOLORCONTROL, REFIID, LPVOID *);
typedef ULONG	(WINAPI *AddRefCC_Type)(LPDIRECTDRAWCOLORCONTROL);
typedef ULONG	(WINAPI *ReleaseCC_Type)(LPDIRECTDRAWCOLORCONTROL);
typedef HRESULT (WINAPI *GetColorControls_Type)(LPDIRECTDRAWCOLORCONTROL, LPDDCOLORCONTROL);
typedef HRESULT (WINAPI *SetColorControls_Type)(LPDIRECTDRAWCOLORCONTROL, LPDDCOLORCONTROL);

/* DirectDrawPalette hook pointers */
QueryInterfaceCC_Type pQueryInterfaceCC = NULL;
AddRefCC_Type pAddRefCC = NULL;
ReleaseCC_Type pReleaseCC = NULL;
GetColorControls_Type pGetColorControls = NULL;
SetColorControls_Type pSetColorControls = NULL;

HRESULT WINAPI extQueryInterfaceCC(LPDIRECTDRAWCOLORCONTROL, REFIID, LPVOID *);
ULONG	WINAPI extAddRefCC(LPDIRECTDRAWCOLORCONTROL);
ULONG	WINAPI extReleaseCC(LPDIRECTDRAWCOLORCONTROL);
HRESULT WINAPI extGetColorControls(LPDIRECTDRAWCOLORCONTROL, LPDDCOLORCONTROL);
HRESULT WINAPI extSetColorControls(LPDIRECTDRAWCOLORCONTROL, LPDDCOLORCONTROL);

void HookDDColorControl(LPDIRECTDRAWCOLORCONTROL *lplpddcc) {
    // IDirectDrawPalette::QueryInterface
    SetHook((void *)(**(DWORD **)lplpddcc + 0), extQueryInterfaceCC, (void **)&pQueryInterfaceCC, "QueryInterface(CC)");
    // IDirectDrawPalette::AddRef
    SetHook((void *)(**(DWORD **)lplpddcc + 4), extAddRefCC, (void **)&pAddRefCC, "AddRef(CC)");
    // IDirectDrawPalette::Release
    SetHook((void *)(**(DWORD **)lplpddcc + 8), extReleaseCC, (void **)&pReleaseCC, "Release(CC)");
    // IDirectDrawPalette::GetCaps
    SetHook((void *)(**(DWORD **)lplpddcc + 12), extGetColorControls, (void **)&pGetColorControls, "GetColorControls(CC)");
    // IDirectDrawPalette::GetEntries
    SetHook((void *)(**(DWORD **)lplpddcc + 16), extSetColorControls, (void **)&pSetColorControls, "SetColorControls(CC)");
}

HRESULT WINAPI extQueryInterfaceCC(LPDIRECTDRAWCOLORCONTROL lpddcc, REFIID riid, LPVOID *obp) {
    HRESULT res;
    //int version;
    trace(">> ColorControl::QueryInterface(lpdd=%x refiid=%s)\n", lpddcc, sGUID((GUID *)&riid));
    res = (*pQueryInterfaceCC)(lpddcc, riid, obp);
    trace("<< ColorControl::QueryInterface(obp=%x) res=%x(%s)\n", *obp, res, ExplainDDError(res));
    //version = GUIDClipperVersion((GUID *)&riid, NULL);
    //HookClipSession((LPDIRECTDRAW *)obp, dwLocalDDVersion);
    return res;
}

ULONG	WINAPI extAddRefCC(LPDIRECTDRAWCOLORCONTROL lpddcc) {
    ULONG res;
    trace(">> ColorControl::AddRef(lpddp=%x)\n", lpddcc);
    res = (*pAddRefCC)(lpddcc);
    trace("<< ColorControl::AddRef res(refcount)=%x\n", res);
    return res;
}

ULONG	WINAPI extReleaseCC(LPDIRECTDRAWCOLORCONTROL lpddcc) {
    ULONG res;
    trace(">> ColorControl::Release(lpddp=%x)\n", lpddcc);
    res = (*pReleaseCC)(lpddcc);
    trace("<< ColorControl::Release res(refcount)=%x\n", res);
    return res;
}

/*
    typedef struct _DDCOLORCONTROL
    {
    DWORD               dwSize;
    DWORD               dwFlags;
    LONG                lBrightness;
    LONG                lContrast;
    LONG                lHue;
    LONG                lSaturation;
    LONG                lSharpness;
    LONG                lGamma;
    LONG                lColorEnable;
    DWORD               dwReserved1;
    } DDCOLORCONTROL;
*/

HRESULT WINAPI extGetColorControls(LPDIRECTDRAWCOLORCONTROL lpddcc, LPDDCOLORCONTROL lpcc) {
    HRESULT res;
    trace(">> ColorControl::GetColorControls(lpddcc=%x)\n", lpddcc);
    res = (*pGetColorControls)(lpddcc, lpcc);
    trace("<< ColorControl::GetColorControls("
          "lpcc=(size=%d flags=%x brightness=%x contrast=%x hue=%x saturation=%x sharpness=%x gamma=%x colorenable=%x reserved=%x)"
          ") res=%x(%s)\n",
          lpcc->dwSize, lpcc->dwFlags, lpcc->lBrightness, lpcc->lContrast, lpcc->lHue, lpcc->lSaturation, lpcc->lSharpness, lpcc->lGamma, lpcc->lColorEnable, lpcc->dwReserved1,
          res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extSetColorControls(LPDIRECTDRAWCOLORCONTROL lpddcc, LPDDCOLORCONTROL lpcc) {
    HRESULT res;
    trace(">> ColorControl::SetColorControls(lpddcc=%x "
          "lpcc=(size=%d flags=%x brightness=%x contrast=%x hue=%x saturation=%x sharpness=%x gamma=%x colorenable=%x reserved=%x)"
          ")\n",
          lpddcc,
          lpcc->dwSize, lpcc->dwFlags, lpcc->lBrightness, lpcc->lContrast, lpcc->lHue, lpcc->lSaturation, lpcc->lSharpness, lpcc->lGamma, lpcc->lColorEnable, lpcc->dwReserved1
         );
    res = (*pSetColorControls)(lpddcc, lpcc);
    trace("<< ColorControl::SetColorControls res=%x(%s)\n", res, ExplainDDError(res));
    return res;
}
