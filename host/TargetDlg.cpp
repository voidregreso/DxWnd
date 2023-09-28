// TargetDlg.cpp : Implementation
//

#include "stdafx.h"
#include "shlwapi.h"
#include "dxwndhost.h"
#include "TargetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL KillProcByName(char *, BOOL, BOOL);
extern BOOL gbDebug;
extern BOOL gbExpertMode;
extern DWORD GetDxWndCaps(void);

/////////////////////////////////////////////////////////////////////////////
// CTargetDlg Dialog

CTargetDlg::CTargetDlg(CWnd *pParent /*=NULL*/)
    : CDialog(CTargetDlg::IDD, pParent) {
    m_ItemIndex = 0;
    //{{AFX_DATA_INIT(CTargetDlg)
    m_DXVersion = 0;
    m_MonitorId = 0; // default: the first one, of course ....
    m_FilterId = 0; // default: none
    m_RendererId = 3; // default: primary surface
    m_MaxDdrawInterface = 6;
    m_SlowRatio = 2;
    m_Coordinates = 0;
    m_InitColorDepth = 0; // default: current color depth
    m_BilinearFilter = 0; // default: ddraw filtering
    m_BlurEffect = 0; // default: ddraw filtering
    m_DCEmulationMode = 0; // default: no emulation
    m_MouseVisibility = 0;
    m_MouseClipper = 0;
    m_OffendingMessages = 0;
    m_TextureHandling = 0;
    m_IATAlignedMode = 0;
    m_SonProcessMode = 0;
    m_ResTypes = 0; // v2.04.68 fix - default: SVGA
    m_ZOrder = 0;
    m_GFocusMode = 0; // default
    m_HookDI = FALSE;
    m_HookDI8 = FALSE;
    m_EmulateRelMouse = FALSE; // ??
    m_ScaleRelMouse = FALSE; // ??
    m_SkipDevTypeHID = FALSE;
    m_SharedKeyboard = FALSE;
    m_SuppressDIErrors = FALSE;
    m_ModifyMouse = TRUE; // default true !!
    m_VirtualJoystick = FALSE;
    m_HideJoysticks = FALSE;
    m_Unacquire = FALSE;
    //m_LogEnabled = FALSE;
    m_OutDebug = FALSE;
    m_RegistryOp = FALSE;
    m_CursorTrace = FALSE;
    m_OutWinMessages = FALSE;
    m_OutDWTrace = FALSE;
    m_OutOGLTrace = FALSE;
    m_OutHexTrace = FALSE;
    m_OutSDLTrace = FALSE;
    m_OutTimeTrace = FALSE;
    m_OutSoundTrace = FALSE;
    m_OutInputs = FALSE;
    m_OutLocale = FALSE;
    m_OutFPS = FALSE;
    m_OutSysLibs = FALSE;
    m_OutWGTrace = FALSE;
    m_OutD3DTrace = FALSE;
    m_OutDDRAWTrace = FALSE;
    m_OutDebugString = FALSE;
    //m_EraseLogFile = FALSE;
    m_AddTimeStamp = FALSE;
    m_AddRelativeTime = FALSE;
    m_AddThreadID = FALSE;
    m_ImportTable = FALSE;
    m_TraceHooks = FALSE;
    m_HandleExceptions = FALSE;
    m_VirtualHeap = FALSE;
    m_VirtualProcHeap = FALSE;
    m_NoBAADFOOD = FALSE;
    m_SuppressIME = FALSE;
    m_SuppressD3DExt = FALSE;
    m_Enum16bitModes = FALSE;
    m_TrimTextureFormats = FALSE;
    m_TransformAndLight = FALSE;
    m_ForceD3DGammaRamp = FALSE;
    m_LightGammaRamp = FALSE;
    m_D3D8Back16 = FALSE;
    m_NoHALDevice = FALSE;
    m_NoTnLDevice = FALSE;
    //m_RawFormat = FALSE;
    m_TextureFileFormat = 0;
    m_SetCompatibility = TRUE; // default true !!
    m_AEROBoost = TRUE; // default true !!
    m_DiabloTweak = FALSE;
    m_HookDirectSound = FALSE;
    m_VirtualCDAudio = FALSE;
    m_ForceTrackRepeat = FALSE;
    m_CDROMPresent = FALSE;
    m_HackMCIFrames = FALSE;
    m_CDPauseCapability = FALSE;
    m_SuppressCDAudio = FALSE;
    m_ReserveMCIDevice = FALSE;
    m_FixDefaultMCIId = FALSE;
    m_LockCDTray = FALSE;
    m_HookEARSound = FALSE;
    m_IgnoreMCIDevId = FALSE;
    m_SoundMute = FALSE;
    m_DSInitVolume = FALSE;
    m_LockVolume = FALSE;
    m_SafeMidiOut = FALSE;
    m_SetCDVolume = FALSE;
    m_HookWinG32 = FALSE;
    m_HookXinput = FALSE;
    m_HookSDLLib = FALSE;
    m_HookSDL2Lib = FALSE;
    m_ExtendSDLHook = FALSE;
    m_SDLEmulation = FALSE;
    m_SDLForceStretch = FALSE;
    m_HookSmackW32 = FALSE;
    m_HookBinkW32 = FALSE;
    m_FixSmackLoop = FALSE;
    m_BlockPriorityClass = FALSE;
    m_EASportsHack = FALSE;
    m_LegacyAlloc = FALSE;
    m_DisableMaxWinMode = FALSE;
    m_AltTabMode = 0;
    m_NoImagehlp = FALSE;
    m_ReplacePrivOps = FALSE;
    m_ForcesHEL = FALSE;
    m_ForcesHAL = FALSE;
    m_ForcesNULL = FALSE;
    m_MinimalCaps = FALSE;
    m_SetZBufferBitDepths = FALSE;
    m_ForcesSwapEffect = FALSE;
    m_ColorFix = FALSE;
    m_FixGlobalUnlock = FALSE;
    m_FixFreeLibrary = FALSE;
    m_SkipFreeLibrary = FALSE;
    m_LoadLibraryErr = FALSE;
    m_FixAlteredPath = FALSE;
    m_FixAdjustWinRect = FALSE;
    m_NoPixelFormat = FALSE;
    m_NoAlphaChannel = FALSE;
    m_FixRefCounter = TRUE; // default true !!
    m_ReturnNullRef = FALSE;
    m_NoD3DReset = FALSE;
    m_HideDesktop = FALSE;
    m_HideTaskbar = FALSE;
    m_NoTaskbarOverlap = FALSE;
    m_SetDPIAware = FALSE;
    m_ActivateApp = FALSE;
    m_D3DResolutionHack = FALSE;
    m_FixAILSoundLocks = FALSE;
    m_NoDestroyWindow = FALSE;
    m_LockSysColors = FALSE;
    m_LockReservedPalette = FALSE;
    m_LimitScreenRes = FALSE;
    m_SingleProcAffinity = FALSE;
    m_UseLastCore = FALSE;
    m_LimitResources = FALSE;
    m_CDROMDriveType = FALSE;
    m_HideCDROMEmpty = FALSE;
    m_FakeHD = FALSE;
    m_FakeCD = FALSE;
    m_FakeHDDrive = "C:";
    m_FakeCDDrive = "D:";
    m_HookGOGLibs = FALSE;
    m_BypassGOGLibs = FALSE;
    m_FontBypass = FALSE;
    m_BufferedIOFix = FALSE;
    m_ZBufferClean = FALSE;
    m_ZBuffer0Clean = FALSE;
    m_DynamicZClean = FALSE;
    m_ZBufferHardClean = FALSE;
    m_ZBufferAlways = FALSE;
    m_HotPatchAlways = FALSE;
    m_FreezeInjectedSon = FALSE;
    m_StressResources = FALSE;
    m_Experimental = FALSE;
    m_FixRandomPalette = FALSE;
    m_DisableFogging = FALSE;
    m_Power2Width = FALSE;
    m_ClearTarget = FALSE;
    m_FixPitch = FALSE,
    m_NoPower2Fix = FALSE;
    m_NoPerfCounter = FALSE;
    m_UnNotify = FALSE;
    m_Windowize = TRUE; // default true !!
    m_HotPatch = FALSE;
    m_HookDLLs = TRUE; // default true !!
    //m_AnsiWide = FALSE;
    m_HookNoRun = FALSE;
    m_CopyNoShims = FALSE;
    m_HotRegistry = FALSE;
    m_HookNoUpdate = FALSE;
    m_TerminateOnClose = FALSE;
    m_ConfirmOnClose = FALSE;
    m_HookEnabled = TRUE; // default true !!
    //m_SetCmdLine = FALSE;
    m_NeedAdminCaps = FALSE;
    m_UseShortPath = FALSE;
    m_EmulateRegistry = FALSE;
    m_OverrideRegistry = FALSE;
    m_Wow64Registry = FALSE;
    m_Wow32Registry = FALSE;
    m_FullScreenOnly = FALSE;
    m_ShowHints = FALSE;
    m_Frontend = FALSE;
    m_BackgroundPriority = FALSE;
    m_PeekAllMessages = FALSE;
    m_NoWinPosChanges = FALSE;
    m_MessagePump = FALSE;
    m_ClipMenu = FALSE;
    m_NoMouseEvents = FALSE;
    m_FixMouseRawInput = FALSE;
    m_MouseShield = FALSE;
    m_AdaptMouseSpeed = FALSE;
    m_FixMouseLParam = FALSE;
    m_SwallowMouseMove = FALSE;
    m_UnnotifyInactive = FALSE;
    m_MouseMoveByEvent = FALSE;
    m_NoBanner = FALSE;
    m_FilePath = _T("");
    m_Module = _T("");
    m_CmdLine = _T("");
    m_SlowDown = FALSE;
    m_BlitFromBackBuffer = FALSE;
    m_NoFlipEmulation = FALSE;
    m_OffscreenZBuffer = FALSE;
    m_NoZBufferAttach = FALSE;
    m_TexturePalette = FALSE;
    m_LockColorDepth = FALSE;
    m_Lock24BitDepth = FALSE;
    m_FullPaintRect = FALSE;
    m_PushActiveMovie = FALSE;
    m_ForceClipChildren = FALSE;
    m_PreventMinimize = FALSE;
    m_NoAccessibility = FALSE;
    m_IgnoreDebOutput = FALSE;
    m_NoOleInitialize = FALSE;
    m_ChaosOverlordsFix = FALSE;
    m_FixFolderPaths = FALSE;
    m_NoComplexMipmaps = FALSE;
    m_InvalidateClient = FALSE;
    m_CreateDCHook = FALSE;
    m_SafePrimLock = FALSE;
    m_SmackBufferNoDepth = FALSE;
    m_CustomLocale = FALSE;
    m_ClassLocale = FALSE;
    m_LockSysSettings = FALSE;
    m_ProjectBuffer = FALSE;
    m_ForceRelAxis = FALSE;
    m_ForceAbsAxis = FALSE;
    m_DirectXReplace = FALSE;
    m_W98OpaqueFont = FALSE;
    m_FakeGlobalAtom = FALSE;
    m_RevertDIBPalette = FALSE;
    m_FixDCAlreadyCreated = FALSE;
    m_SuppressMenus = FALSE;
    m_KillDeadlocks = FALSE;
    m_CommitPage = FALSE;
    m_D3D8MaxWinMode = FALSE;
    m_Mutex4CritSection = FALSE;
    m_DelayCritSection = FALSE;
    m_RemapNumKeypad = FALSE;
    m_SetUSKeyDescr = FALSE;
    m_DisableGammaRamp = FALSE;
    m_PALDIBEmulation = FALSE;
    m_RefreshOnRealize = TRUE;
    m_LoadGammaRamp = FALSE;
    m_AutoRefresh = FALSE;
    m_IndependentRefresh = FALSE;
    m_TextureFormat = FALSE;
    m_VideoToSystemMem = FALSE;
    m_FixTextOut = FALSE;
    m_ShrinkFontWidth = FALSE;
    m_SharedDC = FALSE;
    m_HookGlide = FALSE;
    m_SuppressGlide = FALSE;
    m_RemapMCI = TRUE;
    m_NoMovies = FALSE;
    m_FixMoviesColor = FALSE;
    m_StretchMovies = FALSE;
    m_BypassMCI = FALSE;
    m_FixPCMAudio = FALSE;
    m_SuspendTimeStretch = FALSE;
    m_SuppressRelease = FALSE;
    m_KeepCursorWithin = FALSE;
    m_KeepCursorFixed = FALSE;
    m_UseRGB565 = TRUE; // seems the default for 16bit video mode
    m_SuppressDXErrors = FALSE;
    m_FlipEmulation = TRUE; // better set true default, since this mode is more compatible
    m_MarkBlit = FALSE;
    m_MarkLock = FALSE;
    m_MarkWinG32 = FALSE;
    m_MarkGDI32 = FALSE;
    m_DumpDIBSection = FALSE;
    m_CaptureScreens = FALSE;
    m_PatchExecuteBuffer = FALSE;
    m_ForceZBufferOn = FALSE;
    m_ForceZBufferOff = FALSE;
    m_DumpDevContext = FALSE;
    m_DumpCPUID = FALSE;
    m_NoSysMemPrimary = FALSE;
    m_NoSysMemBackBuf = FALSE;
    m_DumpSurfaces = FALSE;
    m_DumpBlitSrc = FALSE;
    m_DumpBitmaps = FALSE;
    m_BilinearBlt = FALSE;
    m_FastBlt = FALSE;
    //m_GDIColorConv = FALSE;
    m_PreventMaximize = FALSE;
    m_EmulateMaximize = TRUE; // default true !!
    m_ClientRemapping = TRUE; // default true !!
    m_LockWinStyle = FALSE;
    m_DisableDWM = FALSE;
    m_FixParentWin = FALSE;
    m_KeepAspectRatio = FALSE;
    m_AdaptiveRatio = FALSE;
    m_ForceWinResize = FALSE;
    m_HideMultiMonitor = FALSE;
    m_FixD3DFrame = FALSE;
    m_NoWindowMove = FALSE;
    m_HookChildWin = FALSE;
    m_HookDlgWin = FALSE;
    m_MessageProc = FALSE;
    m_FixMouseHook = FALSE;
    m_FixMessageHook = FALSE;
    m_FixNCHITTEST = FALSE;
    m_RecoverScreenMode = FALSE;
    m_RefreshOnResize = FALSE;
    m_Init8BPP = FALSE;
    m_Init16BPP = FALSE;
    m_BackBufAttach = FALSE;
    m_ClearTextureFourCC = FALSE;
    m_SuppressFourCCBlt = FALSE;
    m_NoDDExclusiveMode = FALSE;
    m_LockFullscreenCoop = FALSE;
    m_CreateDesktop = FALSE;
    m_SafePaletteUsage = FALSE;
    m_AllowSysmemOn3DDev = FALSE;
    m_VSyncMode = 0;
    m_VSyncImpl = 0;
    m_WaitMode = 0;
    m_HandleAltF4 = FALSE;
    m_LimitFPS = FALSE;
    m_LimitDIBOperations = FALSE;
    m_LimitBeginScene = FALSE;
    m_LimitFlipOnly = FALSE;
    m_SkipFPS = FALSE;
    m_ShowFPS = 0; // default: no FPS
    m_ShowTimeStretch = FALSE;
    m_TimeStretch = FALSE;
    m_StretchTimers = FALSE;
    m_NormalizePerfCount = FALSE;
    m_StretchPerFrequency = FALSE;
    m_SlowWinPolling = FALSE;
    m_CPUSlowDown = FALSE;
    m_CPUMaxUsage = FALSE;
    m_PreciseTiming = FALSE;
    m_KillVSync = FALSE;
    m_QuarterBlt = FALSE;
    m_MakeWinVisible = FALSE;
    m_FixEmpireOFS = FALSE;
    m_KillBlackWin = FALSE;
    m_ZeroDisplayCounter = FALSE;
    m_InvertMouseXAxis = FALSE;
    m_InvertMouseYAxis = FALSE;
    m_ReplaceDialogs = FALSE;
    m_HandleFourCC = FALSE;
    m_CacheD3DSession = FALSE;
    m_FineTiming = FALSE;
    m_EnableTimeFreeze = FALSE;
    m_ReleaseMouse = FALSE;
    m_EnableHotKeys = TRUE; // default true !!
    m_NoDisablePrint = FALSE;
    m_FixAsyncKeyState = FALSE;
    m_InterceptRDTSC = FALSE;
    m_HookOpenGL = FALSE;
    m_ForceHookOpenGL = FALSE;
    m_FixPixelZoom = FALSE;
    m_FixBindTexture = FALSE;
    m_HookGlut32 = FALSE;
    m_HookWGLContext = TRUE;
    m_ScaleMainViewport = TRUE;
    m_LockGLViewport = FALSE;
    m_GLExtensionsLie = FALSE;
    m_GLFixClamp = FALSE;
    m_ScaleglBitmaps = FALSE;
    m_FakeVersion = FALSE;
    m_FullRectBlt = FALSE;
    m_CenterToWin = FALSE;
    m_LimitDdraw = FALSE;
    m_SuppressOverlay = FALSE;
    m_CapMask = FALSE;
    m_NoWindowHooks = FALSE;
    m_DisableWinHooks = FALSE;
    m_HideWindowChanges = FALSE; // ??
    m_SkipIATHint = FALSE;
    m_NoDDRAWBlt = FALSE;
    m_NoDDRAWFlip = FALSE;
    m_NoGDIBlt = FALSE;
    m_NoFillRect = FALSE;
    m_FixClipperArea = FALSE; // ??
    m_SharedDCHybrid = FALSE; // ??
    m_SyncPalette = FALSE;
    m_NoWinErrors = FALSE;
    m_NoDialogs = FALSE;
    m_InvalidateFullRect = FALSE;
    m_NoSetPixelFormat = FALSE;
    m_ScaleCBTHook = FALSE;
    m_StretchDialogs = FALSE;
    m_PretendVisible = FALSE;
    m_WinInsulation = FALSE;
    m_DisableMMX = FALSE;
    m_SafeAllocs = FALSE; // maybe better set to true ???
    m_QualityFonts = FALSE;
    m_NoPaletteUpdate = FALSE;
    m_WireFrame = FALSE;
    m_NoTextures = FALSE;
    m_BlackWhite = FALSE;
    m_AssertDialog = FALSE;
    m_StartWithToggle = FALSE;
    m_InitialRes = FALSE;
    m_MaximumRes = FALSE;
    m_CustomRes = FALSE;
    m_RemapSysFolders = FALSE;
    m_ClipperMode = 0;
    m_VertexProcessing = 0;
    m_FourCCMode = 0;
    m_PosX = 50;
    m_PosY = 50;
    m_SizX = 800;
    m_SizY = 600;
    m_InitResW = 800;
    m_InitResH = 600;
    m_MaxFPS = 0;
    m_InitTS = 8;
    m_SwapEffect = 0;
    m_InjectionMode = 0;
    m_WinMovementType = 1; // "Floating" position mode, better than "Free"!
    m_ScanLine = 0;
    //m_WindowStyle = 0; // v2.04.43: added forgotten initialization
    m_WindowStyle = 2; // v2.05.20: default to bordered window
    m_Icon = NULL;
    m_LockFPSCorner = FALSE;
    m_SetZBuffer16Bit = FALSE;
    m_SetZBuffer24Bit = FALSE;
    m_Country = 0;
    m_Language = 0;
    m_CodePage = 0;
    m_CommitAddress = 0;
    m_CommitLength = 0;
    // debug
    m_DisableCPUID = FALSE;
    m_CPUDisableMMX = FALSE;
    m_CPUDisableSSE = FALSE;
    m_CPUDisableSSE2 = FALSE;
    m_CPUDisablePBE = FALSE;
    m_CPUVendorId = 0;
    //}}AFX_DATA_INIT
}

BOOL CTargetDlg::OnInitDialog() {
    // BEWARE: this part MUST be syncronized with TabCtrl initialization in dxTabCtrl.cpp !!!
    int i = 0;
    AfxEnableControlContainer();
    CDialog::OnInitDialog();
    // v2.04.74: show item order in window caption, useful for /R:n command line option
    if(m_ItemIndex > 0) {
        char sNumberedCaption[21];
        sprintf(sNumberedCaption, "Target #%d  ", m_ItemIndex);
        this->SetWindowTextA(sNumberedCaption);
    } else
        this->SetWindowTextA("New");
    char sCaption[48 + 1];
    LoadString(AfxGetResourceHandle(), DXW_TAB_MAIN, sCaption, sizeof(sCaption));
    m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_HOOK, sCaption, sizeof(sCaption));
    if (gbExpertMode) m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_VIDEO, sCaption, sizeof(sCaption));
    m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_MOUSE, sCaption, sizeof(sCaption));
    m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_INPUT, sCaption, sizeof(sCaption));
    if (gbExpertMode) m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_MESSAGES, sCaption, sizeof(sCaption));
    if (gbExpertMode) m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_DIRECTX, sCaption, sizeof(sCaption));
    m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_DIRECTX2, sCaption, sizeof(sCaption));
    m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_D3D, sCaption, sizeof(sCaption));
    m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_TIMING, sCaption, sizeof(sCaption));
    if (gbExpertMode) m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_LOGS, sCaption, sizeof(sCaption));
    if (gbExpertMode & (GetDxWndCaps() & DXWCAPS_CANLOG)) m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_LIBS, sCaption, sizeof(sCaption));
    m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_COMPAT, sCaption, sizeof(sCaption));
    if (gbExpertMode) m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_REGISTRY, sCaption, sizeof(sCaption));
    if (gbExpertMode) m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_NOTES, sCaption, sizeof(sCaption));
    m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_OPENGL, sCaption, sizeof(sCaption));
    if (gbExpertMode) m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_SDL, sCaption, sizeof(sCaption));
    if (gbExpertMode) m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_SOUND, sCaption, sizeof(sCaption));
    if (gbExpertMode) m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_TWEAKS, sCaption, sizeof(sCaption));
    if (gbExpertMode) m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_IO, sCaption, sizeof(sCaption));
    if (gbExpertMode) m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_DEBUG, sCaption, sizeof(sCaption));
    if (gbDebug) m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_HW, sCaption, sizeof(sCaption));
    if (gbDebug) m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    LoadString(AfxGetResourceHandle(), DXW_TAB_LOCALE, sCaption, sizeof(sCaption));
    if (gbExpertMode) m_tabdxTabCtrl.InsertItem(i++, _T(sCaption));
    m_tabdxTabCtrl.Init();
    return TRUE;
}

void CTargetDlg::OnOK() {
    m_tabdxTabCtrl.OnOK();
    CDialog::OnOK();
}

void CTargetDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTargetDlg)
    DDX_Control(pDX, IDC_TABPANEL, m_tabdxTabCtrl);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTargetDlg, CDialog)
    //{{AFX_MSG_MAP(CTargetDlg)
    //}}AFX_MSG_MAP

    ON_BN_CLICKED(IDTRY, &CTargetDlg::OnBnClickedTry)
    ON_BN_CLICKED(IDKILL, &CTargetDlg::OnBnClickedKill)
    ON_BN_CLICKED(IDCONTEXTHELP, &CTargetDlg::OnBnClickedHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTargetDlg Message Handler

void CTargetDlg::OnBnClickedTry() {
    char path[MAX_PATH + 1];
    char fullpath[MAX_PATH + 1];
    int iHookStatus;
    TARGETMAP RestrictedMaps[2];
    extern void SetTargetFromDlg(TARGETMAP *, CTargetDlg *);
    extern BOOL CheckStatus(void);
    STARTUPINFO sinfo;
    PROCESS_INFORMATION pinfo;
    char sMsg[81];
    TARGETMAP TargetMap;
    PRIVATEMAP PrivateMap;
    // ThreadInfo must be static type to survive in the debug loop ...
    static ThreadInfo_Type ThreadInfo;
    if(m_FilePath[0] == '?') {
        int choice = MessageBox("Incomplete entry\nplease, update the path field before running",
                                "DxWnd", MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    static BOOL IsLocked = FALSE;
    if(IsLocked) return;
    if (CheckStatus()) return; // don't try when status is active
    IsLocked = TRUE;
    m_tabdxTabCtrl.OnOK();
    SetTargetFromDlg(&RestrictedMaps[0], this);
    memset(&RestrictedMaps[1], 0, sizeof(TARGETMAP));
    strcpy_s(path, sizeof(path), m_FilePath.GetBuffer());
    PathRemoveFileSpec(path);
    SetTarget(NULL, RestrictedMaps);
    iHookStatus = GetHookStatus(NULL);
    if(iHookStatus == DXW_IDLE) StartHook();
    switch(m_InjectionMode) {
    case 1:
        extern DWORD WINAPI InjectDebug(void *);
        strcpy(TargetMap.path, m_FilePath);
        strcpy(PrivateMap.launchpath, m_LaunchPath);
        strcpy(PrivateMap.FakeHDPath, m_FakeHDPath);
        strcpy(PrivateMap.FakeCDPath, m_FakeCDPath);
        strcpy(PrivateMap.FakeCDLabel, m_FakeCDLabel);
        memcpy((void *)&ThreadInfo.TM, (void *)&TargetMap, sizeof(TARGETMAP));
        memcpy((void *)&ThreadInfo.PM, (void *)&PrivateMap, sizeof(PRIVATEMAP));
        strcpy_s(ThreadInfo.ExePath, MAX_PATH, m_FilePath);
        CloseHandle(CreateThread( NULL, 0, InjectDebug, &ThreadInfo, 0, NULL));
        break;
    case 2:
        extern void InjectSuspended(char *, char *, BOOL, BOOL, TARGETMAP *, PRIVATEMAP *);
        strcpy(TargetMap.path, m_FilePath);
        strcpy(PrivateMap.launchpath, m_LaunchPath);
        strcpy(PrivateMap.FakeHDPath, m_FakeHDPath);
        strcpy(PrivateMap.FakeCDPath, m_FakeCDPath);
        strcpy(PrivateMap.FakeCDLabel, m_FakeCDLabel);
        memcpy((void *)&ThreadInfo.TM, (void *)&TargetMap, sizeof(TARGETMAP));
        memcpy((void *)&ThreadInfo.PM, (void *)&PrivateMap, sizeof(PRIVATEMAP));
        InjectSuspended((strlen(PrivateMap.launchpath) > 0) ? PrivateMap.launchpath : TargetMap.path,
                        path, FALSE, TargetMap.flags12 & COMMITPAGE, &TargetMap, &PrivateMap);
        break;
    case 3:
        //extern void InjectProxy(char *, char *, BOOL, BOOL, TARGETMAP *, PRIVATEMAP *);
        //strcpy(TargetMap.path, m_FilePath);
        //strcpy(PrivateMap.launchpath, m_LaunchPath);
        //strcpy(PrivateMap.FakeHDPath, m_FakeHDPath);
        //strcpy(PrivateMap.FakeCDPath, m_FakeCDPath);
        //strcpy(PrivateMap.FakeCDLabel, m_FakeCDLabel);
        //memcpy((void *)&ThreadInfo.TM, (void *)&TargetMap, sizeof(TARGETMAP));
        //memcpy((void *)&ThreadInfo.PM, (void *)&PrivateMap, sizeof(PRIVATEMAP));
        //InjectProxy((strlen(PrivateMap.launchpath)>0) ? PrivateMap.launchpath: TargetMap.path,
        //	path, FALSE, TargetMap.flags12 & COMMITPAGE, &TargetMap, &PrivateMap);
        strcpy(TargetMap.path, m_FilePath);
        strcpy(PrivateMap.launchpath, m_LaunchPath);
        strcpy(PrivateMap.FakeHDPath, m_FakeHDPath);
        strcpy(PrivateMap.FakeCDPath, m_FakeCDPath);
        strcpy(PrivateMap.FakeCDLabel, m_FakeCDLabel);
        memcpy((void *)&ThreadInfo.TM, (void *)&TargetMap, sizeof(TARGETMAP));
        memcpy((void *)&ThreadInfo.PM, (void *)&PrivateMap, sizeof(PRIVATEMAP));
        strcpy_s(ThreadInfo.ExePath, MAX_PATH, m_FilePath);
        CloseHandle(CreateThread( NULL, 0, InjectProxy, &ThreadInfo, 0, NULL));
        break;
    case 0:
    default:
        ZeroMemory(&sinfo, sizeof(sinfo));
        sinfo.cb = sizeof(sinfo);
        strncpy(fullpath, m_LaunchPath.IsEmpty() ? m_FilePath.GetBuffer() : m_LaunchPath.GetBuffer(), MAX_PATH);
        if(!CreateProcess(NULL, fullpath,
                          0, 0, false, CREATE_DEFAULT_ERROR_MODE, NULL, path, &sinfo, &pinfo)) {
            sprintf(sMsg, "CreateProcess ERROR %d", GetLastError());
            MessageBox(sMsg, "Error", MB_ICONEXCLAMATION);
        }
        CloseHandle(pinfo.hProcess); // no longer needed, avoid handle leakage
        CloseHandle(pinfo.hThread); // no longer needed, avoid handle leakage
        break;
    }
    // wait & recover
    Sleep(5000);
    //SetTarget(CDxwndhostView::TargetMaps);
    if(iHookStatus == DXW_IDLE) EndHook();
    IsLocked = FALSE;
}

void CTargetDlg::OnBnClickedKill() {
    char FilePath[MAX_PATH + 1];
    char *lpProcName, *lpNext;
    HRESULT res;
    strncpy(FilePath, m_FilePath.GetBuffer(), MAX_PATH);
    lpProcName = FilePath;
    while (lpNext = strchr(lpProcName, '\\')) lpProcName = lpNext + 1;
    if(!KillProcByName(lpProcName, FALSE, FALSE)) {
        wchar_t *wcstring = new wchar_t[48 + 1];
        mbstowcs_s(NULL, wcstring, 48, lpProcName, _TRUNCATE);
        res = MessageBoxLangArg(
                  DXW_STRING_KILLTASK,
                  DXW_STRING_WARNING,
                  MB_YESNO | MB_ICONQUESTION | MB_TOPMOST,
                  wcstring);
        if(res != IDYES) return;
        KillProcByName(lpProcName, TRUE, FALSE);
    } else
        MessageBoxLang(DXW_STRING_NOKILLTASK, DXW_STRING_INFO, MB_ICONEXCLAMATION);
}

void CTargetDlg::OnBnClickedHelp() {
    extern void ShowHelp(char *);
    ShowHelp(m_tabdxTabCtrl.m_tabHelpers[m_tabdxTabCtrl.m_tabCurrent]);
}
