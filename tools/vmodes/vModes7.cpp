
#define  _CRT_SECURE_NO_WARNINGS
#define DIRECT3D_VERSION 0x0700

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "d3d.h"

//typedef HRESULT (WINAPI *Direct3DCreate_Type)(UINT, LPDIRECT3D *, LPUNKNOWN);
typedef HRESULT (WINAPI *Direct3DCreate_Type)(UINT, LPDIRECT3D *, LPVOID);

static char *sRIID(REFIID riid) {
    static char sRIIDBuffer[81];
    OLECHAR *guidString;
    StringFromCLSID(riid, &guidString);
    sprintf(sRIIDBuffer, "%ls",  guidString);
    ::CoTaskMemFree(guidString);
    return sRIIDBuffer;
}

static void DumpD3DDeviceDesc(LPD3DDEVICEDESC d3, char *label) {
    if(!d3) {
        printf("\t%s ddesc=NULL\n", label);
        return;
    }
    printf("\t%s Size=%d Flags=%#x ", label, d3->dwSize, d3->dwFlags);
    if(d3->dwFlags & D3DDD_COLORMODEL) printf("ColorModel=%#x ", d3->dcmColorModel);
    if(d3->dwFlags & D3DDD_DEVCAPS) printf("DevCaps=%#x ", d3->dwDevCaps);
    if(d3->dwFlags & D3DDD_TRANSFORMCAPS) printf("TransformCaps=%#x ", d3->dtcTransformCaps.dwCaps);
    if(d3->dwFlags & D3DDD_LIGHTINGCAPS) printf("LightingCaps=%#x ", d3->dlcLightingCaps);
    if(d3->dwFlags & D3DDD_BCLIPPING) printf("Clipping=%#x ", d3->bClipping);
    if(d3->dwFlags & D3DDD_LINECAPS) printf("LineCaps=%#x ", d3->dpcLineCaps);
    if(d3->dwFlags & D3DDD_TRICAPS) printf("TriCaps=%#x ", d3->dpcTriCaps);
    if(d3->dwFlags & D3DDD_DEVICERENDERBITDEPTH) printf("DeviceRenderBitDepth=%d ", d3->dwDeviceRenderBitDepth);
    if(d3->dwFlags & D3DDD_DEVICEZBUFFERBITDEPTH) printf("DeviceZBufferBitDepth=%d ", d3->dwDeviceZBufferBitDepth);
    if(d3->dwFlags & D3DDD_MAXBUFFERSIZE) printf("MaxBufferSize=%d ", d3->dwMaxBufferSize);
    if(d3->dwFlags & D3DDD_MAXVERTEXCOUNT) printf("MaxVertexCount=%d ", d3->dwMaxVertexCount);
    printf("Texture min=(%dx%d) max=(%dx%d)\n", d3->dwMinTextureWidth, d3->dwMinTextureHeight, d3->dwMaxTextureWidth, d3->dwMaxTextureHeight);
}

HRESULT WINAPI cbDevices7(LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC7 lpd3ddd, LPVOID arg) {
    printf("------------\n");
    printf("\tDevName      : %s\n", lpDeviceName);
    printf("\tDescription  : %s\n", lpDeviceDescription);
    printf("\tCaps         : %#x\n", lpd3ddd->dwDevCaps);
    printf("\tStencilCaps  : %#x\n", lpd3ddd->dwStencilCaps);
    printf("\tDevRendBDepth: %d\n",  lpd3ddd->dwDeviceRenderBitDepth);
    printf("\tDevZBufBDepth: %d\n", lpd3ddd->dwDeviceZBufferBitDepth);
    return TRUE;
}

HRESULT WINAPI cbDevices3(GUID FAR *lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC lpd3ddd1, LPD3DDEVICEDESC lpd3ddd2, LPVOID arg) {
    printf("------------\n");
    //printf("\tGUID         : %s\n", sRIID(&lpGuid));
    printf("\tDevName      : %s\n", lpDeviceName);
    printf("\tDescription  : %s\n", lpDeviceDescription);
    DumpD3DDeviceDesc(lpd3ddd1, (char *)"HARDWARE     :");
    DumpD3DDeviceDesc(lpd3ddd2, (char *)"SOFTWARE     :");
    return TRUE;
}

int main(int argc, char *argv[]) {
    HRESULT res;
    LPDIRECTDRAW7 lpDD7;
    LPDIRECTDRAW lpDD;
    LPDIRECT3D7 lpD3D7;
    LPDIRECT3D lpD3D;
    LPDIRECT3D2 lpD3D2;
    LPDIRECT3D3 lpD3D3;
    printf("------------ vModes7 v1.00.00\n");
    printf("------------ DisplayDevice\n");
    for(int iDisplayDevice = 0; ; iDisplayDevice++) {
        DISPLAY_DEVICEA DisplayDevice;
        memset(&DisplayDevice, 0, sizeof(DISPLAY_DEVICEA)); // just in case ...
        DisplayDevice.cb = sizeof(DISPLAY_DEVICEA);
        if(!EnumDisplayDevices(NULL, iDisplayDevice, &DisplayDevice, 0)) break;
        printf("DisplayDevice[%i]:\n", iDisplayDevice);
        printf("\tDevName: %.32s\n", DisplayDevice.DeviceName);
        printf("\tDevStr.: %.32s\n", DisplayDevice.DeviceString);
        printf("\tFlags  : %#x\n", DisplayDevice.StateFlags);
        printf("\tDevId  : %.128s\n", DisplayDevice.DeviceID);
        printf("\tDevKey : %.128s\n", DisplayDevice.DeviceKey);
    }
    printf("------------ Direct3D7\n");
    res = DirectDrawCreateEx(NULL, (LPVOID *)&lpDD7, IID_IDirectDraw7, NULL);
    if(res) {
        printf("Direct3DCreateEx ERROR res=%#x\n", res);
        return -1;
    }
    res = lpDD7->QueryInterface(IID_IDirect3D7, (LPVOID *)&lpD3D7);
    if(res) {
        printf("IDirectDraw7::QueryInterface(IID_IDirect3D7) ERROR res=%#x\n", res);
        return -1;
    }
    printf("lpDD7=%#x lpD3D7=%#x\n", lpDD7, lpD3D7);
    res = lpD3D7->EnumDevices(cbDevices7, NULL);
    if(res) {
        printf("IDirect3D7::EnumDevices ERROR res=%#x\n", res);
        return -1;
    }
    lpD3D7->Release();
    lpDD7->Release();
    printf("------------ Direct3D3\n");
    res = DirectDrawCreate(NULL, &lpDD, NULL);
    if(res) {
        printf("Direct3DCreate ERROR res=%#x\n", res);
        return -1;
    }
    res = lpDD->QueryInterface(IID_IDirect3D3, (LPVOID *)&lpD3D3);
    if(res) {
        printf("IDirectDraw::QueryInterface(IID_IDirect3D3) ERROR res=%#x\n", res);
        return -1;
    }
    printf("lpDD=%#x lpD3D3=%#x\n", lpDD, lpD3D3);
    res = lpD3D3->EnumDevices(cbDevices3, NULL);
    if(res) {
        printf("IDirect3D3::EnumDevices ERROR res=%#x\n", res);
        return -1;
    }
    lpD3D3->Release();
    printf("------------ Direct3D2\n");
    res = lpDD->QueryInterface(IID_IDirect3D2, (LPVOID *)&lpD3D2);
    if(res) {
        printf("IDirectDraw::QueryInterface(IID_IDirect3D2) ERROR res=%#x\n", res);
        return -1;
    }
    printf("lpDD=%#x lpD3D2=%#x\n", lpDD, lpD3D2);
    res = lpD3D2->EnumDevices(cbDevices3, NULL);
    if(res) {
        printf("IDirect3D2::EnumDevices ERROR res=%#x\n", res);
        return -1;
    }
    lpD3D2->Release();
    printf("------------ Direct3D\n");
    res = lpDD->QueryInterface(IID_IDirect3D, (LPVOID *)&lpD3D);
    if(res) {
        printf("IDirectDraw::QueryInterface(IID_IDirect3D) ERROR res=%#x\n", res);
        return -1;
    }
    printf("lpDD=%#x lpD3D=%#x\n", lpDD, lpD3D);
    res = lpD3D->EnumDevices(cbDevices3, NULL);
    if(res) {
        printf("IDirect3D::EnumDevices ERROR res=%#x\n", res);
        return -1;
    }
    lpD3D->Release();
    lpDD->Release();
    printf("------------ end\n");
}

