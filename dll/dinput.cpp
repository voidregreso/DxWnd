#define DIRECTINPUT_VERSION 0x800
#define DIRECTINPUT8_VERSION 0x800
#define  _CRT_SECURE_NO_WARNINGS
#define INITGUID

#define RECOVERINPUTLOST TRUE // to become a flag?
//#define RECOVERDATAFORMAT TRUE // to become a flag?

//#define TRACEALL	// hooks unnecessary methods for logging

#include <windows.h>
#include <dinput.h>
#include "dxwnd.h"
#include "dxhook.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhelper.h"
#include "comvjoy.h"
#include "stdio.h"

extern BOOL WINAPI extGetCursorPos(LPPOINT);

#ifndef OLDDIRECTINPUTVERSION
#define OLDDIRECTINPUTVERSION 0x8007047e
#endif

#define ISASCII TRUE
#define ISWIDEC FALSE

/*
    DEFINE_GUID(IID_IDirectInputA,     0x89521360,0xAA8A,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00 ,0x00);
    DEFINE_GUID(IID_IDirectInputW,     0x89521361,0xAA8A,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    DEFINE_GUID(IID_IDirectInput2A,    0x5944E662,0xAA8A,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    DEFINE_GUID(IID_IDirectInput2W,    0x5944E663,0xAA8A,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    DEFINE_GUID(IID_IDirectInput7A,    0x9A4CB684,0x236D,0x11D3,0x8E,0x9D,0x00,0xC0,0x4F,0x68,0x44,0xAE);
    DEFINE_GUID(IID_IDirectInput7W,    0x9A4CB685,0x236D,0x11D3,0x8E,0x9D,0x00,0xC0,0x4F,0x68,0x44,0xAE);
    DEFINE_GUID(IID_IDirectInput8A,    0xBF798030,0x483A,0x4DA2,0xAA,0x99,0x5D,0x64,0xED,0x36,0x97,0x00);
    DEFINE_GUID(IID_IDirectInput8W,    0xBF798031,0x483A,0x4DA2,0xAA,0x99,0x5D,0x64,0xED,0x36,0x97,0x00);
    DEFINE_GUID(IID_IDirectInputDeviceA, 0x5944E680,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    DEFINE_GUID(IID_IDirectInputDeviceW, 0x5944E681,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    DEFINE_GUID(IID_IDirectInputDevice2A,0x5944E682,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    DEFINE_GUID(IID_IDirectInputDevice2W,0x5944E683,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    DEFINE_GUID(IID_IDirectInputDevice7A,0x57D7C6BC,0x2356,0x11D3,0x8E,0x9D,0x00,0xC0,0x4F,0x68,0x44,0xAE);
    DEFINE_GUID(IID_IDirectInputDevice7W,0x57D7C6BD,0x2356,0x11D3,0x8E,0x9D,0x00,0xC0,0x4F,0x68,0x44,0xAE);
    DEFINE_GUID(IID_IDirectInputDevice8A,0x54D41080,0xDC15,0x4833,0xA4,0x1B,0x74,0x8F,0x73,0xA3,0x81,0x79);
    DEFINE_GUID(IID_IDirectInputDevice8W,0x54D41081,0xDC15,0x4833,0xA4,0x1B,0x74,0x8F,0x73,0xA3,0x81,0x79);
    DEFINE_GUID(IID_IDirectInputEffect,  0xE7E1F7C0,0x88D2,0x11D0,0x9A,0xD0,0x00,0xA0,0xC9,0xA0,0x6E,0x35);
*/
//const GUID FAR IID_IDirectInput8A = { 0xBF798030,0x483A,0x4DA2,0xAA,0x99,0x5D,0x64,0xED,0x36,0x97,0x00 };
//const GUID FAR IID_IDirectInput8W = { 0xBF798031,0x483A,0x4DA2,0xAA,0x99,0x5D,0x64,0xED,0x36,0x97,0x00 };

// DirectInput API

typedef HRESULT (WINAPI *DirectInputCreateA_Type)(HINSTANCE, DWORD, LPDIRECTINPUTA *, LPUNKNOWN);
typedef HRESULT (WINAPI *DirectInputCreateW_Type)(HINSTANCE, DWORD, LPDIRECTINPUTW *, LPUNKNOWN);
typedef HRESULT (WINAPI *DirectInputCreateEx_Type)(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);

// IDirectInput methods (A/W, 1/8)

typedef HRESULT (WINAPI *DICreateDeviceA_Type)(LPDIRECTINPUTA, REFGUID, LPDIRECTINPUTDEVICEA *, LPUNKNOWN);
typedef HRESULT (WINAPI *DICreateDeviceW_Type)(LPDIRECTINPUTW, REFGUID, LPDIRECTINPUTDEVICEW *, LPUNKNOWN);
typedef HRESULT (WINAPI *DICreateDeviceExA_Type)(LPDIRECTINPUTA, REFGUID, REFIID, LPVOID *, LPUNKNOWN);
typedef HRESULT (WINAPI *DICreateDeviceExW_Type)(LPDIRECTINPUTW, REFGUID, REFIID, LPVOID *, LPUNKNOWN);
typedef HRESULT (WINAPI *DIEnumDevicesA_Type)(void *, DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD);
typedef HRESULT (WINAPI *QueryInterface_Type)(void *, REFIID, LPVOID *);
typedef HRESULT (WINAPI *GetDeviceStatus8_Type)(void *, REFGUID);
#ifdef TRACEALL
typedef HRESULT (WINAPI *Initialize8_Type)(void *, HINSTANCE, DWORD);
typedef HRESULT (WINAPI *ConfigureDevices_Type)(void *, LPDICONFIGUREDEVICESCALLBACK, LPDICONFIGUREDEVICESPARAMS, DWORD, LPVOID);
#endif // TRACEALL

// IDirectInputDevice methods (A/W, 1/8)

typedef HRESULT (WINAPI *DIRelease_Type)(void *);
typedef HRESULT (WINAPI *GetDeviceData_Type)(LPDIRECTINPUTDEVICE, DWORD, LPVOID, LPDWORD, DWORD);
typedef HRESULT (WINAPI *GetDeviceState_Type)(LPDIRECTINPUTDEVICE, DWORD, LPDIMOUSESTATE);
typedef HRESULT (WINAPI *DISetCooperativeLevel_Type)(LPDIRECTINPUTDEVICE, HWND, DWORD);
typedef HRESULT (WINAPI *SetDataFormat_Type)(LPDIRECTINPUTDEVICE, LPCDIDATAFORMAT);
typedef HRESULT (WINAPI *DIEnumDevicesW_Type)(void *, DWORD, LPDIENUMDEVICESCALLBACKW, LPVOID, DWORD);
typedef HRESULT (WINAPI *Acquire_Type)(LPDIRECTINPUTDEVICE);
typedef HRESULT (WINAPI *Unacquire_Type)(LPDIRECTINPUTDEVICE);
typedef HRESULT (WINAPI *DirectInput8Create_Type)(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);
typedef HRESULT (WINAPI *EnumObjectsA_Type)(LPDIRECTINPUTDEVICE, LPDIENUMDEVICEOBJECTSCALLBACKA, LPVOID, DWORD);
typedef HRESULT (WINAPI *EnumObjectsW_Type)(LPDIRECTINPUTDEVICE, LPDIENUMDEVICEOBJECTSCALLBACKW, LPVOID, DWORD);
#ifdef TRACEALL
typedef HRESULT (WINAPI *GetCapabilities_Type)(LPDIRECTINPUTDEVICE, LPDIDEVCAPS);
typedef HRESULT (WINAPI *SetEventNotification_Type)(LPDIRECTINPUTDEVICE, HANDLE);
typedef HRESULT (WINAPI *SetProperty_Type)(LPDIRECTINPUTDEVICE, REFGUID, LPCDIPROPHEADER);
typedef HRESULT (WINAPI *GetProperty_Type)(LPDIRECTINPUTDEVICE, REFGUID, LPDIPROPHEADER);
typedef HRESULT (WINAPI *CreateEffect_Type)(LPDIRECTINPUTDEVICE, REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT *, LPUNKNOWN);
typedef HRESULT (WINAPI *Poll_Type)(LPDIRECTINPUTDEVICE);
typedef HRESULT (WINAPI *GetObjectInfoA_Type)(LPDIRECTINPUTDEVICE, LPDIDEVICEOBJECTINSTANCEA, DWORD, DWORD);
typedef HRESULT (WINAPI *GetObjectInfoW_Type)(LPDIRECTINPUTDEVICE, LPDIDEVICEOBJECTINSTANCEW, DWORD, DWORD);
typedef HRESULT (WINAPI *GetDeviceInfoA_Type)(LPDIRECTINPUTDEVICE, LPDIDEVICEINSTANCEA);
typedef HRESULT (WINAPI *GetDeviceInfoW_Type)(LPDIRECTINPUTDEVICE, LPDIDEVICEINSTANCEW);
typedef HRESULT (WINAPI *RunControlPanel_Type)(LPDIRECTINPUTDEVICE, HWND, DWORD);
typedef HRESULT (WINAPI *Initialize_Type)(LPDIRECTINPUTDEVICE, HINSTANCE, DWORD, REFGUID);
typedef HRESULT (WINAPI *DIFindDeviceA8_Type)(void *, REFGUID, LPCSTR, LPGUID);
typedef HRESULT (WINAPI *EnumDevicesBySemantics_Type)(void *, LPCTSTR, LPDIACTIONFORMAT, LPDIENUMDEVICESBYSEMANTICSCB, LPVOID, DWORD);
typedef HRESULT (WINAPI *BuildActionMapA_Type)(void *, LPDIACTIONFORMATA, LPCSTR, DWORD);
typedef HRESULT (WINAPI *SetActionMapA_Type)(void *, LPDIACTIONFORMATA, LPCSTR, DWORD);
#endif

// DirectInput API

HRESULT WINAPI extDirectInputCreateA(HINSTANCE, DWORD, LPDIRECTINPUTA *, LPUNKNOWN);
HRESULT WINAPI extDirectInputCreateW(HINSTANCE, DWORD, LPDIRECTINPUTW *, LPUNKNOWN);
HRESULT WINAPI extDirectInputCreateEx(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);
HRESULT WINAPI extDirectInput8Create(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);

// IDirectInput methods (A/W, 1/8)

HRESULT WINAPI extDICreateDeviceA1(LPDIRECTINPUTA, REFGUID, LPDIRECTINPUTDEVICEA *, LPUNKNOWN);
HRESULT WINAPI extDICreateDeviceA2(LPDIRECTINPUTA, REFGUID, LPDIRECTINPUTDEVICEA *, LPUNKNOWN);
HRESULT WINAPI extDICreateDeviceA7(LPDIRECTINPUTA, REFGUID, LPDIRECTINPUTDEVICEA *, LPUNKNOWN);
HRESULT WINAPI extDICreateDeviceA8(LPDIRECTINPUTA, REFGUID, LPDIRECTINPUTDEVICEA *, LPUNKNOWN);
HRESULT WINAPI extDICreateDeviceW1(LPDIRECTINPUTW, REFGUID, LPDIRECTINPUTDEVICEW *, LPUNKNOWN);
HRESULT WINAPI extDICreateDeviceW2(LPDIRECTINPUTW, REFGUID, LPDIRECTINPUTDEVICEW *, LPUNKNOWN);
HRESULT WINAPI extDICreateDeviceW7(LPDIRECTINPUTW, REFGUID, LPDIRECTINPUTDEVICEW *, LPUNKNOWN);
HRESULT WINAPI extDICreateDeviceW8(LPDIRECTINPUTW, REFGUID, LPDIRECTINPUTDEVICEW *, LPUNKNOWN);
HRESULT WINAPI extDICreateDeviceExA(LPDIRECTINPUTA, REFGUID, REFIID, LPVOID *, LPUNKNOWN);
HRESULT WINAPI extDICreateDeviceExW(LPDIRECTINPUTW, REFGUID, REFIID, LPVOID *, LPUNKNOWN);
HRESULT WINAPI extDIEnumDevicesA1(void *, DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD);
HRESULT WINAPI extDIEnumDevicesA2(void *, DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD);
HRESULT WINAPI extDIEnumDevicesA7(void *, DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD);
HRESULT WINAPI extDIEnumDevicesA8(void *, DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD);
HRESULT WINAPI extDIEnumDevicesW1(void *, DWORD, LPDIENUMDEVICESCALLBACKW, LPVOID, DWORD);
HRESULT WINAPI extDIEnumDevicesW2(void *, DWORD, LPDIENUMDEVICESCALLBACKW, LPVOID, DWORD);
HRESULT WINAPI extDIEnumDevicesW7(void *, DWORD, LPDIENUMDEVICESCALLBACKW, LPVOID, DWORD);
HRESULT WINAPI extDIEnumDevicesW8(void *, DWORD, LPDIENUMDEVICESCALLBACKW, LPVOID, DWORD);
HRESULT WINAPI extDIQueryInterface(void *, REFIID, LPVOID *);
HRESULT WINAPI extDIGetDeviceStatus8(void *, REFGUID);
#ifdef TRACEALL
HRESULT WINAPI extDIInitialize8(void *, HINSTANCE, DWORD);
HRESULT WINAPI extConfigureDevicesA8(void *, LPDICONFIGUREDEVICESCALLBACK, LPDICONFIGUREDEVICESPARAMS, DWORD, LPVOID);
#endif // TRACEALL

// IDirectInputDevice methods (A/W, 1/8)

HRESULT WINAPI extGetDeviceData(LPDIRECTINPUTDEVICE, DWORD, LPVOID, LPDWORD, DWORD);
HRESULT WINAPI extGetDeviceState(LPDIRECTINPUTDEVICE, DWORD, LPDIMOUSESTATE);
HRESULT WINAPI extDISetCooperativeLevel(LPDIRECTINPUTDEVICE, HWND, DWORD);
HRESULT WINAPI extSetDataFormat(LPDIRECTINPUTDEVICE, LPCDIDATAFORMAT);
HRESULT WINAPI extDIRelease(void *);
HRESULT WINAPI extAcquire(LPDIRECTINPUTDEVICE);
HRESULT WINAPI extUnacquire(LPDIRECTINPUTDEVICE);
HRESULT WINAPI extDirectInput8Create(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);
HRESULT WINAPI extEnumObjectsA(LPDIRECTINPUTDEVICE, LPDIENUMDEVICEOBJECTSCALLBACKA, LPVOID, DWORD);
HRESULT WINAPI extEnumObjectsW(LPDIRECTINPUTDEVICE, LPDIENUMDEVICEOBJECTSCALLBACKW, LPVOID, DWORD);
#ifdef TRACEALL
HRESULT WINAPI extGetCapabilities(LPDIRECTINPUTDEVICE, LPDIDEVCAPS);
HRESULT WINAPI extSetEventNotification(LPDIRECTINPUTDEVICE, HANDLE);
HRESULT WINAPI extSetProperty(LPDIRECTINPUTDEVICE, REFGUID, LPCDIPROPHEADER);
HRESULT WINAPI extGetProperty(LPDIRECTINPUTDEVICE, REFGUID, LPDIPROPHEADER);
HRESULT WINAPI extCreateEffect(LPDIRECTINPUTDEVICE, REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT *, LPUNKNOWN);
HRESULT WINAPI extPoll(LPDIRECTINPUTDEVICE);
HRESULT WINAPI extGetObjectInfoA(LPDIRECTINPUTDEVICE, LPDIDEVICEOBJECTINSTANCEA, DWORD, DWORD);
HRESULT WINAPI extGetObjectInfoW(LPDIRECTINPUTDEVICE, LPDIDEVICEOBJECTINSTANCEW, DWORD, DWORD);
HRESULT WINAPI extGetDeviceInfoA(LPDIRECTINPUTDEVICE, LPDIDEVICEINSTANCEA);
HRESULT WINAPI extGetDeviceInfoW(LPDIRECTINPUTDEVICE, LPDIDEVICEINSTANCEW);
HRESULT WINAPI extRunControlPanel(LPDIRECTINPUTDEVICE, HWND, DWORD);
HRESULT WINAPI extInitialize(LPDIRECTINPUTDEVICE, HINSTANCE, DWORD, REFGUID);
HRESULT WINAPI extDIFindDeviceA8(void *, REFGUID, LPCSTR, LPGUID);
HRESULT WINAPI extEnumDevicesBySemantics(void *, LPCTSTR, LPDIACTIONFORMAT, LPDIENUMDEVICESBYSEMANTICSCB, LPVOID, DWORD);
HRESULT WINAPI extBuildActionMapA(void *, LPDIACTIONFORMATA, LPCSTR, DWORD);
HRESULT WINAPI extSetActionMapA(void *, LPDIACTIONFORMATA, LPCSTR, DWORD);
#endif

// DirectInput API function pointers

DirectInputCreateA_Type pDirectInputCreateA = NULL;
DirectInputCreateW_Type pDirectInputCreateW = NULL;
DirectInputCreateEx_Type pDirectInputCreateEx;
DirectInput8Create_Type pDirectInput8Create = NULL;

// IDirectInput methods function pointers

DICreateDeviceA_Type pDICreateDeviceA1, pDICreateDeviceA2, pDICreateDeviceA7, pDICreateDeviceA8;
DICreateDeviceW_Type pDICreateDeviceW1, pDICreateDeviceW2, pDICreateDeviceW7, pDICreateDeviceW8;
DICreateDeviceExA_Type pDICreateDeviceExA;
DICreateDeviceExW_Type pDICreateDeviceExW;
DIEnumDevicesA_Type pDIEnumDevicesA1, pDIEnumDevicesA2, pDIEnumDevicesA7, pDIEnumDevicesA8;
DIEnumDevicesW_Type pDIEnumDevicesW1, pDIEnumDevicesW2, pDIEnumDevicesW7, pDIEnumDevicesW8;
QueryInterface_Type pDIQueryInterface = NULL;
GetDeviceStatus8_Type pDIGetDeviceStatus8;
#ifdef TRACEALL
Initialize8_Type pDIInitialize8;
ConfigureDevices_Type pConfigureDevicesA8;
#endif // TRACEALL

// IDirectInputDevice methods function pointers

GetDeviceData_Type pGetDeviceData = NULL;
GetDeviceState_Type pGetDeviceState = NULL;
DISetCooperativeLevel_Type pDISetCooperativeLevel = NULL;
SetDataFormat_Type pSetDataFormat = NULL;
Acquire_Type pAcquire = NULL;
Unacquire_Type pUnacquire = NULL;
DIRelease_Type pDIRelease;
EnumObjectsA_Type pEnumObjectsA = NULL;
EnumObjectsW_Type pEnumObjectsW = NULL;
#ifdef TRACEALL
GetCapabilities_Type pGetCapabilities = NULL;
SetEventNotification_Type pSetEventNotification = NULL;
SetProperty_Type pSetProperty = NULL;
GetProperty_Type pGetProperty = NULL;
CreateEffect_Type pCreateEffect = NULL;
Poll_Type pPoll = NULL;
GetObjectInfoA_Type pGetObjectInfoA = NULL;
GetObjectInfoW_Type pGetObjectInfoW = NULL;
GetDeviceInfoA_Type pGetDeviceInfoA = NULL;
GetDeviceInfoW_Type pGetDeviceInfoW = NULL;
RunControlPanel_Type pRunControlPanel;
Initialize_Type pInitialize;
DIFindDeviceA8_Type pDIFindDeviceA8 = NULL;
EnumDevicesBySemantics_Type pEnumDevicesBySemantics = NULL;
BuildActionMapA_Type pBuildActionMapA;
SetActionMapA_Type pSetActionMapA;
#endif

// DLL hook tables

static HookEntryEx_Type diHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "DirectInputCreateA", (FARPROC)NULL, (FARPROC *) &pDirectInputCreateA, (FARPROC)extDirectInputCreateA},
    {HOOK_HOT_CANDIDATE, 0, "DirectInputCreateW", (FARPROC)NULL, (FARPROC *) &pDirectInputCreateW, (FARPROC)extDirectInputCreateW},
    {HOOK_HOT_CANDIDATE, 0, "DirectInputCreateEx", (FARPROC)NULL, (FARPROC *) &pDirectInputCreateEx, (FARPROC)extDirectInputCreateEx},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type di8Hooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "DirectInput8Create", (FARPROC)NULL, (FARPROC *) &pDirectInput8Create, (FARPROC)extDirectInput8Create},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

// API hook functions

void HookDirectInput(HMODULE module) {
    const GUID di7 = {0x9A4CB684, 0x236D, 0x11D3, 0x8E, 0x9D, 0x00, 0xC0, 0x4F, 0x68, 0x44, 0xAE};
    HINSTANCE hinst;
    LPDIRECTINPUT lpdi;
    if(!(dxw.dwFlags1 & HOOKDI)) return;
    HookLibraryEx(module, diHooks, "dinput.dll");
    if(!pDirectInputCreateA && !pDirectInputCreateW && !pDirectInputCreateEx) {
        hinst = LoadLibrary("dinput.dll");
        if(!hinst) {
            OutTraceE("LoadLibrary dinput.dll ERROR err=%d at %d\n", GetLastError(), __LINE__);
            return;
        }
        pDirectInputCreateA = (DirectInputCreateA_Type)GetProcAddress(hinst, "DirectInputCreateA");
        if(pDirectInputCreateA)
            if(!extDirectInputCreateA(GetModuleHandle(0), DIRECTINPUT_VERSION,
                                      &lpdi, 0)) lpdi->Release();
        pDirectInputCreateEx = (DirectInputCreateEx_Type)GetProcAddress(hinst, "DirectInputCreateEx");
        if(pDirectInputCreateEx)
            if(!extDirectInputCreateEx(GetModuleHandle(0), DIRECTINPUT_VERSION,
                                       di7, (void **)&lpdi, 0)) lpdi->Release();
    }
}

void HookDirectInput8(HMODULE module) {
    const GUID di8 = {0xBF798030, 0x483A, 0x4DA2, 0xAA, 0x99, 0x5D, 0x64, 0xED, 0x36, 0x97, 0x00};
    HINSTANCE hinst;
    LPDIRECTINPUT lpdi;
    if(!(dxw.dwFlags1 & HOOKDI8)) return;
    HookLibraryEx(module, di8Hooks, "dinput8.dll");
    if(!pDirectInput8Create) {
        hinst = LoadLibrary("dinput8.dll");
        if(!hinst) {
            OutTraceE("LoadLibrary dinput8.dll ERROR err=%d at %d\n", GetLastError(), __LINE__);
            return;
        }
        pDirectInput8Create = (DirectInput8Create_Type)GetProcAddress(hinst, "DirectInput8Create");
        if(pDirectInput8Create)
            if(!extDirectInput8Create(GetModuleHandle(0), DIRECTINPUT8_VERSION,
                                      di8, (LPVOID *)&lpdi, 0)) lpdi->Release();
    }
}

FARPROC Remap_DInput_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if ((dxw.dwFlags1 & HOOKDI) && (addr = RemapLibraryEx(proc, hModule, diHooks))) return addr;
    return NULL;
}

FARPROC Remap_DInput8_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if ((dxw.dwFlags1 & HOOKDI8) && (addr = RemapLibraryEx(proc, hModule, di8Hooks))) return addr;
    return NULL;
}

// COM hoookers

void HookDirectInputSession(void **lplpdi, int iVersion, BOOL isAscii) {
    if(isAscii) {
        switch(iVersion) {
        case 1:
            SetHook((void *)(**(DWORD **)lplpdi + 12), extDICreateDeviceA1, (void **)&pDICreateDeviceA1, "CreateDevice(IA1)");
            SetHook((void *)(**(DWORD **)lplpdi + 16), extDIEnumDevicesA1, (void **)&pDIEnumDevicesA1, "EnumDevices(IA1)");
            break;
        case 2:
            SetHook((void *)(**(DWORD **)lplpdi + 12), extDICreateDeviceA2, (void **)&pDICreateDeviceA2, "CreateDevice(IA2)");
            SetHook((void *)(**(DWORD **)lplpdi + 16), extDIEnumDevicesA2, (void **)&pDIEnumDevicesA2, "EnumDevices(IA2)");
            break;
        case 7:
            SetHook((void *)(**(DWORD **)lplpdi + 12), extDICreateDeviceA7, (void **)&pDICreateDeviceA7, "CreateDevice(IA7)");
            SetHook((void *)(**(DWORD **)lplpdi + 16), extDIEnumDevicesA7, (void **)&pDIEnumDevicesA7, "EnumDevices(IA7)");
            SetHook((void *)(**(DWORD **)lplpdi + 36), extDICreateDeviceExA, (void **)&pDICreateDeviceExA, "CreateDeviceEx(IA7)");
            break;
        case 8:
            SetHook((void *)(**(DWORD **)lplpdi + 12), extDICreateDeviceA8, (void **)&pDICreateDeviceA8, "CreateDevice(IA8)");
            SetHook((void *)(**(DWORD **)lplpdi + 16), extDIEnumDevicesA8, (void **)&pDIEnumDevicesA8, "EnumDevices(IA8)");
            SetHook((void *)(**(DWORD **)lplpdi + 20), extDIGetDeviceStatus8, (void **)&pDIGetDeviceStatus8, "GetDeviceStatus(IA8)");
#ifdef TRACEALL
            //SetHook((void *)(**(DWORD **)lplpdi + 24), extDIRunControlPanel8, (void **)&pDIRunControlPanel8, "RunControlPanel(IA8)");
            SetHook((void *)(**(DWORD **)lplpdi + 28), extDIInitialize8, (void **)&pDIInitialize8, "Initialize(IA8)");
            //SetHook((void *)(**(DWORD **)lplpdi + 32), extDIFindDeviceA8, (void **)&pDIFindDeviceA8, "FindDevice(IA8)");
            SetHook((void *)(**(DWORD **)lplpdi + 36), extEnumDevicesBySemantics, (void **)&pEnumDevicesBySemantics, "EnumDevicesBySemantics(I8)");
            SetHook((void *)(**(DWORD **)lplpdi + 40), extConfigureDevicesA8, (void **)&pConfigureDevicesA8, "ConfigureDevices(I8)");
#endif // TRACEALL
            break;
        }
    } else {
        switch(iVersion) {
        case 1:
            SetHook((void *)(**(DWORD **)lplpdi + 12), extDICreateDeviceW1, (void **)&pDICreateDeviceW1, "CreateDevice(IW1)");
            SetHook((void *)(**(DWORD **)lplpdi + 16), extDIEnumDevicesW1, (void **)&pDIEnumDevicesW1, "EnumDevices(IW1)");
            break;
        case 2:
            SetHook((void *)(**(DWORD **)lplpdi + 12), extDICreateDeviceW2, (void **)&pDICreateDeviceW2, "CreateDevice(IW2)");
            SetHook((void *)(**(DWORD **)lplpdi + 16), extDIEnumDevicesW2, (void **)&pDIEnumDevicesW2, "EnumDevices(IW2)");
            break;
        case 7:
            SetHook((void *)(**(DWORD **)lplpdi + 12), extDICreateDeviceW7, (void **)&pDICreateDeviceW7, "CreateDevice(IW7)");
            SetHook((void *)(**(DWORD **)lplpdi + 16), extDIEnumDevicesW7, (void **)&pDIEnumDevicesW7, "EnumDevices(IW7)");
            SetHook((void *)(**(DWORD **)lplpdi + 36), extDICreateDeviceExW, (void **)&pDICreateDeviceExW, "CreateDeviceEx(IW7)");
            break;
        case 8:
            SetHook((void *)(**(DWORD **)lplpdi + 12), extDICreateDeviceW8, (void **)&pDICreateDeviceW8, "CreateDevice(IW8)");
            SetHook((void *)(**(DWORD **)lplpdi + 16), extDIEnumDevicesW8, (void **)&pDIEnumDevicesW8, "EnumDevices(IW8)");
            //SetHook((void *)(**(DWORD **)lplpdi + 32), extDIFindDeviceW8, (void **)&pDIFindDeviceW8, "FindDevice(IW8)");
            //SetHook((void *)(**(DWORD **)lplpdi + 36), extEnumDevicesBySemantics, (void **)&pEnumDevicesBySemantics, "EnumDevicesBySemantics(I8)");
            break;
        }
    }
}

#ifdef TRACEALL
#define TRACEALLFLAG TRUE
#else
#define TRACEALLFLAG FALSE
#endif

void HookDirectInputDevice(void **lplpdid, int iObjectVersion, BOOL isAscii) {
    SetHook((void *)(**(DWORD **)lplpdid), extDIQueryInterface, (void **)&pDIQueryInterface, "QueryInterface(I)");
    SetHook((void *)(**(DWORD **)lplpdid +  8), extDIRelease, (void **)&pDIRelease, "Release(I)");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)lplpdid + 12), extGetCapabilities, (void **)&pGetCapabilities, "GetCapabilities(I)");
#endif
    if((dxw.dwFlags11 & SETUSKEYDESCR) || TRACEALLFLAG) {
        if(isAscii)
            SetHook((void *)(**(DWORD **)lplpdid + 16), extEnumObjectsA, (void **)&pEnumObjectsA, "EnumObjects(I)");
        else
            SetHook((void *)(**(DWORD **)lplpdid + 16), extEnumObjectsW, (void **)&pEnumObjectsW, "EnumObjects(I)");
    }
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)lplpdid + 20), extGetProperty, (void **)&pGetProperty, "GetProperty(I)");
    SetHook((void *)(**(DWORD **)lplpdid + 24), extSetProperty, (void **)&pSetProperty, "SetProperty(I)");
#endif // TRACEALL
    SetHook((void *)(**(DWORD **)lplpdid + 28), extAcquire, (void **)&pAcquire, "Acquire(I)");
    SetHook((void *)(**(DWORD **)lplpdid + 32), extUnacquire, (void **)&pUnacquire, "Unacquire(I)");
    SetHook((void *)(**(DWORD **)lplpdid + 36), extGetDeviceState, (void **)&pGetDeviceState, "GetDeviceState(I)");
    SetHook((void *)(**(DWORD **)lplpdid + 40), extGetDeviceData, (void **)&pGetDeviceData, "GetDeviceData(I)");
    SetHook((void *)(**(DWORD **)lplpdid + 44), extSetDataFormat, (void **)&pSetDataFormat, "SetDataFormat(I)");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)lplpdid + 48), extSetEventNotification, (void **)&pSetEventNotification, "SetEventNotification(I)");
#endif // TRACEALL
    SetHook((void *)(**(DWORD **)lplpdid + 52), extDISetCooperativeLevel, (void **)&pDISetCooperativeLevel, "SetCooperativeLevel(I)");
#ifdef TRACEALL
    if(isAscii) {
        SetHook((void *)(**(DWORD **)lplpdid + 56), extGetObjectInfoA, (void **)&pGetObjectInfoA, "GetObjectInfo(I)");
        SetHook((void *)(**(DWORD **)lplpdid + 60), extGetDeviceInfoA, (void **)&pGetDeviceInfoA, "GetDeviceInfo(I)");
    } else {
        SetHook((void *)(**(DWORD **)lplpdid + 56), extGetObjectInfoW, (void **)&pGetObjectInfoW, "GetObjectInfo(I)");
        SetHook((void *)(**(DWORD **)lplpdid + 60), extGetDeviceInfoW, (void **)&pGetDeviceInfoW, "GetDeviceInfo(I)");
    }
    SetHook((void *)(**(DWORD **)lplpdid + 64), extRunControlPanel, (void **)&pRunControlPanel, "RunControlPanel(I)");
    SetHook((void *)(**(DWORD **)lplpdid + 68), extInitialize, (void **)&pInitialize, "Initialize(I)");
    if(iObjectVersion >= 2) {
        SetHook((void *)(**(DWORD **)lplpdid + 72), extCreateEffect, (void **)&pCreateEffect, "CreateEffect(I)");
        SetHook((void *)(**(DWORD **)lplpdid + 100), extPoll, (void **)&pPoll, "Poll(I)");
        //SetHook((void *)(**(DWORD **)lplpdid +104), extSendDeviceData, (void **)&pSendDeviceData, "SendDeviceData(I)");
        //SetHook((void *)(**(DWORD **)lplpdid +108), extEnumEffectsInFile, (void **)&pEnumEffectsInFile, "EnumEffectsInFile(I)");
        //SetHook((void *)(**(DWORD **)lplpdid +112), extWriteEffectToFile, (void **)&pWriteEffectToFile, "WriteEffectToFile(I)");
        SetHook((void *)(**(DWORD **)lplpdid + 116), extBuildActionMapA, (void **)&pBuildActionMapA, "BuildActionMap(I)");
        SetHook((void *)(**(DWORD **)lplpdid + 120), extSetActionMapA, (void **)&pSetActionMapA, "SetActionMap(I)");
        //SetHook((void *)(**(DWORD **)lplpdid +124), extGetImageInfo, (void **)&pGetImageInfo, "GetImageInfo(I)");
    }
#endif // TRACEALL
}

void GetMousePosition(int *, int *);
void InitPosition(int, int, int, int, int, int);

int iCursorX;
int iCursorY;
int iCursorXBuf;
int iCursorYBuf;
int iCurMinX;
int iCurMinY;
int iCurMaxX;
int iCurMaxY;

LPDIRECTINPUTDEVICE lpDIDDevice  = NULL;
LPDIRECTINPUTDEVICE lpDIDKeyboard = NULL;
LPDIRECTINPUTDEVICE lpDIDSysMouse = NULL;
LPDIRECTINPUTDEVICE lpDIDJoystick = NULL;

#ifndef DXW_NOTRACES
static char *sDevice(LPDIRECTINPUTDEVICE lpdid) {
    char *ret;
    ret = NULL;
    if(lpdid == lpDIDDevice) ret = "Device";
    if(lpdid == lpDIDKeyboard) ret = "Keyboard";
    if(lpdid == lpDIDSysMouse) ret = "Mouse";
    if(lpdid == lpDIDJoystick) ret = "Joystick";
    if (ret)
        return ret;
    else
        return (lpdid ? "unknown" : "NULL");
}

char *sGUIDProperty(REFGUID p) {
    char *s;
    DWORD prop = (DWORD)&p;
    switch(prop) {
    case &DIPROP_BUFFERSIZE:
        s = "BUFFERSIZE";
        break;
    case &DIPROP_AXISMODE:
        s = "AXISMODE";
        break;
    case &DIPROP_GRANULARITY:
        s = "GRANULARITY";
        break;
    case &DIPROP_RANGE:
        s = "RANGE";
        break;
    case &DIPROP_DEADZONE:
        s = "DEADZONE";
        break;
    case &DIPROP_SATURATION:
        s = "SATURATION";
        break;
    case &DIPROP_FFGAIN:
        s = "FFGAIN";
        break;
    case &DIPROP_FFLOAD:
        s = "FFLOAD";
        break;
    case &DIPROP_AUTOCENTER:
        s = "AUTOCENTER";
        break;
    case &DIPROP_CALIBRATIONMODE:
        s = "CALIBRATIONMODE";
        break;
    case &DIPROP_CALIBRATION:
        s = "CALIBRATION";
        break;
    case &DIPROP_GUIDANDPATH:
        s = "GUIDANDPATH";
        break;
    case &DIPROP_INSTANCENAME:
        s = "INSTANCENAME";
        break;
    case &DIPROP_PRODUCTNAME:
        s = "PRODUCTNAME";
        break;
    case &DIPROP_JOYSTICKID:
        s = "JOYSTICKID";
        break;
    case &DIPROP_GETPORTDISPLAYNAME:
        s = "GETPORTDISPLAYNAME";
        break;
    case &DIPROP_PHYSICALRANGE:
        s = "PHYSICALRANGE";
        break;
    case &DIPROP_LOGICALRANGE:
        s = "LOGICALRANGE";
        break;
    case &DIPROP_KEYNAME:
        s = "KEYNAME";
        break;
    case &DIPROP_CPOINTS:
        s = "CPOINTS";
        break;
    case &DIPROP_APPDATA:
        s = "APPDATA";
        break;
    case &DIPROP_SCANCODE:
        s = "SCANCODE";
        break;
    case &DIPROP_VIDPID:
        s = "VIDPID";
        break;
    case &DIPROP_USERNAME:
        s = "USERNAME";
        break;
    case &DIPROP_TYPENAME:
        s = "TYPENAME";
        break;
    default:
        s = "unknown";
    }
    return s;
}

char *sPropHow(DWORD h) {
    char *how[5] = {
        "DEVICE",
        "BYOFFSET",
        "BYID",
        "BYUSAGE",
        "unknown"
    };
    if(h > 4) h = 4;
    return how[h];
}

char *sEnumObjectFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    DWORD f = c & ~DIDFT_ANYINSTANCE;
    strcpy(eb, "DIDFT_");
    if ((c & ~DIDFT_ANYINSTANCE) == 0) strcat(eb, "ALL+");
    if (c & DIDFT_ABSAXIS) strcat(eb, "ABSAXIS+");
    if (c & DIDFT_RELAXIS) strcat(eb, "RELAXIS+");
    if (c & DIDFT_PSHBUTTON) strcat(eb, "PSHBUTTON+");
    if (c & DIDFT_TGLBUTTON) strcat(eb, "TGLBUTTON+");
    if (c & DIDFT_POV) strcat(eb, "POV+");
    if (c & DIDFT_COLLECTION) strcat(eb, "COLLECTION+");
    if (c & DIDFT_NODATA) strcat(eb, "NODATA+");
    if (c & DIDFT_FFACTUATOR) strcat(eb, "FFACTUATOR+");
    if (c & DIDFT_FFEFFECTTRIGGER) strcat(eb, "FFEFFECTTRIGGER+");
    if (c & DIDFT_OUTPUT) strcat(eb, "OUTPUT+");
    if (c & DIDFT_VENDORDEFINED) strcat(eb, "VENDORDEFINED+");
    if (c & DIDFT_ALIAS) strcat(eb, "ALIAS+");
    l = strlen(eb);
    if (l > strlen("DIDFT_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

char *sDataFormatFlag(DWORD f) {
    static char sBuf[120 + 1];
    char sInstance[20 + 1];
    char *s;
    switch(f & 0xFF) {
    case DIDFT_ALL:
        s = "ALL";
        break;
    case DIDFT_ABSAXIS:
        s = "ABSAXIS";
        break;
    case DIDFT_RELAXIS:
        s = "RELAXIS";
        break;
    case DIDFT_AXIS:
        s = "AXIS";
        break;
    case DIDFT_PSHBUTTON:
        s = "PSHBUTTON";
        break;
    case DIDFT_TGLBUTTON:
        s = "TGLBUTTON";
        break;
    case DIDFT_BUTTON:
        s = "BUTTON";
        break;
    case DIDFT_POV:
        s = "POV";
        break;
    case DIDFT_COLLECTION:
        s = "COLLECTION";
        break;	// ???
    case DIDFT_NODATA:
        s = "NODATA";
        break;		// ???
    default:
        s = "???";
        break;
    }
    strcpy_s(sBuf, 120, s);
    if((f & DIDFT_ANYINSTANCE) == DIDFT_ANYINSTANCE)
        strcat_s(sBuf, 120, "(ANY)");
    else {
        sprintf(sInstance, "(%d)", DIDFT_GETINSTANCE(f));
        strcat_s(sBuf, 120, sInstance);
    }
    //if(f & DIDFT_COLLECTION) strcat_s(sBuf, 120, "+COLLECTION");	// ???
    //if(f & DIDFT_NODATA) strcat_s(sBuf, 120, "+NODATA");			// ???
    if(f & DIDFT_FFACTUATOR) strcat_s(sBuf, 120, "+FFACTUATOR");
    if(f & DIDFT_FFEFFECTTRIGGER) strcat_s(sBuf, 120, "+FFEFFECTTRIGGER");
    if(f & DIDFT_OUTPUT) strcat_s(sBuf, 120, "+OUTPUT");
    if(f & DIDFT_VENDORDEFINED) strcat_s(sBuf, 120, "+VENDORDEFINED");
    if(f & DIDFT_ALIAS) strcat_s(sBuf, 120, "+ALIAS");
    if(f & DIDFT_OPTIONAL) strcat_s(sBuf, 120, "+OPTIONAL");
    return sBuf;
}

char *sODFFlag(DWORD f) {
    char *s;
    switch(f) {
    case DIDOI_ASPECTACCEL:
        s = "ACCEL";
        break;
    case DIDOI_ASPECTFORCE:
        s = "FORCE";
        break;
    case DIDOI_ASPECTPOSITION:
        s = "POSITION";
        break;
    case DIDOI_ASPECTVELOCITY:
        s = "VELOCITY";
        break;
    case 0:
        s = "ANY";
        break;
    default:
        s = "unknown";
        break;
    }
    return s;
}

char *sHow(DWORD h) {
    char *s;
    switch(h) {
    case DIPH_BYOFFSET:
        s = "BYOFFSET";
        break;
    case DIPH_BYID:
        s = "BYID";
        break;
    case DIPH_BYUSAGE:
        s = "BYUSAGE";
        break;
    default:
        s = "unknown";
        break;
    }
    return s;
}
#endif // DXW_NOTRACES

#define OLDDIVERSION FALSE

HRESULT WINAPI extDirectInputCreateA(HINSTANCE hinst, DWORD dwversion, LPDIRECTINPUT *lplpdi, LPUNKNOWN pu) {
    HRESULT res;
    ApiName("DirectInputCreateA");
    OutTraceIN("%s(%#x) hinst=%#x lpu=%#x\n", ApiRef, dwversion, hinst, pu);
    res = (*pDirectInputCreateA)(hinst, dwversion, lplpdi, pu);
    if(res) {
        if ((res == OLDDIRECTINPUTVERSION) && (dwversion == 0x800) && OLDDIVERSION) {
            OutTraceE("%s: ERROR err=OLDDIRECTINPUTVERSION version=800 try to call DirectInput8 interface\n", ApiRef);
            if(!pDirectInput8Create) {
                HMODULE hDI8;
                hDI8 = (*pLoadLibraryA)("dinput8.dll");
                pDirectInput8Create = (DirectInput8Create_Type)(*pGetProcAddress)(hDI8, "DirectInput8Create");
            }
            if(pDirectInput8Create) {
                res = extDirectInput8Create(hinst, dwversion, IID_IDirectInput8A, (LPVOID *)lplpdi, pu);
                return res;
            }
        }
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    HookDirectInputSession((void **)lplpdi, 1, ISASCII);
    return 0;
}

HRESULT WINAPI extDirectInputCreateW(HINSTANCE hinst, DWORD dwversion, LPDIRECTINPUTW *lplpdi, LPUNKNOWN pu) {
    HRESULT res;
    ApiName("DirectInputCreateW");
    OutTraceIN("%s(%#x) hinst=%#x lpu=%#x\n", ApiRef, dwversion, hinst, pu);
    res = (*pDirectInputCreateW)(hinst, dwversion, lplpdi, pu);
    if(res) {
        if ((res == OLDDIRECTINPUTVERSION) && (dwversion == 0x800) && OLDDIVERSION) {
            OutTraceE("%s: ERROR err=OLDDIRECTINPUTVERSION version=800 try to call DirectInput8 interface\n", ApiRef);
            if(!pDirectInput8Create) {
                HMODULE hDI8;
                hDI8 = (*pLoadLibraryA)("dinput8.dll");
                pDirectInput8Create = (DirectInput8Create_Type)(*pGetProcAddress)(hDI8, "DirectInput8Create");
            }
            if(pDirectInput8Create) {
                res = extDirectInput8Create(hinst, dwversion, IID_IDirectInput8W, (LPVOID *)lplpdi, pu);
                return res;
            }
        }
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    HookDirectInputSession((void **)lplpdi, 1, ISWIDEC);
    return 0;
}

HRESULT WINAPI extDirectInputCreateEx(HINSTANCE hinst, DWORD dwversion, REFIID riidltf, LPVOID *ppvout, LPUNKNOWN pu) {
    HRESULT res;
    ApiName("DirectInputCreateEx");
    OutTraceIN("%s: dwVersion=%#x REFIID=%#x(%s)\n",
               ApiRef, dwversion, riidltf.Data1, ExplainGUID((GUID *)&riidltf));
    res = (*pDirectInputCreateEx)(hinst, dwversion, riidltf, ppvout, pu);
    if(res) {
        if ((res == OLDDIRECTINPUTVERSION) && (dwversion == 0x800) && OLDDIVERSION) {
            OutTraceE("%s: ERROR err=OLDDIRECTINPUTVERSION version=800 try to call DirectInput8 interface\n", ApiRef);
            if(!pDirectInput8Create) {
                HMODULE hDI8;
                hDI8 = (*pLoadLibraryA)("dinput8.dll");
                pDirectInput8Create = (DirectInput8Create_Type)(*pGetProcAddress)(hDI8, "DirectInput8Create");
            }
            if(pDirectInput8Create) {
                switch (riidltf.Data1) {
                case 0x9A4CB684:
                    res = extDirectInput8Create(hinst, dwversion, IID_IDirectInput8A, ppvout, pu);
                    break;
                case 0x9A4CB685:
                    res = extDirectInput8Create(hinst, dwversion, IID_IDirectInput8W, ppvout, pu);
                    break;
                }
                return res;
            }
        }
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    switch (riidltf.Data1) {
    case 0x9A4CB684:
        HookDirectInputSession(ppvout, 7, ISASCII);
        break;
    case 0x9A4CB685:
        HookDirectInputSession(ppvout, 7, ISWIDEC);
        break;
    }
    return res;
}

typedef enum {
    TYPE_OBJECT_UNKNOWN = 0,
    TYPE_OBJECT_DIRECTINPUT,
    TYPE_OBJECT_DIDEVICE
};

//DEFINE_GUID(IID_IDirectInputDeviceA, 0x5944E680,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
//DEFINE_GUID(IID_IDirectInputDeviceW, 0x5944E681,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
//DEFINE_GUID(IID_IDirectInputDevice2A,0x5944E682,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
//DEFINE_GUID(IID_IDirectInputDevice2W,0x5944E683,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
//DEFINE_GUID(IID_IDirectInputDevice7A,0x57D7C6BC,0x2356,0x11D3,0x8E,0x9D,0x00,0xC0,0x4F,0x68,0x44,0xAE);
//DEFINE_GUID(IID_IDirectInputDevice7W,0x57D7C6BD,0x2356,0x11D3,0x8E,0x9D,0x00,0xC0,0x4F,0x68,0x44,0xAE);
//DEFINE_GUID(IID_IDirectInputDevice8A,0x54D41080,0xDC15,0x4833,0xA4,0x1B,0x74,0x8F,0x73,0xA3,0x81,0x79);
//DEFINE_GUID(IID_IDirectInputDevice8W,0x54D41081,0xDC15,0x4833,0xA4,0x1B,0x74,0x8F,0x73,0xA3,0x81,0x79);

HRESULT WINAPI extDIQueryInterface(void *lpdi, REFIID riid, LPVOID *obp) {
    HRESULT res;
    ApiName("IDirectInput:QueryInterface");
    int iObjectType;
    int iObjectVersion;
    BOOL isAscii;
    extern LPVOID gFakeJoy;
    OutTraceIN("%s: lpdi=%#x REFIID=%#x(%s)\n",
               ApiRef, lpdi, riid.Data1, ExplainGUID((GUID *)&riid));
    if(lpdi == gFakeJoy) { // in "Need for Speed Porsche Unleashed"
        OutTraceIN("%s: fake joystick case\n", ApiRef);
        *obp = gFakeJoy;
        return DI_OK;
    }
    res = (*pDIQueryInterface)(lpdi, riid, obp);
    if(res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    } else
        OutTraceIN("%s: obp=%#x\n", ApiRef, *obp);
    iObjectVersion = 0;
    iObjectType = TYPE_OBJECT_UNKNOWN;
    switch(riid.Data1) {
    case 0x89521360:		//DirectInputA
        iObjectType = TYPE_OBJECT_DIRECTINPUT;
        iObjectVersion = 1;
        isAscii = ISASCII;
        break;
    case 0x89521361:		//DirectInputW
        iObjectType = TYPE_OBJECT_DIRECTINPUT;
        iObjectVersion = 1;
        isAscii = ISWIDEC;
        break;
    case 0x5944E662:		//DirectInput2A
        iObjectType = TYPE_OBJECT_DIRECTINPUT;
        iObjectVersion = 2;
        isAscii = ISASCII;
        break;
    case 0x5944E663:		//DirectInput2W
        iObjectType = TYPE_OBJECT_DIRECTINPUT;
        iObjectVersion = 2;
        isAscii = ISWIDEC;
        break;
    case 0x9A4CB684:		//IDirectInput7A
        iObjectType = TYPE_OBJECT_DIRECTINPUT;
        iObjectVersion = 7;
        isAscii = ISASCII;
        break;
    case 0x9A4CB685:		//IDirectInput7W
        iObjectType = TYPE_OBJECT_DIRECTINPUT;
        iObjectVersion = 7;
        isAscii = ISWIDEC;
        break;
    case 0x5944E680:		// IID_IDirectInputDeviceA
        iObjectType = TYPE_OBJECT_DIDEVICE;
        iObjectVersion = 1;
        isAscii = ISASCII;
        break;
    case 0x5944E681:		// IID_IDirectInputDeviceW
        iObjectType = TYPE_OBJECT_DIDEVICE;
        iObjectVersion = 1;
        isAscii = ISWIDEC;
        break;
    case 0x5944E682:		// IID_IDirectInputDevice2A
        iObjectType = TYPE_OBJECT_DIDEVICE;
        iObjectVersion = 2;
        isAscii = ISASCII;
        break;
    case 0x5944E683:		// IID_IDirectInputDevice2W
        iObjectType = TYPE_OBJECT_DIDEVICE;
        iObjectVersion = 2;
        isAscii = ISWIDEC;
        break;
    case 0x57D7C6BC:		// IID_IDirectInputDevice7A
        iObjectType = TYPE_OBJECT_DIDEVICE;
        iObjectVersion = 7;
        isAscii = ISASCII;
        break;
    case 0x57D7C6BD:		// IID_IDirectInputDevice7W
        iObjectType = TYPE_OBJECT_DIDEVICE;
        iObjectVersion = 7;
        isAscii = ISWIDEC;
        break;
    case 0x54D41080:		// IID_IDirectInputDevice8A: forbidden
    case 0x54D41081:		// IID_IDirectInputDevice8W: forbidden
        break;
    }
    switch(iObjectType) {
    case TYPE_OBJECT_DIRECTINPUT:
        switch(riid.Data1) {
        // DirectInput hooks
        case 0x89521360:		//DirectInputA
            SetHook((void *)(**(DWORD **)obp + 12), extDICreateDeviceA1, (void **)&pDICreateDeviceA1, "CreateDevice(IA1)");
            SetHook((void *)(**(DWORD **)obp + 16), extDIEnumDevicesA1, (void **)&pDIEnumDevicesA1, "EnumDevices(IA1)");
            break;
        case 0x89521361:		//DirectInputW
            SetHook((void *)(**(DWORD **)obp + 12), extDICreateDeviceW1, (void **)&pDICreateDeviceW1, "CreateDevice(IW1)");
            SetHook((void *)(**(DWORD **)obp + 16), extDIEnumDevicesW1, (void **)&pDIEnumDevicesW1, "EnumDevices(IW1)");
            break;
        case 0x5944E662:		//DirectInput2A
            SetHook((void *)(**(DWORD **)obp + 12), extDICreateDeviceA2, (void **)&pDICreateDeviceA2, "CreateDevice(IA2)");
            SetHook((void *)(**(DWORD **)obp + 16), extDIEnumDevicesA2, (void **)&pDIEnumDevicesA2, "EnumDevices(IA2)");
            break;
        case 0x5944E663:		//DirectInput2W
            SetHook((void *)(**(DWORD **)obp + 12), extDICreateDeviceW2, (void **)&pDICreateDeviceW2, "CreateDevice(IW2)");
            SetHook((void *)(**(DWORD **)obp + 16), extDIEnumDevicesW2, (void **)&pDIEnumDevicesW2, "EnumDevices(IW2)");
            break;
        case 0x9A4CB684:		//IDirectInput7A
            SetHook((void *)(**(DWORD **)obp + 12), extDICreateDeviceA7, (void **)&pDICreateDeviceA7, "CreateDevice(IA7)");
            SetHook((void *)(**(DWORD **)obp + 16), extDIEnumDevicesA7, (void **)&pDIEnumDevicesA7, "EnumDevices(IA7)");
            SetHook((void *)(**(DWORD **)obp + 36), extDICreateDeviceExA, (void **)&pDICreateDeviceExA, "CreateDeviceEx(IA7)");
        case 0x9A4CB685:		//IDirectInput7W
            SetHook((void *)(**(DWORD **)obp + 12), extDICreateDeviceW7, (void **)&pDICreateDeviceW7, "CreateDevice(IW7)");
            SetHook((void *)(**(DWORD **)obp + 16), extDIEnumDevicesW7, (void **)&pDIEnumDevicesW7, "EnumDevices(IW7)");
            SetHook((void *)(**(DWORD **)obp + 36), extDICreateDeviceExW, (void **)&pDICreateDeviceExW, "CreateDeviceEx(IW7)");
            break;
        }
        break;
    case TYPE_OBJECT_DIDEVICE:
        HookDirectInputDevice(obp, iObjectVersion, isAscii);
        // v2.05.23: keep track of new devices
        if(lpdi == lpDIDKeyboard) {
            lpDIDKeyboard = (LPDIRECTINPUTDEVICE) * obp;
            OutTraceIN("%s: update lpDIDKeyboard=%#x->%#x\n", ApiRef, lpdi, lpDIDKeyboard);
        }
        if(lpdi == lpDIDSysMouse) {
            lpDIDSysMouse = (LPDIRECTINPUTDEVICE) * obp;
            OutTraceIN("%s: update lpDIDSysMouse=%#x->%#x\n", ApiRef, lpdi, lpDIDSysMouse);
        }
        if(lpdi == lpDIDJoystick) {
            lpDIDJoystick = (LPDIRECTINPUTDEVICE) * obp;
            OutTraceIN("%s: update lpDIDJoystick=%#x->%#x\n", ApiRef, lpdi, lpDIDJoystick);
        }
        break;
    }
    return DI_OK;
}

HRESULT WINAPI extDirectInput8Create(HINSTANCE hinst, DWORD dwversion, REFIID riidltf, LPVOID *ppvout, LPUNKNOWN pu) {
    HRESULT res;
    ApiName("DirectInput8Create");
    OutTraceIN("%s: dwVersion=%#x REFIID=%#x(%s)\n",
               ApiRef, dwversion, riidltf.Data1, ExplainGUID((GUID *)&riidltf));
    res = (*pDirectInput8Create)(hinst, dwversion, riidltf, ppvout, pu);
    if(res) {
        OutTraceE("%s: ERROR res=%#x\n", ApiRef, res);
        return res;
    }
    OutTraceIN("%s: di=%#x\n", ApiRef, *ppvout);
    switch(riidltf.Data1) {
    case 0xBF798030:
        HookDirectInputSession(ppvout, 8, ISASCII);
        break;
    case 0xBF798031:
        HookDirectInputSession(ppvout, 8, ISWIDEC);
        break;
    default:
        MessageBox(0, "Bad DirectInput REFIID", "warning", 0);
        break;
    }
    return 0;
}

#ifndef DXW_NOTRACES
static char *sDeviceType(REFGUID rguid) {
    char *devtype;
    switch(rguid.Data1) {
    case 0x6F1D2B60:
        devtype = "SysMouse";
        break;
    case 0x6F1D2B61:
        devtype = "SysKeyboard";
        break;
    case 0x6F1D2B70:
        devtype = "Joystick";
        break;
    case 0x6F1D2B80:
        devtype = "SysMouseEm";
        break;
    case 0x6F1D2B81:
        devtype = "SysMouseEm2";
        break;
    case 0x6F1D2B82:
        devtype = "SysKeyboardEm";
        break;
    case 0x6F1D2B83:
        devtype = "SysKeyboardEm2";
        break;
    default:
        devtype = "Unknown";
        break;
    }
    return devtype;
}
#endif

static int iDeviceType(REFGUID rguid) {
    int devtype;
    switch(rguid.Data1) {
    case 0x6F1D2B60:
        devtype = DIDEVTYPE_MOUSE;
        break;
    case 0x6F1D2B61:
        devtype = DIDEVTYPE_KEYBOARD;
        break;
    case 0x6F1D2B70:
        devtype = DIDEVTYPE_JOYSTICK;
        break;
    default:
        devtype = 0;
        break;
    }
    return devtype;
}

HRESULT WINAPI extDICreateDeviceA(int diVersion, DICreateDeviceA_Type pDICreateDevice, LPDIRECTINPUT lpdi, REFGUID rguid, LPDIRECTINPUTDEVICEA *lplpdid, LPUNKNOWN pu) {
    HRESULT res;
    ApiName("IDirectInput:CreateDeviceA");
    OutTraceIN("%s: REFGUID=%#x(%s)\n", ApiRef, rguid.Data1, sDeviceType(rguid));
    if((dxw.dwFlags6 & VIRTUALJOYSTICK) && (rguid.Data1 == IFakeJoyInstance.Data1)) {
        extern LPVOID gFakeJoy;
        IFakeDirectInputDeviceA *FakeJoy = new(IFakeDirectInputDeviceA);
        FakeJoy->IVersion = diVersion;
        *lplpdid = (LPDIRECTINPUTDEVICEA)FakeJoy;
        gFakeJoy = (LPVOID)FakeJoy;
        OutTraceIN("%s: created virtual joystick lpdid=%#x\n", ApiRef, gFakeJoy);
        return DI_OK;
    }
    res = (*pDICreateDevice)(lpdi, rguid, lplpdid, pu);
    if(res) {
        OutTraceE("%s: ERROR res=%#x\n", ApiRef, res);
        return res;
    }
    OutTraceIN("%s: did=%#x\n", ApiRef, *lplpdid);
    HookDirectInputDevice((void **)lplpdid, diVersion, ISASCII);
    switch(iDeviceType(rguid)) {
    case DIDEVTYPE_DEVICE:
        lpDIDDevice   = *lplpdid;
        break;
    case DIDEVTYPE_MOUSE:
        lpDIDSysMouse = *lplpdid;
        break;
    case DIDEVTYPE_KEYBOARD:
        lpDIDKeyboard = *lplpdid;
        break;
    case DIDEVTYPE_JOYSTICK:
        lpDIDJoystick = *lplpdid;
        break;
    }
    return DI_OK;
}

HRESULT WINAPI extDICreateDeviceA1(LPDIRECTINPUT lpdi, REFGUID rguid, LPDIRECTINPUTDEVICEA *lplpdid, LPUNKNOWN pu) {
    return extDICreateDeviceA(1, pDICreateDeviceA1, lpdi, rguid, lplpdid, pu);
}
HRESULT WINAPI extDICreateDeviceA2(LPDIRECTINPUT lpdi, REFGUID rguid, LPDIRECTINPUTDEVICEA *lplpdid, LPUNKNOWN pu) {
    return extDICreateDeviceA(2, pDICreateDeviceA2, lpdi, rguid, lplpdid, pu);
}
HRESULT WINAPI extDICreateDeviceA7(LPDIRECTINPUT lpdi, REFGUID rguid, LPDIRECTINPUTDEVICEA *lplpdid, LPUNKNOWN pu) {
    return extDICreateDeviceA(7, pDICreateDeviceA7, lpdi, rguid, lplpdid, pu);
}
HRESULT WINAPI extDICreateDeviceA8(LPDIRECTINPUT lpdi, REFGUID rguid, LPDIRECTINPUTDEVICEA *lplpdid, LPUNKNOWN pu) {
    return extDICreateDeviceA(8, pDICreateDeviceA8, lpdi, rguid, lplpdid, pu);
}

HRESULT WINAPI extDICreateDeviceW(int diVersion, DICreateDeviceW_Type pDICreateDevice, LPDIRECTINPUTW lpdi, REFGUID rguid, LPDIRECTINPUTDEVICEW *lplpdid, LPUNKNOWN pu) {
    HRESULT res;
    ApiName("IDirectInput:CreateDeviceW");
    OutTraceIN("%s: REFGUID=%#x(%s)\n", ApiRef, rguid.Data1, sDeviceType(rguid));
    if((dxw.dwFlags6 & VIRTUALJOYSTICK) && (rguid.Data1 == IFakeJoyInstance.Data1)) {
        extern LPVOID gFakeJoy;
        IFakeDirectInputDeviceW *FakeJoy = new(IFakeDirectInputDeviceW);
        FakeJoy->IVersion = diVersion;
        *lplpdid = (LPDIRECTINPUTDEVICEW)FakeJoy;
        gFakeJoy = (LPVOID)FakeJoy;
        OutTraceIN("%s: created virtual joystick lpdid=%#x\n", ApiRef, gFakeJoy);
        return DI_OK;
    }
    res = (*pDICreateDevice)(lpdi, rguid, lplpdid, pu);
    if(res) {
        OutTraceE("%s: ERROR res=%#x\n", ApiRef, res);
        return res;
    }
    OutTraceIN("%s: did=%#x\n", ApiRef, *lplpdid);
    HookDirectInputDevice((void **)lplpdid, diVersion, ISWIDEC);
    switch(iDeviceType(rguid)) {
    case DIDEVTYPE_DEVICE:
        lpDIDDevice   = (LPDIRECTINPUTDEVICE) * lplpdid;
        break;
    case DIDEVTYPE_MOUSE:
        lpDIDSysMouse = (LPDIRECTINPUTDEVICE) * lplpdid;
        break;
    case DIDEVTYPE_KEYBOARD:
        lpDIDKeyboard = (LPDIRECTINPUTDEVICE) * lplpdid;
        break;
    case DIDEVTYPE_JOYSTICK:
        lpDIDJoystick = (LPDIRECTINPUTDEVICE) * lplpdid;
        break;
    }
    return DI_OK;
}

HRESULT WINAPI extDICreateDeviceW1(LPDIRECTINPUTW lpdi, REFGUID rguid, LPDIRECTINPUTDEVICEW *lplpdid, LPUNKNOWN pu) {
    return extDICreateDeviceW(1, pDICreateDeviceW1, lpdi, rguid, lplpdid, pu);
}
HRESULT WINAPI extDICreateDeviceW2(LPDIRECTINPUTW lpdi, REFGUID rguid, LPDIRECTINPUTDEVICEW *lplpdid, LPUNKNOWN pu) {
    return extDICreateDeviceW(2, pDICreateDeviceW2, lpdi, rguid, lplpdid, pu);
}
HRESULT WINAPI extDICreateDeviceW7(LPDIRECTINPUTW lpdi, REFGUID rguid, LPDIRECTINPUTDEVICEW *lplpdid, LPUNKNOWN pu) {
    return extDICreateDeviceW(7, pDICreateDeviceW7, lpdi, rguid, lplpdid, pu);
}
HRESULT WINAPI extDICreateDeviceW8(LPDIRECTINPUTW lpdi, REFGUID rguid, LPDIRECTINPUTDEVICEW *lplpdid, LPUNKNOWN pu) {
    return extDICreateDeviceW(8, pDICreateDeviceW8, lpdi, rguid, lplpdid, pu);
}

HRESULT WINAPI extDICreateDeviceExA(LPDIRECTINPUTA lpdi, REFGUID rguid, REFIID riid, LPVOID *pvout, LPUNKNOWN pu) {
    HRESULT res;
    ApiName("IDirectInput:CreateDeviceExA");
    int iObjectVersion;
    OutTraceIN("%s: GUID=%#x(%s) REFIID=%#x\n", ApiRef, rguid.Data1, sDeviceType(rguid), riid.Data1);
    if((dxw.dwFlags6 & VIRTUALJOYSTICK) && (rguid.Data1 == IFakeJoyInstance.Data1)) {
        extern LPVOID gFakeJoy;
        IFakeDirectInputDeviceA *FakeJoy = new(IFakeDirectInputDeviceA);
        switch(riid.Data1) {
        case 0x5944E680:
            FakeJoy->IVersion = 1;
            break;
        case 0x5944E682:
            FakeJoy->IVersion = 2;
            break;
        case 0x57D7C6BC:
            FakeJoy->IVersion = 7;
            break;
        default:
            FakeJoy->IVersion = 7;
            break;
        }
        *pvout = (LPDIRECTINPUTDEVICEA)FakeJoy;
        gFakeJoy = (LPVOID)FakeJoy;
        OutTraceIN("%s: created virtual joystick lpdid=%#x\n", ApiRef, gFakeJoy);
        return DI_OK;
    }
    switch(riid.Data1) {
    case 0x5944E680:
        iObjectVersion = 1;
        break;
    case 0x5944E682:
        iObjectVersion = 2;
        break;
    case 0x57D7C6BC:
        iObjectVersion = 7;
        break;
    default:
        iObjectVersion = 7;
        break;
    }
    res = (*pDICreateDeviceExA)(lpdi, rguid, riid, pvout, pu);
    if(res) {
        OutTraceE("%s: ERROR res=%#x\n", ApiRef, res);
        return res;
    }
    OutTraceIN("%s: did=%#x\n", ApiRef, *pvout);
    HookDirectInputDevice(pvout, iObjectVersion, ISASCII);
    switch(iDeviceType(rguid)) {
    case DIDEVTYPE_DEVICE:
        lpDIDDevice   = *(LPDIRECTINPUTDEVICE *)pvout;
        break;
    case DIDEVTYPE_MOUSE:
        lpDIDSysMouse = *(LPDIRECTINPUTDEVICE *)pvout;
        break;
    case DIDEVTYPE_KEYBOARD:
        lpDIDKeyboard = *(LPDIRECTINPUTDEVICE *)pvout;
        break;
    case DIDEVTYPE_JOYSTICK:
        lpDIDJoystick = *(LPDIRECTINPUTDEVICE *)pvout;
        break;
    }
    return DI_OK;
}

HRESULT WINAPI extDICreateDeviceExW(LPDIRECTINPUTW lpdi, REFGUID rguid, REFIID riid, LPVOID *pvout, LPUNKNOWN pu) {
    HRESULT res;
    ApiName("IDirectInput:CreateDeviceExW");
    int iObjectVersion;
    OutTraceIN("%s: GUID=%#x(%s) REFIID=%#x\n", ApiRef, rguid.Data1, sDeviceType(rguid), riid.Data1);
    if((dxw.dwFlags6 & VIRTUALJOYSTICK) && (rguid.Data1 == IFakeJoyInstance.Data1)) {
        extern LPVOID gFakeJoy;
        IFakeDirectInputDeviceW *FakeJoy = new(IFakeDirectInputDeviceW);
        switch(riid.Data1) {
        case 0x5944E681:
            FakeJoy->IVersion = 1;
            break;
        case 0x5944E683:
            FakeJoy->IVersion = 2;
            break;
        case 0x57D7C6BD:
            FakeJoy->IVersion = 7;
            break;
        default:
            FakeJoy->IVersion = 7;
            break;
        }
        *pvout = (LPDIRECTINPUTDEVICEW)FakeJoy;
        gFakeJoy = (LPVOID)FakeJoy;
        OutTraceIN("%s: created virtual joystick lpdid=%#x\n", ApiRef, gFakeJoy);
        return DI_OK;
    }
    switch(riid.Data1) {
    case 0x5944E681:
        iObjectVersion = 1;
        break;
    case 0x5944E683:
        iObjectVersion = 2;
        break;
    case 0x57D7C6BD:
        iObjectVersion = 7;
        break;
    default:
        iObjectVersion = 7;
        break;
    }
    res = (*pDICreateDeviceExW)(lpdi, rguid, riid, pvout, pu);
    if(res) {
        OutTraceE("%s: ERROR res=%#x\n", ApiRef, res);
        return res;
    }
    OutTraceIN("%s: did=%#x\n", ApiRef, *pvout);
    HookDirectInputDevice(pvout, iObjectVersion, ISWIDEC);
    switch(iDeviceType(rguid)) {
    case DIDEVTYPE_DEVICE:
        lpDIDDevice   = *(LPDIRECTINPUTDEVICE *)pvout;
        break;
    case DIDEVTYPE_MOUSE:
        lpDIDSysMouse = *(LPDIRECTINPUTDEVICE *)pvout;
        break;
    case DIDEVTYPE_KEYBOARD:
        lpDIDKeyboard = *(LPDIRECTINPUTDEVICE *)pvout;
        break;
    case DIDEVTYPE_JOYSTICK:
        lpDIDJoystick = *(LPDIRECTINPUTDEVICE *)pvout;
        break;
    }
    return DI_OK;
}

// v2.04.90: added DirectInputDevice*::Release hook to clear the global device descriptors and avoid
// throwing exception after the devices are actually released.
HRESULT WINAPI extDIRelease(void *lpdid) {
    HRESULT ret;
    ApiName("IDirectInput:Release");
    char *s = "Unknown";
    if(lpdid == lpDIDSysMouse) s = "SysMouse";
    if(lpdid == lpDIDKeyboard) s = "Keyboard";
    if(lpdid == lpDIDJoystick) s = "Joystick";
    ret = (*pDIRelease)(lpdid);
    if(ret == 0) {
        if(lpdid == lpDIDSysMouse) lpDIDSysMouse = NULL;
        if(lpdid == lpDIDKeyboard) lpDIDKeyboard = NULL;
        if(lpdid == lpDIDJoystick) lpDIDJoystick = NULL;
    }
    OutTraceIN("%s: ret=%d device=%s\n", ApiRef, ret, s);
    return ret;
}

/*  from MSDN:
    cbObjectData
    Size of the DIDEVICEOBJECTDATA structure, in bytes.
    rgdod
    Array of DIDEVICEOBJECTDATA structures to receive the buffered data.
	The number of elements in this array must be equal to the value of the pdwInOut parameter.
	If this parameter is NULL, the buffered data is not stored anywhere, but all other side effects take place.
    pdwInOut
    On entry, the number of elements in the array pointed to by the rgdod parameter.
	On exit, the number of elements actually obtained.
    dwFlags
    Flags that control the manner in which data is obtained.
	This value can be 0 or the following flag.

    DIGDD_PEEK
        Do not remove the items from the buffer. A subsequent IDirectInputDevice8::GetDeviceData call will read the same data.
		Normally, data is removed from the buffer after it is read.
*/

/*  Mind the following scenarios!
    Your application can query for the number of elements in the device buffer by setting the rgdod parameter to NULL, setting pdwInOut to INFINITE
    and setting dwFlags to DIGDD_PEEK. The following code example illustrates how this can be done.

    dwItems = INFINITE;
    hres = idirectinputdevice9_GetDeviceData(
            pdid,
            sizeof(DIDEVICEOBJECTDATA),
            NULL,
            &dwItems,
            DIGDD_PEEK);
    if (SUCCEEDED(hres)) {
    // dwItems = Number of elements in buffer.
    if (hres == DI_BUFFEROVERFLOW) {
        // Buffer overflow occurred; not all data
        //   was successfully captured.
    }
    }

    To query about whether a buffer overflow has occurred, set the rgdod parameter to NULL and the pdwInOut parameter to 0.
    The following code example illustrates how this can be done.

    dwItems = 0;
    hres = idirectinputdevice9_GetDeviceData(
            pdid,
            sizeof(DIDEVICEOBJECTDATA),
            NULL,
            &dwItems,
            0);
    if (hres == DI_BUFFEROVERFLOW) {
    // Buffer overflow occurred.
    }
*/

HRESULT WINAPI extGetDeviceData(LPDIRECTINPUTDEVICE lpdid, DWORD cbdata, LPVOID rgdod, LPDWORD pdwinout, DWORD dwflags) {
    HRESULT res;
    ApiName("IDirectInput:GetDeviceData");
    BYTE *tmp;
    int i;
    POINT p;
    int in, out;
    OutDebugIN("%s: did=%#x(%s) cbdata=%i rgdod=%#x, inout=%d flags=%#x\n",
               ApiRef, lpdid, sDevice(lpdid), cbdata, rgdod, *pdwinout, dwflags);
    // beware: it seems possible that a program calls GetDeviceData passing a small rgdod area
    // by specifying a small *pdwinout value and then the call returns a bigger number of
    // available DIDEVICEOBJECTDATA structures that are not filled for lack of space.
    // In this case it is dangerous to try to use DIDEVICEOBJECTDATA elements using the final
    // *pdwinout value because it could point to unallocated area.
    // Better keep track of the available and returned values and process only the minimum
    // of the two values that surely corresponds to available data.
    // Problem found in "Gilbert Goodmate and the Mushroom of Phungoria" where sometimes the
    // passwd value of *pdwinout is -1.
    in  = (int) * pdwinout;
    res = (*pGetDeviceData)(lpdid, cbdata, rgdod, pdwinout, dwflags);
    out = (int) * pdwinout;
    out = (in >= out) ? out : in;
#ifdef RECOVERDATAFORMAT
    if((RECOVERDATAFORMAT) && (res == DIERR_NOTBUFFERED)) {
        OutTraceE("%s: recovering DIERR_NOTBUFFERED\n", ApiRef);
        DIPROPDWORD p;
        res = (*pUnacquire)(lpdid);
        OutTraceE("%s: Unacquire res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        p.diph.dwSize = sizeof(DIPROPDWORD);
        p.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        p.diph.dwHow = DIPH_DEVICE;
        p.diph.dwObj = 0;
        p.dwData = 8;
        res = (*pSetProperty)(lpdid, DIPROP_BUFFERSIZE, &p.diph);
        OutTraceE("%s: SetProperty res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        res = (*pAcquire)(lpdid);
        OutTraceE("%s: Acquire res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        in  = (int) * pdwinout;
        res = (*pGetDeviceData)(lpdid, cbdata, rgdod, pdwinout, dwflags);
        out = (int) * pdwinout;
        out = (in >= out) ? out : in;
    }
#endif // RECOVERDATAFORMAT
#ifndef DXW_NOTRACES
    if (IsDebugIN && (res == DI_OK)) {
        LPDIDEVICEOBJECTDATA lpdod = (LPDIDEVICEOBJECTDATA)rgdod;
        OutTrace("%s: inout=%d\n", ApiRef, *pdwinout);
        if (out > 0) for(int i = 0; i < out; i++) {
                OutTrace("> dod[%d]: ofs=%d data=%#x seq=%d time=%#x appdata=%#x\n",
                         i, lpdod[i].dwOfs, lpdod[i].dwData, lpdod[i].dwSequence, lpdod[i].dwTimeStamp, lpdod[i].uAppData);
            }
    }
#endif // DXW_NOTRACES
    if (((res == DIERR_INPUTLOST) || (res == DIERR_NOTACQUIRED)) && RECOVERINPUTLOST) {
        OutTraceE("%s: recovering DIERR_INPUTLOST\n", ApiRef);
        if(lpdid == lpDIDSysMouse) {
            res = (*pDISetCooperativeLevel)(lpdid, dxw.GethWnd(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
            _if(res) OutTraceE("%s: SetCooperativeLevel ERROR: err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
            if(dxw.dwFlags1 & CLIPCURSOR) dxw.SetClipCursor();
        }
        res = (*pAcquire)(lpdid);
        _if(res) OutTraceE("%s: Acquire ERROR: err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        res = (*pGetDeviceData)(lpdid, cbdata, rgdod, pdwinout, dwflags);
    }
    switch(res) {
    case DI_OK:
    case DI_BUFFEROVERFLOW:
        break;
    case DIERR_NOTACQUIRED:
    case DIERR_INPUTLOST:
        OutDebugIN("%s: handling err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        *pdwinout = 0; // to avoid crashes
        break;
    default:
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        if((dxw.dwFlags7 & SUPPRESSDIERRORS) && res) {
            OutTraceIN("%s: SUPPRESSDIERRORS res=DI_OK\n", ApiRef);
            *pdwinout = 0; // to avoid crashes
            res = DI_OK;
        }
        return res;
        break;
    }
    if(!dxw.bActive) {
        *pdwinout = 0;
        return DI_OK;
    }
    if(lpdid == lpDIDSysMouse) {
        if(dxw.dwFlags4 & RELEASEMOUSE) {
            POINT curr;
            RECT client;
            extern GetCursorPos_Type pGetCursorPos;
            extern GetClientRect_Type pGetClientRect;
            extern ScreenToClient_Type pScreenToClient;
            (*pGetCursorPos)(&curr);
            (*pScreenToClient)(dxw.GethWnd(), &curr);
            (*pGetClientRect)(dxw.GethWnd(), &client);
            if ((curr.x < client.left) || (curr.y < client.top) || (curr.x > client.right) || (curr.y > client.bottom)) {
                *pdwinout = 0;
                return DI_OK;
            }
        }
        tmp = (BYTE *)rgdod;
        if(!tmp) return res;
        if(dxw.bDInputAbs) {
            GetMousePosition((int *)&p.x, (int *)&p.y);
            for(i = 0; i < out; i ++) {
                if(((LPDIDEVICEOBJECTDATA)tmp)->dwOfs == DIMOFS_X)((LPDIDEVICEOBJECTDATA)tmp)->dwData = p.x;
                if(((LPDIDEVICEOBJECTDATA)tmp)->dwOfs == DIMOFS_Y)((LPDIDEVICEOBJECTDATA)tmp)->dwData = p.y;
                tmp += cbdata;
            }
            OutDebugIN("%s: ABS mousedata XY=(%d,%d)\n", ApiRef, p.x, p.y);
        } else {
            if(dxw.dwFlags6 & EMULATERELMOUSE) {
                int iMaxX, iMaxY, iMinX, iMinY;
                RECT WinRect = dxw.GetUnmappedScreenRect();
                iMinX = WinRect.left;
                iMaxX = WinRect.right;
                iMinY = WinRect.top;
                iMaxY = WinRect.bottom;
                iCursorX = (iMaxX + iMinX) / 2;
                iCursorY = (iMaxY + iMinY) / 2;
                OutDebugIN("%s: RELATIVE clip=(%d,%d)-(%d,%d) pos=(%d,%d)\n",
                           ApiRef, iMinX, iMinY, iMaxX, iMaxY, iCursorX, iCursorY);
                (*pGetCursorPos)(&p);
                for(i = 0; i < out; i ++) {
                    if(((LPDIDEVICEOBJECTDATA)tmp)->dwOfs == DIMOFS_X) {
                        ((LPDIDEVICEOBJECTDATA)tmp)->dwData = p.x - iCursorX;
                        p.x = iCursorX;
                    }
                    if(((LPDIDEVICEOBJECTDATA)tmp)->dwOfs == DIMOFS_Y) {
                        ((LPDIDEVICEOBJECTDATA)tmp)->dwData = p.y - iCursorY;
                        p.y = iCursorY;
                    }
                }
                (*pSetCursorPos)(p.x, p.y);
            }
            if(dxw.dwFlags6 & SCALERELMOUSE) {
                for(i = 0; i < out; i ++) {
                    if(((LPDIDEVICEOBJECTDATA)tmp)->dwOfs == DIMOFS_X) {
                        POINT p;
                        p.x = (LONG)((LPDIDEVICEOBJECTDATA)tmp)->dwData;
                        p.y = 0;
                        dxw.ScaleRelMouse(ApiRef, &p);
                        ((LPDIDEVICEOBJECTDATA)tmp)->dwData = (DWORD)p.x;
                    }
                    if(((LPDIDEVICEOBJECTDATA)tmp)->dwOfs == DIMOFS_Y) {
                        POINT p;
                        p.y = (LONG)((LPDIDEVICEOBJECTDATA)tmp)->dwData;
                        p.x = 0;
                        dxw.ScaleRelMouse(ApiRef, &p);
                        ((LPDIDEVICEOBJECTDATA)tmp)->dwData = (DWORD)p.y;
                    }
                }
            }
#ifndef DXW_NOTRACES
            else {
                for(i = 0; i < out; i ++) {
                    _if(((LPDIDEVICEOBJECTDATA)tmp)->dwOfs == DIMOFS_X) OutDebugIN("GetDeviceData(I): REL mousedata X=%d\n", ((LPDIDEVICEOBJECTDATA)tmp)->dwData);
                    _if(((LPDIDEVICEOBJECTDATA)tmp)->dwOfs == DIMOFS_Y) OutDebugIN("GetDeviceData(I): REL mousedata Y=%d\n", ((LPDIDEVICEOBJECTDATA)tmp)->dwData);
                    tmp += cbdata;
                }
            }
#endif // DXW_NOTRACES
        }
    }
    return res;
}

HRESULT WINAPI extGetDeviceState(LPDIRECTINPUTDEVICE lpdid, DWORD cbdata, LPDIMOUSESTATE lpvdata) {
    HRESULT res;
    ApiName("IDirectInput:GetDeviceState");
    POINT p = {0, 0};
    OutDebugIN("%s: did=%#x(%s) cbData=%i active=%i\n", ApiRef, lpdid, sDevice(lpdid), cbdata, dxw.bActive);
    res = (*pGetDeviceState)(lpdid, cbdata, lpvdata);
    if (((res == DIERR_INPUTLOST) || (res == DIERR_NOTACQUIRED)) && RECOVERINPUTLOST) {
        OutTraceE("%s: did=%#x recovering DIERR_INPUTLOST\n", ApiRef, lpdid);
        if(lpdid == lpDIDSysMouse) {
            res = (*pDISetCooperativeLevel)(lpdid, dxw.GethWnd(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
            _if(res) OutTraceE("%s: SetCooperativeLevel ERROR: err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
            if(dxw.dwFlags1 & CLIPCURSOR) dxw.SetClipCursor();
        }
        res = (*pAcquire)(lpdid);
        _if(res) OutTraceE("%s: Acquire ERROR: did=%#x err=%#x(%s)\n", ApiRef, lpdid, res, ExplainDDError(res));
        res = (*pGetDeviceState)(lpdid, cbdata, lpvdata);
    }
#ifndef DXW_NOTRACES
    if(IsDebugIN && (res == DI_OK)) {
        LPDIMOUSESTATE lpm;
        LPDIMOUSESTATE2 lpm2;
        LPDIJOYSTATE lpj;
        LPDIJOYSTATE2 lpj2;
        BYTE *kinput;
        switch(cbdata) {
        case sizeof(DIMOUSESTATE):
            lpm = (LPDIMOUSESTATE)lpvdata;
            OutTrace("> MOUSESTATE: pos(x:y:z)=(%d:%d:%d) btn=%#x:%#x:%#x:%#x\n",
                     lpm->lX, lpm->lY, lpm->lZ,
                     lpm->rgbButtons[0], lpm->rgbButtons[1], lpm->rgbButtons[2], lpm->rgbButtons[3]);
            break;
        case sizeof(DIMOUSESTATE2):
            lpm2 = (LPDIMOUSESTATE2)lpvdata;
            OutTrace("> MOUSESTATE2: pos(x:y:z)=(%d:%d:%d) btn=%#x:%#x:%#x:%#x btn2=%#x:%#x:%#x:%#x\n",
                     lpm2->lX, lpm2->lY, lpm2->lZ,
                     lpm2->rgbButtons[0], lpm2->rgbButtons[1], lpm2->rgbButtons[2], lpm2->rgbButtons[3],
                     lpm2->rgbButtons[4], lpm2->rgbButtons[5], lpm2->rgbButtons[6], lpm2->rgbButtons[7]);
            break;
        case sizeof(DIJOYSTATE):
            lpj = (LPDIJOYSTATE)lpvdata;
            OutTrace("> JOYSTATE: pos-rot(x:y:z)=(%d:%d:%d)-(%d:%d:%d) btn=%#x:%#x:%#x:%#x:%#x:%#x:%#x:%#x\n",
                     lpj->lX, lpj->lY, lpj->lZ,
                     lpj->lRx, lpj->lRy, lpj->lRz,
                     lpj->rgbButtons[0], lpj->rgbButtons[1], lpj->rgbButtons[2], lpj->rgbButtons[3],
                     lpj->rgbButtons[4], lpj->rgbButtons[5], lpj->rgbButtons[6], lpj->rgbButtons[7]);
            break;
        case sizeof(DIJOYSTATE2):
            lpj2 = (LPDIJOYSTATE2)lpvdata;
            OutTrace("> JOYSTATE2: pos-rot(x:y:z)=(%d:%d:%d)-(%d:%d:%d) btn=%#x:%#x:%#x:%#x:%#x:%#x:%#x:%#x\n",
                     lpj2->lX, lpj2->lY, lpj2->lZ,
                     lpj2->lRx, lpj2->lRy, lpj2->lRz,
                     lpj2->rgbButtons[0], lpj2->rgbButtons[1], lpj2->rgbButtons[2], lpj2->rgbButtons[3],
                     lpj2->rgbButtons[4], lpj2->rgbButtons[5], lpj2->rgbButtons[6], lpj2->rgbButtons[7]);
            break;
        case 256:
            OutTrace("> KEYBOARD\n");
            kinput = (BYTE *)lpvdata;
            for(int k = 0; k < 256; k++)
                if(kinput[k]) OutTrace("> K[%d]\n", k);
            break;
        default:
            OutTrace("> custom\n");
            break;
        }
        OutHexIN((LPBYTE)lpvdata, cbdata);
    }
#endif // DXW_NOTRACES
    switch(res) {
    case DI_OK:
        break;
    case DIERR_NOTACQUIRED:
    case DIERR_INPUTLOST:
        OutDebugIN("%s: handling err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        break;
    default:
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
        break;
    }
    if(	cbdata == sizeof(DIMOUSESTATE) || cbdata == sizeof(DIMOUSESTATE2)) {
        if(!dxw.bActive) {
            lpvdata->lZ = 0;
            *(DWORD *)lpvdata->rgbButtons = 0;
            OutDebugIN("%s: DEBUG cleared mousestate=(%d,%d)\n", ApiRef, p.x, p.y);
            return DI_OK;
        }
        if(dxw.bDInputAbs) { // absolute position
            POINT p;
            extGetCursorPos(&p);
            lpvdata->lX = p.x;
            lpvdata->lY = p.y;
            OutDebugIN("%s: cursor pos x,y=(%d,%d)\n", ApiRef, p.x, p.y);
        } else { // relative position
            if(dxw.dwFlags6 & EMULATERELMOUSE) {
                int iMaxX, iMaxY, iMinX, iMinY;
                RECT WinRect = dxw.GetUnmappedScreenRect();
                iMinX = WinRect.left;
                iMaxX = WinRect.right;
                iMinY = WinRect.top;
                iMaxY = WinRect.bottom;
                iCursorX = (iMaxX + iMinX) / 2;
                iCursorY = (iMaxY + iMinY) / 2;
                OutDebugIN("%s: RELATIVE clip=(%d,%d)-(%d,%d) pos=(%d,%d)\n",
                           ApiRef, iMinX, iMinY, iMaxX, iMaxY, iCursorX, iCursorY);
                (*pGetCursorPos)(&p);
                lpvdata->lX = p.x - iCursorX;
                lpvdata->lY = p.y - iCursorY;
                (*pSetCursorPos)(iCursorX, iCursorY);
            }
            if(dxw.dwFlags6 & SCALERELMOUSE) {
                POINT p;
                p.x = lpvdata->lX;
                p.y = lpvdata->lY;
                dxw.ScaleRelMouse(ApiRef, &p);
                lpvdata->lX = (DWORD)p.x;
                lpvdata->lY = (DWORD)p.y;
            }
        }
    }
    // SysKeybd device
    if(cbdata == 256 && !dxw.bActive) {
        ZeroMemory(lpvdata, 256);
        OutDebugIN("%s: DEBUG cleared syskeybdstate\n", ApiRef);
    }
    return DI_OK;
}

#ifndef DXW_NOTRACES
char *ExplainDataFormatFlags(DWORD f) {
    char *s;
    s = "unknown";
    switch(f) {
    case DIDF_ABSAXIS:
        s = "DIDF_ABSAXIS";
        break;
    case DIDF_RELAXIS:
        s = "DIDF_RELAXIS";
        break;
    default:
        s = "unknown";
        break;
    }
    return s;
}
#endif // DXW_NOTRACES

HRESULT WINAPI extSetDataFormat(LPDIRECTINPUTDEVICE lpdid, LPCDIDATAFORMAT lpdf) {
    HRESULT res;
    ApiName("IDirectInput:SetDataFormat");
    OutTraceIN("%s: did=%#x(%s) lpdf=%#x size=%d objsize=%d flags=%#x(%s) datasize=%d numobj=%d\n",
               ApiRef, lpdid, sDevice(lpdid), lpdf, lpdf->dwSize, lpdf->dwObjSize, lpdf->dwFlags, ExplainDataFormatFlags(lpdf->dwFlags), lpdf->dwDataSize, lpdf->dwNumObjs);
#ifndef DXW_NOTRACES
    if(IsDebugIN) {
        DIOBJECTDATAFORMAT *df;
        df = lpdf->rgodf;
        for(DWORD i = 0; i < lpdf->dwNumObjs; i++) {
            OutTrace("> obj[%d] ofs=%d flags=%#x(%s) type=%#x(%s)\n",
                     i, df[i].dwOfs, df[i].dwFlags, sODFFlag(df[i].dwFlags), df[i].dwType, sDataFormatFlag(df[i].dwType));
        }
    }
#endif // DXW_NOTRACES
    if(lpdid == lpDIDSysMouse) {
        if(lpdf->dwFlags & DIDF_ABSAXIS) dxw.bDInputAbs = 1;
        if(lpdf->dwFlags & DIDF_RELAXIS) dxw.bDInputAbs = 0;
    }
    if(dxw.dwFlags12 & FORCERELAXIS) {
        OutTraceIN("%s: FORCERELAXIS\n", ApiRef);
        dxw.bDInputAbs = 0;
        DIDATAFORMAT df = *lpdf;
        df.dwFlags &= ~DIDF_ABSAXIS;
        df.dwFlags |= DIDF_RELAXIS;
        return (*pSetDataFormat)(lpdid, &df);
    }
    if(dxw.dwFlags12 & FORCEABSAXIS) {
        OutTraceIN("%s: FORCEABSAXIS\n", ApiRef);
        dxw.bDInputAbs = 1;
        DIDATAFORMAT df = *lpdf;
        df.dwFlags &= ~DIDF_RELAXIS;
        df.dwFlags |= DIDF_ABSAXIS;
        return (*pSetDataFormat)(lpdid, &df);
    }
    res = (*pSetDataFormat)(lpdid, lpdf);
    _if(res) OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extDISetCooperativeLevel(LPDIRECTINPUTDEVICE lpdid, HWND hwnd, DWORD dwflags) {
    HRESULT res;
    ApiName("IDirectInput:SetCooperativeLevel");
    OutTraceIN("%s: did=%#x(%s) hwnd=%#x flags=%#x(%s)\n",
               ApiRef, lpdid, sDevice(lpdid), hwnd, dwflags, ExplainDICooperativeFlags(dwflags));
    if(dxw.IsRealDesktop(hwnd)) {
        OutTraceIN("%s: remap desktop hwnd=%#x -> %#x\n", ApiRef, hwnd, dxw.GethWnd());
        hwnd = dxw.GethWnd();
    }
    if(lpdid == lpDIDSysMouse) {
        dwflags = (DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
        OutTraceIN("%s: SYSMOUSE flags=%#x(%s)\n", ApiRef, dwflags, ExplainDICooperativeFlags(dwflags));
    }
    // v2.03.84: SHAREDKEYBOARD option to prevent situations like "Planet of the Apes"
    // not processing the PrintScreen syskeys.
    if((lpdid == lpDIDKeyboard) && (dxw.dwFlags7 & SHAREDKEYBOARD)) {
        dwflags = (DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
        OutTraceIN("%s: SYSKEYBOARD flags=%#x(%s)\n", ApiRef, dwflags, ExplainDICooperativeFlags(dwflags));
    }
    res = (*pDISetCooperativeLevel)(lpdid, hwnd, dwflags);
    _if(res) OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

// Simplified version, taking in proper account the GetCursorPos API hooking & coordinate processing
void GetMousePosition(int *x, int *y) {
    POINT p;
    extern BOOL WINAPI extGetCursorPos(LPPOINT);
    extGetCursorPos(&p);
    *x = p.x;
    *y = p.y;
    OutTraceIN("DXW:GetMousePosition: x,y=(%d,%d)\n", *x, *y);
}

typedef struct {
    LPDIENUMDEVICESCALLBACK cb;
    LPVOID arg;
    int DIVersion;
} CallbackArg;

HRESULT WINAPI extDeviceProxy(LPCDIDEVICEINSTANCE dev, LPVOID arg) {
    HRESULT res;
    ApiName("IDirectInput:EnumDevices");
    char *p;
    LPCDIDEVICEINSTANCEA devA;
    LPCDIDEVICEINSTANCEW devW;
    switch (dev->dwSize) {
    case sizeof(DIDEVICEINSTANCEA):
        p = "ASCII";
        devA = (LPCDIDEVICEINSTANCEA)dev;
        OutTraceIN("%s: CALLBACK size=%d(%s) GUID=%s type=%#x InstanceName=\"%s\", ProductName=\"%s\"\n",
                   ApiRef,
                   devA->dwSize, p, sRIID(devA->guidInstance),
                   devA->dwDevType, devA->tszInstanceName, devA->tszProductName);
        break;
    case sizeof(DIDEVICEINSTANCEW):
        p = "WIDECHAR";
        devW = (LPCDIDEVICEINSTANCEW)dev;
        OutTraceIN("%s: CALLBACK size=%d(%s) GUID=%s type=%#x InstanceName=\"%ls\", ProductName=\"%ls\"\n",
                   ApiRef,
                   devW->dwSize, p, sRIID(devW->guidInstance),
                   devW->dwDevType, devW->tszInstanceName, devW->tszProductName);
        break;
    default:
        p = "UNKNOWN";
        break;
    }
    if((dxw.dwFlags7 & SKIPDEVTYPEHID) && (dev->dwDevType & DIDEVTYPE_HID)) {
        OutTraceIN("%s: skip HID device devtype=%#x\n", ApiRef, dev->dwDevType);
        return TRUE; // skip DIDEVTYPE_HID
    }
    if((dxw.dwFlags9 & HIDEJOYSTICKS) && (dev->dwDevType == DIDEVTYPE_JOYSTICK)) {
        OutTraceIN("%s: skip JOYSTICK device devtype=%#x\n", ApiRef, dev->dwDevType);
        return TRUE; // skip DIDEVTYPE_JOYSTICK
    }
    if((dxw.dwFlags9 & HIDEJOYSTICKS) && (dev->dwDevType >= DI8DEVTYPE_JOYSTICK)) {
        OutTraceIN("%s: skip DI8 JOYSTICK+ device devtype=%#x\n", ApiRef, dev->dwDevType);
        return TRUE; // skip DI8DEVTYPE_JOYSTICK and following
    }
    res = (*(((CallbackArg *)arg)->cb))(dev, ((CallbackArg *)arg)->arg);
    OutTraceIN("%s: CALLBACK ret=%#x\n", ApiRef, res);
    return res;
}

#ifndef DXW_NOTRACES
char *sEnumFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DIEDFL_");
    if (c & DIEDFL_ATTACHEDONLY) strcat(eb, "ATTACHEDONLY+");
    else strcat(eb, "ALLDEVICES+");
    if (c & DIEDFL_FORCEFEEDBACK) strcat(eb, "FORCEFEEDBACK+");
    if (c & DIEDFL_INCLUDEALIASES) strcat(eb, "INCLUDEALIASES+");
    if (c & DIEDFL_INCLUDEPHANTOMS) strcat(eb, "INCLUDEPHANTOMS+");
    if (c & DIEDFL_INCLUDEHIDDEN) strcat(eb, "INCLUDEHIDDEN+");
    l = strlen(eb);
    if (l > strlen("DIEDFL_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}
#endif // DXW_NOTRACES

HRESULT WINAPI extDIEnumDevicesA(int iDIVersion, DIEnumDevicesA_Type pDIEnumDevices, void *lpdi, DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags) {
    HRESULT res;
    ApiName("IDirectInput:EnumDevicesA");
    CallbackArg Arg;
    OutTraceIN("%s(%d): di=%#x DevType=%#x CallBack=%#x Ref=%#x Flags=%#x(%s)\n",
               ApiRef, iDIVersion, lpdi, dwDevType, lpCallback, pvRef, dwFlags, sEnumFlags(dwFlags));
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) {
        DIDEVICEINSTANCEA DevInstance;
        memset(&DevInstance, 0, sizeof(DevInstance));
        DevInstance.dwSize = sizeof(DIDEVICEINSTANCEA);
        DevInstance.dwDevType = // mimick vJoy
            DIDEVTYPE_HID |
            DIDFT_MAKEINSTANCE(1) |
            (iDIVersion == 8 ? DI8DEVTYPE_JOYSTICK : DIDEVTYPE_JOYSTICK);
        // if SKIPDEVTYPEHID pretend this device is NOT a HID!!
        if(dxw.dwFlags7 & SKIPDEVTYPEHID) DevInstance.dwDevType &= ~DIDEVTYPE_HID;
        memcpy(&DevInstance.guidFFDriver, &IFakeJoyFFDriver, sizeof(GUID));
        memcpy(&DevInstance.guidInstance, &IFakeJoyInstance, sizeof(GUID));
        memcpy(&DevInstance.guidProduct,  &IFakeJoyProduct,  sizeof(GUID));
        strcpy_s(DevInstance.tszInstanceName, 260, "DxWnd Joystick 1");
        strcpy_s(DevInstance.tszProductName, 260, "DxWnd Virtual Joystick");
        BOOL vJoy = FALSE;
        // find out whether to show the virtual joystick or not
        switch(dwDevType) {
        case DI8DEVCLASS_ALL:		// 0 = all also for dinput 1-7
        case DI8DEVCLASS_GAMECTRL:	// same value as dinput 1-7 DIDEVTYPE_JOYSTICK
        case DI8DEVTYPE_JOYSTICK:	// dinput 8 only
            vJoy = TRUE;
            break;
        }
        // if yes create it and show it as first device ...
        if(vJoy) {
            OutTraceIN("%s: Virtual Joystick CALLBACK\n", ApiRef);
            res = (*lpCallback)(&DevInstance, pvRef);
            OutTraceIN("%s: CALLBACK ret=%#x\n", ApiRef, res);
            if(res == DIENUM_STOP) return DI_OK;
        }
    }
    Arg.cb = lpCallback;
    Arg.arg = pvRef;
    Arg.DIVersion = iDIVersion;
    res = (*pDIEnumDevices)( lpdi, dwDevType, (LPDIENUMDEVICESCALLBACKA)extDeviceProxy, &Arg, dwFlags); // V2.02.80 fix
    //res=(*pDIEnumDevices)( lpdi, dwDevType, lpCallback, pvRef, dwFlags);
    OutTraceIN("%s: res=%#x\n", ApiRef, res);
    return res;
}

HRESULT WINAPI extDIEnumDevicesA1(void *lpdi, DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags) {
    return extDIEnumDevicesA(1, pDIEnumDevicesA1, lpdi, dwDevType, lpCallback, pvRef, dwFlags);
}
HRESULT WINAPI extDIEnumDevicesA2(void *lpdi, DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags) {
    return extDIEnumDevicesA(2, pDIEnumDevicesA2, lpdi, dwDevType, lpCallback, pvRef, dwFlags);
}
HRESULT WINAPI extDIEnumDevicesA7(void *lpdi, DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags) {
    return extDIEnumDevicesA(7, pDIEnumDevicesA7, lpdi, dwDevType, lpCallback, pvRef, dwFlags);
}
HRESULT WINAPI extDIEnumDevicesA8(void *lpdi, DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags) {
    return extDIEnumDevicesA(8, pDIEnumDevicesA8, lpdi, dwDevType, lpCallback, pvRef, dwFlags);
}

HRESULT WINAPI extDIEnumDevicesW(int iDIVersion, DIEnumDevicesW_Type pDIEnumDevices, void *lpdi, DWORD dwDevType, LPDIENUMDEVICESCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags) {
    HRESULT res;
    ApiName("IDirectInput:EnumDevicesW");
    CallbackArg Arg;
    OutTraceIN("%s(%d): di=%#x DevType=%#x CallBack=%#x Ref=%#x Flags=%#x(%s)\n",
               ApiRef, iDIVersion, lpdi, dwDevType, lpCallback, pvRef, dwFlags, sEnumFlags(dwFlags));
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) {
        DIDEVICEINSTANCEW DevInstance;
        memset(&DevInstance, 0, sizeof(DevInstance));
        DevInstance.dwSize = sizeof(DIDEVICEINSTANCEW);
        DevInstance.dwDevType = // mimick vJoy
            DIDEVTYPE_HID |
            DIDFT_MAKEINSTANCE(1) |
            (iDIVersion == 8 ? DI8DEVTYPE_JOYSTICK : DIDEVTYPE_JOYSTICK);
        // if SKIPDEVTYPEHID pretend this device is NOT a HID!!
        if(dxw.dwFlags7 & SKIPDEVTYPEHID) DevInstance.dwDevType &= ~DIDEVTYPE_HID;
        memcpy(&DevInstance.guidFFDriver, &IFakeJoyFFDriver, sizeof(GUID));
        memcpy(&DevInstance.guidInstance, &IFakeJoyInstance, sizeof(GUID));
        memcpy(&DevInstance.guidProduct,  &IFakeJoyProduct,  sizeof(GUID));
        wcscpy_s(DevInstance.tszInstanceName, 260, L"DxWnd Joystick 1");
        wcscpy_s(DevInstance.tszProductName, 260, L"DxWnd Virtual Joystick");
        BOOL vJoy = FALSE;
        // find out whether to show the virtual joystick or not
        switch(dwDevType) {
        case DI8DEVCLASS_ALL:		// 0 = all also for dinput 1-7
        case DI8DEVCLASS_GAMECTRL:	// same value as dinput 1-7 DIDEVTYPE_JOYSTICK
        case DI8DEVTYPE_JOYSTICK:	// dinput 8 only
            vJoy = TRUE;
            break;
        }
        // if yes create it and show it as first device ...
        if(vJoy) {
            OutTraceIN("%s: Virtual Joystick CALLBACK\n", ApiRef);
            res = (*lpCallback)(&DevInstance, pvRef);
            OutTraceIN("%s: CALLBACK ret=%#x\n", ApiRef, res);
            if(res == DIENUM_STOP) return DI_OK;
        }
    }
    Arg.cb = (LPDIENUMDEVICESCALLBACKA)lpCallback;
    Arg.arg = pvRef;
    Arg.DIVersion = iDIVersion;
    res = (*pDIEnumDevices)( lpdi, dwDevType, (LPDIENUMDEVICESCALLBACKW)extDeviceProxy, &Arg, dwFlags); // V2.02.80 fix
    //res=(*pDIEnumDevices)( lpdi, dwDevType, lpCallback, pvRef, dwFlags);
    OutTraceIN("%s: res=%#x\n", ApiRef, res);
    return res;
}

HRESULT WINAPI extDIEnumDevicesW1(void *lpdi, DWORD dwDevType, LPDIENUMDEVICESCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags) {
    return extDIEnumDevicesW(1, pDIEnumDevicesW1, lpdi, dwDevType, lpCallback, pvRef, dwFlags);
}
HRESULT WINAPI extDIEnumDevicesW2(void *lpdi, DWORD dwDevType, LPDIENUMDEVICESCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags) {
    return extDIEnumDevicesW(2, pDIEnumDevicesW2, lpdi, dwDevType, lpCallback, pvRef, dwFlags);
}
HRESULT WINAPI extDIEnumDevicesW7(void *lpdi, DWORD dwDevType, LPDIENUMDEVICESCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags) {
    return extDIEnumDevicesW(7, pDIEnumDevicesW7, lpdi, dwDevType, lpCallback, pvRef, dwFlags);
}
HRESULT WINAPI extDIEnumDevicesW8(void *lpdi, DWORD dwDevType, LPDIENUMDEVICESCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags) {
    return extDIEnumDevicesW(8, pDIEnumDevicesW8, lpdi, dwDevType, lpCallback, pvRef, dwFlags);
}

HRESULT WINAPI extAcquire(LPDIRECTINPUTDEVICE lpdid) {
    HRESULT res;
    ApiName("IDirectInputDevice:Acquire");
    res = (*pAcquire)(lpdid);
    OutTraceIN("%s: lpdid=%#x(%s) res=%#x(%s)\n", ApiRef, lpdid, sDevice(lpdid), res, res == DI_NOEFFECT ? "DI_NOEFFECT" : ExplainDDError(res));
    if((dxw.dwFlags7 & SUPPRESSDIERRORS) && (res == DIERR_OTHERAPPHASPRIO)) res = DI_OK;
#ifdef RECOVERDATAFORMAT
    // beware: to get access to c_dfDIKeyboard, c_dfDIMouse and c_dfDIJoystick the dxwnd.dll must
    // be linked with dinput.lib !!
    if((RECOVERDATAFORMAT) && (res == DDERR_INVALIDPARAMS)) {
        LPDIDATAFORMAT lpDIDF;
        if(lpdid == lpDIDKeyboard) lpDIDF = &(DIDATAFORMAT)c_dfDIKeyboard;
        if(lpdid == lpDIDSysMouse) lpDIDF = &(DIDATAFORMAT)c_dfDIMouse;
        if(lpdid == lpDIDJoystick) lpDIDF = &(DIDATAFORMAT)c_dfDIJoystick;
        res = (*pSetDataFormat)(lpdid, lpDIDF);
        OutTraceIN("%s: SetDataFormat res=%#x\n", ApiRef, res);
        res = (*pAcquire)(lpdid);
        OutTraceIN("%s: Acquire res=%#x\n", ApiRef, res);
    }
#endif // RECOVERDATAFORMAT
    return res;
}

HRESULT WINAPI extUnacquire(LPDIRECTINPUTDEVICE lpdid) {
    HRESULT res;
    ApiName("IDirectInputDevice:Unacquire");
    res = (*pUnacquire)(lpdid);
    OutTraceIN("%s: lpdid=%#x(%s) res=%#x(%s)\n", ApiRef, lpdid, sDevice(lpdid), res, res == DI_NOEFFECT ? "DI_NOEFFECT" : ExplainDDError(res));
    if((dxw.dwFlags7 & SUPPRESSDIERRORS) && (res == DIERR_OTHERAPPHASPRIO)) res = DI_OK;
    return res;
}

void ToggleAcquiredDevices(BOOL flag) {
    // v2.04.90: thanks to huh investigation, the device unacquire operation on program termination can
    // produce an exception that may skip important operation of final cleanup. A try/except statement
    // is needed. Fixes "Sentinel Returns" that doesn't terminate properly with hanlde exceptions flag.
    __try {
        if(flag && pAcquire) {
            if(lpDIDSysMouse) (*pAcquire)(lpDIDSysMouse);
            if(lpDIDKeyboard) (*pAcquire)(lpDIDKeyboard);
            if(lpDIDJoystick) (*pAcquire)(lpDIDJoystick);
            if (dxw.dwFlags1 & CLIPCURSOR) dxw.SetClipCursor();
        }
        if(!flag && pUnacquire) {
            if(lpDIDSysMouse) (*pUnacquire)(lpDIDSysMouse);
            if(lpDIDKeyboard) (*pUnacquire)(lpDIDKeyboard);
            if(lpDIDJoystick) (*pUnacquire)(lpDIDJoystick);
            if (dxw.dwFlags1 & CLIPCURSOR) dxw.EraseClipCursor();
        }
    } __except(TRUE) {
    }
}

static 	void RemapUSKey(DWORD ofs, char *name) {
    char *p = NULL;
    switch(ofs) {
    case   1:
        p = "Escape";
        break;
    case  14:
        p = "Backspace";
        break;
    case  15:
        p = "Tab";
        break;
    case  28:
        p = "Enter";
        break;
    case  29:
        p = "Left Ctrl";
        break;
    case  42:
        p = "Left Shift";
        break;
    case  54:
        p = "Right Shift";
        break;
    case  55:
        p = "Numpad *";
        break;
    case  56:
        p = "Left Alt";
        break;
    case  57:
        p = "Space";
        break;
    case  58:
        p = "CapsLock";
        break;
    case  69:
        p = "NumLock";
        break;
    case  70:
        p = "ScrollLock";
        break;
    case  71:
        p = "Numpad 7";
        break;
    case  72:
        p = "Numpad 8";
        break;
    case  73:
        p = "Numpad 9";
        break;
    case  74:
        p = "Numpad -";
        break;
    case  75:
        p = "Numpad 4";
        break;
    case  76:
        p = "Numpad 5";
        break;
    case  77:
        p = "Numpad 6";
        break;
    case  78:
        p = "Numpad +";
        break;
    case  79:
        p = "Numpad 1";
        break;
    case  80:
        p = "Numpad 2";
        break;
    case  81:
        p = "Numpad 3";
        break;
    case  82:
        p = "Numpad 0";
        break;
    case  83:
        p = "Numpad .";
        break;
    case  86:
        p = "OEM Key 102";
        break;
    case 115:
        p = "Non-US / ?";
        break;
    case 126:
        p = "Non-US Numpad .";
        break;
    case 144:
        p = "Prev Track";
        break;
    case 153:
        p = "Next Track";
        break;
    case 156:
        p = "Numpad Enter";
        break;
    case 157:
        p = "Right CTRL";
        break;
    case 160:
        p = "Mute";
        break;
    case 161:
        p = "Calculator";
        break;
    case 162:
        p = "Play/Pause";
        break;
    case 164:
        p = "Media Stop";
        break;
    case 174:
        p = "Volume -";
        break;
    case 176:
        p = "Volume +";
        break;
    case 178:
        p = "Web/Home";
        break;
    case 181:
        p = "Numpad /";
        break;
    case 183:
        p = "SysRq";
        break;
    case 184:
        p = "Right Alt";
        break;
    case 197:
        p = "Pause";
        break;
    case 199:
        p = "Home";
        break;
    case 200:
        p = "Up Arrow";
        break;
    case 201:
        p = "PgUp";
        break;
    case 203:
        p = "Left Arrow";
        break;
    case 205:
        p = "Right Arrow";
        break;
    case 207:
        p = "End";
        break;
    case 208:
        p = "Down Arrow";
        break;
    case 209:
        p = "PgDn";
        break;
    case 210:
        p = "Insert";
        break;
    case 211:
        p = "Delete";
        break;
    case 219:
        p = "Left Win";
        break;
    case 220:
        p = "Right Win";
        break;
    case 221:
        p = "AppMenu";
        break;
    case 222:
        p = "Power";
        break;
    case 223:
        p = "Sleep";
        break;
    case 227:
        p = "Wake";
        break;
    case 229:
        p = "Search";
        break;
    case 230:
        p = "Favorites";
        break;
    case 231:
        p = "Refresh";
        break;
    case 232:
        p = "Web Stop";
        break;
    case 233:
        p = "Forward";
        break;
    case 234:
        p = "Back";
        break;
    case 235:
        p = "My Computer";
        break;
    case 236:
        p = "Mail";
        break;
    case 237:
        p = "Media";
        break;
    }
    if(p) {
        OutDebugIN("RemapUSKey: remap \"%s\" -> \"%s\"\n", name, p);
        strncpy(name, p, 260);
    }
}

typedef struct {
    LPDIENUMDEVICEOBJECTSCALLBACK cb;
    LPVOID arg;
} ObjCallbackArg;

typedef BOOL (FAR PASCAL *LPDIENUMDEVICEOBJECTSCALLBACKA)(LPCDIDEVICEOBJECTINSTANCEA, LPVOID);

HRESULT WINAPI extEnumObjectsProxy(LPCDIDEVICEOBJECTINSTANCE dev, LPVOID arg) {
    HRESULT res;
    ApiName("IDirectInputDevice:EnumObjects");
    char *p;
    LPCDIDEVICEOBJECTINSTANCEA devA;
    LPCDIDEVICEOBJECTINSTANCEW devW;
    switch (dev->dwSize) {
    case sizeof(DIDEVICEOBJECTINSTANCEA):
        p = "ASCII";
        devA = (LPCDIDEVICEOBJECTINSTANCEA)dev;
        OutTraceIN("%s: CALLBACK size=%d(%s) type=%#x flags=%#x ofs=%d dimensions=%d name=\"%s\"\n",
                   ApiRef, devA->dwSize, p, devA->dwType, devA->dwFlags, devA->dwOfs, devA->dwDimension, devA->tszName);
        if ((devA->guidType == GUID_Key) && (dxw.dwFlags11 & SETUSKEYDESCR)) RemapUSKey(devA->dwOfs, (char *)&devA->tszName[0]);
        break;
    case sizeof(DIDEVICEOBJECTINSTANCEW):
        p = "WIDECHAR";
        devW = (LPCDIDEVICEOBJECTINSTANCEW)dev;
        OutTraceIN("%s: CALLBACK size=%d(%s) type=%#x flags=%#x ofs=%d dimensions=%d name=%ls\n",
                   ApiRef, devW->dwSize, p, devW->dwType, devW->dwFlags, devW->dwOfs, devW->dwDimension, devW->tszName);
        break;
    default:
        p = "UNKNOWN";
        OutTraceE("%s: unknown struct size=%d\n", ApiRef, dev->dwSize);
        break;
    }
    res = (*(((ObjCallbackArg *)arg)->cb))(dev, ((CallbackArg *)arg)->arg);
    OutTraceIN("%s: CALLBACK ret=%#x\n", ApiRef, res);
    return res;
}

HRESULT WINAPI extEnumObjectsA(LPDIRECTINPUTDEVICE lpdid, LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags) {
    HRESULT res;
    ApiName("IDirectInputDevice:EnumObjectsA");
    ObjCallbackArg Arg;
    Arg.cb = lpCallback;
    Arg.arg = pvRef;
    OutTraceIN("%s: lpdid=%#x ref=%#x flags=%#x(%s)\n", ApiRef, lpdid, pvRef, dwFlags, sEnumObjectFlags(dwFlags));
    res = (*pEnumObjectsA)( lpdid, (LPDIENUMDEVICEOBJECTSCALLBACKA)extEnumObjectsProxy, &Arg, dwFlags);
    //res = (*pEnumObjects)(lpdid, lpCallback, pvRef, dwFlags);
    OutTraceIN("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extEnumObjectsW(LPDIRECTINPUTDEVICE lpdid, LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags) {
    HRESULT res;
    ApiName("IDirectInputDevice:EnumObjectsW");
    ObjCallbackArg Arg;
    Arg.cb = (LPDIENUMDEVICEOBJECTSCALLBACKA)lpCallback;
    Arg.arg = pvRef;
    OutTraceIN("%s: lpdid=%#x ref=%#x flags=%#x(%s)\n", ApiRef, lpdid, pvRef, dwFlags, sEnumObjectFlags(dwFlags));
    res = (*pEnumObjectsW)( lpdid, (LPDIENUMDEVICEOBJECTSCALLBACKW)extEnumObjectsProxy, &Arg, dwFlags);
    //res = (*pEnumObjects)(lpdid, lpCallback, pvRef, dwFlags);
    OutTraceIN("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extDIGetDeviceStatus8(void *lpdi, REFGUID rguid) {
    HRESULT res;
    ApiName("IDirectInput8:GetDeviceStatus");
    OutTraceIN("%s: lpdi=%#x guid=%s\n", ApiRef, lpdi, sGUID((GUID *)&rguid));
    // v2.05.44: virtual joystick case
    if((dxw.dwFlags6 & VIRTUALJOYSTICK) && (rguid.Data1 == IFakeJoyInstance.Data1)) {
        OutTraceIN("%s: lpdi=%#x VIRTUALJOY res=DI_OK\n", ApiRef, lpdi);
        return DI_OK;
    }
    res = (*pDIGetDeviceStatus8)(lpdi, rguid);
    OutTraceIN("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

#ifdef TRACEALL
HRESULT WINAPI extPoll(LPDIRECTINPUTDEVICE lpdid) {
    HRESULT res;
    ApiName("IDirectInputDevice:Poll");
    OutDebugIN("%s: lpdid=%#x\n", ApiRef, lpdid);
    res = (*pPoll)(lpdid);
    OutDebugIN("%s: res=%#x(%s)\n", ApiRef, res, (res == DI_NOEFFECT) ? "NOEFFECT" : ExplainDDError(res));
    return res;
}

HRESULT WINAPI extGetDeviceInfoA(LPDIRECTINPUTDEVICE lpdid, LPDIDEVICEINSTANCEA lpidi) {
    HRESULT res;
    ApiName("IDirectInputDevice:GetDeviceInfoA");
    OutTraceIN("%s: did=%#x size=%d\n", ApiRef, lpdid, lpidi->dwSize);
    res = (*pGetDeviceInfoA)(lpdid, lpidi);
    if(res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceIN("%s: guidInst=%#x guidProd=%#x devtype=%#x name=\"%s\" prod=\"%s\" guidDrv=%#x usagepage=%#x usage=%#x\n",
               ApiRef,
               lpidi->guidInstance.Data1, lpidi->guidProduct.Data1, lpidi->dwDevType,
               lpidi->tszInstanceName, lpidi->tszProductName, lpidi->guidFFDriver.Data1,
               lpidi->wUsagePage, lpidi->wUsage);
    return DI_OK;
}

HRESULT WINAPI extGetDeviceInfoW(LPDIRECTINPUTDEVICE lpdid, LPDIDEVICEINSTANCEW lpidi) {
    HRESULT res;
    ApiName("IDirectInputDevice:GetDeviceInfoW");
    OutTraceIN("%s: did=%#x size=%d\n", ApiRef, lpdid, lpidi->dwSize);
    res = (*pGetDeviceInfoW)(lpdid, lpidi);
    if(res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceIN("%s: guidInst=%#x guidProd=%#x devtype=%#x name=\"%ls\" prod=\"%ls\" guidDrv=%#x usagepage=%#x usage=%#x\n",
               ApiRef,
               lpidi->guidInstance.Data1, lpidi->guidProduct.Data1, lpidi->dwDevType,
               lpidi->tszInstanceName, lpidi->tszProductName, lpidi->guidFFDriver.Data1,
               lpidi->wUsagePage, lpidi->wUsage);
    return DI_OK;
}

HRESULT WINAPI extSetEventNotification(LPDIRECTINPUTDEVICE lpdid, HANDLE hwnd) {
    HRESULT res;
    ApiName("IDirectInputDevice:SetEventNotification");
    OutTraceIN("%s: lpdid=%#x hwnd=%#x\n", ApiRef, lpdid, hwnd);
    res = (*pSetEventNotification)(lpdid, hwnd);
    if(res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extGetObjectInfoA(LPDIRECTINPUTDEVICE lpdid, LPDIDEVICEOBJECTINSTANCEA lpidoi, DWORD dwObj, DWORD dwHow) {
    HRESULT res;
    ApiName("IDirectInputDevice:GetObjectInfoA");
    OutTraceIN("%s: did=%#x obj=%#x how=%#x(%s)\n", ApiRef, lpdid, dwObj, dwHow, sHow(dwHow));
    res = (*pGetObjectInfoA)(lpdid, lpidoi, dwObj, dwHow);
    if(res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceIN("%s: ofs=%d flags=%#x guid=%#x name=\"%s\"\n",
               ApiRef, lpidoi->dwOfs, lpidoi->dwFlags, lpidoi->guidType.Data1, lpidoi->tszName);
    return (DI_OK);
}

HRESULT WINAPI extGetObjectInfoW(LPDIRECTINPUTDEVICE lpdid, LPDIDEVICEOBJECTINSTANCEW lpidoi, DWORD dwObj, DWORD dwHow) {
    HRESULT res;
    ApiName("IDirectInputDevice:GetObjectInfoW");
    OutTraceIN("%s: did=%#x obj=%#x how=%#x(%s)\n", ApiRef, lpdid, dwObj, dwHow, sHow(dwHow));
    res = (*pGetObjectInfoW)(lpdid, lpidoi, dwObj, dwHow);
    if(res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceIN("%s: ofs=%d flags=%#x guid=%#x name=\"%ls\"\n",
               ApiRef, lpidoi->dwOfs, lpidoi->dwFlags, lpidoi->guidType.Data1, lpidoi->tszName);
    return (DI_OK);
}

HRESULT WINAPI extGetCapabilities(LPDIRECTINPUTDEVICE lpdid, LPDIDEVCAPS lpdidc) {
    HRESULT res;
    ApiName("IDirectInputDevice:GetCapabilities");
    res = (*pGetCapabilities)(lpdid, lpdidc);
    if(res)
        OutTraceE("%s: ERROR lpdid=%#x ret=%#x(%s)\n", ApiRef, lpdid, res, ExplainDDError(res));
    else {
        OutTraceIN("%s: lpdid=%#x "
                   "cap={size=%d flags=%#x devtype=%#x axes=%d buttons=%d povs=%d mintimeres=%d sampleperiod=%d "
                   "version(HW/FW/SW)=(%#x/%#x/%#x)}\n",
                   ApiRef,
                   lpdid,
                   lpdidc->dwSize,
                   lpdidc->dwFlags,
                   lpdidc->dwDevType,
                   lpdidc->dwAxes,
                   lpdidc->dwButtons,
                   lpdidc->dwPOVs,
                   lpdidc->dwFFMinTimeResolution,
                   lpdidc->dwFFSamplePeriod,
                   lpdidc->dwHardwareRevision,
                   lpdidc->dwFirmwareRevision,
                   lpdidc->dwFFDriverVersion
                  );
    }
    return res;
}

HRESULT WINAPI extSetProperty(LPDIRECTINPUTDEVICE lpdid, REFGUID rguidProp, LPCDIPROPHEADER lpdph) {
    HRESULT res;
    ApiName("IDirectInputDevice:SetProperty");
    OutTraceIN("%s: lpdid=%#x guid=(%s) "
               "ph={size=%d hdrsize=%d how=%#x(%s) obj=%#x}\n",
               ApiRef,
               lpdid,
               sGUIDProperty(rguidProp),
               lpdph->dwSize,
               lpdph->dwHeaderSize,
               lpdph->dwHow, sPropHow(lpdph->dwHow),
               lpdph->dwObj);
    DWORD dwGUID = (DWORD)&rguidProp;
    if(dwGUID == 4) { // DIPROP_RANGE
        LPDIPROPRANGE lpRange = (LPDIPROPRANGE)lpdph;
        OutTraceIN("> RANGE(min:max)=(%d:%d)\n", lpRange->lMin, lpRange->lMax);
    }
    res = (*pSetProperty)(lpdid, rguidProp, lpdph);
    if(res != DI_OK)
        OutTraceE("%s: ERROR ret=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else
        OutTraceIN("%s: res=OK\n", ApiRef);
    if((dxw.dwFlags7 & SUPPRESSDIERRORS) && (res == DIERR_UNSUPPORTED)) res = DI_OK;
    return res;
}

HRESULT WINAPI extGetProperty(LPDIRECTINPUTDEVICE lpdid, REFGUID rguidProp, LPDIPROPHEADER lpdph) {
    HRESULT res;
    ApiName("IDirectInputDevice:GetProperty");
    OutTraceIN("%s: lpdid=%#x guid=(%s) "
               "ph={size=%d hdrsize=%d how=%#x(%s) obj=%#x}\n",
               ApiRef,
               lpdid,
               sGUIDProperty(rguidProp),
               lpdph->dwSize,
               lpdph->dwHeaderSize,
               lpdph->dwHow, sPropHow(lpdph->dwHow),
               lpdph->dwObj);
    res = (*pGetProperty)(lpdid, rguidProp, lpdph);
    if(res != DI_OK) {
        OutTraceE("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    DWORD prop = (DWORD)&rguidProp;
    LPDIPROPRANGE lpRange = (LPDIPROPRANGE)lpdph;
    LPDIPROPPOINTER lpData = (LPDIPROPPOINTER)lpdph;
    switch(prop) {
    case &DIPROP_APPDATA:
        OutTraceIN("> appdata=%x\n", lpData->uData);
        break;
    case &DIPROP_AUTOCENTER:
        OutTraceIN("> autocenter=%x(%s)\n", lpData->uData, lpData->uData == DIPROPAUTOCENTER_OFF ? "OFF" : "ON");
        break;
    case &DIPROP_AXISMODE:
        OutTraceIN("> axismode=%x(%s)\n", lpData->uData, lpData->uData == DIPROPAXISMODE_ABS  ? "ABS" : "REL");
        break;
    case &DIPROP_BUFFERSIZE:
        OutTraceIN("> buffersize=%d\n", lpData->uData);
        break;
    case &DIPROP_DEADZONE:
        OutTraceIN("> deadzone=%d\n", lpData->uData);
        break;
    case &DIPROP_RANGE:
        OutTraceIN("> RANGE(min:max)=(%d:%d)\n", lpRange->lMin, lpRange->lMax);
        break;
    case &DIPROP_JOYSTICKID:
        OutTraceIN("> joystickid=%d\n", lpData->uData);
        break;
    case &DIPROP_LOGICALRANGE:
        OutTraceIN("> LOGICALRANGE(min:max)=(%d:%d)\n", lpRange->lMin, lpRange->lMax);
        break;
    case &DIPROP_PHYSICALRANGE:
        OutTraceIN("> PHYSICALRANGE(min:max)=(%d:%d)\n", lpRange->lMin, lpRange->lMax);
        break;
    default:
        OutTraceIN("> unknown: %#x\n", lpData->uData);
        break;
    }
    OutTraceIN("%s: res=OK\n", ApiRef);
    return res;
}

HRESULT WINAPI extCreateEffect(LPDIRECTINPUTDEVICE lpdid, REFGUID giod, LPCDIEFFECT lpe, LPDIRECTINPUTEFFECT *lpdie, LPUNKNOWN lpu) {
    HRESULT res;
    ApiName("IDirectInputDevice:CreateEffect");
    OutTraceIN("%s: lpdid=%#x\n", ApiRef, lpdid);
    res = (*pCreateEffect)(lpdid, giod, lpe, lpdie, lpu);
    OutTraceIN("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extRunControlPanel(LPDIRECTINPUTDEVICE lpdid, HWND hwndOwner, DWORD dwFlags) {
    HRESULT res;
    ApiName("IDirectInputDevice:RunControlPanel");
    OutTraceIN("%s: lpdid=%#x hwnd=%#x flags=%#x\n", ApiRef, lpdid, hwndOwner, dwFlags);
    res = (*pRunControlPanel)(lpdid, hwndOwner, dwFlags);
    OutTraceIN("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extInitialize(LPDIRECTINPUTDEVICE lpdid, HINSTANCE hinst, DWORD dwVersion, REFGUID rguid) {
    HRESULT res;
    ApiName("IDirectInputDevice:Initialize");
    // BEWARE: trying to dump rguid causes crashes ...
    OutTraceIN("%s: lpdid=%#x hinst=%#x version=%d rguid=???\n", ApiRef, lpdid, hinst, dwVersion);
    res = (*pInitialize)(lpdid, hinst, dwVersion, rguid);
    OutTraceIN("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extDIFindDeviceA8(void *lpdi, REFGUID rguidClass, LPCSTR ptszName, LPGUID pguidInstance) {
    HRESULT res;
    ApiName("IDirectInput8:FindDeviceA");
    OutTraceIN("%s: lpdi=%#x refguid=%#x(%s) name=\"%s\" lpguid=%#x\n",
               ApiRef, lpdi, rguidClass, ExplainGUID((GUID *)&rguidClass), ptszName, pguidInstance);
    res = (*pDIFindDeviceA8)(lpdi, rguidClass, ptszName, pguidInstance);
    OutTraceIN("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

static char *sDIEnumFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DIEDBSFL_");
    if (c & DIEDBSFL_ATTACHEDONLY) strcat(eb, "ATTACHEDONLY+");
    if (c & DIEDBSFL_AVAILABLEDEVICES) strcat(eb, "AVAILABLEDEVICES+");
    if (c & DIEDBSFL_FORCEFEEDBACK) strcat(eb, "FORCEFEEDBACK+");
    if (c & DIEDBSFL_MULTIMICEKEYBOARDS) strcat(eb, "MULTIMICEKEYBOARDS+");
    if (c & DIEDBSFL_NONGAMINGDEVICES) strcat(eb, "NONGAMINGDEVICES+");
    if (c & DIEDBSFL_THISUSER) strcat(eb, "THISUSER+");
    if (c & DIEDBSFL_VALID) strcat(eb, "VALID+");
    l = strlen(eb);
    if (l > strlen("DIEDBSFL_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

//    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;

HRESULT WINAPI extDIInitialize8(void *lpdi, HINSTANCE hinst, DWORD flags) {
    HRESULT res;
    ApiName("IDirectInput8:Initialize");
    OutTraceIN("%s: lpdi=%#x hinst=%#x flags=%#x\n", ApiRef, lpdi, hinst, flags);
    res = (*pDIInitialize8)(lpdi, hinst, flags);
    OutTraceIN("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

typedef BOOL (WINAPI *DIEnumDevicesBySemanticsCallback_Type)(LPCDIDEVICEINSTANCE, LPDIRECTINPUTDEVICE8,  DWORD, DWORD, LPVOID);
typedef struct {
    LPDIENUMDEVICESBYSEMANTICSCB pCallback;
    LPVOID pvRef;
} CBackArg;

static char *sCBackFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DIEDBS_");
    if (c & DIEDBS_MAPPEDPRI1) strcat(eb, "MAPPEDPRI1+");
    if (c & DIEDBS_MAPPEDPRI2) strcat(eb, "MAPPEDPRI2+");
    if (c & DIEDBS_RECENTDEVICE) strcat(eb, "RECENTDEVICE+");
    if (c & DIEDBS_NEWDEVICE) strcat(eb, "NEWDEVICE+");
    l = strlen(eb);
    if (l > strlen("DIEDBS_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

BOOL WINAPI extDIEnumDevicesBySemanticsCallback(LPCDIDEVICEINSTANCE lpddi, LPDIRECTINPUTDEVICE8 lpdid, DWORD dwFlags, DWORD dwRemaining, LPVOID pvRef) {
    BOOL res;
    ApiName("EnumDevicesBySemanticsCallback");
    CBackArg *arg = (CBackArg *)pvRef;
    //MessageBox(0, "calling EnumDevicesBySemanticsCallback", "debug", 0);
    OutDebugIN("%s: lpddi=%x lpdid=%#x flags=%#x(%s) remaining=%#x ref=%#x\n", ApiRef, lpddi, lpdid, dwFlags, sCBackFlags(dwFlags), dwRemaining, arg->pvRef);
    res = (*(arg->pCallback))(lpddi, lpdid, dwFlags, dwRemaining, arg->pvRef);
    OutDebugIN("%s: res=%#x\n", ApiRef, res);
    return res;
}

HRESULT WINAPI extEnumDevicesBySemantics(void *lpdi, LPCTSTR ptszUserName, LPDIACTIONFORMAT lpdiActionFormat, LPDIENUMDEVICESBYSEMANTICSCB lpCallback, LPVOID pvRef, DWORD dwFlags) {
    HRESULT res;
    CBackArg arg;
    ApiName("IDirectInput8:EnumDevicesBySemantics");
    OutTraceIN("%s: lpdi=%#x username=\"%s\" flags=%#x(%s)\n",
               ApiRef, lpdi, ptszUserName, dwFlags, sDIEnumFlags(dwFlags));
    if(IsDebugIN) {
        arg.pCallback = lpCallback;
        arg.pvRef = pvRef;
        res = (*pEnumDevicesBySemantics)(lpdi, ptszUserName, lpdiActionFormat, extDIEnumDevicesBySemanticsCallback, (LPVOID)&arg, dwFlags);
    } else
        res = (*pEnumDevicesBySemantics)(lpdi, ptszUserName, lpdiActionFormat, lpCallback, pvRef, dwFlags);
    OutTraceIN("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

char *sDIActionMapFlags(DWORD c) {
    static char eb[64];
    unsigned int l;
    strcpy(eb, "DIDBAM_");
    if (c & DIDBAM_HWDEFAULTS) strcat(eb, "HWDEFAULTS+");
    if (c & DIDBAM_INITIALIZE) strcat(eb, "INITIALIZE+");
    if (c & DIDBAM_PRESERVE) strcat(eb, "PRESERVE+");
    l = strlen(eb);
    if (l > strlen("DIDBAM_")) eb[l - 1] = 0; // delete last '+' if any
    else strcat(eb, "DEFAULT");
    return(eb);
}

HRESULT WINAPI extBuildActionMapA(void *lpdi, LPDIACTIONFORMATA lpdiaf, LPCSTR lpszUserName, DWORD dwFlags) {
    HRESULT res;
    ApiName("IDirectInput8Device:BuildActionMap");
    OutTraceIN("%s: lpdi=%#x lpdiaf=%#x username=\"%s\" flags=%#x(%s)\n",
               ApiRef, lpdi, lpdiaf, lpszUserName, dwFlags, sDIActionMapFlags(dwFlags));
    res = (*pBuildActionMapA)(lpdi, lpdiaf, lpszUserName, dwFlags);
    OutTraceIN("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extSetActionMapA(void *lpdi, LPDIACTIONFORMATA lpdiaf, LPCSTR lpszUserName, DWORD dwFlags) {
    HRESULT res;
    ApiName("IDirectInput8Device:SetActionMap");
    OutTraceIN("%s: lpdi=%#x lpdiaf=%#x username=\"%s\" flags=%#x(%s)\n",
               ApiRef, lpdi, lpdiaf, lpszUserName, dwFlags, sDIActionMapFlags(dwFlags));
    res = (*pSetActionMapA)(lpdi, lpdiaf, lpszUserName, dwFlags);
    OutTraceIN("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extConfigureDevicesA8(void *lpdi, LPDICONFIGUREDEVICESCALLBACK lpdiCallback, LPDICONFIGUREDEVICESPARAMS lpdiCDParams, DWORD dwFlags, LPVOID pvRefData) {
    HRESULT res;
    ApiName("IDirectInput8:ConfigureDevices");
    OutTraceIN("%s: lpdi=%#x flags=%#x(%s) refdata=%#x\n", ApiRef, lpdi, dwFlags, pvRefData);
    res = (*pConfigureDevicesA8)(lpdi, lpdiCallback, lpdiCDParams, dwFlags, pvRefData);
    OutTraceIN("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}
#endif
