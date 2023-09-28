#define  _CRT_SECURE_NO_WARNINGS
#define DIRECTINPUT_VERSION 0x700
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "dinput.h"

//extern HRESULT WINAPI DirectInputCreateA(HINSTANCE, DWORD, LPDIRECTINPUT *, LPUNKNOWN);
//extern HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);

HRESULT WINAPI Callback(LPCDIDEVICEOBJECTINSTANCEA dev, LPVOID arg) {
    HRESULT res;
    LPCDIDEVICEOBJECTINSTANCEA devA;
    LPCDIDEVICEOBJECTINSTANCEW devW;
    switch (dev->dwSize) {
    case sizeof(DIDEVICEOBJECTINSTANCEA):
        devA = (LPCDIDEVICEOBJECTINSTANCEA)dev;
        printf("ASCII type=%x flags=%x ofs=%d dim=%d name=\"%s\"\n",
               devA->dwType, devA->dwFlags, devA->dwOfs, devA->dwDimension, devA->tszName);
        break;
    case sizeof(DIDEVICEOBJECTINSTANCEW):
        devW = (LPCDIDEVICEOBJECTINSTANCEW)dev;
        printf("WIDECHAR type=%x flags=%x ofs=%d dim=%d name=%ls\n",
               devW->dwType, devW->dwFlags, devW->dwOfs, devW->dwDimension, devW->tszName);
        break;
    default:
        printf("unknown struct size=%d\n", dev->dwSize);
        break;
    }
    //HRESULT res;
    //printf("Callback\n");
    //printf("CALLBACK size=%d(%s) flags=%x ofs=%d dimensions=%d name=\"%s\"\n",
    //	devA->dwSize, devA->dwType, devA->dwFlags, devA->dwOfs, devA->dwDimension, devA->tszName);
    //return 0;
}

int main(int argc, char *argv[]) {
    HRESULT res;
    LPDIRECTINPUT lpDI;
    LPDIRECTINPUTDEVICEA lpDId;
    res = DirectInputCreateA(GetModuleHandle(NULL), 0x700, &lpDI, NULL);
    if(res) {
        printf("DirectInputCreateA ERROR: res=%x\n", res);
        return -1;
    }
    printf("lpDI=%x\n", lpDI);
    res = lpDI->CreateDevice(GUID_SysKeyboard, &lpDId, NULL);
    if(res) {
        printf("CreateDevice ERROR: res=%x\n", res);
        return -1;
    }
    printf("lpDId=%x\n", lpDId);
    res = lpDId->EnumObjects((LPDIENUMDEVICEOBJECTSCALLBACKA)Callback, NULL, NULL);
    if(res) {
        printf("EnumObjects ERROR: res=%x\n", res);
        return -1;
    }
}

//DirectInput8Create: dwVersion=800 REFIID=bf798030(IID_IDirectInput8A)