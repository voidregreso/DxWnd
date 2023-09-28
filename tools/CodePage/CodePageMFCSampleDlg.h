// CodePageMFCSampleDlg.h : header file
//

#pragma once
#include "afxcmn.h"

#include "CodePageMFC.h"

// CCodePageDlg dialog
class CCodePageDlg : public CDialog {
    // Construction
public:
    CCodePageDlg(CWnd *pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_CODEPAGEMFCSAMPLE_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);	// DDX/DDV support


    // Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    CListCtrl m_wndInstalled;
    CListCtrl m_wndSupported;
private:
    void InitListControl(CListCtrl &listControl, const rec::CCodePage::CCodePageObject &cpObject);
    static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
};
