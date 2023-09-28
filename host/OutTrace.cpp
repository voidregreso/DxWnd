#include "stdafx.h"

#define DXWMAXLOGSIZE 4096

void OutTrace(const char *format, ...) {
    va_list al;
    char path[MAX_PATH];
    static FILE *fp = NULL; // GHO: thread safe???
    char sBuf[DXWMAXLOGSIZE + 1];
    extern BOOL gbDebug;
    extern char gInitialWorkingDir[MAX_PATH + 1];
    if (!gbDebug) return;
    if (fp == NULL) {
        sprintf_s(path, MAX_PATH, "%s\\dxwnd.gui.log", gInitialWorkingDir);
        fp = fopen(path, "w+");
        if(fp == NULL) return;
    }
    va_start(al, format);
    vsprintf_s(sBuf, DXWMAXLOGSIZE, format, al);
    sBuf[DXWMAXLOGSIZE] = 0; // just in case of log truncation
    va_end(al);
    fputs(sBuf, fp);
    fflush(fp);
}
