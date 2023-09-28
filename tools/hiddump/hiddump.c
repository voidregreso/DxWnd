#define  _CRT_SECURE_NO_WARNINGS
#include "windows.h"
//#include "hidpi.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"

extern BOOLEAN WINAPI HidD_GetProductString(
    HANDLE HidDeviceObject,
    PVOID  Buffer,
    ULONG  BufferLength
);

static char *sRIMType(DWORD type) {
#define RIM_TYPEBAD (RIM_TYPEHID+1)
    if(type > RIM_TYPEBAD) type = RIM_TYPEBAD;
    static char *sRIMTypes[] = {"MOUSE", "KEYBOARD", "HID", "invalid"};
    return sRIMTypes[type];
}

#define MAX_USBHID_STRING_LEN 126

int main(int argc, char *argv[]) {
    UINT nDevices;
    UINT cbSize;
    PRAWINPUTDEVICELIST pRawInputDeviceList, p;
    GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST));
    pRawInputDeviceList = malloc(sizeof(RAWINPUTDEVICELIST) * nDevices);
    GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST));
    printf("hid devices: %d\n", nDevices);
    p = pRawInputDeviceList;
    for (int i = 0; i < nDevices; i++, p++) {
        GetRawInputDeviceInfo(p->hDevice, RIDI_DEVICENAME, NULL, &cbSize);
        //WCHAR *sName = malloc(sizeof(WCHAR)*(cbSize+1));
        char *sName = malloc(cbSize + 1);
        GetRawInputDeviceInfo(p->hDevice, RIDI_DEVICENAME, sName, &cbSize);
        printf("device[%d]: \n> hid=%#x \n> type=%d(%s) \n> name=\"%s\"\n", i, p->hDevice, p->dwType, sRIMType(p->dwType), sName);
        free(sName);
        RID_DEVICE_INFO info;
        RID_DEVICE_INFO_HID *pHid;
        RID_DEVICE_INFO_KEYBOARD *pKbd;
        RID_DEVICE_INFO_MOUSE *pMouse;
        cbSize = sizeof(info);
        GetRawInputDeviceInfo(p->hDevice, RIDI_DEVICEINFO, &info, &cbSize);
        switch (p->dwType) {
        case RIM_TYPEMOUSE:
            pMouse = &(info.mouse);
            printf("> id=%#x\n", pMouse->dwId);
            printf("> nbuttons=%d\n", pMouse->dwNumberOfButtons);
            printf("> samplerate=%d\n", pMouse->dwSampleRate);
            printf("> haswheel=%s\n", pMouse->fHasHorizontalWheel ? "yes" : "no");
            break;
        case RIM_TYPEKEYBOARD:
            pKbd = &(info.keyboard);
            printf("> type=%#x\n", pKbd->dwType);
            printf("> subtype=%#x\n", pKbd->dwSubType);
            printf("> mode=%#x\n", pKbd->dwKeyboardMode);
            printf("> fkeys=%d\n", pKbd->dwNumberOfFunctionKeys);
            printf("> indicators=%d\n", pKbd->dwNumberOfIndicators);
            printf("> ktotal=%d\n", pKbd->dwNumberOfKeysTotal);
            break;
        case RIM_TYPEHID:
            pHid = &(info.hid);
            printf("> vendor=%#x\n", pHid->dwVendorId);
            printf("> product=%#x\n", pHid->dwProductId);
            printf("> version=%#x\n", pHid->dwVersionNumber);
            printf("> usagepage=%#x\n", pHid->usUsagePage);
            printf("> usage=%#x\n", pHid->usUsage);
            break;
        }
        //WCHAR ProductString[MAX_USBHID_STRING_LEN+1];
        //HidD_GetProductString(p->hDevice, ProductString, MAX_USBHID_STRING_LEN);
        //printf("> product=\"%ls\"\n", ProductString);
    }
    // after the job, free the RAWINPUTDEVICELIST
    free(pRawInputDeviceList);
}