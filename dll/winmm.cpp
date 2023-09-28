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
#include "mciplayer.h"

//#define TRACEALL
#ifdef TRACEALL
#define TRACEMIDI
#endif // TRACEALL

#define SUPPRESSMCIERRORS FALSE
#define EMULATEJOY TRUE
#define INVERTJOYAXIS TRUE

extern BOOL vjGetJoy(char *, LPJOYINFO);
extern void vjGetCaps(LPJOYCAPS);

//#include "logall.h" // comment when not debugging

MMRESULT WINAPI extauxGetDevCapsA(UINT_PTR, LPAUXCAPS, UINT);
typedef MMRESULT (WINAPI *auxGetDevCapsA_Type)(UINT_PTR, LPAUXCAPS, UINT);
auxGetDevCapsA_Type pauxGetDevCapsA;

BOOL IsWithinMCICall = FALSE;

typedef MMRESULT (WINAPI *timeGetDevCaps_Type)(LPTIMECAPS, UINT);
timeGetDevCaps_Type ptimeGetDevCaps = NULL;
MMRESULT WINAPI exttimeGetDevCaps(LPTIMECAPS, UINT);

typedef MCIDEVICEID (WINAPI *mciGetDeviceIDA_Type)(LPCTSTR);
mciGetDeviceIDA_Type pmciGetDeviceIDA = NULL;
MCIDEVICEID WINAPI extmciGetDeviceIDA(LPCTSTR);

typedef MCIDEVICEID (WINAPI *mciGetDeviceIDW_Type)(LPCWSTR);
mciGetDeviceIDW_Type pmciGetDeviceIDW = NULL;
MCIDEVICEID WINAPI extmciGetDeviceIDW(LPCWSTR);

typedef DWORD (WINAPI *joyGetNumDevs_Type)(void);
joyGetNumDevs_Type pjoyGetNumDevs = NULL;
DWORD WINAPI extjoyGetNumDevs(void);

typedef MMRESULT (WINAPI *joyGetDevCapsA_Type)(DWORD, LPJOYCAPS, UINT);
joyGetDevCapsA_Type pjoyGetDevCapsA = NULL;
MMRESULT WINAPI extjoyGetDevCapsA(DWORD, LPJOYCAPS, UINT);

typedef MMRESULT (WINAPI *joyGetPosEx_Type)(DWORD, LPJOYINFOEX);
joyGetPosEx_Type pjoyGetPosEx = NULL;
MMRESULT WINAPI extjoyGetPosEx(DWORD, LPJOYINFOEX);

typedef MMRESULT (WINAPI *joyGetPos_Type)(DWORD, LPJOYINFO);
joyGetPos_Type pjoyGetPos = NULL;
MMRESULT WINAPI extjoyGetPos(DWORD, LPJOYINFO);

typedef MMRESULT (WINAPI *joySetCapture_Type)(HWND, UINT, UINT, BOOL);
joySetCapture_Type pjoySetCapture;
MMRESULT WINAPI extjoySetCapture(HWND, UINT, UINT, BOOL);

typedef MMRESULT (WINAPI *joyReleaseCapture_Type)(UINT);
joyReleaseCapture_Type pjoyReleaseCapture;
MMRESULT WINAPI extjoyReleaseCapture(UINT);

typedef BOOL (WINAPI *mciGetErrorStringA_Type)(DWORD, LPCSTR, UINT);
mciGetErrorStringA_Type pmciGetErrorStringA;
BOOL WINAPI extmciGetErrorStringA(DWORD, LPCSTR, UINT);

typedef MMRESULT (WINAPI *mixerGetLineControlsA_Type)(HMIXEROBJ, LPMIXERLINECONTROLS, DWORD);
mixerGetLineControlsA_Type pmixerGetLineControlsA;
MMRESULT WINAPI extmixerGetLineControlsA(HMIXEROBJ, LPMIXERLINECONTROLS, DWORD);

typedef MMRESULT (WINAPI *mixerSetControlDetails_Type)(HMIXEROBJ, LPMIXERCONTROLDETAILS, DWORD);
mixerSetControlDetails_Type pmixerSetControlDetails;
MMRESULT WINAPI extmixerSetControlDetails(HMIXEROBJ, LPMIXERCONTROLDETAILS, DWORD);

typedef UINT (WINAPI *waveOutGetNumDevs_Type)(void);
waveOutGetNumDevs_Type pwaveOutGetNumDevs;
UINT WINAPI extwaveOutGetNumDevs(void);

typedef UINT (WINAPI *midiOutGetNumDevs_Type)(void);
midiOutGetNumDevs_Type pmidiOutGetNumDevs;
UINT WINAPI extmidiOutGetNumDevs(void);

typedef UINT (WINAPI *mixerGetNumDevs_Type)(void);
mixerGetNumDevs_Type pmixerGetNumDevs;
UINT WINAPI extmixerGetNumDevs(void);

//UINT WINAPI extwaveOutGetDevCapsA(UINT_PTR, LPWAVEOUTCAPSA, UINT);
//typedef UINT (WINAPI *waveOutGetDevCapsA_Type)(UINT_PTR, LPWAVEOUTCAPSA, UINT);
UINT WINAPI extwaveOutGetDevCapsA(DWORD, LPWAVEOUTCAPSA, UINT);
typedef UINT (WINAPI *waveOutGetDevCapsA_Type)(DWORD, LPWAVEOUTCAPSA, UINT);
waveOutGetDevCapsA_Type pwaveOutGetDevCapsA;

typedef MMRESULT (WINAPI *midiOutOpen_Type)(LPHMIDIOUT, UINT, DWORD_PTR, DWORD_PTR, DWORD);
midiOutOpen_Type pmidiOutOpen;
MMRESULT WINAPI extmidiOutOpen(LPHMIDIOUT, UINT, DWORD_PTR, DWORD_PTR, DWORD);

typedef MMRESULT (WINAPI *midiOutGetDevCapsA_Type)(DWORD, LPMIDIOUTCAPSA, UINT);
midiOutGetDevCapsA_Type pmidiOutGetDevCapsA;
MMRESULT WINAPI extmidiOutGetDevCapsA(DWORD, LPMIDIOUTCAPSA, UINT);

typedef MMRESULT (WINAPI *midiOutShortMsg_Type)(HMIDIOUT, DWORD);
midiOutShortMsg_Type pmidiOutShortMsg;
MMRESULT WINAPI extmidiOutShortMsg(HMIDIOUT, DWORD);

#ifdef TRACEMIDI
typedef MMRESULT (WINAPI *midiStreamPause_Type)(HMIDISTRM);
midiStreamPause_Type pmidiStreamPause;
MMRESULT WINAPI extmidiStreamPause(HMIDISTRM);

typedef MMRESULT (WINAPI *midiOutGetErrorTextA_Type)(MMRESULT, LPSTR, UINT);
midiOutGetErrorTextA_Type pmidiOutGetErrorTextA;
MMRESULT WINAPI extmidiOutGetErrorTextA(MMRESULT, LPSTR, UINT);

typedef MMRESULT (WINAPI *midiStreamOut_Type)(HMIDISTRM, LPMIDIHDR, UINT);
midiStreamOut_Type pmidiStreamOut;
MMRESULT WINAPI extmidiStreamOut(HMIDISTRM, LPMIDIHDR, UINT);

typedef MMRESULT (WINAPI *midiStreamClose_Type)(HMIDISTRM);
midiStreamClose_Type pmidiStreamClose;
MMRESULT WINAPI extmidiStreamClose(HMIDISTRM);

typedef MMRESULT (WINAPI *midiStreamOpen_Type)(LPHMIDISTRM, LPUINT, DWORD, DWORD_PTR, DWORD_PTR, DWORD);
midiStreamOpen_Type pmidiStreamOpen;
MMRESULT WINAPI extmidiStreamOpen(LPHMIDISTRM, LPUINT, DWORD, DWORD_PTR, DWORD_PTR, DWORD);

typedef MMRESULT (WINAPI *midiStreamStop_Type)(HMIDISTRM);
midiStreamStop_Type pmidiStreamStop;
MMRESULT WINAPI extmidiStreamStop(HMIDISTRM);

typedef MMRESULT (WINAPI *midiStreamRestart_Type)(HMIDISTRM);
midiStreamRestart_Type pmidiStreamRestart;
MMRESULT WINAPI extmidiStreamRestart(HMIDISTRM);

typedef MMRESULT (WINAPI *midiOutPrepareHeader_Type)(HMIDIOUT, LPMIDIHDR, UINT);
midiOutPrepareHeader_Type pmidiOutPrepareHeader;
MMRESULT WINAPI extmidiOutPrepareHeader(HMIDIOUT, LPMIDIHDR, UINT);

typedef MMRESULT (WINAPI *midiOutUnprepareHeader_Type)(HMIDIOUT, LPMIDIHDR, UINT);
midiOutPrepareHeader_Type pmidiOutUnprepareHeader;
MMRESULT WINAPI extmidiOutUnprepareHeader(HMIDIOUT, LPMIDIHDR, UINT);

typedef MMRESULT (WINAPI *midiStreamProperty_Type)(HMIDISTRM, LPBYTE, DWORD);
midiStreamProperty_Type pmidiStreamProperty;
MMRESULT WINAPI extmidiStreamProperty(HMIDISTRM, LPBYTE, DWORD);

#endif

typedef MMRESULT (WINAPI *waveOutClose_Type)(HWAVEOUT);
waveOutClose_Type pwaveOutClose;
MMRESULT WINAPI extwaveOutClose(HWAVEOUT);

typedef MMRESULT (WINAPI *midiOutClose_Type)(HMIDIOUT);
midiOutClose_Type pmidiOutClose;
MMRESULT WINAPI extmidiOutClose(HMIDIOUT);

typedef UINT (WINAPI *timeBeginPeriod_Type)(UINT);
timeBeginPeriod_Type ptimeBeginPeriod;
UINT WINAPI exttimeBeginPeriod(UINT);

typedef UINT (WINAPI *timeEndPeriod_Type)(UINT);
timeEndPeriod_Type ptimeEndPeriod;
UINT WINAPI exttimeEndPeriod(UINT);

typedef MMRESULT (WINAPI *waveOutReset_Type)(HWAVEOUT);
waveOutReset_Type pwaveOutReset;
MMRESULT WINAPI extwaveOutReset(HWAVEOUT);

typedef MMRESULT (WINAPI *waveOutRestart_Type)(HWAVEOUT);
waveOutRestart_Type pwaveOutRestart;
MMRESULT WINAPI extwaveOutRestart(HWAVEOUT);

typedef MMRESULT (WINAPI *waveOutOpen_Type)(LPHWAVEOUT, UINT_PTR, LPWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD);
waveOutOpen_Type pwaveOutOpen = NULL; // must be NULL !!!
MMRESULT WINAPI extwaveOutOpen(LPHWAVEOUT, UINT_PTR, LPWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD);

typedef MMRESULT (WINAPI *waveOutPrepareHeader_Type)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
waveOutPrepareHeader_Type pwaveOutPrepareHeader, pwaveOutUnprepareHeader;
MMRESULT WINAPI extwaveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI extwaveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);

typedef MMRESULT (WINAPI *waveOutWrite_Type)(HWAVEOUT, LPWAVEHDR, UINT);
waveOutWrite_Type pwaveOutWrite;
MMRESULT WINAPI extwaveOutWrite(HWAVEOUT, LPWAVEHDR, UINT);

typedef MMRESULT (WINAPI *auxSetVolume_Type)(UINT, DWORD);
auxSetVolume_Type pauxSetVolume;
MMRESULT WINAPI extauxSetVolume(UINT, DWORD);

typedef MMRESULT (WINAPI *auxGetVolume_Type)(UINT, LPDWORD);
auxGetVolume_Type pauxGetVolume;
MMRESULT WINAPI extauxGetVolume(UINT, LPDWORD);

typedef MMRESULT (WINAPI *waveOutSetVolume_Type)(HWAVEOUT, DWORD);
waveOutSetVolume_Type pwaveOutSetVolume;
MMRESULT WINAPI extwaveOutSetVolume(HWAVEOUT, DWORD);

typedef MMRESULT (WINAPI *waveOutGetVolume_Type)(HWAVEOUT, LPDWORD);
waveOutGetVolume_Type pwaveOutGetVolume;
MMRESULT WINAPI extwaveOutGetVolume(HWAVEOUT, LPDWORD);

typedef MMRESULT (WINAPI *auxGetNumDevs_Type)(void);
MMRESULT WINAPI extauxGetNumDevs(void);
auxGetNumDevs_Type pauxGetNumDevs; // used internally !!!

typedef MMRESULT (WINAPI *midiOutReset_Type)(HMIDIOUT);
MMRESULT WINAPI extmidiOutReset(HMIDIOUT);
midiOutReset_Type pmidiOutReset;

typedef MMRESULT (WINAPI *midiOutSetVolume_Type)(HMIDIOUT, DWORD);
MMRESULT WINAPI extmidiOutSetVolume(HMIDIOUT, DWORD);
midiOutSetVolume_Type pmidiOutSetVolume;

typedef HMMIO (WINAPI *mmioOpenA_Type)(LPCSTR, LPMMIOINFO, DWORD);
mmioOpenA_Type pmmioOpenA;
HMMIO WINAPI extmmioOpenA(LPCSTR, LPMMIOINFO, DWORD);

typedef HMMIO (WINAPI *mmioOpenW_Type)(LPCWSTR, LPMMIOINFO, DWORD);
mmioOpenW_Type pmmioOpenW;
HMMIO WINAPI extmmioOpenW(LPCWSTR, LPMMIOINFO, DWORD);

typedef BOOL (WINAPI *sndPlaySoundA_Type)(LPCSTR, UINT);
sndPlaySoundA_Type psndPlaySoundA;
BOOL WINAPI extsndPlaySoundA(LPCSTR, UINT);

typedef BOOL (WINAPI *sndPlaySoundW_Type)(LPCWSTR, UINT);
sndPlaySoundW_Type psndPlaySoundW;
BOOL WINAPI extsndPlaySoundW(LPCWSTR, UINT);

typedef BOOL (WINAPI *PlaySoundA_Type)(LPCSTR, HMODULE, UINT);
PlaySoundA_Type pPlaySoundA;
BOOL WINAPI extPlaySoundA(LPCSTR, HMODULE, UINT);

typedef BOOL (WINAPI *PlaySoundW_Type)(LPCWSTR, HMODULE, UINT);
PlaySoundW_Type pPlaySoundW;
BOOL WINAPI extPlaySoundW(LPCWSTR, HMODULE, UINT);

static HookEntryEx_Type Hooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "mciSendCommandA", NULL, (FARPROC *) &pmciSendCommandA, (FARPROC)extmciSendCommandA},
    {HOOK_IAT_CANDIDATE, 0, "mciSendCommandW", NULL, (FARPROC *) &pmciSendCommandW, (FARPROC)extmciSendCommandW},
    {HOOK_HOT_CANDIDATE, 0, "mciSendStringA", NULL, (FARPROC *) &pmciSendStringA, (FARPROC)extmciSendStringA},
    {HOOK_HOT_CANDIDATE, 0, "mciSendStringW", NULL, (FARPROC *) &pmciSendStringW, (FARPROC)extmciSendStringW},
    {HOOK_HOT_CANDIDATE, 0, "mciGetDeviceIDA", NULL, (FARPROC *) &pmciGetDeviceIDA, (FARPROC)extmciGetDeviceIDA},
    {HOOK_HOT_CANDIDATE, 0, "mciGetDeviceIDW", NULL, (FARPROC *) &pmciGetDeviceIDW, (FARPROC)extmciGetDeviceIDW},
    {HOOK_HOT_CANDIDATE, 0, "auxGetDevCapsA", NULL, (FARPROC *) &pauxGetDevCapsA, (FARPROC)extauxGetDevCapsA},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type FixIdHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "midiOutGetDevCapsA", NULL, (FARPROC *) &pmidiOutGetDevCapsA, (FARPROC)extmidiOutGetDevCapsA},
    {HOOK_HOT_CANDIDATE, 0, "midiOutOpen", NULL, (FARPROC *) &pmidiOutOpen, (FARPROC)extmidiOutOpen},
    {HOOK_IAT_CANDIDATE, 0, "waveOutGetDevCapsA", NULL, (FARPROC *) &pwaveOutGetDevCapsA, (FARPROC)extwaveOutGetDevCapsA},
    {HOOK_HOT_CANDIDATE, 0, "waveOutOpen", NULL, (FARPROC *) &pwaveOutOpen, (FARPROC)extwaveOutOpen},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type CDEmuHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "auxSetVolume", NULL, (FARPROC *) &pauxSetVolume, (FARPROC)extauxSetVolume},
    {HOOK_HOT_CANDIDATE, 0, "auxGetVolume", NULL, (FARPROC *) &pauxGetVolume, (FARPROC)extauxGetVolume},
    {HOOK_HOT_CANDIDATE, 0, "auxGetNumDevs", NULL, (FARPROC *) &pauxGetNumDevs, (FARPROC)extauxGetNumDevs},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type TimeHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "timeGetTime", NULL, (FARPROC *) &ptimeGetTime, (FARPROC)exttimeGetTime},
    {HOOK_HOT_CANDIDATE, 0, "timeKillEvent", NULL, (FARPROC *) &ptimeKillEvent, (FARPROC)exttimeKillEvent},
    {HOOK_HOT_CANDIDATE, 0, "timeSetEvent", NULL, (FARPROC *) &ptimeSetEvent, (FARPROC)exttimeSetEvent},
    {HOOK_HOT_CANDIDATE, 0, "timeGetDevCaps", NULL, (FARPROC *) &ptimeGetDevCaps, (FARPROC)exttimeGetDevCaps},
    {HOOK_HOT_CANDIDATE, 0, "timeBeginPeriod", NULL, (FARPROC *) &ptimeBeginPeriod, (FARPROC)exttimeBeginPeriod},
    {HOOK_HOT_CANDIDATE, 0, "timeEndPeriod", NULL, (FARPROC *) &ptimeEndPeriod, (FARPROC)exttimeEndPeriod},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type JoyHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "joyGetNumDevs", NULL, (FARPROC *) &pjoyGetNumDevs, (FARPROC)extjoyGetNumDevs},
    {HOOK_HOT_CANDIDATE, 0, "joyGetDevCapsA", NULL, (FARPROC *) &pjoyGetDevCapsA, (FARPROC)extjoyGetDevCapsA},
    {HOOK_HOT_CANDIDATE, 0, "joyGetPosEx", NULL, (FARPROC *) &pjoyGetPosEx, (FARPROC)extjoyGetPosEx},
    {HOOK_HOT_CANDIDATE, 0, "joyGetPos", NULL, (FARPROC *) &pjoyGetPos, (FARPROC)extjoyGetPos},
    {HOOK_HOT_CANDIDATE, 0, "joySetCapture", NULL, (FARPROC *) &pjoySetCapture, (FARPROC)extjoySetCapture},
    {HOOK_HOT_CANDIDATE, 0, "joyReleaseCapture", NULL, (FARPROC *) &pjoyReleaseCapture, (FARPROC)extjoyReleaseCapture},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type AudioHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "waveOutGetNumDevs", NULL, (FARPROC *) &pwaveOutGetNumDevs, (FARPROC)extwaveOutGetNumDevs},
    {HOOK_HOT_CANDIDATE, 0, "waveOutOpen", NULL, (FARPROC *) &pwaveOutOpen, (FARPROC)extwaveOutOpen},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type MidiHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "midiOutReset", NULL, (FARPROC *) &pmidiOutReset, (FARPROC)extmidiOutReset},
    {HOOK_HOT_CANDIDATE, 0, "midiOutSetVolume", NULL, (FARPROC *) &pmidiOutSetVolume, (FARPROC)extmidiOutSetVolume},
#ifdef TRACEMIDI
    {HOOK_HOT_CANDIDATE, 0, "midiOutShortMsg", NULL, (FARPROC *) &pmidiOutShortMsg, (FARPROC)extmidiOutShortMsg},
    {HOOK_HOT_CANDIDATE, 0, "midiStreamPause", NULL, (FARPROC *) &pmidiStreamPause, (FARPROC)extmidiStreamPause},
    {HOOK_HOT_CANDIDATE, 0, "midiOutGetErrorTextA", NULL, (FARPROC *) &pmidiOutGetErrorTextA, (FARPROC)extmidiOutGetErrorTextA},
    {HOOK_HOT_CANDIDATE, 0, "midiStreamOut", NULL, (FARPROC *) &pmidiStreamOut, (FARPROC)extmidiStreamOut},
    {HOOK_HOT_CANDIDATE, 0, "midiStreamClose", NULL, (FARPROC *) &pmidiStreamClose, (FARPROC)extmidiStreamClose},
    {HOOK_HOT_CANDIDATE, 0, "midiStreamOpen", NULL, (FARPROC *) &pmidiStreamOpen, (FARPROC)extmidiStreamOpen},
    {HOOK_HOT_CANDIDATE, 0, "midiStreamStop", NULL, (FARPROC *) &pmidiStreamStop, (FARPROC)extmidiStreamStop},
    {HOOK_HOT_CANDIDATE, 0, "midiStreamRestart", NULL, (FARPROC *) &pmidiStreamRestart, (FARPROC)extmidiStreamRestart},
    {HOOK_HOT_CANDIDATE, 0, "midiOutPrepareHeader", NULL, (FARPROC *) &pmidiOutPrepareHeader, (FARPROC)extmidiOutPrepareHeader},
    {HOOK_HOT_CANDIDATE, 0, "midiOutUnprepareHeader", NULL, (FARPROC *) &pmidiOutUnprepareHeader, (FARPROC)extmidiOutUnprepareHeader},
    {HOOK_HOT_CANDIDATE, 0, "midiStreamProperty", NULL, (FARPROC *) &pmidiStreamProperty, (FARPROC)extmidiStreamProperty},
#endif
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type PathHooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "mmioOpenA", NULL, (FARPROC *) &pmmioOpenA, (FARPROC)extmmioOpenA},
    {HOOK_HOT_CANDIDATE, 0, "mmioOpenW", NULL, (FARPROC *) &pmmioOpenW, (FARPROC)extmmioOpenW},
    {HOOK_HOT_CANDIDATE, 0, "sndPlaySoundA", NULL, (FARPROC *) &psndPlaySoundA, (FARPROC)extsndPlaySoundA},
    {HOOK_HOT_CANDIDATE, 0, "sndPlaySoundW", NULL, (FARPROC *) &psndPlaySoundW, (FARPROC)extsndPlaySoundW},
    {HOOK_HOT_CANDIDATE, 0, "PlaySoundA", NULL, (FARPROC *) &pPlaySoundA, (FARPROC)extPlaySoundA},
    {HOOK_HOT_CANDIDATE, 0, "PlaySoundW", NULL, (FARPROC *) &pPlaySoundW, (FARPROC)extPlaySoundW},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type DebugHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "auxGetNumDevs", NULL, (FARPROC *) &pauxGetNumDevs, (FARPROC)extauxGetNumDevs},
    {HOOK_HOT_CANDIDATE, 0, "auxGetVolume", NULL, (FARPROC *) &pauxGetVolume, (FARPROC)extauxGetVolume},
    {HOOK_HOT_CANDIDATE, 0, "auxSetVolume", NULL, (FARPROC *) &pauxSetVolume, (FARPROC)extauxSetVolume},
    {HOOK_IAT_CANDIDATE, 0, "mciGetErrorStringA", NULL, (FARPROC *) &pmciGetErrorStringA, (FARPROC)extmciGetErrorStringA},
    {HOOK_IAT_CANDIDATE, 0, "midiOutClose", NULL, (FARPROC *) &pmidiOutClose, (FARPROC)extmidiOutClose},
    {HOOK_IAT_CANDIDATE, 0, "midiOutGetDevCapsA", NULL, (FARPROC *) &pmidiOutGetDevCapsA, (FARPROC)extmidiOutGetDevCapsA},
    {HOOK_IAT_CANDIDATE, 0, "midiOutGetNumDevs", NULL, (FARPROC *) &pmidiOutGetNumDevs, (FARPROC)extmidiOutGetNumDevs},
    {HOOK_HOT_CANDIDATE, 0, "midiOutOpen", NULL, (FARPROC *) &pmidiOutOpen, (FARPROC)extmidiOutOpen},
    {HOOK_HOT_CANDIDATE, 0, "midiOutReset", NULL, (FARPROC *) &pmidiOutReset, (FARPROC)extmidiOutReset},
    {HOOK_HOT_CANDIDATE, 0, "midiOutSetVolume", NULL, (FARPROC *) &pmidiOutSetVolume, (FARPROC)extmidiOutSetVolume},
    {HOOK_IAT_CANDIDATE, 0, "midiOutShortMsg", NULL, (FARPROC *) &pmidiOutShortMsg, (FARPROC)extmidiOutShortMsg},
    {HOOK_IAT_CANDIDATE, 0, "mixerGetLineControlsA", NULL, (FARPROC *) &pmixerGetLineControlsA, (FARPROC)extmixerGetLineControlsA},
    {HOOK_IAT_CANDIDATE, 0, "mixerSetControlDetails", NULL, (FARPROC *) &pmixerSetControlDetails, (FARPROC)extmixerSetControlDetails},
    {HOOK_IAT_CANDIDATE, 0, "mixerGetNumDevs", NULL, (FARPROC *) &pmixerGetNumDevs, (FARPROC)extmixerGetNumDevs},
    {HOOK_IAT_CANDIDATE, 0, "waveOutClose", NULL, (FARPROC *) &pwaveOutClose, (FARPROC)extwaveOutClose},
    {HOOK_IAT_CANDIDATE, 0, "waveOutGetDevCapsA", NULL, (FARPROC *) &pwaveOutGetDevCapsA, (FARPROC)extwaveOutGetDevCapsA},
    {HOOK_IAT_CANDIDATE, 0, "waveOutGetNumDevs", NULL, (FARPROC *) &pwaveOutGetNumDevs, (FARPROC)extwaveOutGetNumDevs},
    {HOOK_HOT_CANDIDATE, 0, "waveOutGetVolume", NULL, (FARPROC *) &pwaveOutGetVolume, (FARPROC)extwaveOutGetVolume},
    {HOOK_HOT_CANDIDATE, 0, "waveOutOpen", NULL, (FARPROC *) &pwaveOutOpen, (FARPROC)extwaveOutOpen},
    {HOOK_IAT_CANDIDATE, 0, "waveOutPrepareHeader", NULL, (FARPROC *) &pwaveOutPrepareHeader, (FARPROC)extwaveOutPrepareHeader},
    {HOOK_IAT_CANDIDATE, 0, "waveOutReset", NULL, (FARPROC *) &pwaveOutReset, (FARPROC)extwaveOutReset},
    {HOOK_IAT_CANDIDATE, 0, "waveOutRestart", NULL, (FARPROC *) &pwaveOutRestart, (FARPROC)extwaveOutRestart},
    {HOOK_HOT_CANDIDATE, 0, "waveOutSetVolume", NULL, (FARPROC *) &pwaveOutSetVolume, (FARPROC)extwaveOutSetVolume},
    {HOOK_IAT_CANDIDATE, 0, "waveOutUnprepareHeader", NULL, (FARPROC *) &pwaveOutUnprepareHeader, (FARPROC)extwaveOutUnprepareHeader},
    {HOOK_IAT_CANDIDATE, 0, "waveOutWrite", NULL, (FARPROC *) &pwaveOutWrite, (FARPROC)extwaveOutWrite},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

void HookWinMM(HMODULE module) {
    //char *libname = SysLibsTable[SYSLIBIDX_WINMM].name;
    char *libname = "winmm.dll";
    HookLibraryEx(module, Hooks, libname);
    if(dxw.dwFlags11 & FIXDEFAULTMCIID) HookLibraryEx(module, FixIdHooks, libname);
    if(dxw.dwFlags8 & VIRTUALCDAUDIO) HookLibraryEx(module, CDEmuHooks, libname);
    if(dxw.dwFlags2 & TIMESTRETCH) HookLibraryEx(module, TimeHooks, libname);
    if(dxw.dwFlags8 & FIXAUDIOPCM) HookLibraryEx(module, AudioHooks, libname);
#ifdef TRACEMIDI
    HookLibraryEx(module, MidiHooks, libname);
#else
    if(dxw.dwFlags11 & SAFEMIDIOUT) HookLibraryEx(module, MidiHooks, libname);
#endif
    if((dxw.dwFlags6 & VIRTUALJOYSTICK) || (dxw.dwFlags9 & HIDEJOYSTICKS)) HookLibraryEx(module, JoyHooks, libname);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) HookLibraryEx(module, PathHooks, libname);
    if(IsDebugSND) HookLibraryEx(module, DebugHooks, libname);
}

FARPROC Remap_WinMM_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if (addr = RemapLibraryEx(proc, hModule, Hooks)) return addr;
    if(dxw.dwFlags11 & FIXDEFAULTMCIID) if (addr = RemapLibraryEx(proc, hModule, FixIdHooks)) return addr;
    if(dxw.dwFlags8 & VIRTUALCDAUDIO) if (addr = RemapLibraryEx(proc, hModule, CDEmuHooks)) return addr;
    if(dxw.dwFlags2 & TIMESTRETCH) if (addr = RemapLibraryEx(proc, hModule, TimeHooks)) return addr;
    if(dxw.dwFlags8 & FIXAUDIOPCM) if (addr = RemapLibraryEx(proc, hModule, AudioHooks)) return addr;
    if(dxw.dwFlags11 & SAFEMIDIOUT) if (addr = RemapLibraryEx(proc, hModule, MidiHooks)) return addr;
    if((dxw.dwFlags6 & VIRTUALJOYSTICK) || (dxw.dwFlags9 & HIDEJOYSTICKS)) if (addr = RemapLibraryEx(proc, hModule, JoyHooks)) return addr;
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) if (addr = RemapLibraryEx(proc, hModule, PathHooks)) return addr;
    if(IsDebugDW) if (addr = RemapLibraryEx(proc, hModule, DebugHooks)) return addr;
    return NULL;
}

void HookWinMMInit() {
    //if(dxw.dwFlags8 & VIRTUALCDAUDIO) HookLibInitEx(CDEmuHooks);
    // done with dynamic link not to create dependency with winmm
    if(dxw.dwFlags8 & VIRTUALCDAUDIO) {
        HMODULE hWinMM = (*pLoadLibraryA)("winmm.dll");
        if(!hWinMM) return;
        pauxGetNumDevs = (auxGetNumDevs_Type)(*pGetProcAddress)(hWinMM, "auxGetNumDevs");
    }
}

MMRESULT WINAPI exttimeGetDevCaps(LPTIMECAPS ptc, UINT cbtc) {
    MMRESULT res;
    ApiName("timeGetDevCaps");
    res = (*ptimeGetDevCaps)(ptc, cbtc);
    if(res)
        OutTraceE("%s: ERROR res=%#x err=%d\n", ApiRef, res, GetLastError());
    else
        OutTraceDW("%s: period min=%d max=%d\n", ApiRef, ptc->wPeriodMin, ptc->wPeriodMax);
    return MMSYSERR_NOERROR;
}

DWORD WINAPI exttimeGetTime(void) {
    DWORD ret;
    ApiName("timeGetTime");
    ret = dxw.GetTickCount();
    OutTraceT("%s: time=%#x\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI exttimeSetEvent(UINT uDelay, UINT uResolution, LPTIMECALLBACK lpTimeProc, DWORD_PTR dwUser, UINT fuEvent) {
    MMRESULT res;
    ApiName("timeSetEvent");
    UINT NewDelay;
    OutTraceT("%s: Delay=%d Resolution=%d Event=%#x\n", ApiRef, uDelay, uResolution, fuEvent);
    if(dxw.dwFlags4 & STRETCHTIMERS) NewDelay = dxw.StretchTime(uDelay);
    else NewDelay = uDelay;
    res = (*ptimeSetEvent)(NewDelay, uResolution, lpTimeProc, dwUser, fuEvent);
    if(res) dxw.PushTimer(res, uDelay, uResolution, lpTimeProc, dwUser, fuEvent);
    OutTraceT("%s: ret=%#x\n", ApiRef, res);
    return res;
}

MMRESULT WINAPI exttimeKillEvent(UINT uTimerID) {
    MMRESULT res;
    ApiName("timeKillEvent");
    OutTraceT("%s: TimerID=%#x\n", ApiRef, uTimerID);
    res = (*ptimeKillEvent)(uTimerID);
    if(res == TIMERR_NOERROR) dxw.PopTimer(uTimerID);
    OutTraceT("%s: ret=%#x\n", ApiRef, res);
    return res;
}

MMRESULT WINAPI exttimeBeginPeriod(UINT uPeriod) {
    MMRESULT res;
    ApiName("timeBeginPeriod");
    OutTraceT("%s: period=%d\n", ApiRef, uPeriod);
    res = (*ptimeBeginPeriod)(uPeriod);
    OutTraceT("%s: ret=%#x\n", ApiRef, res);
    return res;
}

MMRESULT WINAPI exttimeEndPeriod(UINT uPeriod) {
    MMRESULT res;
    ApiName("timeEndPeriod");
    OutTraceT("%s: period=%d\n", ApiRef, uPeriod);
    res = (*ptimeEndPeriod)(uPeriod);
    OutTraceT("%s: ret=%#x\n", ApiRef, res);
    return res;
}

/*  MCI_DGV_PUT_FRAME

    The rectangle defined for MCI_DGV_RECT applies to the frame rectangle.
	The frame rectangle specifies the portion of the frame buffer used as the destination of the video images obtained from the video rectangle.
	The video should be scaled to fit within the frame buffer rectangle.
    The rectangle is specified in frame buffer coordinates.
	The default rectangle is the full frame buffer.
	Specifying this rectangle lets the device scale the image as it digitizes the data.
	Devices that cannot scale the image reject this command with MCIERR_UNSUPPORTED_FUNCTION.
	You can use the MCI_GETDEVCAPS_CAN_STRETCH flag with the MCI_GETDEVCAPS command to determine if a device scales the image. A device returns FALSE if it cannot scale the image.
*/

MCIDEVICEID WINAPI extmciGetDeviceIDA(LPCTSTR lpszDevice) {
    MCIDEVICEID ret;
    ApiName("mciGetDeviceIDA");
    OutTraceDW("%s: device=\"%s\"\n", ApiRef, lpszDevice);
    ret = (*pmciGetDeviceIDA)(lpszDevice);
    if(dxw.dwFlags8 & VIRTUALCDAUDIO) {
        if(!strcmp(lpszDevice, "cd") || !strcmp(lpszDevice, "cdaudio")) ret = dxw.VirtualCDAudioDeviceId;
    }
    OutTraceDW("%s: device=\"%s\" ret=%#x\n", ApiRef, lpszDevice, ret);
    return ret;
}

MCIDEVICEID WINAPI extmciGetDeviceIDW(LPCWSTR lpszDevice) {
    MCIDEVICEID ret;
    ApiName("mciGetDeviceIDW");
    OutTraceDW("%s: device=\"%ls\"\n", ApiRef, lpszDevice);
    ret = (*pmciGetDeviceIDW)(lpszDevice);
    if(dxw.dwFlags8 & VIRTUALCDAUDIO) {
        if(!wcscmp(lpszDevice, L"cd") || !wcscmp(lpszDevice, L"cdaudio")) ret = dxw.VirtualCDAudioDeviceId;
    }
    OutTraceDW("%s: device=\"%ls\" ret=%#x\n", ApiRef, lpszDevice, ret);
    return ret;
}

DWORD WINAPI extjoyGetNumDevs(void) {
    DWORD ret;
    if(dxw.dwFlags9 & HIDEJOYSTICKS) {
        OutTraceDW("joyGetNumDevs: hide joystick ret=0\n");
        return 0;
    }
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) {
        OutTraceDW("joyGetNumDevs: emulate joystick ret=1\n");
        return 1;
    }
    ret = (*pjoyGetNumDevs)();
    OutTraceDW("joyGetNumDevs: ret=%d\n", ret);
    return ret;
}


MMRESULT WINAPI extjoyGetDevCapsA(DWORD uJoyID, LPJOYCAPSA pjc, UINT cbjc) {
    MMRESULT ret;
    OutTraceDW("joyGetDevCaps: joyid=%d size=%d\n", uJoyID, cbjc);
    if(dxw.dwFlags9 & HIDEJOYSTICKS) {
        OutTraceDW("joyGetDevCaps: hide joystick ret=MMSYSERR_NODRIVER\n");
        return MMSYSERR_NODRIVER;
    }
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) {
        if((uJoyID != -1) && (uJoyID != 0)) {
            OutTraceDW("joyGetDevCaps: ERROR joyid=%d ret=MMSYSERR_NODRIVER\n", uJoyID, cbjc);
            return MMSYSERR_NODRIVER;
        }
        if(cbjc != sizeof(JOYCAPS)) {
            OutTraceDW("joyGetDevCaps: ERROR joyid=%d size=%d ret=MMSYSERR_INVALPARAM\n", uJoyID, cbjc);
            return MMSYSERR_INVALPARAM;
        }
        // set Joystick capability structure
        vjGetCaps(pjc);
        ret = JOYERR_NOERROR;
    } else
        ret = (*pjoyGetDevCapsA)(uJoyID, pjc, cbjc);
    if(ret != JOYERR_NOERROR) {
        OutTraceE("joyGetDevCaps: ERROR joyid=%d ret=%#x\n", uJoyID, ret);
        return ret;
    }
    OutTraceDW("joyGetDevCaps: caps={"
               "manif.id=%#x prod.id=%#x name=\"%s\" "
               "pos(x:y:z)(min/max)=(%d/%d:%d/%d:%d/%d) "
               "num.buttons=%d period(min/max)=(%d/%d)"
               " caps=%#x maxaxes=%d numaxes=%d maxbtns=%d "
               "pos(r:u:v)(min/max)=(%d/%d:%d/%d:%d/%d) "
               "regkey=\"%s\" oem=\"%s\""
               "}\n",
               pjc->wMid, pjc->wPid, pjc->szPname,
               pjc->wXmin, pjc->wXmax, pjc->wYmin, pjc->wYmax, pjc->wZmin, pjc->wZmax,
               pjc->wNumButtons, pjc->wPeriodMin, pjc->wPeriodMax
               , pjc->wCaps, pjc->wMaxAxes, pjc->wNumAxes, pjc->wMaxButtons,
               pjc->wRmin, pjc->wRmax, pjc->wUmin, pjc->wUmax, pjc->wVmin, pjc->wVmax,
               pjc->szRegKey, pjc->szOEMVxD
              );
    return JOYERR_NOERROR;
}

BOOL JoyProcessMouseWheelMessage(WPARAM wParam, LPARAM lParam) {
    int zDelta;
    DWORD dwSensivity = GetHookInfo()->VJoySensivity;
    DWORD dwJoyStatus =	GetHookInfo()->VJoyStatus;
    if(!(dwJoyStatus & VJMOUSEWHEEL)) return FALSE;
    if(!dwSensivity) dwSensivity = 100;
    //fwKeys = GET_KEYSTATE_WPARAM(wParam);
    zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    if(zDelta >  4 * WHEEL_DELTA) zDelta =  4 * WHEEL_DELTA;
    if(zDelta < -4 * WHEEL_DELTA) zDelta = -4 * WHEEL_DELTA;
    if(zDelta > 0) dwSensivity = (dwSensivity * 110 *  zDelta) / (100 * WHEEL_DELTA);
    if(zDelta < 0) dwSensivity = (dwSensivity * 100 * -zDelta) / (110 * WHEEL_DELTA);
    if(dwSensivity < 32) dwSensivity = 32;
    if(dwSensivity > 250) dwSensivity = 250;
    GetHookInfo()->VJoySensivity = dwSensivity;
    return TRUE;
}

MMRESULT WINAPI extjoyGetPosEx(DWORD uJoyID, LPJOYINFOEX pji) {
    JOYINFO jinfo;
    ApiName("joyGetPosEx");
    OutDebugIN("%s: joyid=%#x\n", ApiRef, uJoyID);
    if(!(dxw.dwFlags6 & VIRTUALJOYSTICK))
        return (*pjoyGetPosEx)(uJoyID, pji);
    if(uJoyID != 0) return JOYERR_UNPLUGGED;
    vjGetJoy("joyGetPosEx", &jinfo);
    // set Joystick JOYINFOEX info structure
    memset(pji, 0, sizeof(JOYINFOEX));
    pji->dwSize = sizeof(JOYINFOEX);
    pji->dwFlags = 0;
    pji->dwXpos = jinfo.wXpos;
    pji->dwYpos = jinfo.wYpos;
    pji->dwButtons = jinfo.wButtons;
    pji->dwFlags = JOY_RETURNX | JOY_RETURNY | JOY_RETURNBUTTONS;
    return JOYERR_NOERROR;
}

MMRESULT WINAPI extjoyGetPos(DWORD uJoyID, LPJOYINFO pji) {
    ApiName("joyGetPos");
    OutDebugIN("%s: joyid=%#x\n", ApiRef, uJoyID);
    if(!(dxw.dwFlags6 & VIRTUALJOYSTICK))
        return (*pjoyGetPos)(uJoyID, pji);
    if(uJoyID != 0) return JOYERR_UNPLUGGED;
    vjGetJoy(ApiRef, pji);
    return JOYERR_NOERROR;
}

MMRESULT WINAPI extjoySetCapture(HWND hwnd, UINT uJoyID, UINT uPeriod, BOOL fChanged) {
    ApiName("joySetCapture");
    OutDebugIN("%s: hwnd=%#x joyid=%#x period=%d changed=%#x\n",
               ApiRef, hwnd, uJoyID, uPeriod, fChanged);
    if(!(dxw.dwFlags6 & VIRTUALJOYSTICK))
        return (*pjoySetCapture)(hwnd, uJoyID, uPeriod, fChanged);
    if(uJoyID != 0) return JOYERR_UNPLUGGED;
    return JOYERR_NOERROR;
}

MMRESULT WINAPI extjoyReleaseCapture(UINT uJoyID) {
    ApiName("joyReleaseCapture");
    OutDebugIN("%s: joyid=%#x\n", ApiRef, uJoyID);
    if(!(dxw.dwFlags6 & VIRTUALJOYSTICK))
        return (*pjoyReleaseCapture)(uJoyID);
    if(uJoyID != 0) return JOYERR_UNPLUGGED;
    return JOYERR_NOERROR;
}

MMRESULT WINAPI extauxGetNumDevs(void) {
    ApiName("auxGetNumDevs");
    UINT ret;
    ret = (*pauxGetNumDevs)();
    OutTraceSND("%s: ret=%d\n", ApiRef, ret);
    if(dxw.dwFlags8 & VIRTUALCDAUDIO) {
        // v2.05.36: pretend there's ALWAYS a first device id = 0 for the audio mixer
        if (ret == 0) {
            ret = 1;
            OutTraceSND("%s: adding fake MIXER id=0\n", ApiRef);
        }
        dxw.VirtualCDAudioDeviceId = ret;
        ret++;
        OutTraceSND("%s: adding fake CD id=%d ret=%d\n",
                    ApiRef, dxw.VirtualCDAudioDeviceId, ret);
    }
    return ret;
}

BOOL WINAPI extmciGetErrorStringA(DWORD fdwError, LPCSTR lpszErrorText, UINT cchErrorText) {
    BOOL ret;
    ret = (*pmciGetErrorStringA)(fdwError, lpszErrorText, cchErrorText);
    OutTraceSND("mciGetErrorStringA: ret=%#x err=%d text=(%d)\"%s\"\n", ret, fdwError, cchErrorText, lpszErrorText);
    return ret;
}

MMRESULT WINAPI extmixerGetLineControlsA(HMIXEROBJ hmxobj, LPMIXERLINECONTROLS pmxlc, DWORD fdwControls) {
    MMRESULT ret;
    ret = (*pmixerGetLineControlsA)(hmxobj, pmxlc, fdwControls);
    OutTraceSND("mixerGetLineControlsA: ret=%#x hmxobj=%#x Controls=%#x\n", ret, hmxobj, fdwControls);
#ifndef DXW_NOTRACES
    if((ret == DD_OK) && IsDebugSND) {
        MIXERCONTROL *mc;
        OutTrace("> size=%d\n", pmxlc->cbStruct);
        OutTrace("> lineid=%#x\n", pmxlc->dwLineID);
        OutTrace("> control=%#x\n", pmxlc->dwControlID);
        OutTrace("> controls=%d\n", pmxlc->cControls);
        OutTrace("> bmxctrl=%d\n", pmxlc->cbmxctrl);
        mc = pmxlc->pamxctrl;
        for(int i = 0; i < (int)pmxlc->cControls; i++) {
            OutTrace("> [%d]\n", i);
            OutTrace(">> size=%d\n", mc->cbStruct);
            OutTrace(">> ControlID=%#x\n", mc->dwControlID);
            OutTrace(">> ControlType=%#x\n", mc->dwControlType);
            OutTrace(">> fControl=%#x\n", mc->fdwControl);
            OutTrace(">> MultipleItems=%d\n", mc->cMultipleItems);
            OutTrace(">> shortname=\"%s\"\n", mc->szShortName);
            OutTrace(">> name=\"%s\"\n", mc->szName);
            OutTrace(">> Min/Max=%d/%d\n", mc->Bounds.lMinimum, mc->Bounds.lMaximum);
            mc = (MIXERCONTROL *)((LPBYTE)mc + pmxlc->cbmxctrl);
        }
    }
#endif
    return ret;
}

MMRESULT WINAPI extmixerSetControlDetails(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails) {
    MMRESULT ret;
    ApiName("mixerSetControlDetails");
#ifndef DXW_NOTRACES
    char *sDetails[] = {"MIXER", "WAVEOUT", "WAVEIN", "MIDIOUT", "MIDIIN", "AUX", "", ""};
    OutTraceSND("%s: hmxobj=%#x details=%#x(%s:%s%s)\n",
                ApiRef, hmxobj, fdwDetails,
                fdwDetails & MIXER_SETCONTROLDETAILSF_CUSTOM ? "CUSTOM" : "VALUE",
                fdwDetails & MIXER_OBJECTF_HANDLE ? "H" : "",
                sDetails[(fdwDetails & 0x70000000) >> 28]
               );
    if(IsDebugSND) {
        OutTrace("> cbStruct=%d\n", pmxcd->cbStruct);
        OutTrace("> dwControlID=%#x\n", pmxcd->dwControlID);
        OutTrace("> cChannels=%d\n", pmxcd->cChannels);
        if(fdwDetails & MIXER_SETCONTROLDETAILSF_CUSTOM)
            OutTrace("> hwndOwner=%#x\n", pmxcd->hwndOwner);
        else
            OutTrace("> cMultipleItems=%d\n", pmxcd->cMultipleItems);
        OutTrace("> cbDetails=%d\n", pmxcd->cbDetails);
        for(DWORD i = 0; i < pmxcd->cbDetails; i++) {
            LPMIXERCONTROLDETAILS_LISTTEXTA p = (LPMIXERCONTROLDETAILS_LISTTEXTA)pmxcd->paDetails;
            // note: better not try to dump p->szName since it doesn't seem to hold a clean string
            OutTrace(">> Detail[%d]: %#x %#x\n", i, p->dwParam1, p->dwParam2);
            p++;
        }
    }
#endif // DXW_NOTRACES
    ret = (*pmixerSetControlDetails)(hmxobj, pmxcd, fdwDetails);
    _if(ret) OutTraceE("%s: ERROR res=%#x\n", ApiRef, ret);
    return ret;
}

#ifndef DXW_NOTRACES
static char *sSupport(DWORD c) {
    static char eb[128];
    unsigned int l;
    strcpy(eb, "WAVECAPS_");
    if (c & WAVECAPS_PITCH) strcat(eb, "PITCH+");
    if (c & WAVECAPS_PLAYBACKRATE) strcat(eb, "PLAYBACKRATE+");
    if (c & WAVECAPS_VOLUME) strcat(eb, "VOLUME+");
    if (c & WAVECAPS_LRVOLUME) strcat(eb, "LRVOLUME+");
    if (c & WAVECAPS_SYNC) strcat(eb, "SYNC+");
    if (c & WAVECAPS_SAMPLEACCURATE) strcat(eb, "SAMPLEACCURATE+");
    l = strlen(eb);
    if (l > strlen("WAVECAPS_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}
#endif

//UINT WINAPI extwaveOutGetDevCapsA(UINT_PTR uDeviceID, LPWAVEOUTCAPSA pwoc, UINT cbwoc)
MMRESULT WINAPI extwaveOutGetDevCapsA(DWORD uDeviceID, LPWAVEOUTCAPSA pwoc, UINT cbwoc) {
    MMRESULT ret;
    ApiName("waveOutGetDevCapsA");
    OutTraceSND("%s: dev=%#x siz=%d\n", ApiRef, uDeviceID, cbwoc);
    if((dxw.dwFlags11 & FIXDEFAULTMCIID) && (uDeviceID == 0x0000FFFF)) {
        uDeviceID = 0xFFFFFFFF;
        OutTraceSND("%s: fixing default dev=%#x\n", ApiRef, uDeviceID);
    }
    ret = (*pwaveOutGetDevCapsA)(uDeviceID, pwoc, cbwoc);
    if(ret) {
        OutTraceE("%s: ret=%d\n", ApiRef, ret);
        return ret;
    }
    OutTraceSND("> Mid = %u\n", pwoc->wMid);
    OutTraceSND("> Pid = %u\n", pwoc->wPid);
    OutTraceSND("> DriverVersion = %u.%u\n", pwoc->vDriverVersion / 256, pwoc->vDriverVersion % 256);
    OutTraceSND("> Pname = %s\n", pwoc->szPname);
    OutTraceSND("> Formats = %#x\n", pwoc->dwFormats);
    OutTraceSND("> Channels = %u\n", pwoc->wChannels);
    OutTraceSND("> Reserved1 = %#x\n", pwoc->wReserved1);
    OutTraceSND("> Support = %#x(%s)\n", pwoc->dwSupport, sSupport(pwoc->dwSupport));
    return ret;
}

#ifndef DXW_NOTRACES
//#define MOD_MIDIPORT    1  /* output port */
//#define MOD_SYNTH       2  /* generic internal synth */
//#define MOD_SQSYNTH     3  /* square wave internal synth */
//#define MOD_FMSYNTH     4  /* FM internal synth */
//#define MOD_MAPPER      5  /* MIDI mapper */
//#define MOD_WAVETABLE   6  /* hardware wavetable synth */
//#define MOD_SWSYNTH     7  /* software synth */

static char *sMidiTechnology(DWORD s) {
    char *captions[] = {"MIDIPORT", "SYNTH", "SQSYNTH", "FMSYNTH", "MAPPER", "WAVETABLE", "SWSYNTH"};
    if((s > 0) && (s < 8)) return captions[s - 1];
    return "unknown";
}
#endif

MMRESULT WINAPI extmidiOutGetDevCapsA(DWORD uDeviceID, LPMIDIOUTCAPSA pmoc, UINT cbmoc) {
    UINT ret;
    ApiName("midiOutGetDevCapsA");
    OutTraceSND("%s: dev=%#x siz=%d\n", ApiRef, uDeviceID, cbmoc);
    if((dxw.dwFlags11 & FIXDEFAULTMCIID) && (uDeviceID == 0x0000FFFF)) {
        uDeviceID = 0xFFFFFFFF;
        OutTraceSND("%s: fixing default dev=%#x\n", ApiRef, uDeviceID);
    }
    ret = (*pmidiOutGetDevCapsA)(uDeviceID, pmoc, cbmoc);
    if(ret) {
        OutTraceE("%s: ret=%d\n", ApiRef, ret);
        return ret;
    }
    OutTraceSND("> Mid = %u\n", pmoc->wMid);
    OutTraceSND("> Pid = %u\n", pmoc->wPid);
    OutTraceSND("> DriverVersion = %u.%u\n", pmoc->vDriverVersion / 256, pmoc->vDriverVersion % 256);
    OutTraceSND("> Pname = %s\n", pmoc->szPname);
    OutTraceSND("> Technology = %#x(%s)\n", pmoc->wTechnology, sMidiTechnology(pmoc->wTechnology));
    OutTraceSND("> Voices = %d\n", pmoc->wVoices);
    OutTraceSND("> Notes = %d\n", pmoc->wNotes);
    OutTraceSND("> ChannelMask = %#x\n", pmoc->wChannelMask);
    OutTraceSND("> Support = %#x(%s)\n", pmoc->dwSupport, sSupport(pmoc->dwSupport));
    return ret;
}

MMRESULT WINAPI extmidiOutOpen(LPHMIDIOUT phmo, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen) {
    MMRESULT ret;
    ApiName("midiOutOpen");
    //OutTraceSND("%s: flags=%#x(%s)\n", ApiRef, fdwOpen, swoCallback(fdwOpen));
    OutTraceSND("%s: flags=%#x\n", ApiRef, fdwOpen);
    if(uDeviceID == MIDI_MAPPER)
        OutTraceSND("%s: devid=MIDI_MAPPER\n", ApiRef);
    else
        OutTraceSND("%s: devid=%#x\n", ApiRef, uDeviceID);
    if((dxw.dwFlags11 & FIXDEFAULTMCIID) && (uDeviceID == 0x0000FFFF)) {
        uDeviceID = 0xFFFFFFFF;
        OutTraceSND("%s: fixing default dev=%#x\n", ApiRef, uDeviceID);
    }
    ret = (*pmidiOutOpen)(phmo, uDeviceID, dwCallback, dwInstance, fdwOpen);
    if(ret == MMSYSERR_NOERROR) {
        OutTraceSND("%s: hmo=%#x\n", ApiRef, *phmo);
        return ret;
    }
    OutTraceSYS("%s: ERROR ret=%d\n", ApiRef, ret);
    if(ret == MCIERR_DEVICE_LOCKED) {
        for (int retry = 0; retry < 4; retry++) {
            (*pSleep)(4000);
            ret = (*pmidiOutOpen)(phmo, uDeviceID, dwCallback, dwInstance, fdwOpen);
            if(ret == MMSYSERR_NOERROR) break;
        }
        OutTraceSND("%s: LOCKED ret=%d at %d\n", ApiRef, ret, __LINE__);
    }
    return ret;
}

UINT WINAPI extmidiOutGetNumDevs() {
    UINT ret;
    ApiName("midiOutGetNumDevs");
    ret = (*pmidiOutGetNumDevs)();
    OutTraceSND("%s: numdevs=%d\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI extwaveOutClose(HWAVEOUT hwo) {
    MMRESULT ret;
    ApiName("waveOutClose");
    OutTraceSND("%s: hwo=%#x\n", ApiRef, hwo);
    ret = (*pwaveOutClose)(hwo);
    OutTraceSND("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI extmidiOutClose(HMIDIOUT hmo) {
    MMRESULT ret;
    ApiName("midiOutClose");
    OutTraceSND("%s: hmo=%#x\n", ApiRef, hmo);
    ret = (*pmidiOutClose)(hmo);
    OutTraceSND("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI extmidiOutShortMsg(HMIDIOUT hmo, DWORD dwMsg) {
    MMRESULT ret;
    ApiName("midiOutShortMsg");
    OutTraceSND("%s: hmo=%#x msg=%#x\n", ApiRef, hmo, dwMsg);
    ret = (*pmidiOutShortMsg)(hmo, dwMsg);
    OutTraceSND("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

#ifdef TRACEMIDI
MMRESULT WINAPI extmidiStreamPause(HMIDISTRM hms) {
    MMRESULT ret;
    ApiName("midiStreamPause");
    OutTraceSND("%s: hms=%#x\n", ApiRef, hms);
    ret = (*pmidiStreamPause)(hms);
    OutTraceSND("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI extmidiOutGetErrorTextA(MMRESULT mmError, LPSTR pszText, UINT cchText) {
    MMRESULT ret;
    ApiName("midiOutGetErrorTextA");
    OutTraceSND("%s: mmerr=%#x cch=%d\n", ApiRef, mmError, cchText);
    ret = (*pmidiOutGetErrorTextA)(mmError, pszText, cchText);
    OutTraceSND("%s: ret=%d text=\"%s\"\n", ApiRef, ret, pszText);
    return ret;
}

MMRESULT WINAPI extmidiStreamOut(HMIDISTRM hms, LPMIDIHDR pmh, UINT cbmh) {
    MMRESULT ret;
    ApiName("midiStreamOut");
    OutTraceSND("%s: hms=%#x pmh=%#x cbmh=%d\n", ApiRef, pmh, cbmh);
    ret = (*pmidiStreamOut)(hms, pmh, cbmh);
    OutTraceSND("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI extmidiStreamClose(HMIDISTRM hms) {
    MMRESULT ret;
    ApiName("midiStreamClose");
    OutTraceSND("%s: hms=%#x\n", ApiRef, hms);
    ret = (*pmidiStreamClose)(hms);
    OutTraceSND("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI extmidiStreamOpen(LPHMIDISTRM phms, LPUINT puDeviceID, DWORD cMidi, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen) {
    MMRESULT ret;
    ApiName("midiStreamOpen");
    char *fdwOpenStrings[] = {"NULL", "WINDOW", "TASK", "FUNCTION", "", "EVENT", "", ""};
    OutTraceSND("%s: phms=%#x devid=%#x cmidi=%#x cback=%#x instance=%#x fdwopen=%#x(%s)\n",
                ApiRef, phms,
                puDeviceID ? *puDeviceID : NULL,
                cMidi,
                dwCallback,
                dwInstance,
                fdwOpen,
                fdwOpenStrings[(fdwOpen & CALLBACK_TYPEMASK) >> 16]);
    ret = (*pmidiStreamOpen)(phms, puDeviceID, cMidi, dwCallback, dwInstance, fdwOpen);
    OutTraceSND("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI extmidiStreamStop(HMIDISTRM hms) {
    MMRESULT ret;
    ApiName("midiStreamStop");
    OutTraceSND("%s: hms=%#x\n", ApiRef, hms);
    ret = (*pmidiStreamStop)(hms);
    OutTraceSND("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI extmidiStreamRestart(HMIDISTRM hms) {
    MMRESULT ret;
    ApiName("midiStreamRestart");
    OutTraceSND("%s: hms=%#x\n", ApiRef, hms);
    ret = (*pmidiStreamRestart)(hms);
    OutTraceSND("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI extmidiOutPrepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh) {
    MMRESULT ret;
    ApiName("midiOutPrepareHeader");
    OutTraceSND("%s: hmo=%#x pmh=%#x cbmh=%d\n", ApiRef, pmh, cbmh);
    ret = (*pmidiOutPrepareHeader)(hmo, pmh, cbmh);
    OutTraceSND("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI extmidiOutUnprepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh) {
    MMRESULT ret;
    ApiName("midiOutUnprepareHeader");
    OutTraceSND("%s: hmo=%#x pmh=%#x cbmh=%d\n", ApiRef, pmh, cbmh);
    ret = (*pmidiOutUnprepareHeader)(hmo, pmh, cbmh);
    OutTraceSND("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI extmidiStreamProperty(HMIDISTRM hms, LPBYTE lppropdata, DWORD dwProperty) {
    MMRESULT ret;
    ApiName("midiStreamProperty");
    OutTraceSND("%s: hms=%#x prop=%#x(%s%s+%s%s)\n", ApiRef, hms,
                dwProperty,
                dwProperty & MIDIPROP_GET ? "GET" : "",
                dwProperty & MIDIPROP_SET ? "SET" : "",
                dwProperty & MIDIPROP_TIMEDIV ? "TIMEDIV" : "",
                dwProperty & MIDIPROP_TEMPO ? "TEMPO" : ""
               );
    ret = (*pmidiStreamProperty)(hms, lppropdata, dwProperty);
    OutTraceSND("%s: ret=%d\n", ApiRef, ret);
    return ret;
}
#endif

UINT WINAPI extmixerGetNumDevs(void) {
    UINT ret;
    ApiName("mixerGetNumDevs");
    ret = (*pmixerGetNumDevs)();
    OutTraceSND("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

#ifndef WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE
#define WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE 0x10
#endif

#ifndef DXW_NOTRACES
static char *swoCallback(DWORD c) {
    char *s;
    switch(c & CALLBACK_TYPEMASK) {
    case CALLBACK_NULL:
        s = "CALLBACK_NULL";
        break;
    case CALLBACK_WINDOW:
        s = "CALLBACK_WINDOW";
        break;
    case CALLBACK_TASK:
        s = "CALLBACK_TASK";
        break;
    case CALLBACK_FUNCTION:
        s = "CALLBACK_FUNCTION";
        break;
    case CALLBACK_EVENT:
        s = "CALLBACK_EVENT";
        break;
    default:
        s = "???";
        break;
    }
    return s;
}
#endif

MMRESULT WINAPI extwaveOutOpen(LPHWAVEOUT phwo, UINT_PTR uDeviceID, LPWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD fdwOpen) {
    MMRESULT ret;
    ApiName("waveOutOpen");
    OutTraceSND("%s: flags=%#x(%s)\n", ApiRef, fdwOpen, swoCallback(fdwOpen));
    if(uDeviceID == WAVE_MAPPER)
        OutTraceSND("%s: devid=WAVE_MAPPER\n", ApiRef);
    else
        OutTraceSND("%s: devid=%#x\n", ApiRef, uDeviceID);
    OutTraceSND("%s: waveformat=(Tag=%#x Channels=%d AvgBytesPerSec=%d BlockAlign=%d BitsXSample=%d SamplesXSec=%d Size=%d)\n",
                ApiRef,
                pwfx->wFormatTag,
                pwfx->nChannels,
                pwfx->nAvgBytesPerSec,
                pwfx->nBlockAlign,
                pwfx->wBitsPerSample,
                pwfx->nSamplesPerSec,
                pwfx->cbSize);
    if((dxw.dwFlags11 & FIXDEFAULTMCIID) && (uDeviceID == 0x0000FFFF)) {
        uDeviceID = 0xFFFFFFFF;
        OutTraceSND("%s: fixing default dev=%#x\n", ApiRef, uDeviceID);
    }
    ret = (*pwaveOutOpen)(phwo, uDeviceID, pwfx, dwCallback, dwCallbackInstance, fdwOpen);
    if(ret == MMSYSERR_NOERROR) {
        // v2.05.15: phwho could be NULL!!
        OutTraceSND("%s: hwo=%#x\n", ApiRef, phwo ? *phwo : NULL);
        return ret;
    }
    OutTraceSYS("%s: ERROR ret=%d\n", ApiRef, ret);
    if((ret == WAVERR_BADFORMAT) && (pwfx->nChannels > 1)) {
        OutTraceSND("%s: bad format, try recovey with MONO channels\n", ApiRef);
        //pwfx->cbSize = 0;
        //pwfx->wFormatTag = WAVE_FORMAT_PCM;
        pwfx->nChannels = 1; // mono
        //pwfx->wBitsPerSample = 16; // 16 bit
        //pwfx->nSamplesPerSec = 8000; // 8 kHz
        //pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
        //pwfx->nAvgBytesPerSec = pwfx->nSamplesPerSec * pwfx->nBlockAlign;
        ret = (*pwaveOutOpen)(phwo, uDeviceID, pwfx, dwCallback, dwCallbackInstance, fdwOpen);
        OutTraceSND("%s: MONO ret=%d at %d\n", ApiRef, ret, __LINE__);
    }
    if(ret == MCIERR_DEVICE_LOCKED) {
        for (int retry = 0; retry < 4; retry++) {
            (*pSleep)(4000);
            ret = (*pwaveOutOpen)(phwo, uDeviceID, pwfx, dwCallback, dwCallbackInstance, fdwOpen);
            if(ret == MMSYSERR_NOERROR) break;
        }
        OutTraceSND("%s: LOCKED ret=%d at %d\n", ApiRef, ret, __LINE__);
    }
    return ret;
}

MMRESULT WINAPI extwaveOutReset(HWAVEOUT hwo) {
    MMRESULT ret;
    ApiName("waveOutReset");
    OutTraceSND("%s: hwo=%#x\n", ApiRef, hwo);
    ret = (*pwaveOutReset)(hwo);
    OutTraceSND("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI extwaveOutRestart(HWAVEOUT hwo) {
    MMRESULT ret;
    ApiName("waveOutRestart");
    OutTraceSND("%s: hwo=%#x\n", ApiRef, hwo);
    ret = (*pwaveOutRestart)(hwo);
    OutTraceSND("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

UINT WINAPI extwaveOutGetNumDevs() {
    UINT ret;
    ApiName("waveOutGetNumDevs");
    ret = (*pwaveOutGetNumDevs)();
    OutTraceSND("%s: numdevs=%d\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI extwaveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh) {
    MMRESULT ret;
    ApiName("waveOutPrepareHeader");
    OutTraceSND("%s: hwo=%#x cbwh=%#x\n", ApiRef, hwo, cbwh);
    ret = (*pwaveOutPrepareHeader)(hwo, pwh, cbwh);
    OutTraceSND("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI extwaveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh) {
    MMRESULT ret;
    ApiName("waveOutUnprepareHeader");
    OutTraceSND("%s: hwo=%#x cbwh=%#x\n", ApiRef, hwo, cbwh);
    ret = (*pwaveOutUnprepareHeader)(hwo, pwh, cbwh);
    OutTraceSND("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

MMRESULT WINAPI extwaveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh) {
    MMRESULT ret;
    ApiName("waveOutWrite");
    OutTraceSND("%s: hwo=%#x cbwh=%#x\n", ApiRef, hwo, cbwh);
    ret = (*pwaveOutWrite)(hwo, pwh, cbwh);
    OutTraceSND("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

extern MCIERROR WINAPI extmciSendString(char *, LPCTSTR, LPTSTR, UINT, HANDLE);

MCIERROR WINAPI extmciSendStringA(LPCTSTR lpszCommand, LPTSTR lpszReturnString, UINT cchReturn, HANDLE hwndCallback) {
    if(IsWithinMCICall) return(*pmciSendStringA)(lpszCommand, lpszReturnString, cchReturn, hwndCallback); // just proxy ...
    if(!strcmp(lpszCommand, "play music notify")) return 0;
    MCIERROR ret;
    LPCSTR lpszCommandA;
    size_t len;
    len = strlen(lpszCommand);
    lpszCommandA = (LPCSTR)malloc(len + 1);
    strcpy((LPSTR)lpszCommandA, lpszCommand);
    // v2.04.96: "Fallen Haven" sends uppercase strings ...
    for(char *p = (char *)lpszCommandA; *p; p++) *p = tolower(*p);
    ret = extmciSendString("mciSendStringA", lpszCommandA, lpszReturnString, cchReturn, hwndCallback);
    free((LPVOID)lpszCommandA);
    return ret;
}

MCIERROR WINAPI extmciSendStringW(LPCWSTR lpszCommand, LPWSTR lpszReturnString, UINT cchReturn, HANDLE hwndCallback) {
    if(IsWithinMCICall) return(*pmciSendStringW)(lpszCommand, lpszReturnString, cchReturn, hwndCallback); // just proxy ...
    MCIERROR ret;
    LPCSTR lpszCommandA;
    LPCSTR lpszReturnStringA;
    size_t len;
    BOOL returnsDWord = FALSE;
    len = wcslen(lpszCommand);
    lpszCommandA = (LPCSTR)malloc((2 * len) + 1);
    lpszReturnStringA = (LPCSTR)malloc((2 * cchReturn) + 1);
    _wcstombs_s_l(&len, (char *)lpszCommandA, 2 * len, lpszCommand, _TRUNCATE, NULL);
    for(char *p = (char *)lpszCommandA; *p; p++) *p = tolower(*p);
    // decides whether to convert a string or just copy a DWORD value
    if (!strncmp(lpszCommandA, "window", strlen("window")) ||
            !strncmp(lpszCommandA, "set", strlen("set")) ||
            !strncmp(lpszCommandA, "status", strlen("status")) ||
            !strncmp(lpszCommandA, "configure", strlen("configure")) ||
            !strncmp(lpszCommandA, "put", strlen("put"))
       ) returnsDWord = TRUE;
    ret = extmciSendString("mciSendStringW", lpszCommandA, (LPTSTR)lpszReturnStringA, cchReturn, hwndCallback);
    // v2.04.54 fix: not all retstrings are strings!!
    if(returnsDWord)
        memcpy (lpszReturnString, lpszReturnStringA, cchReturn);
    else
        mbstowcs (lpszReturnString, lpszReturnStringA, cchReturn);
    free((LPVOID)lpszCommandA);
    free((LPVOID)lpszReturnStringA);
    return ret;
}

MMRESULT WINAPI extauxGetDevCapsA(UINT_PTR uDeviceID, LPAUXCAPSA lpCaps, UINT cbCaps)
//MMRESULT WINAPI extauxGetDevCapsA(DWORD uDeviceID, LPAUXCAPSA lpCaps, UINT cbCaps)
{
    MMRESULT ret;
    ApiName("auxGetDevCapsA");
    dprintf("%s: uDeviceID=%08X cbCaps=%d\n", ApiRef, uDeviceID, cbCaps);
#ifndef DXW_NOTRACES
    if(cbCaps != sizeof(AUXCAPSA)) dprintf("%s: mismatched cbCaps expected=%d\n", ApiRef, sizeof(AUXCAPSA));
#endif
    if((dxw.dwFlags8 & VIRTUALCDAUDIO) && (uDeviceID == dxw.VirtualCDAudioDeviceId)) {
        lpCaps->wMid = 2 /*MM_CREATIVE*/;
        lpCaps->wPid = 401 /*MM_CREATIVE_AUX_CD*/;
        lpCaps->vDriverVersion = 1;
        strcpy(lpCaps->szPname, "dxwnd virtual CD");
        lpCaps->wTechnology = AUXCAPS_CDAUDIO;
        lpCaps->dwSupport = AUXCAPS_VOLUME | AUXCAPS_LRVOLUME;
        lpCaps->wReserved1 = 0;
        ret = MMSYSERR_NOERROR;
    } else
        ret = (*pauxGetDevCapsA)(uDeviceID, lpCaps, cbCaps);
#if 0
    if((ret == 2) && (uDeviceID == 0)) {
        // pretend a mixer device exists ....
        lpCaps->wMid = 2 /*MM_CREATIVE*/;
        lpCaps->wPid = 409 /*MM_CREATIVE_SB16_MIXER*/;
        lpCaps->vDriverVersion = 1;
        strcpy(lpCaps->szPname, "dxwnd virtual mixer");
        lpCaps->wTechnology = AUXCAPS_CDAUDIO;
        lpCaps->dwSupport = AUXCAPS_VOLUME | AUXCAPS_LRVOLUME;
        lpCaps->wReserved1 = 0;
        ret = MMSYSERR_NOERROR;
    }
#endif
    if(ret) {
        dprintf("%s: ERROR ret=%d\n", ApiRef, ret);
        return ret;
    }
    dprintf("> Mid = %#x\n", lpCaps->wMid);
    dprintf("> Pid = %#x\n", lpCaps->wPid);
    dprintf("> DriverVersion = %#x\n", lpCaps->vDriverVersion);
    dprintf("> Pname = %s\n", lpCaps->szPname);
    dprintf("> Technology = %#x\n", lpCaps->wTechnology);
    dprintf("> Reserved1 = %#x\n", lpCaps->wReserved1);
    dprintf("> Support = %#x\n", lpCaps->dwSupport);
    return ret;
}

MMRESULT WINAPI extauxGetVolume(UINT uDeviceID, LPDWORD lpdwVolume) {
    MMRESULT res;
    ApiName("auxGetVolume");
    OutTraceSND("%s: uDeviceId=%08X\n", ApiRef, uDeviceID);
    if((dxw.dwFlags8 & VIRTUALCDAUDIO) && (uDeviceID == dxw.VirtualCDAudioDeviceId)) {
        // "Outlaws" calls auxGetVolume before any mciSendCommand call,
        // so you have to initialize also here.
        if(!pplr_pump) player_init();
        int volume;
        volume = (*pplr_getvolume)();
        dprintf("> volume=%d%%\n", volume);
        int lvol = (volume * 0xFFFF) / 100;
        *lpdwVolume = (lvol & 0xFFFF) | (lvol & 0xFFFF) << 16;
        dprintf("> volume=%08X\n", *lpdwVolume);
        return MMSYSERR_NOERROR;
    }
    res = (*pauxGetVolume)(uDeviceID, lpdwVolume);
    OutDebugSND("%s: res=%#x\n", ApiRef, res);
    return res;
}

MMRESULT WINAPI extauxSetVolume(UINT uDeviceID, DWORD dwVolume) {
    MMRESULT res;
    ApiName("auxSetVolume");
    OutTraceSND("%s: uDeviceId=%08X dwVolume=%08X\n", ApiRef, uDeviceID, dwVolume);
    if((dxw.dwFlags8 & VIRTUALCDAUDIO) && (uDeviceID == dxw.VirtualCDAudioDeviceId)) {
        // "Outlaws" calls auxSetVolume before any mciSendCommand call,
        // so you have to initialize also here.
        if(!pplr_pump) player_init();
        unsigned short left = LOWORD(dwVolume);
        unsigned short right = HIWORD(dwVolume);
        dprintf("> left : %ud (%04X)\n", left, left);
        dprintf("> right: %ud (%04X)\n", right, right);
        if(dxw.dwFlags9 & LOCKVOLUME) {
            dprintf("> volume LOCKED\n");
            return MMSYSERR_NOERROR;
        }
        int volume = ((unsigned int)left * 100) / 0xFFFF;
        if(dxw.dwFlags10 & SETCDVOLUME) volume = dxw.FixedVolume;
        (*pplr_setvolume)(volume);
        dprintf("> Volume=%d%%\n", volume);
        return MMSYSERR_NOERROR;
    }
    res = (*pauxSetVolume)(uDeviceID, dwVolume);
    OutDebugSND("%s: res=%#x\n", ApiRef, res);
    return res;
}

MMRESULT WINAPI extwaveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume) {
    MMRESULT res;
    OutTraceSND("waveOutSetVolume: hwo=%#x vol=%#x\n", hwo, dwVolume);
    if(dxw.dwFlags9 & LOCKVOLUME) {
        dprintf("> volume LOCKED\n");
        return MMSYSERR_NOERROR;
    }
    res = (*pwaveOutSetVolume)(hwo, dwVolume);
    if(res != MMSYSERR_NOERROR)
        OutTraceE("waveOutSetVolume ERROR: res=%d\n", res);
    return res;
}

MMRESULT WINAPI extwaveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume) {
    MMRESULT res;
    OutTraceSND("waveOutGetVolume: hwo=%#x\n", hwo);
    res = (*pwaveOutGetVolume)(hwo, pdwVolume);
    if(res != MMSYSERR_NOERROR)
        OutTraceE("waveOutGetVolume ERROR: res=%d\n", res);
    else
        OutTraceSND("waveOutGetVolume: vol=%#x\n", *pdwVolume);
    return res;
}

// -- SAFEMIDIOUT ---

MMRESULT WINAPI extmidiOutReset(HMIDIOUT hmo) {
    MMRESULT res;
    ApiName("midiOutReset");
    OutTraceSYS("%s: hmo=%#x\n", ApiRef, hmo);
    if(dxw.dwFlags11 & SAFEMIDIOUT) return MMSYSERR_NOERROR;
    res = (*pmidiOutReset)(hmo);
#ifndef DXW_NOTRACES
    if(res != MMSYSERR_NOERROR) OutTraceE("%s: ERROR ret=%d err=%d\n", ApiRef, res, GetLastError());
#endif
    return res;
}

MMRESULT WINAPI extmidiOutSetVolume(HMIDIOUT hmo, DWORD dwVolume) {
    MMRESULT res;
    ApiName("midiOutSetVolume");
    OutTraceSYS("%s: hmo=%#x vol=%04.4x~%04.4x\n", ApiRef, hmo, (dwVolume >> 8) & 0xFFFF, dwVolume & 0xFFFF);
    if(dxw.dwFlags11 & SAFEMIDIOUT) return MMSYSERR_NOERROR;
    res = (*pmidiOutSetVolume)(hmo, dwVolume);
#ifndef DXW_NOTRACES
    if(res != MMSYSERR_NOERROR) OutTraceE("%s: ERROR ret=%d err=%d\n", ApiRef, res, GetLastError());
#endif
    return res;
}

extern LPCSTR dxwTranslatePathA(LPCSTR, DWORD *);
extern LPCWSTR dxwTranslatePathW(LPCWSTR, DWORD *);

HMMIO WINAPI extmmioOpenA(LPCSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen) {
    HMMIO res;
    ApiName("mmioOpenA");
    OutTraceSYS("%s: filename=\"%s\" ioinfo=%#x open=%#x\n", ApiRef, pszFileName, pmmioinfo, fdwOpen);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        // BEWARE: NULL pointer and path containing a '+' character are special mmioOpen cases!
        if(pszFileName && !strchr(pszFileName, '+'))
            pszFileName = dxwTranslatePathA(pszFileName, NULL);
    }
    res = (*pmmioOpenA)(pszFileName, pmmioinfo, fdwOpen);
    if(!res) {
        if(pmmioinfo)
            OutTraceE("%s: ERROR wErrorRet=%#x err=%d\n", ApiRef, pmmioinfo->wErrorRet, GetLastError());
        else
            OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    }
    return res;
}

HMMIO WINAPI extmmioOpenW(LPCWSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen) {
    HMMIO res;
    ApiName("mmioOpenW");
    OutTraceSYS("%s: filename=\"%ls\" ioinfo=%#x open=%#x\n", ApiRef, pszFileName, pmmioinfo, fdwOpen);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        // BEWARE: NULL pointer and path containing a '+' character are special mmioOpen cases!
        if(pszFileName && !wcschr(pszFileName, L'+'))
            pszFileName = dxwTranslatePathW(pszFileName, NULL);
    }
    res = (*pmmioOpenW)(pszFileName, pmmioinfo, fdwOpen);
    if(!res) {
        if(pmmioinfo)
            OutTraceE("%s: ERROR wErrorRet=%#x err=%d\n", ApiRef, pmmioinfo->wErrorRet, GetLastError());
        else
            OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    }
    return res;
}

BOOL WINAPI extsndPlaySoundA(LPCSTR lpszSound, UINT fuSound) {
    BOOL ret;
    ApiName("sndPlaySoundA");
    OutTraceSYS("%s: path=\"%s\" sound=%#x\n", ApiRef, lpszSound, fuSound);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        if(lpszSound) lpszSound = dxwTranslatePathA(lpszSound, NULL);
    }
    ret = (*psndPlaySoundA)(lpszSound, fuSound);
    _if(ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extsndPlaySoundW(LPCWSTR lpszSound, UINT fuSound) {
    BOOL ret;
    ApiName("sndPlaySoundW");
    OutTraceSYS("%s: path=\"%ls\" sound=%#x\n", ApiRef, lpszSound, fuSound);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        if(lpszSound) lpszSound = dxwTranslatePathW(lpszSound, NULL);
    }
    ret = (*psndPlaySoundW)(lpszSound, fuSound);
    _if(ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extPlaySoundA(LPCSTR lpszSound, HMODULE hmod, UINT fuSound) {
    BOOL ret;
    ApiName("PlaySoundA");
    OutTraceSYS("%s: path=\"%s\" hmod=%#x sound=%#x\n", ApiRef, lpszSound, hmod, fuSound);
    if((dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) && (fuSound & SND_FILENAME)) {
        if(lpszSound) lpszSound = dxwTranslatePathA(lpszSound, NULL);
    }
    ret = (*pPlaySoundA)(lpszSound, hmod, fuSound);
    _if(ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extPlaySoundW(LPCWSTR lpszSound, HMODULE hmod, UINT fuSound) {
    BOOL ret;
    ApiName("PlaySoundW");
    OutTraceSYS("%s: path=\"%ls\" hmod=%#x sound=%#x\n", ApiRef, lpszSound, hmod, fuSound);
    if((dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) && (fuSound & SND_FILENAME)) {
        if(lpszSound) lpszSound = dxwTranslatePathW(lpszSound, NULL);
    }
    ret = (*pPlaySoundW)(lpszSound, hmod, fuSound);
    _if(ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}
