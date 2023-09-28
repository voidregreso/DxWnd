#ifndef DXW_NOTRACES
#define  _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhelper.h"
#include "dxdds.h"

extern unsigned int HashSurface(BYTE *, int, int, int);
extern char *SurfaceType(DDPIXELFORMAT);

typedef HRESULT (WINAPI *Lock_Type)(LPDIRECTDRAWSURFACE, LPRECT, LPDDSURFACEDESC, DWORD, HANDLE);
typedef HRESULT (WINAPI *Unlock4_Type)(LPDIRECTDRAWSURFACE, LPRECT);
typedef HRESULT (WINAPI *Unlock1_Type)(LPDIRECTDRAWSURFACE, LPVOID);

extern Lock_Type pLockMethod(int);
extern Unlock4_Type pUnlockMethod(int);
extern int Set_dwSize_From_Surface();
extern void dx8SurfaceDump(int, LPDIRECTDRAWSURFACE);
extern void dx9SurfaceDump(int, LPDIRECTDRAWSURFACE);

void dx17SurfaceDump(int prog, LPDIRECTDRAWSURFACE lpdds, int dxversion) {
    DDSURFACEDESC2 ddsd;
    int w, h, bpp, iSurfaceSize, iScanLineSize;
    HRESULT res;
    char pszFile[MAX_PATH];
    memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = Set_dwSize_From_Surface();
    ddsd.dwFlags = DDSD_LPSURFACE | DDSD_PITCH;
    if(res = (*pLockMethod(dxversion))(lpdds, 0, (LPDDSURFACEDESC)&ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WRITEONLY | DDLOCK_WAIT, 0)) {
        OutTrace("SurfaceDump: Lock ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        return;
    }
    while (TRUE) {
        w = ddsd.dwWidth;
        h = ddsd.dwHeight;
        bpp = ddsd.ddpfPixelFormat.dwRGBBitCount;
        OutTrace("SurfaceDump: prog=%d lpdds=%#x size=(%dx%d) bpp=%d\n", ++prog, lpdds, w, h, bpp);
        if(bpp == 0) {
            OutTrace("SurfaceDump: SKIP 0BPP texture\n");
            break;
        }
        if((ddsd.lPitch == 0) || (ddsd.dwHeight == 0)) {
            OutTrace("SurfaceDump: SKIP void texture\n");
            break;
        }
        iSurfaceSize = ddsd.dwHeight * ddsd.lPitch;
        FILE *hf;
        BITMAPFILEHEADER hdr;       // bitmap file-header
        BITMAPV4HEADER pbi;			// bitmap info-header
        memset((void *)&pbi, 0, sizeof(BITMAPV4HEADER));
        pbi.bV4Size = sizeof(BITMAPV4HEADER);
        pbi.bV4Width = ddsd.dwWidth;
        pbi.bV4Height = ddsd.dwHeight;
        pbi.bV4BitCount = (WORD)bpp;
        pbi.bV4SizeImage = ((pbi.bV4Width * pbi.bV4BitCount + 0x1F) & ~0x1F) / 8 * pbi.bV4Height;
        pbi.bV4Height = - pbi.bV4Height;
        pbi.bV4Planes = 1;
        pbi.bV4V4Compression = BI_BITFIELDS;
        if(pbi.bV4BitCount == 8) pbi.bV4V4Compression = BI_RGB;
        pbi.bV4XPelsPerMeter = 1;
        pbi.bV4YPelsPerMeter = 1;
        pbi.bV4ClrUsed = 0;
        if(pbi.bV4BitCount == 8) pbi.bV4ClrUsed = 256;
        pbi.bV4ClrImportant = 0;
        pbi.bV4RedMask = ddsd.ddpfPixelFormat.dwRBitMask;
        pbi.bV4GreenMask = ddsd.ddpfPixelFormat.dwGBitMask;
        pbi.bV4BlueMask = ddsd.ddpfPixelFormat.dwBBitMask;
        pbi.bV4AlphaMask = ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
        pbi.bV4CSType = LCS_CALIBRATED_RGB;
        iScanLineSize = ((pbi.bV4Width * pbi.bV4BitCount + 0x1F) & ~0x1F) / 8;
        // calculate the bitmap hash
        DWORD hash;
        hash = HashSurface((BYTE *)ddsd.lpSurface, ddsd.lPitch, ddsd.dwWidth, ddsd.dwHeight);
        if(!hash) {
            OutTrace("SurfaceDump: lpdds=%#x hash=NULL\n", lpdds);
            break; // almost certainly, an empty black surface!
        }
        OutTrace("SurfaceDump(%d): dxversion=%d lpdds=%#x hash=%08.8X size=(%dx%d) bits=%d mask(ARGB)=%#x.%#x.%#x.%#x\n",
                 prog, dxversion, lpdds, hash,
                 ddsd.dwWidth, ddsd.dwHeight, bpp,
                 ddsd.ddpfPixelFormat.dwRGBAlphaBitMask,
                 ddsd.ddpfPixelFormat.dwRBitMask,
                 ddsd.ddpfPixelFormat.dwGBitMask,
                 ddsd.ddpfPixelFormat.dwBBitMask
                );
        sprintf_s(pszFile, MAX_PATH, "%s\\surface.out\\%05.5d.%08.8X.%03d.%03d.%s.%08X.bmp",
                  GetDxWndPath(), prog, lpdds, ddsd.dwWidth, ddsd.dwHeight, SurfaceType(ddsd.ddpfPixelFormat), hash);
        hf = fopen(pszFile, "wb");
        if(!hf) break;
        hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"
        // Compute the size of the entire file.
        hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbi.bV4Size + pbi.bV4ClrUsed * sizeof(RGBQUAD) + pbi.bV4SizeImage);
        hdr.bfReserved1 = 0;
        hdr.bfReserved2 = 0;
        // Compute the offset to the array of color indices.
        hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbi.bV4Size + pbi.bV4ClrUsed * sizeof (RGBQUAD);
        // Copy the BITMAPFILEHEADER into the .BMP file.
        fwrite((LPVOID)&hdr, sizeof(BITMAPFILEHEADER), 1, hf);
        // Copy the BITMAPINFOHEADER array into the file.
        fwrite((LPVOID)&pbi, sizeof(BITMAPV4HEADER), 1, hf);
        // Copy the RGBQUAD array into the file.
        if(pbi.bV4ClrUsed) {
            // apply color palette. If none is defined (all entries are 0 = black)
            // try making the picture visible applying the system default palette.
            extern DWORD PaletteEntries[256];
            extern PALETTEENTRY DefaultSystemPalette[256];
            BOOL bBlackPalette = TRUE;
            for(int i = 0; i < 256; i++) if(PaletteEntries[i]) bBlackPalette = FALSE;
            if(bBlackPalette) {
                DWORD Palette[256];
                for(int i = 0; i < 256; i++) {
                    LPPALETTEENTRY lppe = &DefaultSystemPalette[i];
                    //Palette[i] = (lppe->peRed << 16) | (lppe->peGreen << 8) | lppe->peBlue;
                    Palette[i] = ((DWORD)lppe->peRed << 16) | ((DWORD)lppe->peGreen << 8) | (DWORD)lppe->peBlue;
                }
                fwrite((LPVOID)Palette, pbi.bV4ClrUsed * sizeof (RGBQUAD), 1, hf);
            } else
                fwrite((LPVOID)PaletteEntries, pbi.bV4ClrUsed * sizeof (RGBQUAD), 1, hf);
        }
        // Copy the array of color indices into the .BMP file.
        for(int y = 0; y < (int)ddsd.dwHeight; y++)
            fwrite((BYTE *)ddsd.lpSurface + (y * ddsd.lPitch), iScanLineSize, 1, hf);
        // Close the .BMP file.
        fclose(hf);
        break;
    }
    res = (*pUnlockMethod(dxversion))(lpdds, NULL);
    if (res) OutTrace("SurfaceDump: Unlock ERROR lpdds=%#x res=%#x(%s) at %d\n", lpdds, res, ExplainDDError(res), __LINE__);
}
#endif // DXW_NOTRACES
