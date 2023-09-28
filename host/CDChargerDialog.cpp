// CDChargerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "dxwndhost.h"
#include "CDChargerDialog.h"

// CCDChargerDialog dialog

IMPLEMENT_DYNAMIC(CCDChargerDialog, CDialog)

CCDChargerDialog::CCDChargerDialog()
    : CDialog(CCDChargerDialog::IDD) {
}

CCDChargerDialog::~CCDChargerDialog() {
}

void CCDChargerDialog::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Radio(pDX, IDC_CDINDEX1, m_CDIndex);
    //char msg[81];
    //sprintf_s(msg, 80, "set CD#%d", m_CDIndex+1);
    //MessageBox(msg, "DxWnd CD charger", 0);
    GetHookInfo()->CDIndex = (short)m_CDIndex;
}


BEGIN_MESSAGE_MAP(CCDChargerDialog, CDialog)
    ON_COMMAND(IDOK, OnOK)
END_MESSAGE_MAP()

// CCDChargerDialog message handlers

BOOL CCDChargerDialog::OnInitDialog() {
    m_CDIndex = (int)(GetHookInfo()->CDIndex);
    if((m_CDIndex < 0) || (m_CDIndex > 8)) m_CDIndex = 0;
    //char msg[81];
    //sprintf_s(msg, 80, "Initial CD#%d", m_CDIndex+1);
    //MessageBox(msg, "DxWnd CD charger", 0);
    CDialog::OnInitDialog();
    return TRUE;  // return TRUE unless you set the focus to a control
}

void CCDChargerDialog::OnOK() {
    //char msg[81];
    //sprintf_s(msg, 80, "Loading CD#%d", m_CDIndex+1);
    //MessageBox(msg, "DxWnd CD charger", 0);
    //CDialog::OnOK();
    UpdateData();
}

