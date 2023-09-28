// TabTweaks.cpp : implementation file
//

#include "stdafx.h"
#include "dxwndhost.h"
#include "TargetDlg.h"
#include "TabTweaks.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabTweaks dialog

TweakEntry_Type *gAvailableTweaks;

CTabTweaks::CTabTweaks(CWnd *pParent /*=NULL*/)
    : CDialog(CTabTweaks::IDD, pParent) {
    //{{AFX_DATA_INIT(CTabTweaks)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

BOOL CTabTweaks::PreTranslateMessage(MSG *pMsg) {
    //if(((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
    if((pMsg->message == WM_KEYDOWN)
            && (pMsg->wParam == VK_RETURN))
        return TRUE;
    return CWnd::PreTranslateMessage(pMsg);
}

void CTabTweaks::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}

BOOL CTabTweaks::OnInitDialog() {
    CListBox *ListAvail, *ListActive;
    CButton *pBtn;
    HICON hIcn;
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    int i;
    static TweakEntry_Type AvailableTweaks[] = {
        { "FIXD3DFRAME",		"d3d:FixD3DWindowFrame", 0 },
        { "NOWINDOWMOVE",		"d3d:NoD3DWindowMove", 0 },
        { "BACKBUFATTACH",		"ddraw:MakeBackbufferAttachable", 0 },
        { "BLITFROMBACKBUFFER", "ddraw:BlitFromBackbuffer", 0 },
        { "NOSYSMEMPRIMARY",	"ddraw:NoSystemMemoryOnPrimary", 0 },
        { "NOSYSMEMBACKBUF",	"ddraw:NoSystemMemoryOnBackbuffer", 0 },
        { "ALLOWSYSMEMON3DDEV",	"ddraw:AllowSysMemoryOn3DDevices", 0 },
        { "RETURNNULLREF",		"ddraw:ReturnNULLReference", 0 },
        { "FORCESHEL",			"ddraw:ForcesHEL", 0 },
        { "CACHED3DSESSION",	"d3d:CacheD3DSession", 0 },
        { "LOCKCOLORDEPTH",		"ddraw:LockColorDepth", 0 },
        { "FIXPARENTWIN",		"hook:FixParentWindow", 0 },
        { "MOUSEMOVEBYEVENT",	"input:MouseMoveByEvent", 0 },
        { "NODESTROYWINDOW",	"win:NoMainWindowDestruction", 0 },
        { "ACTIVATEAPP",		"win:SendWM_ACTIVATEAPPMessage", 0 },
        { "D3DRESOLUTIONHACK",	"d3d:LegacyD3DResolutionHack", 0 },
        { "FIXAILSOUNDLOCKS",	"sound:FixAILSoundLocks", 0 },
        { "SUPPRESSIME",		"win:SuppressIME", 0 },
        { "QUARTERBLT",			"win:FPSCountUpdatesBigger1/4Screen", 0 },
        { "MAKEWINVISIBLE",		"win:MakeQueriedWindowVisible", 0 },
        { "FIXEMPIREOFS",		"patch:FixEmpireOfTheFadingSuns", 0 },
        { "KILLBLACKWIN",		"win:KillBlackWindows", 0 },
        { "ZERODISPLAYCOUNTER",	"cursor:ZeroDisplayCounter", 0 },
        { "REPLACEDIALOGS",		"win:ReplaceDialogs", 0 },
        { "MANAGEFOURCC",		"ddraw:ManageFourCCSurfaces", 0 },
        { "LOCKFPSCORNER",		"win:LockFPSCorner", 0 },
        { "SETZBUFFER16BIT",	"ddraw:Forces16BitZBufferBitDepth", 0 },
        { "SETZBUFFER24BIT",	"ddraw:Forces24BitZBufferBitDepth", 0 },
        { "LOCK24BITDEPTH",		"ddraw:Lock24BitDepth", 0 },
        { "FULLPAINTRECT",		"win:FullPaintRect", 0 },
        { "PUSHACTIVEMOVIE",	"win:PushActiveMovieWindows", 0 },
        { "FORCECLIPCHILDREN",	"win:ForceClipChildren", 0 },
        { "PREVENTMINIMIZE",	"win:PreventMinimizeWindow", 0 },
        { "NOACCESSIBILITY",	"input:NoAccessibilityKeys", 0 },
        { "IGNOREDEBOUTPUT",	"shim:IgnoreDebugOutput", 0 },
        { "NOOLEINITIALIZE",	"patch:NoOleInitialize", 0 },
        { "CHAOSOVERLORDSFIX",	"patch:ChaosOverlordsFix", 0 },
        { "FIXFOLDERPATHS",		"shim:FixFolderPaths", 0 },
        { "NOCOMPLEXMIPMAPS",	"ddraw:NoComplexMipmaps", 0 },
        { "INVALIDATECLIENT",	"ddraw:InvalidateClient", 0 },
        { "CREATEDCHOOK",		"hook:CreateDCHook", 0 },
        { "SAFEPRIMLOCK",		"ddraw:SafePrimaryLock", 0 },
        { "D3D8MAXWINMODE",		"d3d:D3D8MaximizeWinModeHack", 0 },
        { "MUTEX4CRITSECTION",	"shim:MutexForCriticalSection", 0 },
        { "DELAYCRITSECTION",	"shim:DelayCriticalSection", 0 },
        { "REMAPNUMKEYPAD",		"input:RemapNumericKeypad", 0 },
        { "SETUSKEYDESCR",		"input:SetUSKeyDescr", 0 },
        { "FORCESHAL",			"ddraw:ForcesHAL", 0 },
        { "FORCESNULL",			"ddraw:ForcesNULL", 0 },
        { "SMACKBUFFERNODEPTH",	"ddraw:SmackBufferNoDepth", 0 },
        { "LOCKSYSSETTINGS",	"patch:LockSystemSettings", 0 },
        { "PROJECTBUFFER",		"win:ProjectBuffer", 0 },
        { "FORCERELAXIS",		"input:ForceRelativeAxis", 0 },
        { "FORCEABSAXIS",		"input:ForceAbsoluteAxis", 0 },
        { "DIRECTXREPLACE",		"hook:ReplaceDirectXDLLs", 0 },
        { "W98OPAQUEFONT",		"shim:Win98OpaqueFont", 0 },
        { "FAKEGLOBALATOM",		"shim:FakeGlobalAtom", 0 },
        { "REVERTDIBPALETTE",	"patch:RevertDIBPalette", 0 },
        { "FIXDCALREADYCREATED", "ddraw:FixDCAlreadyCreated", 0 },
        { "SUPPRESSMENUS",		"win:SuppressMenus", 0 },
        { "KILLDEADLOCKS",		"patch:KillDeadlocks", 0 },
        { NULL, NULL, NULL }
    };
    // v2.04.30.fx1: the cTarget class is rebuilt, so each time all BOOL pointers must be reassigned!!!
    for(i = 0; ; i++) {
        if(AvailableTweaks[i].Label == NULL) break;
        switch(i) {
        case 0:
            AvailableTweaks[i].Flag = &(cTarget->m_FixD3DFrame);
            break;
        case 1:
            AvailableTweaks[i].Flag = &(cTarget->m_NoWindowMove);
            break;
        case 2:
            AvailableTweaks[i].Flag = &(cTarget->m_BackBufAttach);
            break;
        case 3:
            AvailableTweaks[i].Flag = &(cTarget->m_BlitFromBackBuffer);
            break;
        case 4:
            AvailableTweaks[i].Flag = &(cTarget->m_NoSysMemPrimary);
            break;
        case 5:
            AvailableTweaks[i].Flag = &(cTarget->m_NoSysMemBackBuf);
            break;
        case 6:
            AvailableTweaks[i].Flag = &(cTarget->m_AllowSysmemOn3DDev);
            break;
        case 7:
            AvailableTweaks[i].Flag = &(cTarget->m_ReturnNullRef);
            break;
        case 8:
            AvailableTweaks[i].Flag = &(cTarget->m_ForcesHEL);
            break;
        case 9:
            AvailableTweaks[i].Flag = &(cTarget->m_CacheD3DSession);
            break;
        case 10:
            AvailableTweaks[i].Flag = &(cTarget->m_LockColorDepth);
            break;
        case 11:
            AvailableTweaks[i].Flag = &(cTarget->m_FixParentWin);
            break;
        case 12:
            AvailableTweaks[i].Flag = &(cTarget->m_MouseMoveByEvent);
            break;
        case 13:
            AvailableTweaks[i].Flag = &(cTarget->m_NoDestroyWindow);
            break;
        case 14:
            AvailableTweaks[i].Flag = &(cTarget->m_ActivateApp);
            break;
        case 15:
            AvailableTweaks[i].Flag = &(cTarget->m_D3DResolutionHack);
            break;
        case 16:
            AvailableTweaks[i].Flag = &(cTarget->m_FixAILSoundLocks);
            break;
        case 17:
            AvailableTweaks[i].Flag = &(cTarget->m_SuppressIME);
            break;
        case 18:
            AvailableTweaks[i].Flag = &(cTarget->m_QuarterBlt);
            break;
        case 19:
            AvailableTweaks[i].Flag = &(cTarget->m_MakeWinVisible);
            break;
        case 20:
            AvailableTweaks[i].Flag = &(cTarget->m_FixEmpireOFS);
            break;
        case 21:
            AvailableTweaks[i].Flag = &(cTarget->m_KillBlackWin);
            break;
        case 22:
            AvailableTweaks[i].Flag = &(cTarget->m_ZeroDisplayCounter);
            break;
        case 23:
            AvailableTweaks[i].Flag = &(cTarget->m_ReplaceDialogs);
            break;
        case 24:
            AvailableTweaks[i].Flag = &(cTarget->m_HandleFourCC);
            break;
        case 25:
            AvailableTweaks[i].Flag = &(cTarget->m_LockFPSCorner);
            break;
        case 26:
            AvailableTweaks[i].Flag = &(cTarget->m_SetZBuffer16Bit);
            break;
        case 27:
            AvailableTweaks[i].Flag = &(cTarget->m_SetZBuffer24Bit);
            break;
        case 28:
            AvailableTweaks[i].Flag = &(cTarget->m_Lock24BitDepth);
            break;
        case 29:
            AvailableTweaks[i].Flag = &(cTarget->m_FullPaintRect);
            break;
        case 30:
            AvailableTweaks[i].Flag = &(cTarget->m_PushActiveMovie);
            break;
        case 31:
            AvailableTweaks[i].Flag = &(cTarget->m_ForceClipChildren);
            break;
        case 32:
            AvailableTweaks[i].Flag = &(cTarget->m_PreventMinimize);
            break;
        case 33:
            AvailableTweaks[i].Flag = &(cTarget->m_NoAccessibility);
            break;
        case 34:
            AvailableTweaks[i].Flag = &(cTarget->m_IgnoreDebOutput);
            break;
        case 35:
            AvailableTweaks[i].Flag = &(cTarget->m_NoOleInitialize);
            break;
        case 36:
            AvailableTweaks[i].Flag = &(cTarget->m_ChaosOverlordsFix);
            break;
        case 37:
            AvailableTweaks[i].Flag = &(cTarget->m_FixFolderPaths);
            break;
        case 38:
            AvailableTweaks[i].Flag = &(cTarget->m_NoComplexMipmaps);
            break;
        case 39:
            AvailableTweaks[i].Flag = &(cTarget->m_InvalidateClient);
            break;
        case 40:
            AvailableTweaks[i].Flag = &(cTarget->m_CreateDCHook);
            break;
        case 41:
            AvailableTweaks[i].Flag = &(cTarget->m_SafePrimLock);
            break;
        case 42:
            AvailableTweaks[i].Flag = &(cTarget->m_D3D8MaxWinMode);
            break;
        case 43:
            AvailableTweaks[i].Flag = &(cTarget->m_Mutex4CritSection);
            break;
        case 44:
            AvailableTweaks[i].Flag = &(cTarget->m_DelayCritSection);
            break;
        case 45:
            AvailableTweaks[i].Flag = &(cTarget->m_RemapNumKeypad);
            break;
        case 46:
            AvailableTweaks[i].Flag = &(cTarget->m_SetUSKeyDescr);
            break;
        case 47:
            AvailableTweaks[i].Flag = &(cTarget->m_ForcesHAL);
            break;
        case 48:
            AvailableTweaks[i].Flag = &(cTarget->m_ForcesNULL);
            break;
        case 49:
            AvailableTweaks[i].Flag = &(cTarget->m_SmackBufferNoDepth);
            break;
        case 50:
            AvailableTweaks[i].Flag = &(cTarget->m_LockSysSettings);
            break;
        case 51:
            AvailableTweaks[i].Flag = &(cTarget->m_ProjectBuffer);
            break;
        case 52:
            AvailableTweaks[i].Flag = &(cTarget->m_ForceRelAxis);
            break;
        case 53:
            AvailableTweaks[i].Flag = &(cTarget->m_ForceAbsAxis);
            break;
        case 54:
            AvailableTweaks[i].Flag = &(cTarget->m_DirectXReplace);
            break;
        case 55:
            AvailableTweaks[i].Flag = &(cTarget->m_W98OpaqueFont);
            break;
        case 56:
            AvailableTweaks[i].Flag = &(cTarget->m_FakeGlobalAtom);
            break;
        case 57:
            AvailableTweaks[i].Flag = &(cTarget->m_RevertDIBPalette);
            break;
        case 58:
            AvailableTweaks[i].Flag = &(cTarget->m_FixDCAlreadyCreated);
            break;
        case 59:
            AvailableTweaks[i].Flag = &(cTarget->m_SuppressMenus);
            break;
        case 60:
            AvailableTweaks[i].Flag = &(cTarget->m_KillDeadlocks);
            break;
        }
    }
    AfxEnableControlContainer();
    ListAvail = (CListBox *)this->GetDlgItem(IDC_LIST_TWEAKSAVAIL);
    ListActive = (CListBox *)this->GetDlgItem(IDC_LIST_TWEAKSACTIVE);
    ListAvail->ResetContent();
    ListActive->ResetContent();
    for(i = 0; ; i++) {
        if(AvailableTweaks[i].Label == NULL) break;
        if(AvailableTweaks[i].Caption[0] == '#') continue; // quick way to comment out entries ....
        ListAvail->AddString(AvailableTweaks[i].Caption);
        if(*(AvailableTweaks[i].Flag)) {
            ListActive->AddString(AvailableTweaks[i].Caption);
            OutTrace("Added tweak \"%s\"\n", AvailableTweaks[i].Label);
        }
    }
    gAvailableTweaks = &AvailableTweaks[0];
    pBtn = (CButton *)this->GetDlgItem(IDC_TWEAK_ADD);
    pBtn->ModifyStyle(0, BS_ICON);
    hIcn = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_TWEAK_ADD), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    pBtn->SetIcon(hIcn);
    pBtn = (CButton *)this->GetDlgItem(IDC_TWEAK_REMOVE);
    pBtn->ModifyStyle(0, BS_ICON);
    hIcn = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_TWEAK_REMOVE), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    pBtn->SetIcon(hIcn);
    pBtn = (CButton *)this->GetDlgItem(IDC_TWEAK_HELP);
    pBtn->ModifyStyle(0, BS_ICON);
    hIcn = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_TWEAK_HELP), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    pBtn->SetIcon(hIcn);
    CDialog::OnInitDialog();
    return TRUE;
}

BEGIN_MESSAGE_MAP(CTabTweaks, CDialog)
    //{{AFX_MSG_MAP(CTabTweaks)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_TWEAK_ADD, &CTabTweaks::OnBnClickedTweakAdd)
    ON_BN_CLICKED(IDC_TWEAK_REMOVE, &CTabTweaks::OnBnClickedTweakRemove)
    ON_BN_CLICKED(IDC_TWEAK_HELP, &CTabTweaks::OnBnClickedTweakHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabTweaks message handlers


void CTabTweaks::OnBnClickedTweakAdd() {
    CListBox *ListAvail, *ListActive;
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    char sBuffer[256];
    int i, k;
    //MessageBox("Add button clicked","click",0);
    ListAvail = (CListBox *)this->GetDlgItem(IDC_LIST_TWEAKSAVAIL);
    ListActive = (CListBox *)this->GetDlgItem(IDC_LIST_TWEAKSACTIVE);
    int nCount = ListAvail->GetSelCount();
    CArray<int, int> aryListBoxSel;
    aryListBoxSel.SetSize(nCount);
    ListAvail->GetSelItems(nCount, aryListBoxSel.GetData());
    for(int j = 0; j < nCount; j++) {
        //i = ListAvail->GetCurSel();
        i = aryListBoxSel.GetAt(j);
        if (i == LB_ERR) return;
        // search for a matching string - the ListAvail list is sorted!
        ListAvail->GetText(i, sBuffer);
        for(k = 0; ; k++) {
            if(gAvailableTweaks[k].Label == NULL) return;
            if(!strcmp(gAvailableTweaks[k].Caption, sBuffer)) break;
        }
        //MessageBox(gAvailableTweaks[i].Caption,"select",0);
        if(!*(gAvailableTweaks[k].Flag)) {
            //MessageBox(gAvailableTweaks[i].Caption,"add",0);
            *(gAvailableTweaks[k].Flag) = TRUE;
            ListActive->AddString(gAvailableTweaks[k].Caption);
            OutTrace("Added tweak \"%s\"\n", gAvailableTweaks[k].Label);
        }
    }
    ListAvail->SetSel(-1, FALSE); // deselect all ....
}

void CTabTweaks::OnBnClickedTweakRemove() {
    CListBox *ListActive;
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    char sBuffer[256];
    int i, k;
    //ListAvail=(CListBox *)this->GetDlgItem(IDC_LIST_TWEAKSAVAIL);
    ListActive = (CListBox *)this->GetDlgItem(IDC_LIST_TWEAKSACTIVE);
    int nCount = ListActive->GetSelCount();
    CArray<int, int> aryListBoxSel;
    aryListBoxSel.SetSize(nCount);
    ListActive->GetSelItems(nCount, aryListBoxSel.GetData());
    for(int j = 0; j < nCount; j++) {
        //i = ListActive->GetCurSel();
        i = aryListBoxSel.GetAt(j);
        if (i == LB_ERR) return;
        ListActive->GetText(i, sBuffer);
        //ListActive->DeleteString(i);
        //MessageBox(sBuffer,"delete",0);
        for(k = 0; ; k++) {
            if(gAvailableTweaks[k].Label == NULL) break;
            if(!strcmp(gAvailableTweaks[k].Caption, sBuffer)) {
                *(gAvailableTweaks[k].Flag) = FALSE;
                OutTrace("Removed tweak \"%s\"\n", gAvailableTweaks[k].Label);
            }
        }
    }
    ListActive->SetSel(-1, FALSE); // deselect all ....
    // rebuild the whole list!
    ListActive->ResetContent();
    for(i = 0; ; i++) {
        if(gAvailableTweaks[i].Label == NULL) break;
        if(gAvailableTweaks[i].Caption[0] == '#') continue; // quick way to comment out entries ....
        if(*(gAvailableTweaks[i].Flag)) {
            ListActive->AddString(gAvailableTweaks[i].Caption);
            OutTrace("Added tweak \"%s\"\n", gAvailableTweaks[i].Label);
        }
    }
}

#define MAX_HELP 1024

void CTabTweaks::OnBnClickedTweakHelp() {
    char sHelp[MAX_HELP];
    char sHelpBuffer[MAX_HELP];
    char sHelpPath[MAX_PATH];
    CListBox *ListAvail;
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    char sBuffer[256];
    char *Title = "tweaks help";
    int i, k;
    char *p;
    ListAvail = (CListBox *)this->GetDlgItem(IDC_LIST_TWEAKSAVAIL);
    int nCount = ListAvail->GetSelCount();
    if(nCount == 0) {
        MessageBox("Please, select one item in available tweaks list", Title, 0);
        return;
    }
    if(nCount > 1) {
        MessageBox("Please, select only ONE item in available tweaks list", Title, 0);
        return;
    }
    int aryListBoxSel[1];
    ListAvail->GetSelItems(nCount, aryListBoxSel);
    i = aryListBoxSel[0];
    if (i == LB_ERR) return;
    // search for a matching string - the ListAvail list is sorted!
    ListAvail->GetText(i, sBuffer);
    for(k = 0; ; k++) {
        if(gAvailableTweaks[k].Label == NULL) return;
        if(!strcmp(gAvailableTweaks[k].Caption, sBuffer)) break;
    }
    p = strstr(sBuffer, ":") + 1;
    //sprintf(sHelpPath, "%s\\tweaks.ini", gInitPath);
    // v2.04.41: fixed path for tweaks.ini help file
    strncpy(sHelpPath, gInitPath, MAX_PATH);
    char *q = sHelpPath + (strlen(sHelpPath) - strlen("dxwnd.ini"));
    strcpy(q, "tweaks.ini");
    GetPrivateProfileString(p, "help", "help unavailable", sHelp, MAX_HELP, sHelpPath);
    sprintf(sHelpBuffer, "%s\n%s\n\n%s", p, gAvailableTweaks[k].Label, sHelp);
    MessageBox(sHelpBuffer, Title, 0);
}
