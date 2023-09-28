#include <windows.h>
#include <ddraw.h>
#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "hddraw.h"

//#define USEOPENMP
#define USE_SCALE2X_SSE2

#define SSE_ALIGN(ptr) ((DWORD *)((DWORD)(((char *)ptr)+0x0000000F) & 0xFFFFFFF0))
#define SSE_SIZE(siz)  ((siz)+16)

typedef HRESULT (WINAPI *Lock_Type)(LPDIRECTDRAWSURFACE, LPRECT, LPDDSURFACEDESC, DWORD, HANDLE);
typedef HRESULT (WINAPI *Unlock4_Type)(LPDIRECTDRAWSURFACE, LPRECT);
typedef HRESULT (WINAPI *Unlock1_Type)(LPDIRECTDRAWSURFACE, LPVOID);

typedef void (* hqx_Type)(DWORD *, int, DWORD *, int, int, int);
typedef void (* hqxInit_Type)(void);
typedef void (* Scale2X_Type)(unsigned, void *, unsigned, const void *, unsigned, unsigned, unsigned, unsigned);

extern Lock_Type pLockMethod(int);
extern Unlock4_Type pUnlockMethod(int);

extern DWORD PaletteEntries[256];
extern DWORD *Palette16BPP;
extern char *ExplainDDError(DWORD);

// just in case ....
#define SwitchdwSize(s) s.dwSize=(s.dwSize==sizeof(DDSURFACEDESC))?sizeof(DDSURFACEDESC2):sizeof(DDSURFACEDESC)

#define MARKBLITCOLOR32 0x00FFFF00
#define MARKBLITCOLOR16 0x0FF0
EmuBlt_Type pEmuBlt;
EmuGamma_Type pGammaCorrection;
PrimaryBlt_Type pPrimaryBlt;

extern Blt_Type pBlt;

//--------------------------------------------------------------------------------------------//
// Emulated blitting procedures: fills a 32BPP surface from the content of 8BPP or 16BPP ones.
//--------------------------------------------------------------------------------------------//

static DWORD Melt32(DWORD c1, DWORD c2) {
    DWORD ret;
    ret =
        ((((c1 & 0x0000FF) + (c2 & 0x0000FF)) >> 1) & 0x0000FF) |
        ((((c1 & 0x00FF00) + (c2 & 0x00FF00)) >> 1) & 0x00FF00) |
        ((((c1 & 0xFF0000) + (c2 & 0xFF0000)) >> 1) & 0xFF0000);
    return ret;
}

static DWORD Melt16_555(DWORD c1, DWORD c2) {
    DWORD ret;
    ret =
        ((((c1 & 0x00001F) + (c2 & 0x00001F)) >> 1) & 0x00001F) |
        ((((c1 & 0x0003E0) + (c2 & 0x0003E0)) >> 1) & 0x0003E0) |
        ((((c1 & 0x007C00) + (c2 & 0x007C00)) >> 1) & 0x007C00);
    return ret;
}

static DWORD Melt16_565(DWORD c1, DWORD c2) {
    DWORD ret;
    ret =
        ((((c1 & 0x00001F) + (c2 & 0x00001F)) >> 1) & 0x00001F) |
        ((((c1 & 0x0007E0) + (c2 & 0x0007E0)) >> 1) & 0x0007E0) |
        ((((c1 & 0x00F800) + (c2 & 0x00F800)) >> 1) & 0x00F800);
    return ret;
}

void SetPalette16BPP() {
    // OutTraceDW("DEBUG: h=%d w=%d src=%#x dst=%#x spitch=%d dpitch=%d\n",h,w,src16,dest,srcpitch,destpitch);
    unsigned int pi;
    Palette16BPP = (DWORD *)malloc(0x10000 * sizeof(DWORD));
    if (dxw.dwFlags3 & BLACKWHITE) {
        // actually, it should be like this: R/G/B = (red * 0.30) + (green * 0.59) + (blue * 0.11)
        // (http://www.codeproject.com/Articles/66253/Converting-Colors-to-Gray-Shades)
        DWORD grey;
        if (dxw.dwFlags1 & USERGB565) {
            for (pi = 0; pi < 0x10000; pi++) {
                //grey = ((((pi & 0x1F)<<3) + ((pi & 0x7E0)>>3) + ((pi & 0xF800)>>8)) / 3) & 0xFF;
                grey = (((((pi & 0x1F) << 3) * 30) + (((pi & 0x7E0) >> 3) * 59) + (((pi & 0xF800) >> 8) * 11)) / 100) & 0xFF;
                Palette16BPP[pi] = (grey) + (grey << 8) + (grey << 16);
            }
        } else {
            for (pi = 0; pi < 0x10000; pi++) {
                //grey = ((((pi & 0x1F)<<3) + ((pi & 0x3E0)>>2) + ((pi & 0x7C00)>>7)) / 3) & 0xFF;
                grey = (((((pi & 0x1F) << 3) * 30) + (((pi & 0x3E0) >> 2) * 59) + (((pi & 0x7C00) >> 7) * 11)) / 100) & 0xFF;
                Palette16BPP[pi] = grey + (grey << 8) + (grey << 16);
            }
        }
    } else {
        if (dxw.dwFlags1 & USERGB565) {
            for (pi = 0; pi < 0x10000; pi++) {
                Palette16BPP[pi] = (pi & 0x1F) << 3 | (pi & 0x7E0) << 5 | (pi & 0xF800) << 8; // RGB565
            }
        } else {
            for (pi = 0; pi < 0x10000; pi++) {
                Palette16BPP[pi] = (pi & 0x1F) << 3 | (pi & 0x3E0) << 6 | (pi & 0x7C00) << 9; // RGB555
            }
        }
    }
}

static hqx_Type LoadHQX(char *fun) {
    hqx_Type ret;
    hqxInit_Type phqinit;
    HMODULE hHQX = (*pLoadLibraryA)("dxhqx.dll");
    if(!hHQX) {
        MessageBox(0, "Load dxhqx.dll failed)", "error", 0);
        return 0;
    }
    if(!(ret = (hqx_Type)(*pGetProcAddress)(hHQX, fun))) return 0;
    if(!(phqinit = (hqxInit_Type)(*pGetProcAddress)(hHQX, "hqxInit"))) return 0;
    (*phqinit)();
    return ret;
}

static Scale2X_Type LoadScale2X(char *lib, char *fun) {
    Scale2X_Type ret;
    HMODULE hDLL = (*pLoadLibraryA)(lib);
    if(!hDLL) {
        char Msg[81];
        sprintf_s(Msg, 80, "Load %s failed - err=%d", lib, GetLastError());
        MessageBox(0, Msg, "error", 0);
        return 0;
    }
    if(!(ret = (Scale2X_Type)(*pGetProcAddress)(hDLL, fun))) return 0;
    return ret;
}

//--------------------------------------------------------------------------------------------//
// Gamma ramp plug-ins.
//--------------------------------------------------------------------------------------------//

static void WINAPI Gamma32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask) {
    DWORD *dest;
    long destdelta;
    int x, y, w, h;
    DWORD pixel;
    int r, g, b;
    BYTE *Gamma;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 2;
    dest = (DWORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - w;
    Gamma = GetHookInfo()->GammaRamp;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            pixel = *dest;
            b = (pixel >>  0) & 0xFF;
            g = (pixel >>  8) & 0xFF;
            r = (pixel >> 16) & 0xFF;
            r = Gamma[  0 + r];
            g = Gamma[256 + g];
            b = Gamma[512 + b];
            pixel = (b << 0) | (g << 8) | (r << 16);
            *dest++ = pixel;
        }
        dest += destdelta;
    }
}

static void WINAPI Gamma565(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask) {
    WORD *dest;
    long destdelta;
    int x, y, w, h;
    WORD pixel;
    int r, g, b;
    BYTE *Gamma;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 1;
    dest = (WORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - w;
    Gamma = GetHookInfo()->GammaRamp;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            pixel = *dest;
            b = (pixel << 3) & 0xF8; // 11111000
            g = (pixel >> 3) & 0xFC; // 11111100
            r = (pixel >> 8) & 0xF8;
            r = Gamma[  0 + r];
            g = Gamma[256 + g];
            b = Gamma[512 + b];
            pixel = ((b & 0xF8) >> 3) | ((g & 0xFC) << 3) | ((r & 0xF8) << 8);
            *dest++ = pixel;
        }
        dest += destdelta;
    }
}

static void WINAPI Gamma555(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask) {
    WORD *dest;
    long destdelta;
    int x, y, w, h;
    WORD pixel;
    int r, g, b;
    BYTE *Gamma;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 1;
    dest = (WORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - w;
    Gamma = GetHookInfo()->GammaRamp;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            pixel = *dest;
            b = (pixel << 3) & 0xF8;
            g = (pixel >> 2) & 0xF8;
            r = (pixel >> 7) & 0xF8;
            r = Gamma[  0 + r];
            g = Gamma[256 + g];
            b = Gamma[512 + b];
            pixel = ((b & 0xF8) >> 3) | ((g & 0xFA) << 2) | ((r & 0xF8) << 7);
            *dest++ = pixel;
        }
        dest += destdelta;
    }
}

//--------------------------------------------------------------------------------------------//
// Emulation plug-ins.
//--------------------------------------------------------------------------------------------//

static void WINAPI EmuBlt_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src8;
    DWORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 2;
    dest = (DWORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - w;
    dest0 = dest;
    // srcpitch >>= 0;
    src8 = (BYTE *)lpsurface;
    src8 += lpsrcrect->top * srcpitch;
    src8 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dest ++) = PaletteEntries[*(src8 ++)];
        dest += destdelta;
        src8 += srcdelta;
    }
}

static void WINAPI BilinearBlt_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src8, *src0;
    DWORD *dest, *dest0;
    //long srcdelta, destdelta;
    int w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    lpdestrect->left <<= 1;
    lpdestrect->top <<= 1;
    lpdestrect->right <<= 1;
    lpdestrect->bottom <<= 1;
    destpitch >>= 2;
    dest = (DWORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    dest0 = dest;
    // srcpitch >>= 0;
    src8 = (BYTE *)lpsurface;
    src8 += (lpsrcrect->top >> 1) * srcpitch;
    src8 += (lpsrcrect->left >> 1);
    src0 = src8;
#ifdef USEOPENMP
    #pragma omp parallel for schedule(static)
#endif
    for(int y = 0; y < h; y ++) {
        register DWORD Q1, Q2, Q3, Q4, Q5;
        src8 = src0 + (y * srcpitch);
        dest = dest0 + ((y * destpitch) << 1);
        Q5 = Melt32(PaletteEntries[*(src8)], PaletteEntries[*(src8 + srcpitch)]);
        for(int x = 0; x < w; x ++) {
            Q1 = PaletteEntries[*(src8)];
            Q2 = Melt32(Q1, PaletteEntries[*(src8 + 1)]);
            Q3 = Q5;
            if(y == h - 1) {
                Q3 = Q1;
                Q4 = Q2;
            } else {
                Q5 = Melt32(PaletteEntries[*(src8 + 1)], PaletteEntries[*(src8 + srcpitch + 1)]); // to be used in next for cycle
                Q4 = Melt32(Q3, Q5);
            }
            *(dest) = Q1;
            *(dest + 1) = Q2;
            *(dest + destpitch) = Q3;
            *(dest + destpitch + 1) = Q4;
            src8++;
            dest += 2;
        }
    }
}

static void WINAPI Deinterlace_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src8;
    DWORD *dest, *dest0;
    long srcpitch0, destpitch0;
    DWORD x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    dest = (DWORD *)lpdest;
    destpitch >>= 2;
    destpitch0 = destpitch;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destpitch = destpitch - w;
    dest0 = dest;
    //srcpitch >>= 0;
    srcpitch0 = srcpitch;
    src8 = (BYTE *)lpsurface;
    src8 += lpsrcrect->top * srcpitch;
    src8 += lpsrcrect->left;
    srcpitch = srcpitch - w;
    int InterlaceLines = 0;
    BOOL allblack = TRUE;
    BYTE *src08 = src8;
    y = ((h >> 2) << 1) + 1;
    for(x = 0; x < w; x ++) {
        if(*((src8 ++) + (y * srcpitch0))) {
            allblack = FALSE;
            break;
        }
    }
    if(allblack) InterlaceLines = 1;
    else {
        y = ((h >> 2) << 1);
        allblack = TRUE;
        src8 = src08;
        for(x = 0; x < w; x ++) {
            if(*((src8 ++) + (y * srcpitch0))) {
                allblack = FALSE;
                break;
            }
            if(allblack) InterlaceLines = 2;
        }
    }
    dest = dest0;
    src8 = src08;
    OutTrace("InterlaceLines=%d\n", InterlaceLines);
    if(InterlaceLines == 1) {
        for(y = 0; y < h; y += 2) {
            for(x = 0; x < w; x ++)
                *(dest ++) = PaletteEntries[*(src8 ++)];
            dest += destpitch + destpitch0;
            src8 += srcpitch + srcpitch0;
        }
        dest = dest0 + destpitch0;
        src8 = src08 + srcpitch0;
        for(y = 1; y < h; y += 2) {
            for(x = 0; x < w; x ++)
                *(dest ++) = *(dest + destpitch0);
            dest += destpitch + destpitch0;
            src8 += srcpitch + srcpitch0;
        }
    } else if(InterlaceLines == 2) {
        dest += destpitch + destpitch0;
        src8 += srcpitch + srcpitch0;
        for(y = 1; y < h; y += 2) {
            for(x = 0; x < w; x ++)
                *(dest ++) = PaletteEntries[*(src8 ++)];
            dest += destpitch + destpitch0;
            src8 += srcpitch + srcpitch0;
        }
        dest = dest0 + destpitch0;
        src8 = src08 + srcpitch0;
        for(y = 0; y < h; y += 2) {
            for(x = 0; x < w; x ++)
                *(dest + destpitch0) = *(dest++);
            dest += destpitch + destpitch0;
            src8 += srcpitch + srcpitch0;
        }
    } else {
        for(y = 0; y < h; y ++) {
            for(x = 0; x < w; x ++)
                *(dest ++) = PaletteEntries[*(src8 ++)];
            dest += destpitch;
            src8 += srcpitch;
        }
    }
}

static void WINAPI Interlace_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src8;
    DWORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 2;
    dest = (DWORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - w;
    dest0 = dest;
    // srcpitch >>= 0;
    src8 = (BYTE *)lpsurface;
    src8 += lpsrcrect->top * srcpitch;
    src8 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    for(y = 0; y < h; y ++) {
        if(y % 2) {
            for(x = 0; x < w; x ++) {
                *(dest ++) = 0; // black
            }
            src8 += w;
        } else {
            for(x = 0; x < w; x ++)
                *(dest ++) = PaletteEntries[*(src8 ++)];
        }
        dest += destdelta;
        src8 += srcdelta;
    }
}

static void WINAPI HQ_X2_Blt_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src8;
    DWORD *dest, *dest0;
    int w, h;
    static BOOL bInit = TRUE;
    static hqx_Type phq2x_32;
    if(bInit) {
        if(!(phq2x_32 = LoadHQX("hq2x_32_rb"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    lpdestrect->left <<= 1;
    lpdestrect->top <<= 1;
    lpdestrect->right <<= 1;
    lpdestrect->bottom <<= 1;
    dest0 = dest = (DWORD *)lpdest;
    src8 = (BYTE *)lpsurface;
    DWORD *tmpbuf = (DWORD *)malloc(w * h * sizeof(DWORD));
    int x, y;
    DWORD *dp = (DWORD *)tmpbuf;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = PaletteEntries[*(src8 ++)];
        src8 += (srcpitch - w);
    }
    (*phq2x_32)(tmpbuf, w * sizeof(DWORD), dest, destpitch, w, h);
    free(tmpbuf);
}

static void WINAPI HQ_X3_Blt_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    BYTE *src8;
    DWORD *dest, *dest0;
    int w, h;
    static BOOL bInit = TRUE;
    static hqx_Type phq3x_32;
    if(bInit) {
        if(!(phq3x_32 = LoadHQX("hq3x_32_rb"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    lpdestrect->left <<= 1;
    lpdestrect->top <<= 1;
    lpdestrect->right <<= 1;
    lpdestrect->bottom <<= 1;
    dest0 = dest = (DWORD *)lpdest;
    src8 = (BYTE *)lpsurface;
    DWORD *tmpbuf = (DWORD *)malloc(w * h * sizeof(DWORD));
    int x, y;
    DWORD *dp = (DWORD *)tmpbuf;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = PaletteEntries[*(src8 ++)];
        src8 += (srcpitch - w);
    }
    (*phq3x_32)(tmpbuf, w * sizeof(DWORD), dest, destpitch, w, h);
    free(tmpbuf);
}

static void WINAPI HQ_X4_Blt_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    BYTE *src8;
    DWORD *dest, *dest0;
    int w, h;
    static BOOL bInit = TRUE;
    static hqx_Type phq4x_32;
    if(bInit) {
        if(!(phq4x_32 = LoadHQX("hq4x_32_rb"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    lpdestrect->left <<= 1;
    lpdestrect->top <<= 1;
    lpdestrect->right <<= 1;
    lpdestrect->bottom <<= 1;
    dest0 = dest = (DWORD *)lpdest;
    src8 = (BYTE *)lpsurface;
    DWORD *tmpbuf = (DWORD *)malloc(w * h * sizeof(DWORD));
    int x, y;
    DWORD *dp = (DWORD *)tmpbuf;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = PaletteEntries[*(src8 ++)];
        src8 += (srcpitch - w);
    }
    (*phq4x_32)(tmpbuf, w * sizeof(DWORD), dest, destpitch, w, h);
    free(tmpbuf);
}

static void WINAPI PIX_X2_Blt_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src8;
    DWORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 2;
    dest = (DWORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (2 * w);
    dest0 = dest;
    // srcpitch >>= 0;
    src8 = (BYTE *)lpsurface;
    src8 += lpsrcrect->top * srcpitch;
    src8 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    DWORD dwPixel;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            dwPixel = PaletteEntries[*(src8 ++)];
            *dest = *(dest + 1) = *(dest + destpitch) = *(dest + destpitch + 1) = dwPixel;
            dest += 2;
        }
        dest += (destpitch + destdelta);
        src8 += srcdelta;
    }
}

static void WINAPI PIX_X3_Blt_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src8;
    DWORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 2;
    dest = (DWORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (3 * w);
    dest0 = dest;
    // srcpitch >>= 0;
    src8 = (BYTE *)lpsurface;
    src8 += lpsrcrect->top * srcpitch;
    src8 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    DWORD dwPixel;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            dwPixel = PaletteEntries[*(src8 ++)];
            *dest = *(dest + 1) = *(dest + 2) =
                                      *(dest + destpitch) = *(dest + destpitch + 1) = *(dest + destpitch + 2) =
                                                  *(dest + (2 * destpitch)) = *(dest + (2 * destpitch) + 1) = *(dest + (2 * destpitch) + 2) =
                                                          dwPixel;
            dest += 3;
        }
        dest += ((2 * destpitch) + destdelta);
        src8 += srcdelta;
    }
}

static void WINAPI PIX_X4_Blt_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src8;
    DWORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 2;
    dest = (DWORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (4 * w);
    dest0 = dest;
    // srcpitch >>= 0;
    src8 = (BYTE *)lpsurface;
    src8 += lpsrcrect->top * srcpitch;
    src8 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    DWORD dwPixel;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            dwPixel = PaletteEntries[*(src8 ++)];
            *dest = *(dest + 1) = *(dest + 2) = *(dest + 3) =
                                                    *(dest + destpitch) = *(dest + destpitch + 1) = *(dest + destpitch + 2) = *(dest + destpitch + 3) =
                                                            *(dest + (2 * destpitch)) = *(dest + (2 * destpitch) + 1) = *(dest + (2 * destpitch) + 2) = *(dest + (2 * destpitch) + 3) =
                                                                    *(dest + (3 * destpitch)) = *(dest + (3 * destpitch) + 1) = *(dest + (3 * destpitch) + 2) = *(dest + (3 * destpitch) + 3) =
                                                                            dwPixel;
            dest += 4;
        }
        dest += ((3 * destpitch) + destdelta);
        src8 += srcdelta;
    }
}

/**
    Apply the Scale effect on a bitmap.
    This function is simply a common interface for ::scale2x(), ::scale3x() and ::scale4x().
    \param scale Scale factor. 2, 203 (fox 2x3), 204 (for 2x4), 3 or 4.
    \param void_dst Pointer at the first pixel of the destination bitmap.
    \param dst_slice Size in bytes of a destination bitmap row.
    \param void_src Pointer at the first pixel of the source bitmap.
    \param src_slice Size in bytes of a source bitmap row.
    \param pixel Bytes per pixel of the source and destination bitmap.
    \param width Horizontal size in pixels of the source bitmap.
    \param height Vertical size in pixels of the source bitmap.
*/

static void WINAPI Scale2X_Blt_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    BYTE *src8;
    DWORD *dest;
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2x", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
#ifdef USE_SCALE2X_SSE2
    dest = (DWORD *)malloc(SSE_SIZE(4 * w * h * sizeof(DWORD)));
#else
    dest = (DWORD *)lpdest;
#endif
    src8 = (BYTE *)lpsurface;
    DWORD *tmpbuf = (DWORD *)malloc(SSE_SIZE(w * h * sizeof(DWORD)));
    int x, y;
    DWORD *dp = SSE_ALIGN(tmpbuf);
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = PaletteEntries[*(src8 ++)];
        src8 += (srcpitch - w);
    }
    (*pScale)(202, SSE_ALIGN(dest), destpitch, SSE_ALIGN(tmpbuf), w * sizeof(DWORD), 4, w, h);
    free(tmpbuf);
#ifdef USE_SCALE2X_SSE2
    memcpy(lpdest, SSE_ALIGN(dest), 4 * w * h * sizeof(DWORD));
    free(dest);
#endif
}

static void WINAPI Scale3X_Blt_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    BYTE *src8;
    DWORD *dest;
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2x", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
#ifdef USE_SCALE2X_SSE2
    dest = (DWORD *)malloc(SSE_SIZE(9 * w * h * sizeof(DWORD)));
#else
    dest = (DWORD *)lpdest;
#endif
    src8 = (BYTE *)lpsurface;
    if((h < 2) || (w < 2)) return;
    DWORD *tmpbuf = (DWORD *)malloc(SSE_SIZE(w * h * sizeof(DWORD)));
    int x, y;
    DWORD *dp = SSE_ALIGN(tmpbuf);
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = PaletteEntries[*(src8 ++)];
        src8 += (srcpitch - w);
    }
    (*pScale)(303, SSE_ALIGN(dest), destpitch, SSE_ALIGN(tmpbuf), w * sizeof(DWORD), 4, w, h);
    free(tmpbuf);
#ifdef USE_SCALE2X_SSE2
    memcpy(lpdest, SSE_ALIGN(dest), 9 * w * h * sizeof(DWORD));
    free(dest);
#endif
}

static void WINAPI Scale4X_Blt_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    BYTE *src8;
    DWORD *dest;
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2x", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
#ifdef USE_SCALE2X_SSE2
    dest = (DWORD *)malloc(SSE_SIZE(16 * w * h * sizeof(DWORD)));
#else
    dest = (DWORD *)lpdest;
#endif
    src8 = (BYTE *)lpsurface;
    DWORD *tmpbuf = (DWORD *)malloc(SSE_SIZE(w * h * sizeof(DWORD)));
    int x, y;
    DWORD *dp = SSE_ALIGN(tmpbuf);
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = PaletteEntries[*(src8 ++)];
        src8 += (srcpitch - w);
    }
    (*pScale)(404, SSE_ALIGN(dest), destpitch, SSE_ALIGN(tmpbuf), w * sizeof(DWORD), 4, w, h);
    free(tmpbuf);
#ifdef USE_SCALE2X_SSE2
    memcpy(lpdest, SSE_ALIGN(dest), 16 * w * h * sizeof(DWORD));
    free(dest);
#endif
}

static void WINAPI Scale2K_Blt_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    BYTE *src8;
    DWORD *dest;
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2k", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
#ifdef USE_SCALE2X_SSE2
    dest = (DWORD *)malloc(SSE_SIZE(4 * w * h * sizeof(DWORD)));
#else
    dest = (DWORD *)lpdest;
#endif
    src8 = (BYTE *)lpsurface;
    DWORD *tmpbuf = (DWORD *)malloc(SSE_SIZE(w * h * sizeof(DWORD)));
    int x, y;
    DWORD *dp = SSE_ALIGN(tmpbuf);
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = PaletteEntries[*(src8 ++)];
        src8 += (srcpitch - w);
    }
    (*pScale)(202, SSE_ALIGN(dest), destpitch, SSE_ALIGN(tmpbuf), w * sizeof(DWORD), 4, w, h);
    free(tmpbuf);
#ifdef USE_SCALE2X_SSE2
    memcpy(lpdest, SSE_ALIGN(dest), 4 * w * h * sizeof(DWORD));
    free(dest);
#endif
}

static void WINAPI Scale3K_Blt_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    BYTE *src8;
    DWORD *dest;
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2k", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
#ifdef USE_SCALE2X_SSE2
    dest = (DWORD *)malloc(SSE_SIZE(9 * w * h * sizeof(DWORD)));
#else
    dest = (DWORD *)lpdest;
#endif
    src8 = (BYTE *)lpsurface;
    DWORD *tmpbuf = (DWORD *)malloc(SSE_SIZE(w * h * sizeof(DWORD)));
    int x, y;
    DWORD *dp = SSE_ALIGN(tmpbuf);
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = PaletteEntries[*(src8 ++)];
        src8 += (srcpitch - w);
    }
    (*pScale)(303, SSE_ALIGN(dest), destpitch, SSE_ALIGN(tmpbuf), w * sizeof(DWORD), 4, w, h);
    free(tmpbuf);
#ifdef USE_SCALE2X_SSE2
    memcpy(lpdest, SSE_ALIGN(dest), 9 * w * h * sizeof(DWORD));
    free(dest);
#endif
}

static void WINAPI Scale4K_Blt_8_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    BYTE *src8;
    DWORD *dest;
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2k", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
#ifdef USE_SCALE2X_SSE2
    dest = (DWORD *)malloc(SSE_SIZE(16 * w * h * sizeof(DWORD)));
#else
    dest = (DWORD *)lpdest;
#endif
    src8 = (BYTE *)lpsurface;
    DWORD *tmpbuf = (DWORD *)malloc(SSE_SIZE(w * h * sizeof(DWORD)));
    int x, y;
    DWORD *dp = SSE_ALIGN(tmpbuf);
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = PaletteEntries[*(src8 ++)];
        src8 += (srcpitch - w);
    }
    (*pScale)(404, SSE_ALIGN(dest), destpitch, SSE_ALIGN(tmpbuf), w * sizeof(DWORD), 4, w, h);
    free(tmpbuf);
#ifdef USE_SCALE2X_SSE2
    memcpy(lpdest, SSE_ALIGN(dest), 16 * w * h * sizeof(DWORD));
    free(dest);
#endif
}

static void WINAPI EmuBlt_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    WORD *src16;
    DWORD *dest, *dest0;
    DWORD x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    dest = (DWORD *)lpdest;
    destpitch >>= 2;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destpitch = destpitch - w;
    dest0 = dest;
    srcpitch >>= 1;
    src16 = (WORD *)lpsurface;
    src16 += lpsrcrect->top * srcpitch;
    src16 += lpsrcrect->left;
    srcpitch = srcpitch - w;
    if (!Palette16BPP) SetPalette16BPP();
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dest ++) = Palette16BPP[*(src16 ++)];
        dest += destpitch;
        src16 += srcpitch;
    }
}

static void WINAPI BilinearBlt_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    WORD *src16, *src0;
    DWORD *dest, *dest0;
    int w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    lpdestrect->left <<= 1;
    lpdestrect->top <<= 1;
    lpdestrect->right <<= 1;
    lpdestrect->bottom <<= 1;
    dest = (DWORD *)lpdest;
    destpitch >>= 2;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    dest0 = dest;
    srcpitch >>= 1;
    src16 = (WORD *)lpsurface;
    src16 += (lpsrcrect->top >> 1) * srcpitch;
    src16 += (lpsrcrect->left >> 1);
    src0 = src16;
    if (!Palette16BPP) SetPalette16BPP();
#ifdef USEOPENMP
    #pragma omp parallel for schedule(static)
#endif
    for(int y = 0; y < h; y ++) {
        register DWORD Q1, Q2, Q3, Q4, Q5;
        src16 = src0 + (y * srcpitch);
        dest = dest0 + ((y * destpitch) << 1);
        Q5 = Melt32(Palette16BPP[*(src16)], Palette16BPP[*(src16 + srcpitch)]);
        for(int x = 0; x < w; x ++) {
            Q1 = Palette16BPP[*(src16)];
            Q2 = Melt32(Q1, Palette16BPP[*(src16 + 1)]);
            Q3 = Q5;
            if(y == h - 1) {
                Q3 = Q1;
                Q4 = Q2;
            } else {
                Q5 = Melt32(Palette16BPP[*(src16 + 1)], Palette16BPP[*(src16 + srcpitch + 1)]); // to be used in next for cycle
                Q4 = Melt32(Q3, Q5);
            }
            *(dest) = Q1;
            *(dest + 1) = Q2;
            *(dest + destpitch) = Q3;
            *(dest + destpitch + 1) = Q4;
            src16++;
            dest += 2;
        }
    }
}

static void WINAPI HQ_X2_Blt_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    WORD *src16;
    DWORD *dest, *dest0;
    int w, h;
    static BOOL bInit = TRUE;
    static hqx_Type phq2x_32;
    if(bInit) {
        if(!(phq2x_32 = LoadHQX("hq2x_32_rb"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    dest0 = dest = (DWORD *)lpdest;
    src16 = (WORD *)lpsurface;
    srcpitch >>= 1;
    if (!Palette16BPP) SetPalette16BPP();
    DWORD *tmpbuf = (DWORD *)malloc(w * h * sizeof(DWORD));
    int x, y;
    DWORD *dp = (DWORD *)tmpbuf;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = Palette16BPP[*(src16 ++)];
        src16 += (srcpitch - w);
    }
    (*phq2x_32)(tmpbuf, w * sizeof(DWORD), dest, destpitch, w, h);
    free(tmpbuf);
}

static void WINAPI HQ_X3_Blt_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    WORD *src16;
    DWORD *dest, *dest0;
    int w, h;
    static BOOL bInit = TRUE;
    static hqx_Type phq3x_32;
    if(bInit) {
        if(!(phq3x_32 = LoadHQX("hq3x_32_rb"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    dest0 = dest = (DWORD *)lpdest;
    src16 = (WORD *)lpsurface;
    srcpitch >>= 1;
    if (!Palette16BPP) SetPalette16BPP();
    DWORD *tmpbuf = (DWORD *)malloc(w * h * sizeof(DWORD));
    int x, y;
    DWORD *dp = (DWORD *)tmpbuf;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = Palette16BPP[*(src16 ++)];
        src16 += (srcpitch - w);
    }
    (*phq3x_32)(tmpbuf, w * sizeof(DWORD), dest, destpitch, w, h);
    free(tmpbuf);
}

static void WINAPI HQ_X4_Blt_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    WORD *src16;
    DWORD *dest, *dest0;
    int w, h;
    static BOOL bInit = TRUE;
    static hqx_Type phq4x_32;
    if(bInit) {
        if(!(phq4x_32 = LoadHQX("hq4x_32_rb"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    dest0 = dest = (DWORD *)lpdest;
    src16 = (WORD *)lpsurface;
    srcpitch >>= 1;
    if (!Palette16BPP) SetPalette16BPP();
    DWORD *tmpbuf = (DWORD *)malloc(w * h * sizeof(DWORD));
    int x, y;
    DWORD *dp = (DWORD *)tmpbuf;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = Palette16BPP[*(src16 ++)];
        src16 += (srcpitch - w);
    }
    (*phq4x_32)(tmpbuf, w * sizeof(DWORD), dest, destpitch, w, h);
    free(tmpbuf);
}

static void WINAPI Deinterlace_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    WORD *src16;
    DWORD *dest, *dest0;
    long srcpitch0, destpitch0;
    DWORD x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    dest = (DWORD *)lpdest;
    destpitch >>= 2;
    destpitch0 = destpitch;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destpitch = destpitch - w;
    dest0 = dest;
    srcpitch >>= 1;
    srcpitch0 = srcpitch;
    src16 = (WORD *)lpsurface;
    src16 += lpsrcrect->top * srcpitch;
    src16 += lpsrcrect->left;
    srcpitch = srcpitch - w;
    if (!Palette16BPP) SetPalette16BPP();
    BOOL allblack = TRUE;
    WORD *src016 = src16;
    y = ((h >> 2) << 1) + 1;
    for(x = 0; x < w; x ++)
        if(Palette16BPP[*((src16 ++) + (y * srcpitch0))]) allblack = FALSE;
    dest = dest0;
    src16 = src016;
    if(allblack) {
        for(y = 0; y < h; y += 2) {
            for(x = 0; x < w; x ++)
                *(dest ++) = Palette16BPP[*(src16 ++)];
            dest += destpitch + destpitch0;
            src16 += srcpitch + srcpitch0;
        }
        dest = dest0 + destpitch0;
        src16 = src016 + srcpitch0;
        for(y = 1; y < h; y += 2) {
            for(x = 0; x < w; x ++)
                *(dest ++) = (*(dest + destpitch0) >> 1) + (*(dest - destpitch0) >> 1) ;
            dest += destpitch + destpitch0;
            src16 += srcpitch + srcpitch0;
        }
    } else {
        for(y = 0; y < h; y ++) {
            for(x = 0; x < w; x ++)
                *(dest ++) = Palette16BPP[*(src16 ++)];
            dest += destpitch;
            src16 += srcpitch;
        }
    }
}

static void WINAPI Interlace_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    WORD *src16;
    DWORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 2;
    dest = (DWORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - w;
    dest0 = dest;
    srcpitch >>= 1;
    src16 = (WORD *)lpsurface;
    src16 += lpsrcrect->top * srcpitch;
    src16 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    if (!Palette16BPP) SetPalette16BPP();
    for(y = 0; y < h; y ++) {
        if(y % 2) {
            for(x = 0; x < w; x ++) {
                *(dest ++) = 0; // black
            }
            src16 += w;
        } else {
            for(x = 0; x < w; x ++)
                *(dest ++) = Palette16BPP[*(src16 ++)];
        }
        dest += destdelta;
        src16 += srcdelta;
    }
}

static void WINAPI PIX_X2_Blt_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    WORD *src16;
    DWORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 2;
    dest = (DWORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (2 * w);
    dest0 = dest;
    srcpitch >>= 1;
    src16 = (WORD *)lpsurface;
    src16 += lpsrcrect->top * srcpitch;
    src16 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    if (!Palette16BPP) SetPalette16BPP();
    DWORD dwPixel;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            dwPixel = Palette16BPP[*(src16 ++)];
            *dest = *(dest + 1) = *(dest + destpitch) = *(dest + destpitch + 1) = dwPixel;
            dest += 2;
        }
        dest += (destpitch + destdelta);
        src16 += srcdelta;
    }
}

static void WINAPI PIX_X3_Blt_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    WORD *src16;
    DWORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 2;
    dest = (DWORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (3 * w);
    dest0 = dest;
    srcpitch >>= 1;
    src16 = (WORD *)lpsurface;
    src16 += lpsrcrect->top * srcpitch;
    src16 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    if (!Palette16BPP) SetPalette16BPP();
    DWORD dwPixel;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            dwPixel = Palette16BPP[*(src16 ++)];
            *dest = *(dest + 1) = *(dest + 2) =
                                      *(dest + destpitch) = *(dest + destpitch + 1) = *(dest + destpitch + 2) =
                                                  *(dest + 2 * destpitch) = *(dest + 2 * destpitch + 1) = *(dest + 2 * destpitch + 2) = dwPixel;
            dest += 3;
        }
        dest += (2 * destpitch + destdelta);
        src16 += srcdelta;
    }
}

static void WINAPI PIX_X4_Blt_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    WORD *src16;
    DWORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 2;
    dest = (DWORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (4 * w);
    dest0 = dest;
    srcpitch >>= 1;
    src16 = (WORD *)lpsurface;
    src16 += lpsrcrect->top * srcpitch;
    src16 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    if (!Palette16BPP) SetPalette16BPP();
    DWORD dwPixel;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            dwPixel = Palette16BPP[*(src16 ++)];
            *dest = *(dest + 1) = *(dest + 2) = *(dest + 3) =
                                                    *(dest + destpitch) = *(dest + destpitch + 1) = *(dest + destpitch + 2) = *(dest + destpitch + 3) =
                                                            *(dest + 2 * destpitch) = *(dest + 2 * destpitch + 1) = *(dest + 2 * destpitch + 2) = *(dest + 2 * destpitch + 3) =
                                                                    *(dest + 3 * destpitch) = *(dest + 3 * destpitch + 1) = *(dest + 3 * destpitch + 2) = *(dest + 3 * destpitch + 3) =
                                                                            dwPixel;
            dest += 4;
        }
        dest += (3 * destpitch + destdelta);
        src16 += srcdelta;
    }
}

static void WINAPI Scale2X_Blt_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    WORD *src16;
    DWORD *dest;
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2x", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    srcpitch >>= 1;
    dest = (DWORD *)lpdest;
    src16 = (WORD *)lpsurface;
    if (!Palette16BPP) SetPalette16BPP();
    DWORD *tmpbuf = (DWORD *)malloc(w * h * sizeof(DWORD));
    int x, y;
    DWORD *dp = (DWORD *)tmpbuf;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = Palette16BPP[*(src16 ++)];
        src16 += (srcpitch - w);
    }
    OutTrace("SCALE2X *** dest=%#x src=%#x\n", dest, tmpbuf);
    (*pScale)(202, dest, destpitch, tmpbuf, w * sizeof(DWORD), 4, w, h);
    free(tmpbuf);
}

static void WINAPI Scale3X_Blt_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    WORD *src16;
    DWORD *dest;
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2x", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    srcpitch >>= 1;
    dest = (DWORD *)lpdest;
    src16 = (WORD *)lpsurface;
    if (!Palette16BPP) SetPalette16BPP();
    DWORD *tmpbuf = (DWORD *)malloc(w * h * sizeof(DWORD));
    int x, y;
    DWORD *dp = (DWORD *)tmpbuf;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = Palette16BPP[*(src16 ++)];
        src16 += (srcpitch - w);
    }
    OutTrace("SCALE3X *** dest=%#x src=%#x\n", dest, tmpbuf);
    (*pScale)(303, dest, destpitch, tmpbuf, w * sizeof(DWORD), 4, w, h);
    free(tmpbuf);
}

static void WINAPI Scale4X_Blt_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    WORD *src16;
    DWORD *dest;
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2x", "scale"))) return;
        bInit = FALSE;
    }
    srcpitch >>= 1;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    dest = (DWORD *)lpdest;
    src16 = (WORD *)lpsurface;
    if (!Palette16BPP) SetPalette16BPP();
    DWORD *tmpbuf = (DWORD *)malloc(SSE_SIZE(w * h * sizeof(DWORD)));
    int x, y;
    DWORD *dp = SSE_ALIGN(tmpbuf);
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = Palette16BPP[*(src16 ++)];
        src16 += (srcpitch - w);
    }
    (*pScale)(404, SSE_ALIGN(dest), destpitch, SSE_ALIGN(tmpbuf), w * sizeof(DWORD), 4, w, h);
    free(tmpbuf);
}

static void WINAPI Scale2K_Blt_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    WORD *src16;
    DWORD *dest;
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2k", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    srcpitch >>= 1;
    dest = (DWORD *)lpdest;
    src16 = (WORD *)lpsurface;
    if (!Palette16BPP) SetPalette16BPP();
    DWORD *tmpbuf = (DWORD *)malloc(w * h * sizeof(DWORD));
    int x, y;
    DWORD *dp = (DWORD *)tmpbuf;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = Palette16BPP[*(src16 ++)];
        src16 += (srcpitch - w);
    }
    OutTrace("SCALE2K *** dest=%#x src=%#x\n", dest, tmpbuf);
    (*pScale)(202, dest, destpitch, tmpbuf, w * sizeof(DWORD), 4, w, h);
    free(tmpbuf);
}

static void WINAPI Scale3K_Blt_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    WORD *src16;
    DWORD *dest;
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2k", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    srcpitch >>= 1;
    dest = (DWORD *)lpdest;
    src16 = (WORD *)lpsurface;
    if (!Palette16BPP) SetPalette16BPP();
    DWORD *tmpbuf = (DWORD *)malloc(w * h * sizeof(DWORD));
    int x, y;
    DWORD *dp = (DWORD *)tmpbuf;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = Palette16BPP[*(src16 ++)];
        src16 += (srcpitch - w);
    }
    OutTrace("SCALE3K *** dest=%#x src=%#x\n", dest, tmpbuf);
    (*pScale)(303, dest, destpitch, tmpbuf, w * sizeof(DWORD), 4, w, h);
    free(tmpbuf);
}

static void WINAPI Scale4K_Blt_16_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    WORD *src16;
    DWORD *dest;
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2k", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    srcpitch >>= 1;
    dest = (DWORD *)lpdest;
    src16 = (WORD *)lpsurface;
    if (!Palette16BPP) SetPalette16BPP();
    DWORD *tmpbuf = (DWORD *)malloc(w * h * sizeof(DWORD));
    int x, y;
    DWORD *dp = (DWORD *)tmpbuf;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = Palette16BPP[*(src16 ++)];
        src16 += (srcpitch - w);
    }
    OutTrace("SCALE4K *** dest=%#x src=%#x\n", dest, tmpbuf);
    (*pScale)(404, dest, destpitch, tmpbuf, w * sizeof(DWORD), 4, w, h);
    free(tmpbuf);
}

static void WINAPI EmuBlt_24_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src24;
    DWORD *dest, *dest0;
    DWORD x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    dest = (DWORD *)lpdest;
    destpitch >>= 2;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destpitch = destpitch - w;
    dest0 = dest;
    src24 = (BYTE *)lpsurface;
    src24 += lpsrcrect->top * srcpitch;
    src24 += lpsrcrect->left;
    srcpitch = srcpitch - 3 * w;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            DWORD dwPixel;
            dwPixel = 0;
            memcpy((void *)&dwPixel, (void *)src24, 3);
            *(dest ++) = dwPixel;
            src24 += 3; // 3 bytes = 24 bits
        }
        dest += destpitch;
        src24 += srcpitch;
    }
}

// note: better avoid direct blitting in case of identical color depth (e.g. EmuBlt_32_to_32, EmuBlt_16_to_16)
// because it does not work between complex surfaces when DDSDCAPS_SYSTEMMEMORY is not omogeneous!

static void WINAPI EmuBlt_32_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    DWORD x, y, w, h;
    DWORD *src32;
    DWORD *dest, *dest0;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    dest = (DWORD *)lpdest;
    destpitch >>= 2;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destpitch = destpitch - w;
    dest0 = dest;
    srcpitch >>= 2;
    src32 = (DWORD *)lpsurface;
    src32 += lpsrcrect->top * srcpitch;
    src32 += lpsrcrect->left;
    srcpitch = srcpitch - w;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dest ++) = *(src32 ++);
        dest += destpitch;
        src32 += srcpitch;
    }
}

static void WINAPI Interlace_32_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    DWORD *src32;
    DWORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 2;
    dest = (DWORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - w;
    dest0 = dest;
    srcpitch >>= 2;
    src32 = (DWORD *)lpsurface;
    src32 += lpsrcrect->top * srcpitch;
    src32 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    for(y = 0; y < h; y ++) {
        if(y % 2) {
            for(x = 0; x < w; x ++) {
                *(dest ++) = 0; // black
            }
            src32 += w;
        } else {
            for(x = 0; x < w; x ++)
                *(dest ++) = *src32 ++;
        }
        dest += destdelta;
        src32 += srcdelta;
    }
}

static void WINAPI PIX_X2_Blt_32_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    DWORD *src32;
    DWORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 2;
    dest = (DWORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (2 * w);
    dest0 = dest;
    srcpitch >>= 2;
    src32 = (DWORD *)lpsurface;
    src32 += lpsrcrect->top * srcpitch;
    src32 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    DWORD dwPixel;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            dwPixel = *src32 ++;
            *dest = *(dest + 1) = *(dest + destpitch) = *(dest + destpitch + 1) = dwPixel;
            dest += 2;
        }
        dest += (destpitch + destdelta);
        src32 += srcdelta;
    }
}

static void WINAPI PIX_X3_Blt_32_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    DWORD *src32;
    DWORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 2;
    dest = (DWORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (3 * w);
    dest0 = dest;
    srcpitch >>= 2;
    src32 = (DWORD *)lpsurface;
    src32 += lpsrcrect->top * srcpitch;
    src32 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    if (!Palette16BPP) SetPalette16BPP();
    DWORD dwPixel;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            dwPixel = *src32++;
            *dest = *(dest + 1) = *(dest + 2) =
                                      *(dest + destpitch) = *(dest + destpitch + 1) = *(dest + destpitch + 2) =
                                                  *(dest + 2 * destpitch) = *(dest + 2 * destpitch + 1) = *(dest + 2 * destpitch + 2) = dwPixel;
            dest += 3;
        }
        dest += (2 * destpitch + destdelta);
        src32 += srcdelta;
    }
}

static void WINAPI PIX_X4_Blt_32_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    DWORD *src32;
    DWORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 2;
    dest = (DWORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (4 * w);
    dest0 = dest;
    srcpitch >>= 2;
    src32 = (DWORD *)lpsurface;
    src32 += lpsrcrect->top * srcpitch;
    src32 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    DWORD dwPixel;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            dwPixel = *src32 ++;
            *dest = *(dest + 1) = *(dest + 2) = *(dest + 3) =
                                                    *(dest + destpitch) = *(dest + destpitch + 1) = *(dest + destpitch + 2) = *(dest + destpitch + 3) =
                                                            *(dest + 2 * destpitch) = *(dest + 2 * destpitch + 1) = *(dest + 2 * destpitch + 2) = *(dest + 2 * destpitch + 3) =
                                                                    *(dest + 3 * destpitch) = *(dest + 3 * destpitch + 1) = *(dest + 3 * destpitch + 2) = *(dest + 3 * destpitch + 3) =
                                                                            dwPixel;
            dest += 4;
        }
        dest += (3 * destpitch + destdelta);
        src32 += srcdelta;
    }
}

static void WINAPI Scale2X_Blt_32_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2x", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    (*pScale)(202, lpdest, destpitch, lpsurface, srcpitch, 4, w, h);
}

static void WINAPI Scale3X_Blt_32_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2x", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    (*pScale)(303, lpdest, destpitch, lpsurface, srcpitch, 4, w, h);
}

static void WINAPI Scale4X_Blt_32_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2x", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    (*pScale)(404, lpdest, destpitch, lpsurface, srcpitch, 4, w, h);
}

static void WINAPI Scale2K_Blt_32_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2k", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    (*pScale)(202, lpdest, destpitch, lpsurface, srcpitch, 4, w, h);
}

static void WINAPI Scale3K_Blt_32_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2k", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    (*pScale)(303, lpdest, destpitch, lpsurface, srcpitch, 4, w, h);
}

static void WINAPI Scale4K_Blt_32_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2k", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    (*pScale)(404, lpdest, destpitch, lpsurface, srcpitch, 4, w, h);
}

static void WINAPI EmuBlt_8_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src8;
    WORD *dest, *dest0;
    DWORD x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    dest = (WORD *)lpdest;
    destpitch >>= 1;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destpitch = destpitch - w;
    dest0 = dest;
    src8 = (BYTE *)lpsurface;
    src8 += lpsrcrect->top * srcpitch;
    src8 += lpsrcrect->left;
    srcpitch = srcpitch - w;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dest ++) = (WORD)PaletteEntries[*(src8 ++)];
        dest += destpitch;
        src8 += srcpitch;
    }
}

static void WINAPI BilinearBlt_8_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src8, *src0;
    SHORT *dest, *dest0;
    int w, h;
    typedef DWORD (*Melt16_Type)(DWORD, DWORD);
    Melt16_Type Melt16;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    lpdestrect->left <<= 1;
    lpdestrect->top <<= 1;
    lpdestrect->right <<= 1;
    lpdestrect->bottom <<= 1;
    Melt16 = (dstGBitMask == 0x3E0) ? Melt16_555 : Melt16_565;
    dest = (SHORT *)lpdest;
    destpitch >>= 1;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    dest0 = dest;
    src8 = (BYTE *)lpsurface;
    src8 += (lpsrcrect->top >> 1) * srcpitch;
    src8 += (lpsrcrect->left >> 1);
    src0 = src8;
#ifdef USEOPENMP
    #pragma omp parallel for schedule(static)
#endif
    for(int y = 0; y < h; y ++) {
        register DWORD Q1, Q2, Q3, Q4, Q5;
        src8 = src0 + (y * srcpitch);
        dest = dest0 + ((y * destpitch) << 1);
        Q5 = Melt16(PaletteEntries[*(src8)], PaletteEntries[*(src8 + srcpitch)]);
        for(int x = 0; x < w; x ++) {
            Q1 = PaletteEntries[*(src8)];
            Q2 = Melt16(Q1, PaletteEntries[*(src8 + 1)]);
            Q3 = Q5;
            if(y == h - 1) {
                Q3 = Q1;
                Q4 = Q2;
            } else {
                Q5 = Melt16(PaletteEntries[*(src8 + 1)], PaletteEntries[*(src8 + srcpitch + 1)]); // to be used in next for cycle
                Q4 = Melt16(Q3, Q5);
            }
            *(dest) = (SHORT)Q1;
            *(dest + 1) = (SHORT)Q2;
            *(dest + destpitch) = (SHORT)Q3;
            *(dest + destpitch + 1) = (SHORT)Q4;
            src8++;
            dest += 2;
        }
    }
}

static void WINAPI HQ_X2_Blt_8_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    MessageBox(0, "HQ_X2_Blt_8_to_16 unimplemented", "Warning", 0);
}

static void WINAPI HQ_X3_Blt_8_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    MessageBox(0, "HQ_X3_Blt_8_to_16 unimplemented", "Warning", 0);
}

static void WINAPI HQ_X4_Blt_8_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    MessageBox(0, "HQ_X4_Blt_8_to_16 unimplemented", "Warning", 0);
}

static void WINAPI Interlace_8_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src8;
    WORD *dest, *dest0;
    DWORD x, y, w, h;
    long srcdelta, destdelta;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    dest = (WORD *)lpdest;
    destpitch >>= 1;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - w;
    dest0 = dest;
    src8 = (BYTE *)lpsurface;
    src8 += lpsrcrect->top * srcpitch;
    src8 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    for(y = 0; y < h; y ++) {
        if(y % 2) {
            for(x = 0; x < w; x ++) {
                *(dest ++) = 0; // black
            }
            src8 += w;
        } else {
            for(x = 0; x < w; x ++)
                *(dest ++) = (WORD)PaletteEntries[*(src8 ++)];
        }
        dest += destdelta;
        src8 += srcdelta;
    }
}

static void WINAPI PIX_X2_Blt_8_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src8;
    WORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 1;
    dest = (WORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (2 * w);
    dest0 = dest;
    // srcpitch >>= 0;
    src8 = (BYTE *)lpsurface;
    src8 += lpsrcrect->top * srcpitch;
    src8 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    WORD wPixel;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            wPixel = (WORD)PaletteEntries[*(src8 ++)];
            *dest = *(dest + 1) = *(dest + destpitch) = *(dest + destpitch + 1) = wPixel;
            dest += 2;
        }
        dest += (destpitch + destdelta);
        src8 += srcdelta;
    }
}

static void WINAPI PIX_X3_Blt_8_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src8;
    WORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 1;
    dest = (WORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (3 * w);
    dest0 = dest;
    // srcpitch >>= 0;
    src8 = (BYTE *)lpsurface;
    src8 += lpsrcrect->top * srcpitch;
    src8 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    WORD wPixel;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            wPixel = (WORD)PaletteEntries[*(src8 ++)];
            *dest = *(dest + 1) = *(dest + 2) =
                                      *(dest + destpitch) = *(dest + destpitch + 1) = *(dest + destpitch + 2) =
                                                  *(dest + (2 * destpitch)) = *(dest + (2 * destpitch) + 1) = *(dest + (2 * destpitch) + 2) =
                                                          wPixel;
            dest += 3;
        }
        dest += ((2 * destpitch) + destdelta);
        src8 += srcdelta;
    }
}

static void WINAPI PIX_X4_Blt_8_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src8;
    WORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 1;
    dest = (WORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (4 * w);
    dest0 = dest;
    // srcpitch >>= 0;
    src8 = (BYTE *)lpsurface;
    src8 += lpsrcrect->top * srcpitch;
    src8 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    WORD wPixel;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            wPixel = (WORD)PaletteEntries[*(src8 ++)];
            *dest = *(dest + 1) = *(dest + 2) = *(dest + 3) =
                                                    *(dest + destpitch) = *(dest + destpitch + 1) = *(dest + destpitch + 2) = *(dest + destpitch + 3) =
                                                            *(dest + (2 * destpitch)) = *(dest + (2 * destpitch) + 1) = *(dest + (2 * destpitch) + 2) = *(dest + (2 * destpitch) + 3) =
                                                                    *(dest + (3 * destpitch)) = *(dest + (3 * destpitch) + 1) = *(dest + (3 * destpitch) + 2) = *(dest + (3 * destpitch) + 3) =
                                                                            wPixel;
            dest += 4;
        }
        dest += ((3 * destpitch) + destdelta);
        src8 += srcdelta;
    }
}

static void WINAPI Scale2X_Blt_8_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    BYTE *src8;
    WORD *dest;
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2x", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    dest = (WORD *)lpdest;
    src8 = (BYTE *)lpsurface;
    DWORD *tmpbuf = (DWORD *)malloc(w * h * sizeof(WORD));
    int x, y;
    WORD *dp = (WORD *)tmpbuf;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = (WORD)PaletteEntries[*(src8 ++)];
        src8 += (srcpitch - w);
    }
    (*pScale)(202, dest, destpitch, tmpbuf, w * sizeof(WORD), 2, w, h);
    free(tmpbuf);
}

static void WINAPI Scale3X_Blt_8_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    BYTE *src8;
    WORD *dest;
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2x", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    dest = (WORD *)lpdest;
    src8 = (BYTE *)lpsurface;
    DWORD *tmpbuf = (DWORD *)malloc(w * h * sizeof(WORD));
    int x, y;
    WORD *dp = (WORD *)tmpbuf;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = (WORD)PaletteEntries[*(src8 ++)];
        src8 += (srcpitch - w);
    }
    (*pScale)(303, dest, destpitch, tmpbuf, w * sizeof(WORD), 2, w, h);
    free(tmpbuf);
}

static void WINAPI Scale4X_Blt_8_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    BYTE *src8;
    WORD *dest;
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2x", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    dest = (WORD *)lpdest;
    src8 = (BYTE *)lpsurface;
    DWORD *tmpbuf = (DWORD *)malloc(w * h * sizeof(WORD));
    int x, y;
    WORD *dp = (WORD *)tmpbuf;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++)
            *(dp ++) = (WORD)PaletteEntries[*(src8 ++)];
        src8 += (srcpitch - w);
    }
    (*pScale)(404, dest, destpitch, tmpbuf, w * sizeof(WORD), 2, w, h);
    free(tmpbuf);
}

static void WINAPI EmuBlt_16_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    SHORT *src16;
    SHORT *dest, *dest0;
    DWORD x, y, w, h;
    BOOL is555src, is555dst;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    dest = (SHORT *)lpdest;
    destpitch >>= 1;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destpitch = destpitch - w;
    dest0 = dest;
    is555dst = (dstGBitMask == 0x3E0);
    srcpitch >>= 1;
    src16 = (SHORT *)lpsurface;
    src16 += lpsrcrect->top * srcpitch;
    src16 += lpsrcrect->left;
    srcpitch = srcpitch - w;
    is555src = (srcGBitMask == 0x3E0);
    if (is555dst == is555src) {
        for(y = 0; y < h; y ++) {
            // copy one whole pixel line at a time....
            memcpy(dest, src16, w << 1);
            dest += (w + destpitch);
            src16 += (w + srcpitch);
        }
    } else {
        if(is555dst) {
            for(y = 0; y < h; y ++) {
                for(x = 0; x < w; x ++) {
                    SHORT pi;
                    pi = *(src16 ++);
                    pi = (pi & 0x1F) | (pi & 0x7C0) >> 1 | (pi & 0xF800) >> 1;
                    *(dest ++) = pi;
                }
                dest += destpitch;
                src16 += srcpitch;
            }
        } else {
            for(y = 0; y < h; y ++) {
                for(x = 0; x < w; x ++) {
                    SHORT pi;
                    pi = *(src16 ++);
                    pi = (pi & 0x1F) | (pi & 0x3E0) << 1 | (pi & 0x7C00) << 1;
                    *(dest ++) = pi;
                }
                dest += destpitch;
                src16 += srcpitch;
            }
        }
    }
}

static void WINAPI BilinearBlt_16_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    WORD *src16, *src0;
    WORD *dest, *dest0;
    int w, h;
    typedef DWORD (*Melt16_Type)(DWORD, DWORD);
    Melt16_Type Melt16;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    lpdestrect->left <<= 1;
    lpdestrect->top <<= 1;
    lpdestrect->right <<= 1;
    lpdestrect->bottom <<= 1;
    Melt16 = (dstGBitMask == 0x3E0) ? Melt16_555 : Melt16_565;
    dest = (WORD *)lpdest;
    destpitch >>= 1;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    dest0 = dest;
    srcpitch >>= 1;
    src16 = (WORD *)lpsurface;
    src16 += (lpsrcrect->top >> 1) * srcpitch;
    src16 += (lpsrcrect->left >> 1);
    src0 = src16;
#ifdef USEOPENMP
    #pragma omp parallel for schedule(static)
#endif
    for(int y = 0; y < h; y ++) {
        register DWORD Q1, Q2, Q3, Q4, Q5;
        src16 = src0 + (y * srcpitch);
        dest = dest0 + ((y * destpitch) << 1);
        Q5 = Melt16(*(src16), *(src16 + srcpitch));
        for(int x = 0; x < w; x ++) {
            Q1 = *(src16);
            Q2 = Melt16(Q1, *(src16 + 1));
            Q3 = Q5;
            if(y == h - 1) {
                Q3 = Q1;
                Q4 = Q2;
            } else {
                Q5 = Melt16(*(src16 + 1), *(src16 + srcpitch + 1)); // to be used in next for cycle
                Q4 = Melt16(Q3, Q5);
            }
            *(dest) = (WORD)Q1;
            *(dest + 1) = (WORD)Q2;
            *(dest + destpitch) = (WORD)Q3;
            *(dest + destpitch + 1) = (WORD)Q4;
            src16++;
            dest += 2;
        }
    }
}

static void WINAPI PIX_X2_Blt_16_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    WORD *src16;
    WORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 1;
    dest = (WORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (2 * w);
    dest0 = dest;
    srcpitch >>= 1;
    src16 = (WORD *)lpsurface;
    src16 += lpsrcrect->top * srcpitch;
    src16 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    WORD wPixel;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            wPixel = *src16 ++;
            *dest = *(dest + 1) = *(dest + destpitch) = *(dest + destpitch + 1) = wPixel;
            dest += 2;
        }
        dest += (destpitch + destdelta);
        src16 += srcdelta;
    }
}

static void WINAPI PIX_X3_Blt_16_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    WORD *src16;
    WORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 1;
    dest = (WORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (3 * w);
    dest0 = dest;
    srcpitch >>= 1;
    src16 = (WORD *)lpsurface;
    src16 += lpsrcrect->top * srcpitch;
    src16 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    WORD wPixel;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            wPixel = *src16 ++;
            *dest = *(dest + 1) = *(dest + 2) =
                                      *(dest + destpitch) = *(dest + destpitch + 1) = *(dest + destpitch + 2) =
                                                  *(dest + (2 * destpitch)) = *(dest + (2 * destpitch) + 1) = *(dest + (2 * destpitch) + 2) =
                                                          wPixel;
            dest += 3;
        }
        dest += ((2 * destpitch) + destdelta);
        src16 += srcdelta;
    }
}

static void WINAPI PIX_X4_Blt_16_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    WORD *src16;
    WORD *dest, *dest0;
    long srcdelta, destdelta;
    int x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    destpitch >>= 1;
    dest = (WORD *)lpdest;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (4 * w);
    dest0 = dest;
    srcpitch >>= 1;
    src16 = (WORD *)lpsurface;
    src16 += lpsrcrect->top * srcpitch;
    src16 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    WORD wPixel;
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            wPixel = *src16 ++;
            *dest = *(dest + 1) = *(dest + 2) = *(dest + 3) =
                                                    *(dest + destpitch) = *(dest + destpitch + 1) = *(dest + destpitch + 2) = *(dest + destpitch + 3) =
                                                            *(dest + (2 * destpitch)) = *(dest + (2 * destpitch) + 1) = *(dest + (2 * destpitch) + 2) = *(dest + (2 * destpitch) + 3) =
                                                                    *(dest + (3 * destpitch)) = *(dest + (3 * destpitch) + 1) = *(dest + (3 * destpitch) + 2) = *(dest + (3 * destpitch) + 3) =
                                                                            wPixel;
            dest += 4;
        }
        dest += ((3 * destpitch) + destdelta);
        src16 += srcdelta;
    }
}

static void WINAPI Scale2X_Blt_16_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2x", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    (*pScale)(202, lpdest, destpitch, lpsurface, srcpitch, 2, w, h);
}

static void WINAPI Scale3X_Blt_16_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2x", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    (*pScale)(303, lpdest, destpitch, lpsurface, srcpitch, 2, w, h);
}

static void WINAPI Scale4X_Blt_16_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2x", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    (*pScale)(404, lpdest, destpitch, lpsurface, srcpitch, 2, w, h);
}

static void WINAPI Scale2K_Blt_16_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2k", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    (*pScale)(202, lpdest, destpitch, lpsurface, srcpitch, 2, w, h);
}

static void WINAPI Scale3K_Blt_16_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2k", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    (*pScale)(303, lpdest, destpitch, lpsurface, srcpitch, 2, w, h);
}

static void WINAPI Scale4K_Blt_16_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    // BEWARE: not valid for partial RECT updates ???
    int w, h;
    static BOOL bInit = TRUE;
    static Scale2X_Type pScale;
    if(bInit) {
        if(!(pScale = LoadScale2X("scale2k", "scale"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    if((h < 2) || (w < 2)) return;
    (*pScale)(404, lpdest, destpitch, lpsurface, srcpitch, 2, w, h);
}

static void WINAPI EmuBlt_24_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src24;
    SHORT *dest, *dest0;
    DWORD x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    dest = (SHORT *)lpdest;
    destpitch >>= 1;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destpitch = destpitch - w;
    dest0 = dest;
    src24 = (BYTE *)lpsurface;
    src24 += lpsrcrect->top * srcpitch;
    src24 += lpsrcrect->left;
    srcpitch = srcpitch - (3 * w);
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            *(dest ++) = (*(src24 + 2) & 0xF8) << 8 | (*(src24 + 1) & 0xFC) << 3 | (*(src24 + 0) & 0xF8) >> 3;
            src24 += 3; // 3 bytes = 24 bits
        }
        dest += destpitch;
        src24 += srcpitch;
    }
}

static void WINAPI EmuBlt_32_to_16(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    BYTE *src32;
    SHORT *dest, *dest0;
    DWORD x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    dest = (SHORT *)lpdest;
    destpitch >>= 1;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destpitch = destpitch - w;
    dest0 = dest;
    src32 = (BYTE *)lpsurface;
    src32 += lpsrcrect->top * srcpitch;
    src32 += lpsrcrect->left;
    srcpitch = srcpitch - (4 * w);
    for(y = 0; y < h; y ++) {
        for(x = 0; x < w; x ++) {
            *(dest ++) = (*(src32 + 2) & 0xF8) << 8 | (*(src32 + 1) & 0xFC) << 3 | (*(src32 + 0) & 0xF8) >> 3;
            src32 += 4; // 4 bytes = 32 bits
        }
        dest += destpitch;
        src32 += srcpitch;
    }
}

static void WINAPI BilinearBlt_32_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    DWORD *src32;
    DWORD *dest, *dest0;
    long srcdelta, destdelta;
    DWORD x, y, w, h;
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    lpdestrect->left <<= 1;
    lpdestrect->top <<= 1;
    lpdestrect->right <<= 1;
    lpdestrect->bottom <<= 1;
    dest = (DWORD *)lpdest;
    destpitch >>= 2;
    dest += lpdestrect->top * destpitch;
    dest += lpdestrect->left;
    destdelta = destpitch - (2 * w);
    dest0 = dest;
    srcpitch >>= 2;
    src32 = (DWORD *)lpsurface;
    src32 += lpsrcrect->top * srcpitch;
    src32 += lpsrcrect->left;
    srcdelta = srcpitch - w;
    for(y = 0; y < h - 1; y ++) {
        register DWORD Q1, Q2, Q3, Q4, Q5;
        Q5 = Melt32(*(src32), *(src32 + srcpitch));
        for(x = 0; x < w; x ++) {
            Q1 = *(src32);
            Q2 = Melt32(Q1, *(src32 + 1));
            Q3 = Q5;
            Q5 = Melt32(*(src32 + 1), *(src32 + srcpitch + 1)); // to be used in next for cycle
            Q4 = Melt32(Q3, Q5);
            *(dest) = Q1;
            *(dest + 1) = Q2;
            *(dest + destpitch) = Q3;
            *(dest + destpitch + 1) = Q4;
            src32++;
            dest += 2;
        }
        src32 += srcdelta;
        dest += (destpitch + destdelta);
    }
    for(x = 0; x < w; x ++) { // last line (there's no next line to melt...)
        register DWORD Q1, Q2;
        Q1 = *(src32);
        Q2 = Melt32(Q1, *(src32 + 1));
        *(dest) = Q1;
        *(dest + 1) = Q2;
        *(dest + destpitch) = Q1;
        *(dest + destpitch + 1) = Q2;
        src32++;
        dest += 2;
    }
}

static void WINAPI HQ_X2_Blt_32_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    int w, h;
    static BOOL bInit = TRUE;
    static hqx_Type phq2x_32;
    if(bInit) {
        if(!(phq2x_32 = LoadHQX("hq2x_32_rb"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    (*phq2x_32)((DWORD *)lpsurface, srcpitch, (DWORD *)lpdest, destpitch, w, h);
}

static void WINAPI HQ_X3_Blt_32_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    int w, h;
    static BOOL bInit = TRUE;
    static hqx_Type phq3x_32;
    if(bInit) {
        if(!(phq3x_32 = LoadHQX("hq3x_32_rb"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    (*phq3x_32)((DWORD *)lpsurface, srcpitch, (DWORD *)lpdest, destpitch, w, h);
}

static void WINAPI HQ_X4_Blt_32_to_32(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    int w, h;
    static BOOL bInit = TRUE;
    static hqx_Type phq4x_32;
    if(bInit) {
        if(!(phq4x_32 = LoadHQX("hq4x_32_rb"))) return;
        bInit = FALSE;
    }
    w = lpdestrect->right - lpdestrect->left;
    h = lpdestrect->bottom - lpdestrect->top;
    (*phq4x_32)((DWORD *)lpsurface, srcpitch, (DWORD *)lpdest, destpitch, w, h);
}

static void WINAPI EmuBlt_Null(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask,
    LPVOID lpsurface, LPRECT lpsrcrect, LONG srcpitch, DWORD srcGBitMask) {
    char *sMsg = "EmuBlt: undefined color depth\n";
    OutTraceE(sMsg);
    if(IsAssertEnabled) MessageBox(0, sMsg, "EmuBlt", MB_OK | MB_ICONEXCLAMATION);
}

static void WINAPI EmuGamma_Null(
    LPVOID lpdest, LPRECT lpdestrect, LONG destpitch, DWORD dstGBitMask) {
}

//--------------------------------------------------------------------------------------------//
// exported methods
//--------------------------------------------------------------------------------------------//

void SetBltTransformations() {
    int bUnsupported = FALSE;
    static BOOL bToWarn = TRUE;
    pPrimaryBlt = PrimaryBlt;
    if(dxw.dwFlags5 & AEROBOOST) pPrimaryBlt = PrimaryStretchBlt;
    if(dxw.dwFlags5 & BILINEARFILTER) pPrimaryBlt = PrimaryBilinearBlt;
    if(dxw.dwDFlags & DOFASTBLT) pPrimaryBlt = PrimaryFastBlt; // debug opt
    if(dxw.dwDFlags & CENTERTOWIN) pPrimaryBlt = PrimaryFastBlt;
    extern HRESULT WINAPI ColorConversionDDRAW(int, LPDIRECTDRAWSURFACE, RECT, LPDIRECTDRAWSURFACE *);
    extern HRESULT WINAPI ColorConversionGDI(int, LPDIRECTDRAWSURFACE, RECT, LPDIRECTDRAWSURFACE *);
    extern HRESULT WINAPI ColorConversionEmulated(int, LPDIRECTDRAWSURFACE, RECT, LPDIRECTDRAWSURFACE *);
    switch(dxw.RendererId) {
    case DXW_RENDERER_NONE:
        pColorConversion = NULL;
        break;
    case DXW_RENDERER_EMULATEBUFFER:
        pColorConversion = ColorConversionDDRAW;
        break;
    case DXW_RENDERER_LOCKEDSURFACE:
        pColorConversion = ColorConversionEmulated;
        break;
    case DXW_RENDERER_PRIMSURFACE:
        pColorConversion = ColorConversionEmulated;
        break;
    case DXW_RENDERER_HYBRID:
        pColorConversion = ColorConversionDDRAW;
        break;
    case DXW_RENDERER_GDI:
        pColorConversion = ColorConversionGDI;
        break;
    case DXW_RENDERER_SDL:
        pColorConversion = NULL;
        break;
    case DXW_RENDERER_SDL2:
        pColorConversion = NULL;
        break;
    case DXW_RENDERER_OPENGL:
        pColorConversion = NULL;
        break;
    }
    if(pColorConversion != ColorConversionEmulated) {
        OutTraceDW("SetBltTransformations: colors=%d->%d renderer=%d(\"%s\")\n",
                   dxw.VirtualPixelFormat.dwRGBBitCount,
                   dxw.ActualPixelFormat.dwRGBBitCount,
                   dxw.Renderer->id, dxw.Renderer->name);
        return;
    }
    /* default (bad) setting */
    pEmuBlt = EmuBlt_Null;
    pGammaCorrection = EmuGamma_Null;
    switch (dxw.ActualPixelFormat.dwRGBBitCount) {
    case 32:
        pGammaCorrection = Gamma32;
        switch(dxw.VirtualPixelFormat.dwRGBBitCount) {
        case 8:
            switch(dxw.FilterId) {
            case DXW_FILTER_NONE:
                pEmuBlt = EmuBlt_8_to_32;
                break;
            case DXW_FILTER_BILX2:
                pEmuBlt = BilinearBlt_8_to_32;
                break;
            case DXW_FILTER_HQX2:
                pEmuBlt = HQ_X2_Blt_8_to_32;
                break;
            case DXW_FILTER_HQX3:
                pEmuBlt = HQ_X3_Blt_8_to_32;
                break;
            case DXW_FILTER_HQX4:
                pEmuBlt = HQ_X4_Blt_8_to_32;
                break;
            case DXW_FILTER_DEINTERLACE:
                pEmuBlt = Deinterlace_8_to_32;
                break;
            case DXW_FILTER_INTERLACE:
                pEmuBlt = Interlace_8_to_32;
                break;
            case DXW_FILTER_PIX2:
                pEmuBlt = PIX_X2_Blt_8_to_32;
                break;
            case DXW_FILTER_PIX3:
                pEmuBlt = PIX_X3_Blt_8_to_32;
                break;
            case DXW_FILTER_PIX4:
                pEmuBlt = PIX_X4_Blt_8_to_32;
                break;
            case DXW_FILTER_SCALE2X:
                pEmuBlt = Scale2X_Blt_8_to_32;
                break;
            case DXW_FILTER_SCALE3X:
                pEmuBlt = Scale3X_Blt_8_to_32;
                break;
            case DXW_FILTER_SCALE4X:
                pEmuBlt = Scale4X_Blt_8_to_32;
                break;
            case DXW_FILTER_SCALE2K:
                pEmuBlt = Scale2K_Blt_8_to_32;
                break;
            case DXW_FILTER_SCALE3K:
                pEmuBlt = Scale3K_Blt_8_to_32;
                break;
            case DXW_FILTER_SCALE4K:
                pEmuBlt = Scale4K_Blt_8_to_32;
                break;
            default:
                pEmuBlt = EmuBlt_8_to_32;
                bUnsupported = TRUE;
                break;
            }
            break;
        case 16:
            switch(dxw.FilterId) {
            case DXW_FILTER_NONE:
                pEmuBlt = EmuBlt_16_to_32;
                break;
            case DXW_FILTER_BILX2:
                pEmuBlt = BilinearBlt_16_to_32;
                break;
            case DXW_FILTER_HQX2:
                pEmuBlt = HQ_X2_Blt_16_to_32;
                break;
            case DXW_FILTER_HQX3:
                pEmuBlt = HQ_X3_Blt_16_to_32;
                break;
            case DXW_FILTER_HQX4:
                pEmuBlt = HQ_X4_Blt_16_to_32;
                break;
            case DXW_FILTER_DEINTERLACE:
                pEmuBlt = Deinterlace_16_to_32;
                break;
            case DXW_FILTER_INTERLACE:
                pEmuBlt = Interlace_16_to_32;
                break;
            case DXW_FILTER_PIX2:
                pEmuBlt = PIX_X2_Blt_16_to_32;
                break;
            case DXW_FILTER_PIX3:
                pEmuBlt = PIX_X3_Blt_16_to_32;
                break;
            case DXW_FILTER_PIX4:
                pEmuBlt = PIX_X4_Blt_16_to_32;
                break;
            case DXW_FILTER_SCALE2X:
                pEmuBlt = Scale2X_Blt_16_to_32;
                break;
            case DXW_FILTER_SCALE3X:
                pEmuBlt = Scale3X_Blt_16_to_32;
                break;
            case DXW_FILTER_SCALE4X:
                pEmuBlt = Scale4X_Blt_16_to_32;
                break;
            case DXW_FILTER_SCALE2K:
                pEmuBlt = Scale2K_Blt_16_to_32;
                break;
            case DXW_FILTER_SCALE3K:
                pEmuBlt = Scale3K_Blt_16_to_32;
                break;
            case DXW_FILTER_SCALE4K:
                pEmuBlt = Scale4K_Blt_16_to_32;
                break;
            default:
                pEmuBlt = EmuBlt_16_to_32;
                bUnsupported = TRUE;
                break;
            }
            break;
        case 24:
            pEmuBlt = EmuBlt_24_to_32;
            if(dxw.FilterId) bUnsupported = TRUE;
            break;
        case 32:
            switch(dxw.FilterId) {
            case DXW_FILTER_NONE:
                pEmuBlt = EmuBlt_32_to_32;
                break;
            case DXW_FILTER_BILX2:
                pEmuBlt = BilinearBlt_32_to_32;
                break;
            case DXW_FILTER_HQX2:
                pEmuBlt = HQ_X2_Blt_32_to_32;
                break;
            case DXW_FILTER_HQX3:
                pEmuBlt = HQ_X3_Blt_32_to_32;
                break;
            case DXW_FILTER_HQX4:
                pEmuBlt = HQ_X4_Blt_32_to_32;
                break;
            case DXW_FILTER_INTERLACE:
                pEmuBlt = Interlace_32_to_32;
                break;
            case DXW_FILTER_PIX2:
                pEmuBlt = PIX_X2_Blt_32_to_32;
                break;
            case DXW_FILTER_PIX3:
                pEmuBlt = PIX_X3_Blt_32_to_32;
                break;
            case DXW_FILTER_PIX4:
                pEmuBlt = PIX_X4_Blt_32_to_32;
                break;
            case DXW_FILTER_SCALE2X:
                pEmuBlt = Scale2X_Blt_32_to_32;
                break;
            case DXW_FILTER_SCALE3X:
                pEmuBlt = Scale3X_Blt_32_to_32;
                break;
            case DXW_FILTER_SCALE4X:
                pEmuBlt = Scale4X_Blt_32_to_32;
                break;
            case DXW_FILTER_SCALE2K:
                pEmuBlt = Scale2K_Blt_32_to_32;
                break;
            case DXW_FILTER_SCALE3K:
                pEmuBlt = Scale3K_Blt_32_to_32;
                break;
            case DXW_FILTER_SCALE4K:
                pEmuBlt = Scale4K_Blt_32_to_32;
                break;
            default:
                pEmuBlt = EmuBlt_32_to_32;
                bUnsupported = TRUE;
                break;
            }
            break;
        default:
            bUnsupported = TRUE;
            break;
        }
        break;
    case 16:
        if(dxw.ActualPixelFormat.dwGBitMask == 0x0007E0) pGammaCorrection = Gamma565;
        if(dxw.ActualPixelFormat.dwGBitMask == 0x0003E0) pGammaCorrection = Gamma555;
        switch(dxw.VirtualPixelFormat.dwRGBBitCount) {
        case 8:
            switch(dxw.FilterId) {
            case DXW_FILTER_NONE:
                pEmuBlt = EmuBlt_8_to_16;
                break;
            case DXW_FILTER_BILX2:
                pEmuBlt = BilinearBlt_8_to_16;
                break;
            //case DXW_FILTER_HQX2: pEmuBlt=HQ_X2_Blt_8_to_16; break;
            //case DXW_FILTER_HQX3: pEmuBlt=HQ_X3_Blt_8_to_16; break;
            //case DXW_FILTER_HQX4: pEmuBlt=HQ_X4_Blt_8_to_16; break;
            case DXW_FILTER_INTERLACE:
                pEmuBlt = Interlace_8_to_16;
                break;
            case DXW_FILTER_PIX2:
                pEmuBlt = PIX_X2_Blt_8_to_16;
                break;
            case DXW_FILTER_PIX3:
                pEmuBlt = PIX_X3_Blt_8_to_16;
                break;
            case DXW_FILTER_PIX4:
                pEmuBlt = PIX_X4_Blt_8_to_16;
                break;
            case DXW_FILTER_SCALE2X:
                pEmuBlt = Scale2X_Blt_8_to_16;
                break;
            case DXW_FILTER_SCALE3X:
                pEmuBlt = Scale3X_Blt_8_to_16;
                break;
            case DXW_FILTER_SCALE4X:
                pEmuBlt = Scale4X_Blt_8_to_16;
                break;
            default:
                pEmuBlt = EmuBlt_8_to_16;
                bUnsupported = TRUE;
                break;
            }
            break;
        case 16:
            switch(dxw.FilterId) {
            case DXW_FILTER_NONE:
                pEmuBlt = EmuBlt_16_to_16;
                break;
            case DXW_FILTER_BILX2:
                pEmuBlt = BilinearBlt_16_to_16;
                break;
            //case DXW_FILTER_HQX2: pEmuBlt=HQ_X2_Blt_16_to_16; break;
            //case DXW_FILTER_HQX3: pEmuBlt=HQ_X3_Blt_16_to_16; break;
            //case DXW_FILTER_HQX4: pEmuBlt=HQ_X4_Blt_16_to_16; break;
            //case DXW_FILTER_INTERLACE: pEmuBlt=Interlace_16_to_16; break;
            case DXW_FILTER_PIX2:
                pEmuBlt = PIX_X2_Blt_16_to_16;
                break;
            case DXW_FILTER_PIX3:
                pEmuBlt = PIX_X3_Blt_16_to_16;
                break;
            case DXW_FILTER_PIX4:
                pEmuBlt = PIX_X4_Blt_16_to_16;
                break;
            case DXW_FILTER_SCALE2X:
                pEmuBlt = Scale2X_Blt_16_to_16;
                break;
            case DXW_FILTER_SCALE3X:
                pEmuBlt = Scale3X_Blt_16_to_16;
                break;
            case DXW_FILTER_SCALE4X:
                pEmuBlt = Scale4X_Blt_16_to_16;
                break;
            case DXW_FILTER_SCALE2K:
                pEmuBlt = Scale2K_Blt_16_to_16;
                break;
            case DXW_FILTER_SCALE3K:
                pEmuBlt = Scale3K_Blt_16_to_16;
                break;
            case DXW_FILTER_SCALE4K:
                pEmuBlt = Scale4K_Blt_16_to_16;
                break;
            default:
                pEmuBlt = EmuBlt_16_to_16;
                bUnsupported = TRUE;
                break;
            }
            break;
        case 24:
            pEmuBlt = EmuBlt_24_to_16;
            if(dxw.FilterId) bUnsupported = TRUE;
            break;
        case 32:
            pEmuBlt = EmuBlt_32_to_16;
            if(dxw.FilterId) bUnsupported = TRUE;
            break;
        default:
            bUnsupported = TRUE;
            break;
        }
        break;
    default:
        bUnsupported = TRUE;
        break;
    }
    OutTraceDW("SetBltTransformations: colors=%d->%d renderer=%d(\"%s\") filter=%d(%s) scaling=(%d,%d) supported=%s\n",
               dxw.VirtualPixelFormat.dwRGBBitCount,
               dxw.ActualPixelFormat.dwRGBBitCount,
               dxw.Renderer->id, dxw.Renderer->name,
               dxw.FilterId, dxwFilters[dxw.FilterId].name,
               dxwFilters[dxw.FilterId].xfactor, dxwFilters[dxw.FilterId].yfactor,
               bUnsupported ? "NO" : "YES");
}
