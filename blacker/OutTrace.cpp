#include "stdafx.h"
#include "stdio.h"

#define DXWMAXLOGSIZE 4096

void OutTrace(const char *format, ...) {
    va_list al;
    static FILE *fp = NULL; // GHO: thread safe???
    char sBuf[DXWMAXLOGSIZE + 1];
    extern BOOL gbDebug;
    if (fp == NULL) {
        fp = fopen(".\\blacker.log", "w+");
        if(fp == NULL) return;
    }
    va_start(al, format);
    vsprintf_s(sBuf, DXWMAXLOGSIZE, format, al);
    sBuf[DXWMAXLOGSIZE] = 0; // just in case of log truncation
    va_end(al);
    fputs(sBuf, fp);
    fflush(fp);
}
