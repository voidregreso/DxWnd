#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <d3d.h>
#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhook.h"
#include "syslibs.h"
#include "dxhelper.h"
#include "hddraw.h"

extern void HookDDSurface(LPDIRECTDRAWSURFACE *, int, BOOL);
extern void HookGammaControl(LPVOID *);
extern void HookDDSession(LPDIRECTDRAW *, int);
extern void HookDirect3DSession(LPDIRECTDRAW *, int);
extern void HookDirect3DDevice(LPVOID *, int);
extern void HookViewport(LPDIRECT3DVIEWPORT *, int);
extern void HookDDClipper(LPDIRECTDRAWCLIPPER FAR *);

// extQueryInterfaceDX: this is the single procedure that manages all QueryInterface methods within the DirectX classes
// it is better to have it unique because of the transitive and reflexive properties of QueryInterface, that means
// that every DirectX COM object can QueryInterface itself to become another object type belonging to this family.

// some unhandled interfaces in emulation mode:
// REFIID={84e63de0-46aa-11cf-816f-0000c020156e}: IID_IDirect3DHALDevice

typedef enum {
    TYPE_OBJECT_UNKNOWN = 0,
    TYPE_OBJECT_DIRECTDRAW,
    TYPE_OBJECT_DDRAWSURFACE,
    TYPE_OBJECT_CLIPPER,
    TYPE_OBJECT_DIRECT3D,
    TYPE_OBJECT_D3DDEVICE,
    TYPE_OBJECT_GAMMARAMP,
    TYPE_OBJECT_TEXTURE,
    TYPE_OBJECT_VIEWPORT
};

HRESULT WINAPI extQueryInterfaceDX(int dxversion, QueryInterface_Type pQueryInterface, void *lpdds, REFIID riid, LPVOID *obp) {
    HRESULT res;
    ApiName("QueryInterface");
    BOOL IsPrim;
    BOOL IsBack;
    int iObjectType;
    int iObjectVersion;
    extern LPDIRECTDRAWSURFACE lpDDSEmu_Prim;
    DWORD caps;
    IsPrim = dxwss.IsAPrimarySurface((LPDIRECTDRAWSURFACE)lpdds);
    IsBack = dxwss.IsABackBufferSurface((LPDIRECTDRAWSURFACE)lpdds);
    OutTraceDDRAW("%s(%d): lpdds=%#x%s REFIID=%#x(%s)\n",
                  ApiRef, dxversion, lpdds, dxwss.ExplainSurfaceRole((LPDIRECTDRAWSURFACE)lpdds), riid.Data1, ExplainGUID((GUID *)&riid));
    iObjectVersion = 0;
    iObjectType = TYPE_OBJECT_UNKNOWN;
    switch(riid.Data1) {
    // DirectDraw
    case 0xD7B70EE0: // CLSID_DirectDraw
        iObjectType = TYPE_OBJECT_DIRECTDRAW;
        iObjectVersion = 1;
        break;
    case 0x3c305196: // CLSID_DirectDraw7
        iObjectType = TYPE_OBJECT_DIRECTDRAW;
        iObjectVersion = 7;
        break;
    case 0x6C14DB80: // IID_IDirectDraw
        iObjectType = TYPE_OBJECT_DIRECTDRAW;
        iObjectVersion = 1;
        break;
    case 0xB3A6F3E0: // IID_IDirectDraw2
        iObjectType = TYPE_OBJECT_DIRECTDRAW;
        iObjectVersion = 2;
        break;
    case 0x618f8ad4: // IID_IDirectDraw3
        iObjectType = TYPE_OBJECT_DIRECTDRAW;
        iObjectVersion = 3;
        break;
    case 0x9c59509a: // IID_IDirectDraw4
        iObjectType = TYPE_OBJECT_DIRECTDRAW;
        iObjectVersion = 4;
        break;
    case 0x15e65ec0: // IID_IDirectDraw7
        iObjectType = TYPE_OBJECT_DIRECTDRAW;
        iObjectVersion = 7;
        break;
    // DirectDrawSurface
    case 0x6C14DB81:
        iObjectType = TYPE_OBJECT_DDRAWSURFACE;
        iObjectVersion = 1;
        break;
    case 0x57805885:		//DDSurface2 - WIP (Dark Reign)
        iObjectType = TYPE_OBJECT_DDRAWSURFACE;
        iObjectVersion = 2;
        break;
    case 0xDA044E00:
        iObjectType = TYPE_OBJECT_DDRAWSURFACE;
        iObjectVersion = 3;
        break;
    case 0x0B2B8630:
        iObjectType = TYPE_OBJECT_DDRAWSURFACE;
        iObjectVersion = 4;
        break;
    case 0x06675a80:
        iObjectType = TYPE_OBJECT_DDRAWSURFACE;
        iObjectVersion = 7;
        break;
    // DirectDrawSurfaceNew
    case 0x1bab8e96:
        OutTraceDW("%s: IID_IDirectDrawSurfaceNew\n", ApiRef);
        break;
    case 0x6C14DB84:
        OutTraceDW("%s: IID_IDirectDrawPalette\n", ApiRef);
        break;
    // Direct3D
    case 0x3BBA0080: // IID_IDirect3D
        iObjectType = TYPE_OBJECT_DIRECT3D;
        iObjectVersion = 1;
        break;
    case 0x6aae1ec1: // IID_IDirect3D2
        iObjectType = TYPE_OBJECT_DIRECT3D;
        iObjectVersion = 2;
        break;
    case 0xbb223240: // IID_IDirect3D3
        iObjectType = TYPE_OBJECT_DIRECT3D;
        iObjectVersion = 3;
        break;
    case 0xf5049e77: // IID_IDirect3D7
        iObjectType = TYPE_OBJECT_DIRECT3D;
        iObjectVersion = 7;
        break;
    // Direct3DDevice
    case 0x84e63de0: // IID_IDirect3DHALDevice
        iObjectType = TYPE_OBJECT_D3DDEVICE;
        iObjectVersion = 1;
        break;
    case 0xA4665C60: // IID_IDirect3DRGBDevice
        iObjectType = TYPE_OBJECT_D3DDEVICE;
        iObjectVersion = 1;
        break;
    case 0xF2086B20: // IID_IDirect3DRampDevice
        iObjectType = TYPE_OBJECT_D3DDEVICE;
        iObjectVersion = 1;
        break;
    case 0x881949a1: // IID_IDirect3DMMXDevice
        iObjectType = TYPE_OBJECT_D3DDEVICE;
        iObjectVersion = 1;
        break;
    case 0x50936643: // IID_IDirect3DRefDevice
        iObjectType = TYPE_OBJECT_D3DDEVICE;
        iObjectVersion = 2;
        break;
    case 0x8767df22: // IID_IDirect3DNullDevice
        iObjectType = TYPE_OBJECT_D3DDEVICE;
        iObjectVersion = 2;
        break;
    case 0xf5049e78: // IID_IDirect3DTnLHalDevice,
        iObjectType = TYPE_OBJECT_D3DDEVICE;
        iObjectVersion = 3;
        break;
    case 0x64108800: // IID_IDirect3DDevice
        iObjectType = TYPE_OBJECT_D3DDEVICE;
        iObjectVersion = 1;
        break;
    case 0x93281501: // IID_IDirect3DDevice2
        iObjectType = TYPE_OBJECT_D3DDEVICE;
        iObjectVersion = 2;
        break;
    case 0xb0ab3b60: // IID_IDirect3DDevice3
        iObjectType = TYPE_OBJECT_D3DDEVICE;
        iObjectVersion = 3;
        break;
    case 0xf5049e79: // IID_IDirect3DDevice7
        iObjectType = TYPE_OBJECT_D3DDEVICE;
        iObjectVersion = 7;
        break;
    case 0x4B9F0EE0:
        OutTraceDW("%s: IID_IDirectDrawColorControl\n", ApiRef);
        break;
    case 0x6C14DB85:
        OutTraceDW("%s: IID_IDirectDrawClipper\n", ApiRef);
        break;
    case 0x69C11C3E:
        iObjectType = TYPE_OBJECT_GAMMARAMP;
        iObjectVersion = 1;
        break;
    // textures
    case 0x2CDCD9E0:
        iObjectType = TYPE_OBJECT_TEXTURE;
        iObjectVersion = 1;
        break;
    case 0x93281502:
        iObjectType = TYPE_OBJECT_TEXTURE;
        iObjectVersion = 2;
        break;
    case 0x4417C146: //IID_IDirect3DViewport
        iObjectType = TYPE_OBJECT_VIEWPORT;
        iObjectVersion = 1;
        break;
    case 0x93281500: //IID_IDirect3DViewport2
        iObjectType = TYPE_OBJECT_VIEWPORT;
        iObjectVersion = 2;
        break;
    case 0xb0ab3b61: //IID_IDirect3DViewport3
        iObjectType = TYPE_OBJECT_VIEWPORT;
        iObjectVersion = 3;
        break;
    case 0x593817A0: //CLSID_DirectDrawClipper
        iObjectType = TYPE_OBJECT_CLIPPER;
        iObjectVersion = 1;
        break;
    }
    char *sLabel;
    switch(iObjectType) {
    case TYPE_OBJECT_UNKNOWN:
        sLabel = "(unknown)";
        break;
    case TYPE_OBJECT_DIRECTDRAW:
        sLabel = "IID_IDirectDraw";
        break;
    case TYPE_OBJECT_DDRAWSURFACE:
        sLabel = "IID_IDirectDrawSurface";
        break;
    case TYPE_OBJECT_CLIPPER:
        sLabel = "CLSID_DirectDrawClipper";
        break;
    case TYPE_OBJECT_DIRECT3D:
        sLabel = "IID_IDirect3D";
        break;
    case TYPE_OBJECT_D3DDEVICE:
        sLabel = "IID_IDirect3DDevice";
        break;
    case TYPE_OBJECT_GAMMARAMP:
        sLabel = "IID_IDirectDrawGammaRamp";
        break;
    case TYPE_OBJECT_TEXTURE:
        sLabel = "IID_IDirect3DTexture";
        break;
    case TYPE_OBJECT_VIEWPORT:
        sLabel = "IID_IDirect3DViewport";
        break;
    }
    OutTraceDW("%s: Got interface for %s version %d\n", ApiRef, sLabel, iObjectVersion);
    switch(iObjectType) {
    // simulate unavailable interfaces (useful?)
    case TYPE_OBJECT_DIRECTDRAW:
        if ((dxw.dwFlags7 & LIMITDDRAW) && (iObjectVersion > (int)dxw.MaxDdrawInterface)) {
            *obp = NULL;
            OutTraceDW("%s: DDVersion=%d SUPPRESS lpdds=%#x(%s) REFIID=%#x obp=(NULL) ret=0 at %d\n",
                       ApiRef, iObjectVersion, lpdds, IsPrim ? "" : "(PRIM)", riid.Data1, __LINE__);
            return(E_POINTER);
        }
        break;
    case TYPE_OBJECT_DDRAWSURFACE:
        if ((dxw.dwFlags7 & LIMITDDRAW) && (iObjectVersion > (int)dxw.MaxDdrawInterface)) {
            *obp = NULL;
            OutTraceDW("%s: DDSVersion=%d SUPPRESS lpdds=%#x(%s) REFIID=%#x obp=(NULL) ret=0 at %d\n",
                       ApiRef, iObjectVersion, lpdds, IsPrim ? "" : "(PRIM)", riid.Data1, __LINE__);
            return(E_POINTER);
        }
        // v2.05.58: Lare Release of backbuffer surface. All this only because we didn't understand
        // how to manage the ref counter for the backbuffer of certain 3D games ...
        if (IsBack && (dxw.dwFlags6 & SUPPRESSRELEASE)) {
            if(lpdds == dxw.lpZombieSurface) {
                extern ReleaseS_Type pReleaseSMethod(int);
                OutTraceDDRAW("%s: LATE RELEASE lpdds=%#x ref=0\n", ApiRef, lpdds);
                (*pReleaseSMethod(dxversion))((LPDIRECTDRAWSURFACE)lpdds);
                dxw.lpZombieSurface = NULL;
            }
        }
        break;
    case TYPE_OBJECT_GAMMARAMP:
        // fix the target for gamma ramp creation: if it is a primary surface, use the real one!!
        // v2.03.37: do this just when in surface emulated mode!!
        // v2.05.25: do this only if there's a valid lpDDSEmu_Prim surface - fixes "Runaway" with
        // gdi, opengl, sdl renderers
        if((dxw.IsEmulated) &&
                dxwss.IsAPrimarySurface((LPDIRECTDRAWSURFACE)lpdds) &&
                lpDDSEmu_Prim)
            lpdds = lpDDSEmu_Prim;
        break;
    }
    res = (*pQueryInterface)(lpdds, riid, obp);
    if(res) {
        OutTraceDW("%s: ERROR lpdds=%#x%s REFIID=%#x obp=%#x ret=%#x(%s) at %d\n",
                   ApiRef, lpdds, IsPrim ? "(PRIM)" : "", riid.Data1, *obp, res, ExplainDDError(res), __LINE__);
        return res;
    }
    if (! *obp) {
        OutTraceDW("%s: Interface for object %#x not found\n", ApiRef, riid.Data1);
        return res;
    }
    // added trace
    OutTraceDW("%s: lpdds=%#x REFIID=%#x obp=%#x obj=%s version=%d ret=0\n",
               ApiRef, lpdds, riid.Data1, *obp, sLabel, iObjectVersion);
    // v2.04.19.fx1: Beware! "Need for Speed 5" performs QueryInterface of one sorface on top of itself!
    // This is allowed, but in this case calling dxwss.PopSurface before dxwss.DuplicateSurface would
    // clear the capabilities to be copied over to the (not) new surface.
    // fixed by inserting an "if" statement before PopSurface
    switch(iObjectType) {
    case TYPE_OBJECT_UNKNOWN:
        if(caps = dxwss.DuplicateSurface((LPDIRECTDRAWSURFACE)lpdds, (LPDIRECTDRAWSURFACE) * obp, iObjectVersion))
            OutTraceDW("%s: MOVE caps=%#x(%s)\n", ApiRef, caps, ExplainDDSCaps(caps));
        break;
    case TYPE_OBJECT_DIRECTDRAW:
        HookDDSession((LPDIRECTDRAW *)obp, iObjectVersion);
        break;
    case TYPE_OBJECT_DDRAWSURFACE:
        dxw.dwDDVersion = iObjectVersion;
        if(caps = dxwss.DuplicateSurface((LPDIRECTDRAWSURFACE)lpdds, (LPDIRECTDRAWSURFACE) * obp, iObjectVersion))
            OutTraceDW("%s: MOVE caps=%#x(%s)\n", ApiRef, caps, ExplainDDSCaps(caps));
        if(IsPrim) {
            OutTraceDW("%s(S): primary=%#x new=%#x\n", ApiRef, lpdds, *obp);
            HookDDSurface((LPDIRECTDRAWSURFACE *)obp, dxw.dwDDVersion, TRUE);
        } else {
            // v2.02.13: seems that hooking inconditionally gives troubles. What is the proper safe hook condition?
            HookDDSurface((LPDIRECTDRAWSURFACE *)obp, dxw.dwDDVersion, FALSE);
        }
        break;
    case TYPE_OBJECT_CLIPPER:
        HookDDClipper((LPDIRECTDRAWCLIPPER *)obp); // there is a single Clipper intrface!
        break;
    case TYPE_OBJECT_DIRECT3D:
        HookDirect3DSession((LPDIRECTDRAW *)obp, iObjectVersion);
        break;
    case TYPE_OBJECT_D3DDEVICE:
        HookDirect3DDevice(obp, iObjectVersion);
        break;
    case TYPE_OBJECT_GAMMARAMP:
        HookGammaControl(obp);
        break;
    case TYPE_OBJECT_TEXTURE:
        HookTexture(obp, iObjectVersion);
        break;
    case TYPE_OBJECT_VIEWPORT:
        HookViewport((LPDIRECT3DVIEWPORT *)obp, iObjectVersion);
        break;
    }
    return res;
}
