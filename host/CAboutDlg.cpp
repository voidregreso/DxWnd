#include "stdafx.h"
#include "dxwndhost.h"
#include "CAboutDlg.h"

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
    //{{AFX_DATA_INIT(CAboutDlg)
    m_Version = _T("");
    m_DxwPlayVersion = _T("");
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    DDX_Text(pDX, IDC_VERSION, m_Version);
    DDX_Text(pDX, IDC_DXWPLAYVERSION, m_DxwPlayVersion);
    DDX_Text(pDX, IDC_THANKS, m_Thanks);
    //}}AFX_DATA_MAP
    DDX_Control(pDX, IDC_ANIMATION, m_Animation);
}

BOOL CAboutDlg::OnInitDialog() {
    CDialog::OnInitDialog();
    SetTimer(ID_HELP_SCROLL, 600, NULL);
    SetTimer(ID_HELP_SCROLL2, 32, NULL);
    if (m_Animation.Load(MAKEINTRESOURCE(IDR_GHO), _T("GIF")))
        //if (m_Animation.Load("gho.gif"))
        m_Animation.Draw();
    return TRUE;  // return TRUE unless you set the focus to a control
}

CString Thanks[] = {
    "Andrea Mazzoleni (www.scale2x.it) for Scale2X filter",
    "Aqrit for proxies, many tweaks & hot patching schema",
    "AxXxB and Old-Games.ru teammates for ZBUFFER fix",
    "Charles Petzold for MCI_Tester source code",
    "Chris Maunder for SystemTray class source code",
    "David Reid for FLAC, MP3 and WAV audio decoders",
    "Dege for creating the excellent dgVoodoo wrappers",
    "Dixie for support on \"Emperor of the Fading Suns\"",
    "Fabian \"ryg\" Giesen & others for DXT1/5 compression",
    "Federico Dossena for WineD3D libraries",
    "FunkyFr3sh for fixes in proxy dll",
    "Gsky916 for chinese translation",
    "Huh for his incredible program support",
    "Jari Kommpa for ddraw wrapper source and d3d hints",
    "Jiri Dvorak for his d3d8 wrapper with 16bpp emulation",
    "Luigi Auriemma for injection syncronization",
    "Mark Ransom for glbitmap scaling source code",
    "Matt Pietrek for PEDUMP source code",
    "Maxim Stepin & Cameron Zemek for hqx filters",
    "Michael Koch for d3d9 proxy dll",
    "Narzoul for sharing DC handling code",
    "Nikolai Serdiuk for MCI CD Player project",
    "OlEG Bykov for animate gif component",
    "Olly (www.ollydbg.de) for OllyDBG & disasm lib",
    "Reg2s for publishing DxWnd OG source code",
    "Riitaoja for shims, tweaks, html help and CD audio support",
    "RomSteady for his kind encouragement",
    "Ryan Geiss for his bilinear filter code",
    "Swiss Frank for file extension association code",
    "TigerhawkT3 for html manual pages",
    "Toni Spets (toni.spets@iki.fi) for audio CD emulator",
    "Tsuda Kageyu for MinHook dll source code",
    "UCyborg for LegacyD3DResolutionHack d3d hack",
    "ZeroX4 for the new DxWnd artworks",
    "*end*"
}; // list terminator

void CAboutDlg::OnTimer(UINT_PTR nIDEvent) {
    if(nIDEvent == ID_HELP_SCROLL) {
        static int i = 0;
        int j;
        int ThanksCount;
        for(ThanksCount = 0; ; ThanksCount++) if(!strcmp(Thanks[ThanksCount], "*end*")) break;
        CString RolledThanks;
        for(j = i; j < ThanksCount; j++) RolledThanks.AppendFormat("%s\n", Thanks[j]);
        for(j = 0; j < i          ; j++) RolledThanks.AppendFormat("%s\n", Thanks[j]);
        this->SetDlgItemTextA(IDC_THANKS, RolledThanks);
        i = (i + 1) % ThanksCount;
    }
    if(nIDEvent == ID_HELP_SCROLL2) {
        CWnd *TB = (CWnd *)this->GetDlgItem(IDC_THANKS);
        TB->ScrollWindow(0, -1, NULL, NULL);
    }
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    ON_WM_TIMER()
    ON_STN_CLICKED(IDC_ANIMATION, &CAboutDlg::OnStnClickedAnimation)
END_MESSAGE_MAP()
