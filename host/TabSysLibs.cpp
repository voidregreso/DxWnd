// TabGDI.cpp : implementation file
//

#include "stdafx.h"
#include "TargetDlg.h"
#include "TabSysLibs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabLogs dialog

CTabSysLibs::CTabSysLibs(CWnd *pParent /*=NULL*/)
    : CDialog(CTabSysLibs::IDD, pParent) {
    //{{AFX_DATA_INIT(CTabSysLibs)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

BOOL CTabSysLibs::PreTranslateMessage(MSG *pMsg) {
    //if(((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
    if((pMsg->message == WM_KEYDOWN)
            && (pMsg->wParam == VK_RETURN))
        return TRUE;
    return CWnd::PreTranslateMessage(pMsg);
}

void CTabSysLibs::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    // GDI
    DDX_Check(pDX, IDC_CLIENTREMAPPING, cTarget->m_ClientRemapping);
    DDX_Radio(pDX, IDC_GDINONE, cTarget->m_DCEmulationMode);
    DDX_Check(pDX, IDC_FIXTEXTOUT, cTarget->m_FixTextOut);
    DDX_Check(pDX, IDC_QUALITYFONTS, cTarget->m_QualityFonts);
    DDX_Check(pDX, IDC_SHRINKFONTWIDTH, cTarget->m_ShrinkFontWidth);
    DDX_Check(pDX, IDC_NOFILLRECT, cTarget->m_NoFillRect);
    DDX_Check(pDX, IDC_FIXCLIPPERAREA, cTarget->m_FixClipperArea);
    DDX_Check(pDX, IDC_SHAREDDCHYBRID, cTarget->m_SharedDCHybrid);
    DDX_Check(pDX, IDC_SYNCPALETTE, cTarget->m_SyncPalette);
    DDX_Check(pDX, IDC_NOWINERRORS, cTarget->m_NoWinErrors);
    DDX_Check(pDX, IDC_NODIALOGS, cTarget->m_NoDialogs);
    DDX_Check(pDX, IDC_STRETCHDIALOGS, cTarget->m_StretchDialogs);
    DDX_Check(pDX, IDC_INVALIDATEFULLRECT, cTarget->m_InvalidateFullRect);
    DDX_Check(pDX, IDC_NOSETPIXELFORMAT, cTarget->m_NoSetPixelFormat);
    DDX_Check(pDX, IDC_SCALECBTHOOK, cTarget->m_ScaleCBTHook);
    // MCI
    DDX_Check(pDX, IDC_REMAPMCI, cTarget->m_RemapMCI);
    DDX_Check(pDX, IDC_NOMOVIES, cTarget->m_NoMovies);
    DDX_Check(pDX, IDC_STRETCHMOVIES, cTarget->m_StretchMovies);
    DDX_Check(pDX, IDC_FIXMOVIESCOLOR, cTarget->m_FixMoviesColor);
    DDX_Check(pDX, IDC_BYPASSMCI, cTarget->m_BypassMCI);
    DDX_Check(pDX, IDC_FIXPCMAUDIO, cTarget->m_FixPCMAudio);
    DDX_Check(pDX, IDC_FIXSMACKLOOP, cTarget->m_FixSmackLoop);
    // color management
    DDX_Check(pDX, IDC_DISABLEGAMMARAMP, cTarget->m_DisableGammaRamp);
    DDX_Check(pDX, IDC_LOCKSYSCOLORS, cTarget->m_LockSysColors);
    DDX_Check(pDX, IDC_LOCKRESERVEDPALETTE, cTarget->m_LockReservedPalette);
    DDX_Check(pDX, IDC_PALDIBEMULATION, cTarget->m_PALDIBEmulation);
    DDX_Check(pDX, IDC_REFRESHONREALIZE, cTarget->m_RefreshOnRealize);
}

BEGIN_MESSAGE_MAP(CTabSysLibs, CDialog)
    //{{AFX_MSG_MAP(CTabLogs)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabLogs message handlers
