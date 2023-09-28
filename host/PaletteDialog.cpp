// StatusDialog.cpp : implementation file
//

#include "stdafx.h"
#include "dxwndhost.h"
#include "PaletteDialog.h"

// CPaletteDialog dialog

BEGIN_MESSAGE_MAP(CPaletteDialog, CDialog)
    ON_COMMAND(IDCANCEL, OnOK)
    ON_WM_TIMER()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CPaletteDialog, CDialog)

CPaletteDialog::CPaletteDialog(CWnd *pParent /*=NULL*/)
    : CDialog(CPaletteDialog::IDD, pParent) {
}

CPaletteDialog::~CPaletteDialog() {
}

void CPaletteDialog::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}


#define IDPaletteTIMER 2

// CPaletteDialog message handlers

void CPaletteDialog::OnTimer(UINT_PTR nIDEvent) {
    DXWNDSTATUS DxWndStatus;
    int DxStatus;
    //extern PRIVATEMAP *pTitles;
    extern TARGETMAP *pTargets;
    RECT Rect;
    int h, w;
    static BOOL PaletteUpdated = FALSE;
    char sMsg[1024];
    RGBQUAD rgbq;
    CDC *dc;
    WARNTIMERS;
    DxStatus = GetHookStatus(&DxWndStatus);
    myWin->GetClientRect(&Rect);
    h = Rect.bottom - Rect.top;
    w = Rect.right - Rect.left;
    if((h == 0) || (w == 0)) return;
    if(DxStatus == DXW_RUNNING) {
        for(int row = 0; row < 16; row++) {
            for(int col = 0; col < 16; col++) {
                PALETTEENTRY *pe = &DxWndStatus.Palette[16 * row + col];
                rgbq.rgbBlue = pe->peBlue;
                rgbq.rgbGreen = pe->peGreen;
                rgbq.rgbRed = pe->peRed;
                rgbq.rgbReserved = 0;
                dib_Palette.SetPixel(col, row, rgbq);
            }
        }
        PALETTEENTRY *pe = &DxWndStatus.Palette[0];
        sprintf(sMsg, "rgb=%02.2x-%02.2x-%02.2x", pe->peRed, pe->peGreen, pe->peBlue);
        this->SetDlgItemTextA(IDC_PALETTE_HELP, sMsg);
        PaletteUpdated = TRUE;
    } else {
        if (PaletteUpdated) dib_Palette.ReadFromResource(IDB_PALETTE);
        PaletteUpdated = FALSE;
    }
    // get pixel info
    POINT Point;
    COLORREF Color;
    ::GetCursorPos(&Point);
    dc = this->GetDesktopWindow()->GetDC();
    Color = dc->GetPixel(Point);
    this->ReleaseDC(dc);
    sprintf(sMsg, "cursor (x,y)=(%d,%d)\ncolor=(%08.8X)",
            Point.x, Point.y, Color);
    // find matching entry, if any ...
    if(PaletteUpdated) {
        BOOL bMatched = FALSE;
        int iPalIndex;
        PALETTEENTRY peColor;
        peColor.peBlue = (BYTE)((Color & 0xFF0000) >> 16);
        peColor.peGreen = (BYTE)((Color & 0x00FF00) >> 8);
        peColor.peRed = (BYTE)(Color & 0x0000FF);
        peColor.peFlags = 0;
        for(iPalIndex = 0; iPalIndex < 256; iPalIndex++) {
            // mask with 0x00FFFFFF because we should ignore pe flags
            if((*(DWORD *)&DxWndStatus.Palette[iPalIndex] & 0x00FFFFFF) == Color) {
                bMatched = TRUE;
                break;
            }
        }
        if(bMatched) sprintf(sMsg, "%s\ncolor matched %d", sMsg, iPalIndex);
        else sprintf(sMsg, "%s\ncolor unmatched", sMsg);
    }
    // show palette entries
    CWnd *ChessBoard;
    RECT BoardSize;
    ChessBoard = this->GetDlgItem(IDC_PALETTECOLORS);
    dc = ChessBoard->GetDC();
    ChessBoard->GetClientRect(&BoardSize);
    w = BoardSize.right;
    h = BoardSize.bottom;
    if(dc) {
        BOOL bDoOnce = TRUE;
        rgbq.rgbBlue = (BYTE)((Color & 0xFF0000) >> 16);
        rgbq.rgbGreen = (BYTE)((Color & 0x00FF00) >> 8);
        rgbq.rgbRed = (BYTE)(Color & 0x0000FF);
        rgbq.rgbReserved = 0;
        dib_Pixel.SetPixel(0, 0, rgbq);
        dib_Pixel.Draw(dc, CRect(w - 16, h + 16, w, h + 32), CRect(0, 0, 1, 1));
        dib_Palette.Draw(dc, CRect(0, 0, w, h), CRect(0, 0, 16, 16));
    }
    this->ReleaseDC(dc);
    // update screen
    this->SetDlgItemTextA(IDC_PALETTE_HELP, sMsg);
}

BOOL CPaletteDialog::OnInitDialog() {
    CDC *myDC;
    CDialog::OnInitDialog();
    if((myDC = this->GetDC()) == NULL) return FALSE;
    if((myWin = myDC->GetWindow()) == NULL) return FALSE;
    this->ReleaseDC(myDC);
    dib_Palette.ReadFromResource(IDB_PALETTE);
    RGBQUAD black = {0, 0, 0};
    dib_Pixel.CreateEmpty(1, 1, 32, black);
    SetTimer(IDPaletteTIMER, 200, NULL);
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CPaletteDialog::OnOK() {
    // TODO: Add your specialized code here and/or call the base class
    // stop timer
    KillTimer(IDPaletteTIMER);
    // free CDib resources
    dib_Pixel.Invalidate();
    dib_Palette.Invalidate();
    CDialog::OnOK();
}
