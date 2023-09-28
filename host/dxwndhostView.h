// dxwndhostView.h : CDxwndhostView class definition and interface declarations.
//
/////////////////////////////////////////////////////////////////////////////

#include "SystemTray.h"

#if !defined(AFX_DXWNDHOSTVIEW_H__E8E112CE_6FE9_4F01_AB94_D53F844D5A09__INCLUDED_)
#define AFX_DXWNDHOSTVIEW_H__E8E112CE_6FE9_4F01_AB94_D53F844D5A09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef enum {
    RUN_DEFAULT = 0,
    RUN_UNHOOKED,
    RUN_SUSPENDED,
    RUN_PANIC
} RunEnumType;

class CDxwndhostView : public CListView {
protected: // Create from serialization only features.
    CDxwndhostView();
    afx_msg void OnHotKey(UINT, UINT, UINT);
    DECLARE_DYNCREATE(CDxwndhostView)
    BOOL isUpdated;
    BOOL isRegistryUpdated;
    BOOL isDriversUpdated;
    void SaveConfigFile();
    void ZapConfigFile();
    BOOL OnInitDialog();
    HICON Extract(CString path);
    char *Serialize(HICON);
    HICON DeSerialize(char *);

private:
    void Resize(void);
    TARGETMAP TargetMaps[MAXTARGETS];
    PRIVATEMAP PrivateMaps[MAXTARGETS];
    DEVMODE InitDevMode;
    CSystemTray SystemTray;
    int LastX, LastY, LastCX, LastCY;
    CImageList m_cImageListNormal, m_cImageListSmall, m_cIconList;
    BOOL bIconViewMode;
    void AppendItemToList(TARGETMAP *, PRIVATEMAP *, int, LV_ITEM *);
    void OnKeyUp(UINT, UINT, UINT);
    void OnKeyDown(UINT, UINT, UINT);
    BOOL InitClearType;
    BOOL InitFontSmoothing;
    UINT InitFontSmoothingContrast;
    UINT InitFontSmoothingOrientation;
    UINT InitFontSmoothingType;
    STICKYKEYS StartupStickyKeys;
    TOGGLEKEYS StartupToggleKeys;
    FILTERKEYS StartupFilterKeys;
    DWORD KeyPressed;
    DWORD KeyTime;

    // Attributes
public:
    CDxwndhostDoc *GetDocument();
    virtual ~CDxwndhostView();
    void Import(char *);
    virtual void OnDraw(CDC *pDC);  // Be overridden when rendering this view.
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    int GetNumTargets(void);
    afx_msg void OnRun(int);
    afx_msg void OnRun(int, int);
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

protected:
    virtual void OnInitialUpdate(); // Called once after the initial construction.
    void UpdateHotKeys();
    void RenewItemList(CListCtrl *);
    void RenewIconList(CListCtrl *);

    // Generated message map functions
    //{{AFX_MSG(CDxwndhostView)
    afx_msg void OnDblclk(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnAdd();
    afx_msg void OnAdd(char *);
    afx_msg void OnDuplicate();
    afx_msg void OnModify();
    afx_msg void OnExport();
    afx_msg void OnImport();
    afx_msg void OnOpenConfig();
    afx_msg void OnExploreDxWnd();
    BOOL OnImport(CString);
    afx_msg void OnDelete();
    afx_msg void OnExplore();
    afx_msg void OnKill();
    afx_msg void OnKill(BOOL);
    afx_msg void OnProcessKill();
    afx_msg void OnProcessKillAll();
    afx_msg void OnPause();
    afx_msg void OnResume();
    afx_msg void OnWindowRestore();
    afx_msg void OnWindowMinimize();
    afx_msg void OnWindowClose();
    afx_msg void OnTaskbarHide();
    afx_msg void OnTaskbarShow();
    afx_msg void OnSort();
    afx_msg void OnGlobalSettings();
    afx_msg void OnGlobalStartUp();
    afx_msg void OnExpertModeToggle();
    afx_msg void OnViewLog();
    afx_msg void OnViewLogTail();
    afx_msg void OnViewGUILog();
    afx_msg void OnDeleteLog();
    afx_msg void OnDebugView();
    afx_msg void OnSetRegistry();
    afx_msg void OnSetPath();
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRun();
    afx_msg void OnRunPanic();
    afx_msg void OnClearAllLogs();
    afx_msg void OnGoToTrayIcon();
    afx_msg void OnSaveFile();
    afx_msg void OnHookStart();
    afx_msg void OnInitMenu();
    afx_msg void OnHookStop();
    afx_msg void OnTrayRestore();
    afx_msg void OnViewStatus();
    afx_msg void OnViewShims();
    afx_msg void OnViewGammaRamp();
    afx_msg void OnViewMousePosition();
    afx_msg void OnViewCDPlayer();
    afx_msg void OnGammaCtrl();
    afx_msg void OnViewDesktop();
    afx_msg void OnViewPalette();
    afx_msg void OnViewTimeSlider();
    afx_msg void OnViewVirtualJoystick();
    afx_msg void OnExit();
    afx_msg void OnDesktopcolordepth8bpp();
    afx_msg void OnDesktopcolordepth16bpp();
    afx_msg void OnDesktopcolordepth24bpp();
    afx_msg void OnDesktopcolordepth32bpp();
    afx_msg void OnRecoverScreenMode();
    afx_msg void OnRunBlacker();
    afx_msg void OnAssociate();
    afx_msg void OnDisassociate();
    afx_msg void OnCDCharger();
    afx_msg void OnClearCompatibilityFlags();
    afx_msg void OnRecoverSystemColors();
    afx_msg void OnRecoverGammaRamp();
    afx_msg void OnMoveTop();
    afx_msg void OnMoveUp();
    afx_msg void OnMoveDown();
    afx_msg void OnMoveBottom();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnViewFlags();
    afx_msg void OnViewImportTable();
    afx_msg void OnProxyRemove();
    afx_msg void OnProxyDdraw();
    afx_msg void OnProxyD3D8();
    afx_msg void OnProxyD3D9();
    afx_msg void OnProxyWinG32();
    afx_msg void OnProxyDInput();
    afx_msg void OnProxyWinMM();
    afx_msg void OnListIcons();
    afx_msg void OnListDetails();
    afx_msg void OnRButtonDown();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    void OnProxyInstall(char *);
    void OnEmergencyKill(void);
    void OnProcessKill(BOOL);
};

#ifndef _DEBUG  // dxwndhostView.cpp for debugging environment.
inline CDxwndhostDoc *CDxwndhostView::GetDocument() {
    return (CDxwndhostDoc *)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ Will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DXWNDHOSTVIEW_H__E8E112CE_6FE9_4F01_AB94_D53F844D5A09__INCLUDED_)
