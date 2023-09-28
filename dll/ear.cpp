#define _CRT_SECURE_NO_WARNINGS

//#define TRACEALL

#include <windows.h>
#include <ear.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhook.h"
#include "dxhelper.h"
#include "syslibs.h"
#include "stdio.h"

typedef DWORD (WINAPI *earProxyW0_Type)(void);
typedef DWORD (WINAPI *earProxyW4_Type)(DWORD);
typedef DWORD (WINAPI *earProxyW8_Type)(DWORD, DWORD);
typedef DWORD (WINAPI *earProxyW16_Type)(DWORD, DWORD, DWORD, DWORD);
typedef VOID  (WINAPI *earProxyV0_Type)(void);
typedef VOID  (WINAPI *earProxyV4_Type)(DWORD);
typedef VOID  (WINAPI *earProxyV8_Type)(DWORD, DWORD);

#ifdef TRACEALL
DWORD WINAPI earAAA_Validate(DWORD);
DWORD WINAPI earAssignHwnd(DWORD);
DWORD WINAPI earChangeChannelControl(DWORD, DWORD);
DWORD WINAPI earClearChannel(DWORD, DWORD);
DWORD WINAPI earEarInactive(void);
DWORD WINAPI earGetEventStatus(DWORD, DWORD);
DWORD WINAPI earGetLastError(void);
DWORD WINAPI earGetVersion(void);
DWORD WINAPI earInitializeEar(DWORD);
DWORD WINAPI earMixEvent(DWORD);
DWORD WINAPI earMoveEvent(DWORD);
DWORD WINAPI earRegisterBank(DWORD, DWORD);
DWORD WINAPI earRegisterChannel(DWORD, DWORD, DWORD, DWORD);
DWORD WINAPI earRegisterEnvironment(DWORD);
DWORD WINAPI earRegisterMatrix(DWORD);
DWORD WINAPI earRegisterPreset(DWORD, DWORD);
VOID  WINAPI earResetEar(void);
VOID  WINAPI earSetAttenuationLevel(DWORD, DWORD);
VOID  WINAPI earSetUserDistanceUnit(DWORD, DWORD);
VOID  WINAPI earShowLastError(void);
DWORD WINAPI earShutDownBank(DWORD);
DWORD WINAPI earShutDownChannel(DWORD);
VOID  WINAPI earShutDownEar(void);
DWORD WINAPI earShutDownEnvironment(DWORD);
DWORD WINAPI earShutDownEvent(DWORD, DWORD);
DWORD WINAPI earShutDownMatrix(DWORD);
DWORD WINAPI earShutDownPreset(DWORD);
VOID  WINAPI earShutDownTimer(void);
DWORD WINAPI earStartEvent(DWORD);
DWORD WINAPI earStartTimer(void);
VOID  WINAPI earUpdateEar(void);

earProxyW4_Type pearAAA_Validate;
earProxyW4_Type pearAssignHwnd;
earProxyW8_Type pearChangeChannelControl;
earProxyW8_Type pearClearChannel;
earProxyW0_Type pearEarInactive;
earProxyW8_Type pearGetEventStatus;
earProxyW0_Type pearGetLastError;
earProxyW0_Type pearGetVersion;
earProxyW4_Type pearInitializeEar;
earProxyW4_Type pearMixEvent;
earProxyW4_Type pearMoveEvent;
earProxyW8_Type pearRegisterBank;
earProxyW16_Type pearRegisterChannel;
earProxyW4_Type pearRegisterEnvironment;
earProxyW4_Type pearRegisterMatrix;
earProxyW8_Type pearRegisterPreset;
earProxyV0_Type pearResetEar;
earProxyV8_Type pearSetAttenuationLevel;
earProxyV8_Type pearSetUserDistanceUnit;
earProxyV0_Type pearShowLastError;
earProxyW4_Type pearShutDownBank;
earProxyW4_Type pearShutDownChannel;
earProxyV0_Type pearShutDownEar;
earProxyW4_Type pearShutDownEnvironment;
earProxyW8_Type pearShutDownEvent;
earProxyW4_Type pearShutDownMatrix;
earProxyW4_Type pearShutDownPreset;
earProxyV0_Type pearShutDownTimer;
earProxyW4_Type pearStartEvent;
earProxyW0_Type pearStartTimer;
earProxyV0_Type pearUpdateEar;
#endif // TRACEALL

static HookEntryEx_Type EARHooks[] = {
#ifdef TRACEALL
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_AAA_Validate@4", (FARPROC)NULL, (FARPROC *) &pearAAA_Validate, (FARPROC)earAAA_Validate},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_AssignHwnd@4", (FARPROC)NULL, (FARPROC *) &pearAssignHwnd, (FARPROC)earAssignHwnd},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_ChangeChannelControl@8", (FARPROC)NULL, (FARPROC *) &pearChangeChannelControl, (FARPROC)earChangeChannelControl},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_ClearChannel@8", (FARPROC)NULL, (FARPROC *) &pearClearChannel, (FARPROC)earClearChannel},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_EarInactive@0", (FARPROC)NULL, (FARPROC *) &pearEarInactive, (FARPROC)earEarInactive},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_GetEventStatus@8", (FARPROC)NULL, (FARPROC *) &pearGetEventStatus, (FARPROC)earGetEventStatus},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_GetLastError@0", (FARPROC)NULL, (FARPROC *) &pearGetLastError, (FARPROC)earGetLastError},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_GetVersion@0", (FARPROC)NULL, (FARPROC *) &pearGetVersion, (FARPROC)earGetVersion},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_InitializeEar@4", (FARPROC)NULL, (FARPROC *) &pearInitializeEar, (FARPROC)earInitializeEar},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_MixEvent@4", (FARPROC)NULL, (FARPROC *) &pearMixEvent, (FARPROC)earMixEvent},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_MoveEvent@4", (FARPROC)NULL, (FARPROC *) &pearMoveEvent, (FARPROC)earMoveEvent},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_RegisterBank@8", (FARPROC)NULL, (FARPROC *) &pearRegisterBank, (FARPROC)earRegisterBank},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_RegisterChannel@16", (FARPROC)NULL, (FARPROC *) &pearRegisterChannel, (FARPROC)earRegisterChannel},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_RegisterEnvironment@4", (FARPROC)NULL, (FARPROC *) &pearRegisterEnvironment, (FARPROC)earRegisterEnvironment},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_RegisterMatrix@4", (FARPROC)NULL, (FARPROC *) &pearRegisterMatrix, (FARPROC)earRegisterMatrix},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_RegisterPreset@8", (FARPROC)NULL, (FARPROC *) &pearRegisterPreset, (FARPROC)earRegisterPreset},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_ResetEar@0", (FARPROC)NULL, (FARPROC *) &pearResetEar, (FARPROC)earResetEar},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_SetAttenuationLevel@8", (FARPROC)NULL, (FARPROC *) &pearSetAttenuationLevel, (FARPROC)earSetAttenuationLevel},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_SetUserDistanceUnit@8", (FARPROC)NULL, (FARPROC *) &pearSetUserDistanceUnit, (FARPROC)earSetUserDistanceUnit},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_ShowLastError@0", (FARPROC)NULL, (FARPROC *) &pearShowLastError, (FARPROC)earShowLastError},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_ShutDownBank@4", (FARPROC)NULL, (FARPROC *) &pearShutDownBank, (FARPROC)earShutDownBank},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_ShutDownChannel@4", (FARPROC)NULL, (FARPROC *) &pearShutDownChannel, (FARPROC)earShutDownChannel},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_ShutDownEar@0", (FARPROC)NULL, (FARPROC *) &pearShutDownEar, (FARPROC)earShutDownEar},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_ShutDownEnvironment@4", (FARPROC)NULL, (FARPROC *) &pearShutDownEnvironment, (FARPROC)earShutDownEnvironment},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_ShutDownEvent@8", (FARPROC)NULL, (FARPROC *) &pearShutDownEvent, (FARPROC)earShutDownEvent},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_ShutDownMatrix@4", (FARPROC)NULL, (FARPROC *) &pearShutDownMatrix, (FARPROC)earShutDownMatrix},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_ShutDownPreset@4", (FARPROC)NULL, (FARPROC *) &pearShutDownPreset, (FARPROC)earShutDownPreset},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_ShutDownTimer@0", (FARPROC)NULL, (FARPROC *) &pearShutDownTimer, (FARPROC)earShutDownTimer},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_StartEvent@4", (FARPROC)NULL, (FARPROC *) &pearStartEvent, (FARPROC)earStartEvent},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_StartTimer@0", (FARPROC)NULL, (FARPROC *) &pearStartTimer, (FARPROC)earStartTimer},
    {HOOK_HOT_CANDIDATE, 0, "_EAR_DLL_UpdateEar@0", (FARPROC)NULL, (FARPROC *) &pearUpdateEar, (FARPROC)earUpdateEar},
#endif // TRACEALL
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

void HookEARpds(HMODULE module) {
    HookLibraryEx(module, EARHooks, "earpds.dll");
}

void HookEARias(HMODULE module) {
    HookLibraryEx(module, EARHooks, "earias.dll");
}

FARPROC Remap_EAR_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if (!(dxw.dwFlags11 & HOOKEARSOUND)) return NULL;
    if (addr = RemapLibraryEx(proc, hModule, EARHooks)) return addr;
    return NULL;
}

#ifdef TRACEALL
DWORD WINAPI earAAA_Validate(DWORD w1) {
    DWORD ret;
    ApiName("ear:AAA_Validate");
    OutTrace("%s: w1=%#x\n", ApiRef, w1);
#ifdef EARBYPASS
    if(w1 == EAR_VALIDATION_NUMBER) return EAR_ERR_NOERROR;
    return EAR_ERR_EARNOTVALIDATED;
#endif
    ret = (*pearAAA_Validate)(w1);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earAssignHwnd(DWORD hwnd) {
    DWORD ret;
    ApiName("ear:AssignHwnd");
    OutTrace("%s: hwnd=%#x\n", ApiRef, hwnd);
#ifdef EARBYPASS
    return 1;
#endif
    ret = (*pearAssignHwnd)(hwnd);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earChangeChannelControl(DWORD w1, DWORD w2) {
    DWORD ret;
    ApiName("ear:ChangeChannelControl");
    OutTrace("%s: w1=%#x w2=%#x\n", ApiRef, w1, w2);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearChangeChannelControl)(w1, w2);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earClearChannel(DWORD w1, DWORD w2) {
    DWORD ret;
    ApiName("ear:ClearChannel");
    OutTrace("%s: w1=%#x w2=%#x\n", ApiRef, w1, w2);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearClearChannel)(w1, w2);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earEarInactive(void) {
    DWORD ret;
    ApiName("ear:EarInactive");
    OutTrace("%s\n", ApiRef);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearEarInactive)();
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earGetEventStatus(DWORD w1, DWORD w2) {
    DWORD ret;
    ApiName("ear:GetEventStatus");
    OutTrace("%s: w1=%#x w2=%#x\n", ApiRef, w1, w2);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearGetEventStatus)(w1, w2);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earGetLastError(void) {
    DWORD ret;
    ApiName("ear:GetLastError");
    OutTrace("%s\n", ApiRef);
#ifdef EARBYPASS
    return EAR_ERR_NOERROR;
#endif
    ret = (*pearGetLastError)();
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earGetVersion(void) {
    DWORD ret;
    ApiName("ear:GetVersion");
    OutTrace("%s\n", ApiRef);
#ifdef EARBYPASS
    return (DWORD)EAR_H_VERSION;
#endif
    ret = (*pearGetVersion)();
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earInitializeEar(DWORD w1) {
    DWORD ret;
    ApiName("ear:InitializeEar");
    OutTrace("%s: w1=%#x\n", ApiRef, w1);
    w1 = 0;
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearInitializeEar)(w1);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earMixEvent(DWORD w1) {
    DWORD ret;
    ApiName("ear:MixEvent");
    OutTrace("%s: w1=%#x\n", ApiRef, w1);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearMixEvent)(w1);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earMoveEvent(DWORD w1) {
    DWORD ret;
    ApiName("ear:MoveEvent");
    OutTrace("%s: w1=%#x\n", ApiRef, w1);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearMoveEvent)(w1);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earRegisterBank(DWORD w1, DWORD w2) {
    DWORD ret;
    ApiName("ear:RegisterBank");
    OutTrace("%s: w1=%#x w2=%#x\n", ApiRef, w1, w2);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearRegisterBank)(w1, w2);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earRegisterChannel(DWORD w1, DWORD w2, DWORD w3, DWORD w4) {
    DWORD ret;
    ApiName("ear:RegisterChannel");
    OutTrace("%s: w1=%#x w2=%#x w3=%#x w4=%#x\n", ApiRef, w1, w2, w3, w4);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearRegisterChannel)(w1, w2, w3, w4);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earRegisterEnvironment(DWORD w1) {
    DWORD ret;
    ApiName("ear:RegisterEnvironment");
    OutTrace("%s: w1=%#x\n", ApiRef, w1);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearRegisterEnvironment)(w1);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earRegisterMatrix(DWORD w1) {
    DWORD ret;
    ApiName("ear:RegisterMatrix");
    OutTrace("%s: w1=%#x\n", ApiRef, w1);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearRegisterMatrix)(w1);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earRegisterPreset(DWORD w1, DWORD w2) {
    DWORD ret;
    ApiName("ear:RegisterPreset");
    OutTrace("%s: w1=%#x w2=%#x\n", ApiRef, w1, w2);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearRegisterPreset)(w1, w2);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

VOID WINAPI earResetEar(void) {
    OutTrace("ear:ResetEar\n");
#ifdef EARBYPASS
    return;
#endif
    (*pearResetEar)();
}

VOID  WINAPI earSetAttenuationLevel(DWORD w1, DWORD w2) {
    ApiName("ear:SetAttenuationLevel");
    OutTrace("%s: w1=%#x w2=%#x\n", ApiRef, w1, w2);
#ifdef EARBYPASS
    return 0;
#endif
    (*pearSetAttenuationLevel)(w1, w2);
}

VOID WINAPI earSetUserDistanceUnit(DWORD w1, DWORD w2) {
    ApiName("ear:SetUserDistanceUnit");
    OutTrace("%s: w1=%#x w2=%#x\n", ApiRef, w1, w2);
#ifdef EARBYPASS
    return 0;
#endif
    (*pearSetUserDistanceUnit)(w1, w2);
}

VOID WINAPI earShowLastError(void) {
    OutTrace("ear:ShowLastError\n");
#ifdef EARBYPASS
    return;
#endif
    (*pearShowLastError)();
}

DWORD WINAPI earShutDownBank(DWORD w1) {
    DWORD ret;
    ApiName("ear:ShutDownBank");
    OutTrace("%s: w1=%#x\n", ApiRef, w1);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearShutDownBank)(w1);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earShutDownChannel(DWORD w1) {
    DWORD ret;
    ApiName("ear:ShutDownChannel");
    OutTrace("%s: w1=%#x\n", ApiRef, w1);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearShutDownChannel)(w1);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

VOID WINAPI earShutDownEar(void) {
    OutTrace("ear:ShutDownEar\n");
#ifdef EARBYPASS
    return;
#endif
    (*pearShutDownEar)();
}

DWORD WINAPI earShutDownEnvironment(DWORD w1) {
    DWORD ret;
    ApiName("ear:ShutDownEnvironment");
    OutTrace("%s: w1=%#x\n", ApiRef, w1);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearShutDownEnvironment)(w1);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earShutDownEvent(DWORD w1, DWORD w2) {
    DWORD ret;
    ApiName("ear:ShutDownEvent");
    OutTrace("%s: w1=%#x w2=%#x\n", ApiRef, w1, w2);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearShutDownEvent)(w1, w2);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earShutDownMatrix(DWORD w1) {
    DWORD ret;
    ApiName("ear:ShutDownMatrix");
    OutTrace("%s: w1=%#x\n", ApiRef, w1);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearShutDownMatrix)(w1);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earShutDownPreset(DWORD w1) {
    DWORD ret;
    ApiName("ear:ShutDownPreset");
    OutTrace("%s: w1=%#x\n", ApiRef, w1);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearShutDownPreset)(w1);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

VOID WINAPI earShutDownTimer(void) {
    OutTrace("ear:ShutDownTimer\n");
#ifdef EARBYPASS
    return;
#endif
    (*pearShutDownTimer)();
}

DWORD WINAPI earStartEvent(DWORD w1) {
    DWORD ret;
    ApiName("ear:StartEvent");
    OutTrace("%s: w1=%#x\n", ApiRef, w1);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearStartEvent)(w1);
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

DWORD WINAPI earStartTimer(void) {
    DWORD ret;
    ApiName("ear:StartTimer");
    OutTrace("%s\n", ApiRef);
#ifdef EARBYPASS
    return 0;
#endif
    ret = (*pearStartTimer)();
    OutTrace("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

VOID WINAPI earUpdateEar(void) {
    OutTrace("ear:UpdateEar\n");
#ifdef EARBYPASS
    return;
#endif
    (*pearUpdateEar)();
}
#endif // TRACEALL