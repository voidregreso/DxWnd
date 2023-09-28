#if !defined(AFX_TABCONFKEYS_H__798A9124_C906_446C_822D_322B5AB6C4C4__INCLUDED_)
#define AFX_TABCONFKEYS_H__798A9124_C906_446C_822D_322B5AB6C4C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabConfKeys.h : header file
//
#include "resource.h"
#include "TargetDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CTabDirectX dialog

//class CTabConfKeys : public CTargetDlg
class CTabConfKeys : public CDialog {
    // Construction
public:
    CTabConfKeys(CWnd *pParent = NULL);   // standard constructor
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    BOOL OnInitDialog();

private:
    BOOL PreTranslateMessage(MSG *);

    // Dialog Data
    //{{AFX_DATA(CTabDirectX)
    enum { IDD = IDD_CONFTAB_HOTKEYS };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CTabDirectX)
    //}}AFX_VIRTUAL

    // Implementation

    // Generated message map functions
    //{{AFX_MSG(CTabDirectX)
    // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABTHREE_H__798A9124_C906_446C_822D_322B5AB6C4C4__INCLUDED_)
