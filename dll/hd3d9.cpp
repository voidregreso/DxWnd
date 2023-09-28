#include <D3D9.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhelper.h"

typedef HRESULT (WINAPI *LockRect_Type)(void *, UINT, D3DLOCKED_RECT *, CONST RECT *, DWORD);
typedef HRESULT (WINAPI *UnlockRect_Type)(void *, UINT);
typedef HRESULT (WINAPI *GetAdapterDisplayMode_Type)(void *, UINT, D3DDISPLAYMODE *);
typedef HRESULT (WINAPI *GetDirect3D9_Type)(void *, void **);
typedef HRESULT (WINAPI *GetFrontBufferData_Type)(void *, UINT, LPDIRECTDRAWSURFACE);

extern LockRect_Type pLockRect9;
extern UnlockRect_Type pUnlockRect9;
extern GetAdapterDisplayMode_Type pGetAdapterDisplayMode9;
extern GetDirect3D9_Type pGetDirect3D9;
extern GetFrontBufferData_Type pGetFrontBufferData9;

extern void D3DTextureDump(D3DSURFACE_DESC, D3DLOCKED_RECT);
extern void D3DTextureHighlight(D3DSURFACE_DESC, D3DLOCKED_RECT);
extern void D3DTextureHack(D3DSURFACE_DESC, D3DLOCKED_RECT);
extern void D3DTextureTransp(D3DSURFACE_DESC, D3DLOCKED_RECT);
extern char *ExplainD3DSurfaceFormat(DWORD);

static char *sTexType(D3DRESOURCETYPE type) {
    char *s;
    switch(type) {
    case D3DRTYPE_SURFACE:
        s = "SURFACE";
        break;
    case D3DRTYPE_VOLUME:
        s = "VOLUME";
        break;
    case D3DRTYPE_TEXTURE:
        s = "TEXTURE";
        break;
    case D3DRTYPE_VOLUMETEXTURE:
        s = "VOLUMETEXTURE";
        break;
    case D3DRTYPE_CUBETEXTURE:
        s = "CUBETEXTURE";
        break;
    case D3DRTYPE_VERTEXBUFFER:
        s = "VERTEXBUFFER";
        break;
    case D3DRTYPE_INDEXBUFFER:
        s = "INDEXBUFFER";
        break;
    default:
        s = "unknown";
        break;
    }
    return s;
}

void D3D9TextureHandling(void *arg, int Level) {
    HRESULT res;
    LPDIRECT3DBASETEXTURE9 lpd3dbase = (LPDIRECT3DBASETEXTURE9)arg;
    LPDIRECT3DTEXTURE9 lpd3dtex = (LPDIRECT3DTEXTURE9)arg;
    IDirect3DSurface9 *pSurfaceLevel;
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT LockedRect;
    D3DRESOURCETYPE TexType = lpd3dbase->GetType();
    OutDebugD3D("D3D9TextureHandling: arg=%#x level=%d type=%d(%s)\n", (DWORD)arg, Level, TexType, sTexType(TexType));
    // v2.04.26: skip attempts to dump objects not D3DRTYPE_TEXTURE type
    if(TexType != D3DRTYPE_TEXTURE) return;
    // Beware: attempts to dump surfaces at level > 0 result in stack corruption!!!
    // v2.04.26: commented out, the stack corruption is protected by statement above
    // if(Level > 0) return;
    if(res = lpd3dtex->GetSurfaceLevel(Level, &pSurfaceLevel)) {
        OutTraceE("Texture::GetSurfaceLevel ERROR: res=%#x(%s)\n", res, ExplainDDError(res));
        return;
    }
    pSurfaceLevel->Release();
    if(res = lpd3dtex->GetLevelDesc(Level, &Desc)) {
        OutTraceE("Texture::GetLevelDesc ERROR: res=%#x(%s)\n", res, ExplainDDError(res));
        return;
    }
    OutDebugD3D("D3D9TextureHandling: level=%d type=%#x usage=%#x\n", Level, Desc.Type, Desc.Usage);
    switch(Desc.Type) {
    case D3DRTYPE_SURFACE:
    case D3DRTYPE_TEXTURE:
        break;
    default:
        OutDebugD3D("D3D9TextureHandling: SKIP type=%#x usage=%#x\n", Desc.Type);
        return;
        break;
    }
    if(Desc.Usage == D3DUSAGE_RENDERTARGET) {
        OutDebugD3D("D3D9TextureHandling: SKIP usage=%#x\n", Desc.Usage);
        return;
    }
    //pSurfaceLevel->GetRenderTargetData(&pRenderTarget, &pDestSurface);
    res = (*pLockRect9)(lpd3dtex, Level, &LockedRect, NULL, D3DLOCK_READONLY);
    OutTraceDW("D3D9TextureHandling: lpd3dtex=%#x level=%d format=%#x(%s) size=(%dx%d) bits=%#x pitch=%d\n",
               lpd3dtex, Level, Desc.Format, ExplainD3DSurfaceFormat(Desc.Format),
               Desc.Width, Desc.Height, LockedRect.pBits, LockedRect.Pitch);
    switch(dxw.dwFlags5 & TEXTUREMASK) {
    case TEXTUREHIGHLIGHT:
        D3DTextureHighlight(Desc, LockedRect);
        break;
    case TEXTUREDUMP:
        D3DTextureDump(Desc, LockedRect);
        break;
    case TEXTUREHACK:
        D3DTextureHack(Desc, LockedRect);
        break;
    case TEXTURETRANSP:
        D3DTextureTransp(Desc, LockedRect);
        break;
    }
    res = (*pUnlockRect9)(lpd3dtex, Level);
}

HRESULT dxGetFrontBuffer9(void *lpd3dd, UINT iSwapChain, LPDIRECTDRAWSURFACE xdest) {
    HRESULT res;
    D3DDEVICE_CREATION_PARAMETERS dcp;
    LPDIRECT3DSURFACE9 lpSurface = NULL;
    LPDIRECT3DSURFACE9 lpStretchedSurface = NULL;
    IDirect3DDevice9 *lpDevice = (IDirect3DDevice9 *)lpd3dd;
    IDirect3DSurface9 *dest = (IDirect3DSurface9 *)xdest;
    dcp.AdapterOrdinal = D3DADAPTER_DEFAULT;
    lpDevice->GetCreationParameters(&dcp);
    D3DDISPLAYMODE dm;
    dm.Width = dm.Height = 0;
    // retrieve pointer to IDirect3D9 interface,
    // which provides the GetAdapterDisplayMode method
    LPDIRECT3D9 lpD3D = NULL;
    //res = lpDevice->GetDirect3D(&lpD3D);
    res = (*pGetDirect3D9)(lpDevice, (void **)&lpD3D);
    if(res) {
        OutTraceE("GetFrontBuffer: GetDirect3D ERROR res=%#x at %d\n", res, __LINE__);
        return res;
    }
    // query the screen dimensions of the current adapter
    //res = lpD3D->GetAdapterDisplayMode(dcp.AdapterOrdinal, &dm);
    res = (*pGetAdapterDisplayMode9)(lpD3D, dcp.AdapterOrdinal, &dm);
    lpD3D->Release();
    if(res) {
        OutTraceE("GetFrontBuffer: GetAdapterDisplayMode ERROR res=%#x at %d\n", res, __LINE__);
        return res;
    } else
        OutDebugD3D("GetFrontBuffer: screen size=(%dx%d)\n", dm.Width, dm.Height);
    //res = lpDevice->CreateOffscreenPlainSurface(dm.Width, dm.Height, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &lpSurface, NULL);
    res = lpDevice->CreateOffscreenPlainSurface(dm.Width, dm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &lpSurface, NULL);
    if(res) {
        OutTraceE("GetFrontBuffer: CreateOffscreenPlainSurface ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        return res;
    }
    RECT dstrect = dxw.GetScreenRect();
    res = lpDevice->CreateOffscreenPlainSurface(dstrect.right, dstrect.bottom, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &lpStretchedSurface, NULL);
    //res = lpDevice->CreateOffscreenPlainSurface(dstrect.right, dstrect.bottom, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &lpStretchedSurface, NULL);
    if(res) {
        lpSurface->Release();
        OutTraceE("GetFrontBuffer: CreateOffscreenPlainSurface ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        return res;
    }
    //res = lpDevice->GetFrontBuffer(lpSurface);
    res = (*pGetFrontBufferData9)(lpDevice, iSwapChain, (LPDIRECTDRAWSURFACE)lpSurface);
    if(res) {
        OutTraceE("GetFrontBuffer: GetFrontBufferData ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        lpStretchedSurface->Release();
        lpSurface->Release();
        return res;
    }
    RECT p0 = dxw.GetUnmappedScreenRect();
    RECT srcrect = dxw.GetScreenRect();
    OffsetRect(&srcrect, p0.left, p0.top);
    OutDebugD3D("GetFrontBuffer: screen rect=(%d,%d)-(%d,%d)\n", srcrect.left, srcrect.top, srcrect.right, srcrect.bottom);
    OutDebugD3D("GetFrontBuffer: dest   rect=(%d,%d)-(%d,%d)\n", dstrect.left, dstrect.top, dstrect.right, dstrect.bottom);
    res = lpDevice->UpdateSurface(lpSurface, &srcrect, lpStretchedSurface, NULL);
    if(res) {
        OutTraceE("GetFrontBuffer: UpdateSurface ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        lpStretchedSurface->Release();
        lpSurface->Release();
        return res;
    }
    // now copy to dest target
    lpSurface->Release();
    lpStretchedSurface->Release();
    if(res) {
        OutTraceE("GetFrontBuffer: StretchRect ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        return res;
    }
    return DD_OK;
}
