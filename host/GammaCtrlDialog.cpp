// TabDirectX.cpp : implementation file
//

#include "stdafx.h"
#include "dxwndhost.h"
#include "TargetDlg.h"
#include "GammaCtrlDialog.h"
#include "bmpext.h"
#include <math.h>

extern char gInitPath[];

#define IDGammaSliderTIMER 8

BEGIN_MESSAGE_MAP(CTabGammaCtrl, CDialog)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_GAMMARESET, &CTabGammaCtrl::OnGammaReset)
    ON_BN_CLICKED(IDC_GAMMASAVE, &CTabGammaCtrl::OnGammaSave)
    ON_BN_CLICKED(IDC_GAMMALOAD, &CTabGammaCtrl::OnGammaLoad)
    ON_COMMAND(IDCANCEL, OnOK)
END_MESSAGE_MAP()

static BYTE GammaEncode(BYTE original, float gamma) {
    float res = pow(
                    (float)((float)original / 255),
                    (float)(1.0 / gamma))
                * (float)255.0;
    return (BYTE)res;
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabGammaCtrl dialog


CTabGammaCtrl::CTabGammaCtrl(CWnd *pParent /*=NULL*/)
    : CDialog(CTabGammaCtrl::IDD, pParent) {
    //{{AFX_DATA_INIT(CTabGammaCtrl)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void CTabGammaCtrl::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Slider(pDX, IDC_RGAMMA, i_RGamma);
    DDX_Slider(pDX, IDC_GGAMMA, i_GGamma);
    DDX_Slider(pDX, IDC_BGAMMA, i_BGamma);
}

BOOL CTabGammaCtrl::OnInitDialog() {
    CSliderCtrl *Slider;
    CButton *Check;
    char sGammaVals[81];
    int iEnable, iLock, iReverse, iRSlider, iGSlider, iBSlider;
    CDialog::OnInitDialog();
    GetPrivateProfileString("window", "gammasettings", "", sGammaVals, 80, gInitPath);
    if(sscanf(sGammaVals, "%d:%d:%d:%d:%d:%d", &iEnable, &iLock, &iReverse, &iRSlider, &iGSlider, &iBSlider) != 6)
        iEnable = iLock = iReverse = iRSlider = iGSlider = iBSlider = 0;
    Check = (CButton *)this->GetDlgItem(IDC_GAMMAVIRTUAL);
    Check->SetCheck(iEnable);
    Check = (CButton *)this->GetDlgItem(IDC_GAMMAREVERSE);
    Check->SetCheck(iReverse);
    Check = (CButton *)this->GetDlgItem(IDC_LOCKSLIDERS);
    Check->SetCheck(iLock);
    Slider = (CSliderCtrl *)this->GetDlgItem(IDC_RGAMMA);
    Slider->SetRange(-16, +16, 0);
    Slider->SetTicFreq(1);
    Slider->SetPos(iRSlider);
    Slider = (CSliderCtrl *)this->GetDlgItem(IDC_GGAMMA);
    Slider->SetRange(-16, +16, 0);
    Slider->SetTicFreq(1);
    Slider->SetPos(iGSlider);
    Slider = (CSliderCtrl *)this->GetDlgItem(IDC_BGAMMA);
    Slider->SetRange(-16, +16, 0);
    Slider->SetTicFreq(1);
    Slider->SetPos(iBSlider);
    KillTimer(IDGammaSliderTIMER); // you never know ...
    SetTimer(IDGammaSliderTIMER, 500, NULL);
    // TODO:  Add extra initialization here
    return TRUE;  // return TRUE unless you set the focus to a control
}

void CTabGammaCtrl::OnTimer(UINT_PTR nIDEvent) {
    float gammaexp;
    BYTE *gamma;
    int i;
    CSliderCtrl *RSlider, *GSlider, *BSlider;
    CButton *Check;
    BYTE GammaBuffer[256 * 3];
    WARNTIMERS;
    Check = (CButton *)this->GetDlgItem(IDC_GAMMAVIRTUAL);
    b_Enabled = (Check->GetCheck() == BST_CHECKED);
    Check = (CButton *)this->GetDlgItem(IDC_GAMMAREVERSE);
    b_Reversed = (Check->GetCheck() == BST_CHECKED);
    Check = (CButton *)this->GetDlgItem(IDC_LOCKSLIDERS);
    b_Locked = (Check->GetCheck() == BST_CHECKED);
    CDialog::OnTimer(nIDEvent);
    RSlider = (CSliderCtrl *)this->GetDlgItem(IDC_RGAMMA);
    i_RGamma = RSlider->GetPos();
    GSlider = (CSliderCtrl *)this->GetDlgItem(IDC_GGAMMA);
    i_GGamma = GSlider->GetPos();
    BSlider = (CSliderCtrl *)this->GetDlgItem(IDC_BGAMMA);
    i_BGamma = BSlider->GetPos();
    if(b_Locked) {
        i_GGamma = i_BGamma = i_RGamma;
        GSlider->SetPos(i_RGamma);
        BSlider->SetPos(i_RGamma);
        GSlider->EnableWindow(FALSE);
        BSlider->EnableWindow(FALSE);
    } else {
        GSlider->EnableWindow(TRUE);
        BSlider->EnableWindow(TRUE);
    }
    memcpy(GammaBuffer, GetHookInfo()->GammaRamp, sizeof(GammaBuffer));
    gamma = GammaBuffer;
    GetHookInfo()->GammaControl = b_Enabled;
    gammaexp = pow ((float)1.1, (float)i_RGamma);
    for(i = 0; i < 256; i++) *gamma++ = GammaEncode(i, gammaexp); // red gamma
    gammaexp = pow ((float)1.1, (float)i_GGamma);
    for(i = 0; i < 256; i++) *gamma++ = GammaEncode(i, gammaexp); // green gamma
    gammaexp = pow ((float)1.1, (float)i_BGamma);
    for(i = 0; i < 256; i++) *gamma++ = GammaEncode(i, gammaexp); // blue gamma
    if(b_Reversed) {
        gamma = GammaBuffer;
        for(i = 0; i < 256; i++) *gamma++ = 255 - *gamma; // red gamma
        for(i = 0; i < 256; i++) *gamma++ = 255 - *gamma; // green gamma
        for(i = 0; i < 256; i++) *gamma++ = 255 - *gamma; // blue gamma
    }
    if(b_Enabled) memcpy(GetHookInfo()->GammaRamp, GammaBuffer, sizeof(GammaBuffer));
    CStatic *pic = (CStatic *)this->GetDlgItem(IDC_WINGAMMARAMP);
    int w, h;
    RECT Rect;
    RGBQUAD White = {0xFF, 0xFF, 0xFF, 0x00};
    RGBQUAD Red   = {0, 0, 255, 0};
    RGBQUAD Green = {0, 255, 0, 0};
    RGBQUAD Blue  = {255, 0, 0, 0};
    gamma = GammaBuffer;
    pic->GetClientRect(&Rect);
    w = Rect.right;
    h = Rect.bottom;
    if(!dib_Gamma.CreateEmpty(256, 256, 32, White)) MessageBox("CreateEmpty error", "error", 0);
    for(i = 0; i < 256; i++) dib_Gamma.SetPixel(i, (256 - *gamma++) & 0xFF, Red); // red
    for(i = 0; i < 256; i++) dib_Gamma.SetPixel(i, (255 - *gamma++) & 0xFF, Green); // green
    for(i = 0; i < 256; i++) dib_Gamma.SetPixel(i, (255 - *gamma++) & 0xFF, Blue); // blue
    CDC *dc;
    dc = pic->GetDC();
    if(!dc) return;
    if(!dib_Gamma.Draw(dc, CRect(0, 0, w, h), CRect(0, 0, 255, 255))) MessageBox("Draw error", "error", 0);
    pic->ReleaseDC(dc);
    dib_Gamma.Invalidate();
}

void CTabGammaCtrl::OnOK() {
    KillTimer(IDGammaSliderTIMER);
    CDialog::OnOK();
}

void CTabGammaCtrl::OnGammaReset() {
    CSliderCtrl *Slider;
    Slider = (CSliderCtrl *)this->GetDlgItem(IDC_RGAMMA);
    Slider->SetPos(0);
    Slider = (CSliderCtrl *)this->GetDlgItem(IDC_GGAMMA);
    Slider->SetPos(0);
    Slider = (CSliderCtrl *)this->GetDlgItem(IDC_BGAMMA);
    Slider->SetPos(0);
}

extern void char2hex(unsigned char *src, char *dst, int len);
extern int hex2char(unsigned char *dst, char *src);

void CTabGammaCtrl::OnGammaSave() {
    BYTE GammaBuffer[256 * 3];
    char GammaString[256 * 2 + 16];
    char sGammaVals[81];
    // save global Enable flag
    WritePrivateProfileString("window", "gammaenabled", GetHookInfo()->GammaControl ? "1" : "0", gInitPath);
    // save gamma ramp
    memcpy(GammaBuffer, GetHookInfo()->GammaRamp, sizeof(GammaBuffer));
    // red
    char2hex(&GammaBuffer[0], GammaString, 256);
    WritePrivateProfileString("window", "gammared", GammaString, gInitPath);
    // green
    char2hex(&GammaBuffer[256], GammaString, 256);
    WritePrivateProfileString("window", "gammagreen", GammaString, gInitPath);
    // blue
    char2hex(&GammaBuffer[512], GammaString, 256);
    WritePrivateProfileString("window", "gammablue", GammaString, gInitPath);
    sprintf(sGammaVals, "%d:%d:%d:%d:%d:%d",
            b_Enabled, b_Locked, b_Reversed, i_RGamma, i_GGamma, i_BGamma);
    WritePrivateProfileString("window", "gammasettings", sGammaVals, gInitPath);
}

void CTabGammaCtrl::OnGammaLoad() {
    // tbd
}

/////////////////////////////////////////////////////////////////////////////
// CTabGammaCtrl message handlers
