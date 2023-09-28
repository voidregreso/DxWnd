
#define  _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "d3d9.h"

static char *sRIID(REFIID riid) {
    static char sRIIDBuffer[81];
    OLECHAR *guidString;
    StringFromCLSID(riid, &guidString);
    sprintf(sRIIDBuffer, "%ls",  guidString);
    ::CoTaskMemFree(guidString);
    return sRIIDBuffer;
}

static char *ExplainD3DDeviceType(D3DDEVTYPE DeviceType) {
    char *s;
    switch(DeviceType) {
    case D3DDEVTYPE_HAL:
        s = (char *)"HAL";
        break;
    case D3DDEVTYPE_NULLREF:
        s = (char *)"NULLREF";
        break;
    case D3DDEVTYPE_REF:
        s = (char *)"REF";
        break;
    case D3DDEVTYPE_SW:
        s = (char *)"SW";
        break;
    default:
        s = (char *)"unknown";
        break;
    }
    return s;
}

static void DumpD3D9Caps(D3DCAPS9 *pCaps) {
    printf("\t"
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
    printf(
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
    printf(
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
    printf(
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

int main(int argc, char *argv[]) {
    HRESULT res;
    printf("------------ vModes9 v1.00.00\n");
    printf("------------ DisplayDevice\n");
    for(int iDisplayDevice = 0; ; iDisplayDevice++) {
        DISPLAY_DEVICEA DisplayDevice;
        memset(&DisplayDevice, 0, sizeof(DISPLAY_DEVICEA)); // just in case ...
        DisplayDevice.cb = sizeof(DISPLAY_DEVICEA);
        if(!EnumDisplayDevices(NULL, iDisplayDevice, &DisplayDevice, 0)) break;
        printf("DisplayDevice[%i]:\n", iDisplayDevice);
        printf("\tDevName: %.32s\n", DisplayDevice.DeviceName);
        printf("\tDevStr.: %.32s\n", DisplayDevice.DeviceString);
        printf("\tFlags  : %#x\n", DisplayDevice.StateFlags);
        printf("\tDevId  : %.128s\n", DisplayDevice.DeviceID);
        printf("\tDevKey : %.128s\n", DisplayDevice.DeviceKey);
    }
    printf("------------ Direct3D9\n");
    IDirect3D9Ex *lpD3D;
    res = Direct3DCreate9Ex(D3D_SDK_VERSION, &lpD3D);
    if(res) {
        printf("Direct3DCreate9 failed err=%x\n", res);
        return -1;
    }
    printf("lpD3D=%x\n", lpD3D);
    UINT iAdapterCount = lpD3D->GetAdapterCount();
    for(int adapter = 0; adapter < iAdapterCount; adapter++) {
        D3DADAPTER_IDENTIFIER9 AdapterId;
        HRESULT bHAL, bSW, bREF;
        D3DCAPS9 D3D9Caps;
        memset(&AdapterId, 0, sizeof(D3DADAPTER_IDENTIFIER9)); // just in case ...
        if(res = lpD3D->GetAdapterIdentifier(adapter, 0, &AdapterId))
            printf("GetAdapterIdentifier[%u] failed err=%x\n", adapter, res);
        else {
            printf("Adapter[%d]%s:\n", adapter, adapter ? "" : " D3DADAPTER_DEFAULT");
            printf("\tDriver : %s\n", AdapterId.Driver);
            printf("\tDescr. : %s\n", AdapterId.Description);
            printf("\tName   : %s\n", AdapterId.DeviceName);
            printf("\tVers.  : %#x.%#x\n", AdapterId.DriverVersion.HighPart, AdapterId.DriverVersion.LowPart);
            printf("\tVendor : %u\n", AdapterId.VendorId);
            printf("\tSubSys.: %u\n", AdapterId.SubSysId);
            printf("\tRevis. : %u\n", AdapterId.Revision);
            printf("\tDevId  : %s\n", sRIID(AdapterId.DeviceIdentifier));
            printf("\tWHQLlev: %u\n", AdapterId.WHQLLevel);
        }
        HMONITOR hMonitor = lpD3D->GetAdapterMonitor(adapter);
        printf("\tMonitor: %u\n", hMonitor);
        if(res = lpD3D->GetDeviceCaps(adapter, D3DDEVTYPE_HAL, &D3D9Caps)) {
            if(res == D3DERR_NOTAVAILABLE)
                printf("D3DDEVTYPE_HAL Caps: UNAVAILABLE\n");
            else
                printf("GetDeviceCaps[%u, D3DDEVTYPE_HAL] failed res=%x\n", adapter, res);
        } else {
            printf("D3DDEVTYPE_HAL Caps:\n");
            DumpD3D9Caps(&D3D9Caps);
        }
        if(res = lpD3D->GetDeviceCaps(adapter, D3DDEVTYPE_SW, &D3D9Caps)) {
            if(res == D3DERR_NOTAVAILABLE)
                printf("D3DDEVTYPE_SW Caps: UNAVAILABLE\n");
            else
                printf("GetDeviceCaps[%u, D3DDEVTYPE_SW] failed res=%x\n", adapter, res);
        } else {
            printf("D3DDEVTYPE_SW Caps:\n");
            DumpD3D9Caps(&D3D9Caps);
        }
        D3DFORMAT format = D3DFMT_X8R8G8B8;
        UINT modeCount = lpD3D->GetAdapterModeCount(adapter, format);
        printf("D3DFMT_X8R8G8B8 count=%d\n", modeCount);
        for (int i = 0; i < modeCount; i++) {
            D3DDISPLAYMODE mode;
            //memset(&mode, 0, sizeof(mode));
            if ((res = lpD3D->EnumAdapterModes(adapter, format, i, &mode)) != D3D_OK) {
                printf("EnumAdapterModes[%u] failed res=%x\n", i, res);
                break;
            }
            bHAL = lpD3D->CheckDeviceType(adapter, D3DDEVTYPE_HAL, format, format, TRUE);
            bSW  = lpD3D->CheckDeviceType(adapter, D3DDEVTYPE_SW, format, format, TRUE);
            bREF = lpD3D->CheckDeviceType(adapter, D3DDEVTYPE_REF, format, format, TRUE);
            printf("EnumAdapterModes[%u]: %ux%u rr=%u fmt=%u HAL=%s SW=%s REF=%s\n",
                   i, mode.Width, mode.Height, mode.RefreshRate, mode.Format,
                   bHAL ? "No" : "Yes",
                   bSW  ? "No" : "Yes",
                   bREF ? "No" : "Yes"
                  );
        }
        format = D3DFMT_A8R8G8B8;
        modeCount = lpD3D->GetAdapterModeCount(adapter, format);
        printf("D3DFMT_A8R8G8B8 count=%d\n", modeCount);
        for (int i = 0; i < modeCount; i++) {
            D3DDISPLAYMODE mode;
            //memset(&mode, 0, sizeof(mode));
            if ((res = lpD3D->EnumAdapterModes(adapter, format, i, &mode)) != D3D_OK) {
                printf("EnumAdapterModes[%u] failed res=%x\n", i, res);
                break;
            }
            bHAL = lpD3D->CheckDeviceType(adapter, D3DDEVTYPE_HAL, format, format, TRUE);
            bSW  = lpD3D->CheckDeviceType(adapter, D3DDEVTYPE_SW, format, format, TRUE);
            bREF = lpD3D->CheckDeviceType(adapter, D3DDEVTYPE_REF, format, format, TRUE);
            printf("EnumAdapterModes[%u]: %ux%u rr=%u fmt=%u HAL=%s SW=%s REF=%s\n",
                   i, mode.Width, mode.Height, mode.RefreshRate, mode.Format,
                   bHAL ? "No" : "Yes",
                   bSW  ? "No" : "Yes",
                   bREF ? "No" : "Yes"
                  );
        }
        format = D3DFMT_R5G6B5;
        modeCount = lpD3D->GetAdapterModeCount(adapter, format);
        printf("D3DFMT_R5G6B5 count=%d\n", modeCount);
        for (int i = 0; i < modeCount; i++) {
            D3DDISPLAYMODE mode;
            //memset(&mode, 0, sizeof(mode));
            if ((res = lpD3D->EnumAdapterModes(adapter, format, i, &mode)) != D3D_OK) {
                printf("EnumAdapterModes[%u] failed res=%x\n", i, res);
                break;
            }
            bHAL = lpD3D->CheckDeviceType(adapter, D3DDEVTYPE_HAL, format, format, TRUE);
            bSW  = lpD3D->CheckDeviceType(adapter, D3DDEVTYPE_SW, format, format, TRUE);
            bREF = lpD3D->CheckDeviceType(adapter, D3DDEVTYPE_REF, format, format, TRUE);
            printf("EnumAdapterModes[%u]: %ux%u rr=%u fmt=%u HAL=%s SW=%s REF=%s\n",
                   i, mode.Width, mode.Height, mode.RefreshRate, mode.Format,
                   bHAL ? "No" : "Yes",
                   bSW  ? "No" : "Yes",
                   bREF ? "No" : "Yes"
                  );
        }
    }
    lpD3D->Release();
    printf("------------ end\n");
}

