#include "Vfw.h"

// MSVFW32.dll:

//#define TRACEALL

extern FARPROC Remap_vfw_ProcAddress(LPCSTR, HMODULE);
extern void HookMSV4WLibs(HMODULE);

#undef DXWEXTERN
#ifdef DXWDECLARATIONS
#define DXWEXTERN
#else
#define DXWEXTERN extern
#endif

typedef HIC (WINAPI *ICGetDisplayFormat_Type)(HIC, LPBITMAPINFOHEADER, LPBITMAPINFOHEADER, int, int, int);
//typedef HWND (WINAPI *MCIWndCreateA_Type)(HWND, HINSTANCE, DWORD, LPCTSTR);
typedef HWND (*MCIWndCreateA_Type)(HWND, HINSTANCE, DWORD, LPCTSTR);
typedef DWORD (WINAPIV *ICDrawBegin_Type)(HIC, DWORD, HPALETTE, HWND,  HDC, int, int, int, int, LPBITMAPINFOHEADER, int, int, int, int, DWORD, DWORD);
typedef BOOL (WINAPI *DrawDibDraw_Type)(HDRAWDIB, HDC, int, int, int, int, LPBITMAPINFOHEADER, LPVOID, int, int, int, int, UINT);
typedef BOOL (WINAPI *DrawDibBegin_Type)(HDRAWDIB, HDC, int, int, LPBITMAPINFOHEADER, int, int, UINT);
#ifdef TRACEALL
typedef LRESULT (WINAPI *ICSendMessage_Type)(HIC, UINT, DWORD_PTR, DWORD_PTR);
typedef HIC (WINAPI *ICOpen_Type)(DWORD, DWORD, UINT);
typedef HIC (WINAPI *ICLocate_Type)(DWORD, DWORD, LPBITMAPINFOHEADER, LPBITMAPINFOHEADER, WORD);
typedef BOOL (WINAPI *DrawDibStart_Type)(HDRAWDIB, LONG);
typedef BOOL (WINAPI *DrawDibStop_Type)(HDRAWDIB);
typedef BOOL (WINAPI *DrawDibEnd_Type)(HDRAWDIB);
typedef HDRAWDIB (WINAPI *DrawDibOpen_Type)(void);
typedef BOOL (WINAPI *DrawDibClose_Type)(HDRAWDIB);
#endif // TRACEALL

extern HIC WINAPI extICGetDisplayFormat(HIC, LPBITMAPINFOHEADER, LPBITMAPINFOHEADER, int, int, int);
//extern HWND WINAPI extMCIWndCreateA(HWND, HINSTANCE, DWORD, LPCTSTR);
extern HWND extMCIWndCreateA(HWND, HINSTANCE, DWORD, LPCTSTR);
DWORD WINAPIV extICDrawBegin(HIC, DWORD, HPALETTE, HWND,  HDC, int, int, int, int, LPBITMAPINFOHEADER, int, int, int, int, DWORD, DWORD);
BOOL WINAPI extDrawDibDraw(HDRAWDIB, HDC, int, int, int, int, LPBITMAPINFOHEADER, LPVOID, int, int, int, int, UINT);
BOOL WINAPI extDrawDibBegin(HDRAWDIB, HDC, int, int, LPBITMAPINFOHEADER, int, int, UINT);
#ifdef TRACEALL
extern LRESULT WINAPI extICSendMessage(HIC, UINT, DWORD_PTR, DWORD_PTR);
extern HIC WINAPI extICOpen(DWORD, DWORD, UINT);
HIC WINAPI extICLocate(DWORD, DWORD, LPBITMAPINFOHEADER, LPBITMAPINFOHEADER, WORD);
BOOL WINAPI extDrawDibStart(HDRAWDIB, LONG);
BOOL WINAPI extDrawDibStop(HDRAWDIB);
BOOL WINAPI extDrawDibEnd(HDRAWDIB);
HDRAWDIB WINAPI extDrawDibOpen(void);
BOOL WINAPI extDrawDibClose(HDRAWDIB);
#endif // TRACEALL

DXWEXTERN ICGetDisplayFormat_Type pICGetDisplayFormat;
DXWEXTERN MCIWndCreateA_Type pMCIWndCreateA;
DXWEXTERN ICDrawBegin_Type pICDrawBegin;
DXWEXTERN DrawDibDraw_Type pDrawDibDraw;
DXWEXTERN DrawDibBegin_Type pDrawDibBegin;
#ifdef TRACEALL
DXWEXTERN ICSendMessage_Type pICSendMessage;
DXWEXTERN ICOpen_Type pICOpen;
DXWEXTERN ICLocate_Type pICLocate;
DXWEXTERN DrawDibStart_Type pDrawDibStart;
DXWEXTERN DrawDibStop_Type pDrawDibStop;
DXWEXTERN DrawDibEnd_Type pDrawDibEnd;
DXWEXTERN DrawDibOpen_Type pDrawDibOpen;
DXWEXTERN DrawDibClose_Type pDrawDibClose;
#endif // TRACEALL

