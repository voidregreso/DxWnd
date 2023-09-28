#include <windows.h>
#include <stdio.h>
#include "dxwnd.h"

typedef int (*StartHook_)();
typedef int (*EndHook_)();
typedef void (*HookInit_)(TARGETMAP *, HWND);

typedef int (*SetTarget_)(DXWNDSTATUS *, TARGETMAP *);

BOOL DxWndEnabled = FALSE;
char DxWndIniPath[] = ".\\dxwnd.dxw";

static TARGETMAP target;
static DXWNDSTATUS status;
static HMODULE hModule;

static int GetIntEntry(char *tag, int default) {
    return GetPrivateProfileInt("target", tag, default, DxWndIniPath);
}

void InitDxWnd(char *name) {
    char DxWndPath[MAX_PATH + 1];
    char TargetPath[MAX_PATH + 1];
    // let's use a file creation CREATE_NEW to syncronize with other linked dlls.
    // The file is hidden (CREATE_NEW) and automatically deleted when the program
    // terminates (FILE_FLAG_DELETE_ON_CLOSE). No I/O operation is performed on it.
    HANDLE hLock = CreateFile(
                       ".\\dxwnd.lockfile.tmp",
                       GENERIC_WRITE,
                       0, //FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                       0,
                       CREATE_NEW,
                       FILE_ATTRIBUTE_HIDDEN | FILE_FLAG_DELETE_ON_CLOSE,
                       0);
    if((hLock == 0) || (hLock == INVALID_HANDLE_VALUE)) return;
    GetPrivateProfileString("system", "dxwndpath", ".", DxWndPath, MAX_PATH, DxWndIniPath);
    //MessageBox(0, DxWndPath, "dxwnd", MB_ICONERROR);
    // if DxWndPath is equal to "." (the default value) that means we don't have a dxwnd.ini
    // file in current folder. We use this to tell it is a INJECTPROXY mode and simply load
    // "dxwnd.dll" with no absolute path since the father DxWnd.exe in this case provided
    // to update the %Path% environment string with dxwnd.dll folder.
    if(!strcmp(DxWndPath, ".")) {
        hModule = LoadLibraryA("dxwnd.dll");
        if (!hModule) {
            char sMessage[1024];
            sprintf_s(sMessage, 1024,
                      "Proxy cannot load dxwnd.dll library\n"
                      "name=%s error=%d",
                      name, GetLastError());
            MessageBox(0, sMessage, "dxwnd", MB_ICONERROR);
            return;
        }
    }
    sprintf_s(DxWndPath, 260, "%s\\dxwnd.dll", DxWndPath);
    //MessageBox(0, DxWndPath, "dxwnd", MB_ICONERROR);
    hModule = LoadLibraryA(DxWndPath);
    //hModule = LoadLibraryA("dxwnd.dll");
    if (hModule) {
        StartHook_ startHook = (StartHook_)GetProcAddress(hModule, "StartHook");
        SetTarget_ setTarget = (SetTarget_)GetProcAddress(hModule, "SetTarget");
        HookInit_ hookInit = (HookInit_)GetProcAddress(hModule, "HookInit");
        if (startHook && setTarget) {
            // target.path must be set to current task pathname.
            GetModuleFileNameA(NULL, target.path, MAX_PATH);
            // v2.04.79: skip hooking if target path doesn't match, but using wildcards too.
            GetPrivateProfileStringA("target", "path0", "", TargetPath, MAX_PATH, DxWndIniPath);
            if(strlen(TargetPath) > 0) {
                if(TargetPath[0] == '*') {
                    // handle wildcard
                    int iNameLength = strlen(TargetPath) - 1;
                    if(_stricmp(&target.path[strlen(target.path) - iNameLength], &TargetPath[1])) return;
                } else {
                    // handle full path
                    if(_stricmp(TargetPath, target.path)) return;
                }
            }
            // v2.05.04: beware - index == -1 becomes the criteria to tell the proxy mode
            // and therefore extract PRIVATEMAP data from dxwnd.dxw instead of dxwnd.ini !
            target.index		= -1;
            target.FakeVersionId = GetIntEntry("winver0", 0);
            target.flags		= GetIntEntry("flag0", MODIFYMOUSE | USERGB565);
            target.flags2		= GetIntEntry("flagg0", WINDOWIZE | SETCOMPATIBILITY);
            target.flags3		= GetIntEntry("flagh0", HOOKDLLS | HOOKENABLED);
            target.flags4		= GetIntEntry("flagi0", SUPPORTSVGA);
            target.flags5		= GetIntEntry("flagj0", AEROBOOST | CENTERTOWIN);
            target.flags6		= GetIntEntry("flagk0", 0);
            target.flags7		= GetIntEntry("flagl0", 0);
            target.flags8		= GetIntEntry("flagm0", 0);
            target.flags9		= GetIntEntry("flagn0", 0);
            target.flags10		= GetIntEntry("flago0", 0);
            target.flags11		= GetIntEntry("flagp0", 0);
            target.flags12		= GetIntEntry("flagq0", 0);
            target.flags13		= GetIntEntry("flagr0", 0);
            target.flags14		= GetIntEntry("flags0", 0);
            target.flags15		= GetIntEntry("flagt0", 0);
            target.flags16		= GetIntEntry("flagu0", 0);
            target.tflags		= GetIntEntry("tflag0", 0);
            target.dflags		= GetIntEntry("dflag0", 0);
            target.dflags2		= GetIntEntry("dflagb0", 0);
            target.posx			= GetIntEntry("posx0", 0);
            target.posy			= GetIntEntry("posy0", 0);
            target.sizx			= GetIntEntry("sizx0", 800);
            target.sizy			= GetIntEntry("sizy0", 600);
            target.coordinates	= GetIntEntry("coord0", 1); // centered to desktop
            target.MaxFPS		= GetIntEntry("maxfps0", 0);
            target.dxversion	= GetIntEntry("ver0", 0);
            if(target.dxversion > 1) target.dxversion += 5;
            target.FakeVersionId = GetIntEntry("winver0", 0);
            target.MaxScreenRes = GetIntEntry("maxres0", -1);
            target.SwapEffect	= GetIntEntry("swapeffect0", 0);
            target.InitTS		= GetIntEntry("initts0", 0);
            target.monitorid	= GetIntEntry("monitorid0", -1);
            target.resw			= GetIntEntry("initresw0", 800);
            target.resh			= GetIntEntry("initresh0", 600);
            target.MaxDdrawInterface = GetIntEntry("maxddinterface0", 7);
            target.ScanLine		= GetIntEntry("scanline0", 0);
            target.SlowRatio	= GetIntEntry("slowratio0", 2);
            // v2.04.38 additions
            // n.b. FilterXScalingFactor & FilterYScalingFactor are set by the dll
            target.FilterId		= GetIntEntry("filterid0", 0);
            // v2.04.39/40 additions
            target.RendererId	= GetIntEntry("renderer0", 3);
            // OpenGLLib
            // module
            // fake drives
            char sBuf[60 + 1];
            GetPrivateProfileStringA("target", "fakehddrive0", "C:", sBuf, 10, DxWndIniPath);
            target.FakeHDDrive = sBuf[0];
            GetPrivateProfileStringA("target", "fakecddrive0", "C:", sBuf, 10, DxWndIniPath);
            target.FakeCDDrive = sBuf[0];
            target.FixedVolume = GetIntEntry("cdvol0", 100);
            // others ...
            GetPrivateProfileStringA("target", "module0", "", sBuf, 60, DxWndIniPath);
            strncpy_s(target.module, sBuf, 60);
            GetPrivateProfileStringA("target", "opengllib0", "", sBuf, 20, DxWndIniPath);
            strncpy_s(target.OpenGLLib, sBuf, 20);
            target.Country = GetIntEntry("country0", 0);
            target.CodePage = GetIntEntry("codepage0", 0);
            target.Language = GetIntEntry("lang0", 0);
            memset(&status, 0, sizeof(status));
            status.Status = DXW_RUNNING;
            status.TaskIdx = 0;
            status.hWnd = NULL;
            status.ColorDepth = 0;
            status.Height = status.Width = 0;
            status.DXVersion = 0;
            status.AllowMultiTask = 0;
            status.TimeShift = target.InitTS; // to initialize time shift properly ... ??
            if(target.flags6 & VIRTUALJOYSTICK) {
                status.VJoyStatus = GetPrivateProfileInt("joystick", "status", VJOYPRESENT | VJOYENABLED | CROSSENABLED, DxWndIniPath);
                status.VJoyDeadZone = GetPrivateProfileInt("joystick", "deadzone", 0, DxWndIniPath);
                status.VJoySensivity = GetPrivateProfileInt("joystick", "sensitivity", 100, DxWndIniPath);
                status.VJoySaturation = GetPrivateProfileInt("joystick", "saturation", 0, DxWndIniPath);
            }
            setTarget(&status, &target);
            //MessageBox(0,"SetTarget ok", "", 0);
            startHook();
            //MessageBox(0,"startHook ok", "", 0);
            hookInit(&target, NULL);
            //MessageBox(0,"HookInit ok", "", 0);
        }
    } else
        MessageBox(0, "Cannot load dxwnd.dll library", "dxwnd", MB_ICONERROR);
}

void DxWndEndHook(void) {
    if (hModule) {
        EndHook_ endHook = (EndHook_)GetProcAddress(hModule, "EndHook");
        if (endHook) endHook();
    }
}
