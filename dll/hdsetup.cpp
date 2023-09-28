/*
    DirectXSetupGetVersion Function

    Retrieves the version number of the Microsoft?DirectX?components that are currently installed.

    Syntax

    INT DirectXSetupGetVersion(

        DWORD *pdwVersion,
        DWORD *pdwRevision
    );

    Parameters

    pdwVersion
        Pointer to the address of a variable to receive the version number. This parameter can be NULL if the version number is not wanted.
    pdwRevision
        Pointer to the address of a variable to receive the revision number. This parameter can be NULL if the revision number is not wanted.

    Return Value

    If the function succeeds, it returns nonzero.

    If it fails, it returns 0.


    Remarks

    The DirectXSetupGetVersion function can be used to retrieve the version and revision numbers before or after the DirectXSetup function is called. If it is called before DirectXSetup is invoked, it gives the version and revision numbers of the DirectX components that are currently installed. If it is called after DirectXSetup is called, but before the computer is restarted, it gives the version and revision numbers of the DirectX components that take effect after the computer is restarted.

    The version number in the pdwVersion parameter is composed of the major version number and the minor version number. The major version number is in the 16 most significant bits of the DWORD when this function returns. The minor version number is in the 16 least significant bits of the DWORD when this function returns. The version numbers can be interpreted as follows:

    DirectX version	Value pointed to by pdwVersion
    DirectX 5.0	0x00040005
    DirectX 6.0	0x00040006
    DirectX 7.0	0x00040007
    DirectX 8.x	0x00040008
    DirectX 9.0	0x00040009

    DirectX 9.0 supports only operating systems and DirectX versions later than Microsoft Windows?98 and DirectX 5.0. This method will not return a value less than 0x00040005 for pdwVersion.

    The version number in the pdwRevision parameter is composed of the release number and the build number. The release number is in the 16 most significant bits of the DWORD when this function returns. The build number is in the 16 least significant bits of the DWORD when this function returns.

    All DirectX 8.x releases have the same pdwVersion value. The pdwRevision parameter differentiates them. DirectX 8.x versions do not install on Windows XP. Windows XP shipped with DirectX 8.1 and the DirectX 8.1a and DirectX 8.1b updates were included in Windows XP Service Pack 1. The following table lists the pdwRevision values for DirectX 8.x.
    DirectX release	pdwRevision	Notes
    DirectX 8.0
    DirectX 8.1	0x0001032A	4.08.01.0810. Windows XP version
    DirectX 8.1	0x00010371	4.08.01.0881. This is the DirectX 8.1 version for the down level operating systems. 0371 is the hex version for 0881 decimal revision value
    DirectX 8.1a	0x00010385	4.08.01.0901. This release includes an update to Microsoft Direct3D?(d3d8.dll).
    DirectX 8.1b	0x00010385	4.08.01.0901. This update includes a fix to Microsoft DirectShow?on Windows 2000 (quartz.dll)
    DirectX 8.2	0x00020386	4.08.02.0134. This version includes Microsoft DirectPlay?updates for both Windows XP and downlevel operating systems.
    Note  There is no way to tell the difference between DirectX 8.1a and DirectX 8.1b with the pdwVersion and pdwRevision values. DirectX 8.1a updates only D3d8.dll. DirectX 8.1b includes the changes in DirectX 8.1a as well as updates to other files. Use GetFileVersionInfo to get version information for the following files.

    On Windows 2000:
    Quartz.dll	6.03.01.0885
    Stream.sys	5.01.2600.0885
    Mpg2splt.ax	6.03.01.0885
    D3d8.dll	5.01.2600.0901
    D3d8d.dll	5.01.2600.0901

    On Windows 95 or later:
    Mpg2splt.ax	6.03.01.0885
    D3d8.dll	4.08.01.0901
    D3d8d.dll	4.08.01.0901

    The following sample code fragment demonstrates how the information returned by DirectXSetupGetVersion can be extracted and used.

    DWORD dwVersion;
    DWORD dwRevision;
    if (DirectXSetupGetVersion(&dwVersion, &dwRevision))
    {
        printf("DirectX version is %d.%d.%d.%d\n",
               HIWORD(dwVersion), LOWORD(dwVersion),
               HIWORD(dwRevision), LOWORD(dwRevision));
    }

    Version and revision numbers can be concatenated into a 64-bit quantity for comparison. The version number is in the 32 most significant bits and the revision number is in the 32 least significant bits.

    Function Information

    Header	dsetup.h
    Import library	dsetup.lib
    Minimum operating systems	Windows 98

    See Also

    DirectXSetup
*/

#define  _CRT_SECURE_NO_WARNINGS
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhook.h"
#include "dxhelper.h"
#include "dsetup/dsetup.h"

//#define TRACEALL

typedef INT (WINAPI *DirectXSetupA_Type)(HWND, LPSTR, DWORD);
typedef INT (WINAPI *DirectXSetupW_Type)(HWND, LPWSTR, DWORD);
typedef INT (WINAPI *DirectXSetupGetVersion_Type)(DWORD *, DWORD *);

DirectXSetupA_Type pDirectXSetupA;
DirectXSetupW_Type pDirectXSetupW;
DirectXSetupGetVersion_Type pDirectXSetupGetVersion;

INT WINAPI extDirectXSetupA(HWND, LPSTR, DWORD);
INT WINAPI extDirectXSetupW(HWND, LPWSTR, DWORD);
INT WINAPI extDirectXSetupGetVersion(DWORD *, DWORD *);

static HookEntryEx_Type DSetupHooks[] = {
#ifdef TRACEALL
    {HOOK_IAT_CANDIDATE, 0x0005, "DirectXSetupA", (FARPROC)NULL, (FARPROC *) &pDirectXSetupA, (FARPROC)extDirectXSetupA},
    {HOOK_IAT_CANDIDATE, 0x0008, "DirectXSetupW", (FARPROC)NULL, (FARPROC *) &pDirectXSetupW, (FARPROC)extDirectXSetupW},
#endif
    {HOOK_IAT_CANDIDATE, 0x000B, "DirectXSetupGetVersion", (FARPROC)NULL, (FARPROC *) &pDirectXSetupGetVersion, (FARPROC)extDirectXSetupGetVersion},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static char *libname = "dsetup.dll";

void HookDSetup(HMODULE module) {
    HookLibraryEx(module, DSetupHooks, libname);
}

FARPROC Remap_DSetup_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if (addr = RemapLibraryEx(proc, hModule, DSetupHooks)) return addr;
    return NULL;
}

// ------ wrappers ------

INT WINAPI extDirectXSetupGetVersion(DWORD *lpdwVersion, DWORD *lpdwMinorVersion) {
    INT ret;
    int ver = 90;
    ApiName("DirectXSetupGetVersion");
    OutTraceDDRAW("%s\n", ApiRef);
#ifdef TRACEALL
    {
        DWORD dwVersion, dwMinorVersion;
        ret = (*pDirectXSetupGetVersion)(&dwVersion, &dwMinorVersion);
        OutTraceDDRAW("%s: REAL ver(major:minor)=(%#x:%#x) ret=%#x\n",
                      ApiRef, dwVersion, dwMinorVersion, ret);
    }
#endif
    ret = (*pDirectXSetupGetVersion)(lpdwVersion, lpdwMinorVersion);
    OutTraceDDRAW("%s: ver(major:minor)=(%#x:%#x) ret=%#x\n",
                  ApiRef,
                  lpdwVersion ? *lpdwVersion : 0,
                  lpdwMinorVersion ? *lpdwMinorVersion : 0,
                  ret);
    if(!ret) {
        switch (ver) {
        case 82:
            if (lpdwVersion) *lpdwVersion = 0x00040008; // DirectX 8.0
            if (lpdwMinorVersion) *lpdwMinorVersion = 0x00020386; // DirectX 8.2
            break;
        case 90:
            if (lpdwVersion) *lpdwVersion = 0x00040009; // DirectX 9.0
            if (lpdwMinorVersion) *lpdwMinorVersion = 0x388 ; // Found on my DirectX 12.0
            break;
        }
        ret = TRUE;
        OutTraceDW("%s: forcing ver(major:minor)=(%#x:%#x) ret=TRUE\n",
                   ApiRef,
                   lpdwVersion ? *lpdwVersion : 0,
                   lpdwMinorVersion ? *lpdwMinorVersion : 0
                  );
    }
    return ret;
}

#ifdef TRACEALL
INT WINAPI extDirectXSetupA(HWND hWnd, LPSTR lpszRootPath, DWORD dwFlags) {
    INT ret;
    ApiName("DirectXSetupA");
    OutTraceDDRAW("%s: hwnd=%#x rootpath=\"%s\" flags=%#x\n", ApiRef, hWnd, lpszRootPath, dwFlags);
    ret = (*pDirectXSetupA)(hWnd, lpszRootPath, dwFlags);
    OutTraceDDRAW("%s: ret=%#x\n", ApiRef, ret);
    //MessageBox(NULL, ApiRef, "DxWnd", 0);
    return ret;
}

INT WINAPI extDirectXSetupW(HWND hWnd, LPWSTR lpszRootPath, DWORD dwFlags) {
    INT ret;
    ApiName("DirectXSetupW");
    OutTraceDDRAW("%s: hwnd=%#x rootpath=\"%ls\" flags=%#x\n", ApiRef, hWnd, lpszRootPath, dwFlags);
    ret = (*pDirectXSetupW)(hWnd, lpszRootPath, dwFlags);
    OutTraceDDRAW("%s: ret=%#x\n", ApiRef, ret);
    //MessageBox(NULL, ApiRef, "DxWnd", 0);
    return ret;
}
#endif

