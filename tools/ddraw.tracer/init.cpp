#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>

FILE *TraceLog;
char *dxTraceVersion = "v01.00.02";
BOOL	bLogAttach = 0;
BOOL	bLogBlits = 0;
BOOL	bLogRefCount = 0;
BOOL	bLogRelease = 0;
BOOL	bLogQueryInterface = 0;
BOOL	bLogAll = 0;

void trace(const char *format, ...) {
    va_list al;
    va_start(al, format);
    //vfprintf(fp, format, al);
    __try {
        vfprintf_s(TraceLog, format, al);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        vfprintf_s(TraceLog, "*** exception ***\n", al);
    }
    va_end(al);
    fflush(TraceLog);
}

void InitDxWndTrace(void) {
    char *sIniPath = ".\\dxtrace.ini";
    TraceLog = fopen(".\\dxtrace.txt", "a");
    bLogAll = GetPrivateProfileInt("dxtrace", "all", 0, sIniPath);
    bLogAttach = GetPrivateProfileInt("dxtrace", "attach", 0, sIniPath);
    bLogBlits = GetPrivateProfileInt("dxtrace", "blit", 0, sIniPath);
    bLogRefCount = GetPrivateProfileInt("dxtrace", "refcount", 0, sIniPath);
    bLogRelease = GetPrivateProfileInt("dxtrace", "release", 0, sIniPath);
    bLogQueryInterface = GetPrivateProfileInt("dxtrace", "queryinterface", 0, sIniPath);
    //if(bLogRefCount) bLogRelease=TRUE;
    if(TraceLog) {
        trace("--- START TRACE ---\n");
        trace("--- DXTRACE %s (c)gho-2018---\n", dxTraceVersion);
        if(bLogAll)			trace("--- log all\n");
        if(bLogAttach)		trace("--- log attach\n");
        if(bLogBlits)		trace("--- log blit\n");
        if(bLogRefCount)	trace("--- log refcount\n");
        if(bLogRelease)		trace("--- log release\n");
        if(bLogQueryInterface)trace("--- log queryinterface\n");
    } else
        MessageBox(0, "Cannot open dxtrace.txt file", "dxwnd", MB_ICONERROR);
}

void DxWndEndTrace(void) {
    if(TraceLog) {
        trace("--- END TRACE ---\n");
        fclose(TraceLog);
    }
}

