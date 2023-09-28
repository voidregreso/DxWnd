// GammaRampDialog.cpp : implementation file
//

#include "stdafx.h"
#include "dxwndhost.h"
#include "GammaRampDialog.h"
#include "bmpext.h"

#define IDGammaTIMER 3

BEGIN_MESSAGE_MAP(CGammaRampDialog, CDialog)
    ON_COMMAND(IDCANCEL, OnOK)
    ON_WM_TIMER()
END_MESSAGE_MAP()

// CGammaRampDialog dialog

IMPLEMENT_DYNAMIC(CGammaRampDialog, CDialog)

CGammaRampDialog::CGammaRampDialog()
    : CDialog(CGammaRampDialog::IDD) {
}

CGammaRampDialog::~CGammaRampDialog() {
    KillTimer(IDGammaTIMER);
}

void CGammaRampDialog::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}

// CGammaRampDialog message handlers

void CGammaRampDialog::OnTimer(UINT_PTR nIDEvent) {
    RECT Rect;
    int h, w, i;
    WORD GammaRamp[256 * 3];
    RGBQUAD Red   = {0, 0, 255, 0};
    RGBQUAD Green = {0, 255, 0, 0};
    RGBQUAD Blue  = {255, 0, 0, 0};
    CStatic *pic = (CStatic *)this->GetDlgItem(IDC_GAMMAVIEWPANEL);
    HDC hdc;
    RGBQUAD White = {0xFF, 0xFF, 0xFF, 0x00};
    WARNTIMERS;
    if(!dib_Gamma.CreateEmpty(256, 256, 32, White)) MessageBox("CreateEmpty error", "error", 0);
    // v2.05.02: fixed memory leakage on desktop DC
    hdc = ::GetDC(NULL);
    GetDeviceGammaRamp(hdc, (LPVOID)GammaRamp);
    ::ReleaseDC(NULL, hdc);
    WORD *Gamma = GammaRamp;
    pic->GetClientRect(&Rect);
    w = Rect.right;
    h = Rect.bottom;
    for(i = 0; i < 256; i++) dib_Gamma.SetPixel(i, (255 - (*Gamma++ >> 8)) & 0xFF, Red); // red
    for(i = 0; i < 256; i++) dib_Gamma.SetPixel(i, (255 - (*Gamma++ >> 8)) & 0xFF, Green); // green
    for(i = 0; i < 256; i++) dib_Gamma.SetPixel(i, (255 - (*Gamma++ >> 8)) & 0xFF, Blue); // blue
    CDC *dc;
    dc = pic->GetDC();
    if(!dc) return;
    if(!dib_Gamma.Draw(dc, CRect(0, 0, w, h), CRect(0, 0, 255, 255))) MessageBox("Draw error", "error", 0);
    pic->ReleaseDC(dc);
    dc->~CDC();
    dib_Gamma.Invalidate();
}

BOOL CGammaRampDialog::OnInitDialog() {
    CDialog::OnInitDialog();
    KillTimer(IDGammaTIMER); // just in case ...
    SetTimer(IDGammaTIMER, 200, NULL);
    return TRUE;  // return TRUE unless you set the focus to a control
}

void CGammaRampDialog::OnOK() {
    KillTimer(IDGammaTIMER);
    CDialog::OnOK();
}

