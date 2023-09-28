#include <dxdiag.h>
#include <dsound.h>
#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhook.h"
#include "dxhelper.h"

extern void HookModule(HMODULE);
extern void HookDirectSoundObj(LPDIRECTSOUND *);
extern void HookDirectDrawFactory(void *);
extern void HookDlls(HMODULE);

typedef HRESULT (WINAPI *CoInitializeEx_Type)(LPVOID, DWORD);
CoInitializeEx_Type pCoInitializeEx;
HRESULT WINAPI extCoInitializeEx(LPVOID, DWORD);

typedef HRESULT (WINAPI *CoGetClassObject_Type)(REFCLSID, DWORD, LPVOID, REFIID, LPVOID FAR *);
CoGetClassObject_Type pCoGetClassObject;
HRESULT WINAPI extCoGetClassObject(REFCLSID, DWORD, LPVOID, REFIID, LPVOID FAR *);

typedef HRESULT (WINAPI *LoadTypeLibEx_Type)(LPCOLESTR, REGKIND, ITypeLib **);
LoadTypeLibEx_Type pLoadTypeLibEx;
HRESULT WINAPI extLoadTypeLibEx(LPCOLESTR, REGKIND, ITypeLib **);

static BOOL bRecursedHook = FALSE;

static HookEntryEx_Type HooksOle32[] = {
    {HOOK_HOT_CANDIDATE, 0, "CoCreateInstance", (FARPROC)CoCreateInstance, (FARPROC *) &pCoCreateInstance, (FARPROC)extCoCreateInstance},
    {HOOK_HOT_CANDIDATE, 0, "CoCreateInstanceEx", (FARPROC)CoCreateInstanceEx, (FARPROC *) &pCoCreateInstanceEx, (FARPROC)extCoCreateInstanceEx},
    {HOOK_HOT_CANDIDATE, 0, "CoInitialize", (FARPROC)CoInitialize, (FARPROC *) &pCoInitialize, (FARPROC)extCoInitialize},
    {HOOK_HOT_CANDIDATE, 0, "CoInitializeEx", (FARPROC)CoInitializeEx, (FARPROC *) &pCoInitializeEx, (FARPROC)extCoInitializeEx},
    {HOOK_HOT_CANDIDATE, 0, "OleInitialize", NULL, (FARPROC *) &pOleInitialize, (FARPROC)extOleInitialize},
    {HOOK_HOT_CANDIDATE, 0, "CoUninitialize", NULL, (FARPROC *) &pCoUninitialize, (FARPROC)extCoUninitialize},
    {HOOK_HOT_CANDIDATE, 0, "CoGetClassObject", (FARPROC)CoGetClassObject, (FARPROC *) &pCoGetClassObject, (FARPROC)extCoGetClassObject},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type HooksOleAut32[] = {
    {HOOK_HOT_CANDIDATE, 0, "LoadTypeLibEx", (FARPROC)LoadTypeLibEx, (FARPROC *) &pLoadTypeLibEx, (FARPROC)extLoadTypeLibEx},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

extern HRESULT WINAPI extDirectDrawCreate(GUID FAR *, LPDIRECTDRAW FAR *, IUnknown FAR *);
extern HRESULT WINAPI extDirectDrawCreateEx(GUID FAR *, LPDIRECTDRAW FAR *, REFIID, IUnknown FAR *);

void HookOle32(HMODULE module) {
    HookLibraryEx(module, HooksOle32, "ole32.dll");
}

void HookOleAut32(HMODULE module) {
    HookLibraryEx(module, HooksOleAut32, "oleaut32.dll");
}

FARPROC Remap_Ole32_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if (addr = RemapLibraryEx(proc, hModule, HooksOle32)) return addr;
    return NULL;
}

FARPROC Remap_OleAut32_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if (addr = RemapLibraryEx(proc, hModule, HooksOleAut32)) return addr;
    return NULL;
}

// -------------------------------------------------------------------------------------
// Ole32 CoCreateInstance handling: you can create DirectDraw objects through it!
// utilized so far in a single game: "Axis & Allies"
// another one: "Crusaders of Might and Magic" ....
// -------------------------------------------------------------------------------------

extern void HookDDSession(LPDIRECTDRAW *, int);

#ifndef DXW_NOTRACES
static char *ExplainCoCError(DWORD r) {
    char *s;
    switch(r) {
    case S_OK:
        s = "OK";
        break;
    case CO_E_NOTINITIALIZED:
        s = "CO_E_NOTINITIALIZED";
        break;
    case CLASS_E_NOAGGREGATION:
        s = "CLASS_E_NOAGGREGATION";
        break;
    case CLASS_E_CLASSNOTAVAILABLE:
        s = "CLASS_E_CLASSNOTAVAILABLE";
        break;
    case CLASS_E_NOTLICENSED:
        s = "CLASS_E_NOTLICENSED";
        break;
    case REGDB_E_CLASSNOTREG:
        s = "REGDB_E_CLASSNOTREG";
        break;
    case E_NOINTERFACE:
        s = "E_NOINTERFACE";
        break;
    case E_POINTER:
        s = "E_POINTER";
        break;
    case RPC_E_SERVERFAULT:
        s = "RPC_E_SERVERFAULT";
        break;
    case RPC_E_CHANGED_MODE:
        s = "RPC_E_CHANGED_MODE";
        break;
    case RPC_E_INVALIDMETHOD:
        s = "RPC_E_INVALIDMETHOD";
        break;
    case 0x80012:
        s = "Interfaces not all implemented";
        break;
    default:
        s = "unknown";
        break;
    }
    return s;
}
#endif

char *sRIID(REFIID riid) {
    static char sRIIDBuffer[81];
    OLECHAR *guidString;
    StringFromCLSID(riid, &guidString);
    sprintf_s(sRIIDBuffer, 80, "%ls",  guidString);
    ::CoTaskMemFree(guidString);
    return sRIIDBuffer;
}

static void HookOLEClass(REFCLSID rclsid) {
    char sSubKey[80];
    HKEY hKey, hSrvKey, hHdlKey;
    LSTATUS oleret;
    char sClassName[81];
    char sDLLPath[MAX_PATH + 1];
    LONG len;
    HMODULE hMod;
    ApiName("HookOLEClass");
    OutTrace("%s: clsid=%s\n", ApiRef, sRIID(rclsid));
    if(!pRegOpenKey) pRegOpenKey = RegOpenKeyA;
    sprintf_s(sSubKey, 80, "\\CLSID\\%s", sRIID(rclsid));
    oleret = (*pRegOpenKey)(HKEY_CLASSES_ROOT, sSubKey, &hKey);
    if(oleret != ERROR_SUCCESS) return;
    if(!pRegQueryValue) pRegQueryValue = RegQueryValueA;
    len = 80;
    oleret = (*pRegQueryValue)(hKey, "", sClassName, &len);
    _if(oleret == ERROR_SUCCESS) OutTrace("%s: class=%s\n", ApiRef, sClassName);
    oleret = (*pRegOpenKey)(hKey, "InProcServer32", &hSrvKey);
    if(oleret != ERROR_SUCCESS) hSrvKey = NULL;
    oleret = (*pRegOpenKey)(hKey, "InProcHandler32", &hHdlKey);
    if(oleret != ERROR_SUCCESS) hHdlKey = NULL;
    if(hSrvKey) {
        len = MAX_PATH;
        oleret = (*pRegQueryValue)(hSrvKey, "", sDLLPath, &len);
        if(oleret != ERROR_SUCCESS) return;
        OutTrace("%s: InProcServer32 dllpath=\"%s\"\n", ApiRef, sDLLPath);
        if(dxw.dwTFlags & ASSERTDIALOG) {
            char msg[MAX_PATH * 2];
            sprintf_s(msg, MAX_PATH * 2, "Hooking OLE server\nclass=%s\nGUID=%s\ndllpath=\"%s\".",
                      sClassName, sRIID(rclsid), sDLLPath);
            MessageBox(0, msg, "DxWnd Assert", 0);
        }
        // v2.05.44 fix: don't hook the dll directly, this is ok only when handling a unknown module.
        // in case of known modules, a direct hook may bring recursion or improper hooking of dependencies.
        // better mimick a LoadLibrary operation that has all code to manage that.
        //hMod = (*pLoadLibraryA)(sDLLPath);
        //if(hMod) HookDlls(hMod);
        extern HMODULE WINAPI LoadLibraryExWrapper(LPVOID, BOOL, HANDLE, DWORD, ApiArg);
        LoadLibraryExWrapper(sDLLPath, FALSE, NULL, 0, ApiRef);
    }
    if(hHdlKey) {
        len = MAX_PATH;
        oleret = (*pRegQueryValue)(hHdlKey, "", sDLLPath, &len);
        if(oleret != ERROR_SUCCESS) return;
        OutTrace("%s: InProcHandler32 dllpath=\"%s\"\n", ApiRef, sDLLPath);
        if(dxw.dwTFlags & ASSERTDIALOG) {
            char msg[MAX_PATH * 2];
            sprintf_s(msg, MAX_PATH * 2, "Hooking OLE handler\nclass=%s\nGUID=%s\ndllpath=\"%s\".",
                      sClassName, sRIID(rclsid), sDLLPath);
            MessageBox(0, msg, "DxWnd Assert", 0);
        }
        hMod = (*pLoadLibraryA)(sDLLPath);
        if(hMod) HookDlls(hMod);
    }
    if(!pRegCloseKey) pRegCloseKey = RegCloseKey;
    if(hSrvKey) (*pRegCloseKey)(hSrvKey);
    if(hHdlKey) (*pRegCloseKey)(hHdlKey);
    (*pRegCloseKey)(hKey);
}

static const GUID DX1VBGUID = {0xE7FF1300, 0x96A5, 0x11D3, {0xAC, 0x85, 0x00, 0xC0, 0x4F, 0xC2, 0xC6, 0x02}}; // ???
static const GUID DX7VBGUID = {0xE1211242, 0x8E94, 0x11D1, {0x88, 0x08, 0x00, 0xC0, 0x4F, 0xC2, 0xC6, 0x02}}; // DX7VB.DLL
static const GUID DX8VBGUID = {0xE1211242, 0x8E94, 0x11D1, {0x88, 0x08, 0x00, 0xC0, 0x4F, 0xC2, 0xC6, 0x03}}; // DX8VB.DLL

// used in "DOVE" with dx7vb.dll
// E1211353-8E94-11D1-8808-00C04FC2C602 IDirectX6.0 ??
// FAFA3599-8B72-11D2-90B2-00C04FC2C602 IDirectX7 ??
// 7FD52380-4E07-101B-AE2D-08002B2EC713 IID_IPersistStreamInit
// 37D84F60-42CB-11CE-8135-00AA004BB851 IPersistPropertyBag

static void HookGUIDObject(ApiArg, REFCLSID rclsid, REFIID riid, LPVOID FAR *ppv) {
    HRESULT res;
    HookOLEClass(rclsid);
    switch (rclsid.Data1) {
    case 0xD7B70EE0: // CLSID_DirectDraw:
    // v2.05.14: added rclsid==CLSID_DirectDraw7 case found in "Matatiki"
    case 0x3C305196: // CLSID_DirectDraw7:
        // v2.03.18: fixed
        OutTraceDW("%s: CLSID_DirectDraw object\n", ApiRef);
        switch (*(DWORD *)&riid) {
            LPDIRECTDRAW lpOldDDraw;
        case 0x6C14DB80:
            // must go through DirectDrawCreate: needed for "Darius Gaiden"
            OutTraceDW("%s: IID_DirectDraw RIID\n", ApiRef);
            res = extDirectDrawCreate(NULL, &lpOldDDraw, 0);
            _if(res) OutTraceDW("%s: DirectDrawCreate res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
            res = lpOldDDraw->QueryInterface(IID_IDirectDraw, (LPVOID *)ppv);
            _if(res) OutTraceDW("%s: QueryInterface res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
            lpOldDDraw->Release();
            break;
        case 0xB3A6F3E0:
            OutTraceDW("%s: IID_DirectDraw2 RIID\n", ApiRef);
            res = extDirectDrawCreate(NULL, &lpOldDDraw, 0);
            _if(res) OutTraceDW("%s: DirectDrawCreate res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
            res = lpOldDDraw->QueryInterface(IID_IDirectDraw2, (LPVOID *)ppv);
            _if(res) OutTraceDW("%s: QueryInterface res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
            lpOldDDraw->Release();
            break;
        case 0x9C59509A:
            OutTraceDW("%s: IID_DirectDraw4 RIID\n", ApiRef);
            res = extDirectDrawCreate(NULL, &lpOldDDraw, 0);
            _if(res) OutTraceDW("%s: DirectDrawCreate res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
            res = lpOldDDraw->QueryInterface(IID_IDirectDraw4, (LPVOID *)ppv);
            _if(res) OutTraceDW("%s: QueryInterface res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
            lpOldDDraw->Release();
            break;
        case 0x15E65EC0:
            OutTraceDW("%s: IID_DirectDraw7 RIID\n", ApiRef);
            res = extDirectDrawCreateEx(NULL, (LPDIRECTDRAW *)ppv, IID_IDirectDraw7, 0);
            _if(res) OutTraceDW("%s: DirectDrawCreateEx res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
            break;
        case 0xE436EBB3:
            break;
        }
        break;
    case 0xA65B8071: // CLSID_DxDiagProvider:
        if ((rclsid.Data2 == 0x3BFE) && (rclsid.Data3 == 0x4213)) {
            OutTraceDW("%s: CLSID_DxDiagProvider object\n", ApiRef);
            res = HookDxDiag(riid, ppv);
        }
        break;
    case 0x47d4d946: // CLSID_DirectSound
        OutTraceDW("%s: CLSID_DirectSound object\n", ApiRef);
        HookDirectSoundObj((LPDIRECTSOUND *)ppv);
        break;
    case 0x4fd2a832: // CLSID_DirectDrawFactory
        if ((rclsid.Data2 == 0x86c8) && (rclsid.Data3 == 0x11d0)) {
            OutTraceDW("%s: CLSID_DirectDrawFactory object\n", ApiRef);
            HookDirectDrawFactory((void *)ppv);
        }
        break;
    default:
        break;
    }
}

HRESULT STDAPICALLTYPE extCoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR *ppv) {
    HRESULT res;
    ApiName("CoCreateInstance");
#ifndef DXW_NOTRACES
    {
        char sGUID1[80], sGUID2[80];
        strcpy_s(sGUID1, 80, sRIID(rclsid));
        strcpy_s(sGUID2, 80, sRIID(riid));
        OutTraceDW("%s: rclsid=%s UnkOuter=%#x ClsContext=%#x refiid=%s(%s)\n",
                   ApiRef, sGUID1, pUnkOuter, dwClsContext, sGUID2, ExplainGUID((GUID *)&riid));
    }
#endif
    bRecursedHook = TRUE;
    // v2.04.49: added try-except condition to handle the case where the pCoCreateInstance pointer read from IAT
    // is no longer valid when later used. It happens with "Gods - Lands of Infinity Special Edition" and could
    // be avoided also by using hot patching hooks, but this way is better.
    __try {
        res = (*pCoCreateInstance)(rclsid, pUnkOuter, dwClsContext, riid, ppv);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        void *prevhook = (void *)pCoCreateInstance;
        HMODULE hOle32 = (*pLoadLibraryA)("ole32.dll");
        pCoCreateInstance = (CoCreateInstance_Type)(*pGetProcAddress)(hOle32, "CoCreateInstance");
        OutTraceDW("%s: renewed hook ptr=%#x->%#x\n", ApiRef, prevhook, pCoCreateInstance);
        res = (*pCoCreateInstance)(rclsid, pUnkOuter, dwClsContext, riid, ppv);
    }
    bRecursedHook = FALSE;
    if(res) {
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainCoCError(res));
        return res;
    }
    OutTraceDW("%s: ppv=%#x->%#x\n", ApiRef, *ppv, *(DWORD *)*ppv);
    HookGUIDObject(ApiRef, rclsid, riid, ppv);
    OutTraceDW("%s: res=%#x ppv=%#x->%#x\n", ApiRef, res, *ppv, *(DWORD *)*ppv);
    return S_OK;
}

HRESULT STDAPICALLTYPE extCoCreateInstanceEx(REFCLSID rclsid, IUnknown *punkOuter, DWORD dwClsCtx, COSERVERINFO *pServerInfo, DWORD dwCount, MULTI_QI *pResults) {
    HRESULT res;
    ApiName("CoCreateInstanceEx");
    OutTraceDW("%s: rclsid=%s UnkOuter=%#x ClsContext=%#x Count=%d\n",
               ApiRef, sRIID(rclsid), punkOuter, dwClsCtx, dwCount);
    // weird error codes ....
    // 0x80012 	Not all the requested interfaces were available
    // 0x80013 	The specified machine name was not found in the cache.
    res = (*pCoCreateInstanceEx)(rclsid, punkOuter, dwClsCtx, pServerInfo, dwCount, pResults);
    if(res) {
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainCoCError(res));
        if (res != 0x80012) return res;
    }
    if(bRecursedHook) return res;
    for(DWORD i = 0; i < dwCount; i++) {
        LPVOID FAR *ppv;
        IID riid = *pResults[i].pIID;
        if(pResults[i].hr) {
            HRESULT res = pResults[i].hr;
            OutTraceE("%s: SKIP riid[%d]=%s res=%#x(%s)\n",
                      ApiRef, i, sRIID(riid), res, ExplainCoCError(res));
        } else {
            ppv = (LPVOID *)pResults[i].pItf;
            OutTraceDW("%s: SCAN riid[%d]=%s ppv=%#x\n",
                       ApiRef, i, sRIID(riid), ppv);
            HookGUIDObject(ApiRef, rclsid, riid, ppv);
        }
    }
    OutTraceSYS("%s: res=%#x\n", ApiRef, res);
    return res;
}

HRESULT WINAPI extCoInitialize(LPVOID pvReserved) {
    HRESULT res;
    ApiName("CoInitialize");
    OutTraceSYS("%s: Reserved=%#x\n", ApiRef, pvReserved);
    // v2.04.74: suppress the potential cause of RPC_E_CHANGED_MODE error. Fixes "Rubik's Games" audio.
    if(dxw.dwFlags10 & NOOLEINITIALIZE) {
        OutTraceDW("%s: SUPPRESS\n", ApiRef);
        return S_OK;
    }
    res = (*pCoInitialize)(pvReserved);
    OutTraceSYS("%s: res=%#x\n", ApiRef, res);
    return res;
}

HRESULT WINAPI extOleInitialize(LPVOID pvReserved) {
    HRESULT res;
    ApiName("OleInitialize");
    OutTraceSYS("%s: Reserved=%#x\n", ApiRef, pvReserved);
    // v2.04.74: suppress the potential cause of RPC_E_CHANGED_MODE error. Fixes "Rubik's Games" audio.
    if(dxw.dwFlags10 & NOOLEINITIALIZE) {
        OutTraceDW("%s: SUPPRESS\n", ApiRef);
        return S_OK;
    }
    res = (*pOleInitialize)(pvReserved);
    OutTraceSYS("%s: res=%#x\n", ApiRef, res);
    return res;
}

void WINAPI extCoUninitialize(void) {
    ApiName("CoUninitialize");
    OutTraceSYS("%s\n", ApiRef);
    if(dxw.dwFlags10 & NOOLEINITIALIZE) {
        OutTraceDW("%s: SUPPRESS\n", ApiRef);
        return;
    }
    (*pCoUninitialize)();
}

HRESULT WINAPI extCoInitializeEx(LPVOID pvReserved, DWORD dwCoInit) {
    HRESULT res;
    ApiName("CoInitializeEx");
    OutTraceSYS("%s: init=%#x\n", ApiRef, dwCoInit);
    if(dxw.dwFlags10 & NOOLEINITIALIZE) {
        OutTraceDW("%s: SUPPRESS\n", ApiRef);
        return S_OK;
    }
    res = (*pCoInitializeEx)(pvReserved, dwCoInit);
    OutTraceSYS("%s: res=%#x\n", ApiRef, res);
    return res;
}

HRESULT WINAPI extCoGetClassObject(REFCLSID rclsid, DWORD dwClsContext, LPVOID pvReserved, REFIID riid, LPVOID FAR *ppv) {
    HRESULT res;
    ApiName("CoGetClassObject");
#ifndef DXW_NOTRACES
    {
        char sGUID1[80], sGUID2[80];
        strcpy_s(sGUID1, 80, sRIID(rclsid));
        strcpy_s(sGUID2, 80, sRIID(riid));
        OutTrace(">> rclsid=%s refiid=%s(%s)\n", sGUID1, sGUID2, ExplainGUID((GUID *)&riid));
        OutTraceSYS("%s: clsid=%s context=%#x reserved=%#x riid=%s ppv=%#x\n",
                    ApiRef, sGUID1, dwClsContext, pvReserved, sGUID2, ppv);
    }
#endif
    res = (*pCoGetClassObject)(rclsid, dwClsContext, pvReserved, riid, ppv);
    OutTraceSYS("%s: res=%#x\n", ApiRef, res);
    return res;
}

HRESULT WINAPI extLoadTypeLibEx(LPCOLESTR szFile, REGKIND regkind, ITypeLib **pptlib) {
    HRESULT res;
    ApiName("LoadTypeLibEx");
    OutTraceSYS("%s: file=\"%s\" regkind=%#x\n", ApiRef, szFile, regkind);
    res = (*pLoadTypeLibEx)(szFile, regkind, pptlib);
    OutTraceSYS("%s: res=%#x\n", ApiRef, res);
    return res;
}
