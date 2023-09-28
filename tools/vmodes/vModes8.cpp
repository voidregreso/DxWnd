
#define  _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "d3d8.h"

static char *sRIID(REFIID riid) {
    static char sRIIDBuffer[81];
    OLECHAR *guidString;
    StringFromCLSID(riid, &guidString);
    sprintf(sRIIDBuffer, "%ls",  guidString);
    ::CoTaskMemFree(guidString);
    return sRIIDBuffer;
}

int main(int argc, char *argv[]) {
    HRESULT res;
    printf("------------ vModes8 v1.00.00\n");
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
    printf("------------ Direct3D8\n");
    IDirect3D8 *lpD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if(!lpD3D) {
        printf("Direct3DCreate8 ERROR err=%d\n", GetLastError());
        return -1;
    }
    printf("lpD3D8=%x\n", lpD3D);
    UINT iAdapterCount = lpD3D->GetAdapterCount();
    for(int adapter = 0; adapter < iAdapterCount; adapter++) {
        D3DADAPTER_IDENTIFIER8 AdapterId;
        memset(&AdapterId, 0, sizeof(D3DADAPTER_IDENTIFIER8)); // just in case ...
        if(res = lpD3D->GetAdapterIdentifier(adapter, 0, &AdapterId))
            printf("GetAdapterIdentifier[%u] failed err=%x\n", adapter, res);
        else {
            printf("Adapter[%d]%s:\n", adapter, adapter ? "" : " D3DADAPTER_DEFAULT");
            printf("\tDriver : %s\n", AdapterId.Driver);
            printf("\tDescr. : %s\n", AdapterId.Description);
            printf("\tVers.  : %#x.%#x\n", AdapterId.DriverVersion.HighPart, AdapterId.DriverVersion.LowPart);
            printf("\tVendor : %u\n", AdapterId.VendorId);
            printf("\tSubSys.: %u\n", AdapterId.SubSysId);
            printf("\tRevis. : %u\n", AdapterId.Revision);
            printf("\tDevId  : %s\n", sRIID(AdapterId.DeviceIdentifier));
            printf("\tWHQLlev: %u\n", AdapterId.WHQLLevel);
        }
        HMONITOR hMonitor = lpD3D->GetAdapterMonitor(adapter);
        printf("\tMonitor: %u\n", hMonitor);
        UINT modeCount = lpD3D->GetAdapterModeCount(adapter);
        printf("count=%d\n", modeCount);
        for (int i = 0; i < modeCount; i++) {
            D3DDISPLAYMODE mode;
            //memset(&mode, 0, sizeof(mode));
            if ((res = lpD3D->EnumAdapterModes(0, i, &mode)) != D3D_OK) {
                printf("EnumAdapterModes[%u] failed err=%x\n", i, res);
                break;
            }
            printf("EnumAdapterModes[%u]: %ux%u rr=%u fmt=%u\n",
                   i, mode.Width, mode.Height, mode.RefreshRate, mode.Format);
        }
    }
    lpD3D->Release();
    printf("------------ end\n");
}

