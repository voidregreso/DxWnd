// GammaRampDialog.cpp : implementation file
//

#include "stdafx.h"
#include "dxwndhost.h"
#include "CDPlayerDialog.h"

#define IDCDPlayerTIMER 5

// CCDPlayerDialog dialog

IMPLEMENT_DYNAMIC(CCDPlayerDialog, CDialog)

BEGIN_MESSAGE_MAP(CCDPlayerDialog, CDialog)
    ON_COMMAND(IDCANCEL, OnOK)
    ON_WM_TIMER()
END_MESSAGE_MAP()

CCDPlayerDialog::CCDPlayerDialog()
    : CDialog(CCDPlayerDialog::IDD) {
}

CCDPlayerDialog::~CCDPlayerDialog() {
    KillTimer(IDCDPlayerTIMER);
}

void CCDPlayerDialog::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}

// CCDPlayerDialog message handlers

static int GetVolIcon(int vol) {
    if(vol == 0)	return IDI_VOLUME0;
    if(vol <= 20)	return IDI_VOLUME1;
    if(vol <= 40)	return IDI_VOLUME2;
    if(vol <= 60)	return IDI_VOLUME3;
    if(vol <= 80)	return IDI_VOLUME4;
    if(vol <= 100)	return IDI_VOLUME5;
    return IDI_VOLUME5; // useless
}

void CCDPlayerDialog::OnTimer(UINT_PTR nIDEvent) {
    CDialog::OnTimer(nIDEvent);
    CStatic *IconBox;
    CStatic *VolumeBox;
    CProgressCtrl *ProgressBar;
    HICON Icon, PrevIcon;
    HICON Volume, PrevVolume;
    static int i = 0;
    char sMsg[1024];
    int DxStatus;
    IconBox = (CStatic *)this->GetDlgItem(IDC_PLAYER_ICON);
    VolumeBox = (CStatic *)this->GetDlgItem(IDC_PLAYER_VOLUME);
    ProgressBar = (CProgressCtrl *)this->GetDlgItem(IDC_PLAYER_PROGRESS);
    DxStatus = GetHookStatus(GetHookInfo());
    PrevIcon = NULL;
    PrevVolume = NULL;
    if(DxStatus == DXW_RUNNING) {
        switch(GetHookInfo()->PlayerStatus) {
        case DXW_PLAYER_STOPPED:
            ProgressBar->ShowWindow(SW_HIDE);
            Icon = ::LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CDSTOP));
            Volume = (HICON)NULL;
            strcpy(sMsg, "STOP");
            break;
        case DXW_PLAYER_PLAYING:
        case DXW_PLAYER_PAUSED:
            ProgressBar->ShowWindow(SW_SHOW);
            Icon = ::LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(GetHookInfo()->PlayerStatus == DXW_PLAYER_PLAYING ? IDI_CDPLAY : IDI_CDPAUSE));
            Volume = ::LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(GetVolIcon(GetHookInfo()->Volume)));
            ProgressBar->SetRange(0, GetHookInfo()->TrackLength);
            ProgressBar->SetPos(GetHookInfo()->TimeElapsed);
            sprintf(sMsg, "%s\nTRACK\t%02d/%02d\nTIME\t%02d:%02d/%02d:%02d\nVOL\t%02d%%",
                    GetHookInfo()->PlayerStatus == DXW_PLAYER_PLAYING ? "PLAY" : "PAUSE",
                    GetHookInfo()->TrackNo, GetHookInfo()->TracksNo,
                    GetHookInfo()->TimeElapsed / 60, GetHookInfo()->TimeElapsed % 60,
                    GetHookInfo()->TrackLength / 60, GetHookInfo()->TrackLength % 60,
                    GetHookInfo()->Volume);
            break;
        }
    } else {
        ProgressBar->ShowWindow(SW_HIDE);
        strcpy (sMsg, "");
        Icon = (HICON)NULL;
        Volume = (HICON)NULL;
    }
    PrevIcon = IconBox->SetIcon(Icon);
    PrevVolume = VolumeBox->SetIcon(Volume);
    this->SetDlgItemTextA(IDC_CDPLAYER, sMsg);
    if(PrevIcon) ::DestroyIcon(PrevIcon);
    if(PrevVolume) ::DestroyIcon(PrevVolume);
}

BOOL CCDPlayerDialog::OnInitDialog() {
    CDialog::OnInitDialog();
    SetTimer(IDCDPlayerTIMER, 200, NULL);
    return TRUE;  // return TRUE unless you set the focus to a control
}

void CCDPlayerDialog::OnOK() {
    KillTimer(IDCDPlayerTIMER);
    CDialog::OnOK();
}

