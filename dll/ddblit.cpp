#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <ddraw.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "stdio.h"
#include "hddraw.h"
#include "dxhelper.h"

extern LPDIRECTDRAWSURFACE lpDDSEmu_Prim;
extern LPDIRECTDRAW lpPrimaryDD;
extern Blt_Type pBlt;
extern ReleaseS_Type pReleaseSMethod(int);
extern CreateSurface1_Type pCreateSurface1;
extern CreateSurface1_Type pCreateSurface2;
extern CreateSurface1_Type pCreateSurface3;
extern CreateSurface2_Type pCreateSurface4;
extern CreateSurface2_Type pCreateSurface7;
extern Unlock4_Type pUnlockMethod(int);
extern HDC hFlippedDC;
extern BOOL bFlippedDC;
extern ReleaseDC_Type pReleaseDC1;
extern ReleaseDC_Type pReleaseDC2;
extern ReleaseDC_Type pReleaseDC3;
extern ReleaseDC_Type pReleaseDC4;
extern ReleaseDC_Type pReleaseDC7;

extern void BlitError(HRESULT, LPRECT, LPRECT, int);
extern void BlitTrace(char *, LPRECT, LPRECT, int);
extern void TextureHandling(LPDIRECTDRAWSURFACE, int);
extern GetSurfaceDesc2_Type pGetSurfaceDescMethod();
extern GetSurfaceDesc2_Type GetSurfaceDescMethod();
extern Blt_Type pBltMethod();
extern PrimaryBlt_Type pPrimaryBlt;
extern void DDrawScreenShot(int, char *);
extern LPVOID dxwConvertFourCC(LPDDSURFACEDESC2);
extern CreateSurface2_Type pCreateSurfaceMethod(int);
extern void FixPixelFormat(int, DDPIXELFORMAT *);
extern Lock_Type pLockMethod(int);
extern void BuildRealSurfaces(LPDIRECTDRAW, CreateSurface_Type, int);
extern void ReleaseRealSurfaces(int);

typedef HRESULT (*FBlitter_Type)(int, Blt_Type, char *, LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX, BOOL);
typedef HRESULT (Blitter_Type)(int, Blt_Type, char *, LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX, BOOL);

FBlitter_Type pRendererBlt;

static BOOL dxwHWBlitUnsupported(int dxversion, Blt_Type pBlt, LPDIRECTDRAWSURFACE lpdds, LPRECT lpemurect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect) {
    // v2.04.74:
    // blit scaling & fourcc conversion is often unsupported, but you can try to split the operation in two:
    // first: blit to an identical size, offscreenplain no fourcc surface
    // second: blit scaling to desired size
    // it worked with "Rubik's Games" YUY2 intro movies
    OutDebugDW("dxwHWBlitUnsupported\n");
    do { // fake loop
        LPDIRECTDRAWSURFACE lpddsTmp;
        DDSURFACEDESC2 ddsd;
        HRESULT res2;
        int dwSize = (dxversion < 4) ? sizeof(DDSURFACEDESC) : sizeof(DDSURFACEDESC2);
        memset(&ddsd, 0, dwSize);
        ddsd.dwSize = dwSize;
        res2 = (*pGetSurfaceDescMethod())((LPDIRECTDRAWSURFACE2)lpddssrc, &ddsd);
        if(res2) {
            OutTraceE("GetSurfaceDesc ERROR: lpdds=%#x err=%#x(%s) at %d\n", lpddssrc, res2, ExplainDDError(res2), __LINE__);
            break;
        }
        //res=(*pLockMethod(dxversion))(lpddssrc, 0, (LPDDSURFACEDESC)&ddsd, DDLOCK_SURFACEMEMORYPTR|DDLOCK_READONLY, 0);
        ddsd.ddpfPixelFormat.dwFourCC = 0;
        FixPixelFormat(ddsd.ddpfPixelFormat.dwRGBBitCount, &ddsd.ddpfPixelFormat);
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
        res2 = (*pCreateSurfaceMethod(dxversion))(lpPrimaryDD, &ddsd, (LPDIRECTDRAWSURFACE *)&lpddsTmp, NULL);
        if(res2) {
            OutTraceE("CreateSurface ERROR: err=%#x(%s) at %d\n", res2, ExplainDDError(res2), __LINE__);
            break;
        }
        //res2=(*pUnlockMethod(dxversion))(lpddssrc, NULL);
        //if(res2) OutTraceE("Unlock ERROR: err=%#x(%s) at %d\n", res2, ExplainDDError(res2), __LINE__);
        res2 = (*pBlt)(lpddsTmp, lpsrcrect, lpddssrc, lpsrcrect, 0, NULL);
        if(res2) {
            OutTraceE("Blt ERROR: err=%#x(%s) at %d\n", res2, ExplainDDError(res2), __LINE__);
            (*pReleaseSMethod(dxversion))(lpddsTmp);
            break;
        }
        res2 = (*pBlt)(lpdds, lpemurect, lpddsTmp, lpsrcrect, 0, NULL);
        if(res2) {
            OutTraceE("Blt ERROR: err=%#x(%s) at %d\n", res2, ExplainDDError(res2), __LINE__);
            (*pReleaseSMethod(dxversion))(lpddsTmp);
            break;
        }
        (*pReleaseSMethod(dxversion))(lpddsTmp);
        return TRUE;
    } while (FALSE);
    return FALSE;
}

static BOOL dxwSWBlitUnsupported(int dxversion, Blt_Type pBlt, LPDIRECTDRAWSURFACE lpdds, LPRECT lpemurect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect) {
    // v2.04.74:
    // blit scaling & fourcc conversion is often unsupported, but you can try to use a sw conversion
    OutDebugDW("dxwSWBlitUnsupported\n");
    do { // fake loop
        LPDIRECTDRAWSURFACE lpddsTmp;
        DDSURFACEDESC2 ddsdsrc, ddsddst;
        HRESULT res2;
        LPVOID pixels;
        int dwSize = (dxversion < 4) ? sizeof(DDSURFACEDESC) : sizeof(DDSURFACEDESC2);
        memset(&ddsdsrc, 0, dwSize);
        ddsdsrc.dwSize = dwSize;
        res2 = (*pGetSurfaceDescMethod())((LPDIRECTDRAWSURFACE2)lpddssrc, &ddsdsrc);
        if(res2) {
            OutTraceE("GetSurfaceDesc ERROR: lpdds=%#x err=%#x(%s) at %d\n", lpddssrc, res2, ExplainDDError(res2), __LINE__);
            break;
        }
        if(ddsdsrc.ddpfPixelFormat.dwFourCC == 0) {
            OutTraceDW("PrimaryEmulated: no fourcc code\n");
            break;
        }
        if(ddsdsrc.lpSurface == 0) {
            OutTraceDW("PrimaryEmulated: no surface data\n");
            break;
        }
        //res=(*pLockMethod(dxversion))(lpddssrc, 0, (LPDDSURFACEDESC)&ddsd, DDLOCK_SURFACEMEMORYPTR|DDLOCK_READONLY, 0);
        memcpy(&ddsddst, &ddsdsrc, sizeof(DDSURFACEDESC2));
        ddsddst.ddpfPixelFormat.dwFourCC = 0;
        FixPixelFormat(ddsddst.ddpfPixelFormat.dwRGBBitCount, &ddsddst.ddpfPixelFormat);
        ddsddst.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
        res2 = (*pCreateSurfaceMethod(dxversion))(lpPrimaryDD, &ddsddst, (LPDIRECTDRAWSURFACE *)&lpddsTmp, NULL);
        if(res2) {
            OutTraceE("CreateSurface ERROR: err=%#x(%s) at %d\n", res2, ExplainDDError(res2), __LINE__);
            break;
        }
        if(ddsdsrc.ddpfPixelFormat.dwFourCC) pixels = dxwConvertFourCC(&ddsdsrc);
        if(!pixels) {
            OutTraceE("SDL2_BlitToWindow: ConvertFourCC ERROR\n");
            return DD_FALSE;
        }
        res2 = (*pLockMethod(dxversion))(lpddsTmp, 0, (LPDDSURFACEDESC)&ddsddst, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WRITEONLY, 0);
        if(res2) {
            OutTraceE("Lock ERROR: err=%#x(%s) at %d\n", res2, ExplainDDError(res2), __LINE__);
            break;
        }
        LPBYTE ps, pd;
        ps = (LPBYTE)pixels;
        pd = (LPBYTE)ddsddst.lpSurface;
        for(unsigned int y = 0; y < ddsdsrc.dwHeight; y++) {
            memcpy(pd, ps, ddsddst.lPitch);
            ps += ddsdsrc.lPitch;
            pd += ddsddst.lPitch;
        }
        res2 = (*pUnlockMethod(dxversion))(lpddsTmp, NULL);
#ifndef DXW_NOTRACES
        if(res2) OutTraceE("Unlock ERROR: err=%#x(%s) at %d\n", res2, ExplainDDError(res2), __LINE__);
#endif // DXW_NOTRACES
        res2 = (*pUnlockMethod(dxversion))(lpddssrc, NULL);
#ifndef DXW_NOTRACES
        if(res2) OutTraceE("Unlock ERROR: err=%#x(%s) at %d\n", res2, ExplainDDError(res2), __LINE__);
#endif // DXW_NOTRACES
        res2 = (*pBlt)(lpdds, lpemurect, lpddsTmp, lpsrcrect, 0, NULL);
        if(res2) {
            OutTraceE("Blt ERROR: err=%#x(%s) at %d\n", res2, ExplainDDError(res2), __LINE__);
            (*pReleaseSMethod(dxversion))(lpddsTmp);
            break;
        }
        (*pReleaseSMethod(dxversion))(lpddsTmp);
        return TRUE;
    } while (FALSE);
    return FALSE;
}

static HRESULT sBltNoPrimary(int dxversion, Blt_Type pBlt, ApiArg, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect,
                             LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    RECT srcrect;
    HRESULT res;
    BOOL FromScreen;
    FromScreen = dxwss.IsAPrimarySurface(lpddssrc);
    // make a working copy of srcrect if not NULL
    if (lpsrcrect) srcrect = *lpsrcrect;
    // when blitting from a primary surface on screen (that is in non emulated mode), correct offsets
    // You should take account also for scaled primary surfaces, but that would be a hard task:
    // a reduced primary surface (in not-emulated mode) would bring quality loss!!!
    // v2.1.83: BLITFROMBACKBUFFER mode, let you chose to blit from backbuffer, where the surface size
    // is fixed no matter how the window/primary surface is scaled.
    // In "The Sims" there is no quality loss, but some scrolling artifact.
    //if(lpsrcrect && FromScreen){
    if(FromScreen) {
        if (dxw.IsEmulated) {
            if(dxw.dwFlags1 & BLITFROMBACKBUFFER) {
                LPDIRECTDRAWSURFACE lpDDSBack;
                lpDDSBack = dxwss.GetBackBufferSurface();
                if(lpDDSBack) lpddssrc = lpDDSBack;
            }
        } else {
            if(dxw.dwFlags1 & BLITFROMBACKBUFFER) {
                LPDIRECTDRAWSURFACE lpDDSBack;
                lpDDSBack = dxwss.GetBackBufferSurface();
                if(lpDDSBack) lpddssrc = lpDDSBack;
            } else
                srcrect = dxw.MapWindowRect(lpsrcrect);
        }
    }
    if (IsDebugDW) BlitTrace("NOPRIM", lpsrcrect, lpdestrect, __LINE__);
    res = (*pBlt)(lpdds, lpdestrect, lpddssrc, lpsrcrect ? &srcrect : NULL, dwflags, lpddbltfx);
    // Blitting compressed data may work to screen surfaces only. In this case, it may be worth
    // trying blitting directly to lpDDSEmu_Prim: it makes DK2 intro movies working.
    // Wrong guess!!! The cause was not compression, but simply a pixelformat mismatch. Better
    // configure things properly and avoid this branch.
    switch(res) {
    // commented out: it was the cause of the "Divine Divinity" flickering.
    // commented in?: it seems useful in "Axis and Allies"....
    //case DDERR_UNSUPPORTED:
    //	if (dxw.IsEmulated){
    //		RECT targetrect;
    //		if (IsDebugDW) BlitTrace("UNSUPP", lpsrcrect ? &srcrect : NULL, lpdestrect, __LINE__);
    //		targetrect = dxw.MapWindowRect(lpdestrect);
    //		res=(*pBlt)(lpDDSEmu_Prim, &targetrect, lpddssrc, lpsrcrect ? &srcrect : NULL, dwflags, lpddbltfx);
    //	}
    //	break;
    case DDERR_SURFACEBUSY:
        if (lpsrcrect) srcrect = *lpsrcrect;
        (*pUnlockMethod(dxversion))(lpdds, NULL);
        if (lpddssrc) (*pUnlockMethod(dxversion))(lpddssrc, NULL);
        if (IsDebugDW) BlitTrace("BUSY", lpsrcrect ? &srcrect : NULL, lpdestrect, __LINE__);
        res = (*pBlt)(lpdds, lpdestrect, lpddssrc, lpsrcrect ? &srcrect : NULL, dwflags | DDBLT_WAIT, lpddbltfx);
        break;
    case DDERR_UNSUPPORTED:
        // to be fixed: parameters coming from BltFast in Star Defender 1 are not compatible with Blt transformation, so clear all....
        if(dwflags & (DDBLT_KEYDEST | DDBLT_KEYSRC)) {
            dwflags &= ~(DDBLT_KEYDEST | DDBLT_KEYSRC);
            res = (*pBlt)(lpdds, lpdestrect, lpddssrc, lpsrcrect ? &srcrect : NULL, dwflags, NULL);
        }
        break;
    case DDERR_SURFACELOST:
        lpdds->Restore();
        res = (*pBlt)(lpdds, lpdestrect, lpddssrc, lpsrcrect ? &srcrect : NULL, dwflags, lpddbltfx);
        OutTraceDW("Blt SURFACELOST RETRY: ret=%#x(%s)\n", res, ExplainDDError(res));
        break;
    default:
        break;
    }
#ifndef DXW_NOTRACES
    if (res) BlitError(res, &srcrect, lpdestrect, __LINE__);
    if(IsDebugDW) {
        DescribeSurface(lpdds, 0, "[DST]", __LINE__);
        if (lpddssrc) DescribeSurface(lpddssrc, 0, "[SRC]", __LINE__);  // lpddssrc could be NULL!!!
    }
#endif // DXW_NOTRACES
    if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    if(dxw.dwFlags5 & TEXTUREMASK) {
        // Texture Handling on Blt
        TextureHandling(lpdds, dxversion);
    }
    return res;
}

static HRESULT PrimaryNotEmulated(int dxversion, Blt_Type pBlt, ApiArg, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect,
                                  LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx, BOOL isFlipping) {
    HRESULT res;
    RECT destrect;
    // blit only when source and dest surface are different. Should make ScreenRefresh faster.
    if (lpdds == lpddssrc) return DD_OK;
    if(dxw.Windowize) {
        destrect = dxw.MapWindowRect(lpdestrect);
        lpdestrect = &destrect;
    }
    dxw.ShowOverlay(lpddssrc);
    if (IsDebugDW) BlitTrace("PRIM-NOEMU", lpsrcrect, lpdestrect, __LINE__);
    res = (*pPrimaryBlt)(dxversion, pBlt, lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
    if(res) {
#ifndef DXW_NOTRACES
        BlitError(res, lpsrcrect, lpdestrect, __LINE__);
        if(IsDebugDW) {
            DescribeSurface(lpdds, 0, "[DST]", __LINE__);
            if (lpddssrc) DescribeSurface(lpddssrc, 0, "[SRC]", __LINE__);  // lpddssrc could be NULL!!!
        }
#endif // DXW_NOTRACES
        // Try to handle HDC lock concurrency....
        if(res == DDERR_SURFACEBUSY) {
            (*pUnlockMethod(dxversion))(lpdds, NULL);
            if(lpddssrc) (*pUnlockMethod(dxversion))(lpdds, NULL);
            if (IsDebugDW) BlitTrace("BUSY", lpsrcrect, lpdestrect, __LINE__);
            res = (*pBlt)(lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
            if (res) BlitError(res, lpsrcrect, lpdestrect, __LINE__);
        }
        // Try to handle DDBLT_KEYSRC on primary surface
        if((res == DDERR_INVALIDPARAMS) && (dwflags & DDBLT_KEYSRC)) {
            // to do: handle possible situations with surface 2 / 4 / 7 types
            DDSURFACEDESC2 ddsd;
            LPDIRECTDRAWSURFACE2 lpddsTmp;
            extern CreateSurface2_Type pCreateSurfaceMethod(int);
            if (IsDebugDW) BlitTrace("KEYSRC", lpsrcrect, lpdestrect, __LINE__);
            memset(&ddsd, 0, sizeof(ddsd));
            ddsd.dwSize = (dxversion < 4) ? sizeof(DDSURFACEDESC) : sizeof(DDSURFACEDESC2);
            (*pGetSurfaceDescMethod())((LPDIRECTDRAWSURFACE2)lpddssrc, &ddsd);
            res = (*pCreateSurfaceMethod(dxversion))(lpPrimaryDD, &ddsd, (LPDIRECTDRAWSURFACE *)&lpddsTmp, NULL);
#ifndef DXW_NOTRACES
            if(res) OutTraceE("CreateSurface: ERROR %#x(%s) at %d", res, ExplainDDError(res), __LINE__);
#endif // DXW_NOTRACES
            // copy background
            res = (*pBlt)((LPDIRECTDRAWSURFACE)lpddsTmp, lpsrcrect, lpdds, lpdestrect, DDBLT_WAIT, NULL);
#ifndef DXW_NOTRACES
            if(res) OutTraceE("Blt: ERROR %#x(%s) at %d", res, ExplainDDError(res), __LINE__);
#endif // DXW_NOTRACES
            // overlay texture
            res = (*pBlt)((LPDIRECTDRAWSURFACE)lpddsTmp, lpsrcrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
#ifndef DXW_NOTRACES
            if(res) OutTraceE("Blt: ERROR %#x(%s) at %d", res, ExplainDDError(res), __LINE__);
#endif // DXW_NOTRACES
            // copy back to destination
            res = (*pBlt)(lpdds, lpdestrect, (LPDIRECTDRAWSURFACE)lpddsTmp, lpsrcrect, DDBLT_WAIT, lpddbltfx);
#ifndef DXW_NOTRACES
            if(res) OutTraceE("Blt: ERROR %#x(%s) at %d", res, ExplainDDError(res), __LINE__);
            if (res) BlitError(res, lpsrcrect, lpdestrect, __LINE__);
#endif // DXW_NOTRACES
            (*pReleaseSMethod(dxversion))((LPDIRECTDRAWSURFACE)lpddsTmp);
        }
        if(res == DDERR_SURFACELOST) {
            lpdds->Restore();
            res = (*pPrimaryBlt)(dxversion, pBlt, lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
            OutTraceDW("Blt SURFACELOST RETRY: ret=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        }
        if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    }
    return res;
}

static HRESULT PrimaryEmulated(int dxversion, Blt_Type pBlt, ApiArg, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect,
                               LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx, BOOL isFlipping) {
    HRESULT res;
    RECT destrect, emurect;
    // TODO: this block should be outside and shared with all renderers
    extern HWND CurrentActiveMovieWin;
    if(CurrentActiveMovieWin && (dxw.dwFlags6 & STRETCHMOVIES)) {
        OutDebugDW("PrimaryEmulated: STRETCHMOVIES hwnd=%#x\n", CurrentActiveMovieWin);
        lpdestrect = NULL;
        lpsrcrect = NULL;
    }
    if (lpdestrect)
        emurect = *lpdestrect;
    else {
        // emurect: emulated rect is full surface (dwWidth x dwHeight)
        emurect.left = 0;
        emurect.top = 0;
        emurect.right = dxw.GetScreenWidth();
        emurect.bottom = dxw.GetScreenHeight();
    }
    // v2.04.41.fx1: NULL coordinates conversion needed in all cases, windowized and fullscreen
    destrect = dxw.MapWindowRect(lpdestrect);
    lpdestrect = &destrect;
    res = DD_OK;
    // blit only when source and dest surface are different. Should make ScreenRefresh faster.
    // v2.05.52: deleted optimization. Game "Wings of Destiny" blits primary to primary to make scrolling
    //if (lpdds != lpddssrc){
    //{
    if (IsDebugDW) BlitTrace("SRC2EMU", lpsrcrect, &emurect, __LINE__);
    if(destrect.top == -32000) return DD_OK; // happens when window is minimized & do not notify on task switch ...
    if(lpdds->IsLost()) lpdds->Restore(); // lpDDSEmu_Back could get lost .....
    res = (*pBlt)(lpdds, &emurect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
    //}
    // attempts various recovery fixes ....
    if (res) {
#ifndef DXW_NOTRACES
        BlitError(res, lpsrcrect, &emurect, __LINE__);
        DescribeSurface(lpdds,    0, "[DST]", __LINE__);
        if (lpddssrc) DescribeSurface(lpddssrc, 0, "[SRC]", __LINE__);  // lpddssrc could be NULL!!!
#endif // DXW_NOTRACES
        if (res == DDERR_UNSUPPORTED) {
            switch(dxw.dwFlags10 & (HWFOURCC | SWFOURCC)) {
            case HWFOURCC:
                if(dxwHWBlitUnsupported(dxversion, pBlt, lpdds, &emurect, lpddssrc, lpsrcrect)) res = DD_OK;
                break;
            case SWFOURCC:
                if(dxwSWBlitUnsupported(dxversion, pBlt, lpdds, &emurect, lpddssrc, lpsrcrect)) res = DD_OK;
                break;
            }
        }
        if (res) {
            /*
                Dungeon Keeper II intro movies bug ....
                it seems that you can't blit from compressed or different surfaces in memory,
                while the operation COULD be supported to video. As a mater of fact, it DOES
                work on my PC. The error code is DDERR_UNSUPPORTED.
                v2.02.98: The same thing happens with "New York Racer", but with DDERR_EXCEPTION error code.
                V2.03.15: The same thing happens with "Silent Hunter III", but with DDERR_INVALIDRECT error code.
            */
            if((res == DDERR_UNSUPPORTED) || (res == DDERR_EXCEPTION) || (res == DDERR_INVALIDRECT)) {
                //RECT targetrect;
                dxw.ShowOverlay(lpddssrc);
                if (IsDebugDW) BlitTrace("UNSUPP", &emurect, &destrect, __LINE__);
                //targetrect=dxw.MapWindowRect(&destrect); // v2.03.18
                //res=(*pBlt)(lpDDSEmu_Prim, &targetrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
                res = (*pBlt)(lpDDSEmu_Prim, &destrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
                if (res) BlitError(res, lpsrcrect, &destrect, __LINE__);
                // v2.04.90: TBD (to be investigated): the DDERR_DEVICEDOESNTOWNSURFACE error means that the
                // program re-opened a new ddraw session and is using resources (surfaces) tied to this new
                // one. When dxversion==7 you can reopen the service surfaces (real primary / backbuffer) by
                // using GetDDInterface. It's not clear if the problem may happen when switching to other dx
                // versions and, in that case, how to handle that.
                if((res == DDERR_DEVICEDOESNTOWNSURFACE) && (dxversion == 7)) {
                    if (IsDebugDW) BlitTrace("DOESNTOWN", lpsrcrect, &destrect, __LINE__);
                    LPDIRECTDRAW lpdd;
                    ((LPDIRECTDRAWSURFACE2)lpddssrc)->GetDDInterface((LPVOID *)&lpdd);
                    ReleaseRealSurfaces(dxversion);
                    BuildRealSurfaces(lpdd, (CreateSurface_Type)pCreateSurface7, dxversion);
                    res = (*pBlt)(lpdds, &destrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
                    if (res) BlitError(res, lpsrcrect, &destrect, __LINE__);
                }
            }
            // Try to handle HDC lock concurrency....
            if(res == DDERR_SURFACEBUSY) {
                if (bFlippedDC) {
                    ReleaseDC_Type pReleaseDC;
                    switch(dxversion) {
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
                    (*pReleaseDC)(lpdds, hFlippedDC);
                }
                // v2.03.49: resumed because if fixes locked surfaces on "Red Alert 1" on WinXP as reported by cloudstr
                if(lpddssrc) { // lpddssrc could be NULL!!!
                    res = (*pUnlockMethod(dxversion))(lpddssrc, NULL);
#ifndef DXW_NOTRACES
                    if(res && (res != DDERR_NOTLOCKED)) OutTraceE("Unlock ERROR: lpdds=%#x err=%#x(%s)\n", lpddssrc, res, ExplainDDError(res));
#endif // DXW_NOTRACES
                }
                res = (*pUnlockMethod(dxversion))(lpdds, NULL); // v2.03.24 reintroduced because of "Virtua Cop"
#ifndef DXW_NOTRACES
                if(res && (res != DDERR_NOTLOCKED)) OutTraceE("Unlock ERROR: lpdds=%#x err=%#x(%s)\n", lpdds, res, ExplainDDError(res));
#endif // DXW_NOTRACES
                if (IsDebugDW) BlitTrace("BUSY", &emurect, &destrect, __LINE__);
                res = (*pBlt)(lpdds, &emurect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
                if (res) BlitError(res, lpsrcrect, &destrect, __LINE__);
            }
            if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
            return res;
        }
    }
    LPDIRECTDRAWSURFACE lpDDSSource;
    if (res = (*pColorConversion)(dxversion, lpdds, emurect, &lpDDSSource)) {
        OutTraceE("sBlt ERROR: Color conversion failed res=%#x(%s)\n", res, ExplainDDError(res));
        if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
        return res;
    }
    if(lpDDSEmu_Prim->IsLost()) lpDDSEmu_Prim->Restore();
    dxw.ShowOverlay(lpDDSSource);
    if(dxw.FilterXScalingFactor) {
        emurect.right *= dxw.FilterXScalingFactor;
        emurect.bottom *= dxw.FilterYScalingFactor;
    }
    if (IsDebugDW) BlitTrace("BACK2PRIM", &emurect, &destrect, __LINE__);
    res = (*pPrimaryBlt)(dxversion, pBlt, lpDDSEmu_Prim, &destrect, lpDDSSource, &emurect, DDBLT_WAIT, NULL);
    if (res) BlitError(res, &emurect, &destrect, __LINE__);
    if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    OutDebugDW("%s: done ret=%#x at %d\n", api, res, __LINE__);
    if((dxw.dwDFlags & CAPTURESCREENS) && dxw.bCustomKeyToggle) dxw.ScreenShot();
    return res;
}

HRESULT BlitToNull(int dxversion, Blt_Type pBlt, ApiArg, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect,
                   LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx, BOOL isFlipping) {
    return DD_OK;
}

static HRESULT sBltToPrimary(int dxversion, Blt_Type pBlt, ApiArg, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect,
                             LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx, BOOL isFlipping) {
    HRESULT res;
    extern int gFlippedToScreen;
    // handle minimized states
    if(!dxw.IsVisible) return DD_OK;
    // check coordinates
    dxw.UpdateDesktopCoordinates();
    // debug suppressions
    if(isFlipping) {
        if(dxw.dwDFlags & NODDRAWFLIP) return DD_OK;
    } else {
        if(dxw.dwDFlags & NODDRAWBLT) return DD_OK;
    }
    if (gFlippedToScreen) {
        OutTraceDW("FlippedToScreen SKIP\n");
        gFlippedToScreen = 0;
        return DD_OK;
    }
    // v2.03.48: on WinXP it may happen (reported by Cloudstr) that alt tabbing produces
    // bad blit attempts where the client coordinates get the (-32000,-32000) - (-32000,-32000)
    // value. In such cases, it's adviseable to simulate an OK return code without attempting
    // any blit operation!
    if(lpdestrect && (lpdestrect->left == -32000)) return DD_OK; // no blit on invisible window
#ifdef ONEPIXELFIX
    if (lpdestrect) {
        if ((lpdestrect->top == 0) && (lpdestrect->bottom == dxw.GetScreenHeight() - 1)) lpdestrect->bottom = dxw.GetScreenHeight();
        if ((lpdestrect->left == 0) && (lpdestrect->right  == dxw.GetScreenWidth()  - 1)) lpdestrect->right  = dxw.GetScreenWidth();
    }
    if (lpsrcrect) {
        if ((lpsrcrect->top == 0) && (lpsrcrect->bottom == dxw.GetScreenHeight() - 1)) lpsrcrect->bottom = dxw.GetScreenHeight();
        if ((lpsrcrect->left == 0) && (lpsrcrect->right  == dxw.GetScreenWidth()  - 1)) lpsrcrect->right  = dxw.GetScreenWidth();
    }
#endif
    // seems necessary to "cure" the "FIFA 2000" soccer game in hw accelerated graphics, when the Unlock() method
    // receives RECT coordinates with big positive or negative numbers!
    // v2.03.94fx1: "FIFA2002" is ok with Unlock rect set to Lock previous value, but other games ("Aztec Wars", ...)
    // require some software clipping when moving the cursor outside the window or similar cases
    if(lpdestrect) {
        if(lpdestrect->top < 0) lpdestrect->top = 0;
        if(lpdestrect->top > (LONG)dxw.GetScreenHeight()) lpdestrect->top = dxw.GetScreenHeight();
        if(lpdestrect->left < 0) lpdestrect->left = 0;
        if(lpdestrect->left > (LONG)dxw.GetScreenWidth()) lpdestrect->left = (LONG)dxw.GetScreenWidth();
        if(lpdestrect->bottom > (LONG)dxw.GetScreenHeight()) lpdestrect->bottom = dxw.GetScreenHeight();
        if(lpdestrect->right > (LONG)dxw.GetScreenWidth()) lpdestrect->right = dxw.GetScreenWidth();
        if(lpdestrect->bottom < lpdestrect->top) lpdestrect->bottom = lpdestrect->top;
        if(lpdestrect->right < lpdestrect->left) lpdestrect->right = lpdestrect->left;
    }
    if(dxw.dwFlags5 & QUARTERBLT) {
        BOOL QuarterUpdate;
        QuarterUpdate = lpdestrect ?
                        (((lpdestrect->bottom - lpdestrect->top) * (lpdestrect->right - lpdestrect->left)) > ((LONG)(dxw.GetScreenHeight() * dxw.GetScreenWidth()) >> 2))
                        :
                        TRUE;
        if(QuarterUpdate) if(dxw.HandleFPS()) return DD_OK;
    } else {
        if(isFlipping || !(dxw.dwFlags12 & LIMITFLIPONLY)) {
            if(dxw.HandleFPS()) return DD_OK;
        }
    }
    //if(!(lpddssrc || (dwflags & DDBLT_COLORFILL))) {
    if((lpddssrc == 0) && !(dwflags & DDBLT_COLORFILL)) {
        lpddssrc = dxwss.GetBackBufferSurface();
        OutTraceDW("Flip: setting flip chain to lpdds=%#x\n", lpddssrc);
    }
    // =========================
    // Blit to primary direct or fullscreen surface
    // =========================
    if((dxw.dwFlags1 & SAVELOAD) || (dxw.dwFlags8 & FORCEVSYNC)) {
        HRESULT res2;
        res2 = lpPrimaryDD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0);
#ifndef DXW_NOTRACES
        if(res2) OutTraceE("WaitForVerticalBlank ERROR: res=%#x\n", res2);
#endif // DXW_NOTRACES
    }
    res = (*pRendererBlt)(dxversion, pBlt, api, lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx, isFlipping);
    return res;
}

HRESULT WINAPI sBlt(int dxversion, Blt_Type pBlt, ApiArg, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect,
                    LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx, BOOL isFlipping) {
    POINT p = {0, 0};
    HRESULT res;
    BOOL ToPrim, FromPrim, ToScreen, FromScreen;
    if(dxw.dwFlags5 & MESSAGEPUMP) dxw.MessagePump();
    ToPrim = dxwss.IsAPrimarySurface(lpdds);
    FromPrim = dxwss.IsAPrimarySurface(lpddssrc);
    ToScreen = ToPrim && !(dxw.IsEmulated);
    FromScreen = FromPrim && !(dxw.IsEmulated); // v2.02.77
    // log
#ifndef DXW_NOTRACES
    if(IsTraceDDRAW) {
        char sLog[256];
        char sInfo[128];
        sprintf(sLog, "%s(%d): dest=%#x%s src=%#x%s dwFlags=%#x(%s)",
                api, dxversion, lpdds, dxwss.ExplainSurfaceRole(lpdds), lpddssrc, dxwss.ExplainSurfaceRole(lpddssrc), dwflags, ExplainBltFlags(dwflags));
        if (lpdestrect)
            sprintf(sInfo, " destrect=(%d,%d)-(%d,%d)", lpdestrect->left, lpdestrect->top, lpdestrect->right, lpdestrect->bottom);
        else
            sprintf(sInfo, " destrect=(NULL)");
        strcat(sLog, sInfo);
        if (lpsrcrect)
            sprintf(sInfo, " srcrect=(%d,%d)-(%d,%d)", lpsrcrect->left, lpsrcrect->top, lpsrcrect->right, lpsrcrect->bottom);
        else
            sprintf(sInfo, " srcrect=(NULL)");
        strcat(sLog, sInfo);
        if(lpddbltfx) {
            if (dwflags & DDBLT_COLORFILL) {
                sprintf(sInfo, " ddbltfx.FillColor=%#x", lpddbltfx->dwFillColor);
                strcat(sLog, sInfo);
            }
            if (dwflags & DDBLT_KEYDESTOVERRIDE) {
                sprintf(sInfo, " ddbltfx.DestColorkey=%#x", lpddbltfx->ddckDestColorkey);
                strcat(sLog, sInfo);
            }
            if (dwflags & DDBLT_KEYSRCOVERRIDE) {
                sprintf(sInfo, " ddbltfx.SrcColorkey=%#x", lpddbltfx->ddckSrcColorkey);
                strcat(sLog, sInfo);
            }
            if (dwflags & DDBLT_ROP) {
                sprintf(sInfo, " ddbltfx.ROP=%#x", lpddbltfx->dwROP);
                strcat(sLog, sInfo);
            }
            if (dwflags & DDBLT_DEPTHFILL) {
                sprintf(sInfo, " ddbltfx.FillDepth=%#x", lpddbltfx->dwFillDepth);
                strcat(sLog, sInfo);
            }
        }
        strcat(sLog, "\n");
        OutTrace(sLog);
    }
#endif // DXW_NOTRACES
    if(dxw.dwFlags8 & FORCEWAIT) {
        dwflags |= DDBLT_WAIT;
        dwflags &= ~(DDBLT_ASYNC | DDBLT_DONOTWAIT);
    }
    if(dxw.dwFlags8 & FORCENOWAIT) {
        dwflags &= ~DDBLT_WAIT;
        dwflags |= (DDBLT_ASYNC | DDBLT_DONOTWAIT);
    }
    if((dxw.dwFlags10 & HANDLEFOURCC) && (dwflags & DDBLT_COLORFILL)) return DD_OK;
    if(ToPrim)
        res = sBltToPrimary(dxversion, pBlt, api, lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx, isFlipping);
    else
        res = sBltNoPrimary(dxversion, pBlt, api, lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
    return res;
}

void SetRendererCallbacks() {
    extern Blitter_Type  BlitToWindow;
    extern Blitter_Type  SDL_BlitToWindow;
    extern Blitter_Type  SDL2_BlitToWindow;
    extern Blitter_Type  OGL_BlitToWindow;
    switch(dxw.RendererId) {
    case DXW_RENDERER_NONE:
    case DXW_RENDERER_LOCKEDSURFACE:
        pRendererBlt = PrimaryNotEmulated;
        break;
    case DXW_RENDERER_GDI:
        pRendererBlt = BlitToWindow;
        break;
    case DXW_RENDERER_SDL:
        pRendererBlt = SDL_BlitToWindow;
        break;
    case DXW_RENDERER_SDL2:
        pRendererBlt = SDL2_BlitToWindow;
        break;
    case DXW_RENDERER_OPENGL:
        pRendererBlt = OGL_BlitToWindow;
        break;
    case DXW_RENDERER_PRIMSURFACE:
    case DXW_RENDERER_EMULATEBUFFER:
    case DXW_RENDERER_HYBRID:
        pRendererBlt = PrimaryEmulated;
        break;
    default:
        pRendererBlt = BlitToNull;
        break;
    }
}