#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhook.h"

/*
    typedef struct {
	char *name;
	BOOL prefixed;
	HMODULE hmodule;
	DWORD flags;
	DxwHookFunction_Type hookf;
	RxwRemapFunction_Type remapf;
    } dxwLibsHookTable_Type;

    #define DXWHOOK_NULL    0x00000000 // unknown module (or pretending to be such) to be hooked if "Hook all DLL" is set
    #define DXWHOOK_HOOK    0x00000001 // the module has its own hook plugin for IAT and GetProcAddress mapping
    #define DXWHOOK_EXTEND  0x00000002 // the module needs to extend hooking on all its dependencies
    #define DXWHOOK_SKIP    0x00000004 // the module should be ignored
    #define DXWHOOK_REPLACE 0x00000008 // the module could have a replacement dll
*/

dxwLibsHookTable_Type SysLibsTable[] = {
    {
        "kernel32",	FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_KERNEL32
        HookKernel32, Remap_kernel32_ProcAddress
    },
    {
        "USER32",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_USER32
        HookUser32, Remap_user32_ProcAddress
    },
    {
        "GDI32",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_GDI32
        HookGDI32, Remap_GDI32_ProcAddress
    },
    {
        "imelib",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_IMELIB
        HookImeLib, Remap_ImeLib_ProcAddress
    },
    {
        "ADVAPI32",	FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_ADVAPI32
        HookAdvApi32, Remap_AdvApi32_ProcAddress
    },
    {
        "ole32",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_OLE32
        HookOle32, Remap_Ole32_ProcAddress
    },
    //{"oleaut32",	FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_OLEAUT32
    //HookOleAut32, Remap_OleAut32_ProcAddress},
    {
        "oleaut32",	FALSE,	NULL,	DXWHOOK_SKIP, // SYSLIBIDX_OLEAUT32
        NULL, NULL
    },
    {
        "uxtheme",		FALSE,	NULL,	DXWHOOK_SKIP, // SYSLIBIDX_UXTHEME
        NULL, NULL
    },
    {
        "setupapi",	FALSE,	NULL,	DXWHOOK_SKIP, // SYSLIBIDX_SETUPAPI
        NULL, NULL
    },
    {
        "dwmapi",		FALSE,	NULL,	DXWHOOK_SKIP, // SYSLIBIDX_DWMAPI
        NULL, NULL
    },
    {
        "ddraw",		FALSE,	NULL,	DXWHOOK_HOOK | DXWHOOK_REPLACE, // SYSLIBIDX_DIRECTDRAW
        HookDDraw, Remap_ddraw_ProcAddress
    },
    {
        "d3d8",		FALSE,	NULL,	DXWHOOK_HOOK | DXWHOOK_REPLACE, // SYSLIBIDX_DIRECT3D8
        HookDirect3D8, Remap_d3d8_ProcAddress
    },
    {
        "d3d9",		FALSE,	NULL,	DXWHOOK_HOOK | DXWHOOK_REPLACE, // SYSLIBIDX_DIRECT3D9
        HookDirect3D9, Remap_d3d9_ProcAddress
    },
    {
        "d3dx9_",		TRUE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_DIRECT3D9_##
        HookDirect3D9_30, NULL
    },
    {
        "d3d10",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_DIRECT3D10
        NULL, Remap_d3d10_ProcAddress
    },
    {
        "d3d10_1",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_DIRECT3D10_1
        NULL, Remap_d3d10_1_ProcAddress
    },
    {
        "d3d11",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_DIRECT3D11
        HookDirect3D11, Remap_d3d11_ProcAddress
    },
    {
        "opengl32",	FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_OPENGL
        HookOpenGL, Remap_gl_ProcAddress
    },
    {
        "msvfw32",		FALSE,	NULL,	DXWHOOK_HOOK | DXWHOOK_EXTEND, // SYSLIBIDX_MSVFW
        HookMSV4WLibs, Remap_vfw_ProcAddress
    },
    {
        "dsound",		FALSE,	NULL,	DXWHOOK_SKIP, // SYSLIBIDX_DSOUND
        HookDirectSound, Remap_DSound_ProcAddress
    },
    {
        "winmm",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_WINMM
        HookWinMM, Remap_WinMM_ProcAddress
    },
    {
        "imm32",		FALSE,	NULL,	DXWHOOK_SKIP, // SYSLIBIDX_IMM32
        NULL, NULL
    },
    {
        "dinput",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_DINPUT
        HookDirectInput, Remap_DInput_ProcAddress
    },
    {
        "dinput8",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_DINPUT8
        HookDirectInput8, Remap_DInput8_ProcAddress
    },
    {
        "wintrust",	FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_WINTRUST
        HookTrust, Remap_trust_ProcAddress
    },
    {
        "d3dim",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_DIRECT3D
        HookDirect3D16, Remap_d3d7_ProcAddress
    },
    {
        "d3dim700",	FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_DIRECT3D700
        HookDirect3D7, Remap_d3d7_ProcAddress
    },
    {
        "imagehlp",	FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_IMAGEHLP
        HookImagehlp, Remap_Imagehlp_ProcAddress
    },
    {
        "comdlg32",	FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_COMDLG32
        HookComDlg32, Remap_ComDlg32_ProcAddress
    },
    {
        "comctl32",	FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_COMCTL32
        HookComCtl32, Remap_ComCtl32_ProcAddress
    },
    {
        "AVIFIL32",	FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_AVIFIL32
        HookAVIFil32, Remap_AVIFil32_ProcAddress
    },
    {
        "glide",		FALSE,	NULL,	DXWHOOK_NULL, // SYSLIBIDX_GLIDE
        NULL, NULL
    },
    {
        "glide2x",		FALSE,	NULL,	DXWHOOK_NULL, // SYSLIBIDX_GLIDE2
        NULL, NULL
    },
    {
        "glide3x",		FALSE,	NULL,	DXWHOOK_NULL, // SYSLIBIDX_GLIDE3
        NULL, NULL
    },
    {
        "SDL",			FALSE,	NULL,	DXWHOOK_EXTEND, // SYSLIBIDX_SDL
        HookSDL, Remap_SDL_ProcAddress
    },
    {
        "SDL2",		FALSE,	NULL,	DXWHOOK_EXTEND, // SYSLIBIDX_SDL2
        HookSDL, Remap_SDL_ProcAddress
    },
    // Beware: if hooked, SmackW32 must extend hook to detect inner references to ddraw and dsound
    {
        "SMACKW32",	FALSE,	NULL,	DXWHOOK_NULL, // SYSLIBIDX_SMACKW32
        HookSmackW32, Remap_smack_ProcAddress
    },
    // SMKWAI32.DLL is a variation of SMACKW32.DLL linked to WAIL32.DLL sound lib and used by
    // Win32 version of "Heroes of Might and Magic"
    {
        "SMKWAI32",	FALSE,	NULL,	DXWHOOK_NULL, // SYSLIBIDX_SMKWAI32
        HookSmkWai32, Remap_smkwai_ProcAddress
    },
    // Beware: if hooked, BinkW32 must extend hook to detect inner references to ddraw and dsound
    {
        "BINKW32",		FALSE,	NULL,	DXWHOOK_NULL, // SYSLIBIDX_BINKW32
        HookBinkW32, Remap_bink_ProcAddress
    },
    {
        "WING32",		FALSE,	NULL,	DXWHOOK_NULL, // SYSLIBIDX_WING32
        HookWinG32,	Remap_WinG32_ProcAddress
    },
    {
        "DPLAYX",		FALSE,	NULL,	DXWHOOK_SKIP, // SYSLIBIDX_DPLAYX
        NULL,	NULL
    },
    {
        "WSOCK32",		FALSE,	NULL,	DXWHOOK_SKIP, // SYSLIBIDX_WSOCK32
        NULL,	NULL
    },
    {
        "VERSION",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_VERSION v2.05.12 hooked for CUSTOMLOCALE
        HookVersion, Remap_Version_ProcAddress
    },
    {
        "WINSPOOL",	FALSE,	NULL,	DXWHOOK_SKIP, // SYSLIBIDX_WINSPOOL
        NULL,	NULL
    },
    {
        "ZLIB",		FALSE,	NULL,	DXWHOOK_SKIP, // SYSLIBIDX_ZLIB
        NULL,	NULL
    },
    {
        "XINPUT1_1",	FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_XINPUT11
        HookXInput11, Remap_XInput_ProcAddress
    },
    {
        "XINPUT1_2",	FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_XINPUT12
        HookXInput12, Remap_XInput_ProcAddress
    },
    {
        "XINPUT1_3",	FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_XINPUT13
        HookXInput13, Remap_XInput_ProcAddress
    },
    {
        "XINPUT1_4",	FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_XINPUT14
        HookXInput14, Remap_XInput_ProcAddress
    },
    {
        "XINPUT9_1_0",	FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_XINPUT910
        HookXInput910, Remap_XInput_ProcAddress
    },
    {
        "NTDLL",		FALSE,	NULL,	DXWHOOK_SKIP, // SYSLIBIDX_NTDLL
        HookNtDll,	NULL
    },
    {
        "DDRAWEX",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_DDRAWEX
        HookDirectDrawFactoryLib, Remap_ddrawex_ProcAddress
    },
    {
        "DXGI",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_DXGI
        HookDXGILib, Remap_DXGI_ProcAddress
    },
    {
        "EARPDS",		FALSE,	NULL,	DXWHOOK_HOOK | DXWHOOK_EXTEND, // SYSLIBIDX_EARPDS
        HookEARpds, Remap_EAR_ProcAddress
    },
    {
        "EARIAS",		FALSE,	NULL,	DXWHOOK_HOOK | DXWHOOK_EXTEND, // SYSLIBIDX_EARIAS
        HookEARias, Remap_EAR_ProcAddress
    },
    {
        "MSVCRT",		FALSE,	NULL,	DXWHOOK_SKIP, // SYSLIBIDX_MSVCRT
        HookMSVCRT, Remap_MSVCRT_ProcAddress
    },
    {
        "KERNELBASE",		FALSE,	NULL,	DXWHOOK_SKIP, // SYSLIBIDX_KERNELBASE
        NULL, NULL
    },
    {
        "DSETUP",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_DSETUP
        HookDSetup, Remap_DSetup_ProcAddress
    },
    {
        "dwmapi",		FALSE,	NULL,	DXWHOOK_HOOK, // SYSLIBIDX_DWMAPI
        HookDwmApi, Remap_DwmApi_ProcAddress
    },
    {NULL,			FALSE,	NULL,	DXWHOOK_NULL, NULL, NULL}
};
