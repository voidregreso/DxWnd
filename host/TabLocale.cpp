// TabLocale.cpp : implementation file
//

#include "stdafx.h"
#include "TargetDlg.h"
#include "TabLocale.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabCompat dialog

CTabLocale::CTabLocale(CWnd *pParent /*=NULL*/)
//	: CTargetDlg(pParent)
    : CDialog(CTabLocale::IDD, pParent) {
    //{{AFX_DATA_INIT(CTabLocale)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void CTabLocale::UpdateCodePage(int CodePage) {
    CHAR Name[(2 * LOCALE_NAME_MAX_LENGTH) + 1];
    CPINFOEXA CPInfoEx;
    if(::GetCPInfoEx(CodePage, 0, &CPInfoEx))
        sprintf(Name, "%s", CPInfoEx.CodePageName);
    else
        sprintf(Name, "invalid codepage err:%d", GetLastError());
    this->SetDlgItemTextA(IDC_CODEPAGE_TEXT, Name);
}

void CTabLocale::UpdateLocaleID(int LocaleID) {
    // v2.05.18: LCIDToLocaleName dynamic loading to stay compatible with WinXP
    WCHAR WName[LOCALE_NAME_MAX_LENGTH + 1];
    CHAR Name[(2 * LOCALE_NAME_MAX_LENGTH) + 1];
    typedef int (WINAPI * LCIDToLocaleName_Type)(LCID, LPWSTR, int, DWORD);
    HMODULE hKernel32 = LoadLibrary("Kernel32.dll");
    if(hKernel32 == NULL) return;
    LCIDToLocaleName_Type pLCIDToLocaleName = (LCIDToLocaleName_Type)GetProcAddress(hKernel32, "LCIDToLocaleName");;
    if(pLCIDToLocaleName == NULL) return;
    if((*pLCIDToLocaleName)(LocaleID, WName, LOCALE_NAME_MAX_LENGTH, 0))
        sprintf(Name, "%ls", WName);
    else
        sprintf(Name, "invalid locale err:%d", GetLastError());
    this->SetDlgItemTextA(IDC_LANGUAGE_TEXT, Name);
    CloseHandle(hKernel32);
}

BOOL CTabLocale::PreTranslateMessage(MSG *pMsg) {
    if((pMsg->message == WM_KEYDOWN)
            && (pMsg->wParam == VK_RETURN)) {
        this->UpdateData();
        return TRUE;
    }
    return CWnd::PreTranslateMessage(pMsg);
}

void CTabLocale::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    DDX_Check(pDX, IDC_CUSTOMLOCALE, cTarget->m_CustomLocale);
    DDX_Check(pDX, IDC_CLASSLOCALE, cTarget->m_ClassLocale);
    DDX_Text(pDX, IDC_COUNTRY, cTarget->m_Country);
    DDX_Text(pDX, IDC_LANGUAGE, cTarget->m_Language);
    DDX_Text(pDX, IDC_CODEPAGE, cTarget->m_CodePage);
    UpdateCodePage(cTarget->m_CodePage);
    UpdateLocaleID(cTarget->m_Language);
}

BEGIN_MESSAGE_MAP(CTabLocale, CDialog)
    //{{AFX_MSG_MAP(CTabCompat)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabCompat message handlers


