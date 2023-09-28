#define DIRECTINPUT_VERSION 0x800
#define DIRECTINPUT8_VERSION 0x800
#define  _CRT_SECURE_NO_WARNINGS

#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhelper.h"
#include "dxwjoy.h"
#define GUID_ALLOCATE
#include "comvjoy.h"
#include "stdio.h"

// uncomment to stop execution on invocation of unimplemented methods
#define TRAPUNIMPLEMENTED
#ifdef TRAPUNIMPLEMENTED
#define utrap(m) MessageBox(0, "unimplemented method" m " !", "DxWnd trap", 0)
#else
#define utrap(m)
#endif
//#define EMULATEVJOFFSETS

LPVOID gFakeJoy = NULL;

extern BOOL vjGetJoy(char *, LPJOYINFO);
extern char *ExplainDICooperativeFlags(DWORD);
extern char *ExplainDataFormatFlags(DWORD);
extern char *sGUIDProperty(REFGUID);
extern char *sPropHow(DWORD);
extern char *sEnumObjectFlags(DWORD);
extern char *sDataFormatFlag(DWORD);
extern char *sODFFlag(DWORD);
extern char *sHow(DWORD);
extern DXWNDSTATUS *pStatus;

#define MINJRANGE 0
#define MAXJRANGE 0xFFFF

IFakeDirectInputDevice::IFakeDirectInputDevice() {
    b_Acquired = FALSE;
    b_AutoCenter = FALSE;
    b_DJInputAbs = TRUE;
    l_MinXRange = MINJRANGE;
    l_MaxXRange = MAXJRANGE;
    l_MinYRange = MINJRANGE;
    l_MaxYRange = MAXJRANGE;
    b_HasHatSwitch = FALSE;
    // default values, but could change at runtime
    ofs_xaxis = DIJOFS_X;
    ofs_yaxis = DIJOFS_Y;
    ofs_btnL = DIJOFS_BUTTON(0);
    ofs_btnR = DIJOFS_BUTTON(1);
    ofs_btnC = DIJOFS_BUTTON(2);
    ofs_pov = DIJOFS_POV(0);
}

IFakeDirectInputDeviceA::IFakeDirectInputDeviceA() {
    b_Acquired = FALSE;
    b_AutoCenter = FALSE;
    b_DJInputAbs = TRUE;
    l_MinXRange = MINJRANGE;
    l_MaxXRange = MAXJRANGE;
    l_MinYRange = MINJRANGE;
    l_MaxYRange = MAXJRANGE;
    b_HasHatSwitch = FALSE;
    // default values, but could change at runtime
    ofs_xaxis = DIJOFS_X;
    ofs_yaxis = DIJOFS_Y;
    ofs_btnL = DIJOFS_BUTTON(0);
    ofs_btnR = DIJOFS_BUTTON(1);
    ofs_btnC = DIJOFS_BUTTON(2);
    ofs_pov = DIJOFS_POV(0);
    IsAscii = TRUE;
    s_ClassName = "IDirectInputDeviceA";
}

IFakeDirectInputDeviceW::IFakeDirectInputDeviceW() {
    b_Acquired = FALSE;
    b_AutoCenter = FALSE;
    b_DJInputAbs = TRUE;
    l_MinXRange = MINJRANGE;
    l_MaxXRange = MAXJRANGE;
    l_MinYRange = MINJRANGE;
    l_MaxYRange = MAXJRANGE;
    b_HasHatSwitch = TRUE;
    // default values, but could change at runtime
    ofs_xaxis = DIJOFS_X;
    ofs_yaxis = DIJOFS_Y;
    ofs_btnL = DIJOFS_BUTTON(0);
    ofs_btnR = DIJOFS_BUTTON(1);
    ofs_btnC = DIJOFS_BUTTON(2);
    ofs_pov = DIJOFS_POV(0);
    IsAscii = FALSE;
    s_ClassName = "IDirectInputDeviceW";
}

HRESULT IFakeDirectInputDevice::QueryInterface(REFIID riid, void **ppv) {
    OutTraceIN("%s::QueryInterface: guid=%s\n", s_ClassName, ExplainGUID((GUID *)&riid));
    b_Acquired = FALSE;
    *ppv = this;
    m_lRef++;
    //DEFINE_GUID(IID_IDirectInputDeviceA, 0x5944E680,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    //DEFINE_GUID(IID_IDirectInputDeviceW, 0x5944E681,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    //DEFINE_GUID(IID_IDirectInputDevice2A,0x5944E682,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    //DEFINE_GUID(IID_IDirectInputDevice2W,0x5944E683,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    //DEFINE_GUID(IID_IDirectInputDevice7A,0x57D7C6BC,0x2356,0x11D3,0x8E,0x9D,0x00,0xC0,0x4F,0x68,0x44,0xAE);
    //DEFINE_GUID(IID_IDirectInputDevice7W,0x57D7C6BD,0x2356,0x11D3,0x8E,0x9D,0x00,0xC0,0x4F,0x68,0x44,0xAE);
    //DEFINE_GUID(IID_IDirectInputDevice8A,0x54D41080,0xDC15,0x4833,0xA4,0x1B,0x74,0x8F,0x73,0xA3,0x81,0x79);
    //DEFINE_GUID(IID_IDirectInputDevice8W,0x54D41081,0xDC15,0x4833,0xA4,0x1B,0x74,0x8F,0x73,0xA3,0x81,0x79);
    switch(riid.Data1) {
    case 0x5944E680:
        IVersion = 1, IsAscii = TRUE;
        s_ClassName = "DirectInputDeviceA";
        break;
    case 0x5944E681:
        IVersion = 1, IsAscii = FALSE;
        s_ClassName = "DirectInputDeviceW";
        break;
    case 0x5944E682:
        IVersion = 2, IsAscii = TRUE;
        s_ClassName = "DirectInputDevice2A";
        break;
    case 0x5944E683:
        IVersion = 2, IsAscii = FALSE;
        s_ClassName = "DirectInputDevice2W";
        break;
    case 0x57D7C6BC:
        IVersion = 7, IsAscii = TRUE;
        s_ClassName = "DirectInputDevice7A";
        break;
    case 0x57D7C6BD:
        IVersion = 7, IsAscii = FALSE;
        s_ClassName = "DirectInputDevice7W";
        break;
    case 0x54D41080:
        IVersion = 8, IsAscii = TRUE;
        s_ClassName = "DirectInputDevice8A";
        break;
    case 0x54D41081:
        IVersion = 8, IsAscii = FALSE;
        s_ClassName = "DirectInputDevice8W";
        break;
    }
    return (HRESULT)DI_OK;
}

ULONG IFakeDirectInputDevice::AddRef(void) {
    OutTraceIN("%s::AddRef\n", s_ClassName);
    return (ULONG)m_lRef++;
}

ULONG IFakeDirectInputDevice::Release(void) {
    OutTraceIN("%s::Release\n", s_ClassName);
    return (ULONG)m_lRef--;
}

HRESULT IFakeDirectInputDevice::GetCapabilities(LPDIDEVCAPS lpDIDevCaps) {
    OutTraceIN("%s::GetCapabilities: size=%d\n", s_ClassName, lpDIDevCaps->dwSize);
    if(lpDIDevCaps->dwSize != sizeof(DIDEVCAPS)) {
        OutTraceE("%s::GetCapabilities: bad size ret=DIERR_NOTINITIALIZED\n", s_ClassName);
        return DIERR_NOTINITIALIZED;
    }
    lpDIDevCaps->dwAxes = 2;
    lpDIDevCaps->dwButtons = 3;
    lpDIDevCaps->dwDevType =
        DIDEVTYPE_HID |
        DIDFT_MAKEINSTANCE(1) |
        (IVersion == 8 ? DI8DEVTYPE_JOYSTICK : DIDEVTYPE_JOYSTICK);
    // if SKIPDEVTYPEHID pretend this device is NOT a HID!!
    if(dxw.dwFlags7 & SKIPDEVTYPEHID) lpDIDevCaps->dwDevType &= ~DIDEVTYPE_HID;
    // Beware: flags should not be exceeding: "Rollcage redux" doesn't accept flags different
    // from 1=DIDC_ATTACHED 2=DIDC_POLLEDDEVICE 4=DIDC_EMULATED also when in combination between them
    // It seems that DIDC_ATTACHED is the condition for polling the joystick data.
    lpDIDevCaps->dwFlags = DIDC_ATTACHED | DIDC_EMULATED;
    //lpDIDevCaps->dwFlags |= 0xCF00; // to emulate vJoy - useless
    //lpDIDevCaps->dwFlags = DIDC_ATTACHED;
    //lpDIDevCaps->dwFlags |= DIDC_POLLEDDATAFORMAT;
    //lpDIDevCaps->dwFlags |= DIDC_POLLEDDEVICE;
    lpDIDevCaps->dwFlags = DIDC_ATTACHED | DIDC_POLLEDDEVICE | DIDC_EMULATED;
    lpDIDevCaps->dwPOVs = 0; // ??
    if(b_HasHatSwitch) lpDIDevCaps->dwPOVs = 1;
    if(lpDIDevCaps->dwSize == 44) {
        lpDIDevCaps->dwFFSamplePeriod = 1000000;
        lpDIDevCaps->dwFFMinTimeResolution = 1000000;
        lpDIDevCaps->dwFirmwareRevision = 0x02;
        lpDIDevCaps->dwHardwareRevision = 0x04;
        lpDIDevCaps->dwFFDriverVersion =  0x62;
    }
    OutTraceIN("%s::GetCapabilities: "
               "cap={size=%d flags=%#x devtype=%#x axes=%d buttons=%d povs=%d mintimeres=%d sampleperiod=%d "
               "version(HW/FW/SW)=(%#x/%#x/%#x)}\n",
               s_ClassName,
               lpDIDevCaps->dwSize,
               lpDIDevCaps->dwFlags,
               lpDIDevCaps->dwDevType,
               lpDIDevCaps->dwAxes,
               lpDIDevCaps->dwButtons,
               lpDIDevCaps->dwPOVs,
               lpDIDevCaps->dwFFMinTimeResolution,
               lpDIDevCaps->dwFFSamplePeriod,
               lpDIDevCaps->dwHardwareRevision,
               lpDIDevCaps->dwFirmwareRevision,
               lpDIDevCaps->dwFFDriverVersion
              );
    OutTraceIN("%s::GetCapabilities: ret=DI_OK\n", s_ClassName);
    return DI_OK;
}

/****************************************************************************

        Predefined object types

 ****************************************************************************/

//DEFINE_GUID(GUID_XAxis,   0xA36D02E0,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
//DEFINE_GUID(GUID_YAxis,   0xA36D02E1,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
//DEFINE_GUID(GUID_ZAxis,   0xA36D02E2,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
//DEFINE_GUID(GUID_RxAxis,  0xA36D02F4,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
//DEFINE_GUID(GUID_RyAxis,  0xA36D02F5,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
//DEFINE_GUID(GUID_RzAxis,  0xA36D02E3,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
//DEFINE_GUID(GUID_Slider,  0xA36D02E4,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
//DEFINE_GUID(GUID_Button,  0xA36D02F0,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
//DEFINE_GUID(GUID_Key,     0x55728220,0xD33C,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
//DEFINE_GUID(GUID_POV,     0xA36D02F2,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
//DEFINE_GUID(GUID_Unknown, 0xA36D02F3,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

HRESULT IFakeDirectInputDevice::EnumObjects(LPVOID cb, LPVOID lpv, DWORD dwFlags) {
    if(IsAscii)
        return (*(IFakeDirectInputDeviceA *)this).EnumObjects((LPDIENUMDEVICEOBJECTSCALLBACKA)cb, lpv, dwFlags);
    else
        return (*(IFakeDirectInputDeviceW *)this).EnumObjects((LPDIENUMDEVICEOBJECTSCALLBACKW)cb, lpv, dwFlags);
}

#ifdef EMULATEVJOFFSETS
#undef DIJOFS_BUTTON
#define DIJOFS_BUTTON(n) (24+n)
#undef DIJOFS_POV
#define DIJOFS_POV(n) (8+(4*n))
#endif

HRESULT IFakeDirectInputDeviceA::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA cb, LPVOID lpv, DWORD dwFlags) {
    HRESULT res;
    DIDEVICEOBJECTINSTANCEA obj;
    OutTraceIN("%s::EnumObjects: dwFlags=%#x(%s)\n", s_ClassName, dwFlags, sEnumObjectFlags(dwFlags));
    memset(&obj, 0, sizeof(obj));
    obj.dwSize = sizeof(obj);
    // enum axis
    if((dwFlags & (DIDFT_ABSAXIS | DIDFT_RELAXIS)) || (dwFlags == DIDFT_ALL)) {
        OutTraceIN("> %s\n", sDataFormatFlag(dwFlags));
        obj.guidType = GUID_XAxis;
        obj.dwOfs = DIJOFS_X;
        obj.dwType = DIDFT_MAKEINSTANCE(0) | (b_DJInputAbs ? DIDFT_ABSAXIS : DIDFT_RELAXIS);
        obj.dwFlags = DIDOI_ASPECTPOSITION;
        strcpy_s(obj.tszName, sizeof(obj.tszName), "X Axis");
        OutTraceIN("> CALLBACK size=%d type=%#x flags=%#x ofs=%d dimensions=%d name=%s\n",
                   obj.dwSize, obj.dwType, obj.dwFlags, obj.dwOfs, obj.dwDimension, obj.tszName);
        res = (*cb)(&obj, lpv);
        if(res == DIENUM_STOP) return DI_OK;
        obj.guidType = GUID_YAxis;
        obj.dwOfs = DIJOFS_Y;
        obj.dwType = DIDFT_MAKEINSTANCE(1) | (b_DJInputAbs ? DIDFT_ABSAXIS : DIDFT_RELAXIS);
        strcpy_s(obj.tszName, sizeof(obj.tszName), "Y Axis");
        OutTraceIN("> CALLBACK size=%d type=%#x flags=%#x ofs=%d dimensions=%d name=%s\n",
                   obj.dwSize, obj.dwType, obj.dwFlags, obj.dwOfs, obj.dwDimension, obj.tszName);
        res = (*cb)(&obj, lpv);
        if(res == DIENUM_STOP) return DI_OK;
    }
    // enum buttons
    if((dwFlags & (DIDFT_PSHBUTTON | DIDFT_TGLBUTTON)) || (dwFlags == DIDFT_ALL)) {
        OutTraceIN("> %s\n", sDataFormatFlag(dwFlags));
        obj.guidType = GUID_Button;
        obj.dwOfs = DIJOFS_BUTTON(0);
        obj.dwType = DIDFT_MAKEINSTANCE(2) | DIDFT_PSHBUTTON;
        obj.dwFlags = DIDOI_ASPECTPOSITION;
        strcpy_s(obj.tszName, 260, "L Button");
        OutTraceIN("> CALLBACK size=%d type=%#x flags=%#x ofs=%d dimensions=%d name=%s\n",
                   obj.dwSize, obj.dwType, obj.dwFlags, obj.dwOfs, obj.dwDimension, obj.tszName);
        res = (*cb)(&obj, lpv);
        if(res == DIENUM_STOP) return DI_OK;
        obj.dwOfs = DIJOFS_BUTTON(1);
        obj.dwType = DIDFT_MAKEINSTANCE(3) | DIDFT_PSHBUTTON;
        strcpy_s(obj.tszName, 260, "R Button");
        OutTraceIN("> CALLBACK size=%d type=%#x flags=%#x ofs=%d dimensions=%d name=%s\n",
                   obj.dwSize, obj.dwType, obj.dwFlags, obj.dwOfs, obj.dwDimension, obj.tszName);
        res = (*cb)(&obj, lpv);
        if(res == DIENUM_STOP) return DI_OK;
        obj.dwOfs = DIJOFS_BUTTON(2);
        obj.dwType = DIDFT_MAKEINSTANCE(4) | DIDFT_PSHBUTTON;
        strcpy_s(obj.tszName, 260, "C Button");
        OutTraceIN("> CALLBACK size=%d type=%#x flags=%#x ofs=%d dimensions=%d name=%s\n",
                   obj.dwSize, obj.dwType, obj.dwFlags, obj.dwOfs, obj.dwDimension, obj.tszName);
        res = (*cb)(&obj, lpv);
        if(res == DIENUM_STOP) return DI_OK;
    }
    // POV
    if(b_HasHatSwitch && ((dwFlags & DIDFT_POV) || (dwFlags == DIDFT_ALL))) {
        OutTraceIN("> %s\n", sDataFormatFlag(dwFlags));
        obj.guidType = GUID_POV;
        obj.dwOfs = DIJOFS_POV(5);
        obj.dwType = DIDFT_MAKEINSTANCE(0) | DIDFT_POV;
        obj.dwFlags = DIDOI_ASPECTPOSITION;
        obj.dwDimension = 20;
        strcpy_s(obj.tszName, 260, "Hat Switch");
        OutTraceIN("> CALLBACK size=%d type=%#x flags=%#x ofs=%d dimensions=%d name=%s\n",
                   obj.dwSize, obj.dwType, obj.dwFlags, obj.dwOfs, obj.dwDimension, obj.tszName);
        res = (*cb)(&obj, lpv);
        if(res == DIENUM_STOP) return DI_OK;
    }
    return DI_OK;
}

HRESULT IFakeDirectInputDeviceW::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKW cb, LPVOID lpv, DWORD dwFlags) {
    HRESULT res;
    DIDEVICEOBJECTINSTANCEW obj;
    OutTraceIN("%s::EnumObjects: dwFlags=%#x(%s)\n", s_ClassName, dwFlags, sEnumObjectFlags(dwFlags));
    memset(&obj, 0, sizeof(obj));
    obj.dwSize = sizeof(obj);
    // enum axis
    if((dwFlags & (DIDFT_ABSAXIS | DIDFT_RELAXIS)) || (dwFlags == DIDFT_ALL)) {
        OutTraceIN("> %s\n", sDataFormatFlag(dwFlags));
        obj.guidType = GUID_XAxis;
        obj.dwOfs = 0;
        obj.dwType = DIDFT_MAKEINSTANCE(0) | (b_DJInputAbs ? DIDFT_ABSAXIS : DIDFT_RELAXIS);
        obj.dwFlags = DIDOI_ASPECTPOSITION;
        wcscpy_s(obj.tszName, 260, L"X Axis");
        OutTraceIN("> CALLBACK size=%d type=%#x flags=%#x ofs=%d dimensions=%d name=%ls\n",
                   obj.dwSize, obj.dwType, obj.dwFlags, obj.dwOfs, obj.dwDimension, obj.tszName);
        res = (*cb)(&obj, lpv);
        if(res == DIENUM_STOP) return DI_OK;
        obj.guidType = GUID_YAxis;
        obj.dwOfs = 4;
        obj.dwType = DIDFT_MAKEINSTANCE(1) | (b_DJInputAbs ? DIDFT_ABSAXIS : DIDFT_RELAXIS);
        wcscpy_s(obj.tszName, 260, L"Y Axis");
        OutTraceIN("> CALLBACK size=%d type=%#x flags=%#x ofs=%d dimensions=%d name=%ls\n",
                   obj.dwSize, obj.dwType, obj.dwFlags, obj.dwOfs, obj.dwDimension, obj.tszName);
        res = (*cb)(&obj, lpv);
        if(res == DIENUM_STOP) return DI_OK;
    }
    // enum buttons
    if((dwFlags & (DIDFT_PSHBUTTON | DIDFT_TGLBUTTON)) || (dwFlags == DIDFT_ALL)) {
        OutTraceIN("> %s\n", sDataFormatFlag(dwFlags));
        obj.guidType = GUID_Button;
        obj.dwOfs = DIJOFS_BUTTON(0);
        obj.dwType = DIDFT_MAKEINSTANCE(0) | DIDFT_PSHBUTTON;
        obj.dwFlags = DIDOI_ASPECTPOSITION;
        wcscpy_s(obj.tszName, 260, L"L Button");
        OutTraceIN("> CALLBACK size=%d type=%#x flags=%#x ofs=%d dimensions=%d name=%ls\n",
                   obj.dwSize, obj.dwType, obj.dwFlags, obj.dwOfs, obj.dwDimension, obj.tszName);
        res = (*cb)(&obj, lpv);
        if(res == DIENUM_STOP) return DI_OK;
        obj.dwOfs = DIJOFS_BUTTON(1);
        obj.dwType = DIDFT_MAKEINSTANCE(1) | DIDFT_PSHBUTTON;
        wcscpy_s(obj.tszName, 260, L"R Button");
        OutTraceIN("> CALLBACK size=%d type=%#x flags=%#x ofs=%d dimensions=%d name=%ls\n",
                   obj.dwSize, obj.dwType, obj.dwFlags, obj.dwOfs, obj.dwDimension, obj.tszName);
        res = (*cb)(&obj, lpv);
        if(res == DIENUM_STOP) return DI_OK;
        obj.dwOfs = DIJOFS_BUTTON(2);
        obj.dwType = DIDFT_MAKEINSTANCE(2) | DIDFT_PSHBUTTON;
        wcscpy_s(obj.tszName, 260, L"C Button");
        OutTraceIN("> CALLBACK size=%d type=%#x flags=%#x ofs=%d dimensions=%d name=%ls\n",
                   obj.dwSize, obj.dwType, obj.dwFlags, obj.dwOfs, obj.dwDimension, obj.tszName);
        res = (*cb)(&obj, lpv);
        if(res == DIENUM_STOP) return DI_OK;
    }
    // POV
    if(b_HasHatSwitch && ((dwFlags & DIDFT_POV) || (dwFlags == DIDFT_ALL))) {
        OutTraceIN("> %s\n", sDataFormatFlag(dwFlags));
        obj.guidType = GUID_POV;
        obj.dwOfs = DIJOFS_POV(0);
        obj.dwType = DIDFT_MAKEINSTANCE(0) | DIDFT_POV;
        obj.dwFlags = DIDOI_ASPECTPOSITION;
        obj.dwDimension = 20;
        wcscpy_s(obj.tszName, 260, L"Hat Switch");
        OutTraceIN("> CALLBACK size=%d type=%#x flags=%#x ofs=%d dimensions=%d name=%ls\n",
                   obj.dwSize, obj.dwType, obj.dwFlags, obj.dwOfs, obj.dwDimension, obj.tszName);
        res = (*cb)(&obj, lpv);
        if(res == DIENUM_STOP) return DI_OK;
    }
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::SetProperty(REFGUID guid, LPCDIPROPHEADER lpdph) {
    OutTraceIN("%s::SetProperty: guid=(%s) "
               "ph={size=%d hdrsize=%d how=%#x(%s) obj=%#x}\n",
               s_ClassName,
               sGUIDProperty(guid),
               lpdph->dwSize,
               lpdph->dwHeaderSize,
               lpdph->dwHow, sPropHow(lpdph->dwHow),
               lpdph->dwObj);
    LPDIPROPDWORD lpWord = (LPDIPROPDWORD)lpdph;
    LPDIPROPRANGE lpRange = (LPDIPROPRANGE)lpdph;
    switch((DWORD)&guid) {
    case &DIPROP_BUFFERSIZE:
        OutTraceIN("> BUFFERSIZE data=%#x\n", lpWord->dwData);
        break;
    case &DIPROP_RANGE:
        OutTraceIN("> RANGE(min:max)=(%d:%d)\n", lpRange->lMin, lpRange->lMax);
        if(lpdph->dwHow == DIPH_BYID) {
            switch(DIDFT_GETINSTANCE(lpdph->dwObj)) {
            case 0:
                OutDebugIN("> X Axis\n");
                l_MinXRange = lpRange->lMin;
                l_MaxXRange = lpRange->lMax;
                break;
            case 1:
                OutDebugIN("> Y Axis\n");
                l_MinYRange = lpRange->lMin;
                l_MaxYRange = lpRange->lMax;
                break;
            default:
                OutDebugIN("> unknown\n");
                break;
            }
        } else if(lpdph->dwHow == DIPH_BYOFFSET) {
            switch(lpdph->dwObj) {
            case DIMOFS_X:
                OutDebugIN("> X Axis\n");
                l_MinXRange = lpRange->lMin;
                l_MaxXRange = lpRange->lMax;
                break;
            case DIMOFS_Y:
                OutDebugIN("> Y Axis\n");
                l_MinYRange = lpRange->lMin;
                l_MaxYRange = lpRange->lMax;
                break;
            default:
                OutDebugIN("> unknown\n");
                break;
            }
        } else if(lpdph->dwHow == DIPH_DEVICE) {
            OutDebugIN("> device\n");
            l_MinXRange = lpRange->lMin;
            l_MaxXRange = lpRange->lMax;
            l_MinYRange = lpRange->lMin;
            l_MaxYRange = lpRange->lMax;
        }
        break;
    case &DIPROP_DEADZONE:
        OutTraceIN("> DEADZONE data=%d\n", lpWord->dwData);
        l_DeadZone = lpWord->dwData;
        // set deadzone value in shared area.
        // N.b. IDIrectInput object deadzone is in range 0 - 10000, where
        // 0 = 0, 10000 = 100%, so the value should be scaled by 100.
        pStatus->VJoyDeadZone = l_DeadZone / 100;
        break;
    case &DIPROP_SATURATION:
        OutTraceIN("> SATURATION data=%d\n", lpWord->dwData);
        l_Saturation = lpWord->dwData;
        break;
    case &DIPROP_AUTOCENTER:
        OutTraceIN("> AUTOCENTER data=%#x\n", lpWord->dwData);
        b_AutoCenter = lpWord->dwData;
        break;
    default:
        OutTraceIN("> unknown guid=%#x\n", (DWORD)&guid);
    }
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::GetProperty(REFGUID guid, LPDIPROPHEADER lpdph) {
    OutTraceIN("%s::GetProperty: guid=(%s) "
               "ph={size=%d hdrsize=%d how=%#x(%s) obj=%#x}\n",
               s_ClassName,
               sGUIDProperty(guid),
               lpdph->dwSize,
               lpdph->dwHeaderSize,
               lpdph->dwHow, sPropHow(lpdph->dwHow),
               lpdph->dwObj);
    LPDIPROPDWORD lpWord = (LPDIPROPDWORD)lpdph;
    LPDIPROPRANGE lpRange = (LPDIPROPRANGE)lpdph;
    switch((DWORD)&guid) {
    case &DIPROP_BUFFERSIZE:
        lpWord->dwData = 1;
        OutTraceIN("> BUFFERSIZE data=%#x\n", lpWord->dwData);
        break;
    case &DIPROP_RANGE:
        if(lpdph->dwHow == DIPH_BYID) {
            switch(DIDFT_GETINSTANCE(lpdph->dwObj)) {
            case 0:
                OutDebugIN("> X Axis\n");
                lpRange->lMin = l_MinXRange;
                lpRange->lMax = l_MaxXRange;
                break;
            case 1:
                OutDebugIN("> Y Axis\n");
                lpRange->lMin = l_MinYRange;
                lpRange->lMax = l_MaxYRange;
                break;
            default:
                OutDebugIN("> unknown\n");
                lpRange->lMin = 0;
                lpRange->lMax = 0;
                break;
            }
        } else if(lpdph->dwHow == DIPH_BYOFFSET) {
            switch(lpdph->dwObj) {
            case DIMOFS_X:
                OutDebugIN("> X Axis\n");
                lpRange->lMin = l_MinXRange;
                lpRange->lMax = l_MaxXRange;
                break;
            case DIMOFS_Y:
                OutDebugIN("> Y Axis\n");
                lpRange->lMin = l_MinYRange;
                lpRange->lMax = l_MaxYRange;
                break;
            default:
                OutDebugIN("> unknown\n");
                lpRange->lMin = 0;
                lpRange->lMax = 0;
                break;
            }
        } else if(lpdph->dwHow == DIPH_DEVICE) {
            OutDebugIN("> device\n");
            lpRange->lMin = l_MinXRange;
            lpRange->lMax = l_MaxXRange;
        }
        OutTraceIN("> RANGE(min:max)=(%d:%d)\n", lpRange->lMin, lpRange->lMax);
        break;
    case &DIPROP_DEADZONE:
        l_DeadZone  =  100 * pStatus->VJoyDeadZone;
        lpWord->dwData = l_DeadZone;
        OutTraceIN("> DEADZONE data=%#x\n", lpWord->dwData);
        break;
    case &DIPROP_SATURATION:
        lpWord->dwData = l_Saturation;
        OutTraceIN("> SATURATION data=%#x\n", lpWord->dwData);
        break;
    case &DIPROP_AUTOCENTER:
        lpWord->dwData = b_AutoCenter;
        OutTraceIN("> AUTOCENTER data=%#x\n", lpWord->dwData);
        break;
    case &DIPROP_AXISMODE:
        lpWord->dwData = DIPROPAXISMODE_ABS;
        OutTraceIN("> AXISMODE data=DIPROPAXISMODE_ABS\n");
        break;
    // IDirectInput8 extensions:
    case &DIPROP_JOYSTICKID:
        lpWord->dwData = 1;
        OutTraceIN("> JOYSTICKID data=1\n");
        break;
    case &DIPROP_GETPORTDISPLAYNAME:
    case &DIPROP_PHYSICALRANGE:
    case &DIPROP_LOGICALRANGE:
    case &DIPROP_KEYNAME:
    case &DIPROP_CPOINTS:
    case &DIPROP_APPDATA:
    case &DIPROP_SCANCODE:
    case &DIPROP_VIDPID:
    case &DIPROP_USERNAME:
    case &DIPROP_TYPENAME:
        OutTraceIN("> unimplemented guid=%#x ret=DIERR_UNSUPPORTED\n", (DWORD)&guid);
        return DIERR_UNSUPPORTED;
        break;
    default:
        OutTraceIN("> unsupported guid=%#x ret=DIERR_UNSUPPORTED\n", (DWORD)&guid);
        return DIERR_UNSUPPORTED;
        break;
    }
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::Acquire() {
    OutTraceIN("%s::Acquire: res=%s\n", s_ClassName, b_Acquired ? "NOEFFECT" : "OK");
    if(b_Acquired) return DI_NOEFFECT; // already acquired
    b_Acquired = TRUE;
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::Unacquire() {
    OutTraceIN("%s::Unacquire\n", s_ClassName);
    OutTraceIN("%s::Unacquire: res=%s\n", s_ClassName, b_Acquired ? "OK" : "NOEFFECT");
    if(!b_Acquired) return DI_NOEFFECT; // already unacquired
    b_Acquired = FALSE;
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::GetDeviceState(DWORD cbData, LPVOID lpvData) {
    LPDIJOYSTATE lpdjs;
    LPDIJOYSTATE2 lpdjs2;
    LPBYTE lpcustom;
    OutTraceIN("%s::GetDeviceState: data=%d active=%#x\n", s_ClassName, cbData, dxw.bActive);
    GetJoyInfo("> GetDeviceState");
    //if(!bActive) {
    //	OutTraceIN("%s::GetDeviceState: return DIERR_NOTACQUIRED\n", s_ClassName);
    //	return DIERR_NOTACQUIRED;
    //}
    memset(lpvData, 0, cbData); // clear structure
    switch(cbData) {
    case sizeof(DIJOYSTATE):
        OutTraceIN("> format=DIJOYSTATE\n");
        lpdjs = (LPDIJOYSTATE)lpvData;
        lpdjs->lX = l_ValX;
        lpdjs->lY = l_ValY;
        lpdjs->rgbButtons[0] = (wButtons & JOY_BUTTON1) ? 0xFF : 0x00;
        lpdjs->rgbButtons[1] = (wButtons & JOY_BUTTON2) ? 0xFF : 0x00;
        lpdjs->rgbButtons[2] = (wButtons & JOY_BUTTON3) ? 0xFF : 0x00;
        // warning: POV undefined state is (WORD)-1, 0 is one valid direction !!!
        if(b_HasHatSwitch) for(int i = 0; i < 4; i++) lpdjs->rgdwPOV[i] = -1;
        break;
    case sizeof(DIJOYSTATE2):
        OutTraceIN("> format=DIJOYSTATE2\n");
        lpdjs2 = (LPDIJOYSTATE2)lpvData;
        lpdjs2->lX = l_ValX;
        lpdjs2->lY = l_ValY;
        lpdjs2->rgbButtons[0] = (wButtons & JOY_BUTTON1) ? 0xFF : 0x00;
        lpdjs2->rgbButtons[1] = (wButtons & JOY_BUTTON2) ? 0xFF : 0x00;
        lpdjs2->rgbButtons[2] = (wButtons & JOY_BUTTON3) ? 0xFF : 0x00;
        // warning: POV undefined state is (WORD)-1, 0 is one valid direction !!!
        if(b_HasHatSwitch) for(int i = 0; i < 4; i++) lpdjs2->rgdwPOV[i] = -1;
        break;
    default:
        OutTraceIN("> format=custom\n");
        lpcustom = (LPBYTE)lpvData;
        memcpy(lpcustom + ofs_xaxis, &l_ValX, sizeof(DWORD));
        memcpy(lpcustom + ofs_yaxis, &l_ValY, sizeof(DWORD));
        lpcustom[ofs_btnL] = (wButtons & JOY_BUTTON1) ? 0xFF : 0x00;
        lpcustom[ofs_btnR] = (wButtons & JOY_BUTTON2) ? 0xFF : 0x00;
        lpcustom[ofs_btnC] = (wButtons & JOY_BUTTON3) ? 0xFF : 0x00;
        // pow ???
        if(b_HasHatSwitch) for(int i = 0; i < 4; i++) lpcustom[ofs_pov + i] = -1;
        break;
    }
    OutHexIN((LPBYTE)lpvData, cbData);
    return DI_OK;
}

#define DODSIZE 5

// dwLastSample: timestamp of last data read
// dwSequence: sequence number of last read
//
HRESULT IFakeDirectInputDevice::GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags) {
    HRESULT res = DI_OK;
    DWORD dwTimeStamp;
    static DWORD dwSequence = 0;	// sequence number of last read
    static DWORD dwLastSample = 0;	// timestamp of last data read
    static DWORD dwLastODIdx = 0;	// index of last data element consumed
    static DWORD iDataAvail = 0;
    DWORD odidx = 0;
    // you may have up to 5 data elements: X axis, Y axis, button 1, 2 and 3.
    static DIDEVICEOBJECTDATA dod[DODSIZE];
    OutDebugIN("%s::GetDeviceData: size=%d inout=%d flags=%#x(%s) rgdod=%#x\n", s_ClassName,
               cbObjectData, *pdwInOut, dwFlags,
               dwFlags ? (dwFlags == DIGDD_PEEK ? "PEEK" : "unknown") : "NULL",
               rgdod);
    if(!dxw.bActive) {
        OutDebugIN("%s::GetDeviceData: inactive inout=0\n", s_ClassName);
        *pdwInOut = 0;
        return DI_OK;
    }
    // limit number of answers on time, max 1 every 10 mSec
    // needed by "Trackmania Nations" where a very tight polling would make the
    // game hung at startup otherwise.
    if(((*pGetTickCount)() - dwLastSample) > 10) {
        // if time elapsed, flush anything could be in the buffer and make another read
        dwLastSample = (*pGetTickCount)();
        iDataAvail = 0; // zero until read ....
        dwLastODIdx = 0;
        dwSequence++;
    } else {
        if(iDataAvail == 0) {
            // queue flushed, wait next time slot
            OutDebugIN("%s::GetDeviceData: emptyqueue inout=0\n", s_ClassName);
            *pdwInOut = 0;
            return DI_OK;
        }
        // else go ahead and return data elements
    }
    if((*pdwInOut == INFINITE) && (rgdod == NULL)) { // v2.05.44: special cases that should return DI_OK
        *pdwInOut = iDataAvail; // if time elapsed, pretend there are some items ready to be read / flushed
        OutDebugIN("%s::GetDeviceData: %s\n", s_ClassName, (dwFlags & DIGDD_PEEK) ? "PEEK" : "FLUSH");
        if(!(dwFlags & DIGDD_PEEK)) iDataAvail = 0; // all data flushed
        return DI_OK;
    }
    dwTimeStamp = dwLastSample;
    if(iDataAvail == 0) {
        // Read joystick data.
        // BEWARE: buttons must show transitions between ON and OFF states or
        // they could be considered permanently ON. Fixes "Hulk".
        GetJoyInfo("> GetDeviceData");
        dod[odidx].dwOfs = ofs_xaxis;
        dod[odidx].dwData = l_ValX;
        dod[odidx].dwTimeStamp = dwTimeStamp;
        dod[odidx].dwSequence = dwSequence;
        odidx++;
        dod[odidx].dwOfs = ofs_yaxis;
        dod[odidx].dwData = l_ValY;
        dod[odidx].dwTimeStamp = dwTimeStamp;
        dod[odidx].dwSequence = dwSequence;
        odidx++;
        dod[odidx].dwOfs = ofs_btnL;
        dod[odidx].dwData = (wButtons & JOY_BUTTON1) ? 0xFF : 0x00;
        dod[odidx].dwTimeStamp = dwTimeStamp;
        dod[odidx].dwSequence = dwSequence;
        odidx++;
        dod[odidx].dwOfs = ofs_btnR;
        dod[odidx].dwData = (wButtons & JOY_BUTTON2) ? 0xFF : 0x00;
        dod[odidx].dwTimeStamp = dwTimeStamp;
        dod[odidx].dwSequence = dwSequence;
        odidx++;
        dod[odidx].dwOfs = ofs_btnC;
        dod[odidx].dwData = (wButtons & JOY_BUTTON3) ? 0xFF : 0x00;
        dod[odidx].dwTimeStamp = dwTimeStamp;
        dod[odidx].dwSequence = dwSequence;
        odidx++;
        iDataAvail = odidx;
    }
    OutDebugIN("%s::GetDeviceData: iDataAvail=%d\n", s_ClassName, iDataAvail);
    // found in "Hulk": reading one DOD element at a time!
    // k: number of OD returned by this call.
    // It is the minimum between what available and what asked for
    DWORD k = *pdwInOut;
    if(k > iDataAvail) k = iDataAvail;
    for(DWORD i = 0; i < k; i++) {
        if(rgdod) rgdod[i] = dod[dwLastODIdx + i];
        OutDebugIN("%s::GetDeviceData: dod[%d] %s seq=%d time=%#x ofs=%d val=%d\n",
                   s_ClassName, i, (dwFlags & DIGDD_PEEK) ? "PEEK" : "GET",
                   rgdod[i].dwSequence, rgdod[i].dwTimeStamp, rgdod[i].dwOfs, rgdod[i].dwData);
    }
    if(!(dwFlags & DIGDD_PEEK)) {
        dwLastODIdx += k;
        iDataAvail -= k;
    }
    res = iDataAvail ? DI_BUFFEROVERFLOW : DI_OK;
    OutTraceIN("%s::GetDeviceData: out=%d res=%s\n", s_ClassName, *pdwInOut, res == DI_OK ? "OK" : "BUFFEROVERFLOW");
    return res;
}

HRESULT IFakeDirectInputDevice::SetDataFormat(LPCDIDATAFORMAT lpdf) {
    LPDIOBJECTDATAFORMAT lpodf;
    OutTraceIN("%s::SetDataFormat: size=%d objsize=%d flags=%#x(%s) datasize=%d numobj=%d\n",
               s_ClassName,
               lpdf->dwSize,
               lpdf->dwObjSize,
               lpdf->dwFlags, ExplainDataFormatFlags(lpdf->dwFlags),
               lpdf->dwDataSize,
               lpdf->dwNumObjs
              );
    lpodf = lpdf->rgodf;
    if(IsDebugIN) {
        for(DWORD i = 0; i < lpdf->dwNumObjs; i++) {
            DWORD ofs = lpodf[i].dwOfs;
            OutTraceIN("%s::SetDataFormat: obj[%d] ofs=%d flags=%#x(%s) type=%#x(%s)\n",
                       s_ClassName, i,
                       lpodf[i].dwOfs,
                       lpodf[i].dwFlags, sODFFlag(lpodf[i].dwFlags),
                       lpodf[i].dwType, sDataFormatFlag(lpodf[i].dwType)
                      );
            switch(i) {
            case 0:
                ofs_xaxis = ofs;
                break;
            case 1:
                ofs_yaxis = ofs;
                break;
            case 2:
                ofs_btnL =  ofs;
                break;
            case 3:
                ofs_btnR =  ofs;
                break;
            case 4:
                ofs_btnC =  ofs;
                break;
            }
        }
    }
    if(lpdf->dwFlags & DIDF_ABSAXIS) b_DJInputAbs = TRUE;
    if(lpdf->dwFlags & DIDF_RELAXIS) b_DJInputAbs = FALSE;
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::SetEventNotification(HANDLE h) {
    OutTraceIN("%s::SetEventNotification: h=%#x\n", s_ClassName, h);
    utrap("SetEventNotification");
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::SetCooperativeLevel(HWND hwnd, DWORD dwFlags) {
    OutTraceIN("%s::SetCooperativeLevel(I): hwnd=%#x flags=%#x(%s)\n",
               s_ClassName, hwnd, dwFlags, ExplainDICooperativeFlags(dwFlags));
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::GetObjectInfo(LPVOID lpidoi, DWORD dwObj, DWORD dwHow) {
    if(IsAscii)
        return (*(IFakeDirectInputDeviceA *)this).GetObjectInfo((LPDIDEVICEOBJECTINSTANCEA)lpidoi, dwObj, dwHow);
    else
        return (*(IFakeDirectInputDeviceW *)this).GetObjectInfo((LPDIDEVICEOBJECTINSTANCEW)lpidoi, dwObj, dwHow);
}

HRESULT IFakeDirectInputDeviceA::GetObjectInfo(LPDIDEVICEOBJECTINSTANCEA lpidoi, DWORD dwObj, DWORD dwHow) {
    OutTraceIN("%s::GetObjectInfo: obj=%#x how=%#x(%s)\n",
               s_ClassName, dwObj, dwHow, sHow(dwHow));
    // common settings
    memset(lpidoi, 0, sizeof(DIDEVICEOBJECTINSTANCEA));
    lpidoi->dwSize = sizeof(DIDEVICEOBJECTINSTANCEA);
    switch(dwHow) {
    case DIPH_BYOFFSET:
        lpidoi->dwOfs = dwObj;
        lpidoi->dwFlags = 0;
        if((dwObj >= DIJOFS_BUTTON(0)) && (dwObj <= DIJOFS_BUTTON(31))) {
            lpidoi->guidType = GUID_Button;
            lpidoi->dwType = DIDFT_BUTTON | DIDFT_MAKEINSTANCE(dwObj - DIJOFS_BUTTON(0));
            // buttons start at offset DIJOFS_BUTTON(0) and have size 1 byte
            // here we enumerate DIJOFS_BUTTON(0) as "Button(1) and so forth
            sprintf_s(lpidoi->tszName, 260, "Button(%d)", dwObj - DIJOFS_BUTTON(0) + 1);
            OutTraceIN("> Button(%d)\n", dwObj - DIJOFS_BUTTON(0) + 1);
        } else if((dwObj >= DIJOFS_POV(0)) && (dwObj <= DIJOFS_POV(3))) {
            lpidoi->guidType = GUID_POV;
            lpidoi->dwType = DIDFT_POV | DIDFT_MAKEINSTANCE((dwObj - DIJOFS_BUTTON(0)) / 4);
            // POV start at offset DIJOFS_POV(0) and have size 4 bytes
            // here we enumerate DIJOFS_POV(0) as "Pov(1) and so forth
            sprintf_s(lpidoi->tszName, 260, "Pov(%d)", (dwObj - DIJOFS_POV(0)) / 4 + 1);
            OutTraceIN("> Pov(%d)\n", (dwObj - DIJOFS_BUTTON(0)) / 4 + 1);
        } else {
            lpidoi->dwType = DIDFT_ABSAXIS;
            lpidoi->dwFlags = DIDOI_ASPECTPOSITION | DIDOI_POLLED;
            switch(dwObj) {
            case DIJOFS_X:
                lpidoi->guidType = GUID_XAxis;
                lpidoi->dwType = DIDFT_MAKEINSTANCE(0) | (b_DJInputAbs ? DIDFT_ABSAXIS : DIDF_RELAXIS);
                strcpy_s(lpidoi->tszName, 260, "X-Axis");
                OutTraceIN("> X-Axis\n");
                break;
            case DIJOFS_Y:
                lpidoi->guidType = GUID_YAxis;
                lpidoi->dwType = DIDFT_MAKEINSTANCE(1) | (b_DJInputAbs ? DIDFT_ABSAXIS : DIDF_RELAXIS);
                strcpy_s(lpidoi->tszName, 260, "Y-Axis");
                OutTraceIN("> Y-Axis\n");
                break;
            //case DIJOFS_Z:
            //	lpidoi->guidType = GUID_ZAxis;
            //	lpidoi->dwType = DIDFT_MAKEINSTANCE(2) | (b_DJInputAbs ? DIDFT_ABSAXIS : DIDF_RELAXIS);
            //	strcpy_s(lpidoi->tszName, 260, "Z-Axis");
            //	OutTraceIN("> Z-Axis\n");
            //	break;
            default:
                OutTraceIN("%s::GetObjectInfo: return DIERR_OBJECTNOTFOUND\n", s_ClassName);
                return DIERR_OBJECTNOTFOUND;
                break;
            }
        }
        break;
    case DIPH_BYID:
    case DIPH_BYUSAGE:
    default:
        OutTraceE("%s::GetObjectInfo: ERROR unknown how=%#x(%s) ret=\n", s_ClassName, dwHow, sHow(dwHow), DIERR_INVALIDPARAM);
        return DIERR_INVALIDPARAM;
        break;
    }
    return DI_OK;
}

HRESULT IFakeDirectInputDeviceW::GetObjectInfo(LPDIDEVICEOBJECTINSTANCEW lpidoi, DWORD dwObj, DWORD dwHow) {
    OutTraceIN("%s::GetObjectInfo: obj=%#x how=%#x(%s)\n",
               s_ClassName, dwObj, dwHow, sHow(dwHow));
    // common settings
    memset(lpidoi, 0, sizeof(DIDEVICEOBJECTINSTANCEW));
    lpidoi->dwSize = sizeof(DIDEVICEOBJECTINSTANCEW);
    switch(dwHow) {
    case DIPH_BYOFFSET:
        lpidoi->dwOfs = dwObj;
        lpidoi->dwFlags = 0;
        if((dwObj >= DIJOFS_BUTTON(0)) && (dwObj <= DIJOFS_BUTTON(31))) {
            lpidoi->guidType = GUID_Button;
            lpidoi->dwType = DIDFT_BUTTON | DIDFT_MAKEINSTANCE(dwObj - DIJOFS_BUTTON(0));
            // buttons start at offset DIJOFS_BUTTON(0) and have size 1 byte
            // here we enumerate DIJOFS_BUTTON(0) as "Button(1) and so forth
            swprintf(lpidoi->tszName, 260, L"Button(%d)", dwObj - DIJOFS_BUTTON(0) + 1);
            OutTraceIN("> Button(%d)\n", dwObj - DIJOFS_BUTTON(0) + 1);
        } else if((dwObj >= DIJOFS_POV(0)) && (dwObj <= DIJOFS_POV(3))) {
            lpidoi->guidType = GUID_POV;
            lpidoi->dwType = DIDFT_POV | DIDFT_MAKEINSTANCE((dwObj - DIJOFS_BUTTON(0)) / 4);
            // POV start at offset DIJOFS_POV(0) and have size 4 bytes
            // here we enumerate DIJOFS_POV(0) as "Pov(1) and so forth
            swprintf_s(lpidoi->tszName, 260, L"Pov(%d)", (dwObj - DIJOFS_POV(0)) / 4 + 1);
            OutTraceIN("> Pov(%d)\n", (dwObj - DIJOFS_BUTTON(0)) / 4 + 1);
        } else {
            switch(dwObj) {
            case DIJOFS_X:
                lpidoi->guidType = GUID_XAxis;
                lpidoi->dwType = DIDFT_MAKEINSTANCE(0) | (b_DJInputAbs ? DIDFT_ABSAXIS : DIDF_RELAXIS);
                wcscpy_s(lpidoi->tszName, 260, L"X-Axis");
                OutTraceIN("> X-Axis\n");
                break;
            case DIJOFS_Y:
                lpidoi->guidType = GUID_YAxis;
                lpidoi->dwType = DIDFT_MAKEINSTANCE(1) | (b_DJInputAbs ? DIDFT_ABSAXIS : DIDF_RELAXIS);
                wcscpy_s(lpidoi->tszName, 260, L"Y-Axis");
                OutTraceIN("> Y-Axis\n");
                break;
            //case DIJOFS_Z:
            //	lpidoi->guidType = GUID_ZAxis;
            //	lpidoi->dwType = DIDFT_MAKEINSTANCE(2) | (b_DJInputAbs ? DIDFT_ABSAXIS : DIDF_RELAXIS);
            //	wcscpy_s(lpidoi->tszName, 260, L"Z-Axis");
            //	OutTraceIN("> Z-Axis\n");
            //	break;
            default:
                OutTraceIN("%s::GetObjectInfo: return DIERR_OBJECTNOTFOUND\n", s_ClassName);
                return DIERR_OBJECTNOTFOUND;
                break;
            }
        }
        break;
    case DIPH_BYID:
    case DIPH_BYUSAGE:
    default:
        OutTraceE("%s::GetObjectInfo: ERROR unknown how=%#x(%s) ret=\n", s_ClassName, dwHow, sHow(dwHow), DIERR_INVALIDPARAM);
        return DIERR_INVALIDPARAM;
        break;
    }
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::GetDeviceInfo(LPVOID lpidi) {
    if(IsAscii)
        return (*(IFakeDirectInputDeviceA *)this).GetDeviceInfo((LPDIDEVICEINSTANCEA)lpidi);
    else
        return (*(IFakeDirectInputDeviceW *)this).GetDeviceInfo((LPDIDEVICEINSTANCEW)lpidi);
}

HRESULT IFakeDirectInputDeviceA::GetDeviceInfo(LPDIDEVICEINSTANCEA lpidi) {
    OutTraceIN("%s::GetDeviceInfo: size=%d\n", s_ClassName, lpidi->dwSize);
    OutTraceIN("%s::GetDeviceInfo: sizeof(DIDEVICEINSTANCEA)=%d\n", s_ClassName, sizeof(DIDEVICEINSTANCEA));
    if(lpidi->dwSize != sizeof(DIDEVICEINSTANCEA)) {
        OutTraceE("%s::GetDeviceInfo: bad size ret=DIERR_NOTINITIALIZED\n", s_ClassName);
        return DIERR_NOTINITIALIZED;
    }
    memcpy((void *) & (lpidi->guidInstance), (void *)&IFakeJoyInstance, sizeof(GUID));
    memcpy((void *) & (lpidi->guidProduct),  (void *)&IFakeJoyProduct,  sizeof(GUID));
    lpidi->dwDevType = DIDEVTYPE_HID | (1 << 8) | (IVersion == 8 ? DI8DEVTYPE_JOYSTICK : DIDEVTYPE_JOYSTICK);
    memcpy(lpidi->tszInstanceName, "DxWnd Joystick 1", sizeof(lpidi->tszInstanceName));
    memcpy(lpidi->tszProductName, "DxWnd Virtual Joystick", sizeof(lpidi->tszProductName));
    memcpy((void *) & (lpidi->guidFFDriver), (void *)&IFakeJoyFFDriver, sizeof(GUID));
    lpidi->wUsagePage = 1; // vJoy alike
    lpidi->wUsage = 4; // vJoy alike
    OutTraceIN("%s::GetDeviceInfo: ret=DI_OK\n", s_ClassName);
    return DI_OK;
}

HRESULT IFakeDirectInputDeviceW::GetDeviceInfo(LPDIDEVICEINSTANCEW lpidi) {
    OutTraceIN("%s::GetDeviceInfo: size=%d\n", s_ClassName, lpidi->dwSize);
    OutTraceIN("%s::GetDeviceInfo: sizeof(DIDEVICEINSTANCEW)=%d\n", s_ClassName, sizeof(DIDEVICEINSTANCEW));
    if(lpidi->dwSize != sizeof(DIDEVICEINSTANCEW)) {
        OutTraceE("%s::GetDeviceInfo: bad size ret=DIERR_NOTINITIALIZED\n", s_ClassName);
        return DIERR_NOTINITIALIZED;
    }
    memcpy((void *) & (lpidi->guidInstance), (void *)&IFakeJoyInstance, sizeof(GUID));
    memcpy((void *) & (lpidi->guidProduct),  (void *)&IFakeJoyProduct,  sizeof(GUID));
    lpidi->dwDevType = DIDEVTYPE_HID | (1 << 8) | (IVersion == 8 ? DI8DEVTYPE_JOYSTICK : DIDEVTYPE_JOYSTICK);
    memcpy(lpidi->tszInstanceName, L"DxWnd Joystick 1", sizeof(lpidi->tszInstanceName));
    memcpy(lpidi->tszProductName, L"DxWnd Virtual Joystick", sizeof(lpidi->tszProductName));
    memcpy((void *) & (lpidi->guidFFDriver), (void *)&IFakeJoyFFDriver, sizeof(GUID));
    lpidi->wUsagePage = 1; // vJoy alike
    lpidi->wUsage = 4; // vJoy alike
    OutTraceIN("%s::GetDeviceInfo: ret=DI_OK\n", s_ClassName);
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::RunControlPanel(HWND h, DWORD dw) {
    OutTraceE("%s::RunControlPanel\n", s_ClassName);
    utrap("RunControlPanel");
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::Initialize(HINSTANCE hi, DWORD dw1, REFGUID ref1) {
    OutTraceIN("%s::Initialize\n", s_ClassName);
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::CreateEffect(REFGUID giod, LPCDIEFFECT lpe, LPDIRECTINPUTEFFECT *lpdie, LPUNKNOWN lpu) {
    OutTraceIN("%s::CreateEffect: ret=DIERR_DEVICEFULL\n", s_ClassName);
    *lpdie = NULL;
    return DIERR_DEVICEFULL;
}

HRESULT IFakeDirectInputDevice::EnumEffects(LPVOID lpe, LPVOID lpv1, DWORD lpw) {
    if(IsAscii)
        return (*(IFakeDirectInputDeviceA *)this).EnumEffects((LPDIENUMEFFECTSCALLBACKA) lpe, lpv1, lpw);
    else
        return (*(IFakeDirectInputDeviceW *)this).EnumEffects((LPDIENUMEFFECTSCALLBACKW) lpe, lpv1, lpw);
}

HRESULT IFakeDirectInputDeviceA::EnumEffects(LPDIENUMEFFECTSCALLBACKA lpe, LPVOID lpv1, DWORD lpw) {
    OutTraceIN("%s::EnumEffects: ret=DI_OK\n", s_ClassName);
    return DI_OK;
}

HRESULT IFakeDirectInputDeviceW::EnumEffects(LPDIENUMEFFECTSCALLBACKW lpe, LPVOID lpv1, DWORD lpw) {
    OutTraceIN("%s::EnumEffects: ret=DI_OK\n", s_ClassName);
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::GetEffectInfo(LPVOID lpei, REFGUID guid) {
    if(IsAscii)
        return (*(IFakeDirectInputDeviceA *)this).GetEffectInfo((LPDIEFFECTINFOA) lpei, guid);
    else
        return (*(IFakeDirectInputDeviceW *)this).GetEffectInfo((LPDIEFFECTINFOW) lpei, guid);
}

HRESULT IFakeDirectInputDeviceA::GetEffectInfo(LPDIEFFECTINFOA lpei, REFGUID guid) {
    OutTraceIN("%s::GetEffectInfo: ret=DIERR_INVALIDPARAM\n", s_ClassName);
    return DIERR_INVALIDPARAM;
}

HRESULT IFakeDirectInputDeviceW::GetEffectInfo(LPDIEFFECTINFOW lpei, REFGUID guid) {
    OutTraceIN("%s::GetEffectInfo: ret=DIERR_INVALIDPARAM\n", s_ClassName);
    return DIERR_INVALIDPARAM;
}

HRESULT IFakeDirectInputDevice::GetForceFeedbackState(LPDWORD lpw) {
    OutTraceIN("%s::GetForceFeedbackState\n", s_ClassName);
    return DIERR_INVALIDPARAM;
}

HRESULT IFakeDirectInputDevice::SendForceFeedbackCommand(DWORD lpw) {
    OutTraceIN("%s::SendForceFeedbackCommand\n", s_ClassName);
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpcb, LPVOID lpv, DWORD dw) {
    OutTraceIN("%s::EnumCreatedEffectObjects\n", s_ClassName);
    utrap("EnumCreatedEffectObjects");
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::Escape(LPDIEFFESCAPE lpe) {
    OutTraceIN("%s::Escape\n", s_ClassName);
    utrap("Escape");
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::Poll(void) {
    if(!b_Acquired) {
        OutDebugIN("%s::Poll NOEFFECT\n", s_ClassName);
        return DI_NOEFFECT; // already unacquired
    }
    OutDebugIN("%s::Poll OK\n", s_ClassName);
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::SendDeviceData(DWORD dw1, LPCDIDEVICEOBJECTDATA lp1, LPDWORD lpdw, DWORD dw2) {
    OutTraceIN("%s::SendDeviceData\n", s_ClassName);
    utrap("SendDeviceData");
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::EnumEffectsInFile(LPCSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags) {
    OutTraceIN("%s::EnumEffectsInFile\n", s_ClassName);
    utrap("EnumEffectsInFile");
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::WriteEffectToFile(LPCSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags) {
    OutTraceIN("%s::WriteEffectToFile\n", s_ClassName);
    utrap("WriteEffectToFile");
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::BuildActionMap(LPDIACTIONFORMATA lpdiaf, LPCSTR lpszUserName, DWORD dwFlags) {
    OutTraceIN("%s::BuildActionMap\n", s_ClassName);
    utrap("BuildActionMap");
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::SetActionMap(LPDIACTIONFORMATA lpdiaf, LPCSTR lpszUserName, DWORD dwFlags) {
    OutTraceIN("%s::SetActionMap\n", s_ClassName);
    utrap("SetActionMap");
    return DI_OK;
}

HRESULT IFakeDirectInputDevice::GetImageInfo(LPDIDEVICEIMAGEINFOHEADERA lpdiDevImageInfoHeader) {
    OutTraceIN("%s::GetImageInfo\n", s_ClassName);
    utrap("GetImageInfo");
    return DI_OK;
}

void IFakeDirectInputDevice::GetJoyInfo(char *method) {
    JOYINFO jinfo;
    bActive = vjGetJoy(method, &jinfo);
    if(b_DJInputAbs) {
        if(bActive) {
            l_ValX = l_MinXRange + (jinfo.wXpos * (l_MaxXRange - l_MinXRange) / DXWJOY_XSPAN);
            l_ValY = l_MinYRange + (jinfo.wYpos * (l_MaxYRange - l_MinYRange) / DXWJOY_YSPAN);
            if(l_ValX > l_MaxXRange) l_ValX = l_MaxXRange;
            if(l_ValX < l_MinXRange) l_ValX = l_MinXRange;
            if(l_ValY > l_MaxYRange) l_ValY = l_MaxYRange;
            if(l_ValY < l_MinYRange) l_ValY = l_MinYRange;
            wButtons = jinfo.wButtons;
        } else {
            l_ValX = (l_MaxXRange + l_MinXRange) / 2;
            l_ValY = (l_MaxYRange + l_MinYRange) / 2;
            wButtons = 0;
        }
    } else {
        static BOOL bInitRelative = FALSE;
        if(bActive) {
            static long lastX = 0;
            static long lastY = 0;
            long curX, curY, spawnX, spawnY;
            curX = jinfo.wXpos * (l_MaxXRange - l_MinXRange) / DXWJOY_XSPAN;
            curY = jinfo.wYpos * (l_MaxYRange - l_MinYRange) / DXWJOY_YSPAN;
            if(!bInitRelative) {
                // first time only !
                lastX = curX;
                lastY = curY;
                bInitRelative = TRUE;
            }
            l_ValX = curX - lastX;
            l_ValY = curY - lastY;
            lastX = curX;
            lastY = curY;
            // avoid rel jumps greater that 1/8 of max spawn
            spawnX = (l_MaxXRange - l_MinXRange) >> 3;
            spawnY = (l_MaxYRange - l_MinYRange) >> 3;
            if (l_ValX > spawnX) l_ValX = spawnX;
            if (l_ValY > spawnY) l_ValY = spawnY;
            wButtons = jinfo.wButtons;
        } else {
            // disabling joystick resets the relative calculations
            bInitRelative = FALSE;
            l_ValX = l_ValY = 0;
            wButtons = 0;
        }
    }
    OutDebugIN("%s: active=%#x abs=%#x val(x,y)=(%d,%d) buttons=%c:%c:%c\n",
               method, bActive, b_DJInputAbs, l_ValX, l_ValY,
               wButtons & JOY_BUTTON1 ? 'X' : '0',
               wButtons & JOY_BUTTON2 ? 'X' : '0',
               wButtons & JOY_BUTTON3 ? 'X' : '0'
              );
}
