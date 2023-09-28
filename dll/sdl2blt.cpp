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
#include "sdl2/SDL.h"

extern GetDC_Type pGetDCMethod();
extern ReleaseDC_Type pReleaseDCMethod();
extern Lock_Type pLockMethod(int);
extern Unlock4_Type pUnlockMethod(int);
extern SetClipper_Type pSetClipperMethod(int);
extern LPVOID dxwConvertFourCC(LPDDSURFACEDESC2);

typedef int (* SDL_Init_Type)(Uint32);
typedef void (* SDL_FreeSurface_Type)(SDL_Surface *);
typedef int (* SDL_SoftStretch_Type)(SDL_Surface *, const SDL_Rect *, SDL_Surface *, SDL_Rect *);
typedef SDL_Surface *(* SDL_CreateRGBSurfaceFrom_Type)(void *, int, int, int, int, Uint32, Uint32, Uint32, Uint32);
typedef SDL_Surface *(* SDL_ConvertSurfaceFormat_Type)(SDL_Surface *, Uint32, Uint32);
typedef SDL_Surface *(* SDL_ConvertSurface_Type)(SDL_Surface *, SDL_PixelFormat *, Uint32);
typedef SDL_Surface *(* SDL_GetWindowSurface_Type)(SDL_Window *);
typedef SDL_Window *(* SDL_CreateWindowFrom_Type)(const void *);
typedef SDL_Palette *(* SDL_AllocPalette_Type)(int);
typedef void (* SDL_FreePalette_Type)(SDL_Palette *);
typedef int (* SDL_SetPaletteColors_Type)(SDL_Palette *, const SDL_Color *, int, int);
typedef int (* SDL_UpdateWindowSurface_Type)(SDL_Window *);
typedef int (* SDL_SetPixelFormatPalette_Type)(SDL_PixelFormat *, SDL_Palette *);

HRESULT SDL2_BlitToWindow(int dxversion, Blt_Type pBlt, char *api, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect,
                          LPDIRECTDRAWSURFACE s, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx, BOOL isFlipping) {
    static BOOL bInitialized = FALSE;
    static SDL_Surface *gSDL_VideoSurface = NULL;
    static SDL_Window *gSDL_Window = NULL;
    SDL_Surface *SDL_DDPrimary;
    SDL_Surface *SDL_Converted;
    static LONG CurW = 0, CurH = 0;
    //RECT Rect;
    DDSURFACEDESC2 ddsd;
    HRESULT res;
    HWND w;
    static SDL_Init_Type pSDL_Init;
    static SDL_FreeSurface_Type pSDL_FreeSurface;
    static SDL_SoftStretch_Type pSDL_SoftStretch = NULL;
    static SDL_CreateRGBSurfaceFrom_Type pSDL_CreateRGBSurfaceFrom;
    static SDL_ConvertSurfaceFormat_Type pSDL_ConvertSurfaceFormat;
    static SDL_ConvertSurface_Type pSDL_ConvertSurface;
    static SDL_GetWindowSurface_Type pSDL_GetWindowSurface;
    static SDL_CreateWindowFrom_Type pSDL_CreateWindowFrom;
    static SDL_AllocPalette_Type pSDL_AllocPalette;
    static SDL_FreePalette_Type pSDL_FreePalette;
    static SDL_SetPaletteColors_Type pSDL_SetPaletteColors;
    static SDL_UpdateWindowSurface_Type pSDL_UpdateWindowSurface;
    static SDL_SetPixelFormatPalette_Type pSDL_SetPixelFormatPalette;
    SDL_Palette *SDLPalette = NULL;
    if(!s) return DD_FALSE; // for surface color fill
    w = dxw.GethWnd();
    // initialization, just once and for all
    if(!bInitialized) {
        HMODULE hSDL;
        // load SDL function pointers
        hSDL = (*pLoadLibraryA)("SDL2.dll");
        if(!hSDL) {
            OutTraceE("%s: load SDL2.dll FAILED\n", api);
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
        pSDL_CreateRGBSurfaceFrom = (SDL_CreateRGBSurfaceFrom_Type)(*pGetProcAddress)(hSDL, "SDL_CreateRGBSurfaceFrom");
        if(!pSDL_CreateRGBSurfaceFrom) {
            OutTraceE("%s: initialize SDL_CreateRGBSurfaceFrom FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_ConvertSurfaceFormat = (SDL_ConvertSurfaceFormat_Type)(*pGetProcAddress)(hSDL, "SDL_ConvertSurfaceFormat");
        if(!pSDL_ConvertSurfaceFormat) {
            OutTraceE("%s: initialize SDL_ConvertSurfaceFormat FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_ConvertSurface = (SDL_ConvertSurface_Type)(*pGetProcAddress)(hSDL, "SDL_ConvertSurface");
        if(!pSDL_ConvertSurface) {
            OutTraceE("%s: initialize SDL_ConvertSurface FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_SoftStretch = (SDL_SoftStretch_Type)(*pGetProcAddress)(hSDL, "SDL_SoftStretch");
        if(!pSDL_SoftStretch) {
            OutTraceE("%s: initialize SDL_SoftStretch FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_GetWindowSurface = (SDL_GetWindowSurface_Type)(*pGetProcAddress)(hSDL, "SDL_GetWindowSurface");
        if(!pSDL_GetWindowSurface) {
            OutTraceE("%s: initialize SDL_GetWindowSurface FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_CreateWindowFrom = (SDL_CreateWindowFrom_Type)(*pGetProcAddress)(hSDL, "SDL_CreateWindowFrom");
        if(!pSDL_CreateWindowFrom) {
            OutTraceE("%s: initialize SDL_CreateWindowFrom FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_AllocPalette = (SDL_AllocPalette_Type)(*pGetProcAddress)(hSDL, "SDL_AllocPalette");
        if(!pSDL_AllocPalette) {
            OutTraceE("%s: initialize SDL_AllocPalette FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_FreePalette = (SDL_FreePalette_Type)(*pGetProcAddress)(hSDL, "SDL_FreePalette");
        if(!pSDL_FreePalette) {
            OutTraceE("%s: initialize SDL_FreePalette FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_SetPaletteColors = (SDL_SetPaletteColors_Type)(*pGetProcAddress)(hSDL, "SDL_SetPaletteColors");
        if(!pSDL_SetPaletteColors) {
            OutTraceE("%s: initialize SDL_SetPaletteColors FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_UpdateWindowSurface = (SDL_UpdateWindowSurface_Type)(*pGetProcAddress)(hSDL, "SDL_UpdateWindowSurface");
        if(!pSDL_UpdateWindowSurface) {
            OutTraceE("%s: initialize SDL_UpdateWindowSurface FAILED\n", api);
            return DD_FALSE;
        }
        pSDL_SetPixelFormatPalette = (SDL_SetPixelFormatPalette_Type)(*pGetProcAddress)(hSDL, "SDL_SetPixelFormatPalette");
        if(!pSDL_SetPixelFormatPalette) {
            OutTraceE("%s: initialize SDL_SetPixelFormatPalette FAILED\n", api);
            return DD_FALSE;
        }
        LONG pWindowProc;
        pWindowProc = (*pGetWindowLong)(dxw.GethWnd(), GWL_WNDPROC);
        (*pSDL_Init)(SDL_INIT_VIDEO);
        gSDL_Window = (*pSDL_CreateWindowFrom)((const void *)w);
        if(!gSDL_Window) {
            OutTraceE("%s: ERROR in SDL_CreateWindowFrom at %d\n", api, __LINE__);
            (*pUnlockMethod(dxversion))(s, NULL);
            return DD_FALSE;
        }
        (WNDPROC)(*pSetWindowLong)(dxw.GethWnd(), GWL_WNDPROC, pWindowProc);
        bInitialized = TRUE;
        OutTraceSDL("%s: SDL2 initialized OK\n", api);
        // clear clipper
        LPDIRECTDRAWCLIPPER lpddc;
        s->GetClipper(&lpddc);
        if(lpddc) (*pSetClipperMethod(dxversion))(s, NULL);
    }
    gSDL_VideoSurface = (*pSDL_GetWindowSurface)(gSDL_Window);
    if(!gSDL_VideoSurface) {
        OutTraceE("%s: ERROR in SDL_GetWindowSurface at %d\n", api, __LINE__);
        (*pUnlockMethod(dxversion))(s, NULL);
        return DD_FALSE;
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
        SDL_Color SDLPaletteBuffer[256];
        for(int i = 0; i < 256; i++) {
            BYTE *b = (BYTE *)&PaletteEntries[i];
            BYTE *c = (BYTE *)&SDLPaletteBuffer[i];
            c[0] = b[2];
            c[1] = b[1];
            c[2] = b[0];
            c[3] = 0; // unused
        }
        SDLPalette = (*pSDL_AllocPalette)(256);
        if(!SDLPalette) {
            OutTraceE("%s: SDL_AllocPalette ERROR\n", api);
            (*pSDL_FreeSurface)(SDL_DDPrimary);
            return DD_FALSE;
        }
        if((*pSDL_SetPaletteColors)(SDLPalette, SDLPaletteBuffer, 0, 256)) {
            OutTraceE("%s: SDL_SetPaletteColors ERROR\n", api);
            (*pSDL_FreeSurface)(SDL_DDPrimary);
            return DD_FALSE;
        }
        if((*pSDL_SetPixelFormatPalette)(SDL_DDPrimary->format, SDLPalette)) {
            OutTraceE("%s: SDL_SetPixelFormatPalette ERROR\n", api);
            (*pSDL_FreeSurface)(SDL_DDPrimary);
            return DD_FALSE;
        }
    }
    // convert to display pixel format
    SDL_Converted = (*pSDL_ConvertSurfaceFormat)(SDL_DDPrimary, SDL_PIXELFORMAT_RGB888, 0);
    //SDL_Converted = (*pSDL_ConvertSurfaceFormat)(SDL_DDPrimary, SDL_PIXELFORMAT_UNKNOWN, 0);
    if(!SDL_Converted) {
        OutTraceE("%s: SDL_ConvertSurfaceFormat ERROR\n", api);
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
                                 lpdestrect ? &drect : NULL)) {
        OutTraceE("%s: SDL_SoftStretch ERROR err=%#x at %d\n", api, res, __LINE__);
        (*pSDL_FreeSurface)(SDL_DDPrimary);
        return DD_FALSE;
    }
    if((*pSDL_UpdateWindowSurface)(gSDL_Window)) {
        OutTraceE("%s: SDL_UpdateWindowSurface ERROR err=%#x\n", api, res);
        (*pSDL_FreeSurface)(SDL_DDPrimary);
        return DD_FALSE;
    }
    // unlock
    res = (*pUnlockMethod(dxversion))(s, NULL);
#ifndef DXW_NOTRACES
    if (res) OutTraceE("%s: Unlock ERROR res=%#x(%s) at %d\n", api, res, ExplainDDError(res), __LINE__);
#endif // DXW_NOTRACES
    // free
    (*pSDL_FreeSurface)(SDL_DDPrimary);
    (*pSDL_FreeSurface)(SDL_Converted);
    if(SDLPalette)(*pSDL_FreePalette)(SDLPalette);
    return DD_OK;
}