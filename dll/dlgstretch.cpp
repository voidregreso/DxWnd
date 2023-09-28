#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <stdio.h>
#include <stdlib.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhelper.h"
#include "dlgstretch.h"

// uncomment to force logs ...
//#undef OutTraceDW
//#define OutTraceDW OutTrace

#ifndef DXW_NOTRACES
static char *ExplainDialogStyle(DWORD f) {
    static char s[512];
    char *sep;
    strcpy(s, ExplainStyle(f));
    sep = s + strlen(s);
    strcat(s, "-DS_");
    if(f & DS_ABSALIGN)			strcat(s, "ABSALIGN+");
    if(f & DS_SYSMODAL)			strcat(s, "SYSMODAL+");
    if(f & DS_LOCALEDIT)			strcat(s, "LOCALEDIT+");
    if(f & DS_SETFONT)			strcat(s, "SETFONT+");
    if(f & DS_MODALFRAME)		strcat(s, "MODALFRAME+");
    if(f & DS_NOIDLEMSG)			strcat(s, "NOIDLEMSG+");
    if(f & DS_SETFOREGROUND)		strcat(s, "SETFOREGROUND+");
    if(f & DS_3DLOOK)			strcat(s, "3DLOOK+");
    if(f & DS_FIXEDSYS)			strcat(s, "FIXEDSYS+");
    if(f & DS_NOFAILCREATE)		strcat(s, "NOFAILCREATE+");
    if(f & DS_CONTROL)			strcat(s, "CONTROL+");
    if(f & DS_CENTER)			strcat(s, "CENTER+");
    if(f & DS_CENTERMOUSE)		strcat(s, "CENTERMOUSE+");
    if(f & DS_CONTEXTHELP)		strcat(s, "CONTEXTHELP+");
    if(strlen(sep) == strlen("-DS_")) *sep = 0; // no DS_xx flags
    else sep[strlen(sep) - 1] = 0; // delete last '+'
    return s;
}

static char *ExplainWinClassId(DWORD cid) {
    char *s;
    switch(cid) {
    case 0x0080:
        s = "Button";
        break;
    case 0x0081:
        s = "Edit";
        break;
    case 0x0082:
        s = "Static";
        break;
    case 0x0083:
        s = "ListBox";
        break;
    case 0x0084:
        s = "ScrollBar";
        break;
    case 0x0085:
        s = "ComboBox";
        break;
    default    :
        s = "???";
        break;
    }
    return s;
}
#endif // DXW_NOTRACES

#pragma pack(1)
#ifndef DLGTEMPLATEEX
typedef struct {
    WORD      dlgVer;
    WORD      signature;
    DWORD     helpID;
    DWORD     exStyle;
    DWORD     style;
    WORD      cdit;
    short     x;
    short     y;
    short     cx;
    short     cy;
} DLGTEMPLATEEX, *LPDLGTEMPLATEEX;
#endif

#ifndef DLGITEMTEMPLATEEX
typedef struct {
    DWORD     helpID;
    DWORD     exStyle;
    DWORD     style;
    short     x;
    short     y;
    short     cx;
    short     cy;
    DWORD     id;
} DLGITEMTEMPLATEEX, *LPDLGITEMTEMPLATEEX;
#endif

static DWORD dxwStretchDialogNoEx(LPVOID lpDialog, DWORD flags) {
    LPDLGTEMPLATE lpTemplate = (LPDLGTEMPLATE)lpDialog;
    LPBYTE lpByte = (LPBYTE)lpDialog;
    LPDLGITEMTEMPLATE lpItem;
    LPWORD lpWord;
    BOOL bLog = flags & DXW_DIALOGFLAG_DUMP;
    BOOL bStretch = flags & DXW_DIALOGFLAG_STRETCH;
    BOOL bStretchFont = flags & DXW_DIALOGFLAG_STRETCHFONT;
    bStretchFont = FALSE; // ???
#ifndef DXW_NOTRACES
    if(bLog) OutTraceDW("dxwStretchDialog: style=%#x(%s) extstyle=%#x cdit=%d pos=(%d,%d) size=(%d,%d)\n",
                            lpTemplate->style, ExplainDialogStyle(lpTemplate->style),
                            lpTemplate->dwExtendedStyle,
                            lpTemplate->cdit,
                            lpTemplate->x, lpTemplate->y,
                            lpTemplate->cx, lpTemplate->cy);
#endif // DXW_NOTRACES
    if(bStretch) {
        dxw.MapClient(&lpTemplate->x, &lpTemplate->y, &lpTemplate->cx, &lpTemplate->cy);
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> SCALED pos=(%d,%d) size=(%d,%d)\n",
                                lpTemplate->x, lpTemplate->y,
                                lpTemplate->cx, lpTemplate->cy);
#endif // DXW_NOTRACES
    }
    lpByte += sizeof(DLGTEMPLATE);
    lpByte += ((DWORD)lpByte & 0x1); // align to WORD
    lpWord = (LPWORD)lpByte;
    if(*lpWord == (WORD)0xFFFF) {
        lpWord++;
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> menuid=%#x\n", *lpWord);
#endif // DXW_NOTRACES
        lpWord++;
        lpByte += 2 * sizeof(WORD);
    } else {
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> menu: \"%ls\"\n", lpByte);
#endif // DXW_NOTRACES
        lpByte += (wcslen((WCHAR *)lpByte) + 1) * sizeof(WCHAR);
        lpWord = (LPWORD)lpByte;
    }
    if(*lpWord == (WORD)0xFFFF) {
        lpWord++;
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> classid=%#x(%s)\n", *lpWord, ExplainWinClassId(*lpWord));
#endif // DXW_NOTRACES
        lpWord++;
        lpByte += 2 * sizeof(WORD);
    } else {
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> class: \"%ls\"\n", lpByte);
#endif // DXW_NOTRACES
        lpByte += (wcslen((WCHAR *)lpByte) + 1) * sizeof(WCHAR);
        lpWord = (LPWORD)lpByte;
    }
#ifndef DXW_NOTRACES
    if(bLog) OutTraceDW("> title: \"%ls\"\n", lpByte);
#endif // DXW_NOTRACES
    lpByte += (wcslen((WCHAR *)lpByte) + 1) * sizeof(WCHAR);
    if(lpTemplate->style & DS_SETFONT) {
        lpWord = (LPWORD)lpByte;
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> fontsize=%d\n", *lpWord);
#endif // DXW_NOTRACES
        if(bStretchFont) {
            WORD dwScaledFontSize;
            dwScaledFontSize = (WORD)(((long) * lpWord * dxw.iSizY) / dxw.GetScreenHeight());
#ifndef DXW_NOTRACES
            if(bLog) OutTraceDW("> scaled fontsize=%d\n", dwScaledFontSize);
#endif // DXW_NOTRACES
            *lpWord = dwScaledFontSize;
        }
        lpByte += sizeof(WORD);
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> font: \"%ls\"\n", lpByte);
#endif // DXW_NOTRACES
        lpByte += (wcslen((WCHAR *)lpByte) + 1) * sizeof(WCHAR);
    }
    lpWord = (LPWORD)lpByte;
    for(int i = 0; i < lpTemplate->cdit; i++) {
        lpByte += ((DWORD)lpByte & 0x3); // align to DWORD
        lpItem = (LPDLGITEMTEMPLATE) lpByte;
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> item[%d] style=%#x(%s) extstyle=%#x id=%d pos=(%d,%d) size=(%d,%d)\n",
                                i,
                                lpItem->style, ExplainDialogStyle(lpItem->style),
                                lpItem->dwExtendedStyle,
                                lpItem->id,
                                lpItem->x, lpItem->y,
                                lpItem->cx, lpItem->cy);
#endif // DXW_NOTRACES
        if(bStretch) dxw.MapClient(&lpItem->x, &lpItem->y, &lpItem->cx, &lpItem->cy);
        lpByte += sizeof(DLGITEMTEMPLATE);
        lpByte += ((DWORD)lpByte & 0x1); // align to WORD
        lpWord = (LPWORD)lpByte;
        if(*lpWord == (WORD)0xFFFF) {
            lpWord++;
#ifndef DXW_NOTRACES
            if(bLog) OutTraceDW(">> classid=%#x(%s)\n", *lpWord, ExplainWinClassId(*lpWord));
#endif // DXW_NOTRACES
            lpWord++;
            lpByte += 2 * sizeof(WORD);
        } else {
#ifndef DXW_NOTRACES
            if(bLog) OutTraceDW(">> class: \"%ls\"\n", lpByte);
#endif // DXW_NOTRACES
            lpByte += (wcslen((WCHAR *)lpByte) + 1) * sizeof(WCHAR);
            lpWord = (LPWORD)lpByte;
        }
        if(*lpWord == (WORD)0xFFFF) {
            lpWord++;
#ifndef DXW_NOTRACES
            if(bLog) OutTraceDW(">> titleid=%#x\n", *lpWord);
#endif // DXW_NOTRACES
            lpWord++;
            lpByte += 2 * sizeof(WORD);
        } else {
#ifndef DXW_NOTRACES
            if(bLog) OutTraceDW(">> title: \"%ls\"\n", lpByte);
#endif // DXW_NOTRACES
            lpByte += (wcslen((WCHAR *)lpByte) + 1) * sizeof(WCHAR);
            lpWord = (LPWORD)lpByte;
        }
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW(">> datelen=%d\n", *lpWord);
#endif // DXW_NOTRACES
        if(*lpWord) lpByte += *lpWord;
        else lpByte += sizeof(WORD);
    }
    return lpByte - (LPBYTE)lpDialog;
}

static DWORD dxwStretchDialogEx(LPVOID lpDialog, DWORD flags) {
    //OutTrace("dxwStretchDialog: extended format - UNIMPLEMENTED\n");
    //return 0;
    LPDLGTEMPLATEEX lpTemplate = (LPDLGTEMPLATEEX)lpDialog;
    LPBYTE lpByte = (LPBYTE)lpDialog;
    LPDLGITEMTEMPLATEEX lpItem;
    LPWORD lpWord;
    BOOL bLog = flags & DXW_DIALOGFLAG_DUMP;
    BOOL bStretch = flags & DXW_DIALOGFLAG_STRETCH;
    BOOL bStretchFont = flags & DXW_DIALOGFLAG_STRETCHFONT;
    bStretchFont = FALSE; // ???
#ifndef DXW_NOTRACES
    if(bLog) OutTraceDW("dxwStretchDialogEx: ver=%d helpid=%#x style=%#x(%s) extstyle=%#x cdit=%d pos=(%d,%d) size=(%d,%d)\n",
                            lpTemplate->dlgVer,
                            lpTemplate->helpID,
                            lpTemplate->style, ExplainDialogStyle(lpTemplate->style),
                            lpTemplate->exStyle,
                            lpTemplate->cdit,
                            lpTemplate->x, lpTemplate->y,
                            lpTemplate->cx, lpTemplate->cy);
#endif // DXW_NOTRACES
    if(bStretch) {
        dxw.MapClient(&lpTemplate->x, &lpTemplate->y, &lpTemplate->cx, &lpTemplate->cy);
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> SCALED pos=(%d,%d) size=(%d,%d)\n",
                                lpTemplate->x, lpTemplate->y,
                                lpTemplate->cx, lpTemplate->cy);
#endif // DXW_NOTRACES
    }
    lpByte += sizeof(DLGTEMPLATEEX);
    lpByte += ((DWORD)lpByte & 0x1); // align to WORD
    lpWord = (LPWORD)lpByte;
    if(*lpWord == (WORD)0x0000) {
        lpWord++;
        lpByte += sizeof(WORD);
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> menu=NONE\n");
#endif // DXW_NOTRACES
    } else {
        if(*lpWord == (WORD)0xFFFF) {
            lpWord++;
#ifndef DXW_NOTRACES
            if(bLog) OutTraceDW("> menuid=%#x\n", *lpWord);
#endif // DXW_NOTRACES
            lpWord++;
            lpByte += 2 * sizeof(WORD);
        } else {
#ifndef DXW_NOTRACES
            if(bLog) OutTraceDW("> menu: \"%ls\"\n", lpByte);
#endif // DXW_NOTRACES
            lpByte += (wcslen((WCHAR *)lpByte) + 1) * sizeof(WCHAR);
            lpWord = (LPWORD)lpByte;
        }
    }
    if(*lpWord == (WORD)0x0000) {
        lpWord++;
        lpByte += sizeof(WORD);
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> class=DEFAULT\n");
#endif // DXW_NOTRACES
    } else {
        if(*lpWord == (WORD)0xFFFF) {
            lpWord++;
#ifndef DXW_NOTRACES
            if(bLog) OutTraceDW("> classid=%#x(%s)\n", *lpWord, ExplainWinClassId(*lpWord));
#endif // DXW_NOTRACES
            lpWord++;
            lpByte += 2 * sizeof(WORD);
        } else {
#ifndef DXW_NOTRACES
            if(bLog) OutTraceDW("> class: \"%ls\"\n", lpByte);
#endif // DXW_NOTRACES
            lpByte += (wcslen((WCHAR *)lpByte) + 1) * sizeof(WCHAR);
            lpWord = (LPWORD)lpByte;
        }
    }
    if(*lpWord == (WORD)0x0000) {
        lpWord++;
        lpByte += sizeof(WORD);
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> title=NONE\n");
#endif // DXW_NOTRACES
    } else {
        DWORD tlen = *lpWord;
        lpWord++;
        lpByte = (LPBYTE)lpWord;
        lpByte += ((DWORD)lpByte & 0x1); // align to WORD
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> title: (%d)\"%*.*ls\"\n", tlen, tlen, tlen, lpByte);
#endif // DXW_NOTRACES
        //lpByte += (wcslen((WCHAR *)lpByte) + 1) * sizeof(WCHAR);
        lpByte = (LPBYTE)lpWord + (tlen * sizeof(WCHAR));
    }
    if(lpTemplate->style & (DS_SETFONT | DS_SHELLFONT)) {
        lpWord = (LPWORD)lpByte;
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> pointsize=%d\n", *lpWord);
#endif // DXW_NOTRACES
        if(bStretchFont) {
            WORD dwScaledFontSize;
            dwScaledFontSize = (WORD)(((long) * lpWord * dxw.iSizY) / dxw.GetScreenHeight());
#ifndef DXW_NOTRACES
            if(bLog) OutTraceDW("> scaled fontsize=%d\n", dwScaledFontSize);
#endif // DXW_NOTRACES
            *lpWord = dwScaledFontSize;
        }
        lpWord++;
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> weight=%d\n", *lpWord);
#endif // DXW_NOTRACES
        lpByte = (LPBYTE)lpWord;
        lpByte++;
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> italic=%d\n", *lpByte);
#endif // DXW_NOTRACES
        lpByte++;
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> charset=%d\n", *lpByte);
#endif // DXW_NOTRACES
        lpByte++;
        lpByte += ((DWORD)lpByte & 0x1); // align to WORD
        lpWord = (LPWORD)lpByte;
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> font: \"%ls\"\n", lpByte);
#endif // DXW_NOTRACES
        lpByte += (wcslen((WCHAR *)lpByte) + 1) * sizeof(WCHAR);
    }
    for(int i = 0; i < lpTemplate->cdit; i++) {
        lpByte += ((DWORD)lpByte & 0x3); // align to DWORD
        lpItem = (LPDLGITEMTEMPLATEEX) lpByte;
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW("> item[%d] helpid=%#x style=%#x(%s) extstyle=%#x id=%d pos=(%d,%d) size=(%d,%d)\n",
                                i,
                                lpItem->helpID,
                                lpItem->style, ExplainDialogStyle(lpItem->style),
                                lpItem->exStyle,
                                lpItem->id,
                                lpItem->x, lpItem->y,
                                lpItem->cx, lpItem->cy);
#endif // DXW_NOTRACES
        if(bStretch) dxw.MapClient(&lpItem->x, &lpItem->y, &lpItem->cx, &lpItem->cy);
        lpByte += sizeof(DLGITEMTEMPLATEEX);
        lpByte += ((DWORD)lpByte & 0x1); // align to WORD
        lpWord = (LPWORD)lpByte;
        if(*lpWord == (WORD)0xFFFF) {
            lpWord++;
#ifndef DXW_NOTRACES
            if(bLog) OutTraceDW(">> classid=%#x(%s)\n", *lpWord, ExplainWinClassId(*lpWord));
#endif // DXW_NOTRACES
            lpWord++;
            lpByte += 2 * sizeof(WORD);
        } else {
#ifndef DXW_NOTRACES
            if(bLog) OutTraceDW(">> class: \"%ls\"\n", lpByte);
#endif // DXW_NOTRACES
            lpByte += (wcslen((WCHAR *)lpByte) + 1) * sizeof(WCHAR);
            lpWord = (LPWORD)lpByte;
        }
        if(*lpWord == (WORD)0xFFFF) {
            lpWord++;
#ifndef DXW_NOTRACES
            if(bLog) OutTraceDW(">> titleid=%#x\n", *lpWord);
#endif // DXW_NOTRACES
            lpWord++;
            lpByte += 2 * sizeof(WORD);
        } else {
#ifndef DXW_NOTRACES
            if(bLog) OutTraceDW(">> title: \"%ls\"\n", lpByte);
#endif // DXW_NOTRACES
            lpByte += (wcslen((WCHAR *)lpByte) + 1) * sizeof(WCHAR);
            lpWord = (LPWORD)lpByte;
        }
#ifndef DXW_NOTRACES
        if(bLog) OutTraceDW(">> datelen=%d\n", *lpWord);
#endif // DXW_NOTRACES
        if(*lpWord) lpByte += *lpWord;
        else lpByte += sizeof(WORD);
        if(*lpWord) {
            lpByte += ((DWORD)lpByte & 0x1); // align to WORD
            lpByte += *lpWord; // extra bytes of creation data
        } else {
            lpWord++;
            lpByte = (LPBYTE)lpWord;
        }
    }
    return lpByte - (LPBYTE)lpDialog;
}

DWORD dxwStretchDialog(LPVOID lpDialog, DWORD flags) {
    DLGTEMPLATEEX *lpTemplateEx = (DLGTEMPLATEEX *)lpDialog;
    if(lpTemplateEx->signature == 0xFFFF)
        return dxwStretchDialogEx(lpDialog, flags);
    else
        return dxwStretchDialogNoEx(lpDialog, flags);
}
