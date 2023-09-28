#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhook.h"
#include "dxhelper.h"
#include "resource.h"
#include "player.h"

#include "MMSystem.h"
#include <stdio.h>
#include <mciavi.h>
#include "mciplayer.h"
#include "Digitalv.h"

HANDLE hPlayer = NULL;

extern BOOL IsWithinMCICall;
extern MCIERROR WINAPI emumciSendCommand(BOOL, MCIDEVICEID, UINT, DWORD, DWORD_PTR);

#ifndef MCI_SETAUDIO
#define MCI_SETAUDIO            0x0873
#endif

static char *ansiIdentity = NULL;
static char *ansiProduct = NULL;
static WCHAR *wideIdentity = NULL;
static WCHAR *wideProduct = NULL;

#ifndef DXW_NOTRACES
static char *sStatusItem(DWORD dwItem) {
    char *s;
    switch(dwItem) {
    case MCI_STATUS_LENGTH:
        s = "LENGTH";
        break;
    case MCI_STATUS_POSITION:
        s = "POSITION";
        break;
    case MCI_STATUS_NUMBER_OF_TRACKS:
        s = "NUMBER_OF_TRACKS";
        break;
    case MCI_STATUS_MODE:
        s = "MODE";
        break;
    case MCI_STATUS_MEDIA_PRESENT:
        s = "MEDIA_PRESENT";
        break;
    case MCI_STATUS_TIME_FORMAT:
        s = "TIME_FORMAT";
        break;
    case MCI_STATUS_READY:
        s = "READY";
        break;
    case MCI_STATUS_CURRENT_TRACK:
        s = "CURRENT_TRACK";
        break;
    // v2.04.85: CDA extension found in "Sentinel Returns"
    case MCI_CDA_STATUS_TYPE_TRACK:
        s = "CDA_STATUS_TYPE_TRACK";
        break;
    default:
        s = "???";
        break;
    }
    return s;
}

static char *sDeviceType(DWORD dt) {
    char *s;
    switch(dt) {
    case MCI_ALL_DEVICE_ID:
        s = "ALL_DEVICE_ID";
        break;
    case MCI_DEVTYPE_VCR:
        s = "VCR";
        break;
    case MCI_DEVTYPE_VIDEODISC:
        s = "VIDEODISC";
        break;
    case MCI_DEVTYPE_OVERLAY:
        s = "OVERLAY";
        break;
    case MCI_DEVTYPE_CD_AUDIO:
        s = "CD_AUDIO";
        break;
    case MCI_DEVTYPE_DAT:
        s = "DAT";
        break;
    case MCI_DEVTYPE_SCANNER:
        s = "SCANNER";
        break;
    case MCI_DEVTYPE_ANIMATION:
        s = "ANIMATION";
        break;
    case MCI_DEVTYPE_DIGITAL_VIDEO:
        s = "DIGITAL_VIDEO";
        break;
    case MCI_DEVTYPE_OTHER:
        s = "OTHER";
        break;
    case MCI_DEVTYPE_WAVEFORM_AUDIO:
        s = "WAVEFORM_AUDIO";
        break;
    case MCI_DEVTYPE_SEQUENCER:
        s = "SEQUENCER";
        break;
    default:
        s = "unknown";
        break;
    }
    return s;
}

static char *sTrackType(DWORD type) {
    char *ret = "???";
    switch(type) {
    case MCI_CDA_TRACK_AUDIO:
        ret = "audio";
        break;
    case MCI_CDA_TRACK_OTHER:
        ret = "other";
        break;
    }
    return ret;
}

static char *sTimeFormat(DWORD tf) {
    char *s;
    switch(tf) {
    case MCI_FORMAT_MILLISECONDS:
        s = "MILLISECONDS";
        break;
    case MCI_FORMAT_HMS:
        s = "HMS";
        break;
    case MCI_FORMAT_MSF:
        s = "MSF";
        break;
    case MCI_FORMAT_FRAMES:
        s = "FRAMES";
        break;
    case MCI_FORMAT_SMPTE_24:
        s = "SMPTE_24";
        break;
    case MCI_FORMAT_SMPTE_25:
        s = "SMPTE_25";
        break;
    case MCI_FORMAT_SMPTE_30:
        s = "SMPTE_30";
        break;
    case MCI_FORMAT_SMPTE_30DROP:
        s = "SMPTE_30DROP";
        break;
    case MCI_FORMAT_BYTES:
        s = "BYTES";
        break;
    case MCI_FORMAT_SAMPLES:
        s = "SAMPLES";
        break;
    case MCI_FORMAT_TMSF:
        s = "TMSF";
        break;
    default:
        s = "unknown";
        break;
    }
    return s;
}

static char *ExplainMCICapability(DWORD item) {
    char *labels[] = {
        "???",
        "CAN_RECORD",
        "HAS_AUDIO",
        "HAS_VIDEO",
        "DEVICE_TYPE",
        "USES_FILES",
        "COMPOUND_DEVICE",
        "CAN_EJECT",
        "CAN_PLAY",
        "CAN_SAVE"
    };
    if((item >= MCI_GETDEVCAPS_CAN_RECORD) && (item <= MCI_GETDEVCAPS_CAN_SAVE)) return labels[item];
    return labels[0];
}

static LPCSTR sDevType(LPCSTR dev) {
    LPCSTR s;
    switch((DWORD)dev) {
    case MCI_ALL_DEVICE_ID:
        s = "MCI_ALL_DEVICE_ID";
        break;
    case MCI_DEVTYPE_VCR:
        s = "MCI_DEVTYPE_VCR";
        break;
    case MCI_DEVTYPE_VIDEODISC:
        s = "MCI_DEVTYPE_VIDEODISC";
        break;
    case MCI_DEVTYPE_OVERLAY:
        s = "MCI_DEVTYPE_OVERLAY";
        break;
    case MCI_DEVTYPE_CD_AUDIO:
        s = "MCI_DEVTYPE_CD_AUDIO";
        break;
    case MCI_DEVTYPE_DAT:
        s = "MCI_DEVTYPE_DAT";
        break;
    case MCI_DEVTYPE_SCANNER:
        s = "MCI_DEVTYPE_SCANNER";
        break;
    case MCI_DEVTYPE_ANIMATION:
        s = "MCI_DEVTYPE_ANIMATION";
        break;
    case MCI_DEVTYPE_DIGITAL_VIDEO:
        s = "MCI_DEVTYPE_DIGITAL_VIDEO";
        break;
    case MCI_DEVTYPE_OTHER:
        s = "MCI_DEVTYPE_OTHER";
        break;
    case MCI_DEVTYPE_WAVEFORM_AUDIO:
        s = "MCI_DEVTYPE_WAVEFORM_AUDIO";
        break;
    case MCI_DEVTYPE_SEQUENCER:
        s = "MCI_DEVTYPE_SEQUENCER";
        break;
    default:
        s = "???";
    }
    return s;
}

static LPCWSTR swDevType(LPCWSTR dev) {
    LPCWSTR s;
    switch((DWORD)dev) {
    case MCI_ALL_DEVICE_ID:
        s = L"MCI_ALL_DEVICE_ID";
        break;
    case MCI_DEVTYPE_VCR:
        s = L"MCI_DEVTYPE_VCR";
        break;
    case MCI_DEVTYPE_VIDEODISC:
        s = L"MCI_DEVTYPE_VIDEODISC";
        break;
    case MCI_DEVTYPE_OVERLAY:
        s = L"MCI_DEVTYPE_OVERLAY";
        break;
    case MCI_DEVTYPE_CD_AUDIO:
        s = L"MCI_DEVTYPE_CD_AUDIO";
        break;
    case MCI_DEVTYPE_DAT:
        s = L"MCI_DEVTYPE_DAT";
        break;
    case MCI_DEVTYPE_SCANNER:
        s = L"MCI_DEVTYPE_SCANNER";
        break;
    case MCI_DEVTYPE_ANIMATION:
        s = L"MCI_DEVTYPE_ANIMATION";
        break;
    case MCI_DEVTYPE_DIGITAL_VIDEO:
        s = L"MCI_DEVTYPE_DIGITAL_VIDEO";
        break;
    case MCI_DEVTYPE_OTHER:
        s = L"MCI_DEVTYPE_OTHER";
        break;
    case MCI_DEVTYPE_WAVEFORM_AUDIO:
        s = L"MCI_DEVTYPE_WAVEFORM_AUDIO";
        break;
    case MCI_DEVTYPE_SEQUENCER:
        s = L"MCI_DEVTYPE_SEQUENCER";
        break;
    default:
        s = L"???";
    }
    return s;
}

static char *sSeekFlags(DWORD f) {
    char *s;
    switch(f) {
    case MCI_SEEK_TO_END:
        s = "SEEK_TO_END";
        break;
    case MCI_SEEK_TO_START:
        s = "SEEK_TO_START";
        break;
    case MCI_TO:
        s = "TO";
        break;
    default:
        s = "???";
    }
    return s;
}

static void DumpMciMessage(char *label, BOOL isAnsi, UINT uMsg, DWORD dwFlags, DWORD_PTR dwParam) {
    char *api = "mciSendCommand";
    int iParamSize = 0;
    switch(uMsg) {
    case MCI_BREAK: {
        LPMCI_BREAK_PARMS lpBreak = (LPMCI_BREAK_PARMS)dwParam;
        OutTrace("%s%s: MCI_BREAK cb=%#x virtkey=%d hwndbreak=%#x\n",
                 api, label, lpBreak->dwCallback, lpBreak->nVirtKey, lpBreak->hwndBreak);
        iParamSize = sizeof(MCI_BREAK_PARMS);
    }
    break;
    case MCI_INFO: {
        LPMCI_INFO_PARMS lpInfo = (LPMCI_INFO_PARMS)dwParam;
        OutTrace("%s%s: MCI_INFO cb=%#x retsize=%#x\n",
                 api, label, lpInfo->dwCallback, lpInfo->dwRetSize);
        iParamSize = sizeof(MCI_INFO_PARMS);
    }
    break;
    case MCI_PLAY: {
        LPMCI_PLAY_PARMS lpPlay = (LPMCI_PLAY_PARMS)dwParam;
        OutTrace("%s%s: MCI_PLAY cb=%#x from=%#x to=%#x\n",
                 api, label, lpPlay->dwCallback, lpPlay->dwFrom, lpPlay->dwTo);
        iParamSize = sizeof(MCI_PLAY_PARMS);
    }
    break;
    case MCI_GETDEVCAPS: {
        LPMCI_GETDEVCAPS_PARMS lpDevCaps = (LPMCI_GETDEVCAPS_PARMS)dwParam;
        OutTrace("%s%s: MCI_GETDEVCAPS cb=%#x ret=%#x item=%#x\n",
                 api, label, lpDevCaps->dwCallback, lpDevCaps->dwReturn, lpDevCaps->dwItem);
        iParamSize = sizeof(MCI_GETDEVCAPS_PARMS);
    }
    break;
    case MCI_OPEN: {
        // how to dump LPMCI_OPEN_PARMS strings without crash?
        if(isAnsi) {
            LPMCI_OPEN_PARMSA lpOpen = (LPMCI_OPEN_PARMSA)dwParam;
            OutTrace("%s%s: MCI_OPEN %scb=%#x devid=%#x devtype=%s elementname=%s alias=%s\n",
                     api, label,
                     (dwFlags & MCI_OPEN_SHAREABLE) ? "OPEN_SHAREABLE " : "",
                     lpOpen->dwCallback,
                     lpOpen->wDeviceID,
                     (dwFlags & MCI_OPEN_TYPE) ? (
                         (dwFlags & MCI_OPEN_TYPE_ID) ?
                         sDevType(lpOpen->lpstrDeviceType)
                         :
                         lpOpen->lpstrDeviceType
                     ) : "",
                     (dwFlags & MCI_OPEN_ELEMENT) ? lpOpen->lpstrElementName : "",
                     (dwFlags & MCI_OPEN_ALIAS) ? lpOpen->lpstrAlias : "");
            iParamSize = sizeof(MCI_OPEN_PARMSA);
        } else {
            LPMCI_OPEN_PARMSW lpOpen = (LPMCI_OPEN_PARMSW)dwParam;
            OutTrace("%s%s: MCI_OPEN cb=%#x devid=%#x devtype=%ls elementname=%ls alias=%ls\n",
                     api, label,
                     (dwFlags & MCI_OPEN_SHAREABLE) ? "OPEN_SHAREABLE " : "",
                     lpOpen->dwCallback,
                     lpOpen->wDeviceID,
                     (dwFlags & MCI_OPEN_TYPE) ? (
                         (dwFlags & MCI_OPEN_TYPE_ID) ?
                         swDevType(lpOpen->lpstrDeviceType)
                         :
                         lpOpen->lpstrDeviceType
                     ) : L"",
                     (dwFlags & MCI_OPEN_ELEMENT) ? lpOpen->lpstrElementName : L"",
                     (dwFlags & MCI_OPEN_ALIAS) ? lpOpen->lpstrAlias : L"");
            iParamSize = sizeof(MCI_OPEN_PARMSW);
        }
    }
    break;
    case MCI_STATUS: {
        LPMCI_STATUS_PARMS lpStatus = (LPMCI_STATUS_PARMS)dwParam;
        OutTrace("%s%s: MCI_STATUS cb=%#x ret=%#x item=%#x(%s) track=%#x\n",
                 api, label, lpStatus->dwCallback, lpStatus->dwReturn, lpStatus->dwItem, sStatusItem(lpStatus->dwItem), lpStatus->dwTrack);
        iParamSize = sizeof(MCI_STATUS_PARMS);
    }
    break;
    case MCI_SYSINFO: {
        LPMCI_SYSINFO_PARMS lpSysInfo = (LPMCI_SYSINFO_PARMS)dwParam;
        if(dwFlags & MCI_SYSINFO_QUANTITY) {
            OutTrace("%s%s: MCI_SYSINFO(QUANTITY) cb=%#x retsize=%#x retbuf=%d number=%#x devtype=%#x(%s)\n",
                     api, label, lpSysInfo->dwCallback, lpSysInfo->dwRetSize,
                     *(DWORD *)lpSysInfo->lpstrReturn,
                     lpSysInfo->dwNumber, lpSysInfo->wDeviceType, sDeviceType(lpSysInfo->wDeviceType));
        } else {
            OutTrace("%s%s: MCI_SYSINFO(STRING) cb=%#x retsize=%#x retbuf=%s number=%#x devtype=%#x(%s)\n",
                     api, label, lpSysInfo->dwCallback, lpSysInfo->dwRetSize,
                     lpSysInfo->lpstrReturn,
                     lpSysInfo->dwNumber, lpSysInfo->wDeviceType, sDeviceType(lpSysInfo->wDeviceType));
        }
        iParamSize = sizeof(MCI_SYSINFO_PARMS);
    }
    break;
    case MCI_SET: {
        LPMCI_SET_PARMS lpSetInfo = (LPMCI_SET_PARMS)dwParam;
        OutTrace("%s%s: MCI_SET cb=%#x audio=%#x timeformat=%#x(%s)\n",
                 api, label, lpSetInfo->dwCallback, lpSetInfo->dwAudio, lpSetInfo->dwTimeFormat, sTimeFormat(lpSetInfo->dwTimeFormat));
        iParamSize = sizeof(MCI_SET_PARMS);
    }
    break;
    case MCI_SEEK: {
        // v2.05.00 added trace
        LPMCI_SEEK_PARMS lpSeekInfo = (LPMCI_SEEK_PARMS)dwParam;
        if(lpSeekInfo) {
            OutTrace("%s%s: MCI_SEEK flags=%#x(%s) cb=%#x to=%#x\n",
                     api, label, dwFlags, sSeekFlags(dwFlags), lpSeekInfo->dwCallback, lpSeekInfo->dwTo);
        } else {
            OutTrace("%s%s: MCI_SEEK flags=%#x(%s) seekinfo=(NULL)\n",
                     api, label, dwFlags, sSeekFlags(dwFlags));
        }
        iParamSize = sizeof(MCI_SEEK_PARMS);
    }
    break;
    default: {
        LPMCI_GENERIC_PARMS lpGeneric = (LPMCI_GENERIC_PARMS)dwParam;
        if(lpGeneric)
            OutTrace("%s%s: %s cb=%#x\n", api, label, ExplainMCICommands(uMsg), lpGeneric->dwCallback);
        else
            OutTrace("%s%s: %s params=(NULL)\n", api, label, ExplainMCICommands(uMsg));
        iParamSize = sizeof(MCI_GENERIC_PARMS);
    }
    break;
    }
    OutHexSND((LPBYTE)dwParam, iParamSize);
}
#endif // DXW_NOTRACES

void start_player() {
    if (hPlayer == NULL) {
        // v2.04.99: CreateThread for CD player, the defauld stack size is not enough for "Speedboat Attack"
        // that crashes for stack overflow. 50K as stack size were ok, but to avoid risks we set 100K
        hPlayer = CreateThread(NULL, 100000, (LPTHREAD_START_ROUTINE)player_main, NULL, 0, NULL);
        if(!hPlayer)
            OutTraceE("> Error starting player_main audio CD emulator\n");
    }
}

static int MapMsecToTTMMSS(int msec) {
    int ttmmss = 0;
    unsigned int seconds = msec / 1000;
    for (DWORD i = 1; i <= mciEmu.dwNumTracks; i++) {
        DWORD trackend = tracks[i].position + tracks[i].length;
        if(seconds < trackend) {
            int delta = seconds - tracks[i].position;
            ttmmss = TTMMSS_ENCODE(i, delta / 60, delta % 60);
            break;
        }
    }
    return ttmmss;
}

static int MapSecondsToTTMMSS(int seconds) {
    int ttmmss = 0;
    for (DWORD i = 1; i <= mciEmu.dwNumTracks; i++) {
        if (tracks[i].position == seconds) { // exact match
            ttmmss = TTMMSS_ENCODE(i, 0, 0);
            break;
        }
        if (tracks[i].position > (UINT)seconds) {
            int delta = seconds - tracks[i - 1].position;
            ttmmss = TTMMSS_ENCODE(i - 1, delta / 60, delta % 60);
            break;
        }
    }
    OutDebugSND("> MapSecondsToTTMMSS(%d) -> (TMS#%d:%d:%d)\n", seconds, (ttmmss >> 16), ((ttmmss >> 8) & 0xFF), (ttmmss & 0xFF));
    return ttmmss;
}

static DWORD TTMMSSToAbsSec(DWORD ttmmss) {
    DWORD sec = 0;
    sec = tracks[TTMMSS_TRACKNO(ttmmss)].position;
    sec += TTMMSS_OFFSET(ttmmss);
    return sec;
}

static DWORD DecodeTTMMSS(DWORD ttmmss) {
    DWORD res = 0;
    int track, min, sec, hours;
    track = TTMMSS_TRACKNO(ttmmss);
    min = TTMMSS_MINUTES(ttmmss);
    sec = TTMMSS_SECONDS(ttmmss);
    switch (mciEmu.dwTimeFormat) {
    case MCI_FORMAT_MILLISECONDS:
        sec = tracks[track].position + (60 * min) + sec;
        res = 1000 * sec;
        break;
    case MCI_FORMAT_TMSF:
        res = MCI_MAKE_TMSF(track, min, sec, 0);
        break;
    case MCI_FORMAT_MSF:
        sec = tracks[track].position + (60 * min) + sec;
        min = sec / 60;
        sec = sec % 60;
        res = MCI_MAKE_MSF(min, sec, 0);
        break;
    case MCI_FORMAT_HMS:
        sec = tracks[track].position + (60 * min) + sec;
        hours = sec / 3600;
        sec = sec % 3600;
        min = sec / 60;
        sec = sec % 60;
        res = MCI_MAKE_HMS(hours, min, sec);
        break;
    }
    OutDebugSND("> DecodeTTMMSS(TMS#%d:%d:%d) -> res=%08.8X\n", (ttmmss >> 16), ((ttmmss >> 8) & 0xFF), (ttmmss & 0xFF), res);
    return res;
}

static DWORD DecodeSec(DWORD sec) {
    DWORD res = 0;
    DWORD min, hours;
    switch (mciEmu.dwTimeFormat) {
    case MCI_FORMAT_MILLISECONDS:
        res = 1000 * sec;
        break;
    //case MCI_FORMAT_TMSF:
    //	min = sec / 60;
    //	sec = sec % 60;
    //	res = MCI_MAKE_TMSF(0, min, sec, 0);
    //	break;
    // v2.04.99: BEWARE! though the time format could be set to TMSF format, track lengths in that
    // case must be returned in MSF format. Fixes "Sentinel Returns" regression bug after v2.04.96
    case MCI_FORMAT_MSF:
    case MCI_FORMAT_TMSF:
        min = sec / 60;
        sec = sec % 60;
        res = MCI_MAKE_MSF(min, sec, 0);
        break;
    case MCI_FORMAT_HMS:
        hours = sec / 3600;
        sec = sec - (2600 * hours);
        min = sec / 60;
        sec = sec % 60;
        res = MCI_MAKE_HMS(hours, min, sec);
        break;
    }
    OutDebugSND("> DecodeSec(%d) -> format=%s res=%08.8X\n", sec, sTimeFormat(mciEmu.dwTimeFormat), res);
    return res;
}

static DWORD EncodeTTMMSS(DWORD t) {
    DWORD ttmmss = 0;
    DWORD sec;
    switch (mciEmu.dwTimeFormat) {
    case MCI_FORMAT_MILLISECONDS:
        ttmmss = MapMsecToTTMMSS(t);
        break;
    case MCI_FORMAT_TMSF:
        ttmmss = (MCI_TMSF_TRACK(t) << 16) | (MCI_TMSF_MINUTE(t) << 8) | (MCI_TMSF_SECOND(t));
        break;
    case MCI_FORMAT_MSF:
        sec = (MCI_MSF_MINUTE(t) * 60) + MCI_MSF_SECOND(t);
        ttmmss = MapSecondsToTTMMSS(sec);
        break;
    case MCI_FORMAT_HMS:
        sec = (MCI_HMS_HOUR(t) * 3600) + (MCI_HMS_MINUTE(t) * 60) + MCI_HMS_SECOND(t);
        ttmmss = MapSecondsToTTMMSS(sec);
        break;
    }
    OutDebugSND("> EncodeTTMMSS(%08.8X) -> TMS#%d:%d:%d\n", t, (ttmmss >> 16), ((ttmmss >> 8) & 0xFF), (ttmmss & 0xFF));
    return ttmmss;
}

MCIERROR WINAPI BypassmciSendCommand(BOOL isAnsi, mciSendCommand_Type pmciSendCommand, MCIDEVICEID IDDevice, UINT uMsg, DWORD dwFlags, DWORD_PTR dwParam) {
    MCIERROR ret;
    MCI_STATUS_PARMS *sp;
    LPMCI_OPEN_PARMSA po;
    ret = 0;
    switch(uMsg) {
    case MCI_OPEN:
        po = (MCI_OPEN_PARMSA *)dwParam;
        po->wDeviceID = 1;
        break;
    case MCI_STATUS:
        if(dwFlags & MCI_STATUS_ITEM) {
            // fix for Tie Fighter 95: when bypassing, let the caller know you have no CD tracks
            // otherwise you risk an almost endless loop going through the unassigned returned
            // number of ghost tracks
            // fix for "Emperor of the Fading Suns": the MCI_STATUS_ITEM is set in .or. with
            // MCI_TRACK
            sp = (MCI_STATUS_PARMS *)dwParam;
            switch(dwFlags) {
            case MCI_TRACK:
                sp->dwReturn = 1;
                break;
            default:
                sp->dwTrack = 0;
                if(sp->dwItem == MCI_STATUS_CURRENT_TRACK) sp->dwTrack = 1;
                if(sp->dwItem == MCI_STATUS_NUMBER_OF_TRACKS) sp->dwTrack = 1;
                if(sp->dwItem == MCI_STATUS_LENGTH) sp->dwTrack = 200;
                if(sp->dwItem == MCI_STATUS_MEDIA_PRESENT) sp->dwTrack = 1;
                sp->dwReturn = 0;
                break;
            }
        }
        break;
    case MCI_CLOSE:
    default:
        break;
    }
#ifndef DXW_NOTRACES
    if(IsDebugSND) DumpMciMessage("<<", isAnsi, uMsg, dwFlags, dwParam);
#endif // DXW_NOTRACES
    return ret;
}

static BOOL IsCDAudioTrackA(LPCSTR arg) {
    int len;
    if(!arg) return FALSE;
    len = strlen(arg);
    if(len < 4) return FALSE;
    //OutTrace("ElementName=%s tail=%s\n", arg, &arg[len-4]);
    return !strcmp(&arg[len - 4], ".cda");
}

static BOOL IsCDAudioTrackW(LPCWSTR arg) {
    int len;
    if(!arg) return FALSE;
    len = wcslen(arg);
    if(len < 4) return FALSE;
    //OutTrace("ElementName=%s tail=%s\n", arg, &arg[len-4]);
    return !wcscmp(&arg[len - 4], L".cda");
}

static DWORD dxwLockMciDeviceId() {
    MCI_OPEN_PARMS mciOpenParms;
    char path[MAX_PATH + 1];
    sprintf(path, "%s/dxwnd.wav", GetDxWndPath());
    mciOpenParms.lpstrDeviceType = "waveaudio";
    mciOpenParms.lpstrElementName  = path;
    if(!(*pmciSendCommandA)(0,
                            MCI_OPEN,
                            MCI_OPEN_TYPE | MCI_OPEN_ELEMENT,
                            (DWORD)(LPVOID) &mciOpenParms)) {
        OutTrace("Locking mciId=%#x\n", mciOpenParms.wDeviceID);
        return mciOpenParms.wDeviceID;
    } else {
        OutTrace("Locking failed - mciId=0xBEEF\n");
        return 0xBEEF;
    }
}

static void dxwUnlockMciDeviceId(DWORD mciid) {
    OutTrace("Unlocking mciId=%#x\n", mciid);
    if(mciid != 0xBEEF) (*pmciSendCommandA)(mciid, MCI_CLOSE, 0, NULL);
}

MCIERROR WINAPI extmciSendCommand(BOOL isAnsi, mciSendCommand_Type pmciSendCommand, MCIDEVICEID IDDevice, UINT uMsg, DWORD dwFlags, DWORD_PTR dwParam) {
    RECT saverect;
    MCIERROR ret;
    MCI_OVLY_WINDOW_PARMSA *pwa = (MCI_OVLY_WINDOW_PARMSA *)dwParam;
    MCI_OVLY_WINDOW_PARMSW *pww = (MCI_OVLY_WINDOW_PARMSW *)dwParam;
    LPMCI_ANIM_RECT_PARMS pr;
    char *api = isAnsi ? "mciSendCommandA" : "mciSendCommandW";
#ifndef DXW_NOTRACES
    if(IsTraceSYS || IsTraceSND) {
        OutTrace("%s: IDDevice=0x%04.4X msg=%#x(%s) flags=%#x(%s)\n",
                 api,
                 IDDevice,
                 uMsg, ExplainMCICommands(uMsg),
                 dwFlags, ExplainMCIFlags(uMsg, dwFlags));
    }
    if(IsDebugSYS || IsDebugSND) DumpMciMessage(">>", isAnsi, uMsg, dwFlags, dwParam);
#endif
    if(dxw.dwFlags6 & BYPASSMCI) {
        switch(uMsg) {
        case MCI_OPEN:
        case MCI_CLOSE:
        case MCI_STATUS:
            return BypassmciSendCommand(isAnsi, pmciSendCommand, IDDevice, uMsg, dwFlags, dwParam);
            break;
        }
    }
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        if((uMsg == MCI_OPEN) && (dwFlags & MCI_OPEN_ELEMENT)) {
            extern LPCSTR dxwTranslatePathA(LPCSTR, DWORD *);
            extern LPCWSTR dxwTranslatePathW(LPCWSTR, DWORD *);
            if(isAnsi) {
                LPMCI_OPEN_PARMSA op = (LPMCI_OPEN_PARMSA)dwParam;
                if(op->lpstrElementName) {
                    op->lpstrElementName = dxwTranslatePathA(op->lpstrElementName, NULL);
                    OutTraceDW("%s: translated path=\"%s\"\n", api, op->lpstrElementName);
                }
            } else {
                LPMCI_OPEN_PARMSW op = (LPMCI_OPEN_PARMSW)dwParam;
                if(op->lpstrElementName) {
                    op->lpstrElementName = dxwTranslatePathW(op->lpstrElementName, NULL);
                    OutTraceDW("%s: translated path=\"%ls\"\n", api, op->lpstrElementName);
                }
            }
        }
    }
    // v2.05.30: fix - suppress fullscreen movie play
    DWORD FullScreenModes = MCI_MCIAVI_PLAY_FULLSCREEN | MCI_MCIAVI_PLAY_FULLBY2;
    if(dxw.Windowize && (uMsg == MCI_PLAY) && (dwFlags & FullScreenModes)) {
        OutTraceDW("%s: suppress MCI_PLAY FULLSCREEN flag\n", api);
        dwFlags &= ~FullScreenModes;
        dwFlags |= MCI_MCIAVI_PLAY_WINDOW;
    }
    if(dxw.dwFlags8 & VIRTUALCDAUDIO) {
        BOOL detour = FALSE;
        if (uMsg == MCI_OPEN) {
            if(isAnsi) {
                LPMCI_OPEN_PARMSA parms = (LPMCI_OPEN_PARMSA)dwParam;
                if (dwFlags & MCI_OPEN_TYPE) {
                    if (dwFlags & MCI_OPEN_TYPE_ID) {
                        if (LOWORD(parms->lpstrDeviceType) == MCI_DEVTYPE_CD_AUDIO) detour = TRUE;
                    } else {
                        if (!_strnicmp(parms->lpstrDeviceType, "cdaudio", strlen("cdaudio"))) detour = TRUE;
                    }
                }
                if ((dwFlags & MCI_OPEN_ELEMENT) && IsCDAudioTrackA(parms->lpstrElementName)) // v2.04.95 - "Absoluter Terror"
                    detour = TRUE;
            } else {
                LPMCI_OPEN_PARMSW parms = (LPMCI_OPEN_PARMSW)dwParam;
                if (dwFlags & MCI_OPEN_TYPE) {
                    if (dwFlags & MCI_OPEN_TYPE_ID) {
                        if (LOWORD(parms->lpstrDeviceType) == MCI_DEVTYPE_CD_AUDIO) detour = TRUE;
                    } else {
                        if (!_wcsnicmp(parms->lpstrDeviceType, L"cdaudio", wcslen(L"cdaudio"))) detour = TRUE;
                    }
                }
                if ((dwFlags & MCI_OPEN_ELEMENT) && IsCDAudioTrackW(parms->lpstrElementName))
                    detour = TRUE;
            }
            if(detour && (dxw.dwFlags12 & SUPPRESSCDAUDIO)) {
                OutTraceSND("%s: suppress CDAUDIO ret=MCIERR_DEVICE_NOT_INSTALLED\n", api);
                ret = MCIERR_DEVICE_NOT_INSTALLED;
                return ret;
            }
            if(dxw.dwFlags12 & RESERVEMCIDEVICE) dxw.VirtualCDAudioDeviceId = dxwLockMciDeviceId();
        } else {
            if ((dxw.dwFlags8 & IGNOREMCIDEVID) ||
                    // mciEmu.dwDevID is set in MCI_OPEN and cleared in MCI_CLOSE
                    (IDDevice == mciEmu.dwDevID) ||
                    (IDDevice == AUX_MAPPER)
               ) {
                switch (uMsg) {
                case MCI_SET:
                case MCI_CLOSE:
                case MCI_SEEK:
                case MCI_PLAY:
                case MCI_STOP:
                case MCI_PAUSE:
                case MCI_RESUME:
                case MCI_STATUS:
                case MCI_INFO:
                case MCI_SYSINFO: // v2.04.97: "Heavy Gear"
                case MCI_GETDEVCAPS: // v2.04.95: "Absolute Terror"
                case MCI_WHERE: // v2.04.95: "Absolute Terror"
                    detour = TRUE;
                    break;
                }
            }
        }
        if(detour) {
            ret = emumciSendCommand(isAnsi, IDDevice, uMsg, dwFlags, dwParam);
            // v2.05.50: set / clear the mciEmu.dwDevID value to avoid routing to CD audio emulation
            // commands directed to other mci devices, like movie player.
            // Fixes "Gooch Grundy's X-Decathlon" when setting CD audio emulation
            if(!ret) {
                if(uMsg == MCI_OPEN)  mciEmu.dwDevID = ((MCI_OPEN_PARMS *)dwParam)->wDeviceID;
                if(uMsg == MCI_CLOSE) {
                    if(dxw.dwFlags12 & RESERVEMCIDEVICE) dxwUnlockMciDeviceId(mciEmu.dwDevID);
                    mciEmu.dwDevID = 0xFFF0; // almost impossible value ...
                }
            }
            return ret;
        }
    }
    if((dxw.dwFlags5 & REMAPMCI) && (dxw.IsFullScreen())) {
        switch(uMsg) {
        case MCI_WINDOW:
            if(dxw.dwFlags6 & STRETCHMOVIES) {
                // Sends the window client coordinates before the MCI_WINDOW command,
                // just in case the application omits that.
                MCI_ANIM_RECT_PARMS aparam;
                pr = &aparam;
                uMsg = MCI_PUT;
                pr->dwCallback = 0;
                (*pGetClientRect)(dxw.GethWnd(), &pr->rc);
                dwFlags = MCI_ANIM_RECT | MCI_ANIM_PUT_DESTINATION;
                ret = (*pmciSendCommand)(IDDevice, uMsg, dwFlags, (DWORD_PTR)&aparam);
                _if(ret) OutTraceE("%s: MCI_PUT ERROR res=%d\n", api, ret);
                uMsg = MCI_WINDOW;
            }
            if(isAnsi) {
                OutDebugDW("%s: hwnd=%#x CmdShow=%#x text=\"%s\"\n",
                           api, pwa->hWnd, pwa->nCmdShow, pwa->lpstrText);
                if(dxw.IsRealDesktop(pwa->hWnd)) {
                    pwa->hWnd = dxw.GethWnd();
                    OutDebugDW("%s: REDIRECT hwnd=%#x\n", api, pwa->hWnd);
                }
                if(dxw.dwFlags6 & STRETCHMOVIES) {
                    dwFlags = MCI_ANIM_WINDOW_HWND | MCI_OVLY_WINDOW_ENABLE_STRETCH;
                    pwa->lpstrText = 0;
                    pwa->hWnd = dxw.GethWnd();
                    pwa->nCmdShow = 0;
                }
            } else {
                OutDebugDW("%s: hwnd=%#x CmdShow=%#x text=\"%ls\"\n",
                           api, pww->hWnd, pww->nCmdShow, pww->lpstrText);
                if(dxw.IsRealDesktop(pww->hWnd)) {
                    pww->hWnd = dxw.GethWnd();
                    OutDebugDW("%s: REDIRECT hwnd=%#x\n", api, pww->hWnd);
                    if(dxw.dwFlags6 & STRETCHMOVIES) {
                        dwFlags = MCI_ANIM_WINDOW_HWND | MCI_OVLY_WINDOW_ENABLE_STRETCH;
                        pww->lpstrText = 0;
                        pww->hWnd = dxw.GethWnd();
                        pww->nCmdShow = 0;
                    }
                }
                // attempt to stretch "Wizardry Chronicle" intro movie, but it doesn't work ...
                //if(1){
                //	dwFlags &= ~MCI_OVLY_WINDOW_DISABLE_STRETCH;
                //	dwFlags |= MCI_OVLY_WINDOW_ENABLE_STRETCH;
                //	dwFlags |= MCI_ANIM_WINDOW_HWND;
                //	OutDebugDW("mciSendCommand: STRETCH flags=%#x hwnd=%#x\n", dwFlags, pw->hWnd);
                //}
                break;
            case MCI_PUT:
                RECT client;
                pr = (MCI_ANIM_RECT_PARMS *)dwParam;
                OutDebugDW("%s: rect=(%d,%d),(%d,%d)\n", api, pr->rc.left, pr->rc.top, pr->rc.right, pr->rc.bottom);
                (*pGetClientRect)(dxw.GethWnd(), &client);
                //dwFlags |= MCI_ANIM_PUT_DESTINATION;
                dwFlags |= MCI_ANIM_RECT;
                saverect = pr->rc;
                pr->rc.top = (pr->rc.top * client.bottom) / dxw.GetScreenHeight();
                pr->rc.bottom = (pr->rc.bottom * client.bottom) / dxw.GetScreenHeight();
                pr->rc.left = (pr->rc.left * client.right) / dxw.GetScreenWidth();
                pr->rc.right = (pr->rc.right * client.right) / dxw.GetScreenWidth();
                OutDebugDW("%s: fixed rect=(%d,%d),(%d,%d)\n", api, pr->rc.left, pr->rc.top, pr->rc.right, pr->rc.bottom);
                break;
            case MCI_PLAY:
                if(dxw.dwFlags6 & NOMOVIES) return 0; // ???
                break;
            case MCI_OPEN:
                if(dxw.dwFlags6 & NOMOVIES) return 275; // quite brutal, but working ....
                break;
            case MCI_STOP:
                if(dxw.dwFlags6 & NOMOVIES) return 0; // ???
                break;
            case MCI_CLOSE:
                if(dxw.dwFlags6 & NOMOVIES) return 0; // ???
                break;
            }
        }
        ret = (*pmciSendCommand)(IDDevice, uMsg, dwFlags, dwParam);
#ifndef DXW_NOTRACES
        if(IsDebugSND) DumpMciMessage("<<", isAnsi, uMsg, dwFlags, dwParam);
#endif // DXW_NOTRACES
        if(ret == 0) {
            switch(uMsg) {
            case MCI_STATUS:
                MCI_STATUS_PARMS *p = (MCI_STATUS_PARMS *)dwParam;
                OutTraceDW("%s: Item=%d Track=%d return=%#x\n", api, p->dwItem, p->dwTrack, p->dwReturn);
                break;
            }
        }
        if(dxw.IsFullScreen() && uMsg == MCI_PUT) pr->rc = saverect;
        if (ret) {
            OutTraceE("%s: ERROR res=%d\n", api, ret);
            return ret;
        }
    }
#ifndef DXW_NOTRACES
    if(IsDebugSND) DumpMciMessage("<<", isAnsi, uMsg, dwFlags, dwParam);
#endif // DXW_NOTRACES
    return ret;
}

MCIERROR WINAPI extmciSendCommandA(MCIDEVICEID IDDevice, UINT uMsg, DWORD dwFlags, DWORD_PTR dwParam) {
    return extmciSendCommand(TRUE, pmciSendCommandA, IDDevice, uMsg, dwFlags, dwParam);
}

MCIERROR WINAPI extmciSendCommandW(MCIDEVICEID IDDevice, UINT uMsg, DWORD dwFlags, DWORD_PTR dwParam) {
    return extmciSendCommand(FALSE, pmciSendCommandW, IDDevice, uMsg, dwFlags, dwParam);
}

static int StringToTTMMSS(char *s) {
    int ttmmss;
    DWORD track, hour, min, sec, frame, tokens;
    switch (mciEmu.dwTimeFormat) {
    case MCI_FORMAT_MILLISECONDS:
        sscanf(s, "%d", &sec);
        sec = sec / 1000;
        for(track = 1; track <= mciEmu.dwNumTracks ; track++) if((int)tracks[track].position > sec) break;
        track--; // go to last track that begun before the given time
        if(track < 1) track = 1;
        if(track > mciEmu.dwNumTracks) track = mciEmu.dwNumTracks;
        sec -= tracks[track].position;
        min = sec / 60;
        sec = sec % 60;
        ttmmss = (track << 16) + (min << 8) + sec;
        break;
    case MCI_FORMAT_TMSF:
        tokens = sscanf(s, "%d:%d:%d:%d", &track, &min, &sec, &frame);
        switch(tokens) {
        case 1:
            ttmmss = track << 16;
            break;
        case 2:
            ttmmss = (track << 16) + (min << 8);
            break;
        // case 3 and 4 identical because we ignore the frames
        case 3:
        case 4:
            ttmmss = (track << 16) + (min << 8) + sec;
            break;
        }
        // "Disney's Hercules" special case ...
        if(track > 99) { // is it the proper condition ???
            DWORD hexval = track;
            track = hexval & 0xFF;
            min = (hexval & 0xFF00) >> 8,
            sec = (hexval & 0xFF0000) >> 16,
            ttmmss = (track << 16) + (min << 8) + sec;
        }
        break;
    case MCI_FORMAT_MSF:
        tokens = sscanf(s, "%d:%d:%d", &min, &sec, &frame);
        sec = (60 * min) + sec; // we ignore frames ....
        for(track = 1; track <= mciEmu.dwNumTracks ; track++) if((int)tracks[track].position > sec) break;
        track--; // go to last track that begun before the given time
        if(track < 1) track = 1;
        if(track > mciEmu.dwNumTracks) track = mciEmu.dwNumTracks;
        sec -= tracks[track].position;
        min = sec / 60;
        sec = sec % 60;
        ttmmss = (track << 16) + (min << 8) + sec;
        break;
    case MCI_FORMAT_HMS:
        tokens = sscanf(s, "%d:%d:%d", &hour, &min, &sec);
        sec = (3600 * hour) + (60 * min) + sec;
        for(track = 1; track <= mciEmu.dwNumTracks ; track++) if((int)tracks[track].position > sec) break;
        track--; // go to last track that begun before the given time
        if(track < 1) track = 1;
        if(track > mciEmu.dwNumTracks) track = mciEmu.dwNumTracks;
        sec -= tracks[track].position;
        min = sec / 60;
        sec = sec % 60;
        ttmmss = (track << 16) + (min << 8) + sec;
        break;
    }
    OutDebugSND("> StringToTTMMSS(\"%s\") = TMS#%d:%d:%d\n", s, (ttmmss >> 16), ((ttmmss >> 8) & 0xFF), (ttmmss & 0xFF));
    return ttmmss;
}

static int mciDurationToString(char *sAnswer, int iMaxLen, unsigned int duration) {
    char sBuf[80 + 1];
    int ret;
    switch(mciEmu.dwTimeFormat) {
    case MCI_FORMAT_MILLISECONDS:
        sprintf_s(sBuf, 80, "%d", duration * 1000);
        break;
    // beware: measures about durations are identical in tmsf and msf formats
    case MCI_FORMAT_MSF:
    case MCI_FORMAT_TMSF:
        sprintf_s(sBuf, 80, "%02d:%02d:00", duration / 60, duration % 60);
        break;
    case MCI_FORMAT_HMS:
        sprintf_s(sBuf, 80, "%02d:%02d:%02d", duration / 3600, (duration % 3600) / 60, duration % 60);
        break;
    case MCI_FORMAT_FRAMES:
        sprintf_s(sBuf, 80, "%d", duration * 75);
        break;
    default:
        return 290; // invalid parameter value
        break;
    }
    ret = 0;
    if(strlen(sBuf) < (size_t)iMaxLen)
        strcpy(sAnswer, sBuf);
    else {
        OutTraceSND("> err=MCIERR_PARAM_OVERFLOW\n");
        ret = MCIERR_PARAM_OVERFLOW;
    }
    return ret;
}

static int mciPositionToString(char *sAnswer, int iMaxLen, int track, unsigned int sec) {
    char sBuf[80 + 1];
    int ret;
    switch(mciEmu.dwTimeFormat) {
    case MCI_FORMAT_MILLISECONDS:
        sprintf_s(sBuf, 80, "%d", sec * 1000);
        break;
    case MCI_FORMAT_TMSF:
        sec -= tracks[track].position;
        sprintf_s(sBuf, 80, "%02d:%02d:%02d:00", track, sec / 60, sec % 60);
        break;
    case MCI_FORMAT_MSF:
        sprintf_s(sBuf, 80, "%02d:%02d:00", sec / 60, sec % 60);
        break;
    case MCI_FORMAT_HMS:
        sprintf_s(sBuf, 80, "%02d:%02d:%02d", sec / 3600, (sec % 3600) / 60, sec % 60);
        break;
    case MCI_FORMAT_FRAMES:
        sprintf_s(sBuf, 80, "%d", sec * 75);
        break;
    default:
        return 290; // invalid parameter value
        break;
    }
    ret = 0;
    if(strlen(sBuf) < (size_t)iMaxLen)
        strcpy(sAnswer, sBuf);
    else {
        OutTraceSND("> err=MCIERR_PARAM_OVERFLOW\n");
        ret = MCIERR_PARAM_OVERFLOW;
    }
    return ret;
}

static int mciModeToString(char *sAnswer, int iMaxLen) {
    int ret = 0;
    char *pMode;
    if(mciEmu.playing) pMode = "playing";
    else if((mciEmu.paused) && (dxw.dwFlags11 & CDPAUSECAPABILITY)) pMode = "paused";
    else pMode = "stopped";
    if((int)strlen(pMode) < iMaxLen) {
        strcpy(sAnswer, pMode);
        OutTraceSND("> status mode=%s\n", sAnswer);
    } else {
        OutTraceSND("> err=MCIERR_PARAM_OVERFLOW\n");
        ret = MCIERR_PARAM_OVERFLOW;
    }
    return ret;
}

static char *sMCIMode() {
    char *pMode;
    if(mciEmu.playing) pMode = "playing";
    else if(mciEmu.paused) pMode = "paused";
    else pMode = "stopped";
    return pMode;
}

static int mciTimeFormatToString(char *sAnswer, int iMaxLen) {
    int ret = 0;
    char *pFormat;
    switch(mciEmu.dwTimeFormat) {
    case MCI_FORMAT_MILLISECONDS:
        pFormat = "milliseconds";
        break;
    case MCI_FORMAT_MSF:
        pFormat = "msf";
        break;
    case MCI_FORMAT_TMSF:
        pFormat = "tmsf";
        break;
    case MCI_FORMAT_HMS:
        pFormat = "hms";
        break;
    case MCI_FORMAT_FRAMES:
        // unsupported by cdaudio devices !!!
        pFormat = "frames";
        break;
    default:
        pFormat = "unsupported";
        break;
    }
    if((int)strlen(pFormat) < iMaxLen) {
        strcpy(sAnswer, pFormat);
        OutTraceSND("> status mode=%s\n", sAnswer);
    } else {
        OutTraceSND("> err=MCIERR_PARAM_OVERFLOW\n");
        ret = MCIERR_PARAM_OVERFLOW;
    }
    return ret;
}

static DWORD DoRetString(char *sAnswer, char *lpszReturnString, int cchReturn, char *label) {
    if(strlen(sAnswer) < (size_t)cchReturn) {
        strcpy(lpszReturnString, sAnswer);
        OutTraceSND("> %s: ret=%s\n", label, lpszReturnString);
        return MMSYSERR_NOERROR;
    } else {
        OutTraceSND("> %s: err=MCIERR_PARAM_OVERFLOW ret=%s\n", label, sAnswer);
        return MCIERR_PARAM_OVERFLOW;
    }
}

// this is really ugly but for christ sake why did anyone use it?!
MCIERROR WINAPI extmciSendString(char *api, LPCTSTR lpszCommand, LPTSTR lpszReturnString, UINT cchReturn, HANDLE hwndCallback) {
#ifndef DXW_NOTRACES
    if(IsTraceSYS || IsTraceSND)
        OutTrace("%s: Command=\"%s\" Return=%#x Callback=%#x\n", api, lpszCommand, cchReturn, hwndCallback);
#endif // DXW_NOTRACES
    MCIERROR ret;
    char sMovieNickName[81];
    char sTail[81];
    RECT rect;
    sTail[0] = 0;
    char NewCommand[256];
    BOOL returnsDWord = FALSE;
    if(dxw.dwFlags5 & REMAPMCI) {
        if(strstr(lpszCommand, " fullscreen")) {
            // eliminate "fullscreen specification"
            char *p = (char *)strstr(lpszCommand, " fullscreen");
            memcpy(NewCommand, lpszCommand, (p - lpszCommand));
            NewCommand[p - lpszCommand] = 0;
            // fails: needs some stretching capability?
            //char sNewPos[81];
            //sprintf(sNewPos, " at %d %d %d %d", dxw.iPosX, dxw.iPosY, dxw.iPosX+dxw.iSizX, dxw.iPosY+dxw.iSizY);
            //strcat(NewCommand, sNewPos);
            strcat(NewCommand, &p[strlen(" fullscreen")]);
            lpszCommand = NewCommand;
            OutTraceDW("%s: replaced Command=\"%s\"\n", api, lpszCommand);
            // now should stretch the mciEmu.dwCurrentTrack window ???
        }
        if (sscanf(lpszCommand, "put %s destination at %ld %ld %ld %ld %s",
                   sMovieNickName, &(rect.left), &(rect.top), &(rect.right), &(rect.bottom), sTail) >= 5) {
            // v2.03.19 height / width fix
            rect.right += rect.left; // convert width to position
            rect.bottom += rect.top; // convert height to position
            rect = dxw.MapClientRect(&rect);
            rect.right -= rect.left; // convert position to width
            rect.bottom -= rect.top; // convert position to height
            sprintf(NewCommand, "put %s destination at %d %d %d %d %s", sMovieNickName, rect.left, rect.top, rect.right, rect.bottom, sTail);
            lpszCommand = NewCommand;
            OutTraceDW("%s: replaced Command=\"%s\"\n", api, lpszCommand);
        }
        if (sscanf(lpszCommand, "put %s window client at %ld %ld %ld %ld %s", // found in "Twisted Metal 1"
                   sMovieNickName, &(rect.left), &(rect.top), &(rect.right), &(rect.bottom), sTail) >= 5) {
            // v2.03.19 height / width fix
            rect.right += rect.left; // convert width to position
            rect.bottom += rect.top; // convert height to position
            rect = dxw.MapClientRect(&rect);
            rect.right -= rect.left; // convert position to width
            rect.bottom -= rect.top; // convert position to height
            sprintf(NewCommand, "put %s window client at %d %d %d %d %s", sMovieNickName, rect.left, rect.top, rect.right, rect.bottom, sTail);
            lpszCommand = NewCommand;
            OutTraceDW("%s: replaced Command=\"%s\"\n", api, lpszCommand);
        }
    }
    // v2.05.47: remapping of "open path ..." commands with fake device.
    // Used for "Goman" fake CD configuration.
    if((dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) && (!strncmp(lpszCommand, "open ", 5))) {
        BOOL hasQuotes = FALSE;
        extern LPCSTR dxwTranslatePathA(LPCSTR, DWORD *);
        const char *path = &lpszCommand[strlen("open ")];
        OutTrace("path=\"%s\"\n", path);
        if(*path == '\"') {
            path = &lpszCommand[strlen("open \"")]; // v2.05.54 fix: path with double-quotes in Imperialism
            hasQuotes = TRUE;
        }
        if(!strncmp(&path[1], ":\\", 2)) {
            // beware: a side effect of dxwTranslatePathA is the conservation of whatever
            // comes after the path and is part of mciSendString syntax. So, no need to
            // split & join the final arguments, though it may appear a little confusing ...
            path = dxwTranslatePathA(path, NULL);
            OutTrace("%s: must replace with \"%s\"\n", api, path);
            char *cmd = (char *)malloc(strlen("open ") + strlen(path) + 2); // +1 for doublequotes +1 for terminator
            strcpy(cmd, "open ");
            if(hasQuotes) strcat(cmd, "\"");
            strcat(cmd, path);
            IsWithinMCICall = TRUE;
            ret = (*pmciSendStringA)((LPCTSTR)cmd, lpszReturnString, cchReturn, (HWND)hwndCallback);
            IsWithinMCICall = FALSE;
            _if(ret) OutTraceDW("%s ERROR: ret=%#x\n", api, ret);
            OutTraceDW("%s: RetString=\"%s\"\n", api, lpszReturnString);
            return ret;
        }
    }
    if(dxw.dwFlags8 & VIRTUALCDAUDIO) {
        static char sPlayerNickName[80 + 1] = "";
        char sNickName[80 + 1];
        char sCommand[80 + 1];
        char sDevice[80 + 1];
        char *sCmdTarget;
        DWORD dwCommand;
        if(!pplr_pump) player_init();
        player_change();
        if(mciEmu.dwTimeFormat == MCI_FORMAT_UNDEFINED) mciEmu.dwTimeFormat = MCI_FORMAT_MSF;
        if(sscanf(lpszCommand, "%s %s", sCommand, sDevice) != 2) {
            OutTraceE("%s: bad syntax on \"%s\"\n", api, lpszCommand);
            return (*pmciSendStringA)((LPCSTR)lpszCommand, lpszReturnString, cchReturn, (HWND)hwndCallback);
        }
        if(!strcmp(sCommand, "open")) dwCommand = MCI_OPEN;
        else if(!strcmp(sCommand, "close")) dwCommand = MCI_CLOSE;
        else if(!strcmp(sCommand, "stop")) dwCommand = MCI_STOP;
        else if(!strcmp(sCommand, "pause")) dwCommand = MCI_PAUSE;
        else if(!strcmp(sCommand, "resume")) dwCommand = MCI_RESUME;
        else if(!strcmp(sCommand, "set")) dwCommand = MCI_SET;
        else if(!strcmp(sCommand, "status")) dwCommand = MCI_STATUS;
        else if(!strcmp(sCommand, "play")) dwCommand = MCI_PLAY;
        else if(!strcmp(sCommand, "seek")) dwCommand = MCI_SEEK;
        else if(!strcmp(sCommand, "capability")) dwCommand = MCI_GETDEVCAPS;
        else if(!strcmp(sCommand, "setaudio")) dwCommand = MCI_SETAUDIO;
        else if(!strcmp(sCommand, "info")) dwCommand = MCI_INFO;
        else
            dwCommand = 0;
        OutDebugSND("> command=%d device=%s\n", dwCommand, sDevice);
        if(dwCommand && (dwCommand != MCI_OPEN)) {
            // don't try to parse unknown commands, nor open command that
            // doesn't necessarily have extra arguments
            sCmdTarget = (char *)lpszCommand;
            while (*sCmdTarget && *sCmdTarget != ' ') sCmdTarget++; // skip command
            while (*sCmdTarget && *sCmdTarget == ' ') sCmdTarget++; // skip first separator
            while (*sCmdTarget && *sCmdTarget != ' ') sCmdTarget++; // skip deviceid
            while (*sCmdTarget && *sCmdTarget == ' ') sCmdTarget++; // skip second separator
        }
        switch (dwCommand) {
        case MCI_OPEN: {
            BOOL bVirtual = FALSE;
            if (sscanf(lpszCommand, "open cdaudio alias %s", sNickName) == 1) {
                strcpy(sPlayerNickName, sNickName);
                OutTraceSND("> registered alias=%s for cdaudio\n", sPlayerNickName);
                bVirtual = TRUE;
            }
            // v2.05.05 found in HoMM1 Win32 version
            char cDriveLetter;
            if (sscanf(lpszCommand, "open %c: type cdaudio alias %s", &cDriveLetter, sNickName) == 2) {
                strcpy(sPlayerNickName, sNickName);
                OutTraceSND("> registered alias=%s for drive %c:\n", sPlayerNickName, cDriveLetter);
                bVirtual = TRUE;
            }
            // v2.05.42 fix: "open cdaudio" must work as any other "open cdaudio ..." command
            // found in "Fighting Force".
            if (!strncmp(lpszCommand, "open cdaudio", strlen("open cdaudio"))) {
                OutTraceSND("> handling cdaudio device\n");
                bVirtual = TRUE;
            }
            if(bVirtual) {
                if(dxw.dwFlags12 & SUPPRESSCDAUDIO) {
                    OutTraceSND("%s: suppress CDAUDIO ret=MCIERR_DEVICE_NOT_INSTALLED\n", api);
                    return MCIERR_DEVICE_NOT_INSTALLED;
                }
                if (cchReturn > 5) {
                    // v2.05.25: replaced 0xBEEF with correct value
                    sprintf(lpszReturnString, "%d", dxw.VirtualCDAudioDeviceId);
                    OutTraceSND("> ret=%s\n", lpszReturnString);
                }
                // do not return error here if cchReturn is 0: "Target" does it and
                // expects the function not to fail.
                player_set_status(MCI_OPEN);
                return 0;
            }
        }
        break;
        case MCI_SYSINFO: {
            // v2.04.93: fix, the correct return string is "cdaudio", not "cd"
            // thanks to "dippy dipper", fixes "Pandemonium!" sound emulation
            return DoRetString("cdaudio", lpszReturnString, cchReturn, sCmdTarget);
        }
        break;
        case MCI_INFO: {
            if(!strcmp(sDevice, "cdaudio") || !strcmp(sDevice, sPlayerNickName)) {
                char sBuffer[81];
                if(strstr(sCmdTarget, "identity")) {
                    (*pGetPrivateProfileStringA)("info", "identity", "", sBuffer, 80, ".\\Music\\mcihack.txt");
                    if(!strlen(sBuffer)) {
                        OutTraceSND("> ret=350(MCIERR_NO_IDENTITY)\n");
                        return MCIERR_NO_IDENTITY;
                    }
                    return DoRetString(sBuffer, lpszReturnString, cchReturn, sCmdTarget);
                }
                if(strstr(sCmdTarget, "upc")) {
                    // v2.05.05: seen in "Heroes of Might and Magic", Win32 CD version.
                    // as alternative, return MCIERR_NO_IDENTITY error ?
                    (*pGetPrivateProfileStringA)("info", "product", "", sBuffer, 80, ".\\Music\\mcihack.txt");
                    if(!strlen(sBuffer)) {
                        OutTraceSND("> ret=350(MCIERR_NO_IDENTITY)\n");
                        return MCIERR_NO_IDENTITY;
                    }
                    return DoRetString(sBuffer, lpszReturnString, cchReturn, sCmdTarget);
                }
            }
        }
        break;
        case MCI_STOP:
        case MCI_PAUSE:
        case MCI_RESUME: {
            if(!strcmp(sDevice, "cdaudio") || !strcmp(sDevice, sPlayerNickName)) {
                player_set_status(dwCommand);
                return 0;
            }
        }
        break;
        case MCI_CLOSE: {
            if(!strcmp(sDevice, "cdaudio") || !strcmp(sDevice, sPlayerNickName)) {
                // Beware: from MSDN examples here https://msdn.microsoft.com/en-us/library/windows/desktop/dd797881(v=vs.85).aspx
                // and from commands sent to "Fighting Force" it seems evident that "close cdaudio" does NOT stop the music play.
                if (cchReturn > 0) strcpy(lpszReturnString, "");
                // v2.05.00: MCI_CLOSE resets time formats
                mciEmu.dwTimeFormat = MCI_FORMAT_UNDEFINED;
                OutTraceSND("> ret=0(MCIERR_NOERROR) from device=%s\n", sDevice);
                return 0;
            }
        }
        break;
        case MCI_SET: {
            returnsDWord = TRUE;
            if(!strcmp(sDevice, "cdaudio") || !strcmp(sDevice, sPlayerNickName)) {
                // v2.05.01: better argument control, fixed bug with "tmsf" and "msf" confused with "ms"
                DWORD dwNewTimeFormat = -1;
                if (strstr(sCmdTarget, "time format")) {
                    char format[81];
                    strcpy(format, "");
                    sscanf(sCmdTarget, "time format %s", format);
                    if (!strcmp(format, "tmsf"))
                        dwNewTimeFormat = MCI_FORMAT_TMSF;
                    else if (!strcmp(format, "msf"))
                        dwNewTimeFormat = MCI_FORMAT_MSF;
                    else if (!strcmp(format, "milliseconds"))
                        dwNewTimeFormat = MCI_FORMAT_MILLISECONDS;
                    else if  (!strcmp(format, "frames"))
                        dwNewTimeFormat = MCI_FORMAT_FRAMES;
                    else if  (!strcmp(format, "hms"))
                        dwNewTimeFormat = MCI_FORMAT_HMS;
                    else if  (!strcmp(format, "ms"))
                        dwNewTimeFormat = MCI_FORMAT_MILLISECONDS;
                    if(dwNewTimeFormat == -1) {
                        OutTraceE("%s: unknown time format \"%s\"\n", api, lpszCommand);
                        return 290;
                    } else {
                        OutTraceSND("%s: set time format %#x(%s)\n", api, dwNewTimeFormat, sTimeFormat(dwNewTimeFormat));
                        mciEmu.dwTimeFormat = dwNewTimeFormat;
                        return 0;
                    }
                } else if (strstr(sCmdTarget, "door open")) {
                    player_set_status(MCIEMU_DOOR_OPEN);
                    return 0;
                } else if (strstr(sCmdTarget, "door closed")) {
                    player_set_status(MCIEMU_DOOR_CLOSED);
                    return 0;
                }
            }
        }
        break;
        case MCI_STATUS: {
            char sAnswer[81]; // big more than enough
            if(!strcmp(sDevice, "cdaudio") || !strcmp(sDevice, sPlayerNickName)) {
                if(strstr(sCmdTarget, "number of tracks")) {
                    _itoa(mciEmu.dwNumTracks, sAnswer, 10);
                    return DoRetString(sAnswer, lpszReturnString, cchReturn, sCmdTarget);
                } else if(strstr(sCmdTarget, "media present")) {
                    // v2.04.81: when CDAUDIO emulation is on, always pretend the CDROM is in the caddy
                    // found in "Terracide"
                    return DoRetString(mciEmu.dooropened ? "false" : "true", lpszReturnString, cchReturn, sCmdTarget);
                } else if(strstr(sCmdTarget, "type track")) {
                    // v2.04.81: "status cdaudio type track <n>" emulation:
                    // first track is data track, all other tracks are supposed to be audio.
                    // found in "Terracide"
                    int iTrackNo;
                    sscanf(sCmdTarget, "type track %d", &iTrackNo);
                    strcpy(sAnswer, iTrackNo == 1 ? "other" : "audio");
                    return DoRetString(sAnswer, lpszReturnString, cchReturn, sCmdTarget);
                } else if(strstr(sCmdTarget, "length track")) {
                    // "status cdaudio length track <num>"
                    // found in "The Fifth Element"
                    int trackno = 0; // avoid exceptions ...
                    if (sscanf(sCmdTarget, "length track %d", &trackno) == 1)
                        return mciDurationToString(lpszReturnString, cchReturn, tracks[trackno].length);
                } else if(strstr(sCmdTarget, "position track")) {
                    // "status cdaudio position track <num>"
                    // "status cdaudio position track <num>" found in "The Fifth Element"
                    // found in codeproject CD player demo:  https://www.codeproject.com/Articles/3549/MCI-CD-Player
                    int trackno = 0; // avoid exceptions ...
                    if (sscanf(sCmdTarget, "position track %d", &trackno) == 1)
                        return mciPositionToString(lpszReturnString, cchReturn, trackno, tracks[trackno].position);
                } else if(strstr(sCmdTarget, "type track")) {
                    // "status cdaudio type track <num>" found in "Terracide"
                    int trackno = 0; // avoid exceptions ...
                    if (sscanf(lpszCommand, "type track %d", &trackno) == 1) {
                        if(tracks[trackno].type == MCI_AUDIO_TRACK)
                            strcpy(sAnswer, "audio");
                        else
                            strcpy(sAnswer, "other");
                        return DoRetString(sAnswer, lpszReturnString, cchReturn, sCmdTarget);
                    }
                } else if(strstr(sCmdTarget, "mode")) {
                    // v2.04.96: "status cdaudio mode" (uppercase) found in "Fallen Haven"
                    // the virtual CD player can't be "not ready" or elsewhere ...
                    return DoRetString(sMCIMode(), lpszReturnString, cchReturn, sCmdTarget);
                } else if(strstr(sCmdTarget, "current track")) {
                    sprintf(sAnswer, "%d", TTMMSS_TRACKNO(mciEmu.dwCurrentTrack));
                    return DoRetString(sAnswer, lpszReturnString, cchReturn, sCmdTarget);
                } else if (strstr(sCmdTarget, "track")) {
                    int trackno = 0; // avoid exceptions ...
                    char spec[81];
                    int tag;
                    int ret = 0;
                    char *sInfo = "???";
                    strcpy(spec, "");
                    if (sscanf(sCmdTarget, "track %d %s", &trackno, spec) == 2) {
                        if(!strcmp(spec, "position")) {
                            OutTraceSND("> track %d position", trackno);
                            return mciPositionToString(lpszReturnString, cchReturn, trackno, tracks[trackno].position);
                        }
                        if(!strcmp(spec, "length")) {
                            OutTraceSND("> track %d length", trackno);
                            return mciDurationToString(lpszReturnString, cchReturn, tracks[trackno].length);
                        }
                    }
                    if (sscanf(sCmdTarget, "track %d %d", &trackno, &tag) == 2) {
                        switch (tag) {
                        case 1:
                            ret = mciDurationToString(lpszReturnString, cchReturn, tracks[trackno].position);
                            sInfo = "length";
                            break;
                        case 2:
                            // seen in "Fallen Haven" !
                            ret = mciPositionToString(lpszReturnString, cchReturn, trackno, tracks[trackno].position);
                            sInfo = "pos";
                            break;
                        case 3:
                            snprintf(lpszReturnString, cchReturn, "%d", mciEmu.dwNumTracks);
                            sInfo = "numtracks";
                            break;
                        case 4:
                            ret = mciModeToString(lpszReturnString, cchReturn);
                            sInfo = "mode";
                            break;
                        case 5:
                            // ??? undocumented
                            strncpy(lpszReturnString, "true", cchReturn);
                            break;
                        case 6:
                            ret = mciTimeFormatToString(lpszReturnString, cchReturn);
                            sInfo = "timeformat";
                            break;
                        case 7:
                            // ??? undocumented
                            strncpy(lpszReturnString, "true", cchReturn);
                            break;
                        case 8:
                            // undocumented. Thanks to huh researches, it seems the last seeked track, track 1 included ....
                            snprintf(lpszReturnString, cchReturn, "%d", TTMMSS_TRACKNO(mciEmu.dwSeekedTrack));
                            sInfo = "seekedtrack";
                            break;
                        default:
                            ret = 274; // The MCI device the system is using does not support the specified command
                            break;
                        }
                        OutTraceSND("> track %d %d(%s): ret=%d:\"%s\"\n", trackno, tag, sInfo, ret, lpszReturnString);
                        return ret;
                    }
                } else if(strstr(sCmdTarget, "position")) { // "position track" and "track position" intercepted above
                    // v2.05.11: must not return a duration but the current position: fixes "The House of the Dead" audio stop / resume
                    int ret = mciPositionToString(lpszReturnString, cchReturn, TTMMSS_TRACKNO(mciEmu.dwCurrentTrack), TTMMSSToAbsSec(mciEmu.dwCurrentTrack));
                    OutTraceSND("> ret=%d:\"%s\"\n", ret, lpszReturnString);
                    return ret;
                } else if(strstr(sCmdTarget, "length")) { // "length track" and "track length" intercepted above
                    // v2.05.01: fixed "status cdaudio length"
                    int ret = mciDurationToString(lpszReturnString, cchReturn, TTMMSSToAbsSec(mciEmu.dwLastTrack));
                    OutTraceSND("> ret=%d:\"%s\"\n", ret, lpszReturnString);
                    return ret;
                }
                if(strstr(sCmdTarget, "ready")) { // should answer "true" if media is present or "false" otherwise
                    // v2.05.42: fixed "status cdaudio ready" found in "Disney's Hercules"
                    int ret = 0;
                    strncpy(lpszReturnString, "true", cchReturn);
                    OutTraceSND("> ret=%d:\"%s\"\n", ret, lpszReturnString);
                    return ret;
                } else
                    OutTraceSND("> unrecognized command\n");
            }
        }
        break;
        case MCI_PLAY: {
            char sFrom[81];
            char sTo[81];
            // v2.05.38 fix: compare with "cdaudio" or nick to avoid confusion with movies or midi files
            // fixes "3-D Ultra Pinball" playing CD tracks of another game instead of its own midi file
            if(!strcmp(sDevice, "cdaudio") || !strcmp(sDevice, sPlayerNickName)) {
                // by default, play the whole disk
                DWORD from = mciEmu.dwFirstTrack;
                DWORD to = mciEmu.dwLastTrack;
                // v2.05.54: initialize all callbacks
                info.play_callback = 0;
                info.stop_callback = 0;
                if(!strcmp("notify", &sCmdTarget[strlen(sCmdTarget) - strlen("notify")])) {
                    OutTraceSND("> notify option hwnd=%#x\n", hwndCallback);
                    info.play_callback = (DWORD_PTR)hwndCallback;
                }
                if (sscanf(sCmdTarget, "from %s to %s", sFrom, sTo) == 2) {
                    from = StringToTTMMSS(sFrom);
                    to = StringToTTMMSS(sTo);
                    if (from == to) to = TTMMSS_TRACKNO(from) | TTMMSS_ENDOFTRACK; // "Target" ...
                    info.first = from;
                    info.last = to;
                } else if (sscanf(sCmdTarget, "from %s", sFrom) == 1) { // v2.05.54 fix
                    from = StringToTTMMSS(sFrom);
                    info.first = from;
                }
                start_player();
                info.first = from;
                info.last  = to;
                player_set_status(MCI_PLAY);
                return 0;
            }
        }
        break;
        case MCI_SEEK: {
            char sTarget[81];
            // v2.05.38 fix: compare with "cdaudio" or nick to avoid confusion with other cases
            if(!strcmp(sDevice, "cdaudio") || !strcmp(sDevice, sPlayerNickName)) {
                if (sscanf(sCmdTarget, "to %s", sTarget) == 1) {
                    if(!strcmp(sTarget, "start"))
                        mciEmu.dwSeekedTrack = mciEmu.dwFirstTrack;
                    else if(!strcmp(sTarget, "end"))
                        mciEmu.dwSeekedTrack = mciEmu.dwLastTrack;
                    else {
                        // v2.05.01: fixed for all time formats
                        mciEmu.dwSeekedTrack = StringToTTMMSS(sTarget);
                    }
                    return 0;
                }
            }
        }
        break;
        case MCI_GETDEVCAPS: {
            char sTarget[81];
            if (!strcmp(sCmdTarget, "device type"))
                return DoRetString("cdaudio", lpszReturnString, cchReturn, sCmdTarget);
            else if (sscanf(sCmdTarget, "can %s", sTarget) == 1) {
                if(!strcmp(sTarget, "eject")) return DoRetString("true", lpszReturnString, cchReturn, sCmdTarget);
                else if(!strcmp(sTarget, "play")) return DoRetString("true", lpszReturnString, cchReturn, sCmdTarget);
                else
                    return DoRetString("false", lpszReturnString, cchReturn, sCmdTarget);
            } else if (sscanf(sCmdTarget, "has %s", sTarget) == 1) {
                if(!strcmp(sTarget, "audio")) return DoRetString("true", lpszReturnString, cchReturn, sCmdTarget);
                else
                    return DoRetString("false", lpszReturnString, cchReturn, sCmdTarget);
            }
        }
        break;
        case MCI_SETAUDIO: {
            UINT nVolume;
            if (sscanf(sCmdTarget, "volume to %d", &nVolume) == 1) {
                (*pplr_setvolume)(nVolume);
                OutTraceSND("> Volume=%d\n", nVolume);
            }
            return DoRetString("", lpszReturnString, cchReturn, sCmdTarget);
        }
        break;
        }
    }
    // decides whether to log a string or a DWORD number
    if (!_strnicmp(lpszCommand, "window", strlen("window")) ||
            !_strnicmp(lpszCommand, "set", strlen("set")) ||
            !_strnicmp(lpszCommand, "status", strlen("status")) ||
            !_strnicmp(lpszCommand, "configure", strlen("configure")) ||
            !_strnicmp(lpszCommand, "put", strlen("put"))
       ) returnsDWord = TRUE;
    IsWithinMCICall = TRUE;
    ret = (*pmciSendStringA)((LPCTSTR)lpszCommand, lpszReturnString, cchReturn, (HWND)hwndCallback);
    IsWithinMCICall = FALSE;
    _if(ret) OutTraceDW("%s ERROR: ret=%#x\n", api, ret);
    if(returnsDWord)
        OutTraceDW("%s: RetString=%#x\n", api, lpszReturnString);
    else
        OutTraceDW("%s: RetString=\"%s\"\n", api, lpszReturnString);
    if(dxw.dwFlags10 & CDROMPRESENT) {
        // v2.04.81: tested with "Terracide" CDROM present check
        char sDevice[80 + 1];
        if (sscanf(lpszCommand, "status %s media present", sDevice) == 1) {
            OutTraceDW("%s BYPASS: pretend CDROM is closed\n", api);
            if(cchReturn > strlen("true")) strcpy(lpszReturnString, "true");
        }
    }
    return ret;
}

MCIERROR WINAPI emumciSendCommand(BOOL isAnsi, MCIDEVICEID IDDevice, UINT uMsg, DWORD dwFlags, DWORD_PTR dwParam) {
    if(!pplr_pump) player_init();
    player_change();
    if(mciEmu.dwTimeFormat == MCI_FORMAT_UNDEFINED) mciEmu.dwTimeFormat = MCI_FORMAT_TMSF;
    switch(uMsg) {
    case MCI_OPEN:
        if (isAnsi) {
            LPMCI_OPEN_PARMSA parms = (LPMCI_OPEN_PARMSA)dwParam;
            if (dwFlags & MCI_OPEN_TYPE_ID) {
                if (LOWORD(parms->lpstrDeviceType) == MCI_DEVTYPE_CD_AUDIO) {
                    dprintf("> Returning magic device id=%d for MCI_DEVTYPE_CD_AUDIO\n", dxw.VirtualCDAudioDeviceId);
                    parms->wDeviceID = dxw.VirtualCDAudioDeviceId;
                    player_set_status(MCI_OPEN);
                    return 0;
                }
            }
            if (dwFlags & MCI_OPEN_TYPE && !(dwFlags & MCI_OPEN_TYPE_ID)) {
                dprintf("> MCI_OPEN_TYPE -> %s\n", parms->lpstrDeviceType);
                if (_stricmp(parms->lpstrDeviceType, "cdaudio") == 0) {
                    dprintf("> Returning magic device id=%d for MCI_OPEN_TYPE_ID cdaudio\n", dxw.VirtualCDAudioDeviceId);
                    parms->wDeviceID = dxw.VirtualCDAudioDeviceId;
                    player_set_status(MCI_OPEN);
                    return 0;
                }
            }
            if (dwFlags & MCI_OPEN_ELEMENT) {
                char *p;
                int iTrackNo;
                dprintf("> MCI_OPEN_ELEMENT -> %s\n", parms->lpstrElementName);
                p = (char *)strstr(parms->lpstrElementName, "\\track");
                if(sscanf(p, "\\track%d.cda", &iTrackNo) == 1) {
                    iTrackNo ++;
                    dprintf("> Track number = %d\n", iTrackNo);
                }
                info.first = (MCI_TMSF_TRACK(iTrackNo) << 16) | (MCI_TMSF_MINUTE(0) << 8) | (MCI_TMSF_SECOND(0));
                info.last = (MCI_TMSF_TRACK(iTrackNo + 1) << 16) | (MCI_TMSF_MINUTE(0) << 8) | (MCI_TMSF_SECOND(0));
                dprintf("> mapped tracks First=%02d.%02d.%02d Last=%02d.%02d.%02d\n",
                        info.first >> 16, (info.first >> 8) & 0xFF, info.first & 0xFF,
                        info.last >> 16,  (info.last >> 8) & 0xFF,  info.last & 0xFF);
                mciEmu.dwSeekedTrack = iTrackNo;
                mciEmu.dwCurrentTrack = iTrackNo;
                start_player();
                player_set_status(MCI_PLAY);
                parms->wDeviceID = dxw.VirtualCDAudioDeviceId;
            }
        } else {
            LPMCI_OPEN_PARMSW parms = (LPMCI_OPEN_PARMSW)dwParam;
            if (dwFlags & MCI_OPEN_TYPE_ID) {
                if (LOWORD(parms->lpstrDeviceType) == MCI_DEVTYPE_CD_AUDIO) {
                    dprintf("> Returning magic device id=%d for MCI_DEVTYPE_CD_AUDIO\n", dxw.VirtualCDAudioDeviceId);
                    parms->wDeviceID = dxw.VirtualCDAudioDeviceId;
                    player_set_status(MCI_OPEN);
                    return 0;
                }
            }
            if (dwFlags & MCI_OPEN_TYPE && !(dwFlags & MCI_OPEN_TYPE_ID)) {
                dprintf("> MCI_OPEN_TYPE -> %ls\n", parms->lpstrDeviceType);
                if (_wcsicmp (parms->lpstrDeviceType, L"cdaudio") == 0) {
                    dprintf("> Returning magic device id=%d for MCI_OPEN_TYPE_ID cdaudio\n", dxw.VirtualCDAudioDeviceId);
                    parms->wDeviceID = dxw.VirtualCDAudioDeviceId;
                    player_set_status(MCI_OPEN);
                    return 0;
                }
            }
            if (dwFlags & MCI_OPEN_ELEMENT) {
                WCHAR *p;
                int iTrackNo;
                dprintf("> MCI_OPEN_ELEMENT -> %ls\n", parms->lpstrElementName);
                p = (WCHAR *)wcsstr(parms->lpstrElementName, L"\\track");
                if(swscanf(p, L"\\track%d.cda", &iTrackNo) == 1) {
                    iTrackNo ++;
                    dprintf("> Track number = %d\n", iTrackNo);
                }
                info.first = (MCI_TMSF_TRACK(iTrackNo) << 16) | (MCI_TMSF_MINUTE(0) << 8) | (MCI_TMSF_SECOND(0));
                info.last = (MCI_TMSF_TRACK(iTrackNo + 1) << 16) | (MCI_TMSF_MINUTE(0) << 8) | (MCI_TMSF_SECOND(0));
                dprintf("> mapped tracks First=%02d.%02d.%02d Last=%02d.%02d.%02d\n",
                        info.first >> 16, (info.first >> 8) & 0xFF, info.first & 0xFF,
                        info.last >> 16,  (info.last >> 8) & 0xFF,  info.last & 0xFF);
                mciEmu.dwSeekedTrack = iTrackNo;
                mciEmu.dwCurrentTrack = iTrackNo;
                start_player();
                player_set_status(MCI_PLAY);
                parms->wDeviceID = dxw.VirtualCDAudioDeviceId;
            }
        }
        break;
    case MCI_GETDEVCAPS: { // v2.04.95: "Absolute Terror"
        LPMCI_GETDEVCAPS_PARMS parms = (LPMCI_GETDEVCAPS_PARMS)dwParam;
        dprintf("> MCI_GETDEVCAPS item=%#x(%s)\n", parms->dwItem, ExplainMCICapability(parms->dwItem));
        parms->dwReturn = 0; // has no capabilities
        switch(parms->dwItem) {
        case MCI_GETDEVCAPS_CAN_PLAY:
        case MCI_GETDEVCAPS_CAN_EJECT:
            parms->dwReturn = TRUE;
            dprintf("> MCI_GETDEVCAPS val=TRUE\n");
            break;
        case MCI_GETDEVCAPS_DEVICE_TYPE:
            parms->dwReturn = MCI_DEVTYPE_CD_AUDIO;
            dprintf("> MCI_GETDEVCAPS val=MCI_DEVTYPE_CD_AUDIO\n");
            break;
        default:
            parms->dwReturn = FALSE;
            dprintf("> MCI_GETDEVCAPS val=FALSE\n");
            break;
        }
        dprintf("> params->dwReturn=%d(%#x)\n", parms->dwReturn, parms->dwReturn);
        return 0;
    }
    break;
    case MCI_SET: {
        LPMCI_SET_PARMS parms = (LPMCI_SET_PARMS)dwParam;
        if (dwFlags & MCI_SET_TIME_FORMAT) {
            mciEmu.dwTimeFormat = parms->dwTimeFormat;
            dprintf("> MCI_SET_TIME_FORMAT format=%s\n", sTimeFormat(mciEmu.dwTimeFormat));
        } else if (dwFlags & MCI_SET_DOOR_OPEN) {
            player_set_status(MCIEMU_DOOR_OPEN);
            dprintf("> MCI_SET_DOOR_OPEN\n");
        } else if (dwFlags & MCI_SET_DOOR_CLOSED) {
            player_set_status(MCIEMU_DOOR_CLOSED);
            dprintf("> MCI_SET_DOOR_CLOSED\n");
        }
    }
    break;
    case MCI_WHERE: {
        LPMCI_GENERIC_PARMS parms = (LPMCI_GENERIC_PARMS)dwParam;
        // do nothing .... ???
        return 0;
    }
    break;
    case MCI_CLOSE: {
        if (hPlayer) player_set_status(MCI_CLOSE);
        // v2.05.00: MCI_CLOSE resets time formats
        mciEmu.dwTimeFormat = MCI_FORMAT_UNDEFINED;
        return 0;
    }
    break;
    case MCI_SEEK: {
        LPMCI_SEEK_PARMS parms = (LPMCI_SEEK_PARMS)dwParam;
        if (dwFlags & MCI_TO) {
            dprintf("> dwTo: %d\n", parms->dwTo);
            mciEmu.dwSeekedTrack = EncodeTTMMSS(parms->dwTo);
        }
        if (dwFlags & MCI_SEEK_TO_START) {
            dprintf("> Seek to Start\n");
            mciEmu.dwSeekedTrack = mciEmu.dwFirstTrack;
        }
        if (dwFlags & MCI_SEEK_TO_END) {
            dprintf("> Seek to End\n");
            mciEmu.dwSeekedTrack = mciEmu.dwLastTrack;
        }
    }
    break;
    case MCI_PLAY: {
        LPMCI_PLAY_PARMS parms = (LPMCI_PLAY_PARMS)dwParam;
        dprintf("> timeformat=%s\n", sTimeFormat(mciEmu.dwTimeFormat));
        if(dwFlags & MCI_NOTIFY) {
            dprintf("> dwCallback: %d\n", parms->dwCallback);
            info.play_callback = parms->dwCallback;
        } else
            info.play_callback = 0;
        // v2.05.33: when playing or paused, MCI_PLAY with no track specification
        // is made equivalent to a MCI_RESUME
        if(((dwFlags & (MCI_FROM | MCI_TO)) == 0) &&
                (mciEmu.paused || mciEmu.playing)
          ) {
            dprintf("> from,to=NIL resume\n", parms->dwFrom);
            player_set_status(MCI_RESUME);
            break;
        }
        // v2.04.74: MCI_PLAY after MCI_SEEK command, in "Rubik's Games"
        if (dwFlags & MCI_FROM)
            dprintf("> dwFrom: %d\n", parms->dwFrom);
        else {
            dprintf("> dwFrom: seeked %d\n", mciEmu.dwSeekedTrack);
            parms->dwFrom = DecodeTTMMSS(mciEmu.dwSeekedTrack);
        }
        info.first = EncodeTTMMSS(parms->dwFrom);
        dprintf("> mapped from(TMS)=%d/%d/%d\n", info.first >> 16, (info.first >> 8) & 0xFF, info.first & 0xFF);
        //dprintf("DEBUG(FROM)> first=%d last=%d firsttrack=%d lasttrack=%d\n", info.first, info.last, mciEmu.dwFirstTrack, mciEmu.dwLastTrack);
        if (info.first < mciEmu.dwFirstTrack) info.first = mciEmu.dwFirstTrack;
        if (info.first > mciEmu.dwLastTrack) info.first = mciEmu.dwLastTrack;
        info.last = info.first + TTMMSS_NEXTTRACK; // next track in ttmmss format
        if(info.last > mciEmu.dwLastTrack) info.last = mciEmu.dwLastTrack; // fix if this is last track, no next one
        if (dwFlags & MCI_TO) {
            dprintf("> dwTo: %d\n", parms->dwTo);
            info.last = EncodeTTMMSS(parms->dwTo);
            //if(TTMMSS_OFFSET(info.last) == 0) info.last |= TTMMSS_ENDOFTRACK; // until the end of that track
            if(info.last == info.first) info.last |= TTMMSS_ENDOFTRACK; // until the end of that track (???)
            dprintf("> mapped to(TMS)=%d/%d/%d\n", info.last >> 16, (info.last >> 8) & 0xFF, info.last & 0xFF);
            //dprintf("DEBUG(TO)> first=%d last=%d firsttrack=%d lasttrack=%d\n", info.first, info.last, mciEmu.dwFirstTrack, mciEmu.dwLastTrack);
            if (info.last < info.first) info.last = info.first;
            if (info.last > mciEmu.dwLastTrack) info.last = mciEmu.dwLastTrack;
        }
        dprintf("> mapped tracks First=%02d.%02d.%02d Last=%02d.%02d.%02d\n",
                info.first >> 16, (info.first >> 8) & 0xFF, info.first & 0xFF,
                info.last >> 16,  (info.last >> 8) & 0xFF,  info.last & 0xFF);
        if ((info.first && (dwFlags & MCI_FROM)) ||
                (info.last && (dwFlags & MCI_TO)) ||		// v2.04.57: needed for "WinQuake" resume play after MCI_PAUSE
                (info.first != 0)							// v2.04.74: needed for "Rubik's Games" play after MCI_SEEK
           ) {
            if(info.first == info.last) return 0; // happens with "The Fifth Element"
            start_player();
            player_set_status(MCI_PLAY);
        }
    }
    break;
    case MCI_STOP:
    // v2.04.57: added MCI_PAUSE case
    case MCI_PAUSE:
    // v2.04.57: added MCI_RESUME case
    case MCI_RESUME: {
        LPMCI_GENERIC_PARMS parms = (LPMCI_GENERIC_PARMS)dwParam;
        if(dwFlags & MCI_NOTIFY) info.stop_callback = parms->dwCallback;
        player_set_status(uMsg);
    }
    break;
    case MCI_INFO: {
        if(isAnsi) {
            // v2.04.97: use bitwise AND to compare instead of numeric identity
            // because both MCI_INFO_MEDIA_IDENTITY and MCI_INFO_PRODUCT could be
            // associated to other flags, es. MCI_WAIT - fixes "Sentinel Returns"
            dprintf("> MCI_INFO command = %#x\n", dwFlags);
            LPMCI_INFO_PARMS lpInfo = (LPMCI_INFO_PARMS)dwParam;
            if(dwFlags & MCI_INFO_MEDIA_IDENTITY) {
                // v2.05.32: return default "fakecd" string. Fixes "Sentinel Returns"
                if(!ansiIdentity) {
                    ansiIdentity = (char *)malloc(lpInfo->dwRetSize + 1);
                    (*pGetPrivateProfileStringA)("info", "identity", "fakecd", ansiIdentity, lpInfo->dwRetSize, ".\\Music\\mcihack.txt");
                }
                lpInfo->dwRetSize = strlen(lpInfo->lpstrReturn);
                if(!strlen(lpInfo->lpstrReturn)) {
                    OutTraceSND("> ret=350(MCIERR_NO_IDENTITY)\n");
                    return MCIERR_NO_IDENTITY;
                }
            }
            if(dwFlags & MCI_INFO_PRODUCT) {
                // v2.05.32: return default "fakecd" string.
                if(!ansiProduct) {
                    ansiProduct = (char *)malloc(lpInfo->dwRetSize + 1);
                    (*pGetPrivateProfileStringA)("info", "product", "fakecd", ansiProduct, lpInfo->dwRetSize, ".\\Music\\mcihack.txt");
                }
                lpInfo->dwRetSize = strlen(lpInfo->lpstrReturn);
                if(!strlen(lpInfo->lpstrReturn)) {
                    OutTraceSND("> ret=350(MCIERR_NO_IDENTITY)\n");
                    return MCIERR_NO_IDENTITY;
                }
            }
            dprintf("> MCI_INFO strret=%s retsize=%d\n", lpInfo->lpstrReturn, lpInfo->dwRetSize);
        } else { // v2.05.54: widechar case
            dprintf("> MCI_INFO command = %#x\n", dwFlags);
            LPMCI_INFO_PARMSW lpInfo = (LPMCI_INFO_PARMSW)dwParam;
            if(dwFlags & MCI_INFO_MEDIA_IDENTITY) {
                // v2.05.32: return default "fakecd" string. Fixes "Sentinel Returns"
                if(!ansiIdentity) {
                    ansiIdentity = (char *)malloc(lpInfo->dwRetSize + 1);
                    (*pGetPrivateProfileStringA)("info", "identity", "fakecd", ansiIdentity, lpInfo->dwRetSize, ".\\Music\\mcihack.txt");
                }
                if(!wideIdentity) {
                    wideIdentity = (WCHAR *)malloc((lpInfo->dwRetSize + 1) * sizeof(WCHAR));
                    mbstowcs(wideIdentity, ansiIdentity, lpInfo->dwRetSize);
                }
                lpInfo->dwRetSize = wcslen(wideIdentity);
                lpInfo->lpstrReturn = wideIdentity;
                if(!wcslen(lpInfo->lpstrReturn)) {
                    OutTraceSND("> ret=350(MCIERR_NO_IDENTITY)\n");
                    return MCIERR_NO_IDENTITY;
                }
            }
            if(dwFlags & MCI_INFO_PRODUCT) {
                // v2.05.32: return default "fakecd" string.
                if(!ansiProduct) {
                    ansiProduct = (char *)malloc(lpInfo->dwRetSize + 1);
                    (*pGetPrivateProfileStringA)("info", "product", "fakecd", ansiProduct, lpInfo->dwRetSize, ".\\Music\\mcihack.txt");
                }
                if(!wideIdentity) {
                    wideIdentity = (WCHAR *)malloc((lpInfo->dwRetSize + 1) * sizeof(WCHAR));
                    mbstowcs(wideProduct, ansiProduct, lpInfo->dwRetSize);
                }
                lpInfo->dwRetSize = wcslen(wideProduct);
                lpInfo->lpstrReturn = wideProduct;
                if(!wcslen(lpInfo->lpstrReturn)) {
                    OutTraceSND("> ret=350(MCIERR_NO_IDENTITY)\n");
                    return MCIERR_NO_IDENTITY;
                }
            }
            dprintf("> MCI_INFO strret=%s retsize=%d\n", lpInfo->lpstrReturn, lpInfo->dwRetSize);
        }
    }
    break;
    case MCI_SYSINFO: {
        MCIERROR ret;
        if(isAnsi) {
            // tbd: found in "Heavy Gear":
            // also found in "Interstate 76 Nitro Pack"
            // MCI_SYSINFO_QUANTITY devtype=CDAUDIO, better return 1
            LPMCI_SYSINFO_PARMSA lpInfo = (LPMCI_SYSINFO_PARMSA)dwParam;
            dprintf("> MCI_SYSINFO command = %#x\n", dwFlags);
            ret = (*pmciSendCommandA)(IDDevice, uMsg, dwFlags, dwParam);
            //if(ret) return ret; // v2.05.36: an error must not prevent the following fix!
            if( (dwFlags & MCI_SYSINFO_QUANTITY) &&
                    (lpInfo->wDeviceType == MCI_DEVTYPE_CD_AUDIO)) {
                dprintf("> MCI_SYSINFO MCI_SYSINFO_QUANTITY MCI_dev=DEVTYPE_CD_AUDIO force return 1\n");
                DWORD num = 1;
                // MSDN: Pointer to a user-supplied buffer for the return string.
                // It is also used to return a DWORD value when the MCI_SYSINFO_QUANTITY flag is used.
                memcpy((LPVOID)(lpInfo->lpstrReturn), (LPVOID)&num, sizeof(DWORD));
                // MSDN: Size, in bytes, of return buffer.
                lpInfo->dwRetSize = sizeof(DWORD);
                // MSDN: Number indicating the device position in the MCI device table or in the list
                // of open devices if the MCI_SYSINFO_OPEN flag is set.
                lpInfo->dwNumber = dxw.VirtualCDAudioDeviceId; // v2.05.36
                ret = 0; // v2.05.36: pretend it's OK
            }
            if(ret) return ret; // v2.05.36
            if(dwFlags & MCI_SYSINFO_NAME) {
                dprintf("> MCI_SYSINFO ret=\"%s\" retsize=%d num=%d devtype=%d\n",
                        lpInfo->lpstrReturn,
                        lpInfo->dwRetSize,
                        lpInfo->dwNumber,
                        lpInfo->wDeviceType);
            } else {
                dprintf("> MCI_SYSINFO ret=%d retsize=%d num=%d devtype=%d\n",
                        *(DWORD *)lpInfo->lpstrReturn,
                        lpInfo->dwRetSize,
                        lpInfo->dwNumber,
                        lpInfo->wDeviceType);
            }
        } else { // 2.05.54: widechar case
            LPMCI_SYSINFO_PARMSW lpInfo = (LPMCI_SYSINFO_PARMSW)dwParam;
            dprintf("> MCI_SYSINFO command = %#x\n", dwFlags);
            ret = (*pmciSendCommandW)(IDDevice, uMsg, dwFlags, dwParam);
            if( (dwFlags & MCI_SYSINFO_QUANTITY) &&
                    (lpInfo->wDeviceType == MCI_DEVTYPE_CD_AUDIO)) {
                dprintf("> MCI_SYSINFO MCI_SYSINFO_QUANTITY MCI_dev=DEVTYPE_CD_AUDIO force return 1\n");
                DWORD num = 1;
                // MSDN: Pointer to a user-supplied buffer for the return string.
                // It is also used to return a DWORD value when the MCI_SYSINFO_QUANTITY flag is used.
                memcpy((LPVOID)(lpInfo->lpstrReturn), (LPVOID)&num, sizeof(DWORD));
                // MSDN: Size, in bytes, of return buffer.
                lpInfo->dwRetSize = sizeof(DWORD);
                // MSDN: Number indicating the device position in the MCI device table or in the list
                // of open devices if the MCI_SYSINFO_OPEN flag is set.
                lpInfo->dwNumber = dxw.VirtualCDAudioDeviceId; // v2.05.36
                ret = 0; // v2.05.36: pretend it's OK
            }
            if(ret) return ret; // v2.05.36
            if(dwFlags & MCI_SYSINFO_NAME) {
                dprintf("> MCI_SYSINFO ret=\"%ls\" retsize=%d num=%d devtype=%d\n",
                        lpInfo->lpstrReturn,
                        lpInfo->dwRetSize,
                        lpInfo->dwNumber,
                        lpInfo->wDeviceType);
            } else {
                dprintf("> MCI_SYSINFO ret=%d retsize=%d num=%d devtype=%d\n",
                        *(DWORD *)lpInfo->lpstrReturn,
                        lpInfo->dwRetSize,
                        lpInfo->dwNumber,
                        lpInfo->wDeviceType);
            }
        }
    }
    break;
    case MCI_STATUS: {
        dprintf("> MCI_STATUS\n");
        LPMCI_STATUS_PARMS parms = (LPMCI_STATUS_PARMS)dwParam;
        parms->dwReturn = 0;
        if (dwFlags & MCI_TRACK)
            dprintf("> MCI_TRACK dwTrack = %d\n", parms->dwTrack);
        if (dwFlags & MCI_STATUS_ITEM) {
            dprintf("> MCI_STATUS_ITEM\n");
            _if (parms->dwItem == MCI_STATUS_CURRENT_TRACK) dprintf("> MCI_STATUS_CURRENT_TRACK\n");
            if (parms->dwItem == MCI_STATUS_LENGTH) {
                int seconds;
                dprintf("> MCI_STATUS_LENGTH tf=%s\n", sTimeFormat(mciEmu.dwTimeFormat));
                if(dwFlags & MCI_TRACK) {
                    // get length of specified track
                    seconds = tracks[parms->dwTrack].length;
                } else {
                    // v2.05.01: get total length of CD (fixed)
                    seconds = TTMMSSToAbsSec(mciEmu.dwLastTrack);
                }
                if (seconds == 0) seconds = 4; // assume no track is less than 4 seconds
                parms->dwReturn = DecodeSec(seconds);
                if(dxw.dwFlags11 & HACKMCIFRAMES) {
                    char *sRoom = NULL;
                    switch(mciEmu.dwTimeFormat) {
                    case MCI_FORMAT_FRAMES:
                        sRoom = "frames";
                        break;
                    case MCI_FORMAT_MILLISECONDS:
                        sRoom = "msec";
                        break;
                    case MCI_FORMAT_TMSF:
                        sRoom = "tmsf";
                        break;
                    }
                    if(sRoom) {
                        DWORD val;
                        char sKey[20];
                        if(dwFlags & MCI_TRACK) sprintf(sKey, "track%02d", parms->dwTrack);
                        else strcpy(sKey, "cd");
                        val = (*pGetPrivateProfileIntA)(sRoom, sKey, 0, ".\\Music\\mcihack.txt");
                        if(val) parms->dwReturn = val;
                        dprintf("> MCI_STATUS_LENGTH hacked value=%#x\n", parms->dwReturn);
                    }
                }
            }
            if (parms->dwItem == MCI_STATUS_POSITION) {
                if (dwFlags & MCI_TRACK) {
                    dprintf("> MCI_STATUS_POSITION MCI_TRACK\n");
                    // from MSDN: https://docs.microsoft.com/en-us/windows/desktop/multimedia/mci-status
                    // When used with MCI_STATUS_POSITION, MCI_TRACK obtains the starting position of the specified track.
                    if(parms->dwTrack < 1) parms->dwTrack = 1;
                    if(parms->dwTrack > MAX_TRACKS) parms->dwTrack = MAX_TRACKS;
                    parms->dwReturn = DecodeTTMMSS(TTMMSS_ENCODE(parms->dwTrack, 0, 0));
                } else {
                    dprintf("> MCI_STATUS_POSITION\n");
                    // v2.05.00/01: from MSDN - the dwReturn member is set to the current position
                    parms->dwReturn = DecodeTTMMSS(mciEmu.dwCurrentTrack);
                }
            }
            if (parms->dwItem == MCI_CDA_STATUS_TYPE_TRACK) {
                // ref. by WinQuake, Sentinel Returns
                parms->dwReturn = (tracks[parms->dwTrack].type == MCI_AUDIO_TRACK) ? MCI_CDA_TRACK_AUDIO : MCI_CDA_TRACK_OTHER;
                dprintf("> MCI_CDA_STATUS_TYPE_TRACK=%s\n", sTrackType(parms->dwReturn));
            }
            if (parms->dwItem == MCI_STATUS_MEDIA_PRESENT) {
                parms->dwReturn = mciEmu.dwLastTrack > 0;
                dprintf("> MCI_STATUS_MEDIA_PRESENT=%#x\n", parms->dwReturn);
            }
            if (parms->dwItem == MCI_STATUS_NUMBER_OF_TRACKS) {
                dprintf("> MCI_STATUS_NUMBER_OF_TRACKS\n");
                parms->dwReturn = mciEmu.dwNumTracks;
            }
            if (parms->dwItem == MCI_STATUS_MODE) {
                dprintf("> MCI_STATUS_MODE: %s\n", mciEmu.playing ? "playing" : (mciEmu.paused ? "paused" : "stop"));
                parms->dwReturn = mciEmu.playing ? MCI_MODE_PLAY : MCI_MODE_STOP;
                // v2.04.98: the MCI_MODE_PAUSE state prevents "Speedboat Attack" to send
                // a MCI_RESUME command afterwards. That should depend on some driver's capability
                if((mciEmu.paused) && (dxw.dwFlags11 & CDPAUSECAPABILITY)) parms->dwReturn = MCI_MODE_PAUSE;
            }
            if (parms->dwItem == MCI_STATUS_READY) {
                // referenced by Quake/cd_win.c
                dprintf("> MCI_STATUS_READY\n");
                parms->dwReturn = TRUE; // TRUE=ready, FALSE=not ready
            }
            _if (parms->dwItem == MCI_STATUS_TIME_FORMAT) dprintf("> MCI_STATUS_TIME_FORMAT\n");
            _if (parms->dwItem == MCI_STATUS_START) dprintf("> MCI_STATUS_START\n");
        }
        dprintf("> params->dwReturn=%d(%#x)\n", parms->dwReturn, parms->dwReturn);
    }
    break;
    }
    return 0;
}
