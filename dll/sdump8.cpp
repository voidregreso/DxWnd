#ifndef DXW_NOTRACES
#include <D3D8.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhelper.h"
#include "stdio.h"
#include "dxdds.h"

#define dxerror(call) if(res) OutTrace("dx8SurfaceDump %s error %#x(%s) at %d\n", call, res, ExplainDDError(res), __LINE__)
extern int dxGetFormatColorDepth(D3DFORMAT);
extern char *ExplainD3DSurfaceFormat(DWORD);
extern unsigned int HashSurface(BYTE *, int, int, int);
extern void dxSetDDSPixelFormat(D3DFORMAT Format, DDS_PIXELFORMAT *pf);

void dx8SurfaceDump(int prog, LPDIRECTDRAWSURFACE lpdds) {
    HRESULT res;
    D3DLOCKED_RECT LockedRect;
    D3DSURFACE_DESC SurfaceDesc;
    DDS_PIXELFORMAT pf;
    int w, h, bpp, iSurfaceSize, iScanLineSize;
    char pszFile[MAX_PATH];
    LPDIRECT3DSURFACE8 lpd8 = (LPDIRECT3DSURFACE8)lpdds;
    res = lpd8->GetDesc(&SurfaceDesc);
    dxerror("GetDesc");
    if(res) return;
    res = lpd8->LockRect(&LockedRect, NULL, D3DLOCK_READONLY);
    dxerror("LockRect");
    if(res) return;
    while (TRUE) {
        w = SurfaceDesc.Width;
        h = SurfaceDesc.Height;
        bpp = dxGetFormatColorDepth(SurfaceDesc.Format);
        dxSetDDSPixelFormat(SurfaceDesc.Format, &pf);
        OutTrace("SurfaceDump: prog=%d lpdds=%#x size=(%dx%d) bpp=%d\n", prog, lpdds, w, h, bpp);
        if(bpp == 0) {
            OutTrace("SurfaceDump: SKIP 0BPP texture\n");
            break;
        }
        if((LockedRect.Pitch == 0) || (h == 0)) {
            OutTrace("SurfaceDump: SKIP void texture\n");
            break;
        }
        iSurfaceSize = h * LockedRect.Pitch;
        FILE *hf;
        BITMAPFILEHEADER hdr;       // bitmap file-header
        BITMAPV4HEADER pbi;			// bitmap info-header
        memset((void *)&pbi, 0, sizeof(BITMAPV4HEADER));
        pbi.bV4Size = sizeof(BITMAPV4HEADER);
        pbi.bV4Width = w;
        pbi.bV4Height = h;
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
        pbi.bV4RedMask = pf.dwRBitMask;
        pbi.bV4GreenMask = pf.dwGBitMask;
        pbi.bV4BlueMask = pf.dwBBitMask;
        //pbi.bV4AlphaMask = pf.dwABitMask;
        pbi.bV4AlphaMask = 0;
        pbi.bV4CSType = LCS_CALIBRATED_RGB;
        iScanLineSize = ((pbi.bV4Width * pbi.bV4BitCount + 0x1F) & ~0x1F) / 8;
        // calculate the bitmap hash
        DWORD hash;
        hash = HashSurface((BYTE *)LockedRect.pBits, LockedRect.Pitch, w, h);
        if(!hash) {
            OutTrace("SurfaceDump: lpdds=%#x hash=NULL\n", lpdds);
            break; // almost certainly, an empty black surface!
        }
        OutTrace("SurfaceDump(%d): dxversion=8 lpdds=%#x hash=%08.8X size=(%dx%d) bits=%d format=%#x(%s)\n",
                 prog, lpdds, hash,
                 w, h, bpp,
                 SurfaceDesc.Format, ExplainD3DSurfaceFormat(SurfaceDesc.Format)
                );
        sprintf_s(pszFile, MAX_PATH, "%s\\surface.out\\%05.5d.%08.8X.%03d.%03d.%s.%08X.bmp",
                  GetDxWndPath(), prog, lpdds, w, h, ExplainD3DSurfaceFormat(SurfaceDesc.Format), hash);
        //hf = fopen(pszFile, "wb");
        if(fopen_s(&hf, pszFile, "wb")) break;
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
        for(int y = 0; y < (int)h; y++)
            fwrite((BYTE *)LockedRect.pBits + (y * LockedRect.Pitch), iScanLineSize, 1, hf);
        // Close the .BMP file.
        fclose(hf);
        break;
    }
    res = lpd8->UnlockRect();
    dxerror("UnlockRect");
}
#endif // DXW_NOTRACES
