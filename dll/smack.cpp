#define  _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhook.h"

#include "smack.h"

//#define SMACK_TRACEALL
#ifdef TRACEALL
#define SMACK_TRACEALL
#endif

//#undef OutTraceDW
//#define OutTraceDW OutTrace

typedef Smack *(WINAPI *SmackOpen_Type)(const char *, UINT32, INT32);
typedef Smack *(WINAPI *Smacker_Type)(Smack *);
typedef Smack *(WINAPI *Smacker2_Type)(Smack *, UINT32);
typedef Smack *(WINAPI *SmackColorRemap_Type)(Smack *, const void *, u32, u32);
typedef Smack *(WINAPI *SmackColorRemapWithTrans_Type)(Smack *, const void *, u32, u32, u32);
typedef Smack *(WINAPI *SmackBufferOpen_Type)(HWND, u32, u32, u32, u32, u32);
typedef u32		(WINAPI *SmackSetSystemRes_Type)(u32);
typedef u32		(WINAPI *SmackToBufferRect_Type)(Smack *, u32);
typedef void	(WINAPI *SmackSoundCheck_Type)(void);
typedef void	(WINAPI *SmackToBuffer_Type)(Smack *, u32, u32, u32, u32, const void *, u32);
typedef void	(WINAPI *SmackClose_Type)(Smack *);
typedef u32		(WINAPI *SmackBufferBlit_Type)(SmackBuf *, HDC, s32, s32, s32, s32, s32, s32);
typedef u32		(WINAPI *SmackBufferFocused_Type)(SmackBuf *);
typedef void	(WINAPI *SmackToScreen6_Type)(Smack *, u32, u32, u32, u32, u32);
typedef void	(WINAPI *SmackToScreen7_Type)(Smack *, u32, u32, u32, const u16 *, void *, u32);
typedef void    (WINAPI *SmackBlit_Type)(HSMACKBLIT, void *, u32, u32, u32, void *, u32, u32, u32, u32, u32);
typedef void	(WINAPI *SmackBlitSetPalette_Type)(HSMACKBLIT, void *, u32);
typedef u32		(WINAPI *SmackBlitSetFlags_Type)(HSMACKBLIT, u32);
typedef HSMACKBLIT (WINAPI *SmackBlitOpen_Type)(u32);

typedef Smack *(WINAPIV *SmackerC_Type)(Smack *);
typedef Smack *(WINAPIV *SmackOpenC_Type)(const char *, UINT32, INT32);
typedef void	(WINAPIV *SmackCloseC_Type)(Smack *);
typedef u32		(WINAPIV *SmackToBufferRectC_Type)(Smack *, u32);
typedef void	(WINAPIV *SmackToBufferC_Type)(Smack *, u32, u32, u32, u32, const void *, u32);
typedef Smack *(WINAPIV *SmackColorRemapC_Type)(Smack *, const void *, u32, u32);

SmackOpen_Type pSmackOpen;
SmackClose_Type pSmackClose, pSmackBufferClose;
Smacker_Type pSmackWait, pSmackDoFrame, pSmackNextFrame, pSmackSoundUseMSS, pSmackSoundUseDirectSound;
Smacker2_Type pSmackSoundOnOff, pSmackGoto;
SmackColorRemap_Type pSmackColorRemap;
SmackColorRemapWithTrans_Type pSmackColorRemapWithTrans;
SmackBufferOpen_Type pSmackBufferOpen;
SmackSetSystemRes_Type pSmackSetSystemRes;
SmackToBufferRect_Type pSmackToBufferRect;
SmackSoundCheck_Type pSmackSoundCheck;
SmackToBuffer_Type pSmackToBuffer;
SmackBufferBlit_Type pSmackBufferBlit;
SmackBufferFocused_Type pSmackBufferFocused;
SmackToScreen6_Type pSmackToScreen6;
SmackToScreen7_Type pSmackToScreen7;
SmackBlit_Type pSmackBlit;
SmackBlitSetPalette_Type pSmackBlitSetPalette;
SmackBlitSetFlags_Type pSmackBlitSetFlags;
SmackBlitOpen_Type pSmackBlitOpen;

SmackOpenC_Type pSmackOpenC;
SmackCloseC_Type pSmackCloseC;
SmackToBufferC_Type pSmackToBufferC;
SmackerC_Type pSmackDoFrameC;
SmackColorRemapC_Type pSmackColorRemapC;

Smack *WINAPI extSmackOpen(const char *, UINT32, INT32);
Smack *WINAPI extSmackWait(Smack *);
Smack *WINAPI extSmackDoFrame(Smack *);
Smack *WINAPI extSmackNextFrame(Smack *);
Smack *WINAPI extSmackSoundUseMSS(Smack *);
Smack *WINAPI extSmackSoundUseDirectSound(Smack *);
Smack *WINAPI extSmackSoundOnOff(Smack *, UINT32);
Smack *WINAPI extSmackGoto(Smack *, UINT32);
Smack *WINAPI extSmackColorRemap(Smack *, const void *, u32, u32);
Smack *WINAPI extSmackColorRemapWithTrans(Smack *, const void *, u32, u32, u32);
Smack *WINAPI extSmackBufferOpen(HWND, u32, u32, u32, u32, u32);
u32 WINAPI extSmackSetSystemRes(u32 mode);
u32	WINAPI extSmackToBufferRect(Smack *, u32);
void WINAPI extSmackSoundCheck(void);
void WINAPI extSmackToBuffer(Smack *, u32, u32, u32, u32, const void *, u32);
void WINAPI extSmackClose(Smack *);
void WINAPI extSmackBufferClose(Smack *);
u32  WINAPI extSmackBufferBlit(SmackBuf *, HDC, s32, s32, s32, s32, s32, s32);
u32 WINAPI extSmackBufferFocused(SmackBuf *);
void WINAPI extSmackToScreen6(Smack *, u32, u32, u32, u32, u32);
void WINAPI extSmackToScreen7(Smack *, u32, u32, u32, const u16 *, void *, u32);
u32 WINAPI extSmackBlitSetFlags(HSMACKBLIT, u32);
HSMACKBLIT WINAPI extSmackBlitOpen(u32);

Smack *WINAPIV extSmackOpenC(const char *, UINT32, INT32);
void WINAPIV extSmackCloseC(Smack *);
void WINAPIV extSmackToBufferC(Smack *, u32, u32, u32, u32, const void *, u32);
Smack *WINAPIV extSmackDoFrameC(Smack *);
Smack *WINAPIV extSmackColorRemapC(Smack *, const void *, u32, u32);
void WINAPI extSmackBlit(HSMACKBLIT, void *, u32, u32, u32, void *, u32, u32, u32, u32, u32);
void WINAPI extSmackBlitSetPalette(HSMACKBLIT, void *, u32);

// beware !!! depending on the smackw32 version, the ordinals are not always in the same place!

static HookEntryEx_Type *Hooks = NULL;

static HookEntryEx_Type Hooks22I[] = {
    {HOOK_IAT_CANDIDATE, 14, "_SmackOpen@12", (FARPROC)NULL, (FARPROC *) &pSmackOpen, (FARPROC)extSmackOpen},
    {HOOK_IAT_CANDIDATE, 23, "_SmackToBuffer@28", (FARPROC)NULL, (FARPROC *) &pSmackToBuffer, (FARPROC)extSmackToBuffer},
    {HOOK_IAT_CANDIDATE, 19, "_SmackDoFrame@4", (FARPROC)NULL, (FARPROC *) &pSmackDoFrame, (FARPROC)extSmackDoFrame},
    {HOOK_IAT_CANDIDATE, 25, "_SmackColorRemap@16", (FARPROC)NULL, (FARPROC *) &pSmackColorRemap, (FARPROC)extSmackColorRemap},
    {HOOK_IAT_CANDIDATE,  9, "_SmackSetSystemRes@4", (FARPROC)NULL, (FARPROC *) &pSmackSetSystemRes, (FARPROC)extSmackSetSystemRes},
    {HOOK_IAT_CANDIDATE, 18, "_SmackClose@4", (FARPROC)NULL, (FARPROC *) &pSmackClose, (FARPROC)extSmackClose},
#ifdef SMACK_TRACEALL
    {HOOK_IAT_CANDIDATE, 27, "_SmackGoto@8", (FARPROC)NULL, (FARPROC *) &pSmackGoto, (FARPROC)extSmackGoto},
    {HOOK_IAT_CANDIDATE, 21, "_SmackNextFrame@4", (FARPROC)NULL, (FARPROC *) &pSmackNextFrame, (FARPROC)extSmackNextFrame},
    {HOOK_IAT_CANDIDATE, 17, "_SmackSoundOnOff@8", (FARPROC)NULL, (FARPROC *) &pSmackSoundOnOff, (FARPROC)extSmackSoundOnOff},
    {HOOK_IAT_CANDIDATE, 33, "_SmackSoundUseMSS@4", (FARPROC)NULL, (FARPROC *) &pSmackSoundUseMSS, (FARPROC)extSmackSoundUseMSS},
    {HOOK_IAT_CANDIDATE,  2, "_SmackBufferOpen@24", (FARPROC)NULL, (FARPROC *) &pSmackBufferOpen, (FARPROC)extSmackBufferOpen},
    {HOOK_IAT_CANDIDATE,  6, "_SmackBufferClose@4", (FARPROC)NULL, (FARPROC *) &pSmackBufferClose, (FARPROC)extSmackBufferClose},
    {HOOK_IAT_CANDIDATE, 38, "_SmackSoundUseDirectSound@4", (FARPROC)NULL, (FARPROC *) &pSmackSoundUseDirectSound, (FARPROC)extSmackSoundUseDirectSound},
    {HOOK_IAT_CANDIDATE,  3, "_SmackBufferBlit@32", (FARPROC)NULL, (FARPROC *) &pSmackBufferBlit, (FARPROC)extSmackBufferBlit},
    {HOOK_IAT_CANDIDATE, 28, "_SmackToBufferRect@8", (FARPROC)NULL, (FARPROC *) &pSmackToBufferRect, (FARPROC)extSmackToBufferRect},
    {HOOK_IAT_CANDIDATE,  4, "_SmackBufferFocused@4", (FARPROC)NULL, (FARPROC *) &pSmackBufferFocused, (FARPROC)extSmackBufferFocused},
    {HOOK_IAT_CANDIDATE, 31, "_SmackSoundCheck@0", (FARPROC)NULL, (FARPROC *) &pSmackSoundCheck, (FARPROC)extSmackSoundCheck},
    {HOOK_IAT_CANDIDATE, 32, "_SmackWait@4", (FARPROC)NULL, (FARPROC *) &pSmackWait, (FARPROC)extSmackWait},
    {HOOK_IAT_CANDIDATE, 22, "_SmackToScreen@24", (FARPROC)NULL, (FARPROC *) &pSmackToScreen6, (FARPROC)extSmackToScreen6},
#endif
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type Hooks31N[] = {
    {HOOK_IAT_CANDIDATE, 39, "_SmackOpen@12", (FARPROC)NULL, (FARPROC *) &pSmackOpen, (FARPROC)extSmackOpen},
    {HOOK_IAT_CANDIDATE, 52, "_SmackToBuffer@28", (FARPROC)NULL, (FARPROC *) &pSmackToBuffer, (FARPROC)extSmackToBuffer},
    {HOOK_IAT_CANDIDATE, 33, "_SmackDoFrame@4", (FARPROC)NULL, (FARPROC *) &pSmackDoFrame, (FARPROC)extSmackDoFrame},
    {HOOK_IAT_CANDIDATE, 30, "_SmackColorRemap@16", (FARPROC)NULL, (FARPROC *) &pSmackColorRemap, (FARPROC)extSmackColorRemap},
    {HOOK_IAT_CANDIDATE, 41, "_SmackSetSystemRes@4", (FARPROC)NULL, (FARPROC *) &pSmackSetSystemRes, (FARPROC)extSmackSetSystemRes},
    {HOOK_IAT_CANDIDATE, 29, "_SmackClose@4", (FARPROC)NULL, (FARPROC *) &pSmackClose, (FARPROC)extSmackClose},
#ifdef SMACK_TRACEALL
    {HOOK_IAT_CANDIDATE, 36, "_SmackGoto@8", (FARPROC)NULL, (FARPROC *) &pSmackGoto, (FARPROC)extSmackGoto},
    {HOOK_IAT_CANDIDATE, 38, "_SmackNextFrame@4", (FARPROC)NULL, (FARPROC *) &pSmackNextFrame, (FARPROC)extSmackNextFrame},
    {HOOK_IAT_CANDIDATE, 45, "_SmackSoundOnOff@8", (FARPROC)NULL, (FARPROC *) &pSmackSoundOnOff, (FARPROC)extSmackSoundOnOff},
    {HOOK_IAT_CANDIDATE, 49, "_SmackSoundUseMSS@4", (FARPROC)NULL, (FARPROC *) &pSmackSoundUseMSS, (FARPROC)extSmackSoundUseMSS},
    {HOOK_IAT_CANDIDATE, 20, "_SmackBufferOpen@24", (FARPROC)NULL, (FARPROC *) &pSmackBufferOpen, (FARPROC)extSmackBufferOpen},
    {HOOK_IAT_CANDIDATE, 14, "_SmackBufferClose@4", (FARPROC)NULL, (FARPROC *) &pSmackBufferClose, (FARPROC)extSmackBufferClose},
    {HOOK_IAT_CANDIDATE, 48, "_SmackSoundUseDirectSound@4", (FARPROC)NULL, (FARPROC *) &pSmackSoundUseDirectSound, (FARPROC)extSmackSoundUseDirectSound},
    {HOOK_IAT_CANDIDATE, 12, "_SmackBufferBlit@32", (FARPROC)NULL, (FARPROC *) &pSmackBufferBlit, (FARPROC)extSmackBufferBlit},
    {HOOK_IAT_CANDIDATE, 53, "_SmackToBufferRect@8", (FARPROC)NULL, (FARPROC *) &pSmackToBufferRect, (FARPROC)extSmackToBufferRect},
    {HOOK_IAT_CANDIDATE, 17, "_SmackBufferFocused@4", (FARPROC)NULL, (FARPROC *) &pSmackBufferFocused, (FARPROC)extSmackBufferFocused},
    {HOOK_IAT_CANDIDATE, 43, "_SmackSoundCheck@0", (FARPROC)NULL, (FARPROC *) &pSmackSoundCheck, (FARPROC)extSmackSoundCheck},
    {HOOK_IAT_CANDIDATE, 57, "_SmackWait@4", (FARPROC)NULL, (FARPROC *) &pSmackWait, (FARPROC)extSmackWait},
    {HOOK_IAT_CANDIDATE, 54, "_SmackToScreen@28", (FARPROC)NULL, (FARPROC *) &pSmackToScreen7, (FARPROC)extSmackToScreen7},
#endif
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type Hooks32G[] = {
    {HOOK_IAT_CANDIDATE, 39, "_SmackOpen@12", (FARPROC)NULL, (FARPROC *) &pSmackOpen, (FARPROC)extSmackOpen},
    {HOOK_IAT_CANDIDATE, 52, "_SmackToBuffer@28", (FARPROC)NULL, (FARPROC *) &pSmackToBuffer, (FARPROC)extSmackToBuffer},
    {HOOK_IAT_CANDIDATE, 33, "_SmackDoFrame@4", (FARPROC)NULL, (FARPROC *) &pSmackDoFrame, (FARPROC)extSmackDoFrame},
    {HOOK_IAT_CANDIDATE, 30, "_SmackColorRemapWithTrans@20", (FARPROC)NULL, (FARPROC *) &pSmackColorRemapWithTrans, (FARPROC)extSmackColorRemapWithTrans},
    {HOOK_IAT_CANDIDATE, 41, "_SmackSetSystemRes@4", (FARPROC)NULL, (FARPROC *) &pSmackSetSystemRes, (FARPROC)extSmackSetSystemRes},
    {HOOK_IAT_CANDIDATE, 29, "_SmackClose@4", (FARPROC)NULL, (FARPROC *) &pSmackClose, (FARPROC)extSmackClose},
#ifdef SMACK_TRACEALL
    {HOOK_IAT_CANDIDATE, 36, "_SmackGoto@8", (FARPROC)NULL, (FARPROC *) &pSmackGoto, (FARPROC)extSmackGoto},
    {HOOK_IAT_CANDIDATE, 38, "_SmackNextFrame@4", (FARPROC)NULL, (FARPROC *) &pSmackNextFrame, (FARPROC)extSmackNextFrame},
    {HOOK_IAT_CANDIDATE, 45, "_SmackSoundOnOff@8", (FARPROC)NULL, (FARPROC *) &pSmackSoundOnOff, (FARPROC)extSmackSoundOnOff},
    {HOOK_IAT_CANDIDATE, 49, "_SmackSoundUseMSS@4", (FARPROC)NULL, (FARPROC *) &pSmackSoundUseMSS, (FARPROC)extSmackSoundUseMSS},
    {HOOK_IAT_CANDIDATE, 20, "_SmackBufferOpen@24", (FARPROC)NULL, (FARPROC *) &pSmackBufferOpen, (FARPROC)extSmackBufferOpen},
    {HOOK_IAT_CANDIDATE, 14, "_SmackBufferClose@4", (FARPROC)NULL, (FARPROC *) &pSmackBufferClose, (FARPROC)extSmackBufferClose},
    {HOOK_IAT_CANDIDATE, 48, "_SmackSoundUseDirectSound@4", (FARPROC)NULL, (FARPROC *) &pSmackSoundUseDirectSound, (FARPROC)extSmackSoundUseDirectSound},
    {HOOK_IAT_CANDIDATE, 12, "_SmackBufferBlit@32", (FARPROC)NULL, (FARPROC *) &pSmackBufferBlit, (FARPROC)extSmackBufferBlit},
    {HOOK_IAT_CANDIDATE, 53, "_SmackToBufferRect@8", (FARPROC)NULL, (FARPROC *) &pSmackToBufferRect, (FARPROC)extSmackToBufferRect},
    {HOOK_IAT_CANDIDATE, 17, "_SmackBufferFocused@4", (FARPROC)NULL, (FARPROC *) &pSmackBufferFocused, (FARPROC)extSmackBufferFocused},
    {HOOK_IAT_CANDIDATE, 43, "_SmackSoundCheck@0", (FARPROC)NULL, (FARPROC *) &pSmackSoundCheck, (FARPROC)extSmackSoundCheck},
    {HOOK_IAT_CANDIDATE, 57, "_SmackWait@4", (FARPROC)NULL, (FARPROC *) &pSmackWait, (FARPROC)extSmackWait},
    {HOOK_IAT_CANDIDATE, 54, "_SmackToScreen@28", (FARPROC)NULL, (FARPROC *) &pSmackToScreen7, (FARPROC)extSmackToScreen7},
#endif
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type Hooks40C[] = { // used by "Daria's Inferno"
    {HOOK_IAT_CANDIDATE, 40, "_SmackOpen@12", (FARPROC)NULL, (FARPROC *) &pSmackOpen, (FARPROC)extSmackOpen},
    {HOOK_IAT_CANDIDATE, 53, "_SmackToBuffer@28", (FARPROC)NULL, (FARPROC *) &pSmackToBuffer, (FARPROC)extSmackToBuffer},
    {HOOK_IAT_CANDIDATE, 33, "_SmackDoFrame@4", (FARPROC)NULL, (FARPROC *) &pSmackDoFrame, (FARPROC)extSmackDoFrame},
    {HOOK_IAT_CANDIDATE, 30, "_SmackColorRemapWithTrans@20", (FARPROC)NULL, (FARPROC *) &pSmackColorRemapWithTrans, (FARPROC)extSmackColorRemapWithTrans},
    {HOOK_IAT_CANDIDATE, 42, "_SmackSetSystemRes@4", (FARPROC)NULL, (FARPROC *) &pSmackSetSystemRes, (FARPROC)extSmackSetSystemRes},
    {HOOK_IAT_CANDIDATE, 29, "_SmackClose@4", (FARPROC)NULL, (FARPROC *) &pSmackClose, (FARPROC)extSmackClose},

#ifdef SMACK_TRACEALL
    {HOOK_IAT_CANDIDATE,  2, "_SmackBlit@44", (FARPROC)NULL, (FARPROC *) &pSmackBlit, (FARPROC)extSmackBlit},
    {HOOK_IAT_CANDIDATE,  9, "_SmackBlitSetPalette@12", (FARPROC)NULL, (FARPROC *) &pSmackBlitSetPalette, (FARPROC)extSmackBlitSetPalette},
    {HOOK_IAT_CANDIDATE,  8, "_SmackBlitSetFlags@8", (FARPROC)NULL, (FARPROC *) &pSmackBlitSetFlags, (FARPROC)extSmackBlitSetFlags},
    {HOOK_IAT_CANDIDATE,  7, "_SmackBlitOpen@4", (FARPROC)NULL, (FARPROC *) &pSmackBlitOpen, (FARPROC)extSmackBlitOpen},
    {HOOK_IAT_CANDIDATE, 37, "_SmackGoto@8", (FARPROC)NULL, (FARPROC *) &pSmackGoto, (FARPROC)extSmackGoto},
    {HOOK_IAT_CANDIDATE, 39, "_SmackNextFrame@4", (FARPROC)NULL, (FARPROC *) &pSmackNextFrame, (FARPROC)extSmackNextFrame},
    {HOOK_IAT_CANDIDATE, 46, "_SmackSoundOnOff@8", (FARPROC)NULL, (FARPROC *) &pSmackSoundOnOff, (FARPROC)extSmackSoundOnOff},
    {HOOK_IAT_CANDIDATE, 50, "_SmackSoundUseMSS@4", (FARPROC)NULL, (FARPROC *) &pSmackSoundUseMSS, (FARPROC)extSmackSoundUseMSS},
    {HOOK_IAT_CANDIDATE, 20, "_SmackBufferOpen@24", (FARPROC)NULL, (FARPROC *) &pSmackBufferOpen, (FARPROC)extSmackBufferOpen},
    {HOOK_IAT_CANDIDATE, 14, "_SmackBufferClose@4", (FARPROC)NULL, (FARPROC *) &pSmackBufferClose, (FARPROC)extSmackBufferClose},
    {HOOK_IAT_CANDIDATE, 49, "_SmackSoundUseDirectSound@4", (FARPROC)NULL, (FARPROC *) &pSmackSoundUseDirectSound, (FARPROC)extSmackSoundUseDirectSound},
    {HOOK_IAT_CANDIDATE, 12, "_SmackBufferBlit@32", (FARPROC)NULL, (FARPROC *) &pSmackBufferBlit, (FARPROC)extSmackBufferBlit},
    {HOOK_IAT_CANDIDATE, 54, "_SmackToBufferRect@8", (FARPROC)NULL, (FARPROC *) &pSmackToBufferRect, (FARPROC)extSmackToBufferRect},
    {HOOK_IAT_CANDIDATE,  0, "_SmackBufferFocused@4", (FARPROC)NULL, (FARPROC *) &pSmackBufferFocused, (FARPROC)extSmackBufferFocused},
    {HOOK_IAT_CANDIDATE, 44, "_SmackSoundCheck@0", (FARPROC)NULL, (FARPROC *) &pSmackSoundCheck, (FARPROC)extSmackSoundCheck},
    {HOOK_IAT_CANDIDATE, 58, "_SmackWait@4", (FARPROC)NULL, (FARPROC *) &pSmackWait, (FARPROC)extSmackWait},
    {HOOK_IAT_CANDIDATE, 55, "_SmackToScreen@28", (FARPROC)NULL, (FARPROC *) &pSmackToScreen6, (FARPROC)extSmackToScreen6},
#endif
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type Hooks40E[] = {
    {HOOK_IAT_CANDIDATE, 42, "_SmackOpen@12", (FARPROC)NULL, (FARPROC *) &pSmackOpen, (FARPROC)extSmackOpen},
    {HOOK_IAT_CANDIDATE, 55, "_SmackToBuffer@28", (FARPROC)NULL, (FARPROC *) &pSmackToBuffer, (FARPROC)extSmackToBuffer},
    {HOOK_IAT_CANDIDATE, 35, "_SmackDoFrame@4", (FARPROC)NULL, (FARPROC *) &pSmackDoFrame, (FARPROC)extSmackDoFrame},
    {HOOK_IAT_CANDIDATE, 32, "_SmackColorRemapWithTrans@20", (FARPROC)NULL, (FARPROC *) &pSmackColorRemapWithTrans, (FARPROC)extSmackColorRemapWithTrans},
    {HOOK_IAT_CANDIDATE, 44, "_SmackSetSystemRes@4", (FARPROC)NULL, (FARPROC *) &pSmackSetSystemRes, (FARPROC)extSmackSetSystemRes},
    {HOOK_IAT_CANDIDATE, 31, "_SmackClose@4", (FARPROC)NULL, (FARPROC *) &pSmackClose, (FARPROC)extSmackClose},

#ifdef SMACK_TRACEALL
    {HOOK_IAT_CANDIDATE,  4, "_SmackBlit@44", (FARPROC)NULL, (FARPROC *) &pSmackBlit, (FARPROC)extSmackBlit},
    {HOOK_IAT_CANDIDATE, 11, "_SmackBlitSetPalette@12", (FARPROC)NULL, (FARPROC *) &pSmackBlitSetPalette, (FARPROC)extSmackBlitSetPalette},
    {HOOK_IAT_CANDIDATE, 10, "_SmackBlitSetFlags@8", (FARPROC)NULL, (FARPROC *) &pSmackBlitSetFlags, (FARPROC)extSmackBlitSetFlags},
    {HOOK_IAT_CANDIDATE,  9, "_SmackBlitOpen@4", (FARPROC)NULL, (FARPROC *) &pSmackBlitOpen, (FARPROC)extSmackBlitOpen},
    {HOOK_IAT_CANDIDATE, 39, "_SmackGoto@8", (FARPROC)NULL, (FARPROC *) &pSmackGoto, (FARPROC)extSmackGoto},
    {HOOK_IAT_CANDIDATE, 41, "_SmackNextFrame@4", (FARPROC)NULL, (FARPROC *) &pSmackNextFrame, (FARPROC)extSmackNextFrame},
    {HOOK_IAT_CANDIDATE, 48, "_SmackSoundOnOff@8", (FARPROC)NULL, (FARPROC *) &pSmackSoundOnOff, (FARPROC)extSmackSoundOnOff},
    {HOOK_IAT_CANDIDATE, 52, "_SmackSoundUseMSS@4", (FARPROC)NULL, (FARPROC *) &pSmackSoundUseMSS, (FARPROC)extSmackSoundUseMSS},
    {HOOK_IAT_CANDIDATE, 22, "_SmackBufferOpen@24", (FARPROC)NULL, (FARPROC *) &pSmackBufferOpen, (FARPROC)extSmackBufferOpen},
    {HOOK_IAT_CANDIDATE, 16, "_SmackBufferClose@4", (FARPROC)NULL, (FARPROC *) &pSmackBufferClose, (FARPROC)extSmackBufferClose},
    {HOOK_IAT_CANDIDATE, 51, "_SmackSoundUseDirectSound@4", (FARPROC)NULL, (FARPROC *) &pSmackSoundUseDirectSound, (FARPROC)extSmackSoundUseDirectSound},
    {HOOK_IAT_CANDIDATE, 14, "_SmackBufferBlit@32", (FARPROC)NULL, (FARPROC *) &pSmackBufferBlit, (FARPROC)extSmackBufferBlit},
    {HOOK_IAT_CANDIDATE, 56, "_SmackToBufferRect@8", (FARPROC)NULL, (FARPROC *) &pSmackToBufferRect, (FARPROC)extSmackToBufferRect},
    {HOOK_IAT_CANDIDATE, 19, "_SmackBufferFocused@4", (FARPROC)NULL, (FARPROC *) &pSmackBufferFocused, (FARPROC)extSmackBufferFocused},
    {HOOK_IAT_CANDIDATE, 46, "_SmackSoundCheck@0", (FARPROC)NULL, (FARPROC *) &pSmackSoundCheck, (FARPROC)extSmackSoundCheck},
    {HOOK_IAT_CANDIDATE, 60, "_SmackWait@4", (FARPROC)NULL, (FARPROC *) &pSmackWait, (FARPROC)extSmackWait},
    {HOOK_IAT_CANDIDATE, 57, "_SmackToScreen@28", (FARPROC)NULL, (FARPROC *) &pSmackToScreen7, (FARPROC)extSmackToScreen7},
#endif
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type Hooks21C[] = {
    // t.b.d. needs better prototypes
    {HOOK_IAT_CANDIDATE, 0x16, "_SmackOpen", (FARPROC)NULL, (FARPROC *) &pSmackOpenC, (FARPROC)extSmackOpenC},
    {HOOK_IAT_CANDIDATE, 0x1C, "_SmackToBuffer", (FARPROC)NULL, (FARPROC *) &pSmackToBufferC, (FARPROC)extSmackToBufferC},
    {HOOK_IAT_CANDIDATE, 0x11, "_SmackDoFrame", (FARPROC)NULL, (FARPROC *) &pSmackDoFrameC, (FARPROC)extSmackDoFrameC},
    {HOOK_IAT_CANDIDATE, 0x0F, "_SmackColorRemap", (FARPROC)NULL, (FARPROC *) &pSmackColorRemapC, (FARPROC)extSmackColorRemapC},
    {HOOK_IAT_CANDIDATE, 0x0E, "_SmackClose", (FARPROC)NULL, (FARPROC *) &pSmackCloseC, (FARPROC)extSmackCloseC},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type HooksSmkWai[] = {
    // t.b.d. needs better prototypes
    {HOOK_IAT_CANDIDATE, 0x16, "_SmackOpen", (FARPROC)NULL, (FARPROC *) &pSmackOpenC, (FARPROC)extSmackOpenC},
    {HOOK_IAT_CANDIDATE, 0x1C, "_SmackToBuffer", (FARPROC)NULL, (FARPROC *) &pSmackToBufferC, (FARPROC)extSmackToBufferC},
    {HOOK_IAT_CANDIDATE, 0x11, "_SmackDoFrame", (FARPROC)NULL, (FARPROC *) &pSmackDoFrameC, (FARPROC)extSmackDoFrameC},
    {HOOK_IAT_CANDIDATE, 0x0F, "_SmackColorRemap", (FARPROC)NULL, (FARPROC *) &pSmackColorRemapC, (FARPROC)extSmackColorRemapC},
    {HOOK_IAT_CANDIDATE, 0x0E, "_SmackClose", (FARPROC)NULL, (FARPROC *) &pSmackCloseC, (FARPROC)extSmackCloseC},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static void InitializeSMKInterface() {
    // select the hook table by looking at the existence of some given key function that
    // does not exists in previous interface versions
    HMODULE h;
    FARPROC fun;
    // can't use the module handle because it doesn't work for main process handle, which
    // is the most useful thing. Need opening (and closing) a new module handle, hoping
    // to load the very same dll ...
    h = (*pLoadLibraryA)("smackw32.dll");
    if(!h) return;
    while(TRUE) { // fake loop
        fun = (*pGetProcAddress)(h, "_RADTimerRead@0");
        if(fun) {
            OutTraceDW("Smack interface 4.0e\n");
            Hooks = Hooks40E;
            break;
        }
        fun = (*pGetProcAddress)(h, "_SmackGetKeyFrame@12");
        if(fun) {
            OutTraceDW("Smack interface 4.0c\n");
            Hooks = Hooks40C;
            break;
        }
        fun = (*pGetProcAddress)(h, "_SmackColorRemapWithTrans@20");
        if(fun) {
            OutTraceDW("Smack interface 3.2g\n");
            Hooks = Hooks32G;
            break;
        }
        fun = (*pGetProcAddress)(h, "_SmackBufferToBufferMask@40");
        if(fun) {
            OutTraceDW("Smack interface 3.1n\n");
            Hooks = Hooks31N;
            break;
        }
        fun = (*pGetProcAddress)(h, "_SmackOpen@12");
        if(fun) {
            OutTraceDW("Smack interface 2.2i\n");
            Hooks = Hooks22I;
            break;
        }
        // found in Hellbender but not identified. Presumably (according to radgametools changelog) 2.0w
        // BEWARE: the calls here are _cdecl instead of _stdcall.
        // Note: the two definitions 1.0x and 1.1x are equivalent up to when _SmackSoundUse* calls won't be hooked
        fun = (*pGetProcAddress)(h, "_SmackSoundUseDirectSound");
        if(fun) {
            OutTraceDW("Smack interface 2.0w\n");
            Hooks = Hooks21C;
            break;
        }
        // found in S.C.A.R.A.B. but not identified. (according to radgametools changelog) 2.0r
        // BEWARE: the calls here are _cdecl instead of _stdcall.
        fun = (*pGetProcAddress)(h, "_SmackOpen");
        if(fun) {
            OutTraceDW("Smack interface 2.0r\n");
            Hooks = Hooks21C;
            break;
        }
        OutTraceE("Smack interface unknown\n");
        break; // exit fake loop
    }
    FreeLibrary(h);
}

FARPROC Remap_smack_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if(!(dxw.dwFlags7 & HOOKSMACKW32)) return NULL;
    if(!Hooks) InitializeSMKInterface();
    if(!Hooks) return NULL;
    if (addr = RemapLibraryEx(proc, hModule, Hooks)) return addr;
    // NULL -> keep the original call address
    return NULL;
}

FARPROC Remap_smkwai_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if(!(dxw.dwFlags7 & HOOKSMACKW32)) return NULL;
    // no version research here
    if (addr = RemapLibraryEx(proc, hModule, Hooks21C)) return addr;
    // NULL -> keep the original call address
    return NULL;
}

static char *libname = "smackw32.dll";

void HookSmackW32(HMODULE hModule) {
    if(!(dxw.dwFlags7 & HOOKSMACKW32)) return;
    if(!Hooks) InitializeSMKInterface();
    if(!Hooks) return;
    HookLibraryEx(hModule, Hooks, libname);
    return;
}

void HookSmkWai32(HMODULE hModule) {
    if(!(dxw.dwFlags7 & HOOKSMACKW32)) return;
    HookLibraryEx(hModule, Hooks21C, "smkwai32.dll");
    return;
}

#ifdef DXW_NOTRACES

#define DumpSmack(s)

#else // DXW_NOTRACES
static char *ExplainSmackFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "SMACK");
    if (c & SMACKNEEDPAN) strcat(eb, "NEEDPAN+");
    if (c & SMACKNEEDVOLUME) strcat(eb, "NEEDVOLUME+");
    if (c & SMACKFRAMERATE) strcat(eb, "FRAMERATE+");
    if (c & SMACKLOADEXTRA) strcat(eb, "LOADEXTRA+");
    if (c & SMACKPRELOADALL) strcat(eb, "PRELOADALL+");
    if (c & SMACKNOSKIP) strcat(eb, "NOSKIP+");
    if (c & SMACKSIMULATE) strcat(eb, "SIMULATE+");
    if (c & SMACKTRACK1) strcat(eb, "TRACK1+");
    if (c & SMACKTRACK2) strcat(eb, "TRACK2+");
    if (c & SMACKTRACK3) strcat(eb, "TRACK3+");
    if (c & SMACKTRACK4) strcat(eb, "TRACK4+");
    if (c & SMACKTRACK5) strcat(eb, "TRACK5+");
    if (c & SMACKTRACK6) strcat(eb, "TRACK6+");
    if (c & SMACKTRACK7) strcat(eb, "TRACK7+");
    if (c & SMACKBUFFERREVERSED) strcat(eb, "BUFFERREVERSED+");
    if (c & SMACKFILEISSMK) strcat(eb, "FILEISSMK+");
    if ((c & SMACKBUFFER16) == SMACKBUFFER555) strcat(eb, "BUFFER555+");
    if ((c & SMACKBUFFER16) == SMACKBUFFER565) strcat(eb, "BUFFER565+");
    if ((c & SMACKYNONE) == SMACKYNONE) strcat(eb, "YNONE+");
    if ((c & SMACKYNONE) == SMACKYDOUBLE) strcat(eb, "YDOUBLE+");
    if ((c & SMACKYNONE) == SMACKYINTERLACE) strcat(eb, "YINTERLACE+");
    l = strlen(eb);
    if (l > strlen("SMACK")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

static void DumpSmack(Smack *s) {
    if(!IsDebugDW) return;
    if(!s) return;
    OutTrace("Smack_%#x: version=\"%04.4s\" size=(%dx%d) frames=%d rate=%d type=%#x palette=%#x extra=%#x rect=(%d,%d)(%d-%d) offset=(%d,%d) err=%#x\n",
             s, s, s->Width, s->Height, s->Frames, s->MSPerFrame, s->SmackerType, s->NewPalette, s->extra,
             s->LastRectx, s->LastRecty, s->LastRectw, s->LastRecth, s->LeftOfs, s->TopOfs, s->ReadError);
    // unreliable:
    // s->FrameNum,
    HexTrace((LPBYTE)s, sizeof(Smack));
}
#endif

// === call proxies ===

// BEWARE: it seems that SmackOpen (since a given release) has the possibility to open either a file path
// or a resource id (file handle?). Seen in "Jagged Alliance 2" (possibly without the play CD inserted)
// the game shows a simple splash screen.

Smack *WINAPI extSmackOpen(const char *SmackFile, UINT32 flags, INT32 extrabuf) {
    Smack *h;
#ifndef DXW_NOTRACES
    if(IsTraceDW) {
        if((DWORD)SmackFile & 0xFFFF0000) {
            OutTraceDW("SmackOpen: SmackFile=\"%s\" flags=%#x(%s) extrabuf=%#x\n",
                       SmackFile, flags, ExplainSmackFlags(flags), extrabuf);
        } else {
            OutTraceDW("SmackOpen: SmackFile=0x%04.4X flags=%#x(%s) extrabuf=%#x\n",
                       SmackFile, flags, ExplainSmackFlags(flags), extrabuf);
        }
    }
    if(!pSmackOpen) OutTraceE("ASSERT: NULL pSmackOpen\n");
#endif // DXW_NOTRACES
    h = (*pSmackOpen)(SmackFile, flags, extrabuf);
    OutTraceDW("SmackOpen: ret=%#x\n", h);
    if (!h) return NULL;
    DumpSmack(h);
    if(dxw.dwFlags6 & NOMOVIES) {
        OutTraceDW("SmackOpen: NOMOVIES\n");
        h->Frames = 1; // returning NULL or a frame counter below 1 is risky!
        return h;
    }
    if(dxw.dwFlags10 & SUSPENDTIMESTRETCH) dxw.SuspendTimeStretch(TIMESHIFT_OFF);
    return h;
}

void WINAPI extSmackClose(Smack *h) {
    OutTraceDW("SmackClose: h=%#x\n", h);
    (*pSmackClose)(h);
    if(dxw.dwFlags10 & SUSPENDTIMESTRETCH) dxw.SuspendTimeStretch(TIMESHIFT_ON);
}

#ifdef SMACK_TRACEALL
Smack *WINAPI extSmackWait(Smack *h) {
    OutDebugDW("SmackWait: h=%#x\n", h);
    DumpSmack((Smack *)h);
    return (*pSmackWait)(h);
}
#endif

Smack *WINAPI extSmackDoFrame(Smack *h) {
    Smack *ret;
    OutTraceDW("SmackDoFrame: h=%#x\n", h);
    DumpSmack((Smack *)h);
    if(dxw.dwFlags6 & NOMOVIES) {
        OutTraceDW("SmackDoFrame: NOMOVIES\n");
        h->Frames = 1; // returning NULL or a frame counter below 1 is risky!
    }
    ret = (*pSmackDoFrame)(h);
    if(dxw.dwFlags7 & FIXSMACKLOOP) {
        dxw.MessagePump();
        dxw.ScreenRefresh();
    }
    return ret;
}

#ifdef SMACK_TRACEALL
Smack *WINAPI extSmackNextFrame(Smack *h) {
    OutTraceDW("SmackNextFrame: h=%#x\n", h);
    DumpSmack(h);
    return (*pSmackNextFrame)(h);
}
#endif

#ifdef SMACK_TRACEALL
Smack *WINAPI extSmackSoundUseMSS(Smack *h) {
    OutTraceDW("SmackSoundUseMSS: h=%#x\n", h);
    DumpSmack(h);
    return (*pSmackSoundUseMSS)(h);
}
#endif

#ifdef SMACK_TRACEALL
Smack *WINAPI extSmackSoundUseDirectSound(Smack *h) {
    OutTraceDW("SmackSoundUseDirectSound: h=%#x\n", h);
    DumpSmack(h);
    return (*pSmackSoundUseDirectSound)(h);
}
#endif

#ifdef SMACK_TRACEALL
Smack *WINAPI extSmackSoundOnOff(Smack *h, UINT32 flag) {
    OutTraceDW("SmackSoundOnOff: h=%#x flag=%#x\n", h, flag);
    DumpSmack(h);
    return (*pSmackSoundOnOff)(h, flag);
}
#endif

#ifdef SMACK_TRACEALL
Smack *WINAPI extSmackGoto(Smack *h, UINT32 flag) {
    OutTraceDW("SmackGoto: h=%#x flag=%#x\n", h, flag);
    DumpSmack(h);
    return (*pSmackGoto)(h, flag);
}
#endif

Smack *WINAPI extSmackColorRemap(Smack *h, const void *remappal, u32 numcolors, u32 paltype) {
    OutTraceDW("SmackColorRemap: h=%#x numcolors=%d paltype=%d\n", h, numcolors, paltype);
    DumpSmack(h);
    // BYPASS the call to avoid resolution changes
    //return (*pSmackColorRemap)(h, remappal, numcolors, paltype);
    return h;
}

Smack *WINAPI extSmackColorRemapWithTrans(Smack *h, const void *remappal, u32 numcolors, u32 paltype, u32 transindex) {
    OutTraceDW("SmackColorRemapWithTrans: h=%#x numcolors=%d paltype=%d transindex=%d\n", h, numcolors, paltype, transindex);
    DumpSmack(h);
    // BYPASS the call to avoid resolution changes
    //return (*pSmackColorRemapWithTrans)(h, remappal, numcolors, paltype);
    return h;
}

#ifdef SMACK_TRACEALL
Smack *WINAPI extSmackBufferOpen(HWND wnd, u32 BlitType, u32 width, u32 height, u32 ZoomW, u32 ZoomH) {
    Smack *h;
    OutTraceDW("SmackBufferOpen: hwnd=%#x BlitType=%#x size=(%dx%d) zoom=(%dx%d)\n",
               wnd, BlitType, width, height, ZoomW, ZoomH);
    // v2.04.54: commented out: using the DXWHOOK_EXTEND flag the hook is extended to ddraw methods below and there
    // is no longer need to compensate for the missing scaling. In addition, all ddraw injected features are still ok.
    //// if Smackw32 is hooked, must do the scaling ...
    //if(dxw.Windowize){
    //	dxw.MapClient(&width, &height);
    //	dxw.MapClient(&ZoomW, &ZoomH);
    //}
    h = (*pSmackBufferOpen)(wnd, BlitType, width, height, ZoomW, ZoomH);
    DumpSmack(h);
    return h;
}
#endif

/*  ---------------------------------------------------------------
    #define SMACKRESRESET    0
    #define SMACKRES640X400  1
    #define SMACKRES640X480  2
    #define SMACKRES800X600  3
    #define SMACKRES1024X768 4

    RADEXPFUNC u32 RADEXPLINK SmackSetSystemRes(u32 mode);  // use SMACKRES* values

    #define SMACKNOCUSTOMBLIT        128
    #define SMACKSMOOTHBLIT          256
    #define SMACKINTERLACEBLIT 512
    /* ------------------------------------------------------------ */

u32 WINAPI extSmackSetSystemRes(u32 mode) {
    char *modes[5] = {"reset", "640X400", "640X480", "800X600", "1024X768"};
    int width, height;
    static int prevwidth, prevheight;
    OutTraceDW("SmackSetSystemRes: mode=%#x(%s)\n", mode, modes[mode % 5]);
    // BYPASS the call to avoid resolution changes
    //return (*pSmackSetSystemRes)(h);
    mode = mode % 5;
    // save previous screen resolution for later mode=0 usage
    if(mode) {
        prevwidth = dxw.GetScreenWidth();
        prevheight = dxw.GetScreenHeight();
        dxw.SetFullScreen(TRUE);
    }
    switch(mode) {
    case 0:
        width = prevwidth;
        height = prevheight;
        break;
    case 1:
        width = 640;
        height = 400;
        break;
    case 2:
        width = 640;
        height = 480;
        break;
    case 3:
        width = 800;
        height = 600;
        break;
    case 4:
        width = 1024;
        height = 768;
        break;
    }
    dxw.SetScreenSize(width, height);
    if(dxw.Windowize && dxw.GethWnd()) dxw.AdjustWindowFrame(dxw.GethWnd(), width, height);
    return 0;
}
/*  ---------------------------------------------------------------
    Used by M2 Tank Platoon II:
    Ordinal 0x0015 name _SmackNextFrame@4
    Ordinal 0x000E name _SmackOpen@12
    Ordinal 0x0026 name _SmackSoundUseDirectSound@4
    Ordinal 0x0012 name _SmackClose@4
    Ordinal 0x0013 name _SmackDoFrame@4
    Ordinal 0x0017 name _SmackToBuffer@28
    Ordinal 0x0020 name _SmackWait@4
    /* ------------------------------------------------------------ */

#ifdef SMACK_TRACEALL
u32	WINAPI extSmackToBufferRect(Smack *h, u32 SmackSurface) {
    u32 ret;
    // t.b.d. role of SmackSurface: it could be a HDC, but no matter how you patch
    // its value nothing seems to change
    OutTraceDW("SmackToBufferRect: h=%#x surface=%#x\n", h, SmackSurface);
    DumpSmack(h);
    ret = (*pSmackToBufferRect)(h, SmackSurface);
    OutTraceDW("SmackToBufferRect: ret=%#x\n", ret);
    return ret;
}
#endif

#ifdef SMACK_TRACEALL
void WINAPI extSmackSoundCheck(void) {
    OutTraceDW("SmackSoundCheck:\n");
    return (*pSmackSoundCheck)();
}
#endif

void WINAPI extSmackToBuffer(Smack *h, u32 left, u32 top, u32 Pitch, u32 destheight, const void *buf, u32 Flags) {
    OutTraceDW("SmackToBuffer: h=%#x left=%d top=%d pitch=%d heigth=%d buf=%#x flags=%#x\n", h, left, top, Pitch, destheight, buf, Flags);
    DumpSmack(h);
    if(dxw.dwFlags11 & SMACKBUFFERNODEPTH) Flags &= ~SMACKBUFFER16; // clear first 2 bits
    return (*pSmackToBuffer)(h, left, top, Pitch, destheight, buf, Flags);
}

#ifdef SMACK_TRACEALL
void WINAPI extSmackBufferClose(Smack *h) {
    OutTraceDW("SmackBufferClose: h=%#x\n", h);
    DumpSmack(h);
    (*pSmackBufferClose)(h);
}
#endif

#ifdef SMACK_TRACEALL
u32 WINAPI extSmackBufferBlit(SmackBuf *sbuf, HDC dc, s32 hwndx, s32 hwndy, s32 subx, s32 suby, s32 subw, s32 subh) {
    u32 ret;
    OutTraceDW("SmackBufferBlit: sbuf=%#x hdc=%#x wnd=(%dx%d) subpos=(%d,%d) subsiz=(%dx%d)\n",
               sbuf, dc, hwndx, hwndy, subx, suby, subw, subh);
    ret = (*pSmackBufferBlit)(sbuf, dc, hwndx, hwndy, subx, suby, subw, subh);
    return ret;
}
#endif

#ifdef SMACK_TRACEALL
u32 WINAPI extSmackBufferFocused(SmackBuf *sbuf) {
    u32 ret;
    OutTraceDW("SmackBufferFocused: sbuf=%#x\n", sbuf);
    ret = (*pSmackBufferFocused)(sbuf);
    OutTraceDW("SmackBufferFocused: ret=%#x\n", ret);
    return ret;
}
#endif

// beware: the function SmackToScreen is available with 6 arguments (_SmackToScreen@24) or with 7 arguments (_SmackToScreen@28)
// so it is necessary to have 2 distinct wrappers. I dot no documentation about _SmackToScreen@24, so here is a dummy proxy.

#ifdef SMACK_TRACEALL
void WINAPI extSmackToScreen6(Smack *sbuf, u32 left, u32 top, u32 arg4, u32 arg5, u32 arg6) {
    OutTraceDW("SmackToScreen: sbuf=%#x left=%d top=%d arg4=%#x arg5=%#x arg6=%#x\n", sbuf, left, top, arg4, arg5, arg6);
    (*pSmackToScreen6)(sbuf, left, top, arg4, arg5, arg6);
}

void WINAPI extSmackToScreen7(Smack *sbuf, u32 left, u32 top, u32 BytePS, const u16 *WinTbl, void *SetBank, u32 Flags) {
    OutTraceDW("SmackToScreen: sbuf=%#x left=%d top=%d BytePS=%d flags=%#x\n", sbuf, left, top, BytePS, Flags);
    (*pSmackToScreen7)(sbuf, left, top, BytePS, WinTbl, SetBank, Flags);
}
#endif

#ifdef SMACK_TRACEALL
static char *ExplainSmackBlitFlags(DWORD c) {
    static char eb[128];
    unsigned int l;
    DWORD colormask;
    strcpy(eb, "SMACK");
    if (c & SMACKBLIT1X) strcat(eb, "BLIT1X+");
    if (c & SMACKBLIT2X) strcat(eb, "BLIT2X+");
    if (c & SMACKBLIT2XSMOOTHING) strcat(eb, "BLIT2XSMOOTHING+");
    if (c & SMACKBLIT2XINTERLACE) strcat(eb, "BLIT2XINTERLACE+");
    colormask = c & 0xC0000000; // last 2 m.s.b. bits
    if (colormask == SMACKBUFFER565) strcat(eb, "BUFFER565+");
    if (colormask == SMACKBUFFER555) strcat(eb, "BUFFER555+");
    l = strlen(eb);
    if (l > strlen("SMACK")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}
#endif

#ifdef SMACK_TRACEALL
HSMACKBLIT WINAPI extSmackBlitOpen(u32 flags) {
    HSMACKBLIT ret;
    OutTraceDW("SmackBlitOpen: flags=%#x(%s)\n", flags, ExplainSmackBlitFlags(flags));
    if(dxw.dwFlags11 & SMACKBUFFERNODEPTH) flags &= ~SMACKBUFFER16; // clear first 2 bits
    ret = (*pSmackBlitOpen)(flags);
    OutTraceDW("SmackBlitOpen: ret=%#x\n", ret);
    return ret;
}
#endif // SMACK_TRACEALL

#ifdef SMACK_TRACEALL
void WINAPI extSmackBlit(HSMACKBLIT sblit, void *dest, u32 destpitch, u32 destx, u32 desty, void *src, u32 srcpitch, u32 srcx, u32 srcy, u32 srcw, u32 srch) {
    OutTraceDW("SmackBlit: sblit=%#x dst=%#x dpitch=%d dpos=(%d,%d) src=%#x spitch=%d spos=(%d,%d) ssize=(%dx%d)\n",
               sblit, dest, destpitch, destx, desty, src, srcpitch, srcx, srcy, srcw, srch);
    (*pSmackBlit)(sblit, dest, destpitch, destx, desty, src, srcpitch, srcx, srcy, srcw, srch);
}
#endif // SMACK_TRACEALL

#ifdef SMACK_TRACEALL
void WINAPI extSmackBlitSetPalette(HSMACKBLIT sblit, void *Palette, u32 PalType) {
    OutTraceDW("SmackBlitSetPalette: sblit=%#x palette=%#x paltype=%d\n", sblit, Palette, PalType);
    (*pSmackBlitSetPalette)(sblit, Palette, PalType);
}
#endif // SMACK_TRACEALL

#ifdef SMACK_TRACEALL
u32 WINAPI extSmackBlitSetFlags(HSMACKBLIT sblit, u32 flags) {
    u32 ret;
    OutTraceDW("SmackBlitSetFlags: sblit=%#x flags=%#x\n", sblit, flags);
    ret = (*pSmackBlitSetFlags)(sblit, flags);
    OutTraceDW("SmackBlitSetFlags: ret=%d\n", ret);
    return ret;
}
#endif // SMACK_TRACEALL

// === smack 2.x "C" proxies ===

Smack *WINAPIV extSmackOpenC(const char *SmackFile, UINT32 flags, INT32 extrabuf) {
    Smack *h;
    OutTraceDW("SmackOpen: SmackFile=\"%s\" flags=%#x(%s) extrabuf=%#x\n", SmackFile, flags, ExplainSmackFlags(flags), extrabuf);
    h = (*pSmackOpenC)(SmackFile, flags, extrabuf);
    OutTraceDW("SmackOpen: ret=%#x\n", h);
    if (!h) return NULL;
    DumpSmack(h);
    if(dxw.dwFlags6 & NOMOVIES) {
        OutTraceDW("SmackOpen: NOMOVIES\n");
        h->Frames = 1; // returning NULL or a frame counter below 1 is risky!
        return h;
    }
    if(dxw.dwFlags10 & SUSPENDTIMESTRETCH) dxw.SuspendTimeStretch(TIMESHIFT_OFF);
    return h;
}

void WINAPIV extSmackCloseC(Smack *h) {
    OutTraceDW("SmackClose: h=%#x\n", h);
    (*pSmackCloseC)(h);
    if(dxw.dwFlags10 & SUSPENDTIMESTRETCH) dxw.SuspendTimeStretch(TIMESHIFT_ON);
}

void WINAPIV extSmackToBufferC(Smack *h, u32 left, u32 top, u32 Pitch, u32 destheight, const void *buf, u32 Flags) {
    OutTraceDW("SmackToBuffer: h=%#x left=%d top=%d pitch=%d heigth=%d buf=%#x flags=%#x\n", h, left, top, Pitch, destheight, buf, Flags);
    DumpSmack(h);
    if(dxw.dwFlags11 & SMACKBUFFERNODEPTH) Flags &= ~SMACKBUFFER16; // clear first 2 bits
    return (*pSmackToBufferC)(h, left, top, Pitch, destheight, buf, Flags);
}

Smack *WINAPIV extSmackDoFrameC(Smack *h) {
    Smack *ret;
    OutTraceDW("SmackDoFrame: h=%#x\n", h);
    DumpSmack((Smack *)h);
    ret = (*pSmackDoFrameC)(h);
    if(dxw.dwFlags7 & FIXSMACKLOOP) {
        dxw.MessagePump();
        dxw.ScreenRefresh();
    }
    return ret;
}

Smack *WINAPIV extSmackColorRemapC(Smack *h, const void *remappal, u32 numcolors, u32 paltype) {
    OutTraceDW("SmackColorRemap: h=%#x numcolors=%d paltype=%d\n", h, numcolors, paltype);
    DumpSmack(h);
    // BYPASS the call to avoid resolution changes
    //return (*pSmackColorRemap)(h, remappal, numcolors, paltype);
    return h;
}
