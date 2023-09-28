#define _CRT_SECURE_NO_DEPRECATE 1

#include <windows.h>
#include <stdio.h>
#include <ddraw.h>
#include <ddrawex.h>
#include <mciavi.h>
#include <digitalv.h>
#include <Vfw.h>	// Video for Windows message definitions
#include <Ime.h>
#include "dxwnd.h"
#include "dxwcore.hpp"

#ifndef DXW_NOTRACES

#ifndef WM_UAHDESTROYWINDOW
#define WM_UAHDESTROYWINDOW			0x0090
#define WM_UAHDRAWMENU				0x0091
#define WM_UAHDRAWMENUITEM			0x0092
#define WM_UAHINITMENU				0x0093
#define WM_UAHMEASUREMENUITEM		0x0094
#define WM_UAHNCPAINTMENUPOPUP		0x0095
#endif // WM_UAHDESTROYWINDOW

#ifndef WM_QUERYAFXWNDPROC
#define WM_QUERYAFXWNDPROC 0x0360
#define WM_SIZEPARENT 0x0361
#define WM_SETMESSAGESTRING 0x0362
#define WM_IDLEUPDATECMDUI 0x0363
#define WM_INITIALUPDATE 0x0364
#define WM_COMMANDHELP 0x0365
#define WM_HELPHITTEST 0x0366
#define WM_EXITHELPMODE 0x0367
#define WM_RECALCPARENT 0x0368
#define WM_SIZECHILD 0x0369
#define WM_KICKIDLE 0x036A
#define WM_QUERYCENTERWND 0x036B
#define WM_DISABLEMODAL 0x036C
#define WM_FLOATSTATUS 0x036D
#define WM_ACTIVATETOPLEVEL 0x036E
#define WM_QUERY3DCONTROLS 0x036F
#define WM_RESERVED_0370 0x0370
#define WM_RESERVED_0371 0x0371
#define WM_RESERVED_0372 0x0372
#define WM_SOCKET_NOTIFY 0x0373
#define WM_SOCKET_DEAD 0x0374
#define WM_POPMESSAGESTRING 0x0375
#define WM_OCC_LOADFROMSTREAM 0x0376
#define WM_OCC_LOADFROMSTORAGE 0x0377
#define WM_OCC_INITNEW 0x0378
#define WM_QUEUE_SENTINEL 0x0379
#define WM_RESERVED_037A 0x037A
#define WM_RESERVED_037B 0x037B
#define WM_RESERVED_037C 0x037C
#define WM_RESERVED_037D 0x037D
#define WM_RESERVED_037E 0x037E
#define WM_RESERVED_037F 0x037F
#endif // WM_QUERYAFXWNDPROC

#ifndef WM_IME_SYSTEM
#define WM_IME_SYSTEM 0x287
#endif // WM_IME_SYSTEM

char *ExplainWinMessage(DWORD c) {
    static char *eb;
    switch(c) {
    case WM_NULL:
        eb = "WM_NULL";
        break;
    case WM_CREATE:
        eb = "WM_CREATE";
        break;
    case WM_DESTROY:
        eb = "WM_DESTROY";
        break;
    case WM_MOVE:
        eb = "WM_MOVE";
        break;
    case WM_SIZE:
        eb = "WM_SIZE";
        break;
    case WM_ACTIVATE:
        eb = "WM_ACTIVATE";
        break;
    case WM_SETFOCUS:
        eb = "WM_SETFOCUS";
        break;
    case WM_KILLFOCUS:
        eb = "WM_KILLFOCUS";
        break;
    case WM_ENABLE:
        eb = "WM_ENABLE";
        break;
    case WM_SETREDRAW:
        eb = "WM_SETREDRAW";
        break;
    case WM_SETTEXT:
        eb = "WM_SETTEXT";
        break;
    case WM_GETTEXT:
        eb = "WM_GETTEXT";
        break;
    case WM_GETTEXTLENGTH:
        eb = "WM_GETTEXTLENGTH";
        break;
    case WM_PAINT:
        eb = "WM_PAINT";
        break;
    case WM_CLOSE:
        eb = "WM_CLOSE";
        break;
    case WM_QUERYENDSESSION:
        eb = "WM_QUERYENDSESSION";
        break;
    case WM_QUERYOPEN:
        eb = "WM_QUERYOPEN";
        break;
    case WM_ENDSESSION:
        eb = "WM_ENDSESSION";
        break;
    case WM_QUIT:
        eb = "WM_QUIT";
        break;
    case WM_ERASEBKGND:
        eb = "WM_ERASEBKGND";
        break;
    case WM_SYSCOLORCHANGE:
        eb = "WM_SYSCOLORCHANGE";
        break;
    case WM_SHOWWINDOW:
        eb = "WM_SHOWWINDOW";
        break;
    case WM_WININICHANGE:
        eb = "WM_WININICHANGE";
        break;
    case WM_DEVMODECHANGE:
        eb = "WM_DEVMODECHANGE";
        break;
    case WM_ACTIVATEAPP:
        eb = "WM_ACTIVATEAPP";
        break;
    case WM_FONTCHANGE:
        eb = "WM_FONTCHANGE";
        break;
    case WM_CANCELMODE:
        eb = "WM_CANCELMODE";
        break;
    case WM_SETCURSOR:
        eb = "WM_SETCURSOR";
        break;
    case WM_MOUSEACTIVATE:
        eb = "WM_MOUSEACTIVATE";
        break;
    case WM_CHILDACTIVATE:
        eb = "WM_CHILDACTIVATE";
        break;
    case WM_QUEUESYNC:
        eb = "WM_QUEUESYNC";
        break;
    case WM_GETMINMAXINFO:
        eb = "WM_GETMINMAXINFO";
        break;
    case WM_PAINTICON:
        eb = "WM_PAINTICON";
        break;
    case WM_ICONERASEBKGND:
        eb = "WM_ICONERASEBKGND";
        break;
    case WM_NEXTDLGCTL:
        eb = "WM_NEXTDLGCTL";
        break;
    case WM_SPOOLERSTATUS:
        eb = "WM_SPOOLERSTATUS";
        break;
    case WM_DRAWITEM:
        eb = "WM_DRAWITEM";
        break;
    case WM_MEASUREITEM:
        eb = "WM_MEASUREITEM";
        break;
    case WM_DELETEITEM:
        eb = "WM_DELETEITEM";
        break;
    case WM_VKEYTOITEM:
        eb = "WM_VKEYTOITEM";
        break;
    case WM_CHARTOITEM:
        eb = "WM_CHARTOITEM";
        break;
    case WM_SETFONT:
        eb = "WM_SETFONT";
        break;
    case WM_GETFONT:
        eb = "WM_GETFONT";
        break;
    case WM_SETHOTKEY:
        eb = "WM_SETHOTKEY";
        break;
    case WM_GETHOTKEY:
        eb = "WM_GETHOTKEY";
        break;
    case WM_QUERYDRAGICON:
        eb = "WM_QUERYDRAGICON";
        break;
    case WM_COMPAREITEM:
        eb = "WM_COMPAREITEM";
        break;
    case WM_GETOBJECT:
        eb = "WM_GETOBJECT";
        break;
    case WM_COMPACTING:
        eb = "WM_COMPACTING";
        break;
    case WM_COMMNOTIFY:
        eb = "WM_COMMNOTIFY";
        break;
    case WM_WINDOWPOSCHANGING:
        eb = "WM_WINDOWPOSCHANGING";
        break;
    case WM_WINDOWPOSCHANGED:
        eb = "WM_WINDOWPOSCHANGED";
        break;
    case WM_POWER:
        eb = "WM_POWER";
        break;
    case WM_COPYDATA:
        eb = "WM_COPYDATA";
        break;
    case WM_CANCELJOURNAL:
        eb = "WM_CANCELJOURNAL";
        break;
    case WM_NOTIFY:
        eb = "WM_NOTIFY";
        break;
    case WM_INPUTLANGCHANGEREQUEST:
        eb = "WM_INPUTLANGCHANGEREQUEST";
        break;
    case WM_INPUTLANGCHANGE:
        eb = "WM_INPUTLANGCHANGE";
        break;
    case WM_TCARD:
        eb = "WM_TCARD";
        break;
    case WM_HELP:
        eb = "WM_HELP";
        break;
    case WM_USERCHANGED:
        eb = "WM_USERCHANGED";
        break;
    case WM_NOTIFYFORMAT:
        eb = "WM_NOTIFYFORMAT";
        break;
    case WM_CONTEXTMENU:
        eb = "WM_CONTEXTMENU";
        break;
    case WM_STYLECHANGING:
        eb = "WM_STYLECHANGING";
        break;
    case WM_STYLECHANGED:
        eb = "WM_STYLECHANGED";
        break;
    case WM_DISPLAYCHANGE:
        eb = "WM_DISPLAYCHANGE";
        break;
    case WM_GETICON:
        eb = "WM_GETICON";
        break;
    case WM_SETICON:
        eb = "WM_SETICON";
        break;
    case WM_NCCREATE:
        eb = "WM_NCCREATE";
        break;
    case WM_NCDESTROY:
        eb = "WM_NCDESTROY";
        break;
    case WM_NCCALCSIZE:
        eb = "WM_NCCALCSIZE";
        break;
    case WM_NCHITTEST:
        eb = "WM_NCHITTEST";
        break;
    case WM_NCPAINT:
        eb = "WM_NCPAINT";
        break;
    case WM_NCACTIVATE:
        eb = "WM_NCACTIVATE";
        break;
    case WM_GETDLGCODE:
        eb = "WM_GETDLGCODE";
        break;
    case WM_MENURBUTTONUP:
        eb = "WM_MENURBUTTONUP";
        break;
    case WM_MENUDRAG:
        eb = "WM_MENUDRAG";
        break;
    case WM_MENUGETOBJECT:
        eb = "WM_MENUGETOBJECT";
        break;
    case WM_UNINITMENUPOPUP:
        eb = "WM_UNINITMENUPOPUP";
        break;
    case WM_MENUCOMMAND:
        eb = "WM_MENUCOMMAND";
        break;
    case WM_CHANGEUISTATE:
        eb = "WM_CHANGEUISTATE";
        break;
    case WM_UPDATEUISTATE:
        eb = "WM_UPDATEUISTATE";
        break;
    case WM_QUERYUISTATE:
        eb = "WM_QUERYUISTATE";
        break;
#ifndef _WIN32_WCE
    case WM_SYNCPAINT:
        eb = "WM_SYNCPAINT";
        break;
#endif
    // legacy stuff found in "Mig Alley"
    case WM_UAHDESTROYWINDOW:
        eb = "WM_UAHDESTROYWINDOW";
        break;
    case WM_UAHDRAWMENU:
        eb = "WM_UAHDRAWMENU";
        break;
    case WM_UAHDRAWMENUITEM:
        eb = "WM_UAHDRAWMENUITEM";
        break;
    case WM_UAHINITMENU:
        eb = "WM_UAHINITMENU";
        break;
    case WM_UAHMEASUREMENUITEM:
        eb = "WM_UAHMEASUREMENUITEM";
        break;
    case WM_UAHNCPAINTMENUPOPUP:
        eb = "WM_UAHNCPAINTMENUPOPUP";
        break;
    case WM_NCMOUSEMOVE:
        eb = "WM_NCMOUSEMOVE";
        break;
    case WM_NCLBUTTONDOWN:
        eb = "WM_NCLBUTTONDOWN";
        break;
    case WM_NCLBUTTONUP:
        eb = "WM_NCLBUTTONUP";
        break;
    case WM_NCLBUTTONDBLCLK:
        eb = "WM_NCLBUTTONDBLCLK";
        break;
    case WM_NCRBUTTONDOWN:
        eb = "WM_NCRBUTTONDOWN";
        break;
    case WM_NCRBUTTONUP:
        eb = "WM_NCRBUTTONUP";
        break;
    case WM_NCRBUTTONDBLCLK:
        eb = "WM_NCRBUTTONDBLCLK";
        break;
    case WM_NCMBUTTONDOWN:
        eb = "WM_NCMBUTTONDOWN";
        break;
    case WM_NCMBUTTONUP:
        eb = "WM_NCMBUTTONUP";
        break;
    case WM_NCMBUTTONDBLCLK:
        eb = "WM_NCMBUTTONDBLCLK";
        break;
#if(_WIN32_WINNT >= 0x0500)
    case WM_NCXBUTTONDOWN:
        eb = "WM_NCXBUTTONDOWN";
        break;
    case WM_NCXBUTTONUP:
        eb = "WM_NCXBUTTONUP";
        break;
    case WM_NCXBUTTONDBLCLK:
        eb = "WM_NCXBUTTONDBLCLK";
        break;
#endif /* _WIN32_WINNT >= 0x0500 */
#if(_WIN32_WINNT >= 0x0501)
    case WM_INPUT:
        eb = "WM_INPUT";
        break;
#endif /* _WIN32_WINNT >= 0x0501 */
    case WM_KEYDOWN:
        eb = "WM_KEYDOWN/FIRST";
        break;
    case WM_KEYUP:
        eb = "WM_KEYUP";
        break;
    case WM_CHAR:
        eb = "WM_CHAR";
        break;
    case WM_DEADCHAR:
        eb = "WM_DEADCHAR";
        break;
    case WM_SYSKEYDOWN:
        eb = "WM_SYSKEYDOWN";
        break;
    case WM_SYSKEYUP:
        eb = "WM_SYSKEYUP";
        break;
    case WM_SYSCHAR:
        eb = "WM_SYSCHAR";
        break;
    case WM_SYSDEADCHAR:
        eb = "WM_SYSDEADCHAR";
        break;
#if(WINVER >= 0x0400)
    case WM_IME_STARTCOMPOSITION:
        eb = "WM_IME_STARTCOMPOSITION";
        break;
    case WM_IME_ENDCOMPOSITION:
        eb = "WM_IME_ENDCOMPOSITION";
        break;
    case WM_IME_COMPOSITION:
        eb = "WM_IME_COMPOSITION/KEYLAST";
        break;
#endif /* WINVER >= 0x0400 */
    case WM_INITDIALOG:
        eb = "WM_INITDIALOG";
        break;
    case WM_COMMAND:
        eb = "WM_COMMAND";
        break;
    case WM_SYSCOMMAND:
        eb = "WM_SYSCOMMAND";
        break;
    case WM_TIMER:
        eb = "WM_TIMER";
        break;
    case WM_HSCROLL:
        eb = "WM_HSCROLL";
        break;
    case WM_VSCROLL:
        eb = "WM_VSCROLL";
        break;
    case WM_INITMENU:
        eb = "WM_INITMENU";
        break;
    case WM_INITMENUPOPUP:
        eb = "WM_INITMENUPOPUP";
        break;
    case WM_MENUSELECT:
        eb = "WM_MENUSELECT";
        break;
    case WM_MENUCHAR:
        eb = "WM_MENUCHAR";
        break;
    case WM_ENTERIDLE:
        eb = "WM_ENTERIDLE";
        break;
    case WM_CTLCOLORMSGBOX:
        eb = "WM_CTLCOLORMSGBOX";
        break;
    case WM_CTLCOLOREDIT:
        eb = "WM_CTLCOLOREDIT";
        break;
    case WM_CTLCOLORLISTBOX:
        eb = "WM_CTLCOLORLISTBOX";
        break;
    case WM_CTLCOLORBTN:
        eb = "WM_CTLCOLORBTN";
        break;
    case WM_CTLCOLORDLG:
        eb = "WM_CTLCOLORDLG";
        break;
    case WM_CTLCOLORSCROLLBAR:
        eb = "WM_CTLCOLORSCROLLBAR";
        break;
    case WM_CTLCOLORSTATIC:
        eb = "WM_CTLCOLORSTATIC";
        break;
    case MN_GETHMENU:
        eb = "MN_GETHMENU";
        break;
    case WM_MOUSEFIRST:
        eb = "WM_MOUSEFIRST/MOUSEMOVE";
        break;
    case WM_LBUTTONDOWN:
        eb = "WM_LBUTTONDOWN";
        break;
    case WM_LBUTTONUP:
        eb = "WM_LBUTTONUP";
        break;
    case WM_LBUTTONDBLCLK:
        eb = "WM_LBUTTONDBLCLK";
        break;
    case WM_RBUTTONDOWN:
        eb = "WM_RBUTTONDOWN";
        break;
    case WM_RBUTTONUP:
        eb = "WM_RBUTTONUP";
        break;
    case WM_RBUTTONDBLCLK:
        eb = "WM_RBUTTONDBLCLK";
        break;
    case WM_MBUTTONDOWN:
        eb = "WM_MBUTTONDOWN";
        break;
    case WM_MBUTTONUP:
        eb = "WM_MBUTTONUP";
        break;
    case WM_MBUTTONDBLCLK:
        eb = "WM_MBUTTONDBLCLK";
        break;
    case WM_MOUSEWHEEL:
        eb = "WM_MOUSEWHEEL";
        break;
    case WM_MOUSEHWHEEL:
        eb = "WM_MOUSEHWHEEL";
        break;
    case WM_PARENTNOTIFY:
        eb = "WM_PARENTNOTIFY";
        break;
    case WM_ENTERMENULOOP:
        eb = "WM_ENTERMENULOOP";
        break;
    case WM_EXITMENULOOP:
        eb = "WM_EXITMENULOOP";
        break;
#if(WINVER >= 0x0400)
    case WM_NEXTMENU:
        eb = "WM_NEXTMENU";
        break;
    case WM_SIZING:
        eb = "WM_SIZING";
        break;
    case WM_CAPTURECHANGED:
        eb = "WM_CAPTURECHANGED";
        break;
    case WM_MOVING:
        eb = "WM_MOVING";
        break;
    case WM_POWERBROADCAST:
        eb = "WM_POWERBROADCAST";
        break;
    case WM_DEVICECHANGE:
        eb = "WM_DEVICECHANGE";
        break;
#endif /* WINVER >= 0x0400 */
    case WM_MDICREATE:
        eb = "WM_MDICREATE";
        break;
    case WM_MDIDESTROY:
        eb = "WM_MDIDESTROY";
        break;
    case WM_MDIACTIVATE:
        eb = "WM_MDIACTIVATE";
        break;
    case WM_MDIRESTORE:
        eb = "WM_MDIRESTORE";
        break;
    case WM_MDINEXT:
        eb = "WM_MDINEXT";
        break;
    case WM_MDIMAXIMIZE:
        eb = "WM_MDIMAXIMIZE";
        break;
    case WM_MDITILE:
        eb = "WM_MDITILE";
        break;
    case WM_MDICASCADE:
        eb = "WM_MDICASCADE";
        break;
    case WM_MDIICONARRANGE:
        eb = "WM_MDIICONARRANGE";
        break;
    case WM_MDIGETACTIVE:
        eb = "WM_MDIGETACTIVE";
        break;
    case WM_MDISETMENU:
        eb = "WM_MDISETMENU";
        break;
    case WM_ENTERSIZEMOVE:
        eb = "WM_ENTERSIZEMOVE";
        break;
    case WM_EXITSIZEMOVE:
        eb = "WM_EXITSIZEMOVE";
        break;
    case WM_DROPFILES:
        eb = "WM_DROPFILES";
        break;
    case WM_MDIREFRESHMENU:
        eb = "WM_MDIREFRESHMENU";
        break;
#if(WINVER >= 0x0400)
    case WM_IME_REPORT:
        eb = "WM_IME_REPORT";
        break;
    case WM_IME_SETCONTEXT:
        eb = "WM_IME_SETCONTEXT";
        break;
    case WM_IME_NOTIFY:
        eb = "WM_IME_NOTIFY";
        break;
    case WM_IME_CONTROL:
        eb = "WM_IME_CONTROL";
        break;
    case WM_IME_COMPOSITIONFULL:
        eb = "WM_IME_COMPOSITIONFULL";
        break;
    case WM_IME_SELECT:
        eb = "WM_IME_SELECT";
        break;
    case WM_IME_CHAR:
        eb = "WM_IME_CHAR";
        break;
    case WM_IME_SYSTEM:
        eb = "WM_IME_SYSTEM";
        break;
    case WM_IME_REQUEST:
        eb = "WM_IME_REQUEST";
        break;
    case WM_IME_KEYDOWN:
        eb = "WM_IME_KEYDOWN";
        break;
    case WM_IME_KEYUP:
        eb = "WM_IME_KEYUP";
        break;
#endif /* WINVER >= 0x0400 */
    case WM_MOUSEHOVER:
        eb = "WM_MOUSEHOVER";
        break;
    case WM_MOUSELEAVE:
        eb = "WM_MOUSELEAVE";
        break;
    case WM_NCMOUSEHOVER:
        eb = "WM_NCMOUSEHOVER";
        break;
    case WM_NCMOUSELEAVE:
        eb = "WM_NCMOUSELEAVE";
        break;
    case WM_DWMCOMPOSITIONCHANGED:
        eb = "WM_DWMCOMPOSITIONCHANGED";
        break;
    case WM_DWMNCRENDERINGCHANGED:
        eb = "WM_DWMNCRENDERINGCHANGED";
        break;
    case WM_DWMCOLORIZATIONCOLORCHANGED:
        eb = "WM_DWMCOLORIZATIONCOLORCHANGED";
        break;
    case WM_DWMWINDOWMAXIMIZEDCHANGE:
        eb = "WM_DWMWINDOWMAXIMIZEDCHANGE";
        break;
    case WM_GETTITLEBARINFOEX:
        eb = "WM_GETTITLEBARINFOEX";
        break;
    case WM_HANDHELDFIRST:
        eb = "WM_HANDHELDFIRST";
        break;
    case WM_HANDHELDLAST:
        eb = "WM_HANDHELDLAST";
        break;
    case WM_PENWINFIRST:
        eb = "WM_PENWINFIRST";
        break;
    // static controls
    case STM_SETICON:
        eb = "STM_SETICON";
        break;
    case STM_GETICON:
        eb = "STM_GETICON";
        break;
    case STM_SETIMAGE:
        eb = "STM_SETIMAGE";
        break;
    case STM_GETIMAGE:
        eb = "STM_GETIMAGE";
        break;
    // button messages
    case BM_GETCHECK:
        eb = "BM_GETCHECK";
        break;
    case BM_SETCHECK:
        eb = "BM_SETCHECK";
        break;
    case BM_GETSTATE:
        eb = "BM_GETSTATE";
        break;
    case BM_SETSTATE:
        eb = "BM_SETSTATE";
        break;
    case BM_SETSTYLE:
        eb = "BM_SETSTYLE";
        break;
    case BM_CLICK:
        eb = "BM_CLICK";
        break;
    case BM_GETIMAGE:
        eb = "BM_GETIMAGE";
        break;
    case BM_SETIMAGE:
        eb = "BM_SETIMAGE";
        break;
    case BM_SETDONTCLICK:
        eb = "BM_SETDONTCLICK";
        break;
    // reported by Wine
    case 0x0401:
        eb = "SB_SETTEXTA/TB_ENABLEBUTTON/...";
        break;
    case 0x0402:
        eb = "SB_GETTEXTA/TB_CHECKBUTTON/...";
        break;
    case 0x0403:
        eb = "SB_GETTEXTLENGTHA/TB_PRESSBUTTON/...";
        break;
    case 0x0404:
        eb = "SB_SETPARTS/TB_HIDEBUTTON/...";
        break;
    case 0x0405:
        eb = "WM_PSD_ENVSTAMPRECT/TB_INDETERMINATE/...";
        break;
    case 0x0406:
        eb = "TB_MARKBUTTON/...";
        break;
    case 0x0407:
        eb = "SB_GETBORDERS/...";
        break;
    case 0x1000:
        eb = "LVM_GETBKCOLOR";
        break;
    case 0x1001:
        eb = "LVM_SETBKCOLOR";
        break;
    case 0x1002:
        eb = "LVM_GETIMAGELIST";
        break;
    case 0x1003:
        eb = "LVM_SETIMAGELIST";
        break;
    case 0x1004:
        eb = "LVM_GETITEMCOUNT";
        break;
    case 0x1005:
        eb = "LVM_GETITEMA";
        break;
    case 0x1006:
        eb = "LVM_SETITEMA";
        break;
    case 0x1007:
        eb = "LVM_INSERTITEMA";
        break;
    case 0x1008:
        eb = "LVM_DELETEITEM";
        break;
    case 0x1009:
        eb = "LVM_DELETEALLITEMS";
        break;
    case 0x100a:
        eb = "LVM_GETCALLBACKMASK";
        break;
    case 0x100b:
        eb = "LVM_SETCALLBACKMASK";
        break;
    case 0x100c:
        eb = "LVM_GETNEXTITEM";
        break;
    case 0x100d:
        eb = "LVM_FINDITEMA";
        break;
    case 0x100e:
        eb = "LVM_GETITEMRECT";
        break;
    case 0x1023:
        eb = "LVM_GETTEXTCOLOR";
        break;
    case 0x1024:
        eb = "LVM_SETTEXTCOLOR";
        break;
    // MCI commands
    case MCI_OPEN:
        eb = "MCI_OPEN";
        break;
    case MCI_CLOSE:
        eb = "MCI_CLOSE";
        break;
    case MCI_ESCAPE:
        eb = "MCI_ESCAPE";
        break;
    case MCI_PLAY:
        eb = "MCI_PLAY";
        break;
    case MCI_SEEK:
        eb = "MCI_SEEK";
        break;
    case MCI_STOP:
        eb = "MCI_STOP";
        break;
    case MCI_PAUSE:
        eb = "MCI_PAUSE";
        break;
    case MCI_INFO:
        eb = "MCI_INFO";
        break;
    case MCI_GETDEVCAPS:
        eb = "MCI_GETDEVCAPS";
        break;
    case MCI_SPIN:
        eb = "MCI_SPIN";
        break;
    case MCI_SET:
        eb = "MCI_SET";
        break;
    case MCI_STEP:
        eb = "MCI_STEP";
        break;
    case MCI_RECORD:
        eb = "MCI_RECORD";
        break;
    case MCI_SYSINFO:
        eb = "MCI_SYSINFO";
        break;
    case MCI_BREAK:
        eb = "MCI_BREAK";
        break;
    case MCI_SAVE:
        eb = "MCI_SAVE";
        break;
    case MCI_STATUS:
        eb = "MCI_STATUS";
        break;
    case MCI_CUE:
        eb = "MCI_CUE";
        break;
    case MCI_REALIZE:
        eb = "MCI_REALIZE";
        break;
    case MCI_WINDOW:
        eb = "MCI_WINDOW";
        break;
    case MCI_PUT:
        eb = "MCI_PUT";
        break;
    case MCI_WHERE:
        eb = "MCI_WHERE";
        break;
    case MCI_FREEZE:
        eb = "MCI_FREEZE";
        break;
    case MCI_UNFREEZE:
        eb = "MCI_UNFREEZE";
        break;
    case MCI_LOAD:
        eb = "MCI_LOAD";
        break;
    case MCI_CUT:
        eb = "MCI_CUT";
        break;
    case MCI_COPY:
        eb = "MCI_COPY";
        break;
    case MCI_PASTE:
        eb = "MCI_PASTE";
        break;
    case MCI_UPDATE:
        eb = "MCI_UPDATE";
        break;
    case MCI_RESUME:
        eb = "MCI_RESUME";
        break;
    case MCI_DELETE:
        eb = "MCI_DELETE";
        break;
    case MM_JOY1MOVE:
        eb = "MM_JOY1MOVE";
        break;
    case MM_JOY2MOVE:
        eb = "MM_JOY2MOVE";
        break;
    case MM_JOY1ZMOVE:
        eb = "MM_JOY1ZMOVE";
        break;
    case MM_JOY2ZMOVE:
        eb = "MM_JOY2ZMOVE";
        break;
    case MM_JOY1BUTTONDOWN:
        eb = "MM_JOY1BUTTONDOWN";
        break;
    case MM_JOY2BUTTONDOWN:
        eb = "MM_JOY2BUTTONDOWN";
        break;
    case MM_JOY1BUTTONUP:
        eb = "MM_JOY1BUTTONUP";
        break;
    case MM_JOY2BUTTONUP:
        eb = "MM_JOY2BUTTONUP";
        break;
    case MM_MCINOTIFY:
        eb = "MM_MCINOTIFY";
        break;
    case MM_WOM_OPEN:
        eb = "MM_WOM_OPEN";
        break;
    case MM_WOM_CLOSE:
        eb = "MM_WOM_CLOSE";
        break;
    case MM_WOM_DONE:
        eb = "MM_WOM_DONE";
        break;
    case MM_WIM_OPEN:
        eb = "MM_WIM_OPEN";
        break;
    case MM_WIM_CLOSE:
        eb = "MM_WIM_CLOSE";
        break;
    case MM_WIM_DATA:
        eb = "MM_WIM_DATA";
        break;
    case MM_MIM_OPEN:
        eb = "MM_MIM_OPEN";
        break;
    case MM_MIM_CLOSE:
        eb = "MM_MIM_CLOSE";
        break;
    case MM_MIM_DATA:
        eb = "MM_MIM_DATA";
        break;
    case MM_MIM_LONGDATA:
        eb = "MM_MIM_LONGDATA";
        break;
    case MM_MIM_ERROR:
        eb = "MM_MIM_ERROR";
        break;
    case MM_MIM_LONGERROR:
        eb = "MM_MIM_LONGERROR";
        break;
    case MM_MOM_OPEN:
        eb = "MM_MOM_OPEN";
        break;
    case MM_MOM_CLOSE:
        eb = "MM_MOM_CLOSE";
        break;
    case MM_MOM_DONE:
        eb = "MM_MOM_DONE";
        break;
    case MM_DRVM_OPEN:
        eb = "MM_DRVM_OPEN";
        break;
    case MM_DRVM_CLOSE:
        eb = "MM_DRVM_CLOSE";
        break;
    case MM_DRVM_DATA:
        eb = "MM_DRVM_DATA";
        break;
    case MM_DRVM_ERROR:
        eb = "MM_DRVM_ERROR";
        break;
    case MM_STREAM_OPEN:
        eb = "MM_STREAM_OPEN";
        break;
    case MM_STREAM_CLOSE:
        eb = "MM_STREAM_CLOSE";
        break;
    case MM_STREAM_DONE:
        eb = "MM_STREAM_DONE";
        break;
    case MM_STREAM_ERROR:
        eb = "MM_STREAM_ERROR";
        break;
    case MM_MOM_POSITIONCB:
        eb = "MM_MOM_POSITIONCB";
        break;
    case MM_MCISIGNAL:
        eb = "MM_MCISIGNAL";
        break;
    case MM_MIM_MOREDATA:
        eb = "MM_MIM_MOREDATA";
        break;
    // Video for Windows
    case ICM_SETSTATE:
        eb = "ICM_SETSTATE";
        break;
    case ICM_GETINFO:
        eb = "ICM_GETINFO";
        break;
    case ICM_CONFIGURE:
        eb = "ICM_CONFIGURE";
        break;
    case ICM_ABOUT:
        eb = "ICM_ABOUT";
        break;
    case ICM_GETDEFAULTQUALITY:
        eb = "ICM_GETDEFAULTQUALITY";
        break;
    case ICM_GETQUALITY:
        eb = "ICM_GETQUALITY";
        break;
    case ICM_SETQUALITY:
        eb = "ICM_SETQUALITY";
        break;
    case ICM_SET:
        eb = "ICM_SET";
        break;
    case ICM_GET:
        eb = "ICM_GET";
        break;
    case ICM_FRAMERATE:
        eb = "ICM_FRAMERATE";
        break;
    case ICM_KEYFRAMERATE:
        eb = "ICM_KEYFRAMERATE";
        break;
    case ICM_COMPRESS_GET_FORMAT:
        eb = "ICM_COMPRESS_GET_FORMAT";
        break;
    case ICM_COMPRESS_GET_SIZE:
        eb = "ICM_COMPRESS_GET_SIZE";
        break;
    case ICM_COMPRESS_QUERY:
        eb = "ICM_COMPRESS_QUERY";
        break;
    case ICM_COMPRESS_BEGIN:
        eb = "ICM_COMPRESS_BEGIN";
        break;
    case ICM_COMPRESS:
        eb = "ICM_COMPRESS";
        break;
    case ICM_COMPRESS_END:
        eb = "ICM_COMPRESS_END";
        break;
    case ICM_DECOMPRESS_GET_FORMAT:
        eb = "ICM_DECOMPRESS_GET_FORMAT";
        break;
    case ICM_DECOMPRESS_QUERY:
        eb = "ICM_DECOMPRESS_QUERY";
        break;
    case ICM_DECOMPRESS_BEGIN:
        eb = "ICM_DECOMPRESS_BEGIN";
        break;
    case ICM_DECOMPRESS:
        eb = "ICM_DECOMPRESS";
        break;
    case ICM_DECOMPRESS_END:
        eb = "ICM_DECOMPRESS_END";
        break;
    case ICM_DECOMPRESS_SET_PALETTE:
        eb = "ICM_DECOMPRESS_SET_PALETTE";
        break;
    case ICM_DECOMPRESS_GET_PALETTE:
        eb = "ICM_DECOMPRESS_GET_PALETTE";
        break;
    case ICM_DRAW_QUERY:
        eb = "ICM_DRAW_QUERY";
        break;
    case ICM_DRAW_BEGIN:
        eb = "ICM_DRAW_BEGIN";
        break;
    case ICM_DRAW_GET_PALETTE:
        eb = "ICM_DRAW_GET_PALETTE";
        break;
    case ICM_DRAW_START:
        eb = "ICM_DRAW_START";
        break;
    case ICM_DRAW_STOP:
        eb = "ICM_DRAW_STOP";
        break;
    case ICM_DRAW_END:
        eb = "ICM_DRAW_END";
        break;
    case ICM_DRAW_GETTIME:
        eb = "ICM_DRAW_GETTIME";
        break;
    case ICM_DRAW:
        eb = "ICM_DRAW";
        break;
    case ICM_DRAW_WINDOW:
        eb = "ICM_DRAW_WINDOW";
        break;
    case ICM_DRAW_SETTIME:
        eb = "ICM_DRAW_SETTIME";
        break;
    case ICM_DRAW_REALIZE:
        eb = "ICM_DRAW_REALIZE";
        break;
    case ICM_DRAW_FLUSH:
        eb = "ICM_DRAW_FLUSH";
        break;
    case ICM_DRAW_RENDERBUFFER:
        eb = "ICM_DRAW_RENDERBUFFER";
        break;
    case ICM_DRAW_START_PLAY:
        eb = "ICM_DRAW_START_PLAY";
        break;
    case ICM_DRAW_STOP_PLAY:
        eb = "ICM_DRAW_STOP_PLAY";
        break;
    case ICM_DRAW_SUGGESTFORMAT:
        eb = "ICM_DRAW_SUGGESTFORMAT";
        break;
    case ICM_DRAW_CHANGEPALETTE:
        eb = "ICM_DRAW_CHANGEPALETTE";
        break;
    case ICM_GETBUFFERSWANTED:
        eb = "ICM_GETBUFFERSWANTED";
        break;
    case ICM_GETDEFAULTKEYFRAMERATE:
        eb = "ICM_GETDEFAULTKEYFRAMERATE";
        break;
    case ICM_DECOMPRESSEX_BEGIN:
        eb = "ICM_DECOMPRESSEX_BEGIN";
        break;
    case ICM_DECOMPRESSEX_QUERY:
        eb = "ICM_DECOMPRESSEX_QUERY";
        break;
    case ICM_DECOMPRESSEX:
        eb = "ICM_DECOMPRESSEX";
        break;
    case ICM_DECOMPRESSEX_END:
        eb = "ICM_DECOMPRESSEX_END";
        break;
    case ICM_COMPRESS_FRAMES_INFO:
        eb = "ICM_COMPRESS_FRAMES_INFO";
        break;
    case ICM_SET_STATUS_PROC:
        eb = "ICM_SET_STATUS_PROC";
        break;
    case MCIWNDM_GETDEVICEID:
        eb = "MCIWNDM_GETDEVICEID";
        break;
    case MCIWNDM_GETSTART:
        eb = "MCIWNDM_GETSTART";
        break;
    case MCIWNDM_GETLENGTH:
        eb = "MCIWNDM_GETLENGTH";
        break;
    case MCIWNDM_GETEND:
        eb = "MCIWNDM_GETEND";
        break;
    case MCIWNDM_EJECT:
        eb = "MCIWNDM_EJECT";
        break;
    case MCIWNDM_SETZOOM:
        eb = "MCIWNDM_SETZOOM";
        break;
    case MCIWNDM_GETZOOM:
        eb = "MCIWNDM_GETZOOM";
        break;
    case MCIWNDM_SETVOLUME:
        eb = "MCIWNDM_SETVOLUME";
        break;
    case MCIWNDM_GETVOLUME:
        eb = "MCIWNDM_GETVOLUME";
        break;
    case MCIWNDM_SETSPEED:
        eb = "MCIWNDM_SETSPEED";
        break;
    case MCIWNDM_GETSPEED:
        eb = "MCIWNDM_GETSPEED";
        break;
    case MCIWNDM_SETREPEAT:
        eb = "MCIWNDM_SETREPEAT";
        break;
    case MCIWNDM_GETREPEAT:
        eb = "MCIWNDM_GETREPEAT";
        break;
    case MCIWNDM_REALIZE:
        eb = "MCIWNDM_REALIZE";
        break;
    case MCIWNDM_VALIDATEMEDIA:
        eb = "MCIWNDM_VALIDATEMEDIA";
        break;
    case MCIWNDM_PLAYFROM:
        eb = "MCIWNDM_PLAYFROM";
        break;
    case MCIWNDM_PLAYTO:
        eb = "MCIWNDM_PLAYTO";
        break;
    case MCIWNDM_GETPALETTE:
        eb = "MCIWNDM_GETPALETTE";
        break;
    case MCIWNDM_SETPALETTE:
        eb = "MCIWNDM_SETPALETTE";
        break;
    case MCIWNDM_SETTIMERS:
        eb = "MCIWNDM_SETTIMERS";
        break;
    case MCIWNDM_SETACTIVETIMER:
        eb = "MCIWNDM_SETACTIVETIMER";
        break;
    case MCIWNDM_SETINACTIVETIMER:
        eb = "MCIWNDM_SETINACTIVETIMER";
        break;
    case MCIWNDM_GETACTIVETIMER:
        eb = "MCIWNDM_GETACTIVETIMER";
        break;
    case MCIWNDM_GETINACTIVETIMER:
        eb = "MCIWNDM_GETINACTIVETIMER";
        break;
    case MCIWNDM_CHANGESTYLES:
        eb = "MCIWNDM_CHANGESTYLES";
        break;
    case MCIWNDM_GETSTYLES:
        eb = "MCIWNDM_GETSTYLES";
        break;
    case MCIWNDM_GETALIAS:
        eb = "MCIWNDM_GETALIAS";
        break;
    case MCIWNDM_PLAYREVERSE:
        eb = "MCIWNDM_PLAYREVERSE";
        break;
    case MCIWNDM_GET_SOURCE:
        eb = "MCIWNDM_GET_SOURCE";
        break;
    case MCIWNDM_PUT_SOURCE:
        eb = "MCIWNDM_PUT_SOURCE";
        break;
    case MCIWNDM_GET_DEST:
        eb = "MCIWNDM_GET_DEST";
        break;
    case MCIWNDM_PUT_DEST:
        eb = "MCIWNDM_PUT_DEST";
        break;
    case MCIWNDM_CAN_PLAY:
        eb = "MCIWNDM_CAN_PLAY";
        break;
    case MCIWNDM_CAN_WINDOW:
        eb = "MCIWNDM_CAN_WINDOW";
        break;
    case MCIWNDM_CAN_RECORD:
        eb = "MCIWNDM_CAN_RECORD";
        break;
    case MCIWNDM_CAN_SAVE:
        eb = "MCIWNDM_CAN_SAVE";
        break;
    case MCIWNDM_CAN_EJECT:
        eb = "MCIWNDM_CAN_EJECT";
        break;
    case MCIWNDM_CAN_CONFIG:
        eb = "MCIWNDM_CAN_CONFIG";
        break;
    case MCIWNDM_PALETTEKICK:
        eb = "MCIWNDM_PALETTEKICK";
        break;
    case MCIWNDM_OPENINTERFACE:
        eb = "MCIWNDM_OPENINTERFACE";
        break;
    case MCIWNDM_SETOWNER:
        eb = "MCIWNDM_SETOWNER";
        break;
    case MCIWNDM_SENDSTRINGA:
        eb = "MCIWNDM_SENDSTRINGA";
        break;
    case MCIWNDM_GETPOSITIONA:
        eb = "MCIWNDM_GETPOSITIONA";
        break;
    case MCIWNDM_GETMODEA:
        eb = "MCIWNDM_GETMODEA";
        break;
    case MCIWNDM_SETTIMEFORMATA:
        eb = "MCIWNDM_SETTIMEFORMATA";
        break;
    case MCIWNDM_GETTIMEFORMATA:
        eb = "MCIWNDM_GETTIMEFORMATA";
        break;
    case MCIWNDM_GETFILENAMEA:
        eb = "MCIWNDM_GETFILENAMEA";
        break;
    case MCIWNDM_GETDEVICEA:
        eb = "MCIWNDM_GETDEVICEA";
        break;
    case MCIWNDM_GETERRORA:
        eb = "MCIWNDM_GETERRORA";
        break;
    case MCIWNDM_NEWA:
        eb = "MCIWNDM_NEWA";
        break;
    case MCIWNDM_RETURNSTRINGA:
        eb = "MCIWNDM_RETURNSTRINGA";
        break;
    case MCIWNDM_OPENA:
        eb = "MCIWNDM_OPENA";
        break;
    case MCIWNDM_SENDSTRINGW:
        eb = "MCIWNDM_SENDSTRINGW/NOTIFYPOS";
        break;
    case MCIWNDM_GETPOSITIONW:
        eb = "MCIWNDM_GETPOSITIONW/NOTIFYSIZE";
        break;
    case MCIWNDM_GETMODEW:
        eb = "MCIWNDM_GETMODEW";
        break;
    case MCIWNDM_SETTIMEFORMATW:
        eb = "MCIWNDM_SETTIMEFORMATW";
        break;
    case MCIWNDM_GETTIMEFORMATW:
        eb = "MCIWNDM_GETTIMEFORMATW";
        break;
    case MCIWNDM_GETFILENAMEW:
        eb = "MCIWNDM_GETFILENAMEW";
        break;
    case MCIWNDM_GETDEVICEW:
        eb = "MCIWNDM_GETDEVICEW";
        break;
    case MCIWNDM_GETERRORW:
        eb = "MCIWNDM_GETERRORW";
        break;
    case MCIWNDM_NEWW:
        eb = "MCIWNDM_NEWW";
        break;
    case MCIWNDM_RETURNSTRINGW:
        eb = "MCIWNDM_RETURNSTRINGW";
        break;
    case MCIWNDM_OPENW:
        eb = "MCIWNDM_OPENW";
        break;
    case MCIWNDM_NOTIFYMODE:
        eb = "MCIWNDM_NOTIFYMODE";
        break;
    // overlapped with MCIWNDM_SENDSTRINGW and MCIWNDM_GETPOSITIONW
    //case MCIWNDM_NOTIFYPOS:			eb="MCIWNDM_NOTIFYPOS"; break;
    //case MCIWNDM_NOTIFYSIZE:		eb="MCIWNDM_NOTIFYSIZE"; break;
    case MCIWNDM_NOTIFYMEDIA:
        eb = "MCIWNDM_NOTIFYMEDIA";
        break;
    case MCIWNDM_NOTIFYERROR:
        eb = "MCIWNDM_NOTIFYERROR";
        break;
    case MCIWNDF_NOTIFYSIZE:
        eb = "MCIWNDF_NOTIFYSIZE";
        break;
    // AFX private messages
    case WM_QUERYAFXWNDPROC:
        eb = "WM_QUERYAFXWNDPROC";
        break;
    case WM_SIZEPARENT:
        eb = "WM_SIZEPARENT";
        break;
    case WM_SETMESSAGESTRING:
        eb = "WM_SETMESSAGESTRING";
        break;
    case WM_IDLEUPDATECMDUI:
        eb = "WM_IDLEUPDATECMDUI";
        break;
    case WM_INITIALUPDATE:
        eb = "WM_INITIALUPDATE";
        break;
    case WM_COMMANDHELP:
        eb = "WM_COMMANDHELP";
        break;
    case WM_HELPHITTEST:
        eb = "WM_HELPHITTEST";
        break;
    case WM_EXITHELPMODE:
        eb = "WM_EXITHELPMODE";
        break;
    case WM_RECALCPARENT:
        eb = "WM_RECALCPARENT";
        break;
    case WM_SIZECHILD:
        eb = "WM_SIZECHILD";
        break;
    case WM_KICKIDLE:
        eb = "WM_KICKIDLE";
        break;
    case WM_QUERYCENTERWND:
        eb = "WM_QUERYCENTERWND";
        break;
    case WM_DISABLEMODAL:
        eb = "WM_DISABLEMODAL";
        break;
    case WM_FLOATSTATUS:
        eb = "WM_FLOATSTATUS";
        break;
    case WM_ACTIVATETOPLEVEL:
        eb = "WM_ACTIVATETOPLEVEL";
        break;
    case WM_QUERY3DCONTROLS:
        eb = "WM_QUERY3DCONTROLS";
        break;
    case WM_RESERVED_0370:
        eb = "WM_RESERVED_0370";
        break;
    case WM_RESERVED_0371:
        eb = "WM_RESERVED_0371";
        break;
    case WM_RESERVED_0372:
        eb = "WM_RESERVED_0372";
        break;
    case WM_SOCKET_NOTIFY:
        eb = "WM_SOCKET_NOTIFY";
        break;
    case WM_SOCKET_DEAD:
        eb = "WM_SOCKET_DEAD";
        break;
    case WM_POPMESSAGESTRING:
        eb = "WM_POPMESSAGESTRING";
        break;
    case WM_OCC_LOADFROMSTREAM:
        eb = "WM_OCC_LOADFROMSTREAM";
        break;
    case WM_OCC_LOADFROMSTORAGE:
        eb = "WM_OCC_LOADFROMSTORAGE";
        break;
    case WM_OCC_INITNEW:
        eb = "WM_OCC_INITNEW";
        break;
    case WM_QUEUE_SENTINEL:
        eb = "WM_QUEUE_SENTINEL";
        break;
    case WM_RESERVED_037A:
        eb = "WM_RESERVED_037A";
        break;
    case WM_RESERVED_037B:
        eb = "WM_RESERVED_037B";
        break;
    case WM_RESERVED_037C:
        eb = "WM_RESERVED_037C";
        break;
    case WM_RESERVED_037D:
        eb = "WM_RESERVED_037D";
        break;
    case WM_RESERVED_037E:
        eb = "WM_RESERVED_037E";
        break;
    case WM_RESERVED_037F:
        eb = "WM_RESERVED_037F";
        break;
    // unknown
    default:
        eb = "???";
        break;
    }
    return eb;
}

#endif // DXW_NOTRACES
