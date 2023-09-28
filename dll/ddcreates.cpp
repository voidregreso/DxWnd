#define _CRT_SECURE_NO_WARNINGS
#define INITGUID
//#define FULLHEXDUMP

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

#define MAXBACKBUFFERS 4

extern void SetPixFmt(LPDDSURFACEDESC2);
extern void GetPixFmt(LPDDSURFACEDESC2);
extern CHAR *LogSurfaceAttributes(LPDDSURFACEDESC2, char *, int);
extern void HookDDSurface(LPDIRECTDRAWSURFACE *, int,  BOOL);
extern void RegisterPixelFormat(int, LPDIRECTDRAWSURFACE);
extern void SetVSyncDelays(int, LPDIRECTDRAW);
extern char *DumpPixelFormat(LPDDSURFACEDESC2);
extern void FixSurfaceCaps(LPDDSURFACEDESC2, int);
extern SetPalette_Type pSetPaletteMethod(int);
extern ReleaseS_Type pReleaseSMethod(int);
extern CreatePalette_Type pCreatePaletteMethod(int);
extern GetGDISurface_Type pGetGDISurfaceMethod(int);

extern int iPrimarySurfaceVersion;
extern LPDIRECTDRAWSURFACE lpDDSEmu_Prim;
extern LPDIRECTDRAWSURFACE lpDDSEmu_Back;
extern LPDIRECTDRAWSURFACE lpDDZBuffer;
extern DDSURFACEDESC2 DDSD_Prim;
extern LPDIRECTDRAWPALETTE lpDDP;
extern LPDIRECTDRAWCLIPPER lpddC;
extern LPDIRECTDRAW lpPrimaryDD;
extern int iDDPExtraRefCounter;
extern DWORD dwBackBufferCaps;
extern BOOL bFlippedDC;
extern PALETTEENTRY DefaultSystemPalette[];
extern GetGDISurface_Type pGetGDISurface1, pGetGDISurface2, pGetGDISurface3, pGetGDISurface4, pGetGDISurface7;
extern GetAttachedSurface_Type pGetAttachedSurface1, pGetAttachedSurface2, pGetAttachedSurface3, pGetAttachedSurface4, pGetAttachedSurface7;
extern CreateSurface1_Type pCreateSurface1, pCreateSurface2, pCreateSurface3;
extern CreateSurface2_Type pCreateSurface4, pCreateSurface7;
extern ReleaseS_Type pReleaseS1, pReleaseS2, pReleaseS3, pReleaseS4, pReleaseS7;
extern GetPixelFormat_Type pGetPixelFormat1, pGetPixelFormat2, pGetPixelFormat3, pGetPixelFormat4, pGetPixelFormat7;

void InitDSScreenParameters(int dxversion, LPDIRECTDRAWSURFACE lpdds) {
    HRESULT res;
    DDPIXELFORMAT p;
    DDSURFACEDESC2 ddsd;
    GetPixelFormat_Type pGetPixelFormat;
    switch(dxversion) {
    case 1:
        pGetPixelFormat = pGetPixelFormat1;
        break;
    case 2:
        pGetPixelFormat = pGetPixelFormat2;
        break;
    case 3:
        pGetPixelFormat = pGetPixelFormat3;
        break;
    case 4:
        pGetPixelFormat = pGetPixelFormat4;
        break;
    case 7:
        pGetPixelFormat = pGetPixelFormat7;
        break;
    }
    //OutTrace("dxversion=%d pGetPixelFormat=%#x\n", dxversion, pGetPixelFormat);
    p.dwSize = sizeof(DDPIXELFORMAT);
    if(res = (*pGetPixelFormat)(lpdds, &p)) {
        OutTraceE("GetPixelFormat: ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        return;
    }
    ddsd.ddpfPixelFormat = p;
    OutTraceDW("InitDSScreenParameters: version=%d Actual %s\n", dxversion, DumpPixelFormat(&ddsd));
    dxw.ActualPixelFormat = p;
    SetBltTransformations();
    return;
}

static void ClearSurfaceDesc(void *ddsd, int dxversion) {
    int size;
    size = (dxversion < 4) ? sizeof(DDSURFACEDESC) : sizeof(DDSURFACEDESC2);
    memset(ddsd, 0, size); // Clean all
    ((LPDDSURFACEDESC)ddsd)->dwSize = size;
}

void BuildRealSurfaces(LPDIRECTDRAW lpdd, CreateSurface_Type pCreateSurface, int dxversion) {
    HRESULT res;
    DDSURFACEDESC2 ddsd;
    OutTraceDW("BuildRealSurfaces: lpdd=%#x pCreateSurface=%#x version=%d\n", lpdd, pCreateSurface, dxversion);
    if(lpDDSEmu_Prim == NULL) {
        ClearSurfaceDesc((void *)&ddsd, dxversion);
        ddsd.dwFlags = DDSD_CAPS;
        // try DDSCAPS_SYSTEMMEMORY first, then suppress it if not supported
        // no, DDSCAPS_SYSTEMMEMORY cause screen flickering while moving the window (and other troubles?)
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
        OutTraceDW("BuildRealSurfaces: %s\n", LogSurfaceAttributes(&ddsd, "[EmuPrim]", __LINE__));
        res = (*pCreateSurface)(lpdd, &ddsd, &lpDDSEmu_Prim, 0);
        if(res == DDERR_PRIMARYSURFACEALREADYEXISTS) {
            OutTraceDW("BuildRealSurfaces: ASSERT DDSEmu_Prim already exists\n");
            if(dxw.Windowize) {
                // in Windowize mode, the desktop properties are untouched, then the current primary surface can be recycled
                res = (*pGetGDISurfaceMethod(dxversion))(lpdd, &lpDDSEmu_Prim);
            } else {
                // in non-Windowized mode, the primary surface must be released and rebuilt with the proper properties
                res = (*pGetGDISurfaceMethod(dxversion))(lpdd, &lpDDSEmu_Prim);
                if (lpDDSEmu_Prim) while((*pReleaseSMethod(dxversion))(lpDDSEmu_Prim));
                res = (*pCreateSurface)(lpdd, &ddsd, &lpDDSEmu_Prim, 0);
            }
        }
        if(res) {
#ifndef DXW_NOTRACES
            OutTraceE("BuildRealSurfaces: CreateSurface ERROR on DDSEmu_Prim res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
            if(res == DDERR_INVALIDPIXELFORMAT) DumpPixFmt(&ddsd);
#endif // DXW_NOTRACES
            return;
        }
#ifndef DXW_NOTRACES
        OutTraceDW("BuildRealSurfaces: created new DDSEmu_Prim=%#x\n", lpDDSEmu_Prim);
        if(IsDebugDW) DescribeSurface(lpDDSEmu_Prim, dxversion, "DDSEmu_Prim", __LINE__);
#endif // DXW_NOTRACES
        InitDSScreenParameters(dxversion, lpDDSEmu_Prim);
        dxwss.PopSurface(lpDDSEmu_Prim);
        // v2.04.88: don't set a clipper against the NULL real desktop - this may happen when creating a ddraw
        // session/surface before than the main window. Ref. "Reader Rabbit Math Ages 6 to 9"
        if ((dxw.dwFlags3 & FORCECLIPPER) && dxw.GethWnd()) {
            OutTraceDW("BuildRealSurfaces: FORCE SetClipper on primary hwnd=%#x lpdds=%#x\n", dxw.GethWnd(), lpDDSEmu_Prim);
            res = lpdd->CreateClipper(0, &lpddC, NULL);
#ifndef DXW_NOTRACES
            if (res) OutTraceE("BuildRealSurfaces: CreateClipper ERROR res=%#x(%s)\n", res, ExplainDDError(res));
#endif // DXW_NOTRACES
            res = lpddC->SetHWnd(0, dxw.GethWnd());
#ifndef DXW_NOTRACES
            if (res) OutTraceE("BuildRealSurfaces: SetHWnd ERROR res=%#x(%s)\n", res, ExplainDDError(res));
#endif // DXW_NOTRACES
            res = lpDDSEmu_Prim->SetClipper(lpddC);
#ifndef DXW_NOTRACES
            if (res) OutTraceE("BuildRealSurfaces: SetClipper ERROR res=%#x(%s)\n", res, ExplainDDError(res));
#endif // DXW_NOTRACES
        }
        // can't hook lpDDSEmu_Prim as generic, since the Flip method is unimplemented for a PRIMARY surface!
        // better avoid it or hook just useful methods.
        //if (dxw.dwTFlags & OUTPROXYTRACE) HookDDSurfaceGeneric(&lpDDSEmu_Prim, dxw.dwDDVersion);
    }
    if(lpDDSEmu_Back == NULL) {
        ClearSurfaceDesc((void *)&ddsd, dxversion);
        ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
        ddsd.ddsCaps.dwCaps = dwBackBufferCaps;
        ddsd.dwWidth = dxw.GetScreenWidth();
        ddsd.dwHeight = dxw.GetScreenHeight();
        // scale backbuffer size
        if(dxw.FilterXScalingFactor) ddsd.dwWidth *= dxw.FilterXScalingFactor;
        if(dxw.FilterYScalingFactor) ddsd.dwHeight *= dxw.FilterYScalingFactor;
        OutTraceDW("BuildRealSurfaces: %s\n", LogSurfaceAttributes(&ddsd, "[EmuBack]", __LINE__));
        res = (*pCreateSurface)(lpdd, &ddsd, &lpDDSEmu_Back, 0);
        if(res) {
            ddsd.ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
            OutTraceDW("BuildRealSurfaces: %s\n", LogSurfaceAttributes(&ddsd, "[EmuBack]", __LINE__));
            res = (*pCreateSurface)(lpdd, &ddsd, &lpDDSEmu_Back, 0);
        }
        if(res) {
#ifndef DXW_NOTRACES
            OutTraceE("BuildRealSurfaces: CreateSurface ERROR on DDSEmuBack : res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
            if(res == DDERR_INVALIDPIXELFORMAT) DumpPixFmt(&ddsd);
#endif // DXW_NOTRACES
            return;
        }
#ifndef DXW_NOTRACES
        OutTraceDW("BuildRealSurfaces: created new DDSEmu_Back=%#x\n", lpDDSEmu_Back);
        if(IsDebugDW) DescribeSurface(lpDDSEmu_Back, dxversion, "DDSEmu_Back", __LINE__);
#endif // DXW_NOTRACES
        dxwss.PopSurface(lpDDSEmu_Back);
        //if (dxw.dwTFlags & OUTPROXYTRACE) HookDDSurfaceGeneric(&lpDDSEmu_Back, dxversion);
    }
}

void ReleaseRealSurfaces(int dxversion) {
    OutTraceDW("ReleaseRealSurfaces\n");
    ReleaseS_Type pReleaseS;
    switch(dxversion) {
    default:
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
    while ((*pReleaseS)(lpDDSEmu_Prim));
    while ((*pReleaseS)(lpDDSEmu_Back));
    lpDDSEmu_Prim = NULL;
    lpDDSEmu_Back = NULL;
}

void RestoreDDrawSurfaces() {
    // if it's a ddraw game ....
    if(lpPrimaryDD) {
        OutTraceDW("RestoreDDrawSurfaces: rebuilding surfaces for dd session %#x\n", lpPrimaryDD);
        ReleaseS_Type pReleaseS;
        CreateSurface_Type pCreateSurface;
        switch(iPrimarySurfaceVersion) {
        case 1:
            pCreateSurface = (CreateSurface_Type)pCreateSurface1;
            pReleaseS = pReleaseS1;
            break;
        case 2:
            pCreateSurface = (CreateSurface_Type)pCreateSurface2;
            pReleaseS = pReleaseS2;
            break;
        case 3:
            pCreateSurface = (CreateSurface_Type)pCreateSurface3;
            pReleaseS = pReleaseS3;
            break;
        case 4:
            pCreateSurface = (CreateSurface_Type)pCreateSurface4;
            pReleaseS = pReleaseS4;
            break;
        case 7:
            pCreateSurface = (CreateSurface_Type)pCreateSurface7;
            pReleaseS = pReleaseS7;
            break;
        }
        if(lpDDSEmu_Back) while((*pReleaseS)(lpDDSEmu_Back));
        lpDDSEmu_Back = NULL;
        if(lpDDSEmu_Prim) while((*pReleaseS)(lpDDSEmu_Prim));
        lpDDSEmu_Prim = NULL;
        BuildRealSurfaces(lpPrimaryDD, pCreateSurface, iPrimarySurfaceVersion);
    }
}

static HRESULT BuildPrimaryEmu(LPDIRECTDRAW lpdd, CreateSurface_Type pCreateSurface, LPDDSURFACEDESC2 lpddsd, int dxversion, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    DDSURFACEDESC2 ddsd;
    HRESULT res;
    OutTraceDW("BuildPrimaryEmu: lpdd=%#x pCreateSurface=%#x lpddsd=%#x version=%d\n", lpdd, pCreateSurface, lpddsd, dxversion);
    // emulated primary surface
    memcpy((void *)&ddsd, lpddsd, lpddsd->dwSize);
    // handle the surface attributes before the ddsd.dwFlags gets updated:
    // if a surface desc is NOT specified, build one
    if(!(ddsd.dwFlags & DDSD_PIXELFORMAT)) SetPixFmt((LPDDSURFACEDESC2)&ddsd);
    // then save it
    dxw.VirtualPixelFormat = ddsd.ddpfPixelFormat;
    OutTraceDW("BuildPrimaryEmu: DDSD_PIXELFORMAT %s\n", ExplainPixelFormat(&(ddsd.ddpfPixelFormat)));
    ddsd.dwFlags &= ~(DDSD_BACKBUFFERCOUNT | DDSD_REFRESHRATE);
    ddsd.dwFlags |= (DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT);
    ddsd.ddsCaps.dwCaps &= ~(DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
    // DDSCAPS_OFFSCREENPLAIN seems required to support the palette in memory surfaces
    ddsd.ddsCaps.dwCaps |= (DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY);
    // on WinXP Fifa 99 doesn't like DDSCAPS_SYSTEMMEMORY cap, so better to leave a way to unset it....
    if(dxw.dwFlags6 & NOSYSMEMPRIMARY) ddsd.ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth = dxw.GetScreenWidth();
    ddsd.dwHeight = dxw.GetScreenHeight();
    if(dxversion >= 4) {
        ddsd.ddsCaps.dwCaps2 = 0;
        ddsd.ddsCaps.dwCaps3 = 0;
        ddsd.ddsCaps.dwCaps4 = 0;
    }
    // create Primary surface
    OutTraceDW("BuildPrimaryEmu: %s\n", LogSurfaceAttributes(&ddsd, "[Primary]", __LINE__));
    res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, 0);
    if(res) {
#ifndef DXW_NOTRACES
        OutTraceE("BuildPrimaryEmu: CreateSurface ERROR on DDSPrim res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        if(res == DDERR_INVALIDPIXELFORMAT) DumpPixFmt(&ddsd);
        if(res == DDERR_INVALIDCAPS) DumpCaps(&ddsd);
#endif // DXW_NOTRACES
        return res;
    }
    iPrimarySurfaceVersion = dxversion; // v2.03.01
#ifndef DXW_NOTRACES
    OutTraceDW("BuildPrimaryEmu: created PRIMARY DDSPrim=%#x\n", *lplpdds);
    if(IsDebugDW) DescribeSurface(*lplpdds, dxversion, "DDSPrim", __LINE__);
#endif // DXW_NOTRACES
    HookDDSurface(lplpdds, dxversion, TRUE);
    // "Hoyle Casino Empire" opens a primary surface and NOT a backbuffer ....
    // build a default System palette and apply it to primary surface
    if((ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) &&
            (dxw.dwFlags6 & SYNCPALETTE)) {
        if(lpDDP == NULL) {
            res = (*pCreatePaletteMethod(dxversion))(lpdd, DDPCAPS_8BIT | DDPCAPS_ALLOW256, DefaultSystemPalette, &lpDDP, NULL);
#ifndef DXW_NOTRACES
            if(res) OutTraceE("BuildPrimaryEmu: CreatePalette ERROR err=%#x at %d\n", res, __LINE__);
#endif // DXW_NOTRACES
        }
        // this must be done after hooking - who knows why?
        res = (*pSetPaletteMethod(dxversion))(*lplpdds, lpDDP);
        if(res)
            OutTraceE("BuildPrimaryEmu: SetPalette ERROR err=%#x at %d\n", res, __LINE__);
        else iDDPExtraRefCounter = 1;
    }
    // set a global capability value for surfaces that have to blit to primary
    // DDSCAPS_OFFSCREENPLAIN seems required to support the palette in memory surfaces
    // DDSCAPS_SYSTEMMEMORY makes operations faster, but it is not always good...
    dwBackBufferCaps = (DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY);
    // on WinXP Fifa 99 doesn't like DDSCAPS_SYSTEMMEMORY cap, so better to leave a way to unset it....
    // this is important to avoid that certain D3D operations will abort - see "Forsaken" problem
    if(dxw.dwFlags6 & NOSYSMEMBACKBUF) dwBackBufferCaps = DDSCAPS_OFFSCREENPLAIN;
    if(dxw.dwFlags6 & SHAREDDC) bFlippedDC = TRUE;
    if(dxw.Renderer->flags & DXWRF_BUILDREALSURFACES)
        BuildRealSurfaces(lpdd, pCreateSurface, dxversion);
    return DD_OK;
}

static HRESULT BuildPrimaryFlippable(LPDIRECTDRAW lpdd, CreateSurface_Type pCreateSurface, LPDDSURFACEDESC2 lpddsd, int dxversion, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    DDSURFACEDESC2 ddsd;
    HRESULT res;
    OutTraceDW("BuildPrimaryFlippable: lpdd=%#x pCreateSurface=%#x lpddsd=%#x version=%d\n", lpdd, pCreateSurface, lpddsd, dxversion);
    // emulated primary surface
    memcpy((void *)&ddsd, lpddsd, lpddsd->dwSize);
    // handle the surface attributes before the ddsd.dwFlags gets updated:
    // if a surface desc is NOT specified, build one
    if(!(ddsd.dwFlags & DDSD_PIXELFORMAT)) SetPixFmt((LPDDSURFACEDESC2)&ddsd);
    // then save it
    dxw.VirtualPixelFormat = ddsd.ddpfPixelFormat;
    OutTraceDW("BuildPrimaryFlippable: DDSD_PIXELFORMAT %s\n", ExplainPixelFormat(&(ddsd.ddpfPixelFormat)));
    // dwFlags
    ddsd.dwFlags &= ~(DDSD_REFRESHRATE);
    ddsd.dwFlags |= (DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_BACKBUFFERCOUNT);
    // DDSCAPS_OFFSCREENPLAIN seems required to support the palette in memory surfaces
    ddsd.ddsCaps.dwCaps |= (DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY);
    // dwBackBufferCount: set to at least 1
    if(!(lpddsd->dwFlags & DDSD_BACKBUFFERCOUNT) || (lpddsd->dwBackBufferCount == 0)) ddsd.dwBackBufferCount = 1;
    // dwCaps
    ddsd.ddsCaps.dwCaps &= ~(DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM | DDSCAPS_FRONTBUFFER);
    ddsd.ddsCaps.dwCaps |= (DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY);
    // on WinXP Fifa 99 doesn't like DDSCAPS_SYSTEMMEMORY cap, so better to leave a way to unset it....
    if(dxw.dwFlags6 & NOSYSMEMPRIMARY) ddsd.ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
    // dwWidth & dwHeight
    ddsd.dwWidth = dxw.GetScreenWidth();
    ddsd.dwHeight = dxw.GetScreenHeight();
    if(dxversion >= 4) {
        ddsd.ddsCaps.dwCaps2 = 0;
        ddsd.ddsCaps.dwCaps3 = 0;
        ddsd.ddsCaps.dwCaps4 = 0;
    }
    // create Primary surface
    OutTraceDW("BuildPrimaryFlippable: %s\n", LogSurfaceAttributes(&ddsd, "[Primary]", __LINE__));
    res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, 0);
    if(res) {
#ifndef DXW_NOTRACES
        OutTraceE("BuildPrimaryFlippable: CreateSurface ERROR on DDSPrim res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        if(res == DDERR_INVALIDPIXELFORMAT) DumpPixFmt(&ddsd);
#endif // DXW_NOTRACES
        return res;
    }
    iPrimarySurfaceVersion = dxversion; // v2.03.01
#ifndef DXW_NOTRACES
    OutTraceDW("BuildPrimaryFlippable: created PRIMARY DDSPrim=%#x\n", *lplpdds);
    if(IsDebugDW) DescribeSurface(*lplpdds, dxversion, "DDSPrim", __LINE__);
#endif // DXW_NOTRACES
    HookDDSurface(lplpdds, dxversion, TRUE);
    // "Hoyle Casino Empire" opens a primary surface and NOT a backbuffer ....
    // build a default System palette and apply it to primary surface
    if((ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) &&
            (dxw.dwFlags6 & SYNCPALETTE)) {
        if(lpDDP == NULL) {
            res = (*pCreatePaletteMethod(dxversion))(lpdd, DDPCAPS_8BIT | DDPCAPS_ALLOW256, DefaultSystemPalette, &lpDDP, NULL);
#ifndef DXW_NOTRACES
            if(res) OutTraceE("BuildPrimaryFlippable: CreatePalette ERROR err=%#x at %d\n", res, __LINE__);
#endif // DXW_NOTRACES
        }
        // this must be done after hooking - who knows why?
        res = (*pSetPaletteMethod(dxversion))(*lplpdds, lpDDP);
        if(res)
            OutTraceE("BuildPrimaryFlippable: SetPalette ERROR err=%#x at %d\n", res, __LINE__);
        else iDDPExtraRefCounter = 1;
    }
    // set a global capability value for surfaces that have to blit to primary
    dwBackBufferCaps = (DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY);
    // on WinXP Fifa 99 doesn't like DDSCAPS_SYSTEMMEMORY cap, so better to leave a way to unset it....
    // this is important to avoid that certain D3D operations will abort - see "Forsaken" problem
    if(dxw.dwFlags6 & NOSYSMEMBACKBUF) dwBackBufferCaps = DDSCAPS_OFFSCREENPLAIN;
    if(dxw.dwFlags6 & SHAREDDC) bFlippedDC = TRUE;
    if(dxw.Renderer->flags & DXWRF_BUILDREALSURFACES)
        BuildRealSurfaces(lpdd, pCreateSurface, dxversion);
    return DD_OK;
}

static HRESULT BuildPrimaryFullscreen(LPDIRECTDRAW lpdd, CreateSurface_Type pCreateSurface, LPDDSURFACEDESC2 lpddsd, int dxversion, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    DDSURFACEDESC2 ddsd;
    HRESULT res;
    OutTraceDW("BuildPrimaryFullscreen: lpdd=%#x pCreateSurface=%#x lpddsd=%#x version=%d\n", lpdd, pCreateSurface, lpddsd, dxversion);
    // genuine primary surface
    memcpy((void *)&ddsd, lpddsd, lpddsd->dwSize);
    // create Primary surface
    OutTraceDW("BuildPrimaryFullscreen: %s\n", LogSurfaceAttributes(&ddsd, "[Primary]", __LINE__));
    res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, 0);
    if(res) {
        if (res == DDERR_PRIMARYSURFACEALREADYEXISTS) {
            LPDIRECTDRAWSURFACE lpPrim;
            GetGDISurface_Type pGetGDISurface;
            pGetGDISurface = pGetGDISurfaceMethod(dxversion);
            OutTraceE("BuildPrimaryFullscreen: CreateSurface DDERR_PRIMARYSURFACEALREADYEXISTS workaround\n");
            (*pGetGDISurface)(lpPrimaryDD, &lpPrim);
            while ((*pReleaseSMethod(dxversion))(lpPrim));
            res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, 0);
        }
        /* fall through */
        if(res) {
#ifndef DXW_NOTRACES
            OutTraceE("BuildPrimaryFullscreen: CreateSurface ERROR on DDSPrim res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
            if(res == DDERR_INVALIDPIXELFORMAT) DumpPixFmt(&ddsd);
#endif // DXW_NOTRACES
            return res;
        }
    }
#ifndef DXW_NOTRACES
    OutTraceDW("BuildPrimaryFullscreen: created PRIMARY DDSPrim=%#x\n", *lplpdds);
    if(IsDebugDW) DescribeSurface(*lplpdds, dxversion, "DDSPrim", __LINE__);
#endif // DXW_NOTRACES
    iPrimarySurfaceVersion = dxversion;
    HookDDSurface(lplpdds, dxversion, TRUE);
    if(dxw.dwFlags1 & CLIPCURSOR) dxw.SetClipCursor();
    return DD_OK;
}

static HRESULT BuildPrimaryDir(LPDIRECTDRAW lpdd, CreateSurface_Type pCreateSurface, LPDDSURFACEDESC2 lpddsd, int dxversion, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    DDSURFACEDESC2 ddsd;
    HRESULT res;
    OutTraceDW("BuildPrimaryDir: lpdd=%#x pCreateSurface=%#x lpddsd=%#x version=%d\n", lpdd, pCreateSurface, lpddsd, dxversion);
    // genuine primary surface
    memcpy((void *)&ddsd, lpddsd, lpddsd->dwSize);
    // v2.03.98 - when going to fullscreen mode and no emulation do not alter the capability masks, or in other words ...
    // if doing emulation or in window mode, fix the capability masks (De Morgan docet)
    ddsd.dwFlags &= ~(DDSD_WIDTH | DDSD_HEIGHT | DDSD_BACKBUFFERCOUNT | DDSD_REFRESHRATE | DDSD_PIXELFORMAT);
    ddsd.ddsCaps.dwCaps &= ~(DDSCAPS_FLIP | DDSCAPS_COMPLEX);
    // v2.02.93: don't move primary / backbuf surfaces on systemmemory when 3DDEVICE is requested
    // this impact also on capabilities for temporary surfaces for AERO optimized handling
    // v2.04.08: this seems not always true, so it can be bypassed by ALLOWSYSMEMON3DDEV, just in case.
    // should be tested with "Tomb Raider 3" in no emulation mode
    if(!(dxw.dwFlags8 & ALLOWSYSMEMON3DDEV)) {
        if ((lpddsd->dwFlags & DDSD_CAPS) && (lpddsd->ddsCaps.dwCaps & DDSCAPS_3DDEVICE))
            ddsd.ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
    }
    // create Primary surface
    OutTraceDW("BuildPrimaryDir: %s\n", LogSurfaceAttributes(&ddsd, "[Primary]", __LINE__));
    res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, 0);
    if(res) {
        if (res == DDERR_PRIMARYSURFACEALREADYEXISTS) {
            LPDIRECTDRAWSURFACE lpPrim;
            GetGDISurface_Type pGetGDISurface;
            pGetGDISurface = pGetGDISurfaceMethod(dxversion);
            OutTraceE("BuildPrimaryDir: CreateSurface DDERR_PRIMARYSURFACEALREADYEXISTS workaround\n");
            (*pGetGDISurface)(lpPrimaryDD, &lpPrim);
            while ((*pReleaseSMethod(dxversion))(lpPrim));
            res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, 0);
        }
        /* fall through */
        if(res) {
#ifndef DXW_NOTRACES
            OutTraceE("BuildPrimaryDir: CreateSurface ERROR on DDSPrim res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
            if(res == DDERR_INVALIDPIXELFORMAT) DumpPixFmt(&ddsd);
#endif // DXW_NOTRACES
            return res;
        }
    }
#ifndef DXW_NOTRACES
    OutTraceDW("BuildPrimaryDir: created PRIMARY DDSPrim=%#x\n", *lplpdds);
    if(IsDebugDW) DescribeSurface(*lplpdds, dxversion, "DDSPrim", __LINE__);
#endif // DXW_NOTRACES
    // v2.04.40: DXW_RENDERER_EMULATEBUFFER is the only case of not emulated renderer that requires a real primary surface,
    // so the if condition can be generalized checking (dxw.RendererId == DXW_RENDERER_EMULATEBUFFER)
    //if(dxw.RendererId == DXW_RENDERER_EMULATEBUFFER){
    if(dxw.Renderer->flags & DXWRF_BUILDREALSURFACES) {
        lpDDSEmu_Prim = *lplpdds;
        dxwss.PopSurface(lpDDSEmu_Prim);
        ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
        // warning: can't create zero sized backbuffer surface !!!!
        ddsd.dwWidth = dxw.GetScreenWidth();
        ddsd.dwHeight = dxw.GetScreenHeight();
        ddsd.ddsCaps.dwCaps = 0;
        if (dxversion >= 4) ddsd.ddsCaps.dwCaps |= DDSCAPS_OFFSCREENPLAIN;
        OutTraceDW("BuildPrimaryDir: %s\n", LogSurfaceAttributes(&ddsd, "[Dir FixBuf]", __LINE__));
        res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, 0);
        if(res) {
            OutTraceE("BuildPrimaryDir: CreateSurface ERROR on DDSPrim res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
            return res;
        }
#ifndef DXW_NOTRACES
        OutTraceDW("BuildPrimaryDir: created FIX DDSPrim=%#x\n", *lplpdds);
        if(IsDebugDW) DescribeSurface(*lplpdds, dxversion, "DDSPrim(2)", __LINE__);
#endif // DXW_NOTRACES
    }
    iPrimarySurfaceVersion = dxversion; // v2.03.37
    HookDDSurface(lplpdds, dxversion, TRUE);
    if(dxw.dwFlags1 & CLIPCURSOR) dxw.SetClipCursor();
    return DD_OK;
}

static HRESULT BuildBackBufferEmu(LPDIRECTDRAW lpdd, CreateSurface_Type pCreateSurface, LPDDSURFACEDESC2 lpddsd, int dxversion, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    DDSURFACEDESC2 ddsd;
    HRESULT res;
    OutTraceDW("BuildBackBufferEmu: lpdd=%#x pCreateSurface=%#x lpddsd=%#x version=%d\n", lpdd, pCreateSurface, lpddsd, dxversion);
    // create BackBuffer surface
    memcpy(&ddsd, lpddsd, lpddsd->dwSize);
    ddsd.dwFlags &= ~(DDSD_BACKBUFFERCOUNT | DDSD_REFRESHRATE);
    ddsd.dwFlags |= (DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT);
    ddsd.ddsCaps.dwCaps &= ~(DDSCAPS_BACKBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
    // v2.03.75: if a surface desc is NOT specified, build one. This will allow ZBUF attach.
    if(!(lpddsd->dwFlags & DDSD_PIXELFORMAT)) SetPixFmt((LPDDSURFACEDESC2)&ddsd);
    // DDSCAPS_OFFSCREENPLAIN seems required to support the palette in memory surfaces
    ddsd.ddsCaps.dwCaps |= (DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN);
    // necessary: Martian Gotic crashes otherwise
    // v2.04.08: this seems no longer true in some cases, so ALLOWSYSMEMON3DDEV will bypass it.
    // Should be tested with "Martian Gotic" and "Dominant Species" SW mode in emulation modes
    if(!(dxw.dwFlags8 & ALLOWSYSMEMON3DDEV)) {
        if((ddsd.dwFlags & DDSD_CAPS) && (ddsd.ddsCaps.dwCaps & DDSCAPS_3DDEVICE))
            ddsd.ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
    }
    // on WinXP Fifa 99 doesn't like DDSCAPS_SYSTEMMEMORY cap, so better to leave a way to unset it....
    if(dxw.dwFlags6 & NOSYSMEMBACKBUF) ddsd.ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth = dxw.GetScreenWidth();
    ddsd.dwHeight = dxw.GetScreenHeight();
    GetPixFmt(&ddsd);
    OutTraceDW("BuildBackBufferEmu: %s\n", LogSurfaceAttributes(&ddsd, "[Backbuf]", __LINE__));
    res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, 0);
    if(res) {
#ifndef DXW_NOTRACES
        OutTraceE("BuildBackBufferEmu: CreateSurface ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        if(res == DDERR_INVALIDPIXELFORMAT) DumpPixFmt(&ddsd);
#endif // DXW_NOTRACES
        return res;
    }
#ifndef DXW_NOTRACES
    OutTraceDW("BuildBackBufferEmu: created BACK DDSBack=%#x\n", *lplpdds);
    if(IsDebugDW) DescribeSurface(*lplpdds, dxversion, "DDSBack", __LINE__);
#endif // DXW_NOTRACES
    HookDDSurface(lplpdds, dxversion, FALSE); // added !!!
    iPrimarySurfaceVersion = dxversion; // v2.02.31
    if((ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) &&
            (dxw.dwFlags6 & SYNCPALETTE)) {
        if(lpDDP == NULL) {
            res = (*pCreatePaletteMethod(dxversion))(lpdd, DDPCAPS_8BIT | DDPCAPS_ALLOW256, DefaultSystemPalette, &lpDDP, NULL);
#ifndef DXW_NOTRACES
            if(res) OutTraceE("BuildBackBufferEmu: CreatePalette ERROR err=%#x at %d\n", res, __LINE__);
#endif // DXW_NOTRACES
        }
        // this must be done after hooking - who knows why?
        res = (*pSetPaletteMethod(dxversion))(*lplpdds, lpDDP);
        if(res)
            OutTraceE("BuildBackBufferEmu: SetPalette ERROR err=%#x at %d\n", res, __LINE__);
        else iDDPExtraRefCounter = 1;
    }
    // V2.1.85/V2.2.34: tricky !!!!
    // When a real backbuffer is created, it has a reference to its frontbuffer.
    // some games (Monopoly 3D) may depend on this setting - i.e. they could close
    // the exceeding references - so this is better be replicated adding an initial
    // reference to the zero count. But you don't have to do this if the backbuffer
    // is created independently by the primary surface.
    (*lplpdds)->AddRef(); // should it be repeated BBCount times????
    return DD_OK;
}

static HRESULT BuildBackBufferFlippable(LPDIRECTDRAW lpdd, CreateSurface_Type pCreateSurface, LPDDSURFACEDESC2 lpddsd, int dxversion, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    DDSURFACEDESC2 ddsd;
    HRESULT res;
    OutTraceDW("BuildBackBufferFlippable: lpdd=%#x pCreateSurface=%#x lpddsd=%#x version=%d\n", lpdd, pCreateSurface, lpddsd, dxversion);
    //MessageBox(NULL, "BuildBackBufferFlippable", "DxWnd", MB_OK);
    // create BackBuffer surface
    memcpy(&ddsd, lpddsd, lpddsd->dwSize);
    ddsd.dwFlags &= ~(DDSD_BACKBUFFERCOUNT | DDSD_REFRESHRATE);
    ddsd.dwFlags |= (DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT);
    ddsd.ddsCaps.dwCaps &= ~(DDSCAPS_PRIMARYSURFACE | DDSCAPS_FRONTBUFFER | DDSCAPS_BACKBUFFER | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
    ddsd.ddsCaps.dwCaps |= (DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN);
    ddsd.dwWidth = dxw.GetScreenWidth();
    ddsd.dwHeight = dxw.GetScreenHeight();
    GetPixFmt(&ddsd);
    OutTraceDW("BuildBackBufferFlippable: %s\n", LogSurfaceAttributes(&ddsd, "[Backbuf]", __LINE__));
    res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, 0);
    if(res) {
#ifndef DXW_NOTRACES
        OutTraceE("BuildBackBufferFlippable: CreateSurface ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        if(res == DDERR_INVALIDPIXELFORMAT) DumpPixFmt(&ddsd);
#endif // DXW_NOTRACES
        return res;
    }
#ifndef DXW_NOTRACES
    OutTraceDW("BuildBackBufferFlippable: created BACK DDSBack=%#x\n", *lplpdds);
    if(IsDebugDW) DescribeSurface(*lplpdds, dxversion, "DDSBack", __LINE__);
#endif // DXW_NOTRACES
    HookDDSurface(lplpdds, dxversion, FALSE); // added !!!
    iPrimarySurfaceVersion = dxversion; // v2.02.31
    if((ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) &&
            (dxw.dwFlags6 & SYNCPALETTE)) {
        if(lpDDP == NULL) {
            res = (*pCreatePaletteMethod(dxversion))(lpdd, DDPCAPS_8BIT | DDPCAPS_ALLOW256, DefaultSystemPalette, &lpDDP, NULL);
#ifndef DXW_NOTRACES
            if(res) OutTraceE("BuildBackBufferFlippable: CreatePalette ERROR err=%#x at %d\n", res, __LINE__);
#endif // DXW_NOTRACES
        }
        // this must be done after hooking - who knows why?
        res = (*pSetPaletteMethod(dxversion))(*lplpdds, lpDDP);
        if(res)
            OutTraceE("BuildBackBufferFlippable: SetPalette ERROR err=%#x at %d\n", res, __LINE__);
        else iDDPExtraRefCounter = 1;
    }
    return DD_OK;
}

static HRESULT AttachBackBufferFlippable(LPDIRECTDRAW lpdd, CreateSurface_Type pCreateSurface, LPDDSURFACEDESC2 lpddsd, int dxversion, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    HRESULT res;
    LPDIRECTDRAWSURFACE lpDDSPrim;
    OutTraceDW("AttachBackBufferFlippable: lpdd=%#x pCreateSurface=%#x lpddsd=%#x version=%d\n", lpdd, pCreateSurface, lpddsd, dxversion);
    // retrieve the attached backbuffer surface and hook it
    if(lpddsd->dwBackBufferCount == 0) return DD_OK; // nothing to retrieve
    GetAttachedSurface_Type pGetAttachedSurface;
    DDSCAPS2 caps;
    switch(dxversion) {
    case 1:
        pGetAttachedSurface = pGetAttachedSurface1;
        break;
    case 2:
        pGetAttachedSurface = pGetAttachedSurface2;
        break;
    case 3:
        pGetAttachedSurface = pGetAttachedSurface3;
        break;
    case 4:
        pGetAttachedSurface = pGetAttachedSurface4;
        break;
    case 7:
        pGetAttachedSurface = pGetAttachedSurface7;
        break;
    }
    memset(&caps, 0, sizeof(caps));
    caps.dwCaps = DDSCAPS_BACKBUFFER;
    lpDDSPrim = dxwss.GetPrimarySurface();
    res = (*pGetAttachedSurface)(lpDDSPrim, (LPDDSCAPS)&caps, lplpdds);
    if(res) {
        OutTraceE("AttachBackBufferFlippable: GetAttachedSurface ERROR on DDSPrim res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        return res;
    }
#ifndef DXW_NOTRACES
    OutTraceDW("AttachBackBufferFlippable: retrieved BACK DDSBack=%#x\n", *lplpdds);
    if(IsDebugDW) DescribeSurface(*lplpdds, dxversion, "DDSBack", __LINE__);
#endif // DXW_NOTRACES
    HookDDSurface(lplpdds, dxversion, FALSE); // added !!!
    iPrimarySurfaceVersion = dxversion; // v2.02.31
    return DD_OK;
}

static HRESULT BuildBackBufferFullscreen(LPDIRECTDRAW lpdd, CreateSurface_Type pCreateSurface, LPDDSURFACEDESC2 lpddsd, int dxversion, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    OutTraceDW("BuildBackBufferFullscreen: lpdd=%#x pCreateSurface=%#x lpddsd=%#x version=%d\n", lpdd, pCreateSurface, lpddsd, dxversion);
    return DD_OK;
}

static HRESULT AttachBackBufferFullscreen(LPDIRECTDRAW lpdd, CreateSurface_Type pCreateSurface, LPDDSURFACEDESC2 lpddsd, int dxversion, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    OutTraceDW("AttachBackBufferFullscreen: lpdd=%#x pCreateSurface=%#x lpddsd=%#x version=%d\n", lpdd, pCreateSurface, lpddsd, dxversion);
    return DD_OK;
}

static HRESULT BuildBackBufferDir(LPDIRECTDRAW lpdd, CreateSurface_Type pCreateSurface, LPDDSURFACEDESC2 lpddsd, int dxversion, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    DDSURFACEDESC2 ddsd;
    HRESULT res;
    OutTraceDW("BuildBackBufferDir: lpdd=%#x pCreateSurface=%#x lpddsd=%#x version=%d\n", lpdd, pCreateSurface, lpddsd, dxversion);
    // create BackBuffer surface
    // ClearSurfaceDesc((void *)&ddsd, dxversion);
    memcpy(&ddsd, lpddsd, lpddsd->dwSize);
    if(dxw.IsEmulated || dxw.Windowize) {
        ddsd.dwFlags &= ~(DDSD_WIDTH | DDSD_HEIGHT | DDSD_BACKBUFFERCOUNT | DDSD_REFRESHRATE | DDSD_PIXELFORMAT);
        ddsd.dwFlags |= (DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH);
        ddsd.ddsCaps.dwCaps &= ~(DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX);
        // v2.02.93: don't move primary / backbuf surfaces on systemmemory when 3DDEVICE is requested
        // v2.04.08: possibly no longer true ... to test with "Tomb Raider 3" non emulated modes
        if(dxw.dwFlags8 & ALLOWSYSMEMON3DDEV) {
            ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
            if (dxversion >= 4) ddsd.ddsCaps.dwCaps |= DDSCAPS_OFFSCREENPLAIN;
            ddsd.ddsCaps.dwCaps &= ~(DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
        } else {
            if(lpddsd->ddsCaps.dwCaps & DDSCAPS_3DDEVICE)
                ddsd.ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
            else {
                ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
                if (dxversion >= 4) ddsd.ddsCaps.dwCaps |= DDSCAPS_OFFSCREENPLAIN;
                ddsd.ddsCaps.dwCaps &= ~(DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
            }
        }
    }
    if(dxw.dwFlags6 & NOSYSMEMBACKBUF) ddsd.ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth = dxw.GetScreenWidth();
    ddsd.dwHeight = dxw.GetScreenHeight();
    if (dxw.dwFlags2 & BACKBUFATTACH) {
        LPDIRECTDRAWSURFACE lpPrim;
        DDSURFACEDESC2 prim;
        GetGDISurface_Type pGetGDISurface;
        pGetGDISurface = pGetGDISurfaceMethod(dxversion);
        (*pGetGDISurface)(lpPrimaryDD, &lpPrim);
        memset(&prim, 0, sizeof(DDSURFACEDESC2));
        prim.dwSize = (dxversion >= 4) ? sizeof(DDSURFACEDESC2) : sizeof(DDSURFACEDESC);
        res = lpPrim->GetSurfaceDesc((DDSURFACEDESC *)&prim);
        (*pReleaseSMethod(dxversion))(lpPrim);
        ddsd.dwWidth = prim.dwWidth;
        ddsd.dwHeight = prim.dwHeight;
        OutTraceDW("BuildBackBufferDir: BMX FIX res=%#x(%s) wxh=(%dx%d)\n", res, ExplainDDError(res), ddsd.dwWidth, ddsd.dwHeight);
    }
    OutTraceDW("BuildBackBufferDir: %s\n", LogSurfaceAttributes(&ddsd, "[Backbuf]", __LINE__));
    res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, 0);
    if(res) {
        if ((dxw.dwFlags1 & SWITCHVIDEOMEMORY) && (res == DDERR_OUTOFVIDEOMEMORY)) {
            OutTraceDW("BuildBackBufferDir: CreateSurface DDERR_OUTOFVIDEOMEMORY ERROR at %d, retry in SYSTEMMEMORY\n", __LINE__);
            ddsd.ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
            ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
            res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, 0);
        }
        if(res) {
            OutTraceE("BuildBackBufferDir: CreateSurface ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
            return res;
        }
    }
#ifndef DXW_NOTRACES
    OutTraceDW("BuildBackBufferDir: created BACK DDSBack=%#x\n", *lplpdds);
    if(IsDebugDW) DescribeSurface(*lplpdds, dxversion, "DDSBack", __LINE__);
#endif // DXW_NOTRACES
    HookDDSurface(lplpdds, dxversion, FALSE); // added !!!
    iPrimarySurfaceVersion = dxversion; // v2.02.31
    return DD_OK;
}

static HRESULT BuildGenericEmu(LPDIRECTDRAW lpdd, CreateSurface_Type pCreateSurface, LPDDSURFACEDESC2 lpddsd, int dxversion, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    DDSURFACEDESC2 ddsd;
    HRESULT res;
    OutTraceDW("BuildGenericEmu: lpdd=%#x pCreateSurface=%#x lpddsd=%#x version=%d\n", lpdd, pCreateSurface, lpddsd, dxversion);
    memcpy(&ddsd, lpddsd, lpddsd->dwSize); // Copy over ....
    FixSurfaceCaps(&ddsd, dxversion);
    // It looks that DDSCAPS_SYSTEMMEMORY surfaces can perfectly be DDSCAPS_3DDEVICE as well.
    // For "Risk II" it is necessary that both the primary surface and the offscreen surfaces are generated
    // with the same type, so that assuming an identical lPitch and memcopy-ing from one buffer to the
    // other is a legitimate operation.
    if(dxw.dwFlags6 & POWER2WIDTH) { // v2.03.28: POWER2WIDTH to fix "Midtown Madness" in surface emulation mode
        if(((ddsd.dwFlags & (DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH)) == (DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH)) &&
                (ddsd.ddsCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN)
          ) {
            DWORD dwWidth;
            dwWidth = ((ddsd.dwWidth + 3) >> 2) << 2;
#ifndef DXW_NOTRACES
            if(dwWidth != ddsd.dwWidth) OutTraceDW("BuildGenericEmu: POWER2WIDTH fix surface width %d->%d\n", ddsd.dwWidth, dwWidth);
#endif // DXW_NOTRACES
            ddsd.dwWidth = dwWidth;
        }
    }
#ifdef NOMIPMAP
    if(NOMIPMAP) {
        ddsd.dwFlags &= ~DDSD_MIPMAPCOUNT;
        ddsd.ddsCaps.dwCaps &= ~DDSCAPS_MIPMAP;
    }
#endif
    res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, pu);
    // v2.04.23: error condition found & fixed when requested ZBUFFER bit depth doesn't match with maximum available
    // So far, found on "Forsaken" HW renderer.
    if ((res == DDERR_UNSUPPORTED) && (ddsd.dwFlags & DDSD_ZBUFFERBITDEPTH)) {
        DWORD maxdepth;
        if(dxw.DXCapsD2.dwZBufferBitDepths & DDBD_1 ) maxdepth = 1;
        if(dxw.DXCapsD2.dwZBufferBitDepths & DDBD_2 ) maxdepth = 2;
        if(dxw.DXCapsD2.dwZBufferBitDepths & DDBD_4 ) maxdepth = 4;
        if(dxw.DXCapsD2.dwZBufferBitDepths & DDBD_8 ) maxdepth = 8;
        if(dxw.DXCapsD2.dwZBufferBitDepths & DDBD_16) maxdepth = 16;
        if(dxw.DXCapsD2.dwZBufferBitDepths & DDBD_24) maxdepth = 24;
        if(dxw.DXCapsD2.dwZBufferBitDepths & DDBD_32) maxdepth = 32;
        switch (dxversion) {
        case 1:
        case 2:
        case 3:
            LPDDSURFACEDESC ddsd1;
            ddsd1 = (LPDDSURFACEDESC)&ddsd;
            OutTraceDW("BuildGenericEmu: CreateSurface ERROR res=%#x(%s) at %d, TEXTURE->dwZBufferBitDepth %d->%d\n",
                       res, ExplainDDError(res), __LINE__, ddsd1->dwZBufferBitDepth, maxdepth);
            if(ddsd1->dwZBufferBitDepth > maxdepth) ddsd1->dwZBufferBitDepth = maxdepth;
            break;
        case 4:
        case 7:
            OutTraceDW("BuildGenericEmu: CreateSurface ERROR res=%#x(%s) at %d, TEXTURE->dwZBufferBitDepth %d->%d\n",
                       res, ExplainDDError(res), __LINE__, ddsd.ddpfPixelFormat.dwZBufferBitDepth, maxdepth);
            if(ddsd.ddpfPixelFormat.dwZBufferBitDepth > maxdepth) ddsd.ddpfPixelFormat.dwZBufferBitDepth = maxdepth;
            break;
        }
        res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, pu);
    }
    // v2.04.05: error condition found & fixed on certain platforms for "Dominant Species" only (so far).
    if (((res == DDERR_UNSUPPORTED) || (res == DDERR_UNSUPPORTEDMODE)) &&
            (ddsd.ddsCaps.dwCaps & DDSCAPS_TEXTURE)) {
        OutTraceDW("BuildGenericEmu: CreateSurface ERROR res=%#x(%s) at %d, TEXTURE->OFFSCREENPLAIN retry\n", res, ExplainDDError(res), __LINE__);
        ddsd.ddsCaps.dwCaps &= ~(DDSCAPS_TEXTURE | DDSCAPS_ALLOCONLOAD);
        ddsd.ddsCaps.dwCaps |= (DDSCAPS_OFFSCREENPLAIN);
        res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, pu);
    }
    if ((dxw.dwFlags1 & SWITCHVIDEOMEMORY) && (res != DD_OK)) {
        OutTraceDW("BuildGenericEmu: CreateSurface ERROR res=%#x(%s) at %d, retry\n", res, ExplainDDError(res), __LINE__);
        ddsd.ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
        ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
        res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, pu);
    }
    if (res) {
        OutTraceE("BuildGenericEmu: CreateSurface ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        OutTraceDW("BuildGenericEmu: ERROR %s\n", LogSurfaceAttributes(&ddsd, "[Emu Generic]", __LINE__));
        return res;
    }
    OutTraceDW("BuildGenericEmu: CREATED lpdds=%#x version=%d %s\n",
               *lplpdds, dxversion, LogSurfaceAttributes(&ddsd, "[Emu Generic]", __LINE__));
    // v2.02.66: if 8BPP paletized surface and a primary palette exixts, apply.
    // fixes "Virtua Fighter PC" palette bug
    // v2.05.15: since "Sleepwalker" fix the following if block to manage "Virtua Fighter PC" palette
    // seems no longer necessary.
    // v2.05.19: code block uncommented, it fixes palette problems on certain versions of "Virtua Fighter PC"
    // and "Speedboat Attack".
    if((ddsd.ddpfPixelFormat.dwRGBBitCount == 8) && (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) && lpDDP) {
        SetPalette_Type pSetPalette;
        if(lpDDP == NULL) {
            // create a main palette if not there's one
            res = (*pCreatePaletteMethod(dxversion))(lpdd, DDPCAPS_8BIT | DDPCAPS_ALLOW256, DefaultSystemPalette, &lpDDP, NULL);
            if(res) OutTraceE("BuildGenericEmu: CreatePalette ERROR err=%#x at %d\n", res, __LINE__);
            else OutTrace("BuildGenericEmu: CreatePalette lpDDP=%#x\n", lpDDP);
        }
        OutTrace("BuildGenericEmu: SetPalette lpdds=%#x lpddp=%#x\n", *lplpdds, lpDDP);
        // v2.04.84: BEWARE: found on "Atripolis 2097"
        // on dxversion==1 and offscreen surface, the SetPalette pointer was NULL
        // added code to properly manage NULL condition
        pSetPalette = pSetPaletteMethod(dxversion);
        if(pSetPalette) {
            res = (*pSetPaletteMethod(dxversion))(*lplpdds, lpDDP);
            if(res) {
                OutTraceE("BuildGenericEmu: SetPalette ERROR lpdds=%#x res=%#x(%s) at %d\n", *lplpdds, res, ExplainDDError(res), __LINE__);
                if(res == DDERR_INVALIDPARAMS) {
                    // v2.04.74: on ddraw version 1 (at least) you can't apply a palette created in a DDraw session
                    // to a surface connected to another DDraw session. Happens in "Ultimate Soccer Manager 98" that
                    // creates a second DDraw session without closing the first one.
                    // t.b.d. resource handling: the handle to first lpDDP is lost ....
                    // while (lpDDP->Release());
                    res = (*pCreatePaletteMethod(dxversion))(lpdd, DDPCAPS_8BIT | DDPCAPS_ALLOW256, DefaultSystemPalette, &lpDDP, NULL);
#ifndef DXW_NOTRACES
                    if(res) OutTraceE("BuildGenericEmu: CreatePalette ERROR err=%#x at %d\n", res, __LINE__);
#endif // DXW_NOTRACES
                    res = (*pSetPaletteMethod(dxversion))(*lplpdds, lpDDP);
#ifndef DXW_NOTRACES
                    if(res) OutTraceE("BuildGenericEmu: SetPaletteMethod ERROR err=%#x at %d\n", res, __LINE__);
#endif // DXW_NOTRACES
                }
            }
            if(!res) {
                OutTraceDW("BuildGenericEmu: applied palette lpddp=%#x to lpdds=%#x\n", lpDDP, *lplpdds);
                iDDPExtraRefCounter = 1;
            }
        } else
            OutTraceDW("BuildGenericEmu: NULL SetPalette method for dxversion=%d\n", dxversion);
    }
    // diagnostic hooks ....
    HookDDSurface(lplpdds, dxversion, FALSE);
    return DD_OK;
}

static HRESULT BuildGenericFlippable(LPDIRECTDRAW lpdd, CreateSurface_Type pCreateSurface, LPDDSURFACEDESC2 lpddsd, int dxversion, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    DDSURFACEDESC2 ddsd;
    HRESULT res;
    OutTraceDW("BuildGenericFlippable: lpdd=%#x pCreateSurface=%#x lpddsd=%#x version=%d\n", lpdd, pCreateSurface, lpddsd, dxversion);
    memcpy(&ddsd, lpddsd, lpddsd->dwSize); // Copy over ....
    FixSurfaceCaps(&ddsd, dxversion);
    if(dxw.dwFlags6 & POWER2WIDTH) { // v2.03.28: POWER2WIDTH to fix "Midtown Madness" in surface emulation mode
        if(((ddsd.dwFlags & (DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH)) == (DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH)) &&
                (ddsd.ddsCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN)
          ) {
            DWORD dwWidth;
            dwWidth = ((ddsd.dwWidth + 3) >> 2) << 2;
#ifndef DXW_NOTRACES
            if(dwWidth != ddsd.dwWidth) OutTraceDW("BuildGenericFlippable: POWER2WIDTH fix surface width %d->%d\n", ddsd.dwWidth, dwWidth);
#endif // DXW_NOTRACES
            ddsd.dwWidth = dwWidth;
        }
    }
    res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, pu);
    if ((dxw.dwFlags1 & SWITCHVIDEOMEMORY) && (res != DD_OK)) {
        OutTraceDW("BuildGenericFlippable: CreateSurface ERROR res=%#x(%s) at %d, retry\n", res, ExplainDDError(res), __LINE__);
        ddsd.ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
        ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
        res = (*pCreateSurface)(lpdd, &ddsd, lplpdds, pu);
    }
    if (res) {
        OutTraceE("BuildGenericFlippable: CreateSurface ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        OutTraceDW("BuildGenericFlippable: ERROR %s\n", LogSurfaceAttributes(&ddsd, "[Emu Generic]", __LINE__));
        return res;
    }
    OutTraceDW("BuildGenericFlippable: CREATED lpdds=%#x version=%d %s\n",
               *lplpdds, dxversion, LogSurfaceAttributes(&ddsd, "[Emu Generic]", __LINE__));
    // v2.02.66: if 8BPP paletized surface and a primary palette exixts, apply.
    // fixes "Virtua Fighter PC" palette bug
    if(lpDDP && (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)) {
        res = (*pSetPaletteMethod(dxversion))(*lplpdds, lpDDP);
        if(res) {
            OutTraceE("BuildGenericFlippable: SetPalette ERROR on lpdds=%#x res=%#x(%s) at %d\n", *lplpdds, res, ExplainDDError(res), __LINE__);
            if(res == DDERR_INVALIDPARAMS) {
                // v2.04.74: on ddraw version 1 (at least) you can't apply a palette created in a DDraw session
                // to a surface connected to another DDraw session. Happens in "Ultimate Soccer Manager 98" that
                // creates a second DDraw session without closing the first one.
                // t.b.d. resource handling: the handle to first lpDDP is lost ....
                // while (lpDDP->Release());
                res = (*pCreatePaletteMethod(dxversion))(lpdd, DDPCAPS_8BIT | DDPCAPS_ALLOW256, DefaultSystemPalette, &lpDDP, NULL);
#ifndef DXW_NOTRACES
                if(res) OutTraceE("BuildGenericFlippable: CreatePalette ERROR err=%#x at %d\n", res, __LINE__);
#endif // DXW_NOTRACES
                res = (*pSetPaletteMethod(dxversion))(*lplpdds, lpDDP);
#ifndef DXW_NOTRACES
                if(res) OutTraceE("BuildGenericFlippable: SetPaletteMethod ERROR err=%#x at %d\n", res, __LINE__);
#endif // DXW_NOTRACES
            }
        }
        if(!res) {
            OutTraceDW("BuildGenericFlippable: applied palette lpddp=%#x to lpdds=%#x\n", lpDDP, *lplpdds);
            iDDPExtraRefCounter++;
        }
    }
    // diagnostic hooks ....
    HookDDSurface(lplpdds, dxversion, FALSE);
    return DD_OK;
}

static HRESULT BuildGenericDir(LPDIRECTDRAW lpdd, CreateSurface_Type pCreateSurface, LPDDSURFACEDESC2 lpddsd, int dxversion, LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    HRESULT res;
    OutTraceDW("BuildGenericDir: lpdd=%#x pCreateSurface=%#x lpddsd=%#x version=%d\n", lpdd, pCreateSurface, lpddsd, dxversion);
    OutTraceDW("BuildGenericDir: %s\n", LogSurfaceAttributes(lpddsd, "[Dir Generic]", __LINE__));
    res = (*pCreateSurface)(lpdd, lpddsd, lplpdds, 0);
    if(res) {
        // v2.02.60: Ref. game Incoming GOG release, post by Marek, error DDERR_UNSUPPORTED while trying to create ZBUFFER surface
        if ((dxw.dwFlags1 & SWITCHVIDEOMEMORY) && ((res == DDERR_OUTOFVIDEOMEMORY) || (res == DDERR_UNSUPPORTED))) {
            OutTraceDW("BuildGenericDir: CreateSurface ERROR res=%#x(%s) at %d, retry\n", res, ExplainDDError(res), __LINE__);
            lpddsd->ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
            lpddsd->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
            res = (*pCreateSurface)(lpdd, lpddsd, lplpdds, 0);
        }
        if(res) {
            OutTraceE("BuildGenericDir: CreateSurface ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
            OutTraceDW("BuildGenericDir: ERROR %s\n", LogSurfaceAttributes(lpddsd, "[Dir Generic]", __LINE__));
            return res;
        }
    }
    OutTraceDW("BuildGenericDir: CREATED lpddsd=%#x version=%d %s\n",
               *lplpdds, dxversion, LogSurfaceAttributes(lpddsd, "[Dir Generic]", __LINE__));
    // hooks ....
    HookDDSurface(lplpdds, dxversion, FALSE);
    return DD_OK;
}

HRESULT WINAPI extCreateSurface(int dxversion, CreateSurface_Type pCreateSurface, LPDIRECTDRAW lpdd, DDSURFACEDESC2 *lpddsd,
                                LPDIRECTDRAWSURFACE *lplpdds, void *pu) {
    HRESULT res;
    ApiName("CreateSurface");
    DDSURFACEDESC2 ddsd;
    LPDIRECTDRAWSURFACE lpDDSPrim;
    LPDIRECTDRAWSURFACE lpDDSBack = NULL;
    DWORD CurFlags;
    int TargetSize;
    typedef HRESULT (*BuildSurface_Type)(LPDIRECTDRAW, CreateSurface_Type, LPDDSURFACEDESC2, int, LPDIRECTDRAWSURFACE *, void *);
    static BuildSurface_Type BuildPrimary = 0;
    static BuildSurface_Type BuildBackBuffer = 0;
    static BuildSurface_Type AttachBackBuffer = 0;
    static BuildSurface_Type BuildGeneric = 0;
    static enum  {
        PRIMARY_FULLSCREEN = 0,
        PRIMARY_DIRECT,
        PRIMARY_FLIPPABLE,
        PRIMARY_EMULATED
    } SurfaceMode;
    OutTraceDDRAW("%s(%d): lpdd=%#x %s\n",
                  ApiRef, dxversion, lpdd, LogSurfaceAttributes(lpddsd, "[CreateSurface]", __LINE__));
    // v2.03.95.fx1 - deleted: some texture handling REQUIRES a proper FourCC codec.
    // maybe it could be suppressed by a dedicated config. flag and on primary surfaces only?
    //lpddsd->ddpfPixelFormat.dwFourCC = 0;
    // if not initialized yet ....
    if(BuildPrimary == 0) {
        char *sLabel;
        SurfaceMode = PRIMARY_FULLSCREEN;
        if(dxw.IsEmulated || dxw.Windowize) {
            SurfaceMode = (dxw.IsEmulated) ?
                          ((dxw.dwFlags6 & FLIPEMULATION) ?
                           PRIMARY_EMULATED :
                           PRIMARY_FLIPPABLE) :
                          PRIMARY_DIRECT;
        }
        switch(SurfaceMode)	{
        case PRIMARY_FULLSCREEN:
            BuildPrimary = BuildPrimaryFullscreen;
            BuildBackBuffer = BuildBackBufferFullscreen;
            AttachBackBuffer = AttachBackBufferFullscreen;
            BuildGeneric = BuildGenericDir;
            sLabel = "FULLSCR";
            break;
        case PRIMARY_DIRECT:
            BuildPrimary = BuildPrimaryDir;
            BuildBackBuffer = BuildBackBufferDir;
            AttachBackBuffer = BuildBackBufferDir;
            BuildGeneric = BuildGenericDir;
            sLabel = "DIRECT";
            break;
        case PRIMARY_FLIPPABLE:
            BuildPrimary = BuildPrimaryFlippable;
            BuildBackBuffer = BuildBackBufferFlippable;
            AttachBackBuffer = AttachBackBufferFlippable;
            BuildGeneric = BuildGenericFlippable;
            sLabel = "FLIPPABLE";
            break;
        case PRIMARY_EMULATED:
            BuildPrimary = BuildPrimaryEmu;
            BuildBackBuffer = BuildBackBufferEmu;
            AttachBackBuffer = BuildBackBufferEmu;
            BuildGeneric = BuildGenericEmu;
            sLabel = "EMULATED";
            break;
        }
        OutTraceDW("%s: MODE INITIALIZED mode=%d(%s)\n", api, SurfaceMode, sLabel);
    }
    // check for lpddsd->dwSize value
    TargetSize = (dxversion < 4) ? sizeof(DDSURFACEDESC) : sizeof(DDSURFACEDESC2);
    if(lpddsd->dwSize != TargetSize) {
#ifndef DXW_NOTRACES
        char sMsg[81];
        sprintf_s(sMsg, 80, "%s: ASSERT bad dwSize=%d dxversion=%d\n",
                  ApiRef, lpddsd->dwSize, dxversion);
        OutTraceDW(sMsg);
        if(IsAssertEnabled) MessageBox(0, sMsg, ApiRef, MB_OK | MB_ICONEXCLAMATION);
#endif // DXW_NOTRACES
        return DDERR_INVALIDPARAMS;
    }
    //GHO workaround (needed for WarWind, Rogue Spear):
    if (lpddsd->dwFlags && !(lpddsd->dwFlags & 0x1)) {
        OutTraceDW("%s: fixing illegal dwFlags value: %#x -> %#x\n",
                   api, lpddsd->dwFlags, (lpddsd->dwFlags | DDSD_CAPS));
        lpddsd->dwFlags |= DDSD_CAPS;
    }
    memcpy(&ddsd, lpddsd, lpddsd->dwSize); // Copy
    // v2.02.38: this is odd: in "Star Force Deluxe" there is no PRIMARY surface, but a surface with
    // 0 flags and 0 capabilities serves for this purpose. Is it a side-effect of old ddraw releases?
    if((dxversion == 1) && ((ddsd.dwFlags & ~DDSD_BACKBUFFERCOUNT) == 0)) { // Star Force Deluxe
        ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
        //if(dxw.VirtualPixelFormat.dwRGBBitCount == 8) ddsd.ddsCaps.dwCaps |= DDSCAPS_PALETTE;
    }
    // creation of the primary surface....
    if(ddsd.dwFlags & DDSD_CAPS && ddsd.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE) {
        OutTraceDW("%s: building primary surface\n", api);
        dxwss.ClearSurfaceList(); // v2.03.83: "Daytona USA" would saturate the list otherwise
        SetVSyncDelays(dxversion, lpdd);
        GetHookInfo()->Height = (short)dxw.GetScreenHeight();
        GetHookInfo()->Width = (short)dxw.GetScreenWidth();
        GetHookInfo()->ColorDepth = (short)dxw.VirtualPixelFormat.dwRGBBitCount;
        GetHookInfo()->DXVersion = dxversion;
        lpPrimaryDD = lpdd; // v2.1.87
        memcpy(&DDSD_Prim, lpddsd, sizeof(DDSD_Prim)); // v2.02.37
        // beware of the different behaviour between older and newer directdraw releases...
        if(dxversion >= 4) {
            if (lpDDSEmu_Back) while(lpDDSEmu_Back->Release());
            if (lpDDSEmu_Prim) while(lpDDSEmu_Prim->Release());
            if (ddsd.dwFlags & DDSD_BACKBUFFERCOUNT) { // Praetorians !!!!
                lpDDSBack = dxwss.GetBackBufferSurface();
                if (lpDDSBack) {
                    if(dxw.dwFlags6 & FLIPEMULATION) while(lpDDSBack->Release());
                    dxwss.PopSurface(lpDDSBack);
                    lpDDSBack = NULL;
                }
            }
        }
        lpDDSEmu_Back = NULL;
        lpDDSEmu_Prim = NULL;
        int BBCount = 0;
        if (ddsd.ddsCaps.dwCaps & DDSCAPS_FLIP) BBCount = 1; // v2.04.95: when has flip capability, build at least one backbuffer
        if (ddsd.dwFlags & DDSD_BACKBUFFERCOUNT) BBCount = ddsd.dwBackBufferCount;
        if ((BBCount > 0) && (iPrimarySurfaceVersion < 4)) {
            lpDDSBack = NULL;
            OutTraceDW("%s: backbuffer cleared - BackBufferCount=%d\n", api, BBCount);
        }
        if (BBCount > MAXBACKBUFFERS) {
#ifndef DXW_NOTRACES
            char sMsg[81];
            sprintf_s(sMsg, 80, "%s: BackBufferCount=%d\n", api, BBCount);
            OutTraceDW(sMsg);
            if (IsAssertEnabled) MessageBox(0, sMsg, api, MB_OK | MB_ICONEXCLAMATION);
            // recover ...
#endif // DXW_NOTRACES
            BBCount = MAXBACKBUFFERS;
        }
        // build emulated primary surface, real primary and backbuffer surfaces
        CurFlags = ddsd.dwFlags;
        res = BuildPrimary(lpdd, pCreateSurface, lpddsd, dxversion, lplpdds, NULL);
        if(res) return res;
        lpDDSPrim = *lplpdds;
        dxwss.PushPrimarySurface(lpDDSPrim, dxversion, lpddsd->ddsCaps.dwCaps);
        RegisterPixelFormat(dxversion, lpDDSPrim);
        if (BBCount) {
            DWORD dwCaps;
            // build emulated backbuffer surface
            res = AttachBackBuffer(lpdd, pCreateSurface, lpddsd, dxversion, &lpDDSBack, NULL);
            if(res) return res;
            // here we try to guess what sort of capabilities would expose a built-in backbuffer surface
            dwCaps = lpddsd->ddsCaps.dwCaps;
            dwCaps &= ~DDSCAPS_PRIMARYSURFACE;
            dwCaps |= (DDSCAPS_BACKBUFFER | DDSCAPS_VIDEOMEMORY);
            dxwss.PushBackBufferSurface(lpDDSBack, dxversion, dwCaps);
        }
#ifndef DXW_NOTRACES
        if(IsTraceDDRAW) {
            char sInfo[256 + 1];
            sprintf(sInfo, "%s: created DDSPrim=%#x DDSBack=%#x", api, lpDDSPrim, lpDDSBack);
            if(dxw.Renderer->flags & DXWRF_BUILDREALSURFACES) sprintf(sInfo, "%s DDSEmu_Prim=%#x", sInfo, lpDDSEmu_Prim);
            if(dxw.Renderer->flags & DXWRF_BUILDREALSURFACES) sprintf(sInfo, "%s DDSEmu_Back=%#x", sInfo, lpDDSEmu_Back);
            strcat(sInfo, "\n");
            OutTrace(sInfo);
        }
#endif // DXW_NOTRACES
        // rebuild the clipper area
        if(dxw.dwFlags1 & CLIPCURSOR) dxw.SetClipCursor();
        // v2.2.64: added extra ref needed to preserve ddraw session for later use. Is it a ddraw1 legacy?
        // seems to fix problems in "Warhammer 40K Rites Of War" that uses a ddraw session after reaching 0 refcount.
        // v2.2.84: avoid the extra referenced in non windowed mode since it causes the window shift reported by gsky916
        // for Wind Fantasy SP.
        // v2.3.59: same extra reference is needed by "Wahammer Chaos Gate" that uses ddraw interface release 2
        // v2.3.72: fixed previous fix: condition is <=2, not >=2 !
        // Be aware that it may perhaps become <=3, if we get the same problem elsewhere
        // v2.3.96: deleted the if(Windowized) condition: AddRef is needed also in fullscreen mode.
        if(dxw.dwDDVersion <= 2) lpdd->AddRef();
        return DD_OK;
    }
    // a request for a separate (not complex) backbuffer to attach later on, maybe.
    if ((ddsd.dwFlags & DDSD_CAPS) && (ddsd.ddsCaps.dwCaps & DDSCAPS_BACKBUFFER)) {
        if (lpDDSBack) {
            OutTraceDW("%s: returning current DDSBack=%#x\n", api, lpDDSBack);
            *lplpdds = lpDDSBack;
            return DD_OK;
        }
        res = BuildBackBuffer(lpdd, pCreateSurface, lpddsd, dxversion, lplpdds, NULL);
        if(res == DD_OK) dxwss.PushBackBufferSurface(*lplpdds, dxversion, lpddsd->ddsCaps.dwCaps);
        return res;
    }
    // if nothing else, it's a generic/zbuffer surface
    // Note: should DxWnd trace a memory 3D surface that will be used as reference surface for D3D CreateDevice?
    res = BuildGeneric(lpdd, pCreateSurface, lpddsd, dxversion, lplpdds, pu);
    if(!res) {
        dxwss.PopSurface(*lplpdds);
        if(lpddsd->ddsCaps.dwCaps & DDSCAPS_ZBUFFER) {
            // v2.03.82: save ZBUFFER capabilities for later fix in D3D CreateDevice
            OutTraceDW("%s: lpDDZBuffer=%#x save ZBUFFER caps=%#x(%s)\n",
                       api, *lplpdds, lpddsd->ddsCaps.dwCaps, ExplainDDSCaps(lpddsd->ddsCaps.dwCaps));
            lpDDZBuffer = *lplpdds;
            // save surface size expressed in bytes
            // v2.04.98: fix use ZBufferBitDepth according to dxversion
            extern int ZBufferSize;
            DWORD ZBufferBitDepth;
            if(dxversion < 4) ZBufferBitDepth = ((LPDDSURFACEDESC)lpddsd)->dwZBufferBitDepth;
            else ZBufferBitDepth = lpddsd->ddpfPixelFormat.dwZBufferBitDepth;
            ZBufferSize = lpddsd->dwWidth * lpddsd->dwHeight * (ZBufferBitDepth >> 3);
            dxwss.PushZBufferSurface(*lplpdds, dxversion, lpddsd->ddsCaps.dwCaps);
            OutTraceDW("%s: ZBufferSize=%d\n", api, ZBufferSize);
        } else {
            if((lpddsd->ddsCaps.dwCaps & DDSCAPS_3DDEVICE) && !(lpddsd->ddsCaps.dwCaps & DDSCAPS_TEXTURE)) {
                // v2.04.30: set real caps first
                // lpddsd->ddsCaps.dwCaps holds the declared caps, but the created surface gets different ones
                // that have to be inserted in the surface stack to perform exact operations.
                // adding DDSCAPS_VIDEOMEMORY|DDSCAPS_LOCALVIDMEM fixes "Drakan Order of the Flame" demo.
                // v2.04.67: when no emulation is performed, the 3DREF capabilities should NOT be fixed, better
                // leave them untouched. This fixes "Motocross Madness 2 trial" in "Software rasterization" mode.
                // v2.04.68.fx1: textures can have DDSCAPS_3DDEVICE capability, but the're not 3DREF surfaces
                // and can be so many to saturate the surface stack. Better avod saving their capabilities.
                // this fixes the white textures in Forsaken.
                // v2.04.69: add VIDEOMEMORY|LOCALVIDMEM only in case no memory type is set, leave SYSTEMMEMORY
                // otherwise ...
                if(dxw.IsEmulated) {
                    if(!(lpddsd->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY))
                        lpddsd->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
                }
                OutTraceDW("%s: lpDD3DRef=%#x save 3DREF caps=%#x(%s)\n",
                           api, *lplpdds, lpddsd->ddsCaps.dwCaps, ExplainDDSCaps(lpddsd->ddsCaps.dwCaps));
                dxwss.Push3DRefSurface(*lplpdds, dxversion, lpddsd->ddsCaps.dwCaps);
            }
        }
    }
    return res;
}