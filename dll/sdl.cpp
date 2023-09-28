#define _CRT_SECURE_NO_WARNINGS

// --------------------------------------------------//
// BEWARE: SDL lib is no WINAPI!!!                   //
// --------------------------------------------------//

#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "hddraw.h"
#include "dxhook.h"
#include "dxhelper.h"
#include "sdl/SDL.h"
#include "sdl/SDL_Events.h"
#include "sdl2/SDL_Video.h"
#include "sdl/SDL_syswm.h"

#include "stdio.h"

//#define _SDL_DEBUG

#ifndef DXW_NOTRACES
#define IfSDLError(ret)	if(ret) OutTraceE("%s: ERROR err=\"%s\"\n", ApiRef, intSDL_ErrorMessage())
#else
#define IfSDLError(ret)
#endif // DXW_NOTRACES


extern BOOL vjGetJoy(char *, LPJOYINFO);

static char *sdlerror(void) {
    return "unknown!";
}

Uint32 gVideoFlags = 0;
BOOL bEmulation = FALSE;
int gSDL_JoystickEventState = TRUE;
static int intSDL_BlitScaled(SDL_Surface *, const SDL_Rect *, SDL_Surface *, SDL_Rect *);

// SDL2 opaque types
#ifndef SDL_Renderer
// opaque definition for SDL2 SDL_Renderer type
#define SDL_Renderer void
#endif
#ifndef SDL_Window
// opaque definition for SDL2 SDL_Window type
#define SDL_Window void
#endif
#ifndef SDL_Texture
// opaque definition for SDL2 SDL_Texture type
#define SDL_Texture void
#endif
#ifndef SDL_GLContext
// opaque definition for SDL2 SDL_GLContext type
#define SDL_GLContext void *
#endif
// definition for SDL2 SDL_DisplayMode type
#ifndef SDL_DisplayMode
typedef struct {
    Uint32 format;              /**< pixel format */
    int w;                      /**< width, in screen coordinates */
    int h;                      /**< height, in screen coordinates */
    int refresh_rate;           /**< refresh rate (or zero for unspecified) */
    void *driverdata;           /**< driver-specific data, initialize to 0 */
} SDL_DisplayMode;
#endif

// ptr to the currently active SDL video surface
SDL_Surface *gSDL_VideoSurface = NULL;
SDL_Surface *gSDL_EmulatedVideo = NULL;
SDL_Window *gSDL_MainWindow = NULL;

typedef int (* SDL_Init_Type)(Uint32);
typedef void (* SDL_Quit_Type)(void);
typedef char *(* SDL_GetError_Type)(void);
typedef SDL_Surface *(* SDL_SetVideoMode_Type)(int, int, int, Uint32);
typedef int (* SDL_VideoModeOK_Type)(int, int, int, Uint32);
typedef Uint32 (* SDL_GetMouseState_Type)(int *, int *);
typedef int (* SDL_Flip_Type)(SDL_Surface *);
typedef int (* SDL_LockSurface_Type)(SDL_Surface *);
typedef void (* SDL_UnlockSurface_Type)(SDL_Surface *);
typedef SDL_Surface *(* SDL_CreateRGBSurface_Type)(Uint32, int, int, int, Uint32, Uint32, Uint32, Uint32);
typedef SDL_Surface *(* SDL_CreateRGBSurfaceFrom_Type)(void *, int, int, int, int, Uint32, Uint32, Uint32, Uint32);
typedef int (* SDL_BlitSurface_Type)(SDL_Surface *, const SDL_Rect *, SDL_Surface *, SDL_Rect *);
typedef int (* SDL_FillRect_Type)(SDL_Surface *, const SDL_Rect *, Uint32);
typedef SDL_Surface *(* SDL_GetVideoSurface_Type)(void);
typedef void (* SDL_FreeSurface_Type)(SDL_Surface *);
typedef int (* SDL_SetAlpha_Type)(SDL_Surface *, Uint32, Uint8);
typedef int (* SDL_SetColorKey_Type)(SDL_Surface *, int, Uint32);
typedef int (* SDL_SetColors_Type)(SDL_Surface *, SDL_Color *, int, int);
typedef void (* SDL_Delay_Type)(Uint32);
typedef Uint32 (* SDL_GetTicks_Type)(void);
typedef int (* SDL_WaitEventTimeout_Type)(SDL_Event *, int);
typedef void (* SDL_WarpMouse_Type)(Uint16, Uint16);
typedef SDL_Overlay *(* SDL_CreateYUVOverlay_Type)(int, int, Uint32, SDL_Surface *);
typedef int (* SDL_DisplayYUVOverlay_Type)(SDL_Overlay *, SDL_Rect *);
typedef void (* SDL_FreeYUVOverlay_Type)(SDL_Overlay *);
typedef int (* SDL_LockYUVOverlay_Type)(SDL_Overlay *);
typedef void (* SDL_UnlockYUVOverlay_Type)(SDL_Overlay *);
typedef SDL_bool (* SDL_SetClipRect_Type)(SDL_Surface *, SDL_Rect *);
typedef void (* SDL_GetClipRect_Type)(SDL_Surface *, SDL_Rect *);
typedef int (* SDL_NumJoysticks_Type)(void);
typedef const char *(* SDL_JoystickName_Type)(SDL_Joystick *);
typedef const char *(* SDL_JoystickNameForIndex_Type)(int); // SDL2.0
typedef SDL_Joystick *(* SDL_JoystickOpen_Type)(int);
typedef int (* SDL_JoystickNum_Type)(SDL_Joystick *);
typedef Sint16 (* SDL_JoystickGetAxis_Type)(SDL_Joystick *, int);
typedef void (* SDL_JoystickUpdate_Type)(void);
typedef Uint8 (* SDL_JoystickGetButton_Type)(SDL_Joystick *, int);
typedef Uint8 (* SDL_JoystickGetHat_Type)(SDL_Joystick *, int);
typedef void (* SDL_JoystickClose_Type)(SDL_Joystick *);
typedef int (* SDL_JoystickEventState_Type)(int);
typedef int (* SDL_SetPalette_Type)(SDL_Surface *, int, SDL_Color *, int, int);
typedef void (* SDL_UpdateRects_Type)(SDL_Surface *, int, SDL_Rect *);
typedef SDL_Surface *(*SDL_ConvertSurface_Type)(SDL_Surface *, const SDL_PixelFormat *, Uint32);
typedef const SDL_VideoInfo *(*SDL_GetVideoInfo_Type)(void);
typedef SDL_Cursor *(*SDL_CreateCursor_Type)(Uint8 *, Uint8 *, int, int, int, int);
typedef void (*SDL_FreeCursor_Type)(SDL_Cursor *);
typedef SDL_Surface *(*SDL_DisplayFormatAlpha_Type)(SDL_Surface *);
typedef SDL_Rect **(*SDL_ListModes_Type)(SDL_PixelFormat *, Uint32);
typedef int (*SDL_JoystickOpened_Type)(int);
//typedef int (*SDL_SetWindowFullscreen_Type)(SDL_Window *, Uint32);
typedef int (*SDL_SetWindowFullscreen_Type)(VOID *, Uint32);
typedef int (*SDL_PollEvent_Type)(SDL_Event *);
typedef SDL_Renderer *(*SDL_CreateRenderer_Type)(SDL_Window *, int, Uint32);
typedef SDL_Renderer *(*SDL_CreateSoftwareRenderer_Type)(SDL_Surface *);
typedef SDL_Window *(*SDL_CreateWindow_Type)(const char *, int, int, int, int, Uint32);
typedef SDL_Texture *(*SDL_CreateTexture_Type)(SDL_Renderer *, Uint32, int, int, int);
typedef int (*SDL_UpdateTexture_Type)(SDL_Texture *, const SDL_Rect *, const void *, int);
typedef int (*SDL_PeepEvents1_Type)(SDL_Event *, int, SDL_eventaction, Uint32, Uint32);
typedef int (*SDL_PeepEvents2_Type)(SDL_Event *, int, SDL_eventaction, Uint32);
typedef void (*SDL_SetWindowPosition_Type)(SDL_Window *, int, int);
typedef void (*SDL_SetWindowSize_Type)(SDL_Window *, int, int);
typedef void (*SDL_GL_SwapWindow_Type)(SDL_Window *);
typedef SDL_bool (*SDL_GetWindowWMInfo_Type)(SDL_Window *, SDL_SysWMinfo *);
typedef int (*SDL_SetWindowBrightness_Type)(SDL_Window *, float);
typedef int (*SDL_GetCurrentDisplayMode_Type)(int, SDL_DisplayMode *);
typedef int (*SDL_GetDesktopDisplayMode_Type)(int, SDL_DisplayMode *);
typedef int (*SDL_GL_SetSwapInterval_Type)(int);
typedef SDL_GLContext (*SDL_GL_CreateContext_Type)(SDL_Window *);
typedef void (*SDL_SetEventFilter_Type)(SDL_EventFilter, void *);
typedef void (*SDL_GetWindowSize_Type)(SDL_Window *, int *, int *);
typedef int (*SDL_GetDisplayMode_Type)(int, int, SDL_DisplayMode *);
typedef int (*SDL_ShowCursor_Type)(int);
typedef int (*SDL_GetDisplayBounds_Type)(int, SDL_Rect *);
typedef int (*SDL_SetWindowDisplayMode_Type)(SDL_Window *, const SDL_DisplayMode *);
typedef int (*SDL_CondWaitTimeout_Type)(SDL_cond *, SDL_mutex *, Uint32);
typedef int (*SDL_GetWMInfo_Type)(SDL_SysWMinfo *);

SDL_GetError_Type pSDL_GetError = NULL;
SDL_Init_Type pSDL_Init, pSDL_InitSubSystem, pSDL_QuitSubSystem;
SDL_Quit_Type pSDL_Quit;
SDL_SetVideoMode_Type pSDL_SetVideoMode;
SDL_VideoModeOK_Type pSDL_VideoModeOK;
SDL_GetMouseState_Type pSDL_GetMouseState, pSDL_GetRelativeMouseState;
SDL_Flip_Type pSDL_Flip;
SDL_LockSurface_Type pSDL_LockSurface;
SDL_UnlockSurface_Type pSDL_UnlockSurface;
SDL_CreateRGBSurface_Type pSDL_CreateRGBSurface;
SDL_CreateRGBSurfaceFrom_Type pSDL_CreateRGBSurfaceFrom;
SDL_BlitSurface_Type pSDL_UpperBlit, pSDL_LowerBlit, pSDL_BlitSurface, pSDL_BlitScaled, pSDL_SoftStretch;
SDL_FillRect_Type pSDL_FillRect;
SDL_GetVideoSurface_Type pSDL_GetVideoSurface;
SDL_FreeSurface_Type pSDL_FreeSurface;
SDL_SetAlpha_Type pSDL_SetAlpha;
SDL_SetColorKey_Type pSDL_SetColorKey;
SDL_SetColors_Type pSDL_SetColors;
SDL_Delay_Type pSDL_Delay;
SDL_GetTicks_Type pSDL_GetTicks;
SDL_WaitEventTimeout_Type pSDL_WaitEventTimeout;
SDL_WarpMouse_Type pSDL_WarpMouse;
SDL_CreateYUVOverlay_Type pSDL_CreateYUVOverlay;
SDL_DisplayYUVOverlay_Type pSDL_DisplayYUVOverlay;
SDL_FreeYUVOverlay_Type pSDL_FreeYUVOverlay;
SDL_LockYUVOverlay_Type pSDL_LockYUVOverlay;
SDL_UnlockYUVOverlay_Type pSDL_UnlockYUVOverlay;
SDL_SetClipRect_Type pSDL_SetClipRect;
SDL_GetClipRect_Type pSDL_GetClipRect;
SDL_NumJoysticks_Type pSDL_NumJoysticks;
SDL_JoystickName_Type pSDL_JoystickName;
SDL_JoystickNameForIndex_Type pSDL_JoystickNameForIndex;
SDL_JoystickOpen_Type pSDL_JoystickOpen;
SDL_JoystickNum_Type pSDL_JoystickNumAxes, pSDL_JoystickNumButtons, pSDL_JoystickNumBalls;
SDL_JoystickGetAxis_Type pSDL_JoystickGetAxis;
SDL_JoystickUpdate_Type pSDL_JoystickUpdate;
SDL_JoystickGetButton_Type pSDL_JoystickGetButton;
SDL_JoystickGetHat_Type pSDL_JoystickGetHat;
SDL_JoystickClose_Type pSDL_JoystickClose;
SDL_JoystickEventState_Type pSDL_JoystickEventState;
SDL_SetPalette_Type pSDL_SetPalette;
SDL_UpdateRects_Type pSDL_UpdateRects;
SDL_ConvertSurface_Type pSDL_ConvertSurface;
SDL_GetVideoInfo_Type pSDL_GetVideoInfo;
SDL_CreateCursor_Type pSDL_CreateCursor;
SDL_FreeCursor_Type pSDL_FreeCursor;
SDL_DisplayFormatAlpha_Type pSDL_DisplayFormatAlpha;
SDL_ListModes_Type pSDL_ListModes;
SDL_JoystickOpened_Type pSDL_JoystickOpened;
SDL_SetWindowFullscreen_Type pSDL_SetWindowFullscreen;
SDL_PollEvent_Type pSDL_PollEvent, pSDL_PushEvent;
SDL_CreateRenderer_Type pSDL_CreateRenderer;
SDL_CreateSoftwareRenderer_Type pSDL_CreateSoftwareRenderer;
SDL_CreateWindow_Type pSDL_CreateWindow;
SDL_CreateTexture_Type pSDL_CreateTexture;
SDL_UpdateTexture_Type pSDL_UpdateTexture;
SDL_PeepEvents1_Type pSDL_PeepEvents1;
SDL_PeepEvents2_Type pSDL_PeepEvents2;
SDL_SetWindowPosition_Type pSDL_SetWindowPosition;
SDL_SetWindowSize_Type pSDL_SetWindowSize;
SDL_GL_SwapWindow_Type pSDL_GL_SwapWindow;
SDL_GetWindowWMInfo_Type pSDL_GetWindowWMInfo;
SDL_SetWindowBrightness_Type pSDL_SetWindowBrightness;
SDL_GetCurrentDisplayMode_Type pSDL_GetCurrentDisplayMode;
SDL_GetDesktopDisplayMode_Type pSDL_GetDesktopDisplayMode;
SDL_GL_SetSwapInterval_Type pSDL_GL_SetSwapInterval;
SDL_GL_CreateContext_Type pSDL_GL_CreateContext;
SDL_SetEventFilter_Type pSDL_SetEventFilter;
SDL_GetWindowSize_Type pSDL_GetWindowSize;
SDL_GetDisplayMode_Type pSDL_GetDisplayMode;
SDL_ShowCursor_Type pSDL_ShowCursor;
SDL_GetDisplayBounds_Type pSDL_GetDisplayBounds, pSDL_GetDisplayUsableBounds;
SDL_SetWindowDisplayMode_Type pSDL_SetWindowDisplayMode;
SDL_CondWaitTimeout_Type pSDL_CondWaitTimeout;
SDL_GetWMInfo_Type pSDL_GetWMInfo;

SDL_BlitSurface_Type pStretch = NULL;

int extSDL_Init(Uint32);
int extSDL_InitSubSystem(Uint32);
void extSDL_Quit(void);
int extSDL_QuitSubSystem(Uint32);
SDL_Surface *extSDL_SetVideoMode(int, int, int, Uint32);
int extSDL_VideoModeOK(int, int, int, Uint32);
Uint32 extSDL_GetMouseState(int *, int *);
Uint32 extSDL_GetRelativeMouseState(int *, int *);
int extSDL_Flip(SDL_Surface *);
int extSDL_LockSurface(SDL_Surface *);
void extSDL_UnlockSurface(SDL_Surface *);
SDL_Surface *extSDL_CreateRGBSurface(Uint32, int, int, int, Uint32, Uint32, Uint32, Uint32);
SDL_Surface *extSDL_CreateRGBSurfaceFrom(void *, int, int, int, int, Uint32, Uint32, Uint32, Uint32);
int extSDL_UpperBlit(SDL_Surface *, const SDL_Rect *, SDL_Surface *, SDL_Rect *);
int extSDL_LowerBlit(SDL_Surface *, const SDL_Rect *, SDL_Surface *, SDL_Rect *);
int extSDL_BlitSurface(SDL_Surface *, const SDL_Rect *, SDL_Surface *, SDL_Rect *);
int extSDL_BlitScaled(SDL_Surface *, const SDL_Rect *, SDL_Surface *, SDL_Rect *);
int extSDL_SoftStretch(SDL_Surface *, const SDL_Rect *, SDL_Surface *, SDL_Rect *);
int extSDL_FillRect(SDL_Surface *, const SDL_Rect *, Uint32);
SDL_Surface *extSDL_GetVideoSurface(void);
void extSDL_FreeSurface(SDL_Surface *);
int extSDL_SetAlpha(SDL_Surface *, Uint32, Uint8);
int extSDL_SetColorKey(SDL_Surface *, int, Uint32);
int extSDL_SetColors(SDL_Surface *, SDL_Color *, int, int);
void extSDL_Delay(Uint32);
Uint32 extSDL_GetTicks(void);
int extSDL_WaitEventTimeout(SDL_Event *, int);
void extSDL_WarpMouse(Uint16, Uint16);
SDL_Overlay *extSDL_CreateYUVOverlay(int, int, Uint32, SDL_Surface *);
int extSDL_DisplayYUVOverlay(SDL_Overlay *, SDL_Rect *);
void extSDL_FreeYUVOverlay(SDL_Overlay *);
int extSDL_LockYUVOverlay(SDL_Overlay *);
void extSDL_UnlockYUVOverlay(SDL_Overlay *);
SDL_bool extSDL_SetClipRect(SDL_Surface *, SDL_Rect *);
void extSDL_GetClipRect(SDL_Surface *, SDL_Rect *);
int extSDL_NumJoysticks(void);
const char *extSDL_JoystickName(SDL_Joystick *);
const char *extSDL_JoystickNameForIndex(int);
SDL_Joystick *extSDL_JoystickOpen(int);
int extSDL_JoystickNumAxes(SDL_Joystick *);
int extSDL_JoystickNumButtons(SDL_Joystick *);
int extSDL_JoystickNumBalls(SDL_Joystick *);
Sint16 extSDL_JoystickGetAxis(SDL_Joystick *, int);
void extSDL_JoystickUpdate(void);
Uint8 extSDL_JoystickGetButton(SDL_Joystick *, int);
Uint8 extSDL_JoystickGetHat(SDL_Joystick *, int);
void extSDL_JoystickClose(SDL_Joystick *);
int extSDL_JoystickEventState(int);
int extSDL_SetPalette(SDL_Surface *, int, SDL_Color *, int, int);
void extSDL_UpdateRects(SDL_Surface *, int, SDL_Rect *);
SDL_Surface *extSDL_ConvertSurface(SDL_Surface *, const SDL_PixelFormat *, Uint32);
const SDL_VideoInfo *extSDL_GetVideoInfo(void);
SDL_Cursor *extSDL_CreateCursor(Uint8 *, Uint8 *, int, int, int, int);
void extSDL_FreeCursor(SDL_Cursor *);
SDL_Surface *extSDL_DisplayFormatAlpha(SDL_Surface *);
SDL_Rect **extSDL_ListModes(SDL_PixelFormat *, Uint32);
int extSDL_JoystickOpened(int);
//int extSDL_SetWindowFullscreen(SDL_Window *, Uint32);
int extSDL_SetWindowFullscreen(VOID *, Uint32);
int extSDL_PollEvent(SDL_Event *);
int extSDL_PushEvent(SDL_Event *);
SDL_Renderer *extSDL_CreateRenderer(SDL_Window *, int, Uint32);
SDL_Renderer *extSDL_CreateSoftwareRenderer(SDL_Surface *);
SDL_Window *extSDL_CreateWindow(const char *, int, int, int, int, Uint32);
SDL_Texture *extSDL_CreateTexture(SDL_Renderer *, Uint32, int, int, int);
int extSDL_UpdateTexture(SDL_Texture *, const SDL_Rect *, const void *, int);
int extSDL_PeepEvents1(SDL_Event *, int, SDL_eventaction, Uint32, Uint32);
int extSDL_PeepEvents2(SDL_Event *, int, SDL_eventaction, Uint32);
void extSDL_SetWindowPosition(SDL_Window *, int, int);
void extSDL_SetWindowSize(SDL_Window *, int, int);
void extSDL_GL_SwapWindow(SDL_Window *);
SDL_bool extSDL_GetWindowWMInfo(SDL_Window *, SDL_SysWMinfo *);
int extSDL_SetWindowBrightness(SDL_Window *, float);
int extSDL_GetCurrentDisplayMode(int, SDL_DisplayMode *);
int extSDL_GetDesktopDisplayMode(int, SDL_DisplayMode *);
int extSDL_GL_SetSwapInterval(int);
SDL_GLContext extSDL_GL_CreateContext(SDL_Window *);
void extSDL_SetEventFilter(SDL_EventFilter, void *);
void extSDL_GetWindowSize(SDL_Window *, int *, int *);
int extSDL_GetDisplayMode(int, int, SDL_DisplayMode *);
int extSDL_ShowCursor(int);
int extSDL_GetDisplayBounds(int, SDL_Rect *);
int extSDL_GetDisplayUsableBounds(int, SDL_Rect *);
int extSDL_SetWindowDisplayMode(SDL_Window *, const SDL_DisplayMode *);
int extSDL_CondWaitTimeout(SDL_cond *, SDL_mutex *, Uint32);

static void Stopper(char *s, int line) {
    char sMsg[81];
    sprintf_s(sMsg, 80, "break: \"%s\"", s);
    MessageBox(0, sMsg, "break", MB_OK | MB_ICONEXCLAMATION);
}

//#define STOPPER_TEST // comment out to eliminate
#ifdef STOPPER_TEST
#define STOPPER(s) Stopper(s, __LINE__)
#else
#define STOPPER(s)
#endif

#define mod(x) ((x > 0) ? (x) : -(x))

#define _Warn(s) MessageBox(0, s, "to do", MB_ICONEXCLAMATION)

static HookEntryEx_Type Hooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "SDL_Init", (FARPROC)NULL, (FARPROC *) &pSDL_Init, (FARPROC)extSDL_Init},
    {HOOK_IAT_CANDIDATE, 0, "SDL_InitSubSystem", (FARPROC)NULL, (FARPROC *) &pSDL_InitSubSystem, (FARPROC)extSDL_InitSubSystem},
    {HOOK_IAT_CANDIDATE, 0, "SDL_Quit", (FARPROC)NULL, (FARPROC *) &pSDL_Quit, (FARPROC)extSDL_Quit},
    {HOOK_IAT_CANDIDATE, 0, "SDL_QuitSubSystem", (FARPROC)NULL, (FARPROC *) &pSDL_QuitSubSystem, (FARPROC)extSDL_QuitSubSystem},
    {HOOK_IAT_CANDIDATE, 0, "SDL_SetVideoMode", (FARPROC)NULL, (FARPROC *) &pSDL_SetVideoMode, (FARPROC)extSDL_SetVideoMode},
    {HOOK_IAT_CANDIDATE, 0, "SDL_VideoModeOK", (FARPROC)NULL, (FARPROC *) &pSDL_VideoModeOK, (FARPROC)extSDL_VideoModeOK},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GetMouseState", (FARPROC)NULL, (FARPROC *) &pSDL_GetMouseState, (FARPROC)extSDL_GetMouseState},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GetRelativeMouseState", (FARPROC)NULL, (FARPROC *) &pSDL_GetRelativeMouseState, (FARPROC)extSDL_GetRelativeMouseState},
    {HOOK_IAT_CANDIDATE, 0, "SDL_FreeSurface", (FARPROC)NULL, (FARPROC *) &pSDL_FreeSurface, (FARPROC)extSDL_FreeSurface},
    {HOOK_IAT_CANDIDATE, 0, "SDL_LockSurface", (FARPROC)NULL, (FARPROC *) &pSDL_LockSurface, (FARPROC)extSDL_LockSurface},
    {HOOK_IAT_CANDIDATE, 0, "SDL_UnlockSurface", (FARPROC)NULL, (FARPROC *) &pSDL_UnlockSurface, (FARPROC)extSDL_UnlockSurface},
    {HOOK_IAT_CANDIDATE, 0, "SDL_Flip", (FARPROC)NULL, (FARPROC *) &pSDL_Flip, (FARPROC)extSDL_Flip},
    {HOOK_IAT_CANDIDATE, 0, "SDL_UpperBlit", (FARPROC)NULL, (FARPROC *) &pSDL_UpperBlit, (FARPROC)extSDL_UpperBlit},
    {HOOK_IAT_CANDIDATE, 0, "SDL_LowerBlit", (FARPROC)NULL, (FARPROC *) &pSDL_LowerBlit, (FARPROC)extSDL_LowerBlit},
    {HOOK_IAT_CANDIDATE, 0, "SDL_BlitSurface", (FARPROC)NULL, (FARPROC *) &pSDL_BlitSurface, (FARPROC)extSDL_BlitSurface},
    {HOOK_IAT_CANDIDATE, 0, "SDL_BlitScaled", (FARPROC)NULL, (FARPROC *) &pSDL_BlitScaled, (FARPROC)extSDL_BlitScaled},
    {HOOK_IAT_CANDIDATE, 0, "SDL_SoftStretch", (FARPROC)NULL, (FARPROC *) &pSDL_SoftStretch, (FARPROC)extSDL_SoftStretch},
    {HOOK_IAT_CANDIDATE, 0, "SDL_UpdateRects", (FARPROC)NULL, (FARPROC *) &pSDL_UpdateRects, (FARPROC)extSDL_UpdateRects},
    {HOOK_IAT_CANDIDATE, 0, "SDL_SetPalette", (FARPROC)NULL, (FARPROC *) &pSDL_SetPalette, (FARPROC)extSDL_SetPalette},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GetVideoSurface", (FARPROC)NULL, (FARPROC *) &pSDL_GetVideoSurface, (FARPROC)extSDL_GetVideoSurface},
    {HOOK_IAT_CANDIDATE, 0, "SDL_SetAlpha", (FARPROC)NULL, (FARPROC *) &pSDL_SetAlpha, (FARPROC)extSDL_SetAlpha},
    {HOOK_IAT_CANDIDATE, 0, "SDL_SetColorKey", (FARPROC)NULL, (FARPROC *) &pSDL_SetColorKey, (FARPROC)extSDL_SetColorKey},
    {HOOK_IAT_CANDIDATE, 0, "SDL_SetColors", (FARPROC)NULL, (FARPROC *) &pSDL_SetColors, (FARPROC)extSDL_SetColors},
    {HOOK_IAT_CANDIDATE, 0, "SDL_WarpMouse", (FARPROC)NULL, (FARPROC *) &pSDL_WarpMouse, (FARPROC)extSDL_WarpMouse},
    {HOOK_IAT_CANDIDATE, 0, "SDL_CreateYUVOverlay", (FARPROC)NULL, (FARPROC *) &pSDL_CreateYUVOverlay, (FARPROC)extSDL_CreateYUVOverlay},
    {HOOK_IAT_CANDIDATE, 0, "SDL_DisplayYUVOverlay", (FARPROC)NULL, (FARPROC *) &pSDL_DisplayYUVOverlay, (FARPROC)extSDL_DisplayYUVOverlay},
    {HOOK_IAT_CANDIDATE, 0, "SDL_PollEvent", (FARPROC)NULL, (FARPROC *) &pSDL_PollEvent, (FARPROC)extSDL_PollEvent},
    {HOOK_IAT_CANDIDATE, 0, "SDL_PushEvent", (FARPROC)NULL, (FARPROC *) &pSDL_PushEvent, (FARPROC)extSDL_PushEvent},
    {HOOK_IAT_CANDIDATE, 0, "SDL_ShowCursor", (FARPROC)NULL, (FARPROC *) &pSDL_ShowCursor, (FARPROC)extSDL_ShowCursor},
    // SDL2
    {HOOK_IAT_CANDIDATE, 0, "SDL_CreateWindow", (FARPROC)NULL, (FARPROC *) &pSDL_CreateWindow, (FARPROC)extSDL_CreateWindow},
    {HOOK_IAT_CANDIDATE, 0, "SDL_SetWindowPosition", (FARPROC)NULL, (FARPROC *) &pSDL_SetWindowPosition, (FARPROC)extSDL_SetWindowPosition},
    {HOOK_IAT_CANDIDATE, 0, "SDL_SetWindowSize", (FARPROC)NULL, (FARPROC *) &pSDL_SetWindowSize, (FARPROC)extSDL_SetWindowSize},
    {HOOK_IAT_CANDIDATE, 0, "SDL_CreateRenderer", (FARPROC)NULL, (FARPROC *) &pSDL_CreateRenderer, (FARPROC)extSDL_CreateRenderer},
    {HOOK_IAT_CANDIDATE, 0, "SDL_CreateSoftwareRenderer", (FARPROC)NULL, (FARPROC *) &pSDL_CreateSoftwareRenderer, (FARPROC)extSDL_CreateSoftwareRenderer},
    {HOOK_IAT_CANDIDATE, 0, "SDL_SetWindowFullscreen", (FARPROC)NULL, (FARPROC *) &pSDL_SetWindowFullscreen, (FARPROC)extSDL_SetWindowFullscreen},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GetWindowWMInfo", (FARPROC)NULL, (FARPROC *) &pSDL_GetWindowWMInfo, (FARPROC)extSDL_GetWindowWMInfo},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GetCurrentDisplayMode", (FARPROC)NULL, (FARPROC *) &pSDL_GetCurrentDisplayMode, (FARPROC)extSDL_GetCurrentDisplayMode},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GetDesktopDisplayMode", (FARPROC)NULL, (FARPROC *) &pSDL_GetDesktopDisplayMode, (FARPROC)extSDL_GetDesktopDisplayMode},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GetWindowSize", (FARPROC)NULL, (FARPROC *) &pSDL_GetWindowSize, (FARPROC)extSDL_GetWindowSize},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GetDisplayMode", (FARPROC)NULL, (FARPROC *) &pSDL_GetDisplayMode, (FARPROC)extSDL_GetDisplayMode},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GetDisplayBounds", (FARPROC)NULL, (FARPROC *) &pSDL_GetDisplayBounds, (FARPROC)extSDL_GetDisplayBounds},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GetDisplayUsableBounds", (FARPROC)NULL, (FARPROC *) &pSDL_GetDisplayUsableBounds, (FARPROC)extSDL_GetDisplayUsableBounds},
    {HOOK_IAT_CANDIDATE, 0, "SDL_SetWindowDisplayMode", (FARPROC)NULL, (FARPROC *) &pSDL_SetWindowDisplayMode, (FARPROC)extSDL_SetWindowDisplayMode},
    //{HOOK_IAT_CANDIDATE, 0, "SDL_ListModes", (FARPROC)NULL, (FARPROC *)&pSDL_ListModes, (FARPROC)extSDL_ListModes},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type SDL1Hooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "SDL_PeepEvents(1)", (FARPROC)NULL, (FARPROC *) &pSDL_PeepEvents1, (FARPROC)extSDL_PeepEvents1},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type SDL2Hooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "SDL_PeepEvents(2)", (FARPROC)NULL, (FARPROC *) &pSDL_PeepEvents2, (FARPROC)extSDL_PeepEvents2},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type TraceHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "SDL_FreeYUVOverlay", (FARPROC)NULL, (FARPROC *) &pSDL_FreeYUVOverlay, (FARPROC)extSDL_FreeYUVOverlay},
    {HOOK_IAT_CANDIDATE, 0, "SDL_LockYUVOverlay", (FARPROC)NULL, (FARPROC *) &pSDL_LockYUVOverlay, (FARPROC)extSDL_LockYUVOverlay},
    {HOOK_IAT_CANDIDATE, 0, "SDL_UnlockYUVOverlay", (FARPROC)NULL, (FARPROC *) &pSDL_UnlockYUVOverlay, (FARPROC)extSDL_UnlockYUVOverlay},
    {HOOK_IAT_CANDIDATE, 0, "SDL_FillRect", (FARPROC)NULL, (FARPROC *) &pSDL_FillRect, (FARPROC)extSDL_FillRect},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GetClipRect", (FARPROC)NULL, (FARPROC *) &pSDL_GetClipRect, (FARPROC)extSDL_GetClipRect},
    {HOOK_IAT_CANDIDATE, 0, "SDL_SetClipRect", (FARPROC)NULL, (FARPROC *) &pSDL_SetClipRect, (FARPROC)extSDL_SetClipRect},
    {HOOK_IAT_CANDIDATE, 0, "SDL_CreateRGBSurface", (FARPROC)NULL, (FARPROC *) &pSDL_CreateRGBSurface, (FARPROC)extSDL_CreateRGBSurface},
    {HOOK_IAT_CANDIDATE, 0, "SDL_CreateRGBSurfaceFrom", (FARPROC)NULL, (FARPROC *) &pSDL_CreateRGBSurfaceFrom, (FARPROC)extSDL_CreateRGBSurfaceFrom},
    {HOOK_IAT_CANDIDATE, 0, "SDL_ConvertSurface", (FARPROC)NULL, (FARPROC *) &pSDL_ConvertSurface, (FARPROC)extSDL_ConvertSurface},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GetVideoInfo", (FARPROC)NULL, (FARPROC *) &pSDL_GetVideoInfo, (FARPROC)extSDL_GetVideoInfo},
    {HOOK_IAT_CANDIDATE, 0, "SDL_CreateCursor", (FARPROC)NULL, (FARPROC *) &pSDL_CreateCursor, (FARPROC)extSDL_CreateCursor},
    {HOOK_IAT_CANDIDATE, 0, "SDL_FreeCursor", (FARPROC)NULL, (FARPROC *) &pSDL_FreeCursor, (FARPROC)extSDL_FreeCursor},
    {HOOK_IAT_CANDIDATE, 0, "SDL_DisplayFormatAlpha", (FARPROC)NULL, (FARPROC *) &pSDL_DisplayFormatAlpha, (FARPROC)extSDL_DisplayFormatAlpha},
    {HOOK_IAT_CANDIDATE, 0, "SDL_ListModes", (FARPROC)NULL, (FARPROC *) &pSDL_ListModes, (FARPROC)extSDL_ListModes},
    // SDL2
    {HOOK_IAT_CANDIDATE, 0, "SDL_CreateTexture", (FARPROC)NULL, (FARPROC *) &pSDL_CreateTexture, (FARPROC)extSDL_CreateTexture},
    {HOOK_IAT_CANDIDATE, 0, "SDL_UpdateTexture", (FARPROC)NULL, (FARPROC *) &pSDL_UpdateTexture, (FARPROC)extSDL_UpdateTexture},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GL_SwapWindow", (FARPROC)NULL, (FARPROC *) &pSDL_GL_SwapWindow, (FARPROC)extSDL_GL_SwapWindow},
    {HOOK_IAT_CANDIDATE, 0, "SDL_SetWindowBrightness", (FARPROC)NULL, (FARPROC *) &pSDL_SetWindowBrightness, (FARPROC)extSDL_SetWindowBrightness},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GL_SetSwapInterval", (FARPROC)NULL, (FARPROC *) &pSDL_GL_SetSwapInterval, (FARPROC)extSDL_GL_SetSwapInterval},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GL_CreateContext", (FARPROC)NULL, (FARPROC *) &pSDL_GL_CreateContext, (FARPROC)extSDL_GL_CreateContext},
    {HOOK_IAT_CANDIDATE, 0, "SDL_SetEventFilter", (FARPROC)NULL, (FARPROC *) &pSDL_SetEventFilter, (FARPROC)extSDL_SetEventFilter},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type TimeHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "SDL_Delay", (FARPROC)NULL, (FARPROC *) &pSDL_Delay, (FARPROC)extSDL_Delay},
    {HOOK_IAT_CANDIDATE, 0, "SDL_GetTicks", (FARPROC)NULL, (FARPROC *) &pSDL_GetTicks, (FARPROC)extSDL_GetTicks},
    {HOOK_IAT_CANDIDATE, 0, "SDL_WaitEventTimeout", (FARPROC)NULL, (FARPROC *) &pSDL_WaitEventTimeout, (FARPROC)extSDL_WaitEventTimeout},
    {HOOK_IAT_CANDIDATE, 0, "SDL_CondWaitTimeout", (FARPROC)NULL, (FARPROC *) &pSDL_CondWaitTimeout, (FARPROC)extSDL_CondWaitTimeout},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type JoystickHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "SDL_NumJoysticks", (FARPROC)NULL, (FARPROC *) &pSDL_NumJoysticks, (FARPROC)extSDL_NumJoysticks},
    {HOOK_IAT_CANDIDATE, 0, "SDL_JoystickName", (FARPROC)NULL, (FARPROC *) &pSDL_JoystickName, (FARPROC)extSDL_JoystickName},
    {HOOK_IAT_CANDIDATE, 0, "SDL_JoystickNameForIndex", (FARPROC)NULL, (FARPROC *) &pSDL_JoystickNameForIndex, (FARPROC)extSDL_JoystickNameForIndex},
    {HOOK_IAT_CANDIDATE, 0, "SDL_JoystickOpen", (FARPROC)NULL, (FARPROC *) &pSDL_JoystickOpen, (FARPROC)extSDL_JoystickOpen},
    {HOOK_IAT_CANDIDATE, 0, "SDL_JoystickNumAxes", (FARPROC)NULL, (FARPROC *) &pSDL_JoystickNumAxes, (FARPROC)extSDL_JoystickNumAxes},
    {HOOK_IAT_CANDIDATE, 0, "SDL_JoystickNumButtons", (FARPROC)NULL, (FARPROC *) &pSDL_JoystickNumButtons, (FARPROC)extSDL_JoystickNumButtons},
    {HOOK_IAT_CANDIDATE, 0, "SDL_JoystickNumBalls", (FARPROC)NULL, (FARPROC *) &pSDL_JoystickNumBalls, (FARPROC)extSDL_JoystickNumBalls},
    {HOOK_IAT_CANDIDATE, 0, "SDL_JoystickGetAxis", (FARPROC)NULL, (FARPROC *) &pSDL_JoystickGetAxis, (FARPROC)extSDL_JoystickGetAxis},
    {HOOK_IAT_CANDIDATE, 0, "SDL_JoystickUpdate", (FARPROC)NULL, (FARPROC *) &pSDL_JoystickUpdate, (FARPROC)extSDL_JoystickUpdate},
    {HOOK_IAT_CANDIDATE, 0, "SDL_JoystickGetButton", (FARPROC)NULL, (FARPROC *) &pSDL_JoystickGetButton, (FARPROC)extSDL_JoystickGetButton},
    {HOOK_IAT_CANDIDATE, 0, "SDL_JoystickGetHat", (FARPROC)NULL, (FARPROC *) &pSDL_JoystickGetHat, (FARPROC)extSDL_JoystickGetHat},
    {HOOK_IAT_CANDIDATE, 0, "SDL_JoystickClose", (FARPROC)NULL, (FARPROC *) &pSDL_JoystickClose, (FARPROC)extSDL_JoystickClose},
    {HOOK_IAT_CANDIDATE, 0, "SDL_JoystickEventState", (FARPROC)NULL, (FARPROC *) &pSDL_JoystickEventState, (FARPROC)extSDL_JoystickEventState},
    {HOOK_IAT_CANDIDATE, 0, "SDL_JoystickOpened", (FARPROC)NULL, (FARPROC *) &pSDL_JoystickOpened, (FARPROC)extSDL_JoystickOpened},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

// ------------------------------------------------------------------------------------------
// hookers ...
// ------------------------------------------------------------------------------------------

static char *libname = "sdl.dll";

void HookSDL(HMODULE module) {
    //if(dxw.dwFlags9 & (SDLEMULATION | SDLFORCESTRETCH)) bEmulation = TRUE;
    if(dxw.dwFlags9 & SDLEMULATION) bEmulation = TRUE;
    if(dxw.dwFlags9 & HOOKSDL2LIB) libname = "sdl2.dll";
    HookLibraryEx(module, Hooks, libname);
    if(dxw.dwFlags9 & HOOKSDL2LIB)
        HookLibraryEx(module, SDL2Hooks, libname);
    else
        HookLibraryEx(module, SDL1Hooks, libname);
    if((dxw.dwFlags9 & HIDEJOYSTICKS) || (dxw.dwFlags6 & VIRTUALJOYSTICK)) HookLibraryEx(module, JoystickHooks, libname);
    if(dxw.dwFlags2 & TIMESTRETCH) HookLibraryEx(module, TimeHooks, libname);
    if(dxw.dwTFlags & OUTSDLTRACE) HookLibraryEx(module, TraceHooks, libname);
    HMODULE sdl;
    sdl = (*pLoadLibraryA)(libname);
#ifndef DXW_NOTRACES
    if(sdl) {
        if(!pSDL_GetError) pSDL_GetError = (SDL_GetError_Type)(*pGetProcAddress)(sdl, "SDL_GetError");
        if(!pSDL_GetError) OutTraceE("HookSDL: GetProcAddress ERROR err=%d at %d\n", GetLastError(), __LINE__);
    } else
        OutTraceE("HookSDL: LoadLibrary ERROR err=%d at %d\n", GetLastError(), __LINE__);
#endif // DXW_NOTRACES
    if(sdl && bEmulation) {
        pStretch = pSDL_SoftStretch;
        //if(1) pStretch = intSDL_BlitScaled; // gho: to test internal bilinear filter ....
        if(!pStretch) pStretch = (SDL_BlitSurface_Type)(*pGetProcAddress)(sdl, "SDL_BlitScaled");
        if(!pStretch) pStretch = (SDL_BlitSurface_Type)(*pGetProcAddress)(sdl, "SDL_SoftStretch");
        if(!pStretch) pStretch = intSDL_BlitScaled; // to test ....
#ifndef DXW_NOTRACES
        if(!pStretch) OutTraceSDL("HookSDL: no pStretch\n");
#endif // DXW_NOTRACES
        if(!pSDL_CreateRGBSurface) pSDL_CreateRGBSurface = (SDL_CreateRGBSurface_Type)(*pGetProcAddress)(sdl, "SDL_CreateRGBSurface");
#ifndef DXW_NOTRACES
        if(!pSDL_CreateRGBSurface) OutTraceSDL("HookSDL: no pSDL_CreateRGBSurface\n");
#endif // DXW_NOTRACES
        if(!pSDL_FreeSurface) pSDL_FreeSurface = (SDL_FreeSurface_Type)(*pGetProcAddress)(sdl, "SDL_FreeSurface");
#ifndef DXW_NOTRACES
        if(!pSDL_FreeSurface) OutTraceSDL("HookSDL: no pSDL_FreeSurface\n");
#endif // DXW_NOTRACES
    }
    pSDL_GetWMInfo = (SDL_GetWMInfo_Type)(*pGetProcAddress)(sdl, "SDL_GetWMInfo");
    if(sdl)(*pFreeLibrary)(sdl);
}

FARPROC Remap_SDL_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if(addr = RemapLibraryEx(proc, hModule, Hooks)) return addr;
    if(dxw.dwFlags9 & HOOKSDL2LIB) {
        if(addr = RemapLibraryEx(proc, hModule, SDL2Hooks)) return addr;
    } else {
        if(addr = RemapLibraryEx(proc, hModule, SDL1Hooks)) return addr;
    }
    if((dxw.dwFlags9 & HIDEJOYSTICKS) || (dxw.dwFlags6 & VIRTUALJOYSTICK)) if(addr = RemapLibraryEx(proc, hModule, JoystickHooks)) return addr;
    if(dxw.dwFlags2 & TIMESTRETCH) if(addr = RemapLibraryEx(proc, hModule, TimeHooks)) return addr;
    if(dxw.dwTFlags & OUTSDLTRACE) if(addr = RemapLibraryEx(proc, hModule, TraceHooks)) return addr;
    return NULL;
}

// ------------------------------------------------------------------------------------------
// Static internal functions, utilities and so on ...
// ------------------------------------------------------------------------------------------

static char *intSDL_ErrorMessage() {
    if(pSDL_GetError) return(*pSDL_GetError)();
    return "unknown";
}

#ifndef DXW_NOTRACES
static char *ExplainInitFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "SDL_INIT_");
    if (c & SDL_INIT_TIMER) strcat(eb, "TIMER+");
    if (c & SDL_INIT_AUDIO) strcat(eb, "AUDIO+");
    if (c & SDL_INIT_VIDEO) strcat(eb, "VIDEO+");
    if (c & SDL_INIT_CDROM) strcat(eb, "CDROM+");
    if (c & SDL_INIT_JOYSTICK) strcat(eb, "JOYSTICK+");
    if (c & SDL_INIT_NOPARACHUTE) strcat(eb, "NOPARACHUTE+");
    if (c & SDL_INIT_EVENTTHREAD) strcat(eb, "EVENTTHREAD+");
    l = strlen(eb);
    if (l > strlen("SDL_INIT_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

static char *ExplainSDLSurfaceFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "SDL_");
    if (c & SDL_HWSURFACE)		strcat(eb, "HWSURFACE+");
    else						strcat(eb, "SWSURFACE+"); // note: SDL_SWSURFACE is 0!
    if (c & SDL_ASYNCBLIT)		strcat(eb, "ASYNCBLIT+");
    if (c & SDL_ANYFORMAT)		strcat(eb, "ANYFORMAT+");
    if (c & SDL_HWPALETTE)		strcat(eb, "HWPALETTE+");
    if (c & SDL_DOUBLEBUF)		strcat(eb, "DOUBLEBUF+");
    if (c & SDL_FULLSCREEN)		strcat(eb, "FULLSCREEN+");
    if (c & SDL_OPENGL)			strcat(eb, "OPENGL+");
    if (c & SDL_OPENGLBLIT)		strcat(eb, "OPENGLBLIT+");
    if (c & SDL_RESIZABLE)		strcat(eb, "RESIZABLE+");
    if (c & SDL_NOFRAME)		strcat(eb, "NOFRAME+");
    if (c & SDL_HWACCEL)		strcat(eb, "HWACCEL+");
    if (c & SDL_SRCCOLORKEY)	strcat(eb, "SRCCOLORKEY+");
    if (c & SDL_RLEACCEL)		strcat(eb, "RLEACCEL+");
    if (c & SDL_SRCALPHA)		strcat(eb, "SRCALPHA+");
    if (c & SDL_PREALLOC)		strcat(eb, "PREALLOC+");
    l = strlen(eb);
    if (l > strlen("SDL_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

static char *sState(int state) {
    char *s;
    switch(state) {
    case SDL_QUERY:
        s = "QUERY";
        break;
    case SDL_IGNORE:
        s = "IGNORE";
        break;
    case SDL_ENABLE:
        s = "ENABLE";
        break;
    default:
        s = "invalid";
        break;
    }
    return s;
}

char *sRect(const SDL_Rect *rect) {
    static char sRectBuf[80];
    if(rect)
        sprintf(sRectBuf, "{pos=(%d,%d) siz=(%dx%d)}", rect->x, rect->y, rect->w, rect->h);
    else
        sprintf(sRectBuf, "{NULL}");
    return sRectBuf;
}

static char *intSDL_PixelFormat(const SDL_PixelFormat *format, char *sbuf, int len) {
#define MAXSDLPIXFMTBUFLEN 200 // v2.05.44: enlarged for "Blob Wars" requirements
    if(format) {
        sprintf_s(sbuf, len - 1,
                  "{%sbpp=%d Bpp=%d RGBAloss=(%d,%d,%d,%d) RGBAshift=(%d,%d,%d,%d) RGBAmask=(%#x,%#x,%#x,%#x) colorkey=%#x alpha=%#x}",
                  format->palette ? "palette " : "",
                  format->BitsPerPixel, format->BytesPerPixel,
                  format->Rloss, format->Gloss, format->Bloss, format->Aloss,
                  format->Rshift, format->Gshift, format->Bshift, format->Ashift,
                  format->Rmask, format->Gmask, format->Bmask, format->Amask,
                  format->colorkey, format->alpha);
    } else
        strcpy_s(sbuf, len, "{NULL}");
    sbuf[len - 1] = 0; // terminator
    return sbuf;
}

static char *intEventCaption(Uint8 type) {
    char *s;
    switch(type) {
    case SDL_NOEVENT:
        s = "NOEVENT";
        break;
    case SDL_ACTIVEEVENT:
        s = "ACTIVEEVENT";
        break;
    case SDL_KEYDOWN:
        s = "KEYDOWN";
        break;
    case SDL_KEYUP:
        s = "KEYUP";
        break;
    case SDL_MOUSEMOTION:
        s = "MOUSEMOTION";
        break;
    case SDL_MOUSEBUTTONDOWN:
        s = "MOUSEBUTTONDOWN";
        break;
    case SDL_MOUSEBUTTONUP:
        s = "MOUSEBUTTONUP";
        break;
    case SDL_JOYAXISMOTION:
        s = "JOYAXISMOTION";
        break;
    case SDL_JOYBALLMOTION:
        s = "JOYBALLMOTION";
        break;
    case SDL_JOYHATMOTION:
        s = "JOYHATMOTION";
        break;
    case SDL_JOYBUTTONDOWN:
        s = "JOYBUTTONDOWN";
        break;
    case SDL_JOYBUTTONUP:
        s = "JOYBUTTONUP";
        break;
    case SDL_QUIT:
        s = "QUIT";
        break;
    case SDL_SYSWMEVENT:
        s = "SYSWMEVENT";
        break;
    case SDL_EVENT_RESERVEDA:
        s = "EVENT_RESERVEDA";
        break;
    case SDL_EVENT_RESERVEDB:
        s = "EVENT_RESERVEDB";
        break;
    case SDL_VIDEORESIZE:
        s = "VIDEORESIZE";
        break;
    case SDL_VIDEOEXPOSE:
        s = "VIDEOEXPOSE";
        break;
    default:
        s = "unknown";
        break;
    }
    return s;
}

void TraceEvent(char *label, SDL_Event *e) {
    switch(e->type) {
    case SDL_MOUSEMOTION:
        OutDebugSDL("%s: %#x(%s)={state=%#x pos=(%d,%d) rel=(%d,%d)}\n",
                    label, e->motion.type, intEventCaption(e->type),
                    e->motion.state, e->motion.x, e->motion.y, e->motion.xrel, e->motion.yrel);
        break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        OutDebugSDL("%s: %#x(%s)={button=%#x state=%#x pos=(%d,%d)}\n",
                    label, e->button.type, intEventCaption(e->type),
                    e->button.button, e->button.state, e->button.x, e->button.y);
        break;
    default:
        OutDebugSDL("%s: %#x(%s)\n",
                    label, e->button.type, intEventCaption(e->type));
    }
}

char *sActionType(SDL_eventaction act) {
    switch(act) {
    case SDL_ADDEVENT:
        return "ADDEVENT";
        break;
    case SDL_PEEKEVENT:
        return "PEEKEVENT";
        break;
    case SDL_GETEVENT:
        return "GETEVENT";
        break;
    }
    return "???";
}
#endif // DXW_NOTRACES

SDL_Rect intSDL_CompleteRect (SDL_Surface *src, const SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
#ifdef _SDL_DEBUG
    OutTrace("CompleteRect srcrect=%s\n", sRect(srcrect));
    OutTrace("CompleteRect dstrect=%s\n", sRect(dstrect));
#endif
    SDL_Rect CompleteRect;
    if(srcrect == NULL) {
        CompleteRect.x = CompleteRect.y = 0;
        CompleteRect.w = src->w;
        CompleteRect.h = src->h;
    } else
        CompleteRect = *srcrect;
    if(dstrect) {
        CompleteRect.x += mod(dstrect->x);
        CompleteRect.y += mod(dstrect->y);
    }
    // trim
    if((CompleteRect.x + CompleteRect.w) > dst->w) CompleteRect.w = dst->w - CompleteRect.x;
    if((CompleteRect.y + CompleteRect.h) > dst->h) CompleteRect.h = dst->h - CompleteRect.y;
#ifdef _SDL_DEBUG
    OutTrace("CompleteRect=%s\n", sRect(&CompleteRect));
#endif
    return CompleteRect;
}

void intSDL_ShowOverlay() {
    // v2.05.44: add FPS overlay, but blit only when rect is NULL, that is
    // it is a full window update.
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    if (pSDL_GetWMInfo && ((*pSDL_GetWMInfo)(&info) >= 0)) {
        HWND hwnd;
        RECT rect;
        hwnd = info.window;
        (*pGetClientRect)(hwnd, &rect);
        HDC hdc = (*pGDIGetDC)(hwnd);
        dxw.ShowOverlay(hdc, rect.right, rect.bottom);
        (*pGDIReleaseDC)(hwnd, hdc);
    }
}

SDL_Rect intSDL_ScaleRect (SDL_Surface *surface, const SDL_Rect *rect) {
    SDL_Rect ScaledRect;
    RECT wrect;
    int width, height;
    OutDebugSDL("ScaledRect rect=%s\n", sRect(rect));
    if(rect == NULL) {
        ScaledRect.x = ScaledRect.y = 0;
        ScaledRect.w = surface->w;
        ScaledRect.h = surface->h;
    } else
        ScaledRect = *rect;
    (*pGetClientRect)(dxw.GethWnd(), &wrect);
    width = (Uint16)(wrect.right - wrect.left);
    height = (Uint16)(wrect.bottom - wrect.top);
    ScaledRect.x = (ScaledRect.x * width) / surface->w;
    ScaledRect.y = (ScaledRect.y * height) / surface->h;
    ScaledRect.w = (ScaledRect.w * width) / surface->w;
    ScaledRect.h = (ScaledRect.h * height) / surface->h;
    // trim
    if(ScaledRect.x > width) ScaledRect.x = width;
    if(ScaledRect.y > height) ScaledRect.x = height;
    if((ScaledRect.x + ScaledRect.w) >= width) ScaledRect.w = width - ScaledRect.x - 1;
    if((ScaledRect.y + ScaledRect.h) >= height) ScaledRect.h = height - ScaledRect.y - 1;
    OutDebugSDL("ScaledRect scaled=%s\n", sRect(&ScaledRect));
    //ScaledRect.w = -ScaledRect.w;
    //ScaledRect.h = -ScaledRect.h;
    return ScaledRect;
}

int intSDL_Refresh(char *apiname, const SDL_Rect *rect) {
    int ret;
    SDL_Rect r;
    SDL_Rect *dstrect = NULL;
    if(dxw.HandleFPS()) return 0;
    if (rect) {
        r = intSDL_ScaleRect(gSDL_EmulatedVideo, rect);
        // v2.04.43: skip operation when width or height is zero. Fixes "BosWars" crash.
        //if((r.h == 0) || (r.w == 0)) return 0;
        if(r.h == 0) r.h = 1;
        if(r.w == 0) r.w = 1;
        dstrect = &r;
    }
    if(ret = (*pStretch)(gSDL_EmulatedVideo, rect, gSDL_VideoSurface, dstrect)) {
        OutTraceE("%s: StretchBlt ERROR err=%#x(\"%s\") at %d\n", apiname, ret, intSDL_ErrorMessage(), __LINE__);
        OutTraceE("> src=%#x srcrect=%s\n", gSDL_EmulatedVideo, sRect(rect));
        OutTraceE("> dst=%#x dstrect=%s\n", gSDL_VideoSurface, sRect(rect));
    }
    intSDL_ShowOverlay();
    return ret;
}

// SDLScreenRefresh: used to resize main window

void SDLScreenRefresh() {
    if(pStretch && gSDL_EmulatedVideo && gSDL_VideoSurface) {
        RECT wrect;
        int ret;
        int height, width, bpp;
        // first, you have to destroy and rebuild the video
        (*pGetClientRect)(dxw.GethWnd(), &wrect);
        width = (Uint16)(wrect.right - wrect.left);
        height = (Uint16)(wrect.bottom - wrect.top);
        bpp = gSDL_VideoSurface->format->BitsPerPixel;
        (*pSDL_FreeSurface)(gSDL_VideoSurface);
        gSDL_VideoSurface = (*pSDL_SetVideoMode)(width, height, bpp, gVideoFlags);
        if(ret = (*pStretch)(gSDL_EmulatedVideo, NULL, gSDL_VideoSurface, NULL))
            OutTraceE("SDLScreenRefresh: StretchBlt ERROR err=%#x(\"%s\") at %d\n", ret, intSDL_ErrorMessage(), __LINE__);
    }
}

#ifndef abs
#define abs(x) ((x) > 0 ? (x) : -(x))
#endif

static int intSDL_BlitScaled(SDL_Surface *src, const SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
    RECT srect, drect;
    typedef void (WINAPI * Resize_HQ_Type)( unsigned char *, RECT *, int, unsigned char *, RECT *, int);
    static Resize_HQ_Type pResize_HQ = NULL;
    // do the filtering
    if(!pResize_HQ) {
        char *filter = NULL;
        HMODULE filterlib;
        switch(src->format->Gmask) {
        //default:
        case 0x00FF00:
            filter = "Resize_HQ_4ch";
            break;
        case 0x0007E0: // RGB565
            filter = "Resize_HQ_2ch565";
            break;
        case 0x0003E0: // RGB555
            filter = "Resize_HQ_2ch555";
            break;
        }
        if(!filter) {
            OutTraceE("intSDL_BlitScaled: unsupported depth=%d mask=%#x.%#x.%#x\n",
                      src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask);
            return -1;
        }
        filterlib = (*pLoadLibraryA)("filter.dll");
        if(!filterlib) {
            char sMsg[80 + 1];
            sprintf(sMsg, "DXWND: ERROR can't load lib=\"filter.dll\" err=%#x\n", GetLastError());
            OutTraceE(sMsg);
            MessageBox(0, sMsg, "ERROR", MB_OK | MB_ICONEXCLAMATION);
            exit(0);
        }
        pResize_HQ = (Resize_HQ_Type)(*pGetProcAddress)(filterlib, filter);
        if(!pResize_HQ) {
            char sMsg[80 + 1];
            sprintf(sMsg, "DXWND: ERROR can't load name=\"%s\"\n", filter);
            OutTraceE(sMsg);
            MessageBox(0, sMsg, "ERROR", MB_OK | MB_ICONEXCLAMATION);
            exit(0);
        }
    }
    if(srcrect) {
        srect.left = srcrect->x;
        srect.top = srcrect->y;
        srect.right = srcrect->x + abs(srcrect->w);
        srect.bottom = srcrect->y + abs(srcrect->h);
    } else {
        srect.left = 0;
        srect.top = 0;
        srect.right = src->w;
        srect.bottom = src->h;
    }
    if(dstrect) {
        drect.left = dstrect->x;
        drect.top = dstrect->y;
        drect.right = dstrect->x + abs(dstrect->w);
        drect.bottom = dstrect->y + abs(dstrect->h);
    } else {
        drect.left = 0;
        drect.top = 0;
        drect.right = dst->w;
        drect.bottom = dst->h;
    }
    OutDebugSDL("Resize_HQ: src=%#x rect=(%d,%d)-(%d,%d) dst=%#x rect=(%d,%d)-(%d,%d)\n",
                src, srect.left, srect.top, srect.right, srect.bottom,
                dst, drect.left, drect.top, drect.right, drect.bottom);
    (*pResize_HQ)((unsigned char *)src->pixels, &srect, src->pitch, (unsigned char *)dst->pixels, &drect, dst->pitch);
    return 0;
}

void dxwSDLSethWnd(char *api, SDL_Window *window) {
    SDL_SysWMinfo wmInfo;
    OutTraceSDL("%s: SDL SethWnd\n", api);
    dxw.SetFullScreen(TRUE);
    gSDL_MainWindow = window;
    if(pSDL_SetWindowPosition)(*pSDL_SetWindowPosition)(window, dxw.iPosX, dxw.iPosY);
    if(pSDL_SetWindowSize)(*pSDL_SetWindowSize)(window, dxw.iSizX, dxw.iSizY);
    if(pSDL_GetWindowWMInfo) {
        SDL_VERSION(&wmInfo.version);
        (*pSDL_GetWindowWMInfo)(window, &wmInfo);
        dxw.SethWnd(wmInfo.window);
    }
}

// ------------------------------------------------------------------------------------------
// SDL wrappers
// ------------------------------------------------------------------------------------------

int extSDL_Init(Uint32 flags) {
    int ret;
    ApiName("SDL_Init");
    OutTraceSDL("%s: flags=%#x(%s)\n", ApiRef, flags, ExplainInitFlags(flags));
    if((dxw.dwFlags6 & VIRTUALJOYSTICK) && (flags & SDL_INIT_JOYSTICK)) {
        OutTraceSDL("SDL_Init: virtual joystick SUPPRESS SDL_INIT_JOYSTICK\n");
        flags &= ~SDL_INIT_JOYSTICK;
        if(flags == 0) return 0;
    }
    ret = (*pSDL_Init)(flags);
    IfSDLError(ret);
    return ret;
}

int extSDL_InitSubSystem(Uint32 flags) {
    int ret;
    ApiName("SDL_InitSubSystem");
    OutTraceSDL("%s: flags=%#x(%s)\n", ApiRef, flags, ExplainInitFlags(flags));
    if((dxw.dwFlags6 & VIRTUALJOYSTICK) && (flags & SDL_INIT_JOYSTICK)) {
        OutTraceSDL("%s: virtual joystick SUPPRESS SDL_INIT_JOYSTICK\n", ApiRef);
        flags &= ~SDL_INIT_JOYSTICK;
        if(flags == 0) return 0;
    }
    ret = (*pSDL_InitSubSystem)(flags);
    IfSDLError(ret);
    return ret;
}

void extSDL_Quit(void) {
    ApiName("SDL_Quit");
    OutTraceSDL("%s\n", ApiRef);
    (*pSDL_Quit)();
}

int extSDL_QuitSubSystem(Uint32 flags) {
    int ret;
    ApiName("SDL_QuitSubSystem");
    OutTraceSDL("%s: flags=%#x(%s)\n", ApiRef, flags, ExplainInitFlags(flags));
    ret = (*pSDL_QuitSubSystem)(flags);
    IfSDLError(ret);
    return ret;
}

SDL_Surface *extSDL_SetVideoMode(int width, int height, int bpp, Uint32 flags) {
    ApiName("SDL_SetVideoMode");
    SDL_Surface *ret;
    BOOL bMove = FALSE;
    int oWidth = width;
    int oHeight = height;
    OutTraceSDL("%s: size=(%dx%d) bpp=%d flags=%#x(%s)\n", ApiRef, width, height, bpp, flags, ExplainSDLSurfaceFlags(flags));
    if((dxw.Windowize && (flags & SDL_FULLSCREEN)) ||
            (dxw.dwFlags9 & SDLFORCESTRETCH)) {
        bMove = TRUE;
        dxw.SetScreenSize(width, height);
        flags &= ~SDL_FULLSCREEN;
        switch(dxw.Coordinates) {
        case DXW_SET_COORDINATES:
        case DXW_DESKTOP_CENTER:
            switch(dxw.WindowStyle) {
            case WSTYLE_DEFAULT:
                break;
            case WSTYLE_MODALSTYLE:
                flags &= ~SDL_RESIZABLE;
                flags |=  SDL_NOFRAME;
                break;
            case WSTYLE_THICKFRAME:
            case WSTYLE_THINFRAME:
                flags &= ~SDL_NOFRAME;
                flags |=  SDL_RESIZABLE;
                break;
            }
            break;
        case DXW_DESKTOP_WORKAREA:
        case DXW_DESKTOP_FULL:
            flags &= ~SDL_RESIZABLE;
            flags |=  SDL_NOFRAME;
            break;
        }
        if(bEmulation) {
            width = dxw.iSizX;
            height = dxw.iSizY;
            if(bpp == 0) bpp = 32; // experimental for emulation mode, should get desktop color depth?
            flags &= ~SDL_HWPALETTE;
            // flags &= ~SDL_DOUBLEBUF; // experimental ...
            flags &= ~SDL_HWSURFACE; // experimental for DOSBox
            flags |=  SDL_SWSURFACE; // experimental for DOSBox
        }
        OutTraceSDL("%s: FIXED size=(%dx%d) bpp=%d flags=%#x(%s)\n", ApiRef, width, height, bpp, flags, ExplainSDLSurfaceFlags(flags));
    }
    gVideoFlags = flags; // store for later use
    if(gSDL_VideoSurface) { // experimental for DOSBox ....
        OutTraceSDL("%s: mapping current surface=%#x\n", ApiRef, gSDL_VideoSurface);
        ret = gSDL_VideoSurface; // use current
    } else {
        ret = (*pSDL_SetVideoMode)(width, height, bpp, flags);
        if(!ret) {
            OutTraceE("%s: ERROR err=\"%s\"\n", ApiRef, intSDL_ErrorMessage());
            return 0;
        }
        // v2.04.44: once created the new window, set it as main window.
        HWND hwnd = (*pGetActiveWindow)();
        // alternative way:
        //SDL_SysWMinfo wmInfo;
        //SDL_GetWMInfo(&wmInfo);
        //HWND window = wmInfo.window;
        OutTraceSDL("%s: setting main win=%#x\n", ApiRef, hwnd);
        dxw.SethWnd(hwnd);
    }
    if(!ret) {
        OutTraceE("%s: ERROR err=\"%s\"\n", ApiRef, intSDL_ErrorMessage());
        return 0;
    }
    OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
    gSDL_VideoSurface = ret;
    if(bMove) {
        HWND hwnd = dxw.GethWnd();
        dxw.AdjustWindowPos(hwnd, width, height);
        dxw.UpdateDesktopCoordinates();
        if(bEmulation && pSDL_CreateRGBSurface) {
            if(gSDL_EmulatedVideo && pSDL_FreeSurface) (*pSDL_FreeSurface)(gSDL_EmulatedVideo);
            // create emulated video surface
            OutDebugSDL("%s: building emuvideo wxh=(%dx%d) bpp=%d RGBAmask=(%#x:%#x:%#x:%#x)\n",
                        ApiRef, oWidth, oHeight, bpp,
                        gSDL_VideoSurface->format->Rmask,
                        gSDL_VideoSurface->format->Gmask,
                        gSDL_VideoSurface->format->Bmask,
                        gSDL_VideoSurface->format->Amask);
            gSDL_EmulatedVideo = (*pSDL_CreateRGBSurface)(0, oWidth, oHeight, bpp,
                                 gSDL_VideoSurface->format->Rmask,
                                 gSDL_VideoSurface->format->Gmask,
                                 gSDL_VideoSurface->format->Bmask,
                                 gSDL_VideoSurface->format->Amask
                                                         );
            gSDL_VideoSurface = ret;
            ret = gSDL_EmulatedVideo;
            OutTraceSDL("%s: EMULATION created surface=%#x\n", ApiRef, gSDL_EmulatedVideo);
        }
    }
    // v2.04.43: refresh cursor clipping coordinates. Fixes "BOS Wars" initial clipping state.
    if(dxw.Windowize && dxw.dwFlags1 & CLIPCURSOR) dxw.SetClipCursor();
    return ret;
}

int extSDL_VideoModeOK(int width, int height, int bpp, Uint32 flags) {
    int ret;
    ApiName("SDL_VideoModeOK");
    OutTraceSDL("%s: size=(%dx%d) bpp=%d flags=%#x(%s)\n", ApiRef, width, height, bpp, flags, ExplainSDLSurfaceFlags(flags));
    if(dxw.Windowize && (flags & SDL_FULLSCREEN)) {
        flags &= ~SDL_FULLSCREEN;
        //width = dxw.iSizX;
        //height = dxw.iSizY;
        //OutTraceSDL("SDL_VideoModeOK: FIXED size=(%dx%d) bpp=%d flags=%#x\n", width, height, bpp, flags);
        OutTraceSDL("%s: FIXED flags=%#x\n", ApiRef, flags);
    }
    if(bEmulation) {
        if(bpp == 0) bpp = 32; // experimental for emulation mode, should get desktop color depth?
        flags &= ~SDL_HWPALETTE;
        // flags &= ~SDL_HWSURFACE; // experimental for DOSBox
    }
    ret = (*pSDL_VideoModeOK)(width, height, bpp, flags);
#ifndef DXW_NOTRACES
    if(!ret)
        OutTraceE("%s: ERROR err=\"%s\"\n", ApiRef, intSDL_ErrorMessage());
    else OutTraceSDL("%s: ret=%d\n", ApiRef, ret);
#endif // DXW_NOTRACES
    return ret;
}

Uint32 extSDL_GetMouseState(int *x, int *y) {
    Uint32 ret;
    ApiName("SDL_GetMouseState");
    ret = (*pSDL_GetMouseState)(x, y);
    if(x && y) {
        if(dxw.dwFlags1 & MODIFYMOUSE) {
            int px, py;
            px = *x;
            py = *y;
            dxw.UnmapClient(x, y);
            OutTraceC("%s: FIXED pos=(%d,%d)->(%d,%d) ret=%#x\n", ApiRef, px, py, *x, *y, ret);
        } else
            OutTraceC("%s: pos=(%d,%d) ret=%#x\n", ApiRef, *x, *y, ret);
    } else
        OutTraceC("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

Uint32 extSDL_GetRelativeMouseState(int *x, int *y) {
    Uint32 ret;
    ApiName("SDL_GetRelativeMouseState");
    ret = (*pSDL_GetRelativeMouseState)(x, y);
    if(x && y) {
        if(dxw.dwFlags1 & MODIFYMOUSE) {
            int px, py;
            px = *x;
            py = *y;
            dxw.UnmapClient(x, y);
            OutTraceC("%s: FIXED pos=(%d,%d)->(%d,%d) ret=%#x\n", ApiRef, px, py, *x, *y, ret);
        } else
            OutTraceC("%s: pos=(%d,%d) ret=%#x\n", ApiRef, *x, *y, ret);
    } else
        OutTraceC("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

int extSDL_Flip(SDL_Surface *screen) {
    int ret;
    ApiName("SDL_Flip");
    if(bEmulation & (screen == gSDL_EmulatedVideo)) {
        OutTraceSDL("%s: EMULATION surface=%#x->%#x\n", ApiRef, screen, gSDL_VideoSurface);
        intSDL_Refresh(ApiRef, NULL);
        screen = gSDL_VideoSurface;
    }
    ret = (*pSDL_Flip)(screen);
    OutTraceSDL("%s: screen=%#x ret=%#x\n", ApiRef, screen, ret);
    return ret;
}

int extSDL_LockSurface(SDL_Surface *surface) {
    int ret;
    ApiName("SDL_LockSurface");
    ret = (*pSDL_LockSurface)(surface);
    OutTraceSDL("%s: surface=%#x ret=%#x\n", ApiRef, surface, ret);
    return ret;
}

void extSDL_UnlockSurface(SDL_Surface *surface) {
    ApiName("SDL_UnlockSurface");
    (*pSDL_UnlockSurface)(surface);
    OutTraceSDL("%s: surface=%#x\n", ApiRef, surface);
    if(bEmulation && (surface == gSDL_EmulatedVideo))
        intSDL_Refresh(ApiRef, NULL);
}

SDL_Surface *extSDL_CreateRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
    SDL_Surface *ret;
    ApiName("SDL_CreateRGBSurface");
    OutTraceSDL("%s: flags=%#x size=(%dx%d) bpp=%d RGBA_mask=%#x.%#x.%#x.%#x\n", ApiRef, flags, width, height, depth, Rmask, Gmask, Bmask, Amask);
    ret = (*pSDL_CreateRGBSurface)(flags, width, height, depth, Rmask, Gmask, Bmask, Amask);
#ifndef DXW_NOTRACES
    if(!ret)
        OutTraceE("%s: ERROR err=\"%s\"\n", ApiRef, intSDL_ErrorMessage());
    else OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
#endif // DXW_NOTRACES
    return ret;
}

SDL_Surface *extSDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
    SDL_Surface *ret;
    ApiName("SDL_CreateRGBSurfaceFrom");
    OutTraceSDL("%s: pixels=%#x size=(%dx%d) bpp=%d pitch=%d RGBA_mask=%#x.%#x.%#x.%#x\n", ApiRef, pixels, width, height, depth, pitch, Rmask, Gmask, Bmask, Amask);
    ret = (*pSDL_CreateRGBSurfaceFrom)(pixels, width, height, depth, pitch, Rmask, Gmask, Bmask, Amask);
#ifndef DXW_NOTRACES
    if(!ret)
        OutTraceE("%s: ERROR err=\"%s\"\n", ApiRef, intSDL_ErrorMessage());
    else OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
#endif // DXW_NOTRACES
    return ret;
}

int intSDL_Blit(char *apiname, SDL_BlitSurface_Type pBlit, SDL_Surface *src, const SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
    int ret;
    ApiName(apiname);
#ifndef DXW_NOTRACES
    char s1[80];
    char s2[80];
    strcpy(s1, sRect(srcrect));
    strcpy(s2, sRect(dstrect));
    OutTraceSDL("%s: src=%#x srcrect=%s dst=%#x dstrect=%s\n", apiname, src, s1, dst, s2);
#endif // DXW_NOTRACES
    // v2.04.47: Blit from NULL surface is not allowed and should return an error, do not try to redirect to video srface
    // fixes Battle for Wesnoth
    if(dxw.Windowize) {
        if(bEmulation) {
            if(!dst) dst = gSDL_EmulatedVideo;
        } else {
            if(!dst) dst = gSDL_VideoSurface;
        }
    }
    if(bEmulation && src && (pBlit == pSDL_UpperBlit) && (dst == gSDL_EmulatedVideo)) {
        SDL_Rect crect;
        OutTraceSDL("%s: EMULATION blit=%#x->%#x\n", ApiRef, src, gSDL_EmulatedVideo);
        ret = (*pBlit)(src, srcrect, gSDL_EmulatedVideo, dstrect);
#ifndef DXW_NOTRACES
        if(ret)
            OutTraceE("%s: ERROR err=%#x(\"%s\") at %d\n", ApiRef, ret, intSDL_ErrorMessage(), __LINE__);
#endif // DXW_NOTRACES
        OutTraceSDL("%s: EMULATION stretch=%#x->%#x\n", ApiRef, gSDL_EmulatedVideo, gSDL_VideoSurface);
        crect =  intSDL_CompleteRect (src, srcrect, dst, dstrect);
        OutTraceSDL("%s: EMULATION crect={pos=(%d,%d) siz=(%dx%d)}\n", ApiRef, crect.x, crect.y, crect.w, crect.h);
        ret = intSDL_Refresh(ApiRef, &crect);
        return ret;
    }
    ret = (*pBlit)(src, srcrect, dst, dstrect);
#ifndef DXW_NOTRACES
    if(ret)
        OutTraceE("%s: ERROR err=%#x(\"%s\")\n", ApiRef, ret, intSDL_ErrorMessage());
#endif // DXW_NOTRACES
    return ret;
}

int extSDL_UpperBlit(SDL_Surface *src, const SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
    return intSDL_Blit("SDL_UpperBlit", pSDL_UpperBlit, src, srcrect, dst, dstrect);
}
int extSDL_LowerBlit(SDL_Surface *src, const SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
    return intSDL_Blit("SDL_LowerBlit", pSDL_LowerBlit, src, srcrect, dst, dstrect);
}
int extSDL_BlitSurface(SDL_Surface *src, const SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
    return intSDL_Blit("SDL_BlitSurface", pSDL_BlitSurface, src, srcrect, dst, dstrect);
}
int extSDL_BlitScaled(SDL_Surface *src, const SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
    return intSDL_Blit("SDL_BlitScaled", pSDL_BlitScaled, src, srcrect, dst, dstrect);
}
int extSDL_SoftStretch(SDL_Surface *src, const SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
    return intSDL_Blit("SDL_SoftStretch", pSDL_SoftStretch, src, srcrect, dst, dstrect);
}

int extSDL_FillRect(SDL_Surface *dst, const SDL_Rect *rect, Uint32 color) {
    int ret;
    ApiName("SDL_FillRect");
    OutTraceSDL("%s: dst=%#x rect=%s, color=%#x\n", ApiRef, dst, sRect(rect), color);
    ret = (*pSDL_FillRect)(dst, rect, color);
    IfSDLError(ret);
    return ret;
}

SDL_Surface *extSDL_GetVideoSurface(void) {
    SDL_Surface *ret;
    ApiName("SDL_GetVideoSurface");
    ret = (*pSDL_GetVideoSurface)();
    if(bEmulation && gSDL_EmulatedVideo) {
        OutTraceSDL("%s: ret=%#x->%#x\n", ApiRef, ret, gSDL_EmulatedVideo);
        ret = gSDL_EmulatedVideo;
    }
    OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

void extSDL_FreeSurface(SDL_Surface *surface) {
    ApiName("SDL_FreeSurface");
    OutTraceSDL("%s: surface=%#x\n", ApiRef, surface);
    //if(surface == gSDL_EmulatedVideo) {
    //	if(gSDL_VideoSurface) (*pSDL_FreeSurface)(gSDL_VideoSurface);
    //	gSDL_VideoSurface=NULL;
    //}
    // v2.04.34: it seems that the video surface can be freed many times without problems,
    // while memory surfaces crash the application. Found in "Battle for Wesnoth", where
    // the program frees the video surface twice (and using it anyway rightr after that!)
    // causing a crash if DxWnd tries to free the virtual surface as well.
    if(surface == gSDL_EmulatedVideo) return;
    (*pSDL_FreeSurface)(surface);
}

static char *sAlphaFlag(Uint32 f) {
    char *s;
    switch(f) {
    case SDL_SRCALPHA:
        s = "SDL_SRCALPHA";
        break;
    default:
        s = "??";
        break;
    }
    return s;
}

int extSDL_SetAlpha(SDL_Surface *surface, Uint32 flag, Uint8 alpha) {
    int ret;
    ApiName("SDL_SetAlpha");
    OutTraceSDL("%s: surface=%#x flag=%#x(%s) alpha=%#x\n", ApiRef, surface, flag, sAlphaFlag(flag), alpha);
    ret = (*pSDL_SetAlpha)(surface, flag, alpha);
    if(bEmulation && (surface == gSDL_EmulatedVideo)) {
        int ret2;
        ret2 = (*pSDL_SetAlpha)(gSDL_VideoSurface, flag, alpha);
        OutTraceSDL("%s: apply alpha to video surface ret=%#x\n", ApiRef, ret2);
    }
    IfSDLError(ret);
    return ret;
}

int extSDL_SetColorKey(SDL_Surface *surface, int flag, Uint32 key) {
    int ret;
    ApiName("SDL_SetColorKey");
    OutTraceSDL("%s: surface=%#x flag=%#x key=%#x\n", ApiRef, surface, flag, key);
    ret = (*pSDL_SetColorKey)(surface, flag, key);
    if(bEmulation && (surface == gSDL_EmulatedVideo)) {
        int ret2;
        ret2 = (*pSDL_SetColorKey)(gSDL_VideoSurface, flag, key);
        OutTraceSDL("%s: apply colorkey to video surface ret=%#x\n", ApiRef, ret2);
    }
    IfSDLError(ret);
    return ret;
}

int extSDL_SetColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors) {
    int ret;
    ApiName("SDL_SetColors");
    OutTraceSDL("%s: surface=%#x firstcolor=%d ncolors=%d\n", ApiRef, surface, firstcolor, ncolors);
    ret = (*pSDL_SetColors)(surface, colors, firstcolor, ncolors);
    if(bEmulation && (surface == gSDL_EmulatedVideo)) {
        int ret2;
        ret2 = (*pSDL_SetColors)(gSDL_VideoSurface, colors, firstcolor, ncolors);
        OutTraceSDL("%s: apply palette to video surface ret=%#x\n", ApiRef, ret2);
    }
    OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

void extSDL_Delay(Uint32 ms) {
    ApiName("SDL_Delay");
    OutTraceT("%s: ms=%d\n", ApiRef, ms);
    if(dxw.dwFlags2 & TIMESTRETCH) {
        Uint32 oms = ms;
        ms = dxw.StretchTime(ms);
        OutTraceT("%s: stretching ms=%d->%d\n", ApiRef, oms, ms);
    }
    return (*pSDL_Delay)(ms);
}

Uint32 extSDL_GetTicks(void) {
    Uint32 ret;
    ApiName("SDL_GetTicks");
    if(dxw.dwFlags2 & TIMESTRETCH) {
        ret = dxw.GetTickCount();
        OutTraceT("%s: stretched ticks=%d\n", ApiRef, ret);
    } else {
        ret = (*pSDL_GetTicks)();
        OutTraceT("%s: ticks=%d\n", ApiRef, ret);
    }
    return ret;
}

int extSDL_WaitEventTimeout(SDL_Event *event, int timeout) {
    int ret;
    ApiName("SDL_WaitEventTimeout");
    OutTraceT("%s: timeout=%d\n", ApiRef, timeout);
    if(dxw.dwFlags2 & TIMESTRETCH) {
        Uint32 otimeout = timeout;
        timeout = dxw.StretchTime(timeout);
        OutTraceT("%s: stretching timeout=%d->%d\n", ApiRef, otimeout, timeout);
    }
    ret = (*pSDL_WaitEventTimeout)(event, timeout);
    return ret;
}

void extSDL_WarpMouse(Uint16 x, Uint16 y) {
    ApiName("SDL_WarpMouse");
    OutTraceSDL("%s: pos=(%d,%d)\n", ApiRef, x, y);
    if(dxw.dwFlags2 & KEEPCURSORFIXED) {
        OutTraceSDL("%s: SUPPRESS pos=(%d,%d)\n", ApiRef, x, y);
        return;
    }
    if(dxw.dwFlags1 & SLOWDOWN) dxw.DoSlow(2);
    if(dxw.dwFlags1 & MODIFYMOUSE) {
        POINT cur;
        dxw.UpdateDesktopCoordinates();
        cur.x = x;
        cur.y = y;
        //dxw.MapWindow(&cur);
        dxw.MapClient(&cur);
        x = (Uint16)cur.x;
        y = (Uint16)cur.y;
        OutDebugSDL("%s: screen=(%dx%d) win=(%dx%d)\n", ApiRef, dxw.GetScreenWidth(), dxw.GetScreenHeight(), dxw.iSizX, dxw.iSizY);
        OutTraceSDL("%s: FIXED pos=(%d,%d)\n", ApiRef, x, y);
    }
    (*pSDL_WarpMouse)(x, y);
}

SDL_Overlay *extSDL_CreateYUVOverlay(int width, int height, Uint32 format, SDL_Surface *display) {
    SDL_Overlay *ret;
    ApiName("SDL_CreateYUVOverlay");
    OutTraceSDL("%s: size=(%dx%d) format=%#x display=%#x\n", ApiRef, width, height, format, display);
    // v2.04.32: when using overlays, always reference the actual video surface
    if(bEmulation && (display == gSDL_EmulatedVideo)) display = gSDL_VideoSurface;
    ret = (*pSDL_CreateYUVOverlay)(width, height, format, display);
#ifndef DXW_NOTRACES
    if(ret)
        OutTraceSDL("%s: overlay=%#x planes=%d hw=%#x\n", ApiRef, ret, ret->planes, ret->hw_overlay);
    else
        OutTraceE("%s: ERROR err=%#x(\"%s\")\n", ApiRef, ret, intSDL_ErrorMessage());
#endif // DXW_NOTRACES
    return ret;
}

int extSDL_DisplayYUVOverlay(SDL_Overlay *overlay, SDL_Rect *dstrect) {
    int ret;
    ApiName("SDL_DisplayYUVOverlay");
    OutTraceSDL("%s: overlay=%#x dstrect=%s\n", ApiRef, overlay, sRect(dstrect));
    // v2.04.32: when using overlays, scale the target rectangle
    if(bEmulation) dstrect = &intSDL_ScaleRect (gSDL_EmulatedVideo, dstrect);
    ret = (*pSDL_DisplayYUVOverlay)(overlay, dstrect);
    // v2.05.26: if error because of locked overlay, unlock it. Fixes "20000 Leguas de Viaje Submarino" intro movies.
    if(ret && pSDL_UnlockYUVOverlay) {
        (*pSDL_UnlockYUVOverlay)(overlay);
        ret = (*pSDL_DisplayYUVOverlay)(overlay, dstrect);
    }
    IfSDLError(ret);
    return ret;
}

void extSDL_FreeYUVOverlay(SDL_Overlay *overlay) {
    ApiName("SDL_FreeYUVOverlay");
    OutTraceSDL("%s: overlay=%#x\n", ApiRef, overlay);
    (*pSDL_FreeYUVOverlay)(overlay);
}

int extSDL_LockYUVOverlay(SDL_Overlay *overlay) {
    int ret;
    ApiName("SDL_LockYUVOverlay");
    OutTraceSDL("%s: overlay=%#x\n", ApiRef, overlay);
    ret = (*pSDL_LockYUVOverlay)(overlay);
    IfSDLError(ret);
    return ret;
}

void extSDL_UnlockYUVOverlay(SDL_Overlay *overlay) {
    ApiName("SDL_LockYUVOverlay");
    OutTraceSDL("%s: overlay=%#x\n", ApiRef, overlay);
    (*pSDL_UnlockYUVOverlay)(overlay);
}

SDL_bool extSDL_SetClipRect(SDL_Surface *surface, SDL_Rect *rect) {
    SDL_bool ret;
    ApiName("SDL_SetClipRect");
    OutTraceSDL("%s: surface=%#x rect=%s\n", ApiRef, surface, sRect(rect));
    ret = (*pSDL_SetClipRect)(surface, rect);
    // useful ???
    if(bEmulation && (surface == gSDL_EmulatedVideo)) {
        SDL_Rect crect;
        OutTraceSDL("%s: EMULATION clip=%#x->%#x\n", ApiRef, surface, gSDL_VideoSurface);
        //crect = intSDL_CompleteRect(surface, rect, gSDL_VideoSurface, NULL);
        crect = intSDL_ScaleRect(gSDL_EmulatedVideo, rect);
        OutTraceSDL("%s: EMULATION crect={pos=(%d,%d) siz=(%dx%d)}\n", ApiRef, crect.x, crect.y, crect.w, crect.h);
        if(!(ret = (*pSDL_SetClipRect)(gSDL_VideoSurface, &crect))) {
            OutTraceE("%s: EMULATION ERROR at %d\n", ApiRef, __LINE__);
            return ret;
        }
    }
    OutTraceSDL("%s: ret=%s\n", ApiRef, ret ? "SDL_TRUE" : "SDL_FALSE");
    return ret;
}

void extSDL_GetClipRect(SDL_Surface *surface, SDL_Rect *rect) {
    ApiName("SDL_GetClipRect");
    OutTraceSDL("%s: surface=%#x rect=%s\n", ApiRef, surface, sRect(rect));
    (*pSDL_GetClipRect)(surface, rect);
}

int extSDL_SetPalette(SDL_Surface *surface, int flags, SDL_Color *colors, int firstcolor, int ncolors) {
    int ret;
    ApiName("SDL_SetPalette");
    OutTraceSDL("%s: surface=%#x flags=%#x first=%d ncolors=%d\n", ApiRef, surface, flags, firstcolor, ncolors);
    ret = (*pSDL_SetPalette)(surface, flags, colors, firstcolor, ncolors);
    if(bEmulation && (surface == gSDL_EmulatedVideo)) {
        int ret2;
        ret2 = (*pSDL_SetPalette)(gSDL_VideoSurface, flags, colors, firstcolor, ncolors);
        OutTraceSDL("%s: apply palette to video surface ret=%#x\n", ApiRef, ret2);
    }
    OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

void extSDL_UpdateRects(SDL_Surface *screen, int numrects, SDL_Rect *rects) {
    ApiName("SDL_UpdateRects");
#ifndef DXW_NOTRACES
    if(IsTraceSDL) {
        OutTrace("%s: screen=%#x numrects=%d\n", ApiRef, screen, numrects);
        for(int i = 0; i < numrects; i++)
            OutTrace("rect[%d]=%s\n", i, sRect(&rects[i]));
    }
#endif // DXW_NOTRACES
    (*pSDL_UpdateRects)(screen, numrects, rects);
    if(bEmulation && (screen == gSDL_EmulatedVideo)) {
        SDL_Rect r;
        RECT wrect;
        intSDL_Refresh(ApiRef, NULL);
        (*pGetClientRect)(dxw.GethWnd(), &wrect);
        r.x = r.y = 0;
        r.w = (Uint16)(wrect.right - wrect.left);
        r.h = (Uint16)(wrect.bottom - wrect.top);
        (*pSDL_UpdateRects)(gSDL_VideoSurface, 1, &r);
    }
}

SDL_Surface *extSDL_ConvertSurface(SDL_Surface *src, const SDL_PixelFormat *fmt, Uint32 flags) {
    SDL_Surface *ret;
    ApiName("SDL_ConvertSurface");
#ifndef DXW_NOTRACES
    char sBuf[200];
#endif // DXW_NOTRACES
    OutTraceSDL("%s: src=%#x pixformat:%s flags=%#x\n",
                ApiRef, src, intSDL_PixelFormat(fmt, sBuf, 200), flags);
    ret = (*pSDL_ConvertSurface)(src, fmt, flags);
    OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

const SDL_VideoInfo *extSDL_GetVideoInfo(void) {
    SDL_VideoInfo *ret;
    ApiName("SDL_GetVideoInfo");
    static SDL_VideoInfo VideoInfo;
    OutTraceSDL("%s:\n", ApiRef);
    ret = (SDL_VideoInfo *)(*pSDL_GetVideoInfo)();
    OutDebugSDL("%s: "
                "hw_avail=%#x wm_avail=%#x "
                "blit_hw=%#x blit_hw_CC=%#x blit_hw_A=%#x "
                "blit_sw=%#x blit_sw_CC=%#x blit_sw_A=%#x "
                "blit_fill=%#x video_mem=%#x "
                "pixfmt.bpp=%d ",
                "current_video_size=(%dx%d)\n",
                ApiRef,
                ret->hw_available, ret->wm_available,
                ret->blit_hw, ret->blit_hw_A, ret->blit_hw_CC,
                ret->blit_sw, ret->blit_sw_A, ret->blit_sw_CC,
                ret->blit_fill, ret->video_mem,
                ret->vfmt->BitsPerPixel,
                ret->current_w, ret->current_h);
    return ret;
}

SDL_Cursor *extSDL_CreateCursor(Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y) {
    SDL_Cursor *ret;
    ApiName("SDL_CreateCursor");
    OutTraceSDL("%s: size=(%dx%d) hot=(%d,%d)\n", ApiRef, w, h, hot_x, hot_y);
    ret = (*pSDL_CreateCursor)(data, mask, w, h, hot_x, hot_y);
    OutTraceSDL("%s: cursor=%#x\n", ApiRef, ret);
    return ret;
}

void extSDL_FreeCursor(SDL_Cursor *cursor) {
    ApiName("SDL_CreateCursor");
    OutTraceSDL("%s: cursor=%#x\n", ApiRef, cursor);
    (*pSDL_FreeCursor)(cursor);
}


SDL_Surface *extSDL_DisplayFormatAlpha(SDL_Surface *surface) {
    SDL_Surface *ret;
    ApiName("SDL_DisplayFormatAlpha");
    OutTraceSDL("%s: surface=%#x\n", ApiRef, surface);
    ret = (*pSDL_DisplayFormatAlpha)(surface);
    OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

SDL_Rect **extSDL_ListModes(SDL_PixelFormat *format, Uint32 flags) {
    SDL_Rect **ret;
    ApiName("SDL_ListModes");
#ifndef DXW_NOTRACES
    char sBuf[200];
#endif // DXW_NOTRACES
    OutTraceSDL("%s: pixfmt=%s flags=%#x\n", ApiRef, intSDL_PixelFormat(format, sBuf, 200), flags);
    ret = (*pSDL_ListModes)(format, flags);
#ifndef DXW_NOTRACES
    switch((int)ret) {
    case -1:
        OutTraceSDL("%s: ret=-1 all resolutions available\n", ApiRef);
        break;
    case  0:
        OutTraceSDL("%s: ret=0 no modes available\n", ApiRef);
        break;
    default:
        OutTraceSDL("%s: available modes:\n", ApiRef);
        for (int i = 0; ret[i]; ++i)
            OutTraceSDL(" %dx%d\n", ret[i]->w, ret[i]->h);
        break;
    }
#endif // DXW_NOTRACES
    return ret;
}

// == Joystick wrappers ==

int extSDL_NumJoysticks(void) {
    int ret;
    ApiName("SDL_NumJoysticks");
    if(dxw.dwFlags9 & HIDEJOYSTICKS) {
        OutTraceDW("%s: hide joystick ret=0\n", ApiRef);
        return 0;
    }
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) {
        OutTraceDW("%s: emulate joystick ret=1\n", ApiRef);
        return 1;
    }
    ret = (*pSDL_NumJoysticks)();
    OutTraceDW("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

const char *extSDL_JoystickName(SDL_Joystick *joystick) {
    char *ret;
    ApiName("SDL_JoystickName");
    if(dxw.dwFlags9 & HIDEJOYSTICKS) {
        OutTraceDW("%s: hide joystick ret=NULL\n", ApiRef);
        return NULL;
    }
    if(dxw.dwFlags6 & VIRTUALJOYSTICK)
        ret = "DxWnd Joystick Emulator";
    else
        ret = (char *)(*pSDL_JoystickName)(joystick);
    OutTraceDW("%s: ret=\"%s\"\n", ApiRef, ret);
    return ret;
}

const char *extSDL_JoystickNameForIndex(int device_index) {
    char *ret;
    ApiName("SDL_JoystickNameForIndex");
    if(dxw.dwFlags9 & HIDEJOYSTICKS) {
        OutTraceDW("%s: hide joystick idx=%d ret=NULL\n", ApiRef, device_index);
        return NULL;
    }
    if(dxw.dwFlags6 & VIRTUALJOYSTICK)
        ret = (device_index == 0) ? "DxWnd Joystick Emulator" : NULL;
    else
        ret = (char *)(*pSDL_JoystickNameForIndex)(device_index);
    OutTraceDW("%s: idx=%d ret=\"%s\"\n", ApiRef, device_index, ret);
    return ret;
}

SDL_Joystick *extSDL_JoystickOpen(int device_index) {
    SDL_Joystick *ret;
    ApiName("SDL_JoystickOpen");
    static DWORD VirtualJoy;
    if(dxw.dwFlags9 & HIDEJOYSTICKS) {
        OutTraceDW("%s: hide joystick idx=%d ret=NULL\n", ApiRef, device_index);
        return NULL;
    }
    if(dxw.dwFlags6 & VIRTUALJOYSTICK)
        ret = (SDL_Joystick *)&VirtualJoy;
    else
        ret = (*pSDL_JoystickOpen)(device_index);
    OutTraceDW("%s: idx=%d ret=%#x\n", ApiRef, device_index, ret);
    return ret;
}

int extSDL_JoystickNumAxes(SDL_Joystick *joy) {
    int ret;
    ApiName("SDL_JoystickNumAxes");
    if(dxw.dwFlags9 & HIDEJOYSTICKS) {
        OutTraceDW("%s: hide joystick joy=%#x ret=-1\n", ApiRef, joy);
        return -1;
    }
    if(dxw.dwFlags6 & VIRTUALJOYSTICK)
        ret =  2;
    else
        ret = (*pSDL_JoystickNumAxes)(joy);
    OutTraceDW("%s: joy=%#x ret=%d\n", ApiRef, joy, ret);
    return ret;
}

int extSDL_JoystickNumButtons(SDL_Joystick *joy) {
    int ret;
    ApiName("SDL_JoystickNumButtons");
    if(dxw.dwFlags9 & HIDEJOYSTICKS) {
        OutTraceDW("%s: hide joystick joy=%#x ret=-1\n", ApiRef, joy);
        return -1;
    }
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) {
        ret = 2; // two buttons for virtual joystick
    } else
        ret = (*pSDL_JoystickNumButtons)(joy);
    OutTraceDW("%s: joy=%#x ret=%d\n", ApiRef, joy, ret);
    return ret;
}

int extSDL_JoystickNumBalls(SDL_Joystick *joy) {
    int ret;
    ApiName("SDL_JoystickNumBalls");
    if(dxw.dwFlags9 & HIDEJOYSTICKS) {
        OutTraceDW("%s: hide joystick joy=%#x ret=-1\n", ApiRef, joy);
        return -1;
    }
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) {
        ret = 0; // no balls for virtual joystick
    } else
        ret = (*pSDL_JoystickNumBalls)(joy);
    OutTraceDW("%s: joy=%#x ret=%d\n", ApiRef, joy, ret);
    return ret;
}

Sint16 extSDL_JoystickGetAxis(SDL_Joystick *joy, int axis) {
    BOOL bActive;
    Sint16 ret = 0;
    ApiName("SDL_JoystickGetAxis");
    if(dxw.dwFlags9 & HIDEJOYSTICKS) {
        OutTraceDW("%s: hide joystick joy=%#x ret=-1\n", ApiRef, joy);
        return -1;
    }
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) {
        JOYINFO jinfo;
        bActive = vjGetJoy(ApiRef, &jinfo);
        switch (axis) {
        case 0:
            ret = (Sint16)((jinfo.wXpos * 32768) / 128);
            break;
        case 1:
            ret = (Sint16)((jinfo.wYpos * 32768) / 128);
            break;
        default:
            ret = 0;
        }
    } else
        ret = (*pSDL_JoystickGetAxis)(joy, axis);
    OutTraceDW("%s: joy=%#x axis=%d ret=%d\n", ApiRef, joy, axis, ret);
    return ret;
}

Uint8 extSDL_JoystickGetButton(SDL_Joystick *joy, int button) {
    Uint8 ret = 0;
    BOOL bActive;
    ApiName("SDL_JoystickGetButton");
    if(dxw.dwFlags9 & HIDEJOYSTICKS) {
        OutTraceDW("%s: hide joystick joy=%#x ret=-1\n", ApiRef, joy);
        return -1;
    }
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) {
        JOYINFO jinfo;
        bActive = vjGetJoy(ApiRef, &jinfo);
        switch (button) {
        case 0:
            ret = (Uint8)(jinfo.wButtons & 0x00000001) ? 1 : 0;
            break;
        case 1:
            ret = (Uint8)(jinfo.wButtons & 0x00000002) ? 1 : 0;
            break;
        default:
            ret = 0;
        }
    } else
        ret = (*pSDL_JoystickGetButton)(joy, button);
    OutTraceDW("%s: joy=%#x axis=%d ret=%d\n", ApiRef, joy, button, ret);
    return ret;
}

Uint8 extSDL_JoystickGetHat(SDL_Joystick *joy, int hat) {
    Uint8 ret;
    ApiName("SDL_JoystickGetHat");
    if(dxw.dwFlags9 & HIDEJOYSTICKS) {
        OutTraceDW("%s: hide joystick joy=%#x ret=-1\n", ApiRef, joy);
        return -1;
    }
    if(dxw.dwFlags6 & VIRTUALJOYSTICK)
        ret = -1;
    else
        ret = (*pSDL_JoystickGetHat)(joy, hat);
    OutTraceDW("%s: joy=%#x hat=%d ret=%d\n", ApiRef, joy, hat, ret);
    return ret;
}

void extSDL_JoystickUpdate(void) {
    ApiName("SDL_JoystickUpdate");
    if((dxw.dwFlags9 & HIDEJOYSTICKS) || (dxw.dwFlags6 & VIRTUALJOYSTICK))
        OutTraceSDL("%s: bypass\n", ApiRef);
    else {
        OutTraceSDL("%s\n", ApiRef);
        (*pSDL_JoystickUpdate)();
    }
}

void extSDL_JoystickClose(SDL_Joystick *joy) {
    ApiName("SDL_JoystickClose");
    if((dxw.dwFlags9 & HIDEJOYSTICKS) || (dxw.dwFlags6 & VIRTUALJOYSTICK))
        OutTraceSDL("%s: bypass\n", ApiRef);
    else {
        OutTraceSDL("%s: joy=%#x\n", ApiRef, joy);
        (*pSDL_JoystickClose)(joy);
    }
}

int extSDL_JoystickEventState(int state) {
    int ret;
    ApiName("SDL_JoystickEventState");
    OutTraceSDL("%s: state=%d(%s)\n", ApiRef, state, sState(state));
    if(dxw.dwFlags9 & HIDEJOYSTICKS) return -1;
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) {
        switch(state) {
        case SDL_QUERY:
            break;
        case SDL_IGNORE:
            gSDL_JoystickEventState = FALSE;
            break;
        case SDL_ENABLE:
            gSDL_JoystickEventState = TRUE;
            break;
        default:
            gSDL_JoystickEventState = state;
            break;
        }
        ret = gSDL_JoystickEventState;
    } else
        ret = (*pSDL_JoystickEventState)(state);
    OutTraceSDL("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

int extSDL_JoystickOpened(int index) {
    int ret;
    ApiName("SDL_JoystickOpened");
    OutTraceSDL("%s: index=%d\n", ApiRef, index);
    if(dxw.dwFlags9 & HIDEJOYSTICKS) return 0; // 0 = not opened
    if(dxw.dwFlags6 & VIRTUALJOYSTICK) {
        // virtual joystick: joystick #0 always open, all others close
        ret = (index == 0) ? 1 : 0;
    } else
        ret = (*pSDL_JoystickOpened)(index);
    OutTraceSDL("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

// == Event wrappers ==

static void HandlePulledEvent(char *label, SDL_Event *e) {
    int x, y;
    switch(e->type) {
    case SDL_MOUSEMOTION:
        x = e->motion.x;
        y = e->motion.y;
        if(dxw.dwFlags1 & MODIFYMOUSE) {
            dxw.UnmapClient(&x, &y);
            e->motion.x = x;
            e->motion.y = y;
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        x = e->button.x;
        y = e->button.y;
        if(dxw.dwFlags1 & MODIFYMOUSE) {
            dxw.UnmapClient(&x, &y);
            e->button.x = x;
            e->button.y = y;
        }
        break;
    }
}

static void HandlePushedEvent(char *label, SDL_Event *e) {
    switch(e->type) {
    case SDL_MOUSEMOTION:
        if(dxw.Windowize && dxw.IsFullScreen()) {
            dxw.MapClient(&(e->motion.x), &(e->motion.y));
            dxw.MapClient((USHORT *) & (e->motion.xrel), (USHORT *) & (e->motion.yrel));
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        if(dxw.Windowize && dxw.IsFullScreen())
            dxw.MapClient(&(e->button.x), &(e->button.y));
    }
}

// v2.04.43: SDL_PollEvent hook to fix mouse coordinates. Fixes "BOS Wars".
// TBD: good in any situation? If not, should be conditioned by SDL flag ....

int extSDL_PollEvent(SDL_Event *e) {
    int ret;
    ApiName("SDL_PollEvent");
    ret = (*pSDL_PollEvent)(e);
    if(ret && e) HandlePulledEvent(ApiRef, e);
    return ret;
}

int extSDL_PushEvent(SDL_Event *e) {
    int ret;
    ApiName("SDL_PushEvent");
#ifndef DXW_NOTRACES
    TraceEvent(ApiRef, e);
#endif // DXW_NOTRACES
    HandlePushedEvent(ApiRef, e);
    ret = (*pSDL_PushEvent)(e);
    OutTraceSDL("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

int extSDL_PeepEvents1(SDL_Event *events, int iNumEvents, SDL_eventaction action, Uint32 minType, Uint32 maxType) {
    int ret;
    ApiName("SDL_PeepEvents");
#ifndef DXW_NOTRACES
    OutTraceSDL("%s: num=%d action=%d(%s) min=%d max=%d\n",
                ApiRef, iNumEvents, action, sActionType(action), minType, maxType);
    if(action == SDL_ADDEVENT) {
        for(int i = 0; i < iNumEvents; i++) TraceEvent(ApiRef, &events[i]);
    }
#endif // DXW_NOTRACES
    // added events must be converted BEFORE the call
    if(action == SDL_ADDEVENT) {
        for(int i = 0; i < iNumEvents; i++) HandlePushedEvent(ApiRef, &events[i]); // v2.05.04 fix: must loop ret times
    }
    ret = (*pSDL_PeepEvents1)(events, iNumEvents, action, minType, maxType);
    // pulled events must be converted AFTER the call
    if(action != SDL_ADDEVENT) {
        for(int i = 0; i < ret; i++) HandlePulledEvent(ApiRef, &events[i]); // v2.05.04 fix: must loop ret times
    }
#ifndef DXW_NOTRACES
    if(action != SDL_ADDEVENT) {
        for(int i = 0; i < ret; i++) TraceEvent(ApiRef, &events[i]); // v2.05.04 fix: must loop ret times
    }
    OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
#endif // DXW_NOTRACES
    return ret;
}

// == SDL2 wrappers ==

int extSDL_PeepEvents2(SDL_Event *events, int iNumEvents, SDL_eventaction action, Uint32 mask) {
    int ret;
    ApiName("SDL_PeepEvents");
#ifndef DXW_NOTRACES
    OutTraceSDL("%s: num=%d action=%d(%s) mask=%#x\n",
                ApiRef, iNumEvents, action, sActionType(action), mask);
    if(action == SDL_ADDEVENT) {
        for(int i = 0; i < iNumEvents; i++) TraceEvent(ApiRef, &events[i]);
    }
#endif // DXW_NOTRACES
    // added events must be converted BEFORE the call
    if(action == SDL_ADDEVENT) {
        for(int i = 0; i < iNumEvents; i++) HandlePushedEvent(ApiRef, &events[i]); // v2.05.04 fix: must loop ret times
    }
    ret = (*pSDL_PeepEvents2)(events, iNumEvents, action, mask);
    // pulled events must be converted AFTER the call
    if(action != SDL_ADDEVENT) {
        for(int i = 0; i < ret; i++) HandlePulledEvent(ApiRef, &events[i]); // v2.05.04 fix: must loop ret times
    }
#ifndef DXW_NOTRACES
    if(action != SDL_ADDEVENT) {
        for(int i = 0; i < ret; i++) TraceEvent(ApiRef, &events[i]); // v2.05.04 fix: must loop ret times
    }
    OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
#endif // DXW_NOTRACES
    return ret;
}

SDL_Renderer *extSDL_CreateRenderer(SDL_Window *window, int index, Uint32 flags) {
    SDL_Renderer *ret;
    ApiName("SDL_CreateRenderer");
    OutTraceSDL("%s: win=%#x index=%d flags=%#x\n", ApiRef, window, index, flags);
    ret = (*pSDL_CreateRenderer)(window, index, flags);
    OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

SDL_Renderer *extSDL_CreateSoftwareRenderer(SDL_Surface *surface) {
    SDL_Renderer *ret;
    ApiName("SDL_CreateSoftwareRenderer");
    OutTraceSDL("%s: surface=%#x\n", ApiRef, surface);
    ret = (*pSDL_CreateSoftwareRenderer)(surface);
    OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

#ifndef SDL_WINDOW_FULLSCREEN
typedef enum {
    SDL_WINDOW_FULLSCREEN = 0x00000001,         /**< fullscreen window */
    SDL_WINDOW_OPENGL = 0x00000002,             /**< window usable with OpenGL context */
    SDL_WINDOW_SHOWN = 0x00000004,              /**< window is visible */
    SDL_WINDOW_HIDDEN = 0x00000008,             /**< window is not visible */
    SDL_WINDOW_BORDERLESS = 0x00000010,         /**< no window decoration */
    SDL_WINDOW_RESIZABLE = 0x00000020,          /**< window can be resized */
    SDL_WINDOW_MINIMIZED = 0x00000040,          /**< window is minimized */
    SDL_WINDOW_MAXIMIZED = 0x00000080,          /**< window is maximized */
    SDL_WINDOW_INPUT_GRABBED = 0x00000100,      /**< window has grabbed input focus */
    SDL_WINDOW_INPUT_FOCUS = 0x00000200,        /**< window has input focus */
    SDL_WINDOW_MOUSE_FOCUS = 0x00000400,        /**< window has mouse focus */
    SDL_WINDOW_FULLSCREEN_DESKTOP = ( SDL_WINDOW_FULLSCREEN | 0x00001000 ),
    SDL_WINDOW_FOREIGN = 0x00000800,            /**< window not created by SDL */
    SDL_WINDOW_ALLOW_HIGHDPI = 0x00002000,      /**< window should be created in high-DPI mode if supported */
    SDL_WINDOW_MOUSE_CAPTURE = 0x00004000,      /**< window has mouse captured (unrelated to INPUT_GRABBED) */
    SDL_WINDOW_ALWAYS_ON_TOP = 0x00008000,      /**< window should always be above others */
    SDL_WINDOW_SKIP_TASKBAR  = 0x00010000,      /**< window should not be added to the taskbar */
    SDL_WINDOW_UTILITY       = 0x00020000,      /**< window should be treated as a utility window */
    SDL_WINDOW_TOOLTIP       = 0x00040000,      /**< window should be treated as a tooltip */
    SDL_WINDOW_POPUP_MENU    = 0x00080000       /**< window should be treated as a popup menu */
} SDL_WindowFlags;
#endif

SDL_Window *extSDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) {
    SDL_Window *ret;
    ApiName("SDL_CreateWindow");
    OutTraceSDL("%s: title=\"%s\" pos=(%d,%d) siz=(%dx%d) flags=%#x\n", ApiRef, title, x, y, w, h, flags);
    //if(dxw.Windowize && (flags & (SDL_WINDOW_FULLSCREEN|SDL_WINDOW_FULLSCREEN_DESKTOP))){
    if(dxw.Windowize) {
        OutTraceSDL("%s: windowize fullscreen request\n", ApiRef);
        x = dxw.iPosX;
        y = dxw.iPosY;
        w = dxw.iSizX;
        h = dxw.iSizY;
        flags &= ~(SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP);
        flags |= SDL_WINDOW_RESIZABLE; // BEWARE: fixes crash problem in SDL_UpdateTexture (where .. ???)
        // with SDL2 you can't ask for whatever window style
        if(dxw.WindowStyle == WSTYLE_MODALSTYLE) flags |= SDL_WINDOW_BORDERLESS;
    }
    ret = (*pSDL_CreateWindow)(title, x, y, w, h, flags);
    OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

int extSDL_SetWindowFullscreen(SDL_Window *window, Uint32 flags) {
    int ret;
    ApiName("SDL_SetWindowFullscreen");
    OutTraceSDL("%s: win=%#x flags=%#x\n", ApiRef, window, flags);
    if(dxw.Windowize) {
        if(flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) {
            dxwSDLSethWnd(ApiRef, window);
            flags = 0;
        }
    }
    ret = (*pSDL_SetWindowFullscreen)(window, flags);
    OutTraceSDL("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

#ifndef SDL_PIXELFORMAT_UNKNOWN
#define SDL_PIXELFORMAT_UNKNOWN 0
#endif

SDL_Texture *extSDL_CreateTexture(SDL_Renderer *renderer, Uint32 format, int access, int w, int h) {
    SDL_Texture *ret;
    ApiName("SDL_CreateTexture");
    OutTraceSDL("%s: renderer=%#x format=%#x access=%#x size=(%dx%d)\n", ApiRef, renderer, format, access, w, h);
    ret = (*pSDL_CreateTexture)(renderer, format, access, w, h);
    OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

int extSDL_UpdateTexture(SDL_Texture *texture, const SDL_Rect *rect, const void *pixels, int pitch) {
    int ret;
    ApiName("SDL_UpdateTexture");
    OutTraceSDL("%s: texture=%#x rect=%s pixels=%#x pitch=%d\n",
                ApiRef, texture, sRect(rect), pixels, pitch);
    ret = (*pSDL_UpdateTexture)(texture, rect, pixels, pitch);
    OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

void extSDL_SetWindowPosition(SDL_Window *window, int x, int y) {
    ApiName("SDL_SetWindowPosition");
    OutTraceSDL("%s: win=%#x pos=(%d,%d)\n", ApiRef, window, x, y);
    if(dxw.Windowize && dxw.IsFullScreen() && (window == gSDL_MainWindow)) {
        OutTraceDW("%s: main win=%#x remapped pos=(%d,%d)\n", ApiRef, window, dxw.iPosX, dxw.iPosY);
        (*pSDL_SetWindowPosition)(window, dxw.iPosX, dxw.iPosY);
    } else
        (*pSDL_SetWindowPosition)(window, x, y);
}

void extSDL_SetWindowSize(SDL_Window *window, int w, int h) {
    ApiName("SDL_SetWindowSize");
    OutTraceSDL("%s: win=%#x size=(%dx%d)\n", ApiRef, window, w, h);
    // in windowized mode therer are two interesting situations:
    // 1) a normal window is requested to grow bigger or equal to current screen resolution
    // 2) a fullscreen window is requested to change size
    // in both cases the window is the (new) main window and the size is the new resolution
    if(dxw.Windowize && (
                (window == gSDL_MainWindow) ||
                ((w >= (int)dxw.GetScreenWidth()) && (h >= (int)dxw.GetScreenHeight()) ))
      ) {
        dxwSDLSethWnd(ApiRef, window);
        dxw.SetScreenSize(w, h);
    } else
        (*pSDL_SetWindowSize)(window, w, h);
}

void extSDL_GL_SwapWindow(SDL_Window *window) {
    ApiName("SDL_GL_SwapWindow");
    OutTraceSDL("%s: win=%#x\n", ApiRef, window);
    (*pSDL_GL_SwapWindow)(window);
}

SDL_bool extSDL_GetWindowWMInfo(SDL_Window *window, SDL_SysWMinfo *info) {
    SDL_bool ret;
    ApiName("SDL_GetWindowWMInfo");
    OutTraceSDL("%s: win=%#x\n", ApiRef, window);
    ret = (*pSDL_GetWindowWMInfo)(window, info);
    return ret;
}

int extSDL_SetWindowBrightness(SDL_Window *window, float brightness) {
    int ret;
    ApiName("SDL_SetWindowBrightness");
    OutTraceSDL("%s: win=%#x brightness=%f\n", ApiRef, window, brightness);
    ret = (*pSDL_SetWindowBrightness)(window, brightness);
    return ret;
}

int extSDL_GetCurrentDisplayMode(int displayIndex, SDL_DisplayMode *mode) {
    int ret;
    ApiName("SDL_GetCurrentDisplayMode");
    OutTraceSDL("%s: index=%d\n", ApiRef, displayIndex);
    ret = (*pSDL_GetCurrentDisplayMode)(displayIndex, mode);
    if(ret) {
        IfSDLError(ret);
        return ret;
    }
    OutTraceSDL("> format=%d\n", mode->format);
    OutTraceSDL("> size=(%dx%d)\n", mode->w, mode->h);
    OutTraceSDL("> refresh_rate=%d\n", mode->refresh_rate);
    if (dxw.IsEmulated) {
        mode->w = dxw.GetScreenWidth();
        mode->h = dxw.GetScreenHeight();
        OutTraceSDL("%s: fixed resolution=(%dx%d)\n", ApiRef, mode->w, mode->h);
    }
    return ret;
}

int extSDL_GetDesktopDisplayMode(int displayIndex, SDL_DisplayMode *mode) {
    int ret;
    ApiName("SDL_GetDesktopDisplayMode");
    OutTraceSDL("%s: index=%d\n", ApiRef, displayIndex);
    ret = (*pSDL_GetDesktopDisplayMode)(displayIndex, mode);
    if(ret) {
        IfSDLError(ret);
        return ret;
    }
    OutTraceSDL("> format=%d\n", mode->format);
    OutTraceSDL("> size=(%dx%d)\n", mode->w, mode->h);
    OutTraceSDL("> refresh_rate=%d\n", mode->refresh_rate);
    if (dxw.IsEmulated) {
        mode->w = dxw.GetScreenWidth();
        mode->h = dxw.GetScreenHeight();
        OutTraceSDL("%s: fixed resolution=(%dx%d)\n", ApiRef, mode->w, mode->h);
    }
    return ret;
}

int extSDL_GetDisplayMode(int displayIndex, int modeIndex, SDL_DisplayMode *mode) {
    int ret;
    ApiName("SDL_GetDisplayMode");
    OutTraceSDL("%s: displayindex=%d modeindex=%d\n", ApiRef, displayIndex, modeIndex);
    ret = (*pSDL_GetDisplayMode)(displayIndex, modeIndex, mode);
    if(ret) {
        IfSDLError(ret);
        return ret;
    }
    OutTraceSDL("> format=%d\n", mode->format);
    OutTraceSDL("> size=(%dx%d)\n", mode->w, mode->h);
    OutTraceSDL("> refresh_rate=%d\n", mode->refresh_rate);
    //if (dxw.IsEmulated){
    //	mode->w = dxw.GetScreenWidth();
    //	mode->h = dxw.GetScreenHeight();
    //	OutTraceSDL("%s: fixed resolution=(%dx%d)\n", ApiRef, mode->w, mode->h);
    //}
    return ret;
}

int extSDL_GL_SetSwapInterval(int interval) {
    int ret;
    ApiName("SDL_GL_SetSwapInterval");
    OutTraceSDL("%s: interval=%d\n", ApiRef, interval);
    ret = (*pSDL_GL_SetSwapInterval)(interval);
    return ret;
}

SDL_GLContext extSDL_GL_CreateContext(SDL_Window *window) {
    SDL_GLContext ret;
    ApiName("SDL_GL_CreateContext");
    OutTraceSDL("%s: window=%#x\n", ApiRef, window);
    ret = (*pSDL_GL_CreateContext)(window);
    OutTraceSDL("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

void extSDL_SetEventFilter(SDL_EventFilter filter, void *userdata) {
    ApiName("SDL_SetEventFilter");
    OutTraceSDL("%s: filter=%#x userdata=%#x\n", ApiRef, filter, userdata);
    (*pSDL_SetEventFilter)(filter, userdata);
}

void extSDL_GetWindowSize(SDL_Window *window, int *w, int *h) {
    ApiName("SDL_GetWindowSize");
    OutTraceSDL("%s: window=%#x\n", ApiRef, window);
    (*pSDL_GetWindowSize)(window, w, h);
    OutTraceSDL("%s: window=%#x size=(%dx%d)\n", ApiRef, window, *w, *h);
    if(window == gSDL_MainWindow) {
        *w = dxw.GetScreenWidth();
        *h = dxw.GetScreenHeight();
        OutTraceSDL("%s: FIXED size=(%dx%d)\n", ApiRef, *w, *h);
    }
}

//void extSDL_ShowWindow(SDL_Window* window){}
//int extSDL_GetNumDisplayModes(int displayIndex){return 0;}

int extSDL_ShowCursor(int toggle) {
    int ret;
    ApiName("SDL_ShowCursor");
    static int iCurrentState = 0;
    OutTraceC("%s: toggle=%#x(%s)\n", ApiRef, toggle,
              toggle == SDL_QUERY ? "QUERY" : (toggle == SDL_ENABLE ? "ENABLE" : "DISABLE"));
    if(toggle != SDL_QUERY) {
        iCurrentState = toggle;
        if (dxw.dwFlags1 & HIDEHWCURSOR) toggle = SDL_DISABLE;
        if (dxw.dwFlags2 & SHOWHWCURSOR) toggle = SDL_ENABLE;
    }
    ret = (*pSDL_ShowCursor)(toggle);
    if(toggle == SDL_QUERY) {
        ret = iCurrentState; // return last status
    } else {
        ret = toggle; // fake successful operation
    }
    OutTraceC("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

int extSDL_GetDisplayBounds(int displayIndex, SDL_Rect *rect) {
    int ret;
    ApiName("SDL_GetDisplayBounds");
    OutTraceSDL("%s: displayindex=%d rect=%#x\n", ApiRef, displayIndex, rect);
    if(dxw.Windowize && dxw.IsFullScreen()) {
        if(rect) {
            rect->x = 0;
            rect->y = 0;
            rect->w = (Uint16)dxw.GetScreenWidth();
            rect->h = (Uint16)dxw.GetScreenHeight();
        }
        ret = 0;
    } else
        ret = (*pSDL_GetDisplayBounds)(displayIndex, rect);
    if(ret)
        OutTraceE("%s: ERROR ret=%d\n", ApiRef, ret);
    else {
        OutTraceSDL("%s: rect={pos=(%d,%d) siz=(%dx%d)}\n",
                    ApiRef, rect->x, rect->y, rect->w, rect->h);
    }
    return ret;
}

int extSDL_GetDisplayUsableBounds(int displayIndex, SDL_Rect *rect) {
    ApiName("SDL_GetDisplayUsableBounds");
    int ret;
    OutTraceSDL("%s: displayindex=%d rect=%#x\n", ApiRef, displayIndex, rect);
    if(dxw.Windowize && dxw.IsFullScreen()) {
        if(rect) {
            rect->x = 0;
            rect->y = 0;
            rect->w = (Uint16)dxw.GetScreenWidth();
            rect->h = (Uint16)dxw.GetScreenHeight();
        }
        ret = 0;
    } else
        ret = (*pSDL_GetDisplayUsableBounds)(displayIndex, rect);
    if(ret)
        OutTraceE("%s: ERROR ret=%d\n", ApiRef, ret);
    else {
        OutTraceSDL("%s: rect={pos=(%d,%d) siz=(%dx%d)}\n",
                    ApiRef, rect->x, rect->y, rect->w, rect->h);
    }
    return ret;
}

int extSDL_SetWindowDisplayMode(SDL_Window *window, const SDL_DisplayMode *mode) {
    ApiName("SDL_SetWindowDisplayMode");
    int ret;
    OutTraceSDL("%s: win=%#x mode={size=(%dx%d) refreshrate=%d format=%#x driverdata=%#x}\n",
                ApiRef, window, mode->w, mode->h, mode->refresh_rate, mode->format, mode->driverdata);
    if(dxw.Windowize) {
        dxw.SetScreenSize(mode->w, mode->h);
        ret = 0;
    } else
        ret = (*pSDL_SetWindowDisplayMode)(window, mode);
    if(ret)
        OutTraceE("%s: ERROR ret=%d\n", ApiRef, ret);
    return ret;
}

#if 0
int extSDL_GetWindowDisplayIndex(SDL_Window *window) {
    return 0;
}

int SDL_GetNumVideoDisplays(void) {
    return 1;
}
#endif

int extSDL_CondWaitTimeout(SDL_cond *cond, SDL_mutex *mutex, Uint32 ms) {
    int ret;
    ApiName("SDL_CondWaitTimeout");
    OutTraceT("%s: ms=%d\n", ApiRef, ms);
    if(dxw.dwFlags2 & TIMESTRETCH) {
        Uint32 oms = ms;
        ms = dxw.StretchTime(ms);
        if(oms && !ms) ms = 1; // avoid infinite wait when not asked for!
        OutTraceT("%s: stretching ms=%d->%d\n", ApiRef, oms, ms);
    }
    ret = (*pSDL_CondWaitTimeout)(cond, mutex, ms);
    return ret;
}
