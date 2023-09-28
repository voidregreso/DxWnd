// CDPlayerDlg.h : header file
//

#if !defined(AFX_CDPLAYERDLG_H__7C00428A_5159_401A_AFE4_6C4126EA9586__INCLUDED_)
#define AFX_CDPLAYERDLG_H__7C00428A_5159_401A_AFE4_6C4126EA9586__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DigitalClock.h"
#include "CDAudio.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// CCDPlayerDlg dialog

class CCDPlayerDlg : public CDialog {
    // Construction
public:
    CCDPlayerDlg(CWnd *pParent = NULL);	// standard constructor

    // Dialog Data
    //{{AFX_DATA(CCDPlayerDlg)
    enum { IDD = IDD_CDPLAYER_DIALOG };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCDPlayerDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX);	// DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    HICON m_hIcon;
    UINT m_volumeTID;
    UINT m_volumeElapse;
    UINT m_LastVolume;

    CCDAudio m_CDAudio;
    CDigitalClock m_DClock;

    CToolTipCtrl m_ToolTip;
    int i_Volume;

    // Generated message map functions
    //{{AFX_MSG(CCDPlayerDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnDestroy();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedPlay();
    afx_msg void OnBnClickedPause();
    afx_msg void OnBnClickedStop();
    afx_msg void OnBnClickedBackward();
    afx_msg void OnBnClickedForward();
    afx_msg void OnBnClickedCdEject();
    afx_msg void OnBnClickedExit();
    afx_msg void OnBnClickedTrackList();
    afx_msg void OnTimer(UINT);
    afx_msg void OnTrackChange(UINT nID);
    CButton m_btnEject;
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    CButton m_btnStop;
    CButton m_btnHelp;
    CButton m_btnNext;
    CButton m_btnPlay;
    CButton m_btnPrevious;
    CButton m_btnExit;
    CButton m_btnPause;
    CButton m_btnTrackList;
    CSliderCtrl m_VolumeSlider;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CDPLAYERDLG_H__7C00428A_5159_401A_AFE4_6C4126EA9586__INCLUDED_)
