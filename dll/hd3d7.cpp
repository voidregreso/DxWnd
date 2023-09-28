#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <d3d.h>
#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhook.h"
#include "syslibs.h"
#include "dxhelper.h"

//#define TRACEALL
//#define DUMPEXECUTEBUFFER

#ifdef TRACEALL
#define TRACETEXTURE
#endif

extern void LimitFrameCount(DWORD);
extern LPDIRECTDRAW lpPrimaryDD;
extern char *ExplainDDError(DWORD);
static void LegacyD3DResolutionHack(int);
LPDIRECT3DVIEWPORT lpCurrViewport = NULL;

typedef HRESULT (WINAPI *QueryInterface_Type)(void *, REFIID, LPVOID *);
extern HRESULT WINAPI extQueryInterfaceDX(int, QueryInterface_Type, void *, REFIID, LPVOID *);
char *sExecuteBufferCaps(DWORD);
LPDIRECT3DVIEWPORT gViewport1 = NULL;

//----------------------------------------------------------------------//
// Hookers typedefs, prototypes, pointers
//----------------------------------------------------------------------//

// exported API

typedef HRESULT (WINAPI *Direct3DCreateDevice_Type)(GUID FAR *, LPDIRECT3D, LPDIRECTDRAWSURFACE, LPDIRECT3D *, LPUNKNOWN);
typedef HRESULT (WINAPI *Direct3DCreate_Type)(UINT, LPDIRECT3D *, LPUNKNOWN);

Direct3DCreateDevice_Type pDirect3DCreateDevice = NULL;
Direct3DCreate_Type pDirect3DCreate = NULL;

HRESULT WINAPI extDirect3DCreateDevice(GUID FAR *, LPDIRECT3D, LPDIRECTDRAWSURFACE, LPDIRECT3D *, LPUNKNOWN);
HRESULT WINAPI extDirect3DCreate(UINT, LPDIRECT3D *, LPUNKNOWN);

// IDirect3D-n interfaces

typedef HRESULT (WINAPI *QueryInterfaceD3_Type)(void *, REFIID, LPVOID *);
typedef HRESULT (WINAPI *Initialize_Type)(void *);
typedef HRESULT (WINAPI *EnumDevices_Type)(void *, LPD3DENUMDEVICESCALLBACK, LPVOID);
typedef HRESULT (WINAPI *EnumDevices7_Type)(void *, LPD3DENUMDEVICESCALLBACK7, LPVOID);
typedef HRESULT (WINAPI *CreateLight_Type)(void *, LPDIRECT3DLIGHT *, IUnknown *);
#ifdef TRACEMATERIAL
typedef HRESULT (WINAPI *CreateMaterial1_Type)(void *, LPDIRECT3DMATERIAL *, IUnknown *);
typedef HRESULT (WINAPI *CreateMaterial2_Type)(void *, LPDIRECT3DMATERIAL2 *, IUnknown *);
typedef HRESULT (WINAPI *CreateMaterial3_Type)(void *, LPDIRECT3DMATERIAL3 *, IUnknown *);
#endif
typedef HRESULT (WINAPI *CreateViewport1_Type)(void *, LPDIRECT3DVIEWPORT *, IUnknown *);
typedef HRESULT (WINAPI *CreateViewport2_Type)(void *, LPDIRECT3DVIEWPORT2 *, IUnknown *);
typedef HRESULT (WINAPI *CreateViewport3_Type)(void *, LPDIRECT3DVIEWPORT3 *, IUnknown *);
typedef HRESULT (WINAPI *FindDevice_Type)(void *, LPD3DFINDDEVICESEARCH, LPD3DFINDDEVICERESULT);
typedef HRESULT (WINAPI *CreateDevice2_Type)(void *, REFCLSID, LPDIRECTDRAWSURFACE, LPDIRECT3DDEVICE2 *);
typedef HRESULT (WINAPI *CreateDevice3_Type)(void *, REFCLSID, LPDIRECTDRAWSURFACE4, LPDIRECT3DDEVICE3 *, LPUNKNOWN);
typedef HRESULT (WINAPI *CreateDevice7_Type)(void *, REFCLSID, LPDIRECTDRAWSURFACE7, LPDIRECT3DDEVICE7 *);
typedef HRESULT (WINAPI *EnumZBufferFormats_Type)(void *, REFCLSID, LPD3DENUMPIXELFORMATSCALLBACK, LPVOID);
typedef HRESULT (WINAPI *EnumTextureFormats12_Type)(void *, LPD3DENUMTEXTUREFORMATSCALLBACK, LPVOID);
typedef HRESULT (WINAPI *EnumTextureFormats37_Type)(void *, LPD3DENUMPIXELFORMATSCALLBACK, LPVOID);

QueryInterfaceD3_Type pQueryInterfaceD31 = NULL;
QueryInterfaceD3_Type pQueryInterfaceD32 = NULL;
QueryInterfaceD3_Type pQueryInterfaceD33 = NULL;
QueryInterfaceD3_Type pQueryInterfaceD37 = NULL;
Initialize_Type pInitialize = NULL;
EnumDevices_Type pEnumDevices1 = NULL;
EnumDevices_Type pEnumDevices2 = NULL;
EnumDevices_Type pEnumDevices3 = NULL;
EnumDevices7_Type pEnumDevices7 = NULL;
CreateLight_Type pCreateLight1 = NULL;
CreateLight_Type pCreateLight2 = NULL;
CreateLight_Type pCreateLight3 = NULL;
#ifdef TRACEMATERIAL
CreateMaterial1_Type pCreateMaterial1 = NULL;
CreateMaterial2_Type pCreateMaterial2 = NULL;
CreateMaterial3_Type pCreateMaterial3 = NULL;
#endif
CreateViewport1_Type pCreateViewport1 = NULL;
CreateViewport2_Type pCreateViewport2 = NULL;
CreateViewport3_Type pCreateViewport3 = NULL;
FindDevice_Type pFindDevice1, pFindDevice2, pFindDevice3;
CreateDevice2_Type pCreateDevice2 = NULL;
CreateDevice3_Type pCreateDevice3 = NULL;
CreateDevice7_Type pCreateDevice7 = NULL;
EnumZBufferFormats_Type pEnumZBufferFormats3 = NULL;
EnumZBufferFormats_Type pEnumZBufferFormats7 = NULL;

EnumTextureFormats12_Type pEnumTextureFormats1, pEnumTextureFormats2;
EnumTextureFormats37_Type pEnumTextureFormats3, pEnumTextureFormats7;

HRESULT WINAPI extQueryInterfaceD31(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceD32(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceD33(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceD37(void *, REFIID, LPVOID *);
HRESULT WINAPI extEnumDevices1(void *, LPD3DENUMDEVICESCALLBACK, LPVOID);
HRESULT WINAPI extEnumDevices2(void *, LPD3DENUMDEVICESCALLBACK, LPVOID);
HRESULT WINAPI extEnumDevices3(void *, LPD3DENUMDEVICESCALLBACK, LPVOID);
HRESULT WINAPI extEnumDevices7(void *, LPD3DENUMDEVICESCALLBACK7, LPVOID);
HRESULT WINAPI extCreateLight1(void *, LPDIRECT3DLIGHT *, IUnknown *);
HRESULT WINAPI extCreateLight2(void *, LPDIRECT3DLIGHT *, IUnknown *);
HRESULT WINAPI extCreateLight3(void *, LPDIRECT3DLIGHT *, IUnknown *);

HRESULT WINAPI extEnumZBufferFormats3(void *, REFCLSID, LPD3DENUMPIXELFORMATSCALLBACK, LPVOID);
HRESULT WINAPI extEnumZBufferFormats7(void *, REFCLSID, LPD3DENUMPIXELFORMATSCALLBACK, LPVOID);

HRESULT WINAPI extEnumTextureFormats1(void *, LPD3DENUMTEXTUREFORMATSCALLBACK, LPVOID);
HRESULT WINAPI extEnumTextureFormats2(void *, LPD3DENUMTEXTUREFORMATSCALLBACK, LPVOID);
HRESULT WINAPI extEnumTextureFormats3(void *, LPD3DENUMPIXELFORMATSCALLBACK, LPVOID);
HRESULT WINAPI extEnumTextureFormats7(void *, LPD3DENUMPIXELFORMATSCALLBACK, LPVOID);

// Direct3DDevice-n interfaces

typedef ULONG   (WINAPI *ReleaseD3D_Type)(LPDIRECT3DDEVICE);
typedef HRESULT (WINAPI *QueryInterfaceD3D_Type)(void *, REFIID, LPVOID *);
typedef HRESULT (WINAPI *D3DInitialize_Type)(void *, LPDIRECT3D, LPGUID, LPD3DDEVICEDESC);
typedef HRESULT (WINAPI *D3DGetCaps_Type)(void *, LPD3DDEVICEDESC, LPD3DDEVICEDESC);
typedef HRESULT (WINAPI *D3DGetCaps7_Type)(void *, LPD3DDEVICEDESC7);
typedef HRESULT (WINAPI *AddViewport1_Type)(void *, LPDIRECT3DVIEWPORT);
typedef HRESULT (WINAPI *AddViewport2_Type)(void *, LPDIRECT3DVIEWPORT2);
typedef HRESULT (WINAPI *AddViewport3_Type)(void *, LPDIRECT3DVIEWPORT3);
typedef HRESULT (WINAPI *Scene_Type)(void *); // BeginScene, EndScene
typedef HRESULT (WINAPI *SetRenderState3_Type)(void *, D3DRENDERSTATETYPE, DWORD);
typedef HRESULT (WINAPI *SetLightState_Type)(void *, D3DLIGHTSTATETYPE, DWORD);
typedef HRESULT (WINAPI *GetCurrentViewport2_Type)(void *, LPDIRECT3DVIEWPORT2 *);
typedef HRESULT (WINAPI *SetCurrentViewport2_Type)(void *, LPDIRECT3DVIEWPORT2);
typedef HRESULT (WINAPI *GetCurrentViewport3_Type)(void *, LPDIRECT3DVIEWPORT3 *);
typedef HRESULT (WINAPI *SetCurrentViewport3_Type)(void *, LPDIRECT3DVIEWPORT3);
typedef HRESULT (WINAPI *SetTexture3_Type)(void *, DWORD, LPDIRECT3DTEXTURE2);
typedef HRESULT (WINAPI *SetTexture7_Type)(void *, DWORD, LPDIRECTDRAWSURFACE7);
typedef HRESULT (WINAPI *SwapTextureHandles_Type)(void *, LPDIRECT3DTEXTURE, LPDIRECT3DTEXTURE);
typedef HRESULT (WINAPI *SwapTextureHandles2_Type)(void *, LPDIRECT3DTEXTURE2, LPDIRECT3DTEXTURE2);
typedef HRESULT (WINAPI *SetLight7_Type)(void *, DWORD, LPD3DLIGHT7);
typedef HRESULT (WINAPI *GetViewport7_Type)(void *, LPD3DVIEWPORT7);
typedef HRESULT (WINAPI *SetViewport7_Type)(void *, LPD3DVIEWPORT7);
#ifdef TRACEALL
typedef HRESULT (WINAPI *SetTransform_Type)(void *, D3DTRANSFORMSTATETYPE, LPD3DMATRIX);
typedef HRESULT (WINAPI *GetLight7_Type)(void *, DWORD, LPD3DLIGHT7);
typedef HRESULT (WINAPI *SetRenderTarget_Type)(void *, LPDIRECTDRAWSURFACE7, DWORD);
typedef HRESULT (WINAPI *GetRenderTarget_Type)(void *, LPDIRECTDRAWSURFACE7 *);
typedef HRESULT (WINAPI *DrawPrimitive2_Type)(void *, D3DPRIMITIVETYPE, D3DVERTEXTYPE, LPVOID, DWORD, DWORD);
typedef HRESULT (WINAPI *DrawPrimitive37_Type)(void *, D3DPRIMITIVETYPE, DWORD, LPVOID, DWORD, DWORD);
typedef HRESULT (WINAPI *GetTextureStageState_Type)(void *, DWORD, D3DTEXTURESTAGESTATETYPE, LPDWORD);
typedef HRESULT (WINAPI *SetTextureStageState_Type)(void *, DWORD, D3DTEXTURESTAGESTATETYPE, DWORD);
#endif
typedef HRESULT (WINAPI *CreateExecuteBuffer_Type)(void *, LPD3DEXECUTEBUFFERDESC, LPDIRECT3DEXECUTEBUFFER *, IUnknown *);
typedef HRESULT (WINAPI *Execute_Type)(void *, LPDIRECT3DEXECUTEBUFFER, LPDIRECT3DVIEWPORT, DWORD);

QueryInterfaceD3_Type pQueryInterfaceD3D1, pQueryInterfaceD3D2, pQueryInterfaceD3D3, pQueryInterfaceD3D7;
ReleaseD3D_Type pReleaseD3D1, pReleaseD3D2, pReleaseD3D3, pReleaseD3D7;
D3DInitialize_Type pD3DInitialize = NULL;
D3DGetCaps_Type pD3DGetCaps1, pD3DGetCaps2, pD3DGetCaps3;
D3DGetCaps7_Type pD3DGetCaps7;
AddViewport1_Type pAddViewport1 = NULL;
AddViewport2_Type pAddViewport2 = NULL;
AddViewport3_Type pAddViewport3 = NULL;
Scene_Type pBeginScene1 = NULL;
Scene_Type pBeginScene2 = NULL;
Scene_Type pBeginScene3 = NULL;
Scene_Type pBeginScene7 = NULL;
Scene_Type pEndScene1 = NULL;
Scene_Type pEndScene2 = NULL;
Scene_Type pEndScene3 = NULL;
Scene_Type pEndScene7 = NULL;
SetRenderState3_Type pSetRenderState2 = NULL;
SetRenderState3_Type pSetRenderState3 = NULL;
SetRenderState3_Type pSetRenderState7 = NULL;
SetLightState_Type pSetLightState3 = NULL;
GetCurrentViewport2_Type pGetCurrentViewport2 = NULL;
SetCurrentViewport2_Type pSetCurrentViewport2 = NULL;
GetCurrentViewport3_Type pGetCurrentViewport3 = NULL;
SetCurrentViewport3_Type pSetCurrentViewport3 = NULL;
SetTexture3_Type pSetTexture3 = NULL;
SetTexture7_Type pSetTexture7 = NULL;
SwapTextureHandles_Type pSwapTextureHandles = NULL;
SwapTextureHandles2_Type pSwapTextureHandles2 = NULL;
SetLight7_Type pSetLight7 = NULL;
GetViewport7_Type pGetViewport7 = NULL;
SetViewport7_Type pSetViewport7 = NULL;
#ifdef TRACEALL
SetTransform_Type pSetTransform2, pSetTransform3, pSetTransform7;
GetLight7_Type pGetLight7 = NULL;
SetRenderTarget_Type pSetRenderTarget7 = NULL;
GetRenderTarget_Type pGetRenderTarget7 = NULL;
DrawPrimitive2_Type pDrawPrimitive2;
DrawPrimitive37_Type pDrawPrimitive3, pDrawPrimitive7;
GetTextureStageState_Type pGetTextureStageState3, pGetTextureStageState7;
SetTextureStageState_Type pSetTextureStageState3, pSetTextureStageState7;
#endif
CreateExecuteBuffer_Type pCreateExecuteBuffer = NULL;
Execute_Type pExecute = NULL;

// IDirect3DViewport-n interfaces

typedef HRESULT (WINAPI *InitializeVP_Type)(void *, LPDIRECT3D);
typedef HRESULT (WINAPI *Pick_Type)(void *, LPDIRECT3DEXECUTEBUFFER, LPDIRECT3DVIEWPORT, DWORD, LPD3DRECT);
typedef HRESULT (WINAPI *SetViewport_Type)(void *, LPD3DVIEWPORT);
typedef HRESULT (WINAPI *GetViewport_Type)(void *, LPD3DVIEWPORT);
typedef HRESULT (WINAPI *GetViewport2_Type)(void *, LPD3DVIEWPORT);
typedef HRESULT (WINAPI *SetViewport2_Type)(void *, LPD3DVIEWPORT);
typedef HRESULT (WINAPI *GetViewport3_Type)(void *, LPD3DVIEWPORT);
typedef HRESULT (WINAPI *SetViewport3_Type)(void *, LPD3DVIEWPORT);
typedef HRESULT (WINAPI *GetViewport2_3_Type)(void *, LPD3DVIEWPORT2);
typedef HRESULT (WINAPI *SetViewport2_3_Type)(void *, LPD3DVIEWPORT2);
typedef HRESULT (WINAPI *DeleteViewport1_Type)(void *, LPDIRECT3DVIEWPORT);
typedef HRESULT (WINAPI *NextViewport1_Type)(void *, LPDIRECT3DVIEWPORT, LPDIRECT3DVIEWPORT *, DWORD);
typedef HRESULT (WINAPI *DeleteViewport2_Type)(void *, LPDIRECT3DVIEWPORT2);
typedef HRESULT (WINAPI *NextViewport2_Type)(void *, LPDIRECT3DVIEWPORT2, LPDIRECT3DVIEWPORT2 *, DWORD);
typedef HRESULT (WINAPI *ViewportClear_Type)(void *, DWORD, LPD3DRECT, DWORD);
#ifdef TRACEALL
typedef HRESULT (WINAPI *TransformVertices_Type)(void *, DWORD, LPD3DTRANSFORMDATA, DWORD, LPDWORD);
typedef HRESULT (WINAPI *LightElements_Type)(void *, DWORD, LPD3DLIGHTDATA);
typedef HRESULT (WINAPI *SetBackground_Type)(void *, D3DMATERIALHANDLE);
typedef HRESULT (WINAPI *GetBackground_Type)(void *, LPD3DMATERIALHANDLE, LPBOOL);
typedef HRESULT (WINAPI *SetBackgroundDepth_Type)(void *, LPDIRECTDRAWSURFACE);
typedef HRESULT (WINAPI *GetBackgroundDepth_Type)(void *, LPDIRECTDRAWSURFACE *, LPBOOL);
typedef HRESULT (WINAPI *Clear_Type)(void *, DWORD, LPD3DRECT, DWORD);
typedef HRESULT (WINAPI *AddLight_Type)(void *, LPDIRECT3DLIGHT);
typedef HRESULT (WINAPI *DeleteLight_Type)(void *, LPDIRECT3DLIGHT);
typedef HRESULT (WINAPI *NextLight_Type)(void *, LPDIRECT3DLIGHT, LPDIRECT3DLIGHT *, DWORD);
#endif
#ifdef TRACEMATERIAL
// IDirect3DMaterial interfaces

typedef HRESULT (WINAPI *SetMaterial_Type)(void *, LPD3DMATERIAL);
typedef HRESULT (WINAPI *GetMaterial_Type)(void *, LPD3DMATERIAL);
#endif

InitializeVP_Type pInitializeVP = NULL; // ???
Pick_Type pPick = NULL;
DeleteViewport1_Type pDeleteViewport1 = NULL;
DeleteViewport2_Type pDeleteViewport2 = NULL;
NextViewport1_Type pNextViewport1 = NULL;
NextViewport2_Type pNextViewport2 = NULL;
ViewportClear_Type pViewportClear = NULL;
SetViewport_Type pSetViewport1 = NULL;
GetViewport_Type pGetViewport1 = NULL;
GetViewport2_Type pGetViewport2 = NULL;
SetViewport2_Type pSetViewport2 = NULL;
GetViewport3_Type pGetViewport3 = NULL;
SetViewport3_Type pSetViewport3 = NULL;
GetViewport2_3_Type pGetViewport2_2 = NULL;
SetViewport2_3_Type pSetViewport2_2 = NULL;
SetViewport2_3_Type pSetViewport2_3 = NULL;
GetViewport2_3_Type pGetViewport2_3 = NULL;
#ifdef TRACEALL
TransformVertices_Type pTransformVertices1, pTransformVertices2, pTransformVertices3;
LightElements_Type pLightElements1, pLightElements2, pLightElements3;
SetBackground_Type pSetBackground1, pSetBackground2, pSetBackground3;
GetBackground_Type pGetBackground1, pGetBackground2, pGetBackground3;
SetBackgroundDepth_Type pSetBackgroundDepth1, pSetBackgroundDepth2, pSetBackgroundDepth3;
GetBackgroundDepth_Type pGetBackgroundDepth1, pGetBackgroundDepth2, pGetBackgroundDepth3;
Clear_Type pClear1, pClear2, pClear3;
AddLight_Type pAddLight1, pAddLight2, pAddLight3;
DeleteLight_Type pDeleteLight1, pDeleteLight2, pDeleteLight3;
NextLight_Type pNextLight1, pNextLight2, pNextLight3;
#endif

#ifdef TRACEMATERIAL
SetMaterial_Type pSetMaterial = NULL;
GetMaterial_Type pGetMaterial = NULL;
#endif

#ifdef TRACEMATERIAL
HRESULT WINAPI extCreateMaterial1(void *, LPDIRECT3DMATERIAL *, IUnknown *);
HRESULT WINAPI extCreateMaterial2(void *, LPDIRECT3DMATERIAL2 *, IUnknown *);
HRESULT WINAPI extCreateMaterial3(void *, LPDIRECT3DMATERIAL3 *, IUnknown *);
HRESULT WINAPI extPick(void *, LPDIRECT3DEXECUTEBUFFER, LPDIRECT3DVIEWPORT, DWORD, LPD3DRECT);
HRESULT WINAPI extInitialize(void *);
#endif
HRESULT WINAPI extCreateViewport1(void *, LPDIRECT3DVIEWPORT *, IUnknown *);
HRESULT WINAPI extCreateViewport2(void *, LPDIRECT3DVIEWPORT2 *, IUnknown *);
HRESULT WINAPI extCreateViewport3(void *, LPDIRECT3DVIEWPORT3 *, IUnknown *);
HRESULT WINAPI extFindDevice1(void *, LPD3DFINDDEVICESEARCH, LPD3DFINDDEVICERESULT);
HRESULT WINAPI extFindDevice2(void *, LPD3DFINDDEVICESEARCH, LPD3DFINDDEVICERESULT);
HRESULT WINAPI extFindDevice3(void *, LPD3DFINDDEVICESEARCH, LPD3DFINDDEVICERESULT);
HRESULT WINAPI extCreateDevice2(void *, REFCLSID, LPDIRECTDRAWSURFACE, LPDIRECT3DDEVICE2 *);
HRESULT WINAPI extCreateDevice3(void *, REFCLSID, LPDIRECTDRAWSURFACE4, LPDIRECT3DDEVICE3 *, LPUNKNOWN);
HRESULT WINAPI extCreateDevice7(void *, REFCLSID, LPDIRECTDRAWSURFACE7, LPDIRECT3DDEVICE7 *);
HRESULT WINAPI extDeleteViewport1(void *, LPDIRECT3DVIEWPORT);
HRESULT WINAPI extNextViewport1(void *, LPDIRECT3DVIEWPORT, LPDIRECT3DVIEWPORT *, DWORD);
HRESULT WINAPI extDeleteViewport2(void *, LPDIRECT3DVIEWPORT2);
HRESULT WINAPI extNextViewport2(void *, LPDIRECT3DVIEWPORT2, LPDIRECT3DVIEWPORT2 *, DWORD);
HRESULT WINAPI extViewportClear(void *, DWORD, LPD3DRECT, DWORD);

HRESULT WINAPI extInitializeVP(void *, LPDIRECT3D);
HRESULT WINAPI extSetViewport1(void *, LPD3DVIEWPORT);
HRESULT WINAPI extGetViewport1(void *, LPD3DVIEWPORT);
HRESULT WINAPI extSetViewport2(void *, LPD3DVIEWPORT);
HRESULT WINAPI extGetViewport2(void *, LPD3DVIEWPORT);
HRESULT WINAPI extSetViewport3(void *, LPD3DVIEWPORT);
HRESULT WINAPI extGetViewport3(void *, LPD3DVIEWPORT);
#ifdef TRACEALL
HRESULT WINAPI extTransformVertices1(void *, DWORD, LPD3DTRANSFORMDATA, DWORD, LPDWORD);
HRESULT WINAPI extTransformVertices2(void *, DWORD, LPD3DTRANSFORMDATA, DWORD, LPDWORD);
HRESULT WINAPI extTransformVertices3(void *, DWORD, LPD3DTRANSFORMDATA, DWORD, LPDWORD);
HRESULT WINAPI extLightElements1(void *, DWORD, LPD3DLIGHTDATA);
HRESULT WINAPI extLightElements2(void *, DWORD, LPD3DLIGHTDATA);
HRESULT WINAPI extLightElements3(void *, DWORD, LPD3DLIGHTDATA);
HRESULT WINAPI extSetBackground1(void *, D3DMATERIALHANDLE);
HRESULT WINAPI extSetBackground2(void *, D3DMATERIALHANDLE);
HRESULT WINAPI extSetBackground3(void *, D3DMATERIALHANDLE);
HRESULT WINAPI extGetBackground1(void *, LPD3DMATERIALHANDLE, LPBOOL);
HRESULT WINAPI extGetBackground2(void *, LPD3DMATERIALHANDLE, LPBOOL);
HRESULT WINAPI extGetBackground3(void *, LPD3DMATERIALHANDLE, LPBOOL);
HRESULT WINAPI extSetBackgroundDepth1(void *, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extSetBackgroundDepth2(void *, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extSetBackgroundDepth3(void *, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extGetBackgroundDepth1(void *, LPDIRECTDRAWSURFACE *, LPBOOL);
HRESULT WINAPI extGetBackgroundDepth2(void *, LPDIRECTDRAWSURFACE *, LPBOOL);
HRESULT WINAPI extGetBackgroundDepth3(void *, LPDIRECTDRAWSURFACE *, LPBOOL);
HRESULT WINAPI extClear1(void *, DWORD, LPD3DRECT, DWORD);
HRESULT WINAPI extClear2(void *, DWORD, LPD3DRECT, DWORD);
HRESULT WINAPI extClear3(void *, DWORD, LPD3DRECT, DWORD);
HRESULT WINAPI extAddLight1(void *, LPDIRECT3DLIGHT);
HRESULT WINAPI extAddLight2(void *, LPDIRECT3DLIGHT);
HRESULT WINAPI extAddLight3(void *, LPDIRECT3DLIGHT);
HRESULT WINAPI extDeleteLight1(void *, LPDIRECT3DLIGHT);
HRESULT WINAPI extDeleteLight2(void *, LPDIRECT3DLIGHT);
HRESULT WINAPI extDeleteLight3(void *, LPDIRECT3DLIGHT);
HRESULT WINAPI extNextLight1(void *, LPDIRECT3DLIGHT, LPDIRECT3DLIGHT *, DWORD);
HRESULT WINAPI extNextLight2(void *, LPDIRECT3DLIGHT, LPDIRECT3DLIGHT *, DWORD);
HRESULT WINAPI extNextLight3(void *, LPDIRECT3DLIGHT, LPDIRECT3DLIGHT *, DWORD);
#endif

#ifdef TRACEMATERIAL
HRESULT WINAPI extSetMaterial(void *, LPD3DMATERIAL);
HRESULT WINAPI extGetMaterial(void *, LPD3DMATERIAL);
#endif
HRESULT WINAPI extQueryInterfaceD3(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceD3D1(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceD3D2(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceD3D3(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceD3D7(void *, REFIID, LPVOID *);

#ifdef TRACEALL
ULONG WINAPI extReleaseD3D1(LPDIRECT3DDEVICE);
ULONG WINAPI extReleaseD3D2(LPDIRECT3DDEVICE);
ULONG WINAPI extReleaseD3D3(LPDIRECT3DDEVICE);
ULONG WINAPI extReleaseD3D7(LPDIRECT3DDEVICE);
#endif
HRESULT WINAPI extBeginScene1(void *);
HRESULT WINAPI extEndScene1(void *);
HRESULT WINAPI extBeginScene2(void *);
HRESULT WINAPI extEndScene2(void *);
HRESULT WINAPI extBeginScene3(void *);
HRESULT WINAPI extEndScene3(void *);
HRESULT WINAPI extBeginScene7(void *);
HRESULT WINAPI extEndScene7(void *);
HRESULT WINAPI extSetRenderState2(void *, D3DRENDERSTATETYPE, DWORD);
HRESULT WINAPI extSetRenderState3(void *, D3DRENDERSTATETYPE, DWORD);
HRESULT WINAPI extSetRenderState7(void *, D3DRENDERSTATETYPE, DWORD);
HRESULT WINAPI extD3DGetCaps1(void *, LPD3DDEVICEDESC, LPD3DDEVICEDESC);
HRESULT WINAPI extD3DGetCaps2(void *, LPD3DDEVICEDESC, LPD3DDEVICEDESC);
HRESULT WINAPI extD3DGetCaps3(void *, LPD3DDEVICEDESC, LPD3DDEVICEDESC);
HRESULT WINAPI extD3DGetCaps7(void *, LPD3DDEVICEDESC7);
HRESULT WINAPI extSetLightState3(void *d3dd, D3DLIGHTSTATETYPE d3dls, DWORD t);
HRESULT WINAPI extSetViewport3(void *, LPD3DVIEWPORT);
HRESULT WINAPI extGetViewport3(void *, LPD3DVIEWPORT);
HRESULT WINAPI extAddViewport1(void *, LPDIRECT3DVIEWPORT);
HRESULT WINAPI extAddViewport2(void *, LPDIRECT3DVIEWPORT2);
HRESULT WINAPI extAddViewport3(void *, LPDIRECT3DVIEWPORT3);
HRESULT WINAPI extGetViewport2(void *, LPD3DVIEWPORT);
HRESULT WINAPI extSetViewport2(void *, LPD3DVIEWPORT);
HRESULT WINAPI extGetViewport2_2(void *, LPD3DVIEWPORT2);
HRESULT WINAPI extSetViewport2_2(void *, LPD3DVIEWPORT2);
HRESULT WINAPI extGetViewport2_3(void *, LPD3DVIEWPORT2);
HRESULT WINAPI extSetViewport2_3(void *, LPD3DVIEWPORT2);
HRESULT WINAPI extSetCurrentViewport2(void *, LPDIRECT3DVIEWPORT2);
HRESULT WINAPI extGetCurrentViewport2(void *, LPDIRECT3DVIEWPORT2 *);
HRESULT WINAPI extSetCurrentViewport3(void *, LPDIRECT3DVIEWPORT3);
HRESULT WINAPI extGetCurrentViewport3(void *, LPDIRECT3DVIEWPORT3 *);
HRESULT WINAPI extSetViewport7(void *, LPD3DVIEWPORT7);
HRESULT WINAPI extGetViewport7(void *, LPD3DVIEWPORT7);
HRESULT WINAPI extSetTexture3(void *, DWORD, LPDIRECT3DTEXTURE2);
HRESULT WINAPI extSetTexture7(void *, DWORD, LPDIRECTDRAWSURFACE7);
HRESULT WINAPI extSwapTextureHandles(void *, LPDIRECT3DTEXTURE, LPDIRECT3DTEXTURE);
HRESULT WINAPI extSwapTextureHandles2(void *, LPDIRECT3DTEXTURE2, LPDIRECT3DTEXTURE2);
HRESULT WINAPI extSetLight7(void *, DWORD, LPD3DLIGHT7);
#ifdef TRACEALL
HRESULT WINAPI extSetTransform3(void *, D3DTRANSFORMSTATETYPE, LPD3DMATRIX);
HRESULT WINAPI extSetTransform7(void *, D3DTRANSFORMSTATETYPE, LPD3DMATRIX);
HRESULT WINAPI extSetTransform2(void *, D3DTRANSFORMSTATETYPE, LPD3DMATRIX);
HRESULT WINAPI extGetLight7(void *, DWORD, LPD3DLIGHT7);
HRESULT WINAPI extSetRenderTarget7(void *, LPDIRECTDRAWSURFACE7, DWORD);
HRESULT WINAPI extGetRenderTarget7(void *, LPDIRECTDRAWSURFACE7 *);
HRESULT WINAPI extDrawPrimitive2(void *, D3DPRIMITIVETYPE, D3DVERTEXTYPE, LPVOID, DWORD, DWORD);
HRESULT WINAPI extDrawPrimitive3(void *, D3DPRIMITIVETYPE, DWORD, LPVOID, DWORD, DWORD);
HRESULT WINAPI extDrawPrimitive7(void *, D3DPRIMITIVETYPE, DWORD, LPVOID, DWORD, DWORD);
HRESULT WINAPI extGetTextureStageState3(void *, DWORD, D3DTEXTURESTAGESTATETYPE, LPDWORD);
HRESULT WINAPI extGetTextureStageState7(void *, DWORD, D3DTEXTURESTAGESTATETYPE, LPDWORD);
HRESULT WINAPI extSetTextureStageState3(void *, DWORD, D3DTEXTURESTAGESTATETYPE, DWORD);
HRESULT WINAPI extSetTextureStageState7(void *, DWORD, D3DTEXTURESTAGESTATETYPE, DWORD);
#endif
HRESULT WINAPI extCreateExecuteBuffer(void *, LPD3DEXECUTEBUFFERDESC, LPDIRECT3DEXECUTEBUFFER *, IUnknown *);
HRESULT WINAPI extExecute(void *, LPDIRECT3DEXECUTEBUFFER, LPDIRECT3DVIEWPORT, DWORD);

// Texture

#ifdef TRACETEXTURE
typedef HRESULT (WINAPI *TexInitialize_Type)(void *, LPDIRECT3DDEVICE, LPDIRECTDRAWSURFACE);
typedef HRESULT (WINAPI *TexGetHandle_Type)(void *, LPDIRECT3DDEVICE, LPD3DTEXTUREHANDLE);
typedef HRESULT (WINAPI *TexPaletteChanged_Type)(void *, DWORD, DWORD);
typedef HRESULT (WINAPI *TexLoad_Type)(void *, LPDIRECT3DTEXTURE);
typedef HRESULT (WINAPI *TexUnload_Type)(void *);

TexInitialize_Type pTInitialize = NULL;
TexGetHandle_Type pTGetHandle1, pTGetHandle2;
TexPaletteChanged_Type pTPaletteChanged1, pTPaletteChanged2;
TexLoad_Type pTLoad1, pTLoad2;
TexUnload_Type pTUnload = NULL;

HRESULT WINAPI extTexInitialize(void *, LPDIRECT3DDEVICE, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extTexGetHandle1(void *, LPDIRECT3DDEVICE, LPD3DTEXTUREHANDLE);
HRESULT WINAPI extTexGetHandle2(void *, LPDIRECT3DDEVICE2, LPD3DTEXTUREHANDLE);
HRESULT WINAPI extTexPaletteChanged1(void *, DWORD, DWORD);
HRESULT WINAPI extTexPaletteChanged2(void *, DWORD, DWORD);
HRESULT WINAPI extTexLoad1(void *, LPDIRECT3DTEXTURE);
HRESULT WINAPI extTexLoad2(void *, LPDIRECT3DTEXTURE);
HRESULT WINAPI extTexUnload(void *);
#endif // TRACETEXTURE


#ifdef TRACEALL
typedef HRESULT (WINAPI *Optimize_Type)(void *, DWORD);
typedef HRESULT (WINAPI *Validate_Type)(void *, LPDWORD, LPD3DVALIDATECALLBACK, LPVOID, DWORD);
typedef HRESULT (WINAPI *EBInitialize_Type)(void *, LPDIRECT3DDEVICE, LPD3DEXECUTEBUFFERDESC);
Optimize_Type pOptimize;
Validate_Type pValidate;
EBInitialize_Type pEBInitialize;
HRESULT WINAPI extOptimize(void *, DWORD);
HRESULT WINAPI extValidate(void *, LPDWORD, LPD3DVALIDATECALLBACK, LPVOID, DWORD);
HRESULT WINAPI extEBInitialize(void *, LPDIRECT3DDEVICE, LPD3DEXECUTEBUFFERDESC);
#endif
typedef HRESULT (WINAPI *EBLock_Type)(void *, LPD3DEXECUTEBUFFERDESC);
typedef HRESULT (WINAPI *EBUnlock_Type)(void *);
typedef HRESULT (WINAPI *SetExecuteData_Type)(void *, LPD3DEXECUTEDATA);
typedef HRESULT (WINAPI *GetExecuteData_Type)(void *, LPD3DEXECUTEDATA);
EBLock_Type pEBLock;
EBUnlock_Type pEBUnlock;
SetExecuteData_Type pSetExecuteData;
GetExecuteData_Type pGetExecuteData;
HRESULT WINAPI extEBLock(void *, LPD3DEXECUTEBUFFERDESC);
HRESULT WINAPI extEBUnlock(void *);
HRESULT WINAPI extSetExecuteData(void *, LPD3DEXECUTEDATA);
HRESULT WINAPI extGetExecuteData(void *, LPD3DEXECUTEDATA);

//----------------------------------------------------------------------//
// Hooking procedures
//----------------------------------------------------------------------//

// v2.05.45:
// BEWARE: since both these calls seem to be used internally by COM methods, QueryInterface in particular,
// a hot patching creates the risk of hooking these objects with the wrong interface version, then creating
// a huge mess. Better leave them patched at IAT level only, and maybe only enabling hot patching in
// exceptional cases. This fixes "WhiteWater Rapids" attempt to force 16 bit desktop through hot patching.

static HookEntryEx_Type d3dHooks[] = {
    {HOOK_IAT_CANDIDATE, 0x00, "Direct3DCreate", (FARPROC)NULL, (FARPROC *) &pDirect3DCreate, (FARPROC)extDirect3DCreate},
    {HOOK_IAT_CANDIDATE, 0x00, "Direct3DCreateDevice", (FARPROC)NULL, (FARPROC *) &pDirect3DCreateDevice, (FARPROC)extDirect3DCreateDevice},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

void HookDirect3D(HMODULE module, char *name) {
    LPDIRECT3D lpd3d = NULL;
    int version = dxw.dwTargetDDVersion;
    OutTraceDW("HookDirect3D: module=%#x name=%s version=%d\n", module, name, version);
    HookLibraryEx(module, d3dHooks, name);
    if(dxw.dwFlags12 & DIRECTXREPLACE) {
        //if(IsHookedBlock(d3dHooks)){ -- better force operation in any case?
        char path[MAX_PATH];
        sprintf(path, "%salt.dll\\%s", GetDxWndPath(), name);
        PinLibraryEx(d3dHooks, path);
        OutTrace("HookDirect3D: alt.dll loaded path=%s\n", path);
        //}
    }
}

void HookDirect3D16(HMODULE module) {
    HookDirect3D(module, "d3dim.dll");
}

void HookDirect3D7(HMODULE module) {
    HookDirect3D(module, "d3dim700.dll");
}

FARPROC Remap_d3d7_ProcAddress(LPCSTR proc, HMODULE hModule) {
    if (!strcmp(proc, "Direct3DCreate") && !pDirect3DCreate) {
        pDirect3DCreate = (Direct3DCreate_Type)(*pGetProcAddress)(hModule, proc);
        OutTraceD3D("GetProcAddress: hooking proc=%s at addr=%#x\n", ProcToString(proc), pDirect3DCreate);
        return (FARPROC)extDirect3DCreate;
    }
    if (!strcmp(proc, "Direct3DCreateDevice") && !pDirect3DCreateDevice) {
        pDirect3DCreateDevice = (Direct3DCreateDevice_Type)(*pGetProcAddress)(hModule, proc);
        OutTraceD3D("GetProcAddress: hooking proc=%s at addr=%#x\n", ProcToString(proc), pDirect3DCreateDevice);
        return (FARPROC)extDirect3DCreateDevice;
    }
    // NULL -> keep the original call address
    return NULL;
}

void HookDirect3DSession(LPDIRECTDRAW *lplpdd, int d3dversion) {
    OutTraceD3D("HookDirect3DSession: d3d=%#x d3dversion=%d\n", *lplpdd, d3dversion);
    if(dxw.dwFlags9 & D3DRESOLUTIONHACK) LegacyD3DResolutionHack(d3dversion);
    switch(d3dversion) {
    case 1:
        SetHook((void *)(**(DWORD **)lplpdd +   0), extQueryInterfaceD31, (void **)&pQueryInterfaceD31, "QueryInterface(D3S1)");
#ifdef TRACEMATERIAL
        SetHook((void *)(**(DWORD **)lplpdd +  12), extInitialize, (void **)&pInitialize, "Initialize(1)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd +  16), extEnumDevices1, (void **)&pEnumDevices1, "EnumDevices(1)");
        SetHook((void *)(**(DWORD **)lplpdd +  20), extCreateLight1, (void **)&pCreateLight1, "CreateLight(1)");
#ifdef TRACEMATERIAL
        SetHook((void *)(**(DWORD **)lplpdd +  24), extCreateMaterial1, (void **)&pCreateMaterial1, "CreateMaterial(1)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd +  28), extCreateViewport1, (void **)&pCreateViewport1, "CreateViewport(1)");
        SetHook((void *)(**(DWORD **)lplpdd +  32), extFindDevice1, (void **)&pFindDevice1, "FindDevice(1)");
        break;
    case 2:
        SetHook((void *)(**(DWORD **)lplpdd +   0), extQueryInterfaceD32, (void **)&pQueryInterfaceD32, "QueryInterface(D3S2)");
        SetHook((void *)(**(DWORD **)lplpdd +  12), extEnumDevices2, (void **)&pEnumDevices2, "EnumDevices(2)");
        SetHook((void *)(**(DWORD **)lplpdd +  16), extCreateLight2, (void **)&pCreateLight2, "CreateLight(2)");
#ifdef TRACEMATERIAL
        SetHook((void *)(**(DWORD **)lplpdd +  20), extCreateMaterial2, (void **)&pCreateMaterial2, "CreateMaterial(2)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd +  24), extCreateViewport2, (void **)&pCreateViewport2, "CreateViewport(2)");
        SetHook((void *)(**(DWORD **)lplpdd +  28), extFindDevice2, (void **)&pFindDevice2, "FindDevice(2)");
        SetHook((void *)(**(DWORD **)lplpdd +  32), extCreateDevice2, (void **)&pCreateDevice2, "CreateDevice(D3D2)");
        break;
    case 3:
        SetHook((void *)(**(DWORD **)lplpdd +   0), extQueryInterfaceD33, (void **)&pQueryInterfaceD33, "QueryInterface(D3S3)");
        SetHook((void *)(**(DWORD **)lplpdd +  12), extEnumDevices3, (void **)&pEnumDevices3, "EnumDevices(3)");
        SetHook((void *)(**(DWORD **)lplpdd +  16), extCreateLight3, (void **)&pCreateLight3, "CreateLight(3)");
#ifdef TRACEMATERIAL
        SetHook((void *)(**(DWORD **)lplpdd +  20), extCreateMaterial3, (void **)&pCreateMaterial3, "CreateMaterial(3)");
#endif
        SetHook((void *)(**(DWORD **)lplpdd +  24), extCreateViewport3, (void **)&pCreateViewport3, "CreateViewport(3)");
        SetHook((void *)(**(DWORD **)lplpdd +  28), extFindDevice3, (void **)&pFindDevice3, "FindDevice(3)");
        SetHook((void *)(**(DWORD **)lplpdd +  32), extCreateDevice3, (void **)&pCreateDevice3, "CreateDevice(D3D3)");
        SetHook((void *)(**(DWORD **)lplpdd +  40), extEnumZBufferFormats3, (void **)&pEnumZBufferFormats3, "EnumZBufferFormats(D3D3)");
        break;
    case 7:
        SetHook((void *)(**(DWORD **)lplpdd +   0), extQueryInterfaceD37, (void **)&pQueryInterfaceD37, "QueryInterface(D3S7)");
        SetHook((void *)(**(DWORD **)lplpdd +  12), extEnumDevices7, (void **)&pEnumDevices7, "EnumDevices(7)");
        SetHook((void *)(**(DWORD **)lplpdd +  16), extCreateDevice7, (void **)&pCreateDevice7, "CreateDevice(D3D7)");
        SetHook((void *)(**(DWORD **)lplpdd +  24), extEnumZBufferFormats7, (void **)&pEnumZBufferFormats7, "EnumZBufferFormats(D3D7)");
        break;
    }
}

void HookDirect3DDevice(void **lpd3ddev, int d3dversion) {
    OutTraceD3D("HookDirect3DDevice: d3ddev=%#x d3dversion=%d\n", *lpd3ddev, d3dversion);
    switch(d3dversion) {
    case 1:
        SetHook((void *)(**(DWORD **)lpd3ddev +   0), extQueryInterfaceD3D1, (void **)&pQueryInterfaceD3D1, "QueryInterface(D3DD1)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lpd3ddev +   8), extReleaseD3D1, (void **)&pReleaseD3D1, "ReleaseD3D(1)");
#endif
        SetHook((void *)(**(DWORD **)lpd3ddev +  16), extD3DGetCaps1, (void **)&pD3DGetCaps1, "GetCaps(1)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  20), extSwapTextureHandles, (void **)&pSwapTextureHandles, "SwapTextureHandles(1)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  24), extCreateExecuteBuffer, (void **)&pCreateExecuteBuffer, "CreateExecuteBuffer(1)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  32), extExecute, (void **)&pExecute, "Execute(1)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  36), extAddViewport1, (void **)&pAddViewport1, "AddViewport(1)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  40), extDeleteViewport1, (void **)&pDeleteViewport1, "DeleteViewport(1)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  44), extNextViewport1, (void **)&pNextViewport1, "NextViewport(1)");
#ifdef TRACEMATERIAL
        SetHook((void *)(**(DWORD **)lpd3ddev +  48), extPick, (void **)&pPick, "Pick(1)");
#endif
        SetHook((void *)(**(DWORD **)lpd3ddev +  56), extEnumTextureFormats1, (void **)&pEnumTextureFormats1, "EnumTextureFormats(1)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  76), extBeginScene1, (void **)&pBeginScene1, "BeginScene(1)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  80), extEndScene1, (void **)&pEndScene1, "EndScene(1)");
        break;
    case 2:
        SetHook((void *)(**(DWORD **)lpd3ddev +   0), extQueryInterfaceD3D2, (void **)&pQueryInterfaceD3D2, "QueryInterface(D3DD2)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lpd3ddev +   8), extReleaseD3D2, (void **)&pReleaseD3D2, "ReleaseD3D(2)");
#endif
        SetHook((void *)(**(DWORD **)lpd3ddev +  12), extD3DGetCaps2, (void **)&pD3DGetCaps2, "GetCaps(2)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  16), extSwapTextureHandles2, (void **)&pSwapTextureHandles2, "SwapTextureHandles(2)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  24), extAddViewport2, (void **)&pAddViewport2, "AddViewport(2)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  28), extDeleteViewport2, (void **)&pDeleteViewport2, "DeleteViewport(2)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  32), extNextViewport2, (void **)&pNextViewport2, "NextViewport(2)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  36), extEnumTextureFormats2, (void **)&pEnumTextureFormats2, "EnumTextureFormats(2)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  40), extBeginScene2, (void **)&pBeginScene2, "BeginScene(2)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  44), extEndScene2, (void **)&pEndScene2, "EndScene(2)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  52), extSetCurrentViewport2, (void **)&pSetCurrentViewport2, "SetCurrentViewport(2)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  56), extGetCurrentViewport2, (void **)&pGetCurrentViewport2, "GetCurrentViewport(2)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  92), extSetRenderState2, (void **)&pSetRenderState2, "SetRenderState(2)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lpd3ddev + 104), extSetTransform2, (void **)&pSetTransform2, "SetTransform(2)"); // v2.04.78 fix
        SetHook((void *)(**(DWORD **)lpd3ddev + 116), extDrawPrimitive2, (void **)&pDrawPrimitive2, "DrawPrimitive(2)");
#endif
        if(pSetRenderState2) {
            if(dxw.dwFlags2 & WIREFRAME)(*pSetRenderState2)(*lpd3ddev, D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
            if(dxw.dwFlags4 & DISABLEFOGGING) (*pSetRenderState2)(*lpd3ddev, D3DRENDERSTATE_FOGENABLE, FALSE);
            if(dxw.dwDFlags & ZBUFFERALWAYS) (*pSetRenderState2)(*lpd3ddev, D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
        }
        break;
    case 3:
        SetHook((void *)(**(DWORD **)lpd3ddev +   0), extQueryInterfaceD3D3, (void **)&pQueryInterfaceD3D3, "QueryInterface(D3DD3)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lpd3ddev +   8), extReleaseD3D3, (void **)&pReleaseD3D3, "ReleaseD3D(3)");
#endif
        SetHook((void *)(**(DWORD **)lpd3ddev +  12), extD3DGetCaps3, (void **)&pD3DGetCaps3, "GetCaps(3)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  20), extAddViewport3, (void **)&pAddViewport3, "AddViewport(3)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  32), extEnumTextureFormats3, (void **)&pEnumTextureFormats3, "EnumTextureFormats(3)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  36), extBeginScene3, (void **)&pBeginScene3, "BeginScene(3)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  40), extEndScene3, (void **)&pEndScene3, "EndScene(3)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  48), extSetCurrentViewport3, (void **)&pSetCurrentViewport3, "SetCurrentViewport(3)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  52), extGetCurrentViewport3, (void **)&pGetCurrentViewport3, "GetCurrentViewport(3)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  88), extSetRenderState3, (void **)&pSetRenderState3, "SetRenderState(3)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  96), extSetLightState3, (void **)&pSetLightState3, "SetLightState(3)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lpd3ddev + 100), extSetTransform3, (void **)&pSetTransform3, "SetTransform(3)");
        SetHook((void *)(**(DWORD **)lpd3ddev + 112), extDrawPrimitive3, (void **)&pDrawPrimitive3, "DrawPrimitive(3)");
#endif
        SetHook((void *)(**(DWORD **)lpd3ddev + 152), extSetTexture3, (void **)&pSetTexture3, "SetTexture(3)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lpd3ddev + 156), extGetTextureStageState3, (void **)&pGetTextureStageState3, "GetTextureStageState(3)");
        SetHook((void *)(**(DWORD **)lpd3ddev + 160), extSetTextureStageState3, (void **)&pSetTextureStageState3, "SetTextureStageState(3)");
#endif
        if(pSetRenderState3) {
            if(dxw.dwFlags2 & WIREFRAME)(*pSetRenderState3)(*lpd3ddev, D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
            if(dxw.dwFlags4 & DISABLEFOGGING) (*pSetRenderState3)(*lpd3ddev, D3DRENDERSTATE_FOGENABLE, FALSE);
            if(dxw.dwDFlags & ZBUFFERALWAYS) (*pSetRenderState3)(*lpd3ddev, D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
        }
        break;
    case 7:
        SetHook((void *)(**(DWORD **)lpd3ddev +   0), extQueryInterfaceD3D7, (void **)&pQueryInterfaceD3D7, "QueryInterface(D3DD7)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lpd3ddev +   8), extReleaseD3D7, (void **)&pReleaseD3D7, "ReleaseD3D(7)");
#endif
        SetHook((void *)(**(DWORD **)lpd3ddev +  12), extD3DGetCaps7, (void **)&pD3DGetCaps7, "GetCaps(7)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  16), extEnumTextureFormats7, (void **)&pEnumTextureFormats7, "EnumTextureFormats(7)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  20), extBeginScene7, (void **)&pBeginScene7, "BeginScene(7)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  24), extEndScene7, (void **)&pEndScene7, "EndScene(7)");
        // 28: GetDirect3D
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lpd3ddev +  32), extSetRenderTarget7, (void **)&pSetRenderTarget7, "SetRenderTarget(7)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  36), extGetRenderTarget7, (void **)&pGetRenderTarget7, "GetRenderTarget(7)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  44), extSetTransform7, (void **)&pSetTransform7, "SetTransform(7)");
#endif
        SetHook((void *)(**(DWORD **)lpd3ddev +  52), extSetViewport7, (void **)&pSetViewport7, "SetViewport(7)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  60), extGetViewport7, (void **)&pGetViewport7, "GetViewport(7)");
        SetHook((void *)(**(DWORD **)lpd3ddev +  72), extSetLight7, (void **)&pSetLight7, "SetLight(7)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lpd3ddev +  76), extGetLight7, (void **)&pGetLight7, "GetLight(7)");
#endif
        SetHook((void *)(**(DWORD **)lpd3ddev +  80), extSetRenderState7, (void **)&pSetRenderState7, "SetRenderState(7)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lpd3ddev + 100), extDrawPrimitive7, (void **)&pDrawPrimitive7, "DrawPrimitive(7)");
#endif
        SetHook((void *)(**(DWORD **)lpd3ddev + 140), extSetTexture7, (void **)&pSetTexture7, "SetTexture(7)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lpd3ddev + 144), extGetTextureStageState7, (void **)&pGetTextureStageState7, "GetTextureStageState(7)");
        SetHook((void *)(**(DWORD **)lpd3ddev + 148), extSetTextureStageState7, (void **)&pSetTextureStageState7, "SetTextureStageState(7)");
#endif
        if(pSetRenderState7) {
            if(dxw.dwFlags2 & WIREFRAME)(*pSetRenderState7)(*lpd3ddev, D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
            if(dxw.dwFlags4 & DISABLEFOGGING) (*pSetRenderState7)(*lpd3ddev, D3DRENDERSTATE_FOGENABLE, FALSE);
            if(dxw.dwDFlags & ZBUFFERALWAYS) (*pSetRenderState7)(*lpd3ddev, D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
        }
        break;
    }
}

void HookViewport(LPDIRECT3DVIEWPORT *lpViewport, int d3dversion) {
    OutTraceD3D("HookViewport: Viewport=%#x d3dversion=%d\n", *lpViewport, d3dversion);
    switch(d3dversion) {
    case 1:
        SetHook((void *)(**(DWORD **)lpViewport +  12), extInitializeVP, (void **)&pInitializeVP, "Initialize(VP1)");
        SetHook((void *)(**(DWORD **)lpViewport +  16), extGetViewport1, (void **)&pGetViewport1, "GetViewport(1)");
        SetHook((void *)(**(DWORD **)lpViewport +  20), extSetViewport1, (void **)&pSetViewport1, "SetViewport(1)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lpViewport +  24), extTransformVertices1, (void **)&pTransformVertices1, "TransformVertices(1)");
        SetHook((void *)(**(DWORD **)lpViewport +  28), extLightElements1, (void **)&pLightElements1, "LightElements(1)");
        SetHook((void *)(**(DWORD **)lpViewport +  32), extSetBackground1, (void **)&pSetBackground1, "SetBackground(1)");
        SetHook((void *)(**(DWORD **)lpViewport +  36), extGetBackground1, (void **)&pGetBackground1, "GetBackground(1)");
        SetHook((void *)(**(DWORD **)lpViewport +  40), extSetBackgroundDepth1, (void **)&pSetBackgroundDepth1, "SetBackgroundDepth(1)");
        SetHook((void *)(**(DWORD **)lpViewport +  44), extGetBackgroundDepth1, (void **)&pGetBackgroundDepth1, "GetBackgroundDepth(1)");
        SetHook((void *)(**(DWORD **)lpViewport +  48), extClear1, (void **)&pClear1, "Clear(1)");
        SetHook((void *)(**(DWORD **)lpViewport +  52), extAddLight1, (void **)&pAddLight1, "AddLight(1)");
        SetHook((void *)(**(DWORD **)lpViewport +  56), extDeleteLight1, (void **)&pDeleteLight1, "DeleteLight(1)");
        SetHook((void *)(**(DWORD **)lpViewport +  60), extNextLight1, (void **)&pNextLight1, "NextLight(1)");
        // to do: why Clear method crashes in "Forsaken" in emulation and GDI mode??? fixed ???
        SetHook((void *)(**(DWORD **)lpViewport +  48), extViewportClear, (void **)&pViewportClear, "Clear(1)");
#endif
        break;
    case 2:
        SetHook((void *)(**(DWORD **)lpViewport +  12), extInitializeVP, (void **)&pInitializeVP, "Initialize(VP2)");
        SetHook((void *)(**(DWORD **)lpViewport +  16), extGetViewport2, (void **)&pGetViewport2, "GetViewport(2)");
        SetHook((void *)(**(DWORD **)lpViewport +  20), extSetViewport2, (void **)&pSetViewport2, "SetViewport(2)");
        SetHook((void *)(**(DWORD **)lpViewport +  64), extGetViewport2_2, (void **)&pGetViewport2_2, "GetViewport2(2)");
        SetHook((void *)(**(DWORD **)lpViewport +  68), extSetViewport2_2, (void **)&pSetViewport2_2, "SetViewport2(2)");
        break;
    case 3:
        SetHook((void *)(**(DWORD **)lpViewport +  12), extInitializeVP, (void **)&pInitializeVP, "Initialize(VP3)");
        SetHook((void *)(**(DWORD **)lpViewport +  16), extGetViewport3, (void **)&pGetViewport3, "GetViewport(3)");
        SetHook((void *)(**(DWORD **)lpViewport +  20), extSetViewport3, (void **)&pSetViewport3, "SetViewport(3)");
#ifdef TRACEALL
        SetHook((void *)(**(DWORD **)lpViewport +  24), extTransformVertices3, (void **)&pTransformVertices3, "TransformVertices(3)");
        SetHook((void *)(**(DWORD **)lpViewport +  28), extLightElements3, (void **)&pLightElements3, "LightElements(3)");
        SetHook((void *)(**(DWORD **)lpViewport +  32), extSetBackground3, (void **)&pSetBackground3, "SetBackground(3)");
        SetHook((void *)(**(DWORD **)lpViewport +  36), extGetBackground3, (void **)&pGetBackground3, "GetBackground(3)");
        SetHook((void *)(**(DWORD **)lpViewport +  40), extSetBackgroundDepth3, (void **)&pSetBackgroundDepth3, "SetBackgroundDepth(3)");
        SetHook((void *)(**(DWORD **)lpViewport +  44), extGetBackgroundDepth3, (void **)&pGetBackgroundDepth3, "GetBackgroundDepth(3)");
        SetHook((void *)(**(DWORD **)lpViewport +  48), extClear3, (void **)&pClear3, "Clear(3)");
        SetHook((void *)(**(DWORD **)lpViewport +  52), extAddLight3, (void **)&pAddLight3, "AddLight(3)");
        SetHook((void *)(**(DWORD **)lpViewport +  56), extDeleteLight3, (void **)&pDeleteLight3, "DeleteLight(3)");
        SetHook((void *)(**(DWORD **)lpViewport +  60), extNextLight3, (void **)&pNextLight3, "NextLight(3)");
#endif
        SetHook((void *)(**(DWORD **)lpViewport +  64), extGetViewport2_3, (void **)&pGetViewport2_3, "GetViewport2(3)");
        SetHook((void *)(**(DWORD **)lpViewport +  68), extSetViewport2_3, (void **)&pSetViewport2_3, "SetViewport2(3)");
        break;
    case 7:
        break;
    }
}

#ifdef TRACEMATERIAL
void HookMaterial(LPDIRECT3DMATERIAL *lpMaterial, int d3dversion) {
    OutTraceD3D("HookMaterial: Material=%#x d3dversion=%d\n", *lpMaterial, d3dversion);
    switch(d3dversion) {
    case 1:
        SetHook((void *)(**(DWORD **)lpMaterial +  16), extSetMaterial, (void **)&pSetMaterial, "SetMaterial");
        SetHook((void *)(**(DWORD **)lpMaterial +  20), extGetMaterial, (void **)&pGetMaterial, "GetMaterial");
        break;
    default:
        SetHook((void *)(**(DWORD **)lpMaterial +  12), extSetMaterial, (void **)&pSetMaterial, "SetMaterial");
        SetHook((void *)(**(DWORD **)lpMaterial +  16), extGetMaterial, (void **)&pGetMaterial, "GetMaterial");
        break;
    }
}
#endif

#ifdef TRACETEXTURE
typedef ULONG	(WINAPI *AddRef_Type)(void *);
typedef ULONG	(WINAPI *Release_Type)(void *);
QueryInterface_Type pQueryInterfaceD3DT1, pQueryInterfaceD3DT2;
AddRef_Type pAddRefD3DT1, pAddRefD3DT2;
Release_Type pReleaseD3DT1, pReleaseD3DT2;
HRESULT WINAPI extQueryInterfaceD3DT1(LPVOID, REFIID, void **);
HRESULT WINAPI extQueryInterfaceD3DT2(LPVOID, REFIID, void **);
ULONG WINAPI extAddRefD3DT1(LPVOID);
ULONG WINAPI extAddRefD3DT2(LPVOID);
ULONG WINAPI extReleaseD3DT1(LPVOID);
ULONG WINAPI extReleaseD3DT2(LPVOID);
#endif

void HookTexture(LPVOID *lpTexture, int version) {
    OutTraceD3D("HookTexture: Texture=%#x version=%d\n", *lpTexture, version);
    switch(version) {
    case 1:
#ifdef TRACETEXTURE
        SetHook((void *)(**(DWORD **)lpTexture +   0), extQueryInterfaceD3DT1, (void **)&pQueryInterfaceD3DT1, "QueryInterface(D3DT1)");
        SetHook((void *)(**(DWORD **)lpTexture +   4), extAddRefD3DT1, (void **)&pAddRefD3DT1, "AddRef(D3DT1)");
        SetHook((void *)(**(DWORD **)lpTexture +   8), extReleaseD3DT1, (void **)&pReleaseD3DT1, "Release(D3DT1)");
        SetHook((void *)(**(DWORD **)lpTexture +  12), extTexInitialize, (void **)&pTInitialize, "Initialize(T1)");
        SetHook((void *)(**(DWORD **)lpTexture +  16), extTexGetHandle1, (void **)&pTGetHandle1, "GetHandle(T1)");
        SetHook((void *)(**(DWORD **)lpTexture +  20), extTexPaletteChanged1, (void **)&pTPaletteChanged1, "PaletteChanged(T1)");
        SetHook((void *)(**(DWORD **)lpTexture +  24), extTexLoad1, (void **)&pTLoad1, "Load(T1)");
        SetHook((void *)(**(DWORD **)lpTexture +  28), extTexUnload, (void **)&pTUnload, "Unload(T1)");
#endif
        break;
    case 2:
#ifdef TRACETEXTURE
        SetHook((void *)(**(DWORD **)lpTexture +   0), extQueryInterfaceD3DT2, (void **)&pQueryInterfaceD3DT2, "QueryInterface(D3DT2)");
        SetHook((void *)(**(DWORD **)lpTexture +   4), extAddRefD3DT2, (void **)&pAddRefD3DT2, "AddRef(D3DT2)");
        SetHook((void *)(**(DWORD **)lpTexture +   8), extReleaseD3DT2, (void **)&pReleaseD3DT2, "Release(D3DT2)");
        SetHook((void *)(**(DWORD **)lpTexture +  12), extTexGetHandle2, (void **)&pTGetHandle2, "GetHandle(T2)");
        SetHook((void *)(**(DWORD **)lpTexture +  16), extTexPaletteChanged2, (void **)&pTPaletteChanged2, "PaletteChanged(T2)");
        SetHook((void *)(**(DWORD **)lpTexture +  20), extTexLoad2, (void **)&pTLoad2, "Load(T2)");
#endif
        break;
    }
}
#ifdef TRACETEXTURE
static ULONG extAddRefD3DT(char *api, AddRef_Type pAddRef, LPVOID lpTex) {
    ULONG ret;
    ret = (*pAddRef)(lpTex);
    OutTrace("%s: ref=%d\n", api, ret);
    return ret;
}

ULONG WINAPI extAddRefD3DT1(LPVOID lpTex) {
    return extAddRefD3DT("Direct3DTexture::AddRef", pAddRefD3DT1, lpTex);
}
ULONG WINAPI extAddRefD3DT2(LPVOID lpTex) {
    return extAddRefD3DT("Direct3DTexture2::AddRef", pAddRefD3DT2, lpTex);
}

static ULONG extReleaseD3DT(char *api, Release_Type pRelease, LPVOID lpTex) {
    ULONG ret;
    ret = (*pRelease)(lpTex);
    OutTrace("%s: ref=%d\n", api, ret);
    return ret;
}

ULONG WINAPI extReleaseD3DT1(LPVOID lpTex) {
    return extReleaseD3DT("Direct3DTexture::Release", pReleaseD3DT1, lpTex);
}
ULONG WINAPI extReleaseD3DT2(LPVOID lpTex) {
    return extReleaseD3DT("Direct3DTexture2::Release", pReleaseD3DT2, lpTex);
}

HRESULT WINAPI extQueryInterfaceD3DT1(void *lpTex, REFIID riid, void **ppvObj) {
    return extQueryInterfaceDX(1, pQueryInterfaceD3DT1, lpTex, riid, ppvObj);
}
HRESULT WINAPI extQueryInterfaceD3DT2(void *lpTex, REFIID riid, void **ppvObj) {
    return extQueryInterfaceDX(2, pQueryInterfaceD3DT2, lpTex, riid, ppvObj);
}
#endif // TRACETEXTURE

void HookExecuteBuffer(LPDIRECT3DEXECUTEBUFFER *lplpeb) {
    OutTraceD3D("HookExecuteBuffer: lpeb=%#x\n", *lplpeb);
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)lplpeb +  12), extEBInitialize, (void **)&pEBInitialize, "Initialize(D3DEB)");
#endif
    SetHook((void *)(**(DWORD **)lplpeb +  16), extEBLock, (void **)&pEBLock, "Lock(D3DEB)");
    SetHook((void *)(**(DWORD **)lplpeb +  20), extEBUnlock, (void **)&pEBUnlock, "Unlock(D3DEB)");
    SetHook((void *)(**(DWORD **)lplpeb +  24), extSetExecuteData, (void **)&pSetExecuteData, "SetExecuteData(D3DEB)");
    SetHook((void *)(**(DWORD **)lplpeb +  28), extGetExecuteData, (void **)&pGetExecuteData, "GetExecuteData(D3DEB)");
#ifdef TRACEALL
    SetHook((void *)(**(DWORD **)lplpeb +  32), extValidate, (void **)&pValidate, "Validate(D3DEB)");
    SetHook((void *)(**(DWORD **)lplpeb +  36), extOptimize, (void **)&pOptimize, "Optimize(D3DEB)");
#endif
}

//----------------------------------------------------------------------//
// Auxiliary functions
//----------------------------------------------------------------------//

typedef struct {
    DWORD state;
    DWORD val;
} D3DSTATEX, *LPD3DSTATEX;

static void dxwForceState(LPDIRECT3DEXECUTEBUFFER lpeb, DWORD state, DWORD val) {
    D3DEXECUTEBUFFERDESC ebd;
    D3DEXECUTEDATA ed;
    LPD3DINSTRUCTION lpInst;
    int fix = 0;
    HRESULT res;
    OutTraceDW("ForceState: state=%#x val=%#x\n", state, val);
    ed.dwSize = sizeof(D3DEXECUTEDATA);
    if(res = (*pGetExecuteData)(lpeb, &ed)) {
        OutTraceE("ForceState: GetExecuteData ERROR res=%#x(%s)\n", res, ExplainDDError(res));
        return;
    }
    ebd.dwSize = sizeof(D3DEXECUTEBUFFERDESC);
    if(res = (*pEBLock)(lpeb, &ebd)) {
        OutTraceE("ForceState: Lock ERROR res=%#x(%s)\n", res, ExplainDDError(res));
        return;
    }
    int len;
    lpInst = (LPD3DINSTRUCTION)((LPBYTE)ebd.lpData + ed.dwInstructionOffset);
    len = (int)ed.dwInstructionLength;
    for(DWORD i = 0; len > 0; i++) {
        if((lpInst->bOpcode < D3DOP_POINT) || (lpInst->bOpcode > D3DOP_SETSTATUS)) break;
        if(lpInst->bOpcode == D3DOP_EXIT) break;
        if(lpInst->bOpcode == D3DOP_STATERENDER) {
            LPD3DSTATEX lpState = (LPD3DSTATEX)((LPBYTE)lpInst + sizeof(D3DINSTRUCTION));
            for (int j = 0; j < lpInst->wCount; j++) {
                if(lpState->state == state) {
                    lpState->val = val;
                    fix ++;
                }
                lpState ++;
            }
        }
        len -= (lpInst->bSize * lpInst->wCount);
        lpInst = (LPD3DINSTRUCTION)((LPBYTE)lpInst + sizeof(D3DINSTRUCTION) + (lpInst->bSize * lpInst->wCount));
    }
    if(res = (*pEBUnlock)(lpeb)) {
        OutTraceE("ForceState: Unlock ERROR res=%#x(%s)\n", res, ExplainDDError(res));
        return;
    }
    OutTraceDW("ForceState: eb=%#x state=%#x val=%#x fixed=%d\n", lpeb, state, val, fix);
}

static void dxwFakeWireFrame(LPDIRECT3DEXECUTEBUFFER lpeb) {
    D3DEXECUTEBUFFERDESC ebd;
    D3DEXECUTEDATA ed;
    LPD3DINSTRUCTION lpInst;
    int fix = 0;
    ed.dwSize = sizeof(D3DEXECUTEDATA);
    if((*pGetExecuteData)(lpeb, &ed)) return;
    ebd.dwSize = sizeof(D3DEXECUTEBUFFERDESC);
    if((*pEBLock)(lpeb, &ebd)) return;
    int len;
    lpInst = (LPD3DINSTRUCTION)((LPBYTE)ebd.lpData + ed.dwInstructionOffset);
    len = (int)ed.dwInstructionLength;
    for(DWORD i = 0; len > 0; i++) {
        if((lpInst->bOpcode < D3DOP_POINT) || (lpInst->bOpcode > D3DOP_SETSTATUS)) break;
        if(lpInst->bOpcode == D3DOP_EXIT) break;
        if(lpInst->bOpcode == D3DOP_TRIANGLE) {
            lpInst->bOpcode = D3DOP_LINE;
            LPWORD lpVertex = (LPWORD)((LPBYTE)lpInst + sizeof(D3DINSTRUCTION));
            for (int j = 0; j < lpInst->wCount; j++) {
                lpVertex[3] = lpVertex[2];
                lpVertex[2] = lpVertex[1];
                lpVertex += 4;
            }
            lpInst->wCount *= 2;
            lpInst->bSize /= 2;
        }
        len -= (lpInst->bSize * lpInst->wCount);
        lpInst = (LPD3DINSTRUCTION)((LPBYTE)lpInst + sizeof(D3DINSTRUCTION) + (lpInst->bSize * lpInst->wCount));
    }
    (*pEBUnlock)(lpeb);
    //OutTraceD3D("ForceState: eb=%#x state=%#x val=%#x fixed=%d\n", lpeb, state, val, fix);
}

static void LegacyD3DResolutionHack(int d3dversion) {
    HMODULE hD3D;
    char *module;
    extern void dxPatchModule(HMODULE, const BYTE *, int, const BYTE *, int, BOOL);
    OutTraceDW("LegacyD3DResolutionHack\n");
#if 0
    // first occurrence
    const BYTE wantedBytes[] =  { 0x75, 0xEC, 0xB8, 0x00, 0x08, 0x00, 0x00, 0x39 };
    const BYTE updatedBytes[] = { 0x75, 0xEC, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF, 0x39 };
    // second occurrence
    //const BYTE wantedBytes[] = { 0x74, 0xDF, 0xB8, 0x00, 0x08, 0x00, 0x00, 0x39 };
    //const BYTE updatedBytes[] = { 0x74, 0xDF, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF, 0x39 };
#else
    const BYTE wantedBytes[] =  { 0xB8, 0x00, 0x08, 0x00, 0x00, 0x39 };
    const BYTE updatedBytes[] = { 0xB8, 0xFF, 0xFF, 0xFF, 0xFF, 0x39 };
#endif
    module = (d3dversion == 7) ? "d3dim700.dll" : "d3dim.dll";
    hD3D = GetModuleHandle(module);
    if(hD3D)
        dxPatchModule(hD3D, wantedBytes, sizeof(wantedBytes), updatedBytes, sizeof(updatedBytes), TRUE);
    else
        OutTraceE("LegacyD3DResolutionHack: no module %s\n", module);
}

static void InsertPatchingExecuteBuffer(void *d3dd, DWORD state, DWORD val) {
    HRESULT res;
    D3DEXECUTEBUFFERDESC ebdesc;
    D3DEXECUTEDATA ed;
    LPDIRECT3DEXECUTEBUFFER lpeb;
    LPD3DINSTRUCTION lpInstruction;
    LPD3DSTATEX lpState;
    LPBYTE lpBuf;
    int dwSize;
    OutTraceDW("Patch: d3dd=%#x state=%#x val=%#x\n", d3dd, state, val);
    // reserve space for the following:
    // instruction D3DOP_STATERENDER with 1 D3DRENDERSTATE_ZFUNC state
    // enstruction EXIT
    dwSize = (2 * sizeof(D3DINSTRUCTION)) + sizeof(D3DSTATE);
    memset(&ebdesc, 0, sizeof(D3DEXECUTEBUFFERDESC));
    ebdesc.dwSize = sizeof(D3DEXECUTEBUFFERDESC);
    ebdesc.dwFlags = D3DDEB_BUFSIZE;
    ebdesc.dwBufferSize = dwSize;
    res = (*pCreateExecuteBuffer)(d3dd, &ebdesc, &lpeb, NULL);
    if(res) {
        OutTraceDW("Patch: CreateExecuteBuffer error ret=%d(%s)\n", res, ExplainDDError(res));
        return;
    }
    res = (*pEBLock)(lpeb, &ebdesc);
    if(res) {
        OutTraceE("Patch: Lock error ret=%d(%s)\n", res, ExplainDDError(res));
        return;
    }
    lpBuf = (LPBYTE)ebdesc.lpData;
    memset(lpBuf, 0, dwSize);
    lpInstruction = (LPD3DINSTRUCTION)lpBuf;
    lpBuf += sizeof(D3DINSTRUCTION);
    lpInstruction->bOpcode = D3DOP_STATERENDER;
    lpInstruction->wCount = 1;
    lpInstruction->bSize = sizeof(D3DSTATE);
    /*  possible values
        D3DCMP_NEVER               = 1,
        D3DCMP_LESS                = 2,
        D3DCMP_EQUAL               = 3,
        D3DCMP_LESSEQUAL           = 4,
        D3DCMP_GREATER             = 5,
        D3DCMP_NOTEQUAL            = 6,
        D3DCMP_GREATEREQUAL        = 7,
        D3DCMP_ALWAYS              = 8,
    */
    lpState = (LPD3DSTATEX)lpBuf;
    lpBuf += sizeof(LPD3DSTATEX);
    lpState->state = state;
    lpState->val = val;
    // the execute buffer must NOT be terminated with a D3DOP_EXIT instruction
    // or the effect won't be propagated to the following execute buffers!!!
    //lpInstruction = (LPD3DINSTRUCTION)lpBuf;
    //lpBuf += sizeof(D3DINSTRUCTION);
    //lpInstruction->bOpcode = D3DOP_EXIT;
    //lpInstruction->wCount = 0;
    //lpInstruction->bSize = 0;
    res = (*pEBUnlock)(lpeb);
    if(res) {
        OutTraceE("Patch: Unlock error ret=%#x(%s)\n", res, ExplainDDError(res));
        return;
    }
    memset(&ed, 0, sizeof(D3DEXECUTEDATA));
    ed.dwSize = sizeof(D3DEXECUTEDATA);
    ed.dwInstructionOffset = 0;
    ed.dwHVertexOffset = 0;
    ed.dwInstructionLength = dwSize;
    ed.dwVertexCount = 0;
    ed.dwVertexOffset = 0;
    // ed.dsStatus ???
    res = (*pSetExecuteData)(lpeb, &ed);
    if(res) {
        OutTraceE("Patch: SetExecuteData error ret=%#x(%s)\n", res, ExplainDDError(res));
        return;
    }
    res = (*pExecute)(d3dd, lpeb, lpCurrViewport, D3DEXECUTE_CLIPPED); // viewport? flags?
    if(res) {
        OutTraceE("Patch: Execute error ret=%#x(%s)\n", res, ExplainDDError(res));
        return;
    }
    (LPDIRECT3DEXECUTEBUFFER)lpeb->Release();
}

#define FORCEEBCAPS (FORCEHWVERTEXPROC|FORCESWVERTEXPROC|FORCEMXVERTEXPROC)

DWORD ForceExecuteBufferCaps(void) {
    DWORD ebcaps;
    switch(dxw.dwFlags10 & FORCEEBCAPS) {
    case FORCEHWVERTEXPROC:
        ebcaps = D3DDEBCAPS_VIDEOMEMORY;
        break;
    case FORCESWVERTEXPROC:
        ebcaps = D3DDEBCAPS_SYSTEMMEMORY;
        break;
    case FORCEMXVERTEXPROC:
        ebcaps = D3DDEBCAPS_VIDEOMEMORY | D3DDEBCAPS_SYSTEMMEMORY;
        break;
    }
    OutTraceDW("ForceExecuteBufferCaps: FORCE cap=%#x(%s)\n", ebcaps, sExecuteBufferCaps(ebcaps));
    return ebcaps;
}

//----------------------------------------------------------------------//
// Tracing
//----------------------------------------------------------------------//

#ifndef DXW_NOTRACES

static char *sFourCC(DWORD fcc) {
    static char sRet[5];
    char c;
    int i;
    char *t = &sRet[0];
    for(i = 0; i < 4; i++) {
        c = fcc & (0xFF);
        *t++ = isprint(c) ? c : '.';
        c = c >> 8;
    }
    *t = 0;
    return sRet;
}

char *DumpPixelFormat(LPDDPIXELFORMAT ddpfPixelFormat) {
    static char sBuf[512];
    char sItem[256];
    DWORD flags = ddpfPixelFormat->dwFlags;
    sprintf(sBuf, " PixelFormat flags=%#x(%s) BPP=%d",
            flags, ExplainPixelFormatFlags(flags), ddpfPixelFormat->dwRGBBitCount);
    if (flags & DDPF_RGB) {
        if (flags & DDPF_ALPHAPIXELS) {
            sprintf(sItem, " RGBA=(%#x,%#x,%#x,%#x)",
                    ddpfPixelFormat->dwRBitMask,
                    ddpfPixelFormat->dwGBitMask,
                    ddpfPixelFormat->dwBBitMask,
                    ddpfPixelFormat->dwRGBAlphaBitMask);
        } else {
            sprintf(sItem, " RGB=(%#x,%#x,%#x)",
                    ddpfPixelFormat->dwRBitMask,
                    ddpfPixelFormat->dwGBitMask,
                    ddpfPixelFormat->dwBBitMask);
        }
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_YUV) {
        sprintf(sItem, " YUVA=(%#x,%#x,%#x,%#x)",
                ddpfPixelFormat->dwYBitMask,
                ddpfPixelFormat->dwUBitMask,
                ddpfPixelFormat->dwVBitMask,
                ddpfPixelFormat->dwYUVAlphaBitMask);
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_ZBUFFER) {
        sprintf(sItem, " SdZSbL=(%#x,%#x,%#x,%#x)",
                ddpfPixelFormat->dwStencilBitDepth,
                ddpfPixelFormat->dwZBitMask,
                ddpfPixelFormat->dwStencilBitMask,
                ddpfPixelFormat->dwLuminanceAlphaBitMask);
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_ALPHA) {
        sprintf(sItem, " LBdBlZ=(%#x,%#x,%#x,%#x)",
                ddpfPixelFormat->dwLuminanceBitMask,
                ddpfPixelFormat->dwBumpDvBitMask,
                ddpfPixelFormat->dwBumpLuminanceBitMask,
                ddpfPixelFormat->dwRGBZBitMask);
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_LUMINANCE) {
        sprintf(sItem, " BMbMF=(%#x,%#x,%#x,%#x)",
                ddpfPixelFormat->dwBumpDuBitMask,
                ddpfPixelFormat->MultiSampleCaps.wBltMSTypes,
                ddpfPixelFormat->MultiSampleCaps.wFlipMSTypes,
                ddpfPixelFormat->dwYUVZBitMask);
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_BUMPDUDV) {
        sprintf(sItem, " O=(%#x)",
                ddpfPixelFormat->dwOperations);
        strcat(sBuf, sItem);
    }
    if (flags & DDPF_FOURCC) {
        sprintf(sItem, " FourCC=%#x(%s)",
                ddpfPixelFormat->dwFourCC, sFourCC(ddpfPixelFormat->dwFourCC));
        strcat(sBuf, sItem);
    }
    return sBuf;
}

static char *ExplainBlendType(DWORD val) {
    char *captions[] = {
        "ZERO", "ONE", "SRCCOLOR", "INVSRCCOLOR", "SRCALPHA", "INVSRCALPHA",
        "DESTALPHA", "INVDESTALPHA", "DESTCOLOR", "INVDESTCOLOR", "SRCALPHASAT",
        "BOTHSRCALPHA", "BOTHINVSRCALPHA"
    };
    if((val < D3DBLEND_ZERO) || (val > D3DBLEND_BOTHINVSRCALPHA)) return "???";
    return captions[val - D3DBLEND_ZERO];
}

static char *ExplainZFuncType(DWORD val) {
    char *captions[] = {
        "NEVER", "LESS", "EQUAL", "LESSEQUAL",
        "GREATER", "NOTEQUAL", "GREATEREQUAL", "ALWAYS"
    };
    if((val < D3DCMP_NEVER) || (val > D3DCMP_ALWAYS)) return "???";
    return captions[val - D3DCMP_NEVER];
}

static char *ExplainFillMode(DWORD val) {
    char *captions[] = {
        "POINT", "WIREFRAME", "SOLID"
    };
    if((val < D3DFILL_POINT) || (val > D3DFILL_SOLID)) return "???";
    return captions[val - D3DFILL_POINT];
}

static char *sTransformType(D3DTRANSFORMSTATETYPE tstype) {
    char *s;
    switch(tstype) {
    case D3DTRANSFORMSTATE_WORLD:
        s = "WORLD";
        break;
    case D3DTRANSFORMSTATE_VIEW:
        s = "VIEW";
        break;
    case D3DTRANSFORMSTATE_PROJECTION:
        s = "PROJECTION";
        break;
    case D3DTRANSFORMSTATE_WORLD1:
        s = "WORLD1";
        break;
    case D3DTRANSFORMSTATE_WORLD2:
        s = "WORLD2";
        break;
    case D3DTRANSFORMSTATE_WORLD3:
        s = "WORLD3";
        break;
    case D3DTRANSFORMSTATE_TEXTURE0:
        s = "TEXTURE0";
        break;
    case D3DTRANSFORMSTATE_TEXTURE1:
        s = "TEXTURE1";
        break;
    case D3DTRANSFORMSTATE_TEXTURE2:
        s = "TEXTURE2";
        break;
    case D3DTRANSFORMSTATE_TEXTURE3:
        s = "TEXTURE3";
        break;
    case D3DTRANSFORMSTATE_TEXTURE4:
        s = "TEXTURE4";
        break;
    case D3DTRANSFORMSTATE_TEXTURE5:
        s = "TEXTURE5";
        break;
    case D3DTRANSFORMSTATE_TEXTURE6:
        s = "TEXTURE6";
        break;
    case D3DTRANSFORMSTATE_TEXTURE7:
        s = "TEXTURE7";
        break;
    default:
        s = "unknown";
        break;
    }
    return s;
}

static char *sExecuteBufferFlags(DWORD c) {
    static char eb[128];
    unsigned int l;
    strcpy(eb, "D3DDEB_");
    if (c & D3DDEB_BUFSIZE) strcat(eb, "BUFSIZE+");
    if (c & D3DDEB_CAPS) strcat(eb, "CAPS+");
    if (c & D3DDEB_LPDATA) strcat(eb, "LPDATA+");
    l = strlen(eb);
    if (l > strlen("D3DDEB_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

static char *sExecuteBufferCaps(DWORD c) {
    static char eb[128];
    unsigned int l;
    strcpy(eb, "D3DDEBCAPS_");
    if (c & D3DDEBCAPS_SYSTEMMEMORY) strcat(eb, "SYSTEMMEMORY+");
    if (c & D3DDEBCAPS_VIDEOMEMORY) strcat(eb, "VIDEOMEMORY+");
    l = strlen(eb);
    if (l > strlen("D3DDEBCAPS_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

#ifdef DUMPEXECUTEBUFFER

static char *sOpcode(BYTE op) {
    char *ops[] = {
        "NULL",
        "POINT",
        "LINE",
        "TRIANGLE",
        "MATRIXLOAD",
        "MATRIXMULTIPLY",
        "STATETRANSFORM",
        "STATELIGHT",
        "STATERENDER",
        "PROCESSVERTICES",
        "TEXTURELOAD",
        "EXIT",
        "BRANCHFORWARD",
        "SPAN",
        "SETSTATUS"
    };
    if(op <= D3DOP_SETSTATUS) return ops[op];
    return "unknown";
}

static char *sRenderStateCode(DWORD code) {
    char *codes[] = {
        "NULL",
        "TEXTUREHANDLE",
        "ANTIALIAS",
        "TEXTUREADDRESS",
        "TEXTUREPERSPECTIVE",
        "WRAPU",
        "WRAPV",
        "ZENABLE",
        "FILLMODE",
        "SHADEMODE",
        "LINEPATTERN", // 10
        "MONOENABLE",
        "ROP2",
        "PLANEMASK",
        "ZWRITEENABLE",
        "ALPHATESTENABLE",
        "LASTPIXEL",
        "TEXTUREMAG",
        "TEXTUREMIN",
        "SRCBLEND",
        "DESTBLEND", // 20
        "TEXTUREMAPBLEND",
        "CULLMODE",
        "ZFUNC",
        "ALPHAREF",
        "ALPHAFUNC",
        "DITHERENABLE",
        "ALPHABLENDENABLE",
        "FOGENABLE",
        "SPECULARENABLE",
        "ZVISIBLE", // 30
        "SUBPIXEL",
        "SUBPIXELX",
        "STIPPLEDALPHA",
        "FOGCOLOR",
        "FOGTABLEMODE",
        "FOGSTART",
        "FOGEND",
        "FOGDENSITY",
        "STIPPLEENABLE",
        "EDGEANTIALIAS", // 40
        "COLORKEYENABLE",
        "42???",
        "BORDERCOLOR",
        "TEXTUREADDRESSU",
        "TEXTUREADDRESSV",
        "MIPMAPLODBIAS",
        "ZBIAS",
        "RANGEFOGENABLE",
        "ANISOTROPY",
        "FLUSHBATCH", // 50
        "TRANSLUCENTSORTINDEPENDENT",
        "STENCILENABLE",
        "STENCILFAIL",
        "STENCILZFAIL",
        "STENCILPASS",
        "STENCILFUNC",
        "STENCILREF",
        "STENCILMASK",
        "STENCILWRITEMASK",
        "TEXTUREFACTOR" // 60
    };
    if(code <= D3DRENDERSTATE_TEXTUREFACTOR) return codes[code];
    return "unknown";
}

//#define DUMPEXECUTEBUFFERVERTEX
#define DUMPEXECUTEBUFFERINSTRUCTIONS

static void DumpEBData(LPDIRECT3DEXECUTEBUFFER lpeb) {
    D3DEXECUTEBUFFERDESC ebd;
    D3DEXECUTEDATA ed;
    ed.dwSize = sizeof(D3DEXECUTEDATA);
    if((*pGetExecuteData)(lpeb, &ed)) return;
    ebd.dwSize = sizeof(D3DEXECUTEBUFFERDESC);
    if((*pEBLock)(lpeb, &ebd)) return;
    OutDebugD3D("> ==== DumpEBData lpeb=%#x ====\n", lpeb);
    OutDebugD3D("> Vertex: offset=%d count=%d hvertexoffset=%d\n", ed.dwVertexOffset, ed.dwVertexCount, ed.dwHVertexOffset);
    OutDebugD3D("> Instr.: offset=%d len=%d\n", ed.dwInstructionOffset, ed.dwInstructionLength);
#ifdef DUMPEXECUTEBUFFERVERTEX
    LPD3DVERTEX lpVertex;
    lpVertex = (LPD3DVERTEX)((LPBYTE)ebd.lpData + ed.dwVertexOffset);
    OutHexD3D((LPBYTE)lpVertex, ed.dwVertexCount * sizeof(D3DVERTEX));
    for(DWORD i = 0; i < lpebd->dwVertexCount; i++) {
        // do not format normal y coordinate, it is often an invalid real (infinite?)
        //OutTraceD3D("> Vertex[%04.4d]: pos=(%f:%f:%f) n=(%f:%f:%f) tex=(%f:%f)\n", i,
        OutTraceD3D("> Vertex[%04.4d]: pos=(%f:%f:%f) tex=(%f:%f)\n", i,
                    lpVertex->x, lpVertex->y, lpVertex->z,
                    //lpVertex->nx, lpVertex->ny, lpVertex->nz,
                    lpVertex->tu, lpVertex->tv);
        lpVertex++;
    }
#endif // DUMPEXECUTEBUFFERVERTEX
#ifdef DUMPEXECUTEBUFFERINSTRUCTIONS
    LPD3DINSTRUCTION lpInst;
    int len;
    lpInst = (LPD3DINSTRUCTION)((LPBYTE)ebd.lpData + ed.dwInstructionOffset);
    //lpInst = (LPD3DINSTRUCTION)lpVertex;
    len = (int)ed.dwInstructionLength;
    OutHexD3D((LPBYTE)lpInst, len);
    for(DWORD i = 0; len > 0; i++) {
        if((lpInst->bOpcode < D3DOP_POINT) || (lpInst->bOpcode > D3DOP_SETSTATUS)) {
            OutTraceD3D("> Instr.[%04.4d]: invalid op=%d\n", i, lpInst->bOpcode);
            break;
        }
        OutTraceD3D("> Instr.[%04.4d]: op=%d(%s) size=%d count=%d\n", i,
                    lpInst->bOpcode, sOpcode(lpInst->bOpcode), lpInst->bSize, lpInst->wCount);
        OutHexD3D((LPBYTE)lpInst + sizeof(D3DINSTRUCTION), (lpInst->bSize * lpInst->wCount));
        switch(lpInst->bOpcode) {
        case D3DOP_TRIANGLE: {
            LPD3DTRIANGLE lpTriangle = (LPD3DTRIANGLE)((LPBYTE)lpInst + sizeof(D3DINSTRUCTION));
            for (int j = 0; j < lpInst->wCount; j++) {
                OutTraceD3D(">> [%04.4d]: vertices=(%d,%d,%d) flags=%#x\n",
                            j, lpTriangle->v1, lpTriangle->v2, lpTriangle->v3, lpTriangle->wFlags);
                // cheats ...
                //lpTriangle->wFlags = 0x300;
                //lpTriangle->v3 = lpTriangle->v1;
                lpTriangle ++;
            }
        }
        break;
        case D3DOP_STATERENDER: {
            LPD3DSTATE lpState = (LPD3DSTATE)((LPBYTE)lpInst + sizeof(D3DINSTRUCTION));
            for (int j = 0; j < lpInst->wCount; j++) {
                OutTraceD3D(">> [%04.4d]: code=%#x(%s) val=%#x\n",
                            j, lpState->drstRenderStateType, sRenderStateCode(lpState->drstRenderStateType), lpState->dwArg[0]);
                //if(lpState->drstRenderStateType == D3DRENDERSTATE_ZENABLE)
                //	lpState->dwArg[0] = 0;
                lpState ++;
            }
        }
        break;
        }
        len -= sizeof(D3DINSTRUCTION) + (lpInst->bSize * lpInst->wCount);
        if(lpInst->bOpcode == D3DOP_EXIT) break;
        lpInst = (LPD3DINSTRUCTION)((LPBYTE)lpInst + sizeof(D3DINSTRUCTION) + (lpInst->bSize * lpInst->wCount));
    }
    if(len) OutTraceD3D("> residual len=%d !!!\n", len);
#endif // DUMPEXECUTEBUFFERINSTRUCTIONS
    (*pEBUnlock)(lpeb);
    OutDebugD3D("> ==== DumpEBData END ====\n");
}
#endif // DUMPEXECUTEBUFFER

#endif // DXW_NOTRACES

//----------------------------------------------------------------------//
// Proxies
//----------------------------------------------------------------------//

HRESULT WINAPI extDirect3DCreate(UINT SDKVersion, LPDIRECT3D *lplpd3d, LPUNKNOWN pUnkOuter) {
    HRESULT res;
    UINT d3dversion;
    d3dversion = 1;
    if(SDKVersion >= 0x0500) d3dversion = 2;
    if(SDKVersion >= 0x0600) d3dversion = 3;
    if(SDKVersion >= 0x0700) d3dversion = 7;
    OutTraceD3D("Direct3DCreate(D3D%d): SDKVersion=%#x UnkOuter=%#x\n", d3dversion, SDKVersion, pUnkOuter);
    res = (*pDirect3DCreate)(SDKVersion, lplpd3d, pUnkOuter);
    if(res) {
        OutTraceE("Direct3DCreate ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        return res;
    }
    HookDirect3DSession((LPDIRECTDRAW *)lplpd3d, d3dversion);
    OutTraceD3D("Direct3DCreate: d3d=%#x\n", *lplpd3d);
    return res;
}

HRESULT WINAPI extDirect3DCreateDevice(GUID FAR *lpGUID, LPDIRECT3D lpd3ddevice, LPDIRECTDRAWSURFACE surf, LPDIRECT3D *lplpd3ddevice, LPUNKNOWN pUnkOuter) {
    HRESULT res;
    int d3dversion = 0;
    switch(lpGUID->Data1) {
    case 0x64108800: // IID_IDirect3DDevice
        d3dversion = 1;
        break;
    case 0x93281501: // IID_IDirect3DDevice2
        d3dversion = 2;
        break;
    case 0xb0ab3b60: // IID_IDirect3DDevice3
        d3dversion = 3;
        break;
    case 0xf5049e79: // IID_IDirect3DDevice7
        d3dversion = 7;
        break;
    case 0x84e63de0: // IID_IDirect3DHALDevice;
        d3dversion = 7;
        break;
    }
    OutTraceD3D("Direct3DCreateDevice: guid=%#x(%s) d3ddevice=%#x dds=%#x%s UnkOuter=%#x\n",
                lpGUID, ExplainGUID(lpGUID), lpd3ddevice, surf, dxwss.ExplainSurfaceRole(surf), pUnkOuter);
    res = (*pDirect3DCreateDevice)(lpGUID, lpd3ddevice, surf, lplpd3ddevice, pUnkOuter);
    if(res) OutTraceE("Direct3DCreateDevice ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else {
        OutTraceD3D("Direct3DCreateDevice: d3ddevice=%#x\n", *lplpd3ddevice);
        if(d3dversion) HookDirect3DDevice((void **)lplpd3ddevice, d3dversion);
    }
    return res;
}

HRESULT WINAPI extQueryInterfaceD31(void *lpd3d, REFIID riid, LPVOID *ppvObj) {
    return extQueryInterfaceDX(1, pQueryInterfaceD31, lpd3d, riid, ppvObj);
}
HRESULT WINAPI extQueryInterfaceD32(void *lpd3d, REFIID riid, LPVOID *ppvObj) {
    return extQueryInterfaceDX(2, pQueryInterfaceD32, lpd3d, riid, ppvObj);
}
HRESULT WINAPI extQueryInterfaceD33(void *lpd3d, REFIID riid, LPVOID *ppvObj) {
    return extQueryInterfaceDX(3, pQueryInterfaceD33, lpd3d, riid, ppvObj);
}
HRESULT WINAPI extQueryInterfaceD37(void *lpd3d, REFIID riid, LPVOID *ppvObj) {
    return extQueryInterfaceDX(7, pQueryInterfaceD37, lpd3d, riid, ppvObj);
}

HRESULT WINAPI extQueryInterfaceD3D1(void *lpd3ddev, REFIID riid, LPVOID *ppvObj) {
    return extQueryInterfaceDX(1, pQueryInterfaceD3D1, lpd3ddev, riid, ppvObj);
}
HRESULT WINAPI extQueryInterfaceD3D2(void *lpd3ddev, REFIID riid, LPVOID *ppvObj) {
    return extQueryInterfaceDX(2, pQueryInterfaceD3D2, lpd3ddev, riid, ppvObj);
}
HRESULT WINAPI extQueryInterfaceD3D3(void *lpd3ddev, REFIID riid, LPVOID *ppvObj) {
    return extQueryInterfaceDX(3, pQueryInterfaceD3D3, lpd3ddev, riid, ppvObj);
}
HRESULT WINAPI extQueryInterfaceD3D7(void *lpd3ddev, REFIID riid, LPVOID *ppvObj) {
    return extQueryInterfaceDX(7, pQueryInterfaceD3D7, lpd3ddev, riid, ppvObj);
}

#ifdef TRACEALL
ULONG WINAPI extReleaseD3D(int d3dversion, ReleaseD3D_Type pReleaseD3D, LPDIRECT3DDEVICE lpd3dd) {
    ULONG ref;
    OutTraceD3D("Release(D3DD%d): d3ddev=%#x \n", d3dversion, lpd3dd);
    ref = (*pReleaseD3D)(lpd3dd);
    OutTraceD3D("Release(D3DD): ref=%d\n", ref);
    return ref;
}

ULONG WINAPI extReleaseD3D1(LPDIRECT3DDEVICE lpd3d) {
    return extReleaseD3D(1, pReleaseD3D1, lpd3d);
}
ULONG WINAPI extReleaseD3D2(LPDIRECT3DDEVICE lpd3d) {
    return extReleaseD3D(2, pReleaseD3D2, lpd3d);
}
ULONG WINAPI extReleaseD3D3(LPDIRECT3DDEVICE lpd3d) {
    return extReleaseD3D(3, pReleaseD3D3, lpd3d);
}
ULONG WINAPI extReleaseD3D7(LPDIRECT3DDEVICE lpd3d) {
    return extReleaseD3D(7, pReleaseD3D7, lpd3d);
}

HRESULT WINAPI extInitialize(void *lpd3d) {
    HRESULT res;
    // the Initialize method is present in D3D interface version 1 only...
    OutTraceD3D("Initialize(D3D1): d3d=%#x\n", lpd3d);
    res = (*pInitialize)(lpd3d);
    if(res) OutTraceE("Initialize ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("Initialize: OK\n");
    return res;
}
#endif

typedef struct {
    LPD3DENUMDEVICESCALLBACK *cb;
    LPVOID arg;
} CallbackArg;

typedef struct {
    LPD3DENUMDEVICESCALLBACK7 *cb;
    LPVOID arg;
} CallbackArg7;

#ifndef DXW_NOTRACES
static void DumpD3DDeviceDesc(LPD3DDEVICEDESC d3, char *label) {
    if(IsTraceD3D) {
        if(d3) {
            OutTrace("EnumDevices: CALLBACK dev=%s Size=%d Flags=%#x ", label, d3->dwSize, d3->dwFlags);
            if(d3->dwFlags & D3DDD_COLORMODEL) OutTrace("ColorModel=%#x ", d3->dcmColorModel);
            if(d3->dwFlags & D3DDD_DEVCAPS) OutTrace("DevCaps=%#x ", d3->dwDevCaps);
            if(d3->dwFlags & D3DDD_TRANSFORMCAPS) OutTrace("TransformCaps=%#x ", d3->dtcTransformCaps.dwCaps);
            if(d3->dwFlags & D3DDD_LIGHTINGCAPS) OutTrace("LightingCaps=%#x ", d3->dlcLightingCaps);
            if(d3->dwFlags & D3DDD_BCLIPPING) OutTrace("Clipping=%#x ", d3->bClipping);
            if(d3->dwFlags & D3DDD_LINECAPS) OutTrace("LineCaps=%#x ", d3->dpcLineCaps);
            if(d3->dwFlags & D3DDD_TRICAPS) OutTrace("TriCaps=%#x ", d3->dpcTriCaps);
            if(d3->dwFlags & D3DDD_DEVICERENDERBITDEPTH) OutTrace("DeviceRenderBitDepth=%d ", d3->dwDeviceRenderBitDepth);
            if(d3->dwFlags & D3DDD_DEVICEZBUFFERBITDEPTH) OutTrace("DeviceZBufferBitDepth=%d ", d3->dwDeviceZBufferBitDepth);
            if(d3->dwFlags & D3DDD_MAXBUFFERSIZE) OutTrace("MaxBufferSize=%d ", d3->dwMaxBufferSize);
            if(d3->dwFlags & D3DDD_MAXVERTEXCOUNT) OutTrace("MaxVertexCount=%d ", d3->dwMaxVertexCount);
            OutTrace("Texture min=(%dx%d) max=(%dx%d)\n", d3->dwMinTextureWidth, d3->dwMinTextureHeight, d3->dwMaxTextureWidth, d3->dwMaxTextureHeight);
            OutHexD3D((unsigned char *)d3, d3->dwSize);
        } else
            OutTrace("EnumDevices: CALLBACK dev=%s ddesc=NULL\n", label);
    }
}

static void DumpD3DPrimCaps(char *label, D3DPRIMCAPS *pc) {
    OutTrace("%s={siz=%d Misc=%#x Raster=%#x ZCmp=%#x SrcBlend=%#x DestBlend=%#x AlphaCmp=%#x Shade=%#x Tex=%#x TexFil=%#x TexBlend=%#x TexAddr=%#x Stipple=(%dx%d)} ",
             label,
             pc->dwSize, pc->dwMiscCaps, pc->dwRasterCaps, pc->dwZCmpCaps, pc->dwSrcBlendCaps, pc->dwDestBlendCaps, pc->dwAlphaCmpCaps,
             pc->dwShadeCaps, pc->dwTextureCaps, pc->dwTextureFilterCaps, pc->dwTextureBlendCaps, pc->dwTextureAddressCaps,
             pc->dwStippleWidth, pc->dwStippleHeight);
}

static void DumpD3DDeviceDesc7(LPD3DDEVICEDESC7 d3, char *label) {
    if(IsTraceD3D) {
        if(d3) {
            OutTrace("EnumDevices: CALLBACK dev=%s DevCaps=%#x GUID=%#x(%s) ", label, d3->dwDevCaps, d3->deviceGUID.Data1, ExplainGUID(&d3->deviceGUID));
            DumpD3DPrimCaps("LineCaps", &d3->dpcLineCaps);
            DumpD3DPrimCaps("TriCaps", &d3->dpcLineCaps);
            OutTrace("RenderBitDepth=%d ZBufferBitDepth", d3->dwDeviceRenderBitDepth, d3->dwDeviceZBufferBitDepth);
            OutTrace("Texture min=(%dx%d) max=(%dx%d)\n", d3->dwMinTextureWidth, d3->dwMinTextureHeight, d3->dwMaxTextureWidth, d3->dwMaxTextureHeight);
            // to be completed ....
            //OutTrace("\n");
            OutHexD3D((unsigned char *)d3, sizeof(D3DDEVICEDESC7));
        } else
            OutTrace("EnumDevices: CALLBACK dev=%s ddesc=NULL\n", label);
    }
}
#endif // DXW_NOTRACES

HRESULT WINAPI extDeviceProxy(GUID FAR *lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC lpd3ddd1, LPD3DDEVICEDESC lpd3ddd2, LPVOID arg) {
    HRESULT res;
#ifndef DXW_NOTRACES
    OutTraceD3D("EnumDevices: CALLBACK GUID=%#x(%s) DeviceDescription=\"%s\", DeviceName=\"%s\", arg=%#x\n", lpGuid->Data1, ExplainGUID(lpGuid), lpDeviceDescription, lpDeviceName, ((CallbackArg *)arg)->arg);
    DumpD3DDeviceDesc(lpd3ddd1, "HWDEV");
    DumpD3DDeviceDesc(lpd3ddd2, "SWDEV");
#endif // DXW_NOTRACES
    // IID_IDirect3DTnLHalDevice = 0xf5049e78
    if((dxw.dwFlags12 & NOTNLDEVICE) && (lpGuid->Data1 == 0xf5049e78)) {
        OutTraceDW("EnumDevices: D3DTNLDEVICE SKIP\n");
        return TRUE;
    }
    // IID_IDirect3DHALDevice = 0x84e63de0....
    if((dxw.dwFlags8 & NOHALDEVICE) && (lpGuid->Data1 == 0x84e63de0)) {
        OutTraceDW("EnumDevices: D3DHALDEVICE SKIP\n");
        return TRUE;
    }
    if(dxw.dwFlags4 & NOPOWER2FIX) {
        D3DDEVICEDESC lpd3ddd1fix, lpd3ddd2fix;
        if(lpd3ddd1) memcpy(&lpd3ddd1fix, lpd3ddd1, sizeof(D3DDEVICEDESC));
        if(lpd3ddd2) memcpy(&lpd3ddd2fix, lpd3ddd2, sizeof(D3DDEVICEDESC));
        lpd3ddd1fix.dpcLineCaps.dwTextureCaps |= D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_POW2;
        lpd3ddd1fix.dpcTriCaps.dwTextureCaps |= D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_POW2;
        lpd3ddd2fix.dpcLineCaps.dwTextureCaps |= D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_POW2;
        lpd3ddd2fix.dpcTriCaps.dwTextureCaps |= D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_POW2;
        res = (*(((CallbackArg *)arg)->cb))(lpGuid, lpDeviceDescription, lpDeviceName, lpd3ddd1 ? &lpd3ddd1fix : NULL, lpd3ddd2 ? &lpd3ddd2fix : NULL, ((CallbackArg *)arg)->arg);
    } else
        res = (*(((CallbackArg *)arg)->cb))(lpGuid, lpDeviceDescription, lpDeviceName, lpd3ddd1, lpd3ddd2, ((CallbackArg *)arg)->arg);
    OutTraceD3D("EnumDevices: CALLBACK ret=%#x\n", res);
    return res;
}

HRESULT WINAPI extDeviceProxy7(LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC7 lpd3ddd, LPVOID arg) {
    HRESULT res;
#ifndef DXW_NOTRACES
    OutTraceD3D("EnumDevices(D3D7): CALLBACK DeviceDescription=\"%s\", DeviceName=\"%s\", arg=%#x\n", lpDeviceDescription, lpDeviceName, ((CallbackArg *)arg)->arg);
    DumpD3DDeviceDesc7(lpd3ddd, "DX7DEV");
#endif // DXW_NOTRACES
    // IID_IDirect3DTnLHalDevice = 0xf5049e78
    if((dxw.dwFlags12 & NOTNLDEVICE) && (lpd3ddd->deviceGUID.Data1 == 0xf5049e78)) {
        OutTraceDW("EnumDevices: D3DTNLDEVICE SKIP\n");
        return TRUE;
    }
    // IID_IDirect3DHALDevice = 0x84e63de0....
    if((dxw.dwFlags8 & NOHALDEVICE) && (lpd3ddd->deviceGUID.Data1 == 0x84e63de0)) {
        OutTraceDW("EnumDevices: D3DHALDEVICE SKIP\n");
        return TRUE;
    }
    if(dxw.dwFlags11 & TRANSFORMANDLIGHT) {
        // v2.05.12: this here to please "Will Rock"
        lpd3ddd->dwDevCaps |= D3DDEVCAPS_HWTRANSFORMANDLIGHT;
    }
    if(dxw.dwFlags4 & NOPOWER2FIX) {
        D3DDEVICEDESC7 lpd3dddfix;
        if(lpd3ddd) memcpy(&lpd3dddfix, lpd3ddd, sizeof(D3DDEVICEDESC7));
        lpd3dddfix.dpcLineCaps.dwTextureCaps |= D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_POW2;
        lpd3dddfix.dpcTriCaps.dwTextureCaps |= D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_POW2;
        res = (*(((CallbackArg7 *)arg)->cb))(lpDeviceDescription, lpDeviceName, lpd3ddd ? &lpd3dddfix : NULL, ((CallbackArg7 *)arg)->arg);
    } else
        res = (*(((CallbackArg7 *)arg)->cb))(lpDeviceDescription, lpDeviceName, lpd3ddd, ((CallbackArg7 *)arg)->arg);
    OutTraceD3D("EnumDevices: CALLBACK ret=%#x\n", res);
    return res;
}

HRESULT WINAPI extEnumDevices(int d3dversion, EnumDevices_Type pEnumDevices, void *lpd3d, LPD3DENUMDEVICESCALLBACK cb, LPVOID arg) {
    HRESULT res;
    CallbackArg Arg;
    OutTraceD3D("EnumDevices(D3D%d): d3d=%#x arg=%#x\n", d3dversion, lpd3d, arg);
    Arg.cb = &cb;
    Arg.arg = arg;
    res = (*pEnumDevices)(lpd3d, (LPD3DENUMDEVICESCALLBACK)extDeviceProxy, (LPVOID)&Arg);
    if(res) OutTraceE("EnumDevices ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("EnumDevices: OK\n");
    return res;
}

HRESULT WINAPI extEnumDevices1(void *lpd3d, LPD3DENUMDEVICESCALLBACK cb, LPVOID arg) {
    return extEnumDevices(1, pEnumDevices1, lpd3d, cb, arg);
}
HRESULT WINAPI extEnumDevices2(void *lpd3d, LPD3DENUMDEVICESCALLBACK cb, LPVOID arg) {
    return extEnumDevices(2, pEnumDevices2, lpd3d, cb, arg);
}
HRESULT WINAPI extEnumDevices3(void *lpd3d, LPD3DENUMDEVICESCALLBACK cb, LPVOID arg) {
    return extEnumDevices(3, pEnumDevices3, lpd3d, cb, arg);
}

HRESULT WINAPI extEnumDevices7(void *lpd3d, LPD3DENUMDEVICESCALLBACK7 cb, LPVOID arg) {
    HRESULT res;
    CallbackArg7 Arg;
    OutTraceD3D("EnumDevices(D3D7): d3d=%#x arg=%#x\n", lpd3d, arg);
    Arg.cb = &cb;
    Arg.arg = arg;
    res = (*pEnumDevices7)(lpd3d, (LPD3DENUMDEVICESCALLBACK7)extDeviceProxy7, (LPVOID)&Arg);
    if(res) OutTraceE("EnumDevices ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("EnumDevices: OK\n");
    return res;
}

HRESULT WINAPI extCreateLight(int d3dversion, CreateLight_Type pCreateLight, void *lpd3d, LPDIRECT3DLIGHT *lpLight, IUnknown *p0) {
    HRESULT res;
    OutTraceD3D("CreateLight(D3D%d): d3d=%#x\n", d3dversion, lpd3d);
    res = (*pCreateLight)(lpd3d, lpLight, p0);
    if(res) OutTraceE("CreateLight ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("CreateLight: OK\n");
    return res;
}

HRESULT WINAPI extCreateLight1(void *lpd3d, LPDIRECT3DLIGHT *lpLight, IUnknown *p0) {
    return extCreateLight(1, pCreateLight1, lpd3d, lpLight, p0);
}
HRESULT WINAPI extCreateLight2(void *lpd3d, LPDIRECT3DLIGHT *lpLight, IUnknown *p0) {
    return extCreateLight(2, pCreateLight2, lpd3d, lpLight, p0);
}
HRESULT WINAPI extCreateLight3(void *lpd3d, LPDIRECT3DLIGHT *lpLight, IUnknown *p0) {
    return extCreateLight(3, pCreateLight3, lpd3d, lpLight, p0);
}

#ifdef TRACEMATERIAL
HRESULT WINAPI extCreateMaterial1(void *lpd3d, LPDIRECT3DMATERIAL *lpMaterial, IUnknown *p0) {
    HRESULT res;
    OutTraceD3D("CreateMaterial(D3D1): d3d=%#x\n", lpd3d);
    res = (*pCreateMaterial1)(lpd3d, lpMaterial, p0);
    if(res) OutTraceE("CreateMaterial ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("CreateMaterial: OK\n");
    HookMaterial(lpMaterial, 0);
    return res;
}

HRESULT WINAPI extCreateMaterial2(void *lpd3d, LPDIRECT3DMATERIAL2 *lpMaterial, IUnknown *p0) {
    HRESULT res;
    OutTraceD3D("CreateMaterial(D3D2): d3d=%#x\n", lpd3d);
    res = (*pCreateMaterial2)(lpd3d, lpMaterial, p0);
    if(res) OutTraceE("CreateMaterial ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("CreateMaterial: OK\n");
    //HookMaterial(lpMaterial, 0);
    return res;
}

HRESULT WINAPI extCreateMaterial3(void *lpd3d, LPDIRECT3DMATERIAL3 *lpMaterial, IUnknown *p0) {
    HRESULT res;
    OutTraceD3D("CreateMaterial(D3D3): d3d=%#x\n", lpd3d);
    res = (*pCreateMaterial3)(lpd3d, lpMaterial, p0);
    if(res) OutTraceE("CreateMaterial ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("CreateMaterial: OK\n");
    //HookMaterial(lpMaterial, 0);
    return res;
}
#endif

HRESULT WINAPI extCreateViewport1(void *lpd3d, LPDIRECT3DVIEWPORT *lpViewport, IUnknown *p0) {
    HRESULT res;
    OutTraceD3D("CreateViewport(D3D1): d3d=%#x p0=%#x\n", lpd3d, p0);
    res = (*pCreateViewport1)(lpd3d, lpViewport, p0);
    if(res)
        OutTraceE("CreateViewport ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else {
        OutTraceD3D("CreateViewport: Viewport=%#x\n", *lpViewport);
        HookViewport(lpViewport, 1);
        gViewport1 = *lpViewport;
    }
    return res;
}

HRESULT WINAPI extCreateViewport2(void *lpd3d, LPDIRECT3DVIEWPORT2 *lpViewport, IUnknown *p0) {
    HRESULT res;
    OutTraceD3D("CreateViewport(D3D2): d3d=%#x\n", lpd3d);
    res = (*pCreateViewport2)(lpd3d, lpViewport, p0);
    if(res) OutTraceE("CreateViewport ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("CreateViewport: Viewport=%#x\n", *lpViewport);
    HookViewport((LPDIRECT3DVIEWPORT *)lpViewport, 2);
    return res;
}

HRESULT WINAPI extCreateViewport3(void *lpd3d, LPDIRECT3DVIEWPORT3 *lpViewport, IUnknown *p0) {
    HRESULT res;
    OutTraceD3D("CreateViewport(D3D3): d3d=%#x\n", lpd3d);
    res = (*pCreateViewport3)(lpd3d, lpViewport, p0);
    if(res) OutTraceE("CreateViewport ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("CreateViewport: Viewport=%#x\n", *lpViewport);
    HookViewport((LPDIRECT3DVIEWPORT *)lpViewport, 3);
    if(IsDebugD3D) {
        HRESULT res2;
        D3DVIEWPORT2 vpdesc;
        vpdesc.dwSize = sizeof(D3DVIEWPORT2);
        res2 = (*pGetViewport2_3)(*lpViewport, &vpdesc);
        if(res2)
            if(res2 == D3DERR_VIEWPORTDATANOTSET)
                OutTrace("CreateViewport: Viewport data not set\n");
            else
                OutTraceE("CreateViewport GetViewport2 ERROR: err=%#x(%s) at %d\n", res2, ExplainDDError(res2), __LINE__);
        else {
            OutTraceD3D("CreateViewport: size=%d pos=(%d,%d) dim=(%dx%d)\n",
                        vpdesc.dwSize, vpdesc.dwX, vpdesc.dwY, vpdesc.dwWidth, vpdesc.dwHeight);
        }
    }
    return res;
}

static HRESULT WINAPI extFindDevice(int d3dversion, FindDevice_Type pFindDevice, void *lpd3d, LPD3DFINDDEVICESEARCH p1, LPD3DFINDDEVICERESULT p2) {
    HRESULT res;
    OutTraceD3D("FindDevice(D3D%d): d3d=%#x devsearch=%#x (size=%d flags=%#x caps=%#x primcaps=%#x colormodel=%#x hw=%#x guid=%#x) p2=%#x\n",
                d3dversion, lpd3d, p1, p1->dwSize, p1->dwFlags, p1->dwCaps, p1->dpcPrimCaps, p1->dcmColorModel, p1->bHardware, p1->guid, p2);
    res = (*pFindDevice)(lpd3d, p1, p2);
    if(res) OutTraceE("FindDevice ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else {
#ifndef DXW_NOTRACES
        OutTraceD3D("FindDevice: GUID=%#x.%#x.%#x.%#x\n", p2->guid.Data1, p2->guid.Data2, p2->guid.Data3, p2->guid.Data4);
        DumpD3DDeviceDesc(&(p2->ddHwDesc), "HWDEV");
        DumpD3DDeviceDesc(&(p2->ddSwDesc), "SWDEV");
#endif // DXW_NOTRACES
    }
    return res;
}

HRESULT WINAPI extFindDevice1(void *lpd3d, LPD3DFINDDEVICESEARCH p1, LPD3DFINDDEVICERESULT p2) {
    return extFindDevice(1, pFindDevice1, lpd3d, p1, p2);
}
HRESULT WINAPI extFindDevice2(void *lpd3d, LPD3DFINDDEVICESEARCH p1, LPD3DFINDDEVICERESULT p2) {
    return extFindDevice(2, pFindDevice2, lpd3d, p1, p2);
}
HRESULT WINAPI extFindDevice3(void *lpd3d, LPD3DFINDDEVICESEARCH p1, LPD3DFINDDEVICERESULT p2) {
    return extFindDevice(3, pFindDevice3, lpd3d, p1, p2);
}

HRESULT WINAPI extInitializeVP(void *lpvp, LPDIRECT3D lpd3d) {
    HRESULT res;
    OutTraceD3D("Initialize(VP): viewport=%#x d3d=%#x\n", lpvp, lpd3d);
    res = (*pInitializeVP)(lpvp, lpd3d);
    if(res) OutTraceE("Initialize(VP) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("Initialize(VP): OK \n");
    return res;
}

#ifndef D3DERR_NOTAVAILABLE
#define _FACD3D  0x876
#define D3DERR_NOTAVAILABLE MAKE_HRESULT(1, _FACD3D, 2154)
#endif

HRESULT WINAPI extCreateDevice2(void *lpd3d, REFCLSID Guid, LPDIRECTDRAWSURFACE lpdds, LPDIRECT3DDEVICE2 *lplpd3dd) {
    HRESULT res;
    OutTraceD3D("CreateDevice(D3D2): d3d=%#x GUID=%#x(%s) lpdds=%#x%s\n",
                lpd3d, Guid.Data1, ExplainGUID((GUID *)&Guid), lpdds, dxwss.ExplainSurfaceRole((LPDIRECTDRAWSURFACE)lpdds));
    if((dxw.dwFlags8 & NOHALDEVICE) && (Guid.Data1 == 0x84e63de0)) {
        OutTraceDW("CreateDevice(D3D2): D3DHALDEVICE SKIP\n");
        return D3DERR_NOTAVAILABLE;
    }
    res = (*pCreateDevice2)(lpd3d, Guid, lpdds, lplpd3dd);
    if(res != DD_OK)
        OutTraceE("CreateDevice(D3D2) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else {
        OutTraceD3D("CreateDevice(D3D2): lpd3d=%#x lpd3dd=%#x\n", lpd3d, *lplpd3dd);
        HookDirect3DDevice((void **)lplpd3dd, 2);
    }
    return res;
}

HRESULT WINAPI extCreateDevice3(void *lpd3d, REFCLSID Guid, LPDIRECTDRAWSURFACE4 lpdds, LPDIRECT3DDEVICE3 *lplpd3dd, LPUNKNOWN unk) {
    HRESULT res;
    OutTraceD3D("CreateDevice(D3D3): d3d=%#x GUID=%#x(%s) lpdds=%#x%s\n",
                lpd3d, Guid.Data1, ExplainGUID((GUID *)&Guid), lpdds, dxwss.ExplainSurfaceRole((LPDIRECTDRAWSURFACE)lpdds));
    if((dxw.dwFlags8 & NOHALDEVICE) && (Guid.Data1 == 0x84e63de0)) {
        OutTraceDW("CreateDevice(D3D3): D3DHALDEVICE SKIP\n");
        return D3DERR_NOTAVAILABLE;
    }
    res = (*pCreateDevice3)(lpd3d, Guid, lpdds, lplpd3dd, unk);
    if(res != DD_OK)
        OutTraceE("CreateDevice(D3D3) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else {
        OutTraceD3D("CreateDevice(D3D3): lpd3d=%#x lpd3dd=%#x\n", lpd3d, *lplpd3dd);
        HookDirect3DDevice((void **)lplpd3dd, 3);
    }
    return res;
}

HRESULT WINAPI extCreateDevice7(void *lpd3d, REFCLSID Guid, LPDIRECTDRAWSURFACE7 lpdds, LPDIRECT3DDEVICE7 *lplpd3dd) {
    // v2.02.83: D3D CreateDevice (version 7? all versions?) internally calls the Release method upon the backbuffer
    // surface, and this has to be avoided since it causes a crash.
    HRESULT res;
    OutTraceD3D("CreateDevice(D3D7): d3d=%#x GUID=%#x(%s) lpdds=%#x%s\n",
                lpd3d, Guid.Data1, ExplainGUID((GUID *)&Guid), lpdds, dxwss.ExplainSurfaceRole((LPDIRECTDRAWSURFACE)lpdds));
    if((dxw.dwFlags12 & NOTNLDEVICE) && (Guid.Data1 == 0xf5049e78)) {
        OutTraceDW("CreateDevice(D3D3): D3DTNLDEVICE SKIP\n");
        return D3DERR_NOTAVAILABLE;
    }
    if((dxw.dwFlags8 & NOHALDEVICE) && (Guid.Data1 == 0x84e63de0)) {
        OutTraceDW("CreateDevice(D3D3): D3DHALDEVICE SKIP\n");
        return D3DERR_NOTAVAILABLE;
    }
    res = (*pCreateDevice7)(lpd3d, Guid, lpdds, lplpd3dd);
    if(res == DD_OK) {
        OutTraceD3D("CreateDevice(D3D7): lpd3d=%#x lpd3dd=%#x\n", lpd3d, *lplpd3dd);
        HookDirect3DDevice((void **)lplpd3dd, 7);
    } else
        OutTraceE("CreateDevice(D3D7) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    return res;
}

HRESULT WINAPI extSetRenderState(int version, SetRenderState3_Type pSetRenderState, void *d3dd, D3DRENDERSTATETYPE State, DWORD Value) {
    HRESULT res;
#ifndef DXW_NOTRACES
    if(IsDebugD3D) {
        OutTrace("SetRenderState(D3D%d): d3dd=%#x State=%#x(%s) Value=%#x\n", version, d3dd, State, ExplainD3DRenderState(State), Value);
        switch(State) {
        case D3DRENDERSTATE_SRCBLEND:
        case D3DRENDERSTATE_DESTBLEND:
            OutTrace("> %s: %d(%s)\n", ExplainD3DRenderState(State), Value, ExplainBlendType(Value));
            break;
        case D3DRENDERSTATE_ZFUNC:
            OutTrace("> D3DRENDERSTATE_ZFUNC: %d(%s)\n", Value, ExplainZFuncType(Value));
            break;
        case D3DRENDERSTATE_FILLMODE:
            OutTrace("> D3DRENDERSTATE_FILLMODE: %d(%s)\n", Value, ExplainFillMode(Value));
            break;
        }
    }
#endif
    if((dxw.dwFlags4 & NOTEXTURES) && (State == (D3DRENDERSTATE_TEXTUREHANDLE))) { // v2.04.88
        OutTraceD3D("SetRenderState: NOTEXTURES - skip TEXTUREHANDLE\n");
        return DD_OK;
    }
    if((dxw.dwDFlags & ZBUFFERALWAYS) && (State == D3DRENDERSTATE_ZFUNC)) {
        OutTraceD3D("SetRenderState: FIXED State=ZFUNC Value=%s->D3DCMP_ALWAYS\n", ExplainRenderstateValue(Value));
        Value = D3DCMP_ALWAYS;
    }
    if((dxw.dwFlags2 & WIREFRAME) && (State == D3DRENDERSTATE_FILLMODE)) {
        OutTraceD3D("SetRenderState: FIXED State=FILLMODE Value=%#x->D3DFILL_WIREFRAME\n", Value);
        Value = D3DFILL_WIREFRAME;
    }
    if((dxw.dwFlags4 & DISABLEFOGGING) && (State == D3DRENDERSTATE_FOGENABLE)) {
        OutTraceD3D("SetRenderState: FIXED State=FOGENABLE Value=%#x->FALSE\n", Value);
        Value = FALSE;
    }
    if((dxw.dwFlags5 & TEXTURETRANSP) && (State == D3DRENDERSTATE_ALPHABLENDENABLE)) {
        OutTraceD3D("SetRenderState: FIXED State=ALPHABLENDENABLE Value=%#x->TRUE\n", Value);
        Value = TRUE;
    }
    // beware!!! likely this code would work for interface version 3 only !!!
    if((State == D3DRENDERSTATE_ZWRITEENABLE) && (Value == TRUE) && (dxw.dwFlags8 & DYNAMICZCLEAN)) {
        //if((State==D3DRENDERSTATE_ZWRITEENABLE) && (Value==FALSE) && (dxw.dwFlags8 & DYNAMICZCLEAN)){
        //if((State==D3DRENDERSTATE_ZWRITEENABLE) && (dxw.dwFlags8 & DYNAMICZCLEAN)){
        HRESULT res2;
        LPDIRECT3DVIEWPORT3 vp;
        D3DVIEWPORT vpd;
        res2 = ((LPDIRECT3DDEVICE3)d3dd)->GetCurrentViewport(&vp);
        if(!res2) {
            D3DRECT d3dRect;
            vpd.dwSize = sizeof(D3DVIEWPORT);
            vp->GetViewport(&vpd);
            d3dRect.x1 = vpd.dwX;
            d3dRect.y1 = vpd.dwY;
            d3dRect.x2 = vpd.dwX + vpd.dwWidth;
            d3dRect.y2 = vpd.dwY + vpd.dwHeight;
            OutTraceD3D("d3dRect=(%d,%d)-(%d,%d)\n", d3dRect.x1, d3dRect.y1, d3dRect.x2, d3dRect.y2);
            if(dxw.dwFlags4 & ZBUFFERCLEAN )vp->Clear2(1, &d3dRect, D3DCLEAR_ZBUFFER, 0, 1.0, 0);
            if(dxw.dwFlags4 & ZBUFFER0CLEAN)vp->Clear2(1, &d3dRect, D3DCLEAR_ZBUFFER, 0, 0.0, 0);
            if(dxw.dwFlags5 & CLEARTARGET) vp->Clear(1, &d3dRect, D3DCLEAR_TARGET);
        }
    }
    res = (*pSetRenderState)(d3dd, State, Value);
    _if(res) OutTraceE("SetRenderState: res=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extSetRenderState2(void *d3dd, D3DRENDERSTATETYPE State, DWORD Value) {
    return extSetRenderState(2, pSetRenderState2, d3dd, State, Value);
}
HRESULT WINAPI extSetRenderState3(void *d3dd, D3DRENDERSTATETYPE State, DWORD Value) {
    return extSetRenderState(3, pSetRenderState3, d3dd, State, Value);
}
HRESULT WINAPI extSetRenderState7(void *d3dd, D3DRENDERSTATETYPE State, DWORD Value) {
    return extSetRenderState(7, pSetRenderState7, d3dd, State, Value);
}

static HRESULT WINAPI dxwRestoreCallback(LPDIRECTDRAWSURFACE lpDDSurface, LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext) {
    HRESULT res;
    OutTrace("dxwRestoreCallback: ANALYZING lpdds=%#x\n", lpDDSurface);
    if(lpDDSurface->IsLost()) {
        if(res = lpDDSurface->Restore()) {
            OutTrace("dxwRestoreCallback: RESTORE FAILED lpdds=%#x err=%#x(%s)\n", lpDDSurface, res, ExplainDDError(res));
            return DDENUMRET_CANCEL;
        }
        OutTrace("dxwRestoreCallback: RESTORED lpdds=%#x\n", lpDDSurface);
    }
    return DDENUMRET_OK;
}

HRESULT WINAPI extBeginScene1(void *d3dd) {
    HRESULT res;
    OutTraceD3D("BeginScene(D3D1): d3dd=%#x\n", d3dd);
    // moved inside Execute wrapper, but no big improvements ....
    //if(dxw.dwDFlags & PATCHEXECUTEBUFFER) InsertPatchingExecuteBuffer(d3dd, D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL); // trytry ....
    //if(dxw.dwFlags2 & WIREFRAME) InsertPatchingExecuteBuffer(d3dd, D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME); // trytry ....
    //if(dxw.dwDFlags & FORCEZBUFFERON) InsertPatchingExecuteBuffer(d3dd, D3DRENDERSTATE_ZENABLE, TRUE);
    //if(dxw.dwDFlags & FORCEZBUFFEROFF) InsertPatchingExecuteBuffer(d3dd, D3DRENDERSTATE_ZENABLE, FALSE);
    if((dxw.dwFlags4 & (ZBUFFERCLEAN | ZBUFFER0CLEAN)) || (dxw.dwFlags5 & CLEARTARGET)) {
        if(gViewport1) {
            D3DRECT d3dRect;
            DWORD dwFlags;
            d3dRect.x1 = (LONG)0.0;
            d3dRect.y1 = (LONG)0.0;
            d3dRect.x2 = (LONG)dxw.GetScreenWidth();
            d3dRect.y2 = (LONG)dxw.GetScreenHeight();
            dwFlags = 0;
            if(dxw.dwFlags4 & (ZBUFFERCLEAN | ZBUFFER0CLEAN)) dwFlags = D3DCLEAR_ZBUFFER;
            if(dxw.dwFlags5 & CLEARTARGET) dwFlags |= D3DCLEAR_TARGET;
            res = gViewport1->Clear(1, &d3dRect, dwFlags);
            _if(res) OutTraceE("BeginScene: viewport Clear ERROR res=%#x(%s)\n", res, ExplainDDError(res));
        }
    }
    if(dxw.dwFlags5 & LIMITBEGINSCENE) LimitFrameCount(dxw.MaxFPS);
    res = (*pBeginScene1)(d3dd);
    if(res == DDERR_SURFACELOST) {
        OutTraceDW("BeginScene: recovering from DDERR_SURFACELOST\n");
        lpPrimaryDD->EnumSurfaces(DDENUMSURFACES_DOESEXIST | DDENUMSURFACES_ALL, NULL, NULL, (LPDDENUMSURFACESCALLBACK)dxwRestoreCallback);
        res = (*pBeginScene1)(d3dd);
    }
    _if(res) OutTraceE("BeginScene: res=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

static HRESULT WINAPI dxwRestoreCallback2(LPDIRECTDRAWSURFACE4 lpDDSurface, LPDDSURFACEDESC2 lpDDSurfaceDesc, LPVOID lpContext) {
    HRESULT res;
    OutTrace("dxwRestoreCallback2: ANALYZING lpdds=%#x\n", lpDDSurface);
    if(lpDDSurface->IsLost()) {
        if(res = lpDDSurface->Restore()) {
            OutTrace("dxwRestoreCallback2: RESTORE FAILED lpdds=%#x err=%#x(%s)\n", lpDDSurface, res, ExplainDDError(res));
            return DDENUMRET_CANCEL;
        }
        OutTrace("dxwRestoreCallback2: RESTORED lpdds=%#x\n", lpDDSurface);
    }
    return DDENUMRET_OK;
}

HRESULT WINAPI extBeginScene2(void *d3dd) {
    HRESULT res;
    OutTraceD3D("BeginScene(D3D2): d3dd=%#x\n", d3dd);
    if(dxw.dwFlags5 & LIMITBEGINSCENE) LimitFrameCount(dxw.MaxFPS);
    if((dxw.dwFlags4 & (ZBUFFERCLEAN | ZBUFFER0CLEAN)) || (dxw.dwFlags5 & CLEARTARGET)) {
        LPDIRECT3DVIEWPORT2 vp;
        D3DVIEWPORT vpd;
        res = ((LPDIRECT3DDEVICE2)d3dd)->GetCurrentViewport(&vp);
        if(!res) {
            D3DRECT d3dRect;
            DWORD dwFlags;
            vpd.dwSize = sizeof(D3DVIEWPORT);
            vp->GetViewport(&vpd);
            d3dRect.x1 = vpd.dwX;
            d3dRect.y1 = vpd.dwY;
            d3dRect.x2 = vpd.dwX + vpd.dwWidth;
            d3dRect.y2 = vpd.dwY + vpd.dwHeight;
            OutTraceD3D("d3dRect=(%d,%d)-(%d,%d)\n", d3dRect.x1, d3dRect.y1, d3dRect.x2, d3dRect.y2);
            dwFlags = 0;
            if(dxw.dwFlags4 & (ZBUFFERCLEAN | ZBUFFER0CLEAN)) dwFlags = D3DCLEAR_ZBUFFER;
            if(dxw.dwFlags5 & CLEARTARGET) dwFlags |= D3DCLEAR_TARGET;
            res = gViewport1->Clear(1, &d3dRect, dwFlags);
            _if(res) OutTraceE("BeginScene: viewport Clear ERROR res=%#x(%s)\n", res, ExplainDDError(res));
        }
    }
    res = (*pBeginScene2)(d3dd);
    if(res == DDERR_SURFACELOST) {
        OutTraceDW("BeginScene: recovering from DDERR_SURFACELOST\n");
        lpPrimaryDD->EnumSurfaces(DDENUMSURFACES_DOESEXIST | DDENUMSURFACES_ALL, NULL, NULL, (LPDDENUMSURFACESCALLBACK)dxwRestoreCallback2);
        res = (*pBeginScene2)(d3dd);
    }
    _if(res) OutTraceE("BeginScene: res=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extBeginScene3(void *d3dd) {
    HRESULT res;
    OutTraceD3D("BeginScene(D3D3): d3dd=%#x\n", d3dd);
    if(dxw.dwFlags5 & LIMITBEGINSCENE) LimitFrameCount(dxw.MaxFPS);
    if((dxw.dwFlags4 & (ZBUFFERCLEAN | ZBUFFER0CLEAN)) || (dxw.dwFlags5 & CLEARTARGET)) {
        LPDIRECT3DVIEWPORT3 vp;
        D3DVIEWPORT vpd;
        res = ((LPDIRECT3DDEVICE3)d3dd)->GetCurrentViewport(&vp);
        if(!res) {
            D3DRECT d3dRect;
            vpd.dwSize = sizeof(D3DVIEWPORT);
            vp->GetViewport(&vpd);
            d3dRect.x1 = vpd.dwX;
            d3dRect.y1 = vpd.dwY;
            d3dRect.x2 = vpd.dwX + vpd.dwWidth;
            d3dRect.y2 = vpd.dwY + vpd.dwHeight;
            OutTraceD3D("d3dRect=(%d,%d)-(%d,%d)\n", d3dRect.x1, d3dRect.y1, d3dRect.x2, d3dRect.y2);
            if(dxw.dwFlags4 & ZBUFFERCLEAN )vp->Clear2(1, &d3dRect, D3DCLEAR_ZBUFFER, 0, 1.0, 0);
            if(dxw.dwFlags4 & ZBUFFER0CLEAN)vp->Clear2(1, &d3dRect, D3DCLEAR_ZBUFFER, 0, 0.0, 0);
            if(dxw.dwFlags5 & CLEARTARGET) vp->Clear(1, &d3dRect, D3DCLEAR_TARGET);
        }
    }
    res = (*pBeginScene3)(d3dd);
    if(res == DDERR_SURFACELOST) {
        OutTraceDW("BeginScene: recovering from DDERR_SURFACELOST\n");
        lpPrimaryDD->EnumSurfaces(DDENUMSURFACES_DOESEXIST | DDENUMSURFACES_ALL, NULL, NULL, (LPDDENUMSURFACESCALLBACK)dxwRestoreCallback2);
        res = (*pBeginScene3)(d3dd);
    }
    _if(res) OutTraceE("BeginScene: res=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

static HRESULT WINAPI dxwRestoreCallback7(LPDIRECTDRAWSURFACE7 lpDDSurface, LPDDSURFACEDESC2 lpDDSurfaceDesc, LPVOID lpContext) {
    HRESULT res;
    OutTrace("dxwRestoreCallback7: ANALYZING lpdds=%#x\n", lpDDSurface);
    if(lpDDSurface->IsLost()) {
        if(res = lpDDSurface->Restore()) {
            OutTrace("dxwRestoreCallback7: RESTORE FAILED lpdds=%#x err=%#x(%s)\n", lpDDSurface, res, ExplainDDError(res));
            return DDENUMRET_CANCEL;
        }
        OutTrace("dxwRestoreCallback7: RESTORED lpdds=%#x\n", lpDDSurface);
    }
    return DDENUMRET_OK;
}

HRESULT WINAPI extBeginScene7(void *d3dd) {
    HRESULT res;
    OutTraceD3D("BeginScene(D3D7): d3dd=%#x\n", d3dd);
    if(dxw.dwFlags5 & LIMITBEGINSCENE) LimitFrameCount(dxw.MaxFPS);
    // there is no Clear method for Viewport object in D3D7 !!!
    res = (*pBeginScene7)(d3dd);
    if(res == DDERR_SURFACELOST) {
        OutTraceDW("BeginScene: recovering from DDERR_SURFACELOST\n");
        lpPrimaryDD->EnumSurfaces(DDENUMSURFACES_DOESEXIST | DDENUMSURFACES_ALL, NULL, NULL, (LPDDENUMSURFACESCALLBACK)dxwRestoreCallback7);
        res = (*pBeginScene7)(d3dd);
    }
    _if(res) OutTraceE("BeginScene: res=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

static HRESULT WINAPI extEndScene(int d3dversion, Scene_Type pEndScene, void *d3dd) {
    HRESULT res;
    OutTraceD3D("EndScene(D3D%d): d3dd=%#x\n", d3dversion, d3dd);
    res = (*pEndScene)(d3dd);
    //dxw.ShowOverlay();
    _if(res) OutTraceE("EndScene: res=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extEndScene1(void *d3dd) {
    return extEndScene(1, pEndScene1, d3dd);
}
HRESULT WINAPI extEndScene2(void *d3dd) {
    return extEndScene(2, pEndScene2, d3dd);
}
HRESULT WINAPI extEndScene3(void *d3dd) {
    return extEndScene(3, pEndScene3, d3dd);
}
HRESULT WINAPI extEndScene7(void *d3dd) {
    return extEndScene(7, pEndScene7, d3dd);
}

HRESULT WINAPI extD3DGetCaps(int d3dversion, D3DGetCaps_Type pD3DGetCaps, void *d3dd, LPD3DDEVICEDESC hd, LPD3DDEVICEDESC sd) {
    HRESULT res;
    OutTraceD3D("GetCaps(D3D%d): d3dd=%#x hd=%#x sd=%#x\n", d3dversion, d3dd, hd, sd);
    res = (*pD3DGetCaps)(d3dd, hd, sd);
    if(res) {
        OutTraceE("GetCaps(D3D): ERROR res=%#x(%s)\n", res, ExplainDDError(res));
        return res;
    }
#ifndef DXW_NOTRACES
    DumpD3DDeviceDesc(hd, "HWDEV");
    DumpD3DDeviceDesc(sd, "SWDEV");
#endif // DXW_NOTRACES
    if(dxw.dwFlags4 & NOPOWER2FIX) {
        if(hd) {
            OutTraceDW("GetCaps(D3D): Fixing NOPOWER2FIX hw caps\n");
            hd->dpcLineCaps.dwTextureCaps |= D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_POW2;
            hd->dpcTriCaps.dwTextureCaps |= D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_POW2;
        }
        if(sd) {
            OutTraceDW("GetCaps(D3D): Fixing NOPOWER2FIX sw caps\n");
            sd->dpcLineCaps.dwTextureCaps |= D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_POW2;
            sd->dpcTriCaps.dwTextureCaps |= D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_POW2;
        }
    }
    return res;
}

HRESULT WINAPI extD3DGetCaps1(void *d3dd, LPD3DDEVICEDESC hd, LPD3DDEVICEDESC sd) {
    return extD3DGetCaps(1, pD3DGetCaps1, d3dd, hd, sd);
}
HRESULT WINAPI extD3DGetCaps2(void *d3dd, LPD3DDEVICEDESC hd, LPD3DDEVICEDESC sd) {
    return extD3DGetCaps(2, pD3DGetCaps2, d3dd, hd, sd);
}
HRESULT WINAPI extD3DGetCaps3(void *d3dd, LPD3DDEVICEDESC hd, LPD3DDEVICEDESC sd) {
    return extD3DGetCaps(3, pD3DGetCaps3, d3dd, hd, sd);
}

HRESULT WINAPI extD3DGetCaps7(void *d3dd, LPD3DDEVICEDESC7 lpd3ddd) {
    HRESULT res;
    OutTraceD3D("GetCaps(D3D7): d3dd=%#x lpd3ddd=%#x\n", d3dd, lpd3ddd);
    res = (*pD3DGetCaps7)(d3dd, lpd3ddd);
    if(res) {
        OutTraceE("GetCaps(D3D): ERROR res=%#x(%s)\n", res, ExplainDDError(res));
        return res;
    }
#ifndef DXW_NOTRACES
    DumpD3DDeviceDesc7(lpd3ddd, "DEV7");
#endif // DXW_NOTRACES
    if(dxw.dwFlags4 & NOPOWER2FIX) {
        if(lpd3ddd) {
            OutTraceDW("GetCaps(D3D): Fixing NOPOWER2FIX hw caps\n");
            lpd3ddd->dpcLineCaps.dwTextureCaps |= D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_POW2;
            lpd3ddd->dpcTriCaps.dwTextureCaps |= D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_POW2;
        }
    }
    if(dxw.dwFlags11 & TRANSFORMANDLIGHT) {
        if(lpd3ddd) {
            OutTraceDW("GetCaps(D3D): Adding HWTRANSFORMANDLIGHT caps\n");
            lpd3ddd->dwDevCaps |= D3DDEVCAPS_HWTRANSFORMANDLIGHT;
        }
    }
    if(dxw.dwFlags12 & NOTNLDEVICE) {
        if(lpd3ddd) {
            OutTraceDW("GetCaps(D3D): Removing HWTRANSFORMANDLIGHT caps\n");
            lpd3ddd->dwDevCaps &= ~D3DDEVCAPS_HWTRANSFORMANDLIGHT;
        }
    }
    return res;
}

HRESULT WINAPI extSetLightState3(void *d3dd, D3DLIGHTSTATETYPE d3dls, DWORD t) {
    HRESULT res;
    OutTraceD3D("SetLightState(D3D3): d3d=%#x lightstate=%#x t=%#x\n", d3dd, d3dls, t);
    res = (*pSetLightState3)(d3dd, d3dls, t);
    _if(res) OutTraceE("SetLightState: ERROR res=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extAddViewport1(void *d3dd, LPDIRECT3DVIEWPORT lpd3dvp) {
    HRESULT res;
    static VOID *LastDevice = 0;
    OutTraceD3D("AddViewport(D3D1): d3d=%#x d3dvp=%#x\n", d3dd, lpd3dvp);
    res = (*pAddViewport1)(d3dd, lpd3dvp);
    if((res == DDERR_INVALIDPARAMS) && LastDevice) {
        // going through here fixes "Die hard trilogy" "DirectX error 15" caused by an AddViewport failure
        OutTraceE("AddViewport DDERR_INVALIDPARAMS: try to unlink from d3dd=%#x\n", LastDevice);
        res = ((LPDIRECT3DDEVICE)LastDevice)->DeleteViewport(lpd3dvp);
        _if(res) OutTrace("DeleteViewport: ERROR res=%#x(%s)\n", res, ExplainDDError(res));
        res = (*pAddViewport1)(d3dd, lpd3dvp);
    }
    if(res) {
        OutTraceE("AddViewport: ERROR res=%#x(%s)\n", res, ExplainDDError(res));
        if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    } else {
        LastDevice = d3dd;
        lpCurrViewport = lpd3dvp;
    }
    return res;
}

HRESULT WINAPI extAddViewport2(void *d3dd, LPDIRECT3DVIEWPORT2 lpd3dvp) {
    HRESULT res;
    static VOID *LastDevice = 0;
    OutTraceD3D("AddViewport(D3D2): d3d=%#x d3dvp=%#x\n", d3dd, lpd3dvp);
    res = (*pAddViewport2)(d3dd, lpd3dvp);
    if((res == DDERR_INVALIDPARAMS) && LastDevice) {
        OutTraceE("AddViewport DDERR_INVALIDPARAMS: try to unlink from d3dd=%#x\n", LastDevice);
        res = ((LPDIRECT3DDEVICE2)LastDevice)->DeleteViewport(lpd3dvp);
        _if(res) OutTrace("DeleteViewport: ERROR res=%#x(%s)\n", res, ExplainDDError(res));
        res = (*pAddViewport2)(d3dd, lpd3dvp);
    }
    if(res) {
        OutTraceE("AddViewport ERROR: res=%#x(%s)\n", res, ExplainDDError(res));
        if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    } else
        LastDevice = d3dd;
    return res;
}

HRESULT WINAPI extAddViewport3(void *d3dd, LPDIRECT3DVIEWPORT3 lpd3dvp) {
    HRESULT res;
    static VOID *LastDevice = 0;
    OutTraceD3D("AddViewport(D3D3): d3d=%#x d3dvp=%#x\n", d3dd, lpd3dvp);
    res = (*pAddViewport3)(d3dd, lpd3dvp);
    if((res == DDERR_INVALIDPARAMS) && LastDevice) {
        OutTraceE("AddViewport DDERR_INVALIDPARAMS: try to unlink from d3dd=%#x\n", LastDevice);
        res = ((LPDIRECT3DDEVICE3)LastDevice)->DeleteViewport(lpd3dvp);
        _if(res) OutTrace("DeleteViewport ERROR: res=%#x(%s)\n", res, ExplainDDError(res));
        res = (*pAddViewport3)(d3dd, lpd3dvp);
    }
    if(res) {
        OutTraceE("AddViewport ERROR: res=%#x(%s)\n", res, ExplainDDError(res));
        if(dxw.dwFlags1 & SUPPRESSDXERRORS) res = DD_OK;
    } else
        LastDevice = d3dd;
    return res;
}

HRESULT WINAPI extSetViewport2_2(void *lpvp, LPD3DVIEWPORT2 vpd) {
    HRESULT res;
    OutTraceD3D("SetViewport2(VP2): viewport=%#x viewportd=%#x size=%d pos=(%d,%d) dim=(%dx%d) clippos=(%f:%f) clipsize=(%f:%f) Z=(%f:%f)\n",
                lpvp, vpd, vpd->dwSize, vpd->dwX, vpd->dwY, vpd->dwWidth, vpd->dwHeight,
                vpd->dvClipX, vpd->dvClipY, vpd->dvClipWidth, vpd->dvClipHeight,
                vpd->dvMinZ, vpd->dvMaxZ);
    res = (*pSetViewport2_2)(lpvp, vpd);
    if(res) OutTraceE("SetViewport2(VP2) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("SetViewport2(VP2): OK\n");
    return res;
}

HRESULT WINAPI extGetViewport2_2(void *lpvp, LPD3DVIEWPORT2 vpd) {
    HRESULT res;
    OutTraceD3D("GetViewport2(VP2): viewport=%#x viewportd=%#x\n", lpvp, vpd);
    res = (*pGetViewport2_2)(lpvp, vpd);
    if(res) OutTraceE("GetViewport2(VP2) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("GetViewport2(VP2): OK size=%d pos=(%d,%d) dim=(%dx%d)\n",
                         vpd->dwSize, vpd->dwX, vpd->dwY, vpd->dwWidth, vpd->dwHeight);
    return res;
}

HRESULT WINAPI extSetViewport2_3(void *lpvp, LPD3DVIEWPORT2 vpd) {
    HRESULT res;
    OutTraceD3D("SetViewport2(VP3): viewport=%#x viewportd=%#x size=%d pos=(%d,%d) dim=(%dx%d) Z=(%f-%f)\n",
                lpvp, vpd, vpd->dwSize, vpd->dwX, vpd->dwY, vpd->dwWidth, vpd->dwHeight, vpd->dvMinZ, vpd->dvMaxZ);
    res = (*pSetViewport2_3)(lpvp, vpd);
    if(res) OutTraceE("SetViewport2(VP3) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("SetViewport2(VP3): OK\n");
    return res;
}

HRESULT WINAPI extGetViewport2_3(void *lpvp, LPD3DVIEWPORT2 vpd) {
    HRESULT res;
    OutTraceD3D("GetViewport2(VP3): viewport=%#x viewportd=%#x\n", lpvp, vpd);
    res = (*pGetViewport2_3)(lpvp, vpd);
    if(res) OutTraceE("GetViewport2(VP3) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("GetViewport2(VP3): OK size=%d pos=(%d,%d) dim=(%dx%d)\n",
                         vpd->dwSize, vpd->dwX, vpd->dwY, vpd->dwWidth, vpd->dwHeight);
    return res;
}

HRESULT WINAPI extGetCurrentViewport2(void *d3dd, LPDIRECT3DVIEWPORT2 *lpd3dvp) {
    HRESULT res;
    OutTraceD3D("GetCurrentViewport(D3DD2): d3dd=%#x viewportd=%#x\n", d3dd, lpd3dvp);
    res = (*pGetCurrentViewport2)(d3dd, lpd3dvp);
    if(res) {
        OutTraceE("GetCurrentViewport(D3DD2) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        return res;
    }
    HookViewport((LPDIRECT3DVIEWPORT *)lpd3dvp, 2);
    return res;
}

HRESULT WINAPI extSetCurrentViewport2(void *d3dd, LPDIRECT3DVIEWPORT2 lpd3dvp) {
    HRESULT res;
    OutTraceD3D("SetCurrentViewport(D3DD2): d3dd=%#x viewportd=%#x\n", d3dd, lpd3dvp);
    res = (*pSetCurrentViewport2)(d3dd, lpd3dvp);
    if(res) OutTraceE("SetCurrentViewport(D3DD2) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("SetCurrentViewport(D3DD2): OK\n");
    return res;
}

HRESULT WINAPI extGetCurrentViewport3(void *d3dd, LPDIRECT3DVIEWPORT3 *lpd3dvp) {
    HRESULT res;
    OutTraceD3D("GetCurrentViewport(D3DD3): d3dd=%#x viewportd=%#x\n", d3dd, lpd3dvp);
    res = (*pGetCurrentViewport3)(d3dd, lpd3dvp);
    if(res) {
        OutTraceE("GetCurrentViewport(D3DD3) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        return res;
    }
    HookViewport((LPDIRECT3DVIEWPORT *)lpd3dvp, 3);
    if(IsDebugD3D) {
        HRESULT res2;
        D3DVIEWPORT2 vpdesc;
        vpdesc.dwSize = sizeof(D3DVIEWPORT2);
        res2 = (*pGetViewport2_3)(lpd3dvp, &vpdesc);
        if(res2)
            OutTraceE("GetCurrentViewport(D3DD3) GetViewport2 ERROR: err=%#x(%s) at %d\n", res2, ExplainDDError(res2), __LINE__);
        else
            OutTraceD3D("GetCurrentViewport(D3DD3): size=%d pos=(%d,%d) dim=(%dx%d)\n",
                        vpdesc.dwSize, vpdesc.dwX, vpdesc.dwY, vpdesc.dwWidth, vpdesc.dwHeight);
    }
    return res;
}

HRESULT WINAPI extSetCurrentViewport3(void *lpvp, LPDIRECT3DVIEWPORT3 lpd3dvp) {
    HRESULT res;
    OutTraceD3D("SetCurrentViewport(D3DD3): viewport=%#x\n", lpvp, lpd3dvp);
    res = (*pSetCurrentViewport3)(lpvp, lpd3dvp);
    if(res) OutTraceE("SetCurrentViewport(D3DD3) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("SetCurrentViewport(D3DD3): OK\n");
    if(IsDebugD3D) {
        HRESULT res2;
        D3DVIEWPORT2 vpdesc;
        vpdesc.dwSize = sizeof(D3DVIEWPORT2);
        res2 = (*pGetViewport2_3)(lpd3dvp, &vpdesc);
        if(res2)
            OutTraceE("SetCurrentViewport(D3DD3) GetViewport2 ERROR: err=%#x(%s) at %d\n", res2, ExplainDDError(res2), __LINE__);
        else
            OutTraceD3D("SetCurrentViewport(D3DD3): size=%d pos=(%d,%d) dim=(%dx%d)\n",
                        vpdesc.dwSize, vpdesc.dwX, vpdesc.dwY, vpdesc.dwWidth, vpdesc.dwHeight);
    }
    return res;
}

HRESULT WINAPI extDeleteViewport1(void *d3dd, LPDIRECT3DVIEWPORT lpd3dvp) {
    HRESULT res;
    OutTraceD3D("DeletetViewport(D3DD1): d3dd=%#x viewport=%#x\n", d3dd, lpd3dvp);
    res = (*pDeleteViewport1)(d3dd, lpd3dvp);
    if(res) OutTraceE("DeletetViewport(D3DD1) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("DeletetViewport(D3DD1): OK\n");
    return res;
}

HRESULT WINAPI extNextViewport1(void *d3dd, LPDIRECT3DVIEWPORT lpd3dvp, LPDIRECT3DVIEWPORT *vpnext, DWORD dw) {
    HRESULT res;
    OutTraceD3D("NextViewport(D3DD1): d3dd=%#x viewport=%#x dw=%#x\n", d3dd, lpd3dvp);
    res = (*pNextViewport1)(d3dd, lpd3dvp, vpnext, dw);
    if(res) OutTraceE("NextViewport(D3DD1) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("NextViewport(D3DD1): next=%#x\n", *vpnext);
    return res;
}

HRESULT WINAPI extDeleteViewport2(void *d3dd, LPDIRECT3DVIEWPORT2 lpd3dvp) {
    HRESULT res;
    OutTraceD3D("DeletetViewport(D3DD2): d3dd=%#x viewport=%#x\n", d3dd, lpd3dvp);
    res = (*pDeleteViewport2)(d3dd, lpd3dvp);
    if(res) OutTraceE("DeletetViewport(D3DD2) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("DeletetViewport(D3DD2): OK\n");
    return res;
}

HRESULT WINAPI extNextViewport2(void *d3dd, LPDIRECT3DVIEWPORT2 lpd3dvp, LPDIRECT3DVIEWPORT2 *vpnext, DWORD dw) {
    HRESULT res;
    OutTraceD3D("NextViewport(D3DD2): d3dd=%#x viewport=%#x dw=%#x\n", d3dd, lpd3dvp);
    res = (*pNextViewport2)(d3dd, lpd3dvp, vpnext, dw);
    if(res) OutTraceE("NextViewport(D3DD2) ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("NextViewport(D3DD2): next=%#x\n", *vpnext);
    return res;
}

HRESULT WINAPI extSetTexture3(void *d3dd, DWORD flags, LPDIRECT3DTEXTURE2 lptex) {
    HRESULT res;
    ApiName("SetTexture(D3D3)");
    OutTraceD3D("%s: d3dd=%#x flags=%#x tex=%#x\n", ApiRef, d3dd, flags, lptex);
    if (dxw.dwFlags4 & NOTEXTURES) return DD_OK;
    res = (*pSetTexture3)(d3dd, flags, lptex);
    _if(res) OutTraceD3D("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extSetTexture7(void *d3dd, DWORD flags, LPDIRECTDRAWSURFACE7 lptex) {
    HRESULT res;
    ApiName("SetTexture(D3D7)");
    OutTraceD3D("%s: d3dd=%#x flags=%#x tex=%#x\n", ApiRef, d3dd, flags, lptex);
    if (dxw.dwFlags4 & NOTEXTURES) return DD_OK;
    res = (*pSetTexture7)(d3dd, flags, lptex);
    _if(res) OutTraceD3D("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

#ifdef TRACEMATERIAL
HRESULT WINAPI extSetMaterial(void *d3dd, LPD3DMATERIAL lpMaterial) {
    HRESULT res;
    OutTraceD3D("SetMaterial: d3dd=%#x material=%#x\n", d3dd, lpMaterial);
    if(lpMaterial && IsDebugD3D) {
        OutTraceD3D("Material: Size=%d Texture=%#x diffuse=(%f,%f,%f,%f) ambient=(%f,%f,%f,%f) specular=(%f,%f,%f,%f) emissive=(%f,%f,%f,%f) power=%f\n",
                    lpMaterial->dwSize, lpMaterial->hTexture,
                    lpMaterial->diffuse.a, lpMaterial->diffuse.r, lpMaterial->diffuse.g, lpMaterial->diffuse.b,
                    lpMaterial->ambient.a, lpMaterial->ambient.r, lpMaterial->ambient.g, lpMaterial->ambient.b,
                    lpMaterial->specular.a, lpMaterial->specular.r, lpMaterial->specular.g, lpMaterial->specular.b,
                    lpMaterial->emissive.a, lpMaterial->emissive.r, lpMaterial->emissive.g, lpMaterial->emissive.b,
                    lpMaterial->power
                   );
    }
    res = (*pSetMaterial)(d3dd, lpMaterial);
    if(res) OutTraceD3D("SetMaterial: ERROR res=%#x\n", res);
    return res;
}

HRESULT WINAPI extGetMaterial(void *d3dd, LPD3DMATERIAL lpMaterial) {
    HRESULT res;
    res = (*pGetMaterial)(d3dd, lpMaterial);
    OutTraceD3D("GetMaterial: d3dd=%#x material=%#x res=%#x\n", d3dd, lpMaterial, res);
    if(lpMaterial && IsDebugD3D && (res == DD_OK)) {
        OutTraceD3D("Material: Size=%d diffuse=(%f,%f,%f,%f) ambient=(%f,%f,%f,%f) specular=(%f,%f,%f,%f) emissive=(%f,%f,%f,%f) power=%f\n",
                    lpMaterial->dwSize,
                    lpMaterial->diffuse.a, lpMaterial->diffuse.r, lpMaterial->diffuse.g, lpMaterial->diffuse.b,
                    lpMaterial->ambient.a, lpMaterial->ambient.r, lpMaterial->ambient.g, lpMaterial->ambient.b,
                    lpMaterial->specular.a, lpMaterial->specular.r, lpMaterial->specular.g, lpMaterial->specular.b,
                    lpMaterial->emissive.a, lpMaterial->emissive.r, lpMaterial->emissive.g, lpMaterial->emissive.b,
                    lpMaterial->power
                   );
    }
    return res;
}
#endif

HRESULT WINAPI extSwapTextureHandles(void *d3dd, LPDIRECT3DTEXTURE t1, LPDIRECT3DTEXTURE t2) {
    HRESULT res;
    OutTraceD3D("SwapTextureHandles(D3D1): d3dd=%#x t1=%#x t2=%#x\n", d3dd, t1, t2);
    if (dxw.dwFlags4 & NOTEXTURES) return DD_OK;
    res = (*pSwapTextureHandles)(d3dd, t1, t2);
    _if(res) OutTraceD3D("SwapTextureHandles: ERROR res=%#x\n", res);
    return res;
}

HRESULT WINAPI extSwapTextureHandles2(void *d3dd, LPDIRECT3DTEXTURE2 t1, LPDIRECT3DTEXTURE2 t2) {
    HRESULT res;
    OutTraceD3D("SwapTextureHandles(D3D2): d3dd=%#x t1=%#x t2=%#x\n", d3dd, t1, t2);
    if (dxw.dwFlags4 & NOTEXTURES) return DD_OK;
    res = (*pSwapTextureHandles2)(d3dd, t1, t2);
    _if(res) OutTraceD3D("SwapTextureHandles: ERROR res=%#x\n", res);
    return res;
}

#ifdef TRACEALL
HRESULT WINAPI extTexInitialize(void *t, LPDIRECT3DDEVICE lpd3dd, LPDIRECTDRAWSURFACE lpdds) {
    OutTraceD3D("Texture::Initialize\n");
    return (*pTInitialize)(t, lpd3dd, lpdds);
}

static HRESULT WINAPI extTexGetHandle(int version, TexGetHandle_Type pTGetHandle, void *t, LPDIRECT3DDEVICE lpd3dd, LPD3DTEXTUREHANDLE lpth) {
    HRESULT ret;
    OutTraceD3D("Texture(%d)::GetHandle lpt=%#x lpd3dd=%#x lpth=%#x\n", version, t, lpd3dd, lpth);
    ret = (*pTGetHandle)(t, lpd3dd, lpth);
    _if(ret) OutTraceE("Texture::Load ERROR res=%#x(%s)\n", ret, ExplainDDError(ret));
    return ret;
}

HRESULT WINAPI extTexGetHandle1(void *t, LPDIRECT3DDEVICE lpd3dd, LPD3DTEXTUREHANDLE lpth) {
    return extTexGetHandle(1, pTGetHandle1, t, lpd3dd, lpth);
}
HRESULT WINAPI extTexGetHandle2(void *t, LPDIRECT3DDEVICE2 lpd3dd, LPD3DTEXTUREHANDLE lpth) {
    return extTexGetHandle(2, pTGetHandle2, t, (LPDIRECT3DDEVICE)lpd3dd, lpth);
}


static HRESULT WINAPI extTexPaletteChanged(int version, TexPaletteChanged_Type pTPaletteChanged, void *t, DWORD dw1, DWORD dw2) {
    HRESULT ret;
    OutTraceD3D("Texture(%d)::PaletteChanged lpt=%#x dw1=%#x dw2=%#x\n", version, t, dw1, dw2);
    ret = (*pTPaletteChanged)(t, dw1, dw2);
    _if(ret) OutTraceE("Texture::PaletteChanged ERROR res=%#x(%s)\n", ret, ExplainDDError(ret));
    return ret;
}

HRESULT WINAPI extTexPaletteChanged1(void *t, DWORD dw1, DWORD dw2) {
    return extTexPaletteChanged(1, pTPaletteChanged1, t, dw1, dw2);
}
HRESULT WINAPI extTexPaletteChanged2(void *t, DWORD dw1, DWORD dw2) {
    return extTexPaletteChanged(2, pTPaletteChanged2, t, dw1, dw2);
}

HRESULT WINAPI extTexLoad(int version, TexLoad_Type pTLoad, void *t, LPDIRECT3DTEXTURE lpt) {
    HRESULT ret;
    OutTraceD3D("Texture(%d)::Load lpt=%#x lpd3dt=%#x\n", version, t, lpt);
    ret = (*pTLoad)(t, lpt);
    _if(ret) OutTraceE("Texture::Load ERROR res=%#x(%s)\n", ret, ExplainDDError(ret));
    return ret;
}

HRESULT WINAPI extTexLoad1(void *t, LPDIRECT3DTEXTURE lpt) {
    return extTexLoad(1, pTLoad1, t, lpt);
}
HRESULT WINAPI extTexLoad2(void *t, LPDIRECT3DTEXTURE lpt) {
    return extTexLoad(2, pTLoad2, t, lpt);
}

HRESULT WINAPI extTexUnload(void *t) {
    HRESULT ret;
    OutTraceD3D("Texture::Unload lpt=%#x\n", t);
    ret = (*pTUnload)(t);
    _if(ret) OutTraceE("Texture::Load ERROR res=%#x(%s)\n", ret, ExplainDDError(ret));
    return ret;
}
#endif

typedef struct {
    VOID *cb;
    LPVOID arg;
} CallbackPixFmtArg;

HRESULT WINAPI extZBufferProxy(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext) {
    HRESULT res;
    OutTraceD3D("EnumZBufferFormats: CALLBACK context=%#x %s \n", ((CallbackPixFmtArg *)lpContext)->arg, ExplainPixelFormat(lpDDPixFmt));
    res = (*(LPD3DENUMPIXELFORMATSCALLBACK)(((CallbackPixFmtArg *)lpContext)->cb))(lpDDPixFmt, ((CallbackPixFmtArg *)lpContext)->arg);
    OutTraceD3D("EnumZBufferFormats: CALLBACK ret=%#x\n", res);
    return res;
}

#define MAXPFTABLESIZE 50
#define MAXTRIMMEDENTRIES 3

typedef struct {
    int nEntries;
    //	LPDDPIXELFORMAT lpPixelFormatEntries;
    DDPIXELFORMAT lpPixelFormatEntries[MAXPFTABLESIZE];
} PixelFormatTable_Type;

HRESULT WINAPI FillPixelFormatTable(LPDDPIXELFORMAT lpDDPixFmt, LPVOID Arg) {
    PixelFormatTable_Type *lpPixelFormatTable = (PixelFormatTable_Type *)Arg;
    OutTraceD3D("EnumZBufferFormats: FILL CALLBACK entry=%d %s\n", lpPixelFormatTable->nEntries, ExplainPixelFormat(lpDDPixFmt));
    if(lpPixelFormatTable->nEntries >= MAXPFTABLESIZE) return FALSE;
    memcpy((LPVOID) & (lpPixelFormatTable->lpPixelFormatEntries[lpPixelFormatTable->nEntries]), (LPVOID)lpDDPixFmt, sizeof(DDPIXELFORMAT));
    lpPixelFormatTable->nEntries ++;
    //lpPixelFormatTable->lpPixelFormatEntries = (LPDDPIXELFORMAT)realloc((LPVOID)(lpPixelFormatTable->lpPixelFormatEntries), lpPixelFormatTable->nEntries * sizeof(DDPIXELFORMAT));
    //OutTraceD3D("lp=%#x err=%s\n", lpPixelFormatTable->lpPixelFormatEntries, GetLastError());
    return TRUE;
}

static HRESULT WINAPI extEnumZBufferFormats(int d3dversion, EnumZBufferFormats_Type pEnumZBufferFormats, void *lpd3d, REFCLSID riidDevice, LPD3DENUMPIXELFORMATSCALLBACK lpEnumCallback, LPVOID lpContext) {
    HRESULT ret;
    OutTraceD3D("Direct3D::EnumZBufferFormats(D3D%d) d3d=%#x clsid=%#x context=%#x\n", d3dversion, lpd3d, riidDevice.Data1, lpContext);
    if(dxw.dwFlags8 & TRIMTEXTUREFORMATS) {
        int iIndex;
        int iEnumerated;
        PixelFormatTable_Type PixelFormatTable;
        PixelFormatTable.nEntries = 0;
        //PixelFormatTable.lpPixelFormatEntries = (LPDDPIXELFORMAT)malloc(sizeof(DDPIXELFORMAT));
        ret = (*pEnumZBufferFormats)(lpd3d, riidDevice, (LPD3DENUMPIXELFORMATSCALLBACK)FillPixelFormatTable, (LPVOID)&PixelFormatTable);
        OutTraceD3D("EnumZBufferFormats: collected entries=%d\n", PixelFormatTable.nEntries);
        // bubble sorting;
        while(TRUE) {
            BOOL bSorted = FALSE;
            for(iIndex = 0; iIndex < PixelFormatTable.nEntries - 1; iIndex++) {
                if(PixelFormatTable.lpPixelFormatEntries[iIndex].dwRGBBitCount > PixelFormatTable.lpPixelFormatEntries[iIndex + 1].dwRGBBitCount) {
                    DDPIXELFORMAT tmp;
                    tmp = PixelFormatTable.lpPixelFormatEntries[iIndex];
                    PixelFormatTable.lpPixelFormatEntries[iIndex] = PixelFormatTable.lpPixelFormatEntries[iIndex + 1];
                    PixelFormatTable.lpPixelFormatEntries[iIndex + 1] = tmp;
                    bSorted = TRUE;
                }
            }
            if(!bSorted) break;
        }
        for(iIndex = 0, iEnumerated = 0; (iIndex < PixelFormatTable.nEntries) && (iEnumerated < MAXTRIMMEDENTRIES); iIndex++) {
            if(PixelFormatTable.lpPixelFormatEntries[iIndex].dwRGBBitCount >= 32) break;
            if((dxw.dwFlags7 & CLEARTEXTUREFOURCC) && (PixelFormatTable.lpPixelFormatEntries[iIndex].dwFlags & DDPF_FOURCC)) continue;
            ret = (*lpEnumCallback)(&(PixelFormatTable.lpPixelFormatEntries[iIndex]), lpContext);
            OutTraceD3D("Direct3D::EnumZBufferFormats: CALLBACK entry=%d ret=%#x %s\n", iIndex, ret, ExplainPixelFormat(&PixelFormatTable.lpPixelFormatEntries[iIndex]));
            if(!ret) break;
            iEnumerated++;
        }
        //free((LPVOID)(PixelFormatTable.lpPixelFormatEntries));
        ret = DD_OK;
    } else {
        CallbackPixFmtArg Arg;
        Arg.cb = lpEnumCallback; // v2.04.71.fx1: fix (deleted &) - tested with "Choanikimura"
        Arg.arg = lpContext;
        ret = (*pEnumZBufferFormats)(lpd3d, riidDevice, (LPD3DENUMPIXELFORMATSCALLBACK)extZBufferProxy, (LPVOID)&Arg);
    }
    _if(ret) OutTraceE("Direct3D::EnumZBufferFormats res=%#x(%s)\n", ret, ExplainDDError(ret));
    return ret;
}

HRESULT WINAPI extEnumZBufferFormats3(void *lpd3d, REFCLSID riidDevice, LPD3DENUMPIXELFORMATSCALLBACK lpEnumCallback, LPVOID lpContext) {
    return extEnumZBufferFormats(3, pEnumZBufferFormats3, lpd3d, riidDevice, lpEnumCallback, lpContext);
}
HRESULT WINAPI extEnumZBufferFormats7(void *lpd3d, REFCLSID riidDevice, LPD3DENUMPIXELFORMATSCALLBACK lpEnumCallback, LPVOID lpContext) {
    return extEnumZBufferFormats(7, pEnumZBufferFormats7, lpd3d, riidDevice, lpEnumCallback, lpContext);
}

// Beware: using service surfaces with DDSCAPS_SYSTEMMEMORY capability may lead to crashes in D3D operations
// like Vievport::Clear() in "Forsaken" set in emulation AERO-friendly mode. To avoid the problem, you can
// suppress the offending cap by use of the NOSYSMEMPRIMARY or NOSYSMEMBACKBUF flags

#ifdef TRACEALL
HRESULT WINAPI extViewportClear(void *lpd3dvp, DWORD dwNRect, LPD3DRECT lpRect, DWORD dwFlags) {
    HRESULT ret;
#ifndef DXW_NOTRACES
    if(IsTraceD3D) {
        OutTrace("Viewport::Clear lpd3dvp=%#x nrect=%#x flags=%#x\n", lpd3dvp, dwNRect, dwFlags);
        if(IsDebugD3D) {
            for(DWORD i = 0; i < dwNRect; i++) {
                OutTrace("> rect[%d]=(%d,%d)-(%d,%d)\n",
                         i, lpRect[i].x1, lpRect[i].y1, lpRect[i].x2, lpRect[i].y2);
            }
        }
    }
#endif
    // proxying the call ....
    ret = (*pViewportClear)(lpd3dvp, dwNRect, lpRect, dwFlags);
    OutTraceD3D("Viewport::Clear ret=%#x\n", ret);
    return ret;
}
#endif

static HRESULT CALLBACK lpTextureTrimmer12(LPDDSURFACEDESC lpDdsd, LPVOID lpContext) {
    HRESULT res;
    BOOL bSkip = FALSE;
    if((dxw.dwFlags7 & CLEARTEXTUREFOURCC) && (lpDdsd->ddpfPixelFormat.dwFlags & DDPF_FOURCC)) bSkip = TRUE;
    OutTraceD3D("EnumTextureFormats: %s context=%#x %s \n",
                bSkip ? "SKIP" : "CALLBACK",
                lpContext, ExplainPixelFormat(&(lpDdsd->ddpfPixelFormat)));
    if(bSkip)
        res = TRUE; // v2.05.44 fix
    else
        res = (*(LPD3DENUMTEXTUREFORMATSCALLBACK)(((CallbackPixFmtArg *)lpContext)->cb))(lpDdsd, ((CallbackPixFmtArg *)lpContext)->arg);
    return res;
}

static HRESULT CALLBACK lpTextureTrimmer37(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext) {
    HRESULT res;
    BOOL bSkip = FALSE;
    if((dxw.dwFlags7 & CLEARTEXTUREFOURCC) && (lpDDPixFmt->dwFlags & DDPF_FOURCC)) bSkip = TRUE;
    OutTraceD3D("EnumTextureFormats: %s context=%#x %s \n",
                bSkip ? "SKIP" : "CALLBACK",
                lpContext, ExplainPixelFormat(lpDDPixFmt));
    if(bSkip)
        res = TRUE; // v2.05.44 fix
    else
        res = (*(LPD3DENUMPIXELFORMATSCALLBACK)(((CallbackPixFmtArg *)lpContext)->cb))(lpDDPixFmt, ((CallbackPixFmtArg *)lpContext)->arg);
    return res;
}

HRESULT WINAPI extEnumTextureFormats12(int d3dversion, EnumTextureFormats12_Type pEnumTextureFormats, void *lpd3dd, LPD3DENUMTEXTUREFORMATSCALLBACK lptfcallback, LPVOID arg) {
    HRESULT res;
    OutTraceD3D("EnumTextureFormats(%d): lpd3dd=%#x cb=%#x arg=%#x\n", d3dversion, lpd3dd, lptfcallback, arg);
    if(IsDebugD3D || (dxw.dwFlags7 & CLEARTEXTUREFOURCC)) {
        CallbackPixFmtArg Arg;
        Arg.cb = (LPD3DENUMTEXTUREFORMATSCALLBACK)lptfcallback; // v2.04.77 fixed
        Arg.arg = arg;
        res = (*pEnumTextureFormats)(lpd3dd, lpTextureTrimmer12, (LPVOID)&Arg);
    } else
        res = (*pEnumTextureFormats)(lpd3dd, lptfcallback, arg);
    _if(res) OutTraceD3D("EnumTextureFormats: res=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extEnumTextureFormats37(int d3dversion, EnumTextureFormats37_Type pEnumTextureFormats, void *lpd3dd, LPD3DENUMPIXELFORMATSCALLBACK lptfcallback, LPVOID arg) {
    HRESULT res;
    OutTraceD3D("EnumTextureFormats(%d): lpd3dd=%#x cb=%#x arg=%#x\n", d3dversion, lpd3dd, lptfcallback, arg);
    if(IsDebugD3D || (dxw.dwFlags7 & CLEARTEXTUREFOURCC)) {
        CallbackPixFmtArg Arg;
        Arg.cb = (LPD3DENUMPIXELFORMATSCALLBACK)lptfcallback; // v2.04.77 fixed
        Arg.arg = arg;
        res = (*pEnumTextureFormats)(lpd3dd, lpTextureTrimmer37, (LPVOID)&Arg);
    } else
        res = (*pEnumTextureFormats)(lpd3dd, lptfcallback, arg);
    _if(res) OutTraceD3D("EnumTextureFormats: res=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extEnumTextureFormats1(void *lpd3dd, LPD3DENUMTEXTUREFORMATSCALLBACK lptfcallback, LPVOID arg) {
    return extEnumTextureFormats12(1, pEnumTextureFormats1, lpd3dd, lptfcallback, arg);
}
HRESULT WINAPI extEnumTextureFormats2(void *lpd3dd, LPD3DENUMTEXTUREFORMATSCALLBACK lptfcallback, LPVOID arg) {
    return extEnumTextureFormats12(2, pEnumTextureFormats2, lpd3dd, lptfcallback, arg);
}
HRESULT WINAPI extEnumTextureFormats3(void *lpd3dd, LPD3DENUMPIXELFORMATSCALLBACK lptfcallback, LPVOID arg) {
    return extEnumTextureFormats37(3, pEnumTextureFormats3, lpd3dd, lptfcallback, arg);
}
HRESULT WINAPI extEnumTextureFormats7(void *lpd3dd, LPD3DENUMPIXELFORMATSCALLBACK lptfcallback, LPVOID arg) {
    return extEnumTextureFormats37(7, pEnumTextureFormats7, lpd3dd, lptfcallback, arg);
}


HRESULT WINAPI extSetLight7(void *lpd3dd, DWORD Index, D3DLIGHT7 *light) {
    HRESULT res;
    extern void AdjustLight(D3DCOLORVALUE *, BYTE *);
#ifndef DXW_NOTRACES
    OutTraceD3D("SetLight(7): d3d=%#x index=%d\n", lpd3dd, Index);
    if(IsDebugD3D) {
        OutTrace("SetLight(7): diffuse={%f,%f,%f,%f}\n", light->dcvDiffuse.r, light->dcvDiffuse.g, light->dcvDiffuse.b, light->dcvDiffuse.a);
        OutTrace("SetLight(7): specular={%f,%f,%f,%f}\n", light->dcvSpecular.r, light->dcvSpecular.g, light->dcvSpecular.b, light->dcvSpecular.a);
        OutTrace("SetLight(7): ambient={%f,%f,%f,%f}\n", light->dcvAmbient.r, light->dcvAmbient.g, light->dcvAmbient.b, light->dcvAmbient.a);
        OutTrace("SetLight(7): attenuation={%f,%f,%f}\n", light->dvAttenuation0, light->dvAttenuation1, light->dvAttenuation2);
    }
#endif
    if(GetHookInfo()->GammaControl) {
        BYTE *Gamma;
        D3DLIGHT7 newlight = *light;
        Gamma = GetHookInfo()->GammaRamp;
        AdjustLight(&newlight.dcvDiffuse, Gamma);
        AdjustLight(&newlight.dcvSpecular, Gamma);
        AdjustLight(&newlight.dcvAmbient, Gamma);
        res = (*pSetLight7)(lpd3dd, Index, &newlight);
    } else
        res = (*pSetLight7)(lpd3dd, Index, light);
    _if(res) OutTraceE("SetLight: ret=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

#define D3DSETSTATUS_STATUS     0x00000001L
#define D3DSETSTATUS_EXTENTS        0x00000002L
#define D3DSETSTATUS_ALL    (D3DSETSTATUS_STATUS | D3DSETSTATUS_EXTENTS)

HRESULT WINAPI extSetExecuteData(void *lpeb, LPD3DEXECUTEDATA lped) {
    HRESULT res;
    OutTraceD3D("SetExecuteData: lpeb=%#x lped=%#x\n", lpeb, lped);
    OutDebugD3D("SetExecuteData: size=%d vertexoffset=%d vertexcount=%d instroffset=%d instrlen=%d hvertexoffset=%d "
                "d3dstatus={flags=%#x status=%#x(%s) extent=%#x}\n",
                lped->dwSize, lped->dwVertexOffset, lped->dwVertexCount,
                lped->dwInstructionOffset, lped->dwInstructionLength, lped->dwHVertexOffset,
                lped->dsStatus.dwFlags,
                lped->dsStatus.dwStatus, lped->dsStatus.dwStatus == D3DSETSTATUS_STATUS ? "STATUS" : (lped->dsStatus.dwStatus == D3DSETSTATUS_EXTENTS ? "EXTENTS" : "??"),
                lped->dsStatus.drExtent);
    res = (*pSetExecuteData)(lpeb, lped);
    if(res)
        OutTraceE("SetExecuteData ERROR: ret=%#x(%s)\n", res, ExplainDDError(res));
    //if(dxw.dwFlags2 & WIREFRAME) dxwForceState((LPDIRECT3DEXECUTEBUFFER)lpeb, D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
    return res;
}

HRESULT WINAPI extGetExecuteData(void *lpeb, LPD3DEXECUTEDATA lped) {
    HRESULT res;
    OutTraceD3D("GetExecuteData: lpeb=%#x lped=%#x\n", lpeb, lped);
    res = (*pGetExecuteData)(lpeb, lped);
    if(res) {
        OutTraceE("GetExecuteData ERROR: ret=%#x(%s)\n", res, ExplainDDError(res));
        return res;
    }
    OutDebugD3D("GetExecuteData: size=%d vertexoffset=%d vertexcount=%d instroffset=%d instrlen=%d hvertexoffset=%d "
                "d3dstatus={flags=%#x status=%#x(%s) extent=%#x}\n",
                lped->dwSize, lped->dwVertexOffset, lped->dwVertexCount,
                lped->dwInstructionOffset, lped->dwInstructionLength, lped->dwHVertexOffset,
                lped->dsStatus.dwFlags,
                lped->dsStatus.dwStatus, lped->dsStatus.dwStatus == D3DSETSTATUS_STATUS ? "STATUS" : (lped->dsStatus.dwStatus == D3DSETSTATUS_EXTENTS ? "EXTENTS" : "??"),
                lped->dsStatus.drExtent);
    return res;
}

HRESULT WINAPI extEBLock(void *lpeb, LPD3DEXECUTEBUFFERDESC lpebd) {
    HRESULT res;
    OutTraceD3D("Lock(EB): lpeb=%#x ebdesc={size=%d bufsize=%d caps=%#x(%s) flags=%#x(%s) data=%#x}\n",
                lpeb, lpebd->dwSize, lpebd->dwBufferSize,
                lpebd->dwCaps, sExecuteBufferCaps(lpebd->dwCaps),
                lpebd->dwFlags, sExecuteBufferFlags(lpebd->dwFlags),
                lpebd->lpData);
    res = (*pEBLock)(lpeb, lpebd);
    if(res) {
        OutTraceE("Lock(EB) ERROR: ret=%#x(%s)\n", res, ExplainDDError(res));
        return res;
    }
    OutTraceD3D("Lock(EB): OK lpeb=%#x ebdesc={size=%d bufsize=%d caps=%#x(%s) flags=%#x(%s) data=%#x}\n",
                lpeb, lpebd->dwSize, lpebd->dwBufferSize,
                lpebd->dwCaps, sExecuteBufferCaps(lpebd->dwCaps),
                lpebd->dwFlags, sExecuteBufferFlags(lpebd->dwFlags),
                lpebd->lpData);
    return res;
}

HRESULT WINAPI extEBUnlock(void *lpeb) {
    HRESULT res;
    OutTraceD3D("Unlock(EB): lpeb=%#x\n", lpeb);
    res = (*pEBUnlock)(lpeb);
    if(res)
        OutTraceE("Unlock(EB) ERROR: ret=%#x(%s)\n", res, ExplainDDError(res));
    else
        OutTraceD3D("Unlock(EB) OK\n");
    return res;
}

HRESULT WINAPI extExecute(void *lpd3d, LPDIRECT3DEXECUTEBUFFER lpeb, LPDIRECT3DVIEWPORT vp, DWORD flags) {
    HRESULT ret;
    OutTraceD3D("Direct3DDevice::Execute lpd3d=%#x eb=%#x vp=%#x flags=%#x\n", lpd3d, lpeb, vp, flags);
#ifdef DUMPEXECUTEBUFFER
#ifndef DXW_NOTRACES
    DumpEBData(lpeb);
#endif // DXW_NOTRACES
#endif // DUMPEXECUTEBUFFER
    if(dxw.dwDFlags & PATCHEXECUTEBUFFER) InsertPatchingExecuteBuffer(lpd3d, D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL); // trytry ....
    if(dxw.dwFlags2 & WIREFRAME) InsertPatchingExecuteBuffer(lpd3d, D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME); // trytry ....
    if(dxw.dwDFlags & FORCEZBUFFERON) InsertPatchingExecuteBuffer(lpd3d, D3DRENDERSTATE_ZENABLE, TRUE);
    if(dxw.dwDFlags & FORCEZBUFFEROFF) InsertPatchingExecuteBuffer(lpd3d, D3DRENDERSTATE_ZENABLE, FALSE);
    if(dxw.dwFlags4 & NOTEXTURES) dxwForceState(lpeb, D3DRENDERSTATE_TEXTUREHANDLE, NULL);
    if(dxw.dwDFlags & ZBUFFERALWAYS) dxwForceState(lpeb, D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
    if(dxw.dwDFlags & FORCEZBUFFERON) dxwForceState(lpeb, D3DRENDERSTATE_ZENABLE, TRUE);
    if(dxw.dwDFlags & FORCEZBUFFEROFF) dxwForceState(lpeb, D3DRENDERSTATE_ZENABLE, FALSE);
    //if(dxw.dwFlags2 & WIREFRAME) dxwFakeWireFrame(lpeb);
    if(dxw.dwFlags2 & WIREFRAME) dxwForceState(lpeb, D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
    if(dxw.dwDFlags & PATCHEXECUTEBUFFER) dxwForceState(lpeb, D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
    ret = (*pExecute)(lpd3d, lpeb, vp, flags);
    _if (ret) OutTraceE("Direct3DDevice::Execute res=%#x(%s)\n", ret, ExplainDDError(ret));
    return DD_OK;
}

HRESULT WINAPI extCreateExecuteBuffer(void *lpd3dd, LPD3DEXECUTEBUFFERDESC lpebd, LPDIRECT3DEXECUTEBUFFER *lplpeb, IUnknown *unk) {
    HRESULT res;
    OutTraceD3D("CreateExecuteBuffer(1): d3d=%#x ebdesc={size=%d bufsize=%d caps=%#x(%s) flags=%#x(%s)}\n",
                lpd3dd, lpebd->dwSize, lpebd->dwBufferSize,
                lpebd->dwCaps, sExecuteBufferCaps(lpebd->dwCaps),
                lpebd->dwFlags, sExecuteBufferFlags(lpebd->dwFlags));
    if(dxw.dwFlags10 & FORCEEBCAPS) {
        lpebd->dwFlags |= D3DDEB_CAPS;
        lpebd->dwCaps = ForceExecuteBufferCaps();
    }
    res = (*pCreateExecuteBuffer)(lpd3dd, lpebd, lplpeb, unk);
    if(res)
        OutTraceE("CreateExecuteBuffer ERROR: ret=%#x(%s)\n", res, ExplainDDError(res));
    else {
        OutTraceD3D("CreateExecuteBuffer: lpeb=%#x\n", *lplpeb);
        HookExecuteBuffer(lplpeb);
    }
    return res;
}

HRESULT WINAPI extSetViewport(int dxversion, SetViewport_Type pSetViewport, void *lpvp, LPD3DVIEWPORT vpd) {
    HRESULT res;
    OutTraceD3D("SetViewport(D3D%d): viewport=%#x viewportd=%#x size=%d pos=(%d,%d) dim=(%dx%d) scale=(%fx%f) maxXYZ=(%f,%f,%f) minZ=%f\n",
                dxversion, lpvp, vpd, vpd->dwSize, vpd->dwX, vpd->dwY, vpd->dwWidth, vpd->dwHeight, vpd->dvScaleX, vpd->dvScaleY,
                vpd->dvMaxX, vpd->dvMaxY, vpd->dvMaxZ, vpd->dvMinZ);
    // v2.03.48: scaled dvScaleX/Y fields. Fixes "Dark Vengeance" viewport size when using D3D interface.
    // no.... see Forsaken
    // no good (useless) also for "Spearhead"
    //if(dxw.Windowize){
    //	dxw.MapClient(&vpd->dvScaleX, &vpd->dvScaleY);
    //	OutTraceDW("SetViewport: FIXED scale=(%fx%f)\n", vpd->dvScaleX, vpd->dvScaleY);
    //	int w = (int) vpd->dwWidth;
    //	int h = (int) vpd->dwHeight;
    //	dxw.MapClient(&w, &h);
    //	vpd->dwWidth = (DWORD) w;
    //	vpd->dwHeight = (DWORD) h;
    //	OutTraceDW("SetViewport: FIXED scale=(%dx%d)\n", vpd->dwWidth, vpd->dwHeight);
    //}
    res = (*pSetViewport)(lpvp, vpd);
    if(res) OutTraceE("SetViewport ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("SetViewport: OK\n");
    return res;
}

HRESULT WINAPI extSetViewport1(void *lpvp, LPD3DVIEWPORT vpd) {
    return extSetViewport(1, pSetViewport1, lpvp, vpd);
}
HRESULT WINAPI extSetViewport2(void *lpvp, LPD3DVIEWPORT vpd) {
    return extSetViewport(2, pSetViewport2, lpvp, vpd);
}
HRESULT WINAPI extSetViewport3(void *lpvp, LPD3DVIEWPORT vpd) {
    return extSetViewport(3, pSetViewport3, lpvp, vpd);
}

HRESULT WINAPI extGetViewport(int d3dversion, GetViewport_Type pGetViewport, void *lpvp, LPD3DVIEWPORT vpd) {
    HRESULT res;
    OutTraceD3D("GetViewport(D3D%d): viewport=%#x viewportd=%#x\n", d3dversion, lpvp, vpd);
    res = (*pGetViewport)(lpvp, vpd);
    // v2.03.48: should the dvScaleX/Y fields be unscaled?
    if(res) OutTraceE("GetViewport ERROR: err=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
    else OutTraceD3D("GetViewport: OK size=%d pos=(%d,%d) dim=(%dx%d) scale=(%fx%f) maxXYZ=(%f,%f,%f) minZ=%f\n",
                         vpd->dwSize, vpd->dwX, vpd->dwY, vpd->dwWidth, vpd->dwHeight, vpd->dvScaleX, vpd->dvScaleY,
                         vpd->dvMaxX, vpd->dvMaxY, vpd->dvMaxZ, vpd->dvMinZ);
    return res;
}

HRESULT WINAPI extGetViewport1(void *lpvp, LPD3DVIEWPORT vpd) {
    return extGetViewport(1, pGetViewport1, lpvp, vpd);
}
HRESULT WINAPI extGetViewport2(void *lpvp, LPD3DVIEWPORT vpd) {
    return extGetViewport(2, pGetViewport2, lpvp, vpd);
}
HRESULT WINAPI extGetViewport3(void *lpvp, LPD3DVIEWPORT vpd) {
    return extGetViewport(3, pGetViewport3, lpvp, vpd);
}

#ifdef TRACEALL
HRESULT WINAPI extTransformVertices(int d3dver, TransformVertices_Type pTransformVertices, void *lpvp, DWORD dw, LPD3DTRANSFORMDATA lpd3dtd, DWORD dw2, LPDWORD lpdw) {
    HRESULT res;
    char *api = "TransformVertices";
    OutTrace("%s(VP%d): lpvp=%#x dw=%#x dw2=%#x\n", api, d3dver, lpvp, dw, dw2);
    res = (*pTransformVertices)(lpvp, dw, lpd3dtd, dw2, lpdw);
    if(res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", api, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extTransformVertices1(void *lpvp, DWORD dw, LPD3DTRANSFORMDATA lpd3dtd, DWORD dw2, LPDWORD lpdw) {
    return extTransformVertices(1, pTransformVertices1, lpvp, dw, lpd3dtd, dw2, lpdw);
}
HRESULT WINAPI extTransformVertices2(void *lpvp, DWORD dw, LPD3DTRANSFORMDATA lpd3dtd, DWORD dw2, LPDWORD lpdw) {
    return extTransformVertices(2, pTransformVertices2, lpvp, dw, lpd3dtd, dw2, lpdw);
}
HRESULT WINAPI extTransformVertices3(void *lpvp, DWORD dw, LPD3DTRANSFORMDATA lpd3dtd, DWORD dw2, LPDWORD lpdw) {
    return extTransformVertices(3, pTransformVertices3, lpvp, dw, lpd3dtd, dw2, lpdw);
}

HRESULT WINAPI extLightElements(int d3dver, LightElements_Type pLightElements, void *lpvp, DWORD dwCount, LPD3DLIGHTDATA lpd3dld) {
    HRESULT res;
    char *api = "LightElements";
    OutTrace("%s(VP%d): lpvp=%#x count=%#x\n", api, d3dver, lpvp, dwCount);
    res = (*pLightElements)(lpvp, dwCount, lpd3dld);
    if(res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", api, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extLightElements1(void *lpvp, DWORD dwCount, LPD3DLIGHTDATA lpd3dld) {
    return extLightElements(1, pLightElements1, lpvp, dwCount, lpd3dld);
}
HRESULT WINAPI extLightElements2(void *lpvp, DWORD dwCount, LPD3DLIGHTDATA lpd3dld) {
    return extLightElements(2, pLightElements2, lpvp, dwCount, lpd3dld);
}
HRESULT WINAPI extLightElements3(void *lpvp, DWORD dwCount, LPD3DLIGHTDATA lpd3dld) {
    return extLightElements(3, pLightElements3, lpvp, dwCount, lpd3dld);
}

HRESULT WINAPI extSetBackground(int d3dver, SetBackground_Type pSetBackground, void *lpvp, D3DMATERIALHANDLE d3dmh) {
    HRESULT res;
    char *api = "SetBackground";
    OutTrace("%s(VP%d): lpvp=%#x mhandle=%#x\n", api, d3dver, lpvp, d3dmh);
    res = (*pSetBackground)(lpvp, d3dmh);
    if(res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", api, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extSetBackground1(void *lpvp, D3DMATERIALHANDLE d3dmh) {
    return extSetBackground(1, pSetBackground1, lpvp, d3dmh);
}
HRESULT WINAPI extSetBackground2(void *lpvp, D3DMATERIALHANDLE d3dmh) {
    return extSetBackground(2, pSetBackground2, lpvp, d3dmh);
}
HRESULT WINAPI extSetBackground3(void *lpvp, D3DMATERIALHANDLE d3dmh) {
    return extSetBackground(3, pSetBackground3, lpvp, d3dmh);
}

HRESULT WINAPI extGetBackground(int d3dver, GetBackground_Type pGetBackground, void *lpvp, LPD3DMATERIALHANDLE lpd3dmh, LPBOOL lpb) {
    HRESULT res;
    char *api = "GetBackground";
    OutTrace("%s(VP%d): lpvp=%#x\n", api, d3dver, lpvp);
    res = (*pGetBackground)(lpvp, lpd3dmh, lpb);
    if(res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", api, res, ExplainDDError(res));
        return res;
    }
    OutTrace("%s: mh=%#x bool=%#x\n", api, *lpd3dmh, *lpb);
    return res;
}

HRESULT WINAPI extGetBackground1(void *lpvp, LPD3DMATERIALHANDLE lpd3dmh, LPBOOL lpb) {
    return extGetBackground(1, pGetBackground1, lpvp, lpd3dmh, lpb);
}
HRESULT WINAPI extGetBackground2(void *lpvp, LPD3DMATERIALHANDLE lpd3dmh, LPBOOL lpb) {
    return extGetBackground(2, pGetBackground2, lpvp, lpd3dmh, lpb);
}
HRESULT WINAPI extGetBackground3(void *lpvp, LPD3DMATERIALHANDLE lpd3dmh, LPBOOL lpb) {
    return extGetBackground(3, pGetBackground3, lpvp, lpd3dmh, lpb);
}

HRESULT WINAPI extSetBackgroundDepth(int d3dver, SetBackgroundDepth_Type pSetBackgroundDepth, void *lpvp, LPDIRECTDRAWSURFACE lpdds) {
    HRESULT res;
    char *api = "SetBackgroundDepth";
    OutTrace("%s(VP%d): lpvp=%#x lpdds=%#x\n", api, d3dver, lpvp, lpdds);
    res = (*pSetBackgroundDepth)(lpvp, lpdds);
    if(res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", api, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extSetBackgroundDepth1(void *lpvp, LPDIRECTDRAWSURFACE lpdds) {
    return extSetBackgroundDepth(1, pSetBackgroundDepth1, lpvp, lpdds);
}
HRESULT WINAPI extSetBackgroundDepth2(void *lpvp, LPDIRECTDRAWSURFACE lpdds) {
    return extSetBackgroundDepth(2, pSetBackgroundDepth2, lpvp, lpdds);
}
HRESULT WINAPI extSetBackgroundDepth3(void *lpvp, LPDIRECTDRAWSURFACE lpdds) {
    return extSetBackgroundDepth(3, pSetBackgroundDepth3, lpvp, lpdds);
}

HRESULT WINAPI extGetBackgroundDepth(int d3dver, GetBackgroundDepth_Type pGetBackgroundDepth, void *lpvp, LPDIRECTDRAWSURFACE *lplpdds, LPBOOL lpb) {
    HRESULT res;
    char *api = "GetBackgroundDepth";
    OutTrace("%s(VP%d): lpvp=%#x\n", api, d3dver, lpvp);
    res = (*pGetBackgroundDepth)(lpvp, lplpdds, lpb);
    if(res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", api, res, ExplainDDError(res));
        return res;
    }
    OutTrace("%s: lpdds=%#x bool=%#x\n", api, *lplpdds, *lpb);
    return res;
}

HRESULT WINAPI extGetBackgroundDepth1(void *lpvp, LPDIRECTDRAWSURFACE *lplpdds, LPBOOL lpb) {
    return extGetBackgroundDepth(1, pGetBackgroundDepth1, lpvp, lplpdds, lpb);
}
HRESULT WINAPI extGetBackgroundDepth2(void *lpvp, LPDIRECTDRAWSURFACE *lplpdds, LPBOOL lpb) {
    return extGetBackgroundDepth(2, pGetBackgroundDepth2, lpvp, lplpdds, lpb);
}
HRESULT WINAPI extGetBackgroundDepth3(void *lpvp, LPDIRECTDRAWSURFACE *lplpdds, LPBOOL lpb) {
    return extGetBackgroundDepth(3, pGetBackgroundDepth3, lpvp, lplpdds, lpb);
}

HRESULT WINAPI extClear(int d3dver, Clear_Type pClear, void *lpvp, DWORD dwNRect, LPD3DRECT lpRect, DWORD dwFlags) {
    HRESULT res;
    char *api = "Clear";
#ifndef DXW_NOTRACES
    if(IsTraceD3D) {
        OutTrace("%s(VP%d): lpvp=%#x nrect=%#x flags=%#x\n", api, d3dver, lpvp, dwNRect, dwFlags);
        if(IsDebugD3D) {
            for(DWORD i = 0; i < dwNRect; i++) {
                OutTrace("> rect[%d]=(%d,%d)-(%d,%d)\n",
                         i, lpRect[i].x1, lpRect[i].y1, lpRect[i].x2, lpRect[i].y2);
            }
        }
    }
#endif
    res = (*pClear)(lpvp, dwNRect, lpRect, dwFlags);
    if(res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", api, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extClear1(void *lpvp, DWORD dwNRect, LPD3DRECT lpRect, DWORD dwFlags) {
    return extClear(1, pClear1, lpvp, dwNRect, lpRect, dwFlags);
}
HRESULT WINAPI extClear2(void *lpvp, DWORD dwNRect, LPD3DRECT lpRect, DWORD dwFlags) {
    return extClear(2, pClear2, lpvp, dwNRect, lpRect, dwFlags);
}
HRESULT WINAPI extClear3(void *lpvp, DWORD dwNRect, LPD3DRECT lpRect, DWORD dwFlags) {
    return extClear(3, pClear3, lpvp, dwNRect, lpRect, dwFlags);
}

HRESULT WINAPI extAddLight(int d3dver, AddLight_Type pAddLight, void *lpvp, LPDIRECT3DLIGHT lpd3dl) {
    HRESULT res;
    char *api = "AddLight";
    OutTrace("%s(VP%d): lpvp=%#x light=%#x\n", api, d3dver, lpvp, lpd3dl);
    res = (*pAddLight)(lpvp, lpd3dl);
    if(res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", api, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extAddLight1(void *lpvp, LPDIRECT3DLIGHT lpd3dl) {
    return extAddLight(1, pAddLight1, lpvp, lpd3dl);
}
HRESULT WINAPI extAddLight2(void *lpvp, LPDIRECT3DLIGHT lpd3dl) {
    return extAddLight(2, pAddLight2, lpvp, lpd3dl);
}
HRESULT WINAPI extAddLight3(void *lpvp, LPDIRECT3DLIGHT lpd3dl) {
    return extAddLight(3, pAddLight3, lpvp, lpd3dl);
}

HRESULT WINAPI extDeleteLight(int d3dver, DeleteLight_Type pDeleteLight, void *lpvp, LPDIRECT3DLIGHT lpd3dl) {
    HRESULT res;
    char *api = "DeleteLight";
    OutTrace("%s(VP%d): lpvp=%#x light=%#x\n", api, d3dver, lpvp, lpd3dl);
    res = (*pDeleteLight)(lpvp, lpd3dl);
    if(res)
        OutTraceE("%s: ERROR err=%#x(%s)\n", api, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extDeleteLight1(void *lpvp, LPDIRECT3DLIGHT lpd3dl) {
    return extDeleteLight(1, pDeleteLight1, lpvp, lpd3dl);
}
HRESULT WINAPI extDeleteLight2(void *lpvp, LPDIRECT3DLIGHT lpd3dl) {
    return extDeleteLight(2, pDeleteLight2, lpvp, lpd3dl);
}
HRESULT WINAPI extDeleteLight3(void *lpvp, LPDIRECT3DLIGHT lpd3dl) {
    return extDeleteLight(3, pDeleteLight3, lpvp, lpd3dl);
}

HRESULT WINAPI extNextLight(int d3dver, NextLight_Type pNextLight, void *lpvp, LPDIRECT3DLIGHT lpd3dl, LPDIRECT3DLIGHT *lpd3dnextl, DWORD dw) {
    HRESULT res;
    char *api = "NextLight";
    OutTrace("%s(VP%d): lpvp=%#x light=%#x dx=%#x\n", api, d3dver, lpvp, lpd3dl, dw);
    res = (*pNextLight)(lpvp, lpd3dl, lpd3dnextl, dw);
    if(res) {
        OutTraceE("%s: ERROR err=%#x(%s)\n", api, res, ExplainDDError(res));
        return res;
    }
    OutTrace("%s: nextlight=%#x\n", api, *lpd3dnextl);
    return res;
}

HRESULT WINAPI extNextLight1(void *lpvp, LPDIRECT3DLIGHT lpd3dl, LPDIRECT3DLIGHT *lpd3dnextl, DWORD dw) {
    return extNextLight(1, pNextLight1, lpvp, lpd3dl, lpd3dnextl, dw);
}
HRESULT WINAPI extNextLight2(void *lpvp, LPDIRECT3DLIGHT lpd3dl, LPDIRECT3DLIGHT *lpd3dnextl, DWORD dw) {
    return extNextLight(2, pNextLight2, lpvp, lpd3dl, lpd3dnextl, dw);
}
HRESULT WINAPI extNextLight3(void *lpvp, LPDIRECT3DLIGHT lpd3dl, LPDIRECT3DLIGHT *lpd3dnextl, DWORD dw) {
    return extNextLight(3, pNextLight3, lpvp, lpd3dl, lpd3dnextl, dw);
}
#endif

HRESULT WINAPI extSetViewport7(void *d3dd, LPD3DVIEWPORT7 lpd3dvp) {
    HRESULT res;
    OutTraceD3D("SetViewport(D3D7): d3dd=%#x d3dvp=%#x\n", d3dd, lpd3dvp);
    if(lpd3dvp && IsDebugD3D) {
        OutTraceD3D("> pos(x,y) : %d,%d\n", lpd3dvp->dwX, lpd3dvp->dwY);
        OutTraceD3D("> size(w,h): %d,%d\n", lpd3dvp->dwWidth, lpd3dvp->dwHeight);
        OutTraceD3D("> z(min,max): %f,%f\n", lpd3dvp->dvMinZ, lpd3dvp->dvMaxZ);
    }
    res = (*pSetViewport7)(d3dd, lpd3dvp);
    _if(res) OutTraceE("SetViewport: ERROR res=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extGetViewport7(void *d3dd, LPD3DVIEWPORT7 lpd3dvp) {
    HRESULT res;
    OutTraceD3D("GetViewport(D3D7): d3dd=%#x d3dvp=%#x\n", d3dd, lpd3dvp);
    res = (*pGetViewport7)(d3dd, lpd3dvp);
    _if(res) OutTraceE("GetViewport: ERROR res=%#x(%s)\n", res, ExplainDDError(res));
    if(!res && lpd3dvp && IsDebugD3D) {
        OutTraceD3D("> pos(x,y) : %d,%d\n", lpd3dvp->dwX, lpd3dvp->dwY);
        OutTraceD3D("> size(w,h): %d,%d\n", lpd3dvp->dwWidth, lpd3dvp->dwHeight);
        OutTraceD3D("> z(min,max): %f,%f\n", lpd3dvp->dvMinZ, lpd3dvp->dvMaxZ);
    }
    return res;
}

// ====== tracing proxies ==========

#ifdef TRACEALL

static HRESULT WINAPI SetTransform(int d3dversion, SetTransform_Type pSetTransform, void *lpd3dd, D3DTRANSFORMSTATETYPE tstype, LPD3DMATRIX matrix) {
    HRESULT res;
    OutTraceD3D("SetTransform(D3D%d): lpd3dd=%#x tstype=%#x(%s) matrix={\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n}\n",
                d3dversion, lpd3dd, tstype, sTransformType(tstype),
                matrix->_11, matrix->_12, matrix->_13, matrix->_14,
                matrix->_21, matrix->_22, matrix->_23, matrix->_24,
                matrix->_31, matrix->_32, matrix->_33, matrix->_34,
                matrix->_41, matrix->_42, matrix->_43, matrix->_44);
    res = (*pSetTransform)(lpd3dd, tstype, matrix);
    _if(res) OutTraceE("SetTransform ERROR: ret=%#x\n", res);
    return res;
}

HRESULT WINAPI extSetTransform2(void *lpd3dd, D3DTRANSFORMSTATETYPE tstype, LPD3DMATRIX matrix) {
    return SetTransform(2, pSetTransform2, lpd3dd, tstype, matrix);
}
HRESULT WINAPI extSetTransform3(void *lpd3dd, D3DTRANSFORMSTATETYPE tstype, LPD3DMATRIX matrix) {
    return SetTransform(3, pSetTransform3, lpd3dd, tstype, matrix);
}
HRESULT WINAPI extSetTransform7(void *lpd3dd, D3DTRANSFORMSTATETYPE tstype, LPD3DMATRIX matrix) {
    return SetTransform(7, pSetTransform7, lpd3dd, tstype, matrix);
}

HRESULT WINAPI extGetLight7(void *lpd3dd, DWORD Index, D3DLIGHT7 *light) {
    HRESULT res;
    OutTraceD3D("GetLight(7): d3d=%#x index=%d\n", lpd3dd, Index);
    res = (*pGetLight7)(lpd3dd, Index, light);
    _if(res) OutTraceE("GetLight: ret=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extSetRenderTarget7(void *lpd3dd, LPDIRECTDRAWSURFACE7 lpdds, DWORD dwFlags) {
    HRESULT res;
    OutTraceD3D("SetRenderTarget(7): d3d=%#x lpdds=%#x flags=%#x\n", lpd3dd, lpdds, dwFlags);
    res = (*pSetRenderTarget7)(lpd3dd, lpdds, dwFlags);
    _if(res) OutTraceE("SetRenderTarget: ret=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extGetRenderTarget7(void *lpd3dd, LPDIRECTDRAWSURFACE7 *lplpdds) {
    HRESULT res;
    OutTraceD3D("GetRenderTarget(7): d3d=%#x\n", lpd3dd);
    res = (*pGetRenderTarget7)(lpd3dd, lplpdds);
    if(res)
        OutTraceE("GetRenderTarget: ret=%#x(%s)\n", res, ExplainDDError(res));
    else
        OutTraceD3D("GetRenderTarget: lpdds=%#x\n", *lplpdds);
    return res;
}

HRESULT WINAPI extEBInitialize(void *lpeb, LPDIRECT3DDEVICE lpd3dd, LPD3DEXECUTEBUFFERDESC lpebd) {
    HRESULT res;
    OutTraceD3D("Initialize(EB): lpeb=%#x lpd3dd=%#x lpebd=%#x\n", lpeb, lpd3dd, lpebd);
    res = (*pEBInitialize)(lpeb, lpd3dd, lpebd);
    if(res)
        OutTraceE("Initialize(EB) ERROR: ret=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extOptimize(void *lpeb, DWORD dwFlags) {
    HRESULT res;
    OutTraceD3D("Optimize: lpeb=%#x flags=%#x\n", lpeb, dwFlags);
    res = (*pOptimize)(lpeb, dwFlags);
    if(res)
        OutTraceE("Optimize ERROR: ret=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extValidate(void *lpeb, LPDWORD offset, LPD3DVALIDATECALLBACK cb, LPVOID ctx, DWORD reserved) {
    HRESULT res;
    OutTraceD3D("Validate: lpeb=%#x offset=%#x cb=%#x ctx=%#x resvd=%#x\n", lpeb, offset, cb, ctx, reserved);
    res = (*pValidate)(lpeb, offset, cb, ctx, reserved);
    if(res)
        OutTraceE("Validate ERROR: ret=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

char *ExplainPrimType(DWORD c) {
    char *p;
    switch(c) {
    case D3DPT_POINTLIST:
        p = "POINTLIST";
        break;
    case D3DPT_LINELIST:
        p = "LINELIST";
        break;
    case D3DPT_LINESTRIP:
        p = "LINESTRIP";
        break;
    case D3DPT_TRIANGLELIST:
        p = "TRIANGLELIST";
        break;
    case D3DPT_TRIANGLESTRIP:
        p = "TRIANGLESTRIP";
        break;
    case D3DPT_TRIANGLEFAN:
        p = "TRIANGLEFAN";
        break;
    default:
        p = "???";
        break;
    }
    return p;
}

char *ExplainVType(DWORD c) {
    char *p;
    switch(c) {
    case D3DVT_VERTEX:
        p = "VERTEX";
        break;
    case D3DVT_LVERTEX:
        p = "LVERTEX";
        break;
    case D3DVT_TLVERTEX:
        p = "TLVERTEX";
        break;
    default:
        p = "???";
        break;
    }
    return p;
}

char *ExplainDrawPrimFlags(DWORD c) {
#ifndef D3DDP_OUTOFORDER
#define D3DDP_OUTOFORDER 0x00000002
#endif
    static char eb[256];
    unsigned int l;
    strcpy(eb, "D3DDP_");
    if (c & D3DDP_WAIT) strcat(eb, "WAIT+");
    if (c & D3DDP_OUTOFORDER) strcat(eb, "OUTOFORDER+");
    if (c & D3DDP_DONOTCLIP) strcat(eb, "DONOTCLIP+");
    if (c & D3DDP_DONOTUPDATEEXTENTS) strcat(eb, "DONOTUPDATEEXTENTS+");
    if (c & D3DDP_DONOTLIGHT) strcat(eb, "DONOTLIGHT+");
    l = strlen(eb);
    if (l > strlen("D3DDP_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

HRESULT WINAPI extDrawPrimitive(int d3dversion, DrawPrimitive37_Type pDrawPrimitive, void *lpd3d, D3DPRIMITIVETYPE ptype, DWORD vtype, LPVOID pVerts, DWORD nVerts, DWORD dwFlags) {
    HRESULT res;
    OutTraceD3D("DrawPrimitive(%d): lpd3d=%#x ptype=%#x(%s) vtype=%#x(%s) verts=%#x nverts=%d flags=%#x(%s)\n",
                d3dversion, lpd3d, ptype, ExplainPrimType(ptype), vtype, ExplainVType(vtype), pVerts, nVerts, dwFlags, ExplainDrawPrimFlags(dwFlags));
#ifdef DUMPEXECUTEBUFFER
    LPD3DHVERTEX pv;
    LPD3DLVERTEX plv;
    LPD3DTLVERTEX ptlv;
    switch(vtype) {
    case D3DVT_VERTEX:
        pv = (LPD3DHVERTEX)pVerts;
        for(DWORD i = 0; i < nVerts; i++, pv++)
            OutTrace("> v[%d] p=(%f,%f,%f} flags=%#x\n",
                     i, pv->hx, pv->hy, pv->hz, pv->dwFlags
                    );
        break;
    case D3DVT_LVERTEX:
        plv = (LPD3DLVERTEX)pVerts;
        for(DWORD i = 0; i < nVerts; i++, plv++)
            OutTrace("> v[%d] p=(%f,%f,%f} tex=(%f,%f) color=%#x specular=%#x\n",
                     i, plv->x, plv->y, plv->z, plv->tu, plv->tv, plv->color, plv->specular
                    );
        break;
    case D3DVT_TLVERTEX:
        ptlv = (LPD3DTLVERTEX)pVerts;
        for(DWORD i = 0; i < nVerts; i++, ptlv++)
            OutTrace("> v[%d] p=(%f,%f,%f} tex=(%f,%f) color=%#x specular=%#x\n",
                     i, ptlv->sx, ptlv->sy, ptlv->sz, ptlv->tu, ptlv->tv, ptlv->color, ptlv->specular
                    );
        break;
    }
#endif
    res = (*pDrawPrimitive)(lpd3d, ptype, vtype, pVerts, nVerts, dwFlags);
    if(res)
        OutTraceE("DrawPrimitive ERROR: ret=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extDrawPrimitive2(void *lpd3d, D3DPRIMITIVETYPE ptype, D3DVERTEXTYPE vtype, LPVOID pVerts, DWORD nVerts, DWORD dwFlags) {
    return extDrawPrimitive(2, (DrawPrimitive37_Type)pDrawPrimitive2, lpd3d, ptype, (DWORD)vtype, pVerts, nVerts, dwFlags);
}
HRESULT WINAPI extDrawPrimitive3(void *lpd3d, D3DPRIMITIVETYPE ptype, DWORD vtype, LPVOID pVerts, DWORD nVerts, DWORD dwFlags) {
    return extDrawPrimitive(3, (DrawPrimitive37_Type)pDrawPrimitive3, lpd3d, ptype, vtype, pVerts, nVerts, dwFlags);
}
HRESULT WINAPI extDrawPrimitive7(void *lpd3d, D3DPRIMITIVETYPE ptype, DWORD vtype, LPVOID pVerts, DWORD nVerts, DWORD dwFlags) {
    return extDrawPrimitive(7, (DrawPrimitive37_Type)pDrawPrimitive7, lpd3d, ptype, vtype, pVerts, nVerts, dwFlags);
}

#endif // TRACEALL

#ifdef TRACEMATERIAL
HRESULT WINAPI extPick(void *lpd3dd, LPDIRECT3DEXECUTEBUFFER lpeb, LPDIRECT3DVIEWPORT lpvp, DWORD dw, LPD3DRECT lprect) {
    HRESULT res;
    OutTraceD3D("Pick(1): d3d=%#x eb=%#x viewport=%#x dw=%#x rect=%#x\n", lpd3dd, lpeb, lpvp, dw, lprect);
    res = (*pPick)(lpd3dd, lpeb, lpvp, dw, lprect);
    if(res)
        OutTraceE("Pick ERROR: ret=%#x(%s)\n", res, ExplainDDError(res));
    return res;
}
#endif

#ifdef TRACEALL
static HRESULT WINAPI extGetTextureStageState(char *api, GetTextureStageState_Type pGetTextureStageState, void *lpd3dd, DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwState, LPDWORD lpdwValue) {
    HRESULT res;
    OutTraceD3D("%s: lpd3dd=%#x stage=%d state=%#x\n", api, lpd3dd, dwStage, dwState);
    res = (*pGetTextureStageState)(lpd3dd, dwStage, dwState, lpdwValue);
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", api, res, ExplainDDError(res));
    else
        OutTraceD3D("%s: value=%#x\n", api, *lpdwValue);
    return res;
}

static HRESULT WINAPI extSetTextureStageState(char *api, SetTextureStageState_Type pSetTextureStageState, void *lpd3dd, DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwState, DWORD dwValue) {
    HRESULT res;
    OutTraceD3D("%s: lpd3dd=%#x stage=%d state=%#x value=%#x\n", api, lpd3dd, dwStage, dwState, dwValue);
    res = (*pSetTextureStageState)(lpd3dd, dwStage, dwState, dwValue);
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", api, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extGetTextureStageState3(void *lpd3dd, DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwState, LPDWORD lpdwValue) {
    return extGetTextureStageState("GetTextureStageState3", pGetTextureStageState3, lpd3dd, dwStage, dwState, lpdwValue);
}
HRESULT WINAPI extGetTextureStageState7(void *lpd3dd, DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwState, LPDWORD lpdwValue) {
    return extGetTextureStageState("GetTextureStageState7", pGetTextureStageState7, lpd3dd, dwStage, dwState, lpdwValue);
}
HRESULT WINAPI extSetTextureStageState3(void *lpd3dd, DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwState, DWORD dwValue) {
    return extSetTextureStageState("SetTextureStageState3", pSetTextureStageState3, lpd3dd, dwStage, dwState, dwValue);
}
HRESULT WINAPI extSetTextureStageState7(void *lpd3dd, DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwState, DWORD dwValue) {
    return extSetTextureStageState("SetTextureStageState7", pSetTextureStageState7, lpd3dd, dwStage, dwState, dwValue);
}
#endif
