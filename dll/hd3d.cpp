//#define D3D10_IGNORE_SDK_LAYERS 1
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <d3d9.h>
#include <d3d8caps.h>
#include <D3D10_1.h>
#include <D3D10Misc.h>
#include <D3D11.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhook.h"
#include "dxhelper.h"
#include "syslibs.h"
#include "stdio.h"
#include "dxdds.h"
#include "hd3d.doc.hpp" // fake include to hold documentation

#define HOOKD3D10ANDLATER 1
//#define TRACEALL 1
int gFlippedToScreen = 0;
D3DDISPLAYMODE gCurrentDisplayMode;

#ifndef DXW_NOTRACES
#define D3DError(r) OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, r, ExplainDDError(r))
#else
#define D3DError(r)
#endif // DXW_NOTRACES

extern void D3D9TextureHandling(void *, int);
extern void D3D8TextureHandling(void *, int);
extern HRESULT dxCopyRects(void *, LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE);
extern HRESULT dwD3D8InitEmulation(void *);
extern LPDIRECTDRAWSURFACE dwGetVirtualBackBuffer();
extern void dwD3D8ShutdownEmulation(void *);
extern HRESULT dwD3D8Present(void *, CONST RECT *, CONST RECT *, HWND, CONST RGNDATA *);
extern char *ExplainDDError(DWORD);
extern char *ExplainRenderstateValue(DWORD Value);
extern HRESULT dxGetFrontBuffer8(void *, LPDIRECTDRAWSURFACE);
extern HRESULT dxGetFrontBuffer9(void *, UINT, LPDIRECTDRAWSURFACE);
extern int dxGetFormatColorDepth(D3DFORMAT);
extern void dxSetDDSPixelFormat(D3DFORMAT, DDS_PIXELFORMAT *);
extern void SurfaceDump(LPDIRECTDRAWSURFACE, int);

void *lpD3DActiveDevice = NULL;
void *lpD3DActiveSession = NULL;
typedef HRESULT (WINAPI *QueryInterface_Type)(void *, REFIID riid, void **ppvObj);

// D3D8/9 API

typedef void *(WINAPI *Direct3DCreate8_Type)(UINT);
typedef void *(WINAPI *Direct3DCreate9_Type)(UINT);
typedef HRESULT (WINAPI *Direct3DCreate9Ex_Type)(UINT, IDirect3D9Ex **);
typedef HRESULT (WINAPI *CheckFullScreen_Type)(void);
typedef BOOL (WINAPI *DisableD3DSpy_Type)(void);
typedef void (WINAPI *D3DPERF_SetOptions_Type)(DWORD);

void *WINAPI extDirect3DCreate8(UINT);
void *WINAPI extDirect3DCreate9(UINT);
HRESULT WINAPI extDirect3DCreate9Ex(UINT, IDirect3D9Ex **);
HRESULT WINAPI extCheckFullScreen(void);
HRESULT WINAPI voidDirect3DShaderValidatorCreate9(void);
void WINAPI voidDebugSetLevel(void);
void WINAPI voidDebugSetMute(void);
BOOL WINAPI voidDisableD3DSpy(void);
BOOL WINAPI extDisableD3DSpy(void);
void WINAPI extD3DPERF_SetOptions(DWORD);

Direct3DCreate8_Type pDirect3DCreate8 = 0;
Direct3DCreate9_Type pDirect3DCreate9 = 0;
Direct3DCreate9Ex_Type pDirect3DCreate9Ex = 0;
CheckFullScreen_Type pCheckFullScreen = 0;
DisableD3DSpy_Type pDisableD3DSpy = 0;
D3DPERF_SetOptions_Type pD3DPERF_SetOptions = 0;

// IDirect3D8/9 methods

typedef UINT	(WINAPI *GetAdapterCount_Type)(void *);
typedef HRESULT (WINAPI *GetAdapterIdentifier_Type)(void *, UINT, DWORD, D3DADAPTER_IDENTIFIER9 *);
typedef UINT	(WINAPI *GetAdapterModeCount8_Type)(void *, UINT);
typedef UINT	(WINAPI *GetAdapterModeCount9_Type)(void *, UINT, D3DFORMAT);
typedef HRESULT (WINAPI *EnumAdapterModes8_Type)(void *, UINT, UINT, D3DDISPLAYMODE *);
typedef HRESULT (WINAPI *EnumAdapterModes9_Type)(void *, UINT, D3DFORMAT, UINT, D3DDISPLAYMODE *);
typedef HRESULT (WINAPI *GetAdapterDisplayMode_Type)(void *, UINT, D3DDISPLAYMODE *);
typedef HRESULT (WINAPI *CheckDeviceType_Type)(void *, UINT, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, BOOL);
typedef HRESULT (WINAPI *D3DGetDeviceCaps8_Type)(void *, UINT, D3DDEVTYPE, D3DCAPS8 *);
typedef HRESULT (WINAPI *D3DGetDeviceCaps9_Type)(void *, UINT, D3DDEVTYPE, D3DCAPS9 *);
typedef HMONITOR (WINAPI *GetAdapterMonitor_Type)(void *, UINT);
typedef HRESULT (WINAPI *CreateDevice_Type)(void *, UINT, D3DDEVTYPE, HWND, DWORD, void *, void **);
typedef HRESULT (WINAPI *CreateDeviceEx_Type)(void *, UINT, D3DDEVTYPE, HWND, DWORD, void *, D3DDISPLAYMODEEX *, void **);
typedef BOOL	(WINAPI *DisableD3DSpy_Type)(void);
typedef HRESULT (WINAPI *GetBackBuffer8_Type)(void *, UINT, D3DBACKBUFFER_TYPE, LPDIRECTDRAWSURFACE *);
typedef HRESULT (WINAPI *GetFrontBufferData_Type)(void *, UINT, LPDIRECTDRAWSURFACE);
typedef ULONG	(WINAPI *ReleaseD3D_Type)(void *);

UINT WINAPI extGetAdapterCount8(void *);
UINT WINAPI extGetAdapterCount9(void *);
HRESULT WINAPI extGetAdapterIdentifier8(void *, UINT, DWORD, D3DADAPTER_IDENTIFIER9 *);
HRESULT WINAPI extGetAdapterIdentifier9(void *, UINT, DWORD, D3DADAPTER_IDENTIFIER9 *);
UINT WINAPI extGetAdapterModeCount8(void *, UINT);
UINT WINAPI extGetAdapterModeCount9(void *, UINT, D3DFORMAT);
HRESULT WINAPI extCheckDeviceType8(void *, UINT, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, BOOL);
HRESULT WINAPI extCheckDeviceType9(void *, UINT, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, BOOL);
HRESULT WINAPI extD3DGetDeviceCaps8(void *, UINT, D3DDEVTYPE, D3DCAPS8 *);
HRESULT WINAPI extD3DGetDeviceCaps9(void *, UINT, D3DDEVTYPE, D3DCAPS9 *);
HMONITOR WINAPI extGetAdapterMonitor8(void *, UINT);
HMONITOR WINAPI extGetAdapterMonitor9(void *, UINT);
HRESULT WINAPI extCreateDevice8(void *, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS *, void **);
HRESULT WINAPI extCreateDevice9(void *, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS *, void **);
HRESULT WINAPI extCreateDeviceEx(void *, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS *, D3DDISPLAYMODEEX *, void **);
HRESULT WINAPI extGetBackBuffer8(void *, UINT, D3DBACKBUFFER_TYPE, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extGetFrontBufferData9(void *, UINT, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extReleaseD3D8(void *);
HRESULT WINAPI extReleaseD3D9(void *);

GetAdapterCount_Type pGetAdapterCount8, pGetAdapterCount9;
GetAdapterModeCount8_Type pGetAdapterModeCount8;
GetAdapterModeCount9_Type pGetAdapterModeCount9;
GetAdapterIdentifier_Type pGetAdapterIdentifier8, pGetAdapterIdentifier9;
CheckDeviceType_Type pCheckDeviceType8, pCheckDeviceType9;
D3DGetDeviceCaps8_Type pD3DGetDeviceCaps8 = 0;
D3DGetDeviceCaps9_Type pD3DGetDeviceCaps9 = 0;
GetAdapterMonitor_Type pGetAdapterMonitor8, pGetAdapterMonitor9;
CreateDevice_Type pCreateDevice8, pCreateDevice9;
CreateDeviceEx_Type pCreateDeviceEx = 0;
GetBackBuffer8_Type pGetBackBuffer8 = 0;
GetFrontBufferData_Type pGetFrontBufferData9;
ReleaseD3D_Type pReleaseD3D8, pReleaseD3D9;

// IDirect3DDevice8/9 methods

typedef UINT	(WINAPI *GetAvailableTextureMem_Type)(void *);
typedef HRESULT (WINAPI *TestCooperativeLevel_Type)(void *);
typedef HRESULT (WINAPI *GetDirect3D8_Type)(void *, void **);
typedef HRESULT (WINAPI *GetDirect3D9_Type)(void *, void **);
typedef HRESULT (WINAPI *GetDisplayMode8_Type)(void *, D3DDISPLAYMODE *);
typedef HRESULT (WINAPI *GetDisplayMode9_Type)(void *, UINT, D3DDISPLAYMODE *);
typedef void	(WINAPI *SetCursorPosition9_Type)(void *, int, int, DWORD);
typedef void	(WINAPI *SetCursorPosition8_Type)(void *, int, int, DWORD);
typedef BOOL	(WINAPI *ShowCursor8_Type)(void *, BOOL);
typedef BOOL	(WINAPI *ShowCursor9_Type)(void *, BOOL);
typedef HRESULT (WINAPI *CreateAdditionalSwapChain_Type)(void *, D3DPRESENT_PARAMETERS *, IDirect3DSwapChain9 **);
typedef HRESULT (WINAPI *GetSwapChain_Type)(void *, UINT, IDirect3DSwapChain9 **);
typedef UINT	(WINAPI *GetNumberOfSwapChains_Type)(void *);
typedef HRESULT (WINAPI *BeginStateBlock_Type)(void *);
typedef HRESULT (WINAPI *EndStateBlock8_Type)(void *, DWORD *);
typedef HRESULT (WINAPI *EndStateBlock9_Type)(void *, IDirect3DStateBlock9 **);
typedef HRESULT (WINAPI *CreateTexture8_Type)(void *, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, void **);
typedef HRESULT (WINAPI *CreateTexture9_Type)(void *, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, void **, HANDLE *);
typedef HRESULT	(WINAPI *CopyRects_Type)(void *, LPDIRECTDRAWSURFACE, CONST RECT *, UINT, LPDIRECTDRAWSURFACE, CONST POINT *);
typedef HRESULT (WINAPI *GetFrontBuffer_Type)(void *, LPDIRECTDRAWSURFACE);
typedef ULONG	(WINAPI *ReleaseDev_Type)(void *);
typedef HRESULT (WINAPI *GetDepthStencilSurface_Type)(void *, void **);
typedef HRESULT (WINAPI *CreateDepthStencilSurface8_Type)(void *, UINT, UINT, D3DFORMAT, D3DMULTISAMPLE_TYPE, void **);
typedef HRESULT (WINAPI *Clear8_Type)(void *, DWORD, CONST D3DRECT *, DWORD, D3DCOLOR, float, DWORD);

UINT	WINAPI extGetAvailableTextureMem8(void *);
UINT	WINAPI extGetAvailableTextureMem9(void *);
HRESULT WINAPI extTestCooperativeLevel8(void *);
HRESULT WINAPI extTestCooperativeLevel9(void *);
HRESULT WINAPI extGetDirect3D8(void *, void **);
HRESULT WINAPI extGetDirect3D9(void *, void **);
HRESULT WINAPI extGetDisplayMode8(void *, D3DDISPLAYMODE *);
HRESULT WINAPI extGetDisplayMode9(void *, UINT, D3DDISPLAYMODE *);
void	WINAPI extSetCursorPosition9(void *, int, int, DWORD);
void	WINAPI extSetCursorPosition8(void *, int, int, DWORD);
BOOL	WINAPI extShowCursor8(void *, BOOL);
BOOL	WINAPI extShowCursor9(void *, BOOL);
HRESULT WINAPI extCreateAdditionalSwapChain8(void *, D3DPRESENT_PARAMETERS *, IDirect3DSwapChain9 **);
HRESULT WINAPI extCreateAdditionalSwapChain9(void *, D3DPRESENT_PARAMETERS *, IDirect3DSwapChain9 **);
HRESULT WINAPI extGetSwapChain(void *, UINT, IDirect3DSwapChain9 **);
UINT WINAPI extGetNumberOfSwapChains(void *);
HRESULT WINAPI extBeginStateBlock8(void *);
HRESULT WINAPI extBeginStateBlock9(void *);
HRESULT WINAPI extEndStateBlock8(void *, DWORD *);
HRESULT WINAPI extEndStateBlock9(void *, IDirect3DStateBlock9 **);
HRESULT WINAPI extCreateTexture8(void *, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, void **);
HRESULT WINAPI extCreateTexture9(void *, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, void **, HANDLE *);
// CopyRects prototype uses IDirect3DSurface8 *, but to avoid including d3d8.h better use a generic ptr as LPDIRECTDRAWSURFACE
HRESULT	WINAPI extCopyRects(void *, LPDIRECTDRAWSURFACE, CONST RECT *, UINT, LPDIRECTDRAWSURFACE, CONST POINT *);
HRESULT WINAPI extGetFrontBuffer(void *, LPDIRECTDRAWSURFACE);
ULONG WINAPI extReleaseDev8(void *);
ULONG WINAPI extReleaseDev9(void *);
#ifdef TRACEALL
HRESULT WINAPI extGetDepthStencilSurface8(void *, void **);
HRESULT WINAPI extCreateDepthStencilSurface8(void *, UINT, UINT, D3DFORMAT, D3DMULTISAMPLE_TYPE, void **);
HRESULT WINAPI extClear8(void *, DWORD, CONST D3DRECT *, DWORD, D3DCOLOR, float, DWORD);
#endif

GetAvailableTextureMem_Type pGetAvailableTextureMem8, pGetAvailableTextureMem9;
TestCooperativeLevel_Type pTestCooperativeLevel8, pTestCooperativeLevel9;
GetDirect3D8_Type pGetDirect3D8 = 0;
GetDirect3D9_Type pGetDirect3D9 = 0;
GetDisplayMode8_Type pGetDisplayMode8 = 0;
GetDisplayMode9_Type pGetDisplayMode9 = 0;
SetCursorPosition9_Type pSetCursorPosition9 = 0;
SetCursorPosition8_Type pSetCursorPosition8 = 0;
ShowCursor8_Type pShowCursor8 = 0;
ShowCursor9_Type pShowCursor9 = 0;
CreateAdditionalSwapChain_Type pCreateAdditionalSwapChain8 = 0;
CreateAdditionalSwapChain_Type pCreateAdditionalSwapChain9 = 0;
GetSwapChain_Type pGetSwapChain = 0;
GetNumberOfSwapChains_Type pGetNumberOfSwapChains = 0;
BeginStateBlock_Type pBeginStateBlock8 = 0;
BeginStateBlock_Type pBeginStateBlock9 = 0;
EndStateBlock8_Type pEndStateBlock8 = 0;
EndStateBlock9_Type pEndStateBlock9 = 0;
CreateTexture8_Type pCreateTexture8 = 0;
CreateTexture9_Type pCreateTexture9 = 0;
CopyRects_Type pCopyRects = 0;
GetFrontBuffer_Type pGetFrontBuffer = 0;
ReleaseDev_Type pReleaseDev8, pReleaseDev9;
#ifdef TRACEALL
GetDepthStencilSurface_Type pGetDepthStencilSurface8;
CreateDepthStencilSurface8_Type pCreateDepthStencilSurface8;
Clear8_Type pClear8;
#endif

// IDirect3DTexture8/9 methods

typedef HRESULT (WINAPI *LockRect_Type)(void *, UINT, D3DLOCKED_RECT *, CONST RECT *, DWORD);
typedef HRESULT (WINAPI *UnlockRect_Type)(void *, UINT);
typedef HRESULT (WINAPI *GetLevelDesc_Type)(void *, UINT, D3DSURFACE_DESC *);
typedef HRESULT (WINAPI *GetSurfaceLevel_Type)(void *, UINT, void **);

HRESULT WINAPI extLockRect8(void *, UINT, D3DLOCKED_RECT *, CONST RECT *, DWORD);
HRESULT WINAPI extLockRect9(void *, UINT, D3DLOCKED_RECT *, CONST RECT *, DWORD);
HRESULT WINAPI extUnlockRect8(void *, UINT);
HRESULT WINAPI extUnlockRect9(void *, UINT);
#ifdef TRACEALL
HRESULT WINAPI extGetLevelDesc8(void *, UINT, D3DSURFACE_DESC *);
HRESULT WINAPI extGetSurfaceLevel8(void *, UINT, void **);
HRESULT WINAPI extGetDepthStencilSurface9(void *, IDirect3DSurface9 **);
HRESULT WINAPI extSetDepthStencilSurface9(void *, IDirect3DSurface9 *);
#endif

LockRect_Type pLockRect8, pLockRect9;
UnlockRect_Type pUnlockRect8, pUnlockRect9;
#ifdef TRACEALL
GetLevelDesc_Type pGetLevelDesc8;
GetSurfaceLevel_Type pGetSurfaceLevel8;
#endif

// to sort ...
typedef HRESULT (WINAPI *GetDepthStencilSurface9_Type)(void *, IDirect3DSurface9 **);
typedef HRESULT (WINAPI *SetDepthStencilSurface9_Type)(void *, IDirect3DSurface9 *);

typedef HRESULT (WINAPI *Present_Type)(void *, CONST RECT *, CONST RECT *, HWND, CONST RGNDATA *);
typedef HRESULT (WINAPI *SetRenderState_Type)(void *, D3DRENDERSTATETYPE, DWORD);
typedef HRESULT (WINAPI *GetRenderState_Type)(void *, D3DRENDERSTATETYPE, DWORD );
typedef HRESULT (WINAPI *GetViewport_Type)(void *, D3DVIEWPORT9 *);
typedef HRESULT (WINAPI *SetViewport_Type)(void *, D3DVIEWPORT9 *);
typedef void	(WINAPI *SetGammaRamp8_Type)(void *, DWORD, D3DGAMMARAMP *);
typedef void	(WINAPI *GetGammaRamp8_Type)(void *, D3DGAMMARAMP *);
typedef void	(WINAPI *SetGammaRamp9_Type)(void *, UINT, DWORD, D3DGAMMARAMP *);
typedef void	(WINAPI *GetGammaRamp9_Type)(void *, UINT, D3DGAMMARAMP *);
typedef HRESULT (WINAPI *SetTexture8_Type)(void *, DWORD, void *);
typedef HRESULT (WINAPI *SetTexture9_Type)(void *, DWORD, void *);
// n.b. prototypes and D3DLIGHT8/D3DLIGHT9 are identical, so 1 fits both D3D8 & D3D9
typedef HRESULT (WINAPI *SetLight_Type)(void *, DWORD Index, CONST D3DLIGHT9 *);
typedef HRESULT (WINAPI *GetLight_Type)(void *, DWORD Index, D3DLIGHT9 *);

typedef ULONG	(WINAPI *CreateImageSurface8_Type)(void *, UINT, UINT, D3DFORMAT, void **);
//typedef ULONG	(WINAPI *CreateRenderTarget8_Type)(void *, UINT, UINT, D3DFORMAT, D3DMULTISAMPLE_TYPE, BOOL, IDirect3DSurface8**);
typedef ULONG	(WINAPI *CreateRenderTarget8_Type)(void *, UINT, UINT, D3DFORMAT, D3DMULTISAMPLE_TYPE, BOOL, void **);
typedef ULONG	(WINAPI *CreateRenderTarget9_Type)(void *, UINT, UINT, D3DFORMAT, D3DMULTISAMPLE_TYPE, BOOL, void **);
typedef ULONG	(WINAPI *GetRenderTarget8_Type)(void *, void **);
typedef ULONG	(WINAPI *GetRenderTarget9_Type)(void *, DWORD, void **);
typedef ULONG	(WINAPI *BeginScene_Type)(void *);
typedef ULONG	(WINAPI *EndScene_Type)(void *);

typedef HRESULT (WINAPI *D3D10CreateDevice_Type)(IDXGIAdapter *, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, ID3D10Device **);
typedef HRESULT (WINAPI *D3D10CreateDeviceAndSwapChain_Type)(IDXGIAdapter *, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **, ID3D10Device **);
typedef HRESULT (WINAPI *D3D10CreateDevice1_Type)(IDXGIAdapter *, D3D10_DRIVER_TYPE, HMODULE, UINT, D3D10_FEATURE_LEVEL1, UINT, ID3D10Device **);
typedef HRESULT (WINAPI *D3D10CreateDeviceAndSwapChain1_Type)(IDXGIAdapter *, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **, ID3D10Device **);
typedef void	(WINAPI *RSSetViewports10_Type)(void *, UINT, D3D10_VIEWPORT *);
typedef ULONG	(WINAPI *AddRef_Type)(void *);
typedef ULONG	(WINAPI *Release_Type)(void *);
typedef HRESULT (WINAPI *Reset_Type)(void *, D3DPRESENT_PARAMETERS *);

HRESULT WINAPI extQueryInterfaceD3D8(void *, REFIID, void **);
HRESULT WINAPI extQueryInterfaceDev8(void *, REFIID, void **);
HRESULT WINAPI extQueryInterfaceD3D9(void *, REFIID, void **);
HRESULT WINAPI extQueryInterfaceDev9(void *, REFIID, void **);

HRESULT WINAPI extEnumAdapterModes8(void *, UINT, UINT, D3DDISPLAYMODE *);
HRESULT WINAPI extEnumAdapterModes9(void *, UINT, D3DFORMAT, UINT, D3DDISPLAYMODE *);
HRESULT WINAPI extGetAdapterDisplayMode8(void *, UINT, D3DDISPLAYMODE *);
HRESULT WINAPI extGetAdapterDisplayMode9(void *, UINT, D3DDISPLAYMODE *);
HRESULT WINAPI extPresent8(void *, CONST RECT *, CONST RECT *, HWND, CONST RGNDATA *);
HRESULT WINAPI extPresent9(void *, CONST RECT *, CONST RECT *, HWND, CONST RGNDATA *);
HRESULT WINAPI extSetRenderState8(void *, D3DRENDERSTATETYPE, DWORD);
HRESULT WINAPI extSetRenderState9(void *, D3DRENDERSTATETYPE, DWORD);
HRESULT WINAPI extGetRenderState8(void *, D3DRENDERSTATETYPE, DWORD);
HRESULT WINAPI extGetRenderState9(void *, D3DRENDERSTATETYPE, DWORD);
HRESULT WINAPI extGetViewport(void *, D3DVIEWPORT9 *);
HRESULT WINAPI extSetViewport(void *, D3DVIEWPORT9 *);
HRESULT WINAPI extReset8(void *, D3DPRESENT_PARAMETERS *);
HRESULT WINAPI extReset9(void *, D3DPRESENT_PARAMETERS *);
void	WINAPI extSetGammaRamp8(void *, DWORD, D3DGAMMARAMP *);
void	WINAPI extSetGammaRamp9(void *, UINT, DWORD, D3DGAMMARAMP *);
void	WINAPI extGetGammaRamp8(void *, D3DGAMMARAMP *);
void	WINAPI extGetGammaRamp9(void *, UINT, D3DGAMMARAMP *);
BOOL    WINAPI voidDisableD3DSpy(void);
HRESULT WINAPI extSetLight(void *, DWORD Index, CONST D3DLIGHT9 *);
HRESULT WINAPI extGetLight(void *, DWORD Index, D3DLIGHT9 *);

ULONG WINAPI extCreateImageSurface8(void *, UINT, UINT, D3DFORMAT, void **);
ULONG WINAPI extCreateRenderTarget8(void *, UINT, UINT, D3DFORMAT, D3DMULTISAMPLE_TYPE, BOOL, void **);
ULONG WINAPI extCreateRenderTarget9(void *, UINT, UINT, D3DFORMAT, D3DMULTISAMPLE_TYPE, BOOL, void **);
ULONG WINAPI extGetRenderTarget8(void *, void **);
ULONG WINAPI extGetRenderTarget9(void *, DWORD, void **);
ULONG WINAPI extBeginScene8(void *);
ULONG WINAPI extEndScene8(void *);
ULONG WINAPI extBeginScene9(void *);
ULONG WINAPI extEndScene9(void *);
ULONG WINAPI extSetTexture8(void *, DWORD, void *);
ULONG WINAPI extSetTexture9(void *, DWORD, void *);

HRESULT WINAPI extD3D10CreateDevice(IDXGIAdapter *, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, ID3D10Device **);
HRESULT WINAPI extD3D10CreateDeviceAndSwapChain(IDXGIAdapter *, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **, ID3D10Device **);
HRESULT WINAPI extD3D10CreateDevice1(IDXGIAdapter *, D3D10_DRIVER_TYPE, HMODULE, UINT, D3D10_FEATURE_LEVEL1, UINT, ID3D10Device **);
HRESULT WINAPI extD3D10CreateDeviceAndSwapChain1(IDXGIAdapter *, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **, ID3D10Device **);
void WINAPI extRSSetViewports10(void *, UINT, D3D10_VIEWPORT *);

QueryInterface_Type pQueryInterfaceD3D8 = 0;
QueryInterface_Type pQueryInterfaceDev8 = 0;
QueryInterface_Type pQueryInterfaceD3D9 = 0;
QueryInterface_Type pQueryInterfaceDev9 = 0;

EnumAdapterModes8_Type pEnumAdapterModes8 = 0;
EnumAdapterModes9_Type pEnumAdapterModes9 = 0;
GetAdapterDisplayMode_Type pGetAdapterDisplayMode8 = 0;
GetAdapterDisplayMode_Type pGetAdapterDisplayMode9 = 0;
Present_Type pPresent8 = 0;
Present_Type pPresent9 = 0;
SetRenderState_Type pSetRenderState8, pSetRenderState9;
GetRenderState_Type pGetRenderState8, pGetRenderState9;
GetViewport_Type pGetViewport = 0;
SetViewport_Type pSetViewport = 0;
SetGammaRamp8_Type pSetGammaRamp8;
SetGammaRamp9_Type pSetGammaRamp9;
GetGammaRamp8_Type pGetGammaRamp8;
GetGammaRamp9_Type pGetGammaRamp9;
SetLight_Type pSetLight;
GetLight_Type pGetLight;

CreateImageSurface8_Type pCreateImageSurface8 = 0;
CreateRenderTarget8_Type pCreateRenderTarget8 = 0;
CreateRenderTarget9_Type pCreateRenderTarget9 = 0;
GetRenderTarget8_Type pGetRenderTarget8 = 0;
GetRenderTarget9_Type pGetRenderTarget9 = 0;
BeginScene_Type pBeginScene8, pBeginScene9;
EndScene_Type pEndScene8, pEndScene9;
Reset_Type pReset8, pReset9;
SetTexture8_Type pSetTexture8 = 0;
SetTexture9_Type pSetTexture9 = 0;

D3D10CreateDevice_Type pD3D10CreateDevice = 0;
D3D10CreateDeviceAndSwapChain_Type pD3D10CreateDeviceAndSwapChain = 0;
D3D10CreateDevice1_Type pD3D10CreateDevice1 = 0;
D3D10CreateDeviceAndSwapChain1_Type pD3D10CreateDeviceAndSwapChain1 = 0;
RSSetViewports10_Type pRSSetViewports10 = 0;

#ifdef TRACEALL
GetDepthStencilSurface9_Type pGetDepthStencilSurface9;
SetDepthStencilSurface9_Type pSetDepthStencilSurface9;
#endif

AddRef_Type pAddRef9 = 0;
Release_Type pRelease9 = 0;

// swap chain
typedef HRESULT (WINAPI *PresentSwC8_Type)(void *, CONST RECT *, CONST RECT *, HWND, CONST RGNDATA *);
typedef HRESULT (WINAPI *PresentSwC9_Type)(void *, CONST RECT *, CONST RECT *, HWND, CONST RGNDATA *, DWORD);
PresentSwC8_Type pPresentSwC8;
PresentSwC9_Type pPresentSwC9;
HRESULT WINAPI extPresentSwC8(void *, CONST RECT *, CONST RECT *, HWND, CONST RGNDATA *);
HRESULT WINAPI extPresentSwC9(void *, CONST RECT *, CONST RECT *, HWND, CONST RGNDATA *, DWORD);

DWORD dwD3DVersion;
DWORD dwD3DSwapEffect;

static HookEntryEx_Type d3d8Hooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "Direct3DCreate8", (FARPROC)NULL, (FARPROC *) &pDirect3DCreate8, (FARPROC)extDirect3DCreate8},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type d3d9Hooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "Direct3DCreate9", (FARPROC)NULL, (FARPROC *) &pDirect3DCreate9, (FARPROC)extDirect3DCreate9},
    {HOOK_HOT_CANDIDATE, 0, "Direct3DCreate9Ex", (FARPROC)NULL, (FARPROC *) &pDirect3DCreate9Ex, (FARPROC)extDirect3DCreate9Ex},
    {HOOK_HOT_CANDIDATE, 0, "CheckFullScreen", (FARPROC)NULL, (FARPROC *) &pCheckFullScreen, (FARPROC)extCheckFullScreen},
    //{HOOK_HOT_CANDIDATE, 0, "DisableD3DSpy", (FARPROC)NULL, (FARPROC *)&pDisableD3DSpy, (FARPROC)extDisableD3DSpy},
    {HOOK_HOT_CANDIDATE, 0, "D3DPERF_SetOptions", (FARPROC)NULL, (FARPROC *) &pD3DPERF_SetOptions, (FARPROC)extD3DPERF_SetOptions},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type d3d9Extra[] = {
    {HOOK_HOT_CANDIDATE, 0, "Direct3DShaderValidatorCreate9", (FARPROC)NULL, (FARPROC *)NULL, (FARPROC)voidDirect3DShaderValidatorCreate9},
    {HOOK_HOT_CANDIDATE, 0, "DebugSetLevel", (FARPROC)NULL, (FARPROC *)NULL, (FARPROC)voidDebugSetLevel},
    {HOOK_HOT_CANDIDATE, 0, "DebugSetMute", (FARPROC)NULL, (FARPROC *)NULL, (FARPROC)voidDebugSetMute},
    {HOOK_HOT_CANDIDATE, 0, "DisableD3DSpy", (FARPROC)NULL, (FARPROC *)NULL, (FARPROC)voidDisableD3DSpy},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type d3d10Hooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "D3D10CreateDevice", (FARPROC)NULL, (FARPROC *) &pD3D10CreateDevice, (FARPROC)extD3D10CreateDevice},
    {HOOK_HOT_CANDIDATE, 0, "D3D10CreateDeviceAndSwapChain", (FARPROC)NULL, (FARPROC *) &pD3D10CreateDeviceAndSwapChain, (FARPROC)extD3D10CreateDeviceAndSwapChain},
    {HOOK_HOT_CANDIDATE, 0, "CheckFullScreen", (FARPROC)NULL, (FARPROC *) &pCheckFullScreen, (FARPROC)extCheckFullScreen},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type d3d10_1Hooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "D3D10CreateDevice1", (FARPROC)NULL, (FARPROC *) &pD3D10CreateDevice1, (FARPROC)extD3D10CreateDevice1},
    {HOOK_HOT_CANDIDATE, 0, "D3D10CreateDeviceAndSwapChain1", (FARPROC)NULL, (FARPROC *) &pD3D10CreateDeviceAndSwapChain1, (FARPROC)extD3D10CreateDeviceAndSwapChain1},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

FARPROC Remap_d3d8_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    //if (dxw.dwTargetDDVersion == HOOKDDRAWNONE) return NULL;
    if (addr = RemapLibraryEx(proc, hModule, d3d8Hooks)) return addr;
    return NULL;
}

FARPROC Remap_d3d9_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    //if (dxw.dwTargetDDVersion == HOOKDDRAWNONE) return NULL;
    if (addr = RemapLibraryEx(proc, hModule, d3d9Hooks)) return addr;
    if (dxw.dwFlags3 & SUPPRESSD3DEXT) if (addr = RemapLibraryEx(proc, hModule, d3d9Extra)) return addr;
    return NULL;
}

FARPROC Remap_d3d10_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    //if (dxw.dwTargetDDVersion == HOOKDDRAWNONE) return NULL;
    if (addr = RemapLibraryEx(proc, hModule, d3d10Hooks)) return addr;
    return NULL;
}

FARPROC Remap_d3d10_1_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    //if (dxw.dwTargetDDVersion == HOOKDDRAWNONE) return NULL;
    if (addr = RemapLibraryEx(proc, hModule, d3d10_1Hooks)) return addr;
    return NULL;
}

static void ddSetD3D8Compatibility() {
    typedef HRESULT (WINAPI * D3D8Shim_Type)(void);
    D3D8Shim_Type pD3D8Shim;
    HINSTANCE hinst;
    hinst = (*pLoadLibraryA)("d3d8.dll");
    if(!hinst)
        OutTraceE("LoadLibrary d3d8.dll ERROR err=%d at %d\n", GetLastError(), __LINE__);
    pD3D8Shim = (D3D8Shim_Type)(*pGetProcAddress)(hinst, "Direct3D8EnableMaximizedWindowedModeShim");
    if(pD3D8Shim) {
        LPBYTE p = (LPBYTE)pD3D8Shim;
        if((*p == 0xC7) && (*(p + 1) == 0x05))  { // if assemby is MOV DWORD PTR DS:[addr] val
            DWORD oldprot;
            LPDWORD lpdwFlagAddr;
            lpdwFlagAddr = (LPDWORD) * (DWORD *)(p + 2); // set flag address
            OutTraceDW("DXWND: D3D8Shim found at addr=%#x flag=%#x->%#x\n", p, lpdwFlagAddr, *lpdwFlagAddr);
            if(!VirtualProtect((LPVOID)p, 10, PAGE_READWRITE, &oldprot)) {
                OutTraceE("VirtualProtect ERROR: target=%#x err=%d at %d\n", p, GetLastError(), __LINE__);
                return; // error condition
            }
            *(p + 6) = 0x00;	// turn 1 to 0
            if(!VirtualProtect((LPVOID)p, 10, oldprot, &oldprot)) {
                OutTraceE("VirtualProtect ERROR; target=%#x, err=%d at %d\n", p, GetLastError(), __LINE__);
                return; // error condition
            }
            if(!VirtualProtect((LPVOID)lpdwFlagAddr, sizeof(DWORD), PAGE_READWRITE, &oldprot)) {
                OutTraceE("VirtualProtect ERROR: target=%#x err=%d at %d\n", p, GetLastError(), __LINE__);
                return; // error condition
            }
            *lpdwFlagAddr = 0x00000000;	// turn 1 to 0
            if(!VirtualProtect((LPVOID)lpdwFlagAddr, sizeof(DWORD), oldprot, &oldprot)) {
                OutTraceE("VirtualProtect ERROR; target=%#x, err=%d at %d\n", p, GetLastError(), __LINE__);
                return; // error condition
            }
        } else
            OutTraceE("HookDirectDraw: unmatching Direct3D8EnableMaximizedWindowedModeShim assembly\n");
    } else
        OutTraceE("HookDirectDraw: missing Direct3D8EnableMaximizedWindowedModeShim call\n");
    //(*pFreeLibrary)(hinst);
}

// beware: from https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dpresent
// >> Remarks
// >> Windowed mode supports D3DPRESENT_INTERVAL_DEFAULT, D3DPRESENT_INTERVAL_IMMEDIATE,
// >> and D3DPRESENT_INTERVAL_ONE. D3DPRESENT_INTERVAL_DEFAULT and the D3DPRESENT_INTERVAL_ONE
// >> are nearly equivalent (see the information regarding timer resolution below).
// >> They perform similarly to COPY_VSYNC in that there is only one present per frame,
// >> and they prevent tearing with beam-following. In contrast, D3DPRESENT_INTERVAL_IMMEDIATE
// >> will attempt to provide an unlimited presentation rate.
// In addition, it seems that IDIrect3DDevice8 in win mode only supports D3DPRESENT_INTERVAL_DEFAULT

static DWORD SetD3DPresentInterval(DWORD dwPresentationInterval) {
    DWORD dwRet = D3DPRESENT_INTERVAL_DEFAULT;
    if (dxw.dwFlags12 & KILLVSYNC) dwRet = D3DPRESENT_INTERVAL_IMMEDIATE;
    _if(dwPresentationInterval != dwRet) OutTraceD3D(">   FORCE PresentationInterval %#x->%#x\n",
            dwPresentationInterval, dwRet);
    return dwRet;
}

static UINT SetD3DAdapter(UINT Adapter) {
    // in window mode, always use the default adapter
    if(dxw.Windowize) return 0;
    // in fullscreen mode, if a specific adapter is set, chose that one
    if(dxw.MonitorId != -1) return dxw.MonitorId;
    // otherwise (fullscreen mode, no forced selection) keep the current one
    return Adapter;
}

static void HookDirect3D(HMODULE module, int hookversion) {
    LPDIRECT3D9 lpd3d;
#ifdef HOOKD3D10ANDLATER
    ID3D10Device *lpd3d10;
#endif
    int version = dxw.dwTargetDDVersion;
    OutTraceDW("HookDirect3D: module=%#x version=%d\n", module, version);
    if(dxw.dwFlags11 & D3D8MAXWINMODE) {
        static BOOL AlreadyDone = FALSE;
        if(!AlreadyDone) {
            ddSetD3D8Compatibility();
            AlreadyDone = TRUE;
        }
    }
    if(dxw.dwFlags4 & HOTPATCH) {
        // hot-patch all APIs and that's all folks!
        HookLibraryEx(module, d3d8Hooks, "d3d8.dll");
        HookLibraryEx(module, d3d9Hooks, "d3d9.dll");
#ifdef HOOKD3D10ANDLATER
        HookLibraryEx(module, d3d10Hooks, "d3d10.dll");
        HookLibraryEx(module, d3d10_1Hooks, "d3d10_1.dll");
#endif
        return;
    }
    switch(version) {
    case 0:
        HookLibraryEx(module, d3d8Hooks, "d3d8.dll");
        HookLibraryEx(module, d3d9Hooks, "d3d9.dll");
#ifdef HOOKD3D10ANDLATER
        HookLibraryEx(module, d3d10Hooks, "d3d10.dll");
        HookLibraryEx(module, d3d10_1Hooks, "d3d10_1.dll");
#endif
        break;
    case 8:
        PinLibraryEx(d3d8Hooks, "d3d8.dll"); // pin for "Port Royale 2"
        HookLibraryEx(module, d3d8Hooks, "d3d8.dll");
        if(pDirect3DCreate8) {
            lpd3d = (LPDIRECT3D9)extDirect3DCreate8(220);
            if(lpd3d) lpd3d->Release();
        }
        break;
    case 9:
        PinLibraryEx(d3d9Hooks, "d3d9.dll");// pin for "Affari tuoi"
        HookLibraryEx(module, d3d9Hooks, "d3d9.dll");
        if(pDirect3DCreate9) {
            lpd3d = (LPDIRECT3D9)extDirect3DCreate9(31);
            if(lpd3d) lpd3d->Release();
        }
        break;
#ifdef HOOKD3D10ANDLATER
    case 10:
        PinLibraryEx(d3d10Hooks, "d3d10.dll");
        PinLibraryEx(d3d10_1Hooks, "d3d10_1.dll");
        HookLibraryEx(module, d3d10Hooks, "d3d10.dll");
        HookLibraryEx(module, d3d10_1Hooks, "d3d10_1.dll");
        if(pD3D10CreateDevice) {
            HRESULT res;
            res = extD3D10CreateDevice(
                      NULL,
                      D3D10_DRIVER_TYPE_HARDWARE,
                      NULL,
                      0,
                      D3D10_SDK_VERSION,
                      &lpd3d10);
            if(res == DD_OK) lpd3d10->Release();
        }
        break;
#endif
    }
    if(dxw.dwFlags12 & DIRECTXREPLACE) {
        HookEntryEx_Type *Hooks = (hookversion == 8) ? d3d8Hooks : d3d9Hooks;
        if(IsHookedBlock(Hooks)) {
            char path[MAX_PATH];
            sprintf(path, "%salt.dll\\d3d%d.dll", GetDxWndPath(), hookversion);
            PinLibraryEx(Hooks, path);
            OutTrace("HookDirect3D: alt.dll pinned ver=%d path=%s\n", hookversion, path);
        }
    }
}

void HookDirect3D8(HMODULE module) {
    HookDirect3D(module, 8);
}

void HookDirect3D9(HMODULE module) {
    HookDirect3D(module, 9);
}

void HookDirect3D9_30(HMODULE module) {
    // It seems that once loaded, the d3d9_30.dll can activate d3d9.dll methods without that d3d9 is
    // hooked yet, then creating a fullscreen device. This hook function makes sure that d3d9.dll is
    // hooked by means of a temporary load/hook/unload (a.k.a. pin) operation.
    // if dxhook == none, do nothing.
    if(dxw.dwTargetDDVersion == HOOKDDRAWNONE) return;
    // hook Direct3DCreate9 only if not done yet, to avoid recursion !!!
    if(pDirect3DCreate9 == NULL) {
        LPDIRECT3D9 lpd3d;
        PinLibraryEx(d3d9Hooks, "d3d9.dll");
        HookLibraryEx(module, d3d9Hooks, "d3d9.dll");
        if(pDirect3DCreate9) {
            lpd3d = (LPDIRECT3D9)extDirect3DCreate9(31);
            if(lpd3d) lpd3d->Release();
        }
    }
}

void HookD3DDevice8(void **ppD3Ddev8) {
    OutTraceD3D("Device hook for IID_IDirect3DDevice8 interface\n");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 +  0), extQueryInterfaceDev8, (void **)&pQueryInterfaceDev8, "QueryInterface(D8)");
    //SetHook((void *)(**(DWORD **)ppD3Ddev8 +  8), extReleaseDev8, (void **)&pReleaseDev8, "Release(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 12), extTestCooperativeLevel8, (void **)&pTestCooperativeLevel8, "TestCooperativeLevel(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 16), extGetAvailableTextureMem8, (void **)&pGetAvailableTextureMem8, "GetAvailableTextureMem(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 24), extGetDirect3D8, (void **)&pGetDirect3D8, "GetDirect3D(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 32), extGetDisplayMode8, (void **)&pGetDisplayMode8, "GetDisplayMode(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 44), extSetCursorPosition8, (void **)&pSetCursorPosition8, "SetCursorPosition(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 48), extShowCursor8, (void **)&pShowCursor8, "ShowCursor(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 52), extCreateAdditionalSwapChain8, (void **)&pCreateAdditionalSwapChain8, "CreateAdditionalSwapChain(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 56), extReset8, (void **)&pReset8, "Reset(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 60), extPresent8, (void **)&pPresent8, "Present(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 64), extGetBackBuffer8, (void **)&pGetBackBuffer8, "GetBackBuffer(D8)");
    if((dxw.dwFlags2 & DISABLEGAMMARAMP) || (dxw.dwFlags10 & FORCED3DGAMMARAMP)) {
        SetHook((void *)(**(DWORD **)ppD3Ddev8 + 72), extSetGammaRamp8, (void **)&pSetGammaRamp8, "SetGammaRamp(D8)");
        SetHook((void *)(**(DWORD **)ppD3Ddev8 + 76), extGetGammaRamp8, (void **)&pGetGammaRamp8, "GetGammaRamp(D8)");
    }
    if(dxw.dwFlags5 & TEXTUREMASK)
        SetHook((void *)(**(DWORD **)ppD3Ddev8 + 80), extCreateTexture8, (void **)&pCreateTexture8, "CreateTexture(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 100), extCreateRenderTarget8, (void **)&pCreateRenderTarget8, "CreateRenderTarget(D8)");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 104), extCreateDepthStencilSurface8, (void **)&pCreateDepthStencilSurface8, "CreateDepthStencilSurface(D8)");
#endif
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 108), extCreateImageSurface8, (void **)&pCreateImageSurface8, "CreateImageSurface(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 112), extCopyRects, (void **)&pCopyRects, "CopyRects(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 120), extGetFrontBuffer, (void **)&pGetFrontBuffer, "GetFrontBuffer(D8)");
    // SetRenderTarget
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 128), extGetRenderTarget8, (void **)&pGetRenderTarget8, "GetRenderTarget(D8)");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 132), extGetDepthStencilSurface8, (void **)&pGetDepthStencilSurface8, "GetDepthStencilSurface(D8)");
#endif
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 136), extBeginScene8, (void **)&pBeginScene8, "BeginScene(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 140), extEndScene8, (void **)&pEndScene8, "EndScene(D8)");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 144), extClear8, (void **)&pClear8, "Clear(D8)");
#endif
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 160), extSetViewport, (void **)&pSetViewport, "SetViewport(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 164), extGetViewport, (void **)&pGetViewport, "GetViewport(D8)");
    if(dxw.dwFlags10 & LIGHTGAMMARAMP) {
        SetHook((void *)(**(DWORD **)ppD3Ddev8 + 176), extSetLight, (void **)&pSetLight, "SetLight(D8)");
        SetHook((void *)(**(DWORD **)ppD3Ddev8 + 180), extGetLight, (void **)&pGetLight, "GetLight(D8)");
    }
    if((dxw.dwFlags2 & WIREFRAME) || (dxw.dwFlags4 & DISABLEFOGGING) || (dxw.dwDFlags & ZBUFFERALWAYS) || (dxw.dwFlags5 & TEXTURETRANSP)) {
        SetHook((void *)(**(DWORD **)ppD3Ddev8 + 200), extSetRenderState8, (void **)&pSetRenderState8, "SetRenderState(D8)");
        SetHook((void *)(**(DWORD **)ppD3Ddev8 + 204), extGetRenderState8, (void **)&pGetRenderState8, "GetRenderState(D8)");
        if(dxw.dwFlags2 & WIREFRAME) (*pSetRenderState8)((void *)*ppD3Ddev8, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
        if(dxw.dwFlags4 & DISABLEFOGGING) (*pSetRenderState8)((void *)*ppD3Ddev8, D3DRS_FOGENABLE, FALSE);
        if(dxw.dwDFlags & ZBUFFERALWAYS) (*pSetRenderState8)((void *)*ppD3Ddev8, D3DRS_ZFUNC, D3DCMP_ALWAYS);
        //if(1) (*pSetRenderState8)((void *)*ppD3Ddev8, D3DRS_SPECULARENABLE, TRUE);
    }
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 208), extBeginStateBlock8, (void **)&pBeginStateBlock8, "BeginStateBlock(D8)");
    SetHook((void *)(**(DWORD **)ppD3Ddev8 + 212), extEndStateBlock8, (void **)&pEndStateBlock8, "EndStateBlock(D8)");
    if((dxw.dwFlags5 & TEXTUREMASK) || (dxw.dwFlags4 & NOTEXTURES))
        SetHook((void *)(**(DWORD **)ppD3Ddev8 + 244), extSetTexture8, (void **)&pSetTexture8, "SetTexture(D8)");
}

void HookD3DDevice9(void **ppD3Ddev9) {
    OutTraceD3D("Device hook for IID_IDirect3DDevice9 interface\n");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 +  0), extQueryInterfaceDev9, (void **)&pQueryInterfaceDev9, "QueryInterface(D9)");
    //SetHook((void *)(**(DWORD **)ppD3Ddev9 +  8), extReleaseDev9, (void **)&pReleaseDev9, "Release(D9)");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 12), extTestCooperativeLevel9, (void **)&pTestCooperativeLevel9, "TestCooperativeLevel(D9)");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 16), extGetAvailableTextureMem9, (void **)&pGetAvailableTextureMem9, "GetAvailableTextureMem(D9)");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 24), extGetDirect3D9, (void **)&pGetDirect3D9, "GetDirect3D(D9)");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 32), extGetDisplayMode9, (void **)&pGetDisplayMode9, "GetDisplayMode(D9)");
    //SetHook((void *)(**(DWORD **)ppD3Ddev9 + 36), extGetGetCreationParameters9, (void **)&pGetCreationParameters9, "GetCreationParameters(D9)");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 44), extSetCursorPosition9, (void **)&pSetCursorPosition9, "SetCursorPosition(D9)");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 48), extShowCursor9, (void **)&pShowCursor9, "ShowCursor(D9)");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 52), extCreateAdditionalSwapChain9, (void **)&pCreateAdditionalSwapChain9, "CreateAdditionalSwapChain(D9)");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 56), extGetSwapChain, (void **)&pGetSwapChain, "GetSwapChain(D9)");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 60), extGetNumberOfSwapChains, (void **)&pGetNumberOfSwapChains, "extGetNumberOfSwapChains(D9)");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 64), extReset9, (void **)&pReset9, "Reset(D9)");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 68), extPresent9, (void **)&pPresent9, "Present(D9)");
    if((dxw.dwFlags2 & DISABLEGAMMARAMP) || (dxw.dwFlags10 & FORCED3DGAMMARAMP)) {
        SetHook((void *)(**(DWORD **)ppD3Ddev9 + 84), extSetGammaRamp9, (void **)&pSetGammaRamp9, "SetGammaRamp(D9)");
        SetHook((void *)(**(DWORD **)ppD3Ddev9 + 88), extGetGammaRamp9, (void **)&pGetGammaRamp9, "GetGammaRamp(D9)");
    }
    if(dxw.dwFlags5 & TEXTUREMASK)
        SetHook((void *)(**(DWORD **)ppD3Ddev9 + 92), extCreateTexture9, (void **)&pCreateTexture9, "CreateTexture(D9)");
    //SetHook((void *)(**(DWORD **)ppD3Ddev9 + 112), extCreateRenderTarget9, (void **)&pCreateRenderTarget9, "CreateRenderTarget(D9)");
    //#ifdef TRACEALL
    //	SetHook((void *)(**(DWORD **)ppD3Ddev9 + 120), extUpdateSurface, (void **)&pUpdateSurface, "UpdateSurface(D9)");
    //	SetHook((void *)(**(DWORD **)ppD3Ddev9 + 124), extUpdateTexture, (void **)&pUpdateTexture, "UpdateTexture(D9)");
    //#endif
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 132), extGetFrontBufferData9, (void **)&pGetFrontBufferData9, "GetFrontBufferData(D9)");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 152), extGetRenderTarget9, (void **)&pGetRenderTarget9, "GetRenderTarget(D9)");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 156), extSetDepthStencilSurface9, (void **)&pSetDepthStencilSurface9, "SetDepthStencilSurface(D9)");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 160), extGetDepthStencilSurface9, (void **)&pGetDepthStencilSurface9, "GetDepthStencilSurface(D9)");
#endif // TRACEALL
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 164), extBeginScene9, (void **)&pBeginScene9, "BeginScene(D9)");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 168), extEndScene9, (void **)&pEndScene9, "EndScene(D9)");
    //SetHook((void *)(**(DWORD **)ppD3Ddev9 +188), extSetViewport, (void **)&pSetViewport, "SetViewport(D9)");
    //SetHook((void *)(**(DWORD **)ppD3Ddev9 +192), extGetViewport, (void **)&pGetViewport, "GetViewport(D9)");
    if(dxw.dwFlags10 & LIGHTGAMMARAMP) {
        SetHook((void *)(**(DWORD **)ppD3Ddev9 + 204), extSetLight, (void **)&pSetLight, "SetLight(D9)");
        SetHook((void *)(**(DWORD **)ppD3Ddev9 + 208), extGetLight, (void **)&pGetLight, "GetLight(D9)");
    }
    if((dxw.dwFlags2 & WIREFRAME) || (dxw.dwFlags4 & DISABLEFOGGING) || (dxw.dwDFlags & ZBUFFERALWAYS) || (dxw.dwFlags5 & TEXTURETRANSP)) {
        SetHook((void *)(**(DWORD **)ppD3Ddev9 + 228), extSetRenderState9, (void **)&pSetRenderState9, "SetRenderState(D9)");
        SetHook((void *)(**(DWORD **)ppD3Ddev9 + 232), extGetRenderState9, (void **)&pGetRenderState9, "GetRenderState(D9)");
        if(dxw.dwFlags2 & WIREFRAME) (*pSetRenderState9)((void *)*ppD3Ddev9, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
        if(dxw.dwFlags4 & DISABLEFOGGING) (*pSetRenderState9)((void *)*ppD3Ddev9, D3DRS_FOGENABLE, FALSE);
        if(dxw.dwDFlags & ZBUFFERALWAYS) (*pSetRenderState9)((void *)*ppD3Ddev9, D3DRS_ZFUNC, D3DCMP_ALWAYS);
        //if(1) (*pSetRenderState9)((void *)*ppD3Ddev9, D3DRS_SPECULARENABLE, TRUE);
    }
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 240), extBeginStateBlock9, (void **)&pBeginStateBlock9, "BeginStateBlock(D9)");
    SetHook((void *)(**(DWORD **)ppD3Ddev9 + 244), extEndStateBlock9, (void **)&pEndStateBlock9, "EndStateBlock(D9)");
    if((dxw.dwFlags5 & TEXTUREMASK) || (dxw.dwFlags4 & NOTEXTURES))
        SetHook((void *)(**(DWORD **)ppD3Ddev9 + 260), extSetTexture9, (void **)&pSetTexture9, "SetTexture(D9)");
}

// WIP
void HookD3DTexture8(void **ppD3Dtex8) {
    OutTraceD3D("Device hook for IDirect3DTexture8 interface\n");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)ppD3Dtex8 + 56), extGetLevelDesc8, (void **)&pGetLevelDesc8, "GetLevelDesc(T8)");
    SetHook((void *)(**(DWORD **)ppD3Dtex8 + 60), extGetSurfaceLevel8, (void **)&pGetSurfaceLevel8, "GetSurfaceLevel(T8)");
#endif
    SetHook((void *)(**(DWORD **)ppD3Dtex8 + 64), extLockRect8, (void **)&pLockRect8, "LockRect(T8)");
    SetHook((void *)(**(DWORD **)ppD3Dtex8 + 68), extUnlockRect8, (void **)&pUnlockRect8, "UnlockRect(T8)");
}

void HookD3DTexture9(void **ppD3Dtex9) {
    OutTraceD3D("Device hook for IDirect3DTexture9 interface\n");
    SetHook((void *)(**(DWORD **)ppD3Dtex9 + 76), extLockRect9, (void **)&pLockRect9, "LockRect(T9)");
    SetHook((void *)(**(DWORD **)ppD3Dtex9 + 80), extUnlockRect9, (void **)&pUnlockRect9, "UnlockRect(T9)");
}

void HookDirect3D8Session(void *lpd3d) {
    SetHook((void *)(*(DWORD *)lpd3d +  0), extQueryInterfaceD3D8, (void **)&pQueryInterfaceD3D8, "QueryInterface(D8)");
    SetHook((void *)(*(DWORD *)lpd3d +  8), extReleaseD3D8, (void **)&pReleaseD3D8, "Release(D8)");
    SetHook((void *)(*(DWORD *)lpd3d + 16), extGetAdapterCount8, (void **)&pGetAdapterCount8, "GetAdapterCount(D8)");
    SetHook((void *)(*(DWORD *)lpd3d + 20), extGetAdapterIdentifier8, (void **)&pGetAdapterIdentifier8, "GetAdapterIdentifier(D8)");
    SetHook((void *)(*(DWORD *)lpd3d + 24), extGetAdapterModeCount8, (void **)&pGetAdapterModeCount8, "GetAdapterGetAdapterModeCount(D8)");
    SetHook((void *)(*(DWORD *)lpd3d + 28), extEnumAdapterModes8, (void **)&pEnumAdapterModes8, "EnumAdapterModes(D8)");
    SetHook((void *)(*(DWORD *)lpd3d + 32), extGetAdapterDisplayMode8, (void **)&pGetAdapterDisplayMode8, "GetAdapterDisplayMode(D8)");
    SetHook((void *)(*(DWORD *)lpd3d + 36), extCheckDeviceType8, (void **)&pCheckDeviceType8, "CheckDeviceType(D8)");
    SetHook((void *)(*(DWORD *)lpd3d + 52), extD3DGetDeviceCaps8, (void **)&pD3DGetDeviceCaps8, "GetDeviceCaps(D8)");
    SetHook((void *)(*(DWORD *)lpd3d + 56), extGetAdapterMonitor8, (void **)&pGetAdapterMonitor8, "GetAdapterMonitor(D8)");
    SetHook((void *)(*(DWORD *)lpd3d + 60), extCreateDevice8, (void **)&pCreateDevice8, "CreateDevice(D8)");
}

void HookDirect3D9Session(void *lpd3d, BOOL ex) {
    SetHook((void *)(*(DWORD *)lpd3d +  0), extQueryInterfaceD3D9, (void **)&pQueryInterfaceD3D9, "QueryInterface(D9)");
    SetHook((void *)(*(DWORD *)lpd3d +  8), extReleaseD3D9, (void **)&pReleaseD3D9, "Release(D9)");
    SetHook((void *)(*(DWORD *)lpd3d + 16), extGetAdapterCount9, (void **)&pGetAdapterCount9, "GetAdapterCount(D9)");
    SetHook((void *)(*(DWORD *)lpd3d + 20), extGetAdapterIdentifier9, (void **)&pGetAdapterIdentifier9, "GetAdapterIdentifier(D9)");
    SetHook((void *)(*(DWORD *)lpd3d + 24), extGetAdapterModeCount9, (void **)&pGetAdapterModeCount9, "GetAdapterGetAdapterModeCount(D9)");
    SetHook((void *)(*(DWORD *)lpd3d + 28), extEnumAdapterModes9, (void **)&pEnumAdapterModes9, "EnumAdapterModes(D9)");
    SetHook((void *)(*(DWORD *)lpd3d + 32), extGetAdapterDisplayMode9, (void **)&pGetAdapterDisplayMode9, "GetAdapterDisplayMode(D9)");
    SetHook((void *)(*(DWORD *)lpd3d + 36), extCheckDeviceType9, (void **)&pCheckDeviceType9, "CheckDeviceType(D9)");
    SetHook((void *)(*(DWORD *)lpd3d + 56), extD3DGetDeviceCaps9, (void **)&pD3DGetDeviceCaps9, "GetDeviceCaps(D9)");
    SetHook((void *)(*(DWORD *)lpd3d + 60), extGetAdapterMonitor9, (void **)&pGetAdapterMonitor9, "GetAdapterMonitor(D9)");
    SetHook((void *)(*(DWORD *)lpd3d + 64), extCreateDevice9, (void **)&pCreateDevice9, "CreateDevice(D9)");
    if(ex) SetHook((void *)(*(DWORD *)lpd3d + 80), extCreateDeviceEx, (void **)&pCreateDeviceEx, "CreateDeviceEx(D9)");
}

void HookSwapChain8(void *lpSwC) {
    OutTraceD3D("Device hook for IDirect3DSwapChain8 interface\n");
    /*** IUnknown methods ***/
    //STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    //STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    //STDMETHOD_(ULONG,Release)(THIS) PURE;
    /*** IDirect3DSwapChain8 methods ***/
    //STDMETHOD(Present)(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion) PURE;
    SetHook((void *)(*(DWORD *)lpSwC + 12), extPresentSwC8, (void **)&pPresentSwC8, "Present(SwC8)");
    //STDMETHOD(GetBackBuffer)(THIS_ UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface8** ppBackBuffer) PURE;
}

void HookSwapChain9(void *lpSwC) {
    OutTraceD3D("Device hook for IDirect3D9SwapChain9 interface\n");
    /*** IUnknown methods ***/
    //STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    //STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    //STDMETHOD_(ULONG,Release)(THIS) PURE;
    /*** IDirect3DSwapChain9 methods ***/
    //STDMETHOD(Present)(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags) PURE;
    SetHook((void *)(*(DWORD *)lpSwC + 12), extPresentSwC9, (void **)&pPresentSwC9, "Present(SwC9)");
    //STDMETHOD(GetFrontBufferData)(THIS_ IDirect3DSurface9* pDestSurface) PURE;
    //STDMETHOD(GetBackBuffer)(THIS_ UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer) PURE;
    //STDMETHOD(GetRasterStatus)(THIS_ D3DRASTER_STATUS* pRasterStatus) PURE;
    //STDMETHOD(GetDisplayMode)(THIS_ D3DDISPLAYMODE* pMode) PURE;
    //STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) PURE;
    //STDMETHOD(GetPresentParameters)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters) PURE;
}

typedef enum {
    TYPE_OBJECT_UNKNOWN = 0,
    TYPE_OBJECT_DIRECT3D,
    TYPE_OBJECT_DIRECT3DDEVICE,
    TYPE_OBJECT_GAMMARAMP,
    TYPE_OBJECT_BASETEXTURE,
    TYPE_OBJECT_TEXTURE,
    TYPE_OBJECT_3DSURFACE,
    TYPE_OBJECT_VIEWPORT
};

static HRESULT WINAPI QueryInterfaceD3D(int d3dversion, QueryInterface_Type pQueryInterfaceD3D, void *obj, REFIID riid, void **ppvObj) {
    HRESULT res;
    ApiName((d3dversion == 9) ? "IDirect3D9::QueryInterface" : "IDirect3D8::QueryInterface");
    int iObjectType;
    int iObjectVersion;
    BOOL bEx = FALSE;
    iObjectVersion = 0;
    iObjectType = TYPE_OBJECT_UNKNOWN;
    OutTraceD3D("%s: d3d=%#x riid=%#x(%s)\n", ApiRef, obj, riid.Data1, ExplainGUID((GUID *)&riid));
    switch(riid.Data1) {
    // DirectDraw
    case 0x1dd9e8da: //IID_IDirect3D8
        iObjectType = TYPE_OBJECT_DIRECT3D;
        iObjectVersion = 8;
        break;
    case 0x81bdcbca: // IID_IDirect3D9
        iObjectType = TYPE_OBJECT_DIRECT3D;
        iObjectVersion = 9;
        break;
    case 0x02177241: // IID_IDirect3D9Ex
        iObjectType = TYPE_OBJECT_DIRECT3D;
        iObjectVersion = 9;
        bEx = TRUE;
        break;
    case 0x7385e5df: // IID_IDirect3DDevice8
        iObjectType = TYPE_OBJECT_DIRECT3DDEVICE;
        iObjectVersion = 8;
        break;
    case 0xd0223b96: // IID_IDirect3DDevice9
        iObjectType = TYPE_OBJECT_DIRECT3DDEVICE;
        iObjectVersion = 9;
        break;
    case 0xb18b10ce: // IID_IDirect3DDevice9Ex
        iObjectType = TYPE_OBJECT_DIRECT3DDEVICE;
        iObjectVersion = 9;
        bEx = TRUE;
        break; // !!!!
    case 0xb4211cfa: // IID_IDirect3DBaseTexture8
        iObjectType = TYPE_OBJECT_BASETEXTURE;
        iObjectVersion = 8;
        break;
    case 0xe4cdd575: // IID_IDirect3DTexture8
        iObjectType = TYPE_OBJECT_TEXTURE;
        iObjectVersion = 8;
        break;
    case 0x580ca87e: // IID_IDirect3DBaseTexture9
        iObjectType = TYPE_OBJECT_BASETEXTURE;
        iObjectVersion = 9;
        break;
    case 0x85c31227: // IID_IDirect3DTexture9
        iObjectType = TYPE_OBJECT_TEXTURE;
        iObjectVersion = 9;
        break;
    case 0xb96eebca: // IID_IDirect3DSurface8
        iObjectType = TYPE_OBJECT_3DSURFACE;
        iObjectVersion = 8;
        break;
    case 0x0cfbaf3a: // IID_IDirect3DSurface9
        iObjectType = TYPE_OBJECT_3DSURFACE;
        iObjectVersion = 9;
        break;
    }
    /* IID_IDirect3DDevice9Video */
    // {26DC4561-A1EE-4ae7-96DA-118A36C0EC95}
    // DEFINE_GUID(IID_IDirect3DDevice9Video, 0x26dc4561, 0xa1ee, 0x4ae7, 0x96, 0xda, 0x11, 0x8a, 0x36, 0xc0, 0xec, 0x95);
    char *sLabel;
    switch(iObjectType) {
    case TYPE_OBJECT_DIRECT3D:
        sLabel = "IID_IDirect3D";
        break;
    case TYPE_OBJECT_DIRECT3DDEVICE:
        sLabel = "IID_IDirect3DDevice";
        break;
    case TYPE_OBJECT_BASETEXTURE:
        sLabel = "IID_IDirect3DBaseTexture";
        break;
    case TYPE_OBJECT_TEXTURE:
        sLabel = "IID_IDirect3DTexture";
        break;
    case TYPE_OBJECT_3DSURFACE:
        sLabel = "IID_IDirect3DSurface";
        break;
    case TYPE_OBJECT_UNKNOWN:
    default:
        sLabel = "(unknown)";
        break;
    }
    OutTraceDW("%s: Got interface for %s version %d\n", ApiRef, sLabel, iObjectVersion);
    res = pQueryInterfaceD3D(obj, riid, ppvObj);
    if(res) {
        OutTraceDW("%s: ERROR obj=%#x REFIID=%#x obp=%#x ret=%#x(%s)\n",
                   ApiRef, obj, riid.Data1, *ppvObj, res, ExplainDDError(res));
        return res;
    }
    if (! *ppvObj) {
        OutTraceDW("%s: Interface for object %#x not found\n", ApiRef, riid.Data1);
        return res;
    }
    // added trace
    OutTraceDW("%s: obj=%#x REFIID=%#x obp=%#x obj=%s version=%d ret=0\n",
               ApiRef, obj, riid.Data1, *ppvObj, sLabel, iObjectVersion);
    if(iObjectVersion == 8) {
        switch(iObjectType) {
        case TYPE_OBJECT_DIRECT3D:
            HookDirect3D8Session(*ppvObj);
            break;
        case TYPE_OBJECT_DIRECT3DDEVICE:
            HookD3DDevice8(ppvObj);
            break;
        case TYPE_OBJECT_TEXTURE:
            HookD3DTexture8(ppvObj);
            break;
        }
    } else { // 9!
        switch(iObjectType) {
        case TYPE_OBJECT_DIRECT3D:
            HookDirect3D9Session(*ppvObj, bEx);
            break;
        case TYPE_OBJECT_DIRECT3DDEVICE:
            HookD3DDevice9(ppvObj);
            break;
        case TYPE_OBJECT_TEXTURE:
            HookD3DTexture9(ppvObj);
            break;
        }
    }
    OutTraceD3D("%s: obp=%#x\n", ApiRef, *ppvObj);
    return res;
}

HRESULT WINAPI extQueryInterfaceD3D8(void *obj, REFIID riid, void **ppvObj) {
    return QueryInterfaceD3D(8, pQueryInterfaceD3D8, obj, riid, ppvObj);
}
HRESULT WINAPI extQueryInterfaceDev8(void *obj, REFIID riid, void **ppvObj) {
    return QueryInterfaceD3D(8, pQueryInterfaceDev8, obj, riid, ppvObj);
}
HRESULT WINAPI extQueryInterfaceD3D9(void *obj, REFIID riid, void **ppvObj) {
    return QueryInterfaceD3D(9, pQueryInterfaceD3D9, obj, riid, ppvObj);
}
HRESULT WINAPI extQueryInterfaceDev9(void *obj, REFIID riid, void **ppvObj) {
    return QueryInterfaceD3D(9, pQueryInterfaceDev9, obj, riid, ppvObj);
}

HRESULT WINAPI extReleaseD3D8(void *lpd3d) {
    HRESULT res;
    ApiName("IDirect3D8::Release");
    res = (*pReleaseD3D8)(lpd3d);
    OutTraceD3D("%s: ref=%d\n", ApiRef, res);
    return res;
}

HRESULT WINAPI extReleaseD3D9(void *lpd3d) {
    HRESULT res;
    ApiName("IDirect3D9::Release");
    res = (*pReleaseD3D9)(lpd3d);
    OutTraceD3D("%s: ref=%d\n", ApiRef, res);
    return res;
}

BOOL WINAPI extDisableD3DSpy(void) {
    if(TRUE) return FALSE;
    return (*pDisableD3DSpy)();
}

void *WINAPI extDirect3DCreate8(UINT sdkversion) {
    void *lpd3d;
    ApiName("Direct3DCreate8");
    OutTraceD3D("%s: sdkversion=%#x\n", ApiRef, sdkversion);
    lpd3d = (*pDirect3DCreate8)(sdkversion);
    if(!lpd3d) {
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
        return 0;
    }
    dwD3DVersion = 8;
    HookDirect3D8Session(lpd3d);
    lpD3DActiveSession = lpd3d;
    OutTraceD3D("%s: d3d=%#x\n", ApiRef, lpd3d);
    return lpd3d;
}

#ifndef DXW_NOTRACES
static char *ExplainD3DBehaviourFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "D3DCREATE_");
    if (c & D3DCREATE_FPU_PRESERVE) strcat(eb, "FPU_PRESERVE+");
    if (c & D3DCREATE_MULTITHREADED) strcat(eb, "MULTITHREADED+");
    if (c & D3DCREATE_PUREDEVICE) strcat(eb, "PUREDEVICE+");
    if (c & D3DCREATE_SOFTWARE_VERTEXPROCESSING) strcat(eb, "SOFTWARE_VERTEXPROCESSING+");
    if (c & D3DCREATE_HARDWARE_VERTEXPROCESSING) strcat(eb, "HARDWARE_VERTEXPROCESSING+");
    if (c & D3DCREATE_MIXED_VERTEXPROCESSING) strcat(eb, "MIXED_VERTEXPROCESSING+");
    if (c & D3DCREATE_DISABLE_DRIVER_MANAGEMENT) strcat(eb, "DISABLE_DRIVER_MANAGEMENT+");
    if (c & D3DCREATE_ADAPTERGROUP_DEVICE) strcat(eb, "ADAPTERGROUP_DEVICE+");
    if (c & D3DCREATE_DISABLE_DRIVER_MANAGEMENT_EX) strcat(eb, "DISABLE_DRIVER_MANAGEMENT_EX+");
    l = strlen(eb);
    if (l > strlen("D3DCREATE_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}
#endif // DXW_NOTRACES

void *WINAPI extDirect3DCreate9(UINT sdkversion) {
    void *lpd3d;
    ApiName("Direct3DCreate9");
    OutTraceD3D("%s: sdkversion=%#x\n", ApiRef, sdkversion);
    lpd3d = (*pDirect3DCreate9)(sdkversion);
    if(!lpd3d) {
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
        return 0;
    }
    dwD3DVersion = 9;
    HookDirect3D9Session(lpd3d, FALSE);
    lpD3DActiveSession = lpd3d;
    OutTraceD3D("%s: d3d=%#x\n", ApiRef, lpd3d);
    return lpd3d;
}

HRESULT WINAPI extDirect3DCreate9Ex(UINT sdkversion, IDirect3D9Ex **ppD3D) {
    void *lpd3d;
    HRESULT res;
    ApiName("Direct3DCreate9Ex");
    OutTraceD3D("%s: sdkversion=%#x\n", ApiRef, sdkversion);
    res = (*pDirect3DCreate9Ex)(sdkversion, ppD3D);
    if(res) {
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    dwD3DVersion = 9;
    lpd3d = *ppD3D;
    HookDirect3D9Session(lpd3d, TRUE);
    lpD3DActiveSession = lpd3d;
    OutTraceD3D("%s: d3d=%#x\n", ApiRef, lpd3d);
    return res;
}

UINT WINAPI extGetAdapterCount8(void *lpd3d) {
    UINT res;
    ApiName("IDirect3D8::GetAdapterCount");
    res = (*pGetAdapterCount8)(lpd3d);
    OutTraceD3D("%s: count=%d\n", ApiRef, res);
    if(dxw.dwFlags2 & HIDEMULTIMONITOR) {
        OutTraceD3D("%s: HIDEMULTIMONITOR count=1\n", ApiRef);
        res = 1;
    }
    return res;
}

UINT WINAPI extGetAdapterCount9(void *lpd3d) {
    UINT res;
    ApiName("IDirect3D9::GetAdapterCount");
    res = (*pGetAdapterCount9)(lpd3d);
    OutTraceD3D("%s: count=%d\n", ApiRef, res);
    if(dxw.dwFlags2 & HIDEMULTIMONITOR) {
        OutTraceD3D("%s: HIDEMULTIMONITOR count=1\n", ApiRef);
        res = 1;
    }
    return res;
}

HRESULT WINAPI extGetAdapterIdentifier8(void *pd3dd, UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9 *pIdentifier) {
    HRESULT res;
    ApiName("IDirect3D8::GetAdapterIdentifier");
    OutTraceD3D("%s: adapter=%d flags=%#x\n", ApiRef, Adapter, Flags);
    res = pGetAdapterIdentifier8(pd3dd, Adapter, Flags, pIdentifier);
    if(res) {
        D3DError(res);
        return res;
    }
    //if(pIdentifier){
    //	OutTraceD3D("\tDriver=%s\n", pIdentifier->Driver);
    //	OutTraceD3D("\tDescription=%s\n", pIdentifier->Description);
    //	OutTraceD3D("\tDeviceName=%s\n", pIdentifier->DeviceName);
    //	OutTraceD3D("\tVersion=%#x\n", pIdentifier->DriverVersion);
    //}
    return res;
}

HRESULT WINAPI extGetAdapterIdentifier9(void *pd3dd, UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9 *pIdentifier) {
    HRESULT res;
    ApiName("IDirect3D9::GetAdapterIdentifier");
    OutTraceD3D("%s: adapter=%d flags=%#x\n", ApiRef, Adapter, Flags);
    res = pGetAdapterIdentifier9(pd3dd, Adapter, Flags, pIdentifier);
    if(res) {
        D3DError(res);
        return res;
    }
    if(pIdentifier) {
        OutTraceD3D("\tDriver=%s\n", pIdentifier->Driver);
        OutTraceD3D("\tDescription=%s\n", pIdentifier->Description);
        OutTraceD3D("\tDeviceName=%s\n", pIdentifier->DeviceName);
        OutTraceD3D("\tVersion=%#x\n", pIdentifier->DriverVersion);
        OutTraceD3D("\tVendorId=%#x\n", pIdentifier->VendorId);
        OutTraceD3D("\tDeviceId=%#x\n", pIdentifier->DeviceId);
        OutTraceD3D("\tSubSysId=%#x\n", pIdentifier->SubSysId);
        OutTraceD3D("\tWHQLLevel=%#x\n", pIdentifier->WHQLLevel);
    }
    return res;
}

#ifndef DXW_NOTRACES
static char *ExplainSwapEffect(DWORD f) {
    char *s;
    switch(f) {
    case D3DSWAPEFFECT_DISCARD:
        s = "DISCARD";
        break;
    case D3DSWAPEFFECT_FLIP:
        s = "FLIP";
        break;
    case D3DSWAPEFFECT_COPY:
        s = "COPY";
        break;
    case D3DSWAPEFFECT_OVERLAY:
        s = "OVERLAY";
        break;
    case D3DSWAPEFFECT_FLIPEX:
        s = "FLIPEX";
        break;
    default:
        s = "unknown";
        break;
    }
    return s;
}
#endif // DXW_NOTRACES

static HRESULT WINAPI extReset(int dwD3DVersion, void *pd3dd, D3DPRESENT_PARAMETERS *pPresParam) {
    HRESULT res;
    ApiName(dwD3DVersion == 9 ? "IDirect3DDevice9::Reset" : "IDirect3DDevice8::Reset");
    DWORD param[64];
    DWORD Windowed;
    Reset_Type pReset;
    memcpy(param, pPresParam, (dwD3DVersion == 9) ? 56 : 52);
    dxw.SetScreenSize(param[0], param[1]);
#ifndef DXW_NOTRACES
    if(IsTraceD3D) {
        DWORD *tmp;
        tmp = param;
        DWORD SwapEffect;
        OutTrace("%s\n", ApiRef);
        OutTrace("    BackBufferWidth = %i\n", *(tmp ++));
        OutTrace("    BackBufferHeight = %i\n", *(tmp ++));
        OutTrace("    BackBufferFormat = %i(%s)\n", *(tmp ++), ExplainD3DSurfaceFormat(param[2]));
        OutTrace("    BackBufferCount = %i\n", *(tmp ++));
        OutTrace("    MultiSampleType = %i\n", *(tmp ++));
        if(dwD3DVersion == 9) OutTrace("    MultiSampleQuality = %i\n", *(tmp ++));
        SwapEffect = *(tmp ++);
        OutTrace("    SwapEffect = %#x(%s)\n", SwapEffect, ExplainSwapEffect(SwapEffect));
        OutTrace("    hDeviceWindow = %#x\n", *(tmp ++));
        OutTrace("    Windowed = %i\n", *(tmp ++));
        OutTrace("    EnableAutoDepthStencil = %i\n", *(tmp ++));
        OutTrace("    AutoDepthStencilFormat = %i\n", *(tmp ++));
        OutTrace("    Flags = %#x\n", *(tmp ++));
        OutTrace("    FullScreen_RefreshRateInHz = %i\n", *(tmp ++));
        OutTrace("    PresentationInterval = %#x\n", *(tmp ++));
    }
#endif // DXW_NOTRACES
    // v2.04.36: safe exit ?
    if(dxw.dwFlags4 & NOD3DRESET) {
        OutTraceD3D("%s: skipped NOD3DRESET ret=D3D_OK\n", ApiRef);
        return D3D_OK;
    }
    Windowed = (dwD3DVersion == 8) ? param[7] : param[8];
    pReset = (dwD3DVersion == 8) ? pReset8 : pReset9;
    if(dxw.Windowize) {
        if(Windowed) {
            // do not attempt to reset when returning to windowed mode: it is useless (you are windowed already)
            // and the code below (GetDirect3D, ...) doesn't work.
            OutTraceD3D("%s: SKIPPED windowed ret=D3D_OK\n", ApiRef);
            return DD_OK;
        }
        if (dwD3DVersion == 9) {
            if(dxw.dwFlags6 & FORCESWAPEFFECT) param[6] = dxw.SwapEffect;			//Swap effect;
            dwD3DSwapEffect = param[6];
            param[7] = 0;			//hDeviceWindow
            dxw.SetFullScreen(~param[8] ? TRUE : FALSE);
            param[8] = 1;			//Windowed
            //param[11] = D3DPRESENTFLAG_DEVICECLIP;			//Flags;
            param[12] = 0;			//FullScreen_RefreshRateInHz;
            param[13] = SetD3DPresentInterval(param[13]);	//PresentationInterval
        } else {
            if(dxw.dwFlags6 & FORCESWAPEFFECT) param[5] = dxw.SwapEffect;			//Swap effect;
            dwD3DSwapEffect = param[5];
            param[6] = 0;			//hDeviceWindow
            dxw.SetFullScreen(~param[7] ? TRUE : FALSE);
            param[7] = 1;			//Windowed
            //param[10] = D3DPRESENTFLAG_DEVICECLIP;			//Flags;
            param[11] = 0;			//FullScreen_RefreshRateInHz;
            // warning: D3DDevice8 can't support anything different from D3DPRESENT_INTERVAL_DEFAULT
            // when in Windowed mode !!
            param[12] = D3DPRESENT_INTERVAL_DEFAULT; //PresentationInterval
        }
    }
    if((dxw.dwFlags8 & D3D8BACK16) && (dwD3DVersion == 8) && (dxw.VirtualPixelFormat.dwRGBBitCount == 16)) {
        D3DPRESENT_PARAMETERS *params = (D3DPRESENT_PARAMETERS *)&param[0];
        dwD3D8ShutdownEmulation(pd3dd);
        // Change the presentation effect from D3DSWAPEFFECT_COPY to D3DSWAPEFFECT_DISCARD to fix blank screen on W10.
        params->SwapEffect = D3DSWAPEFFECT_DISCARD;
        // Drop the lockable backbuffer flag to be safe. We do not need it.
        params->Flags &= ~D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    }
    res = (*pReset)(pd3dd, (D3DPRESENT_PARAMETERS *)param);
    if(res) {
        OutTraceD3D("%s: err=%#x(%s) switching to mode=%d(%s)\n",
                    ApiRef, res, ExplainDDError(res), gCurrentDisplayMode.Format,
                    ExplainD3DSurfaceFormat(gCurrentDisplayMode.Format));
        param[2] = gCurrentDisplayMode.Format; // first attempt: current screen mode
        res = (*pReset)(pd3dd, (D3DPRESENT_PARAMETERS *)param);
    }
    if(res && dxw.IsFullScreen()) {
        OutTraceD3D("%s: switching to mode=D3DFMT_UNKNOWN\n", ApiRef);
        param[2] = D3DFMT_UNKNOWN; // second attempt: unknown, good for windowed mode
        res = (*pReset)(pd3dd, (D3DPRESENT_PARAMETERS *)param);
    }
    if(res == D3DERR_INVALIDCALL) {
        OutTraceD3D("%s: FAILED! D3DERR_INVALIDCALL\n", ApiRef, res);
        return D3DERR_INVALIDCALL;
    }
    if(res) {
        OutTraceD3D("%s: FAILED! %#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: SUCCESS!\n", ApiRef);
    (dwD3DVersion == 8) ? HookD3DDevice8(&pd3dd) : HookD3DDevice9(&pd3dd);
    if((dxw.dwFlags8 & D3D8BACK16) && (dwD3DVersion == 8) && (dxw.VirtualPixelFormat.dwRGBBitCount == 16)) {
        dwD3D8InitEmulation(pd3dd);
#ifndef DXW_NOTRACES
        if(res) OutTraceE("%s: dwD3D8InitEmulation ERROR: err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
#endif // DXW_NOTRACES
    }
    dxw.SetScreenSize(pPresParam->BackBufferWidth, pPresParam->BackBufferHeight);
    dxw.SetVSyncDelays(gCurrentDisplayMode.RefreshRate);
    GetHookInfo()->IsFullScreen = dxw.IsFullScreen();
    GetHookInfo()->DXVersion = (short)dwD3DVersion;
    GetHookInfo()->Height = (short)dxw.GetScreenHeight();
    GetHookInfo()->Width = (short)dxw.GetScreenWidth();
    GetHookInfo()->ColorDepth = (short)dxw.VirtualPixelFormat.dwRGBBitCount;
    return D3D_OK;
}

HRESULT WINAPI extReset8(void *pd3dd, D3DPRESENT_PARAMETERS *pPresParam) {
    return extReset(8, pd3dd, pPresParam);
}
HRESULT WINAPI extReset9(void *pd3dd, D3DPRESENT_PARAMETERS *pPresParam) {
    return extReset(9, pd3dd, pPresParam);
}

static HRESULT WINAPI extPresent(int d3dversion, Present_Type pPresent, void *pd3dd, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion) {
    HRESULT res;
    ApiName(d3dversion == 9 ? "IDirect3DDevice9::Present" : "IDirect3DDevice8::Present");
    RECT RemappedSrcRect, RemappedDstRect;
    BOOL bEmulation = FALSE;
    if(IsDebugD3D) {
        char sSourceRect[81];
        char sDestRect[81];
        if (pSourceRect) sprintf_s(sSourceRect, 80, "(%d,%d)-(%d,%d)", pSourceRect->left, pSourceRect->top, pSourceRect->right, pSourceRect->bottom);
        else strcpy(sSourceRect, "(NULL)");
        if (pDestRect) sprintf_s(sDestRect, 80, "(%d,%d)-(%d,%d)", pDestRect->left, pDestRect->top, pDestRect->right, pDestRect->bottom);
        else strcpy(sDestRect, "(NULL)");
        OutDebugD3D("%s: d3dd=%#x SourceRect=%s DestRect=%s hDestWndOverride=%#x dirtyregion=%#x\n",
                    ApiRef, pd3dd, sSourceRect, sDestRect, hDestWindowOverride, pDirtyRegion);
    }
    // frame counter handling....
    if (dxw.HandleFPS()) return D3D_OK;
    if (dxw.dwFlags1 & SAVELOAD) dxw.VSyncWait();
    if (dxw.dwFlags5 & MESSAGEPUMP) dxw.MessagePump();
    if (gFlippedToScreen) {
        OutTraceD3D("%s: FilledToScreen SKIP\n", ApiRef);
        gFlippedToScreen--;
        return D3D_OK;
    }
    if((dxw.dwFlags8 & D3D8BACK16) && (d3dversion == 8) && (dxw.VirtualPixelFormat.dwRGBBitCount == 16)) {
        res = dwD3D8Present(pd3dd, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
        OutTraceD3D("%s: EMULATION res=%#x\n", ApiRef, res);
        bEmulation = TRUE;
    }
    if(dxw.dwDFlags & FULLRECTBLT) pSourceRect = pDestRect = NULL;
    if(dxw.Windowize) {
        // v2.03.15 - fix target RECT region
        // v2.04.37 - target scaling must be done only when pDestRect is not NULL. Fixes "Fastlane Pinball" black screen.
        if(pDestRect) {
            RemappedDstRect = dxw.MapClientRect((LPRECT)pDestRect);
            pDestRect = &RemappedDstRect;
            OutDebugD3D("%s: FIXED DestRect=(%d,%d)-(%d,%d)\n", ApiRef, RemappedDstRect.left, RemappedDstRect.top, RemappedDstRect.right, RemappedDstRect.bottom);
        }
        // in case of NOD3DRESET, remap source rect. Unfortunately, this doesn't work in fullscreen mode ....
        if((dxw.dwFlags4 & NOD3DRESET) && (pSourceRect == NULL)) {
            RemappedSrcRect = dxw.GetScreenRect();
            pSourceRect = &RemappedSrcRect;
            OutDebugD3D("%s: NOD3DRESET FIXED SourceRect=(%d,%d)-(%d,%d)\n", ApiRef, RemappedSrcRect.left, RemappedSrcRect.top, RemappedSrcRect.right, RemappedSrcRect.bottom);
        }
    }
    res = (*pPresent)(pd3dd, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
    if(res)
        OutTraceE("%s: err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else {
        if(IsDebugDW) {
            if(pDirtyRegion) {
                OutTraceD3D("%s: D3D_OK dirtyreg={buf=%#x type=%d count=%d rgnsize=%#x bound=(%d,%d)-(%d,%d)}\n",
                            ApiRef, pDirtyRegion->Buffer, pDirtyRegion->rdh.iType, pDirtyRegion->rdh.nCount, pDirtyRegion->rdh.nRgnSize,
                            pDirtyRegion->rdh.rcBound.left, pDirtyRegion->rdh.rcBound.top, pDirtyRegion->rdh.rcBound.right, pDirtyRegion->rdh.rcBound.bottom);
            } else
                OutTraceD3D("%s: D3D_OK\n", ApiRef);
        }
    }
    if(bEmulation) (*pInvalidateRect)(dxw.GethWnd(), NULL, FALSE);
    dxw.ShowOverlay();
    return res;
}

HRESULT WINAPI extPresent8(void *pd3dd, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion) {
    return extPresent(8, pPresent8, pd3dd, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}
HRESULT WINAPI extPresent9(void *pd3dd, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion) {
    return extPresent(9, pPresent9, pd3dd, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT WINAPI extGetDisplayMode8(void *lpd3d, D3DDISPLAYMODE *pMode) {
    HRESULT res;
    ApiName("IDirect3DDevice8::GetDisplayMode");
    res = (*pGetDisplayMode8)(lpd3d, pMode);
    OutTraceD3D("%s: size=(%dx%d) RefreshRate=%d Format=%d\n",
                ApiRef, pMode->Width, pMode->Height, pMode->RefreshRate, pMode->Format);
    if(dxw.dwFlags2 & KEEPASPECTRATIO) {
        pMode->Width = dxw.iSizX;
        pMode->Height = dxw.iSizY;
    } else {
        pMode->Width = dxw.GetScreenWidth();
        pMode->Height = dxw.GetScreenHeight();
    }
    OutTraceD3D("%s: fixed size=(%dx%d)\n", ApiRef, pMode->Width, pMode->Height);
    dxw.SetVSyncDelays(pMode->RefreshRate);
    return res;
}

HRESULT WINAPI extGetDisplayMode9(void *lpd3d, UINT iSwapChain, D3DDISPLAYMODE *pMode) {
    HRESULT res;
    ApiName("IDirect3DDevice9::GetDisplayMode");
    res = (*pGetDisplayMode9)(lpd3d, iSwapChain, pMode);
    OutTraceD3D("%s: SwapChain=%d size=(%dx%d) RefreshRate=%d Format=%d\n",
                ApiRef, iSwapChain, pMode->Width, pMode->Height, pMode->RefreshRate, pMode->Format);
    if(dxw.dwFlags2 & KEEPASPECTRATIO) {
        pMode->Width = dxw.iSizX;
        pMode->Height = dxw.iSizY;
    } else {
        pMode->Width = dxw.GetScreenWidth();
        pMode->Height = dxw.GetScreenHeight();
    }
    OutTraceD3D("%s: fixed size=(%dx%d)\n", ApiRef, pMode->Width, pMode->Height);
    dxw.SetVSyncDelays(pMode->RefreshRate);
    return res;
}

#if 0
HRESULT WINAPI extGetCreationParameters9(void *lpd3dd, D3DDEVICE_CREATION_PARAMETERS *pParameters) {
    HRESULT res;
    ApiName("IDirect3DDevice9::GetCreationParameters");
    OutTraceD3D("%s: lpd3dd=%#x\n", lpd3dd);
    res = (*pGetCreationParameters9)(lpd3d, pParameters);
    if(res)
        OutTraceE("%s: err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else {
        OutTraceD3D("%s: AdapterOrdinal=%d DeviceType=%d hFocusWindow=%#x BehaviorFlags=%#x\n",
                    ApiRef,
                    pParameters->AdapterOrdinal,
                    pParameters->DeviceType,
                    pParameters->hFocusWindow,
                    pParameters->BehaviorFlags);
    }
    return res;
}
#endif

static HRESULT WINAPI emuEnumAdapterModes8(UINT Mode, D3DDISPLAYMODE *pMode) {
    SupportedRes_Type *pTypes = SupportedRes;
    UINT MaxModes = 0;
    for(SupportedRes_Type *pTypes0 = pTypes; pTypes0->w; pTypes0++) MaxModes++; // count how many are there ...
    OutTraceD3D("EnumAdapterModes: %d emulated modes\n", MaxModes);
    UINT vMode = Mode;
    pMode->Format = D3DFMT_X8R8G8B8;// v2.05.05 fix: using valid 32BPP D3DFMT_X8R8G8B8 format (see remark)
    if(dxw.dwFlags7 & ENUM16BITMODES) {
        vMode = Mode / 2;
        if(Mode % 2) pMode->Format = D3DFMT_R5G6B5;
    }
    if(vMode >= MaxModes) return D3DERR_NOTAVAILABLE;
    pMode->Width = pTypes[vMode].w;
    pMode->Height = pTypes[vMode].h;
    pMode->RefreshRate = 0;
    OutTraceD3D("EnumAdapterModes: res=(%dx%d) refresh=%dHz format=%d(%s)\n",
                pMode->Width, pMode->Height, pMode->RefreshRate, pMode->Format, ExplainD3DSurfaceFormat(pMode->Format));
    return D3D_OK;
}

static HRESULT WINAPI emuEnumAdapterModes9(UINT Mode, D3DDISPLAYMODE *pMode, D3DFORMAT Format) {
    // from MSDN https://docs.microsoft.com/en-us/windows/desktop/api/d3d9/nf-d3d9-idirect3d9-enumadaptermodes
    //
    // Remarks
    //
    // An application supplies a display mode and a format to EnumAdapterModes which returns a display mode.
    // This method could be used in a loop to enumerate all available display modes.
    //
    // The application specifies a format and the enumeration is restricted to those display modes that exactly
    // match the format (alpha is ignored). Allowed formats (which are members of D3DFORMAT) are as follows:
    //
    //    D3DFMT_A1R5G5B5
    //    D3DFMT_A2R10G10B10
    //    D3DFMT_A8R8G8B8
    //    D3DFMT_R5G6B5
    //    D3DFMT_X1R5G5B5
    //    D3DFMT_X8R8G8B8
    //
    // In addition, EnumAdapterModes treats pixel formats 565 and 555 as equivalent, and returns the correct version.
    // The difference comes into play only when the application locks the back buffer and there is an explicit flag
    // that the application must set in order to accomplish this.
    SupportedRes_Type *pTypes = SupportedRes;
    UINT MaxModes = 0;
    for(SupportedRes_Type *pTypes0 = pTypes; pTypes0->w; pTypes0++) MaxModes++; // count how many are there ...
    OutTraceD3D("EnumAdapterModes: %d emulated modes\n", MaxModes);
    UINT vMode = Mode;
    pMode->Format = Format;
    if(vMode >= MaxModes) return D3DERR_NOTAVAILABLE;
    pMode->Width = pTypes[vMode].w;
    pMode->Height = pTypes[vMode].h;
    pMode->RefreshRate = 0;
    OutTraceD3D("EnumAdapterModes: res=(%dx%d) refresh=%dHz format=%d(%s)\n",
                pMode->Width, pMode->Height, pMode->RefreshRate, pMode->Format, ExplainD3DSurfaceFormat(pMode->Format));
    return D3D_OK;
}

HRESULT WINAPI extEnumAdapterModes8(void *lpd3d, UINT Adapter, UINT Mode, D3DDISPLAYMODE *pMode) {
    HRESULT res;
    ApiName("IDirect3D8::EnumAdapterModes");
    OutTraceD3D("%s: adapter=%d mode=%d pMode=%#x\n", ApiRef, Adapter, Mode, pMode);
    if(dxw.dwFlags4 & (SUPPORTSVGA | SUPPORTHDTV)) return emuEnumAdapterModes8(Mode, pMode);
    Adapter = SetD3DAdapter(Adapter);
    if(dxw.dwFlags7 & ENUM16BITMODES) {
        res = (*pEnumAdapterModes8)(lpd3d, Adapter, Mode / 2, pMode);
        if((res == D3D_OK) && (Mode % 2))
            pMode->Format = (dxw.dwFlags1 & USERGB565) ? D3DFMT_R5G6B5 : D3DFMT_A1R5G5B5;
    } else
        res = (*pEnumAdapterModes8)(lpd3d, Adapter, Mode, pMode);
    if(res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else {
        OutTraceD3D("%s: res=(%dx%d) refresh=%dHz format=%d(%s)\n",
                    ApiRef, pMode->Width, pMode->Height, pMode->RefreshRate, pMode->Format, ExplainD3DSurfaceFormat(pMode->Format));
    }
    return res;
}

HRESULT WINAPI extEnumAdapterModes9(void *lpd3d, UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE *pMode) {
    HRESULT res;
    ApiName("IDirect3D9::EnumAdapterModes");
    OutTraceD3D("%s: adapter=%d format=%#x mode=%d pMode=%#x\n", ApiRef, Adapter, Format, Mode, pMode);
    if(dxw.dwFlags4 & (SUPPORTSVGA | SUPPORTHDTV)) return emuEnumAdapterModes9(Mode, pMode, Format);
    Adapter = SetD3DAdapter(Adapter);
    if(dxw.dwFlags7 & ENUM16BITMODES) {
        res = (*pEnumAdapterModes9)(lpd3d, Adapter, Format, Mode / 2, pMode);
        if((res == D3D_OK) && (Mode % 2))
            pMode->Format = (dxw.dwFlags1 & USERGB565) ? D3DFMT_R5G6B5 : D3DFMT_A1R5G5B5;
    } else
        res = (*pEnumAdapterModes9)(lpd3d, Adapter, Format, Mode, pMode);
    if(res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else {
        OutTraceD3D("%s: res=(%dx%d) refresh=%dHz format=%d(%s)\n",
                    ApiRef, pMode->Width, pMode->Height, pMode->RefreshRate, pMode->Format, ExplainD3DSurfaceFormat(pMode->Format));
    }
    return res;
}

HRESULT WINAPI extGetAdapterDisplayMode(int d3dversion, GetAdapterDisplayMode_Type pGetAdapterDisplayMode, void *lpd3d, UINT Adapter, D3DDISPLAYMODE *pMode) {
    HRESULT res;
    ApiName((d3dversion == 9) ? "IDirect3D9::GetAdapterDisplayMode" : "IDirect3D8::GetAdapterDisplayMode");
    OutTraceD3D("%s: adapter=%d\n", ApiRef, Adapter);
    Adapter = SetD3DAdapter(Adapter);
    res = (*pGetAdapterDisplayMode)(lpd3d, Adapter, pMode);
    if(res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: adapter=%d size=(%dx%d) RefreshRate=%d Format=%d(%s)\n",
                ApiRef, Adapter, pMode->Width, pMode->Height, pMode->RefreshRate, pMode->Format, ExplainD3DSurfaceFormat(pMode->Format));
    dxw.SetVSyncDelays(pMode->RefreshRate);
    if(dxw.Windowize) {
        if(dxw.dwFlags2 & KEEPASPECTRATIO) {
            pMode->Width = dxw.iSizX;
            pMode->Height = dxw.iSizY;
        } else {
            pMode->Width = dxw.GetScreenWidth();
            pMode->Height = dxw.GetScreenHeight();
        }
        OutTraceD3D("%s: fixed size=(%dx%d)\n", ApiRef, pMode->Width, pMode->Height);
    }
    return res;
}

HRESULT WINAPI extGetAdapterDisplayMode8(void *lpd3d, UINT Adapter, D3DDISPLAYMODE *pMode) {
    return extGetAdapterDisplayMode(8, pGetAdapterDisplayMode8, lpd3d, Adapter, pMode);
}
HRESULT WINAPI extGetAdapterDisplayMode9(void *lpd3d, UINT Adapter, D3DDISPLAYMODE *pMode) {
    return extGetAdapterDisplayMode(9, pGetAdapterDisplayMode9, lpd3d, Adapter, pMode);
}


HWND FixD3DWindowFrame(HWND hFocusWin) {
    HWND hRetWnd = hFocusWin;
    if(!(dxw.dwFlags3 & NOWINDOWMOVE))
        dxw.AdjustWindowFrame(dxw.GethWnd(), dxw.GetScreenWidth(), dxw.GetScreenHeight());
    if(dxw.dwFlags3 & FIXD3DFRAME) {
        char ClassName[81];
        RECT workarea;
        HWND hChildWin;
        GetClassName(dxw.GethWnd(), ClassName, 80);
        (*pGetClientRect)(dxw.GethWnd(), &workarea);
        if (dxw.dwFlags2 & KEEPASPECTRATIO) dxw.FixWorkarea(&workarea);
        hChildWin = (*pCreateWindowExA)(
                        0, ClassName, "child",
                        WS_CHILD | WS_VISIBLE,
                        //GetSystemMetrics(SM_CXSIZEFRAME), GetSystemMetrics(SM_CYSIZEFRAME)+GetSystemMetrics(SM_CYCAPTION),
                        workarea.left, workarea.top, workarea.right - workarea.left, workarea.bottom - workarea.top,
                        //workarea.left+20, workarea.top+20, workarea.right-workarea.left-40, workarea.bottom-workarea.top-40,
                        dxw.GethWnd(), NULL, NULL, NULL);
        if (hChildWin)
            OutTraceD3D("CreateDevice: ChildWin=%#x pos=(%d,%d) size=(%d,%d)\n",
                        hChildWin, workarea.left, workarea.top, workarea.right - workarea.left, workarea.bottom - workarea.top);
        else
            OutTraceE("CreateDevice: CreateWindowEx ERROR err=%d\n", GetLastError());
        dxw.SethWnd(hChildWin, dxw.GethWnd());
        dxw.HookWindowProc(hChildWin);
        hRetWnd = hChildWin;
    }
    return hRetWnd;
}

#define FORCEVERTEXPROC (FORCEHWVERTEXPROC|FORCESWVERTEXPROC|FORCEMXVERTEXPROC)

int ForceBehaviourFlags(char *apiname, int behaviorflags) {
    behaviorflags &= ~(D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING);
    switch(dxw.dwFlags10 & FORCEVERTEXPROC) {
    case FORCEHWVERTEXPROC:
        behaviorflags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
        break;
    case FORCESWVERTEXPROC:
        behaviorflags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        behaviorflags &= ~D3DCREATE_PUREDEVICE;
        break;
    case FORCEMXVERTEXPROC:
        behaviorflags |= D3DCREATE_MIXED_VERTEXPROCESSING;
        behaviorflags &= ~D3DCREATE_PUREDEVICE;
        break;
    }
    OutTraceDW("%s: FORCE BehaviorFlags=%#x(%s)\n", apiname, behaviorflags, ExplainD3DBehaviourFlags(behaviorflags));
    return behaviorflags;
}

static HRESULT WINAPI extCreateDevice(void *lpd3d, UINT Adapter, D3DDEVTYPE devicetype,
                                      HWND hfocuswindow, DWORD behaviorflags, D3DPRESENT_PARAMETERS *ppresentparam, void **ppd3dd, int dwD3DVersion) {
    HRESULT res;
    ApiName((dwD3DVersion == 9) ? "IDirect3D9::CreateDevice" : "IDirect3D8::CreateDevice");
    DWORD param[64];
    D3DDISPLAYMODE mode;
    D3DFORMAT BackBufferFormat;
    DDS_PIXELFORMAT pf;
    OutTraceD3D("%s: lpd3d=%#x adapter=%d hFocusWnd=%#x behavior=%#x(%s), size=(%d,%d)\n",
                ApiRef, lpd3d, Adapter, hfocuswindow, behaviorflags, ExplainD3DBehaviourFlags(behaviorflags),
                ppresentparam->BackBufferWidth, ppresentparam->BackBufferHeight);
    if(dxw.dwFlags10 & FORCEVERTEXPROC) behaviorflags = ForceBehaviourFlags(ApiRef, behaviorflags);
    memcpy(param, ppresentparam, (dwD3DVersion == 9) ? 56 : 52);
    //if(hfocuswindow) dxw.SethWnd(hfocuswindow); // do not clear previous setting with an invalid one
    dxw.SethWnd(hfocuswindow);
    dxw.SetScreenSize(param[0], param[1]);
    Adapter = SetD3DAdapter(Adapter);
    // Beware: this d3d session could be different from the first one registered during first Direct3DCreate operation when DxWnd performs
    // this operation at hook start to initialize the d3d8/9 hooks (that is, when you set DirectX version to DirectX8/9).
    if(lpD3DActiveSession != lpd3d) {
        OutTraceD3D("%s: register new session %#x -> %#x\n", ApiRef, lpD3DActiveSession, lpd3d);
        if(dwD3DVersion == 8)
            HookDirect3D8Session(lpd3d);
        else
            HookDirect3D9Session(lpd3d, FALSE); // could be a Direct3DSessionEx, but how could we know?
        lpD3DActiveSession = lpd3d;
    }
    if(dxw.Windowize) hfocuswindow = FixD3DWindowFrame(hfocuswindow);
    // v2.04.37: fix. VirtualPixelFormat set outside trace condition!
    BackBufferFormat = (D3DFORMAT)param[2];
    memset(&pf, 0, sizeof(pf));
    pf.dwSize = sizeof(pf);
    if(BackBufferFormat != D3DFMT_UNKNOWN) {
        dxSetDDSPixelFormat(BackBufferFormat, &pf);
        dxw.VirtualPixelFormat.dwRGBBitCount = pf.dwRGBBitCount;
        dxw.VirtualPixelFormat.dwRBitMask = pf.dwRBitMask;
        dxw.VirtualPixelFormat.dwGBitMask = pf.dwGBitMask;
        dxw.VirtualPixelFormat.dwBBitMask = pf.dwBBitMask;
        dxw.VirtualPixelFormat.dwRGBAlphaBitMask = pf.dwABitMask;
    }
#ifndef DXW_NOTRACES
    if(IsTraceD3D) {
        DWORD *tmp = param;
        DWORD SwapEffect;
        OutTrace("%s\n", ApiRef);
        OutTrace("  Adapter = %i\n", Adapter);
        OutTrace("  DeviceType = %i\n", devicetype);
        OutTrace("  hFocusWindow = %#x\n", hfocuswindow);
        OutTrace("  BehaviorFlags = %#x(%s)\n", behaviorflags, ExplainD3DBehaviourFlags(behaviorflags));
        OutTrace("    BackBufferWidth = %i\n", *(tmp ++));
        OutTrace("    BackBufferHeight = %i\n", *(tmp ++));
        (tmp ++); // BackBufferFormat
        OutTrace("    BackBufferFormat = %i(%s)\n", BackBufferFormat, ExplainD3DSurfaceFormat(BackBufferFormat));
        OutTrace("    BackBufferCount = %i\n", *(tmp ++));
        OutTrace("    MultiSampleType = %i\n", *(tmp ++));
        if(dwD3DVersion == 9) OutTrace("    MultiSampleQuality = %i\n", *(tmp ++));
        SwapEffect = *(tmp ++);
        OutTrace("    SwapEffect = %#x(%s)\n", SwapEffect, ExplainSwapEffect(SwapEffect));
        OutTrace("    hDeviceWindow = %#x\n", *(tmp ++));
        OutTrace("    Windowed = %i\n", *(tmp ++));
        OutTrace("    EnableAutoDepthStencil = %i\n", *(tmp ++));
        OutTrace("    AutoDepthStencilFormat = %i\n", *(tmp ++));
        OutTrace("    Flags = %#x\n", *(tmp ++));
        OutTrace("    FullScreen_RefreshRateInHz = %i\n", *(tmp ++));
        OutTrace("    PresentationInterval = %#x\n", *(tmp ++));
        OutTrace("    Virtual Pixel Format bpp=%d RGBAmask=(%#x.%#x.%#x.%#x)\n",
                 dxw.VirtualPixelFormat.dwRGBBitCount,
                 dxw.VirtualPixelFormat.dwRBitMask,
                 dxw.VirtualPixelFormat.dwGBitMask,
                 dxw.VirtualPixelFormat.dwBBitMask,
                 dxw.VirtualPixelFormat.dwRGBAlphaBitMask);
    }
#endif
    if(dwD3DVersion == 9)
        (*pGetAdapterDisplayMode9)(lpd3d, Adapter, &mode);
    else
        (*pGetAdapterDisplayMode8)(lpd3d, Adapter, &mode);
    OutTraceD3D("    Current Format = %d(%s)\n", mode.Format, ExplainD3DSurfaceFormat(mode.Format));
    OutTraceD3D("    Current ScreenSize = (%dx%d)\n", mode.Width, mode.Height);
    OutTraceD3D("    Current Refresh Rate = %d\n", mode.RefreshRate);
    memcpy(&gCurrentDisplayMode, &mode, sizeof(D3DDISPLAYMODE));
    if((dxw.dwFlags4 & NOD3DRESET) && dxw.Windowize) {
        RECT desktop;
        // Get a handle to the desktop window
        const HWND hDesktop = (*pGetDesktopWindow)();
        // Get the size of screen to the variable desktop
        (*pGetWindowRect)(hDesktop, &desktop);
        // The top left corner will have coordinates (0,0)
        // and the bottom right corner will have coordinates
        // (horizontal, vertical)
        param[0] = desktop.right;
        param[1] = desktop.bottom;
    }
    if(dwD3DVersion == 9) {
        if(dxw.Windowize) {
            if(dxw.dwFlags6 & FORCESWAPEFFECT) param[6] = dxw.SwapEffect;			//Swap effect;
            dwD3DSwapEffect = param[6];
            param[7] = 0;			//hDeviceWindow
            dxw.SetFullScreen(~param[8] ? TRUE : FALSE);
            param[8] = 1;			//Windowed
            //param[11] = D3DPRESENTFLAG_DEVICECLIP;			//Flags;
            param[12] = 0;			//FullScreen_RefreshRateInHz;
            param[13] = SetD3DPresentInterval(param[13]);	//PresentationInterval
        }
        res = (*pCreateDevice9)(lpd3d, Adapter, devicetype, hfocuswindow, behaviorflags, param, ppd3dd);
        if(res) {
            OutTraceD3D("%s: err=%#x(%s) switching to mode=%d(%s)\n", ApiRef, res, ExplainDDError(res), mode.Format, ExplainD3DSurfaceFormat(mode.Format));
            param[2] = mode.Format; // first attempt: current screen mode
            res = (*pCreateDevice9)(lpd3d, 0, devicetype, hfocuswindow, behaviorflags, param, ppd3dd);
        }
        if(res) {
            OutTraceD3D("%s: err=%#x(%s) switching to mode=D3DFMT_UNKNOWN\n", ApiRef, res, ExplainDDError(res));
            param[2] = D3DFMT_UNKNOWN; // second attempt: unknown, good for windowed mode
            res = (*pCreateDevice9)(lpd3d, 0, devicetype, hfocuswindow, behaviorflags, param, ppd3dd);
        }
    } else {
        if(dxw.Windowize) {
            if(dxw.dwFlags6 & FORCESWAPEFFECT) param[5] = dxw.SwapEffect;			//Swap effect;
            dwD3DSwapEffect = param[5];
            param[6] = 0;			//hDeviceWindow
            dxw.SetFullScreen(~param[7] ? TRUE : FALSE);
            param[7] = 1;			//Windowed
            //param[10] = D3DPRESENTFLAG_DEVICECLIP;			//Flags;
            param[11] = 0;			//FullScreen_RefreshRateInHz;
            // warning: D3DDevice8 can't support anything different from D3DPRESENT_INTERVAL_DEFAULT
            // when in Windowed mode !!
            param[12] = D3DPRESENT_INTERVAL_DEFAULT; //PresentationInterval
        }
        res = (*pCreateDevice8)(lpd3d, Adapter, devicetype, hfocuswindow, behaviorflags, param, ppd3dd);
        if(res) {
            OutTraceD3D("%s: err=%#x(%s) switching to mode=%d(%s)\n", ApiRef, res, ExplainDDError(res), mode.Format, ExplainD3DSurfaceFormat(mode.Format));
            param[2] = mode.Format; // first attempt: current screen mode
            res = (*pCreateDevice8)(lpd3d, 0, devicetype, hfocuswindow, behaviorflags, param, ppd3dd);
        }
        if(res) {
            OutTraceD3D("%s: err=%#x(%s) switching to mode=D3DFMT_UNKNOWN\n", ApiRef, res, ExplainDDError(res));
            param[2] = D3DFMT_UNKNOWN; // second attempt: unknown, good for windowed mode
            res = (*pCreateDevice8)(lpd3d, 0, devicetype, hfocuswindow, behaviorflags, param, ppd3dd);
        }
    }
    if(res) {
        OutTraceD3D("%s: FAILED! err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: SUCCESS! device=%#x\n", ApiRef, *ppd3dd);
    lpD3DActiveDevice = *ppd3dd;
    if((dxw.dwFlags8 & D3D8BACK16) && (dwD3DVersion == 8) && (dxw.VirtualPixelFormat.dwRGBBitCount == 16))
        dwD3D8InitEmulation(*ppd3dd);
    if(dwD3DVersion == 8)
        HookD3DDevice8(ppd3dd);
    else
        HookD3DDevice9(ppd3dd);
    dxw.SetVSyncDelays(mode.RefreshRate);
    GetHookInfo()->IsFullScreen = dxw.IsFullScreen();
    GetHookInfo()->DXVersion = (short)dwD3DVersion;
    GetHookInfo()->Height = (short)dxw.GetScreenHeight();
    GetHookInfo()->Width = (short)dxw.GetScreenWidth();
    GetHookInfo()->ColorDepth = (short)dxw.VirtualPixelFormat.dwRGBBitCount;
    return DD_OK;
}

HRESULT WINAPI extCreateDevice8(void *lpd3d, UINT adapter, D3DDEVTYPE dt, HWND hfw, DWORD bf, D3DPRESENT_PARAMETERS *ppp, void **ppd3dd) {
    return extCreateDevice(lpd3d, adapter, dt, hfw, bf, ppp, ppd3dd, 8);
}
HRESULT WINAPI extCreateDevice9(void *lpd3d, UINT adapter, D3DDEVTYPE dt, HWND hfw, DWORD bf, D3DPRESENT_PARAMETERS *ppp, void **ppd3dd) {
    return extCreateDevice(lpd3d, adapter, dt, hfw, bf, ppp, ppd3dd, 9);
}


HRESULT WINAPI extCreateDeviceEx(void *lpd3d, UINT Adapter, D3DDEVTYPE devicetype,
                                 HWND hfocuswindow, DWORD behaviorflags, D3DPRESENT_PARAMETERS *ppresentparam, D3DDISPLAYMODEEX *pFullscreenDisplayMode, void **ppd3dd) {
    HRESULT res;
    ApiName("IDirect3D9::CreateDeviceEx");
    DWORD param[64];
    D3DDISPLAYMODE mode;
    D3DFORMAT BackBufferFormat;
    DDS_PIXELFORMAT pf;
    OutTraceD3D("%s: lpd3d=%#x adapter=%d hFocusWnd=%#x behavior=%#x(%s), size=(%d,%d)\n",
                ApiRef, lpd3d, Adapter, hfocuswindow, behaviorflags, ExplainD3DBehaviourFlags(behaviorflags),
                ppresentparam->BackBufferWidth, ppresentparam->BackBufferHeight);
    if(dxw.dwFlags10 & FORCEVERTEXPROC) behaviorflags = ForceBehaviourFlags(ApiRef, behaviorflags);
    memcpy(param, ppresentparam, 56);
    //if(hfocuswindow) dxw.SethWnd(hfocuswindow); // do not clear previous setting with an invalid one
    dxw.SethWnd(hfocuswindow);
    dxw.SetScreenSize(param[0], param[1]);
    Adapter = SetD3DAdapter(Adapter);
    // Beware: this d3d session could be different from the first one registered during first Direct3DCreateEx operation when DxWnd performs
    // this operation at hook start to initialize the d3d9 hooks (that is, when you set DirectX version to DirectX9).
    if(lpD3DActiveSession != lpd3d) {
        OutTraceD3D("%s: register new session %#x -> %#x\n", ApiRef, lpD3DActiveSession, lpd3d);
        HookDirect3D9Session(lpd3d, TRUE); // it IS a Direct3DSessionEx.
        lpD3DActiveSession = lpd3d;
    }
    if(dxw.Windowize) hfocuswindow = FixD3DWindowFrame(hfocuswindow);
    // v2.04.66: fix. VirtualPixelFormat set outside trace condition!
    BackBufferFormat = (D3DFORMAT)param[2];
    memset(&pf, 0, sizeof(pf));
    pf.dwSize = sizeof(pf);
    if(BackBufferFormat != D3DFMT_UNKNOWN) {
        dxSetDDSPixelFormat(BackBufferFormat, &pf);
        dxw.VirtualPixelFormat.dwRGBBitCount = pf.dwRGBBitCount;
        dxw.VirtualPixelFormat.dwRBitMask = pf.dwRBitMask;
        dxw.VirtualPixelFormat.dwGBitMask = pf.dwGBitMask;
        dxw.VirtualPixelFormat.dwBBitMask = pf.dwBBitMask;
        dxw.VirtualPixelFormat.dwRGBAlphaBitMask = pf.dwABitMask;
    }
#ifndef DXW_NOTRACES
    if(IsTraceD3D) {
        DWORD *tmp = param;
        DWORD SwapEffect;
        OutTrace("%s\n", ApiRef);
        OutTrace("  Adapter = %i\n", Adapter);
        OutTrace("  DeviceType = %i\n", devicetype);
        OutTrace("  hFocusWindow = %#x\n", hfocuswindow);
        OutTrace("  BehaviorFlags = %#x\n", behaviorflags);
        OutTrace("    BackBufferWidth = %i\n", *(tmp ++));
        OutTrace("    BackBufferHeight = %i\n", *(tmp ++));
        OutTrace("    BackBufferFormat = %i(%s)\n", *(tmp ++), ExplainD3DSurfaceFormat(param[2]));
        OutTrace("    BackBufferCount = %i\n", *(tmp ++));
        OutTrace("    MultiSampleType = %i\n", *(tmp ++));
        OutTrace("    MultiSampleQuality = %i\n", *(tmp ++));
        SwapEffect = *(tmp ++);
        OutTrace("    SwapEffect = %#x(%s)\n", SwapEffect, ExplainSwapEffect(SwapEffect));
        OutTrace("    hDeviceWindow = %#x\n", *(tmp ++));
        OutTrace("    Windowed = %i\n", *(tmp ++));
        OutTrace("    EnableAutoDepthStencil = %i\n", *(tmp ++));
        OutTrace("    AutoDepthStencilFormat = %i\n", *(tmp ++));
        OutTrace("    Flags = %#x\n", *(tmp ++));
        OutTrace("    FullScreen_RefreshRateInHz = %i\n", *(tmp ++));
        OutTrace("    PresentationInterval = %#x\n", *(tmp ++));
    }
#endif
    (*pGetAdapterDisplayMode9)(lpd3d, Adapter, &mode);
    OutTraceD3D("    Current Format = %d(%s)\n", mode.Format, ExplainD3DSurfaceFormat(mode.Format));
    OutTraceD3D("    Current ScreenSize = (%dx%d)\n", mode.Width, mode.Height);
    OutTraceD3D("    Current Refresh Rate = %d\n", mode.RefreshRate);
    memcpy(&gCurrentDisplayMode, &mode, sizeof(D3DDISPLAYMODE));
    if((dxw.dwFlags4 & NOD3DRESET) && dxw.Windowize) {
        RECT desktop;
        // Get a handle to the desktop window
        const HWND hDesktop = (*pGetDesktopWindow)();
        // Get the size of screen to the variable desktop
        (*pGetWindowRect)(hDesktop, &desktop);
        // The top left corner will have coordinates (0,0)
        // and the bottom right corner will have coordinates
        // (horizontal, vertical)
        param[0] = desktop.right;
        param[1] = desktop.bottom;
    }
    if(dxw.Windowize) {
        if(dxw.dwFlags6 & FORCESWAPEFFECT) param[6] = dxw.SwapEffect;			//Swap effect;
        dwD3DSwapEffect = param[6];
        param[7] = 0;			//hDeviceWindow
        //param[7] = (DWORD)dxw.GethWnd();			//hDeviceWindow
        dxw.SetFullScreen(~param[8] ? TRUE : FALSE);
        param[8] = 1;			//Windowed
        param[12] = 0;			//FullScreen_RefreshRateInHz;
        param[13] = SetD3DPresentInterval(param[13]);	//PresentationInterval
        // from MSDN:
        // The display mode for when the device is set to fullscreen. See D3DDISPLAYMODEEX.
        // If BehaviorFlags specifies D3DCREATE_ADAPTERGROUP_DEVICE, this parameter is an array.
        // This parameter must be NULL for windowed mode.
        pFullscreenDisplayMode = NULL;
    }
    res = (*pCreateDeviceEx)(lpd3d, Adapter, devicetype, hfocuswindow, behaviorflags, param, pFullscreenDisplayMode, ppd3dd);
    if(res) {
        OutTraceD3D("%s: err=%#x(%s) switching to mode=%d(%s)\n", ApiRef, res, ExplainDDError(res), mode.Format, ExplainD3DSurfaceFormat(mode.Format));
        param[2] = mode.Format; // first attempt: current screen mode
        res = (*pCreateDeviceEx)(lpd3d, 0, devicetype, hfocuswindow, behaviorflags, param, pFullscreenDisplayMode, ppd3dd);
    }
    if(res) {
        OutTraceD3D("%s: err=%#x(%s) switching to mode=D3DFMT_UNKNOWN\n", ApiRef, res, ExplainDDError(res));
        param[2] = D3DFMT_UNKNOWN; // second attempt: unknown, good for windowed mode
        res = (*pCreateDeviceEx)(lpd3d, 0, devicetype, hfocuswindow, behaviorflags, param, pFullscreenDisplayMode, ppd3dd);
    }
    if(res) {
        OutTraceD3D("%s: FAILED! err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("%s: SUCCESS!\n", ApiRef);
    lpD3DActiveDevice = *ppd3dd;
    HookD3DDevice9(ppd3dd);
    dxw.SetVSyncDelays(mode.RefreshRate);
    GetHookInfo()->IsFullScreen = dxw.IsFullScreen();
    GetHookInfo()->DXVersion = (short)dwD3DVersion;
    GetHookInfo()->Height = (short)dxw.GetScreenHeight();
    GetHookInfo()->Width = (short)dxw.GetScreenWidth();
    GetHookInfo()->ColorDepth = (short)dxw.VirtualPixelFormat.dwRGBBitCount;
    return DD_OK;
}

static HRESULT WINAPI extSetRenderState(char *api, SetRenderState_Type pSetRenderState, void *pd3dd, D3DRENDERSTATETYPE State, DWORD Value) {
    HRESULT res;
    OutTraceD3D("%s: d3dd=%#x State=%#x(%s) Value=%#x\n", api, pd3dd, State, ExplainD3DRenderState(State), Value);
    if((dxw.dwDFlags & ZBUFFERALWAYS) && (State == D3DRS_ZFUNC)) {
        OutTraceD3D("%s: FIXED State=ZFUNC Value=%s->D3DCMP_ALWAYS\n", api, ExplainRenderstateValue(Value));
        Value = D3DCMP_ALWAYS;
    }
    if((dxw.dwFlags2 & WIREFRAME) && (State == D3DRS_FILLMODE)) {
        OutTraceD3D("%s: FIXED State=FILLMODE Value=%#x->D3DFILL_WIREFRAME\n", api, Value);
        Value = D3DFILL_WIREFRAME;
    }
    if((dxw.dwFlags4 & DISABLEFOGGING) && (State == D3DRS_FOGENABLE)) {
        OutTraceD3D("%s: FIXED State=FOGENABLE Value=%#x->FALSE\n", api, Value);
        Value = FALSE;
    }
    if((dxw.dwFlags5 & TEXTURETRANSP) && (State == D3DRS_ALPHABLENDENABLE)) {
        OutTraceD3D("%s: FIXED State=ALPHABLENDENABLE Value=%#x->TRUE\n", api, Value);
        Value = TRUE;
    }
    res = (*pSetRenderState)(pd3dd, State, Value);
#ifndef DXW_NOTRACES
    if(res) OutTraceE("%s: res=%#x(%s)\n", api, res, ExplainDDError(res));
#endif // DXW_NOTRACES
    return res;
}

HRESULT WINAPI extSetRenderState8(void *pd3dd, D3DRENDERSTATETYPE State, DWORD Value) {
    return extSetRenderState("IDirect3d8Device::SetRenderState", pSetRenderState8, pd3dd, State, Value);
}
HRESULT WINAPI extSetRenderState9(void *pd3dd, D3DRENDERSTATETYPE State, DWORD Value) {
    return extSetRenderState("IDirect3d9Device::SetRenderState", pSetRenderState9, pd3dd, State, Value);
}

static HRESULT WINAPI extGetRenderState(GetRenderState_Type pGetRenderState, void *pd3dd, D3DRENDERSTATETYPE State, DWORD Value) {
    return (*pGetRenderState)(pd3dd, State, Value);
}

HRESULT WINAPI extGetRenderState8(void *pd3dd, D3DRENDERSTATETYPE State, DWORD Value) {
    return extGetRenderState(pSetRenderState8, pd3dd, State, Value);
}
HRESULT WINAPI extGetRenderState9(void *pd3dd, D3DRENDERSTATETYPE State, DWORD Value) {
    return extGetRenderState(pSetRenderState9, pd3dd, State, Value);
}

HRESULT WINAPI extGetViewport(void *lpd3dd, D3DVIEWPORT9 *pViewport) {
    HRESULT res;
    OutTraceD3D("GetViewport: lpd3dd=%#x\n", lpd3dd);
    res = (*pGetViewport)(lpd3dd, pViewport);
    if(res)
        OutTraceD3D("GetViewport ERROR: err=%#x(%s)\n", res, ExplainDDError(res));
    else {
        OutTraceD3D("GetViewport: orig=(%d,%d) size=(%dx%d) z(min:max)=(%f:%f)\n",
                    pViewport->X, pViewport->Y, pViewport->Width, pViewport->Height, pViewport->MinZ, pViewport->MaxZ);
    }
    return res;
}

HRESULT WINAPI extSetViewport(void *pd3dd, D3DVIEWPORT9 *pViewport) {
    HRESULT res;
    // v2.04.77: commented out - viewport area should NOT be scaled
#if 0
    RECT client;
    POINT p = {0, 0};
    (*pGetClientRect)(dxw.GethWnd(), &client);
    OutTraceD3D("SetViewport: declared pos=(%d,%d) size=(%d,%d) depth=(%f;%f)\n",
                pViewport->X, pViewport->Y, pViewport->Width, pViewport->Height, pViewport->MinZ, pViewport->MaxZ);
    OutDebugD3D("SetViewport: DEBUG win=(%d,%d) screen=(%d,%d)\n",
                client.right, client.bottom, dxw.GetScreenWidth(), dxw.GetScreenHeight());
    pViewport->X = (pViewport->X * (int)client.right) / (int)dxw.GetScreenWidth();
    pViewport->Y = (pViewport->Y * (int)client.bottom) / (int)dxw.GetScreenHeight();
    pViewport->Width = (pViewport->Width * (int)client.right) / (int)dxw.GetScreenWidth();
    pViewport->Height = (pViewport->Height * (int)client.bottom) / (int)dxw.GetScreenHeight();
    OutTraceD3D("SetViewport: remapped pos=(%d,%d) size=(%d,%d)\n",
                pViewport->X, pViewport->Y, pViewport->Width, pViewport->Height);
    return (*pSetViewport)(pd3dd, pViewport);
#endif
    OutTraceD3D("SetViewport: d3dd=%#x orig=(%d,%d) size=(%dx%d) z(min:max)=(%f:%f)\n",
                pd3dd, pViewport->X, pViewport->Y, pViewport->Width, pViewport->Height, pViewport->MinZ, pViewport->MaxZ);
    res = (*pSetViewport)(pd3dd, pViewport);
    if(res)
        OutTraceE("SetViewport ERROR: err=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

static HRESULT WINAPI extCreateAdditionalSwapChain(int dwD3DVersion, void *lpd3dd, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9 **ppSwapChain) {
    HRESULT res;
    ApiName((dwD3DVersion == 9) ? "IDirect3DDevice9::CreateAdditionalSwapChain" : "IDirect3DDevice8::CreateAdditionalSwapChain");
    DWORD param[64];
    CreateAdditionalSwapChain_Type pCreateAdditionalSwapChain;
    OutTraceD3D("%s: d3d=%#x\n", ApiRef, lpd3dd);
    pCreateAdditionalSwapChain = (dwD3DVersion == 8) ? pCreateAdditionalSwapChain8 : pCreateAdditionalSwapChain9;
    memcpy(param, pPresentationParameters, (dwD3DVersion == 9) ? 56 : 52);
    dxw.SetScreenSize(param[0], param[1]);
#ifndef DXW_NOTRACES
    if(IsTraceD3D) {
        DWORD *tmp;
        tmp = param;
        DWORD SwapEffect;
        OutTrace("%s\n", ApiRef);
        OutTrace("    BackBufferWidth = %i\n", *(tmp ++));
        OutTrace("    BackBufferHeight = %i\n", *(tmp ++));
        OutTrace("    BackBufferFormat = %i(%s)\n", *(tmp ++), ExplainD3DSurfaceFormat(param[2]));
        OutTrace("    BackBufferCount = %i\n", *(tmp ++));
        OutTrace("    MultiSampleType = %i\n", *(tmp ++));
        if(dwD3DVersion == 9) OutTrace("    MultiSampleQuality = %i\n", *(tmp ++));
        SwapEffect = *(tmp ++);
        OutTrace("    SwapEffect = %#x(%s)\n", SwapEffect, ExplainSwapEffect(SwapEffect));
        OutTrace("    hDeviceWindow = %#x\n", *(tmp ++));
        OutTrace("    Windowed = %i\n", *(tmp ++));
        OutTrace("    EnableAutoDepthStencil = %i\n", *(tmp ++));
        OutTrace("    AutoDepthStencilFormat = %i\n", *(tmp ++));
        OutTrace("    Flags = %#x\n", *(tmp ++));
        OutTrace("    FullScreen_RefreshRateInHz = %i\n", *(tmp ++));
        OutTrace("    PresentationInterval = %#x\n", *(tmp ++));
    }
#endif // DXW_NOTRACES
    if(dxw.Windowize) {
        if(dwD3DVersion == 9) {
            if(dxw.dwFlags6 & FORCESWAPEFFECT) param[6] = dxw.SwapEffect;			//Swap effect;
            dwD3DSwapEffect = param[6];
            // v2.04.72: do not force to NULL - fixes "Psychotoxic" movies
            //param[7] = 0;			//hDeviceWindow
            dxw.SetFullScreen(~param[8] ? TRUE : FALSE);
            param[8] = 1;			//Windowed
            param[12] = 0;			//FullScreen_RefreshRateInHz;
            param[13] = SetD3DPresentInterval(param[13]);	//PresentationInterval
        } else {
            if(dxw.dwFlags6 & FORCESWAPEFFECT) param[5] = dxw.SwapEffect;			//Swap effect;
            dwD3DSwapEffect = param[5];
            // v2.04.72: symmetric to D3D9 fix above
            //param[6] = 0;			//hDeviceWindow
            dxw.SetFullScreen(~param[7] ? TRUE : FALSE);
            param[7] = 1;			//Windowed
            param[11] = 0;			//FullScreen_RefreshRateInHz;
            // warning: D3DDevice8 can't support anything different from D3DPRESENT_INTERVAL_DEFAULT
            // when in Windowed mode !!
            param[12] = D3DPRESENT_INTERVAL_DEFAULT; //PresentationInterval
        }
    }
    res = (*pCreateAdditionalSwapChain)(lpd3dd, (D3DPRESENT_PARAMETERS *)param, ppSwapChain);
    if(res) {
        OutTraceD3D("%s: switching to mode=D3DFMT_UNKNOWN\n", ApiRef);
        param[2] = D3DFMT_UNKNOWN; // second attempt: unknown, good for windowed mode
        res = (*pCreateAdditionalSwapChain)(lpd3dd, (D3DPRESENT_PARAMETERS *)param, ppSwapChain);
    }
    if(res) {
        OutTraceD3D("%s: switching to size=0,0\n", ApiRef);
        param[0] = param[1] = 0; // third attempt: unknown, good for windowed mode
        res = (*pCreateAdditionalSwapChain)(lpd3dd, (D3DPRESENT_PARAMETERS *)param, ppSwapChain);
    }
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    //(dwD3DVersion == 9) ? HookD3DDevice9(&lpd3dd) : HookD3DDevice8(&lpd3dd);
    // v2.05.30 fix
    (dwD3DVersion == 9) ? HookSwapChain9(&lpd3dd) : HookSwapChain8(&lpd3dd);
    return res;
}

HRESULT WINAPI extCreateAdditionalSwapChain8(void *lpd3dd, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9 **ppSwapChain) {
    return extCreateAdditionalSwapChain(8, lpd3dd, pPresentationParameters, ppSwapChain);
}
HRESULT WINAPI extCreateAdditionalSwapChain9(void *lpd3dd, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9 **ppSwapChain) {
    return extCreateAdditionalSwapChain(9, lpd3dd, pPresentationParameters, ppSwapChain);
}

// to do:
// hook SetCursorPosition ShowCursor to handle cursor

HRESULT WINAPI extD3D10CreateDevice(
    IDXGIAdapter *pAdapter,
    D3D10_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    UINT SDKVersion,
    ID3D10Device **ppDevice) {
    HRESULT res;
    ApiName("D3D10CreateDevice");
    //D3D10_VIEWPORT ViewPort;
    OutTraceD3D("%s: DriverType=%#x Flags=%#x SDKVersion=%#x\n", ApiRef, DriverType, Flags, SDKVersion);
    //return 0x887a0004;
    res = (*pD3D10CreateDevice)(pAdapter, DriverType, Software, Flags, SDKVersion, ppDevice);
    if(res) {
        OutTraceE("%s: ret=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    // useless ....
    //SetHook((void *)(*(DWORD *)*ppDevice + 120), extRSSetViewports10, (void **)&pRSSetViewports10, "RSSetViewports(D10)");
    // useless ....
    //if(dxw.Windowize){
    //	ViewPort.TopLeftX=dxw.iPosX;
    //	ViewPort.TopLeftY=dxw.iPosY;
    //	ViewPort.Width=dxw.iSizX;
    //	ViewPort.Height=dxw.iSizY;
    //	ViewPort.MinDepth=1.0;
    //	ViewPort.MaxDepth=1.0;
    //	(*pRSSetViewports10)((void *)*ppDevice, 1, (D3D10_VIEWPORT *)&ViewPort);
    //}
    OutTraceD3D("%s: ret=%#x\n", ApiRef, res);
    return res;
}

HRESULT WINAPI extD3D10CreateDevice1(
    IDXGIAdapter *pAdapter,
    D3D10_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    D3D10_FEATURE_LEVEL1 HardwareLevel,
    UINT SDKVersion,
    ID3D10Device **ppDevice) {
    HRESULT res;
    ApiName("D3D10CreateDevice1");
    OutTraceD3D("%s: DriverType=%#x Flags=%#x HardwareLevel=%#x SDKVersion=%#x\n",
                ApiRef, DriverType, Flags, HardwareLevel, SDKVersion);
    res = (*pD3D10CreateDevice1)(pAdapter, DriverType, Software, Flags, HardwareLevel, SDKVersion, ppDevice);
    if(res) {
        D3DError(res);
        return res;
    }
    SetHook((void *)(*(DWORD *)*ppDevice + 100), extRSSetViewports10, (void **)&pRSSetViewports10, "RSSetViewports(D10)");
    OutTraceD3D("%s: ret=%#x\n", ApiRef, res);
    return res;
}

HRESULT WINAPI extD3D10CreateDeviceAndSwapChain(
    IDXGIAdapter *pAdapter,
    D3D10_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    UINT SDKVersion,
    DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
    IDXGISwapChain **ppSwapChain,
    ID3D10Device **ppDevice) {
    HRESULT res;
    ApiName("D3D10CreateDeviceAndSwapChain");
    OutTraceD3D("%s: DriverType=%#x Flags=%#x SDKVersion=%#x\n", ApiRef, DriverType, Flags, SDKVersion);
    if(dxw.Windowize) {
        pSwapChainDesc->OutputWindow = dxw.GethWnd();
        pSwapChainDesc->Windowed = true;
    }
    res = (*pD3D10CreateDeviceAndSwapChain)(pAdapter, DriverType, Software, Flags, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice);
    if(res) {
        D3DError(res);
        return res;
    }
    OutTraceD3D("%s: OK\n", ApiRef);
    return res;
}

HRESULT WINAPI extD3D10CreateDeviceAndSwapChain1(
    IDXGIAdapter *pAdapter,
    D3D10_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    UINT SDKVersion,
    DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
    IDXGISwapChain **ppSwapChain,
    ID3D10Device **ppDevice) {
    HRESULT res;
    ApiName("D3D10CreateDeviceAndSwapChain1");
    OutTraceD3D("%s: DriverType=%#x Flags=%#x SDKVersion=%#x\n", ApiRef, DriverType, Flags, SDKVersion);
    if(dxw.Windowize) {
        pSwapChainDesc->OutputWindow = dxw.GethWnd();
        pSwapChainDesc->Windowed = true;
    }
    res = (*pD3D10CreateDeviceAndSwapChain1)(pAdapter, DriverType, Software, Flags, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice);
    if(res) {
        D3DError(res);
        return res;
    }
    OutTraceD3D("%s: OK\n", ApiRef);
    return res;
}

void WINAPI extRSSetViewports10(void *This, UINT NumViewports, D3D10_VIEWPORT *pViewports) {
    ApiName("IDirect3D10Device1::SetViewports");
    OutTraceD3D("%s: NumViewports=%d\n", ApiRef, NumViewports);
    if(dxw.Windowize && (NumViewports == 1)) {
        pViewports->TopLeftX = dxw.iPosX;
        pViewports->TopLeftY = dxw.iPosY;
        pViewports->Width = dxw.iSizX;
        pViewports->Height = dxw.iSizY;
    }
    (*pRSSetViewports10)(This, NumViewports, pViewports);
}

static ULONG WINAPI ReleaseDev(int d3dversion, ReleaseDev_Type pReleaseDev, void *lpdd) {
    ULONG ActualRef;
    ActualRef = (*pReleaseDev)(lpdd);
    OutTraceD3D("Release(D%d): lpdd=%#x ret=%d\n", d3dversion, lpdd, ActualRef);
    return ActualRef;
}

ULONG WINAPI extReleaseDev8(void *lpdd) {
    return ReleaseDev(8, pReleaseDev8, lpdd);
}
ULONG WINAPI extReleaseDev9(void *lpdd) {
    return ReleaseDev(9, pReleaseDev9, lpdd);
}

HRESULT WINAPI extGetDirect3D8(void *lpdd3dd, void **ppD3D8) {
    HRESULT res;
    ApiName("IDirect3DDevice8::GetDirect3D");
    OutTraceD3D("%s: d3dd=%#x\n", ApiRef, lpdd3dd);
    if(dxw.dwFlags9 & CACHED3DSESSION) {
        if (lpD3DActiveSession) ((IUnknown *)lpD3DActiveSession)->AddRef();
        *ppD3D8 = lpD3DActiveSession;
        res = D3D_OK;
    } else
        res = (*pGetDirect3D8)(lpdd3dd, ppD3D8);
    if(res)
        OutTraceE("%s: ERROR d3dd=%#x ret=%#x\n", ApiRef, lpdd3dd, res);
    else {
        OutTraceD3D("%s: d3dd=%#x d3d=%#x\n", ApiRef, lpdd3dd, *ppD3D8);
        _if(*ppD3D8 != lpD3DActiveSession) OutTraceD3D("Device::GetDirect3D: POSSIBLY UNHOOKED!!!\n");
        // HookDirect3D8(*ppD3D8);
        //if(!IsHooked8(*ppD3D8)) HookDirect3D8(*ppD3D8);
    }
    return res;
}

HRESULT WINAPI extGetDirect3D9(void *lpdd3dd, void **ppD3D9) {
    HRESULT res;
    ApiName("IDirect3DDevice9::GetDirect3D");
    OutTraceD3D("%s: d3dd=%#x\n", ApiRef, lpdd3dd);
    if(dxw.dwFlags9 & CACHED3DSESSION) {
        if (lpD3DActiveSession) ((IUnknown *)lpD3DActiveSession)->AddRef();
        *ppD3D9 = lpD3DActiveSession;
        res = D3D_OK;
    } else
        res = (*pGetDirect3D9)(lpdd3dd, ppD3D9);
    if(res)
        OutTraceE("%s: ERROR d3dd=%#x ret=%#x\n", ApiRef, lpdd3dd, res);
    else {
        OutTraceD3D("%s: d3dd=%#x d3d=%#x\n", ApiRef, lpdd3dd, *ppD3D9);
        // re-hook d3d session: neeeded for Need for Speed Underground
        // commented out: NFSU works also without it ....
        _if(*ppD3D9 != lpD3DActiveSession) OutTraceD3D("%s: POSSIBLY UNHOOKED!!!\n", ApiRef);
        // HookDirect3D9(*ppD3D9, FALSE);
        //if(!IsHooked9(*ppD3D9)) HookDirect3D9(*ppD3D9, FALSE);
    }
    return res;
}

HRESULT WINAPI extCheckFullScreen(void) {
    HRESULT res;
    ApiName("CheckFullScreen");
    if(dxw.Windowize) {
        OutTraceD3D("%s: return OK\n", ApiRef);
        res = 0;
    } else {
        res = (*pCheckFullScreen)();
        OutTraceD3D("%s: return %#x\n", ApiRef, res);
    }
    return res;
}

static char *ExplainGammaRampFlags(DWORD f) {
    char *s;
    switch(f) {
    case D3DSGR_CALIBRATE:
        s = "CALIBRATE";
        break;
    case D3DSGR_NO_CALIBRATION:
        s = "NO_CALIBRATION";
        break;
    default:
        s = "???";
        break;
    }
    return s;
}

void WINAPI extSetGammaRamp8(void *lpdd3dd, DWORD Flags, D3DGAMMARAMP *pRamp) {
    ApiName("IDirect3DDevice8::SetGammaRamp");
    OutTraceD3D("%s: dd3dd=%#x flags=%#x(%s)\n", ApiRef, lpdd3dd, Flags, ExplainGammaRampFlags(Flags));
    if (dxw.dwFlags2 & DISABLEGAMMARAMP) return;
    if((dxw.dwFlags10 & FORCED3DGAMMARAMP) && (dxw.pInitialRamp == NULL)) dxw.InitGammaRamp();
    if (dxw.dwFlags10 & FORCED3DGAMMARAMP) {
        // v2.04.97: release desktop DC - avoids DC leakage in "Call of Cthulhu"
        HDC hDCDesktop = (*pGDIGetDC)(NULL);
        if(!pGDISetDeviceGammaRamp) pGDISetDeviceGammaRamp = SetDeviceGammaRamp;
        memcpy(dxw.pCurrentRamp, pRamp, sizeof(D3DGAMMARAMP));
        (*pGDISetDeviceGammaRamp)(hDCDesktop, pRamp);
        (*pGDIReleaseDC)(NULL, hDCDesktop);
        return;
    }
    (*pSetGammaRamp8)(lpdd3dd, Flags, pRamp);
}

void WINAPI extSetGammaRamp9(void *lpdd3dd, UINT iSwapChain, DWORD Flags, D3DGAMMARAMP *pRamp) {
    ApiName("IDirect3DDevice9::SetGammaRamp");
    OutTraceD3D("%s: dd3dd=%#x SwapChain=%d flags=%#x(%s)\n", ApiRef, lpdd3dd, iSwapChain, Flags, ExplainGammaRampFlags(Flags));
    if (dxw.dwFlags2 & DISABLEGAMMARAMP) return;
    if((dxw.dwFlags10 & FORCED3DGAMMARAMP) && (dxw.pInitialRamp == NULL)) dxw.InitGammaRamp();
    if (dxw.dwFlags10 & FORCED3DGAMMARAMP) {
        // v2.04.97: release desktop DC - avoids DC leakage in "Call of Cthulhu"
        HDC hDCDesktop = (*pGDIGetDC)(NULL);
        if(!pGDISetDeviceGammaRamp) pGDISetDeviceGammaRamp = SetDeviceGammaRamp;
        memcpy(dxw.pCurrentRamp, pRamp, sizeof(D3DGAMMARAMP));
        (*pGDISetDeviceGammaRamp)(hDCDesktop, pRamp);
        (*pGDIReleaseDC)(NULL, hDCDesktop);
        return;
    }
    (*pSetGammaRamp9)(lpdd3dd, iSwapChain, Flags, pRamp);
}

void WINAPI extGetGammaRamp8(void *lpdd3dd, D3DGAMMARAMP *pRamp) {
    ApiName("IDirect3DDevice8::GetGammaRamp");
    OutTraceD3D("%s: dd3dd=%#x\n", ApiRef, lpdd3dd);
    (*pGetGammaRamp8)(lpdd3dd, pRamp);
}

void WINAPI extGetGammaRamp9(void *lpdd3dd, UINT iSwapChain, D3DGAMMARAMP *pRamp) {
    ApiName("IDirect3DDevice9::GetGammaRamp");
    OutTraceD3D("%s: dd3dd=%#x SwapChain=%d\n", ApiRef, lpdd3dd, iSwapChain);
    (*pGetGammaRamp9)(lpdd3dd, iSwapChain, pRamp);
}

void WINAPI extSetCursorPosition8(void *lpdd3dd, int X, int Y, DWORD Flags) {
    ApiName("IDirect3DDevice8::SetCursorPosition");
    OutTraceD3D("%s: dd3dd=%#x pos=(%d,%d) flags=%#x\n", ApiRef, lpdd3dd, X, Y, Flags);
    if(dxw.dwFlags2 & KEEPCURSORFIXED) return;
    // to do: proper scaling.....
    //(*pSetCursorPosition8)(lpdd3dd, X, Y, Flags);
}

void WINAPI extSetCursorPosition9(void *lpdd3dd, int X, int Y, DWORD Flags) {
    ApiName("IDirect3DDevice9::SetCursorPosition");
    OutTraceD3D("%s: dd3dd=%#x pos=(%d,%d) flags=%#x\n", ApiRef, lpdd3dd, X, Y, Flags);
    if(dxw.dwFlags2 & KEEPCURSORFIXED) return;
    // to do: proper scaling.....
    //(*pSetCursorPosition9)(lpdd3dd, X, Y, Flags);
}

//ULONG WINAPI extCreateImageSurface8(void *lpdd3dd, UINT Width, UINT Height, D3DFORMAT Format, IDirect3DSurface8** ppSurface)
ULONG WINAPI extCreateImageSurface8(void *lpdd3dd, UINT Width, UINT Height, D3DFORMAT Format, void **ppSurface) {
    ULONG res;
    ApiName("IDirect3DDevice8::CreateImageSurface");
    OutTraceD3D("%s: dd3dd=%#x dim=(%dx%d) Format=%#x(%s)\n",
                ApiRef, lpdd3dd, Width, Height, Format, ExplainD3DSurfaceFormat(Format));
    res = (*pCreateImageSurface8)(lpdd3dd, Width, Height, Format, ppSurface);
    if(res) OutTraceE("%s: ERROR err=%#x\n", ApiRef, res);
    else OutTraceD3D("%s: surface=%#x\n", ApiRef, *ppSurface);
    return res;
}

//ULONG WINAPI extCreateRenderTarget8(void *lpdd3dd, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, BOOL Lockable, IDirect3DSurface8** ppSurface)
ULONG WINAPI extCreateRenderTarget8(void *lpdd3dd, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, BOOL Lockable, void **ppSurface) {
    ULONG res;
    ApiName("IDirect3DDevice8::CreateRenderTarget");
    OutTraceD3D("%s: dd3dd=%#x dim=(%dx%d) Format=%#x(%s) Lockable=%#x\n",
                ApiRef, lpdd3dd, Width, Height, Format, ExplainD3DSurfaceFormat(Format), Lockable);
    res = (*pCreateRenderTarget8)(lpdd3dd, Width, Height, Format, MultiSample, Lockable, ppSurface);
    if(res) OutTraceE("%s: ERROR err=%#x\n", ApiRef, res);
    else OutTraceD3D("%s: surface=%#x\n", ApiRef, *ppSurface);
    return res;
}

//ULONG WINAPI extGetRenderTarget8(void *lpdd3dd, IDirect3DSurface8** ppRenderTarget) PURE;
ULONG WINAPI extGetRenderTarget8(void *lpdd3dd, void **ppRenderTarget) {
    ULONG res;
    ApiName("IDirect3DDevice8::GetRenderTarget");
    OutDebugD3D("%s: d3dd=%#x\n", ApiRef, lpdd3dd);
    res = (*pGetRenderTarget8)(lpdd3dd, ppRenderTarget);
    if(res) OutTraceE("%s: ERROR err=%#x\n", ApiRef, res);
    else OutDebugD3D("%s: surface=%#x\n", ApiRef, *ppRenderTarget);
#ifndef DXW_NOTRACES
    if((dxw.dwDFlags & DUMPSURFACES) && dxw.bCustomKeyToggle) SurfaceDump((LPDIRECTDRAWSURFACE) *ppRenderTarget, 8);
#endif // DXW_NOTRACES
    return res;
}

ULONG WINAPI extGetRenderTarget9(void *lpdd3dd, DWORD dwRenderTargetIndex, void **ppRenderTarget) {
    ULONG res;
    ApiName("IDirect3DDevice9::GetRenderTarget");
    OutDebugD3D("%s: d3dd=%#x index=%d\n", ApiRef, lpdd3dd, dwRenderTargetIndex);
    res = (*pGetRenderTarget9)(lpdd3dd, dwRenderTargetIndex, ppRenderTarget);
    if(res) OutTraceE("%s: ERROR err=%#x\n", ApiRef, res);
    else OutDebugD3D("%s: surface=%#x\n", ApiRef, *ppRenderTarget);
    return res;
}

ULONG WINAPI extBeginScene8(void *lpdd3dd) {
    ULONG res;
    ApiName("IDirect3DDevice8::BeginScene");
    OutDebugD3D("%s: d3dd=%#x\n", ApiRef, lpdd3dd);
    res = (*pBeginScene8)(lpdd3dd);
    _if (res) OutTraceE("%s: ERROR err=%#x\n", ApiRef, res);
    //if(dxw.dwFlags5 & TEXTURETRANSP) (*pSetRenderState8)(lpdd3dd, D3DRS_ALPHABLENDENABLE, TRUE);
    return res;
}

ULONG WINAPI extBeginScene9(void *lpdd3dd) {
    ULONG res;
    ApiName("IDirect3DDevice9::BeginScene");
    OutDebugD3D("%s: d3dd=%#x\n", ApiRef, lpdd3dd);
    res = (*pBeginScene9)(lpdd3dd);
    _if (res) OutTraceE("%s: ERROR err=%#x\n", ApiRef, res);
    //if(dxw.dwFlags5 & TEXTURETRANSP) (*pSetRenderState9)(lpdd3dd, D3DRS_ALPHABLENDENABLE, TRUE);
    return res;
}

ULONG WINAPI extEndScene8(void *lpdd3dd) {
    ULONG res;
    ApiName("IDirect3DDevice8::EndScene");
    OutDebugD3D("%s: d3dd=%#x\n", ApiRef, lpdd3dd);
    res = (*pEndScene8)(lpdd3dd);
    _if (res) OutTraceE("%s: ERROR err=%#x\n", ApiRef, res);
    return res;
}

ULONG WINAPI extEndScene9(void *lpdd3dd) {
    ULONG res;
    ApiName("IDirect3DDevice9::EndScene");
    OutDebugD3D("%s: d3dd=%#x\n", ApiRef, lpdd3dd);
    res = (*pEndScene9)(lpdd3dd);
    _if (res) OutTraceE("%s: ERROR err=%#x\n", ApiRef, res);
    return res;
}

ULONG WINAPI extSetTexture8(void *lpd3dd, DWORD Stage, void *pTexture) {
    ULONG ret;
    ApiName("IDirect3DDevice8::SetTexture");
    if (dxw.dwFlags4 & NOTEXTURES) {
        (*pSetTexture8)(lpd3dd, Stage, NULL);
        OutTraceD3D("%s: d3dd=%#x stage=%#x\n", ApiRef, lpd3dd, Stage);
        return DD_OK;
    }
    ret = (*pSetTexture8)(lpd3dd, Stage, pTexture);
    OutTraceD3D("%s: d3dd=%#x stage=%#x texture=%#x ret=%#x\n", ApiRef, lpd3dd, Stage, pTexture, ret);
    if(pTexture) D3D8TextureHandling((void *)pTexture, Stage);
    return ret;
}

ULONG WINAPI extSetTexture9(void *lpd3dd, DWORD Stage, void *pTexture) {
    ULONG ret;
    ApiName("IDirect3DDevice9::SetTexture");
    if (dxw.dwFlags4 & NOTEXTURES) {
        (*pSetTexture9)(lpd3dd, Stage, NULL);
        OutTraceD3D("%s: d3dd=%#x stage=%#x\n", ApiRef, lpd3dd, Stage);
        return DD_OK;
    }
    ret = (*pSetTexture9)(lpd3dd, Stage, pTexture);
    OutTraceD3D("%s: d3dd=%#x stage=%#x texture=%#x ret=%#x\n", ApiRef, lpd3dd, Stage, pTexture, ret);
    if(pTexture) D3D9TextureHandling((void *)pTexture, (int)Stage);
    return ret;
}

BOOL WINAPI extShowCursor8(void *lpd3dd, BOOL bShow) {
    BOOL res, bNew;
    ApiName("IDirect3DDevice8::ShowCursor");
    bNew = bShow;
    if(dxw.dwFlags2 & SHOWHWCURSOR) bNew = TRUE;
    if(dxw.dwFlags1 & HIDEHWCURSOR) bNew = FALSE;
    res = (*pShowCursor8)(lpd3dd, bNew);
    if(bNew == bShow)
        OutTraceD3D("%s: d3dd=%#x show=%#x res=%#x\n", ApiRef, lpd3dd, bNew, res);
    else
        OutTraceD3D("%s: d3dd=%#x show=%#x->%#x res=%#x\n", ApiRef, lpd3dd, bShow, bNew, res);
    return res;
}

BOOL WINAPI extShowCursor9(void *lpd3dd, BOOL bShow) {
    BOOL res, bNew;
    ApiName("IDirect3DDevice9::ShowCursor");
    bNew = bShow;
    if(dxw.dwFlags2 & SHOWHWCURSOR) bNew = TRUE;
    if(dxw.dwFlags1 & HIDEHWCURSOR) bNew = FALSE;
    res = (*pShowCursor9)(lpd3dd, bNew);
    if(bNew == bShow)
        OutTraceD3D("%s: d3dd=%#x show=%#x res=%#x\n", ApiRef, lpd3dd, bNew, res);
    else
        OutTraceD3D("%s: d3dd=%#x show=%#x->%#x res=%#x\n", ApiRef, lpd3dd, bShow, bNew, res);
    return res;
}

void WINAPI voidDebugSetLevel(void) {
    OutTraceD3D("DebugSetLevel: SUPPRESS\n");
}

void WINAPI voidDebugSetMute(void) {
    OutTraceD3D("DebugSetMute: SUPPRESS\n");
}

BOOL  WINAPI voidDisableD3DSpy(void) {
    OutTraceD3D("DisableD3DSpy: SUPPRESS\n");
    return FALSE;
}

HRESULT WINAPI voidDirect3DShaderValidatorCreate9(void) {
    OutTraceD3D("Direct3DShaderValidatorCreate9: SUPPRESS\n");
    return 0;
}

static char *ExplainD3DDeviceType(D3DDEVTYPE DeviceType) {
    char *s;
    switch(DeviceType) {
    case D3DDEVTYPE_HAL:
        s = "HAL";
        break;
    case D3DDEVTYPE_NULLREF:
        s = "NULLREF";
        break;
    case D3DDEVTYPE_REF:
        s = "REF";
        break;
    case D3DDEVTYPE_SW:
        s = "SW";
        break;
    default:
        s = "unknown";
        break;
    }
    return s;
}

static HRESULT WINAPI extCheckDeviceType(int d3dversion, CheckDeviceType_Type pCheckDeviceType, void *lpd3d, UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) {
    HRESULT res;
    ApiName((d3dversion == 9) ? "IDirect3D9::CheckDeviceType" : "IDirect3D8::CheckDeviceType");
    // v2.05.16: fixed log message
    OutTraceD3D("%s: d3d=%#x adapter=%d devtype=%#x(%s) adapterformat=%d(%s) backbufferformat=%d(%s) windowed=%#x\n",
                ApiRef, lpd3d, Adapter,
                DevType, ExplainD3DDeviceType(DevType),
                AdapterFormat, ExplainD3DSurfaceFormat(AdapterFormat),
                BackBufferFormat, ExplainD3DSurfaceFormat(BackBufferFormat),
                bWindowed);
    // v2.05.42: fullscreen adapter handling
    Adapter = SetD3DAdapter(Adapter);
    // v2.04.71: commented out because prevents "Beyond Divinity" to run giving a bad message about not enough texture memory.
    // since in windowed mode the backbuffer type is remapped to current video mode, the check is useless and probably the wrapper
    // should return always OK.
    // if(dxw.Windowize) bWindowed = TRUE;
    res = (*pCheckDeviceType)(lpd3d, Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
    if(res && dxw.Windowize) {
        // "Star Wars the Complete Saga" tentative fix ...
        // if we want to windowize, give it a second chance ...
        res = (*pCheckDeviceType)(lpd3d, Adapter, DevType, AdapterFormat, D3DFMT_UNKNOWN, TRUE);
    }
    OutTraceD3D("%s: res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extCheckDeviceType8(void *lpd3d, UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) {
    return extCheckDeviceType(8, pCheckDeviceType8, lpd3d, Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
}
HRESULT WINAPI extCheckDeviceType9(void *lpd3d, UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) {
    return extCheckDeviceType(9, pCheckDeviceType9, lpd3d, Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
}

HRESULT WINAPI extD3DGetDeviceCaps(int version, void *lpd3d, UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9 *pCaps) {
    HRESULT res;
    ApiName((version == 9) ? "IDirect3D9::GetDeviceCaps" : "IDirect3D8::GetDeviceCaps");
    OutTraceD3D("%s: d3d=%#x adapter=%d devtype=%#x(%s)\n", ApiRef, lpd3d, Adapter, DeviceType, ExplainD3DDeviceType(DeviceType));
    // v2.05.42: fullscreen adapter handling
    Adapter = SetD3DAdapter(Adapter);
    if(version == 8)
        res = (*pD3DGetDeviceCaps8)(lpd3d, Adapter, DeviceType, (D3DCAPS8 *)pCaps);
    else
        res = (*pD3DGetDeviceCaps9)(lpd3d, Adapter, DeviceType, pCaps);
    if(res)
        OutTraceE("%s: ERROR: err=%#x\n", ApiRef, res);
    else {
#ifndef DXW_NOTRACES
        if(IsDebugD3D) {
            OutTrace("%s:\n\t"
                     "DeviceType=%#x(%s)\n\t"
                     "Caps=%#x Caps2=%#x Caps3=%#x\n\t"
                     "PresentationIntervals=%#x\n\t"
                     "CursorCaps=%#x DevCaps=%#x\n\t"
                     "PrimitiveMiscCaps=%#x\n\t"
                     "RasterCaps=%#x\n\t"
                     "ZCmpCaps=%#x\n\t"
                     "SrcBlendCaps=%#x\n\t"
                     "DestBlendCaps=%#x\n\t"
                     "AlphaCmpCaps=%#x\n\t"
                     "ShadeCaps=%#x\n\t"
                     "TextureCaps=%#x\n\t",
                     ApiRef,
                     pCaps->DeviceType, ExplainD3DDeviceType(pCaps->DeviceType),
                     pCaps->Caps, pCaps->Caps2, pCaps->Caps3,
                     pCaps->PresentationIntervals,
                     pCaps->CursorCaps, pCaps->DevCaps,
                     pCaps->PrimitiveMiscCaps,
                     pCaps->RasterCaps,
                     pCaps->ZCmpCaps,
                     pCaps->SrcBlendCaps,
                     pCaps->DestBlendCaps,
                     pCaps->AlphaCmpCaps,
                     pCaps->ShadeCaps,
                     pCaps->TextureCaps);
            OutTrace(
                "TextureFilterCaps=%#x\n\t"
                "CubeTextureFilterCaps=%#x\n\t"
                "VolumeTextureFilterCaps=%#x\n\t"
                "TextureAddressCaps=%#x\n\t"
                "VolumeTextureAddressCaps=%#x\n\t"
                "LineCaps=%#x\n\t"
                "StencilCaps=%#x\n\t"
                "FVFCaps=%#x\n\t"
                "TextureOpCaps=%#x\n\t"
                "VertexProcessingCaps=%#x\n\t",
                pCaps->TextureFilterCaps,
                pCaps->CubeTextureFilterCaps,
                pCaps->VolumeTextureFilterCaps,
                pCaps->TextureAddressCaps,
                pCaps->VolumeTextureAddressCaps,
                pCaps->LineCaps,
                pCaps->StencilCaps,
                pCaps->FVFCaps,
                pCaps->TextureOpCaps,
                pCaps->VertexProcessingCaps);
            OutTrace(
                "MaxTexture(Width x Height)=(%dx%d)\n\t"
                "MaxVolumeExtent=%d\n\t"
                "MaxTextureRepeat=%d\n\t"
                "MaxTextureAspectRatio=%d\n\t"
                "MaxAnisotropy=%d\n\t"
                "MaxActiveLights=%d\n\t"
                "MaxUserClipPlanes=%#x\n\t"
                "MaxUserClipPlanes=%#x\n\t"
                "MaxVertexW=%f\n\t"
                "GuardBandLeft=%f\n\t"
                "GuardBandTop=%f\n\t"
                "GuardBandRight=%f\n\t"
                "GuardBandBottom=%f\n\t"
                "ExtentsAdjust=%f\n\t"
                "MaxPointSize=%f\n\t"
                "MaxTextureBlendStages=%d\n\t"
                "MaxSimultaneousTextures=%#x\n\t"
                "MaxVertexBlendMatrices=%#x\n\t"
                "MaxVertexBlendMatrixIndex=%#x\n\t"
                "MaxPrimitiveCount=%d\n\t"
                "MaxVertexIndex=%#x\n\t"
                "MaxStreams=%#x\n\t"
                "MaxStreamStride=%#x\n\t"
                "VertexShaderVersion=%#x\n\t"
                "MaxVertexShaderConst=%d\n\t"
                "PixelShaderVersion=%#x\n\t"
                "PixelShader1xMaxValue=%f\n",
                pCaps->MaxTextureWidth, pCaps->MaxTextureHeight,
                pCaps->MaxVolumeExtent,
                pCaps->MaxTextureRepeat,
                pCaps->MaxTextureAspectRatio,
                pCaps->MaxAnisotropy,
                pCaps->MaxActiveLights,
                pCaps->MaxUserClipPlanes,
                pCaps->MaxUserClipPlanes,
                pCaps->MaxVertexW,
                pCaps->GuardBandLeft,
                pCaps->GuardBandTop,
                pCaps->GuardBandRight,
                pCaps->GuardBandBottom,
                pCaps->ExtentsAdjust,
                pCaps->MaxPointSize,
                pCaps->MaxTextureBlendStages,
                pCaps->MaxSimultaneousTextures,
                pCaps->MaxVertexBlendMatrices,
                pCaps->MaxVertexBlendMatrixIndex,
                pCaps->MaxPrimitiveCount,
                pCaps->MaxVertexIndex,
                pCaps->MaxStreams,
                pCaps->MaxStreamStride,
                pCaps->VertexShaderVersion,
                pCaps->MaxVertexShaderConst,
                pCaps->PixelShaderVersion,
                pCaps->PixelShader1xMaxValue);
            if(version == 9) OutTrace(
                    "\tDevCaps2=%#x Reserved5=%#x\n\t"
                    "MaxNpatchTessellationLevel=%f\n\t"
                    "MasterAdapterOrdinal=%i\n\t"
                    "AdapterOrdinalInGroup=%i\n\t"
                    "NumberOfAdaptersInGroup=%i\n\t"
                    "DeclTypes=%#x\n\t"
                    "NumSimultaneousRTs=%#x\n\t"
                    "StretchRectFilterCaps=%#x\n\t"
                    "VertexTextureFilterCaps=%#x\n\t"
                    "MaxVShaderInstructionsExecuted=%#x\n\t"
                    "MaxPShaderInstructionsExecuted=%#x\n\t"
                    "MaxVertexShader30InstructionSlots=%#x\n\t"
                    "MaxPixelShader30InstructionSlots=%#x\n",
                    pCaps->DevCaps2, pCaps->Reserved5,
                    pCaps->MaxNpatchTessellationLevel,
                    pCaps->MasterAdapterOrdinal,
                    pCaps->AdapterOrdinalInGroup,
                    pCaps->NumberOfAdaptersInGroup,
                    pCaps->DeclTypes,
                    pCaps->NumSimultaneousRTs,
                    pCaps->StretchRectFilterCaps,
                    pCaps->VertexTextureFilterCaps,
                    pCaps->MaxVShaderInstructionsExecuted,
                    pCaps->MaxPShaderInstructionsExecuted,
                    pCaps->MaxVertexShader30InstructionSlots,
                    pCaps->MaxPixelShader30InstructionSlots);
        }
#endif // DXW_NOTRACES
    }
    if(dxw.dwFlags7 & SUPPRESSOVERLAY) {
        _if(pCaps->Caps & D3DCAPS_OVERLAY) OutTraceDW("%s: SUPPRESS OVERLAY CAP\n", ApiRef);
        pCaps->Caps &= ~D3DCAPS_OVERLAY;
    }
    if(dxw.dwFlags11 & TRANSFORMANDLIGHT) {
        _if(!(pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT))
        OutTraceDW("%s: PRETEND HWTRANSFORMANDLIGHT CAP\n", ApiRef);
        pCaps->DevCaps |= D3DDEVCAPS_HWTRANSFORMANDLIGHT;
    }
    return res;
}

HRESULT WINAPI extD3DGetDeviceCaps8(void *lpd3d, UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS8 *pCaps) {
    return extD3DGetDeviceCaps(8, lpd3d, Adapter, DeviceType, (D3DCAPS9 *)pCaps);
}
HRESULT WINAPI extD3DGetDeviceCaps9(void *lpd3d, UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9 *pCaps) {
    return extD3DGetDeviceCaps(9, lpd3d, Adapter, DeviceType, pCaps);
}

static HMONITOR WINAPI extGetAdapterMonitor(int d3dversion, GetAdapterMonitor_Type pGetAdapterMonitor, void *lpd3d, UINT Adapter) {
    ApiName((d3dversion == 9) ? "IDirect3D9::GetAdapterMonitor" : "IDirect3D8::GetAdapterMonitor");
    // to do: Adapter handling here ...
    OutTraceD3D("%s: d3d=%#x adapter=%d\n", ApiRef, lpd3d, Adapter);
    if((dxw.dwFlags2 & HIDEMULTIMONITOR) && (Adapter != D3DADAPTER_DEFAULT)) {
        OutTraceD3D("%s: HIDE adapter=%d\n", ApiRef, Adapter);
        return NULL;
    }
    return (*pGetAdapterMonitor)(lpd3d, Adapter);
}

HMONITOR WINAPI extGetAdapterMonitor8(void *lpd3d, UINT Adapter) {
    return extGetAdapterMonitor(8, pGetAdapterMonitor8, lpd3d, Adapter);
}
HMONITOR WINAPI extGetAdapterMonitor9(void *lpd3d, UINT Adapter) {
    return extGetAdapterMonitor(9, pGetAdapterMonitor9, lpd3d, Adapter);
}


static UINT WINAPI extGetAvailableTextureMem(int d3dversion, GetAvailableTextureMem_Type pGetAvailableTextureMem, void *lpd3dd) {
    ApiName((d3dversion == 9) ? "IDirect3DDevice9::GetAvailableTextureMem" : "IDirect3DDevice9::GetAvailableTextureMem");
    const UINT TextureMemoryLimit = 1024 * 1024 * 1024; // 1GB
    // const DWORD dwMaxMem = 0x70000000; = 1.8G
    UINT AvailableTextureMem = (*pGetAvailableTextureMem)(lpd3dd);
    OutTraceD3D("%s: lpd3dd=%#x AvailableTextureMem=%u(%dMB)\n", ApiRef, lpd3dd, AvailableTextureMem, AvailableTextureMem >> 20);
    if(dxw.bHintActive && (AvailableTextureMem > TextureMemoryLimit)) ShowHint(HINT_LIMITMEM);
    if((dxw.dwFlags2 & LIMITRESOURCES) && (AvailableTextureMem > TextureMemoryLimit)) {
        OutTraceDW("%s: LIMIT AvailableTextureMem=%u->%u\n", ApiRef, AvailableTextureMem, TextureMemoryLimit);
        AvailableTextureMem = TextureMemoryLimit;
    }
    if(dxw.dwDFlags & STRESSRESOURCES) AvailableTextureMem = 0xFFFFFFFF;
    return AvailableTextureMem;
}

UINT WINAPI extGetAvailableTextureMem8(void *lpd3dd) {
    return extGetAvailableTextureMem(8, pGetAvailableTextureMem8, lpd3dd);
}
UINT WINAPI extGetAvailableTextureMem9(void *lpd3dd) {
    return extGetAvailableTextureMem(9, pGetAvailableTextureMem9, lpd3dd);
}

static HRESULT WINAPI extTestCooperativeLevel(int d3dversion, TestCooperativeLevel_Type pTestCooperativeLevel, void *lpd3dd) {
    HRESULT res;
    ApiName((d3dversion == 9) ? "IDirect3DDevice9::TestCooperativeLevel" : "IDirect3DDevice8::TestCooperativeLevel");
    res = (*pTestCooperativeLevel)(lpd3dd);
    OutDebugD3D("%s: d3dd=%#x res=%#x\n", ApiRef, lpd3dd, res);
    return res;
}

HRESULT WINAPI extTestCooperativeLevel8(void *lpd3dd) {
    return extTestCooperativeLevel(8, pTestCooperativeLevel8, lpd3dd);
}
HRESULT WINAPI extTestCooperativeLevel9(void *lpd3dd) {
    return extTestCooperativeLevel(9, pTestCooperativeLevel9, lpd3dd);
}

HRESULT WINAPI extGetSwapChain(void *lpd3dd, UINT iSwapChain, IDirect3DSwapChain9 **pSwapChain) {
    HRESULT res;
    ApiName("IDirect3DDevice9::GetSwapChain");
    OutTraceD3D("%s: d3dd=%#x iSwapChain=%d\n", ApiRef, lpd3dd, iSwapChain);
    res = (*pGetSwapChain)(lpd3dd, iSwapChain, pSwapChain);
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else
        OutTraceD3D("%s: pSwapChain=%#x\n", ApiRef, *pSwapChain);
    HookSwapChain9(*pSwapChain);
    return res;
}

UINT WINAPI extGetNumberOfSwapChains(void *lpd3dd) {
    UINT res;
    ApiName("IDirect3DDevice9::GetNumberOfSwapChains");
    res = (*pGetNumberOfSwapChains)(lpd3dd);
    OutTraceD3D("%s: d3dd=%#x res=%d\n", ApiRef, lpd3dd, res);
    return res;
}

HRESULT WINAPI extBeginStateBlock8(void *lpd3dd) {
    HRESULT res;
    ApiName("IDirect3DDevice8::BeginStateBlock");
    OutTraceD3D("%s: d3dd=%#x\n", ApiRef, lpd3dd);
    res = (*pBeginStateBlock8)(lpd3dd);
    HookD3DDevice8(&lpd3dd);
    return res;
}

HRESULT WINAPI extBeginStateBlock9(void *lpd3dd) {
    // v2.02.78: D3DDevice9::BeginStateBlock refreshes the COM D3DDevice9 method pointers, so that
    // you need to hook the device object again. This operation fixes the switch to fullscreen mode
    // in "Freedom Force vs. the Third Reich".
    HRESULT res;
    ApiName("IDirect3DDevice9::BeginStateBlock");
    OutTraceD3D("%s: d3dd=%#x\n", ApiRef, lpd3dd);
    res = (*pBeginStateBlock9)(lpd3dd);
    HookD3DDevice9(&lpd3dd);
    return res;
}

HRESULT WINAPI extEndStateBlock8(void *lpd3dd, DWORD *pToken) {
    HRESULT res;
    ApiName("IDirect3DDevice8::EndStateBlock");
    OutTraceD3D("%s: d3dd=%#x\n", ApiRef, lpd3dd);
    res = (*pEndStateBlock8)(lpd3dd, pToken);
    return res;
}

HRESULT WINAPI extEndStateBlock9(void *lpd3dd, IDirect3DStateBlock9 **ppSB) {
    HRESULT res;
    ApiName("IDirect3DDevice9::EndStateBlock");
    OutTraceD3D("%s: d3dd=%#x\n", ApiRef, lpd3dd);
    res = (*pEndStateBlock9)(lpd3dd, ppSB);
    return res;
}

HRESULT WINAPI extCreateTexture8(void *lpd3dd, UINT Width, UINT Height, UINT Levels,
                                 DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                 void **ppTexture) {
    HRESULT res;
    ApiName("IDirect3DDevice8::CreateTexture");
    OutTraceD3D("%s: lpd3dd=%#x size=(%dx%d) levels=%d usage=%#x format=%d(%s) pool=%#x\n",
                ApiRef, lpd3dd, Width, Height, Levels, Usage, Format, ExplainD3DSurfaceFormat(Format), Pool);
    res = (*pCreateTexture8)(lpd3dd, Width, Height, Levels, Usage, Format, Pool, ppTexture);
    if (res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else {
        OutTraceD3D("%s: lpTexture=%#x\n", ApiRef, *ppTexture);
        HookD3DTexture8(ppTexture);
    }
    return res;
}

HRESULT WINAPI extCreateTexture9(void *lpd3dd, UINT Width, UINT Height, UINT Levels,
                                 DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                 void **ppTexture, HANDLE *pSharedHandle) {
    HRESULT res;
    ApiName("IDirect3DDevice9::CreateTexture");
    OutTraceD3D("%s: lpd3dd=%#x size=(%dx%d) levels=%d usage=%#x format=%d(%s) pool=%#x\n",
                ApiRef, lpd3dd, Width, Height, Levels, Usage, Format, ExplainD3DSurfaceFormat(Format), Pool);
    res = (*pCreateTexture9)(lpd3dd, Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
    if (res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else {
        OutTraceD3D("%s: lpTexture=%#x\n", ApiRef, *ppTexture);
        HookD3DTexture9(ppTexture);
    }
    return res;
}

static HRESULT WINAPI extLockRect(LockRect_Type pLockRect, void *lpd3dtex, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags) {
    HRESULT res;
#ifndef DXW_NOTRACES
    if(IsTraceD3D) {
        char sRect[81];
        if(pRect)
            sprintf_s(sRect, 80, "(%d,%d)-(%d,%d)", pRect->left, pRect->top, pRect->right, pRect->bottom);
        else
            strcpy_s(sRect, 80, "NULL");
        OutTrace("Texture::LockRect: lpd3dtex=%#x level=%d rect=%s flags=%#x\n", lpd3dtex, Level, sRect, Flags);
    }
#endif // DXW_NOTRACES
    res = (*pLockRect)(lpd3dtex, Level, pLockedRect, pRect, Flags);
    if (res) OutTraceE("Device::LockRect ERROR: err=%#x\n", res);
    else
        OutTraceD3D("Texture::LockRect: pBits=%#x pitch=%d\n", pLockedRect->pBits, pLockedRect->Pitch);
    return res;
}

HRESULT WINAPI extLockRect8(void *lpd3dtex, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags) {
    return extLockRect(pLockRect8, lpd3dtex, Level, pLockedRect, pRect, Flags);
}
HRESULT WINAPI extLockRect9(void *lpd3dtex, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags) {
    return extLockRect(pLockRect9, lpd3dtex, Level, pLockedRect, pRect, Flags);
}

typedef void (*TextureHandling_Type)(void *, int);

static HRESULT WINAPI extUnlockRect(UnlockRect_Type pUnlockRect, void *lpd3dtex, UINT Level, TextureHandling_Type TextureHandling) {
    HRESULT res;
    OutTraceD3D("Texture::UnlockRect: lpd3dtex=%#x level=%d\n", lpd3dtex, Level);
    res = (*pUnlockRect)(lpd3dtex, Level);
    if (res) OutTraceE("Texture::UnlockRect ERROR: err=%#x\n", res);
    else {
        if(TextureHandling) TextureHandling(lpd3dtex, Level);
        OutTraceD3D("Texture::UnlockRect: D3D_OK\n");
    }
    return res;
}

HRESULT WINAPI extUnlockRect8(void *lpd3dtex, UINT Level) {
    return extUnlockRect(pUnlockRect8, lpd3dtex, Level, (dxw.dwFlags5 & TEXTUREMASK) ? D3D8TextureHandling : NULL);
}
HRESULT WINAPI extUnlockRect9(void *lpd3dtex, UINT Level) {
    return extUnlockRect(pUnlockRect9, lpd3dtex, Level, (dxw.dwFlags5 & TEXTUREMASK) ? D3D9TextureHandling : NULL);
}

void WINAPI extD3DPERF_SetOptions(DWORD dwOptions) {
    _if(dwOptions) OutTraceD3D("circumvent D3DPERF_SetOptions\n");
    pD3DPERF_SetOptions(0);
}

void RestoreD3DSurfaces(BOOL bFullScreen) {
    //DWORD param[64];
    //HRESULT res;
    // a well programmed D3D game should provide to Reset by itself, so the best thing to do is NOTHING
    return;
    //if(lpD3DActiveDevice){
    //	OutTrace("RestoreD3DSurfaces(%d): reset device %#x fullscreen=%#x\n", dwD3DVersion, lpD3DActiveDevice, bFullScreen);
    //	RECT Desktop;
    //	memcpy(param, gActiveDeviceParams, sizeof(param));
    //	if(bFullScreen){
    //		(*pGetClientRect)(0, &Desktop);
    //		param[0] = Desktop.right;			// BackBufferWidth
    //		param[1] = Desktop.bottom;			// BackBufferHeight
    //	}
    //	res=(*pReset)(lpD3DActiveDevice, (D3DPRESENT_PARAMETERS *)param);
    //	if(res != DD_OK) OutTraceE("RestoreD3DSurfaces: Reset size=(%dx%d) err=%#x(%s)\n",
    //		param[0], param[1], res, ExplainDDError(res));
    //}
}

// v2.05.05: fully revised extGetAdapterModeCount8/9 behavior

static UINT WINAPI GetEmulatedModeCount() {
    SupportedRes_Type *pTypes = SupportedRes;
    UINT Modes = 0;
    for(SupportedRes_Type *pTypes0 = pTypes; pTypes0->w; pTypes0++) Modes++; // count how many are there ...
    return Modes;
}

UINT WINAPI extGetAdapterModeCount8(void *lpd3d, UINT Adapter) {
    UINT ret;
    ApiName("IDirect3D8::GetAdapterModeCount");
    OutTraceD3D("%s: d3d=%#x adapter=%d\n", ApiRef, lpd3d, Adapter);
    Adapter = SetD3DAdapter(Adapter); // v2.05.47 fix
    ret = (*pGetAdapterModeCount8)(lpd3d, Adapter);
    if(dxw.dwFlags4 & (SUPPORTSVGA | SUPPORTHDTV)) {
        if(ret) {
            // this should be valid on Win8/10 only, WinXP/7 are able to provide modes for
            // multiple color depths ....
            ret = GetEmulatedModeCount();
            if(dxw.dwFlags7 & ENUM16BITMODES) ret *= 2;
            OutTraceDW("%s: fake emulated mode count=%d\n", ApiRef, ret);
        }
    }
    OutTraceD3D("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

UINT WINAPI extGetAdapterModeCount9(void *lpd3d, UINT Adapter, D3DFORMAT Format) {
    UINT ret;
    ApiName("IDirect3D9::GetAdapterModeCount");
    OutTraceD3D("%s: d3d=%#x adapter=%d format=%d(%s)\n",
                ApiRef, lpd3d, Adapter, Format, ExplainD3DSurfaceFormat(Format));
    Adapter = SetD3DAdapter(Adapter); // v2.05.47 fix
    ret = (*pGetAdapterModeCount9)(lpd3d, Adapter, Format);
    if(dxw.dwFlags4 & (SUPPORTSVGA | SUPPORTHDTV)) {
        // in D3D9 emulated modes (SVGA/HDTV) return the number of emulated modes only if the
        // pixel format is supported (that is, ret != 0), unless it is 0 but we have the 16bit
        // emulation turned on. Here, there's no handling of "All" (real + emulated) modes.
        if(ret) {
            ret = GetEmulatedModeCount();
            OutTraceDW("%s: fake emulated mode count=%d\n", ApiRef, ret);
        }
        if(!ret && (dxw.dwFlags7 & ENUM16BITMODES)) {
            switch(Format) {
            case D3DFMT_R5G6B5:
            case D3DFMT_X1R5G5B5:
            case D3DFMT_A1R5G5B5:
                ret = GetEmulatedModeCount();
                OutTraceDW("%s: fake 16BPP mode count=%d\n", ApiRef, ret);
                break;
            }
        }
    }
    OutTraceD3D("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

HRESULT WINAPI extGetFrontBuffer(void *lpd3dd, LPDIRECTDRAWSURFACE pDestSurface) {
    HRESULT res;
    ApiName("IDirect3DDevice8::GetFrontBuffer");
    OutTraceD3D("%s: d3d=%#x dest=%#x\n", ApiRef, lpd3dd, pDestSurface);
    res = (*pGetFrontBuffer)(lpd3dd, pDestSurface);
    // v2.04.02: Fix for "Dirt Track Racing 2": GetFrontBuffer in windowed mode through DxWnd intervention may yterminate in error
    // since the Front Buffer size is the whole screen and the surface provided by the application only maps the virtual coordinates.
    // So, the dxGetFrontBuffer8() routine tries to match the surfaces, but in any case, also if an error happens, better return a
    // fake D3D_OK condition.
    if(res == D3DERR_INVALIDCALL) {
        res = dxGetFrontBuffer8(lpd3dd, pDestSurface);
        _if(res) OutTraceDW("%s: ret=%#x return FAKE D3D_OK\n", ApiRef, res);
        res = D3D_OK;
    }
    OutTraceD3D("%s: ret=%#x\n", ApiRef, res);
    return res;
}

HRESULT WINAPI extCopyRects(void *lpd3dd, LPDIRECTDRAWSURFACE pSourceSurface, CONST RECT *pSourceRectsArray, UINT cRects, LPDIRECTDRAWSURFACE pDestinationSurface, CONST POINT *pDestPointsArray) {
    HRESULT res;
    ApiName("IDirect3DDevice8::CopyRects");
    OutTraceD3D("%s: d3d=%#x source=%#x dest=%#x rects=%d\n", ApiRef, lpd3dd, pSourceSurface, pDestinationSurface, cRects);
    // v2.04.02: Fix for "Dirt Track Racing 2": in early D3D8 implementation it seems that invoking this Copyrects method for 0 rects count didn't hurt,
    // but now the method returns an error.
    // added a patch for handling the "if(cRects==0)" case - it is not worth a flag, it seems good for all circumstances, isn't it?
    // Beware: it seems (from some code snippet) that lpDevice->CopyRects(lpSrcSurface, NULL, 0, lpDestSurface, NULL) means copy the whole lpSrcSurface
    // to lpDestSurface. In windowized mode the peration may fail because of the different sizing!
    res = (*pCopyRects)(lpd3dd, pSourceSurface, pSourceRectsArray, cRects, pDestinationSurface, pDestPointsArray);
    if((res == D3DERR_INVALIDCALL) && (cRects == 0)) {
        res = dxCopyRects(lpd3dd, pSourceSurface, pDestinationSurface);
        _if(res) OutTraceDW("%s: res=%#x FAKE D3D_OK on 0 rects\n", ApiRef, res);
        res = D3D_OK;
    }
    OutTraceD3D("%s: ret=%#x\n", ApiRef, res);
#ifndef DXW_NOTRACES
    if((dxw.dwDFlags & DUMPSURFACES) && dxw.bCustomKeyToggle) SurfaceDump(pDestinationSurface, 8);
    if((dxw.dwDFlags2 & DUMPBLITSRC) && dxw.bCustomKeyToggle) SurfaceDump(pSourceSurface, 8);
#endif // DXW_NOTRACES
    return res;
}

//    STDMETHOD(GetBackBuffer)(THIS_ UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer) PURE;

HRESULT WINAPI extGetBackBuffer8(void *lpd3dd, UINT BackBuffer, D3DBACKBUFFER_TYPE Type, LPDIRECTDRAWSURFACE *ppBackBuffer) {
    HRESULT res;
    ApiName("IDirect3DDevice8::GetBackBuffer");
    OutTraceD3D("%s: d3d=%#x backbuffer=%#x type=%#x\n", ApiRef, lpd3dd, BackBuffer, Type);
    if((dxw.dwFlags8 & D3D8BACK16) && (dwD3DVersion == 8) && (dxw.VirtualPixelFormat.dwRGBBitCount == 16)) {
        if ((BackBuffer != 0) || (Type != D3DBACKBUFFER_TYPE_MONO))
            return D3DERR_INVALIDCALL;
        *ppBackBuffer = dwGetVirtualBackBuffer();
        (*ppBackBuffer)->AddRef();
        OutTraceD3D("%s: EMULATION OK\n", ApiRef);
#ifndef DXW_NOTRACES
        if((dxw.dwDFlags & DUMPSURFACES) && dxw.bCustomKeyToggle) SurfaceDump(*ppBackBuffer, 8);
#endif // DXW_NOTRACES
        return D3D_OK;
    }
    res = (*pGetBackBuffer8)(lpd3dd, BackBuffer, Type, ppBackBuffer);
    _if(res) OutTraceE("%s: ret=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
#ifndef DXW_NOTRACES
    if((dxw.dwDFlags & DUMPSURFACES) && dxw.bCustomKeyToggle) SurfaceDump(*ppBackBuffer, 8);
#endif // DXW_NOTRACES
    return res;
}

HRESULT WINAPI extGetFrontBufferData9(void *lpd3dd, UINT iSwapChain, LPDIRECTDRAWSURFACE pDestSurface) {
    HRESULT res;
    ApiName("IDirect3DDevice9::GetFrontBufferData");
    OutTraceD3D("%s: d3d=%#x swapchain=%d destsurface=%#x\n", ApiRef, lpd3dd, iSwapChain, pDestSurface);
    res = (*pGetFrontBufferData9)(lpd3dd, iSwapChain, pDestSurface);
    // v2.04.27: Fix for "Hitchhiker's Guide to the Galaxy remake": D3D9 GetFrontBufferData in windowed mode through DxWnd intervention
    // may yterminate in error since the Front Buffer size is the whole screen and the surface provided by the application only maps the virtual coordinates.
    // Same situation as "Dirt Track Racing 2" with D3D8 GetFrontBuffer, same solution.
    // W.I.P. is not working ok if window size is set to 0, 0.
    if(res == D3DERR_INVALIDCALL) {
        OutTraceD3D("%s: processing D3DERR_INVALIDCALL\n", ApiRef);
        res = dxGetFrontBuffer9(lpd3dd, iSwapChain, (LPDIRECTDRAWSURFACE)pDestSurface);
        if(res && (dxw.dwFlags1 & SUPPRESSDXERRORS)) {
            OutTraceDW("%s: ret=%#x(%s) return FAKE D3D_OK\n", ApiRef, res, ExplainDDError(res));
            res = D3D_OK;
        }
    }
    OutTraceD3D("%s: ret=%#x\n", ApiRef, res);
    return res;
}

void AdjustLight(D3DCOLORVALUE *light, BYTE *Gamma) {
    int idx;
    idx = (int) (256.0 * light->r);
    if (idx < 0) idx = 0;
    if (idx > 255) idx = 255;
    light->r = (float)Gamma[idx] / (float)255.0;
    idx = (int) (256.0 * light->g);
    if (idx < 0) idx = 0;
    if (idx > 255) idx = 255;
    light->g = (float)Gamma[idx + 256] / (float)255.0;
    idx = (int) (256.0 * light->b);
    if (idx < 0) idx = 0;
    if (idx > 255) idx = 255;
    light->b = (float)Gamma[idx + 512] / (float)255.0;
}

HRESULT WINAPI extSetLight(void *lpd3dd, DWORD Index, CONST D3DLIGHT9 *light) {
    HRESULT res;
    OutTraceD3D("SetLight: d3d=%#x index=%d light=%#x\n", lpd3dd, Index, light);
#ifndef DXW_NOTRACES
    if(IsDebugD3D) {
        OutTrace("> diffuse={%f,%f,%f,%f}\n", light->Diffuse.r, light->Diffuse.g, light->Diffuse.b, light->Diffuse.a);
        OutTrace("> specular={%f,%f,%f,%f}\n", light->Specular.r, light->Specular.g, light->Specular.b, light->Specular.a);
        OutTrace("> ambient={%f,%f,%f,%f}\n", light->Ambient.r, light->Ambient.g, light->Ambient.b, light->Ambient.a);
        OutTrace("> attenuation={%f,%f,%f}\n", light->Attenuation0, light->Attenuation1, light->Attenuation2);
    }
#endif // DXW_NOTRACES
    if(GetHookInfo()->GammaControl) {
        BYTE *Gamma;
        D3DLIGHT9 newlight = *light;
        Gamma = GetHookInfo()->GammaRamp;
        AdjustLight(&newlight.Diffuse, Gamma);
        AdjustLight(&newlight.Specular, Gamma);
        AdjustLight(&newlight.Ambient, Gamma);
        res = (*pSetLight)(lpd3dd, Index, &newlight);
    } else
        res = (*pSetLight)(lpd3dd, Index, light);
    _if(res) OutTraceE("SetLight: ret=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extGetLight(void *lpd3dd, DWORD Index, D3DLIGHT9 *light) {
    HRESULT res;
    OutTraceD3D("GetLight: d3d=%#x index=%d\n", lpd3dd, Index);
    res = (*pGetLight)(lpd3dd, Index, light);
    _if(res) OutTraceE("GetLight: ret=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

#ifdef TRACEALL
HRESULT WINAPI extGetDepthStencilSurface9(void *lpd3dd, IDirect3DSurface9 **ppZStencilSurface) {
    HRESULT res;
    OutTraceD3D("GetDepthStencilSurface: d3d=%#x\n", lpd3dd);
    res = (*pGetDepthStencilSurface9)(lpd3dd, ppZStencilSurface);
    if(res)
        OutTraceE("GetDepthStencilSurface: ret=%#x(%s)\n", res, ExplainDDError(res));
    else
        OutTraceD3D("GetDepthStencilSurface: lpdds=%#x\n", *ppZStencilSurface);
    return res;
}

HRESULT WINAPI extSetDepthStencilSurface9(void *lpd3dd, IDirect3DSurface9 *pZNewStencil) {
    HRESULT res;
    OutTraceD3D("SetDepthStencilSurface: d3d=%#x stencil=%#x\n", lpd3dd, pZNewStencil);
    res = (*pSetDepthStencilSurface9)(lpd3dd, pZNewStencil);
    if(res)
        OutTraceE("SetDepthStencilSurface: ret=%#x(%s)\n", res, ExplainDDError(res));
    else
        OutTraceD3D("SetDepthStencilSurface: OK\n");
    return res;
}

HRESULT WINAPI extGetDepthStencilSurface8(void *lpd3dd, void **ppZStencilSurface) {
    HRESULT res;
    OutTraceD3D("GetDepthStencilSurface: d3d=%#x\n", lpd3dd);
    res = (*pGetDepthStencilSurface8)(lpd3dd, ppZStencilSurface);
    if(res)
        OutTraceE("GetDepthStencilSurface: ret=%#x(%s)\n", res, ExplainDDError(res));
    else
        OutTraceD3D("GetDepthStencilSurface: lpdds=%#x\n", *ppZStencilSurface);
    return res;
}

HRESULT WINAPI extCreateDepthStencilSurface8(void *lpd3dd, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, void **ppSurface) {
    HRESULT res;
    OutTraceD3D("CreateDepthStencilSurface: d3d=%#x size=(%dx%d) fomat=%#x msample=%#x\n", lpd3dd, Width, Height, Format, MultiSample);
    res = (*pCreateDepthStencilSurface8)(lpd3dd, Width, Height, Format, MultiSample, ppSurface);
    if(res)
        OutTraceE("CreateDepthStencilSurface: ret=%#x(%s)\n", res, ExplainDDError(res));
    else
        OutTraceD3D("CreateDepthStencilSurface: lpdds=%#x\n", *ppSurface);
    return res;
}

HRESULT WINAPI extClear8(void *lpd3ddClear, DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
    HRESULT res;
    OutTraceD3D("Clear: d3d=%#x count=%d flags=%#x color=%#x Z=%f stencil=%#x\n", lpd3ddClear, Count, Flags, Color, Z, Stencil);
    res = (*pClear8)(lpd3ddClear, Count, pRects, Flags, Color, Z, Stencil);
    if(res) OutTraceE("Clear: ret=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extGetLevelDesc8(void *lpd3ds, UINT Level, D3DSURFACE_DESC *pDesc) {
    HRESULT res;
    OutTraceD3D("GetLevelDesc: lpd3ds=%#x level=%d\n", lpd3ds, Level);
    res = (*pGetLevelDesc8)(lpd3ds, Level, pDesc);
    if(res)
        OutTraceE("GetLevelDesc: ret=%#x(%s)\n", res, ExplainDDError(res));
    else {
        OutTraceD3D("GetLevelDesc: desc={format=%#x type=%d usage=%#x pool=%d mstype=%#x msquality=%#x size=(%dx%d)}\n",
                    pDesc->Format,
                    pDesc->Type,
                    pDesc->Usage,
                    pDesc->Pool,
                    pDesc->MultiSampleType,
                    pDesc->MultiSampleQuality,
                    pDesc->Width, pDesc->Height
                   );
    }
    return res;
}

HRESULT WINAPI extGetSurfaceLevel8(void *lpd3ds, UINT Level, void **ppSurfaceLevel) {
    HRESULT res;
    OutTraceD3D("GetSurfaceLevel: lpd3ds=%#x level=%d\n", lpd3ds, Level);
    res = (*pGetSurfaceLevel8)(lpd3ds, Level, ppSurfaceLevel);
    if(res)
        OutTraceE("GetSurfaceLevel: ret=%#x(%s)\n", res, ExplainDDError(res));
    else {
        OutTraceD3D("GetSurfaceLevel: lpd3ds=%#x\n", *ppSurfaceLevel);
#ifndef DXW_NOTRACES
        if((dxw.dwDFlags & DUMPSURFACES) && dxw.bCustomKeyToggle) SurfaceDump((LPDIRECTDRAWSURFACE)*ppSurfaceLevel, 8);
#endif // DXW_NOTRACES
    }
    return res;
}

//HRESULT WINAPI extCreateDepthStencilSurface9(void *lpd3dd, UINT Width, UINT Height, D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, void **ppSurface, HANDLE* pSharedHandle)
//{
//}
#endif

//#define D3DPRESENT_INTERVAL_DEFAULT     0x00000000L
//#define D3DPRESENT_INTERVAL_ONE         0x00000001L
//#define D3DPRESENT_INTERVAL_TWO         0x00000002L
//#define D3DPRESENT_INTERVAL_THREE       0x00000004L
//#define D3DPRESENT_INTERVAL_FOUR        0x00000008L
//#define D3DPRESENT_INTERVAL_IMMEDIATE   0x80000000L

static char *sSwCPresentFlags(DWORD flags) {
    char *s;
    switch(flags) {
    case 0:
        s = "DEFAULT";
        break;
    case D3DPRESENT_DONOTWAIT:
        s = "DONOTWAIT";
        break;
    case D3DPRESENT_LINEAR_CONTENT:
        s = "LINEAR_CONTENT";
        break;
    case D3DPRESENT_LINEAR_CONTENT|D3DPRESENT_DONOTWAIT:
        s = "DONOTWAIT+LINEAR_CONTENT";
        break;
    default:
        s = "invalid";
        break;
    }
    return s;
}

HRESULT WINAPI extPresentSwC8(void *pd3dd, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion) {
    HRESULT res;
    ApiName("IDirect3DSwapChain8::Present");
    RECT RemappedSrcRect, RemappedDstRect;
    BOOL bEmulation = FALSE;
    int d3dversion = 8; // to fix ...
    if(IsDebugD3D) {
        char sSourceRect[81];
        char sDestRect[81];
        if (pSourceRect) sprintf_s(sSourceRect, 80, "(%d,%d)-(%d,%d)", pSourceRect->left, pSourceRect->top, pSourceRect->right, pSourceRect->bottom);
        else strcpy(sSourceRect, "(NULL)");
        if (pDestRect) sprintf_s(sDestRect, 80, "(%d,%d)-(%d,%d)", pDestRect->left, pDestRect->top, pDestRect->right, pDestRect->bottom);
        else strcpy(sDestRect, "(NULL)");
        OutDebugD3D("%s: d3dd=%#x SourceRect=%s DestRect=%s hDestWndOverride=%#x dirtyregion=%#x\n",
                    ApiRef, pd3dd, sSourceRect, sDestRect, hDestWindowOverride, pDirtyRegion);
    }
    // frame counter handling....
    if (dxw.HandleFPS()) return D3D_OK; // beware: there's no flags here!!!
    if (dxw.dwFlags1 & SAVELOAD) dxw.VSyncWait();
    if (dxw.dwFlags5 & MESSAGEPUMP) dxw.MessagePump();
    if (gFlippedToScreen) {
        OutTraceD3D("FilledToScreen SKIP\n");
        gFlippedToScreen--;
        return D3D_OK;
    }
    // ????
    if((dxw.dwFlags8 & D3D8BACK16) && (dxw.VirtualPixelFormat.dwRGBBitCount == 16)) {
        res = dwD3D8Present(pd3dd, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
        OutTraceD3D("%s: EMULATION res=%#x\n", ApiRef, res);
        bEmulation = TRUE;
    }
    if(dxw.dwDFlags & FULLRECTBLT) pSourceRect = pDestRect = NULL;
    if(dxw.Windowize) {
        // v2.03.15 - fix target RECT region
        // v2.04.37 - target scaling must be done only when pDestRect is not NULL. Fixes "Fastlane Pinball" black screen.
        if(pDestRect) {
            RemappedDstRect = dxw.MapClientRect((LPRECT)pDestRect);
            pDestRect = &RemappedDstRect;
            OutDebugD3D("%s: FIXED DestRect=(%d,%d)-(%d,%d)\n", ApiRef, RemappedDstRect.left, RemappedDstRect.top, RemappedDstRect.right, RemappedDstRect.bottom);
        }
        // in case of NOD3DRESET, remap source rect. Unfortunately, this doesn't work in fullscreen mode ....
        if((dxw.dwFlags4 & NOD3DRESET) && (pSourceRect == NULL)) {
            RemappedSrcRect = dxw.GetScreenRect();
            pSourceRect = &RemappedSrcRect;
            OutDebugD3D("%s: NOD3DRESET FIXED SourceRect=(%d,%d)-(%d,%d)\n", ApiRef, RemappedSrcRect.left, RemappedSrcRect.top, RemappedSrcRect.right, RemappedSrcRect.bottom);
        }
    }
    res = (*pPresentSwC8)(pd3dd, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
    if(res)
        OutTraceE("%s: err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else {
        if(IsDebugDW) {
            if(pDirtyRegion) {
                OutTraceD3D("%s: D3D_OK dirtyreg={buf=%#x type=%d count=%d rgnsize=%#x bound=(%d,%d)-(%d,%d)}\n",
                            ApiRef, pDirtyRegion->Buffer, pDirtyRegion->rdh.iType, pDirtyRegion->rdh.nCount, pDirtyRegion->rdh.nRgnSize,
                            pDirtyRegion->rdh.rcBound.left, pDirtyRegion->rdh.rcBound.top, pDirtyRegion->rdh.rcBound.right, pDirtyRegion->rdh.rcBound.bottom);
            } else
                OutTraceD3D("%s: D3D_OK\n", ApiRef);
        }
    }
    if(bEmulation) (*pInvalidateRect)(dxw.GethWnd(), NULL, FALSE);
    dxw.ShowOverlay();
    return res;
}

HRESULT WINAPI extPresentSwC9(void *pd3dd, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion, DWORD dwFlags) {
    HRESULT res;
    ApiName("IDirect3DSwapChain9::Present");
    RECT RemappedSrcRect, RemappedDstRect;
    BOOL bEmulation = FALSE;
    if(IsDebugD3D) {
        char sSourceRect[81];
        char sDestRect[81];
        if (pSourceRect) sprintf_s(sSourceRect, 80, "(%d,%d)-(%d,%d)", pSourceRect->left, pSourceRect->top, pSourceRect->right, pSourceRect->bottom);
        else strcpy(sSourceRect, "(NULL)");
        if (pDestRect) sprintf_s(sDestRect, 80, "(%d,%d)-(%d,%d)", pDestRect->left, pDestRect->top, pDestRect->right, pDestRect->bottom);
        else strcpy(sDestRect, "(NULL)");
        OutDebugD3D("%s: d3dd=%#x SourceRect=%s DestRect=%s hDestWndOverride=%#x dirtyregion=%#x flags=%#x\n",
                    ApiRef, pd3dd, sSourceRect, sDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
    }
    // frame counter handling....
    if (dxw.HandleFPS()) dwFlags |= D3DPRESENT_DONOTWAIT; // can't simply return OK here!
    if (dxw.dwFlags1 & SAVELOAD) dxw.VSyncWait();
    if (dxw.dwFlags5 & MESSAGEPUMP) dxw.MessagePump();
    if (gFlippedToScreen) {
        OutTraceD3D("FilledToScreen SKIP\n");
        gFlippedToScreen--;
        return D3D_OK;
    }
    if(dxw.dwDFlags & FULLRECTBLT) pSourceRect = pDestRect = NULL;
    if(dxw.Windowize) {
        // v2.03.15 - fix target RECT region
        // v2.04.37 - target scaling must be done only when pDestRect is not NULL. Fixes "Fastlane Pinball" black screen.
        if(pDestRect) {
            RemappedDstRect = dxw.MapClientRect((LPRECT)pDestRect);
            pDestRect = &RemappedDstRect;
            OutDebugD3D("%s: FIXED DestRect=(%d,%d)-(%d,%d)\n", ApiRef, RemappedDstRect.left, RemappedDstRect.top, RemappedDstRect.right, RemappedDstRect.bottom);
        }
        // in case of NOD3DRESET, remap source rect. Unfortunately, this doesn't work in fullscreen mode ....
        if((dxw.dwFlags4 & NOD3DRESET) && (pSourceRect == NULL)) {
            RemappedSrcRect = dxw.GetScreenRect();
            pSourceRect = &RemappedSrcRect;
            OutDebugD3D("%s: NOD3DRESET FIXED SourceRect=(%d,%d)-(%d,%d)\n", ApiRef, RemappedSrcRect.left, RemappedSrcRect.top, RemappedSrcRect.right, RemappedSrcRect.bottom);
        }
    }
    res = (*pPresentSwC9)(pd3dd, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
    if(res)
        OutTraceE("%s: err=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    else {
        if(IsDebugDW) {
            if(pDirtyRegion) {
                OutTraceD3D("%s: D3D_OK dirtyreg={buf=%#x type=%d count=%d rgnsize=%#x bound=(%d,%d)-(%d,%d)}\n",
                            ApiRef, pDirtyRegion->Buffer, pDirtyRegion->rdh.iType, pDirtyRegion->rdh.nCount, pDirtyRegion->rdh.nRgnSize,
                            pDirtyRegion->rdh.rcBound.left, pDirtyRegion->rdh.rcBound.top, pDirtyRegion->rdh.rcBound.right, pDirtyRegion->rdh.rcBound.bottom);
            } else
                OutTraceD3D("%s: D3D_OK\n", ApiRef);
        }
    }
    if(bEmulation) (*pInvalidateRect)(dxw.GethWnd(), NULL, FALSE);
    dxw.ShowOverlay();
    return res;
}
