#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
//#include "syslibs.h"
#include "dxhook.h"
#include "dxhelper.h"
#include <Vfw.h>
#include <ddraw.h>

extern LPCSTR dxwTranslatePathA(LPCSTR, DWORD *);

//#define TRACEALL

typedef HRESULT (WINAPI *AVIFileOpenA_Type)(PAVIFILE *, LPCTSTR, UINT, LPCLSID);
typedef HRESULT (WINAPI *AVIFileOpenW_Type)(PAVIFILE *, LPCWSTR, UINT, LPCLSID);
typedef LONG	(WINAPI *AVIFileRelease_Type)(PAVIFILE);
typedef LONG	(WINAPI *AVIStreamRelease_Type)(PAVISTREAM);
typedef PGETFRAME (WINAPI *AVIStreamGetFrameOpen_Type)(PAVISTREAM, LPBITMAPINFOHEADER);
typedef HRESULT (WINAPI *AVIFileGetStream_Type)(PAVIFILE, PAVISTREAM *, DWORD, LONG);
typedef HRESULT (WINAPI *AVIStreamOpenFromFileA_Type)(PAVISTREAM *, LPCSTR, DWORD, LONG, UINT, CLSID *);
typedef HRESULT (WINAPI *AVIStreamInfoA_Type)(PAVISTREAM, AVISTREAMINFOA *, LONG);
typedef HRESULT (WINAPI *AVIStreamInfoW_Type)(PAVISTREAM, AVISTREAMINFOW *, LONG);
typedef HRESULT (WINAPI *AVIStreamReadFormat_Type)(PAVISTREAM, LONG, LPVOID, LONG *);
#ifdef TRACEALL
typedef VOID	(WINAPI *AVIFileInit_Type)(void);
typedef void (WINAPI *AVIFileExit_Type)(void);
typedef HRESULT (WINAPI *AVIStreamFindSample_Type)(PAVISTREAM, LONG, DWORD);
typedef HRESULT (WINAPI *AVIStreamRead_Type)(PAVISTREAM, LONG, LONG, LPVOID, LONG, LONG *, LONG *);
typedef LONG (WINAPI *AVIStreamSampleToTime_Type)(PAVISTREAM, LONG);
typedef LONG (WINAPI *AVIStreamTimeToSample_Type)(PAVISTREAM, LONG);
typedef LONG (WINAPI *AVIStreamLength_Type)(PAVISTREAM);
typedef HRESULT (WINAPI *AVIStreamGetFrameClose_Type)(PGETFRAME);
#endif

AVIFileOpenA_Type pAVIFileOpenA;
AVIFileOpenW_Type pAVIFileOpenW;
AVIFileRelease_Type pAVIFileRelease;
AVIFileRelease_Type pAVIFileClose;
AVIStreamRelease_Type pAVIStreamRelease;
AVIStreamGetFrameOpen_Type pAVIStreamGetFrameOpen;
AVIFileGetStream_Type pAVIFileGetStream;
AVIStreamOpenFromFileA_Type pAVIStreamOpenFromFileA;
AVIStreamInfoA_Type pAVIStreamInfoA;
AVIStreamInfoW_Type pAVIStreamInfoW;
AVIStreamReadFormat_Type pAVIStreamReadFormat;
#ifdef TRACEALL
AVIFileInit_Type pAVIFileInit;
AVIFileExit_Type pAVIFileExit;
AVIStreamFindSample_Type pAVIStreamFindSample;
AVIStreamRead_Type pAVIStreamRead;
AVIStreamSampleToTime_Type pAVIStreamSampleToTime;
AVIStreamTimeToSample_Type pAVIStreamTimeToSample;
AVIStreamLength_Type pAVIStreamLength;
AVIStreamGetFrameClose_Type pAVIStreamGetFrameClose;
#endif

HRESULT WINAPI extAVIFileOpenA(PAVIFILE *, LPCTSTR, UINT, LPCLSID);
HRESULT WINAPI extAVIFileOpenW(PAVIFILE *, LPCWSTR, UINT, LPCLSID);
LONG WINAPI extAVIFileRelease(PAVIFILE);
LONG WINAPI extAVIFileClose(PAVIFILE);
LONG WINAPI extAVIStreamRelease(PAVISTREAM);
PGETFRAME WINAPI extAVIStreamGetFrameOpen(PAVISTREAM, LPBITMAPINFOHEADER);
HRESULT WINAPI extAVIFileGetStream(PAVIFILE, PAVISTREAM *, DWORD, LONG);
HRESULT WINAPI extAVIStreamOpenFromFileA(PAVISTREAM *, LPCSTR, DWORD, LONG, UINT, CLSID *);
HRESULT WINAPI extAVIStreamInfoA(PAVISTREAM, AVISTREAMINFOA *, LONG);
HRESULT WINAPI extAVIStreamInfoW(PAVISTREAM, AVISTREAMINFOW *, LONG);
HRESULT WINAPI extAVIStreamReadFormat(PAVISTREAM, LONG, LPVOID, LONG *);
#ifdef TRACEALL
VOID WINAPI extAVIFileInit(void);
void WINAPI extAVIFileExit(void);
HRESULT WINAPI extAVIStreamFindSample(PAVISTREAM, LONG, DWORD);
HRESULT WINAPI extAVIStreamRead(PAVISTREAM, LONG, LONG, LPVOID, LONG, LONG *, LONG *);
LONG WINAPI extAVIStreamSampleToTime(PAVISTREAM, LONG);
LONG WINAPI extAVIStreamTimeToSample(PAVISTREAM, LONG);
LONG WINAPI extAVIStreamLength(PAVISTREAM);
HRESULT WINAPI extAVIStreamGetFrameClose(PGETFRAME);
#endif

static HookEntryEx_Type Hooks[] = {
    // v2.04.68.fx1: fixed prototypes of AVIFileOpenA and AVIFileOpenW
    {HOOK_IAT_CANDIDATE, 0, "AVIFileOpenA", NULL, (FARPROC *) &pAVIFileOpenA, (FARPROC)extAVIFileOpenA},
    {HOOK_IAT_CANDIDATE, 0, "AVIFileOpenW", NULL, (FARPROC *) &pAVIFileOpenW, (FARPROC)extAVIFileOpenW},
    {HOOK_IAT_CANDIDATE, 0, "AVIFileRelease", NULL, (FARPROC *) &pAVIFileRelease, (FARPROC)extAVIFileRelease},
    {HOOK_IAT_CANDIDATE, 0, "AVIFileClose", NULL, (FARPROC *) &pAVIFileClose, (FARPROC)extAVIFileClose},
    {HOOK_IAT_CANDIDATE, 0, "AVIStreamRelease", NULL, (FARPROC *) &pAVIStreamRelease, (FARPROC)extAVIStreamRelease},
    {HOOK_IAT_CANDIDATE, 0, "AVIStreamGetFrameOpen", NULL, (FARPROC *) &pAVIStreamGetFrameOpen, (FARPROC)extAVIStreamGetFrameOpen},
    {HOOK_IAT_CANDIDATE, 0, "AVIStreamOpenFromFileA", NULL, (FARPROC *) &pAVIStreamOpenFromFileA, (FARPROC)extAVIStreamOpenFromFileA},
    {HOOK_IAT_CANDIDATE, 0, "AVIStreamInfoA", NULL, (FARPROC *) &pAVIStreamInfoA, (FARPROC)extAVIStreamInfoA},
    {HOOK_IAT_CANDIDATE, 0, "AVIStreamInfoW", NULL, (FARPROC *) &pAVIStreamInfoW, (FARPROC)extAVIStreamInfoW},
    {HOOK_IAT_CANDIDATE, 0, "AVIStreamReadFormat", NULL, (FARPROC *) &pAVIStreamReadFormat, (FARPROC)extAVIStreamReadFormat},
#ifdef TRACEALL
    {HOOK_IAT_CANDIDATE, 0, "AVIFileInit", NULL, (FARPROC *) &pAVIFileInit, (FARPROC)extAVIFileInit},
    {HOOK_IAT_CANDIDATE, 0, "AVIFileExit", NULL, (FARPROC *) &pAVIFileExit, (FARPROC)extAVIFileExit},
    {HOOK_IAT_CANDIDATE, 0, "AVIStreamFindSample", NULL, (FARPROC *) &pAVIStreamFindSample, (FARPROC)extAVIStreamFindSample},
    {HOOK_IAT_CANDIDATE, 0, "AVIStreamRead", NULL, (FARPROC *) &pAVIStreamRead, (FARPROC)extAVIStreamRead},
    {HOOK_IAT_CANDIDATE, 0, "AVIStreamSampleToTime", NULL, (FARPROC *) &pAVIStreamSampleToTime, (FARPROC)extAVIStreamSampleToTime},
    {HOOK_IAT_CANDIDATE, 0, "AVIStreamTimeToSample", NULL, (FARPROC *) &pAVIStreamTimeToSample, (FARPROC)extAVIStreamTimeToSample},
    {HOOK_IAT_CANDIDATE, 0, "AVIStreamLength", NULL, (FARPROC *) &pAVIStreamLength, (FARPROC)extAVIStreamLength},
    {HOOK_IAT_CANDIDATE, 0, "AVIStreamGetFrameClose", NULL, (FARPROC *) &pAVIStreamGetFrameClose, (FARPROC)extAVIStreamGetFrameClose},
#endif
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

void HookAVIFil32(HMODULE module) {
    HookLibraryEx(module, Hooks, "AVIFIL32.dll");
}

FARPROC Remap_AVIFil32_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if (addr = RemapLibraryEx(proc, hModule, Hooks)) return addr;
    return NULL;
}

#ifndef DXW_NOTRACES
static char *AviErr(HRESULT res) {
    char *s;
    switch (res) {
    case AVIERR_BADFORMAT:
        s = "AVIERR_BADFORMAT";
        break;
    case AVIERR_MEMORY:
        s = "AVIERR_MEMORY";
        break;
    case AVIERR_FILEREAD:
        s = "AVIERR_FILEREAD";
        break;
    case AVIERR_FILEOPEN:
        s = "AVIERR_FILEOPEN";
        break;
    case REGDB_E_CLASSNOTREG:
        s = "REGDB_E_CLASSNOTREG";
        break;
    case AVIERR_NODATA:
        s = "AVIERR_NODATA";
        break;
    case AVIERR_BUFFERTOOSMALL:
        s = "AVIERR_BUFFERTOOSMALL";
        break;
    default:
        s = "unknown";
        break;
    }
    return s;
}

// OF_CREATE			Creates a new file. If the file already exists, it is truncated to zero length.
// OF_PARSE				Skips time-consuming operations, such as building an index. Set this flag if you want the function to return as quickly as possible—for example, if you are going to query the file properties but not read the file.
// OF_READ				Opens the file for reading.
// OF_READWRITE			Opens the file for reading and writing.
// OF_SHARE_DENY_NONE	Opens the file nonexclusively. Other processes can open the file with read or write access. AVIFileOpen fails if another process has opened the file in compatibility mode.
// OF_SHARE_DENY_READ	Opens the file nonexclusively. Other processes can open the file with write access. AVIFileOpen fails if another process has opened the file in compatibility mode or has read access to it.
// OF_SHARE_DENY_WRITE	Opens the file nonexclusively. Other processes can open the file with read access. AVIFileOpen fails if another process has opened the file in compatibility mode or has write access to it.
// OF_SHARE_EXCLUSIVE	Opens the file and denies other processes any access to it. AVIFileOpen fails if any other process has opened the file.
// OF_WRITE				Opens the file for writing.

static char *AviMode(UINT c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "OF_");
    if (c & OF_CREATE) strcat(eb, "CREATE+");
    if (c & OF_PARSE) strcat(eb, "PARSE+");
    if (c & OF_READ) strcat(eb, "READ+");
    if (c & OF_READWRITE) strcat(eb, "READWRITE+");
    if (c & OF_SHARE_DENY_NONE) strcat(eb, "SHARE_DENY_NONE+");
    if ((c & OF_SHARE_DENY_READ) == OF_SHARE_DENY_READ) strcat(eb, "SHARE_DENY_READ+"); // 0x30, there are two bits ....
    if (c & OF_SHARE_DENY_WRITE) strcat(eb, "SHARE_DENY_WRITE+");
    if (c & OF_SHARE_EXCLUSIVE) strcat(eb, "SHARE_EXCLUSIVE+");
    if (c & OF_WRITE) strcat(eb, "WRITE+");
    l = strlen(eb);
    if (l > strlen("OF_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}
#endif // DXW_NOTRACES

HRESULT WINAPI extAVIFileOpenA(PAVIFILE *ppfile, LPCSTR szFile, UINT mode, LPCLSID pclsid) {
    HRESULT res;
    ApiName("AVIFileOpenA");
    if(IsTraceSYS) {
        char sClassId[80];
        if(pclsid) sprintf_s(sClassId, 80, "%#x.%#x.%#x.%#x", pclsid->Data1, pclsid->Data2, pclsid->Data3, pclsid->Data4);
        else strcpy(sClassId, "(null)");
        OutTraceSYS("%s: file=\"%s\" mode=%#x(%s) clsid=%s\n", ApiRef, szFile, mode, AviMode(mode), sClassId);
    }
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) szFile = dxwTranslatePathA(szFile, NULL);
    res = (*pAVIFileOpenA)(ppfile, szFile, mode, pclsid);
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, AviErr(res));
    else {
        OutTraceSYS("%s: pfile=%#x\n", ApiRef, *ppfile);
        if(dxw.dwFlags10 & SUSPENDTIMESTRETCH) dxw.SuspendTimeStretch(TIMESHIFT_OFF);
    }
    return res;
}

HRESULT WINAPI extAVIFileOpenW(PAVIFILE *ppfile, LPCWSTR szFile, UINT mode, LPCLSID pclsid) {
    HRESULT res;
    ApiName("AVIFileOpenW");
    if(IsTraceSYS) {
        char sClassId[80];
        if(pclsid) sprintf_s(sClassId, 80, "%#x.%#x.%#x.%#x", pclsid->Data1, pclsid->Data2, pclsid->Data3, pclsid->Data4);
        else strcpy(sClassId, "(null)");
        OutTraceSYS("%s: file=\"%ls\" mode=%#x(%s) clsid=%s\n", ApiRef, szFile, mode, AviMode(mode), sClassId);
    }
    res = (*pAVIFileOpenW)(ppfile, szFile, mode, pclsid);
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, AviErr(res));
    else {
        OutTraceSYS("%s: pfile=%#x\n", ApiRef, *ppfile);
        if(dxw.dwFlags10 & SUSPENDTIMESTRETCH) dxw.SuspendTimeStretch(TIMESHIFT_OFF);
    }
    return res;
}

HRESULT WINAPI extAVIFileGetStream(PAVIFILE pfile, PAVISTREAM *ppavi, DWORD fccType, LONG lParam) {
    HRESULT res;
    ApiName("AVIFileGetStream");
    OutTraceSYS("%s: pfile=%#x fcctype=%#x lparam=%#x\n", ApiRef, pfile, fccType, lParam);
    res = (*pAVIFileGetStream)(pfile, ppavi, fccType, lParam);
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, AviErr(res));
    else
        OutTraceSYS("%s: pavi=%#x\n", ApiRef, *ppavi);
    return res;
}

LONG WINAPI extAVIFileRelease(PAVIFILE pavi) {
    ApiName("AVIFileRelease");
    OutTraceSYS("%s: pavi=%#x\n", ApiRef, pavi);
    if(dxw.dwFlags10 & SUSPENDTIMESTRETCH) dxw.SuspendTimeStretch(TIMESHIFT_ON);
    if((pavi == NULL) || ((DWORD)pavi == 0xFFFF)) {
        // intercepting AVIFileRelease(NULL) avoids an exception in "Die Hard Trilogy" !!!
        // v2.05.53: intercepts AVIFileRelease(0xFFFF) in "Return Fire"
        OutTraceE("%s: pavi=%#x condition - returns OK\n", ApiRef, pavi);
        return 0;
    }
    return (*pAVIFileRelease)(pavi);
}

LONG WINAPI extAVIFileClose(PAVIFILE pavi) {
    ApiName("AVIFileClose");
    OutTraceSYS("%s: pavi=%#x\n", ApiRef, pavi);
    if(dxw.dwFlags10 & SUSPENDTIMESTRETCH) dxw.SuspendTimeStretch(TIMESHIFT_ON);
    if(pavi == NULL) {
        OutTraceE("%s: pavi=NULL condition - returns OK\n", ApiRef);
        return 0;
    }
    return (*pAVIFileClose)(pavi);
}

LONG WINAPI extAVIStreamRelease(PAVISTREAM pavi) {
    LONG ret;
    ApiName("AVIStreamRelease");
    OutTraceSYS("%s: pavi=%#x\n", ApiRef, pavi);
    if(pavi == NULL) {
        OutTraceE("%s: pavi=NULL condition - returns OK\n", ApiRef);
        return 0;
    }
    ret = (*pAVIStreamRelease)(pavi);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

PGETFRAME WINAPI extAVIStreamGetFrameOpen(PAVISTREAM pavi, LPBITMAPINFOHEADER lpbiWanted) {
    ApiName("AVIStreamGetFrameOpen");
    if(IsTraceSYS) {
        char sColor[12];
        switch ((DWORD)lpbiWanted) {
        case 0:
            strcpy(sColor, "DEFAULT");
            break;
        case AVIGETFRAMEF_BESTDISPLAYFMT:
            strcpy(sColor, "BEST");
            break;
        default:
            sprintf(sColor, "%d", lpbiWanted->biBitCount);
        }
        OutTraceSYS("%s: pavi=%#x lpbiwanted=%#x bitcount=%s\n", ApiRef, pavi, lpbiWanted, sColor);
    }
    if((dxw.dwFlags6 & FIXMOVIESCOLOR) && (((DWORD)lpbiWanted == 0) || ((DWORD)lpbiWanted == AVIGETFRAMEF_BESTDISPLAYFMT))) {
        BITMAPINFOHEADER biWanted;
        memset(&biWanted, 0, sizeof(BITMAPINFOHEADER));
        biWanted.biSize = sizeof(BITMAPINFOHEADER);
        biWanted.biBitCount = (WORD)dxw.VirtualPixelFormat.dwRGBBitCount;
        biWanted.biPlanes = 1;
        if(biWanted.biBitCount < 32)
            biWanted.biClrUsed = (0x1 << biWanted.biBitCount); // 8 -> 256;
        else
            biWanted.biClrUsed = 0;
        biWanted.biClrImportant = biWanted.biClrUsed;
        return (*pAVIStreamGetFrameOpen)(pavi, &biWanted);
    }
    return (*pAVIStreamGetFrameOpen)(pavi, lpbiWanted);
}

HRESULT WINAPI extAVIStreamOpenFromFileA(PAVISTREAM *ppavi, LPCSTR szFile, DWORD fccType, LONG lParam, UINT mode, CLSID *pclsidHandler) {
    HRESULT res;
    ApiName("AVIStreamOpenFromFileA");
    OutTraceSYS("%s: pavi=%#x file=\"%s\" fcctype=%#x(%s) lparam=%#x mode=%#x\n",
                ApiRef,
                ppavi,
                szFile,
                fccType, sFourCC(fccType),
                lParam, mode);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) szFile = dxwTranslatePathA(szFile, NULL);
    res = (*pAVIStreamOpenFromFileA)(ppavi, szFile, fccType, lParam, mode, pclsidHandler);
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, AviErr(res));
    return res;
}

HRESULT WINAPI extAVIStreamInfoA(PAVISTREAM pavi, AVISTREAMINFOA *psi, LONG lSize) {
    HRESULT res;
    ApiName("AVIStreamInfoA");
    OutTraceSYS("%s: pavi=%#x size=%d\n", ApiRef, pavi, lSize);
    res = (*pAVIStreamInfoA)(pavi, psi, lSize);
    if(res) {
        OutTraceSYS("%s: ERROR ret=%#x\n", ApiRef, res);
        return res;
    }
    if(IsTraceSYS) {
        OutTrace("> fccType=%#x(%s)\n", psi->fccType, sFourCC(psi->fccType));
        OutTrace("> fccHandler=%#x(%s)\n", psi->fccHandler, sFourCC(psi->fccHandler));
        OutTrace("> dwFlags=%#x\n", psi->dwFlags);
        OutTrace("> dwCaps=%#x\n", psi->dwCaps);
        OutTrace("> wPriority=%d\n", psi->wPriority);
        OutTrace("> wLanguage=%d\n", psi->wLanguage);
        OutTrace("> dwScale=%d\n", psi->dwScale);
        OutTrace("> dwRate=%d\n", psi->dwRate);
        OutTrace("> dwStart=%d\n", psi->dwStart);
        OutTrace("> dwLength=%d\n", psi->dwLength);
        OutTrace("> dwInitialFrames=%d\n", psi->dwInitialFrames);
        OutTrace("> dwSuggestedBufferSize=%d\n", psi->dwSuggestedBufferSize);
        OutTrace("> dwQuality=%d\n", psi->dwQuality);
        OutTrace("> dwSampleSize=%d\n", psi->dwSampleSize);
        OutTrace("> rcFrame=(%d,%d)-(%d,%d)\n", psi->rcFrame.left, psi->rcFrame.top, psi->rcFrame.right, psi->rcFrame.bottom);
        OutTrace("> dwEditCount=%d\n", psi->dwEditCount);
        OutTrace("> dwFormatChangeCount=%d\n", psi->dwFormatChangeCount);
        OutTrace("> szName=%s\n", psi->szName);
    }
    if(dxw.dwFlags6 & NOMOVIES) {
        OutTraceDW("%s: SUPPRESSED (frames=1)\n", ApiRef);
        psi->dwLength = 1; // only 1 frame!!!
    }
    return res;
}

HRESULT WINAPI extAVIStreamInfoW(PAVISTREAM pavi, AVISTREAMINFOW *psi, LONG lSize) {
    HRESULT res;
    ApiName("AVIStreamInfoW");
    OutTraceSYS("%s: pavi=%#x size=%d\n", ApiRef, pavi, lSize);
    res = (*pAVIStreamInfoW)(pavi, psi, lSize);
    if(res) {
        OutTraceSYS("%s: ERROR ret=%#x\n", ApiRef, res);
        return res;
    }
    if(IsTraceSYS) {
        OutTrace("> fccType=%#x\n", psi->fccType);
        OutTrace("> fccHandler=%#x\n", psi->fccHandler);
        OutTrace("> dwFlags=%#x\n", psi->dwFlags);
        OutTrace("> dwCaps=%#x\n", psi->dwCaps);
        OutTrace("> wPriority=%d\n", psi->wPriority);
        OutTrace("> wLanguage=%d\n", psi->wLanguage);
        OutTrace("> dwScale=%d\n", psi->dwScale);
        OutTrace("> dwRate=%d\n", psi->dwRate);
        OutTrace("> dwStart=%d\n", psi->dwStart);
        OutTrace("> dwLength=%d\n", psi->dwLength);
        OutTrace("> dwInitialFrames=%d\n", psi->dwInitialFrames);
        OutTrace("> dwSuggestedBufferSize=%d\n", psi->dwSuggestedBufferSize);
        OutTrace("> dwQuality=%d\n", psi->dwQuality);
        OutTrace("> dwSampleSize=%d\n", psi->dwSampleSize);
        OutTrace("> rcFrame=(%d,%d)-(%d,%d)\n", psi->rcFrame.left, psi->rcFrame.top, psi->rcFrame.right, psi->rcFrame.bottom);
        OutTrace("> dwEditCount=%d\n", psi->dwEditCount);
        OutTrace("> dwFormatChangeCount=%d\n", psi->dwFormatChangeCount);
        OutTrace("> szName=%ls\n", psi->szName);
    }
    if(dxw.dwFlags6 & NOMOVIES) {
        OutTraceDW("%s: SUPPRESSED (frames=1)\n", ApiRef);
        psi->dwLength = 1; // only 1 frame!!!
    }
    return res;
}

HRESULT WINAPI extAVIStreamReadFormat(PAVISTREAM pavi, LONG lPos, LPVOID lpFormat, LONG *lpcbFormat) {
    HRESULT res;
    ApiName("AVIStreamReadFormat");
    OutTraceSYS("%s: pavi=%#x pos=%ld lpformat=%#x len=%d\n", ApiRef, pavi, lPos, lpFormat, lpcbFormat ? *lpcbFormat : NULL);
    if(*lpcbFormat == -1) {
        OutTraceDW("%s: patching bad lpcbFormat field\n", ApiRef);
        *lpcbFormat = 0;
    }
    res = (*pAVIStreamReadFormat)(pavi, lPos, lpFormat, lpcbFormat);
    if(res)
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, AviErr(res));
    else {
        OutTraceSYS("%s: len=%d\n", ApiRef, lpcbFormat ? *lpcbFormat : NULL);
        if(lpcbFormat && (*lpcbFormat == sizeof(BITMAPINFOHEADER)) && lpFormat) {
            LPBITMAPINFOHEADER lpbih = (LPBITMAPINFOHEADER)lpFormat;
            OutTraceSYS("%s: format={bpp=%d size=(%dx%d)}\n",
                        ApiRef, lpbih->biBitCount, lpbih->biWidth, lpbih->biHeight);
        }
    }
    return res;
}

#ifdef TRACEALL

VOID WINAPI extAVIFileInit(void) {
    OutTraceSYS("AVIFileInit\n");
    (*pAVIFileInit)();
}

void WINAPI extAVIFileExit(void) {
    OutTraceSYS("AVIFileExit\n");
    (*pAVIFileExit)();
}

HRESULT WINAPI extAVIStreamFindSample(PAVISTREAM pavi, LONG lPos, DWORD dwFlags) {
    HRESULT res;
    ApiName("AVIStreamFindSample");
    OutTraceSYS("%s: pavi=%#x pos=%d flags=%#x\n", ApiRef, pavi, lPos, dwFlags);
    res = (*pAVIStreamFindSample)(pavi, lPos, dwFlags);
    OutTraceSYS("%s: res=%d\n", ApiRef, res);
    return res;
}

HRESULT WINAPI extAVIStreamRead(PAVISTREAM pavi, LONG lStart, LONG lSamples, LPVOID lpBuffer, LONG cbBuffer, LONG *plBytes, LONG *plSamples) {
    HRESULT res;
    ApiName("AVIStreamRead");
    OutTraceSYS("%s: pavi=%#x start=%d samples=%d buf=%#x buflen=%d\n", ApiRef, pavi, lStart, lSamples, lpBuffer, cbBuffer);
    res = (*pAVIStreamRead)(pavi, lStart, lSamples, lpBuffer, cbBuffer, plBytes, plSamples);
    if(res) {
        OutTraceE("%s: ERROR ret=%#x\n", ApiRef, res);
        return res;
    }
    if(plBytes)
        OutTraceSYS("%s: bytes=%d\n", ApiRef, *plBytes);
    if(plSamples)
        OutTraceSYS("%s: samples=%d\n", ApiRef, *plSamples);
    OutTraceSYS("%s: return ok\n", ApiRef);
    return res;
}

LONG WINAPI extAVIStreamSampleToTime(PAVISTREAM pavi, LONG lSample) {
    HRESULT res;
    ApiName("AVIStreamSampleToTime");
    OutTraceSYS("%s: pavi=%#x sample=%d\n", ApiRef, pavi, lSample);
    res = (*pAVIStreamSampleToTime)(pavi, lSample);
    OutTraceSYS("%s: res=%d\n", ApiRef, res);
    return res;
}

LONG WINAPI extAVIStreamTimeToSample(PAVISTREAM pavi, LONG lTime) {
    HRESULT res;
    ApiName("AVIStreamTimeToSample");
    OutTraceSYS("%s: pavi=%#x time=%d\n", ApiRef, pavi, lTime);
    res = (*pAVIStreamTimeToSample)(pavi, lTime);
    OutTraceSYS("%s: res=%d\n", ApiRef, res);
    return res;
}

LONG WINAPI extAVIStreamLength(PAVISTREAM pavi) {
    LONG res;
    ApiName("AVIStreamLength");
    OutTraceSYS("%s: pavi=%#x \n", ApiRef, pavi);
    res = (*pAVIStreamLength)(pavi);
    OutTraceSYS("%s: res=%d\n", ApiRef, res);
    return res;
}

HRESULT WINAPI extAVIStreamGetFrameClose(PGETFRAME pg) {
    LONG res;
    ApiName("AVIStreamGetFrameClose");
    OutTraceSYS("%s: pg=%#x \n", ApiRef, pg);
    res = (*pAVIStreamGetFrameClose)(pg);
    OutTraceSYS("%s: res=%d\n", ApiRef, res);
    return res;
}
#endif // TRACEALL
