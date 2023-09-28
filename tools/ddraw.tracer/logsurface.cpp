#define  _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include "dxdiag.h"
#include "ddraw.h"

static char *ExplainFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DDSD_");
    if (c & DDSD_CAPS) strcat(eb, "CAPS+");
    if (c & DDSD_HEIGHT) strcat(eb, "HEIGHT+");
    if (c & DDSD_WIDTH) strcat(eb, "WIDTH+");
    if (c & DDSD_PITCH) strcat(eb, "PITCH+");
    if (c & DDSD_BACKBUFFERCOUNT) strcat(eb, "BACKBUFFERCOUNT+");
    if (c & DDSD_ZBUFFERBITDEPTH) strcat(eb, "ZBUFFERBITDEPTH+");
    if (c & DDSD_ALPHABITDEPTH) strcat(eb, "ALPHABITDEPTH+");
    if (c & DDSD_LPSURFACE) strcat(eb, "LPSURFACE+");
    if (c & DDSD_PIXELFORMAT) strcat(eb, "PIXELFORMAT+");
    if (c & DDSD_CKDESTOVERLAY) strcat(eb, "CKDESTOVERLAY+");
    if (c & DDSD_CKDESTBLT) strcat(eb, "CKDESTBLT+");
    if (c & DDSD_CKSRCOVERLAY) strcat(eb, "CKSRCOVERLAY+");
    if (c & DDSD_CKSRCBLT) strcat(eb, "CKSRCBLT+");
    if (c & DDSD_MIPMAPCOUNT) strcat(eb, "MIPMAPCOUNT+");
    if (c & DDSD_REFRESHRATE) strcat(eb, "REFRESHRATE+");
    if (c & DDSD_LINEARSIZE) strcat(eb, "LINEARSIZE+");
    if (c & DDSD_TEXTURESTAGE) strcat(eb, "TEXTURESTAGE+");
    if (c & DDSD_SRCVBHANDLE) strcat(eb, "SRCVBHANDLE+");
    if (c & DDSD_DEPTH) strcat(eb, "DEPTH+");
    l = strlen(eb);
    if (l > strlen("DDSD_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

char *ExplainDDSCaps(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DDSCAPS_");
    if (c & DDSCAPS_RESERVED1) strcat(eb, "RESERVED1+");
    if (c & DDSCAPS_ALPHA) strcat(eb, "ALPHA+");
    if (c & DDSCAPS_BACKBUFFER) strcat(eb, "BACKBUFFER+");
    if (c & DDSCAPS_COMPLEX) strcat(eb, "COMPLEX+");
    if (c & DDSCAPS_FLIP) strcat(eb, "FLIP+");
    if (c & DDSCAPS_FRONTBUFFER) strcat(eb, "FRONTBUFFER+");
    if (c & DDSCAPS_OFFSCREENPLAIN) strcat(eb, "OFFSCREENPLAIN+");
    if (c & DDSCAPS_OVERLAY) strcat(eb, "OVERLAY+");
    if (c & DDSCAPS_PALETTE) strcat(eb, "PALETTE+");
    if (c & DDSCAPS_PRIMARYSURFACE) strcat(eb, "PRIMARYSURFACE+");
    if (c & DDSCAPS_SYSTEMMEMORY) strcat(eb, "SYSTEMMEMORY+");
    if (c & DDSCAPS_TEXTURE) strcat(eb, "TEXTURE+");
    if (c & DDSCAPS_3DDEVICE) strcat(eb, "3DDEVICE+");
    if (c & DDSCAPS_VIDEOMEMORY) strcat(eb, "VIDEOMEMORY+");
    if (c & DDSCAPS_VISIBLE) strcat(eb, "VISIBLE+");
    if (c & DDSCAPS_WRITEONLY) strcat(eb, "WRITEONLY+");
    if (c & DDSCAPS_ZBUFFER) strcat(eb, "ZBUFFER+");
    if (c & DDSCAPS_OWNDC) strcat(eb, "OWNDC+");
    if (c & DDSCAPS_LIVEVIDEO) strcat(eb, "LIVEVIDEO+");
    if (c & DDSCAPS_HWCODEC) strcat(eb, "HWCODEC+");
    if (c & DDSCAPS_MODEX) strcat(eb, "MODEX+");
    if (c & DDSCAPS_MIPMAP) strcat(eb, "MIPMAP+");
    if (c & DDSCAPS_RESERVED2) strcat(eb, "RESERVED2+");
    if (c & DDSCAPS_ALLOCONLOAD) strcat(eb, "ALLOCONLOAD+");
    if (c & DDSCAPS_VIDEOPORT) strcat(eb, "VIDEOPORT+");
    if (c & DDSCAPS_LOCALVIDMEM) strcat(eb, "LOCALVIDMEM+");
    if (c & DDSCAPS_NONLOCALVIDMEM) strcat(eb, "NONLOCALVIDMEM+");
    if (c & DDSCAPS_STANDARDVGAMODE) strcat(eb, "STANDARDVGAMODE+");
    if (c & DDSCAPS_OPTIMIZED) strcat(eb, "OPTIMIZED+");
    l = strlen(eb);
    if (l > strlen("DDSCAPS_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

char *ExplainDDSCaps2(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DDSCAPS2_");
    if (c & DDSCAPS2_RESERVED4) strcat(eb, "RESERVED4+");
    if (c & DDSCAPS2_HINTDYNAMIC) strcat(eb, "HINTDYNAMIC+");
    if (c & DDSCAPS2_HINTSTATIC) strcat(eb, "HINTSTATIC+");
    if (c & DDSCAPS2_TEXTUREMANAGE) strcat(eb, "TEXTUREMANAGE+");
    if (c & DDSCAPS2_RESERVED1) strcat(eb, "RESERVED1+");
    if (c & DDSCAPS2_RESERVED2) strcat(eb, "RESERVED2+");
    if (c & DDSCAPS2_OPAQUE) strcat(eb, "OPAQUE+");
    if (c & DDSCAPS2_HINTANTIALIASING) strcat(eb, "HINTANTIALIASING+");
    if (c & DDSCAPS2_CUBEMAP) strcat(eb, "CUBEMAP+");
    if (c & DDSCAPS2_CUBEMAP_POSITIVEX) strcat(eb, "CUBEMAP_POSITIVEX+");
    if (c & DDSCAPS2_CUBEMAP_NEGATIVEX) strcat(eb, "CUBEMAP_NEGATIVEX+");
    if (c & DDSCAPS2_CUBEMAP_POSITIVEY) strcat(eb, "CUBEMAP_POSITIVEY+");
    if (c & DDSCAPS2_CUBEMAP_NEGATIVEY) strcat(eb, "CUBEMAP_NEGATIVEY+");
    if (c & DDSCAPS2_CUBEMAP_POSITIVEZ) strcat(eb, "CUBEMAP_POSITIVEZ+");
    if (c & DDSCAPS2_CUBEMAP_NEGATIVEZ) strcat(eb, "CUBEMAP_NEGATIVEZ+");
    if (c & DDSCAPS2_MIPMAPSUBLEVEL) strcat(eb, "MIPMAPSUBLEVEL+");
    if (c & DDSCAPS2_D3DTEXTUREMANAGE) strcat(eb, "D3DTEXTUREMANAGE+");
    if (c & DDSCAPS2_DONOTPERSIST) strcat(eb, "DONOTPERSIST+");
    if (c & DDSCAPS2_STEREOSURFACELEFT) strcat(eb, "STEREOSURFACELEFT+");
    if (c & DDSCAPS2_VOLUME) strcat(eb, "VOLUME+");
    if (c & DDSCAPS2_NOTUSERLOCKABLE) strcat(eb, "NOTUSERLOCKABLE+");
    if (c & DDSCAPS2_POINTS) strcat(eb, "POINTS+");
    if (c & DDSCAPS2_RTPATCHES) strcat(eb, "RTPATCHES+");
    if (c & DDSCAPS2_NPATCHES) strcat(eb, "NPATCHES+");
    if (c & DDSCAPS2_RESERVED3) strcat(eb, "RESERVED3+");
    if (c & DDSCAPS2_DISCARDBACKBUFFER) strcat(eb, "DISCARDBACKBUFFER+");
    if (c & DDSCAPS2_ENABLEALPHACHANNEL) strcat(eb, "ENABLEALPHACHANNEL+");
    if (c & DDSCAPS2_EXTENDEDFORMATPRIMARY) strcat(eb, "EXTENDEDFORMATPRIMARY+");
    if (c & DDSCAPS2_ADDITIONALPRIMARY) strcat(eb, "ADDITIONALPRIMARY+");
    l = strlen(eb);
    if (l > strlen("DDSCAPS2_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

char *ExplainDDSCaps3(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DDSCAPS3_");
    if (c & DDSCAPS3_MULTISAMPLE_MASK) strcat(eb, "DDSCAPS3_MULTISAMPLE_MASK+");
    if (c & DDSCAPS3_RESERVED1) strcat(eb, "DDSCAPS3_RESERVED1+");
    if (c & DDSCAPS3_RESERVED2) strcat(eb, "DDSCAPS3_RESERVED2+");
    if (c & DDSCAPS3_LIGHTWEIGHTMIPMAP) strcat(eb, "DDSCAPS3_LIGHTWEIGHTMIPMAP+");
    if (c & DDSCAPS3_AUTOGENMIPMAP) strcat(eb, "DDSCAPS3_AUTOGENMIPMAP+");
    if (c & DDSCAPS3_DMAP) strcat(eb, "DDSCAPS3_DMAP+");
    if (c & DDSCAPS3_MULTISAMPLE_QUALITY_MASK) {
        DWORD dwQuality;
        char sQuality[32];
        dwQuality = (c & DDSCAPS3_MULTISAMPLE_QUALITY_MASK) >> DDSCAPS3_MULTISAMPLE_QUALITY_SHIFT;
        sprintf(sQuality, "QUALITY(%d)+", dwQuality);
        strcat(eb, sQuality);
    }
    l = strlen(eb);
    if (l > strlen("DDSCAPS3_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

char *ExplainPixelFormatFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DDPF_");
    if (c & DDPF_ALPHAPIXELS) strcat(eb, "ALPHAPIXELS+");
    if (c & DDPF_ALPHA) strcat(eb, "ALPHA+");
    if (c & DDPF_FOURCC) strcat(eb, "FOURCC+");
    if (c & DDPF_PALETTEINDEXED4) strcat(eb, "PALETTEINDEXED4+");
    if (c & DDPF_PALETTEINDEXEDTO8) strcat(eb, "PALETTEINDEXEDTO8+");
    if (c & DDPF_PALETTEINDEXED8) strcat(eb, "PALETTEINDEXED8+");
    if (c & DDPF_RGB) strcat(eb, "RGB+");
    if (c & DDPF_COMPRESSED) strcat(eb, "COMPRESSED+");
    if (c & DDPF_RGBTOYUV) strcat(eb, "RGBTOYUV+");
    if (c & DDPF_YUV) strcat(eb, "YUV+");
    if (c & DDPF_ZBUFFER) strcat(eb, "ZBUFFER+");
    if (c & DDPF_PALETTEINDEXED1) strcat(eb, "PALETTEINDEXED1+");
    if (c & DDPF_PALETTEINDEXED2) strcat(eb, "PALETTEINDEXED2+");
    if (c & DDPF_ZPIXELS) strcat(eb, "ZPIXELS+");
    if (c & DDPF_STENCILBUFFER) strcat(eb, "STENCILBUFFER+");
    if (c & DDPF_ALPHAPREMULT) strcat(eb, "ALPHAPREMULT+");
    if (c & DDPF_LUMINANCE) strcat(eb, "LUMINANCE+");
    if (c & DDPF_BUMPLUMINANCE) strcat(eb, "BUMPLUMINANCE+");
    if (c & DDPF_BUMPDUDV) strcat(eb, "BUMPDUDV+");
    l = strlen(eb);
    if (l > strlen("DDPF_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

static char *sFourCC(DWORD fcc) {
    static char sRet[5];
    char c;
    int i;
    char *t = &sRet[0];
    for(i = 0; i < 4; i++) {
        c = fcc & (0xFF);
        *t++ = isprint(c) ? c : '.';
        c = c >> 8;
    }
    *t = 0;
    return sRet;
}

char *ExplainPixelFormat(LPDDPIXELFORMAT ddpfPixelFormat) {
    static char sBuf[512];
    char sItem[256];
    DWORD flags = ddpfPixelFormat->dwFlags;
    sprintf(sBuf, " PixelFormat flags=%x(%s) BPP=%d",
            flags, ExplainPixelFormatFlags(flags), ddpfPixelFormat->dwRGBBitCount);
    if (flags & DDPF_RGB) {
        if (flags & DDPF_ALPHAPIXELS) {
            sprintf(sItem, " RGBA=(%x,%x,%x,%x)",
                    ddpfPixelFormat->dwRBitMask,
                    ddpfPixelFormat->dwGBitMask,
                    ddpfPixelFormat->dwBBitMask,
                    ddpfPixelFormat->dwRGBAlphaBitMask);
        } else {
            sprintf(sItem, " RGB=(%x,%x,%x)",
                    ddpfPixelFormat->dwRBitMask,
                    ddpfPixelFormat->dwGBitMask,
                    ddpfPixelFormat->dwBBitMask);
        }
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_YUV) {
        sprintf(sItem, " YUVA=(%x,%x,%x,%x)",
                ddpfPixelFormat->dwYBitMask,
                ddpfPixelFormat->dwUBitMask,
                ddpfPixelFormat->dwVBitMask,
                ddpfPixelFormat->dwYUVAlphaBitMask);
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_ZBUFFER) {
        sprintf(sItem, " SdZSbL=(%x,%x,%x,%x)",
                ddpfPixelFormat->dwStencilBitDepth,
                ddpfPixelFormat->dwZBitMask,
                ddpfPixelFormat->dwStencilBitMask,
                ddpfPixelFormat->dwLuminanceAlphaBitMask);
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_ALPHA) {
        sprintf(sItem, " LBdBlZ=(%x,%x,%x,%x)",
                ddpfPixelFormat->dwLuminanceBitMask,
                ddpfPixelFormat->dwBumpDvBitMask,
                ddpfPixelFormat->dwBumpLuminanceBitMask,
                ddpfPixelFormat->dwRGBZBitMask);
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_LUMINANCE) {
        sprintf(sItem, " BMbMF=(%x,%x,%x,%x)",
                ddpfPixelFormat->dwBumpDuBitMask,
                ddpfPixelFormat->MultiSampleCaps.wBltMSTypes,
                ddpfPixelFormat->MultiSampleCaps.wFlipMSTypes,
                ddpfPixelFormat->dwYUVZBitMask);
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_BUMPDUDV) {
        sprintf(sItem, " O=(%x)",
                ddpfPixelFormat->dwOperations);
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_FOURCC) {
        sprintf(sItem, " FourCC=%x(%s)",
                ddpfPixelFormat->dwFourCC, sFourCC(ddpfPixelFormat->dwFourCC));
        strcat(sBuf, sItem);
    }
    return sBuf;
}

void LogSurfaceAttributes(FILE *logfp, LPDDSURFACEDESC lpddsd) {
    fprintf(logfp, "SurfaceDesc: (Flags=%x(%s)",
            lpddsd->dwFlags, ExplainFlags(lpddsd->dwFlags));
    if (lpddsd->dwFlags & DDSD_BACKBUFFERCOUNT) fprintf(logfp, " BackBufferCount=%d", lpddsd->dwBackBufferCount);
    if (lpddsd->dwFlags & DDSD_WIDTH) fprintf(logfp, " Width=%d", lpddsd->dwWidth);
    if (lpddsd->dwFlags & DDSD_HEIGHT) fprintf(logfp, " Height=%d", lpddsd->dwHeight);
    if (lpddsd->dwFlags & DDSD_PITCH) fprintf(logfp, " Pitch=%d", lpddsd->lPitch);
    if (lpddsd->dwFlags & DDSD_MIPMAPCOUNT) fprintf(logfp, " MipMapCount=%d", lpddsd->dwMipMapCount);
    if (lpddsd->dwFlags & DDSD_CAPS) {
        fprintf(logfp, " Caps=%x(%s)", lpddsd->ddsCaps.dwCaps, ExplainDDSCaps(lpddsd->ddsCaps.dwCaps));
        if(lpddsd->dwSize == sizeof(DDSURFACEDESC2)) {
            LPDDSURFACEDESC2 lpddsd2 = (LPDDSURFACEDESC2)lpddsd;
            fprintf(logfp, " Caps2=%x(%s)", lpddsd2->ddsCaps.dwCaps2, ExplainDDSCaps2(lpddsd2->ddsCaps.dwCaps2));
            fprintf(logfp, " Caps3=%x(%s)", lpddsd2->ddsCaps.dwCaps3, ExplainDDSCaps3(lpddsd2->ddsCaps.dwCaps3));
        }
    }
    if (lpddsd->dwFlags & DDSD_CKDESTBLT ) fprintf(logfp, " CKDestBlt=(%x,%x)", lpddsd->ddckCKDestBlt.dwColorSpaceLowValue, lpddsd->ddckCKDestBlt.dwColorSpaceHighValue);
    if (lpddsd->dwFlags & DDSD_CKDESTOVERLAY ) fprintf(logfp, " CKDestOverlay=(%x,%x)", lpddsd->ddckCKDestOverlay.dwColorSpaceLowValue, lpddsd->ddckCKDestOverlay.dwColorSpaceHighValue);
    if (lpddsd->dwFlags & DDSD_CKSRCBLT ) fprintf(logfp, " CKSrcBlt=(%x,%x)", lpddsd->ddckCKSrcBlt.dwColorSpaceLowValue, lpddsd->ddckCKSrcBlt.dwColorSpaceHighValue);
    if (lpddsd->dwFlags & DDSD_CKSRCOVERLAY ) fprintf(logfp, " CKSrcOverlay=(%x,%x)", lpddsd->ddckCKSrcOverlay.dwColorSpaceLowValue, lpddsd->ddckCKSrcOverlay.dwColorSpaceHighValue);
    if (lpddsd->dwFlags & DDSD_PIXELFORMAT ) fprintf(logfp, " %s", ExplainPixelFormat(&(lpddsd->ddpfPixelFormat)));
    if (lpddsd->dwFlags & DDSD_LPSURFACE) fprintf(logfp, " Surface=%x", lpddsd->lpSurface);
    if (lpddsd->dwFlags & DDSD_ZBUFFERBITDEPTH) fprintf(logfp, " ZBufferBitDepth=%d", lpddsd->dwZBufferBitDepth);
    if (lpddsd->dwFlags & DDSD_ALPHABITDEPTH) fprintf(logfp, " AlphaBitDepth=%d", lpddsd->dwAlphaBitDepth);
    if (lpddsd->dwFlags & DDSD_REFRESHRATE) fprintf(logfp, " RefreshRate=%d", lpddsd->dwRefreshRate);
    if (lpddsd->dwFlags & DDSD_LINEARSIZE) fprintf(logfp, " LinearSize=%d", lpddsd->dwLinearSize);
    if (lpddsd->dwSize == sizeof(DDSURFACEDESC2)) {
        if (lpddsd->dwFlags & DDSD_TEXTURESTAGE) fprintf(logfp, " TextureStage=%x", ((LPDDSURFACEDESC2)lpddsd)->dwTextureStage);
        if (lpddsd->dwFlags & DDSD_FVF) fprintf(logfp, " FVF=%x", ((LPDDSURFACEDESC2)lpddsd)->dwFVF);
    }
    fprintf(logfp, ")");
}
