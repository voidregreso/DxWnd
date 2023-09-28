#include <windows.h>
#include <dinput.h>

#ifndef DIDEVTYPE_MOUSE
#define DIDEVTYPE_DEVICE        1
#define DIDEVTYPE_MOUSE         2
#define DIDEVTYPE_KEYBOARD      3
#define DIDEVTYPE_JOYSTICK      4
#endif

#ifndef DI8DEVCLASS_ALL
#define DI8DEVCLASS_ALL             0
#define DI8DEVCLASS_DEVICE          1
#define DI8DEVCLASS_POINTER         2
#define DI8DEVCLASS_KEYBOARD        3
#define DI8DEVCLASS_GAMECTRL        4
#define DI8DEVTYPE_DEVICE           0x11
#define DI8DEVTYPE_MOUSE            0x12
#define DI8DEVTYPE_KEYBOARD         0x13
#define DI8DEVTYPE_JOYSTICK         0x14
#define DI8DEVTYPE_GAMEPAD          0x15
#define DI8DEVTYPE_DRIVING          0x16
#define DI8DEVTYPE_FLIGHT           0x17
#define DI8DEVTYPE_1STPERSON        0x18
#define DI8DEVTYPE_DEVICECTRL       0x19
#define DI8DEVTYPE_SCREENPOINTER    0x1A
#define DI8DEVTYPE_REMOTE           0x1B
#define DI8DEVTYPE_SUPPLEMENTAL     0x1C
#endif

interface IFakeDirectInputDevice: IUnknown {
protected:
    // Reference count
    long m_lRef;
    long l_MaxXRange;
    long l_MaxYRange;
    long l_MinXRange;
    long l_MinYRange;
    long l_DeadZone;
    long l_Saturation;
    BOOL b_AutoCenter;
    BOOL b_Acquired;
    BOOL b_DJInputAbs;
    BOOL b_HasHatSwitch;
    char *s_ClassName;
    bool IsAscii;
    long l_ValX, l_ValY;
    BOOL bActive;
    DWORD wButtons;
    DWORD ofs_xaxis, ofs_yaxis;
    DWORD ofs_btnL, ofs_btnR, ofs_btnC, ofs_pov;

public:
    IFakeDirectInputDevice();
    //~IFakeDirectInputDeviceA(void);
    /*** IUnknown methods ***/
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv);
    virtual ULONG   STDMETHODCALLTYPE AddRef(void);
    virtual ULONG   STDMETHODCALLTYPE Release(void);

    /*** IDirectInputDeviceA methods ***/
    virtual HRESULT STDMETHODCALLTYPE GetCapabilities(LPDIDEVCAPS);
    virtual HRESULT STDMETHODCALLTYPE EnumObjects(LPVOID, LPVOID, DWORD);
    virtual HRESULT STDMETHODCALLTYPE GetProperty(REFGUID, LPDIPROPHEADER);
    virtual HRESULT STDMETHODCALLTYPE SetProperty(REFGUID, LPCDIPROPHEADER);
    virtual HRESULT STDMETHODCALLTYPE Acquire(THIS);
    virtual HRESULT STDMETHODCALLTYPE Unacquire(THIS);
    virtual HRESULT STDMETHODCALLTYPE GetDeviceState(DWORD, LPVOID);
    virtual HRESULT STDMETHODCALLTYPE GetDeviceData(DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD);
    virtual HRESULT STDMETHODCALLTYPE SetDataFormat(LPCDIDATAFORMAT);
    virtual HRESULT STDMETHODCALLTYPE SetEventNotification(HANDLE);
    virtual HRESULT STDMETHODCALLTYPE SetCooperativeLevel(HWND, DWORD);
    virtual HRESULT STDMETHODCALLTYPE GetObjectInfo(LPVOID, DWORD, DWORD);
    virtual HRESULT STDMETHODCALLTYPE GetDeviceInfo(LPVOID);
    virtual HRESULT STDMETHODCALLTYPE RunControlPanel(HWND, DWORD);
    virtual HRESULT STDMETHODCALLTYPE Initialize(HINSTANCE, DWORD, REFGUID);

    /*** IDirectInputDevice2A methods ***/
    virtual HRESULT STDMETHODCALLTYPE CreateEffect(REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT *, LPUNKNOWN);
    virtual HRESULT STDMETHODCALLTYPE EnumEffects(LPVOID, LPVOID, DWORD);
    virtual HRESULT STDMETHODCALLTYPE GetEffectInfo(LPVOID, REFGUID);
    virtual HRESULT STDMETHODCALLTYPE GetForceFeedbackState(LPDWORD);
    virtual HRESULT STDMETHODCALLTYPE SendForceFeedbackCommand(DWORD);
    virtual HRESULT STDMETHODCALLTYPE EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, LPVOID, DWORD);
    virtual HRESULT STDMETHODCALLTYPE Escape(LPDIEFFESCAPE);
    virtual HRESULT STDMETHODCALLTYPE Poll(void);
    virtual HRESULT STDMETHODCALLTYPE SendDeviceData(DWORD, LPCDIDEVICEOBJECTDATA, LPDWORD, DWORD);

    /*** IDirectInputDevice7A methods ***/
    virtual HRESULT STDMETHODCALLTYPE EnumEffectsInFile(LPCSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD);
    virtual HRESULT STDMETHODCALLTYPE WriteEffectToFile(LPCSTR, DWORD, LPDIFILEEFFECT, DWORD);
    /*** IDirectInputDevice8A methods ***/
    virtual HRESULT STDMETHODCALLTYPE BuildActionMap(LPDIACTIONFORMATA lpdiaf, LPCSTR lpszUserName, DWORD dwFlags);
    virtual HRESULT STDMETHODCALLTYPE SetActionMap(LPDIACTIONFORMATA lpdiaf, LPCSTR lpszUserName, DWORD dwFlags);
    virtual HRESULT STDMETHODCALLTYPE GetImageInfo(LPDIDEVICEIMAGEINFOHEADERA lpdiDevImageInfoHeader);

    int IVersion;

private:
    void GetJoyInfo(char *);
};

interface IFakeDirectInputDeviceA: IFakeDirectInputDevice {
public:
    IFakeDirectInputDeviceA();

    /*** IDirectInputDeviceA methods ***/
    HRESULT STDMETHODCALLTYPE EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA, LPVOID, DWORD);
    HRESULT STDMETHODCALLTYPE GetObjectInfo(LPDIDEVICEOBJECTINSTANCEA, DWORD, DWORD);
    HRESULT STDMETHODCALLTYPE GetDeviceInfo(LPDIDEVICEINSTANCEA);

    /*** IDirectInputDevice2A methods ***/
    HRESULT STDMETHODCALLTYPE EnumEffects(LPDIENUMEFFECTSCALLBACKA, LPVOID, DWORD);
    HRESULT STDMETHODCALLTYPE GetEffectInfo(LPDIEFFECTINFOA, REFGUID);
};

interface IFakeDirectInputDeviceW: IFakeDirectInputDevice {
public:
    IFakeDirectInputDeviceW();

    /*** IDirectInputDeviceW methods ***/
    HRESULT STDMETHODCALLTYPE EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKW, LPVOID, DWORD);
    HRESULT STDMETHODCALLTYPE GetObjectInfo(LPDIDEVICEOBJECTINSTANCEW, DWORD, DWORD);
    HRESULT STDMETHODCALLTYPE GetDeviceInfo(LPDIDEVICEINSTANCEW);

    /*** IDirectInputDevice2W methods ***/
    HRESULT STDMETHODCALLTYPE EnumEffects(LPDIENUMEFFECTSCALLBACKW, LPVOID, DWORD);
    HRESULT STDMETHODCALLTYPE GetEffectInfo(LPDIEFFECTINFOW, REFGUID);
};

// Fake Virtual Joystick GUID
#ifdef GUID_ALLOCATE
GUID IFakeJoyFFDriver = {0xBADBED0A, 0x1C30, 0x0000, 0x0};
GUID IFakeJoyInstance = {0xBADBED0B, 0x1C30, 0x0000, 0x0};
GUID IFakeJoyProduct  = {0xBADBED0C, 0x1C30, 0x0000, 0x0};
#else
extern GUID IFakeJoyFFDriver;
extern GUID IFakeJoyInstance;
extern GUID IFakeJoyProduct;
#endif

