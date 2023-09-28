
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <D3D11.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhook.h"
#include "dxhelper.h"
#include "syslibs.h"
#include "stdio.h"
#include "dxdds.h"

//#define TRACEALL
extern void HookDXGIDevice(void **);

// ==== dll APIs

typedef HRESULT (WINAPI *D3D11CreateDevice_Type)(IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL *, UINT, UINT, ID3D11Device **, D3D_FEATURE_LEVEL *, ID3D11DeviceContext **);
typedef HRESULT (WINAPI *D3D11CreateDeviceAndSwapChain_Type)(IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL *, UINT, UINT, const DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **, ID3D11Device **, D3D_FEATURE_LEVEL *, ID3D11DeviceContext **);

HRESULT WINAPI extD3D11CreateDevice(IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL *, UINT, UINT, ID3D11Device **, D3D_FEATURE_LEVEL *, ID3D11DeviceContext **);
HRESULT WINAPI extD3D11CreateDeviceAndSwapChain(IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL *, UINT, UINT, const DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **, ID3D11Device **, D3D_FEATURE_LEVEL *, ID3D11DeviceContext **);

D3D11CreateDevice_Type pD3D11CreateDevice;
D3D11CreateDeviceAndSwapChain_Type pD3D11CreateDeviceAndSwapChain;

// ==== D3D11Device

typedef HRESULT (WINAPI *CreateBuffer_Type)(void *, const D3D11_BUFFER_DESC *, const D3D11_SUBRESOURCE_DATA *, ID3D11Buffer **);
typedef HRESULT (WINAPI *CreateTexture1D_Type)(void *, const D3D11_TEXTURE1D_DESC *, const D3D11_SUBRESOURCE_DATA *, ID3D11Texture1D **);
typedef HRESULT (WINAPI *CreateTexture2D_Type)(void *, const D3D11_TEXTURE2D_DESC *, const D3D11_SUBRESOURCE_DATA *, ID3D11Texture2D **);
typedef HRESULT (WINAPI *CreateTexture3D_Type)(void *, const D3D11_TEXTURE3D_DESC *, const D3D11_SUBRESOURCE_DATA *, ID3D11Texture3D **);
typedef HRESULT (WINAPI *CreateShaderResourceView_Type)(void *, ID3D11Resource *, const D3D11_SHADER_RESOURCE_VIEW_DESC *, ID3D11ShaderResourceView **);
typedef HRESULT (WINAPI *CreateUnorderedAccessView_Type)(void *, ID3D11Resource *, const D3D11_UNORDERED_ACCESS_VIEW_DESC *, ID3D11UnorderedAccessView **);
typedef HRESULT (WINAPI *CreateRenderTargetView_Type)(void *, ID3D11Resource *, const D3D11_RENDER_TARGET_VIEW_DESC *, ID3D11RenderTargetView **);
typedef HRESULT (WINAPI *CreateDepthStencilView_Type)(void *, ID3D11Resource *, const D3D11_DEPTH_STENCIL_VIEW_DESC *, ID3D11DepthStencilView **);
typedef HRESULT (WINAPI *CreateInputLayout_Type)(void *, const D3D11_INPUT_ELEMENT_DESC *, UINT, const void *, SIZE_T, ID3D11InputLayout **);
typedef HRESULT (WINAPI *CreateVertexShader_Type)(void *, const void *, SIZE_T, ID3D11ClassLinkage *, ID3D11VertexShader **);
typedef HRESULT (WINAPI *CreateGeometryShader_Type)(void *, const void *, SIZE_T, ID3D11ClassLinkage *, ID3D11GeometryShader **);
typedef HRESULT (WINAPI *CreateGeometryShaderWithStreamOutput_Type)(void *, const void *, SIZE_T, const D3D11_SO_DECLARATION_ENTRY *, UINT, const UINT *, UINT, UINT, ID3D11ClassLinkage *, ID3D11GeometryShader **);
typedef HRESULT (WINAPI *CreatePixelShader_Type)(void *, const void *, SIZE_T, ID3D11ClassLinkage *, ID3D11PixelShader **);
typedef HRESULT (WINAPI *CreateHullShader_Type)(void *, const void *, SIZE_T, ID3D11ClassLinkage *, ID3D11HullShader **);
typedef HRESULT (WINAPI *CreateDomainShader_Type)(void *, const void *, SIZE_T, ID3D11ClassLinkage *, ID3D11DomainShader **);
typedef HRESULT (WINAPI *CreateComputeShader_Type)(void *, const void *, SIZE_T, ID3D11ClassLinkage *, ID3D11ComputeShader **);

#ifdef TRACEALL
HRESULT WINAPI extCreateBuffer(void *, const D3D11_BUFFER_DESC *, const D3D11_SUBRESOURCE_DATA *, ID3D11Buffer **);
HRESULT WINAPI extCreateTexture1D(void *, const D3D11_TEXTURE1D_DESC *, const D3D11_SUBRESOURCE_DATA *, ID3D11Texture1D **);
HRESULT WINAPI extCreateTexture2D(void *, const D3D11_TEXTURE2D_DESC *, const D3D11_SUBRESOURCE_DATA *, ID3D11Texture2D **);
HRESULT WINAPI extCreateTexture3D(void *, const D3D11_TEXTURE3D_DESC *, const D3D11_SUBRESOURCE_DATA *, ID3D11Texture3D **);
HRESULT WINAPI extCreateShaderResourceView(void *, ID3D11Resource *, const D3D11_SHADER_RESOURCE_VIEW_DESC *, ID3D11ShaderResourceView **);
HRESULT WINAPI extCreateUnorderedAccessView(void *, ID3D11Resource *, const D3D11_UNORDERED_ACCESS_VIEW_DESC *, ID3D11UnorderedAccessView **);
HRESULT WINAPI extCreateRenderTargetView(void *, ID3D11Resource *, const D3D11_RENDER_TARGET_VIEW_DESC *, ID3D11RenderTargetView **);
HRESULT WINAPI extCreateDepthStencilView(void *, ID3D11Resource *, const D3D11_DEPTH_STENCIL_VIEW_DESC *, ID3D11DepthStencilView **);
HRESULT WINAPI extCreateInputLayout(void *, const D3D11_INPUT_ELEMENT_DESC *, UINT, const void *, SIZE_T, ID3D11InputLayout **);
HRESULT WINAPI extCreateVertexShader(void *, const void *, SIZE_T, ID3D11ClassLinkage *, ID3D11VertexShader **);
HRESULT WINAPI extCreateGeometryShader(void *, const void *, SIZE_T, ID3D11ClassLinkage *, ID3D11GeometryShader **);
HRESULT WINAPI extCreateGeometryShaderWithStreamOutput(void *, const void *, SIZE_T, const D3D11_SO_DECLARATION_ENTRY *, UINT, const UINT *, UINT, UINT, ID3D11ClassLinkage *, ID3D11GeometryShader **);
HRESULT WINAPI extCreatePixelShader(void *, const void *, SIZE_T, ID3D11ClassLinkage *, ID3D11PixelShader **);
HRESULT WINAPI extCreateHullShader(void *, const void *, SIZE_T, ID3D11ClassLinkage *, ID3D11HullShader **);
HRESULT WINAPI extCreateDomainShader(void *, const void *, SIZE_T, ID3D11ClassLinkage *, ID3D11DomainShader **);
HRESULT WINAPI extCreateComputeShader(void *, const void *, SIZE_T, ID3D11ClassLinkage *, ID3D11ComputeShader **);
#endif // TRACEALL

#ifdef TRACEALL
CreateBuffer_Type pCreateBuffer;
CreateTexture1D_Type pCreateTexture1D;
CreateTexture2D_Type pCreateTexture2D;
CreateTexture3D_Type pCreateTexture3D;
CreateShaderResourceView_Type pCreateShaderResourceView;
CreateUnorderedAccessView_Type pCreateUnorderedAccessView;
CreateRenderTargetView_Type pCreateRenderTargetView;
CreateDepthStencilView_Type pCreateDepthStencilView;
CreateInputLayout_Type pCreateInputLayout;
CreateVertexShader_Type pCreateVertexShader;
CreateGeometryShader_Type pCreateGeometryShader;
CreateGeometryShaderWithStreamOutput_Type pCreateGeometryShaderWithStreamOutput;
CreatePixelShader_Type pCreatePixelShader;
CreateHullShader_Type pCreateHullShader;
CreateDomainShader_Type pCreateDomainShader;
CreateComputeShader_Type pCreateComputeShader;
#endif // TRACEALL

// ==== ID3D11DeviceContext

#ifdef TRACEALL
typedef void (WINAPI *GetDevice_Type)(void *, ID3D11Device **);

void WINAPI extGetDevice(void *, ID3D11Device **);

GetDevice_Type pGetDevice;
#endif // TRACEALL

// ==== ID3D11Device

typedef HRESULT (WINAPI *QueryInterface_Type)(void *, REFIID riid, void **ppvObj);
typedef ULONG	(WINAPI *AddRef_Type)(void *);
typedef ULONG	(WINAPI *Release_Type)(void *);

HRESULT WINAPI extD3D11QueryInterface(void *, REFIID, void **);
#ifdef TRACEALL
ULONG WINAPI extD3D11AddRef(void *);
ULONG WINAPI extD3D11Release(void *);
#endif // TRACEALL

QueryInterface_Type pD3D11QueryInterface;
#ifdef TRACEALL
AddRef_Type pD3D11AddRef;
Release_Type pD3D11Release;
#endif // TRACEALL

static HookEntryEx_Type d3d11Hooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "D3D11CreateDevice", (FARPROC)NULL, (FARPROC *) &pD3D11CreateDevice, (FARPROC)extD3D11CreateDevice},
    {HOOK_HOT_CANDIDATE, 0, "D3D11CreateDeviceAndSwapChain", (FARPROC)NULL, (FARPROC *) &pD3D11CreateDeviceAndSwapChain, (FARPROC)extD3D11CreateDeviceAndSwapChain},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

FARPROC Remap_d3d11_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if (dxw.dwTargetDDVersion == HOOKDDRAWNONE) return NULL;
    if (addr = RemapLibraryEx(proc, hModule, d3d11Hooks)) return addr;
    return NULL;
}

void HookDirect3D11(HMODULE module) {
    ID3D11Device *lpd3d11;
    int version = dxw.dwTargetDDVersion;
    OutTraceDW("HookDirect3D11: module=%#x version=%d\n", module, version);
    switch(version) {
    case 0:
        HookLibraryEx(module, d3d11Hooks, "d3d11.dll");
        break;
    case 11:
        PinLibraryEx(d3d11Hooks, "d3d11.dll");
        HookLibraryEx(module, d3d11Hooks, "d3d11.dll");
        if(pD3D11CreateDevice) {
            HRESULT res;
            D3D_FEATURE_LEVEL FeatureLevel;
            ID3D11DeviceContext *pImmediateContext;
            res = extD3D11CreateDevice(
                      NULL,
                      D3D_DRIVER_TYPE_HARDWARE,
                      NULL,
                      0, // flags
                      NULL, // FeatureLevels
                      0,
                      D3D11_SDK_VERSION,
                      &lpd3d11,
                      &FeatureLevel,
                      &pImmediateContext);
            if(res == DD_OK) lpd3d11->Release();
        }
    }
}

//typedef HRESULT (WINAPI *D3D11CreateDevice_Type)(IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL *, UINT, UINT, ID3D11Device **, D3D_FEATURE_LEVEL *, ID3D11DeviceContext **);
#ifdef TRACEALL
typedef void	(WINAPI *RSSetViewports11_Type)(void *, UINT, D3D11_VIEWPORT *);

void WINAPI extRSSetViewports11(ID3D11DeviceContext *, UINT, D3D11_VIEWPORT *);

RSSetViewports11_Type pRSSetViewports11;
#endif // TRACEALL

void HookD3D11Device(ID3D11Device **ppDevice) {
    OutTrace("HookD3D11Device\n");
    SetHook((void *)(**(DWORD **)ppDevice +  0), extD3D11QueryInterface, (void **)&pD3D11QueryInterface, "QueryInterface(D11)");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)ppDevice +  4), extD3D11AddRef, (void **)&pD3D11AddRef, "AddRef(D11)");
    SetHook((void *)(**(DWORD **)ppDevice +  8), extD3D11Release, (void **)&pD3D11Release, "Release(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 12), extCreateBuffer, (void **)&pCreateBuffer, "CreateBuffer(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 16), extCreateTexture1D, (void **)&pCreateTexture1D, "CreateTexture1D(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 20), extCreateTexture2D, (void **)&pCreateTexture2D, "CreateTexture2D(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 24), extCreateTexture3D, (void **)&pCreateTexture3D, "CreateTexture3D(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 28), extCreateShaderResourceView, (void **)&pCreateShaderResourceView, "CreateShaderResourceView(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 32), extCreateUnorderedAccessView, (void **)&pCreateUnorderedAccessView, "CreateUnorderedAccessView(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 36), extCreateRenderTargetView, (void **)&pCreateRenderTargetView, "CreateRenderTargetView(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 40), extCreateDepthStencilView, (void **)&pCreateDepthStencilView, "CreateDepthStencilView(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 44), extCreateInputLayout, (void **)&pCreateInputLayout, "CreateInputLayout(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 48), extCreateVertexShader, (void **)&pCreateVertexShader, "CreateVertexShader(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 52), extCreateGeometryShader, (void **)&pCreateGeometryShader, "CreateGeometryShader(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 56), extCreateGeometryShaderWithStreamOutput, (void **)&pCreateGeometryShaderWithStreamOutput, "CreateGeometryShaderWithStreamOutput(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 60), extCreatePixelShader, (void **)&pCreatePixelShader, "CreatePixelShader(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 64), extCreateHullShader, (void **)&pCreateHullShader, "CreateHullShader(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 68), extCreateDomainShader, (void **)&pCreateDomainShader, "CreateDomainShader(D11)");
    SetHook((void *)(**(DWORD **)ppDevice + 72), extCreateComputeShader, (void **)&pCreateComputeShader, "CreateComputeShader(D11)");
#endif // TRACEALL
}

void HookD3D11Context(ID3D11DeviceContext **ppImmeCtx) {
    OutTrace("HookD3D11Context\n");
    /*  C DWORD offsets
        7 VSSetConstantBuffers
        12 DrawIndexed
        16 PSSetConstantBuffers
        22 GSSetConstantBuffers
        26 VSSetSamplers
        27 Begin
        28 End
        35 OMSetBlendState
        40 DrawInstancedIndirect
        44 RSSetViewports
    */
#ifdef TRACEALL
    //SetHook((void *)(**(DWORD **)ppImmeCtx +  0), extD3D11QueryInterface, (void **)&pD3D11QueryInterface, "QueryInterface(D11)");
    SetHook((void *)(**(DWORD **)ppImmeCtx +  12), extGetDevice, (void **)&pGetDevice, "GetDevice(D11)");
    //SetHook((void *)(**(DWORD **)ppImmeCtx + 152), extRSSetScissorRects, (void **)&pRSSetScissorRects, "RSSetScissorRects(D11)");
    SetHook((void *)(**(DWORD **)ppImmeCtx + 176), extRSSetViewports11, (void **)&pRSSetViewports11, "RSSetViewports(D11)");
#endif // TRACEALL
}

#ifndef DXW_NOTRACES
char *sDimension(DWORD dim) {
    char *captions[] = {
        "UNKNOWN", "BUFFER", "TEXTURE1D", "TEXTURE1DARRAY", "TEXTURE2D",
        "TEXTURE2DARRAY", "TEXTURE2DMS", "TEXTURE2DMSARRAY", "TEXTURE3D",
        "invalid"
    };
    if(dim > D3D11_RTV_DIMENSION_TEXTURE3D) dim = D3D11_RTV_DIMENSION_TEXTURE3D + 1;
    return captions[dim];
}
#endif

HRESULT WINAPI extD3D11CreateDevice(
    IDXGIAdapter *pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    const D3D_FEATURE_LEVEL *pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    ID3D11Device **ppDevice,
    D3D_FEATURE_LEVEL *pFeatureLevel,
    ID3D11DeviceContext **ppImmediateContext) {
    HRESULT res;
    ApiName("D3D11CreateDevice");
    OutTraceD3D("%s: Adapter=%#x DriverType=%#x Flags=%#x FeatureLevels=%#x SDKVersion=%#x\n",
                ApiRef, pAdapter, DriverType, Flags, FeatureLevels, SDKVersion);
    res = (*pD3D11CreateDevice)(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);
    if(res) {
        OutTraceE("%s ERROR: ret=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    if(ppDevice) HookD3D11Device(ppDevice);
    if(ppImmediateContext) HookD3D11Context(ppImmediateContext);
    OutTraceD3D("%s OK: device=%#x immcontext=%#x\n",
                ApiRef,
                ppDevice ? *ppDevice : 0,
                ppImmediateContext ? *ppImmediateContext : 0
               );
    return res;
}

HRESULT WINAPI extD3D11CreateDeviceAndSwapChain(
    IDXGIAdapter *pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    const D3D_FEATURE_LEVEL *pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
    IDXGISwapChain **ppSwapChain,
    ID3D11Device **ppDevice,
    D3D_FEATURE_LEVEL *pFeatureLevel,
    ID3D11DeviceContext **ppImmediateContext) {
    HRESULT res;
    ApiName("CreateDeviceAndSwapChain(D3D11)");
    OutTraceD3D("%s: DriverType=%#x Flags=%#x FeatureLevels=%#x SDKVersion=%#x\n", ApiRef, DriverType, Flags, FeatureLevels, SDKVersion);
    res = (*pD3D11CreateDeviceAndSwapChain)(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s OK: device=%#x immcontext=%#x swapchain=%#x\n",
                ApiRef,
                ppDevice ? *ppDevice : 0,
                ppImmediateContext ? *ppImmediateContext : 0,
                ppSwapChain ? *ppSwapChain : 0
               );
    return res;
}

HRESULT WINAPI extD3D11QueryInterface(void *obj, REFIID riid, void **ppvObj) {
    HRESULT res;
    ApiName("QueryInterface(D3D11)");
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
    //DEFINE_GUID(IID_IDXGIObject,0xaec22fb8,0x76f3,0x4639,0x9b,0xe0,0x28,0xeb,0x43,0xa6,0x7a,0x2e);
    //DEFINE_GUID(IID_IDXGIDeviceSubObject,0x3d3e0379,0xf9de,0x4d58,0xbb,0x6c,0x18,0xd6,0x29,0x92,0xf1,0xa6);
    //DEFINE_GUID(IID_IDXGIResource,0x035f3ab4,0x482e,0x4e50,0xb4,0x1f,0x8a,0x7f,0x8b,0xd8,0x96,0x0b);
    //DEFINE_GUID(IID_IDXGIKeyedMutex,0x9d8e1289,0xd7b3,0x465f,0x81,0x26,0x25,0x0e,0x34,0x9a,0xf8,0x5d);
    //DEFINE_GUID(IID_IDXGISurface,0xcafcb56c,0x6ac3,0x4889,0xbf,0x47,0x9e,0x23,0xbb,0xd2,0x60,0xec);
    //DEFINE_GUID(IID_IDXGISurface1,0x4AE63092,0x6327,0x4c1b,0x80,0xAE,0xBF,0xE1,0x2E,0xA3,0x2B,0x86);
    //DEFINE_GUID(IID_IDXGIAdapter,0x2411e7e1,0x12ac,0x4ccf,0xbd,0x14,0x97,0x98,0xe8,0x53,0x4d,0xc0);
    //DEFINE_GUID(IID_IDXGIOutput,0xae02eedb,0xc735,0x4690,0x8d,0x52,0x5a,0x8d,0xc2,0x02,0x13,0xaa);
    //DEFINE_GUID(IID_IDXGISwapChain,0x310d36a0,0xd2e7,0x4c0a,0xaa,0x04,0x6a,0x9d,0x23,0xb8,0x88,0x6a);
    //DEFINE_GUID(IID_IDXGIFactory,0x7b7166ec,0x21c7,0x44ae,0xb2,0x1a,0xc9,0xae,0x32,0x1a,0xe3,0x69);
    //DEFINE_GUID(IID_IDXGIDevice,0x54ec77fa,0x1377,0x44e6,0x8c,0x32,0x88,0xfd,0x5f,0x44,0xc8,0x4c);
    //DEFINE_GUID(IID_IDXGIFactory1,0x770aae78,0xf26f,0x4dba,0xa8,0x29,0x25,0x3c,0x83,0xd1,0xb3,0x87);
    //DEFINE_GUID(IID_IDXGIAdapter1,0x29038f61,0x3839,0x4626,0x91,0xfd,0x08,0x68,0x79,0x01,0x1a,0x05);
    //DEFINE_GUID(IID_IDXGIDevice1,0x77db970f,0x6276,0x48ba,0xba,0x28,0x07,0x01,0x43,0xb4,0x39,0x2c);
    // 189819F1-1DB6-4B57-BE54-1821339B85F7 IID_ID3D12Device
    // 79CF2233-7536-4948-9D36-1E4692DC5760 ID3D11Debug (D3D11SDKLayers.h)
    res = pD3D11QueryInterface(obj, riid, ppvObj);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    } else
        OutTraceD3D("%s: pvObj=%#x\n", ApiRef, *ppvObj);
    switch(riid.Data1) {
    case 0x54EC77FA:
        OutTraceD3D("> IDXGIDevice\n");
        HookDXGIDevice(ppvObj);
        break;
    case 0x77db970f:
        OutTraceD3D("> IID_IDXGIDevice1\n");
        HookDXGIDevice(ppvObj);
        break;
    case 0xA04BFB29:
        OutTraceD3D("> ID3D11Device1\n");
        //HookD3D11Device((ID3D11Device **)ppvObj);
        break;
    case 0x9D06DFFA:
        OutTraceD3D("> ID3D11Device2\n");
        //HookD3D11Device((ID3D11Device **)ppvObj);
        break;
    case 0x189819f1:  // 189819f1-1db6-4b57-be54-1821339b85f7
        OutTraceD3D("> ID3D12Device\n");
        break;
    default:
        OutTraceD3D("> unknown GUID\n");
        break;
    }
    OutTraceD3D("%s: obj=%#x\n", ApiRef, *ppvObj);
    return res;
}

#ifdef TRACEALL
ULONG WINAPI extD3D11AddRef(void *lpd3d11d) {
    ULONG res;
#ifndef DXW_NOTRACES
    ApiName("AddRef(D3D11Device)");
    OutTraceD3D("%s: device=%#x\n", ApiRef, lpd3d11d);
#endif
    res = (*pD3D11AddRef)(lpd3d11d);
#ifndef DXW_NOTRACES
    OutTraceD3D("%s: ref=%d\n", ApiRef, res);
#endif
    return res;
}

ULONG WINAPI extD3D11Release(void *lpd3d11d) {
    ULONG res;
#ifndef DXW_NOTRACES
    ApiName("Release(D3D11Device)");
    OutTraceD3D("%s: device=%#x\n", ApiRef, lpd3d11d);
#endif
    res = (*pD3D11Release)(lpd3d11d);
#ifndef DXW_NOTRACES
    OutTraceD3D("%s: ref=%d\n", ApiRef, res);
#endif
    return res;
}

void WINAPI extRSSetViewports11(ID3D11DeviceContext *This, UINT NumViewports, D3D11_VIEWPORT *pViewports) {
#ifndef DXW_NOTRACES
    ApiName("RSSetViewports(D3D11)");
    OutTraceD3D("%s: NumViewports=%d\n", ApiRef, NumViewports);
    for(UINT i = 0; i < NumViewports; i++) {
        OutTraceD3D("> viewport[%d]: pos=(%f,%f) size=(%fx%f) depth(min:max)=(%f:%d)\n",
                    i, pViewports[i].TopLeftX, pViewports[i].TopLeftY, pViewports[i].Width, pViewports[i].Height, pViewports[i].MinDepth, pViewports[i].MaxDepth);
    }
#endif
    //if(dxw.Windowize && 0){
    //	for(UINT i=0; i<NumViewports; i++){
    //		dxw.MapClient(&(pViewports[i].TopLeftX), &(pViewports[i].TopLeftY));
    //		dxw.MapClient(&(pViewports[i].Width), &(pViewports[i].Height));
    //		OutTraceD3D("> FIXED viewport[%d]: pos=(%f,%f) size=(%fx%f)\n",
    //			i, pViewports[i].TopLeftX, pViewports[i].TopLeftY, pViewports[i].Width, pViewports[i].Height);
    //	}
    //}
    (*pRSSetViewports11)(This, NumViewports, pViewports);
}

HRESULT WINAPI extCreateBuffer(void *lpDev, const D3D11_BUFFER_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Buffer **ppBuffer) {
    HRESULT res;
#ifndef DXW_NOTRACES
    ApiName("CreateBuffer(D3D11)");
    OutDebugD3D("%s: device=%#x\n", ApiRef, lpDev);
    OutDebugD3D("> D3D11_BUFFER_DESC=%#x\n", pDesc);
    if(pDesc) {
        OutDebugD3D(">> ByteWidth=%d\n", pDesc->ByteWidth);
        OutDebugD3D(">> Usage=%d(%s)\n", pDesc->Usage, "tbd");
        OutDebugD3D(">> BindFlags=%X\n", pDesc->BindFlags);
        OutDebugD3D(">> CPUAccessFlags=%X\n", pDesc->CPUAccessFlags);
        OutDebugD3D(">> MiscFlags=%X\n", pDesc->MiscFlags);
        OutDebugD3D(">> StructureByteStride=%d\n", pDesc->StructureByteStride);
    }
    OutDebugD3D("> D3D11_SUBRESOURCE_DATA=%#x\n", pInitialData);
    if(pInitialData) {
        OutDebugD3D(">> pSysMem=%#x\n", pInitialData->pSysMem);
        OutDebugD3D(">> SysMemPitch=%d\n", pInitialData->SysMemPitch);
        OutDebugD3D(">> SysMemSlicePitch=%d\n", pInitialData->SysMemSlicePitch);
    }
#endif
    res = (*pCreateBuffer)(lpDev, pDesc, pInitialData, ppBuffer);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    } else
        OutTraceD3D("%s: pBuffer=%#x\n", ApiRef, *ppBuffer);
    return res;
}

HRESULT WINAPI extCreateTexture1D(void *lpDev, const D3D11_TEXTURE1D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture1D **ppTexture1D) {
    HRESULT res;
#ifndef DXW_NOTRACES
    ApiName("CreateTexture1D(D3D11)");
    OutTraceD3D("%s: device=%#x\n", ApiRef, lpDev);
    OutDebugD3D("> D3D11_TEXTURE1D_DESC=%#x\n", pDesc);
    if(pDesc) {
        OutDebugD3D(">> Width=%d\n", pDesc->Width);
        OutDebugD3D(">> MipLevels=%d\n", pDesc->MipLevels);
        OutDebugD3D(">> ArraySize=%d\n", pDesc->ArraySize);
        OutDebugD3D(">> Format=%d(%s)\n", pDesc->Format, "tbd");
        OutDebugD3D(">> Usage=%d(%s)\n", pDesc->Usage, "tbd");
        OutDebugD3D(">> BindFlags=%X\n", pDesc->BindFlags);
        OutDebugD3D(">> CPUAccessFlags=%X\n", pDesc->CPUAccessFlags);
        OutDebugD3D(">> MiscFlags=%X\n", pDesc->MiscFlags);
    }
    OutDebugD3D("> D3D11_SUBRESOURCE_DATA=%#x\n", pInitialData);
    if(pInitialData) {
        OutDebugD3D(">> pSysMem=%#x\n", pInitialData->pSysMem);
        OutDebugD3D(">> SysMemPitch=%d\n", pInitialData->SysMemPitch);
        OutDebugD3D(">> SysMemSlicePitch=%d\n", pInitialData->SysMemSlicePitch);
    }
#endif
    res = (*pCreateTexture1D)(lpDev, pDesc, pInitialData, ppTexture1D);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    } else
        OutTraceD3D("%s: pTexture1D=%#x\n", ApiRef, *ppTexture1D);
    return res;
}

HRESULT WINAPI extCreateTexture2D(void *lpDev, const D3D11_TEXTURE2D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture2D **ppTexture2D) {
    HRESULT res;
#ifndef DXW_NOTRACES
    ApiName("CreateTexture2D(D3D11)");
    OutTraceD3D("%s: device=%#x\n", ApiRef, lpDev);
    OutDebugD3D("> D3D11_TEXTURE2D_DESC=%#x\n", pDesc);
    if(pDesc) {
        OutDebugD3D(">> Width=%d\n", pDesc->Width);
        OutDebugD3D(">> Height=%d\n", pDesc->Height);
        OutDebugD3D(">> MipLevels=%d\n", pDesc->MipLevels);
        OutDebugD3D(">> ArraySize=%d\n", pDesc->ArraySize);
        OutDebugD3D(">> Format=%d(%s)\n", pDesc->Format, "tbd");
        OutDebugD3D(">> SampleDesc.Count=%d\n", pDesc->SampleDesc.Count);
        OutDebugD3D(">> SampleDesc.Quality=%d\n", pDesc->SampleDesc.Quality);
        OutDebugD3D(">> Usage=%d(%s)\n", pDesc->Usage, "tbd");
        OutDebugD3D(">> BindFlags=%X\n", pDesc->BindFlags);
        OutDebugD3D(">> CPUAccessFlags=%X\n", pDesc->CPUAccessFlags);
        OutDebugD3D(">> MiscFlags=%X\n", pDesc->MiscFlags);
    }
    OutDebugD3D("> D3D11_SUBRESOURCE_DATA=%#x\n", pInitialData);
    if(pInitialData) {
        OutDebugD3D(">> pSysMem=%#x\n", pInitialData->pSysMem);
        OutDebugD3D(">> SysMemPitch=%d\n", pInitialData->SysMemPitch);
        OutDebugD3D(">> SysMemSlicePitch=%d\n", pInitialData->SysMemSlicePitch);
    }
#endif
    res = (*pCreateTexture2D)(lpDev, pDesc, pInitialData, ppTexture2D);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    } else
        OutTraceD3D("%s: pTexture2D=%#x\n", ApiRef, *ppTexture2D);
    return res;
}

HRESULT WINAPI extCreateTexture3D(void *lpDev, const D3D11_TEXTURE3D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture3D **ppTexture3D) {
    HRESULT res;
#ifndef DXW_NOTRACES
    ApiName("CreateTexture3D(D3D11)");
    OutTraceD3D("%s: device=%#x\n", ApiRef, lpDev);
    OutDebugD3D("> D3D11_TEXTURE3D_DESC=%#x\n", pDesc);
    if(pDesc) {
        OutDebugD3D(">> Width=%d\n", pDesc->Width);
        OutDebugD3D(">> Height=%d\n", pDesc->Height);
        OutDebugD3D(">> Depth=%d\n", pDesc->Depth);
        OutDebugD3D(">> MipLevels=%d\n", pDesc->MipLevels);
        OutDebugD3D(">> Format=%d(%s)\n", pDesc->Format, "tbd");
        OutDebugD3D(">> Usage=%d(%s)\n", pDesc->Usage, "tbd");
        OutDebugD3D(">> BindFlags=%X\n", pDesc->BindFlags);
        OutDebugD3D(">> CPUAccessFlags=%X\n", pDesc->CPUAccessFlags);
        OutDebugD3D(">> MiscFlags=%X\n", pDesc->MiscFlags);
    }
    OutDebugD3D("> D3D11_SUBRESOURCE_DATA=%#x\n", pInitialData);
    if(pInitialData) {
        OutDebugD3D(">> pSysMem=%#x\n", pInitialData->pSysMem);
        OutDebugD3D(">> SysMemPitch=%d\n", pInitialData->SysMemPitch);
        OutDebugD3D(">> SysMemSlicePitch=%d\n", pInitialData->SysMemSlicePitch);
    }
#endif
    res = (*pCreateTexture3D)(lpDev, pDesc, pInitialData, ppTexture3D);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    } else
        OutTraceD3D("%s: pTexture3D=%#x\n", ApiRef, *ppTexture3D);
    return res;
}

HRESULT WINAPI extCreateShaderResourceView(void *lpDev, ID3D11Resource *pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc, ID3D11ShaderResourceView **ppSRView) {
    HRESULT res;
#ifndef DXW_NOTRACES
    ApiName("CreateShaderResourceView(D3D11)");
    OutTraceD3D("%s: device=%#x resource=%#x\n", ApiRef, lpDev, pResource);
    OutDebugD3D("> D3D11_SHADER_RESOURCE_VIEW_DESC=%#x\n", pDesc);
    if(pDesc) {
        OutDebugD3D(">> Format=%d(%s)\n", pDesc->Format, "tbd");
        OutDebugD3D(">> ViewDimension=%d(%s)\n", pDesc->ViewDimension, sDimension(pDesc->ViewDimension));
    }
#endif
    res = (*pCreateShaderResourceView)(lpDev, pResource, pDesc, ppSRView);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    } else
        OutTraceD3D("%s: pSRView=%#x\n", ApiRef, *ppSRView);
    return res;
}

HRESULT WINAPI extCreateUnorderedAccessView(void *lpDev, ID3D11Resource *pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc, ID3D11UnorderedAccessView **ppUAView) {
    HRESULT res;
#ifndef DXW_NOTRACES
    ApiName("CreateUnorderedAccessView(D3D11)");
    OutTraceD3D("%s: device=%#x resource=%#x\n", ApiRef, lpDev, pResource);
    OutDebugD3D("> D3D11_UNORDERED_ACCESS_VIEW_DESC=%#x\n", pDesc);
    if(pDesc) {
        OutDebugD3D(">> Format=%d(%s)\n", pDesc->Format, "tbd");
        OutDebugD3D(">> ViewDimension=%d(%s)\n", pDesc->ViewDimension, sDimension(pDesc->ViewDimension));
    }
#endif
    res = (*pCreateUnorderedAccessView)(lpDev, pResource, pDesc, ppUAView);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    } else
        OutTraceD3D("%s: pUAView=%#x\n", ApiRef, *ppUAView);
    return res;
}

HRESULT WINAPI extCreateRenderTargetView(void *lpDev, ID3D11Resource *pResource, const D3D11_RENDER_TARGET_VIEW_DESC *pDesc, ID3D11RenderTargetView **ppRTView) {
    HRESULT res;
#ifndef DXW_NOTRACES
    ApiName("CreateRenderTargetView(D3D11)");
    OutTraceD3D("%s: device=%#x pResource=%#x\n", ApiRef, lpDev, pResource);
    OutDebugD3D("> D3D11_RENDER_TARGET_VIEW_DESC=%#x\n", pDesc);
    if(pDesc) {
        OutDebugD3D(">> Format=%d(%s)\n", pDesc->Format, "tbd");
        OutDebugD3D(">> ViewDimension=%d(%s)\n", pDesc->ViewDimension, sDimension(pDesc->ViewDimension));
    }
#endif
    res = (*pCreateRenderTargetView)(lpDev, pResource, pDesc, ppRTView);
    return res;
}

HRESULT WINAPI extCreateDepthStencilView(void *lpDev, ID3D11Resource *pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc, ID3D11DepthStencilView **ppDepthStencilView) {
    HRESULT res;
#ifndef DXW_NOTRACES
    ApiName("CreateDepthStencilView(D3D11)");
    OutTraceD3D("%s: device=%#x resource=%#x\n", ApiRef, lpDev, pResource);
    OutDebugD3D("> D3D11_DEPTH_STENCIL_VIEW_DESC=%#x\n", pDesc);
    if(pDesc) {
        OutDebugD3D(">> Format=%d(%s)\n", pDesc->Format, "tbd");
        OutDebugD3D(">> ViewDimension=%d(%s)\n", pDesc->ViewDimension, sDimension(pDesc->ViewDimension));
    }
#endif
    res = (*pCreateDepthStencilView)(lpDev, pResource, pDesc, ppDepthStencilView);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    } else
        OutTraceD3D("%s: pDepthStencilView=%#x\n", ApiRef, *ppDepthStencilView);
    return res;
}

HRESULT WINAPI extCreateInputLayout(void *lpDev, const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements, const void *pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout **ppInputLayout) {
    HRESULT res;
    ApiName("CreateInputLayout(D3D11)");
    OutTraceD3D("%s: device=%#x ... numelements=%d ...\n", ApiRef, lpDev, NumElements);
    res = (*pCreateInputLayout)(lpDev, pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: pInputLayout=%#x\n", ApiRef, ppInputLayout ? *ppInputLayout : 0);
    // hook ppInputLayout ???
    return res;
}

HRESULT WINAPI extCreateVertexShader(void *lpDev, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11VertexShader **ppVertexShader) {
    HRESULT res;
    ApiName("CreateVertexShader(D3D11)");
    OutTraceD3D("%s: device=%#x Bytecode=%#x BytecodeLen=%d ClassLinkage=%#x\n", ApiRef, lpDev, pShaderBytecode, BytecodeLength, pClassLinkage);
    res = (*pCreateVertexShader)(lpDev, pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: pVertexShader=%#x\n", ApiRef, *ppVertexShader);
    // hook ppVertexShader ???
    return res;
}

HRESULT WINAPI extCreateGeometryShader(void *lpDev, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11GeometryShader **ppGeometryShader) {
    HRESULT res;
    ApiName("CreateGeometryShader(D3D11)");
    OutTraceD3D("%s: device=%#x Bytecode=%#x BytecodeLen=%d ClassLinkage=%#x\n", ApiRef, lpDev, pShaderBytecode, BytecodeLength, pClassLinkage);
    res = (*pCreateGeometryShader)(lpDev, pShaderBytecode, BytecodeLength, pClassLinkage, ppGeometryShader);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: pGeometryShader=%#x\n", ApiRef, *ppGeometryShader);
    // hook ppGeometryShader ???
    return res;
}

HRESULT WINAPI extCreateGeometryShaderWithStreamOutput(void *lpDev, const void *pShaderBytecode, SIZE_T BytecodeLength, const D3D11_SO_DECLARATION_ENTRY *pSODeclaration,
        UINT NumEntries, const UINT *pBufferStrides, UINT NumStrides, UINT RasterizedStream, ID3D11ClassLinkage *pClassLinkage, ID3D11GeometryShader **ppGeometryShader) {
    HRESULT res;
    ApiName("CreateGeometryShaderWithStreamOutput(D3D11)");
    OutTraceD3D("%s: device=%#x Bytecode=%#x BytecodeLen=%d ClassLinkage=%#x\n", ApiRef, lpDev, pShaderBytecode, BytecodeLength, pClassLinkage);
    res = (*pCreateGeometryShaderWithStreamOutput)(lpDev, pShaderBytecode, BytecodeLength, pSODeclaration, NumEntries, pBufferStrides, NumStrides, RasterizedStream, pClassLinkage, ppGeometryShader);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: pGeometryShader=%#x\n", ApiRef, *ppGeometryShader);
    // hook ppGeometryShader ???
    return res;
}

HRESULT WINAPI extCreatePixelShader(void *lpDev, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11PixelShader **ppPixelShader) {
    HRESULT res;
    ApiName("CreatePixelShader(D3D11)");
    OutTraceD3D("%s: device=%#x Bytecode=%#x BytecodeLen=%d ClassLinkage=%#x\n", ApiRef, lpDev, pShaderBytecode, BytecodeLength, pClassLinkage);
    res = (*pCreatePixelShader)(lpDev, pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: pPixelShader=%#x\n", ApiRef, *ppPixelShader);
    // hook ppGeometryShader ???
    return res;
}

HRESULT WINAPI extCreateHullShader(void *lpDev, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11HullShader **ppHullShader) {
    HRESULT res;
    ApiName("CreateHullShader(D3D11)");
    OutTraceD3D("%s: device=%#x Bytecode=%#x BytecodeLen=%d ClassLinkage=%#x\n", ApiRef, lpDev, pShaderBytecode, BytecodeLength, pClassLinkage);
    res = (*pCreateHullShader)(lpDev, pShaderBytecode, BytecodeLength, pClassLinkage, ppHullShader);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: pHullShader=%#x\n", ApiRef, *ppHullShader);
    // hook ppHullShader ???
    return res;
}

HRESULT WINAPI extCreateDomainShader(void *lpDev, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11DomainShader **ppDomainShader) {
    HRESULT res;
    ApiName("CreateDomainShader(D3D11)");
    OutTraceD3D("%s: device=%#x Bytecode=%#x BytecodeLen=%d ClassLinkage=%#x\n", ApiRef, lpDev, pShaderBytecode, BytecodeLength, pClassLinkage);
    res = (*pCreateDomainShader)(lpDev, pShaderBytecode, BytecodeLength, pClassLinkage, ppDomainShader);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: pDomainShader=%#x\n", ApiRef, *ppDomainShader);
    // hook ppDomainShader ???
    return res;
}

HRESULT WINAPI extCreateComputeShader(void *lpDev, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11ComputeShader **ppComputeShader) {
    HRESULT res;
    ApiName("CreateComputeShader(D3D11)");
    OutTraceD3D("%s: device=%#x Bytecode=%#x BytecodeLen=%d ClassLinkage=%#x\n", ApiRef, lpDev, pShaderBytecode, BytecodeLength, pClassLinkage);
    res = (*pCreateComputeShader)(lpDev, pShaderBytecode, BytecodeLength, pClassLinkage, ppComputeShader);
    if(res) {
        OutTraceE("%s ERROR: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: pComputeShader=%#x\n", ApiRef, *ppComputeShader);
    // hook ppComputeShader ???
    return res;
}

#if 0

HRESULT WINAPI extCreateClassLinkage(
    /* [annotation] */
    __out  ID3D11ClassLinkage **ppLinkage) = 0;

HRESULT WINAPI extCreateBlendState(
    /* [annotation] */
    __in  const D3D11_BLEND_DESC *pBlendStateDesc,
    /* [annotation] */
    __out_opt  ID3D11BlendState **ppBlendState) = 0;

HRESULT WINAPI extCreateDepthStencilState(
    /* [annotation] */
    __in  const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc,
    /* [annotation] */
    __out_opt  ID3D11DepthStencilState **ppDepthStencilState) = 0;

HRESULT WINAPI extCreateRasterizerState(
    /* [annotation] */
    __in  const D3D11_RASTERIZER_DESC *pRasterizerDesc,
    /* [annotation] */
    __out_opt  ID3D11RasterizerState **ppRasterizerState) = 0;

HRESULT WINAPI extCreateSamplerState(
    /* [annotation] */
    __in  const D3D11_SAMPLER_DESC *pSamplerDesc,
    /* [annotation] */
    __out_opt  ID3D11SamplerState **ppSamplerState) = 0;

HRESULT WINAPI extCreateQuery(
    /* [annotation] */
    __in  const D3D11_QUERY_DESC *pQueryDesc,
    /* [annotation] */
    __out_opt  ID3D11Query **ppQuery) = 0;

HRESULT WINAPI extCreatePredicate(
    /* [annotation] */
    __in  const D3D11_QUERY_DESC *pPredicateDesc,
    /* [annotation] */
    __out_opt  ID3D11Predicate **ppPredicate) = 0;

HRESULT WINAPI extCreateCounter(
    /* [annotation] */
    __in  const D3D11_COUNTER_DESC *pCounterDesc,
    /* [annotation] */
    __out_opt  ID3D11Counter **ppCounter) = 0;

HRESULT WINAPI extCreateDeferredContext(
    UINT ContextFlags,
    /* [annotation] */
    __out_opt  ID3D11DeviceContext **ppDeferredContext) = 0;

HRESULT WINAPI extOpenSharedResource(
    /* [annotation] */
    __in  HANDLE hResource,
    /* [annotation] */
    __in  REFIID ReturnedInterface,
    /* [annotation] */
    __out_opt  void **ppResource) = 0;

HRESULT WINAPI extCheckFormatSupport(
    /* [annotation] */
    __in  DXGI_FORMAT Format,
    /* [annotation] */
    __out  UINT *pFormatSupport) = 0;

HRESULT WINAPI extCheckMultisampleQualityLevels(
    /* [annotation] */
    __in  DXGI_FORMAT Format,
    /* [annotation] */
    __in  UINT SampleCount,
    /* [annotation] */
    __out  UINT *pNumQualityLevels) = 0;

HRESULT WINAPI extCheckCounterInfo(
    /* [annotation] */
    __out  D3D11_COUNTER_INFO *pCounterInfo) = 0;

HRESULT WINAPI extCheckCounter(
    /* [annotation] */
    __in  const D3D11_COUNTER_DESC *pDesc,
    /* [annotation] */
    __out  D3D11_COUNTER_TYPE *pType,
    /* [annotation] */
    __out  UINT *pActiveCounters,
    /* [annotation] */
    __out_ecount_opt(*pNameLength)  LPSTR szName,
    /* [annotation] */
    __inout_opt  UINT *pNameLength,
    /* [annotation] */
    __out_ecount_opt(*pUnitsLength)  LPSTR szUnits,
    /* [annotation] */
    __inout_opt  UINT *pUnitsLength,
    /* [annotation] */
    __out_ecount_opt(*pDescriptionLength)  LPSTR szDescription,
    /* [annotation] */
    __inout_opt  UINT *pDescriptionLength) = 0;

HRESULT WINAPI extCheckFeatureSupport(
    D3D11_FEATURE Feature,
    /* [annotation] */
    __out_bcount(FeatureSupportDataSize)  void *pFeatureSupportData,
    UINT FeatureSupportDataSize) = 0;

HRESULT WINAPI extGetPrivateData(
    /* [annotation] */
    __in  REFGUID guid,
    /* [annotation] */
    __inout  UINT *pDataSize,
    /* [annotation] */
    __out_bcount_opt(*pDataSize)  void *pData) = 0;

HRESULT WINAPI extSetPrivateData(
    /* [annotation] */
    __in  REFGUID guid,
    /* [annotation] */
    __in  UINT DataSize,
    /* [annotation] */
    __in_bcount_opt(DataSize)  const void *pData) = 0;

HRESULT WINAPI extSetPrivateDataInterface(
    /* [annotation] */
    __in  REFGUID guid,
    /* [annotation] */
    __in_opt  const IUnknown *pData) = 0;

virtual D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetFeatureLevel( void) = 0;

virtual UINT STDMETHODCALLTYPE GetCreationFlags( void) = 0;

virtual HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason( void) = 0;

virtual void STDMETHODCALLTYPE GetImmediateContext(
    /* [annotation] */
    __out  ID3D11DeviceContext **ppImmediateContext) = 0;

virtual HRESULT STDMETHODCALLTYPE SetExceptionMode(
    UINT RaiseFlags) = 0;

virtual UINT STDMETHODCALLTYPE GetExceptionMode( void) = 0;
#endif

#endif // TRACEALL

// IID_ID3D11DeviceChild wrappers

#ifdef TRACEALL
void WINAPI extGetDevice(void *lpDev, ID3D11Device **ppDevice) {
#ifndef DXW_NOTRACES
    ApiName("GetDevice(D3D11)");
    OutTraceD3D("%s: device=%#x\n", ApiRef, lpDev);
#endif
    (*pGetDevice)(lpDev, ppDevice);
    OutTraceD3D("%s: pDevice=%#x\n", ApiRef, *ppDevice);
}
#endif // TRACEALL

