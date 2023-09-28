#define _CRT_SECURE_NO_WARNINGS
#define INITGUID

#include <windows.h>
#include <ddrawex.h>
#include "dxwnd.h"
#include "dxhook.h"
#include "ddrawi.h"
#include "dxwcore.hpp"
#include "stdio.h"
#include "hddraw.h"
#include "dxhelper.h"
#include "syslibs.h"

//#define WORKINGUNHOOKEDMODE

/*
    Using DirectDrawEx

    This article provides a brief overview of DirectDrawEx and how it extends the functionality of a DirectDraw object as described in the Microsoft DirectX® SDK.

    Contents of this article:
    What Is DirectDrawEx?
    Advantages of Using DirectDrawEx
    Creating DirectDraw Objects and Surfaces with DirectDrawEx
    Distinctions Between DirectDraw and DirectDrawEx

    What Is DirectDrawEx?

    DirectDrawEx is a dynamic-link library (DLL) that embellishes current functionality of DirectDraw, enhancing
    existing features and providing new functionality.
    DirectDrawEx also exposes new interfaces that applications can use when you include the ddrawex.h header file.
    To create a DirectDraw object that can use the extended features provided by DirectDrawEx, you must create the
    object by using the IDirectDrawFactory interface. A DirectDraw object created with the IDirectDrawFactory
    interface will support the IDirectDraw3 interface, aggregation of DirectDraw surfaces, data exchange, and palette
    mapping, in addition to the features of DirectDraw objects described in the DirectX SDK.

    Advantages of Using DirectDrawEx

    The primary advantage of creating a DirectDraw object through the IDirectDrawFactory interface is that it exposes
    the IDirectDraw3 interface. The IDirectDraw3 interface inherits all the functionality of the IDirectDraw and the
    IDirectDraw2 interfaces and provides a new method that can retrieve a pointer to an IDirectDrawSurface interface,
    given a handle to a device context.
    To obtain the IDirectDraw3 interface, you must call the IDirectDrawFactory::CreateDirectDraw method to create the
    DirectDraw object and expose the IUnknown and IDirectDraw interfaces. Applications can then call QueryInterface to
    obtain a pointer to the IDirectDraw3 interface. To view sample code that demonstrates this, see Creating DirectDraw
    Objects and Surfaces with DirectDrawEx.
    Another advantage of using DirectDrawEx over using DirectDraw is that you can now aggregate inner objects with outer
    objects by using the IDirectDraw3::CreateSurface method. Formerly, IDirectDraw::CreateSurface and
    IDirectDraw2::CreateSurface did not provide COM aggregation features.
    For a thorough description of how IDirectDraw3 implements aggregation see, IDirectDraw3::CreateSurface.

    Finally, DirectDrawEx now also provides the DDSCAPS_DATAEXCHANGE flag for the DDSCAPS structure's dwcaps member.
    Setting this flag in conjunction with the DDSCAPS_OWNDC flag enables applications to call the
    IDirectDrawSurface::GetDC method to lock the device context for as long they require, without holding a lock on the surface.

    Creating DirectDraw Objects and Surfaces with DirectDrawEx

    The following sample code demonstrates how to create a DirectDraw object by using DirectDrawEx, and get a pointer to
    the IDirectDraw3 interface. The code shows how to create and call DirectDraw objects.

    #include ddrawex.h

    void CreateDDEx()
    {
	//Declarations
	HRESULT 	hr;
	IDirectDraw 	*pDD;
	IDirectDraw3 	*pDD3;
	IDirectDrawFactory *pDDF;

	//Initialize COM library
	CoInitialize(NULL);


	//Create a DirectDrawFactory object and get
	//an IDirectDrawFactory interface pointer
	CoCreateInstance(CLSID_DirectDrawFactory, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectDrawFactory, (void **)&pDDF);

	//Call the IDirectDrawFactory::CreateDirectDraw method to create the
	//DirectDraw surface, set the cooperative level, and get the address
	//of an IDirectDraw interface pointer
	hr = (pDDF->CreateDirectDraw(NULL, GetDesktopWindow(), DDSCL_NORMAL,
				NULL, NULL, &pDD));

	if (hr !=DD_OK) {//error checking
	}

	//Now query for the new IDirectDraw3 interface, and release the old one.
	hr =(pDD->QueryInterface(IID_IDirectDraw3, (LPVOID*)&pDD3));

	if (hr !=S_OK) {//error checking
	}

	//Release IDirectDraw
	pDD->Release();
	pDD= NULL;

	//Initialize the DDSURFACEDESC structure for the primary surface
	ZeroMemory(&ddsd, sizeof(ddsd));
      ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
      ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
      hr = pDD3->CreateSurface(&ddsd, &pPrimarySurface, NULL);


	//Do whatever you need to do in your application here with your
	//DirectDraw surface

	//Release IDirectDraw3, IDirectDrawFactory, and the DirectDraw surface
	pDD3->Release();
	pDDF->Release();
	pPrimarySurface->Release();

	//Close the COM library
	CoUninitialize();
    }

    Distinctions Between DirectDraw and DirectDrawEx
    One important distinction to note between DirectDrawEx and DirectDraw is that applications that have created
    multiple DirectDrawSurfaces through a DirectDrawEx surface must release every DirectDraw surface.
    Also, calling the GetDDInterface method from any surface created under DirectDrawEx will return a pointer to
    the IUnknown interface instead of a pointer to an IDirectDraw interface. Applications must use the
    IUnknown::QueryInterface method to retrieve the IDirectDraw, IDirectDraw2, or IDirectDraw3 interfaces.
    Finally, DirectDrawEx does not currently support blitting between surfaces created by DirectDrawEx and surfaces
    created by DirectDraw. Applications should blit only between similar surfaces.

    © 1997 Microsoft Corporation. All rights reserved. Terms of Use.
*/

/*
    IDirectDrawFactory Interface

    The IDirectDrawFactory interface is used to create and enumerate DirectDraw objects that support the extended features
    of DirectDrawEx (see Using DirectDrawEx for more information).

    When to Implement

    Do not implement this interface; DirectDrawEx implements it for you.

    When to Use

    Use this interface in an application when you want to create a DirectDrawEx object.

    Methods in Vtable Order
    IUnknown methods 	Description
    QueryInterface 	Retrieves pointers to supported interfaces.

    AddRef 	Increments the reference count.
    Release 	Decrements the reference count.

    IDirectDrawFactory methods 	Description

    CreateDirectDraw 	Creates a DirectDraw object and retrieves pointers to the IUnknown and the IDirectDraw interfaces.
    DirectDrawEnumerate 	Enumerates the DirectDraw surfaces installed on the system.

    IDirectDrawFactory::CreateDirectDraw

    IDirectDrawFactory Interface

    Creates a DirectDraw object and retrieves pointers to the IUnknown and the IDirectDraw interfaces.

    STDMETHOD CreateDirectDraw(
    GUID * pGUID,
    HWND hWnd,
    DWORD dwCoopLevelFlags,
    DWORD dwReserved,
    IUnknown *pUnkOuter,
    IDirectDraw **ppDirectDraw
    ) PURE;
    Parameters

    pGUID
    [out] Pointer to the globally unique identifier (GUID) that represents the driver to be created. Set this to NULL to indicate the active display driver, or you can pass one of the following flags to restrict the active display driver's behavior for debugging purposes:
    Value 	Meaning
    DDCREATE_EMULATIONONLY 	The DirectDraw object will use emulation for all features; it will not take advantage of any hardware-supported features.
    DDCREATE_HARDWAREONLY 	The DirectDraw object will never emulate features not supported by the hardware. Attempts to call methods that require unsupported features will fail, returning DDERR_UNSUPPORTED (operation not supported).
    hWnd
    [in] Window handle to the application.
    dwCoopLevelFlags
    [in] Application's top-level behavior. Specify one or more of the following flags:
    Value 	Meaning
    DDSCL_ALLOWMODEX 	Enables the use of Mode X display modes. You must use this flag with the DDSCL_EXCLUSIVE and DDSCL_FULLSCREEN flags.
    DDSCL_ALLOWREBOOT 	Enables CTRL+ALT+DEL to function while in exclusive (full-screen) mode.
    DDSCL_EXCLUSIVE 	Requests the exclusive level. You must use this flag with the DDSCL_FULLSCREEN flag.
    DDSCL_FULLSCREEN 	Indicates that the exclusive-mode owner will be responsible for the entire primary surface. GDI can be ignored. You must use this flag with the DDSCL_EXCLUSIVE flag.
    DDSCL_NORMAL 	Indicates that the application will function as a regular Windows® application. You can't use this flag with the DDSCL_ALLOWMODEX, DDSCL_EXCLUSIVE, or DDSCL_FULLSCREEN flags.
    DDSCL_NOWINDOWCHANGES 	Indicates that DirectDraw can't minimize or restore the application window on activation.
    dwReserved
    [in] Reserved for future use. Must be NULL.
    pUnkOuter
    [in] Pointer to an IUnknown interface on an outer object that will be aggregated with an inner object's IUnknown interface.
    ppDirectDraw
    [out] Address of a pointer to an IDirectDraw interface.

    Return Values

    Returns DD_OK if successful, or one of the following error values otherwise:
    Value 	Meaning
    E_OUTOFMEMORY 	There isn't enough memory available to create a DirectDrawEx object.
    DDERR_GENERIC 	There is an undefined error condition.
    DDERR_UNSUPPORTED 	The operation isn't supported.
    DDERR_DIRECTDRAWALREADYCREATED 	A DirectDrawEx object representing this driver has already been created for this process.
    DDERR_INVALIDDIRECTDRAWGUID 	The GUID passed to this method is not a valid DirectDrawEx driver identifier.
    DDERR_INVALIDPARAMS 	One or more of the parameters passed to the method are incorrect.
    DDERR_NODIRECTDRAWHW 	Hardware-only DirectDrawEx object creation isn't possible; the driver doesn't support any hardware.
    Remarks

    This method creates DirectDraw objects in the same way that the DirectDrawCreate function is used to create DirectDraw objects, and sets cooperative levels the same way the IDirectDraw2::SetCooperativeLevel method sets cooperative levels. However, in addition to creating a DirectDraw object, successful calls to the IDirectDrawFactory::CreateDirectDraw method will obtain a pointer to the IUnknown and IDirectDraw interfaces, which are exposed on the DirectDraw object. Applications can now query to obtain the address of a pointer to an IDirectDraw3 interface.
    IDirectDrawFactory::DirectDrawEnumerate
    IDirectDrawFactory Interface

    Enumerates the DirectDraw objects installed on the system.

    STDMETHOD DirectDrawEnumerate(
    LPDDENUMCALLBACK lpCallback,
    LPVOID lpContext
    ) PURE;
    Parameters

    lpCallback
    [in] Pointer to a DDEnumCallback function that will be called with a description of each DirectDrawEx-enabled hardware abstraction layer (HAL) installed in the system.
    lpContext
    [in] Address of an application-defined structure that will be passed to the callback function each time the function is called.

    Return Values

    Returns DD_OK if successful, or DDERR_INVALIDPARAMS otherwise.
    Remarks

    This method functions in a similar manner to the DirectDrawEnumerate function, defined in the DirectX SDK.

    Note This method can only be called by your application after a successful call to the IDirectDrawFactory::CreateDirectDraw method.
*/
// from ddrawex.h
//DECLARE_INTERFACE_(IDirectDrawFactory, IUnknown)
//{
//    /*** IUnknown methods ***/
//    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
//    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
//    STDMETHOD_(ULONG,Release) (THIS) PURE;
//    /*** IDirectDrawFactory methods ***/
//    STDMETHOD(CreateDirectDraw) (THIS_ GUID * pGUID, HWND hWnd, DWORD dwCoopLevelFlags, DWORD dwReserved, IUnknown *pUnkOuter, IDirectDraw **ppDirectDraw) PURE;
//    STDMETHOD(DirectDrawEnumerate) (THIS_ LPDDENUMCALLBACK lpCallback, LPVOID lpContext) PURE;
//};

// DirectDrawEx API (undocumented)
HRESULT WINAPI extDllCanUnloadNow(void);
HRESULT WINAPI extDllGetClassObject(REFCLSID, REFIID, void **);
HRESULT WINAPI extDllRegisterServer(void);
HRESULT WINAPI extDllUnregisterServer(void);

typedef HRESULT (WINAPI *DllCanUnloadNow_Type)(void);
typedef HRESULT (WINAPI *DllGetClassObject_Type)(REFCLSID, REFIID, void **);
typedef HRESULT (WINAPI *DllRegisterServer_Type)(void);
typedef HRESULT (WINAPI *DllUnregisterServer_Type)(void);

DllCanUnloadNow_Type pDllCanUnloadNow;
DllGetClassObject_Type pDllGetClassObject;
DllRegisterServer_Type pDllRegisterServer;
DllUnregisterServer_Type pDllUnregisterServer;

// DirectDrawEx class objects
#ifndef WORKINGUNHOOKEDMODE
HRESULT WINAPI extQueryInterfaceEX(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceDEX(void *, REFIID, LPVOID *);
#endif
HRESULT WINAPI extCreateDirectDraw(void *, GUID *, HWND, DWORD, DWORD, IUnknown *, IDirectDraw **);
HRESULT WINAPI extDirectDrawEnumerateEX(void *, LPDDENUMCALLBACK, LPVOID);

typedef HRESULT (WINAPI *QueryInterfaceEX_Type)(void *, REFIID, LPVOID *);
typedef HRESULT (WINAPI *CreateDirectDrawEX_Type)(void *, GUID *, HWND, DWORD, DWORD, IUnknown *, IDirectDraw **);
typedef HRESULT (WINAPI *DirectDrawEnumerateEX_Type)(void *, LPDDENUMCALLBACK, LPVOID);

#ifndef WORKINGUNHOOKEDMODE
QueryInterfaceEX_Type pQueryInterfaceEX, pQueryInterfaceDEX;
#endif
CreateDirectDrawEX_Type pCreateDirectDrawEX = NULL;
CreateDirectDrawEX_Type pCreateDirectDrawDEX = NULL;
DirectDrawEnumerateEX_Type pDirectDrawEnumerateEX = NULL;

// Library hook

static HookEntryEx_Type ddexHooks[] = {
    {HOOK_HOT_CANDIDATE, 0x01, "DllCanUnloadNow", (FARPROC)NULL, (FARPROC *) &pDllCanUnloadNow, (FARPROC)extDllCanUnloadNow},
    {HOOK_HOT_CANDIDATE, 0x02, "DllGetClassObject", (FARPROC)NULL, (FARPROC *) &pDllGetClassObject, (FARPROC)extDllGetClassObject},
    {HOOK_HOT_CANDIDATE, 0x03, "DllRegisterServer", (FARPROC)NULL, (FARPROC *) &pDllRegisterServer, (FARPROC)extDllRegisterServer},
    {HOOK_HOT_CANDIDATE, 0x04, "DllUnregisterServer", (FARPROC)NULL, (FARPROC *) &pDllUnregisterServer, (FARPROC)extDllUnregisterServer},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

void HookDirectDrawFactoryLib(HMODULE module) {
    HookLibraryEx(module, ddexHooks, "ddrawex.dll");
}

FARPROC Remap_ddrawex_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if (addr = RemapLibraryEx(proc, hModule, ddexHooks)) return addr;
    return NULL;
}

// Objects Hook

void HookDirectDrawFactory(void *obj) {
    //MessageBox(0,"Hooking IID_DirectDrawFactory object", "DxWnd", MB_OK);
    OutTraceDW("Hooking IID_DirectDrawFactory object\n");
#ifndef WORKINGUNHOOKEDMODE
    SetHook((void *)(**(DWORD **)obj +   0), extQueryInterfaceEX, (void **)&pQueryInterfaceEX, "QueryInterface(ex)");
#endif
    //SetHook((void *)(**(DWORD **)obj +   8), extReleaseD1, (void **)&pReleaseD1, "Release(D1)");
    SetHook((void *)(**(DWORD **)obj +  12), extCreateDirectDraw, (void **)&pCreateDirectDrawEX, "CreateDirectDraw(ex)");
    SetHook((void *)(**(DWORD **)obj +  16), extDirectDrawEnumerateEX, (void **)&pDirectDrawEnumerateEX, "DirectDrawEnumerate(ex)");
}

// API wrappers

HRESULT WINAPI extDllCanUnloadNow(void) {
    HRESULT res;
    OutTraceDW("ddrawex#DllCanUnloadNow\n");
    res = (*pDllCanUnloadNow)();
    _if(res) OutTraceE("ddrawex#DllCanUnloadNow ERROR: res=%#x\n", res);
    return res;
}

HRESULT WINAPI extDllGetClassObject(REFCLSID rclsid, REFIID riid, void **out) {
    HRESULT res;
    OutTraceDW("ddrawex#DllGetClassObject: clsid=%#x refiid=%#x\n", rclsid, riid);
    res = (*pDllGetClassObject)(rclsid, riid, out);
    _if(res) OutTraceE("ddrawex#DllGetClassObject ERROR: res=%#x\n", res);
    return res;
}

HRESULT WINAPI extDllRegisterServer(void) {
    HRESULT res;
    OutTraceDW("ddrawex#DllRegisterServer\n");
    res = (*pDllRegisterServer)();
    _if(res) OutTraceE("ddrawex#DllRegisterServer ERROR: res=%#x\n", res);
    return res;
}

HRESULT WINAPI extDllUnregisterServer(void) {
    HRESULT res;
    OutTraceDW("ddrawex#DllUnregisterServer\n");
    res = (*pDllUnregisterServer)();
    _if(res) OutTraceE("ddrawex#DllUnregisterServer ERROR: res=%#x\n", res);
    return res;
}

#ifndef WORKINGUNHOOKEDMODE
SetCooperativeLevel_Type pSetCooperativeLevelEX;
extern HRESULT WINAPI extSetCooperativeLevel(int, SetCooperativeLevel_Type, LPDIRECTDRAW, HWND, DWORD);

HRESULT WINAPI extSetCooperativeLevelEX(LPDIRECTDRAW lpdd, HWND hwnd, DWORD dwflags) {
    return extSetCooperativeLevel(1, pSetCooperativeLevelEX, lpdd, hwnd, dwflags);
}

void HookDDexSession(LPDIRECTDRAW *lplpdd) {
    OutTraceDW("Hooking ddrawex session dd=%#x thread_id=%#x\n", *lplpdd, GetCurrentThreadId());
    SetHook((void *)(**(DWORD **)lplpdd), extQueryInterfaceDEX, (void **)&pQueryInterfaceDEX, "QueryInterface(ex)");
    SetHook((void *)(**(DWORD **)lplpdd + 80), extSetCooperativeLevelEX, (void **)&pSetCooperativeLevelEX, "SetCooperativeLevel(ex)");
}
#endif

// COM bject wrappers

HRESULT WINAPI extCreateDirectDraw(void *ddf, GUID *pGUID, HWND hWnd, DWORD dwCoopLevelFlags, DWORD dwReserved, IUnknown *pUnkOuter, IDirectDraw **ppDirectDraw) {
    HRESULT res;
    ApiName("CreateDirectDraw(EX)");
    OutTraceDW("%s: factory=%#x guid=%s hwnd=%#x coopflags=%#x(%s)\n",
               ApiRef, ddf, sGUID(pGUID), hWnd, dwCoopLevelFlags, ExplainCoopFlags(dwCoopLevelFlags));
    if(dxw.Windowize) {
        dwCoopLevelFlags &= ~(DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE);
        dwCoopLevelFlags |= DDSCL_NORMAL;
    }
    if(dxw.dwFlags7 & NODDEXCLUSIVEMODE)
        dwCoopLevelFlags &= ~DDSCL_EXCLUSIVE;
    res = (*pCreateDirectDrawEX)(ddf, pGUID, hWnd, dwCoopLevelFlags, dwReserved, pUnkOuter, ppDirectDraw);
    if(res) {
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    // CreateDirectDraw can load an unreferenced ddraw.dll module, so it's time now to hook it.
    extern DirectDrawCreate_Type pDirectDrawCreate;
    extern void HookDDSession(LPDIRECTDRAW *, int);
    extern int HookDirectDraw(HMODULE, int);
    if(pDirectDrawCreate == NULL) {
        HINSTANCE hinst;
        hinst = (*pLoadLibraryA)("ddraw.dll");
        // Do not try to call HookDDraw with undefined version.
        // The correct interface version is 1.
        //HookDDraw(hinst);
        HookDirectDraw(hinst, 1);
        FreeLibrary(hinst);
    }
    // Do not try to hook new directdraw session ...
#ifdef WORKINGUNHOOKEDMODE
    HookDDSession(ppDirectDraw, 1);
#else
    HookDDexSession(ppDirectDraw);
#endif
    OutTraceDW("%s: guid=%s pDirectDraw=%#x\n", ApiRef, sGUID(pGUID), *ppDirectDraw);
    return res;
}

HRESULT WINAPI extDirectDrawEnumerateEX(void *ddf, LPDDENUMCALLBACK lpCallback, LPVOID lpContext) {
    HRESULT res;
    ApiName("DirectDrawEnumerate(EX)");
    OutTraceDW("%s: factory=%#x\n", ApiRef, ddf);
    res = (*pDirectDrawEnumerateEX)(ddf, lpCallback, lpContext);
    _if(res) OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

#ifndef WORKINGUNHOOKEDMODE
extern HRESULT WINAPI extQueryInterfaceDX(int, QueryInterface_Type, void *, REFIID, LPVOID *);

HRESULT WINAPI extQueryInterfaceDEX(void *lpdd, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceDX(1, pQueryInterfaceDEX, lpdd, riid, obp);
}
HRESULT WINAPI extQueryInterfaceEX(void *lpdd, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceDX(1, pQueryInterfaceEX, lpdd, riid, obp);
}
#endif
