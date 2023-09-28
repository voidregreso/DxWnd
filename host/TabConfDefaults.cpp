// TabConfDefaults.cpp : implementation file
//

#include "stdafx.h"
#include "TargetDlg.h"
#include "TabConfDefaults.h"
#include "CGlobalSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabCompat dialog

CTabConfDefaults::CTabConfDefaults(CWnd *pParent /*=NULL*/)
    : CDialog(CTabConfDefaults::IDD, pParent) {
}

BOOL CTabConfDefaults::PreTranslateMessage(MSG *pMsg) {
    if((pMsg->message == WM_KEYDOWN)
            && (pMsg->wParam == VK_RETURN))
        return TRUE;
    return CWnd::PreTranslateMessage(pMsg);
}

void CTabConfDefaults::DoDataExchange(CDataExchange *pDX) {
    //MessageBox("CTabConfDefaults::DoDataExchange","trace",0);
    CString sDefaultPosX, sDefaultPosY;
    CDialog::DoDataExchange(pDX);
    CGlobalSettings *cTarget = ((CGlobalSettings *)(this->GetParent()->GetParent()));
    sDefaultPosX.Format("%d", cTarget->m_DefaultPosX);
    sDefaultPosY.Format("%d", cTarget->m_DefaultPosY);
    DDX_Radio(pDX, IDC_DEFAULTCOORDINATES, cTarget->m_DefaultCoordinates);
    DDX_Text(pDX, IDC_DEFAULTPOSX, sDefaultPosX);
    DDX_Text(pDX, IDC_DEFAULTPOSY, sDefaultPosY);
    DDX_Text(pDX, IDC_DEFAULTSIZX, cTarget->m_DefaultSizX);
    DDX_Text(pDX, IDC_DEFAULTSIZY, cTarget->m_DefaultSizY);
    cTarget->m_DefaultPosX = atoi(sDefaultPosX);
    cTarget->m_DefaultPosY = atoi(sDefaultPosY);
    DDX_Text(pDX, IDC_TEX_MAXX, cTarget->m_TexMaxX);
    DDX_Text(pDX, IDC_TEX_MAXY, cTarget->m_TexMaxY);
    DDX_Text(pDX, IDC_TEX_MINX, cTarget->m_TexMinX);
    DDX_Text(pDX, IDC_TEX_MINY, cTarget->m_TexMinY);
}

BEGIN_MESSAGE_MAP(CTabConfDefaults, CDialog)
    //{{AFX_MSG_MAP(CTabCompat)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabCompat message handlers


