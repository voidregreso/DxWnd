#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE 1

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhelper.h"

extern HHOOK hMouseHook;
extern HHOOK hKeyboardHook;

/*
    typedef struct tagMSG {
    HWND   hwnd;			// A handle to the window whose window procedure receives the message. This member is NULL when the message is a thread message.
    UINT   message;		// The message identifier. Applications can only use the low word; the high word is reserved by the system.
    WPARAM wParam;        // Additional information about the message. The exact meaning depends on the value of the message member.
    LPARAM lParam;        // Additional information about the message. The exact meaning depends on the value of the message member.
    DWORD  time;			// The time at which the message was posted.
    POINT  pt;            // The cursor position, in screen coordinates, when the message was posted.
    } MSG, *PMSG, *LPMSG;
*/

LRESULT CALLBACK KeyboardHook(int code, WPARAM wParam, LPARAM lParam) {
    static BOOL fShiftPressed = FALSE;
    BOOL fHandled = FALSE;
    if((code >= HC_ACTION) && dxw.bActive) {
        KBDLLHOOKSTRUCT *pkbdllhook = (KBDLLHOOKSTRUCT *)lParam;
        switch (wParam) {
        case WM_SYSKEYDOWN:
            switch (pkbdllhook->vkCode) {
            case VK_LSHIFT:
            case VK_RSHIFT:
                // the user pressed the shift key
                fShiftPressed = TRUE;
                break;
            case VK_TAB:
                if (pkbdllhook->flags & LLKHF_ALTDOWN) {
                    // the user pressed Alt+Tab, execute AltTab hotkey handler
                    fHandled = TRUE;
                }
                break;
            case VK_ESCAPE:
                if (pkbdllhook->flags & LLKHF_ALTDOWN) {
                    // the user pressed Escape, end the AltTab container
                    // window without switching the selected window
                    fHandled = TRUE;
                }
            }
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            switch (pkbdllhook->vkCode) {
            case VK_LMENU:
            case VK_RMENU:
                // the user let go of the Alt key, end the AltTab container
                // window switching to the selected window
                break;
            case VK_LSHIFT:
            case VK_RSHIFT:
                // the user released the shift key
                fShiftPressed = FALSE;
                break;
            }
            break;
        }
    }
    return fHandled ? TRUE : CallNextHookEx(hKeyboardHook, code, wParam, lParam);
}

LRESULT CALLBACK MessageHook(int code, WPARAM wParam, LPARAM lParam) {
    static BOOL bSizeMoving = FALSE;
    static BOOL bFrameClick = FALSE;
    if(code == HC_ACTION) {
        if(dxw.IsFullScreen()) {
            MSG *msg;
            msg = (MSG *)lParam;
            OutTraceC("MessageHook: hwnd=%#x message=%d(%s) remove=%d params=(L:%#x,R:%#x) pt=(%d,%d)\n",
                      msg->hwnd, msg->message, ExplainWinMessage(msg->message), wParam, msg->lParam, msg->wParam, msg->pt.x, msg->pt.y);
            switch(msg->message) {
            case WM_ENTERSIZEMOVE:
                bSizeMoving = TRUE;
                break;
            case WM_NCLBUTTONDOWN:
            case WM_NCRBUTTONDOWN:
            case WM_NCMBUTTONDOWN:
                bFrameClick = TRUE;
                break;
            }
            // do not try to fix coordinates for points outside the client area!!
            // in theory, that should primarily depend on the message type, but this is the way it works ...
            // do not do the fixing also when in sizemove mode
            RECT client;
            client = dxw.GetUnmappedScreenRect();
            if ((bSizeMoving) ||
                    (bFrameClick) ||
                    (msg->pt.x < client.left)		||
                    (msg->pt.y < client.top)		||
                    (msg->pt.x > client.right)		||
                    (msg->pt.y > client.bottom)) {
                // do nothing
            } else {
                // fix the message point coordinates
                POINT upleft = {0, 0};
                POINT pt;
                HWND hwnd;
                // v2.03.36: offset to be calculated from target window
                // v2-03.70: unless is a thread message (hwnd == NULL) !!
                hwnd = (msg->hwnd) ? msg->hwnd : dxw.GethWnd();
                (*pClientToScreen)(hwnd, &upleft);
                pt = msg->pt;
                pt = dxw.SubCoordinates(pt, upleft);
                pt = dxw.FixCursorPos(pt);
                msg->pt = pt;
                // beware: needs fix for mousewheel?
                // if NOMOUSEPROC fix point x,y coordinates only to NON MOUSE messages that won't be fixed by WindowProc hooker
                // NOMOUSEPROC is required by "X-Files", must be off for "Star Trek Armada" !!!
                // if((msg->message <= WM_MOUSELAST) && (msg->message >= WM_MOUSEFIRST) && !(dxw.dwFlags6 & NOMOUSEPROC)) msg->lParam = MAKELPARAM(pt.x, pt.y);
                if((msg->message <= WM_MOUSELAST) && (msg->message >= WM_MOUSEFIRST)) msg->lParam = MAKELPARAM(pt.x, pt.y);
                OutTraceC("MessageHook: fixed hwnd=%#x lparam/pt=(%d,%d)\n", msg->hwnd, pt.x, pt.y);
                GetHookInfo()->MsgHookX = (short)pt.x;
                GetHookInfo()->MsgHookY = (short)pt.y;
            }
#if 0
            if(dxw.MustFixCoordinates) {
                MSG *msg;
                msg = (MSG *)lParam;
                POINT point = msg->pt;
                if(dxw.dwFlags12 & FIXMOUSELPARAM) {
                    switch(msg->message) {
                    case WM_MOUSEMOVE:
                    case WM_LBUTTONDOWN:
                    case WM_LBUTTONUP:
                    case WM_LBUTTONDBLCLK:
                    case WM_RBUTTONDOWN:
                    case WM_RBUTTONUP:
                    case WM_RBUTTONDBLCLK:
                    case WM_MBUTTONDOWN:
                    case WM_MBUTTONUP:
                    case WM_MBUTTONDBLCLK:
                    case WM_MOUSEWHEEL:
                    case WM_XBUTTONDOWN:
                    case WM_XBUTTONUP:
                    case WM_XBUTTONDBLCLK:
                    case WM_MOUSEHWHEEL:
                        msg->lParam = MAKELPARAM(point.x, point.y);
                        OutTraceC("MessageHook: fixed lParam=(%d,%d)\n", (short)LOWORD(msg->lParam), (short)HIWORD(msg->lParam));
                        break;
                    }
                }
            }
#endif
            switch(msg->message) {
            case WM_EXITSIZEMOVE:
                bSizeMoving = FALSE;
                break;
            case WM_NCLBUTTONUP:
            case WM_NCRBUTTONUP:
            case WM_NCMBUTTONUP:
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
                bFrameClick = FALSE;
                break;
            }
        }
    }
    return CallNextHookEx(hMouseHook, code, wParam, lParam);
}
