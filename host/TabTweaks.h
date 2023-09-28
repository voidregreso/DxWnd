#if !defined(AFX_TABTWEAKS_H__798A9124_C906_446C_822D_322B5AB6C4C4__INCLUDED_)
#define AFX_TABTWEAKS_H__798A9124_C906_446C_822D_322B5AB6C4C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabTweaks.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CTabTweaks dialog

class CTabTweaks : public CDialog {
    // Construction
public:
    CTabTweaks(CWnd *pParent = NULL);   // standard constructor
    BOOL OnInitDialog();

private:
    BOOL PreTranslateMessage(MSG *);

    // Dialog Data
    //{{AFX_DATA(CTabTweaks)
    enum { IDD = IDD_TAB_TWEAKS };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CTabTweaks)
protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CTabTweaks)
    // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedTweakAdd();
    afx_msg void OnBnClickedTweakRemove();
    afx_msg void OnBnClickedTweakHelp();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

typedef struct {
    char *Label;
    char *Caption;
    BOOL *Flag;
} TweakEntry_Type;

#endif