#define _CRT_SECURE_NO_WARNINGS
#define INITGUID
//#define FULLHEXDUMP

#include <windows.h>
#include <ddraw.h>
#include "dxwnd.h"
#include "ddrawi.h"
#include "dxwcore.hpp"
#include "stdio.h"
#include "hddraw.h"
#include "dxhelper.h"
#include "syslibs.h"
#include "sdl/SDL.h"

extern GetDC_Type pGetDCMethod();
extern ReleaseDC_Type pReleaseDCMethod();
extern Lock_Type pLockMethod(int);
extern Unlock4_Type pUnlockMethod(int);
extern SetClipper_Type pSetClipperMethod(int);
extern LPVOID dxwConvertFourCC(LPDDSURFACEDESC2);

typedef int (* SDL_Init_Type)(Uint32);
typedef SDL_Surface *(* SDL_SetVideoMode_Type)(int, int, int, Uint32);
typedef void (* SDL_FreeSurface_Type)(SDL_Surface *);
typedef int (* SDL_SoftStretch_Type)(SDL_Surface *, const SDL_Rect *, SDL_Surface *, SDL_Rect *);
typedef SDL_Surface *(* SDL_CreateRGBSurfaceFrom_Type)(void *, int, int, int, int, Uint32, Uint32, Uint32, Uint32);
typedef SDL_Surface *(* SDL_DisplayFormatAlpha_Type)(SDL_Surface *);
typedef void (* SDL_UpdateRect_Type)(SDL_Surface *, int, int, int, int);
typedef int (* SDL_SetPalette_Type)(SDL_Surface *, int, SDL_Color *, int, int);
typedef SDL_Surface *(* SDL_ConvertSurface_Type)(SDL_Surface *, SDL_PixelFormat *, Uint32);

HRESULT SDL_BlitToWindow(int dxversion, Blt_Type pBlt, char *api, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect,
                         LPDIRECTDRAWSURFACE s, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx, BOOL isFlipping) {
    static BOOL bInitialized = FALSE;
    static SDL_Surface *gSDL_VideoSurface = NULL;
    SDL_Surface *SDL_DDPrimary;
    SDL_Surface *SDL_Converted;
    static LONG CurW = 0, CurH = 0;
    DDSURFACEDESC2 ddsd;
    HRESULT res;
    HWND w;
    static SDL_Init_Type pSDL_Init;
    static SDL_SetVideoMode_Type pSDL_SetVideoMode;
    static SDL_FreeSurface_Type pSDL_FreeSurface;
    static SDL_SoftStretch_Type pSDL_SoftStretch = NULL;
    static SDL_CreateRGBSurfaceFrom_Type pSDL_CreateRGBSurfaceFrom;
    static SDL_DisplayFormatAlpha_Type pSDL_DisplayFormatAlpha;
    static SDL_UpdateRect_Type pSDL_UpdateRect;
    static SDL_SetPalette_Type pSDL_SetPalette;
    static SDL_ConvertSurface_Type pSDL_ConvertSurface;
    if(!s) return DD_FALSE; // for surface color fill
    w = dxw.GethWnd();
    // initialization, just once and for all
    if(!bInitialized) {
        HMODULE hSDL;
        // load SDL function pointers
        hSDL = (*pLoadLibraryA)("SDL.dll");
        if(!hSDL) {
            OutTraceE("%s: load SDL.dll FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_Init = (SDL_Init_Type)(*pGetProcAddress)(hSDL, "SDL_Init");
        if(!pSDL_Init) {
            OutTraceE("%s: initialize SDL_Init FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_FreeSurface = (SDL_FreeSurface_Type)(*pGetProcAddress)(hSDL, "SDL_FreeSurface");
        if(!pSDL_FreeSurface) {
            OutTraceE("%s: initialize SDL_FreeSurface FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_SetVideoMode = (SDL_SetVideoMode_Type)(*pGetProcAddress)(hSDL, "SDL_SetVideoMode");
        if(!pSDL_SetVideoMode) {
            OutTraceE("%s: initialize SDL_SetVideoMode FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_CreateRGBSurfaceFrom = (SDL_CreateRGBSurfaceFrom_Type)(*pGetProcAddress)(hSDL, "SDL_CreateRGBSurfaceFrom");
        if(!pSDL_CreateRGBSurfaceFrom) {
            OutTraceE("%s: initialize SDL_CreateRGBSurfaceFrom FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_DisplayFormatAlpha = (SDL_DisplayFormatAlpha_Type)(*pGetProcAddress)(hSDL, "SDL_DisplayFormatAlpha");
        if(!pSDL_DisplayFormatAlpha) {
            OutTraceE("%s: initialize SDL_DisplayFormatAlpha FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_ConvertSurface = (SDL_ConvertSurface_Type)(*pGetProcAddress)(hSDL, "SDL_ConvertSurface");
        if(!pSDL_ConvertSurface) {
            OutTraceE("%s: initialize SDL_ConvertSurface FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_UpdateRect = (SDL_UpdateRect_Type)(*pGetProcAddress)(hSDL, "SDL_UpdateRect");
        if(!pSDL_UpdateRect) {
            OutTraceE("%s: initialize SDL_UpdateRect FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_SetPalette = (SDL_SetPalette_Type)(*pGetProcAddress)(hSDL, "SDL_SetPalette");
        if(!pSDL_SetPalette) {
            OutTraceE("%s: initialize SDL_SetPalette FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_SoftStretch = (SDL_SoftStretch_Type)(*pGetProcAddress)(hSDL, "SDL_SoftStretch");
        if(!pSDL_SoftStretch) {
            OutTraceE("%s: initialize SDL_SoftStretch FAILED\n", api);
            return DD_FALSE;
        }
        // SDL_WINDOWID hack: tells DL to use an existing window
        // BEWARE: must be placed before SDL_Init!
        // BEWARE: it seems that SDL_Init may alter the window callback, better save & recover
        // the original value after the operation.
        {
            char sHack[81];
            sprintf(sHack, "SDL_WINDOWID=%d", dxw.GethWnd());
            _putenv(sHack);
        }
        LONG pWindowProc;
        pWindowProc = (*pGetWindowLong)(dxw.GethWnd(), GWL_WNDPROC);
        (*pSDL_Init)(SDL_INIT_VIDEO);
        (WNDPROC)(*pSetWindowLong)(dxw.GethWnd(), GWL_WNDPROC, pWindowProc);
        bInitialized = TRUE;
        OutTraceSDL("%s: initialized OK\n", api);
        // clear clipper
        LPDIRECTDRAWCLIPPER lpddc;
        s->GetClipper(&lpddc);
        if(lpddc) (*pSetClipperMethod(dxversion))(s, NULL);
    }
    if((dxw.iSizX != CurW) || (dxw.iSizY != CurH)) {
        if(pSDL_FreeSurface)(*pSDL_FreeSurface)(gSDL_VideoSurface);
        CurW = dxw.iSizX;
        CurH = dxw.iSizY;
        gSDL_VideoSurface = (*pSDL_SetVideoMode)(CurW, CurH, 0, 0); // args don't matter, they inherit main hwnd ones
        if(!gSDL_VideoSurface) {
            OutTraceE("%s: initialize gSDL_VideoSurface FAILED\n", api);
            return DD_FALSE;
        }
        OutTraceSDL("%s: set video hwnd=%#x size=(%dx%d)\n", api, w, CurW, CurH);
    }
    // get surface infos & lock
    memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = Set_dwSize_From_Surface();
    ddsd.dwFlags = DDSD_LPSURFACE | DDSD_PITCH;
    if(res = (*pLockMethod(dxversion))(s, 0, (LPDDSURFACEDESC)&ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_READONLY, 0)) {
        OutTraceE("%s: Lock ERROR res=%#x(%s) at %d\n", api, res, ExplainDDError(res), __LINE__);
        (*pUnlockMethod(dxversion))(s, NULL);
        return DD_FALSE;
    }
    OutTraceSDL("%s: surface size=(%dx%d) data=%#x pitch=%#x bpp=%d fourcc=%#x rgba=%#x.%#x.%#x.%#x\n",
                api, ddsd.dwWidth, ddsd.dwHeight, ddsd.lpSurface, ddsd.lPitch,
                ddsd.ddpfPixelFormat.dwRGBBitCount, ddsd.ddpfPixelFormat.dwFourCC,
                ddsd.ddpfPixelFormat.dwRBitMask, ddsd.ddpfPixelFormat.dwGBitMask, ddsd.ddpfPixelFormat.dwBBitMask,
                ddsd.ddpfPixelFormat.dwRGBAlphaBitMask);
    // convert FourCC format (if possible)
    LPVOID pixels = ddsd.lpSurface;
    if(ddsd.ddpfPixelFormat.dwFourCC) pixels = dxwConvertFourCC(&ddsd);
    if(!pixels) {
        OutTraceE("%s: ConvertFourCC ERROR\n", api);
        return DD_FALSE;
    }
    // map to SDL surface
    SDL_DDPrimary = (*pSDL_CreateRGBSurfaceFrom)(
                        pixels,
                        ddsd.dwWidth,
                        ddsd.dwHeight,
                        ddsd.ddpfPixelFormat.dwRGBBitCount,
                        ddsd.lPitch,
                        ddsd.ddpfPixelFormat.dwRBitMask,
                        ddsd.ddpfPixelFormat.dwGBitMask,
                        ddsd.ddpfPixelFormat.dwBBitMask,
                        ddsd.ddpfPixelFormat.dwRGBAlphaBitMask);
    // clean up
    if(pixels != ddsd.lpSurface) free(pixels);
    if(!SDL_DDPrimary) {
        OutTraceE("%s: ERROR in SDL_CreateRGBSurfaceFrom at %d\n", api, __LINE__);
        (*pUnlockMethod(dxversion))(s, NULL);
        return DD_FALSE;
    }
    if(ddsd.ddpfPixelFormat.dwRGBBitCount == 8) {
        extern DWORD PaletteEntries[256];
        SDL_Color SDLPalette[256];
        for(int i = 0; i < 256; i++) {
            BYTE *b = (BYTE *)&PaletteEntries[i];
            BYTE *c = (BYTE *)&SDLPalette[i];
            c[0] = b[2];
            c[1] = b[1];
            c[2] = b[0];
            c[3] = 0; // unused
        }
#ifndef DXW_NOTRACES
        if(!(*pSDL_SetPalette)(SDL_DDPrimary, SDL_LOGPAL | SDL_PHYSPAL, SDLPalette, 0, 256))
            OutTraceE("%s: SDL_SetPalette ERROR\n", api);
#else
        (*pSDL_SetPalette)(SDL_DDPrimary, SDL_LOGPAL | SDL_PHYSPAL, SDLPalette, 0, 256);
#endif // DXW_NOTRACES
    }
    // convert to display pixel format
    SDL_Converted = (*pSDL_DisplayFormatAlpha)(SDL_DDPrimary);
    if(!SDL_Converted) {
        OutTraceE("%s: SDL_DisplayFormatAlpha ERROR\n", api);
        (*pSDL_FreeSurface)(SDL_DDPrimary);
        return DD_FALSE;
    }
    // SDL_blit to screen
    SDL_Rect drect, srect;
    if(lpdestrect) {
        RECT rect = *lpdestrect;
        dxw.MapClient(&rect);
        drect.x = (Sint16)rect.left;
        drect.y = (Sint16)rect.top;
        drect.w = (Sint16)(rect.right - rect.left);
        drect.h = (Sint16)(rect.bottom - rect.top);
    }
    if(lpsrcrect) {
        srect.x = (Sint16)lpsrcrect->left;
        srect.y = (Sint16)lpsrcrect->top;
        srect.w = (Sint16)(lpsrcrect->right - lpsrcrect->left);
        srect.h = (Sint16)(lpsrcrect->bottom - lpsrcrect->top);
    }
    if(res = (*pSDL_SoftStretch)(SDL_Converted,
                                 lpsrcrect ? &srect : NULL,
                                 gSDL_VideoSurface,
                                 lpdestrect ? &drect : NULL))
        OutTraceE("%s: SDL_SoftStretch ERROR err=%#x at %d\n", api, res, __LINE__);
    if(lpdestrect)
        (*pSDL_UpdateRect)(gSDL_VideoSurface, drect.x, drect.y, drect.w, drect.h);
    else
        (*pSDL_UpdateRect)(gSDL_VideoSurface, 0, 0, 0, 0);
    // unlock
    res = (*pUnlockMethod(dxversion))(s, NULL);
#ifndef DXW_NOTRACES
    if (res) OutTraceE("%s: Unlock ERROR res=%#x(%s) at %d\n", api, res, ExplainDDError(res), __LINE__);
#endif // DXW_NOTRACES
    // free
    (*pSDL_FreeSurface)(SDL_DDPrimary);
    (*pSDL_FreeSurface)(SDL_Converted);
    return DD_OK;
}