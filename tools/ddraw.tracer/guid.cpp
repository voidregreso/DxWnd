#define  _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include "dxdiag.h"
#include "ddraw.h"

int GUIDDDrawVersion(GUID *lpGUID, char *GuidName) {
    int version = 0;
    char *mode;
    mode = "";
    switch ((DWORD)lpGUID) {
    case 0:
        mode = "NULL";
        break;
    case DDCREATE_EMULATIONONLY:
        mode = "DDCREATE_EMULATIONONLY";
        break;
    case DDCREATE_HARDWAREONLY:
        mode = "DDCREATE_HARDWAREONLY";
        break;
    default:
        switch(lpGUID->Data1) {
        case 0x6C14DB80:
            version = 1;
            mode = "IID_IDirectDraw";
            break;
        case 0xB3A6F3E0:
            version = 2;
            mode = "IID_IDirectDraw2";
            break;
        case 0x9C59509A:
            version = 4;
            mode = "IID_IDirectDraw4";
            break;
        case 0x15E65EC0:
            version = 7;
            mode = "IID_IDirectDraw7";
            break;
        }
        break;
    }
    if(GuidName) strcpy(GuidName, mode);
    return version;
}

int GUIDSurfaceVersion(GUID *lpGUID, char *GuidName) {
    int version = 0;
    char *mode;
    mode = "";
    switch(lpGUID->Data1) {
    case 0x6C14DB81:
        version = 1;
        mode = "IID_IDirectDrawSurface";
        break;
    case 0x57805885:
        version = 2;
        mode = "IID_IDirectDrawSurface2";
        break;
    case 0xDA044E00:
        version = 3;
        mode = "IID_IDirectDrawSurface3";
        break;
    case 0x0B2B8630:
        version = 4;
        mode = "IID_IDirectDrawSurface4";
        break;
    case 0x06675a80:
        version = 7;
        mode = "IID_IDirectDrawSurface7";
        break;
    }
    if(GuidName) strcpy(GuidName, mode);
    return version;
}

char *sGUID(GUID *lpGUID) {
    static char s[81];
    char GuidName[41];
    switch ((DWORD)lpGUID) {
    case 0:
        return "(NULL)";
    case DDCREATE_EMULATIONONLY:
        return "DDCREATE_EMULATIONONLY";
        break;
    case DDCREATE_HARDWAREONLY:
        return "DDCREATE_HARDWAREONLY";
        break;
    default:
        sprintf(s, "%08x.%04x.%04x.%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x",
                lpGUID->Data1, lpGUID->Data2, lpGUID->Data3,
                lpGUID->Data4[0], lpGUID->Data4[1], lpGUID->Data4[2], lpGUID->Data4[3],
                lpGUID->Data4[4], lpGUID->Data4[5], lpGUID->Data4[6], lpGUID->Data4[7]);
    }
    strcpy(GuidName, "???");
    do { // fake loop
        if(GUIDSurfaceVersion(lpGUID, GuidName)) break;
        if(GUIDDDrawVersion(lpGUID, GuidName)) break;
    } while(FALSE);
    sprintf(s, "%s %s", s, GuidName);
    return s;
}