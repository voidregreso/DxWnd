#define  _CRT_SECURE_NO_WARNINGS
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhook.h"
#include "dxhelper.h"
#include "dxwlocale.h"
#include "hddraw.h"
#include "stdio.h"
#include "shlwapi.h"
// v2.05.20: added for DeviceIOControl interpretation about CDROM devices
#include "ntddcdrm.h"
#include "ntddscsi.h"

#ifndef DXW_NOTRACES
#define TraceError() OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError())
#define IfTraceError() if(!res) TraceError()
#else
#define TraceError()
#define IfTraceError()
#endif

// LOCKINJECTIONTHREADS enables the source code necessary to manage the Debug FREEZEINJECTEDSON option!
//#define LOCKINJECTIONTHREADS
#define TRYFATNAMES TRUE
//#define TRACEALL
//#define MONITORTHREADS

#ifdef TRACEALL
#define TRACEIO
#define MONITORTHREADS
#endif

extern HRESULT WINAPI extDirectDrawEnumerateA(LPDDENUMCALLBACK, LPVOID);
extern HRESULT WINAPI extDirectDrawEnumerateExA(LPDDENUMCALLBACKEX, LPVOID, DWORD);
extern void ReplaceCPUID(char *);
extern DirectDrawEnumerateA_Type pDirectDrawEnumerateA;
extern DirectDrawEnumerateExA_Type pDirectDrawEnumerateExA;
extern void HookModule(HMODULE);
extern void HookDlls(HMODULE);

typedef DWORD (WINAPI *WaitForSingleObject_Type)(HANDLE, DWORD);
DWORD WINAPI extWaitForSingleObject(HANDLE, DWORD);
WaitForSingleObject_Type pWaitForSingleObject;

#ifdef TRACEALL
typedef DWORD (WINAPI *GetFullPathNameA_Type)(LPCSTR, DWORD, LPSTR, LPSTR *);
DWORD WINAPI extGetFullPathNameA(LPCSTR, DWORD, LPSTR, LPSTR *);
typedef BOOL (WINAPI *WriteFile_Type)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
BOOL WINAPI extWriteFile(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
LPVOID WINAPI extGlobalLock(HGLOBAL);
HGLOBAL WINAPI extGlobalAlloc(UINT, SIZE_T);
HGLOBAL WINAPI extGlobalReAlloc(HGLOBAL, SIZE_T, UINT);
HGLOBAL WINAPI extGlobalFree(HGLOBAL);
BOOL WINAPI extGlobalMemoryStatusEx(LPMEMORYSTATUSEX);
BOOL WINAPI extGetPhysicallyInstalledSystemMemory(PULONGLONG);
typedef LPVOID (WINAPI *GlobalLock_Type)(HGLOBAL);
typedef HGLOBAL (WINAPI *GlobalAlloc_Type)(UINT, SIZE_T);
typedef HGLOBAL (WINAPI *GlobalReAlloc_Type)(HGLOBAL, SIZE_T, UINT);
typedef HGLOBAL (WINAPI *GlobalFree_Type)(HGLOBAL);
typedef BOOL (WINAPI *GlobalMemoryStatusEx_Type)(LPMEMORYSTATUSEX);
typedef BOOL (WINAPI *GetPhysicallyInstalledSystemMemory_Type)(PULONGLONG);
GlobalLock_Type pGlobalLock = NULL;
GlobalAlloc_Type pGlobalAlloc = NULL;
GlobalReAlloc_Type pGlobalReAlloc = NULL;
GlobalFree_Type pGlobalFree = NULL;
GetFullPathNameA_Type pGetFullPathNameA = NULL;
GlobalMemoryStatusEx_Type pGlobalMemoryStatusEx = NULL;
GetPhysicallyInstalledSystemMemory_Type pGetPhysicallyInstalledSystemMemory = NULL;
WriteFile_Type pWriteFile = NULL;
#endif

#ifdef MONITORTHREADS
HANDLE WINAPI extCreateThread(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
typedef HANDLE (WINAPI *CreateThread_Type)(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
CreateThread_Type pCreateThread = NULL;
#endif // MONITORTHREADS

typedef UINT (WINAPI *GetWindowsDirectoryA_Type)(LPSTR, UINT);
GetWindowsDirectoryA_Type pGetWindowsDirectoryA = NULL;
UINT WINAPI extGetWindowsDirectoryA(LPSTR, UINT);
typedef UINT (WINAPI *GetWindowsDirectoryW_Type)(LPWSTR, UINT);
GetWindowsDirectoryW_Type pGetWindowsDirectoryW = NULL;
UINT WINAPI extGetWindowsDirectoryW(LPWSTR, UINT);
typedef DWORD (WINAPI *GetPrivateProfileStringA_Type)(LPCSTR, LPCSTR, LPCSTR, LPSTR, DWORD, LPCSTR);
GetPrivateProfileStringA_Type pGetPrivateProfileStringA = NULL;
DWORD WINAPI extGetPrivateProfileStringA(LPCSTR, LPCSTR, LPCSTR, LPSTR, DWORD, LPCSTR);
typedef DWORD (WINAPI *GetPrivateProfileStringW_Type)(LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR, DWORD, LPCWSTR);
GetPrivateProfileStringW_Type pGetPrivateProfileStringW = NULL;
DWORD WINAPI extGetPrivateProfileStringW(LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR, DWORD, LPCWSTR);
typedef DWORD (WINAPI *WritePrivateProfileStringA_Type)(LPCSTR, LPCSTR, LPSTR, LPCSTR);
WritePrivateProfileStringA_Type pWritePrivateProfileStringA = NULL;
DWORD WINAPI extWritePrivateProfileStringA(LPCSTR, LPCSTR, LPSTR, LPCSTR);
typedef DWORD (WINAPI *GetProfileStringA_Type)(LPCSTR, LPCSTR, LPCSTR, LPSTR, DWORD);
GetProfileStringA_Type pGetProfileStringA = NULL;
DWORD WINAPI extGetProfileStringA(LPCSTR, LPCSTR, LPCSTR, LPSTR, DWORD);
typedef DWORD (WINAPI *WriteProfileStringA_Type)(LPCSTR, LPCSTR, LPSTR);
WriteProfileStringA_Type pWriteProfileStringA = NULL;
DWORD WINAPI extWriteProfileStringA(LPCSTR, LPCSTR, LPSTR);
typedef UINT (WINAPI *GetPrivateProfileIntA_Type)(LPCTSTR, LPCTSTR, INT, LPCTSTR);
GetPrivateProfileIntA_Type pGetPrivateProfileIntA = NULL;
UINT WINAPI extGetPrivateProfileIntA(LPCTSTR, LPCTSTR, INT, LPCTSTR);
typedef UINT (WINAPI *GetPrivateProfileIntW_Type)(LPCWSTR, LPCWSTR, INT, LPCWSTR);
GetPrivateProfileIntW_Type pGetPrivateProfileIntW = NULL;
UINT WINAPI extGetPrivateProfileIntW(LPCWSTR, LPCWSTR, INT, LPCWSTR);
typedef UINT (WINAPI *GetProfileIntA_Type)(LPCTSTR, LPCTSTR, INT);
GetProfileIntA_Type pGetProfileIntA = NULL;
UINT WINAPI extGetProfileIntA(LPCTSTR, LPCTSTR, INT);
typedef UINT (WINAPI *GetProfileIntW_Type)(LPCWSTR, LPCWSTR, INT);
GetProfileIntW_Type pGetProfileIntW = NULL;
UINT WINAPI extGetProfileIntW(LPCWSTR, LPCWSTR, INT);
typedef BOOL (WINAPI *DeviceIoControl_Type)(HANDLE, DWORD, LPVOID, DWORD, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
DeviceIoControl_Type pDeviceIoControl = NULL;
BOOL WINAPI extDeviceIoControl(HANDLE, DWORD, LPVOID, DWORD, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
typedef BOOL (WINAPI *GetStringTypeA_Type)(LCID, DWORD, LPCSTR, int, LPDWORD);
GetStringTypeA_Type pGetStringTypeA = NULL;
BOOL WINAPI extGetStringTypeA(LCID, DWORD, LPCSTR, int, LPDWORD);
typedef int (WINAPI *LCMapStringA_Type)(LCID, DWORD, LPCSTR, int, LPSTR, int);
LCMapStringA_Type pLCMapStringA = NULL;
int WINAPI extLCMapStringA(LCID, DWORD, LPCSTR, int, LPSTR, int);

typedef LCID (WINAPI *GetThreadLocale_Type)();
GetThreadLocale_Type pGetThreadLocale = NULL;
LCID WINAPI extGetThreadLocale();
typedef BOOL (WINAPI *SetThreadLocale_Type)(LCID);
SetThreadLocale_Type pSetThreadLocale = NULL;
BOOL WINAPI extSetThreadLocale(LCID);
//typedef BOOL (WINAPI *IsDBCSLeadByte_Type)(BYTE);
BOOL WINAPI extIsDBCSLeadByte(BYTE);
UINT WINAPI extGetACP();
UINT WINAPI extGetOEMCP();
typedef int (WINAPI *MultiByteToWideChar_Type)(UINT, DWORD, LPCSTR, int, LPWSTR, int);
MultiByteToWideChar_Type pMultiByteToWideChar = NULL;
int WINAPI extMultiByteToWideChar(UINT, DWORD, LPCSTR, int, LPWSTR, int);
typedef int (WINAPI *WideCharToMultiByte_Type)(UINT, DWORD, LPCWSTR, int, LPSTR, int, LPCSTR, LPBOOL);
WideCharToMultiByte_Type pWideCharToMultiByte = NULL;
int WINAPI extWideCharToMultiByte(UINT, DWORD, LPCWSTR, int, LPSTR, int, LPCSTR, LPBOOL);
typedef HRSRC (WINAPI *FindResourceExA_Type)(HMODULE, LPCSTR, LPCSTR, WORD);
FindResourceExA_Type pFindResourceExA = NULL;
HRSRC WINAPI extFindResourceExA(HMODULE, LPCSTR, LPCSTR, WORD);
LANGID WINAPI extGetUserDefaultUILanguage(void);
LANGID WINAPI extGetSystemDefaultUILanguage(void);
LANGID WINAPI extGetSystemDefaultLangID(void);
LANGID WINAPI extGetUserDefaultLangID(void);
LCID WINAPI extGetThreadLocale(void);
LCID WINAPI extGetSystemDefaultLCID(void);
LCID WINAPI extGetUserDefaultLCID(void);

// definitions for fake global atoms
typedef ATOM (WINAPI *GlobalDeleteAtom_Type)(ATOM);
typedef ATOM (WINAPI *GlobalAddAtomA_Type)(LPCSTR);
typedef ATOM (WINAPI *GlobalFindAtomA_Type)(LPCSTR);
GlobalDeleteAtom_Type pGlobalDeleteAtom = NULL;
GlobalAddAtomA_Type pGlobalAddAtomA = NULL;
GlobalFindAtomA_Type pGlobalFindAtomA = NULL;
ATOM WINAPI extGlobalDeleteAtom(ATOM);
ATOM WINAPI extGlobalAddAtomA(LPCSTR);
ATOM WINAPI extGlobalFindAtomA(LPCSTR);

typedef DWORD (WINAPI *GetCurrentDirectoryA_Type)(DWORD, LPSTR);
GetCurrentDirectoryA_Type pGetCurrentDirectoryA = NULL;
DWORD WINAPI extGetCurrentDirectoryA(DWORD, LPSTR);
typedef DWORD (WINAPI *GetCurrentDirectoryW_Type)(DWORD, LPWSTR);
GetCurrentDirectoryW_Type pGetCurrentDirectoryW = NULL;
DWORD WINAPI extGetCurrentDirectoryW(DWORD, LPWSTR);
typedef BOOL (WINAPI *CreateDirectoryA_Type)(LPCSTR, LPSECURITY_ATTRIBUTES);
CreateDirectoryA_Type pCreateDirectoryA = NULL;
BOOL WINAPI extCreateDirectoryA(LPCSTR, LPSECURITY_ATTRIBUTES);
typedef BOOL (WINAPI *RemoveDirectoryA_Type)(LPCSTR);
RemoveDirectoryA_Type pRemoveDirectoryA = NULL;
BOOL WINAPI extRemoveDirectoryA(LPCSTR);

// v2.02.96: the GetSystemInfo API is NOT hot patchable on Win7. This can cause problems because it can't be hooked by simply
// enabling hot patch. A solution is making all LoadLibrary* calls hot patchable, so that when loading the module, the call
// can be hooked by the IAT lookup. This fixes a problem after movie playing in Wind Fantasy SP.

static HookEntryEx_Type LauncherHooks[] = {
    {HOOK_HOT_REQUIRED, 0, "CreateProcessA", (FARPROC)CreateProcessA, (FARPROC *) &pCreateProcessA, (FARPROC)extCreateProcessA},
    {HOOK_HOT_REQUIRED, 0, "CreateProcessW", (FARPROC)CreateProcessW, (FARPROC *) &pCreateProcessW, (FARPROC)extCreateProcessW},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type Hooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "IsDebuggerPresent", (FARPROC)NULL, (FARPROC *)NULL, (FARPROC)extIsDebuggerPresent},
    {HOOK_IAT_CANDIDATE, 0, "CheckRemoteDebuggerPresent", (FARPROC)NULL, (FARPROC *)NULL, (FARPROC)extCheckRemoteDebuggerPresent},
    {HOOK_HOT_CANDIDATE, 0, "GetProcAddress", (FARPROC)GetProcAddress, (FARPROC *) &pGetProcAddress, (FARPROC)extGetProcAddress},
    {HOOK_HOT_CANDIDATE, 0, "LoadLibraryA", (FARPROC)LoadLibraryA, (FARPROC *) &pLoadLibraryA, (FARPROC)extLoadLibraryA},
    {HOOK_HOT_CANDIDATE, 0, "LoadLibraryExA", (FARPROC)LoadLibraryExA, (FARPROC *) &pLoadLibraryExA, (FARPROC)extLoadLibraryExA},
    {HOOK_HOT_CANDIDATE, 0, "LoadLibraryW", (FARPROC)LoadLibraryW, (FARPROC *) &pLoadLibraryW, (FARPROC)extLoadLibraryW},
    {HOOK_HOT_CANDIDATE, 0, "LoadLibraryExW", (FARPROC)LoadLibraryExW, (FARPROC *) &pLoadLibraryExW, (FARPROC)extLoadLibraryExW},
    {HOOK_IAT_CANDIDATE, 0, "GetDriveTypeA", (FARPROC)NULL, (FARPROC *) &pGetDriveTypeA, (FARPROC)extGetDriveTypeA},
    {HOOK_IAT_CANDIDATE, 0, "GetDriveTypeW", (FARPROC)NULL, (FARPROC *) &pGetDriveTypeW, (FARPROC)extGetDriveTypeW},
    {HOOK_IAT_CANDIDATE, 0, "GetLogicalDrives", (FARPROC)NULL, (FARPROC *) &pGetLogicalDrives, (FARPROC)extGetLogicalDrives},
    {HOOK_IAT_CANDIDATE, 0, "GetTempFileNameA", (FARPROC)GetTempFileNameA, (FARPROC *) &pGetTempFileNameA, (FARPROC)extGetTempFileNameA},
    {HOOK_IAT_CANDIDATE, 0, "GetTempFileNameW", (FARPROC)GetTempFileNameW, (FARPROC *) &pGetTempFileNameW, (FARPROC)extGetTempFileNameW},
    {HOOK_IAT_CANDIDATE, 0, "CreateProcessA", (FARPROC)NULL, (FARPROC *) &pCreateProcessA, (FARPROC)extCreateProcessA},
    {HOOK_IAT_CANDIDATE, 0, "CreateProcessW", (FARPROC)NULL, (FARPROC *) &pCreateProcessW, (FARPROC)extCreateProcessW},
    {HOOK_IAT_CANDIDATE, 0, "WinExec", (FARPROC)NULL, (FARPROC *) &pWinExec, (FARPROC)extWinExec},
    {HOOK_HOT_CANDIDATE, 0, "SetPriorityClass", (FARPROC)SetPriorityClass, (FARPROC *) &pSetPriorityClass, (FARPROC)extSetPriorityClass},
    {HOOK_HOT_CANDIDATE, 0, "GlobalUnlock", (FARPROC)GlobalUnlock, (FARPROC *) &pGlobalUnlock, (FARPROC)extGlobalUnlock},
    {HOOK_HOT_CANDIDATE, 0, "FreeLibrary", (FARPROC)FreeLibrary, (FARPROC *) &pFreeLibrary, (FARPROC)extFreeLibrary},
    {HOOK_HOT_CANDIDATE, 0, "IsProcessorFeaturePresent", (FARPROC)IsProcessorFeaturePresent, (FARPROC *) &pIsProcessorFeaturePresent, (FARPROC)extIsProcessorFeaturePresent},
#ifdef TRACEALL
    {HOOK_HOT_CANDIDATE, 0, "RaiseException", (FARPROC)RaiseException, (FARPROC *) &pRaiseException, (FARPROC)extRaiseException},
    {HOOK_HOT_CANDIDATE, 0, "GlobalLock", (FARPROC)GlobalLock, (FARPROC *) &pGlobalLock, (FARPROC)extGlobalLock},
    {HOOK_HOT_CANDIDATE, 0, "GlobalAlloc", (FARPROC)GlobalAlloc, (FARPROC *) &pGlobalAlloc, (FARPROC)extGlobalAlloc},
    {HOOK_HOT_CANDIDATE, 0, "GlobalReAlloc", (FARPROC)GlobalReAlloc, (FARPROC *) &pGlobalReAlloc, (FARPROC)extGlobalReAlloc},
    {HOOK_HOT_CANDIDATE, 0, "GetFullPathNameA", (FARPROC)GetFullPathNameA, (FARPROC *) &pGetFullPathNameA, (FARPROC)extGetFullPathNameA},
    {HOOK_HOT_CANDIDATE, 0, "GlobalMemoryStatusEx", (FARPROC)GlobalMemoryStatusEx, (FARPROC *) &pGlobalMemoryStatusEx, (FARPROC)extGlobalMemoryStatusEx},
    {HOOK_HOT_CANDIDATE, 0, "GetPhysicallyInstalledSystemMemory", (FARPROC)NULL, (FARPROC *) &pGetPhysicallyInstalledSystemMemory, (FARPROC)extGetPhysicallyInstalledSystemMemory},
#endif // TRACEALL
    {HOOK_HOT_CANDIDATE, 0, "DeviceIoControl", (FARPROC)DeviceIoControl, (FARPROC *) &pDeviceIoControl, (FARPROC)extDeviceIoControl},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type WaitHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "WaitForSingleObject", (FARPROC)WaitForSingleObject, (FARPROC *)pWaitForSingleObject, (FARPROC)extWaitForSingleObject},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type HeapHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "HeapCreate", (FARPROC)HeapCreate, (FARPROC *) &pHeapCreate, (FARPROC)extHeapCreate},
    {HOOK_IAT_CANDIDATE, 0, "HeapAlloc", (FARPROC)HeapAlloc, (FARPROC *) &pHeapAlloc, (FARPROC)extHeapAlloc},
    {HOOK_IAT_CANDIDATE, 0, "HeapReAlloc", (FARPROC)HeapReAlloc, (FARPROC *) &pHeapReAlloc, (FARPROC)extHeapReAlloc},
    {HOOK_IAT_CANDIDATE, 0, "HeapFree", (FARPROC)HeapFree, (FARPROC *) &pHeapFree, (FARPROC)extHeapFree},
    {HOOK_IAT_CANDIDATE, 0, "HeapValidate", (FARPROC)HeapFree, (FARPROC *) &pHeapFree, (FARPROC)extHeapFree},
    {HOOK_IAT_CANDIDATE, 0, "HeapCompact", (FARPROC)HeapCompact, (FARPROC *) &pHeapCompact, (FARPROC)extHeapCompact},
    {HOOK_IAT_CANDIDATE, 0, "HeapDestroy", (FARPROC)HeapDestroy, (FARPROC *) &pHeapDestroy, (FARPROC)extHeapDestroy},
    {HOOK_IAT_CANDIDATE, 0, "HeapSize", (FARPROC)HeapSize, (FARPROC *) &pHeapSize, (FARPROC)extHeapSize},
    {HOOK_IAT_CANDIDATE, 0, "GetProcessHeap", (FARPROC)GetProcessHeap, (FARPROC *) &pGetProcessHeap, (FARPROC)extGetProcessHeap},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type FixPathsHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "SetCurrentDirectoryA", (FARPROC)SetCurrentDirectoryA, (FARPROC *) &pSetCurrentDirectoryA, (FARPROC)extSetCurrentDirectoryA},
    {HOOK_HOT_CANDIDATE, 0, "SetCurrentDirectoryW", (FARPROC)SetCurrentDirectoryW, (FARPROC *) &pSetCurrentDirectoryW, (FARPROC)extSetCurrentDirectoryW},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type FixAllocHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "VirtualAlloc", (FARPROC)VirtualAlloc, (FARPROC *) &pVirtualAlloc, (FARPROC)extVirtualAlloc},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type FixIOHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "ReadFile", (FARPROC)NULL, (FARPROC *) &pReadFile, (FARPROC)extReadFile},
    {HOOK_IAT_CANDIDATE, 0, "CreateFileA", (FARPROC)NULL, (FARPROC *) &pCreateFileA, (FARPROC)extCreateFileA},
    {HOOK_IAT_CANDIDATE, 0, "CreateFileW", (FARPROC)NULL, (FARPROC *) &pCreateFileW, (FARPROC)extCreateFileW},
    {HOOK_IAT_CANDIDATE, 0, "CloseHandle", (FARPROC)NULL, (FARPROC *) &pCloseHandle, (FARPROC)extCloseHandle},
#ifdef TRACEALL
    {HOOK_IAT_CANDIDATE, 0, "SetFilePointer", (FARPROC)NULL, (FARPROC *) &pSetFilePointer, (FARPROC)extSetFilePointer},
    {HOOK_IAT_CANDIDATE, 0, "WriteFile", (FARPROC)NULL, (FARPROC *) &pWriteFile, (FARPROC)extWriteFile},
#endif
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type RemapIOHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "CreateFileA", (FARPROC)NULL, (FARPROC *) &pCreateFileA, (FARPROC)extCreateFileA},
    {HOOK_IAT_CANDIDATE, 0, "CreateFileW", (FARPROC)NULL, (FARPROC *) &pCreateFileW, (FARPROC)extCreateFileW},
    {HOOK_IAT_CANDIDATE, 0, "DeleteFileA", (FARPROC)NULL, (FARPROC *) &pDeleteFileA, (FARPROC)extDeleteFileA},
    {HOOK_IAT_CANDIDATE, 0, "CreateDirectoryA", (FARPROC)NULL, (FARPROC *) &pCreateDirectoryA, (FARPROC)extCreateDirectoryA},
    {HOOK_IAT_CANDIDATE, 0, "RemoveDirectoryA", (FARPROC)NULL, (FARPROC *) &pRemoveDirectoryA, (FARPROC)extRemoveDirectoryA},
    {HOOK_IAT_CANDIDATE, 0, "MoveFileA", (FARPROC)NULL, (FARPROC *) &pMoveFileA, (FARPROC)extMoveFileA},
    {HOOK_IAT_CANDIDATE, 0, "FindFirstFileA", (FARPROC)NULL, (FARPROC *) &pFindFirstFileA, (FARPROC)extFindFirstFileA},
    {HOOK_IAT_CANDIDATE, 0, "FindNextFileA", (FARPROC)NULL, (FARPROC *) &pFindNextFileA, (FARPROC)extFindNextFileA},
    {HOOK_IAT_CANDIDATE, 0, "GetFileAttributesA", (FARPROC)NULL, (FARPROC *) &pGetFileAttributesA, (FARPROC)extGetFileAttributesA},
    {HOOK_IAT_CANDIDATE, 0, "GetVolumeInformationA", (FARPROC)NULL, (FARPROC *) &pGetVolumeInformationA, (FARPROC)extGetVolumeInformationA},
    {HOOK_IAT_CANDIDATE, 0, "GetLogicalDriveStringsA", (FARPROC)NULL, (FARPROC *) &pGetLogicalDriveStringsA, (FARPROC)extGetLogicalDriveStringsA},
    {HOOK_IAT_CANDIDATE, 0, "GetLogicalDriveStringsW", (FARPROC)NULL, (FARPROC *) &pGetLogicalDriveStringsW, (FARPROC)extGetLogicalDriveStringsW},
    {HOOK_IAT_CANDIDATE, 0, "SetCurrentDirectoryA", (FARPROC)NULL, (FARPROC *) &pSetCurrentDirectoryA, (FARPROC)extSetCurrentDirectoryA},
    {HOOK_IAT_CANDIDATE, 0, "SetCurrentDirectoryW", (FARPROC)NULL, (FARPROC *) &pSetCurrentDirectoryW, (FARPROC)extSetCurrentDirectoryW},
    {HOOK_IAT_CANDIDATE, 0, "GetCurrentDirectoryA", (FARPROC)NULL, (FARPROC *) &pGetCurrentDirectoryA, (FARPROC)extGetCurrentDirectoryA},
    {HOOK_IAT_CANDIDATE, 0, "GetCurrentDirectoryW", (FARPROC)NULL, (FARPROC *) &pGetCurrentDirectoryW, (FARPROC)extGetCurrentDirectoryW},
    {HOOK_IAT_CANDIDATE, 0, "_lopen", (FARPROC)NULL, (FARPROC *) &p_lopen, (FARPROC)ext_lopen},
    {HOOK_IAT_CANDIDATE, 0, "_lcreat", (FARPROC)NULL, (FARPROC *) &p_lcreat, (FARPROC)ext_lcreat},
    {HOOK_IAT_CANDIDATE, 0, "OpenFile", (FARPROC)NULL, (FARPROC *) &pOpenFile, (FARPROC)extOpenFile},
    {HOOK_IAT_CANDIDATE, 0, "CreateFileMappingA", (FARPROC)NULL, (FARPROC *) &pCreateFileMappingA, (FARPROC)extCreateFileMappingA},
    {HOOK_HOT_CANDIDATE, 0, "GetDiskFreeSpaceA", (FARPROC)GetDiskFreeSpaceA, (FARPROC *) &pGetDiskFreeSpaceA, (FARPROC)extGetDiskFreeSpaceA},
    {HOOK_HOT_CANDIDATE, 0, "GetPrivateProfileStringA", (FARPROC)GetPrivateProfileStringA, (FARPROC *) &pGetPrivateProfileStringA, (FARPROC)extGetPrivateProfileStringA},
    {HOOK_HOT_CANDIDATE, 0, "GetPrivateProfileStringW", (FARPROC)GetPrivateProfileStringW, (FARPROC *) &pGetPrivateProfileStringW, (FARPROC)extGetPrivateProfileStringW},
    {HOOK_HOT_CANDIDATE, 0, "GetPrivateProfileIntA", (FARPROC)GetPrivateProfileIntA, (FARPROC *) &pGetPrivateProfileIntA, (FARPROC)extGetPrivateProfileIntA},
    {HOOK_HOT_CANDIDATE, 0, "GetPrivateProfileIntW", (FARPROC)GetPrivateProfileIntW, (FARPROC *) &pGetPrivateProfileIntW, (FARPROC)extGetPrivateProfileIntW},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type LimitHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "GetDiskFreeSpaceA", (FARPROC)GetDiskFreeSpaceA, (FARPROC *) &pGetDiskFreeSpaceA, (FARPROC)extGetDiskFreeSpaceA},
    {HOOK_HOT_CANDIDATE, 0, "GlobalMemoryStatus", (FARPROC)GlobalMemoryStatus, (FARPROC *) &pGlobalMemoryStatus, (FARPROC)extGlobalMemoryStatus},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type SysFolderHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "GetWindowsDirectoryA", (FARPROC)GetWindowsDirectoryA, (FARPROC *) &pGetWindowsDirectoryA, (FARPROC)extGetWindowsDirectoryA},
    {HOOK_HOT_CANDIDATE, 0, "GetWindowsDirectoryW", (FARPROC)GetWindowsDirectoryW, (FARPROC *) &pGetWindowsDirectoryW, (FARPROC)extGetWindowsDirectoryW},
    {HOOK_HOT_CANDIDATE, 0, "GetPrivateProfileStringA", (FARPROC)GetPrivateProfileStringA, (FARPROC *) &pGetPrivateProfileStringA, (FARPROC)extGetPrivateProfileStringA},
    {HOOK_HOT_CANDIDATE, 0, "GetPrivateProfileStringW", (FARPROC)GetPrivateProfileStringW, (FARPROC *) &pGetPrivateProfileStringW, (FARPROC)extGetPrivateProfileStringW},
    {HOOK_HOT_CANDIDATE, 0, "WritePrivateProfileStringA", (FARPROC)WritePrivateProfileStringA, (FARPROC *) &pWritePrivateProfileStringA, (FARPROC)extWritePrivateProfileStringA},
    {HOOK_HOT_CANDIDATE, 0, "GetProfileStringA", (FARPROC)GetProfileStringA, (FARPROC *) &pGetProfileStringA, (FARPROC)extGetProfileStringA},
    {HOOK_HOT_CANDIDATE, 0, "WriteProfileStringA", (FARPROC)WriteProfileStringA, (FARPROC *) &pWriteProfileStringA, (FARPROC)extWriteProfileStringA},
    {HOOK_HOT_CANDIDATE, 0, "GetPrivateProfileIntA", (FARPROC)GetPrivateProfileIntA, (FARPROC *) &pGetPrivateProfileIntA, (FARPROC)extGetPrivateProfileIntA},
    {HOOK_HOT_CANDIDATE, 0, "GetPrivateProfileIntW", (FARPROC)GetPrivateProfileIntW, (FARPROC *) &pGetPrivateProfileIntW, (FARPROC)extGetPrivateProfileIntW},
    {HOOK_HOT_CANDIDATE, 0, "GetProfileIntA", (FARPROC)GetProfileIntA, (FARPROC *) &pGetProfileIntA, (FARPROC)extGetProfileIntA},
    {HOOK_HOT_CANDIDATE, 0, "GetProfileIntW", (FARPROC)GetProfileIntW, (FARPROC *) &pGetProfileIntW, (FARPROC)extGetProfileIntW},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type TimeHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "GetTickCount", (FARPROC)GetTickCount, (FARPROC *) &pGetTickCount, (FARPROC)extGetTickCount},
    {HOOK_HOT_CANDIDATE, 0, "GetLocalTime", (FARPROC)GetLocalTime, (FARPROC *) &pGetLocalTime, (FARPROC)extGetLocalTime},
    {HOOK_HOT_CANDIDATE, 0, "GetSystemTime", (FARPROC)GetSystemTime, (FARPROC *) &pGetSystemTime, (FARPROC)extGetSystemTime},
    {HOOK_HOT_CANDIDATE, 0, "GetSystemTimeAsFileTime", (FARPROC)GetSystemTimeAsFileTime, (FARPROC *) &pGetSystemTimeAsFileTime, (FARPROC)extGetSystemTimeAsFileTime},
    {HOOK_HOT_CANDIDATE, 0, "Sleep", (FARPROC)Sleep, (FARPROC *) &pSleep, (FARPROC)extSleep},
    {HOOK_HOT_CANDIDATE, 0, "SleepEx", (FARPROC)SleepEx, (FARPROC *) &pSleepEx, (FARPROC)extSleepEx},
    {HOOK_HOT_CANDIDATE, 0, "QueryPerformanceCounter", (FARPROC)QueryPerformanceCounter, (FARPROC *) &pQueryPerformanceCounter, (FARPROC)extQueryPerformanceCounter},
    {HOOK_HOT_CANDIDATE, 0, "QueryPerformanceFrequency", (FARPROC)QueryPerformanceFrequency, (FARPROC *) &pQueryPerformanceFrequency, (FARPROC)extQueryPerformanceFrequency},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type VersionHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "GetVersion", (FARPROC)GetVersion, (FARPROC *) &pGetVersion, (FARPROC)extGetVersion},
    {HOOK_HOT_CANDIDATE, 0, "GetVersionExA", (FARPROC)GetVersionExA, (FARPROC *) &pGetVersionExA, (FARPROC)extGetVersionExA},
    {HOOK_HOT_CANDIDATE, 0, "GetVersionExW", (FARPROC)GetVersionExW, (FARPROC *) &pGetVersionExW, (FARPROC)extGetVersionExW},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type OutDebHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "OutputDebugStringA", (FARPROC)OutputDebugStringA, (FARPROC *) &pOutputDebugStringA, (FARPROC)extOutputDebugStringA},
    {HOOK_HOT_CANDIDATE, 0, "OutputDebugStringW", (FARPROC)OutputDebugStringW, (FARPROC *) &pOutputDebugStringW, (FARPROC)extOutputDebugStringW},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type CritRegionHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "InitializeCriticalSection", (FARPROC)InitializeCriticalSection, (FARPROC *) &pInitializeCriticalSection, (FARPROC)extInitializeCriticalSection},
    {HOOK_IAT_CANDIDATE, 0, "EnterCriticalSection", (FARPROC)EnterCriticalSection, (FARPROC *) &pEnterCriticalSection, (FARPROC)extEnterCriticalSection},
    {HOOK_IAT_CANDIDATE, 0, "LeaveCriticalSection", (FARPROC)LeaveCriticalSection, (FARPROC *) &pLeaveCriticalSection, (FARPROC)extLeaveCriticalSection},
    {HOOK_IAT_CANDIDATE, 0, "DeleteCriticalSection", (FARPROC)DeleteCriticalSection, (FARPROC *) &pDeleteCriticalSection, (FARPROC)extDeleteCriticalSection},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type InternalHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "GetPrivateProfileStringA", (FARPROC)GetPrivateProfileStringA, (FARPROC *) &pGetPrivateProfileStringA, (FARPROC)extGetPrivateProfileStringA},
    {HOOK_HOT_CANDIDATE, 0, "GetPrivateProfileIntA", (FARPROC)GetPrivateProfileIntA, (FARPROC *) &pGetPrivateProfileIntA, (FARPROC)extGetPrivateProfileIntA},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type NlsHooks[] = {
    // IsDBCSLeadByte hook alone is enough to fix DiscStation's "Restaurant King" game ....
    {HOOK_HOT_REQUIRED, 0, "IsDBCSLeadByte", (FARPROC)IsDBCSLeadByte, (FARPROC *)NULL, (FARPROC)extIsDBCSLeadByte},
    {HOOK_HOT_REQUIRED, 0, "GetACP", (FARPROC)GetACP, (FARPROC *)NULL, (FARPROC)extGetACP},
    {HOOK_HOT_REQUIRED, 0, "GetOEMCP", (FARPROC)GetOEMCP, (FARPROC *)NULL, (FARPROC)extGetOEMCP},
    {HOOK_HOT_REQUIRED, 0, "MultiByteToWideChar", (FARPROC)MultiByteToWideChar, (FARPROC *) &pMultiByteToWideChar, (FARPROC)extMultiByteToWideChar},
    {HOOK_HOT_REQUIRED, 0, "WideCharToMultiByte", (FARPROC)WideCharToMultiByte, (FARPROC *) &pWideCharToMultiByte, (FARPROC)extWideCharToMultiByte},
    {HOOK_HOT_REQUIRED, 0, "FindResourceExA", (FARPROC)FindResourceExA, (FARPROC *) &pFindResourceExA, (FARPROC)extFindResourceExA},
    // added to ntlea hooked calls !!!
    {HOOK_HOT_REQUIRED, 0, "GetStringTypeA", (FARPROC)GetStringTypeA, (FARPROC *) &pGetStringTypeA, (FARPROC)extGetStringTypeA},
    {HOOK_HOT_REQUIRED, 0, "LCMapStringA", (FARPROC)LCMapStringA, (FARPROC *) &pLCMapStringA, (FARPROC)extLCMapStringA},

    // all these requiring no original ptr since they are supposed to return a fixed val (is it correct?)
    {HOOK_HOT_REQUIRED, 0, "GetUserDefaultUILanguage", (FARPROC)GetUserDefaultUILanguage, (FARPROC *)NULL, (FARPROC)extGetUserDefaultUILanguage},
    {HOOK_HOT_REQUIRED, 0, "GetSystemDefaultUILanguage", (FARPROC)GetSystemDefaultUILanguage, (FARPROC *)NULL, (FARPROC)extGetSystemDefaultUILanguage},
    {HOOK_HOT_REQUIRED, 0, "GetSystemDefaultLangID", (FARPROC)GetSystemDefaultLangID, (FARPROC *)NULL, (FARPROC)extGetSystemDefaultLangID},
    {HOOK_HOT_REQUIRED, 0, "GetUserDefaultLangID", (FARPROC)GetUserDefaultLangID, (FARPROC *)NULL, (FARPROC)extGetUserDefaultLangID},
    {HOOK_HOT_REQUIRED, 0, "GetThreadLocale", (FARPROC)GetThreadLocale, (FARPROC *)NULL, (FARPROC)extGetThreadLocale},
    {HOOK_HOT_REQUIRED, 0, "SetThreadLocale", (FARPROC)SetThreadLocale, (FARPROC *) &pSetThreadLocale, (FARPROC)extSetThreadLocale},
    {HOOK_HOT_REQUIRED, 0, "GetSystemDefaultLCID", (FARPROC)GetSystemDefaultLCID, (FARPROC *)NULL, (FARPROC)extGetSystemDefaultLCID},
    {HOOK_HOT_REQUIRED, 0, "GetUserDefaultLCID", (FARPROC)GetUserDefaultLCID, (FARPROC *)NULL, (FARPROC)extGetUserDefaultLCID},

    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type AtomHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "GlobalAddAtomA", (FARPROC)GlobalAddAtomA, (FARPROC *)pGlobalAddAtomA, (FARPROC)extGlobalAddAtomA},
    {HOOK_IAT_CANDIDATE, 0, "GlobalDeleteAtom", (FARPROC)GlobalDeleteAtom, (FARPROC *)pGlobalDeleteAtom, (FARPROC)extGlobalDeleteAtom},
    {HOOK_IAT_CANDIDATE, 0, "GlobalFindAtomA", (FARPROC)GlobalFindAtomA, (FARPROC *)pGlobalFindAtomA, (FARPROC)extGlobalFindAtomA},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

#ifdef MONITORTHREADS
static HookEntryEx_Type ThreadHooks[] = {
    {HOOK_HOT_REQUIRED, 0, "CreateThread", (FARPROC)CreateThread, (FARPROC *)pCreateThread, (FARPROC)extCreateThread},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};
#endif // MONITORTHREADS

static char *libname = "kernel32.dll";

void HookLauncher(HMODULE module) {
    HookLibraryEx(module, LauncherHooks, libname);
}

void HookKernel32(HMODULE module) {
    HookLibraryEx(module, Hooks, libname);
#ifdef TRACEIO
    HookLibraryEx(module, FixIOHooks, libname);
#else
    if(dxw.dwFlags3 & BUFFEREDIOFIX) HookLibraryEx(module, FixIOHooks, libname);
#endif // TRACEIO
    if(dxw.dwFlags2 & LIMITRESOURCES) HookLibraryEx(module, LimitHooks, libname);
    if(dxw.dwFlags2 & TIMESTRETCH) HookLibraryEx(module, TimeHooks, libname);
    if(dxw.dwFlags2 & FAKEVERSION) HookLibraryEx(module, VersionHooks, libname);
    if(dxw.dwFlags6 & LEGACYALLOC) HookLibraryEx(module, FixAllocHooks, libname);
    if(dxw.dwFlags8 & VIRTUALHEAP) HookLibraryEx(module, HeapHooks, libname);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) HookLibraryEx(module, RemapIOHooks, libname);
    if(dxw.dwFlags4 & IGNOREDEBOUTPUT) HookLibraryEx(module, OutDebHooks, libname);
    if(dxw.dwFlags10 & FIXFOLDERPATHS) HookLibraryEx(module, FixPathsHooks, libname);
    if(dxw.dwFlags11 & (MUTEX4CRITSECTION | DELAYCRITSECTION)) HookLibraryEx(module, CritRegionHooks, libname);
    if(dxw.dwFlags11 & REMAPSYSFOLDERS) HookLibraryEx(module, SysFolderHooks, libname);
    if(dxw.dwFlags11 & CUSTOMLOCALE) HookLibraryEx(module, NlsHooks, libname);
    if(dxw.dwFlags12 & FAKEGLOBALATOM) HookLibraryEx(module, AtomHooks, libname);
    if(dxw.dwFlags12 & KILLDEADLOCKS) HookLibraryEx(module, WaitHooks, libname);
#ifdef MONITORTHREADS
    HookLibraryEx(module, ThreadHooks, libname);
#endif // MONITORTHREADS
}

void HookKernel32Init() {
    HookLibInitEx(Hooks);
    HookLibInitEx(FixIOHooks);
    HookLibInitEx(LimitHooks);
    HookLibInitEx(TimeHooks);
    HookLibInitEx(VersionHooks);
    HookLibInitEx(FixAllocHooks);
    HookLibInitEx(HeapHooks);
    HookLibInitEx(InternalHooks); // for DxWnd internal use
}

FARPROC Remap_kernel32_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if (dxw.dwFlags4 & NOPERFCOUNTER) {
        if( !strcmp(proc, "QueryPerformanceCounter") ||
                !strcmp(proc, "QueryPerformanceFrequency")) {
            OutTraceDW("GetProcAddress: HIDING proc=%s\n", proc);
            return NULL;
        }
    }
    if (addr = RemapLibraryEx(proc, hModule, Hooks)) return addr;
#ifdef TRACEIO
    if (addr = RemapLibraryEx(proc, hModule, FixIOHooks)) return addr;
#else
    if(dxw.dwFlags3 & BUFFEREDIOFIX)
        if (addr = RemapLibraryEx(proc, hModule, FixIOHooks)) return addr;
#endif // TRACEIO
    if(dxw.dwFlags2 & LIMITRESOURCES)
        if (addr = RemapLibraryEx(proc, hModule, LimitHooks)) return addr;
    if(dxw.dwFlags2 & TIMESTRETCH)
        if (addr = RemapLibraryEx(proc, hModule, TimeHooks)) return addr;
    if(dxw.dwFlags2 & FAKEVERSION)
        if (addr = RemapLibraryEx(proc, hModule, VersionHooks)) return addr;
    if(dxw.dwFlags6 & LEGACYALLOC)
        if (addr = RemapLibraryEx(proc, hModule, FixAllocHooks)) return addr;
    if(dxw.dwFlags8 & VIRTUALHEAP)
        if (addr = RemapLibraryEx(proc, hModule, HeapHooks)) return addr;
    if(dxw.dwFlags4 & IGNOREDEBOUTPUT)
        if (addr = RemapLibraryEx(proc, hModule, OutDebHooks)) return addr;
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE))
        if (addr = RemapLibraryEx(proc, hModule, RemapIOHooks)) return addr;
    if(dxw.dwFlags10 & FIXFOLDERPATHS)
        if (addr = RemapLibraryEx(proc, hModule, FixPathsHooks)) return addr;
    if(dxw.dwFlags11 & (MUTEX4CRITSECTION | DELAYCRITSECTION))
        if (addr = RemapLibraryEx(proc, hModule, CritRegionHooks)) return addr;
    if(dxw.dwFlags11 & REMAPSYSFOLDERS)
        if (addr = RemapLibraryEx(proc, hModule, SysFolderHooks)) return addr;
    if(dxw.dwFlags12 & FAKEGLOBALATOM)
        if (addr = RemapLibraryEx(proc, hModule, AtomHooks)) return addr;
    if(dxw.dwFlags12 & KILLDEADLOCKS)
        if (addr = RemapLibraryEx(proc, hModule, WaitHooks)) return addr;
    return NULL;
}

/* ------------------------------------------------------------------------------ */
// auxiliary & static functions
/* ------------------------------------------------------------------------------ */

#define DXW_HD_PATH 0
#define DXW_CD_PATH 1

static void PeekHotKeys() {
    // peek/remove a WM_SYSKEYDOWN message to trigger the CD changer fkeys
    MSG msg;
    void HandleHotKeys(HWND, UINT, LPARAM, WPARAM);
    // v2.05.27: skip if PeekMessageA is unhooked
    if(pPeekMessageA && (*pPeekMessageA)(&msg, NULL, WM_SYSKEYDOWN, WM_SYSKEYDOWN, TRUE)) {
        OutTrace("GetFakeDriverPath: WM_SYSKEYDOWN event hwnd=%#x msg=%#x l/wparam=%#x/%#x\n",
                 msg.hwnd, msg.message, msg.lParam, msg.wParam);
        HandleHotKeys(msg.hwnd, msg.message, msg.lParam, msg.wParam);
    }
}

static char *dxwGetFakeDriverPath(char DriverId) {
    static BOOL FirstTimeThrough = TRUE;
    static char *pSuffix;
    static char *FakeHDPath = NULL;
    static char *FakeCDPath = NULL;
    char *ret;
    if(FirstTimeThrough) {
        char InitPath[MAX_PATH + 1];
        char key[12];
        int dwIndex;
        sprintf_s(InitPath, MAX_PATH, "%s\\dxwnd.%s", GetDxWndPath(), (dxw.dwIndex == -1) ? "dxw" : "ini");
        dwIndex = (dxw.dwIndex == -1) ? 0 : dxw.dwIndex;
        OutTrace("debug: InitPath=%s\n", InitPath);
        if(dxw.dwFlags10 & FAKEHDDRIVE) {
            FakeHDPath = (char *)malloc(MAX_PATH + 1);
            sprintf_s(key, sizeof(key), "fakehd%i", dwIndex);
            (*pGetPrivateProfileStringA)("target", key, NULL, FakeHDPath, MAX_PATH, InitPath);
            // default for no string is current folder
            if(strlen(FakeHDPath) == 0)
                GetCurrentDirectory(MAX_PATH, FakeHDPath);
            else {
                // in path, '?' in first position means current folder
                if(FakeHDPath[0] == '?') {
                    char sCurrentPath[MAX_PATH + 1];
                    char sTail[MAX_PATH + 1];
                    strcpy(sTail, &FakeHDPath[1]);
                    GetCurrentDirectory(MAX_PATH, sCurrentPath);
                    sprintf_s(FakeHDPath, MAX_PATH, "%s\\%s", sCurrentPath, sTail);
                }
            }
            OutTraceDW("GetFakeDriverPath: HD index=%d drive=%c: path=\"%s\"\n", dwIndex, dxw.FakeHDDrive, FakeHDPath);
        }
        if(dxw.dwFlags10 & FAKECDDRIVE) {
            FakeCDPath = (char *)malloc(MAX_PATH + 1);
            sprintf_s(key, sizeof(key), "fakecd%i", dwIndex);
            (*pGetPrivateProfileStringA)("target", key, NULL, FakeCDPath, MAX_PATH, InitPath);
            if(strlen(FakeCDPath) == 0)
                GetCurrentDirectory(MAX_PATH, FakeCDPath);
            else {
                // in path, '?' in first position means current folder
                if(FakeCDPath[0] == '?') {
                    char sTail[MAX_PATH + 1];
                    strcpy(sTail, &FakeCDPath[1]);
                    GetCurrentDirectory(MAX_PATH, FakeCDPath);
                    strncat(FakeCDPath, sTail, MAX_PATH);
                }
            }
            OutTraceDW("GetFakeDriverPath: CD index=%d drive=%c:  path=\"%s\"\n", dwIndex, dxw.FakeCDDrive, FakeCDPath);
            pSuffix = &FakeCDPath[strlen(FakeCDPath)];
            dxw.MaxCDVolume = 0;
            HMODULE hshlwapi = LoadLibrary("shlwapi.dll");
            if(hshlwapi) {
                typedef BOOL (WINAPI * PathFileExistsA_Type)(LPCSTR);
                PathFileExistsA_Type pPathFileExistsA = (PathFileExistsA_Type)(*pGetProcAddress)(hshlwapi, "PathFileExistsA");
                for(int i = 2; i < 10; i++) {
                    char sVolumePath[MAX_PATH + 1];
                    sprintf_s(sVolumePath, MAX_PATH, "%s%02d", FakeCDPath, i);
                    if((*pPathFileExistsA)(sVolumePath))
                        dxw.MaxCDVolume = i - 1;
                    else
                        break;
                }
                FreeLibrary(hshlwapi);
            }
            OutTraceDW("GetFakeDriverPath: CD last disk=CD%02d\n", dxw.MaxCDVolume + 1);
        }
        FirstTimeThrough = FALSE;
    }
    switch(DriverId) {
    case DXW_HD_PATH:
        ret = FakeHDPath;
        break;
    case DXW_CD_PATH:
        PeekHotKeys();
        if(dxw.DataCDIndex != GetHookInfo()->CDIndex) {
            dxw.DataCDIndex = GetHookInfo()->CDIndex;
            // avoid switching to non existing disks
            if(dxw.DataCDIndex > dxw.MaxCDVolume) {
                dxw.DataCDIndex = dxw.MaxCDVolume;
                GetHookInfo()->CDIndex = dxw.MaxCDVolume;
            }
            dxw.ShowCDChanger();
            if(dxw.DataCDIndex > 0)
                sprintf(pSuffix, "%02d\\", dxw.DataCDIndex + 1);
            else
                sprintf(pSuffix, "\\");
        }
        ret = FakeCDPath;
        break;
    }
    return ret;
}

// v2.05.53: initialization of folder type for games that are supposed to start from CD folder
// allows a DxWnd-RIP for "Return Fire"
DWORD InitFolderType(void) {
    char sFakePath[MAX_PATH + 1];
    char sWorkPath[MAX_PATH + 1];
    int MaxCount;
    if(!pGetCurrentDirectoryA) pGetCurrentDirectoryA = GetCurrentDirectoryA;
    (*pGetCurrentDirectoryA)(MAX_PATH, sWorkPath);
    for(UINT i = 0; i < strlen(sWorkPath); i++) sWorkPath[i] = toupper(sWorkPath[i]);
    OutTraceDW("InitFolderType: WorkPath=%s\n", sWorkPath);
    if(dxw.dwFlags10 & FAKECDDRIVE) {
        strcpy(sFakePath, dxwGetFakeDriverPath(DXW_CD_PATH));
        for(UINT i = 0; i < strlen(sFakePath); i++) sFakePath[i] = toupper(sFakePath[i]);
        MaxCount = strlen(sFakePath);
        if(!strncmp(sFakePath, sWorkPath, MaxCount)) {
            OutTraceDW("InitFolderType: matched FAKE_CD path=%s\n", sFakePath);
            return DXW_FAKE_CD;
        }
    }
    if(dxw.dwFlags10 & FAKEHDDRIVE) {
        strcpy(sFakePath, dxwGetFakeDriverPath(DXW_HD_PATH));
        for(UINT i = 0; i < strlen(sFakePath); i++) sFakePath[i] = toupper(sFakePath[i]);
        OutTraceDW("InitFolderType: fake HD path=%s\n", sFakePath);
        MaxCount = strlen(sFakePath);
        if(!strncmp(sFakePath, sWorkPath, MaxCount)) {
            OutTraceDW("InitFolderType: matched FAKE_HD path=%s\n", sFakePath);
            return DXW_FAKE_HD;
        }
    }
    OutTraceDW("InitFolderType: matched NO_FAKE\n");
    return DXW_NO_FAKE;
}

LPCSTR dxwTranslatePathA(LPCSTR lpFileName, DWORD *mapping) {
    static char sNewPath[MAX_PATH + 1];
    BOOL hasDriveLetter = FALSE;
    // v2.05.29: bug fix (exception when mapping==NULL & relative & FAKE)
    DWORD dwMapping;
    dwMapping = DXW_NO_FAKE;
    // safeguard
    if(lpFileName == NULL) return NULL;
    if(strlen(lpFileName) > 1)
        if(lpFileName[1] == ':') hasDriveLetter = TRUE;
    sNewPath[MAX_PATH] = 0;
    do { // fake loop
        // v2.04.84: find matching for drive name. Do not look for 3 chars (like "E:\") because
        // some games omit the backslash. For instance, "Need for Speed II" looks for "X:NFSW.EXE"
        // Also, take care that drive letters could be in lowercase
        if((dxw.dwFlags10 & FAKEHDDRIVE) && hasDriveLetter && ((char)toupper(lpFileName[0]) == dxw.FakeHDDrive)) {
            strncpy(sNewPath, dxwGetFakeDriverPath(DXW_HD_PATH), MAX_PATH);
            strncat(sNewPath, &lpFileName[2], MAX_PATH - strlen(sNewPath));
            lpFileName = sNewPath;
            dwMapping = DXW_FAKE_HD;
            OutTraceDW("TranslatePath: fake HD new FileName=\"%s\"\n", lpFileName);
            break;
        }
        if((dxw.dwFlags10 & FAKEHDDRIVE) && !strncmp(lpFileName, "\\\\.\\", 4)) {
            strncpy(sNewPath, dxwGetFakeDriverPath(DXW_HD_PATH), MAX_PATH);
            strncat(sNewPath, &lpFileName[3], MAX_PATH - strlen(sNewPath));
            lpFileName = sNewPath;
            dwMapping = DXW_FAKE_HD;
            OutTraceDW("TranslatePath: fake HD new FileName=\"%s\"\n", lpFileName);
            break;
        }
        if((dxw.dwFlags10 & FAKECDDRIVE) && hasDriveLetter && ((char)toupper(lpFileName[0]) == dxw.FakeCDDrive)) {
            strncpy(sNewPath, dxwGetFakeDriverPath(DXW_CD_PATH), MAX_PATH);
            strncat(sNewPath, &lpFileName[2], MAX_PATH - strlen(sNewPath));
            lpFileName = sNewPath;
            dwMapping = DXW_FAKE_CD;
            OutTraceDW("TranslatePath: fake CD new FileName=\"%s\"\n", lpFileName);
            break;
        }
        if((dxw.dwCurrentFolderType != DXW_NO_FAKE) && !hasDriveLetter) {
            sprintf(sNewPath, "%s\\%s",
                    dxwGetFakeDriverPath((dxw.dwCurrentFolderType == DXW_FAKE_HD) ? DXW_HD_PATH : DXW_CD_PATH),
                    lpFileName);
            lpFileName = sNewPath;
            dwMapping = dxw.dwCurrentFolderType;
            OutTraceDW("TranslatePath: fake CD relative FileName=\"%s\"\n", lpFileName);
        }
    } while(FALSE);
    //OutDebugDW("TranslatePath: mapping=%d FileName=\"%s\"\n",
    //	mapping ? *mapping : 0,
    //	lpFileName);
    if(mapping) *mapping = dwMapping;
    return lpFileName;
}

LPCWSTR dxwTranslatePathW(LPCWSTR lpFileName, DWORD *mapping) {
    static WCHAR sNewPath[MAX_PATH + 1];
    BOOL hasDriveLetter = FALSE;
    size_t converted = 0;
    // v2.05.29: alignement with ASCII routine
    DWORD dwMapping;
    dwMapping = DXW_NO_FAKE;
    // safeguard
    if(lpFileName == NULL) return NULL;
    if(wcslen(lpFileName) > 1)
        if(lpFileName[1] == L':') hasDriveLetter = TRUE;
    sNewPath[MAX_PATH] = 0;
    do { // fake loop
        // v2.04.84: find matching for drive name. Do not look for 3 chars (like "E:\") because
        // some games omit the backslash. For instance, "Need for Speed II" looks for "X:NFSW.EXE"
        // Also, take care that drive letters could be in lowercase
        if((dxw.dwFlags10 & FAKEHDDRIVE) && (lpFileName[1] == L':') && ((char)toupper((char)lpFileName[0]) == dxw.FakeHDDrive)) {
            mbstowcs_s(&converted, sNewPath, MAX_PATH, dxwGetFakeDriverPath(DXW_HD_PATH), _TRUNCATE);
            wcsncat(sNewPath, &lpFileName[2], MAX_PATH - wcslen(sNewPath));
            lpFileName = sNewPath;
            dwMapping = DXW_FAKE_HD;
            OutTraceDW("TranslatePath: fake HD new FileName=\"%ls\"\n", lpFileName);
            break;
        }
        if((dxw.dwFlags10 & FAKEHDDRIVE) && !wcsncmp(lpFileName, L"\\\\.\\", 4)) {
            mbstowcs_s(&converted, sNewPath, MAX_PATH, dxwGetFakeDriverPath(DXW_HD_PATH), _TRUNCATE);
            wcsncat(sNewPath, &lpFileName[3], MAX_PATH - wcslen(sNewPath));
            lpFileName = sNewPath;
            dwMapping = DXW_FAKE_HD;
            OutTraceDW("TranslatePath: fake HD new FileName=\"%ls\"\n", lpFileName);
            break;
        }
        if((dxw.dwFlags10 & FAKECDDRIVE) && (lpFileName[1] == L':') && ((char)toupper((char)lpFileName[0]) == dxw.FakeCDDrive)) {
            mbstowcs_s(&converted, sNewPath, MAX_PATH, dxwGetFakeDriverPath(DXW_CD_PATH), _TRUNCATE);
            wcsncat(sNewPath, &lpFileName[2], MAX_PATH - wcslen(sNewPath));
            lpFileName = sNewPath;
            dwMapping = DXW_FAKE_CD;
            OutTraceDW("TranslatePath: fake CD new FileName=\"%ls\"\n", lpFileName);
            break;
        }
        if((dxw.dwCurrentFolderType != DXW_NO_FAKE) && !hasDriveLetter) {
            mbstowcs_s(&converted, sNewPath, MAX_PATH, dxwGetFakeDriverPath((dxw.dwCurrentFolderType == DXW_FAKE_HD) ? DXW_HD_PATH : DXW_CD_PATH), _TRUNCATE);
            wcsncat(sNewPath, lpFileName, MAX_PATH - wcslen(sNewPath)); // v2.05.53 fix (ref. "Jack Carlton's Soccer Nation")
            lpFileName = sNewPath;
            dwMapping = dxw.dwCurrentFolderType;
            OutTraceDW("TranslatePath: fake CD relative FileName=\"%ls\"\n", lpFileName);
        }
    } while(FALSE);
    //OutDebugDW("TranslatePath: mapping=%d FileName=\"%ls\"\n",
    //	mapping ? *mapping : 0,
    //	lpFileName);
    if(mapping) *mapping = dwMapping;
    return lpFileName;
}

LPCSTR dxwUntranslatePathA(LPCSTR lpFileName, DWORD *mapping) {
    static char sNewPath[MAX_PATH + 1];
    BOOL hasDriveLetter = FALSE;
    DWORD dwMapping;
    dwMapping = DXW_NO_FAKE;
    // safeguard
    if(lpFileName == NULL) return NULL;
    if(strlen(lpFileName) > 1)
        if(lpFileName[1] == ':') hasDriveLetter = TRUE;
    sNewPath[MAX_PATH] = 0;
    do { // fake loop
        // find matching for drive name. Here you can look for 3 chars (like "E:\") because
        // a translated path always has it.
        // make a string compare for 1 character less becuse of the ending backslash '\'
        if((dxw.dwFlags10 & FAKEHDDRIVE) && hasDriveLetter) {
            char *lpFakeHDPath = dxwGetFakeDriverPath(DXW_HD_PATH);
            if(strncmp(lpFileName, lpFakeHDPath, strlen(lpFakeHDPath) - 1) == 0) {
                sprintf(sNewPath, "%c:\\", dxw.FakeHDDrive);
                strcat(sNewPath, lpFileName + strlen(lpFakeHDPath));
                lpFileName = sNewPath;
                dwMapping = DXW_FAKE_HD;
                OutTraceDW("UntranslatePath: fake HD new FileName=\"%s\"\n", lpFileName);
            }
            break;
        }
        if((dxw.dwFlags10 & FAKECDDRIVE) && hasDriveLetter) {
            char *lpFakeCDPath = dxwGetFakeDriverPath(DXW_CD_PATH);
            if(strncmp(lpFileName, lpFakeCDPath, strlen(lpFakeCDPath) - 1) == 0) {
                sprintf(sNewPath, "%c:\\", dxw.FakeCDDrive);
                strcat(sNewPath, lpFileName + strlen(lpFakeCDPath));
                lpFileName = sNewPath;
                dwMapping = DXW_FAKE_CD;
                OutTraceDW("UntranslatePath: fake CD new FileName=\"%s\"\n", lpFileName);
            }
        }
    } while(FALSE);
    //OutDebugDW("TranslatePathA: mapping=%d FileName=\"%s\"\n",
    //	mapping ? *mapping : 0,
    //	lpFileName);
    if(mapping) *mapping = dwMapping;
    return lpFileName;
}

LPWSTR dxwUntranslatePathW(LPWSTR lpFileName, DWORD *mapping) {
    static TCHAR sNewPath[MAX_PATH + 1];
    BOOL hasDriveLetter = FALSE;
    DWORD dwMapping;
    dwMapping = DXW_NO_FAKE;
    // safeguard
    if(lpFileName == NULL) return NULL;
    if(wcslen(lpFileName) > 1)
        if(lpFileName[1] == L':') hasDriveLetter = TRUE;
    sNewPath[MAX_PATH] = 0;
    do { // fake loop
        // v2.04.84: find matching for drive name. Here you can look for 3 chars (like "E:\") because
        // a translated path always has it
        if((dxw.dwFlags10 & FAKEHDDRIVE) && hasDriveLetter) {
            char *lpFakeHDPath = dxwGetFakeDriverPath(DXW_HD_PATH);
            TCHAR FakeHDPathW[MAX_PATH + 1];
            size_t len;
            mbstowcs_s(&len, (LPWSTR)FakeHDPathW, strlen(lpFakeHDPath), lpFakeHDPath, MAX_PATH);
            if(wcsncmp(lpFileName, (LPWSTR)FakeHDPathW, strlen(lpFakeHDPath) - 1) == 0) {
                swprintf((LPWSTR)sNewPath, MAX_PATH, L"%c:\\", dxw.FakeHDDrive);
                wcscat((LPWSTR)sNewPath, lpFileName + strlen(lpFakeHDPath));
                lpFileName = (LPWSTR)sNewPath;
                dwMapping = DXW_FAKE_HD;
                OutTraceDW("UntranslatePath: fake HD new FileName=\"%ls\"\n", lpFileName);
            }
            break;
        }
        if((dxw.dwFlags10 & FAKECDDRIVE) && hasDriveLetter) {
            char *lpFakeCDPath = dxwGetFakeDriverPath(DXW_CD_PATH);
            TCHAR FakeCDPathW[MAX_PATH + 1];
            size_t len;
            mbstowcs_s(&len, (LPWSTR)FakeCDPathW, strlen(lpFakeCDPath), lpFakeCDPath, MAX_PATH);
            if(wcsncmp(lpFileName, (LPWSTR)lpFakeCDPath, strlen(lpFakeCDPath) - 1) == 0) {
                swprintf((LPWSTR)sNewPath, MAX_PATH, L"%c:\\", dxw.FakeCDDrive);
                wcscat((LPWSTR)sNewPath, lpFileName + strlen(lpFakeCDPath));
                lpFileName = (LPWSTR)sNewPath;
                dwMapping = DXW_FAKE_CD;
                OutTraceDW("UntranslatePath: fake CD new FileName=\"%ls\"\n", lpFileName);
            }
        }
    } while(FALSE);
    //OutDebugDW("UntranslatePathW: mapping=%d FileName=\"%ls\"\n",
    //	mapping ? *mapping : 0,
    //	lpFileName);
    if(mapping) *mapping = dwMapping;
    return lpFileName;
}

DWORD dxwVirtualDriveType(LPCTSTR lpPath) {
    char DriveLetter = toupper(lpPath[0]);
    if(DriveLetter == dxw.FakeHDDrive) return DXW_FAKE_HD;
    if(DriveLetter == dxw.FakeCDDrive) return DXW_FAKE_CD;
    return DXW_NO_FAKE;
}

static DWORD GetFakeAttribute(DWORD attribute) {
    // to do: check more cases ...
    DWORD fake;
    switch(attribute) {
    case FILE_ATTRIBUTE_DIRECTORY:
        fake = FILE_ATTRIBUTE_DIRECTORY;
        break;
    case FILE_ATTRIBUTE_READONLY:
        fake = FILE_ATTRIBUTE_READONLY;
        break;
    case FILE_ATTRIBUTE_ARCHIVE:
    default:
        fake = FILE_ATTRIBUTE_READONLY;
        OutTraceDW("mapping file attribute %#x to FILE_ATTRIBUTE_READONLY\n", attribute);
        break; // what is best default?
    }
    return fake;
}

static char *dxwGetCDVolumeName() {
    char InitPath[MAX_PATH + 1];
    char key[20];
    static char s_CDLabel[256 + 1];
    //if(dxw.dwFlags11 & SLOWDEVPOLLING) Sleep(200);
    Sleep(100);
    sprintf_s(InitPath, MAX_PATH, "%s\\dxwnd.%s", GetDxWndPath(), (dxw.dwIndex == -1) ? "dxw" : "ini");
    sprintf_s(key, sizeof(key), "fakecdlabel%i", (dxw.dwIndex == -1) ? 0 : dxw.dwIndex); // v2.05.23 fix for proxy
    //OutTrace("dxwGetCDVolumeName: key=%s\n", key);
    (*pGetPrivateProfileStringA)("target", key, "", s_CDLabel, 256, InitPath);
    OutDebugSYS("CD label path=%s key=%s val=\"%s\"\n", InitPath, key, s_CDLabel);
    // if there's the ';' separator, returns the n-th label value only
    if(strchr(s_CDLabel, ';')) {
        char *pBegin, *pEnd;
        int cdindex = GetHookInfo()->CDIndex;
        pBegin = s_CDLabel;
        pEnd = strchr(pBegin, ';');
        if(pEnd) *pEnd = 0;
        for(int i = 0; i < cdindex; i++) {
            pBegin = pEnd + 1;
            pEnd = strchr(pBegin, ';');
            if(pEnd) *pEnd = 0;
        }
        OutDebugSYS("CD%d label=\"%s\"\n", cdindex + 1, pBegin);
        return pBegin;
    } else {
        OutDebugSYS("CD label=\"%s\"\n", s_CDLabel);
        return s_CDLabel;
    }
}

/* ------------------------------------------------------------------------------ */
// kernel32.dll wrappers
/* ------------------------------------------------------------------------------ */

int WINAPI extIsDebuggerPresent(void) {
    OutTraceDW("extIsDebuggerPresent: return FALSE\n");
    return FALSE;
}

BOOL WINAPI extGetDiskFreeSpaceA(LPCSTR lpRootPathName, LPDWORD lpSectorsPerCluster, LPDWORD lpBytesPerSector, LPDWORD lpNumberOfFreeClusters, LPDWORD lpTotalNumberOfClusters) {
    ApiName("GetDiskFreeSpaceA");
    BOOL ret;
    OutTraceSYS("%s: RootPathName=\"%s\"\n", ApiRef, lpRootPathName);
    ret = (*pGetDiskFreeSpaceA)(lpRootPathName, lpSectorsPerCluster, lpBytesPerSector, lpNumberOfFreeClusters, lpTotalNumberOfClusters);
    if(ret) {
        OutTraceSYS("%s: SectXCluster=%d BytesXSect=%d FreeClusters=%d TotalClusters=%d\n", ApiRef,
                    *lpSectorsPerCluster, *lpBytesPerSector, *lpNumberOfFreeClusters, *lpTotalNumberOfClusters);
    } else
        TraceError();
    // v2.05.09 fix: GetDiskFreeSpace can accept a NULL value for lpRootPathName
    // to tell the root of the current disk. Found in "Byzantine the Betrayal".
    // t.b.d: handle case where the current disk is a fake device!
    if((dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) && lpRootPathName) {
        // v2.05.54: more strict and correct check: avoid mixing fake HD and CD logics
        if((dxw.dwFlags10 & FAKECDDRIVE) && (toupper(lpRootPathName[0]) == dxw.FakeCDDrive)) {
            OutTraceDW("%s: fake CD detected\n", ApiRef);
            // copied from "Need for Speed 2" CD
            if(lpSectorsPerCluster)		*lpSectorsPerCluster = 1;
            if(lpBytesPerSector)		*lpBytesPerSector = 2048;
            if(lpNumberOfFreeClusters)	*lpNumberOfFreeClusters = 0;
            if(lpTotalNumberOfClusters)	*lpTotalNumberOfClusters = 266547;
            return TRUE;
        }
        if((dxw.dwFlags10 & FAKEHDDRIVE) && (toupper(lpRootPathName[0]) == dxw.FakeHDDrive)) {
            OutTraceDW("%s: fake HD detected\n", ApiRef);
            // copied from maximum limited resources
            if(lpSectorsPerCluster)		*lpSectorsPerCluster = 8;
            if(lpBytesPerSector)		*lpBytesPerSector = 512;
            if(lpNumberOfFreeClusters)	*lpNumberOfFreeClusters = 24414;
            if(lpTotalNumberOfClusters)	*lpTotalNumberOfClusters = 29296;
            return TRUE;
        }
    }
    if(dxw.dwFlags2 & LIMITRESOURCES) {
        // try to define 100MB free space in a 120MB hard disk
        DWORD BytesXCluster = *lpBytesPerSector * *lpSectorsPerCluster;
        if(BytesXCluster) {
            *lpNumberOfFreeClusters = 100000000 / BytesXCluster;
            *lpTotalNumberOfClusters = 120000000 / BytesXCluster;
            OutTraceDW("%s: FIXED SectXCluster=%d BytesXSect=%d FreeClusters=%d TotalClusters=%d\n", ApiRef,
                       *lpSectorsPerCluster, *lpBytesPerSector, *lpNumberOfFreeClusters, *lpTotalNumberOfClusters);
        }
    }
    return ret;
}

/*  -------------------------------------------------------------------------------

    GlobalMemoryStatus: MSDN documents that on modern PCs that have more than DWORD
    memory values the GlobalMemoryStatus sets the fields to -1 (0xFFFFFFFF) and you
    should use GlobalMemoryStatusEx instead.
    But in some cases the value is less that DWORD max, but greater that DWORD>>1, that
    is the calling application may get a big value and see it as a signed negative
    value, as it happened to Nocturne on my PC. That's why it's not adviseable to write:
    if(lpBuffer->dwTotalPhys== -1) ...
    but this way:
    if ((int)lpBuffer->dwTotalPhys < 0) ...
    and also don't set
    BIGENOUGH 0x80000000 // possibly negative!!!
    but:
    BIGENOUGH 0x60000000 // surely positive !!!

    v2.03.08: the "Jeff Gordon XS Racing demo" adds the dwAvailPhys to the dwAvailPageFile
    value, so that the sum is negative. To avoid this, all available memory values are
    divided by 2 (HALFBIG macro).
    /* ---------------------------------------------------------------------------- */

#define BIGENOUGH 0x60000000
#define HALFBIG (BIGENOUGH >> 1)
#define TOOBIG	  0xFFFFFFFF

void WINAPI extGlobalMemoryStatus(LPMEMORYSTATUS lpBuffer) {
    ApiName("GlobalMemoryStatus");
    (*pGlobalMemoryStatus)(lpBuffer);
    OutTraceSYS("%s: Length=%d MemoryLoad=%d%c "
                "TotalPhys=%#x AvailPhys=%#x TotalPageFile=%#x AvailPageFile=%#x TotalVirtual=%#x AvailVirtual=%#x\n",
                ApiRef, lpBuffer->dwLength, lpBuffer->dwMemoryLoad, '%', lpBuffer->dwTotalPhys, lpBuffer->dwAvailPhys,
                lpBuffer->dwTotalPageFile, lpBuffer->dwAvailPageFile, lpBuffer->dwTotalVirtual, lpBuffer->dwAvailVirtual);
    if(lpBuffer->dwLength == sizeof(MEMORYSTATUS)) {
        MEMORYSTATUS PrevMemoryStatus;
        memcpy(&PrevMemoryStatus, lpBuffer, sizeof(MEMORYSTATUS));
        if (((int)lpBuffer->dwTotalPhys < 0) || ((int)lpBuffer->dwTotalPhys > BIGENOUGH)) lpBuffer->dwTotalPhys = BIGENOUGH;
        if (((int)lpBuffer->dwAvailPhys < 0) || ((int)lpBuffer->dwAvailPhys > HALFBIG)) lpBuffer->dwAvailPhys = HALFBIG;
        if (((int)lpBuffer->dwTotalPageFile < 0) || ((int)lpBuffer->dwTotalPageFile > BIGENOUGH)) lpBuffer->dwTotalPageFile = BIGENOUGH;
        if (((int)lpBuffer->dwAvailPageFile < 0) || ((int)lpBuffer->dwAvailPageFile > HALFBIG)) lpBuffer->dwAvailPageFile = HALFBIG;
        if (((int)lpBuffer->dwTotalVirtual < 0) || ((int)lpBuffer->dwTotalVirtual > BIGENOUGH)) lpBuffer->dwTotalVirtual = BIGENOUGH;
        if (((int)lpBuffer->dwAvailVirtual < 0) || ((int)lpBuffer->dwAvailVirtual > HALFBIG)) lpBuffer->dwAvailVirtual = HALFBIG;
        if(dxw.dwDFlags & STRESSRESOURCES) {
            lpBuffer->dwTotalPhys = TOOBIG;
            lpBuffer->dwAvailPhys = TOOBIG;
            lpBuffer->dwTotalPageFile = TOOBIG;
            lpBuffer->dwAvailPageFile = TOOBIG;
            lpBuffer->dwTotalVirtual = TOOBIG;
            lpBuffer->dwAvailVirtual = TOOBIG;
        }
        _if(memcmp(&PrevMemoryStatus, lpBuffer, sizeof(MEMORYSTATUS)))
        OutTraceDW("%s UPDATED: Length=%d MemoryLoad=%d%c "
                   "TotalPhys=%#x AvailPhys=%#x TotalPageFile=%#x AvailPageFile=%#x TotalVirtual=%#x AvailVirtual=%#x\n",
                   ApiRef, lpBuffer->dwLength, lpBuffer->dwMemoryLoad, '%', lpBuffer->dwTotalPhys, lpBuffer->dwAvailPhys,
                   lpBuffer->dwTotalPageFile, lpBuffer->dwAvailPageFile, lpBuffer->dwTotalVirtual, lpBuffer->dwAvailVirtual);
    } else
        OutTraceE("%s WARNING: Length=%d sizeof(LPMEMORYSTATUS)=%d\n", ApiRef, lpBuffer->dwLength, sizeof(LPMEMORYSTATUS));
}

#ifdef TRACEALL
BOOL WINAPI extGlobalMemoryStatusEx(LPMEMORYSTATUSEX lpBuffer) {
    BOOL ret;
    ApiName("GlobalMemoryStatusEx");
    ret = (*pGlobalMemoryStatusEx)(lpBuffer);
    if(ret) {
        if(lpBuffer->dwLength == sizeof(MEMORYSTATUSEX)) {
            OutTraceSYS("%s: Length=%d MemoryLoad=%d%c "
                        "TotalPhys=%llx AvailPhys=%llx TotalPageFile=%llx AvailPageFile=%llx TotalVirtual=%llx AvailVirtual=%llx\n",
                        ApiRef, lpBuffer->dwLength, lpBuffer->dwMemoryLoad, '%', lpBuffer->ullTotalPhys, lpBuffer->ullAvailPhys,
                        lpBuffer->ullTotalPageFile, lpBuffer->ullAvailPageFile, lpBuffer->ullTotalVirtual, lpBuffer->ullAvailVirtual);
        } else
            OutTraceE("%s WARNING: Length=%d sizeof(LPMEMORYSTATUSEX)=%d\n", ApiRef, lpBuffer->dwLength, sizeof(LPMEMORYSTATUSEX));
    } else
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extGetPhysicallyInstalledSystemMemory(PULONGLONG pTotalMemoryInKilobytes) {
    BOOL ret;
    ApiName("GetPhysicallyInstalledSystemMemory");
    ret = (*pGetPhysicallyInstalledSystemMemory)(pTotalMemoryInKilobytes);
    if(ret)
        OutTraceDW("%s: TotalMemory(KBytes)=0x%llx\n", ApiRef, *pTotalMemoryInKilobytes);
    else
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}
#endif // TRACEALL

/*
    From MSDN:
    Operating system		Version number	dwMajorVersion	dwMinorVersion	Other
    Windows 8				6.2		6		2		OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
    Windows Server 2012		6.2		6		2		OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
    Windows 7				6.1		6		1		OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
    Windows Server 2008 R2	6.1		6		1		OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
    Windows Server 2008		6.0		6		0		OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
    Windows Vista			6.0		6		0		OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
    Windows Server 2003 R2	5.2		5		2		GetSystemMetrics(SM_SERVERR2) != 0
    Windows Home Server		5.2		5		2		OSVERSIONINFOEX.wSuiteMask & VER_SUITE_WH_SERVER
    Windows Server 2003		5.2		5		2		GetSystemMetrics(SM_SERVERR2) == 0
    Windows XP Pro x64 Ed.	5.2		5		2		(OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION) && (SYSTEM_INFO.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
    Windows XP				5.1		5		1		Not applicable
    Windows 2000			5.0		5		0		Not applicable
    From http://delphi.about.com/cs/adptips2000/a/bltip1100_2.htm
    Windows 95				4.0		4		0
    Windows 98/SE"			4.10	4		10		if osVerInfo.szCSDVersion[1] = 'A' then Windows98SE
    Windows ME				4.90	4		90
*/

/*
    Differences with older Windows versions
    This function is provided even with older Windows versions with some significant differences than stated above:
    The high order bit determins if it's NT based (NT, 2000, XP and newer) or not (Win 3.1, 95, 98, ME)
    The remaining bits of the high order word specify the build number only on NT based Windows verions.

    From older MSDN:
    To distinguish between operating system platforms, use the high order bit and the low order byte, as shown in the following table:

    Windows NT
    High order bit: 0
    Low order byte (major version number): 3 or 4
    Windows 95 and Windows 98
    High order bit: 1
    Low order byte (major version number): 4
    Win32s with Windows 3.1
    High order bit: 1
    Low order byte (major version number): 3

    For Windows NT and Win32s, the remaining bits in the high order word specify the build number.
    For Windows 95 and Windows 98, the remaining bits of the high order word are reserved.
*/

// v2.03.20: "Talonsoft's Operational Art of War II" checks the dwPlatformId field
// v2.03.20: list revised according to Microsoft compatibility settings

static struct {
    DWORD bMajor;
    DWORD bMinor;
    DWORD dwPlatformId;
    DWORD build;
    char *sName;
} WinVersions[9] = {
    {4, 0, VER_PLATFORM_WIN32_WINDOWS,	950,		"Windows 95"},
    {4, 10, VER_PLATFORM_WIN32_WINDOWS,	67766446,	"Windows 98/SE"},
    {4, 90, VER_PLATFORM_WIN32_WINDOWS,	0,			"Windows ME"},
    //	{4, 0, VER_PLATFORM_WIN32_NT,		1381,		"Windows NT4.0(sp5)"},
    {5, 0, VER_PLATFORM_WIN32_NT,		2195,		"Windows 2000"},
    //	{5, 1, VER_PLATFORM_WIN32_NT,		2600,		"Windows XP(sp2)"},
    {5, 1, VER_PLATFORM_WIN32_NT,		2600,		"Windows XP(sp3)"},
    {5, 2, VER_PLATFORM_WIN32_NT,		3790,		"Windows Server 2003(sp1)"},
    //	{6, 0, VER_PLATFORM_WIN32_NT,		6001,		"Windows Server 2008(sp1)"},
    //	{6, 0, VER_PLATFORM_WIN32_NT,		6000,		"Windows Vista"},
    //	{6, 0, VER_PLATFORM_WIN32_NT,		6001,		"Windows Vista(sp1)"},
    {6, 0, VER_PLATFORM_WIN32_NT,		6002,		"Windows Vista(sp2)"},
    {6, 1, VER_PLATFORM_WIN32_NT,		7600,		"Windows 7"},
    {6, 2, VER_PLATFORM_WIN32_NT,		0,			"Windows 8"}
};

DWORD dxwGetMajor() {
    return WinVersions[dxw.FakeVersionId].bMajor;
}

DWORD dxwGetMinor() {
    return WinVersions[dxw.FakeVersionId].bMinor;
}

BOOL WINAPI extGetVersionExA(LPOSVERSIONINFOA lpVersionInfo) {
    ApiName("GetVersionExA");
    BOOL ret;
    ret = (*pGetVersionExA)(lpVersionInfo);
    if(!ret) {
        TraceError();
        return ret;
    }
    OutTraceDW("%s: version=%d.%d platform=%#x build=(%d)\n", ApiRef,
               lpVersionInfo->dwMajorVersion, lpVersionInfo->dwMinorVersion, lpVersionInfo->dwPlatformId, lpVersionInfo->dwBuildNumber);
    if(dxw.dwFlags2 & FAKEVERSION) {
        // fake Win XP build 0
        lpVersionInfo->dwMajorVersion = WinVersions[dxw.FakeVersionId].bMajor;
        lpVersionInfo->dwMinorVersion = WinVersions[dxw.FakeVersionId].bMinor;
        lpVersionInfo->dwPlatformId = WinVersions[dxw.FakeVersionId].dwPlatformId;
        lpVersionInfo->dwBuildNumber = 0;
        OutTraceDW("%s: FIXED version=%d.%d platform=%#x build=(%d) os=\"%s\"\n", ApiRef,
                   lpVersionInfo->dwMajorVersion, lpVersionInfo->dwMinorVersion, lpVersionInfo->dwPlatformId, lpVersionInfo->dwBuildNumber,
                   WinVersions[dxw.FakeVersionId].sName);
    }
    return TRUE;
}

BOOL WINAPI extGetVersionExW(LPOSVERSIONINFOW lpVersionInfo) {
    ApiName("GetVersionExW");
    BOOL ret;
    ret = (*pGetVersionExW)(lpVersionInfo);
    if(!ret) {
        TraceError();
        return ret;
    }
    OutTraceDW("%s: version=%d.%d platform=%#x build=(%d)\n", ApiRef,
               lpVersionInfo->dwMajorVersion, lpVersionInfo->dwMinorVersion, lpVersionInfo->dwPlatformId, lpVersionInfo->dwBuildNumber);
    if(dxw.dwFlags2 & FAKEVERSION) {
        // fake Win XP build 0
        lpVersionInfo->dwMajorVersion = WinVersions[dxw.FakeVersionId].bMajor;
        lpVersionInfo->dwMinorVersion = WinVersions[dxw.FakeVersionId].bMinor;
        lpVersionInfo->dwPlatformId = WinVersions[dxw.FakeVersionId].dwPlatformId;
        lpVersionInfo->dwBuildNumber = 0;
        OutTraceDW("%s: FIXED version=%d.%d platform=%#x build=(%d) os=\"%s\"\n", ApiRef,
                   lpVersionInfo->dwMajorVersion, lpVersionInfo->dwMinorVersion, lpVersionInfo->dwPlatformId, lpVersionInfo->dwBuildNumber,
                   WinVersions[dxw.FakeVersionId].sName);
    }
    return TRUE;
}

DWORD WINAPI extGetVersion(void) {
    ApiName("GetVersion");
    DWORD dwVersion;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuild = 0;
    dwVersion = (*pGetVersion)();
    // Get the Windows version.
    dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
    dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
    // Get the build number.
    if (dwVersion < 0x80000000)
        dwBuild = (DWORD)(HIWORD(dwVersion));
    OutTraceDW("%s: version=%d.%d build=(%d)\n", ApiRef, dwMajorVersion, dwMinorVersion, dwBuild);
    if(dxw.dwFlags2 & FAKEVERSION) {
        dwVersion = WinVersions[dxw.FakeVersionId].bMajor | (WinVersions[dxw.FakeVersionId].bMinor << 8);
        dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
        dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
        dwBuild = (DWORD)(HIWORD(dwVersion));
        if(WinVersions[dxw.FakeVersionId].bMajor == 4) dwVersion |= 0x80000000; // v2.03.11: fixes "Warhead"
        OutTraceDW("%s: FIXED version=%#x: Win%d.%d build=(%d) os=\"%s\"\n", ApiRef,
                   dwVersion, dwMajorVersion, dwMinorVersion, dwBuild, WinVersions[dxw.FakeVersionId].sName);
    }
    return dwVersion;
}

/*  -------------------------------------------------------------------------------

    time related APIs

    /* ---------------------------------------------------------------------------- */

DWORD WINAPI extGetTickCount(void) {
    DWORD ret;
    ret = dxw.GetTickCount();
    OutTraceT("GetTickCount: ret=%#x\n", ret);
    return ret;
}

void WINAPI extGetSystemTime(LPSYSTEMTIME lpSystemTime) {
    dxw.GetSystemTime(lpSystemTime);
    OutTraceT("GetSystemTime: %02d:%02d:%02d.%03d\n",
              lpSystemTime->wHour, lpSystemTime->wMinute, lpSystemTime->wSecond, lpSystemTime->wMilliseconds);
}

void WINAPI extGetLocalTime(LPSYSTEMTIME lpLocalTime) {
    SYSTEMTIME SystemTime;
    dxw.GetSystemTime(&SystemTime);
    SystemTimeToTzSpecificLocalTime(NULL, &SystemTime, lpLocalTime);
    OutTraceT("GetLocalTime: %02d:%02d:%02d.%03d\n",
              lpLocalTime->wHour, lpLocalTime->wMinute, lpLocalTime->wSecond, lpLocalTime->wMilliseconds);
}

VOID WINAPI extSleep(DWORD dwMilliseconds) {
    DWORD dwNewDelay;
    dwNewDelay = dwMilliseconds;
    if ((dwMilliseconds != INFINITE) && (dwMilliseconds != 0)) {
        dwNewDelay = dxw.StretchTime(dwMilliseconds);
        if (dwNewDelay == 0) dwNewDelay = 1; // minimum allowed...
    }
    OutTraceT("Sleep: msec=%d->%d timeshift=%d\n", dwMilliseconds, dwNewDelay, dxw.TimeShift);
    (*pSleep)(dwNewDelay);
    while(dxw.TimeFreeze)(*pSleep)(40);
}

DWORD WINAPI extSleepEx(DWORD dwMilliseconds, BOOL bAlertable) {
    DWORD ret;
    DWORD dwNewDelay;
    dwNewDelay = dwMilliseconds;
    if ((dwMilliseconds != INFINITE) && (dwMilliseconds != 0)) {
        dwNewDelay = dxw.StretchTime(dwMilliseconds);
        if (dwNewDelay == 0) dwNewDelay = 1; // minimum allowed...
    }
    OutTraceT("SleepEx: msec=%d->%d alertable=%#x, timeshift=%d\n", dwMilliseconds, dwNewDelay, bAlertable, dxw.TimeShift);
    ret = (*pSleepEx)(dwNewDelay, bAlertable);
    while(dxw.TimeFreeze)(*pSleep)(40);
    return ret;
}

void WINAPI extGetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime) {
    OutTraceT("GetSystemTimeAsFileTime\n");
    dxw.GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
}

BOOL WINAPI extQueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount) {
    BOOL ret;
    if(dxw.dwFlags4 & NOPERFCOUNTER) {
        ret = 0;
        (*lpPerformanceCount).QuadPart = 0;
    } else {
        LARGE_INTEGER CurrentInCount;
        ret = (*pQueryPerformanceCounter)(&CurrentInCount);
        if(dxw.dwFlags5 & NORMALIZEPERFCOUNT) {
            LARGE_INTEGER PerfFrequency;
            static LARGE_INTEGER StartCounter = {0LL};
            if (StartCounter.QuadPart == 0LL) StartCounter.QuadPart = CurrentInCount.QuadPart;
            (*pQueryPerformanceFrequency)(&PerfFrequency);
            CurrentInCount.QuadPart = ((CurrentInCount.QuadPart - StartCounter.QuadPart) * 1000000LL) / PerfFrequency.QuadPart;
        }
        *lpPerformanceCount = dxw.StretchLargeCounter(CurrentInCount);
    }
    OutTraceT("QueryPerformanceCounter: ret=%#x Count=[%#x-%#x]\n", ret, lpPerformanceCount->HighPart, lpPerformanceCount->LowPart);
    return ret;
}

BOOL WINAPI extQueryPerformanceFrequency(LARGE_INTEGER *lpPerformanceFrequency) {
    BOOL ret;
    if(dxw.dwFlags12 & STRETCHPERFREQUENCY) {
        LARGE_INTEGER myPerfFrequency;
        ret = (*pQueryPerformanceFrequency)(&myPerfFrequency);
        myPerfFrequency.LowPart = dxw.StretchTime(myPerfFrequency.LowPart);
        myPerfFrequency.HighPart = dxw.StretchTime(myPerfFrequency.HighPart);
        *lpPerformanceFrequency = myPerfFrequency;
        return TRUE;
    }
    if(dxw.dwFlags4 & NOPERFCOUNTER) {
        LARGE_INTEGER myPerfFrequency;
        myPerfFrequency.LowPart = 0L;
        myPerfFrequency.HighPart = 0L;
        *lpPerformanceFrequency = myPerfFrequency;
        ret = 0;
    } else {
        if(dxw.dwFlags5 & NORMALIZEPERFCOUNT) {
            lpPerformanceFrequency->QuadPart = 1000000LL;
            ret = TRUE;
        } else
            ret = (*pQueryPerformanceFrequency)(lpPerformanceFrequency);
    }
    OutTraceT("QueryPerformanceFrequency: ret=%#x Frequency=%#x-%#x\n", ret, lpPerformanceFrequency->HighPart, lpPerformanceFrequency->LowPart);
    return ret;
}

/*  -------------------------------------------------------------------------------

    LoadLibrary (hooking) related APIs

    /* ---------------------------------------------------------------------------- */

#ifndef LOAD_LIBRARY_SEARCH_SYSTEM32
#define LOAD_LIBRARY_SEARCH_SYSTEM32 0x00000800
#endif

HMODULE WINAPI LoadLibraryExWrapper(LPVOID lpFileName, BOOL IsWidechar, HANDLE hFile, DWORD dwFlags, ApiArg) {
    HMODULE libhandle;
    int idx;
    // recursion control: this is necessary so far only on WinXP while other OS like Win7,8,10 don't get into
    // recursion problems, but in any case better to leave it here, you never know ....
    static BOOL Recursed = FALSE;
    // troublesome flag causing failure on 64 bit platforms - to be flagged??
    dwFlags &= ~LOAD_LIBRARY_SEARCH_SYSTEM32;
    // v2.04.82: Fix different behavior of LoadLibraryEx with LOAD_WITH_ALTERED_SEARCH_PATH flag on recent OS.
    // Fixes "Anachronox" as alternative fix as manually copying mss32.dll in anoxdata\PLUGIN subfolder
    if((dxw.dwFlags3 & FIXALTEREDPATH) && (dwFlags & LOAD_WITH_ALTERED_SEARCH_PATH)) {
        char LocalPath[MAX_PATH + 1];
        GetCurrentDirectory(MAX_PATH, LocalPath);
        SetDllDirectory(LocalPath);
        OutTraceSYS("%s: fix LOAD_WITH_ALTERED_SEARCH_PATH path=\"%s\"\n", ApiRef, LocalPath);
    }
    if(IsWidechar) {
        OutDebugSYS("%s: file=%ls flags=%#x\n", ApiRef, lpFileName, dwFlags);
        libhandle = (*pLoadLibraryExW)((LPCWSTR)lpFileName, hFile, dwFlags);
    } else {
        OutDebugSYS("%s: file=%s flags=%#x\n", ApiRef, lpFileName, dwFlags);
        libhandle = (*pLoadLibraryExA)((LPCTSTR)lpFileName, hFile, dwFlags);
    }
    if(Recursed) {
        // v2.03.97.fx2: clear Recursed flag when exiting!
        Recursed = FALSE;
        return libhandle;
    }
    Recursed = TRUE;
    // found in "The Rage" (1996): loading a module with relative path after a SetCurrentDirectory may fail, though
    // the module is present in the current directory folder. To fix this problem in case of failure it is possible
    // to retry the operation using a full pathname composed concatenating current dir and module filename.
    // V2.05.38 FIX: "The Rage" hides libraries within files with different extensions, so you can't add the ".dll"
    // extension if missing. Maybe you could check if there's an extension at all, but it seems wiser to alter the
    // path as little as possible, so better not to do it. Added a check only to avoid the useless operation of
    // adding a full path to another full path (comparison of 2nd char with ':', rough but working).
    if(!libhandle) {
        BOOL bIsAbsolutePath = FALSE;
        char lpBuffer[MAX_PATH + 1];
        GetCurrentDirectory(MAX_PATH, lpBuffer);
        if(IsWidechar) {
            bIsAbsolutePath = (((WCHAR *)lpFileName)[1] == L':');
            sprintf_s(lpBuffer, MAX_PATH, "%s\\%ls",
                      lpBuffer,
                      lpFileName);
        } else {
            bIsAbsolutePath = (((char *)lpFileName)[1] == ':');
            sprintf_s(lpBuffer, MAX_PATH, "%s\\%s",
                      lpBuffer,
                      lpFileName);
        }
        if(!bIsAbsolutePath) {
            OutTraceDW("LoadLibrary: RETRY fullpath=\"%s\"\n", lpBuffer);
            libhandle = (*pLoadLibraryExA)(lpBuffer, hFile, dwFlags);
        }
    }
    if(IsWidechar)
        OutTraceDW("%s: FileName=%ls hFile=%#x Flags=%#x(%s) hmodule=%#x\n", ApiRef, lpFileName, hFile, dwFlags, ExplainLoadLibFlags(dwFlags), libhandle);
    else
        OutTraceDW("%s: FileName=%s hFile=%#x Flags=%#x(%s) hmodule=%#x\n", ApiRef, lpFileName, hFile, dwFlags, ExplainLoadLibFlags(dwFlags), libhandle);
    if(!libhandle) {
        if(IsWidechar)
            OutTraceE("%s: ERROR FileName=%ls err=%d\n", ApiRef, lpFileName, GetLastError());
        else
            OutTraceE("%s: ERROR FileName=%s err=%d\n", ApiRef, lpFileName, GetLastError());
        Recursed = FALSE;
        // compatibility issue: some games (Nightmare Creatures) check for the ERROR_DLL_NOT_FOUND
        // errorcode or assume the library is there, hence the dialog box about a SDL.DLL file to delete.
        if((dxw.dwFlags8 & LOADLIBRARYERR) && (GetLastError() == ERROR_MOD_NOT_FOUND)) {
            OutTraceDW("%s: setting err=ERROR_DLL_NOT_FOUND\n", ApiRef);
            SetLastError(ERROR_DLL_NOT_FOUND);
        }
        return libhandle;
    }
    // when loaded with LOAD_LIBRARY_AS_DATAFILE or LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE flags,
    // there's no symbol map, then it isn't possible to hook function calls.
    if(dwFlags & (LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_DATAFILE)) return libhandle;
    char *AnsiFileName;
    if(IsWidechar) {
        static char sFileName[256 + 1];
        wcstombs_s(NULL, sFileName, (LPCWSTR)lpFileName, 80);
        AnsiFileName = sFileName;
    } else
        AnsiFileName = (char *)lpFileName;
    // v2.05.51: "State of War 2" incredibly loads through a LoadLibraryExW call an executable
    // file, c:\\Windows\winhlp32.exe !! Though it's unclear to me why that for, the operation
    // followed by a hook attempt causes the game crash, so it must be bypassed.
    // Here DxWnd bypasses any attempt to load an exe file, you never know ...
    int pathlen = strlen(AnsiFileName);
    if((pathlen > 4) && (!strcmp(&AnsiFileName[pathlen - 4], ".exe"))) {
        OutTraceDW("%s: BYPASS exe hooking path=%s\n", ApiRef, AnsiFileName);
        return libhandle;
    }
    idx = dxw.GetDLLIndex(AnsiFileName); // v2.04.66 !!! AnsiFileName vs. lpFileName !!!
    // handle custom OpenGL library
    if(!lstrcmpi(AnsiFileName, dxw.CustomOpenGLLib)) {
        idx = SYSLIBIDX_OPENGL;
        SysLibsTable[idx].hmodule = libhandle;
    }
    if(dxw.dwFlags12 & SUPPRESSGLIDE) {
        switch(idx) {
        case SYSLIBIDX_GLIDE:
        case SYSLIBIDX_GLIDE2:
        case SYSLIBIDX_GLIDE3:
            OutTraceDW("%s: suppress glide support\n", ApiRef);
            (*pFreeLibrary)(libhandle);
            return NULL;
            break;
        }
    }
    if (idx == -1)  {
        OutTraceDW("%s: hooking lib=\"%s\" handle=%#x\n", ApiRef, AnsiFileName, libhandle);
        HookModule(libhandle);
        HookDlls(libhandle);
    } else {
        OutTraceDW("%s: push idx=%#x library=%s hdl=%#x\n", ApiRef, idx, AnsiFileName, libhandle);
        dxwLibsHookTable_Type *lpEntry = &SysLibsTable[idx];
        lpEntry->hmodule = libhandle;
        if(dxw.dwFlags12 & DIRECTXREPLACE) {
            switch(idx) {
            case SYSLIBIDX_DIRECTDRAW:
            case SYSLIBIDX_DIRECT3D8:
            case SYSLIBIDX_DIRECT3D9:
                if(libhandle) {
                    char path[MAX_PATH];
                    HMODULE newhandle;
                    sprintf(path, "%salt.dll\\%s.dll", GetDxWndPath(), SysLibsTable[idx].name);
                    newhandle = (*pLoadLibraryA)(path);
                    OutTrace("%s: alt.dll path=%s hinst=%#x\n", ApiRef, path, newhandle);
                    if(newhandle) {
                        // replace when a replacement dll is found
                        // CloseHandle may be hooked or not ...
                        CloseHandle(libhandle);
                        libhandle = newhandle;
                    }
                }
                break;
            }
        }
        if((lpEntry->flags & DXWHOOK_HOOK) && lpEntry->hookf) (*lpEntry->hookf)(libhandle);
        if(lpEntry->flags & DXWHOOK_EXTEND) {
            OutTraceDW("%s: extend hooking lib=\"%s\" handle=%#x\n", ApiRef, AnsiFileName, libhandle);
            HookDlls(libhandle);
        }
    }
    Recursed = FALSE;
    OutDebugSYS("%s: ret=%#x\n", ApiRef, libhandle);
    return libhandle;
}

HMODULE WINAPI extLoadLibraryA(LPCTSTR lpFileName) {
    return LoadLibraryExWrapper((LPVOID)lpFileName, FALSE, NULL, 0, "LoadLibraryA");
}

HMODULE WINAPI extLoadLibraryW(LPCWSTR lpFileName) {
    return LoadLibraryExWrapper((LPVOID)lpFileName, TRUE, NULL, 0, "LoadLibraryW");
}

HMODULE WINAPI extLoadLibraryExA(LPCTSTR lpFileName, HANDLE hFile, DWORD dwFlags) {
    return LoadLibraryExWrapper((LPVOID)lpFileName, FALSE, hFile, dwFlags, "LoadLibraryExA");
}

HMODULE WINAPI extLoadLibraryExW(LPCWSTR lpFileName, HANDLE hFile, DWORD dwFlags) {
    return LoadLibraryExWrapper((LPVOID)lpFileName, TRUE, hFile, dwFlags, "LoadLibraryExW");
}

// v2.04.82:
// FIXFREELIBRARY fixes a compatibility issue with w95 where multiple calls to
// FreeLibrary are expected to end up with an error (error code 0) so that Heavy Gear loops
// the FreeLibrary call until it gets a 0 return value, while on recent OS the call always
// return success (1)
// SKIPFREELIBRARY fixes a recursion problem in pre-patched executables (like GOG version of
// Disciples Gold) where FreeLibrary keeps calling itself ending up with a stack overflow

BOOL WINAPI extFreeLibrary(HMODULE hModule) {
    BOOL ret;
    ApiName("FreeLibrary");
    OutDebugSYS("%s: hModule=%#x\n", ApiRef, hModule);
    // early FreeLibrary fix: needed by "GOG Disciples Gold"
    if(dxw.dwFlags3 & SKIPFREELIBRARY) {
        static HMODULE hSkipModule = NULL;
        if (hModule == hSkipModule) {
            OutTraceDW("%s: SKIPFREELIBRARY hack ret=1\n", ApiRef);
            return (dxw.dwFlags7 & FIXFREELIBRARY) ? 0 : 1;
        }
        hSkipModule = hModule;
    }
    ret = (*pFreeLibrary)(hModule);
    if(ret) {
        OutDebugSYS("%s: ret=%#x\n", ApiRef, ret);
        // late FreeeLibrary fix: needed by "Heavy Gear"
        if(dxw.dwFlags7 & FIXFREELIBRARY) {
            static HMODULE hLastModule = NULL;
            if(hModule == hLastModule) {
                OutTraceDW("%s: FIXFREELIBRARY hack ret=0\n", ApiRef);
                ret = 0;
            }
            hLastModule = hModule;
        }
    } else
        OutTraceE("%s ERROR: err=%d\n", ApiRef, GetLastError());
    return ret;
}

extern DirectDrawCreate_Type pDirectDrawCreate;
extern DirectDrawCreateEx_Type pDirectDrawCreateEx;
extern HRESULT WINAPI extDirectDrawCreate(GUID FAR *, LPDIRECTDRAW FAR *, IUnknown FAR *);
extern HRESULT WINAPI extDirectDrawCreateEx(GUID FAR *, LPDIRECTDRAW FAR *, REFIID, IUnknown FAR *);
extern GetProcAddress_Type pGetProcAddress;
//extern HRESULT STDAPICALLTYPE extCoCreateInstance(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID FAR*);

FARPROC WINAPI extGetProcAddress(HMODULE hModule, LPCSTR proc) {
    FARPROC ret;
    int idx;
    ApiName("GetProcAddress");
    // WARNING: seems to be called with bad LPCSTR value....
    // from MSDN:
    // The function or variable name, or the function's ordinal value.
    // If this parameter is an ordinal value, it must be in the low-order word;
    // the high-order word must be zero.
    // v2.04.47: skipping processing of recursive calls.
    // recursion is caused by the use of some of these calls from within the stdlibC routines
    // used for logging, so no logging for these statements. Logging moved forward.
    // a better solution would be linking a private subset of std routines ...
    // fixes "Bruce Lee Remake" with suspend process hooking.
    for(idx = 0; SysLibsTable[idx].name; idx++) {
        if(SysLibsTable[idx].hmodule == hModule) break;
    }
    OutTraceDW("%s: hModule=%#x idx=%d proc=%s\n", ApiRef, hModule, idx, ProcToString(proc));
    if((!SysLibsTable[idx].name) || (SysLibsTable[idx].flags == DXWHOOK_NULL)) {
        ret = (*pGetProcAddress)(hModule, proc);
        OutTraceDW("%s: module=%#x proc=%s ret=%#x\n", ApiRef, hModule, ProcToString(proc), ret);
        return ret;
    }
    if(idx == SYSLIBIDX_GDI32) {
        // avoid recursions
        if(!strncmp(proc, "D3DKMT", 6)) return(*pGetProcAddress)(hModule, proc);
    }
    if((DWORD)proc & 0xFFFF0000) {
        FARPROC remap = 0;
        if((SysLibsTable[idx].flags & DXWHOOK_HOOK) && (SysLibsTable[idx].remapf)) {
            remap = (*SysLibsTable[idx].remapf)(proc, hModule);
            if(remap == (FARPROC) - 1) {
                OutTraceDW("%s: FAKE ret=0\n", ApiRef);
                return 0; // pretend the call isn't there ....
            }
            if(remap) {
                OutTraceDW("%s: HOOK ret=%#x\n", ApiRef, remap);
                return remap;
            }
        }
    } else {
        switch(idx) {
        case SYSLIBIDX_DIRECTDRAW:
            switch((DWORD)proc) {
            case 0x0008: // DirectDrawCreate
                pDirectDrawCreate = (DirectDrawCreate_Type)(*pGetProcAddress)(hModule, proc);
                OutTraceDW("%s: hooking proc=%s at addr=%#x\n", ApiRef, ProcToString(proc), pDirectDrawCreate);
                return (FARPROC)extDirectDrawCreate;
                break;
            case 0x000A: // DirectDrawCreateEx
                pDirectDrawCreateEx = (DirectDrawCreateEx_Type)(*pGetProcAddress)(hModule, proc);
                OutTraceDW("%s: hooking proc=%s at addr=%#x\n", ApiRef, ProcToString(proc), pDirectDrawCreateEx);
                return (FARPROC)extDirectDrawCreateEx;
                break;
            case 0x000B: // DirectDrawEnumerateA
                pDirectDrawEnumerateA = (DirectDrawEnumerateA_Type)(*pGetProcAddress)(hModule, proc);
                OutTraceP("%s: hooking proc=%s at addr=%#x\n", ApiRef, proc, pDirectDrawEnumerateA);
                return (FARPROC)extDirectDrawEnumerateA;
                break;
            case 0x000C: // DirectDrawEnumerateExA
                pDirectDrawEnumerateExA = (DirectDrawEnumerateExA_Type)(*pGetProcAddress)(hModule, proc);
                OutTraceP("%s: hooking proc=%s at addr=%#x\n", ApiRef, proc, pDirectDrawEnumerateExA);
                return (FARPROC)extDirectDrawEnumerateExA;
                break;
            }
            break;
        case SYSLIBIDX_USER32:
            if ((DWORD)proc == 0x0020) { // ChangeDisplaySettingsA
                /* if (!pChangeDisplaySettingsA) */ pChangeDisplaySettingsA = (ChangeDisplaySettingsA_Type)(*pGetProcAddress)(hModule, proc);
                OutTraceDW("%s: hooking proc=%s at addr=%#x\n", ApiRef, ProcToString(proc), pChangeDisplaySettingsA);
                return (FARPROC)extChangeDisplaySettingsA;
            }
            break;
#ifndef ANTICHEATING
        case SYSLIBIDX_KERNEL32:
            if ((DWORD)proc == 0x0305) { // "IsDebuggerPresent"
                OutTraceDW("%s: hooking proc=%s at addr=%#x\n", ApiRef, ProcToString(proc), extIsDebuggerPresent);
                return (FARPROC)extIsDebuggerPresent;
            }
            if ((DWORD)proc == 0x0050) { // "CheckRemoteDebuggerPresent"
                OutTraceDW("%s: hooking proc=%s at addr=%#x\n", ApiRef, ProcToString(proc), extCheckRemoteDebuggerPresent);
                return (FARPROC)extCheckRemoteDebuggerPresent;
            }
#endif
            // v2.04.49: commented out because unreliable, ordinal numbers of OLE32 are not constant
            //case SYSLIBIDX_OLE32:
            //	if ((DWORD)proc == 0x0011){ // "CoCreateInstance"
            //		pCoCreateInstance=(CoCreateInstance_Type)(*pGetProcAddress)(hModule, proc);
            //		OutTraceDW("GetProcAddress: hooking proc=%s at addr=%#x\n", ProcToString(proc), pCoCreateInstance);
            //		return (FARPROC)extCoCreateInstance;
            //	}
            //	break;
        }
    }
    ret = (*pGetProcAddress)(hModule, proc);
    OutTraceDW("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

/*  -------------------------------------------------------------------------------

    I/O related APIs (together with GetDiskFreeSpace)

    /* ---------------------------------------------------------------------------- */

UINT WINAPI extGetDriveTypeA(LPCSTR lpRootPathName) {
    UINT ret;
    ApiName("GetDriveTypeA");
    char *labels[] = {
        "DRIVE_UNKNOWN",
        "DRIVE_NO_ROOT_DIR",
        "DRIVE_REMOVABLE",
        "DRIVE_FIXED",
        "DRIVE_REMOTE",
        "DRIVE_CDROM",
        "DRIVE_RAMDISK"
    };
    OutTraceSYS("%s: path=\"%s\"\n", ApiRef, lpRootPathName);
    // v2.05.44 fix: it is possible to call GetDriveTypeA with NULL argument. Found in "Leadfoot Stadium Off-Road Racing"
    if(lpRootPathName) {
        if ((dxw.dwFlags10 & FAKECDDRIVE) && (dxw.FakeCDDrive == (char)toupper(lpRootPathName[0]))) {
            OutTraceSYS("%s: FAKECDDRIVE - fake CDROM %c: ret=5(DRIVE_CDROM)\n", ApiRef, dxw.FakeCDDrive);
            return DRIVE_CDROM;
        }
        if (dxw.dwFlags3 & CDROMDRIVETYPE) {
            OutTraceSYS("%s: CDROMDRIVETYPE - fake CDROM %c: ret=5(DRIVE_CDROM)\n", ApiRef, lpRootPathName[0]);
            return DRIVE_CDROM;
        }
        if (dxw.dwFlags4 & HIDECDROMEMPTY) {
            BOOL Vol;
            Vol = GetVolumeInformationA(lpRootPathName, NULL, NULL, NULL, 0, 0, 0, 0);
            OutTraceSYS("Vol=%#x\n", Vol);
            if(!Vol) {
                OutTraceSYS("%s: HIDECDROMEMPTY - hide CDROM %c:\n", ApiRef, lpRootPathName[0]);
                return DRIVE_UNKNOWN;
            }
        }
        ret = (*pGetDriveTypeA)(lpRootPathName);
    } else {
        // v2.05.50: when passing NULL (current folder) use the fake driver specification
        ret = (*pGetDriveTypeA)(lpRootPathName);
        if (dxw.dwFlags10 & (FAKECDDRIVE | FAKEHDDRIVE)) {
            if(dxw.dwCurrentFolderType == DXW_FAKE_HD) ret = DRIVE_FIXED;
            if(dxw.dwCurrentFolderType == DXW_FAKE_CD) ret = DRIVE_CDROM;
        }
    }
    OutTraceSYS("%s: ret=%#x(%s)\n", ApiRef, ret, ((ret >= DRIVE_UNKNOWN) && (ret <= DRIVE_RAMDISK)) ? labels[ret] : "unknown");
    return ret;
}

UINT WINAPI extGetDriveTypeW(LPWSTR lpRootPathName) {
    UINT ret;
    ApiName("GetDriveTypeA");
    char *labels[] = {
        "DRIVE_UNKNOWN",
        "DRIVE_NO_ROOT_DIR",
        "DRIVE_REMOVABLE",
        "DRIVE_FIXED",
        "DRIVE_REMOTE",
        "DRIVE_CDROM",
        "DRIVE_RAMDISK"
    };
    OutTraceSYS("%s: path=\"%ls\"\n", ApiRef, lpRootPathName);
    // v2.05.44 fix: it is possible to call GetDriveTypeW with NULL argument.
    if(lpRootPathName) {
        if ((dxw.dwFlags10 & FAKECDDRIVE) && (dxw.FakeCDDrive == (char)toupper(lpRootPathName[0]))) {
            OutTraceSYS("%s: FAKECDDRIVE - fake CDROM %c: ret=5(DRIVE_CDROM)\n", ApiRef, dxw.FakeCDDrive);
            return DRIVE_CDROM;
        }
        if (dxw.dwFlags3 & CDROMDRIVETYPE) {
            OutTraceSYS("%s: CDROMDRIVETYPE - fake CDROM %c: ret=5(DRIVE_CDROM)\n", ApiRef, lpRootPathName[0]);
            return DRIVE_CDROM;
        }
        if (dxw.dwFlags4 & HIDECDROMEMPTY) {
            BOOL Vol;
            Vol = GetVolumeInformationW(lpRootPathName, NULL, NULL, NULL, 0, 0, 0, 0);
            OutTraceSYS("Vol=%#x\n", Vol);
            if(!Vol) {
                OutTraceSYS("%s: HIDECDROMEMPTY - hide CDROM %c:\n", ApiRef, (char)lpRootPathName[0]);
                return DRIVE_UNKNOWN;
            }
        }
        ret = (*pGetDriveTypeW)(lpRootPathName);
    } else {
        // v2.05.50: when passing NULL (current folder) use the fake driver specification
        ret = (*pGetDriveTypeW)(lpRootPathName);
        if (dxw.dwFlags10 & (FAKECDDRIVE | FAKEHDDRIVE)) {
            if(dxw.dwCurrentFolderType == DXW_FAKE_HD) ret = DRIVE_FIXED;
            if(dxw.dwCurrentFolderType == DXW_FAKE_CD) ret = DRIVE_CDROM;
        }
    }
    OutTraceSYS("%s: ret=%#x(%s)\n", ApiRef, ret, ((ret >= DRIVE_UNKNOWN) && (ret <= DRIVE_RAMDISK)) ? labels[ret] : "unknown");
    return ret;
}

#ifndef DXW_NOTRACES
static char *sDevArray(DWORD mask) {
    static char sDevLetters[32 + 1];
    for(int i = 0; i < 32; i++) {
        sDevLetters[i] = (mask & 0x1) ? 'A' + i : '-';
        mask >>= 1;
    }
    sDevLetters[32] = 0;
    return sDevLetters;
}
#endif // DXW_NOTRACES

DWORD WINAPI extGetLogicalDrives(void) {
    DWORD DevMask;
    OutTraceDW("GetLogicalDrives:\n");
    DevMask = (*pGetLogicalDrives)();
    if (dxw.dwFlags4 & HIDECDROMEMPTY) {
        for(int i = 0; i < 32; i++) {
            DWORD DevBit;
            BOOL Vol;
            DevBit = 0x1 << i;
            if(DevMask & DevBit) {
                char RootPathName[10];
                sprintf_s(RootPathName, 4, "%c:\\", 'A' + i);
                Vol = GetVolumeInformation(RootPathName, NULL, NULL, NULL, 0, 0, 0, 0);
                OutTraceSYS("Vol=%s status=%#x\n", RootPathName, Vol);
                if(!Vol) DevMask &= ~DevBit;
            }
        }
    }
    // v2.04.84: handling of fake HD drive when FAKEHDDRIVE
    if (dxw.dwFlags10 & FAKEHDDRIVE) {
        int BitShift = dxw.FakeHDDrive - 'A';
        DevMask |= 0x1 << BitShift;
        OutTraceSYS("GetLogicalDrives: added virtual HD %c:\\\n", dxw.FakeHDDrive);
    }
    // v2.04.84: handling of fake CDROM drive when FAKECDDRIVE
    if (dxw.dwFlags10 & FAKECDDRIVE) {
        int BitShift = dxw.FakeCDDrive - 'A';
        DevMask |= 0x1 << BitShift;
        OutTraceSYS("GetLogicalDrives: added virtual CDROM %c:\\\n", dxw.FakeCDDrive);
    }
    OutTraceSYS("GetLogicalDrives: ret=%08.8X(%s)\n", DevMask, sDevArray(DevMask));
    return DevMask;
}

#ifndef DXW_NOTRACES
static char *ExplainFlagsAndAttributes(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "");
    if (c & FILE_ATTRIBUTE_ARCHIVE) strcat(eb, "FILE_ATTRIBUTE_ARCHIVE+");
    if (c & FILE_ATTRIBUTE_ENCRYPTED) strcat(eb, "FILE_ATTRIBUTE_ENCRYPTED+");
    if (c & FILE_ATTRIBUTE_HIDDEN) strcat(eb, "FILE_ATTRIBUTE_HIDDEN+");
    if (c & FILE_ATTRIBUTE_NORMAL) strcat(eb, "FILE_ATTRIBUTE_NORMAL+");
    if (c & FILE_ATTRIBUTE_OFFLINE) strcat(eb, "FILE_ATTRIBUTE_OFFLINE+");
    if (c & FILE_ATTRIBUTE_READONLY) strcat(eb, "FILE_ATTRIBUTE_READONLY+");
    if (c & FILE_ATTRIBUTE_SYSTEM) strcat(eb, "FILE_ATTRIBUTE_SYSTEM+");
    if (c & FILE_ATTRIBUTE_TEMPORARY) strcat(eb, "FILE_ATTRIBUTE_TEMPORARY+");
    if (c & FILE_FLAG_BACKUP_SEMANTICS) strcat(eb, "FILE_FLAG_BACKUP_SEMANTICS+");
    if (c & FILE_FLAG_DELETE_ON_CLOSE) strcat(eb, "FILE_FLAG_DELETE_ON_CLOSE+");
    if (c & FILE_FLAG_NO_BUFFERING) strcat(eb, "FILE_FLAG_NO_BUFFERING+");
    if (c & FILE_FLAG_OPEN_NO_RECALL) strcat(eb, "FILE_FLAG_OPEN_NO_RECALL+");
    if (c & FILE_FLAG_OPEN_REPARSE_POINT) strcat(eb, "FILE_FLAG_OPEN_REPARSE_POINT+");
    if (c & FILE_FLAG_OVERLAPPED) strcat(eb, "FILE_FLAG_OVERLAPPED+");
    if (c & FILE_FLAG_POSIX_SEMANTICS) strcat(eb, "FILE_FLAG_POSIX_SEMANTICS+");
    if (c & FILE_FLAG_RANDOM_ACCESS) strcat(eb, "FILE_FLAG_RANDOM_ACCESS+");
    //if (c & FILE_FLAG_SESSION_AWARE) strcat(eb, "FILE_FLAG_SESSION_AWARE+");
    if (c & FILE_FLAG_SEQUENTIAL_SCAN) strcat(eb, "FILE_FLAG_SEQUENTIAL_SCAN+");
    if (c & FILE_FLAG_WRITE_THROUGH) strcat(eb, "FILE_FLAG_WRITE_THROUGH+");
    if (c & SECURITY_ANONYMOUS) strcat(eb, "SECURITY_ANONYMOUS+");
    if (c & SECURITY_CONTEXT_TRACKING) strcat(eb, "SECURITY_CONTEXT_TRACKING+");
    if (c & SECURITY_DELEGATION) strcat(eb, "SECURITY_DELEGATION+");
    if (c & SECURITY_EFFECTIVE_ONLY) strcat(eb, "SECURITY_EFFECTIVE_ONLY+");
    if (c & SECURITY_IDENTIFICATION) strcat(eb, "SECURITY_IDENTIFICATION+");
    if (c & SECURITY_IMPERSONATION) strcat(eb, "SECURITY_IMPERSONATION+");
    l = strlen(eb);
    if (l > strlen("")) eb[l - 1] = 0; // delete last '+' if any
    return(eb);
}

static char *ExplainDesiredAccess(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "GENERIC_");
    if (c & GENERIC_READ) strcat(eb, "READ+");
    if (c & GENERIC_WRITE) strcat(eb, "WRITE+");
    if (c & GENERIC_EXECUTE) strcat(eb, "EXECUTE+");
    if (c & GENERIC_ALL) strcat(eb, "ALL+");
    l = strlen(eb);
    if (l > strlen("GENERIC_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}
#endif // DXW_NOTRACES

HANDLE WINAPI extCreateFileA(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                             LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                             DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    HANDLE ret;
    ApiName("CreateFileA");
    int err = 0;
    OutDebugSYS("%s: FileName=\"%s\" DesiredAccess=%#x(%s) SharedMode=%#x Disposition=%#x Flags=%#x(%s)\n",
                ApiRef, lpFileName, dwDesiredAccess, ExplainDesiredAccess(dwDesiredAccess), dwShareMode, dwCreationDisposition,
                dwFlagsAndAttributes, ExplainFlagsAndAttributes(dwFlagsAndAttributes));
    if((dxw.dwFlags3 & BUFFEREDIOFIX) && (dwFlagsAndAttributes & FILE_FLAG_NO_BUFFERING)) {
        OutTraceDW("%s: suppress FILE_FLAG_NO_BUFFERING on Filename=\"%s\"\n", ApiRef, lpFileName);
        dwFlagsAndAttributes &= ~FILE_FLAG_NO_BUFFERING;
    }
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        DWORD mapping;
        lpFileName = dxwTranslatePathA(lpFileName, &mapping);
        // if mapped on virtual CD and write access required, you should fake a no access error code
        if(mapping == DXW_FAKE_CD) {
            if(dwDesiredAccess & (GENERIC_WRITE | GENERIC_ALL)) {
                OutTraceDW("%s: simulate ERROR_ACCESS_DENIED on CD\n", ApiRef);
                // should set lasterror here?
                SetLastError(ERROR_ACCESS_DENIED);// assuming the file was there, ERROR_FILE_NOT_FOUND otherwise ?
                return INVALID_HANDLE_VALUE;
            }
        }
    }
    ret = (*pCreateFileA)(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    if(ret && (ret != INVALID_HANDLE_VALUE))
        OutDebugSYS("%s: ret=%#x\n", ApiRef, ret);
    else
        OutTraceSYS("%s ERROR: err=%d fname=\"%s\"\n", ApiRef, GetLastError(), lpFileName);
    return ret;
}

HANDLE WINAPI extCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                             LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                             DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    HANDLE ret;
    ApiName("CreateFileW");
    int err = 0;
    OutDebugSYS("%s: FileName=\"%ls\" DesiredAccess=%#x(%s) SharedMode=%#x Disposition=%#x Flags=%#x(%s)\n",
                ApiRef, lpFileName, dwDesiredAccess, ExplainDesiredAccess(dwDesiredAccess), dwShareMode, dwCreationDisposition,
                dwFlagsAndAttributes, ExplainFlagsAndAttributes(dwFlagsAndAttributes));
    if((dxw.dwFlags3 & BUFFEREDIOFIX) && (dwFlagsAndAttributes & FILE_FLAG_NO_BUFFERING)) {
        OutTraceDW("%s: suppress FILE_FLAG_NO_BUFFERING on Filename=\"%ls\"\n", ApiRef, lpFileName);
        dwFlagsAndAttributes &= ~FILE_FLAG_NO_BUFFERING;
    }
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        DWORD mapping;
        lpFileName = dxwTranslatePathW(lpFileName, &mapping);
        // if mapped on virtual CD and write access required, you should fake a no access error code
        if(mapping == DXW_FAKE_CD) {
            if(dwDesiredAccess & (GENERIC_WRITE | GENERIC_ALL)) {
                OutTraceDW("%s: simulate ERROR_ACCESS_DENIED on CD\n", ApiRef);
                // should set lasterror here?
                SetLastError(ERROR_ACCESS_DENIED);// assuming the file was there, ERROR_FILE_NOT_FOUND otherwise ?
                return INVALID_HANDLE_VALUE;
            }
        }
    }
    ret = (*pCreateFileW)(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    if(ret && (ret != INVALID_HANDLE_VALUE))
        OutDebugSYS("%s: ret=%#x\n", ApiRef, ret);
    else
        OutTraceSYS("%s ERROR: err=%d fname=\"%ls\"\n", ApiRef, GetLastError(), lpFileName);
    return ret;
}

BOOL WINAPI extDeleteFileA(LPCSTR lpFileName) {
    BOOL ret;
    ApiName("DeleteFileA");
    DWORD mapping = DXW_NO_FAKE;
    OutTraceSYS("%s: path=\"%s\"\n", ApiRef, lpFileName);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) lpFileName = dxwTranslatePathA(lpFileName, &mapping);
    if(mapping == DXW_FAKE_CD) {
        OutTraceDW("%s: simulate ERROR_ACCESS_DENIED on CD\n", ApiRef);
        // should set lasterror here?
        SetLastError(ERROR_ACCESS_DENIED);// assuming the file was there, ERROR_FILE_NOT_FOUND otherwise ?
        return FALSE;
    }
    ret = (*pDeleteFileA)(lpFileName);
    if(!ret)
        OutTraceE("%s ERROR: res=%#x err=%d\n", ApiRef, ret, GetLastError());
    return ret;
}

BOOL WINAPI extCreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecAttr) {
    BOOL ret;
    ApiName("CreateDirectoryA");
    DWORD mapping = DXW_NO_FAKE;
    OutTraceSYS("%s: path=\"%s\" secattr=%#x\n", ApiRef, lpPathName, lpSecAttr);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) lpPathName = dxwTranslatePathA(lpPathName, &mapping);
    if(mapping == DXW_FAKE_CD) {
        OutTraceDW("%s: simulate ERROR_ACCESS_DENIED on CD\n", ApiRef);
        // should set lasterror here?
        SetLastError(ERROR_ACCESS_DENIED);// assuming the file was there, ERROR_FILE_NOT_FOUND otherwise ?
        return FALSE;
    }
    ret = (*pCreateDirectoryA)(lpPathName, lpSecAttr);
    if(!ret)
        OutTraceE("%s ERROR: res=%#x err=%d\n", ApiRef, ret, GetLastError());
    return ret;
}

BOOL WINAPI extRemoveDirectoryA(LPCSTR lpPathName) {
    BOOL ret;
    ApiName("RemoveDirectoryA");
    DWORD mapping = DXW_NO_FAKE;
    OutTraceSYS("%s: path=\"%s\"\n", ApiRef, lpPathName);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) lpPathName = dxwTranslatePathA(lpPathName, &mapping);
    if(mapping == DXW_FAKE_CD) {
        OutTraceDW("%s: simulate ERROR_ACCESS_DENIED on CD\n", ApiRef);
        // should set lasterror here?
        SetLastError(ERROR_ACCESS_DENIED);// assuming the file was there, ERROR_FILE_NOT_FOUND otherwise ?
        return FALSE;
    }
    ret = (*pRemoveDirectoryA)(lpPathName);
    if(!ret)
        OutTraceE("%s ERROR: res=%#x err=%d\n", ApiRef, ret, GetLastError());
    return ret;
}

BOOL WINAPI extMoveFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName) {
    BOOL ret;
    ApiName("MoveFileA");
    DWORD mapping = DXW_NO_FAKE;
    BOOL IsaCD = FALSE;
    CHAR ExistingFileName[MAX_PATH + 1];
    OutTraceSYS("%s: from=\"%s\" to=\"%s\"\n", ApiRef, lpExistingFileName, lpNewFileName);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        lpExistingFileName = dxwTranslatePathA(lpExistingFileName, &mapping);
        strncpy(ExistingFileName, lpExistingFileName, MAX_PATH);
        lpExistingFileName = ExistingFileName;
        if(mapping == DXW_FAKE_CD) IsaCD = TRUE;
        lpNewFileName = dxwTranslatePathA(lpNewFileName, &mapping);
        if(mapping == DXW_FAKE_CD) IsaCD = TRUE;
    }
    if(IsaCD) {
        OutTraceDW("%s: simulate ERROR_ACCESS_DENIED on CD\n", ApiRef);
        // should set lasterror here?
        SetLastError(ERROR_ACCESS_DENIED);// assuming the file was there, ERROR_FILE_NOT_FOUND otherwise ?
        return FALSE;
    }
    ret = (*pMoveFileA)(lpExistingFileName, lpNewFileName);
    if(!ret)
        OutTraceE("%s ERROR: res=%#x err=%d\n", ApiRef, ret, GetLastError());
    return ret;
}

HANDLE WINAPI extFindFirstFileA(LPCTSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) {
    HANDLE ret;
    DWORD mapping;
    OutTraceSYS("FindFirstFileA: path=\"%s\"\n", lpFileName);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        lpFileName = dxwTranslatePathA(lpFileName, &mapping);
        dxw.dwFindFileMapping = mapping;
    }
    ret = (*pFindFirstFileA)(lpFileName, lpFindFileData);
    if((ret == 0) || (ret == INVALID_HANDLE_VALUE)) {
        OutTraceE("FindFirstFileA ERROR: res=%#x err=%d\n", ret, GetLastError());
        return ret;
    }
    // when making FindFirstFile on a root folder, the file sequence does NOT begins with the
    // two folders "." and "..", so if making the operation on a fake drive and on a fake root
    // you have to dkip these.
    // Found in "Precision Skateboarding" when trying to emulate the game CD.
    if(dxw.bIsRootFolder) {
        if(!strcmp(lpFindFileData->cFileName, ".")) {
            OutTraceE("FindFirstFileA skip .\n");
            (*pFindNextFileA)(ret, lpFindFileData);
        }
        if(!strcmp(lpFindFileData->cFileName, "..")) {
            OutTraceE("FindFirstFileA skip ..\n");
            (*pFindNextFileA)(ret, lpFindFileData);
        }
    }
    if(dxw.dwFindFileMapping == DXW_FAKE_CD) {
        OutTraceDW("FindFirstFileA: setting CD attributes for path=\"%s\"\n", lpFindFileData->cFileName);
        lpFindFileData->dwFileAttributes = GetFakeAttribute(lpFindFileData->dwFileAttributes);
        lpFindFileData->ftLastAccessTime.dwLowDateTime = 0;
        lpFindFileData->ftLastAccessTime.dwHighDateTime = 0;
    }
    OutDebugSYS("> filename=\"%s\"\n", lpFindFileData->cFileName);
    OutDebugSYS("> altname=\"%0.14s\"\n", lpFindFileData->cAlternateFileName);
    OutDebugSYS("> attributes=%#x\n", lpFindFileData->dwFileAttributes);
    OutDebugSYS("> filetime=%#x.%#x\n", lpFindFileData->ftCreationTime.dwLowDateTime, lpFindFileData->ftCreationTime.dwHighDateTime);
    OutDebugSYS("> lastaccesstime=%#x.%#x\n", lpFindFileData->ftLastAccessTime.dwLowDateTime, lpFindFileData->ftLastAccessTime.dwHighDateTime);
    OutDebugSYS("> lastwritetime=%#x.%#x\n", lpFindFileData->ftLastWriteTime.dwLowDateTime, lpFindFileData->ftLastWriteTime.dwHighDateTime);
    return ret;
}

BOOL WINAPI extFindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData) {
    BOOL ret;
    OutTraceSYS("FindNextFileA: hfile=%#x\n", hFindFile);
    ret = (*pFindNextFileA)(hFindFile, lpFindFileData);
    if(!ret) {
        OutTraceE("FindNextFileA ERROR: err=%d\n", GetLastError());
        return ret;
    }
    if(dxw.dwFindFileMapping == DXW_FAKE_CD) {
        OutTraceDW("FindFirstFileA: setting CD attributes for path=\"%s\"\n", lpFindFileData->cFileName);
        lpFindFileData->dwFileAttributes = GetFakeAttribute(lpFindFileData->dwFileAttributes);
        lpFindFileData->ftLastAccessTime.dwLowDateTime = 0;
        lpFindFileData->ftLastAccessTime.dwHighDateTime = 0;
    }
    OutDebugSYS("> filename=\"%s\"\n", lpFindFileData->cFileName);
    OutDebugSYS("> altname=\"%0.14s\"\n", lpFindFileData->cAlternateFileName);
    OutDebugSYS("> attributes=%#x\n", lpFindFileData->dwFileAttributes);
    OutDebugSYS("> filetime=%#x.%#x\n", lpFindFileData->ftCreationTime.dwLowDateTime, lpFindFileData->ftCreationTime.dwHighDateTime);
    OutDebugSYS("> lastaccesstime=%#x.%#x\n", lpFindFileData->ftLastAccessTime.dwLowDateTime, lpFindFileData->ftLastAccessTime.dwHighDateTime);
    OutDebugSYS("> lastwritetime=%#x.%#x\n", lpFindFileData->ftLastWriteTime.dwLowDateTime, lpFindFileData->ftLastWriteTime.dwHighDateTime);
    return ret;
}

DWORD WINAPI extGetFileAttributesA(LPCSTR lpFileName) {
    DWORD ret;
    DWORD mapping;
    OutTraceSYS("GetFileAttributesA: path=\"%s\"\n", lpFileName);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        lpFileName = dxwTranslatePathA(lpFileName, &mapping);
        //if(mapping) {
        //	OutTraceDW("GetFileAttributesA: FAKE2REAL path=\"%s\"\n", lpFileName);
        //}
    }
    ret = (*pGetFileAttributesA)(lpFileName);
    if(ret == INVALID_FILE_ATTRIBUTES) {
        OutTraceE("GetFileAttributesA ERROR: err=%d\n", GetLastError());
        return ret;
    }
    // v2.04.91: curiously, folders in CD drives don't get the FILE_ATTRIBUTE_READONLY but only
    // keep FILE_ATTRIBUTE_DIRECTORY. Fixes "Scooby Doo Case File #1".
    OutTraceSYS("GetFileAttributesA: attributes=%#x\n", ret);
    if(mapping == DXW_FAKE_CD) ret = GetFakeAttribute(ret);
    return ret;
}

BOOL WINAPI extGetVolumeInformationA(LPCSTR lpRootPathName, LPSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber,
                                     LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize) {
    BOOL ret;
    ApiName("GetVolumeInformationA");
    OutTraceSYS("%s: root=\"%s\" vnamesize=%d fsnamesize=%d\n",
                ApiRef, lpRootPathName, nVolumeNameSize, nFileSystemNameSize);
    ret = (*pGetVolumeInformationA)(lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber,
                                    lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize);
    // v2.05.51: skip control when lpRootPathName is NULL or there will be a crash. Fixes "Diggles".
    if((dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) && lpRootPathName) {
        switch(dxwVirtualDriveType(lpRootPathName)) {
        case DXW_FAKE_HD:
            // on fake HD return same info as C:
            OutTraceSYS("%s: getting C: volume info for fake HD drive %c:\n", ApiRef, dxw.FakeHDDrive);
            ret = (*pGetVolumeInformationA)("C:\\", lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber,
                                            lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize);
            ret = TRUE;
            break;
        case DXW_FAKE_CD:
            // lpMaximumComponentLength and lpFileSystemFlags borrowed from "Battle of Heroes"
            OutTraceSYS("%s: getting volume info for fake CD drive %c:\n", ApiRef, dxw.FakeCDDrive);
            PeekHotKeys();
            if(lpVolumeNameBuffer) strncpy(lpVolumeNameBuffer, dxwGetCDVolumeName(), nVolumeNameSize);
            if(lpFileSystemNameBuffer) strncpy(lpFileSystemNameBuffer, "CDFS", nFileSystemNameSize);
            if(lpMaximumComponentLength) *lpMaximumComponentLength = 110;
            if(lpFileSystemFlags) *lpFileSystemFlags = 0x1080005;
            ret = TRUE;
            break;
        }
    }
    if(!ret) {
        OutTraceSYS("%s: ERROR err=%d\n", ApiRef, GetLastError());
        return ret;
    }
    if(lpVolumeNameBuffer)
        OutTraceSYS("> vname=\"%s\"\n", lpVolumeNameBuffer);
    if(lpFileSystemNameBuffer)
        OutTraceSYS("> fsname=\"%s\"\n", lpFileSystemNameBuffer);
    if(lpVolumeSerialNumber)
        OutTraceSYS("> serial=%08X\n", *lpVolumeSerialNumber);
    if(lpMaximumComponentLength)
        OutTraceSYS("> maxcomplen=%d\n", *lpMaximumComponentLength);
    if(lpFileSystemFlags)
        OutTraceSYS("> fsflags=%08.0X\n", *lpFileSystemFlags);
    return ret;
}

#define MAXDRIVESPACE ((('Z' - 'A' + 1) * 4) + 1)

DWORD WINAPI extGetLogicalDriveStringsA(DWORD nBufferLength, LPCSTR lpBuffer) {
    DWORD ret;
    char *p;
    ApiName("GetLogicalDriveStringsA");
    OutTraceSYS("%s: buflen=%d\n", ApiRef, nBufferLength);
    char *sTmpBuffer = (char *)malloc(MAXDRIVESPACE);
    ret = (*pGetLogicalDriveStringsA)(MAXDRIVESPACE, sTmpBuffer);
    if(!ret) {
        OutTraceE("%s ERROR: err=%d\n", ApiRef, GetLastError());
        free(sTmpBuffer);
        return ret;
    } else
        OutDebugSYS("%s: actual ret=%d\n", ApiRef, ret);
    // should insert virtual CD/HD drives here ...
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        BOOL bMustAddHD = (dxw.dwFlags10 & FAKEHDDRIVE);
        BOOL bMustAddCD = (dxw.dwFlags10 & FAKECDDRIVE);
        ret = 1; // to count for the final terminator
        // reach the end of the list and verify the need to add drives
        p = (char *)sTmpBuffer;
        while(*p) {
            if(bMustAddHD && (toupper(*p) == dxw.FakeHDDrive)) bMustAddHD = FALSE;
            if(bMustAddCD && (toupper(*p) == dxw.FakeCDDrive)) bMustAddCD = FALSE;
            ret += strlen(p) + 1;
            p += strlen(p) + 1;
        }
        if(bMustAddHD) {
            sprintf(p, "%c:\\", dxw.FakeHDDrive);
            ret += 4;
            p += 4;
        }
        if(bMustAddCD) {
            sprintf(p, "%c:\\", dxw.FakeCDDrive);
            ret += 4;
            p += 4;
        }
        *p = 0;
    }
    p = (char *)sTmpBuffer;
    while(*p) {
        OutTraceSYS("> %s\n", p);
        p += strlen(p) + 1;
    }
    // copy to destination buffer for available space only
    // v2.05.32: trim if user provided too high value! Fixes "Nox" crash.
    if(lpBuffer) memcpy((char *)lpBuffer, sTmpBuffer, (nBufferLength > MAXDRIVESPACE) ? MAXDRIVESPACE : nBufferLength);
    free(sTmpBuffer);
    OutTraceSYS("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI extGetLogicalDriveStringsW(DWORD nBufferLength, LPWSTR lpBuffer) {
    DWORD ret;
    WCHAR *p;
    ApiName("GetLogicalDriveStringsW");
    OutTraceSYS("%s: buflen=%d\n", ApiRef, nBufferLength);
    WCHAR *sTmpBuffer = (WCHAR *)malloc(MAXDRIVESPACE * sizeof(WCHAR));
    ret = (*pGetLogicalDriveStringsW)(MAXDRIVESPACE, sTmpBuffer);
    if(!ret) {
        OutTraceE("%s ERROR: err=%d\n", ApiRef, GetLastError());
        free(sTmpBuffer);
        return ret;
    } else
        OutDebugSYS("%s: actual ret=%d\n", ApiRef, ret);
    // should insert virtual CD/HD drives here ...
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        BOOL bMustAddHD = (dxw.dwFlags10 & FAKEHDDRIVE);
        BOOL bMustAddCD = (dxw.dwFlags10 & FAKECDDRIVE);
        ret = 1; // to count for the final terminator
        // reach the end of the list and verify the need to add drives
        p = (WCHAR *)sTmpBuffer;
        while(*p) {
            if(bMustAddHD && (toupper(*p) == dxw.FakeHDDrive)) bMustAddHD = FALSE;
            if(bMustAddCD && (toupper(*p) == dxw.FakeCDDrive)) bMustAddCD = FALSE;
            ret += wcslen(p) + 1;
            p += wcslen(p) + 1;
        }
        if(bMustAddHD) {
            swprintf(p, MAXDRIVESPACE * sizeof(WCHAR), L"%c:\\", dxw.FakeHDDrive);
            ret += 4;
            p += 4;
        }
        if(bMustAddCD) {
            swprintf(p, MAXDRIVESPACE * sizeof(WCHAR), L"%c:\\", dxw.FakeCDDrive);
            ret += 4;
            p += 4;
        }
        *p = 0;
    }
    p = sTmpBuffer;
    while(*p) {
        OutTraceSYS("> %ls\n", p);
        p += wcslen(p) + 1;
    }
    // copy to destination buffer for available space only
    // v2.05.32: trim if user provided too high value! Fixes "Nox" crash.
    if(lpBuffer) wcsncpy(lpBuffer, sTmpBuffer, (nBufferLength > MAXDRIVESPACE) ? MAXDRIVESPACE : nBufferLength);
    free(sTmpBuffer);
    OutTraceSYS("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

HFILE WINAPI ext_lopen(LPCSTR lpPathName, int iReadWrite) {
    HFILE ret;
    DWORD mapping;
    OutTraceSYS("_lopen: lpPathName=\"%s\" rw=%#x\n", lpPathName, iReadWrite);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) lpPathName = dxwTranslatePathA(lpPathName, &mapping);
    if(mapping == DXW_FAKE_CD) {
        // tbd: fake error codes for write access on fake CD
        if(iReadWrite & (OF_READWRITE | OF_WRITE)) {
            OutTraceDW("_lopen: simulate ERROR_ACCESS_DENIED on CD\n");
            SetLastError(ERROR_ACCESS_DENIED);// assuming the file was there, ERROR_FILE_NOT_FOUND otherwise ?
            return (HFILE)INVALID_HANDLE_VALUE;
        }
    }
    ret = (*p_lopen)(lpPathName, iReadWrite);
    if(ret == HFILE_ERROR)
        OutTraceE("_lopen ERROR: err=%d\n", GetLastError());
    else
        OutDebugSYS("_lopen: res=%#x\n", ret);
    return ret;
}

HFILE WINAPI ext_lcreat(LPCSTR lpPathName, int iAttribute) {
    HFILE ret;
    DWORD mapping;
    OutTraceSYS("_lcreat: lpPathName=\"%s\" attr=%#x\n", lpPathName, iAttribute);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) lpPathName = dxwTranslatePathA(lpPathName, &mapping);
    if(mapping == DXW_FAKE_CD) {
        OutTraceDW("_lcreat: simulate ERROR_ACCESS_DENIED on CD\n");
        SetLastError(ERROR_ACCESS_DENIED);// assuming the file was there, ERROR_FILE_NOT_FOUND otherwise ?
        return (HFILE)INVALID_HANDLE_VALUE;
    }
    ret = (*p_lcreat)(lpPathName, iAttribute);
    if(ret == HFILE_ERROR)
        OutTraceE("_lcreat ERROR: err=%d\n", GetLastError());
    else
        OutDebugSYS("_lcreat: res=%#x\n", ret);
    return ret;
}

HFILE WINAPI extOpenFile(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle) {
    HFILE ret;
    DWORD mapping;
    OutTraceSYS("OpenFile: lpPathName=\"%s\" style=%#x\n", lpFileName, uStyle);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) lpFileName = dxwTranslatePathA(lpFileName, &mapping);
    if(mapping == DXW_FAKE_CD) {
        if(uStyle & (OF_CREATE | OF_DELETE | OF_READWRITE | OF_WRITE)) {
            OutTraceDW("OpenFile: simulate write access error on CD\n");
            SetLastError(ERROR_ACCESS_DENIED);// assuming the file was there, ERROR_FILE_NOT_FOUND otherwise ?
            return (HFILE)INVALID_HANDLE_VALUE;
        }
    }
    ret = (*pOpenFile)(lpFileName, lpReOpenBuff, uStyle);
    if(ret == HFILE_ERROR)
        OutTraceE("OpenFile ERROR: err=%d\n", GetLastError());
    else
        OutDebugSYS("OpenFile: res=%#x\n", ret);
    return ret;
}

HANDLE WINAPI extCreateFileMappingA(HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName) {
    HANDLE ret;
    ApiName("CreateFileMappingA");
    DWORD mapping = DXW_NO_FAKE;
    OutTraceSYS("%s: hfile=%#x protect=%#x lpName=\"%s\"\n", ApiRef, hFile, flProtect, lpName);
    // v2.05.19: lpName could be NULL, so bypass dxwTranslatePathA in this case
    if(lpName && (dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)))
        lpName = dxwTranslatePathA(lpName, &mapping);
    if(mapping == DXW_FAKE_CD) {
        // to do ....
    }
    ret = (*pCreateFileMappingA)(hFile, lpFileMappingAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);
    if(!ret)
        OutTraceE("%s ERROR: err=%d\n", ApiRef, GetLastError());
    else
        OutDebugSYS("%s: res=%#x\n", ApiRef, ret);
    return ret;
}

BOOL WINAPI extCloseHandle(HANDLE hObject) {
    BOOL ret;
    if (hObject && (hObject != (HANDLE) - 1)) {
        OutDebugDW("CloseHandle: hFile=%#x\n", hObject);
        __try {
            ret = CloseHandle(hObject);
        } __except(EXCEPTION_EXECUTE_HANDLER) {};
    } else
        OutTraceDW("CloseHandle: BYPASS hFile=%#x\n", hObject);
    return ret;
}

#ifdef TRACEIO
DWORD WINAPI extSetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod) {
    OutTraceSYS("SetFilePointer: hFile=%#x DistanceToMove=0x%lx DistanceToMoveHigh=%#x MoveMethod=%#x\n",
                hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);
    return (*pSetFilePointer)(hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);
}
#endif

#ifndef DXW_NOTRACES
static char *ExplainDebugEvent(DWORD ec) {
    char *e;
    switch(ec) {
    case EXCEPTION_DEBUG_EVENT:
        e = "EXCEPTION";
        break;
    case CREATE_THREAD_DEBUG_EVENT:
        e = "CREATE_THREAD";
        break;
    case CREATE_PROCESS_DEBUG_EVENT:
        e = "CREATE_PROCESS";
        break;
    case EXIT_THREAD_DEBUG_EVENT:
        e = "EXIT_THREAD";
        break;
    case EXIT_PROCESS_DEBUG_EVENT:
        e = "EXIT_PROCESS";
        break;
    case LOAD_DLL_DEBUG_EVENT:
        e = "LOAD_DLL";
        break;
    case UNLOAD_DLL_DEBUG_EVENT:
        e = "UNLOAD_DLL";
        break;
    case OUTPUT_DEBUG_STRING_EVENT:
        e = "OUTPUT_DEBUG";
        break;
    case RIP_EVENT:
        e = "RIP";
        break;
    default:
        e = "unknown";
        break;
    }
    return e;
}
#endif // DXW_NOTRACES

static BOOL CreateProcessDebugA(
    LPCTSTR lpApplicationName,
    LPTSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCTSTR lpCurrentDirectory,
    LPSTARTUPINFO lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
) {
    ApiName("CreateProcessA");
    BOOL res;
    DEBUG_EVENT debug_event = {0};
    char path[MAX_PATH];
    DWORD dwContinueStatus = DBG_CONTINUE;
    extern BOOL Inject(DWORD, const char *);
    LPVOID LastExceptionPtr = 0;
#ifdef LOCKINJECTIONTHREADS
    DWORD StartingCode;
    LPVOID StartAddress = 0;
    extern LPVOID GetThreadStartAddress(HANDLE);
#endif
    dwCreationFlags |= DEBUG_ONLY_THIS_PROCESS;
    res = (*pCreateProcessA)(
              lpApplicationName, lpCommandLine,
              lpProcessAttributes, lpThreadAttributes, bInheritHandles,
              dwCreationFlags, lpEnvironment,
              lpCurrentDirectory, lpStartupInfo, lpProcessInformation
          );
    OutTraceSYS("%s res=%#x\n", ApiRef, res);
    if(!res) return res;
    BOOL bContinueDebugging = TRUE;
    while(bContinueDebugging) {
        if (!WaitForDebugEvent(&debug_event, INFINITE)) break;
        OutDebugDW("%s: WaitForDebugEvent pid=%#x tid=%#x event=%#x(%s)\n",
                   ApiRef, debug_event.dwProcessId, debug_event.dwThreadId, debug_event.dwDebugEventCode, ExplainDebugEvent(debug_event.dwDebugEventCode));
        switch(debug_event.dwDebugEventCode) {
        case EXIT_PROCESS_DEBUG_EVENT:
            bContinueDebugging = false;
            break;
        case CREATE_PROCESS_DEBUG_EVENT:
            GetModuleFileName(GetModuleHandle("dxwnd"), path, MAX_PATH);
            OutTraceDW("%s: injecting path=%s\n", ApiRef, path);
            if(!Inject(lpProcessInformation->dwProcessId, path))
                OutTraceE("%s: Injection ERROR pid=%#x dll=%s\n", ApiRef, lpProcessInformation->dwProcessId, path);
#ifdef LOCKINJECTIONTHREADS
            HANDLE TargetHandle;
            extern LPVOID GetThreadStartAddress(HANDLE);
            DWORD EndlessLoop;
            EndlessLoop = 0x9090FEEB; // assembly for JMP to here, NOP, NOP
            SIZE_T BytesCount;
            TargetHandle = OpenProcess(
                               PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
                               FALSE,
                               lpProcessInformation->dwProcessId);
            if(TargetHandle) {
                StartAddress = GetThreadStartAddress(lpProcessInformation->hThread);
                OutTraceSYS("%s: StartAddress=%#x\n", ApiRef, StartAddress);
                if(StartAddress) {
                    if(!ReadProcessMemory(lpProcessInformation->hProcess, StartAddress, &StartingCode, 4, &BytesCount))
                        OutTraceSYS("%s: ReadProcessMemory error=%d\n", ApiRef, GetLastError());
                    OutTrace("CreateProcess: StartCode=%#x\n", StartingCode);
                    if(!WriteProcessMemory(lpProcessInformation->hProcess, StartAddress, &EndlessLoop, 4, &BytesCount))
                        OutTraceSYS("%s: WriteProcessMemory error=%d\n", ApiRef, GetLastError());
                }
            }
#endif
            OutTraceDW("%s: injection started\n", ApiRef, res);
            CloseHandle(debug_event.u.CreateProcessInfo.hFile);
            break;
        case EXCEPTION_DEBUG_EVENT: {
            LPEXCEPTION_DEBUG_INFO ei;
            ei = (LPEXCEPTION_DEBUG_INFO)&debug_event.u;
            OutTraceE("%s: EXCEPTION code=%#x flags=%#x addr=%#x firstchance=%#x\n",
                      ApiRef,
                      ei->ExceptionRecord.ExceptionCode,
                      ei->ExceptionRecord.ExceptionFlags,
                      ei->ExceptionRecord.ExceptionAddress,
                      debug_event.u.Exception.dwFirstChance);
            // exception twice in same address, then do not continue.
            if(LastExceptionPtr == ei->ExceptionRecord.ExceptionAddress) bContinueDebugging = FALSE;
            //if(ei->dwFirstChance == 0) bContinueDebugging = FALSE;
            LastExceptionPtr = ei->ExceptionRecord.ExceptionAddress;
        }
        bContinueDebugging = false;
        break;
        case LOAD_DLL_DEBUG_EVENT:
            //OutTrace("CreateProcess: event=%#x(%s) dll=%s address=%#x\n",
            //	debug_event.dwDebugEventCode, ExplainDebugEvent(debug_event.dwDebugEventCode),
            //	((LOAD_DLL_DEBUG_INFO *)&debug_event.u)->lpImageName, ((LOAD_DLL_DEBUG_INFO *)&debug_event.u)->lpBaseOfDll);
            CloseHandle(debug_event.u.LoadDll.hFile);
            break;
        case CREATE_THREAD_DEBUG_EVENT:
            OutDebugDW("%s: THREAD %#x\n", ApiRef, debug_event.u.CreateThread.hThread);
            break;
        case EXIT_THREAD_DEBUG_EVENT:
#ifdef LOCKINJECTIONTHREADS
            if(TargetHandle && StartAddress) {
                if(dxw.dwDFlags & FREEZEINJECTEDSON)
                    OutTraceDW("%s: FREEZEINJECTEDSON leaving son process in endless loop\n", ApiRef, GetLastError());
                else {
                    if(!WriteProcessMemory(lpProcessInformation->hProcess, StartAddress, &StartingCode, 4, &BytesCount))
                        OutTraceE("%s: WriteProcessMemory error=%d\n", ApiRef, GetLastError());
                }
                CloseHandle((HANDLE)TargetHandle);
                OutTraceDW("%s: injection terminated\n", ApiRef, res);
            }
#endif
            OutDebugDW("%s: thread exit code=%#x\n", ApiRef, debug_event.u.ExitThread.dwExitCode);
            bContinueDebugging = false;
        default:
            break;
        }
        if(bContinueDebugging) {
            ContinueDebugEvent(debug_event.dwProcessId,
                               debug_event.dwThreadId,
                               dwContinueStatus);
        } else {
            ContinueDebugEvent(debug_event.dwProcessId, debug_event.dwThreadId, DBG_CONTINUE);
            if(!DebugSetProcessKillOnExit(FALSE))
                OutTraceE("%s: DebugSetProcessKillOnExit ERROR err=%d\n", ApiRef, GetLastError());
            if(!DebugActiveProcessStop(debug_event.dwProcessId)) {
                OutTraceE("%s: DebugActiveProcessStop ERROR err=%d\n", ApiRef, GetLastError());
                MessageBox(NULL, "Error in DebugActiveProcessStop", "dxwnd", MB_OK);
            }
        }
    }
    OutTraceDW("%s: detached\n", ApiRef);
    return res;
}

static BOOL CreateProcessDebugW(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
) {
    ApiName("CreateProcessW");
    BOOL res;
    DEBUG_EVENT debug_event = {0};
    char path[MAX_PATH];
    DWORD dwContinueStatus = DBG_CONTINUE;
    extern BOOL Inject(DWORD, const char *);
    LPVOID LastExceptionPtr = 0;
#ifdef LOCKINJECTIONTHREADS
    DWORD StartingCode;
    LPVOID StartAddress = 0;
    extern LPVOID GetThreadStartAddress(HANDLE);
#endif
    dwCreationFlags |= DEBUG_ONLY_THIS_PROCESS;
    res = (*pCreateProcessW)(
              lpApplicationName, lpCommandLine,
              lpProcessAttributes, lpThreadAttributes, bInheritHandles,
              dwCreationFlags, lpEnvironment,
              lpCurrentDirectory, lpStartupInfo, lpProcessInformation
          );
    OutTraceSYS("%s res=%#x\n", ApiRef, res);
    if(!res) return res;
    BOOL bContinueDebugging = TRUE;
    while(bContinueDebugging) {
        if (!WaitForDebugEvent(&debug_event, INFINITE)) break;
        OutDebugDW("%s: WaitForDebugEvent pid=%#x tid=%#x event=%#x(%s)\n",
                   ApiRef, debug_event.dwProcessId, debug_event.dwThreadId, debug_event.dwDebugEventCode, ExplainDebugEvent(debug_event.dwDebugEventCode));
        switch(debug_event.dwDebugEventCode) {
        case EXIT_PROCESS_DEBUG_EVENT:
            bContinueDebugging = false;
            break;
        case CREATE_PROCESS_DEBUG_EVENT:
            GetModuleFileName(GetModuleHandle("dxwnd"), path, MAX_PATH);
            OutTraceDW("%s: injecting path=%s\n", ApiRef, path);
            if(!Inject(lpProcessInformation->dwProcessId, path))
                OutTraceE("%s: Injection ERROR pid=%#x dll=%s\n", ApiRef, lpProcessInformation->dwProcessId, path);
#ifdef LOCKINJECTIONTHREADS
            HANDLE TargetHandle;
            extern LPVOID GetThreadStartAddress(HANDLE);
            DWORD EndlessLoop;
            EndlessLoop = 0x9090FEEB; // assembly for JMP to here, NOP, NOP
            SIZE_T BytesCount;
            TargetHandle = OpenProcess(
                               PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
                               FALSE,
                               lpProcessInformation->dwProcessId);
            if(TargetHandle) {
                StartAddress = GetThreadStartAddress(lpProcessInformation->hThread);
                OutTraceSYS("%s: StartAddress=%#x\n", ApiRef, StartAddress);
                if(StartAddress) {
                    if(!ReadProcessMemory(lpProcessInformation->hProcess, StartAddress, &StartingCode, 4, &BytesCount))
                        OutTraceSYS("%s: ReadProcessMemory error=%d\n", ApiRef, GetLastError());
                    OutTrace("CreateProcess: StartCode=%#x\n", StartingCode);
                    if(!WriteProcessMemory(lpProcessInformation->hProcess, StartAddress, &EndlessLoop, 4, &BytesCount))
                        OutTraceSYS("%s: WriteProcessMemory error=%d\n", ApiRef, GetLastError());
                }
            }
#endif
            OutTraceDW("%s: injection started\n", ApiRef, res);
            CloseHandle(debug_event.u.CreateProcessInfo.hFile);
            break;
        case EXCEPTION_DEBUG_EVENT: {
            LPEXCEPTION_DEBUG_INFO ei;
            ei = (LPEXCEPTION_DEBUG_INFO)&debug_event.u;
            OutTraceE("%s: EXCEPTION code=%#x flags=%#x addr=%#x firstchance=%#x\n",
                      ApiRef,
                      ei->ExceptionRecord.ExceptionCode,
                      ei->ExceptionRecord.ExceptionFlags,
                      ei->ExceptionRecord.ExceptionAddress,
                      debug_event.u.Exception.dwFirstChance);
            // exception twice in same address, then do not continue.
            if(LastExceptionPtr == ei->ExceptionRecord.ExceptionAddress) bContinueDebugging = FALSE;
            //if(ei->dwFirstChance == 0) bContinueDebugging = FALSE;
            LastExceptionPtr = ei->ExceptionRecord.ExceptionAddress;
        }
        bContinueDebugging = false;
        break;
        case LOAD_DLL_DEBUG_EVENT:
            //OutTrace("CreateProcess: event=%#x(%s) dll=%s address=%#x\n",
            //	debug_event.dwDebugEventCode, ExplainDebugEvent(debug_event.dwDebugEventCode),
            //	((LOAD_DLL_DEBUG_INFO *)&debug_event.u)->lpImageName, ((LOAD_DLL_DEBUG_INFO *)&debug_event.u)->lpBaseOfDll);
            CloseHandle(debug_event.u.LoadDll.hFile);
            break;
        case CREATE_THREAD_DEBUG_EVENT:
            OutDebugDW("%s: THREAD %#x\n", ApiRef, debug_event.u.CreateThread.hThread);
            break;
        case EXIT_THREAD_DEBUG_EVENT:
#ifdef LOCKINJECTIONTHREADS
            if(TargetHandle && StartAddress) {
                if(dxw.dwDFlags & FREEZEINJECTEDSON)
                    OutTraceDW("%s: FREEZEINJECTEDSON leaving son process in endless loop\n", ApiRef, GetLastError());
                else {
                    if(!WriteProcessMemory(lpProcessInformation->hProcess, StartAddress, &StartingCode, 4, &BytesCount))
                        OutTraceE("%s: WriteProcessMemory error=%d\n", ApiRef, GetLastError());
                }
                CloseHandle((HANDLE)TargetHandle);
                OutTraceDW("%s: injection terminated\n", ApiRef, res);
            }
#endif
            OutDebugDW("%s: thread exit code=%#x\n", ApiRef, debug_event.u.ExitThread.dwExitCode);
            bContinueDebugging = false;
        default:
            break;
        }
        if(bContinueDebugging) {
            ContinueDebugEvent(debug_event.dwProcessId,
                               debug_event.dwThreadId,
                               dwContinueStatus);
        } else {
            ContinueDebugEvent(debug_event.dwProcessId, debug_event.dwThreadId, DBG_CONTINUE);
            if(!DebugSetProcessKillOnExit(FALSE))
                OutTraceE("%s: DebugSetProcessKillOnExit ERROR err=%d\n", ApiRef, GetLastError());
            if(!DebugActiveProcessStop(debug_event.dwProcessId)) {
                OutTraceE("%s: DebugActiveProcessStop ERROR err=%d\n", ApiRef, GetLastError());
                MessageBox(NULL, "Error in DebugActiveProcessStop", "dxwnd", MB_OK);
            }
        }
    }
    OutTraceDW("%s: detached\n", ApiRef);
    return res;
}

static BOOL CreateProcessSuspendedA(
    LPCTSTR lpApplicationName,
    LPTSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCTSTR lpCurrentDirectory,
    LPSTARTUPINFO lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
) {
    ApiName("CreateProcessA");
    BOOL res;
    extern BOOL Inject(DWORD, const char *);
    char StartingCode[4];
    DWORD EndlessLoop;
    EndlessLoop = 0x9090FEEB; // careful: it's BIG ENDIAN: EB FE 90 90
    DWORD BytesCount;
    DWORD OldProt;
    DWORD PEHeader[0x70];
    char dllpath[MAX_PATH];
    LPVOID StartAddress;
    HANDLE TargetHandle;
    FILE *fExe = NULL;
    BOOL bKillProcess = FALSE;
    BOOL bExeFromPath = FALSE;
    OutTraceDW("%s: appname=\"%s\" commandline=\"%s\" dir=\"%s\"\n",
               ApiRef, lpApplicationName, lpCommandLine, lpCurrentDirectory);
    // attempt to load the specified target
    res = (*pCreateProcessA)(
              lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, false,
              dwCreationFlags | CREATE_SUSPENDED, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    if (!res) {
        OutTraceE("%s(CREATE_SUSPENDED) ERROR: err=%d\n", ApiRef, GetLastError());
        res = (*pCreateProcessA)(NULL, lpCommandLine, 0, 0, false, dwCreationFlags, NULL, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
        if(!res)
            OutTraceE("%s ERROR: err=%d\n", ApiRef, GetLastError());
        return res;
    }
    while(TRUE) { // fake loop
        bKillProcess = TRUE;
        LPTSTR path;
        // locate the entry point
        TargetHandle = OpenProcess(
                           PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_SUSPEND_RESUME,
                           FALSE,
                           lpProcessInformation->dwProcessId);
        // v2.05.09: in "Zork Nemesis" both lpCommandLine and lpApplicationName are not null, but the game
        // path is in lpApplicationName, so this must take precedence
        path = (LPTSTR)lpApplicationName; //lpCommandLine;
        if(path == NULL) {
            path = lpCommandLine; //lpApplicationName;
            bExeFromPath = TRUE;
        } else {
            //if(strlen(lpCommandLine) == 0) path = lpApplicationName;
            if(strlen(lpApplicationName) == 0) path = lpCommandLine;
            bExeFromPath = TRUE;
        }
        LPTSTR sExe;
        if(bExeFromPath) {
            sExe = (LPTSTR)malloc((MAX_PATH + 1) * sizeof(CHAR));
            strcpy(sExe, path);
            if(sExe[0] == '"') {
                path = &sExe[1];
                strtok(path, "\"");
            }
            OutTraceDW("%s: exepath=\"%s\"\n", ApiRef, path);
        }
        FILE *fExe = fopen(path, "rb");
        if(fExe == NULL) {
            OutTraceE("%s: fopen %s error=%d\n", ApiRef, path, GetLastError());
            break;
        }
        // read DOS header
        if(fread((void *)PEHeader, sizeof(DWORD), 0x10, fExe) != 0x10) {
            OutTraceE("%s: fread DOSHDR error=%d\n", ApiRef, GetLastError());
            break;
        }
        OutDebugDW("%s: NT Header offset=%X\n", ApiRef, PEHeader[0xF]);
        fseek(fExe, PEHeader[0xF], 0);
        // read File header + Optional header
        if(fread((void *)PEHeader, sizeof(DWORD), 0x70, fExe) != 0x70) {
            OutTraceE("%s: fread NTHDR error=%d\n", ApiRef, GetLastError());
            break;
        }
        StartAddress = (LPVOID)(PEHeader[0xA] + PEHeader[0xD]);
        OutDebugDW("%s: AddressOfEntryPoint=%X ImageBase=%X startaddr=%X\n", ApiRef, PEHeader[0xA], PEHeader[0xD], StartAddress);
        // patch the entry point with infinite loop
        if(!VirtualProtectEx(TargetHandle, StartAddress, 4, PAGE_EXECUTE_READWRITE, &OldProt )) {
            OutTraceE("%s: VirtualProtectEx error=%d\n", ApiRef, GetLastError());
            break;
        }
        if(!ReadProcessMemory(TargetHandle, StartAddress, &StartingCode, 4, &BytesCount)) {
            OutTraceE("%s: ReadProcessMemory error=%d\n", ApiRef, GetLastError());
            break;
        }
        if(!WriteProcessMemory(TargetHandle, StartAddress, &EndlessLoop, 4, &BytesCount)) {
            OutTraceE("%s: WriteProcessMemory error=%d\n", ApiRef, GetLastError());
            break;
        }
        if(!FlushInstructionCache(TargetHandle, StartAddress, 4)) {
            OutTraceE("%s: FlushInstructionCache ERROR err=%#x\n", ApiRef, GetLastError());
            break; // error condition
        }
        // resume the main thread
        if(ResumeThread(lpProcessInformation->hThread) == (DWORD) - 1) {
            OutTraceE("%s: ResumeThread error=%d at:%d\n", ApiRef, GetLastError(), __LINE__);
            break;
        }
        // wait until the thread stuck at entry point
        CONTEXT context;
        context.Eip = (DWORD)0; // initialize to impossible value
        for ( unsigned int i = 0; i < 80 && context.Eip != (DWORD)StartAddress; ++i ) {
            // patience.
            Sleep(50);
            // read the thread context
            context.ContextFlags = CONTEXT_CONTROL;
            if(!GetThreadContext(lpProcessInformation->hThread, &context)) {
                OutTraceE("%s: GetThreadContext error=%d\n", ApiRef, GetLastError());
                break;
            }
            OutDebugDW("wait cycle %d eip=%#x\n", i, context.Eip);
        }
        if (context.Eip != (DWORD)StartAddress) {
            // wait timed out
            OutTraceE("%s: thread blocked eip=%#x addr=%#x\n", ApiRef, context.Eip, StartAddress);
            break;
        }
        // inject DLL payload into remote process
        GetFullPathName("dxwnd.dll", MAX_PATH, dllpath, NULL);
        if(!Inject(lpProcessInformation->dwProcessId, dllpath)) {
            // DXW_STRING_INJECTION
            OutTraceE("%s: Injection error: pid=%#x dll=%s\n", ApiRef, lpProcessInformation->dwProcessId, dllpath);
            break;
        }
        // pause
        if(SuspendThread(lpProcessInformation->hThread) == (DWORD) - 1) {
            OutTraceE("%s: SuspendThread error=%d\n", ApiRef, GetLastError());
            break;
        }
        // restore original entry point
        if(!WriteProcessMemory(TargetHandle, StartAddress, &StartingCode, 4, &BytesCount)) {
            OutTraceE("%s: WriteProcessMemory error=%d\n", ApiRef, GetLastError());
            break;
        }
        if(!FlushInstructionCache(TargetHandle, StartAddress, 4)) {
            OutTraceE("%s: FlushInstructionCache ERROR err=%#x\n", ApiRef, GetLastError());
            break; // error condition
        }
        // you are ready to go
        // pause and restore original entry point
        if(ResumeThread(lpProcessInformation->hThread) == (DWORD) - 1) {
            OutTraceE("%s: ResumeThread error=%d at:%d\n", ApiRef, GetLastError(), __LINE__);
            break;
        }
        bKillProcess = FALSE;
        break; // exit fake loop
    }
    // cleanup ....
    if(fExe) fclose(fExe);
    if(TargetHandle) CloseHandle(TargetHandle);
    // terminate the newly spawned process
    if(bKillProcess) {
        OutTraceDW("%s: Kill son process hproc=%#x pid=%#x\n", ApiRef, lpProcessInformation->hProcess, lpProcessInformation->dwProcessId);
        if(!TerminateProcess( lpProcessInformation->hProcess, -1 ))
            OutTraceE("%s: failed to kill hproc=%#x err=%d\n", ApiRef, lpProcessInformation->hProcess, GetLastError());
    }
    OutDebugDW("%s: resumed\n", ApiRef);
    return res;
}

static BOOL CreateProcessSuspendedW(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
) {
    ApiName("CreateProcessW");
    BOOL res;
    extern BOOL Inject(DWORD, const char *);
    char StartingCode[4];
    DWORD EndlessLoop;
    EndlessLoop = 0x9090FEEB; // careful: it's BIG ENDIAN: EB FE 90 90
    DWORD BytesCount;
    DWORD OldProt;
    DWORD PEHeader[0x70];
    char dllpath[MAX_PATH];
    LPVOID StartAddress;
    HANDLE TargetHandle;
    FILE *fExe = NULL;
    BOOL bKillProcess = FALSE;
    BOOL bExeFromPath = FALSE;
    OutTraceDW("%s: appname=\"%ls\" commandline=\"%ls\" dir=\"%ls\"\n",
               ApiRef, lpApplicationName, lpCommandLine, lpCurrentDirectory);
    // attempt to load the specified target
    res = (*pCreateProcessW)(
              lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, false,
              dwCreationFlags | CREATE_SUSPENDED, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    if (!res) {
        OutTraceE("%s(CREATE_SUSPENDED) ERROR: err=%d\n", ApiRef, GetLastError());
        res = (*pCreateProcessW)(NULL, lpCommandLine, 0, 0, false, dwCreationFlags, NULL, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
        if(!res)
            OutTraceE("%s ERROR: err=%d\n", ApiRef, GetLastError());
        return res;
    }
    while(TRUE) { // fake loop
        bKillProcess = TRUE;
        LPWSTR path;
        // locate the entry point
        TargetHandle = OpenProcess(
                           PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_SUSPEND_RESUME,
                           FALSE,
                           lpProcessInformation->dwProcessId);
        // v2.05.18: in "TechnoParrotUi" lpApplicationName is null, but the Command Line has doublequotes and arguments,
        // so you have to strip them away to get the son executable path
        path = (LPWSTR)lpApplicationName; //lpCommandLine;
        if(path == NULL) {
            path = lpCommandLine; //lpApplicationName;
            bExeFromPath = TRUE;
        } else {
            //if(strlen(lpCommandLine) == 0) path = lpApplicationName;
            if(wcslen(lpApplicationName) == 0) path = lpCommandLine;
            bExeFromPath = TRUE;
        }
        LPWSTR sExe;
        if(bExeFromPath) {
            sExe = (LPWSTR)malloc((MAX_PATH + 1) * sizeof(WCHAR));
            wcscpy(sExe, path);
            if(sExe[0] == L'"') {
                path = &sExe[1];
                wcstok(path, L"\"");
            }
            OutTraceDW("%s: exepath=\"%ls\"\n", ApiRef, path);
        }
        FILE *fExe = _wfopen(path, L"rb");
        if(fExe == NULL) {
            OutTraceE("%s: fopen \"%ls\" error=%d\n", ApiRef, path, GetLastError());
            break;
        }
        if(bExeFromPath) free(sExe);
        // read DOS header
        if(fread((void *)PEHeader, sizeof(DWORD), 0x10, fExe) != 0x10) {
            OutTraceE("%s: fread DOSHDR error=%d\n", ApiRef, GetLastError());
            break;
        }
        OutDebugDW("%s: NT Header offset=%X\n", ApiRef, PEHeader[0xF]);
        fseek(fExe, PEHeader[0xF], 0);
        // read File header + Optional header
        if(fread((void *)PEHeader, sizeof(DWORD), 0x70, fExe) != 0x70) {
            OutTraceE("%s: fread NTHDR error=%d\n", ApiRef, GetLastError());
            break;
        }
        StartAddress = (LPVOID)(PEHeader[0xA] + PEHeader[0xD]);
        OutDebugDW("%s: AddressOfEntryPoint=%X ImageBase=%X startaddr=%X\n", ApiRef, PEHeader[0xA], PEHeader[0xD], StartAddress);
        // patch the entry point with infinite loop
        if(!VirtualProtectEx(TargetHandle, StartAddress, 4, PAGE_EXECUTE_READWRITE, &OldProt )) {
            OutTraceE("%s: VirtualProtectEx error=%d\n", ApiRef, GetLastError());
            break;
        }
        if(!ReadProcessMemory(TargetHandle, StartAddress, &StartingCode, 4, &BytesCount)) {
            OutTraceE("%s: ReadProcessMemory error=%d\n", ApiRef, GetLastError());
            break;
        }
        if(!WriteProcessMemory(TargetHandle, StartAddress, &EndlessLoop, 4, &BytesCount)) {
            OutTraceE("%s: WriteProcessMemory error=%d\n", ApiRef, GetLastError());
            break;
        }
        if(!FlushInstructionCache(TargetHandle, StartAddress, 4)) {
            OutTraceE("%s: FlushInstructionCache ERROR err=%#x\n", ApiRef, GetLastError());
            break; // error condition
        }
        // resume the main thread
        if(ResumeThread(lpProcessInformation->hThread) == (DWORD) - 1) {
            OutTraceE("%s: ResumeThread error=%d at:%d\n", ApiRef, GetLastError(), __LINE__);
            break;
        }
        // wait until the thread stuck at entry point
        CONTEXT context;
        context.Eip = (DWORD)0; // initialize to impossible value
        for ( unsigned int i = 0; i < 80 && context.Eip != (DWORD)StartAddress; ++i ) {
            // patience.
            Sleep(50);
            // read the thread context
            context.ContextFlags = CONTEXT_CONTROL;
            if(!GetThreadContext(lpProcessInformation->hThread, &context)) {
                OutTraceE("%s: GetThreadContext error=%d\n", ApiRef, GetLastError());
                break;
            }
            OutDebugDW("wait cycle %d eip=%#x\n", i, context.Eip);
        }
        if (context.Eip != (DWORD)StartAddress) {
            // wait timed out
            OutTraceE("%s: thread blocked eip=%#x addr=%#x\n", ApiRef, context.Eip, StartAddress);
            break;
        }
        // inject DLL payload into remote process
        GetFullPathName("dxwnd.dll", MAX_PATH, dllpath, NULL);
        if(!Inject(lpProcessInformation->dwProcessId, dllpath)) {
            // DXW_STRING_INJECTION
            OutTraceE("%s: Injection error: pid=%#x dll=%s\n", ApiRef, lpProcessInformation->dwProcessId, dllpath);
            break;
        }
        // pause
        if(SuspendThread(lpProcessInformation->hThread) == (DWORD) - 1) {
            OutTraceE("%s: SuspendThread error=%d\n", ApiRef, GetLastError());
            break;
        }
        // restore original entry point
        if(!WriteProcessMemory(TargetHandle, StartAddress, &StartingCode, 4, &BytesCount)) {
            OutTraceE("%s: WriteProcessMemory error=%d\n", ApiRef, GetLastError());
            break;
        }
        if(!FlushInstructionCache(TargetHandle, StartAddress, 4)) {
            OutTraceE("%s: FlushInstructionCache ERROR err=%#x\n", ApiRef, GetLastError());
            break; // error condition
        }
        // you are ready to go
        // pause and restore original entry point
        if(ResumeThread(lpProcessInformation->hThread) == (DWORD) - 1) {
            OutTraceE("%s: ResumeThread error=%d at:%d\n", ApiRef, GetLastError(), __LINE__);
            break;
        }
        bKillProcess = FALSE;
        break; // exit fake loop
    }
    // cleanup ....
    if(fExe) fclose(fExe);
    if(TargetHandle) CloseHandle(TargetHandle);
    // terminate the newly spawned process
    if(bKillProcess) {
        OutTraceDW("%s: Kill son process hproc=%#x pid=%#x\n", ApiRef, lpProcessInformation->hProcess, lpProcessInformation->dwProcessId);
        if(!TerminateProcess( lpProcessInformation->hProcess, -1 ))
            OutTraceE("%s: failed to kill hproc=%#x err=%d\n", ApiRef, lpProcessInformation->hProcess, GetLastError());
    }
    OutDebugDW("%s: resumed\n", ApiRef);
    return res;
}

BOOL WINAPI extCreateProcessA(
    LPCTSTR lpApplicationName,
    LPTSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCTSTR lpCurrentDirectory,
    LPSTARTUPINFO lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
) {
    ApiName("CreateProcessA");
    BOOL res;
#ifndef DXW_NOTRACES
    char *mode =
        (dxw.dwFlags4 & SUPPRESSCHILD) ? "SUPPRESS" :
        (dxw.dwFlags5 & DEBUGSON) ? "DEBUG" :
        (dxw.dwFlags5 & INJECTSON) ? "INJECT" :
        "DEFAULT";
    OutTraceDW("%s: ApplicationName=\"%s\" CommandLine=\"%s\" CreationFlags=%#x CurrentDir=\"%s\" mode=%s\n",
               ApiRef,
               lpApplicationName ? lpApplicationName : "NULL",
               lpCommandLine ? lpCommandLine : "NULL",
               dwCreationFlags,
               lpCurrentDirectory ? lpCurrentDirectory : "NULL",
               mode);
#endif // DXW_NOTRACES
    if(dxw.dwFlags4 & SUPPRESSCHILD) {
        OutTraceDW("%s: SUPPRESS\n", ApiRef);
        return TRUE;
    }
    if(dxw.dwFlags5 & (INJECTSON | DEBUGSON)) {
        extern HANDLE hLockMutex;
        ReleaseMutex(hLockMutex);
    }
    if(dxw.dwFlags5 & DEBUGSON) {
        res = CreateProcessDebugA(
                  lpApplicationName,
                  lpCommandLine,
                  lpProcessAttributes,
                  lpThreadAttributes,
                  bInheritHandles,
                  dwCreationFlags,
                  lpEnvironment,
                  lpCurrentDirectory,
                  lpStartupInfo,
                  lpProcessInformation
              );
    } else if(dxw.dwFlags5 & INJECTSON) {
        res = CreateProcessSuspendedA(
                  lpApplicationName,
                  lpCommandLine,
                  lpProcessAttributes,
                  lpThreadAttributes,
                  bInheritHandles,
                  dwCreationFlags,
                  lpEnvironment,
                  lpCurrentDirectory,
                  lpStartupInfo,
                  lpProcessInformation
              );
    } else {
        res = (*pCreateProcessA)(
                  lpApplicationName,
                  lpCommandLine,
                  lpProcessAttributes,
                  lpThreadAttributes,
                  bInheritHandles,
                  dwCreationFlags,
                  lpEnvironment,
                  lpCurrentDirectory,
                  lpStartupInfo,
                  lpProcessInformation
              );
    }
    _if(!res) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return res;
}

BOOL WINAPI extCreateProcessW(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
) {
    ApiName("CreateProcessW");
    BOOL res;
#ifndef DXW_NOTRACES
    char *mode =
        (dxw.dwFlags4 & SUPPRESSCHILD) ? "SUPPRESS" :
        (dxw.dwFlags5 & DEBUGSON) ? "DEBUG" :
        (dxw.dwFlags5 & INJECTSON) ? "INJECT" :
        "DEFAULT";
    OutTraceDW("%s: ApplicationName=\"%ls\" CommandLine=\"%ls\" CreationFlags=%#x CurrentDir=\"%ls\" mode=%s\n",
               ApiRef,
               lpApplicationName ? lpApplicationName : L"NULL",
               lpCommandLine ? lpCommandLine : L"NULL",
               dwCreationFlags,
               lpCurrentDirectory ? lpCurrentDirectory : L"NULL",
               mode);
#endif // DXW_NOTRACES
    if(dxw.dwFlags4 & SUPPRESSCHILD) {
        OutTraceDW("%s: SUPPRESS\n", ApiRef);
        return TRUE;
    }
    if(dxw.dwFlags5 & (INJECTSON | DEBUGSON)) {
        extern HANDLE hLockMutex;
        ReleaseMutex(hLockMutex);
    }
    if(dxw.dwFlags5 & DEBUGSON) {
        res = CreateProcessDebugW(
                  lpApplicationName,
                  lpCommandLine,
                  lpProcessAttributes,
                  lpThreadAttributes,
                  bInheritHandles,
                  dwCreationFlags,
                  lpEnvironment,
                  lpCurrentDirectory,
                  lpStartupInfo,
                  lpProcessInformation
              );
    } else if(dxw.dwFlags5 & INJECTSON) {
        res = CreateProcessSuspendedW(
                  lpApplicationName,
                  lpCommandLine,
                  lpProcessAttributes,
                  lpThreadAttributes,
                  bInheritHandles,
                  dwCreationFlags,
                  lpEnvironment,
                  lpCurrentDirectory,
                  lpStartupInfo,
                  lpProcessInformation
              );
    } else {
        res = (*pCreateProcessW)(
                  lpApplicationName,
                  lpCommandLine,
                  lpProcessAttributes,
                  lpThreadAttributes,
                  bInheritHandles,
                  dwCreationFlags,
                  lpEnvironment,
                  lpCurrentDirectory,
                  lpStartupInfo,
                  lpProcessInformation
              );
    }
    _if(!res) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return res;
}

BOOL WINAPI extGetExitCodeProcess(HANDLE hProcess, LPDWORD lpExitCode) {
    BOOL res;
    ApiName("GetExitCodeProcess");
    OutTraceDW("%s: hProcess=%#x\n", ApiRef, hProcess);
    if(dxw.dwFlags4 & SUPPRESSCHILD) {
        OutTraceDW("%s: FAKE exit code=0\n", ApiRef);
        lpExitCode = 0;
        return TRUE;
    }
    res = (*pGetExitCodeProcess)(hProcess, lpExitCode);
    if(dxw.dwFlags5 & (INJECTSON | DEBUGSON)) {
        if(*lpExitCode != STILL_ACTIVE) {
            OutTraceDW("%s: locking mutex\n", ApiRef);
            extern HANDLE hLockMutex;
            WaitForSingleObject(hLockMutex, 0);
        }
    }
    OutTraceDW("%s: hProcess=%#x ExitCode=%#x res=%#x\n", ApiRef, hProcess, *lpExitCode, res);
    return res;
}

BOOL WINAPI extCheckRemoteDebuggerPresent(HANDLE hProcess, PBOOL pbDebuggerPresent) {
    BOOL ret;
    ApiName("CheckRemoteDebuggerPresent");
    if(pbDebuggerPresent) *pbDebuggerPresent = FALSE;
    ret = (hProcess == (HANDLE)0xFFFFFFFF) ? FALSE : TRUE;
    OutTraceDW("%s: hProcess=%#x ret=%#x\n", ApiRef, hProcess, ret);
    return ret;
}

UINT WINAPI extGetTempFileNameA(LPCSTR lpPathName, LPCSTR lpPrefixString, UINT uUnique, LPSTR lpTempFileName) {
    UINT ret;
    ApiName("GetTempFileNameA");
    OutTraceDW("%s: PathName=\"%s\" PrefixString=%s Unique=%d\n", ApiRef, lpPathName, lpPrefixString, uUnique);
    //if(1) {
    //	lpTempFileName = ".\\Temp1";
    //	ret = 1;
    //	OutTraceDW("%s: TempFileName=\"%s\" ret=%d\n", ApiRef, lpTempFileName, ret);
    //	return ret;
    //}
    ret = (*pGetTempFileNameA)(lpPathName, lpPrefixString, uUnique, lpTempFileName);
    if(ret == 0) {
        // GetTempFileName patch to make "Powerslide" working
        OutTraceDW("%s: ERROR err=%d at %d\n", ApiRef, GetLastError(), __LINE__);
        char sTmpDir[MAX_PATH + 1];
        GetTempPathA(sizeof(sTmpDir), sTmpDir);
        ret = (*pGetTempFileNameA)(sTmpDir, lpPrefixString, uUnique, lpTempFileName);
        _if(ret == 0) OutTraceDW("%s: ERROR err=%d PathName=\"%s\" at %d\n",
                                 ApiRef, GetLastError(), sTmpDir, __LINE__);
    }
    if(ret)
        OutTraceDW("%s: TempFileName=\"%s\" ret=%d\n", ApiRef, lpTempFileName, ret);
    return ret;
}

UINT WINAPI extGetTempFileNameW(LPCWSTR lpPathName, LPCWSTR lpPrefixString, UINT uUnique, LPWSTR lpTempFileName) {
    UINT ret;
    ApiName("GetTempFileNameW");
    OutTraceDW("%s: PathName=\"%ls\" PrefixString=%ls Unique=%d\n", ApiRef, lpPathName, lpPrefixString, uUnique);
    //if(1) {
    //	lpTempFileName = L".\\Temp1";
    //	ret = 1;
    //	OutTraceDW("%s: TempFileName=\"%ls\" ret=%d\n", ApiRef, lpTempFileName, ret);
    //	return ret;
    //}
    ret = (*pGetTempFileNameW)(lpPathName, lpPrefixString, uUnique, lpTempFileName);
    if(ret == 0) {
        // GetTempFileName patch, useless so far ...
        OutTraceDW("%s: ERROR err=%d at %d\n", ApiRef, GetLastError(), __LINE__);
        WCHAR sTmpDir[MAX_PATH + 1];
        GetTempPathW(sizeof(sTmpDir), sTmpDir);
        ret = (*pGetTempFileNameW)(sTmpDir, lpPrefixString, uUnique, lpTempFileName);
        _if(ret == 0) OutTraceDW("%s: ERROR err=%d PathName=\"%ls\" at %d\n",
                                 ApiRef, GetLastError(), sTmpDir, __LINE__);
    }
    if(ret)
        OutTraceDW("%s: TempFileName=\"%ls\" ret=%d\n", ApiRef, lpTempFileName, ret);
    return ret;
}

LPVOID WINAPI extVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) {
    // v2.03.20: handling of legacy memory segments.
    // Some games (Crusaders of Might and Magic, the demo and the GOG release) rely on the fact that the
    // program can VirtualAlloc-ate memory on certain tipically free segments (0x4000000, 0x5000000,
    // 0x6000000, 0x7000000 and 0x8000000) but when the program is hooked by DxWnd these segments could
    // be allocated to extra dlls or allocated memory.
    // The trick is ti pre-allocate this memory and free it upon error to make it certainly available to
    // the calling program.
    LPVOID ret;
    ApiName("VirtualAlloc");
    OutDebugSYS("%s: lpAddress=%#x size=%#x flag=%#x protect=%#x\n", ApiRef, lpAddress, dwSize, flAllocationType, flProtect);
    ret = (*pVirtualAlloc)(lpAddress, dwSize, flAllocationType, flProtect);
    if((ret == NULL) && lpAddress) {
        OutTraceE("%s: RECOVERY lpAddress=%#x size=%#x flag=%#x protect=%#x\n",
                  ApiRef, lpAddress, dwSize, flAllocationType, flProtect);
        if (((DWORD)lpAddress & 0xF0FFFFFF) == 0) {
            BOOL bret;
            bret = VirtualFree(lpAddress, 0x00000000, MEM_RELEASE);
            _if(!bret) OutTraceE("%s: VirtualFree MEM_RELEASE err=%d\n", ApiRef, GetLastError());
            ret = (*pVirtualAlloc)(lpAddress, dwSize, flAllocationType, flProtect);
            _if (ret == NULL) OutTraceE("%s: addr=%#x err=%d\n", ApiRef, lpAddress, GetLastError());
        }
        if (!ret) ret = (*pVirtualAlloc)((LPVOID)0x00000000, dwSize, flAllocationType, flProtect);
        _if(ret == NULL) OutTraceE("%s: addr=NULL err=%d\n", ApiRef, GetLastError());
    }
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

// WinExec: used by "Star Wars X-Wings Alliance" frontend, but fortunately it's not essential to hook it....
// used by "Daemonica", and in this case it WOULD be essential ...
UINT WINAPI extWinExec(LPCSTR lpCmdLine, UINT uCmdShow) {
    UINT ret;
    ApiName("WinExec");
    OutTraceDW("%s: lpCmdLine=\"%s\" CmdShow=%#x\n", ApiRef, lpCmdLine, uCmdShow);
    if(dxw.dwFlags4 & SUPPRESSCHILD) {
        OutTraceDW("%s: SUPPRESS\n", ApiRef);
        // from MSDN: If the function succeeds, the return value is greater than 31.
        return 32;
    }
    ret = (*pWinExec)(lpCmdLine, uCmdShow);
    if(ret < 31)
        OutTraceE("%s: ERROR err=%d ret=%#x\n", ApiRef, GetLastError(), ret);
    else
        OutTraceE("%s: OK ret=%#x\n", ApiRef, ret);
    return ret;
}

BOOL WINAPI extSetPriorityClass(HANDLE hProcess, DWORD dwPriorityClass) {
    ApiName("SetPriorityClass");
    OutTraceDW("%s: hProcess=%#x class=%#x\n", ApiRef, hProcess, dwPriorityClass);
    if(dxw.dwFlags7 & BLOCKPRIORITYCLASS) {
        OutTraceDW("%s: BLOCKED\n", ApiRef);
        return TRUE;
    }
    return (*pSetPriorityClass)(hProcess, dwPriorityClass);
}

// ---------------------------------------------------------------------
// Virtual Heap
// ---------------------------------------------------------------------

static LPVOID VHeapMin = (LPVOID)0xFFFFFFFF;
static LPVOID VHeapMax = (LPVOID)0x00000000;
static LPVOID lpLast   = (LPVOID)0x00000000;
static int iProg = 1;

#define HEAPTRACE FALSE
// FORCEVIRTUALHEAP TRUE will force going to malloc also for default heap, but then HeapFree, HeapDestroy
// calls are at risk!
// A proper usage of the flag is in combination with early-hook methods.
#define FORCEVIRTUALHEAP FALSE

LPVOID WINAPI extHeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes) {
    LPVOID ret;
    ApiName("HeapAlloc");
    OutDebugSYS("%s: heap=%#x flags=%#x bytes=%d\n", ApiRef, hHeap, dwFlags, dwBytes);
    if(HEAPTRACE) {
        ret = (*pHeapAlloc)(hHeap, dwFlags, dwBytes);
        OutDebugDW("%s: ret=%#x\n", ApiRef, ret);
        return ret;
    }
    if(FORCEVIRTUALHEAP || (((DWORD)hHeap >= 0xDEADBEEF) && ((DWORD)hHeap <= 0xDEADBEEF + iProg))) {
        ret = malloc(dwBytes);
        lpLast = 0;
        if(ret) {
            if(ret > VHeapMax) VHeapMax = ret;
            if(ret < VHeapMin) VHeapMin = ret;
        }
        if(ret)
            OutDebugSYS("%s: (virtual) ret=%#x\n", ApiRef, ret);
        else
            OutTraceE("%s: (virtual) ret=0 err=%d\n", ApiRef, GetLastError());
    } else {
        ret = (*pHeapAlloc)(hHeap, dwFlags, dwBytes);
        OutDebugSYS("%s: ret=%#x\n", ApiRef, ret);
    }
    if(ret && (dxw.dwFlags9 & NOBAADFOOD)) memset(ret, 0, dwBytes);
    return ret;
}

LPVOID WINAPI extHeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes) {
    LPVOID ret;
    ApiName("HeapReAlloc");
    OutDebugSYS("%s: heap=%#x flags=%#x mem=%#x bytes=%d\n", ApiRef, hHeap, dwFlags, lpMem, dwBytes);
    if(HEAPTRACE) return (*pHeapReAlloc)(hHeap, dwFlags, lpMem, dwBytes);
    if((lpMem >= VHeapMin) && (lpMem <= VHeapMax)) {
        ret = realloc(lpMem, dwBytes);
        lpLast = 0;
        if(ret) {
            if(ret > VHeapMax) VHeapMax = ret;
            if(ret < VHeapMin) VHeapMin = ret;
        }
        OutDebugSYS("%s: (virtual) ret=%#x\n", ApiRef, ret);
    } else {
        ret = (*pHeapReAlloc)(hHeap, dwFlags, lpMem, dwBytes);
        OutDebugSYS("%s: ret=%#x\n", ApiRef, ret);
    }
    return ret;
}

BOOL WINAPI extHeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem) {
    BOOL ret;
    ApiName("HeapFree");
    OutDebugSYS("%s: heap=%#x flags=%#x mem=%#x\n", ApiRef, hHeap, dwFlags, lpMem);
    if(HEAPTRACE) return (*pHeapFree)(hHeap, dwFlags, lpMem);
    if(lpLast == lpMem) {
        OutTraceDW("%s: (virtual) skip duplicate free\n", ApiRef);
        return 1;
    }
    __try { // v2.05.06: quick & dirty fix for HeapFree exceptions
        if((lpMem >= VHeapMin) && (lpMem <= VHeapMax)) {
            free(lpMem);
            lpLast = lpMem;
            ret = TRUE;
            OutDebugSYS("%s: (virtual) ret=%#x\n", ApiRef, ret);
        } else {
            ret = (*pHeapFree)(hHeap, dwFlags, lpMem);
            OutDebugSYS("%s: ret=%#x\n", ApiRef, ret);
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        OutTraceE("%s: exception\n", ApiRef);
        ret = TRUE;
    }
    return ret;
}

BOOL WINAPI extHeapValidate(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem) {
    BOOL ret;
    ApiName("HeapValidate");
    OutDebugSYS("%s: heap=%#x flags=%#x mem=%#x\n", ApiRef, hHeap, dwFlags, lpMem);
    if(HEAPTRACE) return (*pHeapValidate)(hHeap, dwFlags, lpMem);
    if((lpMem >= VHeapMin) && (lpMem <= VHeapMax)) {
        ret = TRUE;
        OutDebugSYS("%s: (virtual) ret=%#x\n", ApiRef, ret);
    } else {
        ret = (*pHeapValidate)(hHeap, dwFlags, lpMem);
        OutDebugSYS("%s: ret=%#x\n", ApiRef, ret);
    }
    return ret;
}

SIZE_T WINAPI extHeapCompact(HANDLE hHeap, DWORD dwFlags) {
    SIZE_T ret;
    ApiName("HeapCompact");
    OutDebugSYS("%s: heap=%#x flags=%#x\n", ApiRef, hHeap, dwFlags);
    if(HEAPTRACE) return (*pHeapCompact)(hHeap, dwFlags);
    if(((DWORD)hHeap >= 0xDEADBEEF) && ((DWORD)hHeap < 0xDEADBEEF + iProg)) {
        ret = 100000; // just a number ....
        OutDebugSYS("%s: (virtual) ret=%d\n", ApiRef, ret);
    } else {
        ret = (*pHeapCompact)(hHeap, dwFlags);
        OutDebugSYS("%s: ret=%d\n", ApiRef, ret);
    }
    return ret;
}

HANDLE WINAPI extHeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) {
    HANDLE ret;
    ApiName("HeapCreate");
    OutDebugSYS("%s: flags=%#x size(init-max)=(%d-%d)\n", ApiRef, flOptions, dwInitialSize, dwMaximumSize);
    if(HEAPTRACE) return (*pHeapCreate)(flOptions, dwInitialSize, dwMaximumSize);
    ret = (HANDLE)(0xDEADBEEF + iProg++);
    OutDebugSYS("%s: (virtual) ret=%X\n", ApiRef, ret);
    return ret;
}

HANDLE WINAPI extGetProcessHeap(void) {
    HANDLE ret;
    ApiName("GetProcessHeap");
    OutDebugSYS("%s\n", ApiRef);
    if(HEAPTRACE) return (*pGetProcessHeap)();
    if(dxw.dwFlags11 & VIRTUALPROCHEAP) ret = (HANDLE)0xDEADBEEF;
    else ret = (*pGetProcessHeap)();
    OutDebugSYS("%s: ret=%#x%s\n", ApiRef, ret, FORCEVIRTUALHEAP ? "(virtual)" : "");
    return ret;
}

BOOL WINAPI extHeapDestroy(HANDLE hHeap) {
    BOOL ret;
    ApiName("HeapDestroy");
    OutDebugSYS("%s: heap=%#x\n", ApiRef, hHeap);
    if(HEAPTRACE) return (*pHeapDestroy)(hHeap);
    if(((DWORD)hHeap >= 0xDEADBEEF) && ((DWORD)hHeap < 0xDEADBEEF + iProg))
        ret = TRUE;
    else
        ret = (*pHeapDestroy)(hHeap);
    OutDebugSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

SIZE_T WINAPI extHeapSize(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem) {
    SIZE_T ret;
    ApiName("HeapSize");
    OutDebugSYS("%s: heap=%#x flags=%#x mem=%#x\n", ApiRef, hHeap, dwFlags, lpMem);
    if(HEAPTRACE) return (*pHeapSize)(hHeap, dwFlags, lpMem);
    if(((DWORD)hHeap >= 0xDEADBEEF) && ((DWORD)hHeap <= 0xDEADBEEF + iProg)) {
        // malloc-ed areas can't have an easily measured size, try returning
        // the size of default process heap.
        // no good for "Crime Cities" - maybe it expects a growing size?
        //ret = (*pHeapSize)((*pGetProcessHeap)(), dwFlags, lpMem);
        ret = 400000;
    } else
        ret = (*pHeapSize)(hHeap, dwFlags, lpMem);
    OutDebugSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

// -- end of Heap wrappers

void WINAPI extOutputDebugStringA(LPCTSTR str) {
    CHAR c = 0;
    if(str[strlen(str) - 1] != '\n') c = '\n';
    OutTraceSYS("OutputDebugStringA: %s%c", str, c);
}

extern void WINAPI extOutputDebugStringW(LPCWSTR str) {
    CHAR c = 0;
    if(str[wcslen(str) - 1] != L'\n') c = '\n';
    OutTraceSYS("OutputDebugStringW: %Ls%c", str, c);
}

// 0: PF_FLOATING_POINT_PRECISION_ERRATA
// On a Pentium, a floating-point precision error can occur in rare circumstances.
//
// 1: PF_FLOATING_POINT_EMULATED
// Floating-point operations are emulated using a software emulator.
//
// 2: PF_COMPARE_EXCHANGE_DOUBLE
// The atomic compare and exchange operation (cmpxchg) is available.
//
// 3: PF_MMX_INSTRUCTIONS_AVAILABLE
// The MMX instruction set is available.
//
// 6: PF_XMMI_INSTRUCTIONS_AVAILABLE
// The SSE instruction set is available.
//
// 7: PF_3DNOW_INSTRUCTIONS_AVAILABLE
// The 3D-Now instruction set is available.
//
// 8: PF_RDTSC_INSTRUCTION_AVAILABLE
// The RDTSC instruction is available.
//
// 9: PF_PAE_ENABLED
// The processor is PAE-enabled. For more information, see Physical Address Extension.
// All x64 processors always return a nonzero value for this feature.
//
// 10: PF_XMMI64_INSTRUCTIONS_AVAILABLE
// The SSE2 instruction set is available.
// Windows 2000:  This feature is not supported.
//
// 12: PF_NX_ENABLED
// Data execution prevention is enabled.
// Windows XP/2000:  This feature is not supported until Windows XP with SP2 and Windows Server 2003 with SP1.
//
// 13: PF_SSE3_INSTRUCTIONS_AVAILABLE
// The SSE3 instruction set is available.
// Windows Server 2003 and Windows XP/2000:  This feature is not supported.
//
// 14: PF_COMPARE_EXCHANGE128
// The atomic compare and exchange 128-bit operation (cmpxchg16b) is available.
// Windows Server 2003 and Windows XP/2000:  This feature is not supported.
//
// 15: PF_COMPARE64_EXCHANGE128
// The atomic compare 64 and exchange 128-bit operation (cmp8xchg16) is available.
// Windows Server 2003 and Windows XP/2000:  This feature is not supported.
//
// 16: PF_CHANNELS_ENABLED
// The processor channels are enabled.
//
// 17: PF_XSAVE_ENABLED
// The processor implements the XSAVE and XRSTOR instructions.
//
// 18: PF_ARM_VFP_32_REGISTERS_AVAILABLE
// The VFP/Neon: 32 x 64bit register bank is present. This flag has the same meaning as PF_ARM_VFP_EXTENDED_REGISTERS.
//
// 20: PF_SECOND_LEVEL_ADDRESS_TRANSLATION
// Second Level Address Translation is supported by the hardware.
//
// 21: PF_VIRT_FIRMWARE_ENABLED
// Virtualization is enabled in the firmware.
//
// 22: PF_RDWRFSGSBASE_AVAILABLE
// RDFSBASE, RDGSBASE, WRFSBASE, and WRGSBASE instructions are available.
//
// 23: PF_FASTFAIL_AVAILABLE
// _fastfail() is available.
// This function returns a nonzero value if floating-point operations are emulated; otherwise, it returns zero.
//
// 24: PF_ARM_DIVIDE_INSTRUCTION_AVAILABLE
// The divide instructions are available.
//
// 25: PF_ARM_64BIT_LOADSTORE_ATOMIC
// The 64-bit load/store atomic instructions are available.
//
// 26: PF_ARM_EXTERNAL_CACHE_AVAILABLE
// The external cache is available.
//
// 27: PF_ARM_FMAC_INSTRUCTIONS_AVAILABLE
// The floating-point multiply-accumulate instruction is available.

BOOL WINAPI extIsProcessorFeaturePresent(DWORD ProcessorFeature) {
    BOOL res;
    res = (*pIsProcessorFeaturePresent)(ProcessorFeature);
    OutTraceSYS("IsProcessorFeaturePresent: feature=%#x ret=%#x\n", ProcessorFeature, res);
    return res;
}

DWORD WINAPI extGetCurrentDirectoryA(DWORD nBufferLength, LPSTR lpBuffer) {
    DWORD res;
    ApiName("GetCurrentDirectoryA");
    DWORD mapping;
    OutTraceSYS("%s: buflen=%d buf=%#x\n", ApiRef, nBufferLength, lpBuffer);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        res = (*pGetCurrentDirectoryA)(0, NULL);
        if(!res) {
            OutTraceSYS("%s: ERROR err=%d\n", ApiRef, GetLastError());
            return 0;
        }
        char *fullBuffer = (char *)malloc(res + 1); // len + terminator
        char *lpPathName;
        res = (*pGetCurrentDirectoryA)(res, fullBuffer);
        OutTraceSYS("%s: actual ret=%d buf=%s\n", ApiRef, res, fullBuffer);
        lpPathName = (char *)dxwUntranslatePathA((LPCSTR)fullBuffer, &mapping);
        strncpy(lpBuffer, lpPathName, nBufferLength);
        res = strlen(lpBuffer);
        free(fullBuffer);
    } else
        res = (*pGetCurrentDirectoryA)(nBufferLength, lpBuffer);
    OutTraceSYS("%s: ret=%d buf=%s\n", ApiRef, res, lpBuffer);
    return res;
}

DWORD WINAPI extGetCurrentDirectoryW(DWORD nBufferLength, LPWSTR lpBuffer) {
    DWORD res;
    ApiName("GetCurrentDirectoryW");
    DWORD mapping;
    OutTraceSYS("%s: buflen=%d buf=%#x\n", ApiRef, nBufferLength, lpBuffer);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        res = (*pGetCurrentDirectoryW)(0, NULL);
        if(!res) {
            OutTraceSYS("%s: ERROR err=%d\n", ApiRef, GetLastError());
            return 0;
        }
        LPWSTR fullBuffer = (LPWSTR)malloc(sizeof(TCHAR) * (res + 1)); // len + terminator, in TCHARs (2 bytes)
        LPWSTR lpPathName;
        res = (*pGetCurrentDirectoryW)(res, fullBuffer);
        OutTraceSYS("%s: actual ret=%d buf=%ls\n", ApiRef, res, fullBuffer);
        lpPathName = (LPWSTR)dxwUntranslatePathW((LPWSTR)fullBuffer, &mapping);
        wcsncpy(lpBuffer, lpPathName, nBufferLength);
        res = wcslen(lpBuffer);
        free(fullBuffer);
    } else
        res = (*pGetCurrentDirectoryW)(nBufferLength, lpBuffer);
    OutTraceSYS("%s: ret=%d buf=%ls\n", ApiRef, res, lpBuffer);
    return res;
}

BOOL WINAPI extSetCurrentDirectoryA(LPCSTR lpPathName) {
    BOOL res;
    ApiName("SetCurrentDirectoryA");
    DWORD mapping;
    OutTraceSYS("%s: path=\"%s\"\n", ApiRef, lpPathName);
    // fake drive handling
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        // try to understand if moving to a root folder BEFORE making the fake device mapping
        dxw.bIsRootFolder = FALSE;
        if(lpPathName) {
            // case of "\" root folder
            if(!strcmp(lpPathName, "\\")) dxw.bIsRootFolder = TRUE;
            // case of "X:\" root folder
            if((strlen(lpPathName) == 3) && !strcmp(lpPathName + 1, ":\\")) dxw.bIsRootFolder = TRUE;
        }
        lpPathName = dxwTranslatePathA(lpPathName, &mapping);
    }
    if(dxw.dwFlags10 & FIXFOLDERPATHS) {
        if(lpPathName[strlen(lpPathName) - 1] == '.') {
            ((UCHAR *)lpPathName)[strlen(lpPathName) - 1] = 0;
            OutTraceSYS("%s: fixed path=\"%s\"\n", ApiRef, lpPathName);
        }
        // v2.05.12: SetCurrentDirectory("") returns error 123 ERROR_INVALID_NAME - seen in "SpyCraft".
        if(strlen(lpPathName) == 0) {
            lpPathName = ".";
            OutTraceSYS("%s: fixed path=\"%s\"\n", ApiRef, lpPathName);
        }
    }
    res = (*pSetCurrentDirectoryA)(lpPathName);
    if(mapping != DXW_NO_FAKE)
        OutTraceDW("%s: moved to virtual path=\"%s\"\n", ApiRef, lpPathName);
    if(res)
        dxw.dwCurrentFolderType = mapping;
    else
        OutTraceE("%s: ERROR path=\"%s\" err=%d\n", ApiRef, lpPathName, GetLastError());
    return res;
}

BOOL WINAPI extSetCurrentDirectoryW(LPWSTR lpPathName) {
    BOOL res;
    ApiName("SetCurrentDirectoryW");
    DWORD mapping;
    OutTraceSYS("%s: path=\"%ls\"\n", ApiRef, lpPathName);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE))
        lpPathName = (LPWSTR)dxwTranslatePathW(lpPathName, &mapping);
    if(dxw.dwFlags10 & FIXFOLDERPATHS) {
        if(lpPathName[wcslen(lpPathName) - 1] == (WCHAR)'.') {
            ((WCHAR *)lpPathName)[wcslen(lpPathName) - 1] = (WCHAR)0;
            OutTraceSYS("%s: fixed path=\"%ls\"\n", ApiRef, lpPathName);
        }
        // v2.05.12: SetCurrentDirectory("") returns error 123 ERROR_INVALID_NAME.
        if(wcslen(lpPathName) == 0) {
            lpPathName = L".";
            OutTraceSYS("%s: fixed path=\"%ls\"\n", ApiRef, lpPathName);
        }
    }
    // if moving to fake drive, keep track of the drive type for later use
    res = (*pSetCurrentDirectoryW)(lpPathName);
    if(mapping != DXW_NO_FAKE)
        OutTraceDW("%s: moved to virtual path=\"%ls\"\n", ApiRef, lpPathName);
    if(res)
        dxw.dwCurrentFolderType = mapping;
    else
        OutTraceE("%s: ERROR path=\"%ls\" err=%d\n", ApiRef, lpPathName, GetLastError());
    return res;
}

// Critical Section API

#define DELAYCYCLES		0x4000
#define DELAYACTIVITY	for(int i=0; i<DELAYCYCLES; i++) {DWORD j=(*pGetTickCount)();}

void WINAPI extInitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
    ApiName("InitializeCriticalSection");
    OutTraceSYS("%s: lpcs=%#x\n", ApiRef, lpCriticalSection);
    if(dxw.dwFlags11 & MUTEX4CRITSECTION) {
        char MutexName[21];
        sprintf(MutexName, "Mx%08.8X", lpCriticalSection);
        lpCriticalSection->LockCount = (LONG)CreateMutexA(NULL, FALSE, MutexName);
        OutTraceSYS("%s: mutex name=%s handle=%#x\n", ApiRef, MutexName, lpCriticalSection->LockCount);
    } else {
        OutTraceSYS("%s: cs={lockcount=%d spincount=%d reccount=%d locksem=%#x ownthread=%#x}\n", ApiRef,
                    lpCriticalSection->LockCount,
                    lpCriticalSection->SpinCount,
                    lpCriticalSection->RecursionCount,
                    lpCriticalSection->LockSemaphore,
                    lpCriticalSection->OwningThread);
        (*pInitializeCriticalSection)(lpCriticalSection);
    }
    OutTraceSYS("%s: terminated\n", ApiRef);
}

void WINAPI extEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
    ApiName("EnterCriticalSection");
    OutDebugSYS("%s: lpcs=%#x\n", ApiRef, lpCriticalSection);
    if(dxw.dwFlags11 & DELAYCRITSECTION) DELAYACTIVITY;
    if(dxw.dwFlags11 & MUTEX4CRITSECTION) {
        OutTraceSYS("%s: mutex handle=%#x\n", ApiRef, lpCriticalSection->LockCount);
        WaitForSingleObject((HANDLE)lpCriticalSection->LockCount, INFINITE);
        //WaitForSingleObject((HANDLE)lpCriticalSection->LockCount, 1);
    } else {
        OutDebugSYS("%s: cs={lockcount=%d spincount=%d reccount=%d locksem=%#x ownthread=%#x}\n", ApiRef,
                    lpCriticalSection->LockCount,
                    lpCriticalSection->SpinCount,
                    lpCriticalSection->RecursionCount,
                    lpCriticalSection->LockSemaphore,
                    lpCriticalSection->OwningThread);
        (*pEnterCriticalSection)(lpCriticalSection);
    }
    //if(dxw.dwFlags11 & DELAYCRITSECTION) DELAYACTIVITY;
    OutDebugSYS("%s: terminated\n", ApiRef);
}

void WINAPI extLeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
    ApiName("LeaveCriticalSection");
    OutDebugSYS("%s: lpcs=%#x\n", ApiRef, lpCriticalSection);
    if(dxw.dwFlags11 & DELAYCRITSECTION) DELAYACTIVITY;
    if(dxw.dwFlags11 & MUTEX4CRITSECTION) {
        OutTraceSYS("%s: mutex handle=%#x\n", ApiRef, lpCriticalSection->LockCount);
        ReleaseMutex((HANDLE)lpCriticalSection->LockCount);
    } else {
        OutDebugSYS("%s: cs={lockcount=%d spincount=%d reccount=%d locksem=%#x ownthread=%#x}\n", ApiRef,
                    lpCriticalSection->LockCount,
                    lpCriticalSection->SpinCount,
                    lpCriticalSection->RecursionCount,
                    lpCriticalSection->LockSemaphore,
                    lpCriticalSection->OwningThread);
        (*pLeaveCriticalSection)(lpCriticalSection);
    }
    //if(dxw.dwFlags11 & DELAYCRITSECTION) DELAYACTIVITY;
    OutDebugSYS("%s: terminated\n", ApiRef);
}

void WINAPI extDeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
    ApiName("DeleteCriticalSection");
    OutTraceSYS("%s: lpcs=%#x\n", ApiRef, lpCriticalSection);
    if(dxw.dwFlags11 & MUTEX4CRITSECTION) {
        OutTraceSYS("%s: mutex handle=%#x\n", ApiRef, lpCriticalSection->LockCount);
        CloseHandle((HANDLE)lpCriticalSection->LockCount);
    } else {
        OutTraceSYS("%s: cs={lockcount=%d spincount=%d reccount=%d locksem=%#x ownthread=%#x}\n", ApiRef,
                    lpCriticalSection->LockCount,
                    lpCriticalSection->SpinCount,
                    lpCriticalSection->RecursionCount,
                    lpCriticalSection->LockSemaphore,
                    lpCriticalSection->OwningThread);
        (*pDeleteCriticalSection)(lpCriticalSection);
    }
    OutTraceSYS("%s: terminated\n", ApiRef);
}

// ===========================================================================
// Global memory handling
// ===========================================================================

BOOL WINAPI extGlobalUnlock(HGLOBAL hMem) {
    BOOL res;
    ApiName("GlobalUnlock");
    OutDebugSYS("%s: hg=%#x\n", ApiRef, hMem);
    res = (*pGlobalUnlock)(hMem);
    OutDebugSYS("%s: ret=%#x\n", ApiRef, res);
    if((dxw.dwFlags7 & FIXGLOBALUNLOCK) && (res == 1)) {
        static HGLOBAL hLastMem = NULL;
        if((hLastMem == NULL) || (hMem == hLastMem)) {
            OutTraceDW("%s: FIXED RETCODE hMem=%#x\n", ApiRef, hMem);
            res = 0;
            SetLastError(NO_ERROR);
            hLastMem = NULL;
        } else
            hLastMem = hMem;
    }
    return res;
}

#ifdef TRACEALL
LPVOID WINAPI extGlobalLock(HGLOBAL hMem) {
    LPVOID res;
    ApiName("GlobalLock");
    OutTrace("%s: hg=%#x\n", ApiRef, hMem);
    res = (*pGlobalLock)(hMem);
    OutTrace("%s: ret=%#x\n", ApiRef, res);
    return res;
}

HGLOBAL WINAPI extGlobalAlloc(UINT uFlags, SIZE_T dwBytes) {
    HGLOBAL res;
    ApiName("GlobalAlloc");
    OutTrace("%s: flags=%#x size=%d\n", ApiRef, uFlags, dwBytes);
    res = (*pGlobalAlloc)(uFlags, dwBytes);
    OutTrace("%s: ret=%#x\n", ApiRef, res);
    return res;
}

HGLOBAL WINAPI extGlobalReAlloc(HGLOBAL hMem, SIZE_T dwBytes, UINT uFlags) {
    HGLOBAL res;
    ApiName("GlobalRealloc");
    OutTrace("%s: hmem=%#x flags=%#x size=%d\n", ApiRef, hMem, uFlags, dwBytes);
    res = (*pGlobalReAlloc)(hMem, dwBytes, uFlags);
    OutTrace("%s: ret=%#x\n", ApiRef, res);
    return res;
}

HGLOBAL WINAPI extGlobalFree(HGLOBAL hMem) {
    HGLOBAL res;
    ApiName("GlobalFree");
    OutTrace("%s: hmem=%#x\n", ApiRef, hMem);
    res = (*pGlobalFree)(hMem);
    OutTrace("%s: ret=%#x\n", ApiRef, res);
    return res;
}

VOID WINAPI extRaiseException(DWORD dwExceptionCode, DWORD dwExceptionFlags, DWORD nArgs, CONST ULONG_PTR *lpArguments) {
    ApiName("RaiseException");
    OutTrace("%s: code=%Xh flags=%Xh args=%d\n", ApiRef, dwExceptionCode, dwExceptionFlags, nArgs);
    (*pRaiseException)(dwExceptionCode, dwExceptionFlags, nArgs, lpArguments);
}
#endif // TRACEALL

// -------------------- REMAPSYSFOLDERS hooks --------------------

static char *GetFakeWinDirA() {
    static char myPathA[MAX_PATH];
    GetModuleFileNameA(NULL, myPathA, MAX_PATH);
    //PathRemoveFileSpecA(myPathA);
    char *p = myPathA + strlen(myPathA);
    while((*p != '\\') && (p > myPathA)) p--;
    if(*p == '\\') *p = 0;
    strcat_s(myPathA, MAX_PATH, "\\Windows");
    return myPathA;
}

static WCHAR *GetFakeWinDirW() {
    static WCHAR myPathW[MAX_PATH];
    GetModuleFileNameW(NULL, myPathW, MAX_PATH);
    //PathRemoveFileSpecW(myPathW);
    WCHAR *p = myPathW + wcslen(myPathW);
    while((*p != L'\\') && (p > myPathW)) p--;
    if(*p == L'\\') *p = 0;
    wcscat_s(myPathW, MAX_PATH, L"\\Windows");
    return myPathW;
}

static BOOL isRelativePathA(LPCSTR path) {
    // v2.05.19: NULL path is to be considered as relative
    if(path == NULL) return TRUE;
    char *lpFolder = GetFakeWinDirA();
    if(!strncmp(path, lpFolder, strlen(lpFolder)))
        return FALSE;
    if(strlen(path) > 2) {
        if((path[1] == ':') && (path[2] == '\\')) return FALSE;
    }
    return TRUE;
}

static BOOL isRelativePathW(LPCWSTR path) {
    // v2.05.29: NULL path is to be considered as relative
    if(path == NULL) return TRUE;
    WCHAR *lpFolder = GetFakeWinDirW();
    if(!wcsncmp(path, lpFolder, wcslen(lpFolder)))
        return FALSE;
    if(wcslen(path) > 2) {
        if((path[1] == L':') && (path[2] == L'\\')) return FALSE;
    }
    return TRUE;
}

UINT WINAPI extGetWindowsDirectoryA(LPSTR lpBuffer, UINT uSize) {
    UINT ret;
    ApiName("GetWindowsDirectoryA");
    char *sFakePath = GetFakeWinDirA();
    ret = strlen(sFakePath);
    strncpy(lpBuffer, sFakePath, uSize);
    OutTraceDW("%s: returning relative path\n", ApiRef);
    return ret;
}

UINT WINAPI extGetWindowsDirectoryW(LPWSTR lpBuffer, UINT uSize) {
    UINT ret;
    ApiName("GetWindowsDirectoryW");
    WCHAR *sFakePath = GetFakeWinDirW();
    ret = wcslen(sFakePath);
    wcsncpy(lpBuffer, sFakePath, uSize);
    OutTraceDW("%s: returning relative path\n", ApiRef);
    return ret;
}

DWORD WINAPI extGetPrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName) {
    DWORD ret;
    ApiName("GetPrivateProfileStringA");
    char lpNewPath[MAX_PATH];
    OutTraceSYS("%s: app=%s key=%s default=%s size=%d filename=\"%s\"\n", ApiRef, lpAppName, lpKeyName, lpDefault, nSize, lpFileName);
    // v2.05.19: safeguard against possibly NULL lpFileName
    if((dxw.dwFlags11 & REMAPSYSFOLDERS) && isRelativePathA(lpFileName)) {
        strcpy(lpNewPath, GetFakeWinDirA());
        strcat(lpNewPath, "\\");
        strcat(lpNewPath, lpFileName ? lpFileName : "Win.ini");
        lpFileName = lpNewPath;
        OutTraceDW("%s: remapped path=\"%s\"\n", ApiRef, lpFileName);
    }
    // v2.05.19: safeguard against possibly NULL lpFileName
    if((dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) && lpFileName)
        lpFileName = dxwTranslatePathA(lpFileName, NULL);
    ret = (*pGetPrivateProfileStringA)(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
    OutTraceSYS("%s: ret=%#x retstring=\"%s\"\n", ApiRef, ret, lpReturnedString);
    return ret;
}

DWORD WINAPI extGetPrivateProfileStringW(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName) {
    DWORD ret;
    ApiName("GetPrivateProfileStringW");
    WCHAR lpNewPath[MAX_PATH];
    OutTraceSYS("%s: app=%ls key=%ls default=%ls size=%d filename=\"%ls\"\n", ApiRef, lpAppName, lpKeyName, lpDefault, nSize, lpFileName);
    // v2.05.19: safeguard against possibly NULL lpFileName
    if((dxw.dwFlags11 & REMAPSYSFOLDERS) && isRelativePathW(lpFileName)) {
        wcscpy(lpNewPath, GetFakeWinDirW());
        wcscat(lpNewPath, L"\\");
        wcscat(lpNewPath, lpFileName ? lpFileName : L"Win.ini");
        lpFileName = lpNewPath;
        OutTraceDW("%s: remapped path=\"%ls\"\n", ApiRef, lpFileName);
    }
    // v2.05.19: safeguard against possibly NULL lpFileName
    if((dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) && lpFileName)
        lpFileName = dxwTranslatePathW(lpFileName, NULL);
    ret = (*pGetPrivateProfileStringW)(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
    OutTraceSYS("%s: ret=%#x retstring=\"%ls\"\n", ApiRef, ret, lpReturnedString);
    return ret;
}

DWORD WINAPI extWritePrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPSTR lpString, LPCSTR lpFileName) {
    DWORD ret;
    ApiName("WritePrivateProfileStringA");
    char lpNewPath[MAX_PATH];
    OutTraceSYS("%s: app=%s key=%s val=%s filename=\"%s\"\n", ApiRef, lpAppName, lpKeyName, lpString, lpFileName);
    if(isRelativePathA(lpFileName)) {
        strcpy(lpNewPath, GetFakeWinDirA());
        strcat(lpNewPath, "\\");
        strcat(lpNewPath, lpFileName); // should I set to Win.ini if NULL??
        lpFileName = lpNewPath;
        OutTraceDW("%s: remapped path=\"%s\"\n", ApiRef, lpFileName);
    }
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        DWORD mapping;
        lpFileName = dxwTranslatePathA(lpFileName, &mapping);
        // what if we try to write on a CD ini file?
        if(mapping == DXW_FAKE_CD) return TRUE;
    }
    ret = (*pWritePrivateProfileStringA)(lpAppName, lpKeyName, lpString, lpFileName);
    return ret;
}

DWORD WINAPI extGetProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize) {
    DWORD ret;
    ApiName("GetProfileStringA");
    char sPath[MAX_PATH];
    OutTraceSYS("%s: app=%s key=%s default=%s size=%d\n", ApiRef, lpAppName, lpKeyName, lpDefault, nSize);
    strcpy_s(sPath, GetFakeWinDirA());
    strcat_s(sPath, "\\Win.ini");
    OutTraceDW("%s: remapped path=\"%s\"\n", ApiRef, sPath);
    ret = (*pGetPrivateProfileStringA)(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, sPath);
    OutTraceSYS("%s: ret=%#x retstring=\"%s\"\n", ApiRef, ret, lpReturnedString);
    return ret;
}

DWORD WINAPI extWriteProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPSTR lpString) {
    DWORD ret;
    ApiName("WriteProfileStringA");
    char sPath[MAX_PATH];
    OutTraceSYS("%s: app=%s key=%s val=%s\n", ApiRef, lpAppName, lpKeyName, lpString);
    strcpy_s(sPath, GetFakeWinDirA());
    strcat_s(sPath, "\\Win.ini");
    OutTraceDW("%s: remapped path=\"%s\"\n", ApiRef, sPath);
    ret = (*pWritePrivateProfileStringA)(lpAppName, lpKeyName, lpString, sPath);
    return ret;
}

UINT WINAPI extGetPrivateProfileIntA(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault, LPCTSTR lpFileName) {
    DWORD ret;
    ApiName("GetPrivateProfileIntA");
    char lpNewPath[MAX_PATH];
    OutTraceSYS("%s: app=%s key=%s default=%d filename=\"%s\"\n", ApiRef, lpAppName, lpKeyName, nDefault, lpFileName);
    // v2.05.19: safeguard against possibly NULL lpFileName
    if((dxw.dwFlags11 & REMAPSYSFOLDERS) && isRelativePathA(lpFileName)) {
        strcpy(lpNewPath, GetFakeWinDirA());
        strcat(lpNewPath, "\\");
        strcat(lpNewPath, lpFileName ? lpFileName : "Win.ini");
        lpFileName = lpNewPath;
        OutTraceDW("%s: remapped path=\"%s\"\n", ApiRef, lpFileName);
    }
    // v2.05.19: safeguard against possibly NULL lpFileName
    if((dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) && lpFileName)
        lpFileName = dxwTranslatePathA(lpFileName, NULL);
    ret = (*pGetPrivateProfileIntA)(lpAppName, lpKeyName, nDefault, lpFileName);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

UINT WINAPI extGetPrivateProfileIntW(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault, LPCWSTR lpFileName) {
    DWORD ret;
    ApiName("GetPrivateProfileIntW");
    WCHAR lpNewPath[MAX_PATH];
    OutTraceSYS("%s: app=%ls key=%ls default=%d filename=\"%ls\"\n", ApiRef, lpAppName, lpKeyName, nDefault, lpFileName);
    // v2.05.19: safeguard against possibly NULL lpFileName
    if((dxw.dwFlags11 & REMAPSYSFOLDERS) && isRelativePathW(lpFileName)) {
        wcscpy(lpNewPath, GetFakeWinDirW());
        wcscat(lpNewPath, L"\\");
        wcscat(lpNewPath, lpFileName ? lpFileName : L"Win.ini");
        lpFileName = lpNewPath;
        OutTraceDW("%s: remapped path=\"%ls\"\n", ApiRef, lpFileName);
    }
    // v2.05.19: safeguard against possibly NULL lpFileName
    if((dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) && lpFileName)
        lpFileName = dxwTranslatePathW(lpFileName, NULL);
    ret = (*pGetPrivateProfileIntW)(lpAppName, lpKeyName, nDefault, lpFileName);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

UINT WINAPI extGetProfileIntA(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault) {
    DWORD ret;
    ApiName("GetProfileIntA");
    char sPath[MAX_PATH];
    OutTraceSYS("%s: app=%s key=%s default=%d\n", ApiRef, lpAppName, lpKeyName, nDefault);
    strcpy_s(sPath, MAX_PATH, GetFakeWinDirA());
    strcat_s(sPath, MAX_PATH, "\\Win.ini");
    OutTraceDW("%s: remapped path=\"%s\"\n", ApiRef, sPath);
    ret = (*pGetPrivateProfileIntA)(lpAppName, lpKeyName, nDefault, sPath);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

UINT WINAPI extGetProfileIntW(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault) {
    DWORD ret;
    ApiName("GetProfileIntW");
    WCHAR sPath[MAX_PATH];
    OutTraceSYS("%s: app=%ls key=%ls default=%d\n", ApiRef, lpAppName, lpKeyName, nDefault);
    wcscpy_s(sPath, MAX_PATH, GetFakeWinDirW());
    wcscat_s(sPath, MAX_PATH, L"\\Win.ini");
    OutTraceDW("%s: remapped path=\"%s\"\n", ApiRef, sPath);
    ret = (*pGetPrivateProfileIntW)(lpAppName, lpKeyName, nDefault, sPath);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

#ifdef TRACEALL
DWORD WINAPI extGetFullPathNameA(LPCSTR lpFileName, DWORD nBufferLength, LPSTR lpBuffer, LPSTR *lpFilePart) {
    DWORD ret;
    ApiName("GetFullPathNameA");
    OutTraceSYS("%s: filename=%s buflen=%d buf=%#x filepart=%#x\n", ApiRef, lpFileName, nBufferLength, lpBuffer, lpFilePart);
    ret = (*pGetFullPathNameA)(lpFileName, nBufferLength, lpBuffer, lpFilePart);
    if(ret) {
        OutTraceSYS("%s: ret=%d fullpath=\"%s\" part=\"%s\"\n", ApiRef, ret,
                    lpBuffer ? lpBuffer : "",
                    lpFilePart ? *lpFilePart : ""
                   );
    } else
        OutTraceE("%s: ERROR err=%d", ApiRef, GetLastError());
    return ret;
}
#endif

BOOL WINAPI extSetThreadLocale(LCID Locale) {
    BOOL ret;
    ApiName("SetThreadLocale");
    OutTraceSYS("%s: locale=%#x\n", ApiRef, Locale);
    ret = (*pSetThreadLocale)(Locale);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

BOOL WINAPI extIsDBCSLeadByte(BYTE testchar) {
    BOOL ret;
    ApiName("IsDBCSLeadByte");
    ret = IsDBCSLeadByteEx(dxw.CodePage, testchar);
    OutTraceLOC("%s: char=%#x ret=%#x\n", ApiRef, testchar, ret);
    return ret;
}

UINT WINAPI extGetACP() {
    OutTraceLOC("GetACP: cp=%d\n", dxw.CodePage);
    return dxw.CodePage;
}

UINT WINAPI extGetOEMCP() {
    OutTraceLOC("GetOEMCP: cp=%d\n", dxw.CodePage);
    return dxw.CodePage;
}

#ifndef DXW_NOTRACES
static char *ExplainLCFlags(DWORD f) {
    static char eb[256];
    unsigned int l;
    eb[0] = 0;
    if (f & MB_PRECOMPOSED) strcat(eb, "MB_PRECOMPOSED+");
    if (f & MB_COMPOSITE) strcat(eb, "MB_COMPOSITE+");
    if (f & MB_USEGLYPHCHARS) strcat(eb, "MB_USEGLYPHCHARS+");
    if (f & MB_ERR_INVALID_CHARS) strcat(eb, "MB_ERR_INVALID_CHARS+");
    if (f & WC_COMPOSITECHECK) strcat(eb, "WC_COMPOSITECHECK+");
    if (f & WC_DISCARDNS) strcat(eb, "WC_DISCARDNS+");
    if (f & WC_SEPCHARS) strcat(eb, "WC_SEPCHARS+");
    if (f & WC_DEFAULTCHAR) strcat(eb, "WC_DEFAULTCHAR+");
    if (f & WC_ERR_INVALID_CHARS) strcat(eb, "WC_ERR_INVALID_CHARS+");
    if (f & WC_NO_BEST_FIT_CHARS) strcat(eb, "WC_NO_BEST_FIT_CHARS+");
    l = strlen(eb);
    if (l) eb[l - 1] = 0; // delete last '+' if any
    return(eb);
}
#endif

int WINAPI extMultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar) {
    int ret;
    UINT dwCodePage;
    dwCodePage = (CodePage >= CP_UTF7/*support UTF-7 UTF-8*/) ? CodePage : dxw.CodePage;
    ret = (*pMultiByteToWideChar)(dwCodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
#ifndef DXW_NOTRACES
    if(IsDebugLOC) {
        OutTraceLOC("MultiByteToWideChar: cp=%d flags=%#x(%s) ret=%d len=%d wcstr=%#x\n",
                    CodePage, dwFlags, ExplainLCFlags(dwFlags), ret, cbMultiByte, lpWideCharStr);
        OutTraceLOC("< mbstr=\"%.*s\"\n", cbMultiByte, lpMultiByteStr);
        if((DWORD)lpWideCharStr) {
            if(cbMultiByte == -1)
                OutTraceLOC("> wcstr=\"%ls\"\n", lpWideCharStr);
            else
                OutTraceLOC("> wcstr=\"%.*ls\"\n", ret, lpWideCharStr);
        }
    }
#endif // DXW_NOTRACES
    return ret;
}

int WINAPI extWideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar) {
    int ret;
    UINT dwCodePage;
    dwCodePage = (CodePage >= CP_UTF7/*support UTF-7 UTF-8*/) ? CodePage : dxw.CodePage;
    ret = (*pWideCharToMultiByte)(dwCodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
#ifndef DXW_NOTRACES
    if(IsDebugLOC) {
        OutTraceLOC("WideCharToMultiByte: cp=%d flags=%#x(%s) ret=%d len=%d mbstr=%#x\n",
                    CodePage, dwFlags, ExplainLCFlags(dwFlags), ret, cchWideChar, lpMultiByteStr);
        OutTraceLOC("< wcstr=\"%.*ls\"\n", cchWideChar, lpWideCharStr);
        if((DWORD)lpMultiByteStr) {
            if(cchWideChar == -1)
                OutTraceLOC("> mbstr=\"%s\"\n", lpMultiByteStr);
            else
                OutTraceLOC("> mbstr=\"%.*s\"\n", ret, lpMultiByteStr);
        }
    }
#endif // DXW_NOTRACES
    return ret;
}

#ifndef DXW_NOTRACES
char *ResNames[] = {
    "NULL", "CURSOR", "BITMAP", "ICON", "MENU", "DIALOG", "STRING",
    "FONTDIR", "FONT", "ACCELERATOR", "RCDATA", "MESSAGETABLE", "R12", "R13",
    "R14", "R15", "VERSION", "DLGINCLUDE", "R18", "PLUGPLAY", "VXD",
    "ANICURSOR", "ANIICON", "HTML"
};
#endif

HRSRC WINAPI extFindResourceExA(HMODULE hModule, LPCSTR lpType, LPCSTR lpName, WORD wLanguage) {
    HRSRC hrsrc;
    ApiName("FindResourceExA");
    BOOL isStrType = ((DWORD_PTR)lpType & WM_CLASSMASK);
    BOOL isStrName = ((DWORD_PTR)lpName & WM_CLASSMASK);
#ifndef DXW_NOTRACES
    if(IsTraceLOC) {
        char hexType[10 + 1];
        char hexName[10 + 1];
        if(!isStrType) sprintf(hexType, "0x%04.4x", (DWORD)lpType);
        if(((DWORD)lpType) <= (DWORD)RT_HTML) strcpy(hexType, ResNames[(DWORD)lpType]);
        if(!isStrName) sprintf(hexName, "0x%04.4x", (DWORD)lpName);
        OutTrace("%s: hmodule=%#x type=%s name=%s lang=%d\n",
                 ApiRef, hModule,
                 isStrType ? lpType : hexType,
                 isStrName ? lpName : hexName,
                 wLanguage);
    }
#endif // DXW_NOTRACES
    // warning: better leave wLanguage unaltered when not set or some resources won't be found.
    // example: "Tang Poetry II"
    if(dxw.Language) wLanguage = (WORD)dxw.Language;
    LPCWSTR lpTypeW = isStrType ? MultiByteToWideCharInternal(lpType) : (LPCWSTR)lpType;
    LPCWSTR lpNameW = isStrName ? MultiByteToWideCharInternal(lpName) : (LPCWSTR)lpName;
    hrsrc = FindResourceExW(hModule, lpTypeW, lpNameW, wLanguage);
    if (isStrName) FreeStringInternal((LPVOID)lpNameW);
    if (isStrType) FreeStringInternal((LPVOID)lpTypeW);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, hrsrc);
    return hrsrc;
}

// ??? for all the following 7 calls ntlea always returns the Locale ID
// also when the Language ID is requested !!!

LANGID WINAPI extGetUserDefaultUILanguage(void) {
    OutTraceLOC("GetUserDefaultUILanguage: ret=%d\n", dxw.Language);
    return dxw.Language;
}

LANGID WINAPI extGetSystemDefaultUILanguage(void) {
    OutTraceLOC("GetSystemDefaultUILanguage: ret=%d\n", dxw.Language);
    return dxw.Language;
}

LANGID WINAPI extGetSystemDefaultLangID(void) {
    OutTraceLOC("GetSystemDefaultLangID: ret=%d\n", dxw.Language);
    return dxw.Language;
}

LANGID WINAPI extGetUserDefaultLangID(void) {
    OutTraceLOC("GetUserDefaultLangID: ret=%d\n", dxw.Language);
    return dxw.Language;
}

LCID WINAPI extGetThreadLocale(void) {
    OutTraceLOC("GetThreadLocale: ret=%d\n", dxw.Language);
    return dxw.Language;
}

LCID WINAPI extGetSystemDefaultLCID(void) {
    OutTraceLOC("GetSystemDefaultLCID: ret=%d\n", dxw.Language);
    return dxw.Language;
}

LCID WINAPI extGetUserDefaultLCID(void) {
    OutTraceLOC("GetUserDefaultLCID: ret=%d\n", dxw.Language);
    return dxw.Language;
}

BOOL WINAPI extGetStringTypeA(LCID Locale, DWORD dwInfoType, LPCSTR lpSrcStr, int cchSrc, LPDWORD lpCharType) {
    BOOL ret;
    OutTraceSYS("GetStringTypeA: locale=%d infotype=%#x str=(%d)\"%s\"\n",
                Locale, dwInfoType, cchSrc, lpSrcStr);
    switch(Locale) {
    case LOCALE_SYSTEM_DEFAULT:
    case LOCALE_USER_DEFAULT:
    case LOCALE_CUSTOM_DEFAULT:
    case LOCALE_CUSTOM_UI_DEFAULT:
    case LOCALE_CUSTOM_UNSPECIFIED:
        OutTraceLOC("GetStringTypeA: locale=%d->%d str=(%d)\"%s\"\n",
                    Locale, dxw.CodePage, cchSrc, lpSrcStr);
        Locale = dxw.CodePage;
        break;
    }
    ret = (*pGetStringTypeA)(Locale, dwInfoType, lpSrcStr, cchSrc, lpCharType);
    return ret;
}

int WINAPI extLCMapStringA(LCID Locale, DWORD dwMapFLags, LPCSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest) {
    int ret;
    OutTraceSYS("LCMapStringA: locale=%d flags=%#x str=(%d)\"%s\" dest=(%d)\n",
                Locale, dwMapFLags, cchSrc, lpSrcStr, cchDest);
    switch(Locale) {
    case LOCALE_SYSTEM_DEFAULT:
    case LOCALE_USER_DEFAULT:
    case LOCALE_CUSTOM_DEFAULT:
    case LOCALE_CUSTOM_UI_DEFAULT:
    case LOCALE_CUSTOM_UNSPECIFIED:
        OutTraceLOC("LCMapStringA: locale=%d->%d str=(%d)\"%s\"\n",
                    Locale, dxw.CodePage, cchSrc, lpSrcStr);
        Locale = dxw.CodePage;
        break;
    }
    ret = (*pLCMapStringA)(Locale, dwMapFLags, lpSrcStr, cchSrc, lpDestStr, cchDest);
    return ret;
}

#define ACCESS_FROM_CTL_CODE(c) (((DWORD)(c & 0x0000c000)) >> 14)
#define FUNCTION_FROM_CTL_CODE(c) (((DWORD)(c & 0x00003ffc)) >> 2)
#ifndef IOCTL_CDROM_READ_TOC
#define IOCTL_CDROM_READ_TOC 0x24000
#endif

#ifndef DXW_NOTRACES
static char *sDeviceType(DWORD dev) {
    char *devices[] = {
        "BEEP",
        "CD_ROM",
        "CD_ROM_FILE_SYSTEM",
        "CONTROLLER",
        "DATALINK",
        "DFS",
        "DISK",
        "DISK_FILE_SYSTEM",
        "FILE_SYSTEM",
        "INPORT_PORT",
        "KEYBOARD",
        "MAILSLOT",
        "MIDI_IN",
        "MIDI_OUT",
        "MOUSE",
        "MULTI_UNC_PROVIDER",
        "NAMED_PIPE",
        "NETWORK",
        "NETWORK_BROWSER",
        "NETWORK_FILE_SYSTEM",
        "NULL",
        "PARALLEL_PORT",
        "PHYSICAL_NETCARD",
        "PRINTER",
        "SCANNER",
        "SERIAL_MOUSE_PORT",
        "SERIAL_PORT",
        "SCREEN",
        "SOUND",
        "STREAMS",
        "TAPE",
        "TAPE_FILE_SYSTEM",
        "TRANSPORT",
        "UNKNOWN",
        "VIDEO",
        "VIRTUAL_DISK",
        "WAVE_IN",
        "WAVE_OUT",
        "8042_PORT",
        "NETWORK_REDIRECTOR",
        "BATTERY",
        "BUS_EXTENDER",
        "MODEM",
        "VDM",
        "MASS_STORAGE",
        "SMB",
        "KS",
        "CHANGER",
        "SMARTCARD",
        "ACPI",
        "DVD",
        "FULLSCREEN_VIDEO",
        "DFS_FILE_SYSTEM",
        "DFS_VOLUME",
        "SERENUM",
        "TERMSRV",
        "KSEC",
        "FIPS",
        "INFINIBAND",
        "VMBUS",
        "CRYPT_PROVIDER",
        "WPD",
        "BLUETOOTH",
    };
    if((dev > 0) && (dev <= FILE_DEVICE_BLUETOOTH)) return devices[dev - 1];
    return ("unknown");
}

static char *sAccess(DWORD acc) {
    char *accesses[] = {
        "ANY_ACCESS",
        "READ_ACCESS",
        "WRITE_ACCESS",
        "READ+WRITE_ACCESS"
    };
    return accesses[acc & 0x3];
}

static void DumpIOControlCode(ApiArg, DWORD ctrlCode) {
    DWORD dwDeviceType, dwFunction, dwMethod, dwAccess;
    dwDeviceType = DEVICE_TYPE_FROM_CTL_CODE(ctrlCode);
    dwFunction = FUNCTION_FROM_CTL_CODE(ctrlCode);
    dwMethod = METHOD_FROM_CTL_CODE(ctrlCode);
    dwAccess = ACCESS_FROM_CTL_CODE(ctrlCode);
    OutTrace("%s: code {DeviceType=%#x(FILE_DEVICE_%s) Function=%#x Method=%#x Access=%#x(%s)}\n",
             ApiRef,
             dwDeviceType, sDeviceType(dwDeviceType),
             dwFunction, dwMethod,
             dwAccess, sAccess(dwAccess));
}

static char *sKnownCode(DWORD code) {
    char *ret = "";
    switch(code) {
    case IOCTL_SCSI_PASS_THROUGH:
        ret = "IOCTL_SCSI_PASS_THROUGH";
        break;
    case IOCTL_SCSI_PASS_THROUGH_DIRECT:
        ret = "IOCTL_SCSI_PASS_THROUGH_DIRECT";
        break; // Heaven & Hell Securom
    case IOCTL_CDROM_READ_TOC:
        ret = "IOCTL_CDROM_READ_TOC";
        break;
    case IOCTL_STORAGE_CHECK_VERIFY:
        ret = "IOCTL_STORAGE_CHECK_VERIFY";
        break;
    case IOCTL_STORAGE_CHECK_VERIFY2:
        ret = "IOCTL_STORAGE_CHECK_VERIFY2";
        break;
    case IOCTL_STORAGE_MEDIA_REMOVAL:
        ret = "IOCTL_STORAGE_MEDIA_REMOVAL";
        break;
    case IOCTL_DISK_MEDIA_REMOVAL:
        ret = "IOCTL_DISK_MEDIA_REMOVAL";
        break; // Rugrats Adventure Game
    }
    return ret;
}
#endif // DXW_NOTRACES

BOOL WINAPI extDeviceIoControl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped) {
    BOOL ret;
    ApiName("DeviceIoControl");
    OutTraceSYS("%s: hdevice=%#x code=%#x(%s) insize=%d outsize=%d\n",
                ApiRef, hDevice, dwIoControlCode, sKnownCode(dwIoControlCode), nInBufferSize, nOutBufferSize);
#ifndef DXW_NOTRACES
    if(IsDebugSYS) {
        DumpIOControlCode(ApiRef, dwIoControlCode);
        if(IsTraceHex) HexTrace((LPBYTE)lpInBuffer, nInBufferSize);
    }
#endif // DXW_NOTRACES
    if(dxw.dwFlags12 & LOCKCDTRAY) {
        switch (dwIoControlCode) {
        case IOCTL_STORAGE_EJECT_MEDIA:
            OutTraceDW("%s: prevent EJECT media\n", ApiRef);
            return TRUE;
            break;
        case IOCTL_STORAGE_LOAD_MEDIA:
            OutTraceDW("%s: prevent LOAD media\n", ApiRef);
            return TRUE;
            break;
        default:
            break;
        }
    }
    ret = (*pDeviceIoControl)(hDevice, dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned, lpOverlapped);
    if(ret) {
        //OutTraceSYS("%s: OK retbytes=%d overlapped=%d\n", ApiRef, *lpBytesReturned, *lpOverlapped);
        OutTraceSYS("%s: OK outsize=%d\n", ApiRef, lpBytesReturned ? *lpBytesReturned : 0);
#ifndef DXW_NOTRACES
        if(IsDebugSYS && IsTraceHex) {
            if(lpBytesReturned && *lpBytesReturned) {
                OutTrace("> OutBuffer:\n");
                HexTrace((LPBYTE)lpOutBuffer, *lpBytesReturned);
            }
            if(lpOverlapped) {
                OutTrace("> Overlapped:\n");
                HexTrace((LPBYTE)lpOverlapped, sizeof(OVERLAPPED));
            }
        }
#endif // DXW_NOTRACES
    } else
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    if(ret && (dwIoControlCode == IOCTL_CDROM_READ_TOC)) {
        PCDROM_TOC pToc = (PCDROM_TOC)lpOutBuffer;
        PTRACK_DATA pTrack = pToc->TrackData;
        int nTracks;
        OutTrace("TOC: len=%d Track:(first=%d last=%d)\n",
                 (WORD)pToc->Length, pToc->FirstTrack, pToc->LastTrack);
        nTracks = pToc->LastTrack - pToc->FirstTrack;
        if(nTracks > 0) for(int i = 0; i < nTracks; i++) {
                OutTrace("Track %d: ctrl=%#x adr=%#x num=%d address=%#x\n",
                         i + 1, pTrack->Control, pTrack->Adr, pTrack->TrackNumber, pTrack->Address);
                pTrack++;
            }
    }
    // Rugrats Adventure Game - ???
    //if(!ret && (dwIoControlCode == IOCTL_DISK_MEDIA_REMOVAL)){
    //	// pretend it worked ...
    //	OutTraceDW("%s: simulate IOCTL_DISK_MEDIA_REMOVAL\n", ApiRef);
    //	ret = TRUE;
    //}
    return ret;
}

// ==== Fake Global Atom wrappers

ATOM WINAPI extGlobalDeleteAtom(ATOM nAtom) {
    ApiName("GlobalDeleteAtom");
    static int Kount = 0;
    OutTraceSYS("%s: atom=%#x\n", ApiRef, nAtom);
    if(dxw.dwFlags12 & FAKEGLOBALATOM) {
        ATOM ret = 0;
        if(Kount++) ret = (ATOM)0x1;
        OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
        return ret;
    }
    return (*pGlobalDeleteAtom)(nAtom);
}

ATOM WINAPI extGlobalAddAtomA(LPCSTR lpString) {
    ApiName("GlobalAddAtomA");
    OutTraceSYS("%s: string=\"%s\"\n", ApiRef, lpString);
    if(dxw.dwFlags12 & FAKEGLOBALATOM) return (ATOM)0xBEEF;
    return (*pGlobalAddAtomA)(lpString);
}

ATOM WINAPI extGlobalFindAtomA(LPCSTR lpString) {
    ApiName("GlobalFindAtomA");
    OutTraceSYS("%s: string=\"%s\"\n", ApiRef, lpString);
    if(dxw.dwFlags12 & FAKEGLOBALATOM) return (ATOM)0x0;
    return (*pGlobalFindAtomA)(lpString);
}

DWORD WINAPI extWaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds) {
    if(dwMilliseconds == INFINITE) {
        OutTrace("WaitForSingleObject: trimmed INFINITE time on hdl=%#x\n", hHandle);
        dwMilliseconds = 1000;
    }
    if(!pWaitForSingleObject) {
        OutTrace("WaitForSingleObject: missing hook\n");
        pWaitForSingleObject = WaitForSingleObject;
    }
    return (*pWaitForSingleObject)(hHandle, dwMilliseconds);
}

BOOL WINAPI extReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) {
    BOOL ret;
    ApiName("ReadFile");
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        char sBuf[81];
        sBuf[0] = 0; // empty string
        if(lpOverlapped) sprintf_s(sBuf, 80, "->(Offset=%#x OffsetHigh=%#x)", lpOverlapped->Offset, lpOverlapped->OffsetHigh);
        OutTrace("%s: hFile=%#x Buffer=%#x BytesToRead=%d Overlapped=%#x%s\n", ApiRef, hFile, lpBuffer, nNumberOfBytesToRead, lpOverlapped, sBuf);
    }
#endif // DXW_NOTRACES
    ret = (*pReadFile)(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
#ifndef DXW_NOTRACES
    if(ret) {
        OutTraceSYS("%s: NumberOfBytesRead=%d\n", ApiRef, *lpNumberOfBytesRead);
        OutHexSYS((LPBYTE)lpBuffer, *lpNumberOfBytesRead);
    } else
        OutTraceSYS("%s: ERROR err=%d\n", ApiRef, GetLastError());
#endif // DXW_NOTRACES
    return ret;
}

#ifdef TRACEALL
BOOL WINAPI extWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped) {
    BOOL res;
    ApiName("WriteFile");
#ifndef DXW_NOTRACES
    if(IsTraceSYS) {
        char sBuf[81];
        sBuf[0] = 0; // empty string
        if(lpOverlapped) sprintf_s(sBuf, 80, "->(Offset=%#x OffsetHigh=%#x)", lpOverlapped->Offset, lpOverlapped->OffsetHigh);
        OutTrace("%s: hFile=%#x Buffer%#x BytesToWrite=%d Overlapped=%#x%s\n", ApiRef, hFile, lpBuffer, nNumberOfBytesToWrite, lpOverlapped, sBuf);
    }
#endif // DXW_NOTRACES
    res = (*pWriteFile)(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
#ifndef DXW_NOTRACES
    if(res) {
        OutTraceSYS("%s: BytesWritten=%d\n", ApiRef, *lpNumberOfBytesWritten);
        OutHexSYS((LPBYTE)lpBuffer, *lpNumberOfBytesWritten);
    } else
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
#endif // DXW_NOTRACES
    return res;
}
#endif

#ifdef MONITORTHREADS
HANDLE WINAPI extCreateThread(
    LPSECURITY_ATTRIBUTES   lpThreadAttributes,
    SIZE_T                  dwStackSize,
    LPTHREAD_START_ROUTINE  lpStartAddress,
    LPVOID				  lpParameter,
    DWORD                   dwCreationFlags,
    LPDWORD                 lpThreadId) {
    HANDLE res;
    ApiName("CreateThread");
    DWORD ThreadId;
    OutTrace("%s: curthid=%#x stacksize=%d routine=%#x arg=%#x flags=%#x\n",
             ApiRef, GetCurrentThreadId(), dwStackSize, lpStartAddress, lpParameter, dwCreationFlags);
    if(!pCreateThread) pCreateThread = CreateThread;
    res = (*pCreateThread)(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, &ThreadId);
    if(lpThreadId) *lpThreadId = ThreadId;
    OutTrace("%s: thid=%#x hdl(res)=%#x\n", ApiRef, ThreadId, res);
    return res;
}
#endif // MONITORTHREADS
