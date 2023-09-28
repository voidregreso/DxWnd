/* ------------------------------------------------------------------ */
// Font Stack implementation
/* ------------------------------------------------------------------ */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"

#define FONTSTACKTRACE

dxwFStack::dxwFStack() {
    FontDB = (FontDB_Type *)malloc(sizeof(FontDB_Type) * (MAXFONTS + 1));
    memset(FontDB, 0, sizeof(FontDB_Type) * (MAXFONTS + 1)); // v2.04.93, v2.05.01: fixed bugged array initialization !!!
    StackSize = MAXFONTS;
#ifdef FONTSTACKTRACE
    int freeslots = 0;
    for(DWORD k = 0; k < StackSize; k++) if(FontDB[k].font == 0) freeslots++;
    OutTrace("dxwFStack::dxwFStack: free=%d\n", freeslots);
#endif
}

dxwFStack::~dxwFStack() {
    free(FontDB);
}

void dxwFStack::Push(HFONT font, HFONT scaled) {
    DWORD i;
    FontDB_Type *e;
    for (i = 0; i < StackSize; i++) {
        e = &FontDB[i];
        if (e->font == 0) break; // got matching entry or end of the list
    }
    if(i == StackSize) {
        OutTraceE("dxwFStack::Push: Font stack realloc, stacksize=%d\n", i);
        StackSize += MAXFONTS;
        FontDB = (FontDB_Type *)realloc((LPVOID)FontDB, sizeof(FontDB_Type) * (StackSize + 1));
        if(!FontDB)
            OutTraceE("dxwFStack::Push: Font stack realloc failed, stacksize=%d\n", StackSize);
    }
    e->font = font;
    e->scaled = scaled;
#ifdef FONTSTACKTRACE
    int freeslots = 0;
    for(DWORD k = 0; k < StackSize; k++) if(FontDB[k].font == 0) freeslots++;
    OutTrace("dxwFStack::Push: hfont=%#x scaled=%#x free=%d\n", font, scaled, freeslots);
#endif
}

HFONT dxwFStack::GetScaledFont(HFONT font) {
    DWORD i;
    FontDB_Type *e;
    for (i = 0; i < StackSize; i++) {
        e = &FontDB[i];
        if (e->font == font) return e->scaled; // got matching entry
        if (e->font == 0) return 0; // got end of the list
    }
    return 0; // got max entries
}

HFONT dxwFStack::DeleteFont(HFONT font) {
    DWORD i;
    HFONT scaled = NULL; // v2.04.93
    FontDB_Type *e;
    for (i = 0; i < StackSize; i++) {
        e = &FontDB[i];
        if (e->font == font) {
            scaled = e->scaled;
            break; // got matching entry
        }
    }
    if(i == StackSize) return NULL;
    for (; i < StackSize - 1; i++)
        FontDB[i] = FontDB[i + 1];
    FontDB[i].font = 0;
    FontDB[i].scaled = 0;
#ifdef FONTSTACKTRACE
    int freeslots = 0;
    for(DWORD k = 0; k < StackSize; k++) if(FontDB[k].font == 0) freeslots++;
    OutTrace("dxwFStack::DeleteFont: hfont=%#x scaled=%#x free=%d\n", font, scaled, freeslots);
#endif
    return scaled;
}