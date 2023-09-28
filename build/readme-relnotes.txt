v2.05.54
fix: [advapi.cpp] added RegEnumKey/Ex wrappers the handling of virtual registry
fix: [advapi.cpp] fixed bug in query virtual registry value when lpcbData is NULL
fix: [advapi.cpp] ignore leading blanks in hex continuation lines of virtual registry
fix: [advapi.cpp] added GetUserNameA wrapper
fix: [mciwrapper.cpp] fixed mciSendString parsing of play command
fix: [mciplayer.cpp mcplayer.h mciwrapper.cpp] handling of different notifications for PLAY/STOP/PAUSE/RESUME commands
fix: [mciwrapper.cpp] wrapper for mciSendCommandW call
fix: [mciwrapper.cpp] mciSendCommand MCI_INFO returning non static informations for identity and product fields
fix: [mciwrapper.cpp] mciSendString path translation for virtual CD/HD in case of double-quoted pathname
fix: [msvfw.cpp] logging of flags captions
add: [opengl.cpp] added GLFIXCLAMP flag. Fixes "Bugdom" textures
add: [kernel32.cpp] ifdefed conditional tracing of thread creations
fix: [kernel32.cpp] proper check for fake CD/HD drive in GetDiskFreeSpace wrapper

v2.05.53
fix: [ddraw.cpp gdi32.cpp] revised palette handling to manage reserved palette entries. Fixes "Baku Baku" etc.
fix: [ddraw.cpp] added setting of color depth in status panel for FixPixelFormat routine
fix: [kernel32.cpp] fixed dxwTranslatePathW routine. Fixes "Jack Carlton's Soccer Nation" file loading with relative paths
fix: [ddraw.cpp] fix IDirectDrawSurface::SetEntries wrapper trimming dwcout > 256. Useful for "Jack Carlton's Soccer Nation" palette.
add: [ddraw.cpp gdi32.cpp] added logging of actual palette when actual color depth is 8
fix: [dsound.cpp] fix in CreateBuffer wrapper to trim unsupported capabilities. Fixes "KA52 Team Alligator" missing sound.
fix: [dsound.cpp] complete #ifdef-ed wrapping of all methods for both primary and secondary buffers.
add: [GUI] added handling of program command line, deleted SETCMDLINE flag.
fix: [GUI] in "Debugger mode" start, now DxWnd debugger ignores exceptions for the first 5 seconds from program start. 
fix: [advapi.cpp] implemented "path" and "work" tokens made different. Useful for "Jack Carlton's Soccer Nation" configuration.

v2.05.52
fix: [kernel32.cpp] fixed missing function pointers initialization
fix: [avifile.cpp] fixed AVIStreamReadFormat wrapper to handle *lpcbFormat==-1. Fixes "Wings of Destiny".
fix: [ddraw.cpp] fixed Set/GetClipList wrappers to make proper regions scaling
fix: [ddraw.cpp] emulated ZBUFFER detach in DeleteAttachedSurface wrapper. Fixes "Jack Charlton's Soccer Nation" termination procedure
fix: [ddraw.cpp] deleted reference counter in DeleteAttachedSurface wrapper detach of backbuffer surface (to be tested)
fix: [ddraw.cpp] fixed SetClipper wrapper for backbuffer clipper set with HWND reference. Fixes "Caponata".
add: [ddraw.cpp] added MAXCLIPPER option. Fixes "Full Pipe".
add: [winmm.cpp] more detailed log in mixerSetControlDetails wrapper
add: [InjectDebug.cpp] trace of debug messages in debug start mode
add: [InjectDebug.cpp] timestamp of log messages for debug start mode
add: [InjectDebug.cpp] fixed InjectDebug for debug start mode
add: [dxwndhost.cpp] added dialog to advertise DxWnd help pages

v2.05.51
fix: [user32.cpp] fixed SUPPRESSMENUS tweak, now working with "Bust-a-Move 4" and other fames
fix: [kernel32.cpp] added wrappers for CreateDirectoryA/RemoveDirectoryA
fix: [kernel32.cpp] when dynamically loading an executable file, bypass hooking. Fixes "State of War 2".
fix: [kernel32.cpp] fixed GetVolumeInformationA wrapper in case of NULL path. Fixes "Diggles".
fix: [hooktable.cpp] added DXWHOOK_EXTEND flag to msvfw32.dll to extend GDI stretching. Fixes "Robotron X" movies (and many others, probably)
add: [mciwrapper.cpp] added SUPPRESSCDAUDIO flag
add: [mciwrapper.cpp] added RESERVEMCIDEVICE flag
fix: [GUI] fixed some clipped text strings
fix: [GUI] added warning message when trying to run a second task in single-hook mode

v2.05.50
fix: [kernel32.cpp] fixed logic of fake CD/HD in GetDriveTypeA/W, SetCurrentDirectoryA/W wrappers
fix: [kernel32.cpp] eliminated the "." and ".." files from root fake device in FindFirstFileA wrapper
fix: [kernel32.cpp] fixed file attributes in FindFirstFileA / FindNextFileA wrappers when listing files from fake CD folder
fix: [winmm.cpp] fixed routing of MCI messages to CD audio emulation, now based on the effective virtual device ID opened by MCI_OPEN and until MCI_CLOSE. Fixes "Gooch Grundy's X-Decathlon".
fix: [kernel32.cpp] GetVolumeInformationA wrapper now returns information with precedence for fake drives so that you can define a fake drive also over an existing drive
fix: [user32.cpp] fixed GetUpdateRect wrapper to handle possibly NULL lpRect value. Fixes Silent Storm 2 & 3
fix: [GUI] fixed flags without initialized value
fix: [GUI] added MB_SYSTEMMODAL | MB_TOPMOST to kill process messagebox to make sure it is always visible and clickable

v2.05.49
fix: [kernel32.cpp] fixed bad test for INJECTSON flag.
fix: [hdxgi.cpp] fixed prototype of CreateDXGIFactory2 method
fix: [hdxgi.cpp] fixed DXGISetFullscreenState wrapper
fix: [sdl.cpp] fixed SDL_PeepEvents wrappers that have different prototypes in SDL and SDL2
add: [dwmapi.cpp] added trace hooks for a few dwmapi calls
add: [dxhelper.cpp] added some IDXGI GUID for tracing
add: [opengl.cpp] added ifdef-ed trace logs for several calls
fix: [GUI] gui dxwnd.log file renamed as dxwnd.gui.log
add: [GUI] added run with autolog mode for non-expert users

v2.05.48
fix: [GUI] set hscroll to LaunchPath field to allow up to MAX_PATH field length
fix: [ddraw.cpp queryint.cpp] added late Release on backbuffer surfaces. Fixes resource leakage in "Goman"
fix: [dxwcore.cpp] fixed (again) dxwCore::AdjustWindowFrame to propagate clipping style flags. Fixes "C&C Red Alert 2"
fix: [oglblt.cpp] revides condition for ogl context rebuild. Reduces leakage on "Dungeon Keeper 2".
add: [user32.cpp] added DISABLEWINHOOKS debug flag
add: [hdsetup.cpp] added wrapper for dsetup.dll calls. Used for "Starfleet Command III"
add: [advapi.cpp] added wrapper for RegSetValueA call. Used in "Loony Labyrinth Pinball"
add: [ddraw.cpp] added tracing wrappers for RestoreAllSurfaces and GetDeviceIdentifier methods
add: [ddraw.cpp] added tracing log messages for WaitForVerticalBlank method
add: [hd3d7.cpp] added tracing wrappers for Get/SetTextureStageState methods
add: [hd3d7.cpp] added tracing wrappers for AddrRef/Release/QueryInterface texture methods

v2.05.47
fix: [user32.cpp] on window mode, RegisterRawInputDevices wrapper suppresses RIDEV_CAPTUREMOUSE flag 
fix: [user32.cpp] added handling of WM_ACTIVATE WM_NCACTIVATE messages in DefWindowProc wrappers. Fixes the focus detection in "Screamer 4x4" game.
fix: [hd3d.cpp] fixes Adapter value in IDirect3D8/9 GetAdapterCount wrappers to reflect the same adapter used in other calls like EnumAdapterModes. Fixes "Scallywag" (and likely others ...) in multi-monitor configurations.
fix: [dxhook.cpp dxwcore.cpp ...] moved GetDeviceGammaRamp call outside form dxwnd.dll DLLMain() call. Fixes hangs of GUI and crashes on Win10 after build 18362.53 (see https://answers.microsoft.com/en-us/insider/forum/all/getdevicegammaramp-api-hangs-in-windows-10-insider/6a5d6719-d2b9-4666-aad7-c94d58d6adeb?auth=1 )
fix: [mciwrapper.cpp] in mciSendString wrapper, handling of "open path ..." command strings in case the path refers to a fake device. Fixes movie play in "Goman" with fake CD device.

v2.05.46
fix: [oglblt.cpp] fixed opengl blitter on window DC invalidation. Fixes "Re-Volt" with OpenGL renderer.
fix: [KillProc.cpp] expanded GUI scans of process list from 250 to 2000 processes maximum. Fixes /r:n option in computers with more than 250 processes.
fix: [dxwcore.cpp] fixed bug in INVERTMOUSEYAXIS option. Fixes "Sherlock Holmes - The Mystery of the Mummy"
add: [dxhook.cpp kernel32.cpp] added SUPPRESSGLIDE flag
add: [GUI] saving a backup file dxwnd.bak before saving changes to dxwnd.ini

v2.05.45
add: [GUI] lock flag in virtual joystick panel. Enables / disables the use of middle mouse button to lock the virtual joystick.
fix: [dinput.cpp comvjoy.cpp] rewrite of dinput interface for virtual joystick. Fixes "The Hulk"
add: [ddraw.cpp] DUMPBLITSRC debug flag
fix: [hd3d7.cpp] disabled HOT patching on Direct3DCreate and Direct3DCreateDevice API that are called also internally by D3D COM methods with stack corruption. Fixes "WhiteWater Rapids" in D3D 16bit color mode through hot patching.

v2.05.44
add: [ExpFinder.cpp] /p option for existing path matching report
fix: [sdl.cpp] preliminary overlay for sdl programs
fix: [dxwcore.cpp] deleted unused ANSIWIDE flag
fix: [hd3d7.cpp] EnumTextureFormats wrappers
fix: [sdl.cpp] Log buffer overflow. Fixes "Blob Wars" crash.
fix: [kernel32.cpp] fixed crash when GetDriveType argument is NULL. Fixes "Leadfoot Stadium Off-Road Racing" crash with fake CD.
add: [kernel32.cpp] added GetDriveTypeW wrapper.
fix: [ole32.cpp] fixed HookOLEClass to avoid possible mis-hooking of known libs. Fixes "Hulk".
fix: [comvjoy.cpp] added dinput8 DIPROP_JOYSTICKID handling to GetProperty wrapper. Found in "Hulk".
add: [winmm.cpp] wrapper for joySetCapure joyReleaseCapture calls. Fixes virtual joystick for "Agile Warrior F111X".
add: [dinput.cpp] fixed handling of virtual joystick in IDirectInputDevice8::GetDeviceStatus wrapper
add: [GUI] preliminary configuration for overlay features

v2.05.43
fix: [dxwcore.cpp] fully revised handling of FPS and time stretching overlays
fix: [ddraw.cpp] fix for DirectDrawEnumerate* callbacks to allow matching for default monitor with GUID==NULL.
fix: [oglblt.cpp] opengl blitter resets the ddraw clipper. Fixes partial screen display for "Age of Empires".
fix: [oglblt.cpp] better criteria for glContext renewal. Fixes "A.M.E.R.I.C.A No peace beyond the line" crashes.
add: [oglblt.cpp] GL_LINEAR filtering conditioned to BILINEARFILTER flag.
add: [oglblt.cpp] Blur effect, thanks to ddhack code.
fix: [oglblt.cpp] FPS/time overlay drawn on top of ddraw surface instead of window DC. Less flickering.
add: [kernel32.cpp] "patch:KillDeadlocks" tweak. Fixes "Decisive Battles of WWII: the Ardennes Offensive" game.
fix: [GUI] right-click auto-select the item below the cursor.
fix: [GUI] Status panel checking the proper item flags.
add: FPS counters now always measured and showed on status panel.
add: FPS logging.
add: [dxemublt.cpp] Deinterlace_8_to_32 filter. Deinterlaces "Wing Commander III" movies in 8bpp color depth.
fix: [dxwcore.cpp] new experimental FPS overlay with color inversion and quality font
fix: [GUI] stronger handling for kill hotkey
new: [GUI] tabbed global configuration panel
add: [GUI] "Hide on ESC key" global configuration flag

v2.05.42
fix: [dxwcore.cpp] unified and fixed implementation for GUID string conversion sGUID()
add: [ddraw.cpp hd3d.cpp hd3d7.cpp] handling of forced monitor id in ddraw and d3d1-9 sessions in fullscreen mode
fix: [ddrawex.cpp] fixed handling of IDIrectDrawFactory object. Fixes "Stratego"
fix: [ddrawex.cpp] added DirectDrawEx_Object GUID and DIERR_NOTBUFFERED error code
add: [dinput.cpp] more wrapped methods in TRACEALL build, better logging
add: [dinput.cpp] SUPPRESSDIERRORS flag now hides DIERR_UNSUPPORTED case in IDirectInputDevice::SetProperty
fix: [hd3d7.cpp] fixed some log messages in Createdevice wrappers
fix: [mciwrapper.cpp] handling of mciSendString tsmf secret hex format. Fixes "Disney's Hercules" CD sound emulation
fix: [mciwrapper.cpp] handling of mciSendString "status cdaudio ready" command. Fixes "Disney's Hercules" CD sound emulation
fix: [mciwrapper.cpp] handling of mciSendString plain "open cdaudio" command. Fixes "Fighting Force" CD sound emulation

v2.05.41
fix: [ddraw.cpp] now makes default "display" device visible by DirectDrawEnumerate* also with monitor filtering. Fixes "Battle Realms" on secondary monitor.
fix: [user32.cpp] fixed the fix in v2.05.40: AdjustWindowFrame replaced by dxw.HookwindowProc to hook the window procedure without altering the window position & style. Fixes "Jane's USAF" client window.
fix: [msvcrt.cpp] added wrapper for fopen call for fake CD/HD redirection. Fixes "Iron Storm" DxWnd RIP.
fix: [hd3d.cpp] some bad log messages in CreateAdditionalSwapChain & BeginStateBlock8 wrappers.
fix: [user32.cpp] added check for ATOM values in KILLBLACKWIN string compare. Added "Curtain" class to WIDECHAR black classes.
add: [winproc.cpp] ADAPTMOUSESPEED flag.
add: [kernel32.cpp] GetLogicalDriveStringsW wrapper (untested).

v2.05.40
fix: [ddraw.cpp] restored use of DDLOCK_NOSYSLOCK flag for ddraw surface Lock: required to avoid "Battle Realms" block.
fix: [user32.cpp] added missing AdjustWindowFrame call in CreateWindow* wrappers. Fixes "Loe Runner Online" and possibly other pure-GDI32 games.
fix: [dxwndhostView.cpp] added missing initialization of task index.
fix: [injectSuspended.cpp] in case of injection errors, the GUI now shows only the first error message and skips all following operations.
fix: [dxwnd.cpp] mutex lock now made in a separate and endless thread to avoid losing the DXW_RUNNING state
add: [users32.cpp] experimental SUPPRESSMENUS tweak to bypass the SetMenu call. 

v2.05.39
add: [binkw32.cpp] more #ifdef-ed tracing wrappers
add: [gdi32.cpp] more #idef-ed tracing wrappers and better tracing
add: [kernel32.cpp] added CreateFileW wrapper
add: [winmm.cpp] added mmioOpenW wrapper
add: [user32.cpp] added #idef-ed tracing wrappers for menu handling calls
fix: [dxhook.cpp] fixed window style settings for HD DPI scaling
fix: [ole32.cpp] fixed CoUnitialize wrapper log message
fix: [dxmapping.cpp, toggle_fs.cpp] using GetMonitorInfo original function pointer
fix: [user32.cpp] fixed several #ifdef-ed tracing messages
fix: [dxhook.cpp] fixed MESSAGEHOOK processing with repetition of the operation to intercept obfuscated programs. Needed for "Swords Online"
fix: [kernel32.cpp] enabled hot patching for GetProcAddress call. Needed for "Swords Online"
fix: [user32.cpp] enabled hot patching for GetMessagePos, GetMessageA/W, PeekMessageA/W calls. Needed for "Swords Online"

v2.05.38
fix: [dsound.cpp] fixed  Directsound::SetCooperativeLevel wrapper for "Championship Manager 2006".
fix: [dxhook.cpp] fixed hGet routine buffer initialization. Fixes a crash when compiled in debug mode.
fix: [hd3d.cpp] fixed D3D hook routine for "Hot patch" mode.
fix: [mciplayer.cpp] MCI_NOTIFY message sent on MCI_PAUSE/STOP events only when requested by MCI_NOTIFY
fix: [mciwrapper.cpp] mciSendString "play" and "seek" commands directed to CD emulation only for "cdaudio" device or alias. Fixes "play" command of midi files in "3-D Ultra Pinball Creep Nights".
fix: [user32.cpp] fixed args logging in GetRawInputDeviceInfo wrapper for RIDI_DEVICEINFO / RIM_TYPEMOUSE case.
mod: [bchunk.c] simplified bchunk.exe tool interface - see syntax helper.
add: [dxwndHostView.cpp] added incomplete path marker '?'
add: [gdi32.cpp] #ifdef-ed wrapper for debug tracing of GetObjectW, GetCurrentObject, GetWindowExtEx, SetWindowExtEx
fix: [gdi32.cpp] complete tracing of GetObjectA/W arguments
fix: [dxgi] tracing of all wrappers in D3D logging cathegory (renamed "OutTrace" to "OutTraceD3D").
fix: [kernel32] removed ".dll" suffix in absolute path transformation in LoadLibrary wrapper. Fixes "The Rage".
fix: [kernel32] don't try absolute path transformation on absolute paths in LoadLibrary wrapper

v2.05.37
add: [ole32.cpp] HookOLEClass routine to hook new dlls imported by CoCreateInstance calls
add: [helpmsgs.cpp] added multimedia MM_* macro translation for logging.
add: [mciwrapper.cpp] added hexdump log for mciSendMessage argument
fix: [mciplayer.cpp] added handling of MCI_NOTIFY request on MCI_PAUSE/STOP events. Fixes "Jimmy Whites 2 Cueball" CD tracks play.

v2.05.36
fix: [dxhook.cpp] reintroduced (in addition) old hook schema to catch unlinked ddraw/d3d objects.
fix: [dxhook.cpp winmm.cpp] eliminated need for "No zero aux device id" flag.
fix: [mciwrapper.cpp] fixed handling of MCI_SYSINFO message. Fixes CD sound emulation in "Interstate 76" and "Interstate 76 Nitro Pack".
fix: [hd3d.cpp] wrong API name "IDirect3DDevice8::ShowCursor" in logging d3d9 wrapper.
fix: [ole32.cpp] fixed GUID/RIID strings logging.
fix: [ole32.cpp] logging of decoded OLE error 0x80012 .
fix: GUI - changed "Sound" flags layout.
mod: [binkw32.cpp ddblip.cpp ddcreates.cpp ddraw.cpp kernel.cpp queryint.cpp version.cpp xinput.cpp] logging code review.

v2.05.35
fix: [dxhook.cpp kernel32.cpp] new recursive hooking schema. Should reach function calls wherever.
fix: [hooktable.cpp] added "KERNELBASE.dll" in bypassed dll list
fix: [dibdump.cpp] fixed bitmap format for DIB dumps of 16/32 bpp with BI_BITFIELDS compression. Found in "Aaron vs. Ruth".
fix: [gdi32.cpp] CreateFontA wrapper log message
fix: [ddrawex.cpp] fixed sGUID utility. Now prints the whole GUID string.
fix: [ole32.cpp] better hook targeting. Now works for VB6 games like "Xtreme Tankz Madness II"
add: dsound proxy
add: [ddraw.cpp] new ddraw:FixDCAlreadyCreated tweak to fix GetDC DDERR_DCALREADYCREATED error. Fixes "Spearhead" mission map screen bug.

v2.05.34
fix: [user32.cpp] fix BeginPaint wrapper rcRect scaling. Fixes "Railroad Tycoon II" partial blits 
fix: [gdi32.cpp] CreateICA/W wrappers fixed for tracing only. To be investigated.
add: [user32.cpp] conditional compilation for tracing of rect manipulation functions
add: [user32.cpp] hooking for SetWindowsHook CBT routine with SCALECBTHOOK flag. Fixes many "Spearhead" problems
add: [user32.cpp] added GetMessagePos wrapper with mouse scaling. Fixes unobfuscated versions of "Sword Online" client.
add: [dumphbmp.cpp] added DUMPBITMAP debug option

v2.05.33
fix: global code restyling for logging in WINMM, DINPUT, DSOUND, ADVAPI
fix: [mciwrapper.cpp] when paused, MCI_PLAY with no track specification is made equivalent to a MCI_RESUME. Fixes "O.D.T. Escape ... or Die Trying" CD sound resume
fix: [user32.cpp gdi32.cpp] complete rewrite of clipper handling routines. Fixes "688(I) Hunter Killer" and possibly much more
fix: [user32.cpp] deleted duplicate entry of ScrollDC wrapper causing recursion with "Hot Patch" hook. Fixes "688(I) Hunter Killer" sonar panel
fix: [advapi.cpp] fixed bug in SeekValueName internal routine that could cause crash in some conditions
fix: [gdi32.cpp user32.cpp] additions and fixes of regions handling. Now "688(I) Hunter Killer" is ok.
fix: help pages
add: [winmm.cpp] wrapper for sndPlaySound and PlaySound calls for virtual CD/HD path.
add: [dsound.cpp] new DSINITVOLUME flag. Fixes "O.D.T. Escape ... or Die Trying" movie sound volume
add: [dxwndhostView.cpp] "View -> log Tail" menu command
add: [dsound.cpp] complete tracing (in TRACEALL build) of IDIrectSound and IDirectSoundBuffer methods
add: opengl lier proxy to trim all OpenGL extension strings

v2.05.32
fix: [helpmsgs.cpp] added many missing message codes to ExplainWinMessage helper
fix: [gdi32.cpp] complete rewrite of regions handling
fix: [gdi32.cpp] code reorganization: logging with ApiRef tags, #ifdef'ed TRACEALL calls and more ...
fix: [user32] aligned GetUpdateRgn wrapper to new gdi region schema
fix: [dxhook.cpp] NOZEROAUXDEVID fix in fake CD audio initialization. Fixes "Airfix Dogfighter"
fix: [kernel32.cpp] fixed GetLogicalDriveStringsA wrapper causing possible exceptions. Fixes "Nox".
fix: [mciwrapper.cpp] fack CD returns default "fakecd". Fixes "Sentinel Returns" CD audio.
fix: [winmm.cpp] default device id for NOZEROAUXDEVID is now 0xBEEF again.
add: [MousePositionDialog.cpp] "Mouse position" panel now shows 4 different types of mouse position

v2.05.31
fix: [mciwrapper.cpp] fixed STRETCHMOVIE for mciSendCommand activated movies. Fixes "Three Dirty Dwarves" movie.
add: [InjectProxy.cpp] added INJECTPROXY mode, some code reorganization too. 
add: [MousePositionDialog.cpp] added "Mouse position" panel

v2.05.30
fix: [hdxgi.cpp] fixed window settings to DXGI swap chain
fix: [hdxgi.cpp] apply FORCEWINRESIZE to DXGI swap chain
fix: [hdxgi.cpp] apply NODISABLEPRINT to DXGI swap chain
fix: [hdxgi.cpp] apply frame skip & delay to DXGI Present method
fix: [hdxgi.cpp] fixed method names in log, now using actual Object::Method syntax
fix: [hd3d.cpp] added wrapper for Present method of Swap Chain. Extends FPS control.
fix: [hd3d.cpp] fixed method names in log, now using actual Object::Method syntax
fix: [mciwrapper.cpp] cleared MCI_MCIAVI_PLAY_FULLSCREEN and MCI_MCIAVI_PLAY_FULLBY2 flags in MCI_PLAY in window mode. Fixes "Three Dirty Dwarfs" fullscreen movie.
fix: [user32.cpp] added "DDFullBck" window class to black windows to be suppressed by KILLBLACKWIN flag. Fixes "Three Dirty Dwarfs" START.AVI movie.
fix: [mciwrapper.cpp] fixed prototype of mciSendCommand
fix: [dxwndhostView.cpp] fixed /r:n option
add: [dxhelper.cpp] added MCI_MCIAVI_PLAY* flags to helper
add: [kernel32.cpp] new STRETCHPERFREQUENCY flag. Works for time stretching with "Gun Wings"

v2.05.29
fix: [user32.cpp] NOFILLRECT option made active also with no GDI emulation
fix: [iatpatch.cpp] merged log messages in a single line to avoid multiple timestamps in log
fix: [opengl.cpp] erased coordinate scaling in glRasterPos* functions since they operate in logical coordinates. Fixes "Nanosaur Extreme"
fix: [kernel32.cpp] fixed internal dxwTranslatePathA routine possible exception when mapping == NULL. Fixes "Three Dirty Dwarfs" exception with CD emulation.
fix: [kernel32.cpp] aligned WIDECHAR dxwTranslatePathW to ASCII counterpart dxwTranslatePathA
fix: [hdxgi.cpp] fixed HookDXGISwapChain hooker for DCGI SwapChain object. Fixes crash in "Varstray"
fix: [hdxgi.cpp] fixed DXGI CreateSwapChain wrapper to properly stretch the swap chain to window. Fixes scaling in "Varstray"
add: [kernel32.cpp] FAKEGLOBALATOM tweak. Fixes "Three Dirty Dwarfs" bugged protection.
add: help page for Locale tab and some more thanks
mod: general code change to get rid of empty "if" statements on stream build

v2.05.28
fix: [excepthandler.cpp] tracing of the whole assembly instruction
fix: [kernel32.cpp] fixed GetLogicalDriveStringsA wrapper in case used with insufficient space. Fixes "Tie Fighter 95" exception.
fix: [hd3d11.cpp] fixed TRACEALL wrapper for D3D11 Device
fix: [hd3d11.cpp] added missing hook to IID_IDXGIFactory1 object in CreateDXGIFactory wrappers
fix: [hd3d7.cpp] split handling of d3dim.dll and d3dim700.dll. To be tested thoroughfully 
fix: [hdxgi.cpp] added IID_IDXGIAdaptyer1 trace, added IID_IDXGIFactory1 hook
fix: [binkw32.cpp, bink.h] implementation of SKIPMOVIE for bink player. 
add: [hd3d11.cpp] more log info
add: [winmm.cpp] NOZEROAUXDEVID flag. Fixes "Tie Fighter 95" audio
add: [gdi32.cpp] NOSETPIXELFORMAT flag. Fixes "Heretic II" on Wine/Linux
add: [opengl.cpp] GLEXTENSIONSLIE flag. Could fix "Oni".
add: improvements in expfinder tool
add: bink.h file collection for 7 releases

v2.05.27
fix: [kernel32.cpp] check PeekMessageA pointer before using it. Skips a crash in "FIFA 97" window version
fix: [ddraw.cpp] deleted duplicated HookLibraryEx call
fix: [hd3d.cpp, hooktable.cpp] HookDirect3D function split into two for d3d8 and d3d9 (preliminary ... not working as intended yet)
fix: [user32.cpp] improvement in SWALLOWMOUSEMOVE flag suggested by Kayel Gee: use MaxFPS when defined. Improves "Akte Europa".
fix: [advapi.cpp] fixed sscanf format in hex value parsing, format must be %x rather than %#x. 
add: [ddblit.cpp] new functionality suggested by Kayel Gee: flag LIMITFLIPONLY
add: [ddraw.cpp, hd3d.cpp, kernel32.cpp] new flag DIRECTXREPLACE


v2.05.26
fix: [ddraw.cpp] fixed pixel format settings for NOALPHACHANNEL case
fix: [oglblt.cpp] fixed in case the HDC is invalidated. Fixes DK2 intro movies.
fix: [sdl.cpp] on display overlay error because of locked surface, unlock it and retry. Fixes "20000 Leguas de Viaje Submarino" intro movies.
fix: [dinput.cpp] fixed GetDeviceData wrapper crashing when logging read data in debug mode and small DIDEVICEOBJECTDATA array. Fixes "Gilbert Goodmate" demo crash when logging DirectInput operations.
fix: [dinput.cpp] fixed GetDeviceData wrapper to implement EMULATERELMOUSE also in this method. Fixes "Gilbert Goodmate" mouse control.
fix: fixed mci logic: MCI_CLOSE no longer stops playing audio tracks. Fixes "Mad Trax" that issues MCI_PLAY and MCI_CLOSE in sequence.
fix: [winproc.cpp] force release of mouse clipper on Alt-F4 termination.
add: [dinput.cpp] added FORCEABSAXIS and FORCERELAXIS tweaks to force DirectInput axis mode. FORCERELAXIS fixes "Sacked - Your last day" a.k.a. "Gefeuert" mouse control on my portable pc.
add: [kernel32.cpp] more logging on DeviceIoControl wrapper

v2.05.25
add: startup settings, with help
add: more GUID identifications in log
add: ESC key to minimize DxWnd to icon tray
add: check on valid Win32 program header for hooked task
fix: mci wrapper using a correct fake device id for CDAudio aux device. Thanks to UCyborg. Fixes "Interstate '76"
fix: [oglblt.cpp] added transformation routine for 24 bpp color depth. Fixes "Runaway" movies
fix: [queryint.cpp] fixed gamma ramp detour when there's no real primary surface. Fixes "Runaway"  with opengl & gdi renderers
fix: [gdiblt.cpp] fixed gdi and opengl blitters for source/dest RECT smaller than full surface. Fixes "Runaway"  with gdi and opengl renderer
fix: [ddraw.cpp] handling of BlrtFast DDBLTFAST_DONOTWAIT flag
fix: [init.cpp] added proxy interlock. Now it is possible to copy multiple proxy dlls to the same folder

v2.05.24
fix: [oglblt.cpp] fixed RGB->BGR color conversion in opengl renderer @32 bit color depth. Fixes "Dungeon Keeper 2"
fix: [oglblt.cpp] fixed opengl renderer blitting surfaces with backbuffer size different from primary surface size
fix: [oglblt.cpp] fixed possible opengl exceptions by means of exception handling. Fixes "Dungeon Keeper II" DKII-DX.exe instability
fix: [gdiblt.cpp] fixed gdi renderer blitting surfaces with backbuffer size different from primary surface size
fix: [player.cpp] fixed play loop ending for .wav audio format to avoid crash at end of track
add: [hd3d7.cpp] more logs in Viewport calls
add: [sdl.cpp] SDL_CondWaitTimeout wrapper for time stretching

v2.05.23
fix: [oglblt.cpp] fixed log message
fix: [dxwndHostView.cpp] fixed handling of gray icons in case of failure. Fixes icon handling on Wine/Linux environment
fix: [opengl.cpp] several fixes to avoid wgl errors when virtualizing & windowing. Fixes "Sid Meier's SimGolf" error message boxes.
fix: [dinput.cpp] keeping track of device type upon QueryInterface remapping (see Dragon Riders)
fix: [opengl.cpp] added internal format GL_LINE_STRIP to opengl texture dumps - reveals SimGolf textures
fix: [opengl.cpp] recovered opengl texture dump pathnames, no "0x" prefixes on file paths. 
fix: [kernel32.cpp] fixed fake cd label reading from proxy configuration 
fix: [user32.cpp] some log messages
add: [dxwndHostView.cpp ...] add SETCMDLINE option
add: [dxwndhost.cpp, dxwplay.cpp] added dxwplay.dll version on GUI Help -> About ... panel
add: [kernel32.cpp] added log for IOCTL_SCSI_PASS_THROUGH_DIRECT IoControl code - found in "Heaven & Hell" Securom protection.
add: [winmm.cpp] added traces for mixerGetLineControlsA wrapper
add: GUI "View -> DxWnd logs" expert menu command to open the GUI logfile dxwnd.log
add: [opengl.cpp] more opengl trace wrappers in traceall build

v2.05.22
fix: [GameIcons.cpp] better controls to avoid a GUI crash on Wine/Linux environment
fix: [dxhelper.cpp] added DDSD_FVF flag in ddraw surface trace 
fix: [Inject.cpp] fixed bug crash in "Inject suspended" mode caused by early free of path string

v2.05.21
add: [Inject.cpp & others ...] Injection mode with committed pages. Makes "South Park XP patch" working.
fix: CD player status initialization
fix: rebuilt blacker tool 
fix: [dinput.cpp] HIDEJOYSTICKS flag now working also on dinput EnumDevicesA/W methods

v2.05.20
add: [kernel32.cpp] full DeviceIOControl trace
fix: [ddraw.cpp] rolled back v2.04.95 fix in GetAttachedSurface wrapper. Fixes "Star Wars Rogue Squadron 3D" crash.
fix: [dr_flac.h] included up-to-date dr_flac release v0.11.10 - 2019-06-26
fix: [player.cpp] fixed bug in dxwplay.dll crashing at the end of a flac track play.
fix: [TargetDlg.cpp] defaulting window style to bordered 

v2.05.19
fix: [ddraw.cpp] uncommented code block for palette initialization because it was necessary for "Virtua Fighter PC" and "Speedboat Attack" initialization.
fix: [kernel32.cpp] CreateFileMappingA wrapper throwing exception while trying to translate a possibly null path.
fix: [oglblt.cpp] added DDBLT_COLORFILL processing to opengl blitter
fix: [kernel32.cpp] added GetPrivateProfileStringW wrapper
fix: [kernel32.cpp] fixed logic for GetPrivateProfile* calls with NULL path
fix: [ddraw.cpp] fixed logic to manage the possible virtual screen resolution change by means of RestoreDisplayMode method (ref. Flight Unlimited II)
fix: [ddraw.cpp] fixed login in Flip wrapper when the screen resolution is smaller than the backbuffer size (ref. Flight Unlimited II)
add: [gdi32.cpp opengl.cpp] experimental: buffer projection facility
fix: [advapi.cpp] regression bug on REG_DWORD key parsing - fixed "The Hive" virtual registry settings.
fix: [dxhook.cpp] cleared WC_CLIPCHILDREN style flag when INVALIDATECLIENT tweak is set - recovers "Gangsters Organized Crime" menu text visibility.

v2.05.18
fix: [oglblt.cpp] added pitch compensation in opengl renderer. Fixes rendering on cases where the blitted surface is not plain memory surface, es. "Emperor Battle for Dune".
fix: [ddraw.cpp] Force_HAL/HEL/NULL flags applied consistently also to DirectDrawCreateEx
fix: [ddraw.cpp] suppressed unsupported DirectDrawSurface::Lock DDLOCK_NOSYSLOCK flag, unsupported Win16 legacy found in "Star Wars X-Wing" Win95 port XWING95.EXE
fix: [player.cpp] support for Linux/Wine 16 bit flac/wav samples, enlarged buffers.
fix: [player.cpp] fflush on logging, no log loss upon crash.
fix: [kernel32.cpp] fixed CreateProcess hook modes
add: [kernel32.cpp] added wrapper for CreateProcessW
add: [dxhook.cpp, kernel32.cpp] FRONTEND mode flag
fix: [TabLocale.cpp] dynamic loading of LCIDToLocaleName to keep WinXP compatibility

v2.05.16
fix: [hdxgi.cpp] separate wrappers for Adapter/Factory objects in TRACEALL build - fixes "Ananke" crash
fix: [hd3d.cpp] log message for CheckDeviceType wrapper
fix: [hd3d.cpp] D3D10CreateDevice1 wrapper crashing because tried to hook methods also when failed. Fixes "Tobi".
fix: [user32.cpp] recovered some locale handling oddities that make this version work better.

v2.05.15
add: [kernel32.cpp] hook to DeviceIoControl call for LOCKCDTRAY flag
add: [opengl.cpp] new "Lock Viewport" (LOCKGLVIEWPORT) flag to set a valid viewport in case the game forgets it!
add: [hd3d7.cpp] new "No TnL HAL device" (NOTNLDEVICE) flag to suppress IID_IDIrect3DTnLHALDevice device
add: locale logging cathegory
fix: [ddraw.cpp] initial palette handling - fixes "Sleepwalker" missing palette creation.
fix: [winmm.cpp] fix waveOutOpen wrapper crashing while trying to log a NULL ptr
fix: [user32.cpp] SetCursorPos automatically disabled when the window loses focus. Fixes a "Yu No" issue.
fix: [user32.cpp] setting fullscreen mode when a window is grown to big enough size
fix: [user32.cpp] issue about locale handling in SetWindowTextA wrapper producing truncated ASCII/UNICODE title.
fix: [user21.cpp] LOCKSYSSETTINGS bugged flag 
fix: code reorganization & cleaning 
beware: some possible locale handling regression

v2.05.14
add: GUI /o argument to set read-only mode on dxwnd.ini configuration file.
add: (very) preliminary ntlea integration for locale emulation
fix: [dxhook.cpp] hot patching hook in case of multiple entries of same name leading to recusrive calls
fix: [ole32.cpp] handling of CoCreateInstance for IID_DirectDraw7 objects

v2.05.13
fix: [dxhook.cpp] FixWindowFrame procedure now keeps taking in proper account the former window style/exstyle flags. In particular, keeps the WS_CLIPCHILDREN/SIBLINGS flags necessary in many situations. Improves "Minigolf Masters Miniverse".
fix: [hd3d.cpp] fixed handling of BackBufferFormat equal to D3DFMT_UNKNOWN - found in "Sub Command"
fix: [hd3d.cpp] fixed D3D8::Present wrapper in 16bpp emulation mode, adds InvalidateRect call to force screen update. Fixes "Sub Command" mouse trails.
fix: [hd3d.cpp] fixed return code of D3D8::GetDisplayModesCount that must be doubled when emulation of 16BPP surfaces is active. Allows running "Sub Command" with emulated SVGA/HDMI modes.
add: [dxwndhost.cpp] added write capability check on dxwnd.ini to warn against possible loss of configuration updates
fix: [TargetDlg.cpp] GUI: fixed random target # in add new task form.

v2.05.12
fix: [dxwcore.cpp] with BLITFROMBACKBUFFER tweak, refresh routine using backbuffer. Fixes "Festal Sailing"
add: [hd3d.cpp hd3d7.cpp] "Pretend HW T&L cap." (TRANSFORMANDLIGHT) flag.Fixed "Will Rock"
add: [kernel32.cpp] #ifdef-ed tracing wrappers for GlobalMemoryStatusEx and GetPhysicallyInstalledSystemMemory calls
fix: [kernel32.cpp] added fkey polling in GetVolumeInformationA wrapper. Allows virtual CD change in "Zork Nemesis".
add: [sdl.cpp] added tentative SDL2 wrappers for SDL_GetDisplayBounds, SDL_GetDisplayUsableBounds and SDL_SetWindowDisplayMode calls
add: [user32.cpp] MOUSEMOVEBYEVENT tweak now using SendInput instead of deprecated mouse_event
fix: [dxwnd.cpp] fixed self-generated message about UnhookWindowsHookEx(0) error.
add: [version.cpp] #ifdef-ed conditional trace for some operations of version.dll
add: [winmm.cpp] "Fix default MCI Id" (FIXDEFAULTMCIID) flag to fix the legacy default device id 0xFFFF to 0xFFFFFFFF after WinME. Fixes SpyCraft midi/wave operations.
add: [winmm.cpp] more MCI tracing wrappers for debug logging

v2.05.11
add: [mciwrapper.cpp] log messages for msiSendString return code/string for 2 commands
fix: [mciwrapper.cpp] correct return string for msiSendString  "status cdaudio position" in TMSF mode. Fixes "The House of the Dead" audio resume
fix: [user32.cpp winproc.cpp] fully revised hot keys handling, added "cdnext", "cdprev" function keys to swith CD in charger
fix: virtual disk path remapping in proxy mode
fix: GUI no longer hooking tasks when launched while hooking is stopped

v2.05.10
fix: [user32.cpp winproc.cpp] hot keys handling now applied also in case of PeekMessage loop: makes hot keys working also for "Yu No".
fix: [dxwcore.cpp MapKeysInit] hot key definitions in proxy mode now loaded from dxwnd.dxw instead of dxwnd.ini
add: [user32.cpp] added "Suspend GetAsyncKeyState" (FIXASYNCKEYSTATE) flag to avoid detecting mouse/keyboard events when the application has no focus. Fixes GOG's version of "Wizards & Warriors".
fix: [winshot.cpp] screenshot capture now working either from ddraw surfaces or window DC
add: [kernel32.cpp] virtual disk path remapping applied also to MoveFileA and CreateFileMappingA
fix: [kernel32.cpp] now path remapping keeps track of current folder in case it was movet to a virtual disk. Fixes CD switch with "Dreams of Reality".
add: [excepthandler.cpp] now detecting and neutralizing the vSync wait condition performed on VGA cards through assembly instructions IN 08 on port 0x3DA and potential endless loop. Fixes "Dreams of Reality" in executable GDIDREAMS.EXE.
add: [kernel32.cpp] #ifdef-ed tracing hooker for GetFullPathNameA.
fix: [dxwcore.cpp] ScreenToClient method recovering in case of GetClientRect error
fix: winmm proxy compiled with disabled optimization option to avoid AV false positive detections

v2.05.09
add: cd charger first release
add: [msvcrt.cpp dxhook.cpp] added "Safe MSVCRT allocs" (SAFEALLOCS) flag to protect against malloc/realloc/free bugs.
add: [kernel32.cpp - dxwTranslatePathW] pathname virtualization for widechar calls - to be tested. 
fix: proxy install copies hot keys configuration when required
fix: [kernel32.cpp] added virtual CD/HD definition to Get/WritePrivateProfileString/IntA/W wrappers.Fixes "Dementia" on virtual CD reading from a CD ini file.
fix: [player.cpp] fixed bug in virtual audio CD emulation for .flac files initialization
add: [oglblt.cpp] #ifdef-ed experimental screen rotation for opengl blitter
fix: [kernel32.cpp] GetDiskFreeSpace wrapper for NULL lpRootPathName value
fix: [kernel32.cpp CreateProcessSuspended: using lpApplicationName instead of lpCommandLine when both not NULL to find the path of a new hooked process. Fixes "Zork Nemesis" when running itself.

v2.05.08
add: [oglblt.cpp] new OpenGL blitter for ddraw surface
fix: [sdlblt.cpp] fix in SDL blitter to manage partial RECT blits
fix: [sdlblt2.cpp] fix in SDL2 blitter to manage partial RECT blits
fix: [hdxgi.cpp] fix in method hooking & operation trace: fixes "Halo Spartan strike"
fix: [hd3d.cpp] fix in IDirect3D8/9 GetAdapterModesCount and EnumAdapterModes wrapper to properly emulate the different behavior of d3d8 vs. d3d9
fix: help pages
 
v2.05.07
add: "Safe midi" (SAFEMIDIOUT) flag to bypass midi harmful calls (if repeated too often?) midiOutReset, midiOutSetVolume
add: "Shring text width" (SHRINGTEXTWIDTH) flag to reduce all font widths. Fixes some eastern (asian) games that exceed the width when played on western locale.
fix: [avifile.cpp] NOMOVIES flag now stops also movies played through AVIFile calls
add: [avifile.cpp] more trace wrappers for #ifdef TRACEALL compile
fix: [msvfw.cpp] fixed log messages, added debug wrapper fot ICSelect.
fix: mciGetDeviceIDA wrapper exception caused by doubled hook
fix: mciSendCommandA wrapper for fake device path translation in MCI_OPEN + MCI_OPEN_ELEMENT case
fix: mmioOpen wrapper for fake device path translation
fix: better error message box in proxies when original dll can't be loaded
add: GUI /L /Y /Z /S client commands (see help), updated help pages

v2.05.06
fix: [hdxgi.cpp] SwapChain hooker with wrong offsets: "Ys Memories of Celceta" no longer crashes.
fix: [dxwndhostView.cpp] cleaned some spurious (but not harmful) configuration keys from dxwnd.ini when deleting entries.
fix: [hdxgi.cpp] fixed and completed hooking of dxgi.dll calls & methods.
fix: [user32.cpp] fixed SetWindowsHookEx wrapper for FIXMOUSEHOOK processing, assign the current thread id when NULL. Fixes many DiskStation games.
fix: [kernel32.cpp] wraps a faulty HeapFree call in a try/catch statement. Fixes "RKing" DiskStation game.
add: [advapi.cpp] wrapper for RegOpenKeyA. Needed for "Moon Child".
fix: [kernel32.cpp] Heap log traces moved from DW cathegory to SYS
add: [dxwcore.bpp] special warning banner to tell the presence of active debug flags
add: [user32.cpp] new flag "Recover Print Screen key" (NODISABLEPRINT) to recover the print screen key when disabled in WH_KEYBOARD or WH_KEYBOARD_LL hooks.

v2.05.05.fx1/fx2
fix: exported joystick configuration to dxwnd.dxw proxy configuration file when virtual joystick is enabled.
fix: GUI bug failing to save entries that were imported, exported or proxed.
fix: some html help pages to reflect changes in configuration tabs (Mouse tab).
fix: [dxwcore.cpp FixNCHITCursorPos] fixed regression since v2.05.02 for "Fix WM_NCHITTEST" flag, fixed Zombie Shooter games.
fix: "blacker.exe" now waits for the target to enter in fullscreen status. Used for "Jane's Fighters Anthology" to make the initial splash dialog visible.

v2.05.05
fix: [hd3d.cpp] fixed EnumAdapterModes, GetAdapterModesCount and CheckDeviceType methods for d3d8/9 with VGA/HDMI emulation. Lets many d3d8/9 games running with DxWnd default settings.
fix: [user32.cpp] bypass of SystemParametersInfo call for SPI_SETWORKAREA mode. Fixes "Jetboat Superchamps 2" desktop resizing after termination.
add: GUI autosave mode
add: optimized configuration save, only updated entries are saved
add: LockSysSettings (LOCKSYSSETTINGS) tweak to prevent any possible system setting update
fix: ignored (EXCEPTION_CONTINUE_SEARCH) exception 0xe06d7363 from C++ compiler. Fixes "Rage of Mages - Allods" with "Handle exceptions" flag.
fix: [mciwrapper.cpp] added handling of mci commands like "open %c: type cdaudio alias %s". Found in HoMM1.
fix: [mciwrapper.cpp] bug in usage of audio cd alias
add: [smack.cpp] hooking for "SMKWAI32.DLL", WAIL32 variant for SMACKW32.DLL. Referenced by HoMM1 Win32 version
fix: [winmm.cpp] deleted 2 debugging STEP instructions
add: [user32.cpp] added "Invalidate full rect" (INVALIDATEFULLRECT) flag to force NULL rect value in USER32 InvalidateRect proxy. Fixes HoMM1 video glitches.
add: GUI configuration: "Input" tab split to "Input" and "Mouse" to get more space, Alt-Tab settings and invert mouse axis brought from tweaks to config panels.
add: experimental "Correct MESSAGEHOOK callback" - not working yet.

v2.05.04
add: [ddshot.cpp] added sequence number for captured screenshots to match bmp files and logs
add: [excepthandler.cpp] added handling of priv instructions OUT and REP OUTS to emulate VGA direct hardware palette definition through DAC WRITE operations. Fixes "Dark Judgement" movies.
fix: [kernel32.cpp] log message for file I/O operations in a single LogTrace call. Avoids multiple timestamps on log line.
fix: [kernel32.cpp proxy\init.cpp] fixed proxy mode that must read PRIVATEMAP fields from dxwnd.dxw instead of dxwnd.ini
fix: [advapi.cpp ddraw.cpp] error condition on dxwnd.dll file attributes
fix: revised generation of dxwnd-relative paths, now using GetDxWndPath thoroughfully
add: added "cursor:InvertMouseX/YAxis" (INVERTMOUSEX/YAXIS) tweaks. Improve "Sherlock Holmes The Mystery of The Mummy" mouse problems.
add: [smack.cpp] added "ddraw:SmackBufferNoDepth" (SMACKBUFFERNODEPTH) tweak. Unlock smackw32 color depth specification. Fixes "Galapagos" into movie.
fix: [smack.cpp] fully revised smackw32.dll hooking to manage different releases / interfaces / ordinals / calling conventions.
fix: [sdl.cpp] revised code bugs, added SDL_PushEvent wrapper. Fixes "Darkest of Days" mouse control.
add: [sdl.cpp] added SDL_ShowCursor wrapper to react at cursor visibility flags.
add: [ddraw.cpp] more ifdef-conditioned method traces, use of TRACEALL macro.
add: [hd3d7.cpp, hd3d11.cpp, hdxgi.cpp] more wrapping and traces ... 
fix: [dinput.cpp, ole32.cpp] replaced GUID trace with sRIID common call
add: [dwdisasm.cpp] added popup messages in case of missing disasm.dll
fix: [dxhook.cpp] deleted dxw.SetScreenSize in AdjustWindowRect: was setting bad virtual resolutions (including window borders!)
add: in tools folder (full DxWnd release only) added dlldump, paldump, pedump tools.

v2.05.03
fix: GUI bug about unclearable NOWINDOWMOVE tweak
fix: [dxmapping.cpp] window position initialization
add: [sdl.cpp] wrappers for many SDL2 calls, including SDL_SetWindowPosition and SDL_SetWindowSize. Ref. "Belzebub" (Diablo mod.)
fix: added Xinput ordinals - ref. "Ys Memories of Celceta"
add: dxw.ScaleRelMouse to scale the relative mouse deltas with no roundong errors
fix: [user32.cpp] applied dxw.ScaleRelMouse in GetRawInputData wrapper
add: [hdxgi.cpp] added wrapper for SetFullscreenState, GetParent. Needed for "Ys Memories of Celceta"
add: [hd3d11.cpp] added many wrappers. Needed for "Ys Memories of Celceta"
add: [opengl.cpp] SCALEMAINVIEWPORT flag to scale only viewports referred to main win. Ref. "Belzebub" (Diablo mod.)
add: [ddraw.cpp @ extBltFast] debug surface dump added to BltFast wrapper
fix: DxWnd support for long pathnames conversion to short format on WinXP
add: [dxwndhostView.cpp] added error code + log message on mailslot errors
add: [ddraw.cpp] FORCESHAL & FORCESNULL tweaks
add: FIXRANDOMPALETTE debug flag

v2.05.02
fix: [GameIcons.cpp] fixed GUI locking programs after icon extraction
fix: [kernel32.cpp] fixed double redirection in .ini pathnames, added more wrappers for WIDECHAR functions
fix: [GammaRampDialog.cpp] fixed DC leakage on gamma ramp view timer
fix: [GammaCtrlDialog.cpp] fixed DC leakage on gamma ramp control timer
fix: [PaletteDialog.cpp] fixed leakage on view palette panel
fix: generic review for GUI timer handling and window exit
fix: [user32.cpp] fixed/improved Get/SetCursorPos, now working with "Birthright the Gorgon's Alliance"
add: [dxhelper.cpp, queryint.cpp] added unknown IID_IDirectDrawSurfaceNew to helper and logs
fix: [queryint.cpp] set correct error code E_POINTER to handle unsupported interfaces
fix: [ddraw.cpp@extGetGDISurface] when GetGDISurface wrapper returns the emulated primary surface it must also call AddRef to emulate the real method. Fixes "Conquest Frontier Wars" initial crash.

v2.05.01
fix: [ddraw.cpp] fixed recovered window size after minimize/recover. Fixes "Heavy Gear 2" resume.
add: [dinput.cpp] added log messages for DirectInputCreate args and RemapUSKey tweak
fix: [fontstack.cpp] fixed fontstack to grow dynamically. Fixes "Allied General" font leakage.
fix: [user32.cpp] fixed MoveWindow handling of BIG windows. Fixes "Allied General" window handling.
fix: [ddraw.cpp ddcreates.cpp] diffused code reorganization
fix: [mciwrapper.cpp] fixed MCI_RESUME regression. Fixes "Heavy Gear 2" music resume after pause with Esc key.
fix: [mciwrapper.cpp] fixed a few mciSendString and mciSendCommand bugs
add: [kernel32.cpp] REMAPSYSFOLDERS flag to avoid writing on Windows system folders
fix: GUI didn't show content of flag11 and flag12 on View Flags panel
fix: [ddraw.cpp] fixed possible crash in GetAvailableVidMem wrapper log message 
fix: [mciwrapper.cpp] fixed status length and position string outputs for all supported time formats
fix: [mciwrapper.cpp] set proper error codes for unsupported time formats
fix: [mciwrapper.cpp] support for alternate syntax "track n length" and "length track n"
fix: [mciwrapper.cpp] support for alternate syntax "track n position" and "position track n"

v2.05.00
fix: [hooktable.cpp] wrong dll name and flags in hook table, prevented correct hook of earias.dll and earpds.dll
fix: [mciplayer.cpp] renamed player routines in logs, added track duration in seconds format
fix: [mciwrapper.cpp] added MCI_SEEK tracing in logs
fix: [mciwrapper.cpp] fixed MCI_SYSINFO logic
fix: [mciwrapper.cpp] extended HACKMCIFRAMES logit to MCI_FORMAT_MILLISECONDS and MCI_FORMAT_TMSF formats
fix: [mciwrapper.cpp] correct interpretation of MCI_STATUS + MCI_STATUS_POSITION request with no MCI_TRACK
fix: [mciwrapper.cpp] simplified logic for MCI_CDA_STATUS_TYPE_TRACK request
fix: [mciwrapper.cpp] added return code trace both in decimal and hexadecimal format
fix: [winmm.cpp] fixed hooking of auxGetDevCapsA
fix: [mciwrapper.cpp] fixed MapMsecToTTMMSS routine unable to handle last track
fix: [mciwrapper.cpp] reset time format after MCI_CLOSE command or after "close" command string
fix: [player.cpp, dr_mp3.h] integrated mp3 player (thanks to drmp3 sources)
add: dxwnd project with TRACEALL switch to build dxwnd.traceall.dll

v2.04.99
fix: set a different default aspect ratio for HDTV resolutions (16:9 vs. 8:6) [dxmapping.cpp]
add: ADAPTIVERATIO flag [dxwcore.cpp - FixWorkarea]: useful to play "Cossacks 2" with variable aspect ratio
fix: [hd3d.cpp] in D3D8/9 EnumAdapterModes and GetAdapterModeCount methods, using virtual resolutions instead of real ones when VGA or HDTV flags are set. Preliminary implementation.
fix: fixed virtual CD player for possible current directory change [mcilayer.cpp, player.cpp]. Fixes "Dreams to Reality" mci errors.
fix: error condition for activating virtual CD player for "cdaudio" device name in mciSendCommand [mciwrapper.cpp]: fixes "Dreams to Reality" mci errors.
fix: [player.cpp] wrong parameters for .flac decoding. Fixes low pitch and missing audio channel problems
fix: set a initial stack size for virtual CD player thread: fixes crash in "Speedboat Attack" with flac encoded tracks.
fix: [mciwrapper.cpp - sDevType, swDevType] bug in sound debug log message causing application crash - happens for instance with "Sentinel Returns".
fix: [mciwrapper.cpp] bugged track length reported by mciSendCommand(MCI_STATUS) call. Fixes "Sentinel Returns" copy protection.
fix: [player.cpp] flac time progress evaluation
fix: [user32.cpp] FPS processing (measure and delay) now applied also to StratchBlt to screen. Setting some delay fixes immediate death in "Dreams to Reality" gdi version.
fix: revised (and simplified) coding for supported emulated video resolutions
add: possibility to support both SVGA and HDTV resolutions plus custom one.
fix: eliminated (again) hack with auxGetNumDevs returning 1 available device always. 

v2.04.98
add: UNNOTIFYINACTIVE flag to suppress window disable only - fixes "Evil Islands: Curse of the Lost Souls"
fix: ddraw AddAttachedSurface marking as flippable backbuffer any videomemory+offscreenplain surface attached to primary. Fixes "Alchemist" flip operation
fix: calculation of ZBuffer surface size for "Clean ZBUFFER hard" flag with dxversion < 4. 
add: dxgi preliminary hooking
add: more d3d11 hooking
fix: updated html help
fix: revised protocol between dxwnd.dll and dxwplay.dll
add: preliminary capability for virtual CD audio to play .flac, .mp3 and .wav sound formats
add: GUI /n "no Spinnig" option to suppress the spinning icon on active status
add: CDPAUSECAPABILITY to emulate a CD driver that can pause music play
add: SETUSKEYDESCR tweak, useful for "Heavy Gear 2"
add: REMAPNUMERICKEYPAD tweak, useful for "Ecstatica" win32 version
add: HOOKEARSOUND flag to hook ear sound libraries (tracing only)
fix: Sample CD Player with volume control

v2.04.97
add: preliminary (partial) implementation of ADVAPI32 RegQueryInfoKeyA wrapper - allow running "Fallen Haven" with virtual registry
fix: eliminated DC leakage in virtual joystick handling
fix: eliminated DC leakage in "Force setting gamma ramp" flag handling
fix: eliminated DC leakage in several OpenGL/wgl wrappers
fix: handling of window scaling when created within main window creation - to be reviewed, improves "Fallen Haven"
fix: full review of mci wrappers for virtual CD audio, several bug fixes.
add: sample CD Player based on mciSendCommand function, thanks to Nikolai Serdiuk

v2.04.96
add: GDI32 Chord wrapper - referred in "Fallen Haven"
add: frames unit of measure handling in mciSendCommand wrapper
add: HACKMCIFRAMES flag to fake a customary frame number on each audio track - unprotects "Absolute Terror"
fix: handling of case insensitive commands in mciSendString wrapper - needed for "Fallen Haven"
fix: code reorganization for mciSendString wrapper
fix: handling of "status cdaudio mode" command string in mciSendString wrapper - needed for "Fallen Haven"
fix: USER32 ScrollDC wrapper in stretched mode
fix: mci sound emulation starting with correct msf time format instead of tmsf
fix: mci sound emulation fixed time format procedures in several cases
add: tentative full implementation of mciSendStringW wrapper
fix: inhibit virtual resolution changes from windows created from within a main window - improves "Fallen Haven" rendering

v2.04.95
add: added tool command "View import table"
fix: build flippable backbuffer also when number of backbuffer surfaces is unspecified - fixes "Gadget Tycoon"
fix: fixed "Fix GlobalUnlock" flag implementation - fixes "Sega World Wide Soccer" errors
add: preliminary support for mciSendCommand operations about track<nn>.cda audio pseudo-files - adds CD sound to "Absolute Terror"

v2.04.94
fix: fixed 8bit DIB palette emulation. Fixes "iM1A2 Abrams" colors
add: #ifdef-ed trace wrappers for user32.dll window scrolling calls
add: better logging for Kernel32.dll GetDriveType wrapper
fix: fixed & uniformed logic to decide whether a window needs absolute or relative position, using internal BOOL IsRelativePosition routine. Fixes "Industry Giant" that renders over a CHILD+POPUP window.
fix: fixed log messages about window styles
fix: fixed position calculation for centered option
fix: code reorganization of user32.cpp user32.dll wrapper
fix: fixed message hook callback to avoid scaling mouse coordinates directed towards the child window borders - fixes "Imperialism!" help dialogs.

v2.04.93
add: #ifdef-ed NOFASTBLT option to replace FastBlt method with more robust Blt
fix: complete implementation for dialog stretching for both DLGTEMPLATE and DLGTEMPLATEEX formats
fix: updated and more readable log trace with timing and threads - timing starts from 0 and seconds are split from msec
fix: fontstack initialization for font scaling - suppresses error dialog box on font scaling usage
fix: fontstack handling -  doesn't build a scaled font if the original-sized font creation failed.
fix: virtual Heap handling - suppress double consecutive free for same memory address
fix: virtual Heap handling - on NOBAADFOOD opt, don't clear the area if malloc failed
add: virtual Heap handling - flag VIRTUALPROCHEAP to replace virtual heap also on initial process heap
add: #ifdef-ed trace wrappers for user32.dll SetActiveWindow, GetAncestor
fix: STRETCHDIALOGS flag now applied also to CreateDialogIndirectParamA
fix: possible use of uninitialized variable in fontstack.cpp dxwFStack::Deletefont()
fix: in gdi32.dll DeleteObject wrapper, possible use of uninitialized variable for scaled font handle
fix: in user32.dll TabbedTextOutA wrapper, possible crash in parameter's log
fix: in user32.dll TabbedTextOutA wrapper, scaling of tab position array
fix: blacker.exe proper widechar text on window title, thanks to dippy dipper
fix: HIDEWINDOWCHANGES logic applied also to MoveWindow and SetWindowPos wrappers
fix: changed MoveWindow wrapper method to get the main window coordinates
fix: mciSendCommand wrapper - sysinfo must return "cdaudio" string, thanks to dippy dipper - fixes "Pandemonium!" with cd audio emulation

v2.04.92.fx1
fix: fixed "Desktop" mode crash on Win7 
add: experimental D3D8MaximizeWinModeHack tweak to recover real fullscreen mode on Win10 D3D8
v2.04.92
!!!: enlarged shared memory area - this release is not compatible with older ones
fix: defauld SDL hooking, dependencies are hooked
add: EXTENDSDLHOOK flag to extend hooking to SDL dependencies also when SDL is hooked
add: STRETCHDIALOGS flag to scale embedded dialog resources at runtime - useful for "Aaron vs. Ruth".
add: limitation to the number of consecutive patched exceptions - after 20 in a row is probably no use to keep going.
add: TRACEALL #ifdef-ed flag in kernel32.dll wrapper - logs RaiseException call
add: tentative kernel32 HeapSize wrapper
fix: revised kernel32 Heap wrappers
add: SDL SDL_PeepEvents wrapper
fix: SDL streamlined compilation excluding log routines and helpers
fix: user32 CreateWindow* wrapper calling ShowWindow only for main window - fixes regression bug in "Diablo" and "Gunbound"
fix: better logging in DialogBoxParamA wrapper

v2.04.91
fix: added wrapper for IDIrectDraw::GetFourCCCodes to clear available FourCC list
fix: commented out (#ifdef-ed) debug messages about IDIrectDraw::GetScanLine
fix: on "Stretch Active Movie" blitting is forced to full primary surface area
fix: handling of black hider window, now recreated when no longer valid - fixes "Space Clash" with hide desktop opt.
fix: KERNEL32 CreateProcess wrapper in inject suspended proc mode in case lpCommandLine is not NULL but empty string - fixes "Space Clash" CLASH.exe running SPACE.exe in extended hook mode.
fix: revised handling of ddraw DDERR_DOESNTOWNSURFACE error 
fix: on ReplacePrivilegedOps() now using psapi GetModuleInformation() to retrieve text segment informations
fix: on ReplacePrivilegedOps() added 0x0F20xx (MOV xx CRx) to privileged instructions list - 0x0F20E0 = MOV EAX CR4 found in "Crime Cities" 
fix: revised handling of initial position & size: now working also with centered and fake-fullscreen modes
fix: fixed GetFileAttributesA wrapper in fake CD mode, now returning FILE_ATTRIBUTE_DIRECTORY for CD folders - fixes "Scooby Doo Case File #1"
fix: fixed GetLogicalDriveStringsA wrapper in fake CD/HD mode, now returning also fake devices - fixes "Scooby Doo Case File #1"
fix: fixed SetWindowPos wrapper to properly handle big windows and size/position flags - fixes some "Space Clash" movie problems.
fix: GUI now proposing configuration save also in case of change in fake CD/HD drive letters


v2.04.90
fix: added possibility to hook dll families by wildcarded path, useful to hook similar d3dx9_*.dll libraries
fix: GDI ChangeDisplaySettings wrapper: proper handling of CDS_RESET, CDS_NORESET flags. Fixes "LEGO Racers" display mode settings.
fix: DirectInput* devices: cleared handles upon Release to avoid exceptions in ToggleAcquiredDevices. Fixes "Sentinel Returns" exception that leaves the process running in background. 
fix: complete dinput.cpp code reorganization, no hooking duplicated code.
fix: partial user32.cpp code reorganization, #ifdef-ed unused wrappers, TRACEALL switch
fix: USER32 SetWindowPos wrapper various fixes. Partially fixes "LEGO Racers" intro movies.
fix: in GUI, adaptation to WinXP - Win7 & later ADVAPI32.dll changes to still support WinXP platform.
fix: in ddraw.cpp, tentative handling of DDERR_DEVICEDOESNTOWNSURFACE error (see notes). Fixes some movie rendering problems.
fix: ddraw:CreateSurface prototypes
add: initial hooking of D3D11, hd3d.cpp code reorganization - W.I.P. not working yet.


v2.04.89
fix: some more diagnostic messages in ddraw SetCooperativeLevel wrapper
fix: log messages with IMMDeviceEnumerator GUID 
fix: set correct default screen size in non-windowed mode
fix: fixed "PE browse" hook (IATPatchByFT routine) in case of module changes
fix: fixed OpenGL GLint scaling for signed negative values - fixes some of "Star Wars: Knights of the Old Republic" problems
fix: suppress EMULATEMAXIMIZE and PREVENTMAXIMIZE effects in ShowWindow wrapper in case of non-windowed mode
fix: fixed IsFullscreenWindow internal logic

v2.04.88
add: more (#ifdef-disabled) wrappers for D3D2-7 DrawPrimitive methods - for "Riverworld" debugging
fix: 1 STEP log removed
fix: avoid setting a clipper against NULL window on FORCECLIPPER mode
fix: fixed EmuBlt unlock error message reporting bad error code
fix: revised logic for window creation - now window style & position setting made only once
fix: dxwCore::FixCursorPos now using more reliable pre-registered window sizes instead of using GetClientRect system call
fix: fully revised CreateWindow* wrapper
fix: handling of NOTEXTURE flag in d3d SetRenderState wrapper
fix: deleted unused FIXCHILDSIZE handling
fix: minor cosmetic changes in logging of hooker routines
add: new experimental "Hide Window changes" (HIDEWINDOWCHANGES) flag
add: DxWnd status indication in titlebar
add: more (#ifdef-disabled) wrappers for d3d2-7 DrawPrimitive method

v2.04.87
add: new IAT browsing mode by FT inspection in .exe file, new functions IATPatchByFT and DumpImportTableByFT
add: thanks to Matt Pietrek for PEDUMP source code
add: more (#ifdef-disabled) wrappers for BinkW32 calls
add: more (#ifdef-disabled) wrappers for OpenGL32 calls
add: USER32 SetWindowsHook wrapper  (#ifdef-disabled) logic to scale WH_CALLWNDPROC and WH_CALLWNDPROCRET callback coordinate values
add: DxWnd.exe client/server usage: client commands to import a .dxw file (/L:<path>), clear (/Z) or save (/S) the programs list
add: DxWnd.exe file association with .dxw files to automatically import a .dxw file on file Open from Windows
fix: GDI32 SetPixelFormat wrapper now handling a possibly NULL ppfd pointer. Fixes "Neverwinter Nights Platinum" on Win10
fix: glBitmap wrapper log message
fix: BinkW32.dll broken hooking - needed by "Neverwinter Nights"
fix: added hooking to glu32.dll (different from glut32.dll !!!), one wrapper (gluGetString) moved to proper context.
fix: OpenGL scaling aligned to generic method implemented in dxw class (in glScissor, glViewport, glGetIntegerv wrappers)
fix: OpenGL fixed glBitmap log message
fix: USER32 CreateWindow* wrapper, fixed (?) position and size for big windows, both WS_CHILD type and not.
fix: USER32 CreateWindow* wrapper, fixed CW_USEDEFAULT test condition, now using logical .AND. instead of DWORD equal condition - fixes "Neverwinter Nights" that creates windows whose size is CW_USEDEFAULT+offset 
fix: USER32 GetMonitorInfo wrapper, with better logging
fix: WM_MOVE log message, width and height were inverted.

v2.04.86
fix: D3dx9_30 hooker, avoid hooking Direct3DCreate9 twice to avoid recursion in hot patch mode
fix: in ICDrawBegin, DrawDibDraw & DrawDibBegin wrappers added coordinate scaling on GDI scaled coordinates flag
fix: tentative handling of fullscreen modes in ICDrawBegin wrapper
fix: skip attempt to dump 1x1 pixel DIBs that leads to progrm crash
fix: added dxw.LockedRes to handle dialogs that temporary suspend Windowed mode but should not allow desktop mode changes - fixes "Aaron vs. Ruth" Alt-F4 reaction. 
fix: "lower background priority" taking effect also when "Do not notify on task switch" is active (??)
fix: wrong flags passed to BltFast in PrimaryStretchBlt routine
fix: deleted usage of BltFast in video renderer since it can fail blitting but telling no error code
fix: added dxwnd address check in HookProc to completely avoid double dxwnd hooking in ddraw methods with different interface - fixes "Darkstone" in emulation "none" mode and others ...

v2.04.85
fix: GetVolumeInformationA wrapper logs & behavior
add: preliminary GetLogicalDriveStringsA wrapper
add: more wrappers for virtual drives feature: SetCurrentDirectoryA, DeleteFileA, _lopen, _lcreat, OpenFile, GetDiskFreeSpaceA
fix: added returned error codes for write access I/O operations on fake CD files
fix: added in SetCurrentDirectoryA wrapper the logic for handling virtual drives. Fixes "Drift when Worlds Collide" on virtual CD drive
fix: MCI wrapper for mciSendCommand on MCI_CDA_STATUS_TYPE_TRACK case - fixes "Sentinel Returns" CD checks on virtual CD audio.
fix: virtual CD filesystem string set to "CDFS"
fix: virtual drives configuration fields not properly saved in dxwnd.ini
add: '?' as replacement of game folder in virtual drives path
add: virtual drives tutorial in help pages, help updates.

v2.04.84
new: merged extLock() and extLockDir() wrappers into same routine
add: new "ddraw:SafePrimaryLock" (SAFEPRIMLOCK) tweak - fixes "Signus the Artefact Wars" bugs
add: hook for d3dx9_30.dll to hook d3d9.dll before it's too late 
add: configurable virtual HD/CD drives
add: more kernel32 wrappers: FindFirstFileA, FindNextFileA, GetFileAttributesA - needed to run "Need for Speed II" with fake CD.
add: kernel32 GetVolumeInformationA wrapper
fix: fixed d3d1-7 execute buffer dumping routines (conditional compilation only!)
fix: ddraw CreateSurface wrapper: on version 1 and offscreenplain surfaces, it seems that the SetPalette method is NULL so it should be unused in this case. Fixes "Atripolis 2097" crash.
fix: added fake HD/CD drives to GetLogicalDrives wrapper
fix: mapping of fake driver pathnames in MCIWNDM_OPENA message - fixes "Lego Loco" intro movie

v2.04.83
fix: as suggested by caniza mimorix, removed UnmapViewOfFile(pMapping) statements in dxwnd.cpp
fix: some code reorganization for surface dumps
fix: fixed texture highlight hack for some d3d texture format
fix: enlarged x,y,w,h fields in GUI to hold 4 digits
fix: log message for CreateDCW wrapper
fix: reateDC wrappers should avoid remapping to main win when undefined
fix: bugged activation for debug DUMPDIBSECTION feature
fix: MapWindowPoints log message for negative displacements
fix: ActiveMovie parent window remapped from desktop to main window
add: INVALIDATECLIENT tweak: invalidate the client area after a ddraw screen refresh, makes "Gangsters: Organized Crime" menu text visible.
add: added "Curtain" window class to KILLBLACKWIN targets - kills "Tennis Critters" gray background window
add: "Mouse shield" (MOUSESHIELD) Input option - suppresses WM_MOUSEMOVE messages being processed when task has no focus 
add: preliminary dxwnd.dll capability mask to disable GUI elements for unsupported features
add: hook:CreateDCHook (CREATEDCHOOK) tweak - to bypass harmful CreateDCA wrapper in "Silkolene Honda Motocross GP"
add: GDI32 CreateICW wrapper
add: new "Suppress ZBUFFER attach" (NOZBUFATTACH) flag - fixes "Silkolene Honda Motocross GP" in both HAL & RGB configurations.

v2.04.82
fix: fixed wrong prototype and log message for SmackOpen call
fix: prevent window style changes for movie windows
fix: added "MSCTFIME UI" window to movie windows - found in "Extreme Boards Blades"
fix: surface stack bounds - fixes "The Sims Deluxe Edition" primary surface lost.
fix: surface stack ExplainSurfaceRole() returning "(PRIM)" for lpdds=0
fix: bugged SUPPRESSOVERLAY flag - acting on wrong flags!
fix: bugged NOHALDEVICE flag - acting on wrong flags!
fix: bugged pixel format for RGB565 texture/surface dumps
add: added debug option "Dump surface updates" (DUMPSURFACES)
add: added SUPPRESSFOURCCBLT flag to suppress all FOURCC ddraw capabilities - fixes "The Sims" intro movies
add: GUI: pixel analyzer added to palette view

v2.04.81
add: CDROMPRESENT flag to pretend the cd drive has a CDROM inside
add: virtual CD audio - emulation for mciSendString "status cdaudio media present" command
add: virtual CD audio - emulation for mciSendString "status cdaudio type track <n>" command
fix: NOFLIPEMULATION mode with renderers with DXWRF_USEBACKBUFFER property
fix: GUI icon update using *.ico files
fix: as suggested by caniza mimorix, added UnmapViewOfFile(pStatus) statements in dxwnd.cpp
fix: FIXPITCH logic applied also to non-emulated Lock/Unlock operation (extUnlockDir)

v2.04.80
fix: fixed RETURN and other special keys passed from target program to GUI - final fix with timestamp control
fix: eliminated error log message for window point conversions when current window is 0 - reduces useless log lines
fix: log message for window style update - was reporting identical values before and after the operation
fix: replaced SetWindowsHookEx callback for WH_KEYBOARD_LL hook and NODISABLEALTTAB flag that more gently filters SYSKEY events only. Needed for "Recoil".
fix: renamed HOOKNOUPDATE flag as HOOKDOUPDATE to reflect the real behavior (the flag enables the hook updates!). 
fix: on the GUI, "No hook update" was changed to "Do hook update".
fix: simplified HOOKDOUPDATE logic - no changes, hust more readable
add: "NoComplexMipmaps" (NOCOMPLEXMIPMAPS) flag to improve compatibility with "Extreme-G2" pc game.

v2.04.79
add: FIXFOLDERPATHS tweak to fix the "." dot-ending paths in "Drift When Worlds Collide"
add: preliminary ddraw tracing wrapper
fix: surface stack bug triggered by "Drift When Worlds Collide"
fix: PrimaryStretchBlt and PrimaryBilinearBlt routines when backbuffer is not attached. Allows AEROBOOST and bilinear filtering on "Drift When Worlds Collide".
fix: log message in ddraw SetDisplayMode wrapper for ddraw 2 & 3 interfaces
fix: revised ddraw::EnumDisplayModes for non-emulated windowed modes: fixes "Tomb Raider III" empty video modes list
fix: revised ddraw:SetDisplayMode to suppress unnecessary video mode changes when not necessary
fix: fixed DXW_RENDERER_EMULATEBUFFER = "primary buffer" mode, tested with "Tomb Raider III Gold" on Win7 (still broken on WinXP)
fix: fixed DXW_RENDERER_LOCKEDSURFACE = "locked surface" mode, tested with "Tomb Raider III Gold" on Win7+WinXP

v2.04.78
fix: forces initial resolution when specified instead of using fixed default 800x600
fix: dialog handling: in window mode append the dialog to virtual desktop instead of 0 - fixes "Sentinel Returns"
fix: dialog handling: in window mode suspend window mode while in dialog creation callback - fixes "Sentinel Returns"
fix: recovered LIMITDDRAW flag behavior (useless?)
fix: bug in attempt to trace WM_INPUT message content
fix: added 400x300 resolution limit where missing
fix: GUI Direct3D default for vertex processing selector is now "default".
fix: D3D2 SetTransform hook to wrong address (in TRACEALL mode only)
add: kernel32 IsProcessorFeaturePresent wrapper for tracing
add: ChaosOverlordsFix patch (CHAOSOVERLORDSFIX) in tweaks section. Maybe not the final word, but it works ...

v2.04.77
add: "Custom" (CUSTOMRES) resolution flag to add a non-standard resolution video mode to the emulated list
add: added some supported emulated resolutions: 400x300, 1152x864, 1280x960, 1400x1050.
add: main hWnd value shown in GUI status panel
add: preliminary "blacker" tool & launcher menu command
fix: DXWNDSTATUS shared area no longer cleared after 5 seconds from game start from DxWnd GUI
fix: GUI problem with GetOpenFileName call in dxw Import command breaking the early hook modes.
fix: GUI RestrictedMap not applied when father/son hooks are required. Fixes "Die Hard Trilogy"
fix: EnumTextureFormats wrapper crashing with CLEARTEXTUREFOURCC flag
fix: deleted improper D3D8 Setviewport coordinate scaling - "Zombie Shooter" fixed again.

v2.04.76
add: "Set emulated CD volume" (SETCDVOLUME) flag to fix CD music volume level
add: hook IDIrectDraw<n>::Compact method currently unimplemented. Fixes "Hyper Crazy Climber" initial error.
fix: ddraw debug message in PrimaryEmulated routine

v2.04.75
GUI: /x argument to suppress icon in icon tray
GUI: "Explore DxWnd" expert menu command
GUI: fixed RETURN and other special keys passed from target program to GUI
fix: code reorganization
fix: palette handling for 8 bit plain surfaces, useful for USM98 and FM98
add: "Fix mouse RawInput" (FIXMOUSERAWINPUT) flag

v2.04.74
add: "IgnoreDebugOutput" (IGNOREDEBOUTPUT) tweak: emulates MS "IgnoreDebugOutput" shim.
add: apply FAKEHDDRIVEC and FAKECDDRIVED flags on avifile.dll pathnames. Allow "F16 Aggressor" RIP to relocate movies
fix: if setting the main palette to a new 8bit surface fails because the new surface is connected to a different DDraw, use a new palette. Fixes "Ultimate Soccer Manager 98" black screens.
fix: added RPC_E_CHANGED_MODE error code 0x800010106 to helper. Happens in "Rubik's Games"
add: D3D8 Get/Setviewport wrappers for logging
add: preliminary YUY2 fourcc handling in SDL/SDL2 renderers: work with "Rubik's Games" intro movies
add: waveOutSet/GetVolume wrappers
fix: added emulated CD player the possibility to PLAY from a SEEK-ed track. Happens in "Rubik's Games"
add" NoOleInitialize (NOOLEINITIALIZE) tweak to suppress harmful OleInitialize & CoInitialize calls. Fixes "Rubik's Games" missing sound.
add: GUI shows target number in target update panel. It helps finding the target number for /R:n command line option
add: "Limit DIB operations" (LIMITDIBOPERATIONS) flag to apply delay to GDI32 DIB operations only. Helps improving "Rubik's Games" animations.
fix: auxGet/SetVolume bad prototypes
add: waveOutGet/SetVolume wrappers
add: FourCC customary management (No/HW/SW) in ddraw 

v2.04.73
fix: bypassed window operations in opengl wrappers when not in window mode
add: "ForceClipChildren" (FORCECLPCHILDREN) tweak: fixes "Psychotoxic" flickering
add: "PushActiveMovieWindows" (PUSHACTIVEMOVIE) tweak: useful to make ActiveMovie window invisible
add: "PreventMinimizeWindow" (PREVENTMINIMIZE) tweak: usable with "Anachronox"
add: "DisableAltTab" (DISABLEALTTAB) tweak: disables Alt-tab and Alt-Esc keys during gameplay
add: "NoAccessibilityKeys" (NOACCESSIBILITY) tweak: disables prompting for accessibility keys definition
fix: DISABLEDISABLEALTTAB renamed as NODISABLEALTTAB and moved to tweaks section
add: preliminary wrapper for ddraw EnumSurfaces and GetSurfaceFromDC methods

v2.04.72
fix: Direct3D8/9 CreateAdditionalSwapChain wrapper: simplified and fixed to make "Psychotoxic" movies working
add: desktop font parameters saved and recovered by "Tools->Recover screen mode" command. Fixes lost font smoothing when "The Longest Journey" terminates.
add: "FULLPAINTRECT win:FullPaintRect" tweak to force full rect values for GDI BeginPaint. Fixes "RollerCoaster Tycoon 2" partial screen updates.
fix: addwd MCI_CLOSE message to messages processed by "Bypass MCI calls" - could avoid possible errors
add: SKIPFREELIBRARY flag to avoid FreeLibrary recursive call - fixes "Disciples 1"
add: FIXALTEREDPATH flag to fix LoadLibrary legacy with LOAD_WITH_ALTERED_SEARCH_PATH flag - fixes "Anachronox"
fix: better (smaller) stramlined version with no trace messages


v2.04.71.fx1
fix: wrapper for D3D3/7::EnumZBufferFormats
v2.04.71
fix: wrong prototypes for EnumTextureFormats D3D versions 1 & 2
fix: a few log messages in OpenGL wrapper
fix: ChangeDisplaySettings wrapper in rare condition where there is a request for big window and no previous main window - fixes Psychotoxic with DxWnd compiled in debug mode.
fix: GetPalette wrapper now returns the correct error code DDERR_NOPALETTEATTACHED when asking the palette for primary/backbuffer surfaces with color depth different from 8 bits - needed for "Team 47 GoMan"
fix: D3D8/9 CheckDeviceType wrapper - fixes "Beyond Divinity" error

v2.04.70
fix: DirectSound hooking further partial fixing
add: DirectSound global focus disposition
add: "Set DPI awareness" flag now also acting within DxWnd.dll
fix: <return> key on GUI no longer clears the input tabs
add: <del> key on GUI as shortcut for delete program
add: <return> key on GUI as shortcut for run program
add: <insert> key on GUI as shortcut for new program
add: <ctrl-cancel> key on GUI as shortcut for program kill
fix: moved "Libs / Load saved gamma ramp" flag to "DirectX(2) / Enable gamma ramp emulation"
add: Gamma control panel status now fully saved on configuration file
fix: shifted labels on dxwnd.log file for some trace flags (i.e. OUTSOUNDTRACE reported as OUTINPUT)

v2.04.69
fix: added bypass of GetDirect3D method in D3D8/9::Reset wrapper when CACHED3DSESSION is set. Fixed "GTA San Andreas" crash when run by Steam interface by Telingur.
fix: set EXCEPTION_CONTINUE_SEARCH on 0xc0000008 exception. Fixes Dungeon Keeper crash in menues.
fix: DirectSound hooking partial fixing
fix: on 3DDEVICE ddraw surfaces add VIDEOMEMORY|LOCALVIDMEM only in case no memory type is set, leave SYSTEMMEMORY otherwise ...

v2.04.68.fx2
fix: AVIFileOpenA/W prototypes, eliminate crash when hooking
fix: added WM_NCPAINT and WM_NCACTIVATE to defaulted messages to ensure non-client area repaint back from Win+D minimize
v2.04.68.fx1
fix: avoid capability registration for 3DDEVICE & TEXTURE surfaces to avoid surface stack saturation. Fixes "Forsaken" whitened textures.
fix: recovered the repositioning of first small window (when their size is lesser than the declared screen resolution) when the "Fullscreen only" flag is set.
fix: streamlined dll compilation project
v2.04.68
add: Lock24BitDepth (LOCK24BITDEPTH) to suppress ddraw emulation of 24 bit color depth. Helps improving "Tlon" that has many avi movies with unsupported 24 bit color depth, better rendered at 32 bit.
fix: fixed D3D flags for D3D1 interface - WIREFRAME, CLEARTARGET, ZBUFFERCLEAN, ZBUFFER0CLEAN
add: preliminary addition of D3D1 debug flags FORCEZBUFFERON, FORCEZBUFFEROFF, PATCHEXECUTEBUFFER
fix: erased duplicated vSync delay in ddraw emulation
fix: reorganized avifile.cpp module: more hooked functions, TRACEALL compile flag, SYS logging ...
fix: user32.cpp - avoided recursive WindowProc hooking of child dialog windows. Fixes "Red Alert 2" serie games.

v2.04.67
fix: RECOVERSCREENMODE flag based on registry screen settings - fixes "CandyLand" for Win95
add: Forces16BitZBufferBitDepth tweak (SETZBUFFER16BIT) to force 16bit ZBufferBitDepth - fixes 3D Slot Car Racing
add: Forces24BitZBufferBitDepth tweak (SETZBUFFER24BIT) to force 24bit ZBufferBitDepth - just in case ...
fix: ddraw surface capability settings for 3DREF & ZBUFFER surfaces in non emulated modes - makes "Motocross MAdness 2 trial" playable in all possible modes

v2.04.66
add: new GUI with Msgs tab in expert mode, all joystick related flags in Input tab
add: "System libs" trace flag to log system libraries events separately
fix: skip hooking of NTDLL.dll module
fix: suppression of LOAD_LIBRARY_SEARCH_SYSTEM32 flag from LoadLibraryEx calls - fixes Heavy Gear
fix: known dll array searched with ANSI dll name
fix: OpenGL hooking
fix: restored D3D1 ClearViewport hook
fix: recovered ddrawex.dll CreateDirectDraw hook - fixes "Theocracy" regression problems since v2.04.54
add: hook to ddrawex.dll, hook to ddraw session created by IID_DirectDrawFactory object
add: DXW_NOTRACES macro to compile streamlined dll

v2.04.65
fix: debug mode hook debugging loop: let the exception handling routines free to run
fix: inject mode hook: added syncronization event to ensure hooking initialization is completed before running the program
fix: fixed DISABLEMMX flag and added CPUDISABLEPBE flags to turn CPU PBE capability off - fixes Cryo "Arthur's Knights" CPU hacks
add: better D3D object tracing
fix: new html help files

v2.04.64
add: new flag "Video / Set DPI awareness" (SETDPIAWARE)
fix: various fixes on DirectInput support for virtual joystick
fix: some SDL log message priorities
fix: separated hexdump tracing macros
add: preliminary xinput Xbox 360 controller emulation, new flag "Hook / Hook XInput" (HOOKXINPUT)
add: joystick preliminary deadzone control
add: Xbox 360 controller flags to assign mouse reading to left / right / both sticks
fix: new joystick icons

v2.04.63
fix: logging system review
fix: improved DirectInput support for virtual joystick

v2.04.62
fix: removed NUMCOLORS case in GetDeviceCaps wrapper, fix in v2.04.60. Damages ....
fix: added dinput wrapper for CreateDeviceExW widechar method
fix: dinput QueryInterface wrapper to handle both DirectInput and DirectInputDevice objects (W.I.P.)
add: "Inputs" trace flag to log joystick and dinput events separately
add: preliminary DirectInput support for virtual joystick
add: detailed logging for dinput and vjoystick objects

v2.04.61
add: GUI contextual help
fix: GUI "crossfire" replaced by "crosshairs" 
fix: GUI eliminated de-serialize pop-up window
fix: GUI fixed dirty initialization of "registry" & "notes" fields
fix: eliminated "ScaleWindowExtEx" trap message (call found in "Lego Marvel Superheroes" by Mchaidang)
fix: recovered static linking to MinHook to avoid early hook regression bug since release v2.04.58.fx2
fix: updated help with virtual joystick panel

v2.04.60
fix: ddraw GetAttachedSurface wrapper returns backbuffer also when asked for DDSCAPS_FLIP capability from backbuffer. Needed by "Honour and Freedom".
fix: added DIB stretching to SetDIBits wrapper in GDI stretched mode. Needed by "Honour and Freedom".
fix: in 8 bit DIB emulation fixed case where the Quad entries were not initializad. Fixes crash in "G-Nome".
fix: deleted unreferenced code for LockScreenMode static routine
fix: added NUMCOLORS case in GetDeviceCaps wrapper. Unnecessary?
add: new "Swallow mouse move messages" (SWALLOWMOUSEMOVE) flag - Fixes "Akte Europa" blocked rendering while moving the mouse
add: new "Open config ..." expert mode command. Allows switching multiple config files in addition to default dxwnd.ini
fix: -c:<path> command line argument fix. Ensures the path is referred to DxWnd folder.
fix: possible fix to GUI duplicate entry bug
add: banner message SpeedRun OK/KO to tell usage of speedrun banned flags

v2.04.59
add: View->"CD Player" panel
fix: more enhancements in CD audio emulator, plr_seek and plr_tell interfaces
fix: linked updated Minhook dll with dynamic linking
add: "Limit D3D BeginScene" (LIMITBEGINSCENE) flag to limit FPS in D3D games avoiding doubled delay
add: "Ignore Device Id." (IGNOREMCIDEVID) flag to ignore the device id field in mci cd player operations
fix: "Lock volume" now prevents alse mci cd player volume changes

v2.04.58
fix: more enhancements in CD audio emulator

v2.04.57
fix: various enhancements in CD audio emulator

v2.04.56
add: "Virtual CD Audio" (VIRTUALCDAUDIO) flag for CD audio emulation (thanks to Tony Spets source code)
add: "Load saved gamma ramp" (LOADGAMMARAMP) flag
add: "Hook / Skip Hint in IAT seq. scan" (SKIPIATHINT) flag: fixes "The Black Mirror" hooking problems. 
add: "Refresh on palette Realize" (REFRESHONREALIZE) flag to properly handle "Titanic Adventure out of time".
fix: fixed Flip wrapper for non-emulated & non-windowed case with NULL source.


v2.04.55
add: "PAL DIB Emulation" (PALDIBEMULATION) flag to emulate 8bit palettized DIBs: fixes "G-Nome".
fix: SetGammaRamp log message
fix: DumpDibSection debug utility making up-side-down images of DIBs with negatve height.
fix: moved Set/GetDIBColorTable hookers from wrong place USER32 to correct place GDI32
fix: GDI viewport origin scaling in case of main window resizing - needed for "G-Nome"

v2.04.54
add: Suspend time stretch flag - tentative apply to avifile lib
fix: reorganized c code for hooking through SysLibsTable array
fix: capability to both hook a library and its dependencies - needed in Smackw32 hooking for Suspend time stretch
fix: SDL and SDL2 libs hooking made independent
fix: reduced maximum hexdump data size - avoids crash in bonkheads registry trace
fix: cleaned smackw32 wrapper
fix: separated flags for smackw32 vs. binkw32 hooking
fix: DirectSound API ordinal numbers
fix: virtual gamma control for 16bpp real desktop mode
add: custom key toggle
add: continuous screen capture for emulated ddraw 
add: virtual gamma ramp control to D3D8/9 lights
add: debug flag STARTWITHTOGGLE to initialize custom key toggle to active
add: LIGHTGAMMARAMP to enable gamma ramp correction in D3D lights
---: code reorganization - debug flags moved to debug configuration word

v2.04.53
fix: again, another bug in ddraw surface stack. Fixes "Rival Realms" blitting problems.
add: virtual gamma control for ddraw surface emulation 32bpp desktop, GUI + dll.
add: Suspend time stretch flag to play movies at normal speed and avoid audio shuttering (implemented for bink and smack videos only so far)
add: CPU config panel to experiment with CPU features (developer's only!)
add: experimental CPU debug flags
add: slow down window polling flag (SLOWWINPOLLING) to reduce CPU load in some programs
fix: interception of window messages in NOWINPOSCHANGES flag: fixes "eXtreme Bull Rider" displacements

v2.04.52
add: preliminary handling of FOURCC YUY2 surfaces with ddraw:ManageFourCCSurfaces tweak. Fixes movies in "Kirikou" and "King of Fighters Maximum Impact" emulator. 
fix: bug in IDirectDrawSurface::GetAttachedSurface wrapper with wrong caps initialiation for CAPS2 structure
fix: D3D8::GetRenderTarget wrapper with wrong prototype
fix: added MessagePump in D3D8/9::Present method to avoid game crash in movie tight loops
add: log wrappers for D3D8/9 GetRenderTarget and CreateImageSurface methods
add: GUI recover gamma ramp menu command
add: GUI view gamma ramp panel
add: automatic gamma ramp recovery on DxWnd termination

v2.04.51
fix: fixed criteria to avoid "Kiss Pinball" tables to show partially
fix: fixed wrong prototypes for Direct3D8 Get/SetGammaRamp prototypes causing game crash
fix: GetScanLine log message
fix: missing real screen size initialization
add: FORCED3DGAMMARAMP to enable gamma ramp control from windowed game, though changing the whole desktop. Makes "Morrowind" more playable
add: GUI - 4 vs. 32 bit icons support
add: GUI - grayed icons for unreacheable targets

v2.04.50
fix: wrong log messages for Direct3D(3) Viewport methods
fix: wrong vectored exception handlers sequence, prevented CPUID & RTDSC replacement exceptions to work together with generic exception handler
fix: CPUID emulation now trims MMX + SSE + SSE2 bits
fix: implemented maximum entries limit per role type in surface stack. Fixes "Premier Manager 98" problems with 3DREF type surface overflow
fix: log UpdateWindow event at debug level to avoid lengthy logs
fix: GUI icon view known bugs
fix: GUI disables filter combo-box when not applicable depending on renderer
fix: GUI process kill dialog box is now TOPMOST to be always visible (thanks to FunkyFr3sh suggestion)
fix: GUI defaulted icon view mode
add: more debug logging on D3D SetRenderState wrapper

v2.04.49
add: virtual drives - tested with "Atripolis 2097" that would require some files in C:\A folder!
add: game icons saved on exported configuration files
add: GUI - icon view
fix: CoCreateInstance wrapper handling the odd case of corrupted hook pointer. Fixes "Gods Lands Of Infinity Special Edition" with default DxWnd settings
add: new optimized version of Scale2X filters, with support for MMX features, courtesy of A. Mazzoleni from AdvanceMAME
add: Scale2K filters, again courtesy of A. Mazzoleni from AdvanceMAME
fix: GUI default program name now gets stripped from .exe suffix
bug: known bug - adding or importing a new program can break the icon view until next DxWnd start.

v2.04.48.fx1
fix: exception handling for exception codes minor than 0x80000000
v2.04.48
fix: exception routine for assembly instructions added with AddVectoredExceptionHandler. Fixes "Dark Judgement".
fix: corrected misleading PeekMessage wrapper logs
add: more DirectSound wrappers / logs
fix: DirectSound mute option against buffers starting automatically without Play method - fixes mute option with "Digging Jim"
add: wrapper for KERNEL32:WinExec call for SUPPRESSCHILD flag - prevents registration form with "Digging Jim"
fix: bug in UnmappedScreenRect desktop heigth calculation
fix: moved CreateVirtualDesktop routine to dxw generic class
fix: fixed CreateVirtualDesktop window properties according to position & style modes
add: added REPLACEDIALOGS flag to relace troublesome dialog windows - fixes "Dark Rift" in windowed mode
fix: reduced DxWnd banner flickering outside main window
add: added option /h for GUI to stay hidden in icon tray after program termination

v2.04.47
add: circular logs mode. dxwnd.log is filled with up to 50000 messages and then swapped to dxwnd.0.log
add: SDL2 renderer
add: DISABLEMMX flag: fixes "The New Adventures of the Time Machine"
add: ADDTHREADID log flag: adds thread id to log messages
fix: added some dlls to system unhooked list: oleaut32, uxtheme, setupapi, dwmapi
fix: in user32 EnumDisplaySettings wrapper, returning current virtual desktop size instead of fixed 800x600
fix: SDL_BlitSurface wrapper on NULL src condition - fixes "Battle for Wesnoth" rendering
fix: cleared clipper in SDL and SDL2 renderers - fixes "Abuse" with SDL
fix: various log fixes

v2.04.46
fix: improved GUI dpi-awareness logic - fixes Win7 125% case
fix: DWL_DLGPROC handling - fixes "Return to Mysterious Island" and more ...

v2.04.45
add: GUI preliminary dpi-awareness logic
add: GUI preliminary "Sound" tab (DirectInput only)
add: Sound preliminary "Sound mute" and "Lock volume" flags (DirectInput only)
add: Sound trace
add: precise timing flag to control CPU speed with a more precise timer than system Sleep.
fix: GetProcAddress wrapper must skip SysAllocString/SysAllocStringLen calls to avoid reursion

v2.04.44
fix: fixed log message about window position
fix: fixed window styles attribution
fix: fixed SDL SDL_SetVideoMode wrapper failing to assign main window handle
fix: GUI fixed executable file lock (caused by icon extraction) preventing executable detetion until DxWnd was closed
fix: reduced scope of REPLACEPRIVOPS option: not on entire text segment but only from entry point to end. Reduces (but not clears) chances of a improper non-assembly replacement.

v2.04.43
add: D3D8/9 Force Vertex Processing option
add: hooker for SDL SDL_PollEvent call to fix mouse coordinates: fixes 
add: hookers for SDL2 calls SDL_Renderer, SDL_Window, SDL_CreateTexture, SDL_UpdateTexture
fix: in intSDL_Refresh skip operation when width or height is zero. Fixes "BosWars" crash.
fix: in SDL SLL_SetVideoMode added refresh cursor clipping coordinates. Fixes "BOS Wars" initial clipping state.
fix: in SDL SDL_CreateWindow hooker updated args to adapt to windowed mode.
fix: added tracing of GDI CreateDIBitmap return code.
fix: do not elect to main window any window smaller than fullscreen. Fixes (partially) some spash screens, e.g. "Talvisota Icy Hell".
fix: in hook initialization, revised operations that must run once only.
fix: in dxwCore::FixWindowFrame fiked potentially fake error conditions that may lead to partial operations. 
fix: GUI missing initialization of window style selector
add: integration with Scale2X SSE version, great speed boost!

v2.04.42
add: ZeroDisplayCounter tweak to force 0 return code on ShowCursor(FALSE): fixes "Leopold the Cat" initial loop.
fix: uninitialized variable in ShowCursor hooker
fix: better tracing on IDIrectDraw::GetCaps hooker
fix: better implementation of exception handling
upd: preliminary implementation of Scale2X filters 2x and 3x with mp support to speed operations with multi-core CPUs

v2.04.41.fx2
fix: added StartHook call to proxies to ensure WinProc hook - fixes virtual keys in fullscreen mode
fix: added dummy implementation for SetAppCompatData entry in ddraw.dll proxy for WinXP platform with no entry in real ddraw.dll library
v2.04.41.fx1
fix: fixed uninitialized RECT structure when blitting in fullscreen mode
v2.04.41
fix: fixed time stretching initialization for proxy hooking mode
fix: fixed virtual keys initialization for proxy hooking mode
fix: added search for slash '/' separator in addition of backslash '\' in LoadLibrary path cleanup: fixes problem in OpenGL library detection for "Crime Cities"
add: KillBlackWin tweak for unwanted window suppression: fixes (eliminates) "Kane & Lynch 2" black background window
fix: logging and error condition in USER32 RedrawWindow wrapper
fix: logging in USER32 EnumWindows wrapper
add: "Fix PCM audio" flag to support stereo PCM audio by downgrding the audio channels to 1 (mono). Fixes "Capcom Arcade Hits Vol 1" audio problems.
fix: GUI fixed path for tweaks.ini help file

v2.04.40
upd: code reorganization to support renderers
fix: ddraw proxy breaking aero compatibility
fix: SDL renderer flickering problem

v2.04.39
add: OpenGL tentative wrapper for glDrawArrays
add: OpenGL tentative dump for GL_LUMINANCE textures
fix: OpenGL fix for texture size in dump
add: OpenGL texture dump protection by try/catch statemens
fix: fixed GUI using deallocated data in Debug and Injection run modes
fix: fixed GUI broken menu link for Proxy->dinput proxy install command
fix: fixed GUI Debug mode run handling of breakpoint exceptions: the Debug mode now works on WinXP
fix: fixed GUI determination of run program start address: the Debug mode now works also in case of ASLR (Address Space Layout Randomization)
add: better GUI diagnostic in case of run error
add: added FlushInstructionCache in Debug mode run. Useful?
add: GUI messagebox to acknowledge proxy installation

v2.04.38.fx4
add: added hook for glTexSubImage2D
fix: hooker for wglGetProcAddress redirects not only wgl* calls but also gl* calls (needed in "BStone" port of "Blak Stone")
add: tentative dump of GL_LUMINANCE and GL_RGBA textures (not working)
add: DxWnd proxy through dinput.dll 
fix: fixed broken flag for "full bilinear filtering"
fix: deleted "Deinterlace" tweak, moved to filters
v2.04.38.fx1/2/3
fix: dxwnd proxies
fix: filter Scale2X request for images smaller than 2 pixels wide / heigth
fix: FastBlt operations with wrong (too big) RECT coordinates. Fixes some "Road Rash" problems.
fix: DxWnd.exe logfiles created on current working directory - fixed using initial absolute path.
v2.04.38
add: semi-pluggable filters: Scale2X, HQX, PIX, Interlace filters
add: experimental SDL1.2 renderer
new: dxemublt.cpp major code reorganization
new: enlarged communication area - possible errors when running previous releases before this one!!!
fix: "Addword Abe Oddysee" patch: don't emulate the strange ddraw::BltFast operation on single video lines
add: thanks to Andrea Mazzoleni, Maxim Stepin & Cameron Zemek  for filters source code
add: fancy thanks scrolling

v2.04.37
add: virtual registry handling of hex(n) data types - needed for "Nightmare Creatures"
add: virtual registry preliminary proxy for RegEnumKey and RegEnumKeyEx tracing
fix: virtual registry logging
fix: fixed problem with empty copies of dxwnd.ini left in the file system
fix: fixed missing update of dxwnd.reg file when virtual registry is updated

v2.04.36.fx3
fix: "Emulate 16bpp D3D8 backbuffer" was working only with D3D trace on (regression introduced in v2.04.36)
fix: D3D8/9::Present method, destination rect should be scaled only if not NULL (regression introduced in v2.04.36)
fix: ChangeDisplaySetting* should ignore CDS_SET_PRIMARY when in windowed mode - fixes "Fastlane Pinball"

v2.04.36
fix: revised logic to switch to fake-fullscreen modes through toggle keys
add: NOTASKBAROVERLAP flag to prevent window to overlap the desktop taskbar
fix: removed attempts to always hook D3D10/11 interfaces
fix: bad reference passed to HookDirect3D8/9 for IDirect3D8/9 pointer
add: hookers for tracing IDirect3D8/9::Release calls
fix: "Suppress Direct3D8/9 reset" now skips more operations (safer)
fix: D3D8 16bit color emulation working tentatively also for not NULL rects
fix: D3D8 16bit color emulation activated only when necessary (on selected 16bpp backbuffer format)
fix: IDirect3D8/9::Present surface scaling in more general conditions
add: trace for SDL2:SDL_SetWindowFullscreen call
add: experimental CACHED3DSESSION flag

v2.04.35
fix: fxed missing clear of surface stack for primary surface - fixes crash on auto refresh option and in particular fixes "M1 Tank Platoon 2"
fix: logic to avoid window overlap on top of desktop taskbar
fix: window client coordinates fix to avoid mouse ointing errors avter window trimming
add: completed implementation of "thin frame" option
fix: some SDL alpha channel functions not remapped to real video surface

v2.04.34
fix: suppressed duplicated coordinates correction in PeekMessage when message hook is active
fix: window position when the wanted size is too big to fit
fix: DxWnd icon on system tray recovery on explorer.exe restart and WinXP platform
add: more OpenGL gl* and wgl* functions tracing
fix: OpenGL window styles problems - now on "18 Wheels of Steel" fake fullscreen modes are borderless
add: SCALEGLBITMAPS flag for runtime scaling of OpenGL glBitmap bitmaps - usable in "Devil Whiskey"
add: HOOKWGLCONTEXT flag to conditionally hook OpenGL wgl extensions 
fix: main window remapping & resizing moved from wglCreateContext to wglMakeCurrent wrapper and fixed to avoid flickering
fix: FREEZEINJECTEDSON debug flag
add: GUI "Run suspended" command to start a program with DxWnd injection and blocked waiting for debugger attach
fix: SDL_FreeSurface wrapper handling the possible duplicated free of video surface - fixes "Battle for Wesnoth" crash
add: more SDL wrappers

v2.04.33
fix: uncompressed icon resources for WinXP compatibility
add: support for setting / removing DxWnd proxy hookers
fix: blit to primary surface protected by try/catch exception handlers: improves "Empire of the Fading Suns" robustness
add: logs: window message log decoding legacy messages 0x90-0x95
add: logs: logging Direct3DDevice::Execute flags 
add: logs: logging user32:ReleaseCapture calls
add: FIXEMPIREOFS tweak to make runtime patch for "Empire of the Fading Suns" 
fix: wrapper for glRasterPos4fv scaling x,y coordinates: partially fixes "Devil Whiskey" text on screen
add: texture dumps for GL_RGB textures (bad colors...)
fix: updated user32:MoveWindow wrapper for BIG windows move operations
fix: tweaks lists now support multiple selection

v2.04.32
add: preliminary hooking of ddraw GetSurfaceFromDC API (not the method!) with dialog box for detection
fix: try / catch to manage exceptions in log routine
add: custom banner dxwanim.bmp. Could be deleted or replaced ...
fix: GDI32 BitBlt hooker doesn't use StretchBlt for intra-memory blits - fixes "Avernum 3" transparency problems
fix: desktop hider: deleted call to UpdateWindow - fixes the crash of "Star Trek Birth of the Federation" in "hide desktop" mode
fix: USER32 ScrollWindow wrapper crashing when lpRect or lpClipRect arguments were NULL. Fixes "Mig Alley" crash.
fix: fixes implementation of ddraw RestoreDisplayMode wrapper. Useful for "Mig Alley".
fix: added support for definition of default desktop size & color depth. Useful for "Mig Alley".
add: experimental tweak "MAKEWINVISIBLE" to force visibility of windows queried to check for visibility
add: preliminary SDL overlay handling.
add: tracing of UnhookWindowsHook system call
fix: proper handling of ChangeDisplaySettings(NULL, 0) condition. Useful for "Mig Alley".
add: "EMULATEMAXIMIZE" flag to assign proper size & position to maximized windows. Useful for "Mig Alley".
fix: trimming of window size & position in MoveWindow wrapper. Useful for "Mig Alley".
add: GUI - preliminary tweaks contextual help

v2.04.31
add: very preliminary hooking of sound system wail32.dll
add: lock window z-order to top
new: updated artworks, thanks to ZeroX4
fix: LegacyD3DResolutionHack integration bug
fix: SDL hooking, added methods allow emulation of DOSBox screen

v2.04.30
add: dedicated time stretched operation trace
fix: preliminary SDL handling fixes
fix: gsky916 suggested fix for shared dc leakage
fix: gsky916 suggested fix for GDI blit FPS control
add: gsky916 suggested add for "mouse move by event" flag. Replaces "SetCursorPos" calls with equivalent "mouse_event". Fixes WF1, another game of wind fantasy series.
fix: GetWindowRect hooker when called to get desktop size. Fixes "Avernum 3" demo original version.
gui: independent expert and debug modes
gui: tweaks tab for infrequently used boolean flags
fix: missing screen refresh on fast palette transitions - fixes "Jane's Fighters Anthology" striped effect
add: new tweak LegacyD3DResolutionHack to remove resolution limitations from legacy d3d interfaces
fix: handling of D3D surface capabilities - makes "Drakan the Order of Flame" demo working

v2.04.29
fix: CallWindowProcW log error
fix: improved size & position locks in WindowProcess message procedure
fix: MCIWndCreate flags for movie stretching: fixes "Megaman 3" and "Megaman 4" movie stretching
add: more window messages decoded in log file
add: preliminary "hide joysticks" flag
add: preliminary SDL handling

v2.04.28
add: fourCC tracing, added symbolic string when available
fix: opengl wglCreateContext sets window size, position & style according to configuration
fix: opengl glDrawPixels wrapper scales 2D texture, fixes the "Crazy Marble" initial splash screen
fix: opengl glGetString & gluGetString wrappers causing a crash when trace is enabled
fix: GDI32 RoundRect wrapper to scale coordinates - fixes "iF22" menu graphic 
add: added hooking "No scan" method to avoid hooking scrambled IAT - fixes games of Sierra Hoyle series
fix: cursor handling in centered and fake-fullscreen modes

v2.04.27
fix: virtual registry errors for hex keys - fixes "Marble Drops" RIP registry access
fix: ddraw GetAttachedSurface wrapper returning backbuffer attached to primary on proper conditions - fixes "Frogger 2 Swampy's Revenge"
fix: ddraw GetClipList wrapper - fixes "New Robinson" problems when logging is not set
add: D3D9::GetFrontBufferData wrapper for method emulation while in fullscreen mode - fixes "Hitchhiker's Guide to the Galaxy remake"
add: "hot registry" flag, experimental, to force hooking & logging of all registry operations 

v2.04.26
add: LOCKFULLSCREENCOOP flag "Lock FULLSCREEN coop level" to prevent return to normal coop level in non windowed mode, used in "Speedboat Attack" fullscreen.
add: frame counter limitation added to D3D1-7 BeginScene method
fix: Texture dump crash for D3D8-9 textures
fix: proper return codes for NODIALOGS mode
fix: logs for LoadLibraryW wrapper
add: NOBAADFOOD flag "No BAADFOOD" to avoid (virtual) heap initialization to 0xBAADFOOD value, setting to 0x0 instead. Needed to run "Independence Day" RIP.

v2.04.25
fix: better handling of DirectDrawPalette refernce counters, incremented only once
add: SAFEPALETTEUSAGE option to fix DirectDrawPalette use and abuse (fixes "Speedboat Attack")
fix: handling of smack descriptor trace in case of NULL pointer
add: NODIALOG option to suppress error messages on dialog boxes
fix: added coordinate remapping to mciSendString "put window client" command - fixes "Twisted Metal" video coordinates 

v2.04.24
fix: applied correct capabilities to attached ZBUFFER surface. Fixes "Wipeout XL".
fix: ddraw SelectPalette wrapper. Fixes "Yu-No" and "speedboat attack" crashes.

v2.04.23 - "The Sting!" release
fix: bug in DirectDrawSurface::Lock() wrapper for "locked surface" mode: service surface is created with fixed arguments
fix: proper handling of DirectDrawSurface::Blt() method when using DDFLAGS_FILLCOLOR flag
fix: PrimaryStretchBlt fixes, propagation of Blt dwflags and lpddbltfx arguments

v2.04.22 - "The Italian Job" release
add: preliminary integration binkw32.dll
add: new "No mouse event generation" flag
add: new "byte aligned" IAT navigation schema for IAT with no OFT indication
add: preliminary USER32 SendInput hook
fix: DirectDrawEnumerate/Ex wrappers, default monitor device name "display" now considered equivalent of ".\\DISPLAY1"

v2.04.21
fix: proxed calls to SetCooperativeLevel with no specification for screen mode changes
fix: locked rect fix applied only in case of not NULL rect specification. Fixes "Shadow Watch" font blitting problems.
fix: DirectDrawEnumerate/Ex wrappers now filter video devices properly when a given monitor is selected. Partially fixes "The Sting!" behaviour on multi-monitor environment.
fix: aligned primary surface capabilities for flipped native and emulated modes
add: wrappers for ddraw7 StartModeTest and EvaluateMode methods only for logging 
fix: wrapper for SetCooperativeLevel proxy the call when flags is DDSCL_FPUPRESERVE or DDSCL_FPUSETUP only. Seen in "G-Darius"
add: separated logging flag for structures hexdump
fix: eliminated initial logging for undefined virtual keys (useless)
fix: fixed unconditional logging for USER32:PolyLine
add: diagniostic DC dump in GDI32:SetDIBitsToDevice wrapper
fix: GDI32:SetViewportOrgEx wrapper when ddraw surface is still missing
add: in ddraw::QueryInterface, logging for detection of Clipper or Palette objects
fix: fix for /c:path argument of DxWnd command line usage, now can accept full file paths
fix: log=none as default value for new dxwnd program entries

v2.04.20
GUI:
add: two more 32bit words for additional flags
add: timestamp and dxwnd version added to exported files
fix: window style selector through radio buttons
fix: implementation of View->Shims
DLL:
add: management of 3DREF surfaces to handle D3D CreateDevice with offscreen surface
add: FIXTHINFRAME option to get overlapped window with thin border
fix: revised implementation for DirectDrawSurface getCaps and GetSurfaceDesc wrappers
add: hooker for SetWindowsHookEx WH_MOUSE_LL events - used by "Royal Tramp 2"
fix: added WM_NCMOUSELEAVE to the list of potentially offending messages

v2.04.19/fx1
fix: major code reorganization: caps database & surface stack merged into a single object class, including ZBUFFER surfaces
fix: eliminated arbitrary surface properties settings
fix: joystick api promoted to hot patching to get a more powerful hooking
add: hooking of WM_STYLECHANGING/STYLECHANGED messages to allow D3D rendering within the window borders instead of over the  whole window area - fixes "Nightmare Creatures" and others.
fix: fixed "Need fos Speed 5" peculiarity: QueryInterface of an object on top of itself.

v2.04.18
add: "Fix AdjustWindowRect" flag (FIXADJUSTWINDOWRECT) to selectively turn the AdjustWindowRect coordinate patching. The flag must be checked for "Man TT Super Bike" and unckecked for "Imperialism II".
add: "Hook child WindowProc" and "Hook dialog WindowProc" flags for better control over these risky operations.
fix: bad GetParent hook: caused crash when checking "Hook child WindowProc" flag.
fix: improvements in "Hybrid" emulation mode

v2.04.17
add: 2 more DWORD flags, unused for now
add: virtual joystick sensivity control
fix: check on ZBUFFER capability attribution in case the capsdb holds a wrong capability belonging to a different surface that was deleted
add: "Fix LoadLibrary error" flag (LOADLIBRARYERR) to return ERROR_DLL_NOT_FOUND instead of ERROR_MOD_NOT_FOUND error code. Fixes "Nightmare Creatures".
add: "Hybrid" flag for "Shared ddraw & GDI DC" mode - reduces "Star Treck: Armada" problems. 

v2.04.16
add: improved virtual joystick with configuration and monitoring panel, invert axis options, show crossfire.
fix: GetAttachedSurface now simulates also the primary surface as attach to backbuffer. Fixes GTA first episode windows version.

v2.04.15
fix: suppress DDERR_EXCLUSIVEMODEALREADYSET error in ddraw::SetCooperativeLevel(). Fixes program termination in "Sid Meier's Alpha centauri".
fix: exception caught on DirectDrawSurface::Release(): it may fix problems when using ddraw proxies
fix: added clipper object to ddraw QueryInterface management
fix: added IID_UNKNOWN object to ddraw QueryInterface management
fix: better implementation of LIMITRESOURCES flag: now HD virtual space is 100MB free in a 120MB disk.
fix: SLOWDOWN flag in PeekMessage now uses dxw.DoSlow(1) instead of *pSleep(1), preserves more messages
add: more cases in mciSendCommand wrapper for BYPASSMCI flag
fix: in mciSendCommand using ANSI MCI_OVLY_WINDOW_PARMS instead of WIDECHAR version
fix: DirectDraw:GetCaps wrapper crash when setting SUPPRESSOVERLAY and the hw or sw caps pointer is NULL
fix: exception when trying to use ddraw SetAppCompatData and LoadLibrary failed
fix: error messages on LoadLibrary errors

v2.04.14
fix: virtual registry parser, token names now case insensitive (es. "path" = "Path")
fix: virtual registry crash when replacing tokens and lplpData is NULL
fix: virtual registry return code, must not return ERROR_MORE_DATA when lpData is NULL (fixes "Rollcage II")
fix: eliminated error message in virtual palette processing when color depth is 8 bits
fix: improved logging in EnumDisplayModes wrapper
fix: EnumDisplayModes wrapper for nonemulated nonwindowed mode
fix: deleted improper input clipper destruction after primary surface destruction. Fixes "Settlers III" clipper problems
add: virtual heap (makes some "Wing Commander" games work)
fix: freak help->about...
add: GUI "Tools->Recover system colors" utility

v2.04.13
fix: moved USER32/InvalidateRect hooker to always activate it and further reduce desktop flickering
fix: fixed virtual registry tags that introduced extra characters in key string expansion
add: hooked SelectClipRgn - preliminary to attempt fixing "Sid Meier's Alpha Centauri"
fix: fixed unsupported call to SetWindowsHook(WH_CBT, NULL). Fixes "Starsiege" deaf mouse control.
fix: completed and rebuilt proxy dlls to be aligned with current DxWnd structure

v2.04.12
GUI:
add: "Support offscreen 3D" flag (OFFSCREENZBUFFER)
DLL:
fix: recursion bug in CreateWindowExA hooker
add: OFFSCREENZBUFFER flag

v2.04.11
GUI:
add: "Kill all" menu command on expert mode
add: "Lower background priority" flag
add: "Include menu in clipper area" flag
DLL:
fix: decrement virtual reference counter in case of 3D surfaces with ZBuffer (experimental): fixes "Microsoft International Soccer 2000".
add: more message types in window message logging
fix: fixed mouse clipper logic and moved code in dxwCore class
fix: fixed GDI palette syncronization with ddraw: recovers "Hyperblade" black screen in gameplay
fix: unhooked USER32 GetParent() call, causing recursion in hot patch mode, blanked dialogs ("Galapagos") and in general more problems than benefits 
fix: fixed AdjustWindowRect/Ex() wrapper to always neutralize window changes (good for "Man TT Superbike")
fix: fixed message processing for FORCEWINRESIZE: now "Man TT Superbike" can be moved and resized (at least on Win10).
add: added BACKGROUNDPRIORITY handling
add: added CLIPMENU handling

v2.04.10
GUI:
add: view flags window (expert mode only)
DLL:
fix: prevent InvalidateRect on desktop, eliminates some flickering
fix: ZBUFFER capability management - fixes "Galapagos" in HW mode (but it may crash because of the intro movie...)
fix: logging of log flags
fix: D3D8/9 QueryInterface fully revisited 
add: (very) preliminary texture transparency hack

v2.04.09
add: hook USER32 AdjustWindowRect/Ex calls: fix window position problems for "Man TT Superbike"
add: hook for USER32 GetActiveWindow() tracing. 
fix: version hooking for IDIrectDraw methods called by IDIrectDrawSurface methods: fixes many crashes on ddraw games
fix: capability registration passed in IDIrectDrawSurface::QueryInterface() calls: fixes settings for ZBUFFER surfaces
fix: capsdb logging (developer's use only)
fix: handling of WM_NCCALCSIZE & WM_NCPAINT messages in winproc hooker to align behaviour to comment. To be tested ...

v2.04.08/fx1
fix: improved DIB dumping & logging: fixed BMP header with negative size, dump only when DIB has attached image
fix: safe CreateWindow mode, preserves original size & position within win creation. Fixes "Warhammer SotHR"
blitting problems
fix: virtual registry file created by GUI and not extracted by the dll. Ths avoids getting the first matching entry.
fix: "Scale font parameters" fully revised, also renamed to "Scale font size"
add: "High quality fonts" flag to force antialiasing font style
fix: ddraw surface capability regression for emulated backbuffer: fixes "Dominant Species" textures in SW mode
fix: ddraw surface capability regression for direct primary/backbuffer: should fix "Tomb raider 3" in HW mode
fix: ddraw surface capability regression for zbuffer: fixes "Toca Touring Car" in SW mode
fix: in fx1, bug crashing app when querying unknown interfaces!

v2.04.07
fix: QueryInterface wrapper for ddraw classes unified in a single procedure.
fix: D3D2 SwapTextureHandles hook fix - fixes "Wizardry Chronicle" texture problems
fix: string overflow in TextureHack procedure for D3D1-7 textures

v2.04.06/fx3
GUI:
add: Expert mode
add: experimental dump od DIB sections and blitted dc for debugging purposes
DLL:
fix: corrected condition to print two virtual registry log messages
fix: improved logic to recover surfaces with DDSCAPS_TEXTURE capability - ref. "Dominant Species"
fix: fixed cursor clipping state and toggle key to make it work in all conditions
fix: D3D objects hooking with QueryInterface method. Fixes "Wizardry Chronicle"
fix: add hook for GDI32 GdiGradientFill and GdiTransparentBlt, present in "RHEM"
fix: log messages for CloseHandle wrapper
fix: log messages for GetPixel wrapper
fix: window position in MoveWindow for ANCHORED mode and big windows
fix: DialogBoxParamA wrapper

v2.04.05
GUI:
add: preliminary implementation of Dummy/Expert GUI modes
fix: "Explore" command implementation for WinXP
DLL:
add: preliminary implementation of virtual registry tokens to make configuration relocable
fix: capability fix on surface creation. DDSCAPS_TEXTURE is replaced by DDSCAPS_OFFSCREENPLAIN. Fixes "Dominant Species" on certain situations where the surface creatuion returns DDERR_UNSUPPORTED code.
add: preliminary hook for GDI32 RemoveFontResources and EnumFonts
fix: CreateRectRgnIndirect wrapper altered input RECT* coordinates
fix: separated D3D QueryInterface pointers and wrappers, added wrapping for QueryInterface D3D v7
fix: log cleanup
add: "Dynamic ZBUFFER fix" flag (DYNAMICZCLEAN): needed to fix some of "Tonic Trouble" rendering problems.
add: OpenGL dedicated logging
add: WinG32 dedicated logging
fix?: bypass for WM_FONTCHANGE: avoids "Warhammer: Shadow of the Horned Rat" crash when entering battle
add: WinG32 debug highlighting of blitted rects on screen
add: GDI32 debug highlighting of blitted rects on screen
add: hooks for GDI32 GetTextExtentPointA and GetTextExtentPoint32A: needed for proper string rendering on "Warhammer: Shadow of the Horned Rat"
add: hook for GDI32 GdiAlphaBlend, very rare call that appeared in "RHEM" during savegame load.

v2.04.04
GUI:
add: run unhooked command 
fix: improved logic to propose config save only when virtual registry string is potentially updated
fix: when hooking is disabled, STARTDEBUG and INJECTSUSPENDED processing don't take place
DLL:
fix: crashing condition on "hide desktop background" option - fixes M&MVI crash
fix: mouse loop coordinate fix when main window is not set
fix: eliminated log of dmDeviceName field in ChangeDisplaySettings* calls because the field may be not initialized
fix: NOMOVIES option now stops also SmackW32 movies (making movie length only 1 photogram).

v2.04.03-fx1
add: D3D8 16bpp backbuffer emulation on 32bpp color depth. Fixes "Dangerous Waters" on 32bpp Win8-10 
add: more OpenGL wgl* hooks, used by "StarSiege 2014".
fix: using correct version method pointers in bilinear blit, avoids crash in "Daytona USA" using bilinear filtering
fix: do not try to CloseHandle the handle returned by GetMenu, avoids debugger exceptions.

v2.04.02
fix: avoid crashing on View Shims command on WinXP
fix: bilinear 2X crash
fix: handling of 16 to 32 bpp color conversion in D3D8 IDirect3DDevice8 Copyrects and GetFrontBuffer methods. Fixes "Dirt track Racing 2" missing panels.
fix: more / better logging
fix: in USER32/CreateWindow* calls, handles the case where a to-be main window is moved/resized before being declared as main window. Fixes uncontrolled position changes in "Civil Wars 2 Generals".
add: preliminary hooks for message loop APIs USER32/PeekMessage, GetMessage, PostMessage. Now PeekMessage includes SLOWDOW flag processing, reduces CPU time in "Civil Wars 2 Generals".

v2.04.01/fx5
add: preliminary WinG32 handling
add: minimal WinG32 replacement, thank to Wine source code
add: DirectX(2)/"Create a Desktop Win" option. Fixes "Man TT Super Bike"
add: new hooking schema for IAT organized like original "Al Unser Jr. Arcade Racing"
fix: in virtual registry fix to handle the '@' void label case properly
fix: added hook for RegQueryValueA, used by "Warhammer 40.000 Shadow of the Horned Rat"
fix: virtual registry logging
add: added SmackW32 hooking to bypass SmackColorRemapWithTrans and SmackColorRemap: SmackColorRemap seems involved in changing screen resolution in "Civil War Generals 2"
fix: some log messages
fix: SetWindowLong hooker: eliminated a unassigned variable condition
fix: added hooker for GDI32/SetROP2 call to insert a InvalidateRect as in older OS. Makes "Titanic" working.
fix: added hooker for Smack32/SmackSetSystemRes call to prevent resolution change and set proper virtual screen size: see "Spearhead"
fix: fixed virtual screen size initialization so that it doesn't override settings defined before first window creation: see "Spearhead"
fix: fixed error condition when DirectDraw::GetCaps is invoked with wrong length - fix "Spearhead" bug
fix?: ZBUFFER original capabilities passed to new surface created by QueryInterface - so far, useless

v2.04.00/fx1
add: texture management for D3D textures: XCTn compressed textures, raw mode, texture highlight and hack
add: Input / "fix MOUSEHOOK callback" option, fixes "Jagged Alliance 2" mouse problems
add: GDI / "Pretend Win visible & on top" option: experimental, let the program believe that the main window is visible and on top of z-order as usually happens to full screen applications
add: GDI / "Win insulation" flag: avoid enumerating windows so that the program can't interfere with them. Fixes "Tribal Rage" damages to desktop.
add: hook to ddraw DuplicateSurface() method, so far for logging only.
fix: eliminated possible divide by 0 exception when minimizing windows
fix: mouse clipping now selected by radio buttons (default, off, on, locked)
fix: bug in D3D device enumeration log
fix: bug in Hybrid and GDI ddraw surface rendering
fix: missing "No HAL Device" flag default to disabled
fix: improvements in texture handling, dds format support for DirectDraw textures

v2.03.99
fix: unwanted hook to directsound when passing by CoCreateInstance()
fix: transient mode, now checks for both the target and the launcher to be dead before terminating
add: Direct3D TRIMTEXTUREFORMAT flag, to mimic MS "DirectXTrimTextureFormat" shim. Fixes problems in "Heavy Gear 2" and "Star Wars: Rogue Squardon 3D".
add: D3DDevice GetCaps hooking and full dump of D3DDevice capabilities
add: "No HAL Device" flag, making it unavailable the IID_Direct3DHALDevice device. Fixes "Grand Prix World" when the 3D car models are invisible.
add: reorganization of mouse clipper fields, with the addition of LOCK mode (useful for Tribal Rage)

v2.03.98/fx1
add: fine tuning for vsync options: three modes (default: game native, force on, force off) three algorythms (ddraw WaitForVerticalBlank, loop controlled by GetScanLine, loop controlled by time delay)
add: fine tuning for WAIT options in ddraw Flip() and Blt() methods: three modes (default: game native, force on, force off)
fix: better support for non windowed + no emulation mode, surfaces and blit operations are left (almost) in original state, surface capabilities are left inaltered
fix: added missing recovery on surface lost
fix: some missing or wrong logs
fix: suppression of WS_EX_TOPMOST flag in all conditions for UNLOCKZORDER mode
fix: SUPPRESSOVERLAY flag applied also to D3D capabilities from D3D GetDeviceCaps() method
add: added more API hookers in IMAGEHLP.DLL bypass
fix: fixed logic to hook WindowProc routines for main window, childs and dialogs.
fix: more frequent updates of window coordinates to avoid scaling errors
fix: fixed CreateDialogIndirectParam and CreateDialogParam wrappers to make inner windows properly scaled (fixes red Alert 2)
fix: recovered DEFAULTMESSAGES option
fix: normalized output for ddraw GetScanLine() method
fix: fixed the policy for redrecting the WindowProc routine. Fixes "Ultimate Spiderman" clipping problems, and possibly much more

v2.03.97.rc1/fx1-3:
fix: emulation of proper ddraw surface capabilities in GetSurfaceDesc() and GetCaps() methods. Seems to cope with needs of most (all?) games.
fix: recovered handling of WM_ERASEBKGND message. Fixes missing backgrounds in "Adrenix" and "Adrenix 3D" games.
fix: cleaned up and unified capability handling in DirectDrawSurface GetCaps and GetSurfaceDesc methods
add: warning message and update when running a program with non updated virtual registry settings.fix: LoadLibrary wrappers failed to clear the recursion flag, stopping the hook of new modules after a recursion case.
add: warning messages when closing DxWnd and possibility to cancel the operation

v2.03.96:
preliminary handling of hidden window
fix: handling of non-windowed mode. Fixes "Warhammer 40.000 Rites of War" in non-windowed mode
add: NODDEXCLUSIVEMODE flag. Forces non exclusive ddraw coop level in non-windowed mode. Fixes "Warhammer 40.000 Rites of War" in non-windowed mode
add: CLEARTEXTUREFOURCC flag. Suppresses FOURCC capability and handling. Should help to process compressed textures.
fix: CreateDIBSection wrapper log messages
fix: PREVENTMAXIMIZE window size checks: should avoid size bigger than initial size, bugged coding inhibited any growth.
fix: GUI setting of execution folder when running "Launcher" task.

v2.03.95:
fix: fixed prototype for ICDrawBegin wrapper
add: extended logging for mci multimedia API
add: window position selector & "locked size" + "anchored" modes
fix: transient mode when system tray is not available
fix: recursion problem in kernel32 LoadLibrary* wrappers. Allows "Hot patch" on WinXP.
fix: initial position problem.

v2.03.94.fx1-3:
fix: aligned Lock cheats in Direct/Indirect mode ......
fix: no window interventions in non windowed mode. Fix "Gunbound" in fullscreen mode
fix: avoid multiple injection for early-hooked programs. Fix "Gunbound" regression.
fix: Restore of all lost surfaces upon D3D BeginScene DDERR_LOSTSURFACE error. Fix "Gunbound" lost textures in fullscreen mode.
add: more logging in mciSendCommand - possibly some regression problem...
fix: corrected clipping bug introduced in v2.03.93.
fix: SetWindowLog hooker preventing to set DxWnd windowproc when not in fullscreen mode. Fixes "Nascar Racing 3" recursion and crash.
fix: eliminated useless filter to suppress high resolutions detection in PREVENTMAXIMAZE mode
fix: fixed mciSendCommand wrapper log messages that could crash the program

v2.03.93:
add: GUI game icon extracted also form separated *.ico icon file
add: GUI definition of default values for window position
fix: ddrawsurface::Unlock lprect usage: doesn't use the input value but refers to rect passed in Lock operation. Fixes "fifa 2000" crash.
fix: avoid issuing a GetPalette method against a deallocated object
fix: automatic creation of Clipper object if needed for emulated ddraw blit to primary surface - fixes the well known black blitting problem
fix: proper handling of dinput DirectInputDevice::GetDeviceData() DI_BUFFEROVERFLOW error condition
fix: user32 GetCursorPos() wrapper
fix: user32 mouse_event() wrapper

v2.03.92.fx1-2:
add: option "Fix FreeLibrary" to emulate the Win9X behaviour returning errorcode upon attempts to free a deallocated module. Fixes "Heavy Gear" patch 1.2 endless loop problem.
fix: added reference to Palette object to allow final Release() without crash - fixes "Heavy Gear" crash at end of mission
fix: fix RECT structure passed to ddraw::Unlock method in case it points to bad coordinates. Fixes FIFA2000 in D3D mode
fix: wrapped all OpenGL GetString methods and inserted proper logging. For diagnostic purposes.
fix: fix ddraw::SetPalette wrapper in case lppd is NULL. Fixes Adrenix sw mode crash.
fix: better way to manage extra modules to be hooked (Glide ddls, SmackW32)

v2.03.91.fx1-5:
fix: eliminated LOC causing bad window sizing
fix: better desktop hider management
fix: DC emulation object leakage - fixes "Yu No" leakage
fix: USER32 EndPaint DC leakage - fixes "Yu No" leakage
fix: window position problems
fix: GUI auto hide mode enabled and disabled without need of GUI restart
fix: ddraw surface stack policy. Recovers "Warhammer 40K Rites of War" after v2.03.83 fix.
add: /q quiet mode, no message dialogs on screen

v2.03.91
fix: lowered priority level for dinput/GetDeviceState log message to debug
fix: added unlock of source surface in EmuBlt* routines
add: reintroduced SmackW32.dll hooking - necessary for "Galapagos" intro movie
fix: fixed GDI32 CreateDCA/W, hookers (pretty much reverted to previous working code...)
add: preliminary hook for OpenGL glCopyTexImage2D
add: OpenGL better logging
fix: defaulting process all textures for limit = 0,0
fix: manages the possibility or ValidateRect receiving a NULL RECT pointer - fix "Galapagos" crash in GDI scaled mode
add: auto-blit mode in SmakW32 frame loop - shows "Galapagos" company logo intro movie
fix: force stop blitting on window minimize event - fixes "Galapagos" and many others ....

v2.03.90
add: added flags/fields to define initial/maximum screen resolution
add: "Lock color depth" ddraw flag. Fixes "Ahlgrens Bilspelet" color problems
add: preliminary opengl/glut32 hook
fix: ZBUFFER capabilities - allow "Galapagos" to run in hw accelerated mode
fix: separated D3D FindDevice hookers for D3D version 1/2/3
fix: some unused code cleanup

v2.03.89
fix: replaced hooked LoadLibrary with original call (*pLoadLibraryA), this seems to avoid the hook address replacements and the need for the "No hook update" flag. To be tested.
fix: added some missing IID_DirectDraw3 case in DirectDraw hooking
fix: fixed the logic for centering the main window for desktop with non-zero offset
fix: improved logic for window placement in CreateWindowEx call
fix: added wrapper for winmm/GetJoyPos() virtual joystick referenced in "Jane's FA18"

v2.03.88
fix: revised (simpler) window handling, hopefully more reliable (Diablo no longer crashes on fake-fullscreen modes?)
fix: eliminated hook of smackw32.dll to reach GDI32/user32 calls beneath
fix: added ddraw hooking in ddrawex/CreateDirectDraw() call: now "Theocracy" works with default "Automatic" ddraw hooking
fix: added hook for EnumDisplayDevicesW used by "Battleground Europe"
fix: added "Hide multi monitors" logic to EnumDisplayDevicesA/W 

v2.03.87
fix: possibility to use @ token instead of "" in fake registry
fix: independent hooking of DirectDraw3 objects, needed for "Theocracy"
fix: eliminated shared hooking of "Compact" ddraw methods
add: improved caps masking in debug mode, now can change any flags
fix: added missing hook for DirectDraw3 in QueryInterface
fix: fixed wrong hook for GetGDISurface in DirectDraw3 interface
fix: eliminated dialog box when hooking ddrawex
fix: eliminated experimental hook to auxGetNumDevs returning 1 available device always. Fixes problems with "Imperialism II"

v2.03.86
fix: completely revised the "Keep aspect ratio" flag, now should work with multi-windowed games too.
add: multi-monitor handling with dynamic detection and monitor selection
fix: basic class reorganization - now it is a little more neat and simple
add: added 1280x1024 resolution limit
add: multi hooks option now available in global settings panel
add: viewfinder to grab coordinates from a window on screen 

v2.03.85
add: added control to avoid hide of tray window. Useful for "Planet of the Apes"
fix: fixed BIG window coordinates. Fixes "Deer Hunter" window positioning problems.
fix: mciSendCommand wrapper for BYPASSMCI flag

v2.03.84
add: new flag "No hook update" (HOOKNOUPDATE) to disable update of original call address in case of change 
add: hint message for "No hook update" condition
add: hint message for memory signed integer overflow
add: GUI flag "Name by Folder" to default the program name field with the program folder instead of program filename
add: new flag "Set keyboard shared coop. level" (SHAREDKEYBOARD) to change directinput keyboard cooperative level mode
add: new flag Direct3D / "Enumerate 16bit modes" (ENUM16BITMODES) to force enumeration of 16 bit video modes on Win10 desktop when not listed
fix: GUI randomic value for Injection mode selector when adding new entry

v2.03.83
fix: cleared surface list on primary surface creation: "Daytona USA" keeps creating new primary surfaces, so it saturated the surface list.
fix: calling the proper GetGDISurface pointer in Lock wrapper
fix: HookProc routine causing crash with GTA3, fixed for Win7 and Win10
fix: LoadLibrayA/W after a SetWorkingDir invocation didn't load the local modules. Fixes "The Rage".

v2.03.82
fix: fixed situations where the virtual desktop window was not fully blackened during initialization
fix: added FlushInstructionCache call to "suspended process" injection type to make it more reliable
fix: suppressed more capabilities when setting "Suppress overlay capability" flag
fix: fixed logic for ZBUFFER capabilities: "The Creed" seems to have two, one in SYSTEMMEMORY and one in VIDEOMEMORY...
fix: avoid setting cursor clipping when client area is returned with zero size. 
fix: fixed CreateProcessA hooker for "suspended process" injection type to forward all parameters to the system call
add: added "Fix glBindTexture texture" flag to compensate for the missing glActiveTexture call. Fixes "bugdom" 2D panels.

v2.03.81
fix: hook AVIFileRelease to avoid call when avi file is NULL. Fixes a crash in "Die hard trilogy"
fix: in DirectDrawSurface::QueryInterface added handling of Direct3DDevice objects: Needed for "Die hard trilogy"
fix: hooking of DirectDrawSurface GetPalette / SetPalette methods
add: added "Suppress DI common errors" for Acquire / Unacquire methods: Needed for "Die hard trilogy"
add: some DirectSound methods for logging
add: some Direct3D error codes for logging
fix: fixed some Direct3D methods hooking
fix: Direct3DDevice::AddViewport hooker now delete a viewport that is still linked to a previous device. Needed for "Die hard trilogy"
fix: fixed CreateProcess hooker for "Extend injection" mode: Needed for "Die hard trilogy"

v2.03.80
add: "Suppress DI common errors" flag, fixes initial failures of Acquire method.
fix: in centered window option, trims the window coordinates if they exceeds the screen size and risk getting negative values.
fix: fixed injection logic to avoid hooking task when the "Hook enabled" flag is off
fix: avoid calling GDI SetPixelFormat more than one upon a window since this is not supported. Fixes "bugdom" OpenGL problems.
fix: scales glGetIntegerv(GL_VIEWPORT) returned values. Fixes "bugdom" rendering problems.
add: added GUI logging when debug mode is on.
add: added a third injection mode "Inject suspended process". Modified the GUI to select the injection mode through radio buttons. Improves injection compatibility, working mode for "T-ZWei" on WinXP.

v2.03.79
fix: proper handling of DirectInputCreateEx function - fixes "Gothik 2" mouse and window control
fix: avoids setting the pixel format of a window more than once through GDI SetPixelFormat call. Fixes "Bugdom" OpenGL rendering
fix: improved bilinear filtering speed through gsky916 loop optimization 
fix: added delay handling to several gdi32 amnd user32 routines blitting to screen

v2.03.78
fix: DirectDrawSurface::GetCaps now returning VIDEOMEMORY+LOCALVIDMEM type for 3DDEVICES in system memory. Fixes "Gothic 2" controls.
add: Added SKIPDEVTYPEHID to avoid passing HID input devices to DirectInput::EnumDevices callback. Fixes "Jetboat Superchamps 2" crash.
add: hint for OS version compatibility flag
add: hint for obfuscated IAT detection
add: hint for safedisk and securom game protection
add: hint to show safedisc version 
fix: centralized handling of window styles

v2.03.77
fix: SetCooperativeLevel with 0 HDC should not alter fullscreen status (fixes Portugal 1111 scrolling problem)
add: preliminary "Show hints" flag and feature
GUI: multi-line tab control
GUI: clicking on notes and registry icons the GUI goes straight to the relative tab pages.
GUI: The DirectX tab is now splitted in two, so that there no longer are DirectX switches into the Direct3D tab.

v2.03.76
fix: DC handle leakage in several places, this also fixed GDI "emulated devie context" mode.
fix: FillRect wrapper, fixed coordinate remapping with wrong values. Fixes "Imperialism" menu and detailed view.
fix: suppressed "reuse emulated DC" flag: it was no longer referenced in the code.
fix: workaround for occasional DDERR_INVALIDPARAMS error in service GetDisplayMode to detect current video mode.
fix: code cleanup: eliminated CREATEDESKTOP handling, experimental and no longer supported
fix: vdxcore::IsToRemap now detects the NULL dc handle as remappable DC
fix: some logging imperfections
fix: palette log message crashing the program

v2.03.75/fix1
fix: handling of restore previous screen mode when a primary fullscreen window is terminated. Fixes "Dethkarz" initialization error after 8BPP intro movie termination.
fix: SystemParameterInfo for SPI_SETSCREENSAVERRUNNING does not return an error
fix: added default pixel format specification to backbuffer: improves the possibility of ZBUFFER attach
fix: recovery of DDERR_SURFACELOST error on primary surface in PrimaryStretchBlt routine. 
fix: possible palette handle leakage 
fix: DC leakage caused by CreateCompatibleDC. Fix "Mechwarrior 3" repeated play sessions.
add: GUI drag & drop of game executables
fix: RegQueryValueEx bad retcode when Data ptr is NULL
fix: recovered support for Windows XP
fix: fake registry did not read last registry line! Fixed.

v2.03.74
fix: logging of dwFOURCC field text value
fix: Injection routine: avoid closing a few handles that should not be closed, simplified logic.
add: GUI global configuration panel

v2.03.73
fix: address hooking for GDI SetPixelFormat(): avoids resolution changes in "T-Zwey" configured for D3D8 rendering engine.
fix: logic for restering last window position on multimonitor configurations
add: "Help -> View Help" to show help page
add: "Fix GlobalUnlock" flag to fix GlobalUnlock bad errorcodes in "Grand Prix World" and possibly other games.
add: revised log configuration, added "Separate" mode to make each process to write on a dedicated file.

v2.03.72
fix: inserted bypass for DirectDraw::Compact method, that is unimplemented but can return errors. Fixes "Crazy Climber" error detection.
fix: error in d3d9::CrerateDeviceEx method preventing it to work when moving fullscreen devices to window mode, as tipically happens using DxWnd. Fixes d3d9 version of "Saint's Row the Third"
fix: accurate determination of ".text" or "CODE" segments where to apply the REPLACERTSC and REPLACEPRIVOPS flags.
fix: error in logic to add extra reference to backbuffer surface: fixes "Warhammer 40k Rites of War".

v2.03.71
fix: no need to save configuration before running with fake registry
add: icons to notify for non-empty registry and notes tabs
fix: hooked ddrawex.dll DirectDrawEx methods - fixes "Whiteout", what else?

v2.03.70
add: ddraw Overlay tracing (waiting to implement an emulation strategy)
add: SetWindowsHookExA logging, SetWindowsHookExW hooking
fix: MessageHook logging
GUI: some rearrangements / rationalizations to the interface
add: INIT24BPP and INIT32BPP flags

v2.03.69
fix: fixed "CPU max usage" option
fix: WindowFromPoint & WindowFromPointEx hooks
fix: recovered partilly working implementation of "shared dc" mode
add: "Suppress Win errors" option, needed to skip a blocking error and make "Migrosoft Golf '98" starting

v2.03.68
add: options to limit the cpu usage of program threads, either inconditionally ("CPU slow down") or only for CPU consuming threads ("CPU max usage").
fix: some logging details 

v2.03.67
fix: D3D7 hooks to make NBA2002 working
fix: better virtual color depth settings to allow "Moto Racer 2" believe it is running on a fake 16bpp desktop
fix: GUI placement coordinates are not saved when terminating in minimized or maximized state

v2.03.66
fix: terminated the separation between ddraw method interfaces
fix: force clipper now builds a clipper for the real primary surface (useful when desktop composition is disabled and on XP)

v2.03.65
fix: handling of fullscreen switching on closest monitor (in multimonitor systems)
fix: X,Y starting win position can now be negative integer (in multimonitor systems)

v2.03.64
fix: completed the hook pointers separation per COM interface version: this is of growing importance on Win10 because of the system updates (shims?)
fix: fixed incoherences in task refences of the GUI
fix: recovered task kill on WinXP
fix: updated RedrawWindow, now allows better handling of "Galapagos" pause screen

v2.03.63
fix: saved config changes in case DxWnd is restarted for acquiring admin caps
fix: duplicated several function pointers in D3D8/9 wrappers: fix some GTA3 crashes and reduce the need to suppress Reset operation
add: flag "Widechar program vs. ANSI" to replace default ansi calls with widechar equivalents: fixes "Tomb Raider Anniversary" mouse problems

v2.03.62
fix: flag "Intercept Alt-F4 key" now working also alone with no need to set "Enable hot keys"
fix: file open dialogs initialization on WinXP platform
fix: hooked ANSI & WIDECHAR version of DirectInput8CreateDevice: makes "Gooka the Mystery of Janatris" working.
fix: rebuild up-to-date dxwnd proxies

v2.03.61
fix: set hook to ddraw SetPalette for all surface types (Drakan Order of the Flame)
add: preliminary support for windowed / fullscreen switching
fix: preliminary d3d7 function pointers separation for different interfaces
add: block process priority class
add: autohide DxWnd feature
fix: GUI kill process now kills the whole process tree to avoid leaving live processes around. Happens for instance in Win7 with rundll32.exe for compatibility in 64 bit environment, and that one uses a full core CPU!
fix: added CDS_UPDATEREGISTRY to the ChangeDisplaySettings modes to be neutralized. Needed in "Severance: Blade of Darkness".

v2.03.60 
too many things, sorry, I'll fix it later.

v2.03.59
fix: added extra ref to surface for ddraw version 2: fixes "Warhammer Chaos Gate"
add: view shims menu command, to analyze Win7/Win10 compatibility patches
GUI: added the hook tab to move hooking flags in a single tab page

v2.03/58
add: added program manifest stating Win10/8/7/Vista compatibility
add: "Clear compatbility flag" tool
add: "Recover screen mode" tool
add: "Set path" tool
fix: reorganized menu commands
fix: proper handling of DEFAULT win size on CreateWindowEx - fixes "Imperialism" problem
fix: fixed stretced blit operation because Blt behaves differently from StretchBlt. Fixes "3D Minigolf" partial background
fix: GetPixel logging

v2.03.57
fix: improved handling of clipping regions in shared dc mode
add: added DWM status to log file
add: flags for preliminary hook of DirectSound and Smack libraries
fix: changed icon colors (yellow for red, doesn't suggest the idea of error or danger)
add: menu commands for entry movement (top, bottom, up, down) or duplication

v2.03.56/fix1
add: capability to hook calls referenced by ordinal number
add: "Acquire admin caps" flag to self elevate DxWnd if configured as necessary
add: added third mode to handle potentially offending messages, that is process them by the Window default process routine WinDefaultProc. Good for "Red Alert 2".
add: "fix clipper area in shared DC", temporary solution to adapt the shared DC mode to the different situations (yet to finish).
fix: fully integrated the patch for "Silver" game: returning backbuffer as a doublebuffer also when requested with DDSCAPS_VIDEOMEMORY caps.
add: FunkyFr3sh addition to process WM_SYSCOMMAND SC_CLOSE and force program termination when requested by "Terminate on window close" option.

v2.03.55
fix: differentiated handling of blitting from memory to DC vs. from DC vs. memory. The second case is less frequent, but altered the correct rendering of "Battlezone 1998 edition", now working both in scaled, emulated DC shared DC and shared ddraw and GDI DC. The fix impacts both BitBlt and ScaledBlt GDI calls.
fix: prevented InvalidateRect to return giving a scaled rect. Fixes "Deadlock 2"partial screen updates.

v2.03.53(skipped)
v2.03.54 (fix1+2)
add: syslib shared dc mode preliminary implementation, thank to Narzoul's ideas. Works for "MS Golf 98" and "Deadlock II rel. 1.2"
fix: handling of cursor hide/show for programs that don't go through the message window ("Deadlock II rel. 1.2")
add: /R:n command line option to automatically start DxWnd in icon tray, run the n-th program in configuration and terminate.
add: option to disable the disabling of Alt-Tab key through SetWindowHooks through WH_KEYBOARD_LL event. Make it posssible to use Alt-Tab key on "Dungeon Kepper II".
fix: palette object reference count decremented from extra references and zeroed by the "Return 0 refcount" flag: make it possible to run "Dark Earth" that makes a check on the palette reerence counter before starting the game.

v2.03.52
add: added Win NT 4.0 to the list of detected OS
fix: DirectInput SetCooperativeLevel: should not change window handle - fix startup error in "Storm Angel"
fix: fixed GetWindowRect to replace real desktop with virtual one - fix startup error in "Storm Angel"
fix: ddraw hookers keep different original pointers for each interface: fixes several ddraw UNKNOWN_ERROR
fix: eliminated the ddraw full log functionality, because of the increased complexity
add: added several supported fake screen resolutions - needed for "Last resort" SWAT3 mod
fix: used fake resolutions also in USER32 EnumDisplaySettings
fix: avoid hooking directinput dlls if not requested when loaded dynamically
fix: added recovery of lost device in DirectInput GetDeviceData method
fix: changed SetHook so that it can get function pointers without necessarily replacing them
add: option "Limit ddraw interface" to exclude support for IDirectDrawInterface greater than limit (range 1-7)

v2.03.51
fix: WinXP select file dialog bug, not appearing if pointing to nonexistent directory
fix: export file bug - now forces the file to have a .dxw extension
fix: eliminated 6 useless fields in dinput handling, now their value is calculated automatically and at runtime
add: DirectInput / "Emulate mouse relative movement". Eliminates the fake clipped regions inside the window (fix Geneforge series)
fix: directinput mouse mode (absolute vs. relative) now correctly checked using the mouse device
fix: attempt to fix FIXMOVIESCOLOR flag for 32BPP target color depth, but it doesn't still work on Win10
fix: directinput RELEASEMOUSE flag applied to mouse device only

v2.03.50
fix: check for errors that could cause random GDI cursor clipping regions
fix: more accurate hooking of ddraw pointers - fixes several DD_GENERIC_ERROR in Win8/10 platforms, makes "SWAT 3" working
fix: improvements (not finished) in dinput handling
add: the OS version info is traced in the log file
fix: added cursor clipping when requested on new main window creation
fix: handling of cursor clipping when the window gain/lose focus threough DefWindowProc message handling

v2.03.49
fix: a few improvements in handling NULL lpDDSSource in Blit/Flip operations
add: option to print relative timing to log
fix: avoid handling textures at level greater than 0 since this seems to cause application crash
fix: restore cursor clipping through WM_SETFOCUS and DefWindowProc when focus is gained
fix: "One Must Fall Battlegrounds" keyboard fix - can't SetWindowsHookEx with WH_KEYBOARD and NULL thread

v2.03.48
fix: set proper surface capabilities for 3DDEVICE surfaces, according to VIDEOMEMORY capability.
add: "force clipper" flag to redirect clipper definition to main window and primary surface
fix: better handling of surfaces on minimize / restore events, mainly on WinXP
fix: proper default values for"Share ddraw and GDI DC" and "Lock/Unlock Pitch Fix" flags

v2.03.47
add: added handling of DirectInput8 dinput8.dll
fix: completely revised directinput hooking to manage multiple devices acquire/unacquire
fix: added some dinput error codes to log messages
fix: increased time freeze toggle key hysteresis time to 1 sec to avoid multiple activations

v2.03.46
add: added compatibility options "Hook GOG libs" and "Byass GOG libs" to manage GOG games
add: added "Enable Time Freeze" timing option and the virtual key "freezetime"
add: command line option /E to terminate exixting DxWnd session
fix: relocation of WS_POPUP windows. Fixes "Riven" menu window.

v2.03.45
add: virtual joystick center-lock feature, helps using the mouse as mouse leaving the joystick locked.
fix: revised dinput hooking. dinput8 still to be completed. 
add: option "Unacquire input devices on focus lost". Needed for Star wars "Tie Fighter 95".
fix: set dinput SetCooperativeLevel flags to DISCL_NONEXCLUSIVE | DISCL_FOREGROUND
fix: fixed CreateIC hooker call method (WINAPI)
fix: emulated Unacquire on virtual joystick. Needed for Star wars "Tie Fighter 95".

v2.03.44
fix: GetSystemPaletteUse error message
fix: transparent virtual joystick icons
fix: RealizePalette hooker crash in "Super Real Mahjong P7"
fix: complete recoding of GDI BitBlt, StretchBlt and PatBlt hookers: it fixes "Reah" problems
fix: changes in USER32 BeginPaint hooker to support DC sharing with ddraw
fix: increased configuration panels heigth do avoid clipping when desktop font size is set to 125%

v2.03.43
fix: eliminated SYSTEMMEMORY usage on DDSCAPS_3DDEVICE surfaces. Needed for "Arx Fatalis"
fix: deleted emulation of ZBUFFER attach. Needed for "Arx Fatalis"
fix: added emulation of PixelFormat in getPixelFormat hooker. Needed for "Arx Fatalis"
add: added virtual joystick feature. Required to play "Tie Fighter 95" with no real joystick
v2.03.43.fix1
fix: recovered virtual joystick buttons

v2.03.42
add: option "Syncronize GDI and DDRAW palette" - fixes "Man in Black" palette problems
add: apply "Remap MCI coordinates" options also to movies rendered through MCIWndCreate: fixes "Man in Black" movie problems
fix: fixed InvalidateRect hooker logic - fixes "Imperialism" graphic.

v2.03.41
fix: filtering of mouse messages in "Message pump" option. Makes games more responsive to mouse clicks. Improves "X-files".
fix: exclusion of DDSCL_SETDEVICEWINDOW, DDSCL_CREATEDEVICEWINDOW and DDSCL_SETFOCUSWINDOW flags in ddraw SetCooperativeLevel windowed mode
fix: guess PRIMARY surface in ddraw1 when no flags (but BACKBUFFERCOUNT) are set. Necessary for "Star Force Deluxe"
delete: suppressed frame compensation option FRAMECOMPENSATION (unused).
add: added debug flag NOWINDOWHOOKS to avoid WindowProc hook.
fix: GetPaletteEntries hook returns 256 value against hpal=NULL. Let "M.I.B." run, though palette is still not ok.
fix: StretchDIBits hook no longer can return an unassigned value
fix: anti-recursion check in DialogBox WindowProc call

v2.03.40
fix: detecting main window when setting FULLSCREEN mode against it. Helps handling "Reah"
add: option to de-interlace videos on 16BPP to 32BPP color transformation. Required for deinterlacing "11th Hour" movies
add: cornerized mode, could help handling Battlenet GDI menus
add: preliminary implementation of virtual desktop window. Useless so far.
fix: hooked more GDI calls: CreateICA, GetViewportExtEx, IntersectClipRect, SetRectRgn
fix: ChangeDisplaySettings no longer take note of video mode when called with CDS_TEST flag
fix: merged user32 FillRect and FrameRect handling
fix: fixed bug that was leaving a copy of dxwnd.ini on last visited directory on window close

v2.03.39
add: "Fix glPixelZoom args" flag to scale images rendered by OpenGL through GDI. It is needed for "Strength and Honor".
add: "Reuse emulated DC" flag to optimize GDI emulation. experimental.

v2.03.38
fix: handling of WM_ACTIVATE and WM_NCACTIVATE messages, should bring the window to HWND_NOTOPMOST z-order position only when activation is on, not off!
fix: in "Share GDI and draw DC" mode, check if the primary surface really has a DC and switch to another one if the DC is missing.
fix: eliminated some redundant code in GDI ReleaseDC in DC emulated mode

v2.03.37:
add: handling of RegEnumValue advapi call in virtual registry. Useful for "Star trek Generations"
add: virtual registry now integrated in dxwnd.ini configuration and activated automatically. No more handling of dxwnd.reg file
fix: virtual registry optimization, code reorganization, bug fixing ....
add: "Stretch ActiveMovie window" flag. Fixes "The Thing" intro movies.
fix: apply "Blit from backbuffer" option to both surface emulated and non emulated cases. Should help fixing "Galapagos" screen locks.
fix: apply "Limit screen resolution" to the list of detected video modes in VGA emulated case
fix: eliminated "Automatic" surface emulation mode, no longer supported
fix: fixed InvalidateRect rectangle: avoids flickering in "Microsoft Pandora's Box"
add: added "Bypass MCI calls" flag. 

v2.03.36:
fix: fixed "fix movies color depth" flag for color depths greater than 8BPP. Fixes the "Horde 2 the Citadel" intro movie.
add: added the "Set KEY_WOW64_32KEY flag" flag to prevent registry redirection for win32 applications
add: virtual registry configuration integrated in DxWnd configuration and exported files
add: hooking of RegEnumValue() call. Useful for "Star trek Generations"
fix: fixed mouse handling through "Message processing" option for child windows. Fixes "Star Trek Armada" mouse problems
fix: fixed GetDC hooker for child windows. Fixes some "Star Trek Armada" graphic problems
fix: CreateCompatibleDC hooker printing wrong error messages in normal conditions
fix: fixed DrawText, DrawTextEx hookers returning wrong RECT structure. Fixes "Star Trek Armada" text placement problems
add: completed log messages for DrawTextEx, CreateDialogParam, DialogBoxParam and CreateDialogIndirectParam

v2.03.35:
add: added "Set ZBufferBitDepths capability" flag to set a no longer supported fields in the capability structure. This makes ""Banzai Bug" bypass the capability checks.
fix: better handling of surface capabilities in flippable mode
fix: fixed bug in primary surface handling when already created ...
add: added ""Share ddraw and GDI DC" flag: sharing was introduced in release v2.03.19, but proved to be incompatible in many situations. Now it is off by default and enabled when necessary.
fix: the flags for disabling DDSCAPS_SYSTEMMEMORY capability are now used also in non-emulated flipping emulation mode. Depending on the video card, it may help getting compatibility.
fix: handling of NULL DC in non emulated modes: the NULL DC (corresponding to the whole desktop) is replaced by the window DC. This reduces problems like clearing the whole desktop.
fix: EnumDisplayModes was returning wrong modes list in SVGA mode.
fix: applied the error suppression to the DeleteAttachedSurface hooker.
fix: hooked User32 CreateRectRegion/Indirect calls. 

v2.03.34:
fix: handling of real primary/backbuffer surfaces in non-emulated modes and WinXP: now uses system memory if possible, video memory otherwise. 
fix: simulation of backbuffer attach in non emulated modes.
fix: mciSendString hooker, was losing a final command line argument

v2.03.33:
add: new surface handling, more similar to D3DWindower. The previous mode can be activated by setting the "Flip emulation" flag. The new mode can successfully manage "Microsoft Motocross Madness 2".
fix: Reelease operation for primary surface when used for GDI GetDC operation
fix: protections for GDI ReleaseDC operations against null window or DC

v2.03.32:
add: flags "Ask confirmation on window close" and "Terminate on window close"
fix: implemented surface stack cleanup on overflow: this should allow "European Air War" to run through several seasons.
debug: added a few diagnostic messages to trap possible errors in DLL injection functions
fix: recovered the menu Edit->Add command that went disabled

v2.03.31:
fix: handling of "No SYSTEMMEMORY on BackBuffer" flag for no-emulation mode: necessary to run "Microsoft Motocross Madness 2" in this mode.
fix: allowing hot patching to GetCursorPos() user32.dll call. This allows correct mouse control with obfuscated exes, like the chinese RPG "Paladin 3".

v2.03.30:
fix: reset for flipped DC retrieved from ddraw interface after ddraw creation. Fixes "Powerslide".
add: added "Send WM_ACTIVATEAPP message" flag: fixes "Championship Manager 03 04" mouse and "Thorgal" lock
fix: FillRect hooker. Good for "Premier Manager 98" and "Premier Manager 97".
fix: revised the DDSCAPS_SYSTEMMEMORY suppression, now separated for primary & backbuffer surfaces

v2.03.29:
fix: wrong d3d7 hook, now "Echelon" works again.
add: "hide taskbar" option. A little experimental, so far. If the game crashes, the taskbar may not be reactivated: use menu commands to recover.

v2.03.28:
add: added "Lock / Unlock pitch fix" flag to compensate for pitch change in emulation mode. Fixes "Snowboard racer" slanted graphic
add: added "Width not power of 2 fix" flag to compensate for pitch change in emulation mode. Fixes "Microsoft Midtown Madness" slanted graphic
add: debug flag "Highlight Locked surfaces" to highlight service surfaces that are Lock-ed and Unlock-ed.

v2.03.27:
add: dxwnd proxyes to get standalone windowed mode (ask for instructions)
fix: added SetAppCompatData(1, 0) to AERO compatibility for lock operations - improves AERO compatibility, e.g. "Age of Empires" intro movie
add: added "Disable max window mode" flag for better compatibility with Win8/8.1
fix: cleaned up some opengl hooking code. Mind possible regressions ....

v2.03.26:
add: Added "Set KEY_WOW64_64KEY flag" flag to registry operations. It could prove useful for old programs on recent W64 platforms.
add: Added "fix movies color depth" flag. It fixes some problems when windows multimedia functions detect and process the actual video color mode and they should rather consider the virtual value. Fixes "Killing Time" intro movies decoding.
fix: suppressed hooking for comdlg32.dll module. Common dialogues never need to be altered. Fixes "Road Rash" save/load game dialog problems.

v2.03.25:
fix: CoCreateInstance hooking for IDirectDraw interface: fixes "Darius Gaiden"
fix: BackBufferCaps value for NOSYSTEMEMULATED option. Fixes "Forsaken" crashes in AERO mode.
fix: GDI GetDC and GetWindowDC must not redirect the zero hWnd to the current main window if not in fullscreen mode.

v2.03.24:
fix: added missing log for RegSetValueEx key value when type is REG_SZ (string)
fix: handling of DDERR_SURFACEBUSY error in blit operations, recovers "Virtua Cop"
fix: several changes in palette handling, improve (but don't fix!) "Man in Black" palette rendering
fix: EnumDisplayMode hooker, passing wrong vodeo modes to the callback routine. Fixes "Total Annihilation Kingdoms" crash
fix: DxWnd GUI build with very large width, causing slow interface responsiveness.
add: added the "Suppress Release on backbuffer" that makes "Tetris Worlds" working, avoiding to release the backbuffer surface too many times.

v2.03.23:
fix: implemented the correct interface of registry query calls that can be used to know the key data length 
fix: hexdump routine with NULL input used in registry operation log was causing crash
add: preliminary hooking for user32.dll desktop routines, like CreateDesktop() - makes "STCC Swedish TouringCar Championship" working

v2.03.22:
more code reorganization and cleanup
eliminated GDI over directdraw emulation (MAPGDITOPRIMARY flag)
add: support for texture D3DFMT_L8 dump ("Turok")
fix: eliminate a possible divide by zero error
fix: avoid intercepting WindowProc routines when set to special values 0xFFFFxxxx - makes "The Hulk" demo working

v2.03.21:
fix: full re-design of ddraw "special" surface management (primary & backbuffer): greater compatibility for emulation mode
fix: GDI edit window positioning for "Imperialism" city name selection

v2.03.20:
fix: revised hookers for kernel32 GetVersionExA/W api, adding support for dwPlatformId field
fix: texture handling for texture dump/hack/highlight
add: "Reserve legacy memory segments" compatibility flag, needed for "Crusaders of Might & Magic" 
add: preliminary support for ddraw7 DirectDrawCreateClipper api
add: "Suppress main window destruction" flag, to make "Prince of Persia 3D" playable
fix: fully revised ddraw surface handling in d3d1-7 versions. Now "Might & Magic IX" is supported

v2.03.19:
fix: scaling of movie coordinates rendered through MciSendString 
fix: doubled rendering to screen with fast 2x bilinear filtering
fix: crash when setting "showFPS overlay" with DirectX "hybrid mode"
fix: "keep aspect ratio" with DirectX "GDI mode"
fix: when ddraw hook is set to "none", no hook operations are performed on ddraw
add: possibility to control the D3D8/9 SwapEffect field with the "force swap effect" flag
fix: revised handling of GDI device context shared with DirectDraw primary surface, improved behaviour of "Star trek Armada"

v2.03.18:
fix: eliminated direct output to primary surface, causing more troubles than goods ("Divine Divinity" flickering...)
fix: handled CoCreateInstance calling CoCreateInstanceEx in hot patch mode.
fix: eliminated InvalidateRect calls in ddrow Unlock hooker: this is not the right way to fix "Deadlock II", sorry.
fix: DirectDrawEnumerateEx log
fix: cursor is hidden in fullscreen mode only
fix: CoCreateInstance & CoCreateInstanceEx wrappers. Now "Crusaders of Might & Magic" is playable in window

v2.03.16
fix: MapWindowPoints hook - added coordinate scaling (fixes "NBA Live 99" components size and position)
fix: using "Suppress D3D8/9 reset" sets the backbuffer area as large as the whole desktop to avoid clipping
add: added "Unlock Z-order" flag to avoid window to stay locked on top of z-order (useful for "NBA Live 99")
add: added "EA Sprots hack" flag to suppress some interfering hooks set by EA games internally (useful for "NBA Live 99")

v2.03.15
fix: keep aspect ratio calculations
fix: d3d Present method, to properly scale to window size (fixes "Silent Hunter III" rendering)
fix: GetMonitorInfo hooker: in windowed mode the call may fail, a virtual size and ok retcode should be returned
fix: Blt method recovering errors when D3D CreateAdditionalSwapChain method fails: allow to see the intro movies of "Silent Hunter III".
fix: added some missing D3D errorcode labels in log file

v2.03.14
add: window commands Minimize, Restore & Close
add: made palette behaviour configurable (until better understood...)
add: "updatepaths" tag in dxwnd.ini

v2.03.13
add: integrated printscreen facility with 8BPP paletized dump, activated by "printscreen" special key (default Alt-F12)
fix: recovery for lost surfaces (i.e. after a Ctrl+Alt+Del command)
fix: palette display form showing active entries
fix: EnumDisplayModes hook in emulate surface mode
fix: several log improvements

v2.03.12
fix: several issues in fake registry hooks
add: continue char '\' in fake registry configuration for hex values
add: GetClipList ddraw hooker: fixes "Full Pipe" blitting coordinates
fix: proper handling of palette reserved entries
fix: dinput GetDeviceData
add: implemented "Release mouse outside window" functionality in dinput calls
fix: now "Release mouse outside window" leaves the mouse close to where it left the window
fix: in GUI, keeps memory of last used paths
fix: several log messages

v2.03.11
fix: added DDSCAPS_FLIP capability to emulated backbuffer surface. Fixes "Injection" missing flips
fix: added high order bit set to kernel32.dll GetVersion() wrapper in case of Win95/98 emulation. Fixes "Warhead" frontend program
fix: coordinates returned by user32.dll GetClipCursor() must be scaled accordingly with virtual desktop size. Fixes "SubCulture" mouse control problems

v2.03.10
add: real time logging through OutputDebugString
add: menu command to launch DbgView.exe
add: support for timestamped logs
add: "Erase trace file" flag
fix: handle leakage for primary hdc, causing rapid performance downgrade
fix: log message reorganization
add: SetStretchBltMode HALFTONE in GDI mode to provide bilinear filtered GDI stretching
fix: some GDI mode optimization - should bring performances similar to D3DWindower, with similar configuration
fix: GDI mode bug, crashing when blitting from NULL surface (to do a color fill operation)

v2.03.09
code reorganization & reuse
add: "Set texture pixel format" flag, makes "Jeff Gordon XS Racing" working on emulated mode and 32BPP desktop
add: "GDI mode" emulation uses HALFTONE to activate GDI bilinear stretching when "Full Bilinear" filter is activated: slower, but better quality
add: preliminary hooking for EnumZBufferFormats ddraw7 method
fix: eliminated some handle leakage when injecting launched processes

v2.03.08
add: "GDI mode" ddraw surface handling mode
add: "center to window" option
add: "message pump" input flag - fixes Arxel Tribe games (Faust, Legend of the prophet & assassin, ...)
fix: "Viper Racing" crash
fix: "limit resources" option for "Tiger Woods PGA World Tour 08"
fix: "limit resources" option for "Jeff Gordon XS Racing demo"
add: debug "stress resources" option
fix: added a couple of directinput diagnostic logs
add: debug option "freeze injected son"
add: made check for admin rights configurable in dxwnd.ini
add: "portable" capability to configure relative pathnames
fix: small bug in game menu string width - now game titles should not be trunked any longer.

v2.03.07
fix: key matching for virtual registry now made case insensitive (needed for "Die Hard Trilogy")
fix: handling of null values passed to extRegQueryValueEx as lpType and lpData arguments (needed for "Die Hard Trilogy")
fix: DirectDrawSurface::GetPalette returns the virtual palette when applied to virtual primary / backup surfaces (needed for "Die Hard Trilogy")
fix: fixed dump for 8BPP palettized textures (needed for "Die Hard Trilogy")
fix: handling (with no operation) of D3DFMT_Q8W8V8U8 texture type and other bumpmap formats (used by "Tiger Woods PGA Tour 08")
fix: handling of LIMITRESOURCES flag for DirectDraw::GetCaps method when memory exceeds 0x70000000 bytes
fix: handling of LIMITRESOURCES flag for Direct3DDevice::GetAvailableTextureMem method when memory exceeds 1GB
fix: don't change screen resolution in SetDisplayMode when wrong (negative) values are passed. Fixes a problem in binkplayer.exe
fix: fixed OutTrace to avoid possible infinite recursion when loading C runtime libraries and logging LoadLibrary activity
fix: eliminated critical races when using DLL injection, thank to Luigi Auriemma's suggestion (inject an endless loop in the main thread and remove it at the end of injection)
fix: implemented DLL injection according to Luigi Auriemma's schema in CreateProcess hooking routine (needed for "Die Hard Trilogy")
fix: using MinHook library to acquire compatibility with all APIs
fix: hooked GetExitCodeProcess to handle "SUPPRESSCHILD" special case
fix: using hot patching for SystemParametersInfo APIs
fix: in SystemParametersInfo suppressed invalid operations in window mode: SPI_SETKEYBOARDDELAY SPI_SETKEYBOARDSPEED
add: son process handling with 4 different cases: 2 old cases (default case and "SUPPRESSCHILD") plus "INJECTSON" and "ENABLESONHOOK" to hook the son process without/with DLL injection
add: debug color conversion mode through GDI routines
add: multi-hooking for multiple processes contemporarily, adding the line "multiprocesshook=1" in [window] section of dxwnd.ini. Use at your own risk!
add: partial logging of Direct3DDevice::GetDeviceCaps output (to be completed)
add: handling of notes in the DxWnd GUI (configuration notes tab)
mod: when log is not possible on program's folder, it is no longer written in %TEMP% dir, is now written in DxWnd local dir.

v2.03.06
fix: do not try to set vsync delays on ddraw surface when not created yet 
fix: catched several sporadic errors before they could crash the application
fix: GetAttachedSurface() now retrieves a backbuffer from the list, instead of referencing the last one - this fixes "Tomb Raider III" GOG release in non emulated mode.
add: "Normalize performance counter" flag to fix an improper use of QueryPerformanceCounter() made by "Cyber Gladiators"
add: "GDI Color conversion" debug flag

v2.03.05
add: texture dump for d3d8 & d3d9. Note: highlight & hack yet to be implemented. d3d10 & d3d11 yet to be implemented. Texture types not complete.
fix: handling of d3d10 (Assassin's Creed) 

v2.03.04
fix: changed surface capability policy so that "Risk II" works with identical surface pitches

v2.03.02
fix: when creating an IID_IDirectDrawGammaControl object through lpdds->QueryInterface redirect fake primary surface to real one to make the object working. Fixes "Might & Magic 7"
fix: fixed logging of ddraw Set/GetGammaRamp methods. Added SUPPRESSDXERRORS error suppression
add: texture extraction parameters (min & max x,y size) in dxwnd.ini config file
add: texture extraction / hack made in DxWnd install folder, to allow usage also for games running from unwritable (CDRom) directory
fix: in "prevent maximize" mode, set the main window in maximized state. Allow "Hoyle Casino Empire" to continue after the intro movie is terminated
add: Process kill command now warns you about existence of task
fix: DxWnd window position save now works on multiscreen desktop

v2.03.01
fix: fixed some bitmap handling while processing textures
add: texture management dump to texture.out folder and hack from texture.in folder
fix: AERO optimized mode with ddraw7 only surfaces: "Hoyle Casino Empire" works at least in fullscreen mode.
fix: bilinear filters compiled with optimized and openmp flags - big speed improvement.
fix: reduced the maximum amount of visible video card with "Limit available resources" flag: now "Breath of Fire IV" works.

v2.02.99
fix: completed bilinear filtering for 16bpp desktop color depth
fix: fixed surface handling, Suppress SYSTEMMEMORY options now moved to debug flags (maybe unnecessary)
fix: AERO compatibility is now declared just once
fix: fast bilinear blitting in the 32 -> 32 BPP case
add: texture hooking
add: texture management
add: Direct3D dedicated configuration tab

v2.02.98
fix: better ddraw surface handling - now  "Darkened Skye" runs in emulated surface mode with perfect colors
fix: fixed RDTSC opcode search loop - fixed time stretching for "Ubik"
add: added "Peplace privileged opcodes" flag - makes unpatched "Ubik" run
fix: revised ddsurface capabilities policy to allow D3D1-7 games to run in emulated mode and bilinear filtering

v2.02.97
fix: mouse black trails in "Deadlock II"
fix: missing default value for filter mode

v2.02.96
fix: FPS inticator on window title was causing the program to become irresponsive (partial fix)
fix: proper setting of ddraw surface capabilities will allow primary surface emulation for most D3D1-7 games
fix: missing initialization of variables in screen size limit handling
fix: processing of mouse messages 
fix: GetSystemMetrics and LoadLibrary* calls hooked by hot patching to fix "Wind Fantasy SP" movie problems
fix: completed winmm multimedia api hooking to fix "Wind Fantasy SP" movie problems, and not only....
fix: revised FPS control to assure more stable fps when a FPS limit delay is set 

v2.02.95
fix: handling of limit resolution field
fix: FPS handling to screen updated made by SetDIBitsToDevice
fix: PeekMessage implementation with "Peek all message in queue": fixes "Shadow Watch"
fix: missing hook to CreateProcess - needed for "Suppress child process creation".
fix: exception for bilinear filtering applied to certain games (e.g. "Shadow Watch")

v2.02.94
fix: handling of "Keep aspect ratio" for aspect ratios different from 4:3
fix: missing resolution 400x300 in "Limit resolution" handling
fix: surface handling switch in automatic mode. 
added two flags for DDSCAPS_SYSTEMMEMORY handling on different contexts. Added extra performances and compatibility with D3D in emulated mode.
added "Updated bigger than 1/4 screen size" flag, to perform FPS counting or limitations upon substantial screen updated (heuristic: bigger than 1/4 of the total size...)
added true bilinear filter. Previous one was renamed as fast 2x filter (BILINEAR2XFILTER).
fix: when the window was minimized & the "do not notify on task switch" flag was on, the game crashed. Now the blitting operations to invalid rect coordinates are suppressed.
fix: debug option "highlight blit to primary" was causing game crashes when the updated rect was too little. 
fix: trapped sporadic CloseHandle(hMenu) crashes in a try/catch clause.
fix: attempt to fix FIXD3DFRAME option
fix: added SetStretchBltMode(hdc,HALFTONE) to SetDIBitsToDevice hooker to improve stretching: fixes "Celtic Kings Rage of War"

v2.02.93
fix: handling of primary / backbuffer surfaces when DDSCAPS_3DDEVICE is set (es. Fifa 99 in Direct3D mode)

v2.02.92
fix: "Keep aspect ratio" fixed for AERO environment
fix: improved "Oprtimized for AERO mode" compatibility, both when set / unset.
fix: Skip / Limit FPS now acting on full scren blits only. For instance, they no longer slow cursor sprite.

v2.02.91
fix: bug in bilinear filtering mode causing possible game crashes when blitting partial rectangles (es. Age of Empires II cursor)
added: AERO friendly mode - brings AERO compatibility and incredible performance boost for ddraw games in surface emulation mode

v2.02.90
added: "Filter winposchange messages" flag - needed for "RollerCoaster Tycoon / Deluxe / II" games
fix: correct handling of texture surfaces for "Zoo Tycoon" in surface emulation mode.
fix: handling of GDI32.dll calls in GetProcAddress
fix: improved logging
fix: GetRgnBox hooking moved to normal window processing, to handle RollerCoaster Tycoon series.
added: GetTempFileName fix to handle Win95 differences. This makes "Powerslide" playable.

v2.02.89
fix: added SVGA 512x384 video mode. Necessary for "Outcast" loading screen.
fix: using WINNLSEnableIME call to suppress IME window. 
fix: improved some logging

v2.02.88
fix: DIABLOTWEAK flag defaulted to FALSE
add: CLEARTARGET flag - forces a Clear CLEAR_TARGET operation on D3D BeginPaint: useful in combination with wireframe mode.
updated "Optimize CPU" behavior: now it emulates a VSync wait also on D3D8/D3D9. Fixes some "swimming" problem in "Soul Reaver 2", and saves CPU time by limiting the FPS to the refresh rate.
fixed positioning of FPS and time stretching indication on screen overlay
cleaned up GUI project: no more japanese and localized resources, no more unreferenced resources.
GUI: added Desktop status window
GUI: added pixel format descriptor to DxWnd status

v2.02.87
attempt to fix Win8 missing support for earlier ddraw releases (Croc legend of the Gobbos, Project I.G.I., ...)
fix: optimized bilinear filtering with no horizontal / vertical sawtooth artifacts
added support for 16BPP and 32BPP desktop bilinear filtering
suppressed compatibility checking
fix: doubled default window size when bilinear filtering is active
added repositioning of control parent window when main win is moved 
added debug messages about hooked window class and name
fix: ignore IME window when hooking main win, destroy it when requested
fix: ignore HWND_MESSAGE type windows
added "Diablo tweak" compatibility flag for Diablo proper screen update handling. It is a temporary tweak waiting to understand better how to properly manage it.

v2.02.86
fix: thank to aqrit's research, hooked and hanlded user32.dll GetUpdateRgn API. This fixes refresh and crash problems in Diablo & Hellfire.
added bilinear filtering to directdraw palettized 8BPP emulated mode (where it is mostly needed!). A must try is "Genocide" !

v2.02.85
fix: revised handling of d3d D3DFORMAT field: fixes "Call of Cthulhu DCotE" color problems
fix: added recovery for rounded child win coordinates: fixes blitting problems in Diablo when win size is not an exact multiple of native resolution.

v2.02.84
fix: removed extra reference to ddraw session causing window movements on fullscreen game exit: fixes another "Wind Fantasy SP" reported bug

v2.02.83
fix: small error in one coordinate scaling procedure
fix: completed translation in italian (just for testing....)
fix: added palette window activation in icon tray menu
fix: supppressed BackBuffer release within D3D7:CreateDevice: this makes "Tetris Worlds" fully playable
add: build-in check for compatibility modes set!

v2.02.82
fix: completed chinese translation and language selection through dxwnd.ini file.
added very preliminary (and incomplete) italian translation for debugging.

v2.02.81
fix: hooked GetMonitorInfoA/W that is a possible way to get the screen resolution. The hooker sets the virtual screen size  updating both rcWork & rcMonitor rects in the MONITORINFO structure. This fixes the mouse control in "Tomb Raider Underworld" and likely in other games with a similar engine.

v2.02.80
Preliminary multilanguage release, english and chinese, many thanks to gsky916 support
fixed Sleep bug for small delays and accelerated timeshift: fixes "Wind Fantasy SP" reported bug
added log for DirectInput::SetCooperativeLevel flags
fixed flip emulation mode when backbuffer is lost: fixes cursor problems in "HellCopter"
fixed Pitch setting in DirectDraw::EnumDisplayModes when in SVGA emulation mode. Let "Outlive" use this setting

v2.02.79
fix: recovered capability to "pin" ddraw and d3d calls

v2.02.78
fix: hooked D3D8/9Device::BeginStateBlock
ddraw API hot swappable
log: helper for ChangeDisplaySettings() flags
fix: hot patch handling of loaded modules
log: fixed some messages with no line ending for GetObjectType()
d3d begin of code reorganization and cleaning
ole32, user32 API hot patched
handling of DWL_DLGPROC message 

v2.02.77
GUI: added program icon display
fix: some log messages
fix: handling of GetProcAddress D3D9 api 
fix: handling of EMULATEBUFFER option - fixes "Star Trek Birth of the Federation" 
fix: expanded hot patching scope - now can handle QueryPerformanceCounter and fix time stretching on obfuscated version of "Wind Fantasy SP"
added logging in D3DDevice9::GetAdapterIdentifier 

v2.02.76
fixed and enhanced several features about registry emulation: added flags "Emulate registry" (to add missing entries) and "Override registry" (to fake existing entries with different values). Fixed "Requiem Avenging Angel" DirectX bogus check bug.
fixed bug in emulate surface palette handling affecting "Requiem Avenging Angel" colors

v2.02.75
Hooked D3DDevice::ShowCursor method to apply force show/hide cursor (preliminary...)
fix: dynamic allocation for WndProc stack to eliminate predefined limit of 256 entries. Star Wars Rebellion uses more than 256.
added: "Suppress D3D8/9 Reset" flag
fix: improved show FPS and Time Stretch overlay so that the two overlays won't overlap each other
fix: bug in ddraw "Locked surface" mode preventing output on screen
fix: fixed bug in critical common portion of the code that was crashing even unhooked programs (namely, Flash Player, ...)

v2.02.74
Add: API hot patching, thank to aqrit precious support. Now Doom 3 is played in window.
hooked GetCursorInfo call
fix: recalculate vsync timers in case of time stretching
fix: log failure for CreateWindowExW wchar api
Add: configurable keyboard commands
Add: timestretch toggle command
Add: "Enable hotkeys" flag to enable/disable hotkeys
fix: eliminated dialogs with timer warning messages (to be investigated...)
fix: moved "no banner" flag to main tab again
fix: potential string violation, possible cause for flash crash?

v2.02.73
Add: preliminary support for (unchecked) "Run in window" flag. Now if the flag is unchecked, DxWnd does NOT force the program to run in a window. Still, some rendering modes are not working perfectly.
add: support for Unicode DefWindowProcW API call
fixed some differences in D3D CreateDevice vs. CreateDeviceEx handling
added "Window frame compensation" input flag to set the cursor position properly in "Sub Commando"
fix: properly handled the case of NULL WindowProc routine in CreateDialog/Indirect API calls. Makes "LEGO Stunt Rally" working.
added support for Unicode SystemParametersInfoW API call

v2.02.72
fix: fixed Launch field used with "Use DLL injection" flag
fix: somehow improved "Use DLL injection" to avoid blocked tasks and allow exception handling

v2.02.71
fix: Set/GetWindowLongA/W are always hooked.
fix: added user32 GetDCEx hook for GDI Emulation & Directraw mode
fix: hooked "FrameRect", "TabbedTextOutA", "DrawTextA", "DrawTextExA", "FillRect" in scaled mode only
fix: FIXNCHITTEST mode
fix: when main win is closed, blit area is made null to avoid messing with a wrong screen area
added "Release mouse outside window" option. This option causes the get cursor position to detect a centered mouse position when the cursor is moved outside the window, allowing interaction with other windows without scrolling ot the windowed program. Mainly, this option is meant to help people with a physical disability to use other programs (e. g. the virtual keyboard) to play games.
added "Launch" field (optional) to start the program with arguments or a separate task

v2.02.70
GUI:
fixed saving wrong coordinates when dxwnd is terminated while minimized
added check for adminstrative capabilities on startup
DLL:
fix: moved DrawText/Ex api hooks into right library
fix: corrected child window procedure handling - fixes "Imperialism" child window closing 
fixed FillRect coordinate handling - fixes "Imperialism" menus
fixed SetWindowPlacement handling

v2.02.69
improved exception catching to intercept memory violation exceptions and to neutralize offending code. Thank again to olly didasm lib, now the assembly instruction length is automatically determined so that the opcode can be replaced by the correct number of NOP instructions. If this doesn't mean much for you, just consider that this makes playable the Win95 (patched) release of "Star Wars Tie Fighter".
fixed FillRect user32 call - brings some improvements to Imperialism (still far from being acceptable).

v2.02.68
added screen resolution choice: either a set of SVGA resolutions (mostly for 3:4 monitors), HDTV resolutions (mostly for 16:9 monitors) or the native set of resolutions offered by your monitor / video card.

v2.02.67
fixed log for D3D CreateDevice method, missing in some D3D interface versions
added wildcarded program path: now you needn't set the full pathname, but it is sufficient to specify the rightmost part, e.g. the executable filename.
added emulated mode color conversion from 32BPP to 16BPP. Despite the fact that the best way to run a 32BPP game is against a 32BPP desktop, now it is possible to downgrade 32BPP colors to 16BPP. This fixed the fact that 32BPP games run on a 16BPP desktop showed simply black screens (e.g. Baldur's Gate II)
fixed logic for EnumDisplayModes implementation: now screen resolutions and, for emulated mode only, also color depth, are generated by DxWnd and not derived directly from the real ones. This make it possible to fake support for very low screen resolutions (e.g. 320x200) on modern screens that support them no longer. This make "Genocide" running on both supported resolutions 320x200 and 320x240.

v2.02.66
fixed palette bug: "Virtua Fighter PC" now shows correct colors.

v2.02.65
added "Fine time adjust" flag to finely set timeslider by 10% steps ranging from about :2 to x2
fixed a bug in DWORD timers introduced in v2.02.60: now "Warcraft 2" timing works again
revised logit to calculate delays to emulate VSync timing - now should be more accurate and depending on actual screen refresh rate

v2.02.64
GUI: implemented multiple file import
DLL:
fixed a ddraw session reference count error that prevented "Jet Moto" to start
fixed "Compensate Flip emulation" for ddraw7 games: now "Empire Earth" supports this flag.
fixed CloseWindow hook: now games that minimize/restore the main window on task switch can possibly recover (e.g. Hundred Swords)
fixed process hook: now it should be a little more robust and efficient. Hopefully should fix some Macromedia Flash problems.
fixed IDDrawSurface::Lock() prototype. Not gameplay improvements, though...
added DirectX "Return 0 ref counter" option as quick & dirty solution to many reference counter problems!
fixed DirectDrawCreateEx failing to register the main ddraw session handle
fixed palette problem in emulated mode: palette must be applied to backbuffer surface as well. Now "Duckman" and "Total Soccer 2000" show perfect colors.

v2.02.63
fixed d3d hooking for Reset and GetDirect3D methods: now Jumpgate works at any available resolution, and the "Add proxy libs" option is no longer necessary
fixed "Compensate Flip emulation" option for non emulated surface mode: fixes mouse artifacts for "Rogue Spear Black Thorn" 
added preliminary handling for "Disable Textures" option

v2.02.62
fixed d3d7 CreateDevice hook and d3d7 rendering options (wireframe, disablefogging, zbufferalways)
fixed doublebuffer emulation - avoid returning the backbuffer when zbuffer is requested. Fixes rendering problems of many games!

v2.02.61
added directx "Compensate Flip emulation" flag: used primarily to get rid of mouse artifacts due to emulated Flip rpocedure in windowed mode. Fixes Gruntz issues and some other games...

v2.02.60
fixed time stretching to handle Total Soccer 2000
fixed ZBUFFER surface creation to handle GOG Incoming troubles on some cards
fixed handling of special effects (wireframe, no fogging, zbuffer always) to be valid on all d3d versions

v2.02.59
fixed IDIrectDraw reference counter for ddraw release 1 session: fixes "Warhammer 40K Rites Of War" first screen
fixed QueryPerformanceCounter handling: fixes "New York Racer" time stretching problems

v2.02.58
Added Automatic DirectX mode: not workink always, but should make it much easier to guess a proper surface setting.
Automatic mode allows dynamic mode change depending on the situation: it is necessary for Populous 3 D3D version.
fixed BltFast operation with SRC color key (often used to draw cursor from texures) directly to primary surface: this makes the cursor visible in Populous 3.
minor fixes on logging

v2.02.57
fixed WS_NCCALCSIZE & WM_NCPAINT messages to ensure a proper window's border and size - fixed Black Thorn window problems
fixed "Hide desktop background" flag 

v2.02.56
hooked RegisterClassA API (useless so far...)
fixed bug in SendMessageW hooking: caused truncated text in window titles and form captions
fixed several d3d1-7 hooks: prevented some d3d games to properly work, i.e. "Thief the Dark Project"
fixed d3d GetDisplayMode to return virtual desktop size - fixed "Affari Tuoi", an italian game.
fixed some log messages

v2.02.55
added "Hide desktop background" video option

v2.02.54
fixed a bug in d3d GetAvailableVidMem that was causing too many troubles...
fixed a bug in Suppress IME option
fixed a bug in handled library list, possibly affecting d3d/d3d7 games
avoid fixing window style for non-desktop windows (fixed a Port Royale 2 bug)
eliminated "Disable HAL support", no longer necessary
some incomplete work on gdi game handling
added preliminary, incomplete (not working) glide handling

v2.02.53
Handling of ddraw screen color depth 15BPP - in "Hesperian Wars"
fixed IAT scanning: now function addresses for remapping are searched in possibly multiple instances of dll text segment - in "Aaron Hall's Dungeon Odissey"
hooked SystemParametersInfo call
fixed window size logic to check for window menu bar, considering also the WS_CHILD case.
fixed window move/resize when message processing is enabled: coordinates outside the child area or within ENTER/EXITSIZEMOVE must NOT be altered.
expanded timer processing to include user32 Set/KillTimer - in "Aaron Hall's Dungeon Odissey"
fixed possible recursion while hooking child window procedure identical to parent's one
fixed gdi emulation on top of ddraw surface, enough to play "Sid Meyer's Civilization III".

v2.02.51/52
Hooked window timers: now "Ed Hunter" can be time stretched
added NOFILLRECT debug option
fixed WM_DISPLAYCHANGE handling (x,y swapped coordinates?)
fixed int64 arithmetic for performance counters
Added (untested !!!) handling for RDTSCP opcode with "Intercept RDTSC" option
Added "Limit screen resolution" option: seems necessary for "Sid Meyer's Civilization III" to work
Improved initial window coordinate & style handling
fixed virtual screen size showing into status panel
hooked SetPixelFormat, GetPixelFormat, ChoosePixelFormat and DescribePixelFormat to redirect desktop hwnd and make wglCreateContext work (needed for Civ III)
fixed log message for TextOut parameters
hooked DisableD3DSpy (invoked by The Bard's Tale)
fixed extglViewport coordinate remapping when invoked with CW_USEDEFAULT values (Civ III)
fixed bug in DirectDarawCreate/Ex hooking with wrong module handle

v2.02.50
Added "Intercept RDTSC" option: so far, tested on Unreal Tournament only.
To implement RDTSC detection, I owe a big thank you to Olly who shared the disasm lib used to browse the assembly code.

v2.02.49
Separated ddraw, d3d and dxwnd log messages
Hooked GetAvailableVidMem method to limit memory values
Hooked ChangeDisplaySettings call in both versions ANSI and WIDECHAR, to prevent messing up with the screen
Added HookWindowProc() subroutine, called at dxhook.c@1534
Added "Add proxy libs" flag - so far to copy a d3d9.dll proxy lib to fix d3d9 games
Hooked d3d8/9 SetCursorPosition method
Hooked d3d8/9 AddRef / release methods (for logging)
Fixed D3DDevice8/9 hooking
Hooked QueryPerformanceFrequency
Fixed a bug in d3d7 hooking
GUI: added desktop color setting

v2.02.48
New options: "Disable fogging", "Textures not power of 2 fix"
A few fix to prevent game crashes

v2.02.47
GUI:
Configuration tabs reorganization: now some tabs have merged making it easier to reach all options
core:
improved d3d wrapping and sdded ZBUFFER cleanup flags to fix some d3d games. In particular:
Clean ZBUFFER @1.0 fix: to fix "Star Wars Episode I Racer"
Clean ZBUFFER @0.0 fix: to fix "Crimson Skies" black blocks bug on ATI cards

v2.02.46
Fixed handling of user32 CreateDialogIndirectParam call to avoid processing in non fullscreen mode - fix necessary to start "Crimson Skies".

v2.02.45
A small fix for ddraw 7 games that makes Praetorians playable, though with some problems!

v2.02.44
Improved debug logging for all palette operations
fixed a bugged log causing the crash of Empire Earth in debug mode
added show time stretching flag - preliminary version. Some code cleaning as well.
change in automatic screen refresh - should make the "palette update don't blit" flag obsolete.....
added a static definition for the default system palette
added interception for CoCreateInstance following cases:
	case 0xe436ebb3: Module="quartz"; Class="CLSID_FilterGraph";
	case 0x4fd2a832: Module="ddrawex"; Class="CLSID_DirectDrawEx";
	case 0x49c47ce5: Module="amstream"; Class="CLSID_AMMultiMediaStream";
preliminary (proxed) interception for "GetActiveWindow" and "GetForegroundWindow"

v2.02.43
Several changes for Empire Earth:
fix: restore 16BPP color mode after directdraw session termination
fix: recovered window destruction
fix: proper handling of MIPMAP and LOCALVIDMEM surfaces 
fix: color depth handling - now when the program terminates the desktop is brought to the original color depth

v2.02.42
added hook to kernel32.dll QueryPerformanceCounter API to enable time stretching to Rayman 2
handled GetAttachedSurface emulation for FLIP capability on primary surface - that makes the intro movies of Empire Earth visible.
Some code cleaning
Cleared invisible debug flags ...

v2.02.41
Added "suppress child process creation"flag. This flag is introduced to manage games such as "Heart of Iron 2" that start intro movie by means of a separate process. HoI2 is starting the binkplay.exe program in the game's avi subfolder. This way you don't get a windowed movie, but completely suppress it.

v2.02.38-40
Fixed coordinate calculation for blit operations when keeping aspect ratio.
Fixed window coordinate placement in desktop / client area mode for programs with a visible menu bar.
Changed "Lock win coordinates" flag: now it allows window movement / resizing when driven by mouse input.
Fixed a bug in GDI "Map DC to primary surface"mode.
Added palette display window to GUI

v2.02.37
code reorganization
GUI:
removed useless flags and moved debug options in a hidden tab

v2.02.36
hooked GetAttachedSurface to non-PRIMARY surface to track ZBUFFER attach to BACKBUFFER. Useless, so far...
revised capability handling in CreateSurface 
hook trace is now activated from a separate log flag to reduce debug log size
added "Peek all messages in queue" to avoid queue saturation and automatic task kill in Win7 (thank to P K help)
fixed message handling in the case messages are passed to a routine handle

v2.02.35
fixed BACKBUFFER surface attributes in direct (not emulated) mode: Rayman 2 playable again
added FILTERMESSAGES flag ("filter offending messages") to eliminate some problems to games not developed to work windowized (Rayman 2, Mirror's Edge ...)
fixed bug crashing the program when "keep aspect ratio" is selected with window size set to 0

v2.02.34
CORE:
Much better surface description in log
Completely revised CreateSurface hook: emulated and direct code are merged as much as possible, and reference counter are kept accurate. Now most games can work both in emulated and direct mode.
Fixed surface capabilities for "Vangers", "The Sims" (now working in emulated mode again)
Updated "Fixed aspect ratio" option: now uses the x,y size declared in the configuration instead of the fixed 800 x 600 standard one.
Added virtual fullscreen "Desktop" mode.
Completely revised the Peek/GetMessage handling: now it uses the standard SetWindowHook API instead of the address redirection, making the handling more reliable and accurate: now "Age of Empires III" works at last!
GUI:
Added virtual fullscreen "Desktop" mode.
Added "Fix buffered IO after Win98" flag: this is meant to fix an incompatibility in the ReadFile kernel32 API that must read from block boundaries, where a block was smaller up to Win98 and bigger after it.
As a result, games like "Dylan Dog Horror Luna Park" show IO errors while reading data from the CD. Unfortunately, this is not the only problem of this game, so Dylan Dog fans will have to wait further.
Maybe some other programs suffer of this problem, and I will strongly appreciate if anyone finds some.

v2.02.33
CORE:
experimental attempt to emulate the ZBUFFER attach to the BACKBUFFER surface
improved log to detail all ddraw object properties
experimental emulation of RGB to YUV conversion to investigate "duckman" problems on Win7
improved grayscale algorythm
fixed missing DDPF_ALPHAPIXELS property causing several blit incompabilities
fixed surface handling for "Submarine Titans"
fixed mousewheel handling
fixed hooking bug causing several problems (i.e. Age of Empires III)
Added FULLSCREENONLY option: fixes "Submarine Titans" intro movies
Added "Bypass font unsupported api" compatibility flag: makes "Stratego" working
fixed several d3d8/9 hooking problems
GUI
added controls for new core features
defaulted ddraw emulation mode to surface emulation

v2.02.32
CORE: 
added new GDI handling mode, "Emulated devce context". Currently tested successfully on "G-Nome" only.
fixed "Prevent maximize" windows handling avoiding to disable topmost style for child windows
fixed handling of DDSCAPS_3DDEVICE surfaces in emulated mode: "Nightmare Ned" is working ok, "The Sims" works better (still crashing..)
fixed banner handling in case of device context remmapped coordinates
fixed memory leakage in SetDIBitsToDevice hooker
GUI
added radiobutton group to set the preferred GDI emulation mode

v2.02.31
CORE:
disabled annoying compilation security warnings
improved advapi32.dll hooking and logging
handling of complex primary surfaces with backbuffer surface inherited from one directdraw session to the following and with different interfaces
fixed SetCooperativeLevel handling in case of WINDOWED mode against desktop hwnd==NULL
fixed GetGDISurface in EMULATED mode 
hooked gdi32 API set for Imperialism I & II and 688 Hunter Killer:GetClipBox, Polyline, PolyBezierTo, PolylineTo, PolyDraw, MoveToEx, ArcTo, LineTo, StretchDIBits, SetDIBitsToDevice, SetPixel, Ellipse, Polygon, Arc, CreateEllipticRgn, CreateEllipticRgnIndirect, CreateRectRgn, CreateRectRgnIndirect, CreatePolygonRgn, DrawTextA, DrawTextExA.
fixed gdi32.dll hooking for TextOutA, TabbedTextOutA, Rectangle, BitBlt, PatBlt, StretchBlt, CreateFont, CreateFontIndirect
improved ddraw proxy logging
added / fixed user32.dll API FrameRect, TabbedTextOutA, CloseWindow, DestroyWindow, SendMessageW
opengl: fixed glCreateContext, wglMakeCurrent to make Descent III playable in opengl video mode.
fixed DeferWindowPos hook to make Imperialism II working
fixed SM_CXVIRTUALSCREEN, SM_CYVIRTUALSCREEN properties
fixed window class logging causing program crash
GUI:
added "GDI/Scale font parameters" & "DirectX/Disable HAL support" options

v2.02.30
CORE:
Some ddraw::GetDC log message fixes
added preliminary registry emulation - tested ok with "duckman"
added separate flag for preliminary registry operation trace
added CDROM drive type emulation - tested ok with "Fighting Forces" RIP
fixed FixCursorPos routine: cursor x,y compensation must always use pseudo-fullscreen window parameters - fixes Imperialism II mouse problems
fixed Desktop Workarea position mode for ddraw games
added CoCreateInstanceEx hook - "Final Fighter" seems to be using it, but who knows why...?
added "Don't move D3D Rendering Window" window option to make "Fable Lost Chapters" working  
GUI:
updated default values on new entry creation to map most-likely-to-work parameters

v2.02.29
CORE:
hooked DirectDrawSurface::Lock method to center primary surface memory updates centered into the window (in no emulated mode)

v2.02.28
CORE:
eliminated experimental ICSendMessage and ICOpen hooks, preventing some games (Alien Nations) to work
added mciSendCommand hook to fix video playback
fixed MoveWindow bug - still, it's unknown the reason why some programs (Emergency) tries to MoveWindow upon the hWnd=0 desktop!
fixed MapWindowPoints hook (Alien Nations)
fixed desktop detections in some places, now correctly using dxw.IsDesktop() method
Now "Alien Nations" and "Emergency" working (almost) perfectly.
fixed d3d8/9 hook to Get/SetGammaRamp. This should make a great number of recent games playable again.

v2.02.27
CORE:
fixed GetDC/ReleaseDC ddraw implementation to refresh GDI operation on primary surface. Warlords 3 text is now visible.
preliminary implementation of MapWindowPoints - to be tested
GUI:
Fixed log flags wrong initialization

v2.02.26
CORE:
Fixed Black&White mode for 16BPP color depth
Revised hooking code, now more compact and clear....
Fixed DxWnd splash screen, for those who love it
Increased child win table - now 688 hunter killer works perfectly 
GUI:
Added /debug flag to enable debugging options
Revised hooking code, now more compact and clear....
Restored Hook child win option

v2.02.25
CORE:
Added 'Single CPU Process Affinity' flag to let old games run on a single CPU core.
Fixed bug causing possible crash using 'preserve surface caps' flag
Switched lpDDSBack reference when backbuffer is upgraded through QueryInterface
Eliminated lpDDSBack clearing when refcount is zero - why is this working better? Needs further investigation
Added DDSCAPS_3DDEVICE capability when in EMULATED mode
GUI:
Added Kill process by name functionality (right click menu on program's list)
Fixed GUI initial position: now checks for desktop size to fit GUI within visible borders
Added single cpu process affinity checkbox in compatibility tab

v2.02.23/24
Fixed "disable setting gamma ramp" flag to intercept both GDI and D3D calls
Fixed client workarea setting to occupy the whole client area even when preserving aspect ratio (it draws black rectangles to the left/right or top/bottom side)
Added DisableThreadLibraryCalls optimization
Added B&W screen simulation (for primary emulation only)
Improved the primary emulation capacity to handle 3D games
Added the "preserve surface capabilities" flag
Fixed an hooking bug for user32.dll

v2.02.22
Major code rearrangement in API hooking. Expect some troubles...
Added desktop workarea and centered coordinate settings
Fixed imelib hooking
Added HOOKENABLED flag
Fixed ChangeDisplaySettings 
GUI: added ListView icons
GUI: added pause command
GUI: updated commands layout

v2.02.21
it's a w.i.p. release, wait 'till finished....

v2.02.20
Fixed an error in D3D10/11 preliminary code: AoE III working again
Added "Hook all DLLs" option to simplify congiguration: see new Diablo setup
Added screen coordinates to log
Some code rearrangement

v2.02.19
Saves GUI coordinates
Fixed "Remap client rect" option for Diablo's windows and Premier Manager 98 mouse movements
Added "Highlight blit to primary" option to draw a yellow bounding box around blits to primary surface
Fixed some exception conditions when closing the programs
Fixed CreateSurface handling to allow Premier Manager 98 start in emulated mode
Fixed ONEPIXELFIX handling
Fixed BIG WIN handling for Diablo's windows
Fixed FillRect hook to prevent filling outside virtual desktop
Disabled hooking of system libraries
Fixed a nasty bug that caused your desktop to freeze until shutdown !!!
Fixed GetWindowRect handling for windows not created by the task: Diablo queries the explorer window size! Now the retrieved RECT can't be larger than the virtual desktop

v2.02.18
Improved LoadLibrary family hook and other improvements to let DxWnd better retrieve OpenGL libs:  now Homeworld 2 is working
Fixed a bug in backbuffer ZBUFFER attach emulation. Now Dungeon Keeper 2 crashes no more.
Moved "Remap client rect" option from directx tab to main program tab
Added the "Force Hook" in the OpenGL tab. Useless for now.
Preliminary work for d3d10/d3d11 wrapping. Unfinished and not working so far.
Added LoadLibraryEx flags explaination in log
Added support for mouse X,Y coordinates display in status window
OpenGL hooking: fixed bug to prevent hooking same call twice
OpenGL hooking: added hook for wglMakeCurrent call to keep track of rendered window
OpenGL hooking: fixed glViewport and extglScissor hook coordinates handling
D3D hooking: Added Wireframe option for D3D games
Added hooking of CLSID_DxDiagProvider through CoCreateInstance
Fixed (further simplified) clipping handling of primary/backbuffer surfaces 
ChangeDisplaySettings hook: fixed x,y coordinate inversion in log message

v2.02.16
DLL injection finally working! Quake 2 is supported.
Added Wireframe option for OpenGL games
Improved fix for clipper handling of primary/backbuffer surfaces 

v2.02.15
Fixed clipping handling of primary/backbuffer surfaces 
Added option to set AERO compatibility for Vista/Win7/Win8 platforms. Need to call a undocumented ddraw API. Thanks to Vovchik that discovered it. Use at your own risk!

V2.02.14
Started dll injection to handle startup code
Fixed directx CreateSurface hook to fix "Wargames" error in emulated mode
Fixed directx SetClipper hook to properly handle backbuffer clipping and fix "Wargames" clipping problems

v2.02.13
Added decoding of WINDOWPOS.flags field in trace log
revised whole hooking procedures to use HMODULE handle instead of module name
Added WM_GETMINMAXINFO and WM_NCCALCSIZE handling in WinProcess hook
Attempt to handle double buffering through surface attach to backbuffer (???)
Fixed CHILD window positioning for BIG windows in CreateWindowExA hook
Added GlobalMemoryStatus hook to fix huge values when value exceeds DWORD range. Fixes Nocturne intro warning message.

v2.02.12
GUI: Tabbed setup panel. More space for more future options.
DLL: (optional) splash screen 
probably, some regression bugs....
 
v2.02.11:
Added debug messages for GetSystemMetrics() modes, MapWindowPoints() points, DirectDrawEnumerate/Ex() devices.
ompiled with #define _WIN32_WINNT 0x0600 -> handles Vista modes
Added NOPALETTEUPDATE ("Palette update don't Blit" flag) to eliminate flickering when ddraw and GDI methods conflict
Hooked all LoadLibraryA/W
 and LoadLibraryExA/W calls
Hooked 
extDirectDrawEnumerate/Ex ddraw calls to handle Hide multi-monitor option.
Detected directshow activation through CoCreateInstance and hooked quartz.dll segment: now Urban Assault movies don't require "Full RECT Blit" option to be set.
Updated DDSurface::Release hook 

v2.02.10:
Added "Full RECT Blit" mode: may be useful to handle problematic situations (e.g. "Urban Assault" intro movies)
Fixed ClientToScreen and ScreenToClient hookers to properly handle scaled windows. This makes "Postal" working.
Fixed global palette reference count (??) in DirectDraw::Release hook
Fixed Window messages handling for SWP_NOMOVE, SWP_NOSIZE modes.

v2.02.09:
Fixed some x,y window coordinates bugs
Fixed some proxy log messages (missing \n line terminator)
Fixed Trace "DirectX" flag.
improved GetDC handling in 8BPP palette mode: AddPalette called on demand, and on any surface (including backbuffers): makes Emergency work with no "Handle DC" flag set. Beware: this may affect the "Map GDI HDC on Primary DC" flag causing surface locks.
Fixed limit FPS timing issues: now the max possible FPS is 1000/delay.
Fixed EndPaint bug causing HDC lock in "Map GDI HDC to Primary DC" mode.

v2.02.08:
fixed some errors in the main directdraw palette descriptor. That gives better compatibility and less complicated source code.
added Fake Version feature: now Dungeon Keeper II (original version, not GOG hack) can detect a fake Win2000 / WinXP environment on Win7 and newer. Tested and working on Win7. Many thanks to Maxim for pushing me hard to win my lazyness and implement this new feature.

v2.02.07:
many fixes on the FPS and time control features.

v2.02.06:
preliminary FPS counter overlapped on game screen, Alt-F7 to toggle display on/off.
fixed buf on time stretch logging (and possible game crash).
revised GetDC handling with 8BPP paletized surfaces: avoided need to emulate reverse-blitting and got an impressive speed improvement for games such as Age of Empires I & II and Hyperblade.

v2.02.05:
hooked winmm timeGetTime() API: makes time stretching work for Age of Empires series
changed time stretching grain: now it's not the coarse grained 2x, 4x,... series in 9 possible values but the fine grained series 1.5x, 2x, 3x,.... in 17 possible values
added status and time stretching view panels to tray icon menu

v2.02.03/04:
code cleanup - see syslib calls hooking
new configuration flags: Hook GDI and Hook OpenGL
OpenGL custom library field
API hooking fix with module specification
Time stretching by keyboard control (Alt F5/F6) and/or Time Slider dialog

v2.2.02
preliminary time stretching: so far applies to GetTickCount() only, and is controlled by Alt-F5 / F6 keys.
Fixed bug in GDI BitBlt call: stretching must be made on screen DC only, or it's made twice.

v2.2.00/01
major code rewriting - introduced dxwCore class
preliminary FPS handling: Limit, Skip & Count with configurable delay
Hide Multi Monitor configuration flag - used for "Dream Acquarium" on multimonitor PC.
 
v2.1.100
double project definition, for vs2005 and vs2008
fixed PeekMessage logic to avoid message regeneration
added "none/OpenGL" emulation mode to avoid interfering with ddraw/d3d when unnecessary
added preliminary DirectDrawEnumerate hooking - just proxed so far
fixed crash when DirectDrawCreate/Ex was called with DDCREATE_HARDWAREONLY or DDCREATE_EMULATIONONLY flags
some code cleanup
fixed WM_NCHITEST handling to translate mouse coordinates properly: now "7th Legion" finally works.
fixed LoadLibrary logic to ignore full path when searching for specific libraries
fixes CreateWindowEx hook to handle the special case of "big" windows created with CW_USEDEFAULT position or bigger size: the fixes improve "The Grinch" window handling.
experimental changes/fixes to message processing

v2.01.98/99
wrapped SendMessage to revert fix to X,Y mouse coordinates: fixes "Pax Imparia Eminent Domain" right mouse problem
added dxwnd version and flags configuration to log for better diagnostic
updated the manual (at last!)

v2.01.96/97
better support for OpenGL games (still preliminary...): Hexen II, American McGee Alice, ...
fixed "Simulate 8/16BPP desktop" flag: "Moto Racer" is now working on 32BPP desktop.
various improvement and log changes.

v2.01.95
very preliminary support for OpenGL games. See "Star Wars Jedi Knight II: Jedi Outcast"....

v2.01.94
fixed d3d EnumAdapterModes proxy hook that has different prototype for version 8 and 9: apps calling this method crash no more.
fixed GDI GetDC/GetWindowDC hooks: Tipically, you'd better prevent the app to access the desktop window, where it can retrieve info about the actual screen size and color depth, so the call is redirected to the main window. But, in certain circumstances, the window handle may become invalid, so that it's better to let the desktop hanlde pass. The fix makes "Halo: Combat Evolved" working.
fixed CreateWindowEx hook, preventing the creation of zero-sized windows. Makes "Star Wars Jedi Knight II: Jedi Outcast" at least visible...
added "Disable setting gamma ramp" flag: avoid changing screen brightness on the whole desktop surface, as "Star Wars Jedi Knight II: Jedi Outcast" may do.

v2.01.93
Emergency special edition:
fully revised the GDI ClipCursor and GetClipCursor handling, to ensure that the cursor stays confined within the clipping region: now Emergency crashes no more when you attempt to move the cursor close to the window borders.
Introduced a "tolerance" interval to avoid unreacheable clip/window borders and difficulty to scroll the game area.
fixed "Keep aspect ratio" option to take in account the inner client area instead of the outer window size.

v2.01.92
Road Rash special edition:
several fixes in WS_CHLD windows handling: Child window process routine, MoveWindow and GetWindowPos hook routines.
fixed BIG window detection: now every BIG window (that is (0,0)-(ScreenX,ScreenY) in position) is detected and mapped on the main window coordinate system
fixed a log bug in extMoveWindow()
now "Road Rash" is fully playable, apart from the game movies that can't be resized and are correctly placed only with 640x480 window size.
known problem: the game window can be resized, but it returns back to the original size every time you get back to the menus.
loggin routine fix: now, if it can't open the logfile (ROADRASH.EXE stays on CD) tries to write on %TEMP% folder. Don't forget to clean it up!
GUI: added delete of %TEMP%\dxwnd.log file in "Clear all logs" command.

v2.01.91
Hyperblade fixes:
Wrapping of GDI PatBlt API to scale rect coordinates
fixing of wrong Blt destrect coordinates when blitting to primary surface: Hyperblade fails using the proper screen size 640x480 and uses an invalid 800x600 size.
again: revised GetDC/ReleaseDC handling to fix regression problems with other games
again: revised mouse and clipping logic to improve previous fixes. Now clipping mode is restored only after clicking on the window client area.

v2.01.90
fully revised GetDC/ReleaseDC handling: Warlords III seems playable
revised mouse and clipping logic to fix some visibility and control problems in window resizing
project adapted and compiled with VS2008. Eliminated references to undefined ColorCorrectPalette API.

V2.01.89
new window option: "Force win resize" - experimental
revised GetDC reverse blitting algorithm - should now handle surfaces bigger than the primary (ref. Warlords III)
logging: added result of WM_NCHITTEST processing
added Alt-F8 toggle key for "Handle DC" option to speed up operations when accuracy can be sacrified
Direct3D preliminary hooks for display modes query
Directx9: hooked Reset method
fixed bug in ChangeDisplaySettings: now it saves current size also in emulated mode

V2.01.88
GUI: layout reorganization
new window option: "Keep aspect ratio" - to lock aspect ratio when resizing the game window
added "Limit available resources" option (void - to be implemented)

V2.01.87
new window option: "Modal style"
fixed dd::EnumDisplayModes hook with separate handling for dd interface 1 and 4
fixed handling of primary dd object (the one that owns the primary surface and the service objects). Makes Dungeon Keeper II playable again.
new games tested & working: eXpendable, Rally Championship 2000, Thief the Dark Project, In the Hunt, M1 Tank Platoon II, ...

V2.01.84/85/86
minor fixes on the GUI
fixed BackBuffer creation with an initial refcount=1 to simulate its initial reference by the frontbuffer. This makes "Monopoly 3D" working.
added "Suppress clipping" surface option: eliminates interference with primary surface blittin on "Monopoly 3D".
fixed bug in Alt-F10 log toggle command (I know I said that before....)
added logging status in status window (just to be sure the previous bug is FIXED!)
log fixes
experimental: PrimarySurface created with 1 backbuffer when no backbuffercount option is specified
fixed primary surface refcount in SetPalette, when a palette is attached to the primary surface
deleted the EXTRASURFACEFIX code prtion: now Diablo works without this trick, thanks to the proper refcount to avoid deleting a closed surface.
regression: Dungeon Keeper II no longer works.
Added sys libraries hooking to LoadLibrary and LoadLibraryEx hookers
fixed window coordinates in MoveWindow hooker
major: revised the sys libraries hooking mechanism. So far, nothing good happens, but ....

V2.01.83
Added "Blit from backbuffer" option - improves "The Sims"...
Eliminated useless "Automatic Emulation"
GUI: emulation modes (none, emulate buffer, emulate surface) are now mutually exclusive
minor fixes

V2.01.82
Fixed DDSURFACE::GetCaps adding DDSCAPS_FLIP capability in emulated mode. Makes "Funtracks" visible.
Eliminated useless "Use absolute coord" flag
fixed bug in Alt-F10 log toggle command

V2.01.81
Eliminated useless "Reset PRIMARY surface" opt.
added 0xC0000094 exception code ("Ultim@te race pro" - untested)
added handling of fake backbuffer attached surfaces - should fix "Silver"
fixed missing hook to LoadLibrary & LoadLibraryEx - caused several oddities. Should allow more games be handled by default hook mode.
fixed some positioning logic in Fix Parent Window mode
fixed the "Video -> System surface on fail" behaviour: switch is tried not only on DDERR_OUTOFVIDEOMEMORY error condition, but also on DDERR_INVALIDPIXELFORMAT. This makes "The Sims" playable also in non-emulated mode.

V2.01.79/80
fixed/improved some more log messages (fixed BltFast flags dump, added ROP dump, added ColorKey dump for blit operations).
fixed reverse blitting algorythm from 16/32BPP to palitezed 8BPP chosing a best matching index dinamically: that fixed several color problems in "Beasts & Bumpkins" and "Dink Smallwood".
code cleanup in GetDC hook function.
updated EnumDisplayModes hook function to avoid showing hi-res screen modes when PREVENTMAXIMIZE is set (useful?).
revised dxwFixWindowPos and AdjustWindowPos: code merged into CalculateWindowPos.
fixed bug in window positioning while toggling with ALT-F9.

V2.01.77,78
added hook for GetSystemPaletteEntries - useless for now.....
fixed/improved several logs
fixed WS_EX_CONTROLPARENT exstyle windows handling: now "Diablo" works fine, and also Worms series shows some improvement.
added EnumDisplayModes hook with setting of emulated color depth. This makes "Grim Fandango" working with no need to set RGB565 encoding.
eliminated NULLRECTBLIT, TRANSPARENTALL and TRANSPARENTWIN handling, no longer useful
fixed bug in SetClipper handling for NULL lpddc: fixes "Gangsters" ingame screen.
added FIWPARENTWIN flag: fixes "Sid Meier's Golf" position and improves "Sleepwalker" behaviour
GUI:
game properties tab layout revised
eliminated useless options
View Status dialog
Recover desktop settings when changed on task kill or DxWnd exit

V2.01.76
added hook for MoveWindow API: fixed Lionheart positioning problem.

V2.01.75
added "Handle Exceptions" flag. Applied to IDIV and CLI opcodes (exceptions 0xc0000095 divide by zero and 0xc0000096 priviliged instruction): now legacy "SonicR" and "Resident Evil" are working. 
fixed GetClientRect logic to handle desktop special case: this fixes "AfterLife".
revised VirtualScr Height & Width settings: now, they can't possibly be 0 ever, they're defaulted to 800x600.
fixed BlitError logging routine
added hooking to GDI32 palette API: CreatePalette, SelectPalette, RealizePalette. Fixed display for "Dementia".
fixed import table dump output: forwarderchain field is numeric
fixed a regression bug about the window style - now default style is successfully set to WS_OVERLAPPEDWINDOW (the windows default).

V2.01.74
added bidirectional info exchange between GUI & DLL: now GUI can show internal emulation status.
GUI:
Added DxWnd status to trayicon tooltip
Fixed "Kill" menu command logic to act on the specific & correct task
Fixed bug that prevented asking for save configuration when task were added / deleted / sorted.
Fixed bug that disaligned task name field when deleting a task in the list.
CORE:
Updated GetHookStatus() arguments & behaviour
Fixed double-typing bug when "Process Messages" option is set.

V2.01.73
CORE:
Completely revised the logging code.
Added Import table entry logging
Added separate flags for mouse/cursor events
Added timestamp log banner
Fixed crash caused by attempt to write log on error condition (e.g. trying to write on a game CD)
Fixed GetDesktopWindow() and SetCooperativeLevel() to handle operations on desktop window: GP500 and Microsoft Midtown Madness are now working better.
hanling COLOR operations on blitting/flip: nor Pacific General half-vertical-lines movies don't show the previous screen any more.
By default, don't alter the window properties. Some games will lack the frame border: the "Fix Window Frame Style" should be checked.

V2.01.72
CORE:
Fixed a regression bug on child windows handling
Added hook for DeferWindowPos() - to test
cleaned up a DEBUG message

V2.01.71
CORE:
Added hooking for MapWindowPoints: makes "Commandos 2" working.
fixed windows message handling procedure: avoid calling fake address in the form of 0xFFFFnnnn returned by GetWindowLong (necessary for "Commandos 2")
Improved DirectDrawCreate/DirectDrawCreateEx hooking: now auto mode should work in more cases
Fixed isFullScreen toggle: now it handles both entering and exiting from fullscreen state
Fixed PITCHBUGFIX handling
introduced experimental ONEPIXELFIX flag
Fixed GetSurfaceDesc returned values for primary surface: needed for "Commandos 2".
Fixed PeekMessage/GetMessage whnd handling.

V2.01.70
CORE:
Fixed "Force cursor clipping" in non emulated mode.
Added retry in timed out surface lock operation. Fixes sporadic "Ancient Evil" crash in emulated mode.
Entirely rewritten the "Pitch Bug Fix" mode: now it uses more a service surface in memory to write to, and then the surface is blitted to the emulated primary surface, using more performant dx blit routines and allowing for surface scaling.
GUI:
The "Pitch Bug Fix" has been renamed to "Emulate Primary Buffer".

V2.01.69
CORE:
Fixed bug: "GP500" is now working
Inserted "Keep cursor fixed" flag: "Necrodrome" is now working
Fixed window message handling for "Fix window style", "Prevent Maximize" and "Lock win properties" flags
Optimized IPC shared space, now the supported entries are 256 again.
Extra special key: Alt-F9 toggle window position locking
Extra special key: Alt-F10 toggle logging
Added "Blit using NULL rect spec." to handle some odd cases: makes "Railroad Tycoon II" working.
Fixed "Remap GDI Client rect" option: now sets the LPPAINTSTRUCT returned by BeginPaint. This makes "Railroad Tycoon II" working with no need of the emergency "Blit using NULL rect spec." flag.
Fixed dx_ScreenRefresh() internal routine: more accurate screen refreshing with "Refresh on win resize" mode.
Hooked InvalidateRect() to invalidate whole window: improves "Railroad Tycoon II" accuracy.
Some improvement in Diablo menu screens (character selection menu)
Some more logging and error detection.
Hook API code revision: HookSysLibs takes care of both HookSysLibsByAddr and HookSysLibsByName. No more similar routines to keep aligned!
GUI: 
Added command line arguments: /I (start idle), /T (start into tray)
added save changes confirmation message.

V2.01.68.FIX
Fixed a new bug that prevented execution of ANY multi-threaded game!!!!

V2.01.68
Fixed mouse message processing to alter ALL mouse events: This makes "Uprising 2" handling the mouse correctly.
Aligned the hooking logic in proxy mode to the new logic in normal mode. 
Inserted the Diablo FIX: this is odd! Apparently, and perhaps for an internal game bug, the game works only if you create an extra surface after the primary one, maybe to insulate some memory corruption. As a matter of fact, doing so, you have an useless and unused extra surface, but old Diablo works pretty well.
Some unused code and comments cleanup.
GUI major changes:
- Added system tray handling
- Added control about maximum number of entries reached
- Added controls about allowed GUI commands
- Eliminated several useless commands
- cleanup of unused resources & code 
- Added control to avoid multiple task activation

V2.01.66/67
Added wrapping for BeginPaint() in "Map GDI HDC to Primary DC" mode, returning the virtual primary surface DC: makes "Deadlock 2" playable and stretchable.
Deleted the "Mouse to Screen Fix" option (useless).
Fixed bug in palette handling in desktop 16BPP color depth: Fixes colors in Age of Empires, Enemy Infestation.

V2.01.65:
Fixed handling of inherited backbuffers from previous and closed primary surface in no-emulated mode. Fixes "Warhammer 40k Rites of War" display problems.
Hook for DIRECTDRAW object creation through CoCreateInstance: new supported game "Axis & Allies"

V2.01.64:
Hooked & neutralized IDirect3DDevice8->Reset() method (preliminary implementation)
New supported games: Virtua Tennis, Crazy Taxi

V2.01.63:
Implemented INIT16BPP flag
hdinput.cpp: Fixed DirectInput GetMousePosition() call
u32lib.cpp: Fixed FixWindowFrame() to avoid messing with windows when not in fullscreen mode
More code for handling of dynamically created flipping chains (unfinished)
Hooked address for dynamically loaded ChangeDisplaySettingsA
New supported games: Monster Truck Madness 1 & 2, Mortal Kombat 4

V2.01.62:
Fully revised COM hooking method
Warlord series now fully working
Implemented 24BPP color depth (Delta Force)
Started handling of backbuffer array for flipping chain emulation (unfinished)
Added handling of DDERR_NOCLIPPINGLIST error
Added "Simulate 8BPP desktop" mode
Added preliminary implementation of "Automatic Emulation mode"
Syberia & Syberia 2 now working
GUI: fixed game list sort command
More games, more stable, less parameters....

V2.01.56:
Implemented DirectDraw full proxy for method logging. 
Big code reorganization: new files hddproxy.cpp hddproxy.h
fixed hooking to IDirectDrawSurface::GetCaps, IDirectDrawSurface::GetAttachedSurface, IDirectDrawSurface::DeleteAttachedSurface
fixed hooking of extDirectDrawCreateEx
improved logging (DumpSurfaceAttributes)
BackBuffer created only when necessary

V2.01.54:
Fixed PeekMessage/GetMessage hook: "uprising" and "uprising 2" mouse handling now working
Implemented "Refresh on win resize" flag: useful for "uprising"
Fixed bug on new flags handling (not defaulted to OFF).

V2.01.53:
Major code reworking for emulation mode blitting rendering functions
Fixed GetPalette hook: "uprising" and "uprising 2" now supported

V2.01.52:
Added handling of screen color setting to 16BPP color depth (partial).
Handling of games that check and require 16BPP color depth: "Resident Evil" now supported.
Fixed problem about "Turok 2", now supported

V2.01.50:
Fixed bug on attached surface handling in non-emulated mode ("Alien vs. Predator" now supported)
Added Restore Screen Mode flag ("Army Men" OK)

V2.01.48:
Implemented proper handling of blitting from on-screen primary surfaces. 
Now DxWnd supports un-emulated resized "The Sims" and "Warhammer 40K Rites of War".
Improved handling of attached backbuffers, even dynamically created ones.
Improved handling of GDI calls using HDC. "688I Hunter Killer" partially working. "Black Moon Chronicles" working.
