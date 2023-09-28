//------------------------------------------------------------------------------
// (not so)Tiny(anymore) DirectDraw Proxy DLL v1.0 [28.05.2010]
// Copyright 2010 Evgeny Vrublevsky <veg@tut.by>
// Copied and adapted by GHO for trivial tracing
//------------------------------------------------------------------------------

#define  _CRT_SECURE_NO_WARNINGS

#define APP_NAME		"Tiny DirectDraw Proxy Trace"
#define APP_VERSION		"1.0 [31.08.2015]"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include "dxdiag.h"
#include "ddraw.h"
#include "proxy.h"

void InitDxWndTrace(void);
void DxWndEndTrace(void);

#define INNERHOOK

// ddraw dll api
typedef HRESULT (WINAPI *DirectDrawCreateEx_Type)(GUID *, LPDIRECTDRAW *, void *, void FAR *);
typedef HRESULT (WINAPI *DirectDrawCreate_Type)(GUID *, LPDIRECTDRAW *, void FAR *);

struct ddraw_dll {
    HMODULE dll;
    FARPROC	AcquireDDThreadLock;
    FARPROC	CheckFullscreen;
    FARPROC	CompleteCreateSysmemSurface;
    FARPROC	D3DParseUnknownCommand;
    FARPROC	DDGetAttachedSurfaceLcl;
    FARPROC	DDInternalLock;
    FARPROC	DDInternalUnlock;
    FARPROC	DSoundHelp;
    DirectDrawCreate_Type DirectDrawCreate;
    FARPROC	DirectDrawCreateClipper;
    DirectDrawCreateEx_Type	DirectDrawCreateEx;
    FARPROC	DirectDrawEnumerateA;
    FARPROC	DirectDrawEnumerateExA;
    FARPROC	DirectDrawEnumerateExW;
    FARPROC	DirectDrawEnumerateW;
    FARPROC	DllCanUnloadNow;
    FARPROC	DllGetClassObject;
    FARPROC	GetDDSurfaceLocal;
    FARPROC	GetOLEThunkData;
    FARPROC	GetSurfaceFromDC;
    FARPROC	RegisterSpecialCase;
    FARPROC	ReleaseDDThreadLock;
} ddraw;

__declspec(naked) void FakeAcquireDDThreadLock()			{ //trace("AcquireDDThreadLock\n");
    _asm { jmp [ddraw.AcquireDDThreadLock] }
}
__declspec(naked) void FakeCheckFullscreen()				{
    trace("CheckFullscreen\n");
    _asm { jmp [ddraw.CheckFullscreen] }
}
__declspec(naked) void FakeCompleteCreateSysmemSurface()	{
    trace("CompleteCreateSysmemSurface\n");
    _asm { jmp [ddraw.CompleteCreateSysmemSurface] }
}
__declspec(naked) void FakeD3DParseUnknownCommand()			{
    trace("D3DParseUnknownCommand\n");
    _asm { jmp [ddraw.D3DParseUnknownCommand] }
}
__declspec(naked) void FakeDDGetAttachedSurfaceLcl()		{
    trace("DDGetAttachedSurfaceLcl\n");
    _asm { jmp [ddraw.DDGetAttachedSurfaceLcl] }
}
__declspec(naked) void FakeDDInternalLock()					{
    trace("DDInternalLock\n");
    _asm { jmp [ddraw.DDInternalLock] }
}
__declspec(naked) void FakeDDInternalUnlock()				{
    trace("DDInternalUnlock\n");
    _asm { jmp [ddraw.DDInternalUnlock] }
}
__declspec(naked) void FakeDSoundHelp()						{
    trace("DSoundHelp\n");
    _asm { jmp [ddraw.DSoundHelp] }
}

#ifdef INNERHOOK
// HRESULT WINAPI DirectDrawCreate( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
HRESULT WINAPI FakeDirectDrawCreate( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter ) {
    HRESULT res;
    trace(">> DirectDrawCreate(guid=%s)\n", sGUID(lpGUID));
    res = ddraw.DirectDrawCreate(lpGUID, lplpDD, pUnkOuter );
    trace("<< DirectDrawCreate(lpDD=%x) res=%x(%s)\n", *lplpDD, res, ExplainDDError(res));
    HookDDSession(lplpDD, 1);
    return res;
}
#else
__declspec(naked) void FakeDirectDrawCreate()				{
    trace("DirectDrawCreate\n");
    _asm { jmp [ddraw.DirectDrawCreate] }
}
#endif

// HRESULT WINAPI DirectDrawCreateClipper( DWORD dwFlags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter );
__declspec(naked) void FakeDirectDrawCreateClipper()		{
    trace("DirectDrawCreateClipper\n");
    _asm { jmp [ddraw.DirectDrawCreateClipper] }
}
#ifdef INNERHOOK
//HRESULT WINAPI DirectDrawCreateEx(GUID FAR * lpGuid, LPVOID *lplpDD, REFIID iid, IUnknown FAR *pUnkOuter);
HRESULT WINAPI FakeDirectDrawCreateEx(GUID *lpGuid, LPDIRECTDRAW *lplpDD, void *iid, void FAR *pUnkOuter ) {
    HRESULT res;
    trace(">> DirectDrawCreateEx(guid=%s)\n", sGUID(lpGuid));
    res = ddraw.DirectDrawCreateEx(lpGuid, lplpDD, iid, pUnkOuter );
    trace("<< DirectDrawCreateEx(lpDD=%x) res=%x(%s)\n", *lplpDD, res, ExplainDDError(res));
    HookDDSession(lplpDD, 7);
    return res;
}
#else
__declspec(naked) void FakeDirectDrawCreateEx()				{
    trace("DirectDrawCreateEx\n");
    _asm { jmp [ddraw.DirectDrawCreateEx] }
}
#endif
// HRESULT WINAPI DirectDrawEnumerateA( LPDDENUMCALLBACKA lpCallback, LPVOID lpContext );
__declspec(naked) void FakeDirectDrawEnumerateA()			{
    trace("DirectDrawEnumerateA\n");
    _asm { jmp [ddraw.DirectDrawEnumerateA] }
}
// HRESULT WINAPI DirectDrawEnumerateExA( LPDDENUMCALLBACKEXA lpCallback, LPVOID lpContext, DWORD dwFlags );
__declspec(naked) void FakeDirectDrawEnumerateExA()			{
    trace("DirectDrawEnumerateExA\n");
    _asm { jmp [ddraw.DirectDrawEnumerateExA] }
}
// HRESULT WINAPI DirectDrawEnumerateExW( LPDDENUMCALLBACKEXW lpCallback, LPVOID lpContext, DWORD dwFlags );
__declspec(naked) void FakeDirectDrawEnumerateExW()			{
    trace("DirectDrawEnumerateExW\n");
    _asm { jmp [ddraw.DirectDrawEnumerateExW] }
}
// HRESULT WINAPI DirectDrawEnumerateW( LPDDENUMCALLBACKW lpCallback, LPVOID lpContext );
__declspec(naked) void FakeDirectDrawEnumerateW()			{
    trace("DirectDrawEnumerateW\n");
    _asm { jmp [ddraw.DirectDrawEnumerateW] }
}
__declspec(naked) void FakeDllCanUnloadNow()				{
    trace("DllCanUnloadNow\n");
    _asm { jmp [ddraw.DllCanUnloadNow] }
}
__declspec(naked) void FakeDllGetClassObject()				{
    trace("DllGetClassObject\n");
    _asm { jmp [ddraw.DllGetClassObject] }
}
__declspec(naked) void FakeGetDDSurfaceLocal()				{
    trace("GetDDSurfaceLocal\n");
    _asm { jmp [ddraw.GetDDSurfaceLocal] }
}
__declspec(naked) void FakeGetOLEThunkData()				{
    trace("GetOLEThunkData\n");
    _asm { jmp [ddraw.GetOLEThunkData] }
}
__declspec(naked) void FakeGetSurfaceFromDC()				{
    trace("GetSurfaceFromDC\n");
    _asm { jmp [ddraw.GetSurfaceFromDC] }
}
__declspec(naked) void FakeRegisterSpecialCase()			{
    trace("RegisterSpecialCase\n");
    _asm { jmp [ddraw.RegisterSpecialCase] }
}
__declspec(naked) void FakeReleaseDDThreadLock()			{ //trace("ReleaseDDThreadLock\n");
    _asm { jmp [ddraw.ReleaseDDThreadLock] }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    char path[MAX_PATH];
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        CopyMemory(path + GetSystemDirectory(path, MAX_PATH - 10), "\\ddraw.dll", 11);
        ddraw.dll = LoadLibrary(path);
        if (ddraw.dll == false) {
            MessageBox(0, "Cannot load original ddraw.dll library", APP_NAME, MB_ICONERROR);
            ExitProcess(0);
        }
        ddraw.AcquireDDThreadLock			= GetProcAddress(ddraw.dll, "AcquireDDThreadLock");
        ddraw.CheckFullscreen				= GetProcAddress(ddraw.dll, "CheckFullscreen");
        ddraw.CompleteCreateSysmemSurface	= GetProcAddress(ddraw.dll, "CompleteCreateSysmemSurface");
        ddraw.D3DParseUnknownCommand		= GetProcAddress(ddraw.dll, "D3DParseUnknownCommand");
        ddraw.DDGetAttachedSurfaceLcl		= GetProcAddress(ddraw.dll, "DDGetAttachedSurfaceLcl");
        ddraw.DDInternalLock				= GetProcAddress(ddraw.dll, "DDInternalLock");
        ddraw.DDInternalUnlock				= GetProcAddress(ddraw.dll, "DDInternalUnlock");
        ddraw.DSoundHelp					= GetProcAddress(ddraw.dll, "DSoundHelp");
        ddraw.DirectDrawCreate				= (DirectDrawCreate_Type) GetProcAddress(ddraw.dll, "DirectDrawCreate");
        ddraw.DirectDrawCreateClipper		= GetProcAddress(ddraw.dll, "DirectDrawCreateClipper");
        ddraw.DirectDrawCreateEx			= (DirectDrawCreateEx_Type)GetProcAddress(ddraw.dll, "DirectDrawCreateEx");
        ddraw.DirectDrawEnumerateA			= GetProcAddress(ddraw.dll, "DirectDrawEnumerateA");
        ddraw.DirectDrawEnumerateExA		= GetProcAddress(ddraw.dll, "DirectDrawEnumerateExA");
        ddraw.DirectDrawEnumerateExW		= GetProcAddress(ddraw.dll, "DirectDrawEnumerateExW");
        ddraw.DirectDrawEnumerateW			= GetProcAddress(ddraw.dll, "DirectDrawEnumerateW");
        ddraw.DllCanUnloadNow				= GetProcAddress(ddraw.dll, "DllCanUnloadNow");
        ddraw.DllGetClassObject				= GetProcAddress(ddraw.dll, "DllGetClassObject");
        ddraw.GetDDSurfaceLocal				= GetProcAddress(ddraw.dll, "GetDDSurfaceLocal");
        ddraw.GetOLEThunkData				= GetProcAddress(ddraw.dll, "GetOLEThunkData");
        ddraw.GetSurfaceFromDC				= GetProcAddress(ddraw.dll, "GetSurfaceFromDC");
        ddraw.RegisterSpecialCase			= GetProcAddress(ddraw.dll, "RegisterSpecialCase");
        ddraw.ReleaseDDThreadLock			= GetProcAddress(ddraw.dll, "ReleaseDDThreadLock");
        InitDxWndTrace();
        break;
    }
    case DLL_PROCESS_DETACH: {
        DxWndEndTrace();
        FreeLibrary(ddraw.dll);
    }
    break;
    }
    return TRUE;
}
