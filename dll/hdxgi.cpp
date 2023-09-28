#define _CRT_SECURE_NO_WARNINGS
//#define TRACEALL

#include <windows.h>
#include <dxgi.h>
#include <dxgitype.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhook.h"
#include "dxhelper.h"
#include "syslibs.h"
#include "stdio.h"

#ifndef DXGI_ERROR_NOT_FOUND
#define DXGI_ERROR_NOT_FOUND    MAKE_DXGI_HRESULT( 2 )
#endif

typedef HRESULT (WINAPI *CreateDXGIFactory_Type)(REFIID, void **);
typedef HRESULT (WINAPI *CreateDXGIFactory2_Type)(UINT, REFIID, void **);

HRESULT WINAPI extCreateDXGIFactory(REFIID, void **);
HRESULT WINAPI extCreateDXGIFactory1(REFIID, void **);
HRESULT WINAPI extCreateDXGIFactory2(UINT, REFIID, void **);

CreateDXGIFactory_Type pCreateDXGIFactory, pCreateDXGIFactory1;
CreateDXGIFactory2_Type pCreateDXGIFactory2;

// QueryInterface

typedef HRESULT (WINAPI *DXGIQueryInterface_Type)(void *obj, REFIID riid, void **ppvObj);

HRESULT WINAPI extDXGIQueryInterfaceF(void *, REFIID, void **);
HRESULT WINAPI extDXGIQueryInterfaceA(void *, REFIID, void **);

DXGIQueryInterface_Type pDXGIQueryInterfaceF, pDXGIQueryInterfaceA;

// IDXGIFactory interfaces

typedef HRESULT (WINAPI *DXGIEnumAdapters_Type)(void *, UINT, IDXGIAdapter **);
typedef HRESULT (WINAPI *DXGIMakeWindowAssociation_Type)(void *, HWND WindowHandle, UINT Flags);
typedef HRESULT (WINAPI *DXGIGetWindowAssociation_Type)(void *, HWND *);
typedef HRESULT (WINAPI *DXGICreateSwapChain_Type)(void *, IUnknown *, DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **);
typedef HRESULT (WINAPI *DXGICreateSoftwareAdapter_Type)(void *, HMODULE, IDXGIAdapter **);

typedef HRESULT (WINAPI *DXGIGetBuffer_Type)(void *, UINT, REFIID, void **);
typedef HRESULT (WINAPI *DXGIPresent_Type)(void *, UINT, UINT);
typedef HRESULT (WINAPI *DXGISetFullscreenState_Type)(void *, BOOL, IDXGIOutput *);
typedef HRESULT (WINAPI *DXGIGetFullscreenState_Type)(void *, BOOL *, IDXGIOutput **);
typedef HRESULT (WINAPI *DXSCGetDesc_Type)(void *, DXGI_SWAP_CHAIN_DESC *);
typedef HRESULT (WINAPI *DXGIResizeBuffers_Type)(void *, UINT, UINT, UINT, DXGI_FORMAT, UINT);
typedef HRESULT (WINAPI *DXGIResizeTarget_Type)(void *, const DXGI_MODE_DESC *);
typedef HRESULT (WINAPI *DXGIGetContainingOutput_Type)(void *, IDXGIOutput **);

HRESULT WINAPI extDXGIEnumAdapters(void *, UINT, IDXGIAdapter **);
#ifdef TRACEALL
HRESULT WINAPI extDXGIMakeWindowAssociation(void *, HWND WindowHandle, UINT Flags);
HRESULT WINAPI extDXGIGetWindowAssociation(void *, HWND *pWindowHandle);
#endif // TRACEALL
HRESULT WINAPI extDXGICreateSwapChain(void *, IUnknown *, DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **);
HRESULT WINAPI extDXGICreateSoftwareAdapter(void *, HMODULE, IDXGIAdapter **);

#ifdef TRACEALL
HRESULT WINAPI extDXGIGetBuffer(void *, UINT, REFIID, void **);
#endif // TRACEALL
HRESULT WINAPI extDXGIPresent(void *, UINT, UINT);
HRESULT WINAPI extDXGISetFullscreenState(void *, BOOL, IDXGIOutput *);
HRESULT WINAPI extDXGIGetFullscreenState(void *, BOOL *, IDXGIOutput **);
HRESULT WINAPI extDXSCGetDesc(void *, DXGI_SWAP_CHAIN_DESC *);
HRESULT WINAPI extDXGIResizeBuffers(void *, UINT, UINT, UINT, DXGI_FORMAT, UINT);
HRESULT WINAPI extDXGIResizeTarget(void *, const DXGI_MODE_DESC *);
#ifdef TRACEALL
HRESULT WINAPI extDXGIGetContainingOutput(void *, IDXGIOutput **);
#endif // TRACEALL

DXGIEnumAdapters_Type pDXGIEnumAdapters;
#ifdef TRACEALL
DXGIMakeWindowAssociation_Type pDXGIMakeWindowAssociation;
DXGIGetWindowAssociation_Type pDXGIGetWindowAssociation;
#endif // TRACEALL
DXGICreateSwapChain_Type pDXGICreateSwapChain;
DXGICreateSoftwareAdapter_Type pDXGICreateSoftwareAdapter;

#ifdef TRACEALL
DXGIGetBuffer_Type pDXGIGetBuffer;
#endif // TRACEALL
DXGIPresent_Type pDXGIPresent;
DXGISetFullscreenState_Type pDXGISetFullscreenState;
DXGIGetFullscreenState_Type pDXGIGetFullscreenState;
DXSCGetDesc_Type pDXSCGetDesc;
DXGIResizeBuffers_Type pDXGIResizeBuffers;
DXGIResizeTarget_Type pDXGIResizeTarget;
#ifdef TRACEALL
DXGIGetContainingOutput_Type pDXGIGetContainingOutput;
#endif // TRACEALL

// IDXGIAdapter interfaces

typedef HRESULT (WINAPI *DXGIGetParent_Type)(void *, REFIID, void **);
typedef HRESULT (WINAPI *DXGIEnumOutputs_Type)(void *, UINT, IDXGIOutput **);
typedef HRESULT (WINAPI *DXADGetDesc_Type)(void *, DXGI_ADAPTER_DESC *);
typedef HRESULT (WINAPI *DXGICheckInterfaceSupport_Type)(void *, REFGUID, LARGE_INTEGER *);
typedef HRESULT (WINAPI *DXGIGetPrivateData_Type)(void *, REFGUID, UINT *, void *);
typedef HRESULT (WINAPI *DXGISetPrivateDataInterface_Type)(void *, REFGUID, const IUnknown *);
typedef HRESULT (WINAPI *DXGISetPrivateData_Type)(void *, REFGUID, UINT, const void *);

HRESULT WINAPI extDXGIGetParentA(void *, REFIID, void **);
HRESULT WINAPI extDXGIGetParentF(void *, REFIID, void **);
#ifdef TRACEALL
HRESULT WINAPI extDXGIEnumOutputs(void *, UINT, IDXGIOutput **);
HRESULT WINAPI extDXADGetDesc(void *, DXGI_ADAPTER_DESC *);
HRESULT WINAPI extDXGICheckInterfaceSupport(void *, REFGUID InterfaceName, LARGE_INTEGER *pUMDVersion);
HRESULT WINAPI extDXGIGetPrivateDataA(void *, REFGUID, UINT *, void *);
HRESULT WINAPI extDXGIGetPrivateDataF(void *, REFGUID, UINT *, void *);
HRESULT WINAPI extDXGISetPrivateDataInterfaceA(void *, REFGUID, const IUnknown *);
HRESULT WINAPI extDXGISetPrivateDataInterfaceF(void *, REFGUID, const IUnknown *);
HRESULT WINAPI extDXGISetPrivateDataA(void *, REFGUID, UINT, const void *);
HRESULT WINAPI extDXGISetPrivateDataF(void *, REFGUID, UINT, const void *);
#endif // TRACEALL

DXGIGetParent_Type pDXGIGetParentA, pDXGIGetParentF;
#ifdef TRACEALL
DXGIEnumOutputs_Type pDXGIEnumOutputs;
DXADGetDesc_Type pDXADGetDesc;
DXGICheckInterfaceSupport_Type pDXGICheckInterfaceSupport;
DXGIGetPrivateData_Type pDXGIGetPrivateDataA, pDXGIGetPrivateDataF;
DXGISetPrivateDataInterface_Type pDXGISetPrivateDataInterfaceA, pDXGISetPrivateDataInterfaceF;
DXGISetPrivateData_Type pDXGISetPrivateDataA, pDXGISetPrivateDataF;
#endif // TRACEALL

// IDXGIDevice interfaces

typedef HRESULT (WINAPI *DXGIGetAdapter_Type)(void *, IDXGIAdapter **);
typedef HRESULT (WINAPI *DXGICreateSurface_Type)(void *, const DXGI_SURFACE_DESC *, UINT, DXGI_USAGE, const DXGI_SHARED_RESOURCE *, IDXGISurface **);

HRESULT WINAPI extDXGIGetAdapter(void *, IDXGIAdapter **);
HRESULT WINAPI extDXGICreateSurface(void *, const DXGI_SURFACE_DESC *, UINT, DXGI_USAGE, const DXGI_SHARED_RESOURCE *, IDXGISurface **);

DXGIGetAdapter_Type pDXGIGetAdapter;
#ifdef TRACEALL
DXGICreateSurface_Type pDXGICreateSurface;
#endif // TRACEALL

static HookEntryEx_Type DXGIHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "CreateDXGIFactory", (FARPROC)NULL, (FARPROC *) &pCreateDXGIFactory, (FARPROC)extCreateDXGIFactory},
    {HOOK_HOT_CANDIDATE, 0, "CreateDXGIFactory1", (FARPROC)NULL, (FARPROC *) &pCreateDXGIFactory1, (FARPROC)extCreateDXGIFactory1},
    {HOOK_HOT_CANDIDATE, 0, "CreateDXGIFactory2", (FARPROC)NULL, (FARPROC *) &pCreateDXGIFactory2, (FARPROC)extCreateDXGIFactory2},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

void HookDXGILib(HMODULE module) {
    if (dxw.dwTargetDDVersion == HOOKDDRAWNONE) return;
    HookLibraryEx(module, DXGIHooks, "dxgi.dll");
}

FARPROC Remap_DXGI_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if (dxw.dwTargetDDVersion == HOOKDDRAWNONE) return NULL;
    if (addr = RemapLibraryEx(proc, hModule, DXGIHooks)) return addr;
    return NULL;
}

void HookDXGIUnknownF(void **obj) {
    OutTraceD3D("HookDXGIUnknown\n");
    SetHook((void *)(**(DWORD **)obj +   0), extDXGIQueryInterfaceF, (void **)&pDXGIQueryInterfaceF, "QueryInterface(DXGI)");
}

void HookDXGIUnknownA(void **obj) {
    OutTraceD3D("HookDXGIUnknown\n");
    SetHook((void *)(**(DWORD **)obj +   0), extDXGIQueryInterfaceA, (void **)&pDXGIQueryInterfaceA, "QueryInterface(DXGI)");
}

void HookDXGIObjectA(void **ppObject) {
    // IDXGIObjects inherits from IUnknown (query, addref, release) then offset = 4 * 3 = 12
    // SetPrivateData is first method
    OutTraceD3D("HookDXGIObjectA\n");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)ppObject +  12), extDXGISetPrivateDataA, (void **)&pDXGISetPrivateDataA, "SetPrivateData(DXGI)");
    SetHook((void *)(**(DWORD **)ppObject +  16), extDXGISetPrivateDataInterfaceA, (void **)&pDXGISetPrivateDataInterfaceA, "SetPrivateDataInterface(DXGI)");
    SetHook((void *)(**(DWORD **)ppObject +  20), extDXGIGetPrivateDataA, (void **)&pDXGIGetPrivateDataA, "GetPrivateData(DXGI)");
#endif // TRACEALL
    SetHook((void *)(**(DWORD **)ppObject +  24), extDXGIGetParentA, (void **)&pDXGIGetParentA, "GetParent(DXGI)");
    // end
}

void HookDXGIObjectF(void **ppObject) {
    // IDXGIObjects inherits from IUnknown (query, addref, release) then offset = 4 * 3 = 12
    // SetPrivateData is first method
    OutTraceD3D("HookDXGIObjectF\n");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)ppObject +  12), extDXGISetPrivateDataF, (void **)&pDXGISetPrivateDataF, "SetPrivateData(DXGI)");
    SetHook((void *)(**(DWORD **)ppObject +  16), extDXGISetPrivateDataInterfaceF, (void **)&pDXGISetPrivateDataInterfaceF, "SetPrivateDataInterface(DXGI)");
    SetHook((void *)(**(DWORD **)ppObject +  20), extDXGIGetPrivateDataF, (void **)&pDXGIGetPrivateDataF, "GetPrivateData(DXGI)");
#endif // TRACEALL
    SetHook((void *)(**(DWORD **)ppObject +  24), extDXGIGetParentF, (void **)&pDXGIGetParentF, "GetParent(DXGI)");
    // end
}

void HookDXGIFactory(void **ppFactory) {
    HookDXGIUnknownF(ppFactory);
    HookDXGIObjectF(ppFactory);
    OutTraceD3D("HookDXGIFactory\n");
    // IDXGIFactory inherits from IUnknown (query, addref, release) + IDXGIObject (4 methods), then offset = 4 * 7 = 28
    // DXGIEnumAdapters is first method
    SetHook((void *)(**(DWORD **)ppFactory +  28), extDXGIEnumAdapters, (void **)&pDXGIEnumAdapters, "EnumAdapters(DXGI)");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)ppFactory +  32), extDXGIMakeWindowAssociation, (void **)&pDXGIMakeWindowAssociation, "MakeWindowAssociation(DXGI)");
    SetHook((void *)(**(DWORD **)ppFactory +  36), extDXGIGetWindowAssociation, (void **)&pDXGIGetWindowAssociation, "GetWindowAssociation(DXGI)");
#endif // TRACEALL
    SetHook((void *)(**(DWORD **)ppFactory +  40), extDXGICreateSwapChain, (void **)&pDXGICreateSwapChain, "CreateSwapChain(DXGI)");
    SetHook((void *)(**(DWORD **)ppFactory +  44), extDXGICreateSoftwareAdapter, (void **)&pDXGICreateSoftwareAdapter, "CreateSoftwareAdapter(DXGI)");
    // end
}

void HookDXGIAdapter(void **ppAdapter) {
    HookDXGIUnknownA(ppAdapter);
    HookDXGIObjectA(ppAdapter);
    OutTraceD3D("HookDXGIAdapter\n");
    //SetHook((void *)(**(DWORD **)ppAdapter +   0), extDXGIQueryInterface, (void **)&pDXGIQueryInterface, "QueryInterfac(DXGI)");
    // IDXGIAdapter inherits from IUnknown (query, addref, release) + IDXGIObject (4 methods), then offset = 4 * 7 = 28
    // EnumOutputs is first method
    SetHook((void *)(**(DWORD **)ppAdapter +  24), extDXGIGetParentA, (void **)&pDXGIGetParentA, "GetParent(DXGI)");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)ppAdapter +  28), extDXGIEnumOutputs, (void **)&pDXGIEnumOutputs, "EnumOutputs(DXGI)");
    SetHook((void *)(**(DWORD **)ppAdapter +  32), extDXADGetDesc, (void **)&pDXADGetDesc, "GetDesc(DXGI)");
    SetHook((void *)(**(DWORD **)ppAdapter +  36), extDXGICheckInterfaceSupport, (void **)&pDXGICheckInterfaceSupport, "CheckInterfaceSupport(DXGI)");
#endif // TRACEALL
    // end
}

void HookDXGIDevice(void **ppDevice) {
    OutTraceD3D("HookDXGIDevice\n");
    SetHook((void *)(**(DWORD **)ppDevice +  28), extDXGIGetAdapter, (void **)&pDXGIGetAdapter, "GetAdapter(DXGI)");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)ppDevice +  32), extDXGICreateSurface, (void **)&pDXGICreateSurface, "CreateSurface(DXGI)");
    //SetHook((void *)(**(DWORD **)ppDevice +  36), extDXGIQueryResourceResidency, (void **)&pDXGIQueryResourceResidency, "QueryResourceResidency(DXGI)");
    //SetHook((void *)(**(DWORD **)ppDevice +  40), extDXGISetGPUThreadPriority, (void **)&pDXGISetGPUThreadPriority, "SetGPUThreadPriority(DXGI)");
    //SetHook((void *)(**(DWORD **)ppDevice +  44), extDXGIGetGPUThreadPriority, (void **)&pDXGIGetGPUThreadPriority, "GetGPUThreadPriority(DXGI)");
#endif // TRACEALL
    // end
}

void HookDXGISwapChain(void **ppSwapChain) {
    OutTraceD3D("HookDXGISwapChain\n");
    // v2.05.29 fix: hook to wrong method deleted
    //  0: QueryInterface
    //  4: AddRef
    //  8: Release
    // 12: SetPrivateData
    // 16: SetPrivateDataInterface
    // 20: GetPrivateData
    // SetHook((void *)(**(DWORD **)ppSwapChain +  24), extDXGIGetParent, (void **)&pDXGIGetParent, "GetParent(DXGISwapChain)");
    // SetHook((void *)(**(DWORD **)ppSwapChain +  28), extDXGIGetDevice, (void **)&pDXGIGetDevice, "GetDevice(DXGISwapChain)");
    SetHook((void *)(**(DWORD **)ppSwapChain +  32), extDXGIPresent, (void **)&pDXGIPresent, "Present(DXGISwapChain)");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)ppSwapChain +  36), extDXGIGetBuffer, (void **)&pDXGIGetBuffer, "GetBuffer(DXGISwapChain)");
#endif // TRACEALL
    SetHook((void *)(**(DWORD **)ppSwapChain +  40), extDXGISetFullscreenState, (void **)&pDXGISetFullscreenState, "SetFullscreenState(DXGISwapChain)");
    SetHook((void *)(**(DWORD **)ppSwapChain +  44), extDXGIGetFullscreenState, (void **)&pDXGIGetFullscreenState, "GetFullscreenState(DXGISwapChain)");
    SetHook((void *)(**(DWORD **)ppSwapChain +  48), extDXSCGetDesc, (void **)&pDXSCGetDesc, "GetDesc(DXSCSwapChain)"); // duplicated name
    SetHook((void *)(**(DWORD **)ppSwapChain +  52), extDXGIResizeBuffers, (void **)&pDXGIResizeBuffers, "ResizeBuffers(DXGISwapChain)");
    SetHook((void *)(**(DWORD **)ppSwapChain +  56), extDXGIResizeTarget, (void **)&pDXGIResizeTarget, "ResizeTarget(DXGISwapChain)");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)ppSwapChain +  60), extDXGIGetContainingOutput, (void **)&pDXGIGetContainingOutput, "GetContainingOutput(DXGISwapChain)");
    //SetHook((void *)(**(DWORD **)ppSwapChain +  64), extDXGIGetFrameStatistics, (void **)&pDXGIGetFrameStatistics, "GetFrameStatistics(DXGISwapChain)");
    //SetHook((void *)(**(DWORD **)ppSwapChain +  68), extDXGIGetLastPresentCount, (void **)&pDXGIGetLastPresentCount, "GetLastPresentCount(DXGISwapChain)");
#endif // TRACEALL
    // end
}

void HookDXGIOutput(void **ppOutput) {
    OutTraceD3D("HookDXGIOutput\n");
    //SetHook((void *)(**(DWORD **)ppOutput +  28), extDXOUGetDesc, (void **)&pDXOUGetDesc, "GetDesc(DXGIOutput)"); // duplicated name
    //SetHook((void *)(**(DWORD **)ppOutput +  32), extDXGIGetDisplayModeList, (void **)&pDXGIGetDisplayModeList, "GetDisplayModeList(DXGIOutput)");
    //SetHook((void *)(**(DWORD **)ppOutput +  36), extDXGIFindClosestMatchingMode, (void **)&pDXGIFindClosestMatchingMode, "FindClosestMatchingMode(DXGIOutput)");
    //SetHook((void *)(**(DWORD **)ppOutput +  40), extDXGIWaitForVBlank, (void **)&pDXGIWaitForVBlank, "WaitForVBlank(DXGIOutput)");
    //SetHook((void *)(**(DWORD **)ppOutput +  44), extDXGITakeOwnership, (void **)&pDXGITakeOwnership, "TakeOwnership(DXGIOutput)");
    //SetHook((void *)(**(DWORD **)ppOutput +  48), extDXGIReleaseOwnership, (void **)&pDXGIReleaseOwnership, "ReleaseOwnership(DXGIOutput)");
    //SetHook((void *)(**(DWORD **)ppOutput +  52), extDXGIGetGammaControlCapabilities, (void **)&pDXGIGetGammaControlCapabilities, "GetGammaControlCapabilities(DXGIOutput)");
    //SetHook((void *)(**(DWORD **)ppOutput +  56), extDXGISetGammaControl, (void **)&pDXGISetGammaControl, "SetGammaControl(DXGIOutput)");
    //SetHook((void *)(**(DWORD **)ppOutput +  60), extDXGIGetGammaControl, (void **)&pDXGIGetGammaControl, "GetGammaControl(DXGIOutput)");
    //SetHook((void *)(**(DWORD **)ppOutput +  64), extDXGISetDisplaySurface, (void **)&pDXGISetDisplaySurface, "SetDisplaySurface(DXGIOutput)");
    //SetHook((void *)(**(DWORD **)ppOutput +  68), extDXGIGetDisplaySurfaceData, (void **)&pDXGIGetDisplaySurfaceData, "GetDisplaySurfaceData(DXGIOutput)");
    //SetHook((void *)(**(DWORD **)ppOutput +  72), extDXGIGetFrameStatistics, (void **)&pDXGIGetFrameStatistics, "GetFrameStatistics(DXGIOutput)");
    // end
}

HRESULT WINAPI extDXGIQueryInterface(DXGIQueryInterface_Type pDXGIQueryInterface, void *obj, REFIID riid, void **ppvObj) {
    HRESULT res;
    ApiName("DXGI::QueryInterface");
    OutTraceD3D("%s: obj=%#x riid=%s(%s)\n", ApiRef, obj, sRIID(riid), ExplainGUID((GUID *)&riid));
    //"a04bfb29-08ef-43d6-a49c-a9bdbdcbe686" : ID3D11Device1 (d3d11_1.h)
    //"9d06dffa-d1e5-4d07-83a8-1bb123f2f841" : ID3D11Device2 (d3d11_2.h)
    //"189819f1-1db6-4b57-be54-1821339b85f7" : ID3D12Device ???
    //"54ec77fa-1377-44e6-8c32-88fd5f44c84c" : IDXGIDevice (dxgi.h)
    //"b898d4fd-b5b3-4ffc-623f693c
    //"9B7E4A00-342C-4106-A19F-4F2704F689F0" : ???
    //"54ec77fa.1377.44e6.623f58c8" : IDXGIDevice (dxgi.h)
    //DEFINE_GUID(IID_IDXGIObject,0xaec22fb8,0x76f3,0x4639,0x9b,0xe0,0x28,0xeb,0x43,0xa6,0x7a,0x2e);
    //DEFINE_GUID(IID_IDXGIDeviceSubObject,0x3d3e0379,0xf9de,0x4d58,0xbb,0x6c,0x18,0xd6,0x29,0x92,0xf1,0xa6);
    //DEFINE_GUID(IID_IDXGIResource,0x035f3ab4,0x482e,0x4e50,0xb4,0x1f,0x8a,0x7f,0x8b,0xd8,0x96,0x0b);
    //DEFINE_GUID(IID_IDXGISurface,0xcafcb56c,0x6ac3,0x4889,0xbf,0x47,0x9e,0x23,0xbb,0xd2,0x60,0xec);
    //DEFINE_GUID(IID_IDXGIAdapter,0x2411e7e1,0x12ac,0x4ccf,0xbd,0x14,0x97,0x98,0xe8,0x53,0x4d,0xc0);
    //DEFINE_GUID(IID_IDXGIOutput,0xae02eedb,0xc735,0x4690,0x8d,0x52,0x5a,0x8d,0xc2,0x02,0x13,0xaa);
    //DEFINE_GUID(IID_IDXGISwapChain,0x310d36a0,0xd2e7,0x4c0a,0xaa,0x04,0x6a,0x9d,0x23,0xb8,0x88,0x6a);
    //DEFINE_GUID(IID_IDXGIFactory,0x7b7166ec,0x21c7,0x44ae,0xb2,0x1a,0xc9,0xae,0x32,0x1a,0xe3,0x69);
    //DEFINE_GUID(IID_IDXGIDevice,0x54ec77fa,0x1377,0x44e6,0x8c,0x32,0x88,0xfd,0x5f,0x44,0xc8,0x4c);
    //DEFINE_GUID(IID_IDXGIFactory2, 50c83a1c-e072-4c48-87b0-3630fa36a6d0 (dxgi1_2.idl)
    //DEFINE_GUID(IID_IDXGIAdapter4, 3c8d99d1-4fbf-4181-a82c-af66bf7bd24e
    //DEFINE_GUID(IID_IDXGISwapChain3,0x94d99bdb,0xf1f8,0x4ab0,0xb2,0x36,0x7d,0xa0,0x17,0x0e,0xda,0xb1);
    //DEFINE_GUID(IID_IDXGIOutput4,0xdc7dca35,0x2196,0x414d,0x9F,0x53,0x61,0x78,0x84,0x03,0x2a,0x60);
    //DEFINE_GUID(IID_IDXGIFactory4,0x1bc6ea02,0xef36,0x464f,0xbf,0x0c,0x21,0xca,0x39,0xe5,0x16,0x8a);
    //DEFINE_GUID(IID_IDXGIAdapter3,0x645967A4,0x1392,0x4310,0xA7,0x98,0x80,0x53,0xCE,0x3E,0x93,0xFD);
    //DEFINE_GUID(IID_IDXGIOutput4,0xdc7dca35,0x2196,0x414d,0x9F,0x53,0x61,0x78,0x84,0x03,0x2a,0x60);" )
    //DEFINE_GUID(IID_IDXGIFactory4,0x1bc6ea02,0xef36,0x464f,0xbf,0x0c,0x21,0xca,0x39,0xe5,0x16,0x8a);" )
    //DEFINE_GUID(IID_IDXGIAdapter3,0x645967A4,0x1392,0x4310,0xA7,0x98,0x80,0x53,0xCE,0x3E,0x93,0xFD);" )
    //DEFINE_GUID(IID_IDXGIDevice3,0x6007896c,0x3244,0x4afd,0xbf,0x18,0xa6,0xd3,0xbe,0xda,0x50,0x23);" )
    //DEFINE_GUID(IID_IDXGISwapChain2,0xa8be2ac4,0x199f,0x4946,0xb3,0x31,0x79,0x59,0x9f,0xb9,0x8d,0xe7);" )
    //DEFINE_GUID(IID_IDXGIOutput2,0x595e39d1,0x2724,0x4663,0x99,0xb1,0xda,0x96,0x9d,0xe2,0x83,0x64);" )
    //DEFINE_GUID(IID_IDXGIFactory3,0x25483823,0xcd46,0x4c7d,0x86,0xca,0x47,0xaa,0x95,0xb8,0x37,0xbd);" )
    //DEFINE_GUID(IID_IDXGIDecodeSwapChain,0x2633066b,0x4514,0x4c7a,0x8f,0xd8,0x12,0xea,0x98,0x05,0x9d,0x18);" )
    //DEFINE_GUID(IID_IDXGIFactoryMedia,0x41e7d1f2,0xa591,0x4f7b,0xa2,0xe5,0xfa,0x9c,0x84,0x3e,0x1c,0x12);" )
    //DEFINE_GUID(IID_IDXGISwapChainMedia,0xdd95b90b,0xf05f,0x4f6a,0xbd,0x65,0x25,0xbf,0xb2,0x64,0xbd,0x84);" )
    //DEFINE_GUID(IID_IDXGIOutput3,0x8a6bb301,0x7e7e,0x41F4,0xa8,0xe0,0x5b,0x32,0xf7,0xf9,0x9b,0x18);" )
    res = pDXGIQueryInterface(obj, riid, ppvObj);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    switch(riid.Data1) {
    case 0xaec22fb8:
        OutTraceD3D("> IID_IDXGIObject\n");
        break;
    case 0x3d3e0379:
        OutTraceD3D("> IID_IDXGIDeviceSubObject\n");
        break;
    case 0x035f3ab4:
        OutTraceD3D("> IID_IDXGIResource\n");
        break;
    case 0xcafcb56c:
        OutTraceD3D("> IID_IDXGISurface\n");
        break;
    case 0x2411e7e1:
        OutTraceD3D("> IID_IDXGIAdapter\n");
        //HookDXGIAdapter(ppvObj);
        break;
    case 0x29038f61:
        //IID_IDXGIAdapter1: TGUID = '{29038f61-3839-4626-91fd-086879011a05}';
        OutTraceD3D("> IID_IDXGIAdapter1\n");
        //HookDXGIAdapter(ppvObj);
        break;
    case 0x0aa1ae0a:
        OutTraceD3D("> IID_IDXGIAdapter2\n");
        //HookDXGIAdapter(ppvObj);
        break;
    case 0x645967A4:
        OutTraceD3D("> IID_IDXGIAdapter3\n");
        //HookDXGIAdapter(ppvObj);
        break;
    case 0x3c8d99d1:
        OutTraceD3D("> IID_IDXGIAdapter4\n");
        //HookDXGIAdapter(ppvObj);
        break;
    case 0xae02eedb:
        OutTraceD3D("> IID_IDXGIOutput\n");
        break;
    case 0x310d36a0:
        OutTraceD3D("> IID_IDXGISwapChain\n");
        HookDXGISwapChain(ppvObj);
        break;
    case 0xa8be2ac4:
        OutTraceD3D("> IID_IDXGISwapChain2\n");
        //HookDXGISwapChain(ppvObj);
        break;
    case 0x94d99bdb:
        OutTraceD3D("> IID_IDXGISwapChain3\n");
        //HookDXGISwapChain(ppvObj);
        break;
    case 0x54ec77fa:
        OutTraceD3D("> IID_IDXGIDevice\n");
        HookDXGIDevice(ppvObj);
        break;
    case 0x7b7166ec:
        OutTraceD3D("> IID_IDXGIFactory\n");
        HookDXGIFactory(ppvObj);
        break;
    case 0x770aae78:
        OutTraceD3D("> IID_IDXGIFactory1\n");
        HookDXGIFactory(ppvObj);
        break;
    case 0x50c83a1c:
        OutTraceD3D("> IID_IDXGIFactory2\n");
        HookDXGIFactory(ppvObj);
        break;
    default:
        OutTraceD3D("> unknown GUID\n");
        break;
    }
    OutTraceD3D("%s: obj=%#x\n", ApiRef, *ppvObj);
    return res;
}

HRESULT WINAPI extDXGIQueryInterfaceF(void *obj, REFIID riid, void **ppvObj) {
    return extDXGIQueryInterface(pDXGIQueryInterfaceF, obj, riid, ppvObj);
}
HRESULT WINAPI extDXGIQueryInterfaceA(void *obj, REFIID riid, void **ppvObj) {
    return extDXGIQueryInterface(pDXGIQueryInterfaceA, obj, riid, ppvObj);
}

//IID_IDXGIFactory1: TGUID = '{770aae78-f26f-4dba-a829-253c83d1b387}';
//IID_IDXGIAdapter1: TGUID = '{29038f61-3839-4626-91fd-086879011a05}';

HRESULT WINAPI intCreateDXGIFactory(char *api, CreateDXGIFactory_Type pCreateDXGIFactory, REFIID riid, void **ppFactory) {
    HRESULT res;
    OutTraceD3D("%s: riid=%s(%s)\n", ApiRef, sRIID(riid), ExplainGUID((GUID *)&riid));
    res = pCreateDXGIFactory(riid, ppFactory);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: factory=%#x\n", ApiRef, *ppFactory);
    switch(riid.Data1) {
    case 0x7b7166ec:
        ///IDXGIFactory : public IDXGIObject
        HookDXGIFactory(ppFactory);
        break;
    case 0x770aae78:
        ///IDXGIFactory1 : public IDXGIObject
        HookDXGIFactory(ppFactory);
        break;
    }
    return res;
}

HRESULT WINAPI extCreateDXGIFactory(REFIID riid, void **ppFactory) {
    return intCreateDXGIFactory("CreateDXGIFactory", pCreateDXGIFactory, riid, ppFactory);
}
HRESULT WINAPI extCreateDXGIFactory1(REFIID riid, void **ppFactory) {
    return intCreateDXGIFactory("CreateDXGIFactory1", pCreateDXGIFactory1, riid, ppFactory);
}

// v2.05.49: CreateDXGIFactory2 has different prototype!
HRESULT WINAPI extCreateDXGIFactory2(UINT Flags, REFIID riid, void **ppFactory) {
    HRESULT res;
    ApiName("CreateDXGIFactory2");
    OutTraceD3D("%s: flags=%#x riid=%s(%s)\n", ApiRef, Flags, sRIID(riid), ExplainGUID((GUID *)&riid));
    res = pCreateDXGIFactory2(Flags, riid, ppFactory);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: factory=%#x\n", ApiRef, *ppFactory);
    HookDXGIFactory(ppFactory);
    return res;
}

HRESULT WINAPI extDXGIEnumAdapters(void *obj, UINT Adapter, IDXGIAdapter **ppAdapter) {
    HRESULT res;
    ApiName("DXGIFactory::EnumAdapters");
    OutTraceD3D("%s: this=%#x adapter=%d\n", ApiRef, obj, Adapter);
    res = (*pDXGIEnumAdapters)(obj, Adapter, ppAdapter);
    if (res) {
        if(res == DXGI_ERROR_NOT_FOUND)
            OutTraceD3D("%s: DXGI_ERROR_NOT_FOUND\n", ApiRef);
        else
            OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: pAdapter=%#x\n", ApiRef, *ppAdapter);
    HookDXGIAdapter((void **)ppAdapter);
    return res;
}

// DXGI_MWA_NO_WINDOW_CHANGES - Prevent DXGI from monitoring an applications message queue; this makes DXGI unable to respond to mode changes.
// DXGI_MWA_NO_ALT_ENTER - Prevent DXGI from responding to an alt-enter sequence.
// DXGI_MWA_NO_PRINT_SCREEN - Prevent DXGI from responding to a print-screen key.

#ifndef DXW_NOTRACES
static char *ExplainWinAssFlags(UINT f) {
    static char eb[81];
    unsigned int l;
    strcpy(eb, "DXGI_MWA_");
    if(f & DXGI_MWA_NO_WINDOW_CHANGES) strcat(eb, "NO_WINDOW_CHANGES+");
    if(f & DXGI_MWA_NO_ALT_ENTER) strcat(eb, "NO_ALT_ENTER+");
    if(f & DXGI_MWA_NO_PRINT_SCREEN) strcat(eb, "NO_PRINT_SCREEN");
    l = strlen(eb);
    if (l > strlen("DXGI_MWA_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "NULL");
    return(eb);
}
#endif // DXW_NOTRACES

#ifdef TRACEALL
HRESULT WINAPI extDXGIMakeWindowAssociation(void *obj, HWND WindowHandle, UINT Flags) {
    HRESULT res;
    ApiName("DXGIFactory::MakeWindowAssociation");
    OutTraceD3D("%s: obj=%#x hwnd=%#x flags=%#x(%s)\n", ApiRef, obj, WindowHandle, Flags, ExplainWinAssFlags(Flags));
    if((dxw.dwFlags2 & FORCEWINRESIZE) && (Flags & DXGI_MWA_NO_WINDOW_CHANGES)) {
        OutTraceD3D("%s: suppress DXGI_MWA_NO_WINDOW_CHANGES\n", ApiRef);
        Flags &= ~DXGI_MWA_NO_WINDOW_CHANGES;
    }
    if((dxw.dwFlags11 & NODISABLEPRINT) && (Flags & DXGI_MWA_NO_PRINT_SCREEN)) {
        OutTraceD3D("%s: suppress DXGI_MWA_NO_PRINT_SCREEN\n", ApiRef);
        Flags &= ~DXGI_MWA_NO_PRINT_SCREEN;
    }
    res = (*pDXGIMakeWindowAssociation)(obj, WindowHandle, Flags);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: ok\n", ApiRef);
    return res;
}

HRESULT WINAPI extDXGIGetWindowAssociation(void *obj, HWND *pWindowHandle) {
    HRESULT res;
    ApiName("DXGIFactory::GetWindowAssociation");
    OutTraceD3D("%s: obj=%#x\n", ApiRef, obj);
    res = (*pDXGIGetWindowAssociation)(obj, pWindowHandle);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: hwnd=%#x\n", ApiRef, *pWindowHandle);
    return res;
}
#endif // TRACEALL

HRESULT WINAPI extDXGICreateSwapChain(void *obj, IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain) {
    HRESULT res;
#ifndef DXW_NOTRACES
    ApiName("DXGIFactory::CreateSwapChain");
    OutTraceD3D("%s: obj=%#x dev=%#x desc=%#x\n", ApiRef, obj, pDevice, pDesc);
    if(pDesc) {
        OutTraceD3D("DXGI_SWAP_CHAIN_DESC:\n");
        OutTraceD3D("> BufferDesc.size=(%dx%d)\n", pDesc->BufferDesc.Width, pDesc->BufferDesc.Height);
        OutTraceD3D("> BufferDesc.RefreshRate=(%d/%d)\n", pDesc->BufferDesc.RefreshRate.Numerator, pDesc->BufferDesc.RefreshRate.Denominator);
        OutTraceD3D("> BufferDesc.Format=%d\n", pDesc->BufferDesc.Format);
        OutTraceD3D("> BufferDesc.Scaling=%d\n", pDesc->BufferDesc.Scaling);
        OutTraceD3D("> SampleDesc.Count=%d\n", pDesc->SampleDesc.Count);
        OutTraceD3D("> SampleDesc.Quality=%d\n", pDesc->SampleDesc.Quality);
        OutTraceD3D("> BufferUsage=%d\n", pDesc->BufferUsage);
        OutTraceD3D("> BufferCount=%d\n", pDesc->BufferCount);
        OutTraceD3D("> OutputWindow=%#x\n", pDesc->OutputWindow);
        OutTraceD3D("> Windowed=%#x\n", pDesc->Windowed);
        OutTraceD3D("> SwapEffect=%d\n", pDesc->SwapEffect);
        OutTraceD3D("> Flags=%#x\n", pDesc->Flags);
    }
#endif
    if(dxw.Windowize) {
        // v2.05.29 fix: to fit the swap chain in the window you don't have to
        // change the buffer size, but rather set the stretched scaling method !!
        dxw.SetScreenSize(pDesc->BufferDesc.Width, pDesc->BufferDesc.Height);
        OutTraceDW("%s: set windowed size=(%dx%d)\n", ApiRef, dxw.iSizX, dxw.iSizY);
        pDesc->Windowed = TRUE;
        //pDesc->BufferDesc.Width = dxw.iSizX;
        //pDesc->BufferDesc.Height = dxw.iSizY;
        pDesc->BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
        // v2.05.30: added window style & size fixup
        dxw.AdjustWindowFrame(pDesc->OutputWindow, dxw.iSizX, dxw.iSizY);
    }
    res = (*pDXGICreateSwapChain)(obj, pDevice, pDesc, ppSwapChain);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    HookDXGISwapChain((void **)ppSwapChain);
    OutTraceD3D("%s: swapchain=%#x\n", ApiRef, *ppSwapChain);
    return res;
}

HRESULT WINAPI extDXGICreateSoftwareAdapter(void *obj, HMODULE Module, IDXGIAdapter **ppAdapter) {
    HRESULT res;
    ApiName("DXGIFactory::CreateSoftwareAdapter");
    OutTraceD3D("%s: obj=%#x module=%#x\n", ApiRef, obj, Module);
    res = (*pDXGICreateSoftwareAdapter)(obj, Module, ppAdapter);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    HookDXGIAdapter((void **)ppAdapter);
    OutTraceD3D("%s: adapter=%#x\n", ApiRef, *ppAdapter);
    return res;
}

HRESULT WINAPI extDXGIGetParentA(void *obj, REFIID riid, void **ppParent) {
    HRESULT res;
    ApiName("DXGIAdapter::GetParent");
    OutTraceD3D("%s: obj=%#x riid=%s\n", ApiRef, obj, sRIID(riid));
    res = (*pDXGIGetParentA)(obj, riid, ppParent);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    switch(riid.Data1) {
    case 0x77DB970F:
        OutTraceD3D("> IID_IDXGIDevice1\n");
        HookDXGIDevice(ppParent);
        break;
    case 0x7B7166EC:
        OutTraceD3D("> IID_IDXGIFactory\n");
        HookDXGIFactory(ppParent);
        break;
    }
    OutTraceD3D("%s: parent=%#x\n", ApiRef, *ppParent);
    return res;
}

HRESULT WINAPI extDXGIGetParentF(void *obj, REFIID riid, void **ppParent) {
    HRESULT res;
    ApiName("DXGIFactory::GetParent");
    OutTraceD3D("%s: obj=%#x riid=%s\n", ApiRef, obj, sRIID(riid));
    res = (*pDXGIGetParentF)(obj, riid, ppParent);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    switch(riid.Data1) {
    case 0x77DB970F:
        OutTraceD3D("> IID_IDXGIDevice1\n");
        HookDXGIDevice(ppParent);
        break;
    case 0x7B7166EC:
        OutTraceD3D("> IID_IDXGIFactory\n");
        HookDXGIFactory(ppParent);
        break;
    }
    OutTraceD3D("%s: parent=%#x\n", ApiRef, *ppParent);
    return res;
}


#ifdef TRACEALL
HRESULT WINAPI extDXGIEnumOutputs(void *obj, UINT Output, IDXGIOutput **ppOutput) {
    HRESULT res;
    ApiName("DXGIAdapter::EnumOutputs");
    OutTraceD3D("%s: obj=%#x output=%d\n", ApiRef, obj, Output);
    res = (*pDXGIEnumOutputs)(obj, Output, ppOutput);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: output=%#x\n", ApiRef, *ppOutput);
    return res;
}

HRESULT WINAPI extDXADGetDesc(void *obj, DXGI_ADAPTER_DESC *pDesc) {
    HRESULT res;
    ApiName("DXGIAdapter::GetDesc");
    OutTraceD3D("%s: obj=%#x\n", ApiRef, obj);
    res = (*pDXADGetDesc)(obj, pDesc);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: desc=%#x\n", ApiRef, pDesc);
    OutTraceD3D("> descr=\"%ls\"\n", pDesc->Description);
    OutTraceD3D("> id: vendor=%u device=%u subsys=%u rev=%u\n", pDesc->VendorId, pDesc->DeviceId, pDesc->SubSysId, pDesc->Revision);
    OutTraceD3D("> DedicatedVideoMemory=%u\n", pDesc->DedicatedVideoMemory);
    OutTraceD3D("> DedicatedSystemMemory=%u\n", pDesc->DedicatedSystemMemory);
    OutTraceD3D("> SharedSystemMemory=%u\n", pDesc->SharedSystemMemory);
    return res;
}

HRESULT WINAPI extDXGICheckInterfaceSupport(void *obj, REFGUID InterfaceName, LARGE_INTEGER *pUMDVersion) {
    HRESULT res;
    ApiName("DXGIAdapter::CheckInterfaceSupport");
    OutTraceD3D("%s: obj=%#x\n", ApiRef, obj);
    res = (*pDXGICheckInterfaceSupport)(obj, InterfaceName, pUMDVersion);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: version=%#x-%#x\n", ApiRef, (*pUMDVersion).HighPart, (*pUMDVersion).LowPart);
    return res;
}

HRESULT WINAPI extDXGISetPrivateData(char *ApiRef, DXGISetPrivateData_Type pDXGISetPrivateData, void *obj, REFGUID Name, UINT DataSize, const void *pData) {
    HRESULT res;
    OutTraceD3D("%s: obj=%#x name=%s datasize=%d pdata=%#x\n", ApiRef, obj, sRIID(Name), DataSize, pData);
    res = (*pDXGISetPrivateData)(obj, Name, DataSize, pData);
    if (res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extDXGISetPrivateDataA(void *obj, REFGUID Name, UINT DataSize, const void *pData) {
    return extDXGISetPrivateData("DXGIAdapter::SetPrivateData", pDXGISetPrivateDataA, obj, Name, DataSize, pData);
}
HRESULT WINAPI extDXGISetPrivateDataF(void *obj, REFGUID Name, UINT DataSize, const void *pData) {
    return extDXGISetPrivateData("DXGIFactory::SetPrivateData", pDXGISetPrivateDataF, obj, Name, DataSize, pData);
}

HRESULT WINAPI extDXGISetPrivateDataInterface(char *ApiRef, DXGISetPrivateDataInterface_Type pDXGISetPrivateDataInterface, void *obj, REFGUID Name, const IUnknown *pUnknown) {
    HRESULT res;
    OutTraceD3D("%s: obj=%#x name=%s pUnknown=%#x\n", ApiRef, obj, sRIID(Name), pUnknown);
    res = (*pDXGISetPrivateDataInterface)(obj, Name, pUnknown);
    if (res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extDXGISetPrivateDataInterfaceA(void *obj, REFGUID Name, const IUnknown *pUnknown) {
    return extDXGISetPrivateDataInterface("DXGIAdapter::SetPrivateDataInterface", pDXGISetPrivateDataInterfaceA, obj, Name, pUnknown);
}
HRESULT WINAPI extDXGISetPrivateDataInterfaceF(void *obj, REFGUID Name, const IUnknown *pUnknown) {
    return extDXGISetPrivateDataInterface("DXGIFactory::SetPrivateDataInterface", pDXGISetPrivateDataInterfaceF, obj, Name, pUnknown);
}

HRESULT WINAPI extDXGIGetPrivateData(char *ApiRef, DXGIGetPrivateData_Type pDXGIGetPrivateData, void *obj, REFGUID Name, UINT *pDataSize, void *pData) {
    HRESULT res;
    OutTraceD3D("%s: obj=%#x name=%s datasize=%d pdata=%#x\n", ApiRef, obj, sRIID(Name), *pDataSize, pData);
    res = (*pDXGIGetPrivateData)(obj, Name, pDataSize, pData);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: datasize=%d\n", ApiRef, *pDataSize);
    return res;
}

HRESULT WINAPI extDXGIGetPrivateDataA(void *obj, REFGUID Name, UINT *pDataSize, void *pData) {
    return extDXGIGetPrivateData("DXGIAdapter::GetPrivateData", pDXGIGetPrivateDataA, obj, Name, pDataSize, pData);
}
HRESULT WINAPI extDXGIGetPrivateDataF(void *obj, REFGUID Name, UINT *pDataSize, void *pData) {
    return extDXGIGetPrivateData("DXGIFactory::GetPrivateData", pDXGIGetPrivateDataF, obj, Name, pDataSize, pData);
}
#endif // TRACEALL

HRESULT WINAPI extDXGIGetAdapter(void *obj, IDXGIAdapter **ppAdapter) {
    HRESULT res;
    ApiName("DXGIDevice::GetAdapter");
    OutTraceD3D("%s: obj=%#x\n", ApiRef, obj);
    res = (*pDXGIGetAdapter)(obj, ppAdapter);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    HookDXGIAdapter((void **)ppAdapter);
    OutTraceD3D("%s: adapter=%#x\n", ApiRef, *ppAdapter);
    return res;
}

HRESULT WINAPI extDXGIPresent(void *obj, UINT SyncInterval, UINT Flags) {
    HRESULT res;
    ApiName("DXGISwapChain::Present");
    OutTraceD3D("%s: obj=%#x syncinterval=%d flags=%#x\n",
                ApiRef, obj, SyncInterval, Flags);
    // it's safer to set SyncInterval to 0 rather than returning OK. Returning OK may
    // fill the swap chain with additional commands and lead to a game crash!
    if (dxw.HandleFPS()) SyncInterval = 0;
    if(dxw.dwFlags12 & KILLVSYNC) SyncInterval = 0;
    res = (*pDXGIPresent)(obj, SyncInterval, Flags);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    return res;
}

#ifdef TRACEALL
HRESULT WINAPI extDXGICreateSurface(void *obj, const DXGI_SURFACE_DESC *pDesc, UINT NumSurfaces, DXGI_USAGE Usage, const DXGI_SHARED_RESOURCE *pSharedResource, IDXGISurface **ppSurface) {
    HRESULT res;
    ApiName("DXGIDevice::CreateSurface");
    OutTraceD3D("%s: obj=%#x desc={size=(%dx%d) format=%d} nsurfaces=%d usage=%#x\n",
                ApiRef, obj,
                pDesc->Width, pDesc->Height, pDesc->Format,
                NumSurfaces, Usage);
    res = (*pDXGICreateSurface)(obj, pDesc, NumSurfaces, Usage, pSharedResource, ppSurface);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: surface=%#x\n", ApiRef, *ppSurface);
    return res;
}

HRESULT WINAPI extDXGIGetBuffer(void *obj, UINT Buffer, REFIID riid, void **ppSurface) {
    HRESULT res;
    ApiName("DXGISwapChain::GetBuffer");
    OutTraceD3D("%s: buffer=%d riid=%s\n", ApiRef, obj, Buffer, sRIID(riid));
    res = (*pDXGIGetBuffer)(obj, Buffer, riid, ppSurface);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: surface=%#x\n", ApiRef, *ppSurface);
    return res;
}
#endif // TRACEALL

HRESULT WINAPI extDXGISetFullscreenState(void *obj, BOOL Fullscreen, IDXGIOutput *pTarget) {
    HRESULT res;
    ApiName("DXGISwapChain::SetFullscreenState");
    OutTraceD3D("%s: obj=%#x fullscreen=%#x pTarget=%#x\n",
                ApiRef, obj, Fullscreen, pTarget);
    if(dxw.Windowize && Fullscreen) {
        // v2.05.49: try setting Fullscreen=FALSE and pTarget to NULL, but be ready to ignore the error
        OutTraceD3D("%s: disable Fullscreen on SwapChain\n", ApiRef);
        dxw.SetFullScreen(TRUE);
        res = (*pDXGISetFullscreenState)(obj, FALSE, NULL);
        _if (res) OutTraceE("%s: ignore ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return DD_OK;
    }
    res = (*pDXGISetFullscreenState)(obj, Fullscreen, pTarget);
    _if (res) OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extDXGIGetFullscreenState(void *obj, BOOL *pFullscreen, IDXGIOutput **ppTarget) {
    HRESULT res;
    ApiName("DXGISwapChain::GetFullscreenState");
    OutTraceD3D("%s: obj=%#x\n", ApiRef, obj);
    res = (*pDXGIGetFullscreenState)(obj, pFullscreen, ppTarget);
    if (res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    // beware: ppTarget could be NULL
    OutTraceD3D("%s: Fullscreen=%#x pTarget=%#x\n", ApiRef, *pFullscreen, ppTarget ? *ppTarget : 0);
    HookDXGIOutput((void **)ppTarget);
    if(dxw.Windowize && dxw.IsFullScreen() && !*pFullscreen) {
        OutTraceD3D("%s: pretend Fullscreen on SwapChain\n", ApiRef);
        *pFullscreen = TRUE;
    }
    return res;
}

HRESULT WINAPI extDXSCGetDesc(void *obj, DXGI_SWAP_CHAIN_DESC *pDesc) {
    HRESULT res;
    ApiName("DXGISwapChain::GetDesc");
    OutTraceD3D("%s: obj=%#x\n", ApiRef, obj);
    res = (*pDXSCGetDesc)(obj, pDesc);
    if (res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    OutTraceD3D(">BufferDesc.Size=(%dx%d)\n", pDesc->BufferDesc.Width, pDesc->BufferDesc.Height);
    OutTraceD3D(">BufferDesc.Format=%d\n", pDesc->BufferDesc.Format);
    OutTraceD3D(">BufferDesc.Scaling=%d\n", pDesc->BufferDesc.Scaling);
    OutTraceD3D(">BufferDesc.ScanLineOrdering=%d\n", pDesc->BufferDesc.ScanlineOrdering);
    OutTraceD3D(">BufferDesc.RefreshRate=%d\n", pDesc->BufferDesc.RefreshRate);
    OutTraceD3D(">SampleDesc.Count=%d\n", pDesc->SampleDesc.Count);
    OutTraceD3D(">SampleDesc.Quality=%d\n", pDesc->SampleDesc.Quality);
    OutTraceD3D(">SampleDesc.BufferUsage=%d\n", pDesc->BufferUsage);
    OutTraceD3D(">SampleDesc.BufferCount=%d\n", pDesc->BufferCount);
    OutTraceD3D(">SampleDesc.OutputWindow=%#x\n", pDesc->OutputWindow);
    OutTraceD3D(">SampleDesc.Windowed=%#x\n", pDesc->Windowed);
    OutTraceD3D(">SampleDesc.SwapEffect=%#x\n", pDesc->SwapEffect);
    OutTraceD3D(">SampleDesc.Flags=%#x\n", pDesc->Flags);
    if(dxw.Windowize && dxw.IsFullScreen() && pDesc->Windowed) {
        OutTraceD3D("%s: pretend Fullscreen on SwapChain\n", ApiRef);
        pDesc->Windowed = FALSE;
        pDesc->BufferDesc.Width = dxw.GetScreenWidth();
        pDesc->BufferDesc.Height = dxw.GetScreenHeight();
    }
    return res;
}

HRESULT WINAPI extDXGIResizeBuffers(void *obj, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
    HRESULT res;
    ApiName("DXGISwapChain::ResizeBuffers");
    OutTraceD3D("%s: obj=%#x bufcnt=%d size=(%dx%d) format=%d flags=%#x\n",
                ApiRef, obj, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    res = (*pDXGIResizeBuffers)(obj, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    if (res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    if(dxw.Windowize)
        dxw.SetScreenSize(Width, Height);
    return res;
}

HRESULT WINAPI extDXGIResizeTarget(void *obj, const DXGI_MODE_DESC *pNewTargetParameters) {
    HRESULT res;
    ApiName("DXGISwapChain::ResizeTarget");
    OutTraceD3D("%s: obj=%#x newparms=%#x\n", ApiRef, obj, pNewTargetParameters);
    OutTraceD3D("> Size=(%dx%d)\n", pNewTargetParameters->Width, pNewTargetParameters->Height);
    OutTraceD3D("> RefreshRate=%d\n", pNewTargetParameters->RefreshRate);
    OutTraceD3D("> Format=%d\n", pNewTargetParameters->Format);
    OutTraceD3D("> ScanlineOrdering=%d\n", pNewTargetParameters->ScanlineOrdering);
    OutTraceD3D("> Scaling=%d\n", pNewTargetParameters->Scaling);
    res = (*pDXGIResizeTarget)(obj, pNewTargetParameters);
    if (res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    if(dxw.Windowize)
        dxw.SetScreenSize(pNewTargetParameters->Width, pNewTargetParameters->Height);
    return res;
}

#ifdef TRACEALL
HRESULT WINAPI extDXGIGetContainingOutput(void *obj, IDXGIOutput **ppOutput) {
    HRESULT res;
    ApiName("DXGISwapChain::GetContainingOutput");
    OutTraceD3D("%s: obj=%#x\n", ApiRef, obj);
    res = (*pDXGIGetContainingOutput)(obj, ppOutput);
    if (res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    OutTraceD3D("%s: pOutput=%#x\n", ApiRef, *ppOutput);
    return res;
}
#endif // TRACEALL