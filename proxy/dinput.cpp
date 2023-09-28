//------------------------------------------------------------------------------
// Tiny DirectDraw Proxy DLL v1.0 [28.05.2010]
// Copyright 2010 Evgeny Vrublevsky <veg@tut.by>
//------------------------------------------------------------------------------
#define APP_NAME		"Tiny DirectInput Proxy"
#define APP_VERSION		"1.0 [30.07.2017]"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <ctype.h>

void InitDxWnd(char *);
void DxWndEndHook(void);

struct ddraw_dll {
    HMODULE dll;
    FARPROC	DirectInputCreateA;
    FARPROC	DirectInputCreateW;
    FARPROC	DirectInputCreateEx;
    FARPROC	DllCanUnloadNow;
    FARPROC	DllGetClassObject;
    FARPROC	DllRegisterServer;
    FARPROC	DllUnregisterServer;
} dinput;

__declspec(naked) void FakeDirectInputCreateA()			{
    _asm { jmp [dinput.DirectInputCreateA] }
}
__declspec(naked) void FakeDirectInputCreateW()			{
    _asm { jmp [dinput.DirectInputCreateW] }
}
__declspec(naked) void FakeDirectInputCreateEx()		{
    _asm { jmp [dinput.DirectInputCreateEx] }
}
__declspec(naked) void FakeDllCanUnloadNow()			{
    _asm { jmp [dinput.DllCanUnloadNow] }
}
__declspec(naked) void FakeDllGetClassObject()			{
    _asm { jmp [dinput.DllGetClassObject] }
}
__declspec(naked) void FakeDllRegisterServer()			{
    _asm { jmp [dinput.DllRegisterServer] }
}
__declspec(naked) void FakeDllUnregisterServer()		{
    _asm { jmp [dinput.DllUnregisterServer] }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    char path[MAX_PATH];
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        int fnamelen = strlen("\\dinput.dll");
        CopyMemory(path + GetSystemDirectory(path, MAX_PATH - (fnamelen + 1)), "\\dinput.dll", fnamelen + 1); // copy +1 byte for string terminator
        dinput.dll = LoadLibrary(path);
        if (dinput.dll == false) {
            char msg[512 + 1];
            sprintf_s(msg, 512, "Cannot load original dinput.dll library\nerror=%d\npath=\"%s\"", GetLastError(), path);
            MessageBox(0, msg, "dinput proxy", MB_ICONERROR);
            ExitProcess(0);
        }
        dinput.DirectInputCreateA			= GetProcAddress(dinput.dll, "DirectInputCreateA");
        dinput.DirectInputCreateW			= GetProcAddress(dinput.dll, "DirectInputCreateW");
        dinput.DirectInputCreateEx			= GetProcAddress(dinput.dll, "DirectInputCreateEx");
        dinput.DllCanUnloadNow				= GetProcAddress(dinput.dll, "DllCanUnloadNow");
        dinput.DllGetClassObject			= GetProcAddress(dinput.dll, "DllGetClassObject");
        dinput.DllRegisterServer			= GetProcAddress(dinput.dll, "DllRegisterServer");
        dinput.DllUnregisterServer			= GetProcAddress(dinput.dll, "DllUnregisterServer");
        InitDxWnd("dinput");
        break;
    }
    case DLL_PROCESS_DETACH: {
        DxWndEndHook();
        FreeLibrary(dinput.dll);
    }
    break;
    }
    return TRUE;
}
