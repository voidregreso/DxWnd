#define DIRECTINPUT_VERSION 0x800
#define DIRECTINPUT8_VERSION 0x800
#define  _CRT_SECURE_NO_WARNINGS
#define INITGUID

//#define TRACEALL	// hooks unnecessary methods for logging

#include <windows.h>
#include <XInput.h>
#include "dxwnd.h"
#include "dxhook.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhelper.h"
#include "dxwjoy.h"
#include "stdio.h"

// legacy removed items ???
#ifndef XINPUT_KEYSTROKE
typedef struct _XINPUT_KEYSTROKE {
    WORD VirtualKey;
    WCHAR Unicode;
    WORD Flags;
    BYTE UserIndex;
    BYTE HidCode;
} XINPUT_KEYSTROKE, *PXINPUT_KEYSTROKE;
#endif


extern BOOL vjGetJoy(char *, LPJOYINFO);
extern DXWNDSTATUS *pStatus;
DWORD WINAPI emuXInputGetState(char *, DWORD, XINPUT_STATE *);
DWORD WINAPI emuXInputGetCapabilities(DWORD, DWORD, XINPUT_CAPABILITIES *);

static BOOL bXInputEnabled = TRUE;

typedef void  (WINAPI *XInputEnable_Type)(BOOL);
typedef DWORD (WINAPI *XInputGetCapabilities_Type)(DWORD, DWORD, XINPUT_CAPABILITIES *);
typedef DWORD (WINAPI *XInputGetKeystroke_Type)(DWORD, DWORD, XINPUT_KEYSTROKE *);
typedef DWORD (WINAPI *XInputGetState_Type)(DWORD, XINPUT_STATE *);
typedef DWORD (WINAPI *XInputSetState_Type)(DWORD, XINPUT_VIBRATION *);

void  WINAPI extXInputEnable(BOOL);
DWORD WINAPI extXInputGetCapabilities(DWORD, DWORD, XINPUT_CAPABILITIES *);
DWORD WINAPI extXInputGetKeystroke(DWORD, DWORD, XINPUT_KEYSTROKE *);
DWORD WINAPI extXInputGetState(DWORD, XINPUT_STATE *);
DWORD WINAPI extXInputSetState(DWORD, XINPUT_VIBRATION *);

XInputEnable_Type pXInputEnable;
XInputGetCapabilities_Type pXInputGetCapabilities;
XInputGetKeystroke_Type pXInputGetKeystroke;
XInputGetState_Type pXInputGetState;
XInputSetState_Type pXInputSetState;

static HookEntryEx_Type xiHooks[] = {
    {HOOK_HOT_CANDIDATE, 0x05, "XInputEnable", (FARPROC)NULL, (FARPROC *) &pXInputEnable, (FARPROC)extXInputEnable},
    {HOOK_HOT_CANDIDATE, 0x04, "XInputGetCapabilities", (FARPROC)NULL, (FARPROC *) &pXInputGetCapabilities, (FARPROC)extXInputGetCapabilities},
    {HOOK_HOT_CANDIDATE, 0x08, "XInputGetKeystroke", (FARPROC)NULL, (FARPROC *) &pXInputGetKeystroke, (FARPROC)extXInputGetKeystroke},
    {HOOK_HOT_CANDIDATE, 0x02, "XInputGetState", (FARPROC)NULL, (FARPROC *) &pXInputGetState, (FARPROC)extXInputGetState},
    {HOOK_HOT_CANDIDATE, 0x03, "XInputSetState", (FARPROC)NULL, (FARPROC *) &pXInputSetState, (FARPROC)extXInputSetState},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

void HookXInput11(HMODULE module) {
    if(!(dxw.dwFlags1 & HOOKXINPUT)) return;
    HookLibraryEx(module, xiHooks, "xinput1_1.dll");
}

void HookXInput12(HMODULE module) {
    if(!(dxw.dwFlags1 & HOOKXINPUT)) return;
    HookLibraryEx(module, xiHooks, "xinput1_2.dll");
}

void HookXInput13(HMODULE module) {
    if(!(dxw.dwFlags1 & HOOKXINPUT)) return;
    HookLibraryEx(module, xiHooks, "xinput1_3.dll");
}

void HookXInput14(HMODULE module) {
    if(!(dxw.dwFlags1 & HOOKXINPUT)) return;
    HookLibraryEx(module, xiHooks, "xinput1_4.dll");
}

void HookXInput910(HMODULE module) {
    if(!(dxw.dwFlags1 & HOOKXINPUT)) return;
    HookLibraryEx(module, xiHooks, "xinput9_1_0.dll");
}

FARPROC Remap_XInput_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if ((dxw.dwFlags1 & HOOKXINPUT) && (addr = RemapLibraryEx(proc, hModule, xiHooks))) return addr;
    return NULL;
}

// ==== wrappers =====

void WINAPI extXInputEnable(BOOL enable) {
    ApiName("XInputEnable");
    OutTraceIN("%s: enable=%#x\n", ApiRef, enable);
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) {
        bXInputEnabled = enable;
        return;
    }
    (*pXInputEnable)(enable);
}

// from https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinputgetcapabilities(v=vs.85).aspx
// Remarks
// Note  The legacy XINPUT 9.1.0 version (included in Windows Vista and later) always returned
// a fixed set of capabilities regardless of attached device.

DWORD WINAPI extXInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES *pCaps) {
    DWORD res;
    ApiName("XInputGetCapabilities");
    OutTraceIN("%s: idx=%d\n", ApiRef, dwUserIndex);
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) return emuXInputGetCapabilities(dwUserIndex, dwFlags, pCaps);
    res = (*pXInputGetCapabilities)(dwUserIndex, dwFlags, pCaps);
    OutTraceIN("%s: ret=%d\n", ApiRef, res);
    OutHexIN((LPBYTE)pCaps, sizeof(XINPUT_CAPABILITIES));
    return res;
}

DWORD WINAPI extXInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE *pKey) {
    DWORD res;
    ApiName("XInputGetKeystroke");
    OutTraceIN("%s: idx=%d resvd=%#x\n", ApiRef, dwUserIndex);
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) {
        if(dwUserIndex != 0) return ERROR_DEVICE_NOT_CONNECTED;
        // ???
        return ERROR_SUCCESS;
    }
    res = (*pXInputGetKeystroke)(dwUserIndex, dwReserved, pKey);
    OutTraceIN("%s: ret=%d\n", ApiRef, res);
    return res;
}

DWORD WINAPI extXInputGetState(DWORD dwUserIndex, XINPUT_STATE *pState) {
    DWORD res;
    ApiName("XInputGetState");
    OutTraceIN("%s: idx=%d\n", ApiRef, dwUserIndex);
    // Xbox gamepad emulation
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) return emuXInputGetState(ApiRef, dwUserIndex, pState);
    // proxy
    res = (*pXInputGetState)(dwUserIndex, pState);
    OutTraceIN("%s: ret=%d\n", ApiRef, res);
    return res;
}

DWORD WINAPI extXInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration) {
    DWORD res;
    ApiName("XInputSetState");
    OutTraceIN("%s: idx=%d motorspeed(L,R)=(%d,%d)\n",
               ApiRef,
               dwUserIndex,
               pVibration->wLeftMotorSpeed,
               pVibration->wRightMotorSpeed);
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) {
        if(dwUserIndex != 0) return ERROR_DEVICE_NOT_CONNECTED;
        return ERROR_SUCCESS;
    }
    res = (*pXInputSetState)(dwUserIndex, pVibration);
    OutTraceIN("%s: ret=%d\n", ApiRef, res);
    return res;
}

// === XInput XBOX 360 emulation

DWORD WINAPI emuXInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES *pCaps) {
    if(dwUserIndex != 0) return ERROR_DEVICE_NOT_CONNECTED;
    memset(pCaps, 0, sizeof(XINPUT_CAPABILITIES));
    pCaps->Type = XINPUT_DEVTYPE_GAMEPAD;
    pCaps->SubType = XINPUT_DEVSUBTYPE_GAMEPAD;
    //pCaps->Flags = XINPUT_CAPS_NO_NAVIGATION; // let's make it as poor as possible!
    return ERROR_SUCCESS;
}

DWORD WINAPI emuXInputGetState(ApiArg, DWORD dwUserIndex, XINPUT_STATE *pState) {
    static DWORD dwPacketNumber = 0;
    XINPUT_GAMEPAD *pGamepad;
    BOOL bActive;
    JOYINFO jinfo;
    SHORT ThumbX, ThumbY;
    if(dwUserIndex != 0) return ERROR_DEVICE_NOT_CONNECTED;
    pGamepad = &(pState->Gamepad);
    memset(pGamepad, 0, sizeof(XINPUT_GAMEPAD));
    if(!bXInputEnabled) {
        OutDebugIN("%s: disabled\n", ApiRef);
        pState->dwPacketNumber = dwPacketNumber;
        return ERROR_SUCCESS;
    }
    pState->dwPacketNumber = ++dwPacketNumber;
    bActive = vjGetJoy(ApiRef, &jinfo);
    pGamepad->wButtons = 0;
    // random choice ...
    //if(jinfo.wButtons & JOY_BUTTON1) pGamepad->wButtons |= XINPUT_GAMEPAD_DPAD_UP;
    //if(jinfo.wButtons & JOY_BUTTON2) pGamepad->wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
    //if(jinfo.wButtons & JOY_BUTTON3) pGamepad->wButtons |= XINPUT_GAMEPAD_DPAD_START;
    if(jinfo.wButtons & JOY_BUTTON1) pGamepad->bLeftTrigger = 255;
    if(jinfo.wButtons & JOY_BUTTON1) pGamepad->bRightTrigger = 255;
    ThumbX = ((jinfo.wXpos << 16) / DXWJOY_XSPAN) - (1 << 15);
    ThumbY = ((jinfo.wYpos << 16) / DXWJOY_XSPAN) - (1 << 15);
    switch(pStatus->VJoyStatus & (VJXLEFTONLY | VJXRIGHTONLY)) {
    case VJXLEFTONLY:
        pGamepad->sThumbLX = ThumbX;
        pGamepad->sThumbLY = ThumbY;
        pGamepad->sThumbRX = 0;
        pGamepad->sThumbRY = 0;
        break;
    case VJXRIGHTONLY:
        pGamepad->sThumbLX = 0;
        pGamepad->sThumbLY = 0;
        pGamepad->sThumbRX = ThumbX;
        pGamepad->sThumbRY = ThumbY;
        break;
    default:
        pGamepad->sThumbLX = ThumbX;
        pGamepad->sThumbLY = ThumbY;
        pGamepad->sThumbRX = ThumbX;
        pGamepad->sThumbRY = ThumbY;
        break;
    }
    OutDebugIN("%s: pos(x,y)=(%d,%d) trigger(L,R)=(%d,%d)\n",
               ApiRef, ThumbX, ThumbY,
               pGamepad->bLeftTrigger, pGamepad->bRightTrigger);
    return ERROR_SUCCESS;
}
