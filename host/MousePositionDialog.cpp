#include "stdafx.h"
#include "dxwndhost.h"
#include "MousePositionDialog.h"
#include "bmpext.h"

#define IDMousePosTIMER 4

BEGIN_MESSAGE_MAP(CMousePositionDialog, CDialog)
    //ON_COMMAND(IDCANCEL, OnOK)
    ON_WM_TIMER()
END_MESSAGE_MAP()

// CMousePositionDialog dialog

IMPLEMENT_DYNAMIC(CMousePositionDialog, CDialog)

CMousePositionDialog::CMousePositionDialog()
    : CDialog(CMousePositionDialog::IDD) {
}

CMousePositionDialog::~CMousePositionDialog() {
    KillTimer(IDMousePosTIMER);
}

void CMousePositionDialog::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}

typedef enum {
    PIC_VIEWFINDER_CURSORPOS = 0,
    PIC_VIEWFINDER_MESSAGEPT,
    PIC_VIEWFINDER_WINPROC,
    PIC_VIEWFINDER_MESSAGEHOOK
};

// CMousePositionDialog message handlers

void CMousePositionDialog::OnTimer(UINT_PTR nIDEvent) {
    CStatic *pic = (CStatic *)this->GetDlgItem(IDC_MOUSEVIEWPANEL);
    DXWNDSTATUS DxWndStatus;
    DWORD DxStatus;
    RECT Rect;
    POINT pt;
    int w, h, x, y;
    CDC *dc;
    RGBQUAD White = {0xFF, 0xFF, 0xFF, 0x00};
    RGBQUAD Red   = {0x00, 0x00, 0xFF, 0x00};
    RGBQUAD Green = {0x00, 0xFF, 0x00, 0x00};
    RGBQUAD Blue  = {0xFF, 0x00, 0x00, 0x00};
    // get panel size
    this->GetClientRect(&Rect);
    w = Rect.right;
    h = Rect.bottom;
    // if no task running, return
    DxStatus = GetHookStatus(&DxWndStatus);
    if(DxStatus != DXW_RUNNING) return;
    // if error, return
    dc = pic->GetDC();
    if(!dc) return;
    // check for 0 DIV
    if (DxWndStatus.Width == 0) return;
    if (DxWndStatus.Height == 0) return;
    // clear background
    if((iCount % 20) == 0) dib_Background.Draw(dc, CRect(0, 0, w, h), CRect(0, 0, 1, 1));
    iCount = (iCount + 1) % 20;
    // draw MouseHook viewfinder ...
    if((DxWndStatus.MsgHookX != (short) - 1) && (DxWndStatus.MsgHookY != (short) - 1)) {
        x = (DxWndStatus.MsgHookX * w) / DxWndStatus.Width;
        y = (DxWndStatus.MsgHookY * h) / DxWndStatus.Height;
        if(x < 0) x = 0;
        if(x >= w) x = w - 1;
        if(y < 0) y = 0;
        if(y >= h) y = h - 1;
        pt.x = x - 16;
        pt.y = y - 16;
        m_imageList.Draw(dc, PIC_VIEWFINDER_MESSAGEHOOK, pt, ILD_NORMAL);
    }
    // draw GetCursorPos viewfinder ...
    if((DxWndStatus.CursorX != (short) - 1) && (DxWndStatus.CursorY != (short) - 1)) {
        x = (DxWndStatus.CursorX * w) / DxWndStatus.Width;
        y = (DxWndStatus.CursorY * h) / DxWndStatus.Height;
        if(x < 0) x = 0;
        if(x >= w) x = w - 1;
        if(y < 0) y = 0;
        if(y >= h) y = h - 1;
        pt.x = x - 16;
        pt.y = y - 16;
        m_imageList.Draw(dc, PIC_VIEWFINDER_CURSORPOS, pt, ILD_NORMAL);
    }
    // draw Message viewfinder ...
    if((DxWndStatus.MessageX != (short) - 1) && (DxWndStatus.MessageY != (short) - 1)) {
        x = (DxWndStatus.MessageX * w) / DxWndStatus.Width;
        y = (DxWndStatus.MessageY * h) / DxWndStatus.Height;
        if(x < 0) x = 0;
        if(x >= w) x = w - 1;
        if(y < 0) y = 0;
        if(y >= h) y = h - 1;
        pt.x = x - 16;
        pt.y = y - 16;
        m_imageList.Draw(dc, PIC_VIEWFINDER_MESSAGEPT, pt, ILD_NORMAL);
    }
    // draw Message viewfinder ...
    if((DxWndStatus.WinProcX != (short) - 1) && (DxWndStatus.WinProcY != (short) - 1)) {
        x = (DxWndStatus.WinProcX * w) / DxWndStatus.Width;
        y = (DxWndStatus.WinProcY * h) / DxWndStatus.Height;
        if(x < 0) x = 0;
        if(x >= w) x = w - 1;
        if(y < 0) y = 0;
        if(y >= h) y = h - 1;
        pt.x = x - 16;
        pt.y = y - 16;
        m_imageList.Draw(dc, PIC_VIEWFINDER_WINPROC, pt, ILD_NORMAL);
    }
    // cleanup
    pic->ReleaseDC(dc);
    dc->~CDC();
    //dib_Background.Invalidate();
}

BOOL CMousePositionDialog::OnInitDialog() {
    RGBQUAD White = {0xFF, 0xFF, 0xFF, 0x00};
    RGBQUAD Red   = {0x00, 0x00, 0xFF, 0x00};
    RGBQUAD Green = {0x00, 0xFF, 0x00, 0x00};
    RGBQUAD Blue  = {0xFF, 0x00, 0x00, 0x00};
    CBitmap bm;
    CDC *dc;
    RECT Rect;
    int w, h;
    CDialog::OnInitDialog();
    iCount = 0;
    CStatic *pic = (CStatic *)this->GetDlgItem(IDC_MOUSEVIEWPANEL);
    dc = pic->GetDC();
    // get panel size
    this->GetClientRect(&Rect);
    w = Rect.right;
    h = Rect.bottom;
    KillTimer(IDMousePosTIMER); // just in case ...
    SetTimer(IDMousePosTIMER, 40, NULL);
    if(!dib_Background.CreateEmpty(1, 1, 32, White)) MessageBox("CreateEmpty error", "error", 0);
    dib_Background.Draw(dc, CRect(0, 0, w, h), CRect(0, 0, 1, 1));
    m_imageList.Create(31, 31, ILC_MASK | ILC_COLOR24, 0, 0);
    if(!bm.LoadBitmap(IDB_FINDER_RED)) MessageBox("LoadBitmap IDB_FINDER_RED failed");
    m_imageList.Add(&bm, RGB(255, 255, 255));
    if(!bm.LoadBitmap(IDB_FINDER_GREEN)) MessageBox("LoadBitmap IDB_FINDER_GREEN failed");
    m_imageList.Add(&bm, RGB(255, 255, 255));
    if(!bm.LoadBitmap(IDB_FINDER_BLUE)) MessageBox("LoadBitmap IDB_FINDER_BLUE failed");
    m_imageList.Add(&bm, RGB(255, 255, 255));
    if(!bm.LoadBitmap(IDB_FINDER_YELLOW)) MessageBox("LoadBitmap IDB_FINDER_YELLOW failed");
    m_imageList.Add(&bm, RGB(255, 255, 255));
    return TRUE;  // return TRUE unless you set the focus to a control
}

void CMousePositionDialog::OnOK() {
    KillTimer(IDMousePosTIMER);
    dib_Background.Invalidate();
    m_imageList.DeleteImageList();
    CDialog::OnOK();
}

