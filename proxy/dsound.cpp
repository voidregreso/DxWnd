//------------------------------------------------------------------------------
// Tiny WinG32 Proxy DLL v1.0 [30.01.2017]
// Copyright 2010 Evgeny Vrublevsky <veg@tut.by>, 2017 GHO
//------------------------------------------------------------------------------
#define APP_NAME		"Tiny WinG32 Proxy"
#define APP_VERSION		"1.0 [28.05.2010]"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <ctype.h>

void InitDxWnd(char *);
void DxWndEndHook(void);

struct wing32_dll {
    HMODULE dll;
    FARPROC	DirectSoundCaptureCreate;
    FARPROC	DirectSoundCaptureEnumerateA;
    FARPROC	DirectSoundCaptureEnumerateW;
    FARPROC	DirectSoundCreate;
    FARPROC	DirectSoundEnumerateA;
    FARPROC	DirectSoundEnumerateW;
} dsound;

__declspec(naked) void FakeDirectSoundCaptureCreate()		{
    _asm { jmp [dsound.DirectSoundCaptureCreate] }
}
__declspec(naked) void FakeDirectSoundCaptureEnumerateA()	{
    _asm { jmp [dsound.DirectSoundCaptureEnumerateA] }
}
__declspec(naked) void FakeDirectSoundCaptureEnumerateW()	{
    _asm { jmp [dsound.DirectSoundCaptureEnumerateW] }
}
__declspec(naked) void FakeDirectSoundCreate()				{
    _asm { jmp [dsound.DirectSoundCreate] }
}
__declspec(naked) void FakeDirectSoundEnumerateA()			{
    _asm { jmp [dsound.DirectSoundEnumerateA] }
}
__declspec(naked) void FakeDirectSoundEnumerateW()			{
    _asm { jmp [dsound.DirectSoundEnumerateW] }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    char path[MAX_PATH];
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        int fnamelen = strlen("\\dsound.dll");
        CopyMemory(path + GetSystemDirectory(path, MAX_PATH - 10), "\\dsound.dll", fnamelen + 1); // copy +1 byte for string terminator
        dsound.dll = LoadLibrary(path);
        if (dsound.dll == false) {
            char msg[512 + 1];
            sprintf_s(msg, 512, "Cannot load original dsound.dll library\nerror=%d\npath=\"%s\"", GetLastError(), path);
            MessageBox(0, msg, "dsound proxy", MB_ICONERROR);
            ExitProcess(0);
        }
        dsound.DirectSoundCaptureCreate		= GetProcAddress(dsound.dll, "DirectSoundCaptureCreate");
        dsound.DirectSoundCaptureEnumerateA	= GetProcAddress(dsound.dll, "DirectSoundCaptureEnumerateA");
        dsound.DirectSoundCaptureEnumerateW	= GetProcAddress(dsound.dll, "DirectSoundCaptureEnumerateW");
        dsound.DirectSoundCreate			= GetProcAddress(dsound.dll, "DirectSoundCreate");
        dsound.DirectSoundEnumerateA		= GetProcAddress(dsound.dll, "DirectSoundEnumerateA");
        dsound.DirectSoundEnumerateW		= GetProcAddress(dsound.dll, "DirectSoundEnumerateW");
        InitDxWnd("WinG32");
        break;
    }
    case DLL_PROCESS_DETACH: {
        DxWndEndHook();
        FreeLibrary(dsound.dll);
    }
    break;
    }
    return TRUE;
}
