extern void HookDDraw(HMODULE);
extern void HookDirect3D8(HMODULE);
extern void HookDirect3D9(HMODULE);
extern void HookDirect3D11(HMODULE);
extern void HookDirect3D9_30(HMODULE);
extern void HookDirect3D7(HMODULE);
extern void HookDirect3D16(HMODULE);
extern void HookOle32(HMODULE);
extern void HookOleAut32(HMODULE);
extern void HookGDI32(HMODULE);
extern void HookDirectInput(HMODULE);
extern void HookDirectInput8(HMODULE);
extern void HookDirectSound(HMODULE);
extern void HookImeLib(HMODULE);
extern void HookKernel32(HMODULE);
extern void HookUser32(HMODULE);
extern void HookTrust(HMODULE);
extern void HookWinMM(HMODULE);
extern void HookAdvApi32(HMODULE);
extern void HookImagehlp(HMODULE);
extern void HookSmackW32(HMODULE);
extern void HookSmkWai32(HMODULE);
extern void HookBinkW32(HMODULE);
extern void HookAVIFil32(HMODULE);
extern void HookComDlg32(HMODULE);
extern void HookComCtl32(HMODULE);
extern void HookWinG32(HMODULE);
extern void HookSDL(HMODULE);
extern void HookWAIL32(HMODULE);
extern void HookMSV4WLibs(HMODULE);
extern void HookOpenGL(HMODULE);
extern void HookXInput11(HMODULE);
extern void HookXInput12(HMODULE);
extern void HookXInput13(HMODULE);
extern void HookXInput14(HMODULE);
extern void HookXInput910(HMODULE);
extern void HookDirectDrawFactoryLib(HMODULE);
extern void HookDXGILib(HMODULE);
extern void HookEARpds(HMODULE);
extern void HookEARias(HMODULE);
extern void HookMSVCRT(HMODULE);
extern void HookVersion(HMODULE);
extern void HookNtDll(HMODULE);
extern void	HookLauncher(HMODULE);
extern void	HookDSetup(HMODULE);
extern void	HookDwmApi(HMODULE);

extern void InitPosition(int, int, int, int, int, int);
//void InitWindowPos(int, int, int, int);
extern LPCSTR ProcToString(LPCSTR proc);
extern HRESULT HookDxDiag(REFIID, LPVOID FAR *);

extern FARPROC Remap_AVIFil32_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_ddraw_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_d3d7_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_d3d8_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_d3d9_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_d3d10_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_d3d10_1_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_d3d11_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_gl_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_smack_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_smkwai_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_bink_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_user32_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_GDI32_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_kernel32_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_Ole32_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_OleAut32_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_trust_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_WinMM_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_ImeLib_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_vfw_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_AdvApi32_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_Glide_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_Imagehlp_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_DInput_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_DInput8_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_ComCtl32_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_ComDlg32_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_DSound_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_WinG32_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_SDL_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_XInput_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_ddrawex_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_DXGI_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_EAR_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_MSVCRT_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_Version_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_DSetup_ProcAddress(LPCSTR, HMODULE);
extern FARPROC Remap_DwmApi_ProcAddress(LPCSTR, HMODULE);

// extern function declaration

extern void	HookKernel32Init();
extern void	HookUser32Init();
extern void	HookGDI32Init();
extern void HookWinMMInit();

typedef enum {
    HOOK_IAT_CANDIDATE = 0,
    HOOK_HOT_CANDIDATE,
    HOOK_HOT_REQUIRED,
    HOOK_IAT_LINKED,
    HOOK_HOT_LINKED
} HookEntry_Status;

typedef struct {
    HookEntry_Status HookStatus;
    DWORD ordinal;
    char *APIName;
    FARPROC OriginalAddress;
    FARPROC *StoreAddress;
    FARPROC HookerAddress;
} HookEntryEx_Type;

extern FARPROC RemapLibraryEx(LPCSTR, HMODULE, HookEntryEx_Type *);
extern void HookLibraryEx(HMODULE, HookEntryEx_Type *, char *);
extern void PinLibraryEx(HookEntryEx_Type *, char *);
extern void HookLibInitEx(HookEntryEx_Type *);
extern BOOL IsHotPatchedEx(HookEntryEx_Type *, char *);
extern BOOL IsHookedBlock(HookEntryEx_Type *);

#define MISSING ((FARPROC)-1)