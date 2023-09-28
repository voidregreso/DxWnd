#define _CRT_SECURE_NO_DEPRECATE 1
#define DIRECTINPUT_VERSION 0x0800

#include <windows.h>
#include <stdio.h>
#include <ddraw.h>
#include <ddrawex.h>
#include <dsound.h>
#include <d3d.h>
#include <dinput.h>
#include <mciavi.h>
#include <digitalv.h>
#include "dxwnd.h"
#include "dxwcore.hpp"

#ifndef DXW_NOTRACES

// debug functions to make the log more readable

char *sGUID(GUID *guid) {
    static char sRIIDBuffer[81];
    if(((DWORD)guid & 0xFFFF0000) == 0) {
        switch ((DWORD)guid) {
        case NULL:
            strcpy(sRIIDBuffer, "NULL");
            break;
        case DDCREATE_HARDWAREONLY:
            strcpy(sRIIDBuffer, "DDCREATE_HARDWAREONLY");
            break;
        case DDCREATE_EMULATIONONLY:
            strcpy(sRIIDBuffer, "DDCREATE_EMULATIONONLY");
            break;
        default:
            sprintf(sRIIDBuffer, "ATOM(0x%04X)", (DWORD)guid);
            break;
        }
    } else {
        OLECHAR *guidString;
        StringFromCLSID(*guid, &guidString);
        sprintf_s(sRIIDBuffer, 80, "%ls",  guidString);
        ::CoTaskMemFree(guidString);
    }
    return sRIIDBuffer;
}

void DumpRect(char *what, LPRECT r, int line) {
    OutTrace("DEBUG: RECT %s at %d:", what, line);
    if (r)
        OutTrace("(%d,%d)-(%d,%d)\n", r->left, r->top, r->right, r->bottom);
    else
        OutTrace("(NULL)\n");
}

char *ExplainFlags(DWORD c) {
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
    if (c & DDSD_FVF) strcat(eb, "FVF+");
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

char *ExplainDDSCaps4(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DDSCAPS4_");
    // insert here ....
    // if (c & DDSCAPS4_XXX) strcat(eb, "XXX+");
    l = strlen(eb);
    if (l > strlen("DDSCAPS4_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

char *ExplainDDDCaps(DWORD c) {
    static char eb[512];
    unsigned int l;
    strcpy(eb, "DDCAPS_");
    if (c & DDCAPS_3D) strcat(eb, "3D+");
    if (c & DDCAPS_ALIGNBOUNDARYDEST) strcat(eb, "ALIGNBOUNDARYDEST+");
    if (c & DDCAPS_ALIGNSIZEDEST) strcat(eb, "ALIGNSIZEDEST+");
    if (c & DDCAPS_ALIGNBOUNDARYSRC) strcat(eb, "ALIGNBOUNDARYSRC+");
    if (c & DDCAPS_ALIGNSIZESRC) strcat(eb, "ALIGNSIZESRC+");
    if (c & DDCAPS_ALIGNSTRIDE) strcat(eb, "ALIGNSTRIDE+");
    if (c & DDCAPS_BLT) strcat(eb, "BLT+");
    if (c & DDCAPS_BLTQUEUE) strcat(eb, "BLTQUEUE+");
    if (c & DDCAPS_BLTFOURCC) strcat(eb, "BLTFOURCC+");
    if (c & DDCAPS_BLTSTRETCH) strcat(eb, "BLTSTRETCH+");
    if (c & DDCAPS_GDI) strcat(eb, "GDI+");
    if (c & DDCAPS_OVERLAY) strcat(eb, "OVERLAY+");
    if (c & DDCAPS_OVERLAYCANTCLIP) strcat(eb, "OVERLAYCANTCLIP+");
    if (c & DDCAPS_OVERLAYFOURCC) strcat(eb, "OVERLAYFOURCC+");
    if (c & DDCAPS_OVERLAYSTRETCH) strcat(eb, "OVERLAYSTRETCH+");
    if (c & DDCAPS_PALETTE) strcat(eb, "PALETTE+");
    if (c & DDCAPS_PALETTEVSYNC) strcat(eb, "PALETTEVSYNC+");
    if (c & DDCAPS_READSCANLINE) strcat(eb, "READSCANLINE+");
    if (c & DDCAPS_RESERVED1) strcat(eb, "RESERVED1+");
    if (c & DDCAPS_VBI) strcat(eb, "VBI+");
    if (c & DDCAPS_ZBLTS) strcat(eb, "ZBLTS+");
    if (c & DDCAPS_ZOVERLAYS) strcat(eb, "ZOVERLAYS+");
    if (c & DDCAPS_COLORKEY) strcat(eb, "COLORKEY+");
    if (c & DDCAPS_ALPHA) strcat(eb, "ALPHA+");
    if (c & DDCAPS_COLORKEYHWASSIST) strcat(eb, "COLORKEYHWASSIST+");
    if (c & DDCAPS_NOHARDWARE) strcat(eb, "NOHARDWARE+");
    if (c & DDCAPS_BLTCOLORFILL) strcat(eb, "BLTCOLORFILL+");
    if (c & DDCAPS_BANKSWITCHED) strcat(eb, "BANKSWITCHED+");
    if (c & DDCAPS_BLTDEPTHFILL) strcat(eb, "BLTDEPTHFILL+");
    if (c & DDCAPS_CANCLIP) strcat(eb, "CANCLIP+");
    if (c & DDCAPS_CANCLIPSTRETCHED) strcat(eb, "CANCLIPSTRETCHED+");
    if (c & DDCAPS_CANBLTSYSMEM) strcat(eb, "CANBLTSYSMEM+");
    l = strlen(eb);
    if (l > strlen("DDCAPS_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

char *ExplainDDDCaps2(DWORD c) {
    static char eb[512];
    unsigned int l;
    strcpy(eb, "DDCAPS2_");
    if (c & DDCAPS2_CERTIFIED) strcat(eb, "CERTIFIED+");
    if (c & DDCAPS2_NO2DDURING3DSCENE) strcat(eb, "NO2DDURING3DSCENE+");
    if (c & DDCAPS2_VIDEOPORT) strcat(eb, "VIDEOPORT+");
    if (c & DDCAPS2_AUTOFLIPOVERLAY) strcat(eb, "AUTOFLIPOVERLAY+");
    if (c & DDCAPS2_CANBOBINTERLEAVED) strcat(eb, "CANBOBINTERLEAVED+");
    if (c & DDCAPS2_CANBOBNONINTERLEAVED) strcat(eb, "CANBOBNONINTERLEAVED+");
    if (c & DDCAPS2_COLORCONTROLOVERLAY) strcat(eb, "COLORCONTROLOVERLAY+");
    if (c & DDCAPS2_COLORCONTROLPRIMARY) strcat(eb, "COLORCONTROLPRIMARY+");
    if (c & DDCAPS2_CANDROPZ16BIT) strcat(eb, "CANDROPZ16BIT+");
    if (c & DDCAPS2_NONLOCALVIDMEM) strcat(eb, "NONLOCALVIDMEM+");
    if (c & DDCAPS2_NONLOCALVIDMEMCAPS) strcat(eb, "NONLOCALVIDMEMCAPS+");
    if (c & DDCAPS2_NOPAGELOCKREQUIRED) strcat(eb, "NOPAGELOCKREQUIRED+");
    if (c & DDCAPS2_WIDESURFACES) strcat(eb, "WIDESURFACES+");
    if (c & DDCAPS2_CANFLIPODDEVEN) strcat(eb, "CANFLIPODDEVEN+");
    if (c & DDCAPS2_CANBOBHARDWARE) strcat(eb, "CANBOBHARDWARE+");
    if (c & DDCAPS2_COPYFOURCC) strcat(eb, "COPYFOURCC+");
    if (c & DDCAPS2_PRIMARYGAMMA) strcat(eb, "PRIMARYGAMMA+");
    if (c & DDCAPS2_CANRENDERWINDOWED) strcat(eb, "CANRENDERWINDOWED+");
    if (c & DDCAPS2_CANCALIBRATEGAMMA) strcat(eb, "CANCALIBRATEGAMMA+");
    if (c & DDCAPS2_FLIPINTERVAL) strcat(eb, "FLIPINTERVAL+");
    if (c & DDCAPS2_FLIPNOVSYNC) strcat(eb, "FLIPNOVSYNC+");
    if (c & DDCAPS2_CANMANAGETEXTURE) strcat(eb, "CANMANAGETEXTURE+");
    if (c & DDCAPS2_TEXMANINNONLOCALVIDMEM) strcat(eb, "TEXMANINNONLOCALVIDMEM+");
    if (c & DDCAPS2_STEREO) strcat(eb, "STEREO+");
    if (c & DDCAPS2_SYSTONONLOCAL_AS_SYSTOLOCAL) strcat(eb, "SYSTONONLOCAL_AS_SYSTOLOCAL+");
    if (c & DDCAPS2_RESERVED1) strcat(eb, "RESERVED1/PUREHAL+");
    if (c & DDCAPS2_CANMANAGERESOURCE) strcat(eb, "CANMANAGERESOURCE+");
    if (c & DDCAPS2_DYNAMICTEXTURES) strcat(eb, "DYNAMICTEXTURES+");
    if (c & DDCAPS2_CANAUTOGENMIPMAP) strcat(eb, "CANAUTOGENMIPMAP+");
    l = strlen(eb);
    if (l > strlen("DDCAPS2_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

char *ExplainDDFXALPHACaps(DWORD c) {
    static char eb[512];
    unsigned int l;
    strcpy(eb, "DDFXALPHACAPS_");
    if (c & DDFXALPHACAPS_BLTALPHAEDGEBLEND) strcat(eb, "BLTALPHAEDGEBLEND+");
    if (c & DDFXALPHACAPS_BLTALPHAPIXELS) strcat(eb, "BLTALPHAPIXELS+");
    if (c & DDFXALPHACAPS_BLTALPHAPIXELSNEG) strcat(eb, "BLTALPHAPIXELSNEG+");
    if (c & DDFXALPHACAPS_BLTALPHASURFACES) strcat(eb, "BLTALPHASURFACES+");
    if (c & DDFXALPHACAPS_BLTALPHASURFACESNEG) strcat(eb, "BLTALPHASURFACESNEG+");
    if (c & DDFXALPHACAPS_OVERLAYALPHAEDGEBLEND) strcat(eb, "OVERLAYALPHAEDGEBLEND+");
    if (c & DDFXALPHACAPS_OVERLAYALPHAPIXELS) strcat(eb, "OVERLAYALPHAPIXELS+");
    if (c & DDFXALPHACAPS_OVERLAYALPHAPIXELSNEG) strcat(eb, "OVERLAYALPHAPIXELSNEG+");
    if (c & DDFXALPHACAPS_OVERLAYALPHASURFACES) strcat(eb, "OVERLAYALPHASURFACES+");
    if (c & DDFXALPHACAPS_OVERLAYALPHASURFACESNEG) strcat(eb, "OVERLAYALPHASURFACESNEG+");
    l = strlen(eb);
    if (l > strlen("DDFXALPHACAPS_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

char *ExplainDDFXCaps(DWORD c) {
    static char eb[512];
    unsigned int l;
    strcpy(eb, "DDFXCAPS_");
    if (c & DDFXCAPS_BLTARITHSTRETCHY) strcat(eb, "BLTARITHSTRETCHY+");
    if (c & DDFXCAPS_BLTARITHSTRETCHYN) strcat(eb, "BLTARITHSTRETCHYN+");
    if (c & DDFXCAPS_BLTMIRRORLEFTRIGHT) strcat(eb, "BLTMIRRORLEFTRIGHT+");
    if (c & DDFXCAPS_BLTMIRRORUPDOWN) strcat(eb, "BLTMIRRORUPDOWN+");
    if (c & DDFXCAPS_BLTROTATION) strcat(eb, "BLTROTATION+");
    if (c & DDFXCAPS_BLTROTATION90) strcat(eb, "BLTROTATION90+");
    if (c & DDFXCAPS_BLTSHRINKX) strcat(eb, "BLTSHRINKX+");
    if (c & DDFXCAPS_BLTSHRINKXN) strcat(eb, "BLTSHRINKXN+");
    if (c & DDFXCAPS_BLTSHRINKY) strcat(eb, "BLTSHRINKY+");
    if (c & DDFXCAPS_BLTSHRINKYN) strcat(eb, "BLTSHRINKYN+");
    if (c & DDFXCAPS_BLTSTRETCHX) strcat(eb, "BLTSTRETCHX+");
    if (c & DDFXCAPS_BLTSTRETCHXN) strcat(eb, "BLTSTRETCHXN+");
    if (c & DDFXCAPS_BLTSTRETCHY) strcat(eb, "BLTSTRETCHY+");
    if (c & DDFXCAPS_BLTSTRETCHYN) strcat(eb, "BLTSTRETCHYN+");
    if (c & DDFXCAPS_OVERLAYARITHSTRETCHY) strcat(eb, "OVERLAYARITHSTRETCHY+");
    if (c & DDFXCAPS_OVERLAYARITHSTRETCHYN) strcat(eb, "OVERLAYARITHSTRETCHYN+");
    if (c & DDFXCAPS_OVERLAYSHRINKX) strcat(eb, "OVERLAYSHRINKX+");
    if (c & DDFXCAPS_OVERLAYSHRINKXN) strcat(eb, "OVERLAYSHRINKXN+");
    if (c & DDFXCAPS_OVERLAYSHRINKY) strcat(eb, "OVERLAYSHRINKY+");
    if (c & DDFXCAPS_OVERLAYSHRINKYN) strcat(eb, "OVERLAYSHRINKYN+");
    if (c & DDFXCAPS_OVERLAYSTRETCHX) strcat(eb, "OVERLAYSTRETCHX+");
    if (c & DDFXCAPS_OVERLAYSTRETCHXN) strcat(eb, "OVERLAYSTRETCHXN+");
    if (c & DDFXCAPS_OVERLAYSTRETCHY) strcat(eb, "OVERLAYSTRETCHY+");
    if (c & DDFXCAPS_OVERLAYSTRETCHYN) strcat(eb, "OVERLAYSTRETCHYN+");
    if (c & DDFXCAPS_OVERLAYMIRRORLEFTRIGHT) strcat(eb, "OVERLAYMIRRORLEFTRIGHT+");
    if (c & DDFXCAPS_OVERLAYMIRRORUPDOWN) strcat(eb, "OVERLAYMIRRORUPDOWN+");
    if (c & DDFXCAPS_OVERLAYDEINTERLACE) strcat(eb, "OVERLAYDEINTERLACE+");
    if (c & DDFXCAPS_BLTALPHA) strcat(eb, "BLTALPHA+");
    if (c & DDFXCAPS_BLTFILTER) strcat(eb, "BLTFILTER+");
    if (c & DDFXCAPS_OVERLAYALPHA) strcat(eb, "OVERLAYALPHA+");
    if (c & DDFXCAPS_BLTARITHSTRETCHY) strcat(eb, "BLTARITHSTRETCHY+");
    if (c & DDFXCAPS_OVERLAYFILTER) strcat(eb, "OVERLAYFILTER+");
    if (c & DDFXCAPS_OVERLAYARITHSTRETCHY) strcat(eb, "OVERLAYARITHSTRETCHY+");
    l = strlen(eb);
    if (l > strlen("DDFXCAPS_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

char *ExplainDDPalCaps(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DDPCAPS_");
    if (c & DDPCAPS_ALPHA) strcat(eb, "ALPHA+");
    if (c & DDPCAPS_4BIT) strcat(eb, "4BIT+");
    if (c & DDPCAPS_8BITENTRIES) strcat(eb, "8BITENTRIES+");
    if (c & DDPCAPS_8BIT) strcat(eb, "8BIT+");
    if (c & DDPCAPS_INITIALIZE) strcat(eb, "INITIALIZE+");
    if (c & DDPCAPS_PRIMARYSURFACELEFT) strcat(eb, "PRIMARYSURFACELEFT+");
    if (c & DDPCAPS_ALLOW256) strcat(eb, "ALLOW256+");
    if (c & DDPCAPS_VSYNC) strcat(eb, "VSYNC+");
    if (c & DDPCAPS_1BIT) strcat(eb, "1BIT+");
    if (c & DDPCAPS_2BIT) strcat(eb, "2BIT+");
    if (c & DDPCAPS_PRIMARYSURFACE) strcat(eb, "PRIMARYSURFACE+");
    l = strlen(eb);
    if (l > strlen("DDPCAPS_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

char *ExplainDDCKeyCaps(DWORD c) {
    static char eb[512];
    unsigned int l;
    strcpy(eb, "DDCKEYCAPS_");
    if (c & DDCKEYCAPS_DESTBLT) strcat(eb, "DESTBLT+");
    if (c & DDCKEYCAPS_DESTBLTCLRSPACE) strcat(eb, "DESTBLTCLRSPACE+");
    if (c & DDCKEYCAPS_DESTBLTCLRSPACEYUV) strcat(eb, "DESTBLTCLRSPACEYUV+");
    if (c & DDCKEYCAPS_DESTBLTYUV) strcat(eb, "DESTBLTYUV+");
    if (c & DDCKEYCAPS_DESTOVERLAY) strcat(eb, "DESTOVERLAY+");
    if (c & DDCKEYCAPS_DESTOVERLAYCLRSPACE) strcat(eb, "DESTOVERLAYCLRSPACE+");
    if (c & DDCKEYCAPS_DESTOVERLAYCLRSPACEYUV) strcat(eb, "DESTOVERLAYCLRSPACEYUV+");
    if (c & DDCKEYCAPS_DESTOVERLAYONEACTIVE) strcat(eb, "DESTOVERLAYONEACTIVE+");
    if (c & DDCKEYCAPS_DESTOVERLAYYUV) strcat(eb, "DESTOVERLAYYUV+");
    if (c & DDCKEYCAPS_SRCBLT) strcat(eb, "SRCBLT+");
    if (c & DDCKEYCAPS_SRCBLTCLRSPACE) strcat(eb, "SRCBLTCLRSPACE+");
    if (c & DDCKEYCAPS_SRCBLTCLRSPACEYUV) strcat(eb, "SRCBLTCLRSPACEYUV+");
    if (c & DDCKEYCAPS_SRCBLTYUV) strcat(eb, "SRCBLTYUV+");
    if (c & DDCKEYCAPS_SRCOVERLAY) strcat(eb, "SRCOVERLAY+");
    if (c & DDCKEYCAPS_SRCOVERLAYCLRSPACE) strcat(eb, "SRCOVERLAYCLRSPACE+");
    if (c & DDCKEYCAPS_SRCOVERLAYCLRSPACEYUV) strcat(eb, "SRCOVERLAYCLRSPACEYUV+");
    if (c & DDCKEYCAPS_SRCOVERLAYONEACTIVE) strcat(eb, "SRCOVERLAYONEACTIVE+");
    if (c & DDCKEYCAPS_SRCOVERLAYYUV) strcat(eb, "SRCOVERLAYYUV+");
    if (c & DDCKEYCAPS_NOCOSTOVERLAY) strcat(eb, "NOCOSTOVERLAY+");
    l = strlen(eb);
    if (l > strlen("DDCKEYCAPS_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

char *ExplainCoopFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DDSCL_");
    if (c & DDSCL_FULLSCREEN) strcat(eb, "FULLSCREEN+");
    if (c & DDSCL_ALLOWREBOOT) strcat(eb, "ALLOWREBOOT+");
    if (c & DDSCL_NOWINDOWCHANGES) strcat(eb, "NOWINDOWCHANGES+");
    if (c & DDSCL_NORMAL) strcat(eb, "NORMAL+");
    if (c & DDSCL_EXCLUSIVE) strcat(eb, "EXCLUSIVE+");
    if (c & DDSCL_ALLOWMODEX) strcat(eb, "ALLOWMODEX+");
    if (c & DDSCL_SETFOCUSWINDOW) strcat(eb, "SETFOCUSWINDOW+");
    if (c & DDSCL_SETDEVICEWINDOW) strcat(eb, "SETDEVICEWINDOW+");
    if (c & DDSCL_CREATEDEVICEWINDOW) strcat(eb, "CREATEDEVICEWINDOW+");
    if (c & DDSCL_MULTITHREADED) strcat(eb, "MULTITHREADED+");
    if (c & DDSCL_FPUSETUP) strcat(eb, "FPUSETUP+");
    if (c & DDSCL_FPUPRESERVE) strcat(eb, "FPUPRESERVE+");
    l = strlen(eb);
    if (l > strlen("DDSCL_")) eb[l - 1] = 0; // delete last '+' if any
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

char *ExplainFlipFlags(DWORD c) {
    static char eb[512];
    unsigned int l;
    strcpy(eb, "DDFLIP_");
    if (c & DDFLIP_WAIT) strcat(eb, "WAIT+");
    if (c & DDFLIP_EVEN) strcat(eb, "EVEN+");
    if (c & DDFLIP_ODD) strcat(eb, "ODD+");
    if (c & DDFLIP_NOVSYNC) strcat(eb, "NOVSYNC+");
    if (c & DDFLIP_INTERVAL3) strcat(eb, "INTERVAL3+");
    else if (c & DDFLIP_INTERVAL2) strcat(eb, "INTERVAL2+");
    if (c & DDFLIP_INTERVAL4) strcat(eb, "INTERVAL4+");
    if (c & DDFLIP_STEREO) strcat(eb, "STEREO+");
    if (c & DDFLIP_DONOTWAIT) strcat(eb, "DONOTWAIT+");
    l = strlen(eb);
    if (l > strlen("DDFLIP_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

char *ExplainBltFlags(DWORD c) {
    static char eb[512];
    unsigned int l;
    strcpy(eb, "DDBLT_");
    if (c & DDBLT_ALPHADEST) strcat(eb, "ALPHADEST+");
    if (c & DDBLT_ALPHADESTCONSTOVERRIDE) strcat(eb, "ALPHADESTCONSTOVERRIDE+");
    if (c & DDBLT_ALPHADESTNEG) strcat(eb, "ALPHADESTNEG+");
    if (c & DDBLT_ALPHADESTSURFACEOVERRIDE) strcat(eb, "ALPHADESTSURFACEOVERRIDE+");
    if (c & DDBLT_ALPHAEDGEBLEND) strcat(eb, "ALPHAEDGEBLEND+");
    if (c & DDBLT_ALPHASRC) strcat(eb, "ALPHASRC+");
    if (c & DDBLT_ALPHASRCCONSTOVERRIDE) strcat(eb, "ALPHASRCCONSTOVERRIDE+");
    if (c & DDBLT_ALPHASRCNEG) strcat(eb, "ALPHASRCNEG+");
    if (c & DDBLT_ALPHASRCSURFACEOVERRIDE) strcat(eb, "ALPHASRCSURFACEOVERRIDE+");
    if (c & DDBLT_ASYNC) strcat(eb, "ASYNC+");
    if (c & DDBLT_COLORFILL) strcat(eb, "COLORFILL+");
    if (c & DDBLT_DDFX) strcat(eb, "DDFX+");
    if (c & DDBLT_DDROPS) strcat(eb, "DDROPS+");
    if (c & DDBLT_KEYDEST) strcat(eb, "KEYDEST+");
    if (c & DDBLT_KEYDESTOVERRIDE) strcat(eb, "KEYDESTOVERRIDE+");
    if (c & DDBLT_KEYSRC) strcat(eb, "KEYSRC+");
    if (c & DDBLT_KEYSRCOVERRIDE) strcat(eb, "KEYSRCOVERRIDE+");
    if (c & DDBLT_ROP) strcat(eb, "ROP+");
    if (c & DDBLT_ROTATIONANGLE) strcat(eb, "ROTATIONANGLE+");
    if (c & DDBLT_ZBUFFER) strcat(eb, "ZBUFFER+");
    if (c & DDBLT_ZBUFFERDESTCONSTOVERRIDE) strcat(eb, "ZBUFFERDESTCONSTOVERRIDE+");
    if (c & DDBLT_ZBUFFERDESTOVERRIDE) strcat(eb, "ZBUFFERDESTOVERRIDE+");
    if (c & DDBLT_ZBUFFERSRCCONSTOVERRIDE) strcat(eb, "ZBUFFERSRCCONSTOVERRIDE+");
    if (c & DDBLT_ZBUFFERSRCOVERRIDE) strcat(eb, "ZBUFFERSRCOVERRIDE+");
    if (c & DDBLT_WAIT) strcat(eb, "WAIT+");
    if (c & DDBLT_DEPTHFILL) strcat(eb, "DEPTHFILL+");
    if (c & DDBLT_DONOTWAIT) strcat(eb, "DONOTWAIT+");
    if (c & DDBLT_ROTATIONANGLE) strcat(eb, "ROTATIONANGLE+");
    l = strlen(eb);
    if (l > strlen("DDBLT_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

char *ExplainBltFastFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DDBLTFAST_");
    if (!(c & (DDBLTFAST_SRCCOLORKEY | DDBLTFAST_DESTCOLORKEY))) strcat(eb, "NOCOLORKEY+");
    if (c & DDBLTFAST_SRCCOLORKEY) strcat(eb, "SRCCOLORKEY+");
    if (c & DDBLTFAST_DESTCOLORKEY) strcat(eb, "DESTCOLORKEY+");
    if (c & DDBLTFAST_WAIT) strcat(eb, "WAIT+");
    if (c & DDBLTFAST_DONOTWAIT) strcat(eb, "DONOTWAIT+");
    l = strlen(eb);
    if (l > strlen("DDBLTFAST_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

#define DDPCAPS_INITIALIZE_LEGACY 0x00000008l

char *ExplainCreatePaletteFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DDPCAPS_");
    if (c & DDPCAPS_4BIT) strcat(eb, "4BIT+");
    if (c & DDPCAPS_8BITENTRIES) strcat(eb, "8BITENTRIES+");
    if (c & DDPCAPS_8BIT) strcat(eb, "8BIT+");
    //if (c & DDPCAPS_INITIALIZE) strcat(eb, "INITIALIZE+");
    // DDPCAPS_INITIALIZE is obsolete and redefined to 0x0, but that is not the legacy value embedded in assembly!
    if (c & DDPCAPS_INITIALIZE_LEGACY) strcat(eb, "INITIALIZE+");
    if (c & DDPCAPS_PRIMARYSURFACE) strcat(eb, "PRIMARYSURFACE+");
    if (c & DDPCAPS_PRIMARYSURFACELEFT) strcat(eb, "PRIMARYSURFACELEFT+");
    if (c & DDPCAPS_ALLOW256) strcat(eb, "ALLOW256+");
    if (c & DDPCAPS_VSYNC) strcat(eb, "VSYNC+");
    if (c & DDPCAPS_1BIT) strcat(eb, "1BIT+");
    if (c & DDPCAPS_2BIT) strcat(eb, "2BIT+");
    if (c & DDPCAPS_ALPHA) strcat(eb, "ALPHA+");
    l = strlen(eb);
    if (l > strlen("DDPCAPS_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

char *ExplainROP(DWORD c) {
    static char *eb;
    switch(c) {
    case SRCCOPY:
        eb = "SRCCOPY";
        break;
    case SRCPAINT:
        eb = "SRCPAINT";
        break;
    case SRCAND:
        eb = "SRCAND";
        break;
    case SRCINVERT:
        eb = "SRCINVERT";
        break;
    case SRCERASE:
        eb = "SRCERASE";
        break;
    case NOTSRCCOPY:
        eb = "NOTSRCCOPY";
        break;
    case NOTSRCERASE:
        eb = "NOTSRCERASE";
        break;
    case MERGECOPY:
        eb = "MERGECOPY";
        break;
    case MERGEPAINT:
        eb = "MERGEPAINT";
        break;
    case PATCOPY:
        eb = "PATCOPY";
        break;
    case PATPAINT:
        eb = "PATPAINT";
        break;
    case PATINVERT:
        eb = "PATINVERT";
        break;
    case DSTINVERT:
        eb = "DSTINVERT";
        break;
    case BLACKNESS:
        eb = "BLACKNESS";
        break;
    case WHITENESS:
        eb = "WHITENESS";
        break;
    case NOMIRRORBITMAP:
        eb = "NOMIRRORBITMAP";
        break;
    case CAPTUREBLT:
        eb = "CAPTUREBLT";
        break;
    default:
        eb = "unknown";
        break;
    }
    return(eb);
}

char *ExplainLockFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DDLOCK_");
    if (c & DDLOCK_WAIT) strcat(eb, "WAIT+");
    if (c & DDLOCK_EVENT) strcat(eb, "EVENT+");
    if (c & DDLOCK_READONLY) strcat(eb, "READONLY+");
    if (c & DDLOCK_WRITEONLY) strcat(eb, "WRITEONLY+");
    if (c & DDLOCK_NOSYSLOCK) strcat(eb, "NOSYSLOCK+");
    if (c & DDLOCK_NOOVERWRITE) strcat(eb, "NOOVERWRITE+");
    if (c & DDLOCK_DISCARDCONTENTS) strcat(eb, "DISCARDCONTENTS+");
    if (c & DDLOCK_DONOTWAIT) strcat(eb, "DONOTWAIT+");
    if (c & DDLOCK_HASVOLUMETEXTUREBOXRECT) strcat(eb, "HASVOLUMETEXTUREBOXRECT+");
    if (c & DDLOCK_NODIRTYUPDATE) strcat(eb, "NODIRTYUPDATE+");
    l = strlen(eb);
    if (l > strlen("DDLOCK_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "DDLOCK_SURFACEMEMORYPTR"); // when zero...
    return(eb);
}

char *ExplainStyle(DWORD c) {
    static char eb[512];
    unsigned int l;
    strcpy(eb, "WS_");
    if (c & WS_BORDER) strcat(eb, "BORDER+");
    if (c & WS_CAPTION) strcat(eb, "CAPTION+");
    if (c & WS_CHILD) strcat(eb, "CHILD+");
    if (c & WS_CLIPCHILDREN) strcat(eb, "CLIPCHILDREN+");
    if (c & WS_CLIPSIBLINGS) strcat(eb, "CLIPSIBLINGS+");
    if (c & WS_DISABLED) strcat(eb, "DISABLED+");
    if (c & WS_DLGFRAME) strcat(eb, "DLGFRAME+");
    if (c & WS_GROUP) strcat(eb, "GROUP+");
    if (c & WS_HSCROLL) strcat(eb, "HSCROLL+");
    if (c & WS_MAXIMIZE) strcat(eb, "MAXIMIZE+");
    if (c & WS_MAXIMIZEBOX) strcat(eb, "MAXIMIZEBOX+");
    if (c & WS_MINIMIZE) strcat(eb, "MINIMIZE+");
    if (c & WS_MINIMIZEBOX) strcat(eb, "MINIMIZEBOX+");
    if (c & WS_POPUP) strcat(eb, "POPUP+");
    if (c & WS_SIZEBOX) strcat(eb, "SIZEBOX+");
    if (c & WS_SYSMENU) strcat(eb, "SYSMENU+");
    if (c & WS_TABSTOP) strcat(eb, "TABSTOP+");
    if (c & WS_THICKFRAME) strcat(eb, "THICKFRAME+");
    if (c & WS_TILED) strcat(eb, "TILED+");
    if (c & WS_VISIBLE) strcat(eb, "VISIBLE+");
    if (c & WS_VSCROLL) strcat(eb, "VSCROLL+");
    l = strlen(eb);
    if (l > strlen("WS_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "WS_OVERLAPPED"); // when zero ...
    char *s = &eb[strlen(eb)];
    strcat(s, "+CS_");
    if (c & CS_VREDRAW) strcat(eb, "VREDRAW+");
    if (c & CS_HREDRAW) strcat(eb, "HREDRAW+");
    if (c & CS_DBLCLKS) strcat(eb, "DBLCLKS+");
    if (c & CS_OWNDC) strcat(eb, "OWNDC+");
    if (c & CS_CLASSDC) strcat(eb, "CLASSDC+");
    if (c & CS_PARENTDC) strcat(eb, "PARENTDC+");
    if (c & CS_NOCLOSE) strcat(eb, "NOCLOSE+");
    if (c & CS_SAVEBITS) strcat(eb, "SAVEBITS+");
    if (c & CS_BYTEALIGNCLIENT) strcat(eb, "BYTEALIGNCLIENT+");
    if (c & CS_BYTEALIGNWINDOW) strcat(eb, "BYTEALIGNWINDOW+");
    if (c & CS_GLOBALCLASS) strcat(eb, "GLOBALCLASS+");
    if (c & CS_IME) strcat(eb, "IME+");
    if (c & CS_DROPSHADOW) strcat(eb, "DROPSHADOW+");
    l = strlen(s);
    if (l > strlen("+CS_")) s[l - 1] = 0; // delete last '+' if any
    else *s = 0; // when zero ...
    return(eb);
}

char *ExplainExStyle(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "WS_EX_");
    if (c & WS_EX_ACCEPTFILES) strcat(eb, "ACCEPTFILES+");
    if (c & WS_EX_APPWINDOW) strcat(eb, "APPWINDOW+");
    if (c & WS_EX_CLIENTEDGE) strcat(eb, "CLIENTEDGE+");
    //if (c & WS_EX_COMPOSITED) strcat(eb, "COMPOSITED+");
    if (c & WS_EX_CONTEXTHELP) strcat(eb, "CONTEXTHELP+");
    if (c & WS_EX_CONTROLPARENT) strcat(eb, "CONTROLPARENT+");
    if (c & WS_EX_DLGMODALFRAME) strcat(eb, "DLGMODALFRAME+");
    //if (c & WS_EX_LAYERED) strcat(eb, "LAYERED+");
    if (c & WS_EX_LAYOUTRTL) strcat(eb, "LAYOUTRTL+");
    if (c & WS_EX_LEFT) strcat(eb, "LEFT+");
    if (c & WS_EX_LEFTSCROLLBAR) strcat(eb, "LEFTSCROLLBAR+");
    if (c & WS_EX_LTRREADING) strcat(eb, "LTRREADING+");
    if (c & WS_EX_MDICHILD) strcat(eb, "MDICHILD+");
    //if (c & WS_EX_NOACTIVATE) strcat(eb, "NOACTIVATE+");
    if (c & WS_EX_NOINHERITLAYOUT) strcat(eb, "NOINHERITLAYOUT+");
    if (c & WS_EX_NOPARENTNOTIFY) strcat(eb, "NOPARENTNOTIFY+");
    if (c & WS_EX_RIGHT) strcat(eb, "RIGHT+");
    if (c & WS_EX_RTLREADING) strcat(eb, "RTLREADING+");
    if (c & WS_EX_STATICEDGE) strcat(eb, "STATICEDGE+");
    if (c & WS_EX_TOOLWINDOW) strcat(eb, "TOOLWINDOW+");
    if (c & WS_EX_TOPMOST) strcat(eb, "TOPMOST+");
    if (c & WS_EX_TRANSPARENT) strcat(eb, "TRANSPARENT+");
    if (c & WS_EX_WINDOWEDGE) strcat(eb, "WINDOWEDGE+");
    l = strlen(eb);
    if (l > strlen("WS_EX_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "WS_EX_RIGHTSCROLLBAR"); // when zero ...
    return(eb);
}

char *ExplainShowCmd(int c) {
    static char *eb;
    switch(c) {
    case SW_HIDE:
        eb = "SW_HIDE";
        break; // 0
    case SW_SHOWNORMAL:
        eb = "SW_SHOWNORMAL";
        break; // 1
    case SW_SHOWMINIMIZED:
        eb = "SW_SHOWMINIMIZED";
        break; // 2
    case SW_MAXIMIZE:
        eb = "SW_MAXIMIZE";
        break; // 3
    //case SW_SHOWMAXIMIZED: eb="SW_SHOWMAXIMIZED"; break; // 3 = SW_MAXIMIZE
    case SW_SHOWNOACTIVATE:
        eb = "SW_SHOWNOACTIVATE";
        break; // 4
    case SW_SHOW:
        eb = "SW_SHOW";
        break; // 5
    case SW_MINIMIZE:
        eb = "SW_MINIMIZE";
        break; // 6
    case SW_SHOWMINNOACTIVE:
        eb = "SW_SHOWMINNOACTIVE";
        break; // 7
    case SW_SHOWNA:
        eb = "SW_SHOWNA";
        break; // 8
    case SW_RESTORE:
        eb = "SW_RESTORE";
        break; // 9
    case SW_SHOWDEFAULT:
        eb = "SW_SHOWDEFAULT";
        break; // 10
    case SW_FORCEMINIMIZE:
        eb = "SW_FORCEMINIMIZE";
        break; // 11
    default:
        eb = "unknown";
        break;
    }
    return(eb);
}

char *ExplainBltStatus(DWORD c) {
    static char *eb;
    switch(c) {
    case DDGBS_CANBLT:
        eb = "DDGBS_CANBLT";
        break;
    case DDGBS_ISBLTDONE:
        eb = "DDGBS_ISBLTDONE";
        break;
    default:
        eb = "invalid";
        break;
    }
    return(eb);
}

char *ExplainFlipStatus(DWORD c) {
    static char *eb;
    switch(c) {
    case DDGFS_CANFLIP:
        eb = "DDGFS_CANFLIP";
        break;
    case DDGFS_ISFLIPDONE:
        eb = "DDGFS_ISFLIPDONE";
        break;
    default:
        eb = "invalid";
        break;
    }
    return(eb);
}


char *ExplainResizing(DWORD c) {
    static char *eb;
    switch(c) {
    case SIZE_MAXHIDE:
        eb = "SIZE_MAXHIDE";
        break;
    case SIZE_MAXIMIZED:
        eb = "SIZE_MAXIMIZED";
        break;
    case SIZE_MAXSHOW:
        eb = "SIZE_MAXSHOW";
        break;
    case SIZE_MINIMIZED:
        eb = "SIZE_MINIMIZED";
        break;
    case SIZE_RESTORED:
        eb = "SIZE_RESTORED";
        break;
    default:
        eb = "???";
        break;
    }
    return eb;
}

char *ExplainDeviceCaps(DWORD c) {
    static char *eb;
    switch(c) {
    case DRIVERVERSION:
        eb = "DRIVERVERSION";
        break;
    case TECHNOLOGY:
        eb = "TECHNOLOGY";
        break;
    case HORZSIZE:
        eb = "HORZSIZE";
        break;
    case VERTSIZE:
        eb = "VERTSIZE";
        break;
    case HORZRES:
        eb = "HORZRES";
        break;
    case VERTRES:
        eb = "VERTRES";
        break;
    case LOGPIXELSX:
        eb = "LOGPIXELSX";
        break;
    case LOGPIXELSY:
        eb = "LOGPIXELSY";
        break;
    case BITSPIXEL:
        eb = "BITSPIXEL";
        break;
    case PLANES:
        eb = "PLANES";
        break;
    case NUMBRUSHES:
        eb = "NUMBRUSHES";
        break;
    case NUMPENS:
        eb = "NUMPENS";
        break;
    case NUMFONTS:
        eb = "NUMFONTS";
        break;
    case NUMCOLORS:
        eb = "NUMCOLORS";
        break;
    case ASPECTX:
        eb = "ASPECTX";
        break;
    case ASPECTY:
        eb = "ASPECTY";
        break;
    case ASPECTXY:
        eb = "ASPECTXY";
        break;
    case PDEVICESIZE:
        eb = "PDEVICESIZE";
        break;
    case CLIPCAPS:
        eb = "CLIPCAPS";
        break;
    case SIZEPALETTE:
        eb = "SIZEPALETTE";
        break;
    case NUMRESERVED:
        eb = "NUMRESERVED";
        break;
    case COLORRES:
        eb = "COLORRES";
        break;
    case PHYSICALWIDTH:
        eb = "PHYSICALWIDTH";
        break;
    case PHYSICALHEIGHT:
        eb = "PHYSICALHEIGHT";
        break;
    case PHYSICALOFFSETX:
        eb = "PHYSICALOFFSETX";
        break;
    case PHYSICALOFFSETY:
        eb = "PHYSICALOFFSETY";
        break;
    case VREFRESH:
        eb = "VREFRESH";
        break;
    case SCALINGFACTORX:
        eb = "SCALINGFACTORX";
        break;
    case SCALINGFACTORY:
        eb = "SCALINGFACTORY";
        break;
    case BLTALIGNMENT:
        eb = "BLTALIGNMENT";
        break;
    case SHADEBLENDCAPS:
        eb = "SHADEBLENDCAPS";
        break;
    case RASTERCAPS:
        eb = "RASTERCAPS";
        break;
    case CURVECAPS:
        eb = "CURVECAPS";
        break;
    case LINECAPS:
        eb = "LINECAPS";
        break;
    case POLYGONALCAPS:
        eb = "POLYGONALCAPS";
        break;
    case TEXTCAPS:
        eb = "TEXTCAPS";
        break;
    case COLORMGMTCAPS:
        eb = "COLORMGMTCAPS";
        break;
    default:
        eb = "???";
        break;
    }
    return eb;
}

char *ExplainDisplaySettingsRetcode(DWORD c) {
    static char *eb;
    switch(c) {
    case DISP_CHANGE_SUCCESSFUL:
        eb = "DISP_CHANGE_SUCCESSFUL";
        break;
    //case DISP_CHANGE_BADDUALVIEW:	eb="DISP_CHANGE_BADDUALVIEW"; break;
    case DISP_CHANGE_BADFLAGS:
        eb = "DISP_CHANGE_BADFLAGS";
        break;
    case DISP_CHANGE_BADMODE:
        eb = "DISP_CHANGE_BADMODE";
        break;
    case DISP_CHANGE_BADPARAM:
        eb = "DISP_CHANGE_BADPARAM";
        break;
    case DISP_CHANGE_FAILED:
        eb = "DISP_CHANGE_FAILED";
        break;
    case DISP_CHANGE_NOTUPDATED:
        eb = "DISP_CHANGE_NOTUPDATED";
        break;
    case DISP_CHANGE_RESTART:
        eb = "DISP_CHANGE_RESTART";
        break;
    }
    return eb;
}

char *ExplainSetWindowIndex(DWORD c) {
    static char *eb;
    switch(c) {
    case GWL_EXSTYLE:
        eb = "GWL_EXSTYLE";
        break;
    case GWL_HINSTANCE:
        eb = "GWL_HINSTANCE";
        break;
    case GWL_ID:
        eb = "GWL_ID";
        break;
    case GWL_STYLE:
        eb = "GWL_STYLE";
        break;
    case GWL_USERDATA:
        eb = "GWL_USERDATA";
        break;
    case GWL_WNDPROC:
        eb = "GWL_WNDPROC";
        break;
    case GWL_HWNDPARENT:
        eb = "GWL_HWNDPARENT";
        break;
    case DWL_DLGPROC:
        eb = "DWL_DLGPROC";
        break;
    case DWL_MSGRESULT:
        eb = "DWL_MSGRESULT";
        break;
    case DWL_USER:
        eb = "DWL_USER";
        break;
    default:
        eb = "";
        break;
    }
    return eb;
}

char *ExplainColorKeyFlag(DWORD c) {
    static char *eb;
    switch(c) {
    case DDCKEY_COLORSPACE:
        eb = "DDCKEY_COLORSPACE";
        break;
    case DDCKEY_DESTBLT:
        eb = "DDCKEY_DESTBLT";
        break;
    case DDCKEY_DESTOVERLAY:
        eb = "DDCKEY_DESTOVERLAY";
        break;
    case DDCKEY_SRCBLT:
        eb = "DDCKEY_SRCBLT";
        break;
    case DDCKEY_SRCOVERLAY:
        eb = "DDCKEY_SRCOVERLAY";
        break;
    default:
        eb = "";
        break;
    }
    return eb;
}

char *ExplainGUID(GUID FAR *lpguid) {
    static char *sguid;
    switch ((DWORD)lpguid) {
    case NULL:
        return "NULL";
        break;
    case DDCREATE_HARDWAREONLY:
        return "DDCREATE_HARDWAREONLY";
        break;
    case DDCREATE_EMULATIONONLY:
        return "DDCREATE_EMULATIONONLY";
        break;
    }
    switch (lpguid->Data1) {
    // COM
    case 0x00000000:
        sguid = "IID_IUnknown";
        break;
    case 0x0000002F:
        sguid = "IID_RecordInfo";
        break;
    case 0x00020400:
        sguid = "IID_IDispatch";
        break;
    case 0x00020403:
        sguid = "IID_ITypeComp";
        break;
    case 0x00020401:
        sguid = "IID_ITypeInfo";
        break;
    case 0x00020412:
        sguid = "IID_ITypeInfo2";
        break;
    case 0x00020402:
        sguid = "IID_ITypeLib";
        break;
    case 0x00020404:
        sguid = "IID_IEnumVARIANT";
        break;
    // DirectDraw
    case 0x6C14DB80:
        sguid = "IID_IDirectDraw";
        break;
    case 0xB3A6F3E0:
        sguid = "IID_IDirectDraw2";
        break;
    case 0x618f8ad4:
        sguid = "IID_IDirectDraw3";
        break;
    case 0x9c59509a:
        sguid = "IID_IDirectDraw4";
        break;
    case 0x15e65ec0:
        sguid = "IID_IDirectDraw7";
        break;
    case 0x6C14DB81:
        sguid = "IID_IDirectDrawSurface";
        break;
    case 0x57805885:
        sguid = "IID_IDirectDrawSurface2";
        break;
    case 0xDA044E00:
        sguid = "IID_IDirectDrawSurface3";
        break;
    case 0x0B2B8630:
        sguid = "IID_IDirectDrawSurface4";
        break;
    case 0x06675a80:
        sguid = "IID_IDirectDrawSurface7";
        break;
    case 0x6C14DB84:
        sguid = "IID_IDirectDrawPalette";
        break;
    case 0x6C14DB85:
        sguid = "IID_IDirectDrawClipper";
        break;
    case 0x4B9F0EE0:
        sguid = "IID_IDirectDrawColorControl";
        break;
    case 0x69C11C3E:
        sguid = "IID_IDirectDrawGammaControl";
        break;
    case 0xA4665C60:
        sguid = "IID_IDirect3DRGBDevice";
        break;
    case 0x84E63dE0:
        sguid = "IID_IDirect3DHALDevice";
        break;
    case 0x3BBA0080:
        sguid = "IID_IDirect3D";
        break;
    case 0x6aae1ec1:
        sguid = "IID_IDirect3D2";
        break;
    case 0xbb223240:
        sguid = "IID_IDirect3D3";
        break;
    case 0xf5049e77:
        sguid = "IID_IDirect3D7";
        break;
    case 0x64108800:
        sguid = "IID_IDirect3DDevice";
        break;
    case 0x93281501:
        sguid = "IID_IDirect3DDevice2";
        break;
    case 0xb0ab3b60:
        sguid = "IID_IDirect3DDevice3";
        break;
    case 0xf5049e79:
        sguid = "IID_IDirect3DDevice7";
        break;
    case 0x2CDCD9E0:
        sguid = "IID_IDirect3DTexture";
        break;
    case 0x93281502:
        sguid = "IID_IDirect3DTexture2";
        break;
    case 0x4417C142:
        sguid = "IID_IDirect3DLight";
        break;
    case 0x4417C144:
        sguid = "IID_IDirect3DMaterial";
        break;
    case 0x93281503:
        sguid = "IID_IDirect3DMaterial2";
        break;
    case 0xca9c46f4:
        sguid = "IID_IDirect3DMaterial3";
        break;
    case 0x4417C145:
        sguid = "IID_IDirect3DExecuteBuffer";
        break;
    case 0x4417C146:
        sguid = "IID_IDirect3DViewport";
        break;
    case 0x93281500:
        sguid = "IID_IDirect3DViewport2";
        break;
    case 0xb0ab3b61:
        sguid = "IID_IDirect3DViewport3";
        break;
    case 0x7a503555:
        sguid = "IID_IDirect3DVertexBuffer";
        break;
    case 0xf5049e7d:
        sguid = "IID_IDirect3DVertexBuffer7";
        break;
    case 0xF2086B20:
        sguid = "IID_IDirect3DRampDevice";
        break;
    case 0x881949a1:
        sguid = "IID_IDirect3DMMXDevice";
        break;
    case 0x50936643:
        sguid = "IID_IDirect3DRefDevice";
        break;
    case 0x8767df22:
        sguid = "IID_IDirect3DNullDevice";
        break;
    case 0xf5049e78:
        sguid = "IID_IDirect3DTnLHalDevice";
        break;
    case 0xD7B70EE0:
        sguid = "CLSID_DirectDraw";
        break;
    case 0x3c305196:
        sguid = "CLSID_DirectDraw7";
        break;
    case 0x593817A0:
        sguid = "CLSID_DirectDrawClipper";
        break;
    case 0x89521360:
        sguid = "IID_IDirectInputA";
        break;
    case 0x89521361:
        sguid = "IID_IDirectInputW";
        break;
    case 0x5944E662:
        sguid = "IID_IDirectInput2A";
        break;
    case 0x5944E663:
        sguid = "IID_IDirectInput2W";
        break;
    case 0x9A4CB684:
        sguid = "IID_IDirectInput7A";
        break;
    case 0x9A4CB685:
        sguid = "IID_IDirectInput7W";
        break;
    case 0xBF798030:
        sguid = "IID_IDirectInput8A";
        break;
    case 0xBF798031:
        sguid = "IID_IDirectInput8W";
        break;
    case 0x5944E680:
        sguid = "IID_IDirectInputDeviceA";
        break;
    case 0x5944E681:
        sguid = "IID_IDirectInputDeviceW";
        break;
    case 0x5944E682:
        sguid = "IID_IDirectInputDevice2A";
        break;
    case 0x5944E683:
        sguid = "IID_IDirectInputDevice2W";
        break;
    case 0x57D7C6BC:
        sguid = "IID_IDirectInputDevice7A";
        break;
    case 0x57D7C6BD:
        sguid = "IID_IDirectInputDevice7W";
        break;
    case 0x54D41080:
        sguid = "IID_IDirectInputDevice8A";
        break;
    case 0x54D41081:
        sguid = "IID_IDirectInputDevice8W";
        break;
    case 0xE7E1F7C0:
        sguid = "IID_IDirectInputEffect";
        break;
    case 0x1dd9e8da:
        sguid = "IID_IDirect3D8";
        break;
    case 0x81bdcbca:
        sguid = "IID_IDirect3D9";
        break;
    case 0x02177241:
        sguid = "IID_IDirect3D9Ex";
        break;
    case 0x7385e5df:
        sguid = "IID_IDirect3DDevice8";
        break;
    case 0xd0223b96:
        sguid = "IID_IDirect3DDevice9";
        break;
    case 0xb18b10ce:
        sguid = "IID_IDirect3DDevice9Ex";
        break;
    case 0xb4211cfa:
        sguid = "IID_IDirect3DBaseTexture8";
        break;
    case 0xe4cdd575:
        sguid = "IID_IDirect3DTexture8";
        break;
    case 0x580ca87e:
        sguid = "IID_IDirect3DBaseTexture9";
        break;
    case 0x85c31227:
        sguid = "IID_IDirect3DTexture9";
        break;
    case 0xb96eebca:
        sguid = "IID_IDirect3DSurface8";
        break;
    case 0x0cfbaf3a:
        sguid = "IID_IDirect3DSurface9";
        break;
    case 0x1bab8e96:
        sguid = "IID_IDirectDrawSurfaceNew";
        break;
    // from D3DRM.H
    case 0x2bc49361:
        sguid = "IID_IDirect3DRM";
        break; // 0x2bc49361, 0x8327, 0x11cf, 0xac, 0x4a, 0x0, 0x0, 0xc0, 0x38, 0x25, 0xa1);
    case 0x4516ecc8:
        sguid = "IID_IDirect3DRM2";
        break; // 0x4516ecc8, 0x8f20, 0x11d0, 0x9b, 0x6d, 0x00, 0x00, 0xc0, 0x78, 0x1b, 0xc3);
    // DirectSound
    //DEFINE_GUID(CLSID_DirectSound,		0x47d4d946, 0x62e8, 0x11cf, 0x93, 0xbc, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0);
    //DEFINE_GUID(IID_IDirectSound,		0x279AFA83,0x4981,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);
    //DEFINE_GUID(IID_IDirectSoundBuffer,	0x279AFA85,0x4981,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);
    //DEFINE_GUID(IID_IDirectSoundNotify,	0xB0210783,0x89cd,0x11d0,0xAF,0x08,0x00,0xA0,0xC9,0x25,0xCD,0x16);
    //DEFINE_GUID(IID_IDirectSound3DListener,	0x279AFA84,0x4981,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);
    //DEFINE_GUID(IID_IDirectSound3DBuffer,	0x279AFA86,0x4981,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);
    //DEFINE_GUID(IID_IDirectSoundCapture,	0xB0210781,0x89CD,0x11D0,0xAF,0x08,0x00,0xA0,0xC9,0x25,0xCD,0x16);
    //DEFINE_GUID(IID_IDirectSoundCaptureBuffer,0xB0210782,0x89CD,0x11D0,0xAF,0x08,0x00,0xA0,0xC9,0x25,0xCD,0x16);
    //DEFINE_GUID(IID_IKsPropertySet,		0x31EFAC30,0x515C,0x11D0,0xA9,0xAA,0x00,0xAA,0x00,0x61,0xBE,0x93);
    case 0x279afa83:
        sguid = "IID_IDirectSound";
        break;
    case 0x47d4d946:
        sguid = "CLSID_DirectSound";
        break;
    case 0x279AFA85:
        sguid = "IID_IDirectSoundBuffer";
        break;
    case 0xB0210783:
        sguid = "IID_IDirectSoundNotify";
        break;
    case 0x279AFA84:
        sguid = "IID_IDirectSound3DListener";
        break;
    case 0x279AFA86:
        sguid = "IID_IDirectSound3DBuffer";
        break;
    case 0xB0210781:
        sguid = "IID_IDirectSoundCapture";
        break;
    case 0xB0210782:
        sguid = "IID_IDirectSoundCaptureBuffer";
        break;
    case 0x31EFAC30:
        sguid = "IID_IKsPropertySet";
        break;
    case 0x3901cc3f:
        sguid = "CLSID_DirectSound8";
        break;
    case 0xb0210780:
        sguid = "CLSID_DirectSoundCapture";
        break;
    case 0xe4bcac13:
        sguid = "CLSID_DirectSoundCapture8";
        break;
    case 0xfea4300c:
        sguid = "CLSID_DirectSoundFullDuplex";
        break;
    case 0xdef00000:
        sguid = "DSDEVID_DefaultPlayback";
        break;
    case 0xdef00001:
        sguid = "DSDEVID_DefaultCapture";
        break;
    case 0xdef00002:
        sguid = "DSDEVID_DefaultVoicePlayback";
        break;
    case 0xdef00003:
        sguid = "DSDEVID_DefaultVoiceCapture";
        break;
    // mixed common ones ...
    case 0xa95664d2:
        sguid = "IMMDeviceEnumerator";
        break;
    case 0x4fd2a833:
        sguid = "IDirectDrawFactory";
        break; //4fd2a833-86c8-11d0-8fca-00c04fd9189d
    case 0x4fd2a832:
        sguid = "DirectDrawEx_Object";
        break; //4fd2a832-86c8-11d0-8fca-00c04fd9189d
    case 0xA65B8071:
        sguid = "CLSID_DxDiagProvider";
        break;
    case 0xD8F1EEE0:
        sguid = "CLSID_A3d";
        break; // {D8F1EEE0-F634-11CF-8700-00A0245D918B}
    case 0xD8F1EEE1:
        sguid = "IID_IA3d";
        break; // {D8F1EEE1-F634-11CF-8700-00A0245D918B}
    case 0xFB80D1E0:
        sguid = "IID_IA3d2";
        break; // {FB80D1E0-98D3-11D1-90FB-006008A1F441}
    case 0xE4C40280:
        sguid = "IID_IA3d4";
        break; // {E4C40280-CCBA-11D2-9DCF-00500411582F}
    case 0x4315D437:
        sguid = "IID_IDeviceMoniker";
        break; // {4315D437-5B8C-11D0-BD3B-00A0C911CE86}
    case 0x31CE832E:
        sguid = "IVMRSurfaceAllocator";
        break; // {31CE832E-4484-458B-8CCA-F4D7E3DB0B52}
    // quartz.dll related
    case 0xA38CC06E:
        sguid = "quartz.dll";
        break; // {A38CC06E-5926-48DF-9926-571458145E80}
    case 0xe436ebb3:
        sguid = "CLSID_FilterGraph";
        break; // in quartz.dll
    // direct music
    case 0x6536115a:
        sguid = "IDirectMusic";
        break;
    case 0x2ffaaca2:
        sguid = "IID_IDirectMusicLoader";
        break; // {2ffaaca2-5dca-11d2-afa6-00aa0024d8b6}
    case 0x19e7c08c:
        sguid = "IID_IDirectMusicLoader8";
        break; // {19e7c08c-0a44-4e6a-a116-595a7cd5de8c}
    //C87631F5-23BE-4986-8836-05832FCC48F9 IDirectMusicAudioPath
    //9301E386-1F22-11D3-8226-D2FA76255D47 IDirectMusicContainer
    //2252373A-5814-489B-8209-31FEDEBAF137 IDirectMusicScript
    //51C22E10-B49F-46FC-BEC2-E6288FB9EDE6 IDirectMusicPatternTrack
    //FD24AD8A-A260-453D-BF50-6F9384F70985 IDirectMusicStyle8
    //A50E4730-0AE4-48A7-9839-BC04BFE07772 IDirectMusicSegmentState8
    //C6784488-41A3-418F-AA15-B35093BA42D4 IDirectMusicSegment8
    //679C4137-C62E-4147-B2B4-9D569ACB254C IDirectMusicPerformance8
    //19E7C08C-0A44-4E6A-A116-595A7CD5DE8C IDirectMusicLoader8
    //D38894D1-C052-11D2-872F-00600893B1BD IDirectMusicSegment2
    //6FC2CAE0-BC78-11D2-AFA6-00AA0024D8B6 IDirectMusicPerformance2
    //D2AC28C0-B39B-11D1-8704-00600893B1BD IDirectMusicBand
    //D2AC28BF-B39B-11D1-8704-00600893B1BD IDirectMusicComposer
    //D2AC28BE-B39B-11D1-8704-00600893B1BD IDirectMusicChordMap
    //D2AC28BD-B39B-11D1-8704-00600893B1BD IDirectMusicStyle
    //2BEFC277-5497-11D2-BCCB-00A0C922E6EB IDirectMusicGraph
    //07D43D03-6523-11D2-871D-00600893B1BD IDirectMusicPerformance
    //A3AFDCC7-D3EE-11D1-BC8D-00A0C922E6EB IDirectMusicSegmentState
    //F96029A2-4282-11D2-8717-00600893B1BD IDirectMusicSegment
    //D2AC28B5-B39B-11D1-8704-00600893B1BD IDirectMusicObject
    //68A04844-D13D-11D1-AFA6-00AA0024D8B6 IDirectMusicGetLoader
    //2FFAACA2-5DCA-11D2-AFA6-00AA0024D8B6 IDirectMusicLoader
    //0E674304-3B05-11D3-9BD1-F9E7F0A01536 IDirectMusicTrack8
    //0E674303-3B05-11D3-9BD1-F9E7F0A01536 IDirectMusicTool8
    //D2AC28BA-B39B-11D1-8704-00600893B1BD IDirectMusicTool
    //F96029A1-4282-11D2-8717-00600893B1BD IDirectMusicTrack
    //2D3629F7-813D-4939-8508-F05C6B75FD97 IDirectMusic8
    //6FC2CAE1-BC78-11D2-AFA6-00AA0024D8B6 IDirectMusic2
    //D2AC287E-B39B-11D1-8704-00600893B1BD IDirectMusicDownloadedInstrument
    //D2AC287D-B39B-11D1-8704-00600893B1BD IDirectMusicInstrument
    //D2AC287C-B39B-11D1-8704-00600893B1BD IDirectMusicCollection
    //D2AC287B-B39B-11D1-8704-00600893B1BD IDirectMusicDownload
    //D2AC287A-B39B-11D1-8704-00600893B1BD IDirectMusicPortDownload
    //CED153E7-3606-11D2-B9F9-0000F875AC12 IDirectMusicThru
    //08F2D8C9-37C2-11D2-B9F9-0000F875AC12 IDirectMusicPort
    //D2AC2878-B39B-11D1-8704-00600893B1BD IDirectMusicBuffer
    //6536115A-7B2D-11D2-BA18-0000F875AC12 IDirectMusic
    // DirectShow
    case 0x56A868A9:
        sguid = "IID_IGraphBuilder";
        break; // {56A868A9-0AD4-11CE-B03A-0020AF0BA770}
    case 0x56A868B1:
        sguid = "IID_IMediaControl";
        break;
    case 0x36B73880:
        sguid = "IID_IMediaSeeking";
        break;
    case 0x56A868B3:
        sguid = "IID_IBasicAudio";
        break;
    case 0x56A8689C:
        sguid = "IID_IMemAllocator";
        break; // {56A8689C-0AD4-11CE-B03A-0020AF0BA770}
    case 0x56A86895:
        sguid = "IID_IBasicFilter";
        break; // {56A86895-0AD4-11CE-B03A-0020AF0BA770}
    // DXGI
    //DEFINE_GUID(IID_IDXGIObject,0xaec22fb8,0x76f3,0x4639,0x9b,0xe0,0x28,0xeb,0x43,0xa6,0x7a,0x2e);
    //DEFINE_GUID(IID_IDXGIDeviceSubObject,0x3d3e0379,0xf9de,0x4d58,0xbb,0x6c,0x18,0xd6,0x29,0x92,0xf1,0xa6);
    //DEFINE_GUID(IID_IDXGIResource,0x035f3ab4,0x482e,0x4e50,0xb4,0x1f,0x8a,0x7f,0x8b,0xd8,0x96,0x0b);
    //DEFINE_GUID(IID_IDXGISurface,0xcafcb56c,0x6ac3,0x4889,0xbf,0x47,0x9e,0x23,0xbb,0xd2,0x60,0xec);
    //DEFINE_GUID(IID_IDXGIAdapter,0x2411e7e1,0x12ac,0x4ccf,0xbd,0x14,0x97,0x98,0xe8,0x53,0x4d,0xc0);
    //DEFINE_GUID(IID_IDXGIOutput,0xae02eedb,0xc735,0x4690,0x8d,0x52,0x5a,0x8d,0xc2,0x02,0x13,0xaa);
    //DEFINE_GUID(IID_IDXGISwapChain,0x310d36a0,0xd2e7,0x4c0a,0xaa,0x04,0x6a,0x9d,0x23,0xb8,0x88,0x6a);
    //DEFINE_GUID(IID_IDXGIFactory,0x7b7166ec,0x21c7,0x44ae,0xb2,0x1a,0xc9,0xae,0x32,0x1a,0xe3,0x69);
    //DEFINE_GUID(IID_IDXGIDevice,0x54ec77fa,0x1377,0x44e6,0x8c,0x32,0x88,0xfd,0x5f,0x44,0xc8,0x4c);
    //DEFINE_GUID(IID_IDXGIObject,0xaec22fb8,0x76f3,0x4639,0x9b,0xe0,0x28,0xeb,0x43,0xa6,0x7a,0x2e);
    //DEFINE_GUID(IID_IDXGIDeviceSubObject,0x3d3e0379,0xf9de,0x4d58,0xbb,0x6c,0x18,0xd6,0x29,0x92,0xf1,0xa6);
    //DEFINE_GUID(IID_IDXGIResource,0x035f3ab4,0x482e,0x4e50,0xb4,0x1f,0x8a,0x7f,0x8b,0xd8,0x96,0x0b);
    //DEFINE_GUID(IID_IDXGIKeyedMutex,0x9d8e1289,0xd7b3,0x465f,0x81,0x26,0x25,0x0e,0x34,0x9a,0xf8,0x5d);
    //DEFINE_GUID(IID_IDXGISurface,0xcafcb56c,0x6ac3,0x4889,0xbf,0x47,0x9e,0x23,0xbb,0xd2,0x60,0xec);
    //DEFINE_GUID(IID_IDXGISurface1,0x4AE63092,0x6327,0x4c1b,0x80,0xAE,0xBF,0xE1,0x2E,0xA3,0x2B,0x86);
    //DEFINE_GUID(IID_IDXGIAdapter,0x2411e7e1,0x12ac,0x4ccf,0xbd,0x14,0x97,0x98,0xe8,0x53,0x4d,0xc0);
    //DEFINE_GUID(IID_IDXGIOutput,0xae02eedb,0xc735,0x4690,0x8d,0x52,0x5a,0x8d,0xc2,0x02,0x13,0xaa);
    //DEFINE_GUID(IID_IDXGISwapChain,0x310d36a0,0xd2e7,0x4c0a,0xaa,0x04,0x6a,0x9d,0x23,0xb8,0x88,0x6a);
    //DEFINE_GUID(IID_IDXGIFactory,0x7b7166ec,0x21c7,0x44ae,0xb2,0x1a,0xc9,0xae,0x32,0x1a,0xe3,0x69);
    //DEFINE_GUID(IID_IDXGIDevice,0x54ec77fa,0x1377,0x44e6,0x8c,0x32,0x88,0xfd,0x5f,0x44,0xc8,0x4c);
    //DEFINE_GUID(IID_IDXGIFactory1,0x770aae78,0xf26f,0x4dba,0xa8,0x29,0x25,0x3c,0x83,0xd1,0xb3,0x87);
    //DEFINE_GUID(IID_IDXGIAdapter1,0x29038f61,0x3839,0x4626,0x91,0xfd,0x08,0x68,0x79,0x01,0x1a,0x05);
    //DEFINE_GUID(IID_IDXGIDevice1,0x77db970f,0x6276,0x48ba,0xba,0x28,0x07,0x01,0x43,0xb4,0x39,0x2c);
    //DEFINE_GUID(IID_IDXGIFactory2, 0x50c83a1c, 0xe072, 0x4c48, 0x87,0xb0, 0x36,0x30,0xfa,0x36,0xa6,0xd0);
    //IID_IDXGIDevice2: TGUID = '{05008617-fbfd-4051-a790-144884b4f6a9}';
    //IID_IDXGIAdapter2: TGUID = '{0AA1AE0A-FA0E-4B84-8644-E05FF8E5ACB5}';
    //DEFINE_GUID (IID_IDXGIOutput5, 0x80A07424, 0xAB52, 0x42EB, 0x83, 0x3C, 0x0C, 0x42, 0xFD, 0x28, 0x2D, 0x98)
    //DEFINE_GUID (IID_IDXGISwapChain4, 0x3D585D5A, 0xBD4A, 0x489E, 0xB1, 0xF4, 0x3D, 0xBC, 0xB6, 0x45, 0x2F, 0xFB)
    //DEFINE_GUID (IID_IDXGIDevice4, 0x95B4F95F, 0xD8DA, 0x4CA4, 0x9E, 0xE6, 0x3B, 0x76, 0xD5, 0x96, 0x8A, 0x10)
    //DEFINE_GUID (IID_IDXGIFactory5, 0x7632e1f5, 0xee65, 0x4dca, 0x87, 0xfd, 0x84, 0xcd, 0x75, 0xf8, 0x83, 0x8d)
    //IID_IDXGIDevice3: TGUID = '{6007896c-3244-4afd-bf18-a6d3beda5023}';
    //IID_IDXGISwapChain2: TGUID = '{a8be2ac4-199f-4946-b331-79599fb98de7}';
    //IID_IDXGIOutput2: TGUID = '{595e39d1-2724-4663-99b1-da969de28364}';
    //IID_IDXGIFactory3: TGUID = '{25483823-cd46-4c7d-86ca-47aa95b837bd}';
    //IID_IDXGIDecodeSwapChain: TGUID = '{2633066b-4514-4c7a-8fd8-12ea98059d18}';
    //IID_IDXGIFactoryMedia: TGUID = '{41e7d1f2-a591-4f7b-a2e5-fa9c843e1c12}';
    //IID_IDXGISwapChainMedia: TGUID = '{dd95b90b-f05f-4f6a-bd65-25bfb264bd84}';
    //IID_IDXGIOutput3: TGUID = '{8a6bb301-7e7e-41F4-a8e0-5b32f7f99b18}';
    //DEFINE_GUID(IID_IDXGIOutput2,0x595e39d1,0x2724,0x4663,0x99,0xb1,0xda,0x96,0x9d,0xe2,0x83,0x64);
    //DEFINE_GUID(IID_IDXGIDevice3,0x6007896c,0x3244,0x4afd,0xbf,0x18,0xa6,0xd3,0xbe,0xda,0x50,0x23);
    //DEFINE_GUID(IID_IDXGISwapChain2,0xa8be2ac4,0x199f,0x4946,0xb3,0x31,0x79,0x59,0x9f,0xb9,0x8d,0xe7);
    //DEFINE_GUID(IID_IDXGIFactory3,0x25483823,0xcd46,0x4c7d,0x86,0xca,0x47,0xaa,0x95,0xb8,0x37,0xbd);
    //DEFINE_GUID(IID_IDXGIDecodeSwapChain,0x2633066b,0x4514,0x4c7a,0x8f,0xd8,0x12,0xea,0x98,0x05,0x9d,0x18);
    //DEFINE_GUID(IID_IDXGIFactoryMedia,0x41e7d1f2,0xa591,0x4f7b,0xa2,0xe5,0xfa,0x9c,0x84,0x3e,0x1c,0x12);
    //DEFINE_GUID(IID_IDXGISwapChainMedia,0xdd95b90b,0xf05f,0x4f6a,0xbd,0x65,0x25,0xbf,0xb2,0x64,0xbd,0x84);
    //DEFINE_GUID(IID_IDXGIOutput3,0x8a6bb301,0x7e7e,0x41F4,0xa8,0xe0,0x5b,0x32,0xf7,0xf9,0x9b,0x18);
    //IID_IDXGISwapChain3: TGUID = '{94d99bdb-f1f8-4ab0-b236-7da0170edab1}';
    //IID_IDXGIOutput4: TGUID = '{dc7dca35-2196-414d-9F53-617884032a60}';
    //IID_IDXGIFactory4: TGUID = '{1bc6ea02-ef36-464f-bf0c-21ca39e5168a}';
    //IID_IDXGIAdapter3: TGUID = '{645967A4-1392-4310-A798-8053CE3E93FD}';
    //IID_IDXGIDisplayControl: TGUID = '{ea9dbf1a-c88e-4486-854a-98aa0138f30c}';
    //IID_IDXGIOutputDuplication: TGUID = '{191cfac3-a341-470d-b26e-a864f428319c}';
    //IID_IDXGISurface2: TGUID = '{aba496dd-b617-4cb8-a866-bc44d7eb1fa2}';
    //IID_IDXGIResource1: TGUID = '{30961379-4609-4a41-998e-54fe567ee0c1}';
    //IID_IDXGIDevice2: TGUID = '{05008617-fbfd-4051-a790-144884b4f6a9}';
    //IID_IDXGISwapChain1: TGUID = '{790a45f7-0d42-4876-983a-0a55cfe6f4aa}';
    //IID_IDXGIFactory2: TGUID = '{50c83a1c-e072-4c48-87b0-3630fa36a6d0}';
    //IID_IDXGIAdapter2: TGUID = '{0AA1AE0A-FA0E-4B84-8644-E05FF8E5ACB5}';
    //IID_IDXGIOutput1: TGUID = '{00cddea8-939b-4b83-a340-a685226666cc}';
    //DEFINE_GUID(IID_IDXGIAdapter4,0x3c8d99d1,0x4fbf,0x4181,0xa8,0x2c,0xaf,0x66,0xbf,0x7b,0xd2,0x4e);
    //DEFINE_GUID(IID_IDXGIOutput6,0x068346e8,0xaaec,0x4b84,0xad,0xd7,0x13,0x7f,0x51,0x3f,0x77,0xa1);
    case 0xaec22fb8:
        sguid = "IID_IDXGIObject";
        break;
    case 0x3d3e0379:
        sguid = "IID_IDXGIDeviceSubObject";
        break;
    case 0x191cfac3:
        sguid = "IID_IDXGIOutputDuplication";
        break;
    case 0xea9dbf1a:
        sguid = "IID_IDXGIDisplayControl";
        break;
    case 0x035f3ab4:
        sguid = "IID_IDXGIResource";
        break;
    case 0x30961379:
        sguid = "IID_IDXGIResource1";
        break;
    case 0xcafcb56c:
        sguid = "IID_IDXGISurface";
        break;
    case 0x4AE63092:
        sguid = "IID_IDXGISurface1";
        break;
    case 0xaba496dd:
        sguid = "IID_IDXGISurface2";
        break;
    case 0xae02eedb:
        sguid = "IID_IDXGIOutput";
        break;
    case 0x00cddea8:
        sguid = "IID_IDXGIOutput1";
        break;
    case 0x595e39d1:
        sguid = "IID_IDXGIOutput2";
        break;
    case 0x8a6bb301:
        sguid = "IID_IDXGIOutput3";
        break;
    case 0xdc7dca35:
        sguid = "IID_IDXGIOutput4";
        break;
    case 0x80A07424:
        sguid = "IID_IDXGIOutput5";
        break;
    case 0x068346e8:
        sguid = "IID_IDXGIOutput6";
        break;
    case 0x310d36a0:
        sguid = "IID_IDXGISwapChain";
        break;
    case 0x790a45f7:
        sguid = "IID_IDXGISwapChain1";
        break;
    case 0xa8be2ac4:
        sguid = "IID_IDXGISwapChain2";
        break;
    case 0x94d99bdb:
        sguid = "IID_IDXGISwapChain3";
        break;
    case 0x3D585D5A:
        sguid = "IID_IDXGISwapChain4";
        break;
    case 0x7b7166ec:
        sguid = "IID_IDXGIFactory";
        break;
    case 0x770aae78:
        sguid = "IID_IDXGIFactory1";
        break;
    case 0x50c83a1c:
        sguid = "IID_IDXGIFactory2";
        break;
    case 0x25483823:
        sguid = "IID_IDXGIFactory3";
        break;
    case 0x1bc6ea02:
        sguid = "IID_IDXGIFactory4";
        break;
    case 0x7632e1f5:
        sguid = "IID_IDXGIFactory5";
        break;
    case 0x2411e7e1:
        sguid = "IID_IDXGIAdapter";
        break;
    case 0x29038f61:
        sguid = "IID_IDXGIAdapter1";
        break;
    case 0x0AA1AE0A:
        sguid = "IID_IDXGIAdapter2";
        break;
    case 0x645967A4:
        sguid = "IID_IDXGIAdapter3";
        break;
    case 0x3c8d99d1:
        sguid = "IID_IDXGIAdapter4";
        break;
    case 0x54ec77fa:
        sguid = "IID_IDXGIDevice";
        break;
    case 0x77db970f:
        sguid = "IID_IDXGIDevice1";
        break;
    case 0x05008617:
        sguid = "IID_IDXGIDevice2";
        break;
    case 0x6007896c:
        sguid = "IID_IDXGIDevice3";
        break;
    case 0x95B4F95F:
        sguid = "IID_IDXGIDevice4";
        break;
    case 0x9d8e1289:
        sguid = "IID_IDXGIKeyedMutex";
        break;
    case 0x7abb6563:
        sguid = "IID_IDXGIAdapterInternal2";
        break; // from apitrace guids_entries.h
    case 0x2633066b:
        sguid = "IID_IDXGIDecodeSwapChain";
        break;
    case 0x41e7d1f2:
        sguid = "IID_IDXGIFactoryMedia";
        break;
    case 0xdd95b90b:
        sguid = "IID_IDXGISwapChainMedia";
        break;
    // D3D10
    // MIDL_INTERFACE("9B7E4C00-342C-4106-A19F-4F2704F689F0") ID3D10DeviceChild
    // MIDL_INTERFACE("2B4B1CC8-A4AD-41f8-8322-CA86FC3EC675") ID3D10DepthStencilState
    // MIDL_INTERFACE("EDAD8D19-8A35-4d6d-8566-2EA276CDE161") ID3D10BlendState
    // MIDL_INTERFACE("A2A07292-89AF-4345-BE2E-C53D9FBB6E9F") ID3D10RasterizerState
    // MIDL_INTERFACE("9B7E4C01-342C-4106-A19F-4F2704F689F0") ID3D10Resource
    // MIDL_INTERFACE("9B7E4C02-342C-4106-A19F-4F2704F689F0") ID3D10Buffer
    // MIDL_INTERFACE("9B7E4C03-342C-4106-A19F-4F2704F689F0") ID3D10Texture1D
    // MIDL_INTERFACE("9B7E4C04-342C-4106-A19F-4F2704F689F0") ID3D10Texture2D
    // MIDL_INTERFACE("9B7E4C05-342C-4106-A19F-4F2704F689F0") ID3D10Texture3D
    // MIDL_INTERFACE("C902B03F-60A7-49BA-9936-2A3AB37A7E33") ID3D10View
    // MIDL_INTERFACE("9B7E4C07-342C-4106-A19F-4F2704F689F0") ID3D10ShaderResourceView
    // MIDL_INTERFACE("9B7E4C08-342C-4106-A19F-4F2704F689F0") ID3D10RenderTargetView
    // MIDL_INTERFACE("9B7E4C09-342C-4106-A19F-4F2704F689F0") ID3D10DepthStencilView
    // MIDL_INTERFACE("9B7E4C0A-342C-4106-A19F-4F2704F689F0") ID3D10VertexShader
    // MIDL_INTERFACE("6316BE88-54CD-4040-AB44-20461BC81F68") ID3D10GeometryShader
    // MIDL_INTERFACE("4968B601-9D00-4cde-8346-8E7F675819B6") ID3D10PixelShader
    // MIDL_INTERFACE("9B7E4C0B-342C-4106-A19F-4F2704F689F0") ID3D10InputLayout
    // MIDL_INTERFACE("9B7E4C0C-342C-4106-A19F-4F2704F689F0") ID3D10SamplerState
    // MIDL_INTERFACE("9B7E4C0D-342C-4106-A19F-4F2704F689F0") ID3D10Asynchronous
    // MIDL_INTERFACE("9B7E4C0E-342C-4106-A19F-4F2704F689F0") ID3D10Query
    // MIDL_INTERFACE("9B7E4C10-342C-4106-A19F-4F2704F689F0") ID3D10Predicate
    // MIDL_INTERFACE("9B7E4C11-342C-4106-A19F-4F2704F689F0") ID3D10Counter
    // MIDL_INTERFACE("9B7E4C0F-342C-4106-A19F-4F2704F689F0") ID3D10Device
    // MIDL_INTERFACE("9B7E4E00-342C-4106-A19F-4F2704F689F0") ID3D10Multithread
    // MIDL_INTERFACE("9B7E4C8F-342C-4106-A19F-4F2704F689F0") ID3D10Device1
    // MIDL_INTERFACE("EDAD8D99-8A35-4d6d-8566-2EA276CDE161") ID3D10BlendState1
    // MIDL_INTERFACE("9B7E4C87-342C-4106-A19F-4F2704F689F0") ID3D10ShaderResourceView1
    case 0x9B7E4C00:
        sguid = "ID3D10DeviceChild";
        break;
    case 0x2B4B1CC8:
        sguid = "ID3D10DepthStencilState";
        break;
    case 0xEDAD8D19:
        sguid = "ID3D10BlendState";
        break;
    case 0xA2A07292:
        sguid = "ID3D10RasterizerState";
        break;
    case 0x9B7E4C01:
        sguid = "ID3D10Resource";
        break;
    case 0x9B7E4C02:
        sguid = "ID3D10Buffer";
        break;
    case 0x9B7E4C03:
        sguid = "ID3D10Texture1D";
        break;
    case 0x9B7E4C04:
        sguid = "ID3D10Texture2D";
        break;
    case 0x9B7E4C05:
        sguid = "ID3D10Texture3D";
        break;
    case 0xC902B03F:
        sguid = "ID3D10View";
        break;
    case 0x9B7E4C07:
        sguid = "ID3D10ShaderResourceView";
        break;
    case 0x9B7E4C08:
        sguid = "ID3D10RenderTargetView";
        break;
    case 0x9B7E4C09:
        sguid = "ID3D10DepthStencilView";
        break;
    case 0x9B7E4C0A:
        sguid = "ID3D10VertexShader";
        break;
    case 0x6316BE88:
        sguid = "ID3D10GeometryShader";
        break;
    case 0x4968B601:
        sguid = "ID3D10PixelShader";
        break;
    case 0x9B7E4C0B:
        sguid = "ID3D10InputLayout";
        break;
    case 0x9B7E4C0C:
        sguid = "ID3D10SamplerState";
        break;
    case 0x9B7E4C0D:
        sguid = "ID3D10Asynchronous";
        break;
    case 0x9B7E4C0E:
        sguid = "ID3D10Query";
        break;
    case 0x9B7E4C10:
        sguid = "ID3D10Predicate";
        break;
    case 0x9B7E4C11:
        sguid = "ID3D10Counter";
        break;
    case 0x9B7E4C0F:
        sguid = "ID3D10Device";
        break;
    case 0x9B7E4E00:
        sguid = "ID3D10Multithread";
        break;
    case 0x9B7E4C8F:
        sguid = "ID3D10Device1";
        break;
    case 0xEDAD8D99:
        sguid = "ID3D10BlendState1";
        break;
    case 0x9B7E4C87:
        sguid = "ID3D10ShaderResourceView1";
        break;
    // D3D11
    // 79CF2233-7536-4948-9D36-1E4692DC5760 ID3D11Debug (D3D11SDKLayers.h)
    // 77DB970F-6276-48BA-BA28-070143B4392C IID_IDXGIDevice1
    //"a04bfb29-08ef-43d6-a49c-a9bdbdcbe686" : ID3D11Device1 (d3d11_1.h)
    //"9d06dffa-d1e5-4d07-83a8-1bb123f2f841" : ID3D11Device2 (d3d11_2.h)
    //"189819f1-1db6-4b57-be54-1821339b85f7" : ID3D12Device ???
    //"54ec77fa-1377-44e6-8c32-88fd5f44c84c" : IDXGIDevice (dxgi.h)
    //F74EE86F-7270-48E8-9D63-38AF75F22D57
    case 0x79CF2233:
        sguid = "IID_ID3D11Debug";
        break;
    case 0xa04bfb29:
        sguid = "ID3D11Device1";
        break;
    case 0x9d06dffa:
        sguid = "ID3D11Device2";
        break;
    // D3D12
    // 189819F1-1DB6-4B57-BE54-1821339B85F7 IID_ID3D12Device
    case 0x189819f1:
        sguid = "ID3D12Device";
        break;
    // default: unknown
    default:
        sguid = "UNKNOWN";
        break;
    }
    return sguid;
}

char *ExplainNChitTest(DWORD c) {
    static char *eb;
    switch(c) {
    case HTERROR:
        eb = "HTERROR";
        break;
    case HTTRANSPARENT:
        eb = "HTTRANSPARENT";
        break;
    case HTNOWHERE:
        eb = "HTNOWHERE";
        break;
    case HTCLIENT:
        eb = "HTCLIENT";
        break;
    case HTCAPTION:
        eb = "HTCAPTION";
        break;
    case HTSYSMENU:
        eb = "HTSYSMENU";
        break;
    case HTGROWBOX:
        eb = "HTGROWBOX";
        break;
    case HTMENU:
        eb = "HTMENU";
        break;
    case HTHSCROLL:
        eb = "HTHSCROLL";
        break;
    case HTVSCROLL:
        eb = "HTVSCROLL";
        break;
    case HTMINBUTTON:
        eb = "HTMINBUTTON";
        break;
    case HTMAXBUTTON:
        eb = "HTMAXBUTTON";
        break;
    case HTLEFT:
        eb = "HTLEFT";
        break;
    case HTRIGHT:
        eb = "HTRIGHT";
        break;
    case HTTOP:
        eb = "HTTOP";
        break;
    case HTTOPLEFT:
        eb = "HTTOPLEFT";
        break;
    case HTTOPRIGHT:
        eb = "HTTOPRIGHT";
        break;
    case HTBOTTOM:
        eb = "HTBOTTOM";
        break;
    case HTBOTTOMLEFT:
        eb = "HTBOTTOMLEFT";
        break;
    case HTBOTTOMRIGHT:
        eb = "HTBOTTOMRIGHT";
        break;
    case HTBORDER:
        eb = "HTBORDER";
        break;
    case HTOBJECT:
        eb = "HTOBJECT";
        break;
    case HTCLOSE:
        eb = "HTCLOSE";
        break;
    case HTHELP:
        eb = "HTHELP";
        break;
    default:
        eb = "Unknown";
        break;
    }
    return eb;
}

char *ExplainDDEnumerateFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DDENUM_");
    if (c & DDENUM_ATTACHEDSECONDARYDEVICES) strcat(eb, "ATTACHEDSECONDARYDEVICES+");
    if (c & DDENUM_DETACHEDSECONDARYDEVICES) strcat(eb, "DETACHEDSECONDARYDEVICES+");
    if (c & DDENUM_NONDISPLAYDEVICES) strcat(eb, "NONDISPLAYDEVICES+");
    l = strlen(eb);
    if (l > strlen("DDENUM_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "NULL");
    return(eb);
}

char *ExplainsSystemMetrics(DWORD c) {
    char *Captions[94] = {
        "SM_CXSCREEN", "SM_CYSCREEN", "SM_CXVSCROLL", "SM_CYHSCROLL", "SM_CYCAPTION",
        "SM_CXBORDER", "SM_CYBORDER", "SM_CXDLGFRAME", "SM_CYDLGFRAME", "SM_CYVTHUMB",
        "SM_CXHTHUMB", "SM_CXICON", "SM_CYICON", "SM_CXCURSOR", "SM_CYCURSOR",
        "SM_CYMENU", "SM_CXFULLSCREEN", "SM_CYFULLSCREEN", "SM_CYKANJIWINDOW", "SM_MOUSEPRESENT",
        "SM_CYVSCROLL", "SM_CXHSCROLL", "SM_DEBUG", "SM_SWAPBUTTON", "SM_RESERVED1",
        "SM_RESERVED2", "SM_RESERVED3", "SM_RESERVED4", "SM_CXMIN", "SM_CYMIN",
        "SM_CXSIZE", "SM_CYSIZE", "SM_CXFRAME", "SM_CYFRAME", "SM_CXMINTRACK",
        "SM_CYMINTRACK", "SM_CXDOUBLECLK", "SM_CYDOUBLECLK", "SM_CXICONSPACING", "SM_CYICONSPACING",
        "SM_MENUDROPALIGNMENT", "SM_PENWINDOWS", "SM_DBCSENABLED", "SM_CMOUSEBUTTONS", "SM_SECURE",
        "SM_CXEDGE", "SM_CYEDGE", "SM_CXMINSPACING", "SM_CYMINSPACING", "SM_CXSMICON",
        "SM_CYSMICON", "SM_CYSMCAPTION", "SM_CXSMSIZE", "SM_CYSMSIZE", "SM_CXMENUSIZE",
        "SM_CYMENUSIZE", "SM_ARRANGE", "SM_CXMINIMIZED", "SM_CYMINIMIZED", "SM_CXMAXTRACK",
        "SM_CYMAXTRACK", "SM_CXMAXIMIZED", "SM_CYMAXIMIZED", "SM_NETWORK", "64???",
        "65???", "66???", "SM_CLEANBOOT", "SM_CXDRAG", "SM_CYDRAG",
        "SM_SHOWSOUNDS", "SM_CXMENUCHECK", "SM_CYMENUCHECK", "SM_SLOWMACHINE", "SM_MIDEASTENABLED",
        "SM_MOUSEWHEELPRESENT", "SM_XVIRTUALSCREEN", "SM_YVIRTUALSCREEN", "SM_CXVIRTUALSCREEN", "SM_CYVIRTUALSCREEN",
        "SM_CMONITORS", "SM_SAMEDISPLAYFORMAT", "SM_IMMENABLED", "SM_CXFOCUSBORDER", "SM_CYFOCUSBORDER",
        "85???", "SM_TABLETPC", "SM_MEDIACENTER", "SM_STARTER", "SM_SERVERR2",
        "SM_CMETRICS(Win 0x501)", "SM_MOUSEHORIZONTALWHEELPRESENT", "SM_CXPADDEDBORDER", "SM_CMETRICS"
    };
    if (c >= 0 && c < 94) return Captions[c];
    switch(c) {
    case SM_REMOTESESSION:
        return "SM_REMOTESESSION";
    case SM_SHUTTINGDOWN:
        return "SM_SHUTTINGDOWN";
    case SM_REMOTECONTROL:
        return "SM_REMOTECONTROL";
    case SM_CARETBLINKINGENABLED:
        return "SM_CARETBLINKINGENABLED";
    }
    return "???";
}

char *ExplainWPFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "SWP_");
    if (c & SWP_NOSIZE) strcat(eb, "NOSIZE+");
    if (c & SWP_NOMOVE) strcat(eb, "NOMOVE+");
    if (c & SWP_NOZORDER) strcat(eb, "NOZORDER+");
    if (c & SWP_NOREDRAW) strcat(eb, "NOREDRAW+");
    if (c & SWP_NOACTIVATE) strcat(eb, "NOACTIVATE+");
    if (c & SWP_FRAMECHANGED) strcat(eb, "FRAMECHANGED+");
    if (c & SWP_SHOWWINDOW) strcat(eb, "SHOWWINDOW+");
    if (c & SWP_HIDEWINDOW) strcat(eb, "HIDEWINDOW+");
    if (c & SWP_NOCOPYBITS) strcat(eb, "NOCOPYBITS+");
    if (c & SWP_NOOWNERZORDER) strcat(eb, "NOOWNERZORDER+");
    if (c & SWP_NOSENDCHANGING) strcat(eb, "NOSENDCHANGING+");
    l = strlen(eb);
    if (l > strlen("SWP_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "NULL");
    return(eb);
}

char *ExplainLoadLibFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "");
    if (c & DONT_RESOLVE_DLL_REFERENCES) strcat(eb, "DONT_RESOLVE_DLL_REFERENCES+");
    if (c & LOAD_LIBRARY_AS_DATAFILE) strcat(eb, "LOAD_LIBRARY_AS_DATAFILE+");
    if (c & LOAD_WITH_ALTERED_SEARCH_PATH) strcat(eb, "LOAD_WITH_ALTERED_SEARCH_PATH+");
    if (c & LOAD_IGNORE_CODE_AUTHZ_LEVEL) strcat(eb, "LOAD_IGNORE_CODE_AUTHZ_LEVEL+");
    if (c & LOAD_LIBRARY_AS_IMAGE_RESOURCE) strcat(eb, "LOAD_LIBRARY_AS_IMAGE_RESOURCE+");
    if (c & LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE) strcat(eb, "LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE+");
    l = strlen(eb);
    if (l > 0) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "NULL");
    return(eb);
}

char *ExplainDevModeFields(DWORD c) {
    static char eb[512];
    unsigned int l;
    strcpy(eb, "DM_");
    if (c & DM_ORIENTATION) strcat(eb, "ORIENTATION+");
    if (c & DM_PAPERSIZE) strcat(eb, "PAPERSIZE+");
    if (c & DM_PAPERLENGTH) strcat(eb, "PAPERLENGTH+");
    if (c & DM_PAPERWIDTH) strcat(eb, "PAPERWIDTH+");
    if (c & DM_SCALE) strcat(eb, "SCALE+");
    if (c & DM_COPIES) strcat(eb, "COPIES+");
    if (c & DM_DEFAULTSOURCE) strcat(eb, "DEFAULTSOURCE+");
    if (c & DM_PRINTQUALITY) strcat(eb, "PRINTQUALITY+");
    if (c & DM_POSITION) strcat(eb, "POSITION+");
    if (c & DM_DISPLAYORIENTATION) strcat(eb, "DISPLAYORIENTATION+");
    if (c & DM_DISPLAYFIXEDOUTPUT) strcat(eb, "DISPLAYFIXEDOUTPUT+");
    if (c & DM_COLOR) strcat(eb, "COLOR+");
    if (c & DM_DUPLEX) strcat(eb, "DUPLEX+");
    if (c & DM_YRESOLUTION) strcat(eb, "YRESOLUTION+");
    if (c & DM_TTOPTION) strcat(eb, "TTOPTION+");
    if (c & DM_COLLATE) strcat(eb, "COLLATE+");
    if (c & DM_FORMNAME) strcat(eb, "FORMNAME+");
    if (c & DM_LOGPIXELS) strcat(eb, "LOGPIXELS+");
    if (c & DM_BITSPERPEL) strcat(eb, "BITSPERPEL+");
    if (c & DM_PELSWIDTH) strcat(eb, "PELSWIDTH+");
    if (c & DM_PELSHEIGHT) strcat(eb, "PELSHEIGHT+");
    if (c & DM_DISPLAYFLAGS) strcat(eb, "DISPLAYFLAGS+");
    if (c & DM_NUP) strcat(eb, "NUP+");
    if (c & DM_DISPLAYFREQUENCY) strcat(eb, "DISPLAYFREQUENCY+");
    if (c & DM_ICMMETHOD) strcat(eb, "ICMMETHOD+");
    if (c & DM_ICMINTENT) strcat(eb, "ICMINTENT+");
    if (c & DM_MEDIATYPE) strcat(eb, "MEDIATYPE+");
    if (c & DM_DITHERTYPE) strcat(eb, "DITHERTYPE+");
    if (c & DM_PANNINGWIDTH) strcat(eb, "PANNINGWIDTH+");
    if (c & DM_PANNINGHEIGHT) strcat(eb, "PANNINGHEIGHT+");
    l = strlen(eb);
    if (l > strlen("DM_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "NULL");
    return(eb);
}

char *ExplainMCICommands(DWORD c) {
    switch(c) {
    case MCI_OPEN:
        return "MCI_OPEN";
    case MCI_CLOSE:
        return "MCI_CLOSE";
    case MCI_ESCAPE:
        return "MCI_ESCAPE";
    case MCI_PLAY:
        return "MCI_PLAY";
    case MCI_SEEK:
        return "MCI_SEEK";
    case MCI_STOP:
        return "MCI_STOP";
    case MCI_PAUSE:
        return "MCI_PAUSE";
    case MCI_INFO:
        return "MCI_INFO";
    case MCI_GETDEVCAPS:
        return "MCI_GETDEVCAPS";
    case MCI_SPIN:
        return "MCI_SPIN";
    case MCI_SET:
        return "MCI_SET";
    case MCI_STEP:
        return "MCI_STEP";
    case MCI_RECORD:
        return "MCI_RECORD";
    case MCI_SYSINFO:
        return "MCI_SYSINFO";
    case MCI_BREAK:
        return "MCI_BREAK";
    case MCI_SAVE:
        return "MCI_SAVE";
    case MCI_STATUS:
        return "MCI_STATUS";
    case MCI_CUE:
        return "MCI_CUE";
    case MCI_REALIZE:
        return "MCI_REALIZE";
    case MCI_WINDOW:
        return "MCI_WINDOW";
    case MCI_PUT:
        return "MCI_PUT";
    case MCI_WHERE:
        return "MCI_WHERE";
    case MCI_FREEZE:
        return "MCI_FREEZE";
    case MCI_UNFREEZE:
        return "MCI_UNFREEZE";
    case MCI_LOAD:
        return "MCI_LOAD";
    case MCI_CUT:
        return "MCI_CUT";
    case MCI_COPY:
        return "MCI_COPY";
    case MCI_PASTE:
        return "MCI_PASTE";
    case MCI_UPDATE:
        return "MCI_UPDATE";
    case MCI_RESUME:
        return "MCI_RESUME";
    case MCI_DELETE:
        return "MCI_DELETE";
    }
    return "???";
}

char *ExplainMCIFlags(DWORD cmd, DWORD c) {
    static char eb[512];
    unsigned int l;
    strcpy(eb, "MCI_");
    // common flags
    if (c & MCI_NOTIFY) strcat(eb, "NOTIFY+");
    if (c & MCI_WAIT) strcat(eb, "WAIT+");
    if (c & MCI_FROM) strcat(eb, "FROM+");
    if (c & MCI_TO) strcat(eb, "TO+");
    if (c & MCI_TRACK) strcat(eb, "TRACK+");
    switch(cmd) {
    case MCI_OPEN:
        if (c & MCI_OPEN_SHAREABLE) strcat(eb, "OPEN_SHAREABLE+");
        if (c & MCI_OPEN_ELEMENT) strcat(eb, "OPEN_ELEMENT+");
        if (c & MCI_OPEN_ALIAS) strcat(eb, "OPEN_ALIAS+");
        if (c & MCI_OPEN_ELEMENT_ID) strcat(eb, "OPEN_ELEMENT_ID+");
        if (c & MCI_OPEN_TYPE_ID) strcat(eb, "OPEN_TYPE_ID+");
        if (c & MCI_OPEN_TYPE) strcat(eb, "OPEN_TYPE+");
        if (c & MCI_WAVE_OPEN_BUFFER) strcat(eb, "WAVE_OPEN_BUFFER+");
        break;
    case MCI_SEEK:
        if (c & MCI_SEEK_TO_START) strcat(eb, "SEEK_TO_START+");
        if (c & MCI_STATUS_START) strcat(eb, "STATUS_START+");
        if (c & MCI_VD_SEEK_REVERSE) strcat(eb, "VD_SEEK_REVERSE+");
        break;
    case MCI_STATUS:
        if (c & MCI_STATUS_ITEM) strcat(eb, "STATUS_ITEM+");
        if (c & MCI_STATUS_START) strcat(eb, "STATUS_START+");
        break;
    case MCI_INFO:
        if (c & MCI_INFO_PRODUCT) strcat(eb, "INFO_PRODUCT+");
        if (c & MCI_INFO_FILE) strcat(eb, "INFO_FILE+");
        if (c & MCI_INFO_MEDIA_UPC) strcat(eb, "INFO_MEDIA_UPC+");
        if (c & MCI_INFO_MEDIA_IDENTITY) strcat(eb, "INFO_MEDIA_IDENTITY+");
        if (c & MCI_INFO_NAME) strcat(eb, "INFO_NAME+");
        if (c & MCI_INFO_COPYRIGHT) strcat(eb, "INFO_COPYRIGHT+");
        break;
    case MCI_GETDEVCAPS:
        if (c & MCI_VD_GETDEVCAPS_CLV) strcat(eb, "VD_GETDEVCAPS_CLV+");
        if (c & MCI_VD_GETDEVCAPS_CAV) strcat(eb, "VD_GETDEVCAPS_CAV+");
        if (c & MCI_GETDEVCAPS_ITEM) strcat(eb, "GETDEVCAPS_ITEM+");
        break;
    case MCI_SYSINFO:
        if (c & MCI_SYSINFO_QUANTITY) strcat(eb, "SYSINFO_QUANTITY+");
        if (c & MCI_SYSINFO_OPEN) strcat(eb, "SYSINFO_OPEN+");
        if (c & MCI_SYSINFO_NAME) strcat(eb, "SYSINFO_NAME+");
        if (c & MCI_SYSINFO_INSTALLNAME) strcat(eb, "SYSINFO_INSTALLNAME+");
        break;
    case MCI_SET:
        if (c & MCI_SET_DOOR_OPEN) strcat(eb, "SET_DOOR_OPEN+");
        if (c & MCI_SET_DOOR_CLOSED) strcat(eb, "SET_DOOR_CLOSED+");
        if (c & MCI_SET_TIME_FORMAT) strcat(eb, "SET_TIME_FORMAT+");
        if (c & MCI_SET_AUDIO) strcat(eb, "SET_AUDIO+");
        if (c & MCI_SET_VIDEO) strcat(eb, "SET_VIDEO+");
        if (c & MCI_SET_ON) strcat(eb, "SET_ON+");
        if (c & MCI_SET_OFF) strcat(eb, "SET_OFF+");
        if (c & MCI_SEQ_SET_TEMPO) strcat(eb, "SEQ_SET_TEMPO+");
        if (c & MCI_SEQ_SET_PORT) strcat(eb, "SEQ_SET_PORT+");
        if (c & MCI_SEQ_SET_SLAVE) strcat(eb, "SEQ_SET_SLAVE+");
        if (c & MCI_SEQ_SET_MASTER) strcat(eb, "SEQ_SET_MASTER+");
        if (c & MCI_SEQ_SET_OFFSET) strcat(eb, "SEQ_SET_OFFSET+");
        break;
    case MCI_BREAK:
        if (c & MCI_BREAK_KEY) strcat(eb, "BREAK_KEY+");
        if (c & MCI_BREAK_HWND) strcat(eb, "BREAK_HWND+");
        if (c & MCI_BREAK_OFF) strcat(eb, "BREAK_OFF+");
        break;
    case MCI_RECORD:
        if (c & MCI_RECORD_INSERT) strcat(eb, "RECORD_INSERT+");
        if (c & MCI_RECORD_OVERWRITE) strcat(eb, "RECORD_OVERWRITE+");
        break;
    case MCI_SAVE:
        if (c & MCI_SAVE_FILE) strcat(eb, "SAVE_FILE+");
        break;
    case MCI_LOAD:
        if (c & MCI_LOAD_FILE) strcat(eb, "SAVE_FILE+");
        break;
    case MCI_PLAY:
        if (c & MCI_VD_PLAY_REVERSE) strcat(eb, "VD_PLAY_REVERSE+");
        if (c & MCI_VD_PLAY_FAST) strcat(eb, "VD_PLAY_FAST+");
        if (c & MCI_VD_PLAY_SPEED) strcat(eb, "VD_PLAY_SPEED+");
        if (c & MCI_VD_PLAY_SCAN) strcat(eb, "VD_PLAY_SCAN+");
        if (c & MCI_VD_PLAY_SLOW) strcat(eb, "VD_PLAY_SLOW+");
        if (c & MCI_MCIAVI_PLAY_WINDOW) strcat(eb, "MCIAVI_PLAY_WINDOW+");
        if (c & MCI_MCIAVI_PLAY_FULLSCREEN) strcat(eb, "MCIAVI_PLAY_FULLSCREEN+");
        if (c & MCI_MCIAVI_PLAY_FULLBY2) strcat(eb, "MCIAVI_PLAY_FULLBY2+");
        break;
    case MCI_STEP:
        if (c & MCI_VD_STEP_FRAMES) strcat(eb, "VD_STEP_FRAMES+");
        if (c & MCI_VD_STEP_REVERSE) strcat(eb, "VD_STEP_REVERSE+");
        break;
    case MCI_WINDOW:
        if (c & MCI_ANIM_WINDOW_HWND) strcat(eb, "ANIM_WINDOW_HWND+");
        if (c & MCI_ANIM_WINDOW_STATE) strcat(eb, "ANIM_WINDOW_STATE+");
        if (c & MCI_ANIM_WINDOW_TEXT) strcat(eb, "ANIM_WINDOW_TEXT+");
        if (c & MCI_ANIM_WINDOW_ENABLE_STRETCH) strcat(eb, "ANIM_WINDOW_ENABLE_STRETCH+");
        if (c & MCI_ANIM_WINDOW_DISABLE_STRETCH) strcat(eb, "ANIM_WINDOW_DISABLE_STRETCH+");
        break;
    case MCI_PUT:
        if (c & MCI_ANIM_RECT) strcat(eb, "ANIM_RECT+");
        if (c & MCI_ANIM_PUT_SOURCE) strcat(eb, "ANIM_PUT_SOURCE+");
        if (c & MCI_ANIM_PUT_DESTINATION) strcat(eb, "ANIM_PUT_DESTINATION+");
        break;
    case MCI_WHERE:
        if (c & MCI_ANIM_WHERE_SOURCE) strcat(eb, "ANIM_WHERE_SOURCE+");
        if (c & MCI_ANIM_WHERE_DESTINATION) strcat(eb, "ANIM_WHERE_DESTINATION+");
        break;
    case MCI_UPDATE:
        if (c & MCI_ANIM_UPDATE_HDC) strcat(eb, "ANIM_UPDATE_HDC+");
        break;
    }
    /*
        if (c & MCI_OVLY_OPEN_WS) strcat(eb, "OVLY_OPEN_WS+");
        if (c & MCI_OVLY_OPEN_PARENT) strcat(eb, "OVLY_OPEN_PARENT+");
        if (c & MCI_OVLY_STATUS_HWND) strcat(eb, "OVLY_STATUS_HWND+");
        if (c & MCI_OVLY_STATUS_STRETCH) strcat(eb, "OVLY_STATUS_STRETCH+");
        if (c & MCI_OVLY_INFO_TEXT) strcat(eb, "OVLY_INFO_TEXT+");
        if (c & MCI_OVLY_WINDOW_HWND) strcat(eb, "OVLY_WINDOW_HWND+");
        if (c & MCI_OVLY_WINDOW_STATE) strcat(eb, "OVLY_WINDOW_STATE+");
        if (c & MCI_OVLY_WINDOW_TEXT) strcat(eb, "OVLY_WINDOW_TEXT+");
        if (c & MCI_OVLY_WINDOW_ENABLE_STRETCH) strcat(eb, "OVLY_WINDOW_ENABLE_STRETCH+");
        if (c & MCI_OVLY_WINDOW_DISABLE_STRETCH) strcat(eb, "OVLY_WINDOW_DISABLE_STRETCH+");
        if (c & MCI_OVLY_WINDOW_DEFAULT) strcat(eb, "OVLY_WINDOW_DEFAULT+");
        if (c & MCI_OVLY_RECT) strcat(eb, "OVLY_RECT+");
        if (c & MCI_OVLY_PUT_SOURCE) strcat(eb, "OVLY_PUT_SOURCE+");
        if (c & MCI_OVLY_PUT_DESTINATION) strcat(eb, "OVLY_PUT_DESTINATION+");
        if (c & MCI_OVLY_PUT_FRAME) strcat(eb, "OVLY_PUT_FRAME+");
        if (c & MCI_OVLY_PUT_VIDEO) strcat(eb, "OVLY_PUT_VIDEO+");
        if (c & MCI_OVLY_WHERE_SOURCE) strcat(eb, "OVLY_WHERE_SOURCE+");
        if (c & MCI_OVLY_WHERE_DESTINATION) strcat(eb, "OVLY_WHERE_DESTINATION+");
        if (c & MCI_OVLY_WHERE_FRAME) strcat(eb, "OVLY_WHERE_FRAME+");
        if (c & MCI_OVLY_WHERE_VIDEO) strcat(eb, "OVLY_WHERE_VIDEO+");
    */
    l = strlen(eb);
    if (l > strlen("MCI_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "NULL");
    return(eb);
}

char *ExplainRegType(DWORD c) {
    char *Captions[12] = {
        "REG_NONE", "REG_SZ", "REG_EXPAND_SZ", "REG_BINARY",
        "REG_DWORD", "REG_DWORD_BIG_ENDIAN", "REG_LINK", "REG_MULTI_SZ",
        "REG_RESOURCE_LIST", "REG_FULL_RESOURCE_DESCRIPTOR",
        "REG_RESOURCE_REQUIREMENTS_LIST", "REG_QWORD"
    };
    if (c >= 0 && c < 12) return Captions[c];
    return "???";
}

char *ExplainDCType(DWORD c) {
    char *Captions[GDI_OBJ_LAST + 1] = {
        "NULL", "OBJ_PEN", "OBJ_BRUSH", "OBJ_DC",
        "OBJ_METADC", "OBJ_PAL", "OBJ_FONT", "OBJ_BITMAP",
        "OBJ_REGION", "OBJ_METAFILE", "OBJ_MEMDC", "OBJ_EXTPEN",
        "OBJ_ENHMETADC", "OBJ_ENHMETAFILE", "OBJ_COLORSPACE"
    };
    if (c >= 0 && c <= GDI_OBJ_LAST) return Captions[c];
    return "???";
}

char *ExplainPeekRemoveMsg(DWORD c) {
    static char eb[128];
    strcpy(eb, (c & PM_REMOVE) ? "PM_REMOVE" : "PM_NOREMOVE");
    if(c & PM_NOYIELD) strcat(eb, "+NOYIELD");
    c >>= 16;
    if(c & QS_MOUSEMOVE) strcat(eb, "+MOUSEMOVE");
    if(c & QS_MOUSEBUTTON) strcat(eb, "+MOUSEBUTTON");
    if(c & QS_KEY) strcat(eb, "+KEY");
    if(c & QS_RAWINPUT) strcat(eb, "+RAWINPUT");
    if(c & QS_PAINT) strcat(eb, "+PAINT");
    if(c & QS_POSTMESSAGE) strcat(eb, "+POSTMESSAGE");
    if(c & QS_HOTKEY) strcat(eb, "+HOTKEY");
    if(c & QS_TIMER) strcat(eb, "+TIMER");
    if(c & QS_SENDMESSAGE) strcat(eb, "+SENDMESSAGE");
    if(c & QS_ALLPOSTMESSAGE) strcat(eb, "+ALLPOSTMESSAGE");
    return(eb);
}

char *ExplainGetDCExFlags(DWORD c) {
    static char eb[128];
    unsigned int l;
    strcpy(eb, "DCX_");
    if(c & DCX_WINDOW) strcat(eb, "WINDOW+");
    if(c & DCX_CACHE) strcat(eb, "CACHE+");
    if(c & DCX_PARENTCLIP) strcat(eb, "PARENTCLIP+");
    if(c & DCX_CLIPSIBLINGS) strcat(eb, "CLIPSIBLINGS+");
    if(c & DCX_CLIPCHILDREN) strcat(eb, "CLIPCHILDREN+");
    if(c & DCX_NORESETATTRS) strcat(eb, "NORESETATTRS+");
    if(c & DCX_EXCLUDERGN) strcat(eb, "EXCLUDERGN+");
    if(c & DCX_EXCLUDEUPDATE) strcat(eb, "EXCLUDEUPDATE+");
    if(c & DCX_INTERSECTRGN) strcat(eb, "INTERSECTRGN+");
    if(c & DCX_INTERSECTUPDATE) strcat(eb, "INTERSECTUPDATE+");
    if(c & DCX_VALIDATE) strcat(eb, "VALIDATE+");
    l = strlen(eb);
    if (l > strlen("DCX_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "NULL");
    return(eb);
}

char *ExplainPaletteUse(UINT uUsage) {
    char *eb = "SYSPAL_ERROR";
    switch(uUsage) {
    case SYSPAL_STATIC:
        eb = "SYSPAL_STATIC";
        break;
    case SYSPAL_NOSTATIC:
        eb = "SYSPAL_NOSTATIC";
        break;
    case SYSPAL_NOSTATIC256:
        eb = "SYSPAL_NOSTATIC256";
        break;
    }
    return eb;
}

char *ExplainRasterCaps(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "RC_");
    if(c & RC_BITBLT) strcat(eb, "BITBLT+");
    if(c & RC_BANDING) strcat(eb, "BANDING+");
    if(c & RC_SCALING) strcat(eb, "SCALING+");
    if(c & RC_BITMAP64) strcat(eb, "BITMAP64+");
    if(c & RC_GDI20_OUTPUT) strcat(eb, "GDI20_OUTPUT+");
    if(c & RC_GDI20_STATE) strcat(eb, "GDI20_STATE+");
    if(c & RC_SAVEBITMAP) strcat(eb, "SAVEBITMAP+");
    if(c & RC_DI_BITMAP) strcat(eb, "DI_BITMAP+");
    if(c & RC_PALETTE) strcat(eb, "PALETTE+");
    if(c & RC_DIBTODEV) strcat(eb, "DIBTODEV+");
    if(c & RC_BIGFONT) strcat(eb, "BIGFONT+");
    if(c & RC_STRETCHBLT) strcat(eb, "STRETCHBLT+");
    if(c & RC_FLOODFILL) strcat(eb, "FLOODFILL+");
    if(c & RC_STRETCHDIB) strcat(eb, "STRETCHDIB+");
    if(c & RC_OP_DX_OUTPUT) strcat(eb, "OP_DX_OUTPUT+");
    if(c & RC_DEVBITS) strcat(eb, "DEVBITS+");
    l = strlen(eb);
    if (l > strlen("RC_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "NULL");
    return(eb);
}

char *ExplainD3DRenderState(DWORD c) {
    char *p;
    if((c >= D3DRENDERSTATE_STIPPLEPATTERN00) && (c <= D3DRENDERSTATE_STIPPLEPATTERN31)) p = "STIPPLEPATTERNnn";
    else if((c >= D3DRENDERSTATE_WRAP0) && (c <= D3DRENDERSTATE_WRAP7)) p = "WRAPn";
    else
        switch(c) {
        case D3DRENDERSTATE_ANTIALIAS:
            p = "ANTIALIAS";
            break;
        case D3DRENDERSTATE_TEXTUREPERSPECTIVE:
            p = "TEXTUREPERSPECTIVE";
            break;
        case D3DRENDERSTATE_ZENABLE:
            p = "ZENABLE";
            break;
        case D3DRENDERSTATE_FILLMODE:
            p = "FILLMODE";
            break;
        case D3DRENDERSTATE_SHADEMODE:
            p = "SHADEMODE";
            break;
        case D3DRENDERSTATE_LINEPATTERN:
            p = "LINEPATTERN";
            break;
        case D3DRENDERSTATE_ZWRITEENABLE:
            p = "ZWRITEENABLE";
            break;
        case D3DRENDERSTATE_ALPHATESTENABLE:
            p = "ALPHATESTENABLE";
            break;
        case D3DRENDERSTATE_LASTPIXEL:
            p = "LASTPIXEL";
            break;
        case D3DRENDERSTATE_SRCBLEND:
            p = "SRCBLEND";
            break;
        case D3DRENDERSTATE_DESTBLEND:
            p = "DESTBLEND";
            break;
        case D3DRENDERSTATE_CULLMODE:
            p = "CULLMODE";
            break;
        case D3DRENDERSTATE_ZFUNC:
            p = "ZFUNC";
            break;
        case D3DRENDERSTATE_ALPHAREF:
            p = "ALPHAREF";
            break;
        case D3DRENDERSTATE_ALPHAFUNC:
            p = "ALPHAFUNC";
            break;
        case D3DRENDERSTATE_DITHERENABLE:
            p = "DITHERENABLE";
            break;
        case D3DRENDERSTATE_ALPHABLENDENABLE:
            p = "ALPHABLENDENABLE";
            break;
        case D3DRENDERSTATE_FOGENABLE:
            p = "FOGENABLE";
            break;
        case D3DRENDERSTATE_SPECULARENABLE:
            p = "SPECULARENABLE";
            break;
        case D3DRENDERSTATE_ZVISIBLE:
            p = "ZVISIBLE";
            break;
        case D3DRENDERSTATE_STIPPLEDALPHA:
            p = "STIPPLEDALPHA";
            break;
        case D3DRENDERSTATE_FOGCOLOR:
            p = "FOGCOLOR";
            break;
        case D3DRENDERSTATE_FOGTABLEMODE:
            p = "FOGTABLEMODE";
            break;
        case D3DRENDERSTATE_FOGSTART:
            p = "FOGSTART";
            break;
        case D3DRENDERSTATE_FOGEND:
            p = "FOGEND";
            break;
        case D3DRENDERSTATE_FOGDENSITY:
            p = "FOGDENSITY";
            break;
        case D3DRENDERSTATE_EDGEANTIALIAS:
            p = "EDGEANTIALIAS";
            break;
        case D3DRENDERSTATE_COLORKEYENABLE:
            p = "COLORKEYENABLE";
            break;
        case D3DRENDERSTATE_STENCILENABLE:
            p = "STENCILENABLE";
            break;
        case D3DRENDERSTATE_ZBIAS:
            p = "ZBIAS";
            break;
        case D3DRENDERSTATE_RANGEFOGENABLE:
            p = "RANGEFOGENABLE";
            break;
        case D3DRENDERSTATE_STENCILFAIL:
            p = "STENCILFAIL";
            break;
        case D3DRENDERSTATE_STENCILZFAIL:
            p = "STENCILZFAIL";
            break;
        case D3DRENDERSTATE_STENCILPASS:
            p = "STENCILPASS";
            break;
        case D3DRENDERSTATE_STENCILFUNC:
            p = "STENCILFUNC";
            break;
        case D3DRENDERSTATE_STENCILREF:
            p = "STENCILREF";
            break;
        case D3DRENDERSTATE_STENCILMASK:
            p = "STENCILMASK";
            break;
        case D3DRENDERSTATE_STENCILWRITEMASK:
            p = "STENCILWRITEMASK";
            break;
        case D3DRENDERSTATE_TEXTUREFACTOR:
            p = "TEXTUREFACTOR";
            break;
        case D3DRENDERSTATE_CLIPPING:
            p = "CLIPPING";
            break;
        case D3DRENDERSTATE_LIGHTING:
            p = "LIGHTING";
            break;
        case D3DRENDERSTATE_EXTENTS:
            p = "EXTENTS";
            break;
        case D3DRENDERSTATE_AMBIENT:
            p = "AMBIENT";
            break;
        case D3DRENDERSTATE_FOGVERTEXMODE:
            p = "FOGVERTEXMODE";
            break;
        case D3DRENDERSTATE_COLORVERTEX:
            p = "COLORVERTEX";
            break;
        case D3DRENDERSTATE_LOCALVIEWER:
            p = "LOCALVIEWER";
            break;
        case D3DRENDERSTATE_NORMALIZENORMALS:
            p = "NORMALIZENORMALS";
            break;
        case D3DRENDERSTATE_COLORKEYBLENDENABLE:
            p = "COLORKEYBLENDENABLE";
            break;
        case D3DRENDERSTATE_DIFFUSEMATERIALSOURCE:
            p = "DIFFUSEMATERIALSOURCE";
            break;
        case D3DRENDERSTATE_SPECULARMATERIALSOURCE:
            p = "SPECULARMATERIALSOURCE";
            break;
        case D3DRENDERSTATE_AMBIENTMATERIALSOURCE:
            p = "AMBIENTMATERIALSOURCE";
            break;
        case D3DRENDERSTATE_EMISSIVEMATERIALSOURCE:
            p = "EMISSIVEMATERIALSOURCE";
            break;
        case D3DRENDERSTATE_VERTEXBLEND:
            p = "VERTEXBLEND";
            break;
        case D3DRENDERSTATE_CLIPPLANEENABLE:
            p = "CLIPPLANEENABLE";
            break;
        // unsupported legacy
        case D3DRENDERSTATE_TEXTUREHANDLE:
            p = "TEXTUREHANDLE";
            break;
        case D3DRENDERSTATE_TEXTUREADDRESS:
            p = "TEXTUREADDRESS";
            break;
        case D3DRENDERSTATE_WRAPU:
            p = "WRAPU";
            break;
        case D3DRENDERSTATE_WRAPV:
            p = "WRAPV";
            break;
        case D3DRENDERSTATE_MONOENABLE:
            p = "MONOENABLE";
            break;
        case D3DRENDERSTATE_ROP2:
            p = "ROP2";
            break;
        case D3DRENDERSTATE_PLANEMASK:
            p = "PLANEMASK";
            break;
        case D3DRENDERSTATE_TEXTUREMAG:
            p = "TEXTUREMAG";
            break;
        case D3DRENDERSTATE_TEXTUREMIN:
            p = "TEXTUREMIN";
            break;
        case D3DRENDERSTATE_TEXTUREMAPBLEND:
            p = "TEXTUREMAPBLEND";
            break;
        case D3DRENDERSTATE_SUBPIXEL:
            p = "SUBPIXEL";
            break;
        case D3DRENDERSTATE_SUBPIXELX:
            p = "SUBPIXELX";
            break;
        case D3DRENDERSTATE_STIPPLEENABLE:
            p = "STIPPLEENABLE";
            break;
        case D3DRENDERSTATE_BORDERCOLOR:
            p = "BORDERCOLOR";
            break;
        case D3DRENDERSTATE_TEXTUREADDRESSU:
            p = "TEXTUREADDRESSU";
            break;
        case D3DRENDERSTATE_TEXTUREADDRESSV:
            p = "TEXTUREADDRESSV";
            break;
        case D3DRENDERSTATE_MIPMAPLODBIAS:
            p = "MIPMAPLODBIAS";
            break;
        case D3DRENDERSTATE_ANISOTROPY:
            p = "ANISOTROPY";
            break;
        case D3DRENDERSTATE_FLUSHBATCH:
            p = "FLUSHBATCH";
            break;
        case D3DRENDERSTATE_FORCE_DWORD:
            p = "FORCE_DWORD";
            break;
        default:
            p = "???";
            break;
        }
    return p;
}

char *ExplainRenderstateValue(DWORD Value) {
    char *p;
    switch(Value) {
    case D3DCMP_NEVER               :
        p = "D3DCMP_NEVER";
        break;
    case D3DCMP_LESS                :
        p = "D3DCMP_LESS";
        break;
    case D3DCMP_EQUAL               :
        p = "D3DCMP_EQUAL";
        break;
    case D3DCMP_LESSEQUAL           :
        p = "D3DCMP_LESSEQUAL";
        break;
    case D3DCMP_GREATER             :
        p = "D3DCMP_GREATER";
        break;
    case D3DCMP_NOTEQUAL            :
        p = "D3DCMP_NOTEQUAL";
        break;
    case D3DCMP_GREATEREQUAL        :
        p = "D3DCMP_GREATEREQUAL";
        break;
    case D3DCMP_ALWAYS              :
        p = "D3DCMP_ALWAYS";
        break;
    default							:
        p = "???";
        break;
    }
    return p;
}

char *ExplainWfPFlags(DWORD c) {
    static char eb[128];
    unsigned int l;
    strcpy(eb, "CWP_");
    if(c & CWP_SKIPDISABLED) strcat(eb, "SKIPDISABLED+");
    if(c & CWP_SKIPINVISIBLE) strcat(eb, "SKIPINVISIBLE+");
    if(c & CWP_SKIPTRANSPARENT) strcat(eb, "SKIPTRANSPARENT+");
    l = strlen(eb);
    if (l > strlen("CWP_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "CWP_ALL");
    return(eb);
}

char *ExplainChangeDisplaySettingsFlags(DWORD c) {
    static char eb[128];
    unsigned int l;
    strcpy(eb, "CDS_");
    if(c & CDS_UPDATEREGISTRY) strcat(eb, "UPDATEREGISTRY+");
    if(c & CDS_TEST) strcat(eb, "TEST+");
    if(c & CDS_FULLSCREEN) strcat(eb, "FULLSCREEN+");
    if(c & CDS_GLOBAL) strcat(eb, "GLOBAL+");
    if(c & CDS_SET_PRIMARY) strcat(eb, "SET_PRIMARY+");
    if(c & CDS_VIDEOPARAMETERS) strcat(eb, "VIDEOPARAMETERS+");
    if(c & CDS_ENABLE_UNSAFE_MODES) strcat(eb, "ENABLE_UNSAFE_MODES+");
    if(c & CDS_DISABLE_UNSAFE_MODES) strcat(eb, "DISABLE_UNSAFE_MODES+");
    if(c & CDS_RESET) strcat(eb, "RESET+");
    if(c & CDS_NORESET) strcat(eb, "NORESET+");
    l = strlen(eb);
    if (l > strlen("CDS_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "NULL");
    return(eb);
}

char *ExplainDICooperativeFlags(DWORD c) {
    static char eb[128];
    unsigned int l;
    strcpy(eb, "DISCL_");
    if(c & DISCL_EXCLUSIVE) strcat(eb, "EXCLUSIVE+");
    if(c & DISCL_NONEXCLUSIVE) strcat(eb, "NONEXCLUSIVE+");
    if(c & DISCL_FOREGROUND) strcat(eb, "FOREGROUND+");
    if(c & DISCL_BACKGROUND) strcat(eb, "BACKGROUND+");
    if(c & DISCL_NOWINKEY) strcat(eb, "NOWINKEY+");
    l = strlen(eb);
    if (l > strlen("DISCL_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "NULL");
    return(eb);
}

char *ExplainRegionType(DWORD c) {
    static char *sRetCodes[4] = {"ERROR", "NULLREGION", "SIMPLEREGION", "COMPLEXREGION"};
    if(c < 4) return sRetCodes[c];
    return "unknown";
}

char *ExplainZBufferBitDepths(DWORD c) {
    static char eb[128];
    unsigned int l;
    strcpy(eb, "DDBD_");
    if(c & DDBD_8) strcat(eb, "8+");
    if(c & DDBD_16) strcat(eb, "16+");
    if(c & DDBD_24) strcat(eb, "24+");
    if(c & DDBD_32) strcat(eb, "32+");
    l = strlen(eb);
    if (l > strlen("DDBD_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "NULL");
    return(eb);
}


#define _FACD3D  0x876
#define MAKE_D3DHRESULT( code )  MAKE_HRESULT( 1, _FACD3D, code )
#define MAKE_D3DSTATUS( code )  MAKE_HRESULT( 0, _FACD3D, code )

#define D3DERR_DRIVERINTERNALERROR              MAKE_D3DHRESULT(2087)
#define D3DERR_NOTFOUND                         MAKE_D3DHRESULT(2150)
#define D3DERR_MOREDATA                         MAKE_D3DHRESULT(2151)
#define D3DERR_DEVICELOST                       MAKE_D3DHRESULT(2152)
#define D3DERR_DEVICENOTRESET                   MAKE_D3DHRESULT(2153)
#define D3DERR_NOTAVAILABLE                     MAKE_D3DHRESULT(2154)
#define D3DERR_OUTOFVIDEOMEMORY                 MAKE_D3DHRESULT(380)
#define D3DERR_INVALIDDEVICE                    MAKE_D3DHRESULT(2155)
#define D3DERR_INVALIDCALL                      MAKE_D3DHRESULT(2156)
#define D3DERR_DRIVERINVALIDCALL                MAKE_D3DHRESULT(2157)
#define D3DERR_WASSTILLDRAWING                  MAKE_D3DHRESULT(540)
#define D3DOK_NOAUTOGEN                         MAKE_D3DSTATUS(2159)

#define D3DERR_DEVICEREMOVED                    MAKE_D3DHRESULT(2160)
#define S_NOT_RESIDENT                          MAKE_D3DSTATUS(2165)
#define S_RESIDENT_IN_SHARED_MEMORY             MAKE_D3DSTATUS(2166)
#define S_PRESENT_MODE_CHANGED                  MAKE_D3DSTATUS(2167)
#define S_PRESENT_OCCLUDED                      MAKE_D3DSTATUS(2168)
#define D3DERR_DEVICEHUNG                       MAKE_D3DHRESULT(2164)
#define D3DERR_UNSUPPORTEDOVERLAY               MAKE_D3DHRESULT(2171)
#define D3DERR_UNSUPPORTEDOVERLAYFORMAT         MAKE_D3DHRESULT(2172)
#define D3DERR_CANNOTPROTECTCONTENT             MAKE_D3DHRESULT(2173)
#define D3DERR_UNSUPPORTEDCRYPTO                MAKE_D3DHRESULT(2174)
#define D3DERR_PRESENT_STATISTICS_DISJOINT      MAKE_D3DHRESULT(2180)

char *ExplainDDError(DWORD c) {
    static char *eb;
    switch(c) {
    case DD_OK:
        eb = "DD_OK";
        break;
    case DIERR_OBJECTNOTFOUND:
        eb = "DIERR_OBJECTNOTFOUND";
        break;
    case DDERR_ALREADYINITIALIZED:
        eb = "DDERR_ALREADYINITIALIZED";
        break;
    case DDERR_BLTFASTCANTCLIP:
        eb = "DDERR_BLTFASTCANTCLIP";
        break;
    case DDERR_CANNOTATTACHSURFACE:
        eb = "DDERR_CANNOTATTACHSURFACE";
        break;
    case DDERR_CANNOTDETACHSURFACE:
        eb = "DDERR_CANNOTDETACHSURFACE";
        break;
    case DDERR_CANTCREATEDC:
        eb = "DDERR_CANTCREATEDC";
        break;
    case DDERR_CANTDUPLICATE:
        eb = "DDERR_CANTDUPLICATE";
        break;
    case DDERR_CANTLOCKSURFACE:
        eb = "DDERR_CANTLOCKSURFACE";
        break;
    case DDERR_CANTPAGELOCK:
        eb = "DDERR_CANTPAGELOCK";
        break;
    case DDERR_CANTPAGEUNLOCK:
        eb = "DDERR_CANTPAGEUNLOCK";
        break;
    case DDERR_CLIPPERISUSINGHWND:
        eb = "DDERR_CLIPPERISUSINGHWND";
        break;
    case DDERR_COLORKEYNOTSET:
        eb = "DDERR_COLORKEYNOTSET";
        break;
    case DDERR_CURRENTLYNOTAVAIL:
        eb = "DDERR_CURRENTLYNOTAVAIL";
        break;
    case DDERR_DCALREADYCREATED:
        eb = "DDERR_DCALREADYCREATED";
        break;
    case DDERR_DEVICEDOESNTOWNSURFACE:
        eb = "DDERR_DEVICEDOESNTOWNSURFACE";
        break;
    case DDERR_DIRECTDRAWALREADYCREATED:
        eb = "DDERR_DIRECTDRAWALREADYCREATED";
        break;
    case DDERR_EXCEPTION:
        eb = "DDERR_EXCEPTION";
        break;
    case DDERR_EXCLUSIVEMODEALREADYSET:
        eb = "DDERR_EXCLUSIVEMODEALREADYSET";
        break;
    case DDERR_EXPIRED:
        eb = "DDERR_EXPIRED";
        break;
    case DDERR_GENERIC:
        eb = "DDERR_GENERIC";
        break;
    case DDERR_HEIGHTALIGN:
        eb = "DDERR_HEIGHTALIGN";
        break;
    case DDERR_HWNDALREADYSET:
        eb = "DDERR_HWNDALREADYSET";
        break;
    case DDERR_HWNDSUBCLASSED:
        eb = "DDERR_HWNDSUBCLASSED";
        break;
    case DDERR_IMPLICITLYCREATED:
        eb = "DDERR_IMPLICITLYCREATED";
        break;
    case DDERR_INCOMPATIBLEPRIMARY:
        eb = "DDERR_INCOMPATIBLEPRIMARY";
        break;
    case DDERR_INVALIDCAPS:
        eb = "DDERR_INVALIDCAPS";
        break;
    case DDERR_INVALIDCLIPLIST:
        eb = "DDERR_INVALIDCLIPLIST";
        break;
    case DDERR_INVALIDDIRECTDRAWGUID:
        eb = "DDERR_INVALIDDIRECTDRAWGUID";
        break;
    case DDERR_INVALIDMODE:
        eb = "DDERR_INVALIDMODE";
        break;
    case DDERR_INVALIDOBJECT:
        eb = "DDERR_INVALIDOBJECT";
        break;
    case DDERR_INVALIDPARAMS:
        eb = "DDERR_INVALIDPARAMS";
        break;
    case DDERR_INVALIDPIXELFORMAT:
        eb = "DDERR_INVALIDPIXELFORMAT";
        break;
    case DDERR_INVALIDPOSITION:
        eb = "DDERR_INVALIDPOSITION";
        break;
    case DDERR_INVALIDRECT:
        eb = "DDERR_INVALIDRECT";
        break;
    case DDERR_INVALIDSTREAM:
        eb = "DDERR_INVALIDSTREAM";
        break;
    case DDERR_INVALIDSURFACETYPE:
        eb = "DDERR_INVALIDSURFACETYPE";
        break;
    case DDERR_LOCKEDSURFACES:
        eb = "DDERR_LOCKEDSURFACES";
        break;
    case DDERR_MOREDATA:
        eb = "DDERR_MOREDATA";
        break;
    case DDERR_NO3D:
        eb = "DDERR_NO3D";
        break;
    case DDERR_NOALPHAHW:
        eb = "DDERR_NOALPHAHW";
        break;
    case DDERR_NOBLTHW:
        eb = "DDERR_NOBLTHW";
        break;
    case DDERR_NOCLIPLIST:
        eb = "DDERR_NOCLIPLIST";
        break;
    case DDERR_NOCLIPPERATTACHED:
        eb = "DDERR_NOCLIPPERATTACHED";
        break;
    case DDERR_NOCOLORCONVHW:
        eb = "DDERR_NOCOLORCONVHW";
        break;
    case DDERR_NOCOLORKEY:
        eb = "DDERR_NOCOLORKEY";
        break;
    case DDERR_NOCOLORKEYHW:
        eb = "DDERR_NOCOLORKEYHW";
        break;
    case DDERR_NOCOOPERATIVELEVELSET:
        eb = "DDERR_NOCOOPERATIVELEVELSET";
        break;
    case DDERR_NODC:
        eb = "DDERR_NODC";
        break;
    case DDERR_NODDROPSHW:
        eb = "DDERR_NODDROPSHW";
        break;
    case DDERR_NODIRECTDRAWHW:
        eb = "DDERR_NODIRECTDRAWHW";
        break;
    case DDERR_NODIRECTDRAWSUPPORT:
        eb = "DDERR_NODIRECTDRAWSUPPORT";
        break;
    case DDERR_NOEMULATION:
        eb = "DDERR_NOEMULATION";
        break;
    case DDERR_NOEXCLUSIVEMODE:
        eb = "DDERR_NOEXCLUSIVEMODE";
        break;
    case DDERR_NOFLIPHW:
        eb = "DDERR_NOFLIPHW";
        break;
    case DDERR_NOFOCUSWINDOW:
        eb = "DDERR_NOFOCUSWINDOW";
        break;
    case DDERR_NOGDI:
        eb = "DDERR_NOGDI";
        break;
    case DDERR_NOHWND:
        eb = "DDERR_NOHWND";
        break;
    case DDERR_NOMIPMAPHW:
        eb = "DDERR_NOMIPMAPHW";
        break;
    case DDERR_NOMIRRORHW:
        eb = "DDERR_NOMIRRORHW";
        break;
    case DDERR_NONONLOCALVIDMEM:
        eb = "DDERR_NONONLOCALVIDMEM";
        break;
    case DDERR_NOOPTIMIZEHW:
        eb = "DDERR_NOOPTIMIZEHW";
        break;
    case DDERR_NOOVERLAYDEST:
        eb = "DDERR_NOOVERLAYDEST";
        break;
    case DDERR_NOOVERLAYHW:
        eb = "DDERR_NOOVERLAYHW";
        break;
    case DDERR_NOPALETTEATTACHED:
        eb = "DDERR_NOPALETTEATTACHED";
        break;
    case DDERR_NOPALETTEHW:
        eb = "DDERR_NOPALETTEHW";
        break;
    case DDERR_NORASTEROPHW:
        eb = "DDERR_NORASTEROPHW";
        break;
    case DDERR_NOROTATIONHW:
        eb = "DDERR_NOROTATIONHW";
        break;
    case DDERR_NOSTRETCHHW:
        eb = "DDERR_NOSTRETCHHW";
        break;
    case DDERR_NOT4BITCOLOR:
        eb = "DDERR_NOT4BITCOLOR";
        break;
    case DDERR_NOT4BITCOLORINDEX:
        eb = "DDERR_NOT4BITCOLORINDEX";
        break;
    case DDERR_NOT8BITCOLOR:
        eb = "DDERR_NOT8BITCOLOR";
        break;
    case DDERR_NOTAOVERLAYSURFACE:
        eb = "DDERR_NOTAOVERLAYSURFACE";
        break;
    case DDERR_NOTEXTUREHW:
        eb = "DDERR_NOTEXTUREHW";
        break;
    case DDERR_NOTFLIPPABLE:
        eb = "DDERR_NOTFLIPPABLE";
        break;
    case DDERR_NOTFOUND:
        eb = "DDERR_NOTFOUND";
        break;
    case DDERR_NOTINITIALIZED:
        eb = "DDERR_NOTINITIALIZED";
        break;
    case DDERR_NOTLOADED:
        eb = "DDERR_NOTLOADED";
        break;
    case DDERR_NOTLOCKED:
        eb = "DDERR_NOTLOCKED";
        break;
    case DDERR_NOTPAGELOCKED:
        eb = "DDERR_NOTPAGELOCKED";
        break;
    case DDERR_NOTPALETTIZED:
        eb = "DDERR_NOTPALETTIZED";
        break;
    case DDERR_NOVSYNCHW:
        eb = "DDERR_NOVSYNCHW";
        break;
    case DDERR_NOZBUFFERHW:
        eb = "DDERR_NOZBUFFERHW";
        break;
    case DDERR_NOZOVERLAYHW:
        eb = "DDERR_NOZOVERLAYHW";
        break;
    case DDERR_OUTOFCAPS:
        eb = "DDERR_OUTOFCAPS";
        break;
    case DDERR_OUTOFMEMORY:
        eb = "DDERR_OUTOFMEMORY";
        break;
    case DDERR_OUTOFVIDEOMEMORY:
        eb = "DDERR_OUTOFVIDEOMEMORY";
        break;
    case DDERR_OVERLAPPINGRECTS:
        eb = "DDERR_OVERLAPPINGRECTS";
        break;
    case DDERR_OVERLAYCANTCLIP:
        eb = "DDERR_OVERLAYCANTCLIP";
        break;
    case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
        eb = "DDERR_OVERLAYCOLORKEYONLYONEACTIVE";
        break;
    case DDERR_OVERLAYNOTVISIBLE:
        eb = "DDERR_OVERLAYNOTVISIBLE";
        break;
    case DDERR_PALETTEBUSY:
        eb = "DDERR_PALETTEBUSY";
        break;
    case DDERR_PRIMARYSURFACEALREADYEXISTS:
        eb = "DDERR_PRIMARYSURFACEALREADYEXISTS";
        break;
    case DDERR_REGIONTOOSMALL:
        eb = "DDERR_REGIONTOOSMALL";
        break;
    case DDERR_SURFACEALREADYATTACHED:
        eb = "DDERR_SURFACEALREADYATTACHED";
        break;
    case DDERR_SURFACEALREADYDEPENDENT:
        eb = "DDERR_SURFACEALREADYDEPENDENT";
        break;
    case DDERR_SURFACEBUSY:
        eb = "DDERR_SURFACEBUSY";
        break;
    case DDERR_SURFACEISOBSCURED:
        eb = "DDERR_SURFACEISOBSCURED";
        break;
    case DDERR_SURFACELOST:
        eb = "DDERR_SURFACELOST";
        break;
    case DDERR_SURFACENOTATTACHED:
        eb = "DDERR_SURFACENOTATTACHED";
        break;
    case DDERR_TOOBIGHEIGHT:
        eb = "DDERR_TOOBIGHEIGHT";
        break;
    case DDERR_TOOBIGSIZE:
        eb = "DDERR_TOOBIGSIZE";
        break;
    case DDERR_TOOBIGWIDTH:
        eb = "DDERR_TOOBIGWIDTH";
        break;
    case DDERR_UNSUPPORTED:
        eb = "DDERR_UNSUPPORTED";
        break;
    case DDERR_UNSUPPORTEDFORMAT:
        eb = "DDERR_UNSUPPORTEDFORMAT";
        break;
    case DDERR_UNSUPPORTEDMASK:
        eb = "DDERR_UNSUPPORTEDMASK";
        break;
    case DDERR_UNSUPPORTEDMODE:
        eb = "DDERR_UNSUPPORTEDMODE";
        break;
    case DDERR_VERTICALBLANKINPROGRESS:
        eb = "DDERR_VERTICALBLANKINPROGRESS";
        break;
    case DDERR_VIDEONOTACTIVE:
        eb = "DDERR_VIDEONOTACTIVE";
        break;
    case DDERR_WASSTILLDRAWING:
        eb = "DDERR_WASSTILLDRAWING";
        break;
    case DDERR_WRONGMODE:
        eb = "DDERR_WRONGMODE";
        break;
    case DDERR_XALIGN:
        eb = "DDERR_XALIGN";
        break;
    case DDERR_NOTONMIPMAPSUBLEVEL:
        eb = "DDERR_NOTONMIPMAPSUBLEVEL";
        break;
    // D3D errors
    case D3DERR_WRONGTEXTUREFORMAT:
        eb = "D3DERR_WRONGTEXTUREFORMAT";
        break;
    case D3DERR_UNSUPPORTEDCOLOROPERATION:
        eb = "D3DERR_UNSUPPORTEDCOLOROPERATION";
        break;
    case D3DERR_UNSUPPORTEDCOLORARG:
        eb = "D3DERR_UNSUPPORTEDCOLORARG";
        break;
    case D3DERR_UNSUPPORTEDALPHAOPERATION:
        eb = "D3DERR_UNSUPPORTEDALPHAOPERATION";
        break;
    case D3DERR_UNSUPPORTEDALPHAARG:
        eb = "D3DERR_UNSUPPORTEDALPHAARG";
        break;
    case D3DERR_TOOMANYOPERATIONS:
        eb = "D3DERR_TOOMANYOPERATIONS";
        break;
    case D3DERR_CONFLICTINGTEXTUREFILTER:
        eb = "D3DERR_CONFLICTINGTEXTUREFILTER";
        break;
    case D3DERR_UNSUPPORTEDFACTORVALUE:
        eb = "D3DERR_UNSUPPORTEDFACTORVALUE";
        break;
    case D3DERR_CONFLICTINGRENDERSTATE:
        eb = "D3DERR_CONFLICTINGRENDERSTATE";
        break;
    case D3DERR_UNSUPPORTEDTEXTUREFILTER:
        eb = "D3DERR_UNSUPPORTEDTEXTUREFILTER";
        break;
    case D3DERR_CONFLICTINGTEXTUREPALETTE:
        eb = "D3DERR_CONFLICTINGTEXTUREPALETTE";
        break;
    case D3DERR_DRIVERINTERNALERROR:
        eb = "D3DERR_DRIVERINTERNALERROR";
        break;
    case D3DERR_NOTFOUND:
        eb = "D3DERR_NOTFOUND";
        break;
    case D3DERR_MOREDATA:
        eb = "D3DERR_MOREDATA";
        break;
    case D3DERR_DEVICELOST:
        eb = "D3DERR_DEVICELOST";
        break;
    case D3DERR_DEVICENOTRESET:
        eb = "D3DERR_DEVICENOTRESET";
        break;
    case D3DERR_NOTAVAILABLE:
        eb = "D3DERR_NOTAVAILABLE";
        break;
    //case D3DERR_OUTOFVIDEOMEMORY: 		 	eb="D3DERR_OUTOFVIDEOMEMORY"; break;
    case D3DERR_INVALIDDEVICE:
        eb = "D3DERR_INVALIDDEVICE";
        break;
    case D3DERR_INVALIDCALL:
        eb = "D3DERR_INVALIDCALL";
        break;
    case D3DERR_DRIVERINVALIDCALL:
        eb = "D3DERR_DRIVERINVALIDCALL";
        break;
    //case D3DERR_WASSTILLDRAWING: 		 	eb="D3DERR_WASSTILLDRAWING"; break;
    case D3DOK_NOAUTOGEN:
        eb = "D3DOK_NOAUTOGEN";
        break;
    case D3DERR_DEVICEREMOVED:
        eb = "D3DERR_DEVICEREMOVED";
        break;
    case D3DERR_DEVICEHUNG:
        eb = "D3DERR_DEVICEHUNG";
        break;
    case D3DERR_UNSUPPORTEDOVERLAY:
        eb = "D3DERR_UNSUPPORTEDOVERLAY";
        break;
    case D3DERR_UNSUPPORTEDOVERLAYFORMAT:
        eb = "D3DERR_UNSUPPORTEDOVERLAYFORMAT";
        break;
    case D3DERR_CANNOTPROTECTCONTENT:
        eb = "D3DERR_CANNOTPROTECTCONTENT";
        break;
    case D3DERR_UNSUPPORTEDCRYPTO:
        eb = "D3DERR_UNSUPPORTEDCRYPTO";
        break;
    case D3DERR_PRESENT_STATISTICS_DISJOINT:
        eb = "D3DERR_PRESENT_STATISTICS_DISJOINT";
        break;
    case D3DERR_INBEGIN:
        eb = "D3DERR_INBEGIN";
        break;
    case D3DERR_NOTINBEGIN:
        eb = "D3DERR_NOTINBEGIN";
        break;
    case D3DERR_NOVIEWPORTS:
        eb = "D3DERR_NOVIEWPORTS";
        break;
    case D3DERR_VIEWPORTDATANOTSET:
        eb = "D3DERR_VIEWPORTDATANOTSET";
        break;
    case D3DERR_VIEWPORTHASNODEVICE:
        eb = "D3DERR_VIEWPORTHASNODEVICE";
        break;
    case D3DERR_NOCURRENTVIEWPORT:
        eb = "D3DERR_NOCURRENTVIEWPORT";
        break;
    // ddrawex error codes
    case DDERR_LOADFAILED:
        eb = "DDERR_LOADFAILED";
        break;
    case DDERR_BADVERSIONINFO:
        eb = "DDERR_BADVERSIONINFO";
        break;
    case DDERR_BADPROCADDRESS:
        eb = "DDERR_BADPROCADDRESS";
        break;
    case DDERR_LEGACYUSAGE:
        eb = "DDERR_LEGACYUSAGE";
        break;
    // DSOUND error codes
    case DSERR_ALLOCATED:
        eb = "DSERR_ALLOCATED";
        break;
    case DS_NO_VIRTUALIZATION:
        eb = "DS_NO_VIRTUALIZATION";
        break;
    case DSERR_CONTROLUNAVAIL:
        eb = "DSERR_CONTROLUNAVAIL";
        break;
    //case DSERR_INVALIDPARAM:                eb="DSERR_INVALIDPARAM"; break;
    case DSERR_INVALIDCALL:
        eb = "DSERR_INVALIDCALL";
        break;
    case DSERR_PRIOLEVELNEEDED:
        eb = "DSERR_PRIOLEVELNEEDED";
        break;
    //case DSERR_OUTOFMEMORY:                 eb="DSERR_OUTOFMEMORY"; break;
    case DSERR_BADFORMAT:
        eb = "DSERR_BADFORMAT";
        break;
    case DSERR_NODRIVER:
        eb = "DSERR_NODRIVER";
        break;
    case DSERR_ALREADYINITIALIZED:
        eb = "DSERR_ALREADYINITIALIZED";
        break;
    case DSERR_NOAGGREGATION:
        eb = "DSERR_NOAGGREGATION";
        break;
    case DSERR_BUFFERLOST:
        eb = "DSERR_BUFFERLOST";
        break;
    case DSERR_OTHERAPPHASPRIO:
        eb = "DSERR_OTHERAPPHASPRIO";
        break;
    case DSERR_UNINITIALIZED:
        eb = "DSERR_UNINITIALIZED";
        break;
    case DSERR_NOINTERFACE:
        eb = "DSERR_NOINTERFACE";
        break;
    case DSERR_ACCESSDENIED:
        eb = "DSERR_ACCESSDENIED";
        break;
    case DSERR_BUFFERTOOSMALL:
        eb = "DSERR_BUFFERTOOSMALL";
        break;
    case DSERR_DS8_REQUIRED:
        eb = "DSERR_DS8_REQUIRED";
        break;
    case DSERR_SENDLOOP:
        eb = "DSERR_SENDLOOP";
        break;
    case DSERR_BADSENDBUFFERGUID:
        eb = "DSERR_BADSENDBUFFERGUID";
        break;
    case DSERR_OBJECTNOTFOUND:
        eb = "DSERR_OBJECTNOTFOUND";
        break;
    case DSERR_FXUNAVAILABLE:
        eb = "DSERR_FXUNAVAILABLE";
        break;
    // DINPUT errors
    case DIERR_INPUTLOST:
        eb = "DIERR_INPUTLOST";
        break;
    //case DIERR_INVALIDPARAM:				eb="DIERR_INVALIDPARAM"; break;
    case DIERR_NOTACQUIRED:
        eb = "DIERR_NOTACQUIRED";
        break;
    case DIERR_NOTINITIALIZED:
        eb = "DIERR_NOTINITIALIZED";
        break;
    case DIERR_NOTBUFFERED:
        eb = "DIERR_NOTBUFFERED";
        break;
    case E_PENDING:
        eb = "E_PENDING";
        break;
    /*
        // DXGI errors
        case DXGI_ERROR_ACCESS_DENIED:			eb="DXGI_ERROR_ACCESS_DENIED"; break;
        case DXGI_ERROR_ACCESS_LOST:			eb="DXGI_ERROR_ACCESS_LOST"; break;
        case DXGI_ERROR_ALREADY_EXISTS:			eb="DXGI_ERROR_ALREADY_EXISTS"; break;
        case DXGI_ERROR_CANNOT_PROTECT_CONTENT:	eb="DXGI_ERROR_CANNOT_PROTECT_CONTENT"; break;
        case DXGI_ERROR_DEVICE_HUNG:			eb="DXGI_ERROR_DEVICE_HUNG"; break;
        case DXGI_ERROR_DEVICE_REMOVED:			eb="DXGI_ERROR_DEVICE_REMOVED"; break;
        case DXGI_ERROR_DEVICE_RESET:			eb="DXGI_ERROR_DEVICE_RESET"; break;
        case DXGI_ERROR_DRIVER_INTERNAL_ERROR:	eb="DXGI_ERROR_DRIVER_INTERNAL_ERROR"; break;
        case DXGI_ERROR_FRAME_STATISTICS_DISJOINT:	eb="DXGI_ERROR_FRAME_STATISTICS_DISJOINT"; break;
        case DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE:	eb="DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE"; break;
        case DXGI_ERROR_INVALID_CALL:			eb="DXGI_ERROR_INVALID_CALL"; break;
        case DXGI_ERROR_MORE_DATA:				eb="DXGI_ERROR_MORE_DATA"; break;
        case DXGI_ERROR_NAME_ALREADY_EXISTS:	eb="DXGI_ERROR_NAME_ALREADY_EXISTS"; break;
        case DXGI_ERROR_NONEXCLUSIVE:			eb="DXGI_ERROR_NONEXCLUSIVE"; break;
        case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE:	eb="DXGI_ERROR_NOT_CURRENTLY_AVAILABLE"; break;
        case DXGI_ERROR_NOT_FOUND:				eb="DXGI_ERROR_NOT_FOUND"; break;
        case DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED:	eb="DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED"; break;
        case DXGI_ERROR_REMOTE_OUTOFMEMORY:		eb="DXGI_ERROR_REMOTE_OUTOFMEMORY"; break;
        case DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE:	eb="DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE"; break;
        case DXGI_ERROR_SDK_COMPONENT_MISSING:	eb="DXGI_ERROR_SDK_COMPONENT_MISSING"; break;
        case DXGI_ERROR_SESSION_DISCONNECTED:	eb="DXGI_ERROR_SESSION_DISCONNECTED"; break;
        case DXGI_ERROR_UNSUPPORTED:			eb="DXGI_ERROR_UNSUPPORTED"; break;
        case DXGI_ERROR_WAIT_TIMEOUT:			eb="DXGI_ERROR_WAIT_TIMEOUT"; break;
        case DXGI_ERROR_WAS_STILL_DRAWING:		eb="DXGI_ERROR_WAS_STILL_DRAWING"; break;
    */
    // weirdos ...
    case RPC_E_CHANGED_MODE:
        eb = "RPC_E_CHANGED_MODE";
        break; // in "Rubik's Games"
    default:
        eb = "unknown";
        break;
    }
    return eb;
}

typedef enum _myD3DFORMAT {
    D3DFMT_UNKNOWN              =  0,
    D3DFMT_R8G8B8               = 20,
    D3DFMT_A8R8G8B8             = 21,
    D3DFMT_X8R8G8B8             = 22,
    D3DFMT_R5G6B5               = 23,
    D3DFMT_X1R5G5B5             = 24,
    D3DFMT_A1R5G5B5             = 25,
    D3DFMT_A4R4G4B4             = 26,
    D3DFMT_R3G3B2               = 27,
    D3DFMT_A8                   = 28,
    D3DFMT_A8R3G3B2             = 29,
    D3DFMT_X4R4G4B4             = 30,
    D3DFMT_A2B10G10R10          = 31,
    D3DFMT_A8B8G8R8             = 32,
    D3DFMT_X8B8G8R8             = 33,
    D3DFMT_G16R16               = 34,
    D3DFMT_A2R10G10B10          = 35,
    D3DFMT_A16B16G16R16         = 36,
    D3DFMT_A8P8                 = 40,
    D3DFMT_P8                   = 41,
    D3DFMT_L8                   = 50,
    D3DFMT_A8L8                 = 51,
    D3DFMT_A4L4                 = 52,
    D3DFMT_V8U8                 = 60,
    D3DFMT_L6V5U5               = 61,
    D3DFMT_X8L8V8U8             = 62,
    D3DFMT_Q8W8V8U8             = 63,
    D3DFMT_V16U16               = 64,
    D3DFMT_A2W10V10U10          = 67,
    D3DFMT_D16_LOCKABLE         = 70,
    D3DFMT_D32                  = 71,
    D3DFMT_D15S1                = 73,
    D3DFMT_D24S8                = 75,
    D3DFMT_D24X8                = 77,
    D3DFMT_D24X4S4              = 79,
    D3DFMT_D16                  = 80,
    D3DFMT_D32F_LOCKABLE        = 82,
    D3DFMT_D24FS8               = 83,
    D3DFMT_D32_LOCKABLE         = 84,
    D3DFMT_S8_LOCKABLE          = 85,
    D3DFMT_L16                  = 81,
    D3DFMT_VERTEXDATA           = 100,
    D3DFMT_INDEX16              = 101,
    D3DFMT_INDEX32              = 102,
    D3DFMT_Q16W16V16U16         = 110,
    D3DFMT_R16F                 = 111,
    D3DFMT_G16R16F              = 112,
    D3DFMT_A16B16G16R16F        = 113,
    D3DFMT_R32F                 = 114,
    D3DFMT_G32R32F              = 115,
    D3DFMT_A32B32G32R32F        = 116,
    D3DFMT_CxV8U8               = 117,
    D3DFMT_A1                   = 118,
    D3DFMT_A2B10G10R10_XR_BIAS  = 119,
    D3DFMT_BINARYBUFFER         = 199,
    D3DFMT_FORCE_DWORD          = 0x7fffffff
} myD3DFORMAT;

/*
    char *sCompression(DWORD c)
    {
	char *t[]={"RGB", "RLE8", "RLE4", "BITFIELDS", "JPEG", "PNG" };
	if (c <= BI_PNG) return t[c];
	return "unknown";
    }
*/

char *sFourCC(DWORD fcc) {
    static char sRet[7];
    char *pRet;
    char c;
    int i;
    switch(fcc) {
    case BI_RGB						:
        pRet = "RGB";
        break;
    case BI_RLE8					:
        pRet = "RLE8";
        break;
    case BI_RLE4					:
        pRet = "RLE4";
        break;
    case BI_BITFIELDS               :
        pRet = "BITFIELDS";
        break;
    case BI_JPEG					:
        pRet = "JPEG";
        break;
    case BI_PNG						:
        pRet = "PNG";
        break;
    //case D3DFMT_UNKNOWN              : pRet = "UNKNOWN"; break;
    case D3DFMT_R8G8B8               :
        pRet = "R8G8B8";
        break;
    case D3DFMT_A8R8G8B8             :
        pRet = "A8R8G8B8";
        break;
    case D3DFMT_X8R8G8B8             :
        pRet = "X8R8G8B8";
        break;
    case D3DFMT_R5G6B5               :
        pRet = "R5G6B5";
        break;
    case D3DFMT_X1R5G5B5             :
        pRet = "X1R5G5B5";
        break;
    case D3DFMT_A1R5G5B5             :
        pRet = "A1R5G5B5";
        break;
    case D3DFMT_A4R4G4B4             :
        pRet = "A4R4G4B4";
        break;
    case D3DFMT_R3G3B2               :
        pRet = "R3G3B2";
        break;
    case D3DFMT_A8                   :
        pRet = "A8";
        break;
    case D3DFMT_A8R3G3B2             :
        pRet = "A8R3G3B2";
        break;
    case D3DFMT_X4R4G4B4             :
        pRet = "X4R4G4B4";
        break;
    case D3DFMT_A2B10G10R10          :
        pRet = "A2B10G10R10";
        break;
    case D3DFMT_A8B8G8R8             :
        pRet = "A8B8G8R8";
        break;
    case D3DFMT_X8B8G8R8             :
        pRet = "X8B8G8R8";
        break;
    case D3DFMT_G16R16               :
        pRet = "G16R16";
        break;
    case D3DFMT_A2R10G10B10          :
        pRet = "A2R10G10B10";
        break;
    case D3DFMT_A16B16G16R16         :
        pRet = "A16B16G16R16";
        break;
    case D3DFMT_A8P8                 :
        pRet = "A8P8";
        break;
    case D3DFMT_P8                   :
        pRet = "P8";
        break;
    case D3DFMT_L8                   :
        pRet = "L8";
        break;
    case D3DFMT_A8L8                 :
        pRet = "A8L8";
        break;
    case D3DFMT_A4L4                 :
        pRet = "A4L4";
        break;
    case D3DFMT_V8U8                 :
        pRet = "V8U8";
        break;
    case D3DFMT_L6V5U5               :
        pRet = "L6V5U5";
        break;
    case D3DFMT_X8L8V8U8             :
        pRet = "X8L8V8U8";
        break;
    case D3DFMT_Q8W8V8U8             :
        pRet = "Q8W8V8U8";
        break;
    case D3DFMT_V16U16               :
        pRet = "V16U16";
        break;
    case D3DFMT_A2W10V10U10          :
        pRet = "A2W10V10U10";
        break;
    case D3DFMT_D16_LOCKABLE         :
        pRet = "D16_LOCKABLE";
        break;
    case D3DFMT_D32                  :
        pRet = "D32";
        break;
    case D3DFMT_D15S1                :
        pRet = "D15S1";
        break;
    case D3DFMT_D24S8                :
        pRet = "D24S8";
        break;
    case D3DFMT_D24X8                :
        pRet = "D24X8";
        break;
    case D3DFMT_D24X4S4              :
        pRet = "D24X4S4";
        break;
    case D3DFMT_D16                  :
        pRet = "D16";
        break;
    case D3DFMT_D32F_LOCKABLE        :
        pRet = "D32F_LOCKABLE";
        break;
    case D3DFMT_D24FS8               :
        pRet = "D24FS8";
        break;
    case D3DFMT_D32_LOCKABLE         :
        pRet = "D32_LOCKABLE";
        break;
    case D3DFMT_S8_LOCKABLE          :
        pRet = "S8_LOCKABLE";
        break;
    case D3DFMT_L16                  :
        pRet = "L16";
        break;
    case D3DFMT_VERTEXDATA           :
        pRet = "VERTEXDATA";
        break;
    case D3DFMT_INDEX16              :
        pRet = "INDEX16";
        break;
    case D3DFMT_INDEX32              :
        pRet = "INDEX32";
        break;
    case D3DFMT_Q16W16V16U16         :
        pRet = "Q16W16V16U16";
        break;
    case D3DFMT_R16F                 :
        pRet = "R16F";
        break;
    case D3DFMT_G16R16F              :
        pRet = "G16R16F";
        break;
    case D3DFMT_A16B16G16R16F        :
        pRet = "A16B16G16R16F";
        break;
    case D3DFMT_R32F                 :
        pRet = "R32F";
        break;
    case D3DFMT_G32R32F              :
        pRet = "G32R32F";
        break;
    case D3DFMT_A32B32G32R32F        :
        pRet = "A32B32G32R32F";
        break;
    case D3DFMT_CxV8U8               :
        pRet = "CxV8U8";
        break;
    case D3DFMT_A1                   :
        pRet = "A1";
        break;
    case D3DFMT_A2B10G10R10_XR_BIAS  :
        pRet = "A2B10G10R10_XR_BIAS";
        break;
    case D3DFMT_BINARYBUFFER         :
        pRet = "BINARYBUFFER";
        break;
    default:
        char *t = &sRet[0];
        *(t++) = '\"';
        for(i = 0; i < 4; i++) {
            c = fcc & (0xFF);
            *(t++) = isprint(c) ? c : '.';
            fcc = fcc >> 8;
        }
        *(t++) = '\"';
        *t = 0;
        pRet = sRet;
        break;
    }
    return pRet;
}

char *ExplainPixelFormat(LPDDPIXELFORMAT ddpfPixelFormat) {
    static char sBuf[512];
    char sItem[256];
    DWORD flags = ddpfPixelFormat->dwFlags;
    sprintf(sBuf, " PixelFormat flags=%#x(%s) BPP=%d",
            flags, ExplainPixelFormatFlags(flags), ddpfPixelFormat->dwRGBBitCount);
    if (flags & DDPF_RGB) {
        if (flags & DDPF_ALPHAPIXELS) {
            sprintf(sItem, " RGBA=(%#x,%#x,%#x,%#x)",
                    ddpfPixelFormat->dwRBitMask,
                    ddpfPixelFormat->dwGBitMask,
                    ddpfPixelFormat->dwBBitMask,
                    ddpfPixelFormat->dwRGBAlphaBitMask);
        } else {
            sprintf(sItem, " RGB=(%#x,%#x,%#x)",
                    ddpfPixelFormat->dwRBitMask,
                    ddpfPixelFormat->dwGBitMask,
                    ddpfPixelFormat->dwBBitMask);
        }
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_YUV) {
        sprintf(sItem, " YUVA=(%#x,%#x,%#x,%#x)",
                ddpfPixelFormat->dwYBitMask,
                ddpfPixelFormat->dwUBitMask,
                ddpfPixelFormat->dwVBitMask,
                ddpfPixelFormat->dwYUVAlphaBitMask);
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_ZBUFFER) {
        sprintf(sItem, " SdZSbL=(%#x,%#x,%#x,%#x)",
                ddpfPixelFormat->dwStencilBitDepth,
                ddpfPixelFormat->dwZBitMask,
                ddpfPixelFormat->dwStencilBitMask,
                ddpfPixelFormat->dwLuminanceAlphaBitMask);
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_ALPHA) {
        sprintf(sItem, " LBdBlZ=(%#x,%#x,%#x,%#x)",
                ddpfPixelFormat->dwLuminanceBitMask,
                ddpfPixelFormat->dwBumpDvBitMask,
                ddpfPixelFormat->dwBumpLuminanceBitMask,
                ddpfPixelFormat->dwRGBZBitMask);
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_LUMINANCE) {
        sprintf(sItem, " BMbMF=(%#x,%#x,%#x,%#x)",
                ddpfPixelFormat->dwBumpDuBitMask,
                ddpfPixelFormat->MultiSampleCaps.wBltMSTypes,
                ddpfPixelFormat->MultiSampleCaps.wFlipMSTypes,
                ddpfPixelFormat->dwYUVZBitMask);
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_BUMPDUDV) {
        sprintf(sItem, " O=(%#x)",
                ddpfPixelFormat->dwOperations);
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_FOURCC) {
        sprintf(sItem, " FourCC=%#x(%s)",
                ddpfPixelFormat->dwFourCC, sFourCC(ddpfPixelFormat->dwFourCC));
        strcat(sBuf, sItem);
    }
    return sBuf;
}

char *GetObjectTypeStr(HDC hdc) {
    char *s;
    static char sBuf[21];
    switch (GetObjectType(hdc)) {
    case OBJ_PEN:
        s = "PEN";
        break;
    case OBJ_BRUSH:
        s = "BRUSH";
        break;
    case OBJ_DC:
        s = "DC";
        break;
    case OBJ_METADC:
        s = "METADC";
        break;
    case OBJ_PAL:
        s = "PAL";
        break;
    case OBJ_FONT:
        s = "FONT";
        break;
    case OBJ_BITMAP:
        s = "BITMAP";
        break;
    case OBJ_REGION:
        s = "REGION";
        break;
    case OBJ_METAFILE:
        s = "METAFILE";
        break;
    case OBJ_MEMDC:
        s = "MEMDC";
        break;
    case OBJ_EXTPEN:
        s = "EXTPEN";
        break;
    case OBJ_ENHMETADC:
        s = "ENHMETADC";
        break;
    case OBJ_ENHMETAFILE:
        s = "ENHMETAFILE";
        break;
    case OBJ_COLORSPACE:
        s = "COLORSPACE";
        break;
    default:
        //s="unknown"; break;
        sprintf(sBuf, "0x%X", GetObjectType(hdc));
        s = sBuf;
        break;
    }
    return s;
}

char *ExplainRgnType(int type) {
    char *s;
    switch(type) {
    case NULLREGION:
        s = "NULLREGION";
        break;
    case SIMPLEREGION:
        s = "SIMPLEREGION";
        break;
    case COMPLEXREGION:
        s = "COMPLEXREGION";
        break;
    case ERROR:
        s = "ERROR";
        break;
    default:
        s = "unknown";
        break;
    }
    return s;
}
#endif
