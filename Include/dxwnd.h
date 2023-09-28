#include <ddraw.h>
//#include "dxwcore.hpp"

#define DXW_IDLE		0
#define DXW_ACTIVE		1
#define DXW_RUNNING		2

#define MAXTARGETS			256

#define ONEPIXELFIX 1
#define HOOKDDRAWNONE 12

// first flags DWORD dwFlags1:
#define UNNOTIFY			0x00000001 // do not notify on task switch - suppresses WM_ACTIVATE and WM_NCACTIVATE messages
#define SETDPIAWARE			0x00000002 // set DPI awareness to avoid automatic scaling
#define CLIPCURSOR		 	0x00000004 // Force cursor clipping within window
#define NEEDADMINCAPS		0x00000008 // DxWnd needs administrator's privileges to hook this program
#define HOOKDI				0x00000010 // Hook DirectInput
#define MODIFYMOUSE			0x00000020 // Scale x,y coordinates from GetCursorPos call
#define HANDLEEXCEPTIONS	0x00000040 // Handles exceptions: Div by 0 ....
#define SAVELOAD			0x00000080
#define HOOKXINPUT			0x00000100 // hook xinput_1_n.dll libraries
#define HOOKDI8				0x00000200 // Hooks DirectInput8
#define BLITFROMBACKBUFFER	0x00000400
#define SUPPRESSCLIPPING	0x00000800 // suppress ddraw clipper to be set upon a window / surface
#define AUTOREFRESH			0x00001000 // Automatically preforms a periodic blit from backbuffer to screen
#define FIXWINFRAME			0x00002000 // Fix window frame style
#define HIDEHWCURSOR		0x00004000 // suppresses the system cursor
#define SLOWDOWN			0x00008000
#define DISABLECLIPPING		0x00010000
#define LOCKWINSTYLE		0x00020000
#define MAPGDITOPRIMARY	 	0x00040000
#define FIXTEXTOUT			0x00080000
#define KEEPCURSORWITHIN	0x00100000
#define USERGB565			0x00200000
#define SUPPRESSDXERRORS	0x00400000 // suppresses some common dx errors (BUSY)
#define PREVENTMAXIMIZE		0x00800000 // struggle to avoid window maximize
#define LOCK24BITDEPTH		0x01000000 // inhibits ddraw 24 bit color depth replacing it with current color
#define FIXPARENTWIN		0x02000000 // fixes parent window size & position
#define SWITCHVIDEOMEMORY 	0x04000000 // when VIDEO memory is over, switches to SYSTEM memory
#define CLIENTREMAPPING		0x08000000 // hooks ClientToRect, RectToClient, GetClientRect, GetWinRect
#define HANDLEALTF4			0x10000000 // forces quitting the program when receiving Alt-F4 key
#define LOCKWINPOS			0x20000000 // prevent the program to change its own windows properties
#define HOOKCHILDWIN		0x40000000 // hook CHILD windows to alter placement coordinates
#define MESSAGEPROC			0x80000000 // process peek/get messages

// second flags DWORD dxw.dwFlags2:
#define RECOVERSCREENMODE	0x00000001 // actively set screen mode to initial state after hooking
#define REFRESHONRESIZE		0x00000002 // forces a refresh (blitting from backbuffer to primary) upon win resize
#define BACKBUFATTACH		0x00000004 // sets backbuf wxh dim. equal to primary surface so that ZBUFFER is attachable.....
#define MODALSTYLE			0x00000008 // window with no borders
#define KEEPASPECTRATIO		0x00000010 // when resizing, keep original aspect ratio
#define INIT8BPP			0x00000020 // simulate a 8BPP initial desktop setting (in GetDeviceCaps API)
#define FORCEWINRESIZE		0x00000040 // adds NCMOUSEUP/DOWN processing to the win procedure if missing
#define INIT16BPP			0x00000080 // simulate a 16BPP initial desktop setting (in GetDeviceCaps API)
#define KEEPCURSORFIXED		0x00000100 // inhibit SetCursorPos operation
#define DISABLEGAMMARAMP	0x00000200 // let the application retrieve the desktop DC (for capability queries)
#define INDEPENDENTREFRESH	0x00000400 // Makes automatic primary surface refresh but independently from the message loop processing
#define FIXNCHITTEST		0x00000800 // fixes WM_NCHITTEST message X,Y coordinates 
#define LIMITFPS			0x00001000 // delays primary blit operations to limit FPS 
#define SKIPFPS				0x00002000 // skips primary blit operations up to limit
#define SHOWFPS				0x00004000 // shows FPS value to status win / log / title bar
#define HIDEMULTIMONITOR	0x00008000 // hide multimonitor configurations: GetAdapterCount returns 1.
#define TIMESTRETCH			0x00010000 // make system time stretchable 
#define HOOKOPENGL			0x00020000 // Hook OpenGL calls
#define LOCKEDSIZE			0x00040000 // window resizing is not allowed
#define SHOWHWCURSOR		0x00080000 // enable hardware cursor
#define GDISTRETCHED		0x00100000 // Stretch GDI/user32 coordinates to adapt to window size
#define SHOWFPSOVERLAY		0x00200000 // shows FPS value to screen overlay
#define FAKEVERSION			0x00400000 // pretends the platvorm is a given window version / subversion
#define PALDIBEMULATION		0x00800000 // emulate 8bit DIB_PAL_COLORS DIBs by converting them to native color depth DIBs
#define NOPALETTEUPDATE		0x01000000 // Do not refresh primary surface on palette updates
#define SUPPRESSIME			0x02000000 // suppress IME 
#define NOBANNER			0x04000000 // suppress fancy logo & banneer effects 
#define WINDOWIZE			0x08000000 // Run in a Window (default TRUE) 
#define LIMITRESOURCES		0x10000000 // Limit resources to fit an old program's expectations 
#define STARTDEBUG			0x20000000 // Start in DEBUG mode
#define SETCOMPATIBILITY	0x40000000 // invoke ddraw SetAppCompatData to set aero compatibility mode
#define WIREFRAME			0x80000000 // forces wireframe display for D3D and Glide games

// third flags DWORD dxw.dwFlags3:
#define FORCEHOOKOPENGL		0x00000001 // loads OpenGL32.dll and hooks it
#define REFRESHONREALIZE	0x00000002 // GDI32 palette Realize() ought to cause a window refresh
#define HOOKDLLS			0x00000004 // Hook all DLLs referenced in PE
#define SUPPRESSD3DEXT		0x00000008 // Disables extended d3d APIs for Vista/Win7/Win8 platforms
#define HOOKENABLED			0x00000010 // Enables task hooking
#define FIXD3DFRAME			0x00000020 // Preserve windows frame in D3D9 programs
#define FORCE16BPP			0x00000040 // Forces 16BPP desktop color depth
#define BLACKWHITE			0x00000080 // Simulate a B&W screen monitor mapping colors to grayscales
#define SKIPIATHINT			0x00000100 // skip IAT hint field in IAT sequential scanning
#define SINGLEPROCAFFINITY	0x00000200 // Set Process Affinity to a single core
#define EMULATEREGISTRY		0x00000400 // Emulate registry api to read extra keys
#define CDROMDRIVETYPE		0x00000800 // Pretends that GetDriveType() always returns DRIVE_CDROM
#define NOWINDOWMOVE		0x00001000 // Do not try to update window position & size on D3D rendering
#define FORCECLIPPER		0x00002000 // Set ddraw clipping for real primary surface and primary window - but FastBlt cannot be used!
#define LOCKSYSCOLORS		0x00004000 // Lock Sys Colors changes by SetSysColors() call
#define GDIEMULATEDC		0x00008000 // Map GDI/user32 calls to primary to a memory surface to be stretch-blitted to the primary
#define FULLSCREENONLY		0x00010000 // assume that the program is always in fullscreen mode
#define FONTBYPASS			0x00020000 // bypass font unsupported API
#define MINIMALCAPS			0x00040000 // Simulate minimal HW caps (copied from VMWare virtual screen)
#define DEFAULTMESSAGES		0x00080000 // peocess  offending messages that are typical of a window with default action
#define BUFFEREDIOFIX		0x00100000 // fix buffered IO incompatibilities between pre-Win98 and post-WinNT
#define FILTERMESSAGES		0x00200000 // ignore offending messages that are typical of a window and are hot handled by a fullscreeen app
#define PEEKALLMESSAGES		0x00400000 // force Peek-ing all sort of messages to avoid Win7 message queue saturation that leads to program halt 
#define SETZBUFFER16BIT		0x00800000 // forces ZBUFFER 16 bit capability
#define SETZBUFFER24BIT		0x01000000 // forces ZBUFFER 24 bit capability
#define FORCESHEL			0x02000000 // Forces HEL  (DDCREATE_EMULATIONONLY)
#define SKIPFREELIBRARY		0x04000000 // Avoids recursion in FreeLibrary calls of pre-patched (GOG) programs
#define COLORFIX			0x08000000 // Win7 color fix for 8bpp paletized modes
#define FULLPAINTRECT		0x10000000 // Performa all GDI BeginPaint operations agains the full screen rect
#define FIXALTEREDPATH		0x20000000 // Fix compatibility issue for LoadLibraryEx with LOAD_WITH_ALTERED_SEARCH_PATH flag
#define LOCKFPSCORNER		0x40000000 // Suppress GDI Blt to video device
#define NOPIXELFORMAT		0x80000000 // Don't fix the pixel format for plain surfaces

// fourth flags DWORD dxw.dwFlags4:
#define NOALPHACHANNEL		0x00000001 // By default, create surfaces with no DDPF_ALPHAPIXELS capability
#define SUPPRESSCHILD		0x00000002 // suppress child process generation (CreateProcessA, ...?)
#define FIXREFCOUNTER		0x00000004 // fixes the reference counter for the main directdraw session (the one connected to primary surfaces)
#define SHOWTIMESTRETCH		0x00000008 // Shows time stretching multiplier on screen overlay
#define ZBUFFERCLEAN		0x00000010 // Clean ZBUFFER upon (fix for "Star Wars  Ep. I - Racer")
#define ZBUFFER0CLEAN		0x00000020 // Clean ZBUFFER upon (fix for "Star Wars  Ep. I - Racer")
#define FORCECLIPCHILDREN	0x00000040 // Forces WS_CLIPCHILDREN style on all potential main windows
#define DISABLEFOGGING		0x00000080 // Disable D3D fogging
#define NOPOWER2FIX			0x00000100 // Handle textures whose size is not a power of 2 (32, 64, 128 ...)
#define NOPERFCOUNTER		0x00000200 // Disables the GetPerfCounter performance metrics API,as if it was not supported....
#define PREVENTMINIMIZE		0x00000400 // Prevents main window to minimize itself
#define INTERCEPTRDTSC		0x00000800 // Intercapts RDTSC opcodes to hook at assembly level
#define LIMITSCREENRES		0x00001000 // Limit available screen resolution up to defined maximum
#define NOFILLRECT			0x00002000 // Suppress FillRect calls
#define HOOKGLIDE			0x00004000 // Hook glide calls
#define HIDEDESKTOP			0x00008000 // Hide desktop background
#define STRETCHTIMERS		0x00010000 // Enables timers stretching when TIMESTRETCH is on
#define NOFLIPEMULATION		0x00020000 // Compensate the effect of Flip-ping through Blt operations
#define NOTEXTURES			0x00040000 // Disabled (whitened) textures
#define RETURNNULLREF		0x00080000 // IDirectDraw::Release and IDDrawSurface::Release will always return 0 ref. counter
#define FINETIMING			0x00100000 // Fine timeslider coarse, by 10% steps ranging from about :2 to x2 
#define NATIVERES			0x00200000 // Supports hardware native resolutions (depending on your PC)
#define SUPPORTSVGA			0x00400000 // Supports SVGA standard resolutions
#define SUPPORTHDTV			0x00800000 // Supports HDTV standard resolutions
#define RELEASEMOUSE		0x01000000 // GetCursorPos returns centered coordinates when the mouse cursor is moved outside the window
#define ENABLETIMEFREEZE	0x02000000 // Enable Time Freeze functionality and toggle key
#define HOTPATCH			0x04000000 // Use hot patching technique to handle obfuscated IAT
#define ENABLEHOTKEYS		0x08000000 // Enables hot keys
#define IGNOREDEBOUTPUT		0x10000000 // Emulates the "IgnoreDebugOutput" shim
#define NOD3DRESET			0x20000000 // Disables D3D8/9 D3DDevice::Reset method
#define OVERRIDEREGISTRY	0x40000000 // same as EMULATEREGISTRY, but fake keys takeprecedence
#define HIDECDROMEMPTY		0x80000000 // hide empty CDROM drivers

// fifth flags DWORD dxw.dwFlags5:
#define DIABLOTWEAK			0x00000001 // ... ??? ...
#define CLEARTARGET			0x00000002 // forces a D3D clean on target surface upon each BeginScene invocation
#define NOWINPOSCHANGES		0x00000004 // suppress WM_WINDOWPOSCHANGING/CHANGED messages (RollerCoaster Tycoon...)
#define MAXCLIPPER			0x00000008 // forces the ddraw clip region to be the whole virtual desktop
#define LIMITBEGINSCENE		0x00000010 // suppress blit to primary surface
#define USELASTCORE			0x00000020 // Same as single core process affinity, but using the last availabe core instead of first one
#define SWALLOWMOUSEMOVE	0x00000040 // Swallow (delete) too frequent WM_MOUSEMOVE messages from input message queue
#define AEROBOOST			0x00000080 // Optimize for AERO environment: does all stretching in sysmemory surfaces
#define QUARTERBLT			0x00000100 // Consider a screen update (to count or limit FPS) only  blt operations bigger than a quarter of the whole primary surface
#define NOIMAGEHLP			0x00000200 // Interceptd Imagehlp.dll unsupported calls (used by "the 5th element")
#define BILINEARFILTER		0x00000400 // experimental bilinear filtering
#define REPLACEPRIVOPS		0x00000800 // replace privileged opcodes, such as IN (Ubik)
#define REMAPMCI			0x00001000 // remap MCI calls coordinates in mciSendString
#define TEXTUREHIGHLIGHT	0x00002000 // highlight textures with random color & grid
#define TEXTUREDUMP			0x00004000 // dump textures to file system as bmp files
#define TEXTUREHACK			0x00008000 // load (replace) hacked textures from file system (bmp files)
#define TEXTURETRANSP		0x00010000 // transparent textures (unimplemented)
#define NORMALIZEPERFCOUNT	0x00020000 // Normalize Performance Counter to a Performance Frequency of 1MHz
#define DISABLEMMX			0x00040000 // Makes CPUID return a CPU state without MMX support
#define NOACCESSIBILITY		0x00080000 // Disables Accessibility Shortcut Keys
#define INJECTSON			0x00100000 // forward inject hooking capability to son processes
#define DEBUGSON			0x00200000 // forward debug hooking capability to son processes
#define DISABLEALTTAB		0x00400000 // disables Alt+Tab and Esc+Tab sequences 
#define HOOKBINKW32			0x00800000 // Hook BinkW32.dll
#define GLOBALFOCUSON		0x01000000 // Set DirectSound GLOBALFOCUS flag ON
#define GLOBALFOCUSOFF		0x02000000 // Set DirectSound GLOBALFOCUS flag OFF
#define MESSAGEPUMP			0x04000000 // inserts a "message pump" loop between repeated operation that may stop the task on Win7 and greater
#define TEXTUREFORMAT		0x08000000 // Apply virtual pixel format to texture surfaces without DDSD_PIXELFORMAT attribute
#define PUSHACTIVEMOVIE		0x10000000 // Push ActiveMovie windows back to the window zorder stack to make them invisile
#define LOCKRESERVEDPALETTE 0x20000000 // lock the reserved palette entries (usually 20: 0-9 and 246-255)
#define UNLOCKZORDER		0x40000000 // Inhibit attempts to keep the main win on top of ZORDER by stripping BringWindowToTop and SetForegroundWindow calls
#define EASPORTSHACK		0X80000000 // Hack to intercept and neutralize some of the hooks set internally by EA Sports games

// sixth flags DWORD dxw.dwFlags6:
#define FORCESWAPEFFECT		0x00000001 // in D3D8/9, forces the SwapEffect value in CreateDevice/Reset operations
#define LEGACYALLOC			0x00000002 // reserve legacy memory segments
#define NODESTROYWINDOW		0x00000004 // suppress the main window destruction
#define NOMOVIES			0x00000008 // suppress MCI Movies
#define SUPPRESSRELEASE		0x00000010 // Avoid releasing critical emulated surfaces - the backbuffer
#define FIXMOVIESCOLOR		0x00000020 // Fixes vfw movies color depth forcing the virtual value 
#define WOW64REGISTRY		0x00000040 // Sets KEY_WOW64_64KEY registry flag
#define DISABLEMAXWINMODE	0x00000080 // invoke ddraw SetAppCompatData(12,0) to set DisableMaxWindowedMode
#define FIXPITCH			0x00000100 // Fixes surface filling made with no concern to actual pitch value
#define POWER2WIDTH			0x00000200 // Fixes .....
#define HIDETASKBAR			0x00000400 // Hides taskbar & start menu during game execution
#define ACTIVATEAPP			0x00000800 // sends WM_ACTIVATEAPP message after every WM_WINPOSCHANGED message
#define NOSYSMEMPRIMARY		0x00001000 // forces suppression of DDSCAPS_SYSTEMMEMORY capability on emulated primary surface
#define NOSYSMEMBACKBUF		0x00002000 // forces suppression of DDSCAPS_SYSTEMMEMORY capability on emulated backbuffer surface
#define CONFIRMONCLOSE		0x00004000 // on close button, ask the user before closing the window / terminating the program
#define TERMINATEONCLOSE	0x00008000 // on WM_CLOSE message, also terminates the program
#define FLIPEMULATION		0x00010000 // create virtual primary and backbuffer as un-attached surfaces and replace Flip with Blt
#define SETZBUFFERBITDEPTHS	0x00020000 // when not set, set the legacy dwZBufferBitDepths field in the directdraw capability from GetCaps()
#define SHAREDDC			0x00040000 // enable sharing window DC and primary surface DC
#define WOW32REGISTRY		0x00080000 // Sets KEY_WOW32_64KEY registry flag
#define STRETCHMOVIES		0x00100000 // Stretches ActiveMovie Windows to the whole window size
#define BYPASSMCI			0x00200000 // bypass all MCI calls returning OK.
#define FIXPIXELZOOM		0x00400000
#define SCALERELMOUSE		0x00800000 // scales mouse relative movements in dinput/dinput8 mouse relative access
#define CREATEDESKTOP		0x01000000 // creates a virtual desktop window to replace the real one
#define FRONTEND			0x02000000 // hook the CreateProcess* calls only to run child with injection/debug mode
#define SYNCPALETTE			0x04000000 // Syncronize GDI to DDRAW palette
#define VIRTUALJOYSTICK		0x08000000 // Enables the DxWnd virtual joystick
#define UNACQUIRE			0x10000000 // Unacquire DirectInput devices when losing focus and acquire back when gaining focus
#define HOOKGOGLIBS			0x20000000 // Hook additional libraries provided by GOG with different names but same functionalities as system ones
#define BYPASSGOGLIBS		0x40000000 // Bypass GOG proxy system libraries loading directly the system libraries from the system folder
#define EMULATERELMOUSE		0x80000000 // Emulates the dinput detection of relative mouse position by keeping the mouse at the center of window and looking for movements

// seventh flags DWORD dxw.dwFlags7:
#define LIMITDDRAW			0x00000001 // Limit the maximum available ddraw object version
#define NODISABLEALTTAB		0x00000002 // Disables the compatibility patch that disables the Alt-Tab key and other special combinations
#define FIXCLIPPERAREA		0x00000004 // fix clipper area in ddraw & GDI shared DC mode
#define HOOKDIRECTSOUND		0x00000008 // Hook DirectSound dll
#define HOOKSMACKW32		0x00000010 // hook Smackw32 functions
#define BLOCKPRIORITYCLASS  0x00000020 // blocks attempts to change the process priority class
#define CPUSLOWDOWN			0x00000040 // reduces CPU time dedicated to non time critical threads
#define CPUMAXUSAGE			0x00000080 // reduces CPU time to non time critical threads that consume more than the given ratio
#define NOWINERRORS			0x00000100 // suppress some possible window errors returning OK also in case of failure
#define SUPPRESSOVERLAY		0x00000200 // Supress ddraw overlay capabilities (moto racer 2)
#define INIT24BPP			0x00000400 // simulate a 24BPP initial desktop setting (in GetDeviceCaps API)
#define INIT32BPP			0x00000800 // simulate a 24BPP initial desktop setting (in GetDeviceCaps API)
#define FIXGLOBALUNLOCK		0x00001000 // Fix GlobalUnlock kernel32 API when it keeps returning the 1 retcode. 
#define SHOWHINTS			0x00002000 // show configuration hints at runtime
#define SKIPDEVTYPEHID		0x00004000 // do not enumerate HID device types, some programs are not properly coded!
#define INJECTSUSPENDED		0x00008000 // Inject the suspended process while running infinite loop ad starting address
#define SUPPRESSDIERRORS	0x00010000 // Suppress DirectInput common errors
#define HOOKNORUN			0x00020000 // Hook process but don't start it from DxWnd interface
#define FIXBINDTEXTURE		0x00040000 // Fix the problems caused by the lack of glActiveTexture on some Windows OpenGL32.dll versions
#define ENUM16BITMODES		0x00080000 // Enumerates 16 bit video modes when using DirectDraw8/9::EnumAdapterModes on a Win10 OS
#define SHAREDKEYBOARD		0x00100000 // Forces DirectInput cooperative level to NONEXCLUSIVE mode for the keyboard
#define HOOKDOUPDATE		0x00200000 // Updates the original addresses when changed
#define HOOKGLUT32			0x00400000 // Hook glut32.dll opengl extension library
#define INITIALRES			0x00800000 // Set initial screen resolution accorting to sizw x sizh fields
#define MAXIMUMRES			0x01000000 // Set maximum screen resolution accorting to sizw x sizh fields
#define LOCKCOLORDEPTH		0x02000000 // lock ddraw color depth to current desktop value (used in SetDisplayMode)
#define FIXSMACKLOOP		0x04000000 // fix Smack loop by insert processmessage and blit between frames
#define FIXFREELIBRARY		0x08000000 // fix FreeLibrary simulating retcode 0(error) when attempting free of unallocated module
#define ANCHORED			0x10000000 // Anchored position & size, mo stretching or sliding is allowed
#define CLEARTEXTUREFOURCC	0x20000000 // Clear DDSCAPS2_FOURCC bit and makes texture load without codec
#define NODDEXCLUSIVEMODE	0x40000000 // Do not allow ddraw EXCLUSIVE mode also in non windowed mode
#define COPYNOSHIMS			0x80000000 // Create a hiden and temporary copy of the target exe so that shims are not applied

// eighth flags DWORD dxw.dwFlags8:
#define FORCEWAIT			0x00000001 // Forces DDBLT/DDFLIP WAIT flags
#define FORCENOWAIT			0x00000002 // Forces DDBLT/DDFLIP NOWAIT flags
#define FORCEVSYNC			0x00000004 // Forces hardware VSync 
#define FORCENOVSYNC		0x00000008 // Forces NO hardware VSync 
#define VSYNCSCANLINE		0x00000010 // Activates WaitForVerticalBlank algorythm based on reaching a given scan line
#define TRIMTEXTUREFORMATS	0x00000020 // Emulates "DirectXTrimTextureFormats" shim to fix textures in Heavy Gear II
#define NOHALDEVICE			0x00000040 // Simulates a situation of lack of 3D hardware support, like in presence of remote desktop
#define CLIPLOCKED			0x00000080 // never destroy cursor clipper on window move events ....
#define PRETENDVISIBLE		0x00000100 // Pretend the main window is visible (IsWindowVisible) and on Z-order top (GetTopWindow)
#define RAWFORMAT			0x00000200 // texture dump / hack are performed in raw format, compression / decompression to be made offline
#define WININSULATION		0x00000400 // EnumerateWin finds no windows!
#define FIXMOUSEHOOK		0x00000800 // fixes mouse coordinates retrieved through MouseProc routine associated to SetWindowsHook(WH_MOUSE,..)
#define DDSFORMAT			0x00001000 // texture dump / hack are performed in MS DDS format
#define HOOKWING32			0x00002000 // Hook WinG32.dll
#define FIXAUDIOPCM			0x00004000 // Tries to fix audio PCM incompatibilities, i.e. setting mono channel instead of stereo
#define D3D8BACK16			0x00008000 // D3D8 emulate 16 bpp backbuffer on a 32 bpp desktop (Win8-10 needed feature)
#define VIRTUALCDAUDIO		0x00010000 // Virtual CD audio played from .ogg files Audio/Tracknn.ogg
#define DYNAMICZCLEAN		0x00020000 // Performs ZBUFFER cleaning also every time the ZBUFFER is activated
#define FORCETRACKREPEAT	0x00040000 // Forces track repetition once completed
#define IGNOREMCIDEVID		0x00080000 // Ignores the value of device id in mci calls for cd sound emulation
#define LOADGAMMARAMP		0x00100000 // auto-load the saved gamma ramp curve saved in dxwnd.ini
#define QUALITYFONTS		0x00200000 // Forces adoption of high quality fonts (CLEARTYPE_NATURAL_QUALITY)
#define ALLOWSYSMEMON3DDEV	0x00400000 // Suppress DDSCAPS_SYSTEMMEMORY option when DDSCAPS_3DDEVICE is set
#define CLIPMENU			0x00800000 // Include window menu in mouse clipper area
#define BACKGROUNDPRIORITY	0x01000000 // Lower process priority when in background state
#define OFFSCREENZBUFFER	0x02000000 // Allow building plain surfaces and ZBuffer on offscreen videomemory surfaces
#define VIRTUALHEAP			0x04000000 // Remap heap calls to C library memory allocation routines
#define ZBUFFERHARDCLEAN	0x08000000 // Clean ZBUFFER the hard way (memset 0xFF on whole surface buffer ....)
#define LOADLIBRARYERR		0x10000000 // When LoadLibrary* fail for missing file returns ... instead of ....
#define SHAREDDCHYBRID		0x20000000 // Hybrid mode for shared dc GDI emulation - blits both on ddraw surface and window surface
#define FIXADJUSTWINRECT	0x40000000 // Hybrid mode for shared dc GDI emulation - blits both on ddraw surface and window surface
#define HOOKDLGWIN			0x80000000 // Hook dialog WindowProc

// 9th flags DWORD dxw.dwFlags9:
#define FIXTHINFRAME		0x00000001 // Fix window style like OVERLAPPEDWINDOW but with thin border
#define NOMOUSEEVENTS		0x00000002 // Suppress all mouse_event and SendInput mouse events
#define IATWORDALIGNED		0x00000004 // OFT stripped IAT has API entries aligned on word boundaries
#define IATBYTEALIGNED		0x00000008 // OFT stripped IAT has API entries aligned on byte boundaries
#define WRITEON000B0000		0x00000010 // Unprotect and make writeable the memory segment at 0x000B0000
#define NODIALOGS			0x00000020 // Suppresses all dialog boxes created by .....
#define SAFEPALETTEUSAGE	0x00000040 // Use the ddraw palette objects with care ...
#define LOCKFULLSCREENCOOP	0x00000080 // When in fullscreen mode, forces FULLSCREEN cooperative level and returns error when attempting to set NORMAL
#define NOBAADFOOD			0x00000100 // do not initialize heap allocated areas with 0xBAADFOOD initial value, set to 0x00000000!
#define HOTREGISTRY			0x00000200 // force hot patching for advapi32 registry calls
#define NOIATSCAN			0x00000400 // disable IAT scanning for sick cases like Hoyle games DLL
#define HOOKSDLLIB			0x00000800 // Hook SDL library
#define SDLEMULATION		0x00001000 // SDL surface emulation
#define HIDEJOYSTICKS		0x00002000 // Hides joystick devices
#define HOOKSDL2LIB			0x00004000 // Hook SDL lib version 2.x in SDL2.dll
#define SDLFORCESTRETCH		0x00008000 // Force SDL surface stretching also in non-fullscreen context
#define MOUSEMOVEBYEVENT	0x00010000 // replaces SetCursorPos by mouse_event
#define D3DRESOLUTIONHACK	0x00020000 // eliminates d3d maximum resolution limits
#define FIXAILSOUNDLOCKS	0x00040000 // attempt to fix Miles WAIL32 sound deadlocks
#define LOCKTOPZORDER		0x00080000 // tries to lock the window on top
#define EMULATEMAXIMIZE		0x00100000 // emulates MAXIMIZE state 
#define MAKEWINVISIBLE		0x00200000 // makes visible any window queried for visibility status
#define FIXEMPIREOFS		0x00400000 // Runtime patch for "Empire of the Fading Suns"
#define SCALEGLBITMAPS		0x00800000 // Scale bitmap structures passed to OpenGL glBitmap call
#define HOOKWGLCONTEXT		0x01000000 // Hook OpenGL "wiggie" wgl calls related to WGL contexts
#define NOTASKBAROVERLAP	0x02000000 // Prevent window overlap to taskbar
#define CACHED3DSESSION		0x04000000 // Cache D3DSession pointer value (experimental)
#define SLOWSCROLLING		0x08000000 // slow down the scrolling speed (at least, it tries...)
#define KILLBLACKWIN		0x10000000 // suppresses unwanted windows (usually black backgrounds)
#define ZERODISPLAYCOUNTER  0x20000000 // upon ShowCursor(FALSE) forces 0 return value
#define SOUNDMUTE			0x40000000
#define LOCKVOLUME			0x80000000

// 10th flags DWORD dxw.dwFlags10:
#define FORCEHWVERTEXPROC	0x00000001
#define FORCESWVERTEXPROC	0x00000002
#define FORCEMXVERTEXPROC	0x00000004
#define PRECISETIMING		0x00000008 // Use nop cycles instead of Sleep() to suspend threads with precise timing
#define REPLACEDIALOGS		0x00000010 // Replaces troublesome dialog windows with a virtual desktop win
#define FAKEHDDRIVE			0x00000020 // Enable emulation of a fake HD drive
#define FAKECDDRIVE			0x00000040 // Enable emulation of a fake CD drive
#define LIGHTGAMMARAMP		0x00000080 // enables gamma ramp correction to D3D Light color parameters
#define FORCED3DGAMMARAMP	0x00000100 // force setting of gamma ramp in D3D8/9 windowed mode by using GDI SetDeviceGammaRamp call
#define HANDLEFOURCC		0x00000200 // experimental ...
#define SUSPENDTIMESTRETCH	0x00000400 // suspend time stretching during movie play
#define SLOWWINPOLLING		0x00000800 // slow down window status polling with GetActiveWindow & GetForegroundWindow
#define NOOLEINITIALIZE		0x00001000 // suppress harful CoInitialize / OleInitialize calls
#define HWFOURCC			0x00002000 // attempts to handle FourCC surfaces with accelerated ddraw functions
#define SWFOURCC			0x00004000 // attempts to handle FourCC surfaces with software functions
#define LIMITDIBOPERATIONS	0x00008000 // Limit GDI32 operations with DIB against video objects
#define FIXMOUSERAWINPUT	0x00010000 // scales X,Y coordinates retrieved from mouse devices in RawInput mode
#define SETCDVOLUME			0x00020000 // Sets the emulated CD audio volume
#define CUSTOMRES			0x00040000 // add a custom resolution to the list of supported ones
#define CHAOSOVERLORDSFIX	0x00080000 // tweak for "Chaos Overlords" BitBlt problem ....
#define FIXFOLDERPATHS		0x00100000 // fixes some path errors & problems
#define NOCOMPLEXMIPMAPS	0x00200000 // suppresses requests for texture surfaces with COMPLEX+MIPMAP capabilities
#define CDROMPRESENT		0x00400000 // pretend there is a CDROM in each queried device
#define SUPPRESSFOURCCBLT	0x00800000 // pretend the video device has no capabilities to blit FOURCC surfaces
#define INVALIDATECLIENT	0x01000000 // forces InvalidateRect on client area
#define MOUSESHIELD			0x02000000 // suppresses WM_MOUSEMOVE messages being processed when task has no focus
#define CREATEDCHOOK		0x04000000 // hooks GDI32 CreateDCA/W calls (???)
#define NOZBUFATTACH		0x08000000 // Pretends the ZBUFFER is not attached to a surface to allow the creation of a D3D Device
#define SAFEPRIMLOCK		0x10000000 // adds extra space to primary surface lpSurface to bare memcpy overflows
#define PEFILEHOOK			0x20000000 // do dll hooking by PE browsing in exe file 
#define HIDEWINDOWCHANGES	0x40000000 // DxWnd updates the window size & position when WindowProc is disabled
#define STRETCHDIALOGS		0x80000000 // Stretches dialog resources

// 11th flags DWORD dxw.dwFlags11:
#define EXTENDSDLHOOK		0x00000001 // extend hook to SDL/SDL2 dependencies, like OpenGL
#define D3D8MAXWINMODE		0x00000002 // changes Direct3D8EnableMaximizedWindowedModeShim effects setting flag to 0 
#define VIRTUALPROCHEAP		0x00000004 // Virtual Heap also for main process heap
#define MUTEX4CRITSECTION	0x00000008 // Replaces critical regions calls with mutex calls
#define DELAYCRITSECTION	0x00000010 // insert a small delay before entering critical regions
#define HACKMCIFRAMES		0x00000020 // use CD track frame counter stored in mcihack.txt file
#define UNNOTIFYINACTIVE	0x00000040 // do not notify on task switch only for inactive transition
#define REMAPNUMKEYPAD		0x00000080 // remap arrow & function keys to numeric keypad keys
#define SETUSKEYDESCR		0x00000100 // Makes dinput EnumObjects return US keyboard keys descriptions
#define HOOKEARSOUND		0x00000200 // Hooks earpds.dll and earias.dll sound libraries
#define CDPAUSECAPABILITY	0x00000400 // emulate the CD driver's pause capability
#define ADAPTIVERATIO		0x00000800 // fixes aspect ratio according to screen resolution
#define REMAPSYSFOLDERS		0x00001000 // makes Windows system folders relative to current target folder
#define SCALEMAINVIEWPORT	0x00002000 // scales only the viewport size that corresponds with the main window
#define FORCESHAL			0x00004000
#define FORCESNULL			0x00008000
#define USESHORTPATH		0x00010000 // convert long path to short path for Win95 compatibility settings
#define INVERTMOUSEXAXIS	0x00020000 // invert mouse X axis (what else?)
#define INVERTMOUSEYAXIS	0x00040000 // invert mouse Y axis (what else?)
#define SMACKBUFFERNODEPTH	0x00080000 // clears smackw32 flags that force a 16 bit color depth
#define LOCKSYSSETTINGS		0x00100000 // bypass for all SystemParametersInfo(SPI_SET*) operations
#define INVALIDATEFULLRECT	0x00200000 // forces NULL value as rect for InvalidateRect wrapper
#define FIXMESSAGEHOOK		0x00400000 // fixes mouse coordinates retrieved through MouseProc routine associated to SetWindowsHook(WH_MESSAGES,..)
#define NODISABLEPRINT		0x00800000 // disables the attempt to suppress Alt-PrintScreen key with a WH_KEYBOARD or WH_KEYBOARD_LL hook
#define SHRINKFONTWIDTH		0x01000000 // slightly shrings (9/10 ratio) the font width in case they overflow the available space
#define SAFEMIDIOUT			0x02000000 // bypass some faulty winmm.dll midiOut calls
#define MERGEMULTIPLECD		0x04000000 // maps different CD file systems on the same folder avoiding common data replication
#define SAFEALLOCS			0x08000000 // performs additional checks on allocated memory to avoid risky operations
#define FIXASYNCKEYSTATE	0x10000000 // blocks GetAsyncKeyState detection while the window is not active
#define TRANSFORMANDLIGHT	0x20000000 // pretend that the video card supports HW transform & light by setting the capability bit
#define CUSTOMLOCALE		0x40000000 // use custom locale settings from locale.ini
#define FIXDEFAULTMCIID		0x80000000 // remaps Win95/ME default 0xFFFF to the valid 0xFFFFFFFF DWORD value

// 12th flags DWORD dxw.dwFlags12:

#define LOCKCDTRAY			0x00000001 // prevents the programmatic opening/closing of CD tray
#define LOCKGLVIEWPORT		0x00000002 // imposes a fixed viewport area in case the application omitted the operation
#define NOTNLDEVICE			0x00000004 // exclude IID_IDirect3DTnLHalDevice devices
#define CLASSLOCALE			0x00000008 // Hook windows ANSI class api to localize (temporary flag?)
#define PROJECTBUFFER		0x00000010 // Projects the window DC to the selected desktop area - good for custom video cabinates
#define COMMITPAGE			0x00000020 // Reserves (commit) a memory area so that it can't be used/overwritten by other dlls.
// currently used as a "South Park" tweak only, it needs data entry to generalize.
#define SETCMDLINE			0x00000040 // When running a program, set the CommandLine argument with the exe pathname rather than NULL
// needed for some Lucas Arts games that would fire the frontend instead.
#define FORCERELAXIS		0x00000080 // forces mouse devices to send relative axis data
#define FORCEABSAXIS		0x00000100 // forces mouse devices to send absolute axis data
#define LIMITFLIPONLY		0x00000200 // In FPS limit apply delay to Flip operations only
#define DIRECTXREPLACE		0x00000400 // replace directx libraries with alternative implementation like dgVoodoo
#define W98OPAQUEFONT		0x00000800 // suppress the alpha channel on font colors to emulate Win95/98 limitations
#define SUPPRESSGLIDE		0x00001000 // pretend there is no glide support, even when emulated by glide wrappers
#define NOSETPIXELFORMAT	0x00002000 // avoid setting the pixel format when surface emulation is active
#define GLEXTENSIONSLIE		0x00004000 // return a fake, dummy extension string to all opengl queries
#define FIXMOUSELPARAM		0x00008000 // when processing window messages, scales also the lParam field of mouse related messages
#define FAKEGLOBALATOM		0x00010000 // fixes some incompatibilities with global atoms 
#define KILLVSYNC			0x00020000 // Kills vSync option in D3D operations
#define STRETCHPERFREQUENCY	0x00040000 // Stretches the Performance
#define INJECTPROXY			0x00080000 // Proxy mode injection
#define DSINITVOLUME		0x00100000 // Initializes audio volume after DirectSound initialization
#define SCALECBTHOOK		0x00200000 // scale window measures passed to CBT windows hook
#define REVERTDIBPALETTE	0x00400000 // tries to convert a DIB data from 32bpp to 8bpp palettized format
#define FIXDCALREADYCREATED 0x00800000 // tries to fix the GetDC DDERR_DCALREADYCREATED error
#define SUPPRESSMENUS		0x01000000 // suppress SetMenu calls
#define ADAPTMOUSESPEED		0x02000000 // adaptive mouse speed
#define KILLDEADLOCKS		0x04000000 // Suppress some deadlock by setting a finite timeout to Wait operations
#define BLUREFFECT			0x08000000 // set blur effect
#define TEXTUREPALETTE		0x10000000 // Support (emulate) 8BPP palettized textures
#define DISABLEDWM			0x20000000 // Suppress DWM on main window
#define SUPPRESSCDAUDIO		0x40000000 // Suppress CD audio (pretends there is no CD audio in the system)
#define RESERVEMCIDEVICE	0x80000000 // reserves the mci devId used for CD audio emulation

// debug Dflags DWORD:
#define CPUVENDORMASK		0x00000007
#define CPUGENUINEINTEL		0x00000001
#define CPUAUTHENTICAMD		0x00000002
#define CPUCYRIXINSTEAD		0x00000003
#define CPUFAKEDXWNDCPU		0x00000007
#define CPUDISABLECPUID		0x00000008
#define CPUDISABLEMMX		0x00000010
#define CPUDISABLESSE		0x00000020
#define CPUDISABLESSE2		0x00000040
#define CPUDISABLEPBE		0x00000080 // disable PBE flag in EDX register upon CPUID query with EAX = 1. Fixes Cryo "Arthu's Knights" games.
#define DUMPDIBSECTION		0x00000100 // dumps DIBs to BMP files for debugging purposes
#define DUMPDEVCONTEXT		0x00000200 // dumps source HDC to BMP files for debugging purposes
#define DUMPCPUID			0x00000400 // together with DISABLEMMX dups the CPUID registers but does not update them (debugging only!)
#define MARKBLIT			0x00000800 // higlights the blit to primary surface operation by surroundig the rect in color
#define MARKLOCK			0x00001000 // higlights the locked/unlocked surfaces by surroundig the rect in color
#define MARKWING32			0x00002000 // Marks (highlights with colored frames) all WinG32 operations
#define MARKGDI32			0x00004000 // Marks (highlights with colored frames) the main GDI32 operations
#define DOFASTBLT			0x00008000 // use FastBlt to primary surface
#define CENTERTOWIN			0x00010000 // do NOT stretch the image to the whole window client area, but center to it.
#define DUMPSURFACES		0x00020000 // dump all updates to ddraw surfaces in bmp format on surface.out folder
#define NODDRAWBLT			0x00040000 // Suppress ddraw Blt to primary surface
#define NODDRAWFLIP			0x00080000 // Suppress ddraw Flip to primary surface
#define NOGDIBLT			0x00100000 // Suppress GDI Blt to video device
#define STRESSRESOURCES		0x00200000 // simulates a lack of resources condition, for testing (debug opt.)
#define CAPMASK				0x00400000 // Enable capability mask (according to ini file)
#define FULLRECTBLT			0x00800000 // blit to primary surface using NULL source & dest rect
#define ZBUFFERALWAYS		0x01000000 // Forces ZBUFFER D3DCMP_ALWAYS condition to show all planes....
#define HOTPATCHALWAYS		0x02000000 // Force hot patching to every call
#define FREEZEINJECTEDSON	0x04000000 // BEWARE!!!! you must know what you're doing. Leaves the injected son in an endless loop 
#define STARTWITHTOGGLE		0x08000000 // Initialize custom key toggle to TRUE to take effect from program begin
#define CAPTURESCREENS		0x10000000
#define PATCHEXECUTEBUFFER	0x20000000 // patch D3D1 execute buffers by inserting a first one
#define FORCEZBUFFERON		0x40000000 // Force ZBUFFER state ON
#define FORCEZBUFFEROFF		0x80000000 // Force ZBUFFER state OFF

// 13th flags DWORD dxw.dwFlags13:
#define GLFIXCLAMP			0x00000001 // Forces GL_CLAMP_TO_EDGE instead of GL_CLAMP

// 14th flags DWORD dxw.dwFlags14:
// 15th flags DWORD dxw.dwFlags15:
// 16th flags DWORD dxw.dwFlags16:

// debug Dflags2 DWORD:
#define NOWINDOWHOOKS		0x00000001 // do not intercept window callback routines
#define EXPERIMENTAL		0x00000002 // activate version-specific code variants. Developer's use only.
#define FIXRANDOMPALETTE	0x00000004 // ignores palette directive and set a fixed palette with random colors
#define DUMPBITMAPS			0x00000008 // dump HBITMAP objects
#define DUMPBLITSRC			0x00000010 // dump surfaces source of a Blt operation
#define DISABLEWINHOOKS		0x00000020 // disables all windows hooks

// logging Tflags DWORD:
#define OUTTRACE			0x00000001 // enables tracing to dxwnd.log in general
#define OUTDDRAWTRACE		0x00000002 // traces DxWnd directdraw screen handling
#define OUTWINMESSAGES		0x00000004 // traces windows messages
#define OUTCURSORTRACE		0x00000008 // traces cursor positions & operations
#define OUTSEPARATED		0x00000010 // write the log on a separate file named dxwnd(n).log where n is the figure of first non-existent file
#define OUTCIRCULAR			0x00000020 // keep writing up to 1000 log lines and swap file when maximum reached
#define ASSERTDIALOG		0x00000040 // show assert messages in Dialog Box
#define OUTIMPORTTABLE		0x00000080 // dump import table contents
#define OUTDEBUG			0x00000100 // detailed debugging information
#define OUTREGISTRY			0x00000200 // log registry operations
#define TRACEHOOKS			0x00000400 // log hook operations
#define OUTD3DTRACE			0x00000800 // traces DxWnd direct3d screen handling
#define OUTDXWINTRACE		0x00001000 // traces DxWnd internal operations
#define OUTWINGTRACE		0x00002000 // traces WinG32 hooked calls
#define OUTOGLTRACE			0x00004000 // traces OpenGL hooked calls
#define OUTHEXTRACE			0x00008000 // traces hexdump of critical structures
#define OUTSDLTRACE			0x00010000 // traces SDL calls
#define OUTTIMETRACE		0x00020000 // traces time stretching calls and operations
#define OUTSOUNDTRACE		0x00040000 // sound trace (DirectSound etc.)
#define OUTINPUTS			0x00080000 // input trace (winmm joy*, DirectInput etc.)
#define OUTSYSLIBS			0x00100000 // system libraries: GDI, USER32, KERNEL 
#define OUTLOCALE			0x00200000 // Locale-related operations 
#define OUTFPS				0x00400000 // FPS counters 
#define ADDRELATIVETIME		0x08000000 // log timestamp is relative to previous line
#define ADDTHREADID			0x10000000 // add thread id
#define ADDTIMESTAMP		0x20000000 // add timestamp (GetTickCount) to log file
#define OUTDEBUGSTRING		0x40000000 // duplicate logs through OutputDebugString()
#define ERASELOGFILE		0x80000000 // clears old trace file before writing new logs

#define HANDLEFPS			(SHOWFPS | SHOWFPSOVERLAY | LIMITFPS | SKIPFPS)
#define TEXTUREMASK			(TEXTUREHIGHLIGHT|TEXTUREDUMP|TEXTUREHACK|TEXTURETRANSP)
#define CPUALLFLAGSMASK		(CPUVENDORMASK|CPUDISABLEMMX|CPUDISABLECPUID|CPUDISABLESSE|CPUDISABLESSE2|CPUDISABLEPBE)

// DxWnd dll capabilities

#define DXWCAPS_CANLOG		0x00000001
#define DXWCAPS_DEBUG		0x00000002

// DxWnd host app data to be passed to the hook callback
typedef struct TARGETMAP {
    int index;
    char path[MAX_PATH + 1];
    char module[60 + 1];
    char OpenGLLib[20 + 1];
    int dxversion;
    int coordinates;
    int flags;
    int flags2;
    int flags3;
    int flags4;
    int flags5;
    int flags6;
    int flags7;
    int flags8;
    int flags9;
    int flags10;
    int flags11;
    int flags12;
    int flags13;
    int flags14;
    int flags15;
    int flags16;
    int tflags; // trace flags (cleared when exported/imported)
    int dflags; // debug flags (cleared when exported/imported)
    int dflags2; // debug2 flags (cleared when exported/imported)
    int monitorid;
    short posx;
    short posy;
    short sizx;
    short sizy;
    short resw;
    short resh;
    short MaxFPS;
    short InitTS;
    short FakeVersionId;
    short MaxScreenRes;
    short SwapEffect;
    short MaxDdrawInterface;
    short SlowRatio;
    short ScanLine;
    short RendererId;
    short FilterId;
    short FilterXScalingFactor;
    short FilterYScalingFactor;
    short FixedVolume;
    char  FakeHDDrive;
    char  FakeCDDrive;
    short Country;
    short CodePage;
    short Language;
} TARGETMAP;

// VJoyStatus flags
#define VJOYPRESENT		0x00000001 // Virtual joystick configured for the program
#define VJOYENABLED		0x00000002 // Virtual Joystick enabled and sending input coordinates
#define CROSSENABLED	0x00000004 // Crosshairs display enabled
#define INVERTYAXIS		0x00000008 // Invert virtual joystick Y axis
#define INVERTXAXIS		0x00000010 // Invert virtual joystick X axis
#define B1AUTOFIRE		0x00000020 // Button 1 auto fire
#define B2AUTOFIRE		0x00000040 // Button 2 auto fire
#define VJAUTOCENTER	0x00000080 // auto-center feature
#define VJKEYBOARDMAP	0x00000100 // mapvirtual joystick lever & buttons to keyboard
#define VJMOUSEMAP		0x00000200 // mapvirtual joystick lever & buttons to mouse
#define VJSENSIVITY		0x00000400 // tune sensibility through mouse wheel
#define VJMOUSEWHEEL	0x00000800 // use mouse wheel for Y axis 
#define VJXLEFTONLY		0x00001000 // control XBOX 360 left stick only
#define VJXRIGHTONLY	0x00002000 // control XBOX 360 right stick only
#define VJBORDERPOV		0x00004000 // controls POV status by moving cursor close to win borders
#define VJREMOTECONTROL 0x00008000 // control joystick from control panel window
#define LOCKONMIDBTN	0x00010000 // use mouse mid button to lock the joystick to centered position & keys off

typedef struct {
    short Status;
    short TaskIdx;
    short OrigIdx;
    short IsFullScreen;
    short Width, Height;
    short ColorDepth;
    DDPIXELFORMAT pfd;
    short DXVersion;
    HWND hWnd;
    DWORD dwPid;
    BOOL isLogging;
    DWORD FPSCount;
    int TimeShift;
    short CursorX, CursorY;
    PALETTEENTRY Palette[256];
    BOOL AllowMultiTask;
    DWORD VJoyStatus;
    DWORD VJoySensivity;
    DWORD VJoyDeadZone;
    DWORD VJoySaturation;
    DWORD LockOnMidBtn;
    short joyposx;
    short joyposy;
    DWORD joyButtons;
    BOOL GammaControl;
    BYTE GammaRamp[256 * 3];
    short PlayerStatus;
    short TrackNo;
    short TracksNo;
    short Volume;
    int TimeElapsed;
    int TrackLength;
    short CDIndex;
    short ScreenRotation;
    //DWORD dummy1;
    short MessageX;
    short MessageY;
    short WinProcX;
    short WinProcY;
    short MsgHookX;
    short MsgHookY;
} DXWNDSTATUS;

extern DXWNDSTATUS DxWndStatus;

int SetTarget(DXWNDSTATUS *, TARGETMAP *);
int StartHook(void);
int EndHook(void);
void GetDllVersion(char *);
int GetHookStatus(DXWNDSTATUS *);
DXWNDSTATUS *GetHookInfo();
void HookInit(TARGETMAP *, HWND);
void ShowHint(int);
LPCSTR GetFlagCaption(int, int);

char *GetDxWndPath();
void *SetHook(void *, void *);
void SetHook(void *, void *, void **, char *);
void *HookAPI(HMODULE, char *, void *, const char *, void *);
char *hexdump(unsigned char *, int);
void HexTrace(unsigned char *, int);

LRESULT CALLBACK extWindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK extChildWindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK extDialogWindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK dw_Hider_Message_Handler(HWND, UINT, WPARAM, LPARAM);

typedef enum {
    DXW_FILTER_NONE = 0,
    DXW_FILTER_BILX2,
    DXW_FILTER_HQX2,
    DXW_FILTER_HQX3,
    DXW_FILTER_HQX4,
    DXW_FILTER_DEINTERLACE,
    DXW_FILTER_INTERLACE,
    DXW_FILTER_PIX2,
    DXW_FILTER_PIX3,
    DXW_FILTER_PIX4,
    DXW_FILTER_SCALE2X,
    DXW_FILTER_SCALE3X,
    DXW_FILTER_SCALE4X,
    DXW_FILTER_SCALE2K,
    DXW_FILTER_SCALE3K,
    DXW_FILTER_SCALE4K,
    DXW_FILTER_LAST
} dxw_Filter_Id;

typedef enum {
    DXW_RENDERER_NONE = 0,
    DXW_RENDERER_EMULATEBUFFER,
    DXW_RENDERER_LOCKEDSURFACE,
    DXW_RENDERER_PRIMSURFACE,
    DXW_RENDERER_HYBRID,
    DXW_RENDERER_GDI,
    DXW_RENDERER_SDL,
    DXW_RENDERER_SDL2,
    DXW_RENDERER_OPENGL,
    DXW_RENDERER_LAST
} dxw_Renderer_Id;

typedef enum {
    DXW_PLAYER_STOPPED = 0,
    DXW_PLAYER_PLAYING,
    DXW_PLAYER_PAUSED
} dxw_player_status;

// Renderer flags
// DXWRF_EMULATED: filter works on emulated resources, that is memory surfaces with no relation to the final real resources
#define DXWRF_EMULATED				0x00000001
// DXWRF_BUILDREALSURFACES: filter requires the creation of ddraw real surfaces, that is a primary surface and its backbuffer
#define DXWRF_BUILDREALSURFACES		0x00000002
// DXWRF_BUILDLOCKSURFACE: filter requires the creation of a support surface to transfer screen image before lock/unlock operations on primary
#define DXWRF_BUILDLOCKSURFACE		0x00000004
// DXWRF_PALETTEREFRESH: filter supports screen changes on palette update, so that it's not necessary to add a screen refresh
#define DXWRF_PALETTEREFRESH		0x00000008
// DXWRF_USEBACKBUFFER: screenshots are best captured from backbuffer surface instead of primary
#define DXWRF_USEBACKBUFFER			0x00000010
// DXWRF_SUPPORTFILTERS: you can apply rendering filters to the primary surface
#define DXWRF_SUPPORTFILTERS		0x00000020
// DXWRF_BUILDLOCKBUFFER: filter requires the creation of a support memory buffer to transfer screen image before lock/unlock operations on primary
#define DXWRF_BUILDLOCKBUFFER		0x00000040

typedef struct {
    char *name;
    USHORT id;
    int xfactor;
    int yfactor;
} dxw_Filter_Type;

typedef struct {
    char *name;
    USHORT id;
    DWORD flags;
} dxw_Renderer_Type;

typedef enum {
    DXW_SET_COORDINATES = 0,
    DXW_DESKTOP_CENTER,
    DXW_DESKTOP_WORKAREA,
    DXW_DESKTOP_FULL
} Coordinates_Types;

typedef enum {
    DXW_NO_LIMIT = 0,
    DXW_LIMIT_320x200,
    DXW_LIMIT_400x300,
    DXW_LIMIT_640x480,
    DXW_LIMIT_800x600,
    DXW_LIMIT_1024x768,
    DXW_LIMIT_1280x960,
    DXW_LIMIT_1280x1024
} ResolutionLimits_Types;

typedef enum {
    TIMER_TYPE_NONE = 0,
    TIMER_TYPE_USER32,
    TIMER_TYPE_WINMM
} Timer_Types;

typedef struct {
    DWORD w;
    DWORD h;
} SupportedRes_Type;

extern SupportedRes_Type SupportedRes[];
extern int SupportedDepths[];

#define SUPPORTED_DEPTHS_NUMBER 4

