#include "dxTabConfCtrl.h"
#include "specialedit.h"

#pragma once

typedef struct {
    int iLabelResourceId;
    char *sLabel;
    int iComboResourceId;
    char *sIniLabel;
    int cursor;
    DWORD dwKey;
} Key_Type;

typedef struct {
    DWORD dwVKeyCode;
    char *sVKeyLabel;
} KeyCombo_Type;

// CGlobalSettings dialog

class CGlobalSettings : public CDialog {
    DECLARE_DYNAMIC(CGlobalSettings)

public:
    CGlobalSettings(CWnd *pParent = NULL);   // standard constructor
    virtual ~CGlobalSettings();
    CDXTabConfCtrl	m_tabdxTabConfCtrl;

    // Dialog Data
    enum { IDD = IDD_GLOBAL_CONFIG };

    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

    BOOL m_DebugMode;
    BOOL m_UpdatePaths;
    BOOL m_AutoHideMode;
    BOOL m_HideOnEscape;
    BOOL m_CheckAdminRights;
    BOOL m_NameFromFolder;
    BOOL m_MultiHooks;
    BOOL m_WarnOnExit;
    BOOL m_32BitIcons;
    BOOL m_GrayIcons;
    BOOL m_AutoSave;
    int m_TexMinX;
    int m_TexMinY;
    int m_TexMaxX;
    int m_TexMaxY;
    int m_DefaultCoordinates;
    int m_DefaultPosX;
    int m_DefaultPosY;
    int m_DefaultSizX;
    int m_DefaultSizY;
    int m_OverlayPosition;
    int m_OverlayStyle;

    DECLARE_MESSAGE_MAP()
protected:
    CSpecialEdit m_EditPosX;
    CSpecialEdit m_EditPosY;
    virtual void OnOK();
    virtual void OnHelp();

    //DECLARE_MESSAGE_MAP()

};
