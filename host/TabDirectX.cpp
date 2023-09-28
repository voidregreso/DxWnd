// TabDirectX.cpp : implementation file
//

#include "stdafx.h"
#include "TargetDlg.h"
#include "TabDirectX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern dxw_Filter_Type *GetFilterList(void);
extern dxw_Renderer_Type *GetRendererList(void);

//typedef struct {
//	char *name;
//	int xfactor;
//	int yfactor;
//} filter_type;

/////////////////////////////////////////////////////////////////////////////
// CTabDirectX dialog

CTabDirectX::CTabDirectX(CWnd *pParent /*=NULL*/)
    : CDialog(CTabDirectX::IDD, pParent) {
    //{{AFX_DATA_INIT(CTabDirectX)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

BOOL CTabDirectX::PreTranslateMessage(MSG *pMsg) {
    //if(((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
    if((pMsg->message == WM_KEYDOWN)
            && (pMsg->wParam == VK_RETURN))
        return TRUE;
    return CWnd::PreTranslateMessage(pMsg);
}

void CTabDirectX::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    DDX_Radio(pDX, IDC_AUTO, cTarget->m_DXVersion);
    //DDX_Radio(pDX, IDC_NOEMULATESURFACE, cTarget->m_DxEmulationMode);
    DDX_Check(pDX, IDC_BILINEARFILTER, cTarget->m_BilinearFilter);
    DDX_Check(pDX, IDC_BLUREFFECT, cTarget->m_BlurEffect);
    DDX_Check(pDX, IDC_BLACKWHITE, cTarget->m_BlackWhite);
    DDX_Check(pDX, IDC_AUTOREFRESH, cTarget->m_AutoRefresh);
    DDX_Check(pDX, IDC_INDEPENDENTREFRESH, cTarget->m_IndependentRefresh);
    DDX_Check(pDX, IDC_TEXTUREFORMAT, cTarget->m_TextureFormat);
    DDX_Check(pDX, IDC_SUPPRESSRELEASE, cTarget->m_SuppressRelease);
    DDX_Check(pDX, IDC_VIDEOTOSYSTEMMEM, cTarget->m_VideoToSystemMem);
    DDX_Check(pDX, IDC_SUPPRESSDXERRORS, cTarget->m_SuppressDXErrors);
    DDX_Check(pDX, IDC_NOPALETTEUPDATE, cTarget->m_NoPaletteUpdate);
    DDX_Check(pDX, IDC_NOPIXELFORMAT, cTarget->m_NoPixelFormat);
    DDX_Check(pDX, IDC_NOALPHACHANNEL, cTarget->m_NoAlphaChannel);
    DDX_Check(pDX, IDC_NOFLIPEMULATION, cTarget->m_NoFlipEmulation);
    DDX_Check(pDX, IDC_OFFSCREENZBUFFER, cTarget->m_OffscreenZBuffer);
    DDX_Check(pDX, IDC_NOZBUFFERATTACH, cTarget->m_NoZBufferAttach);
    DDX_Check(pDX, IDC_TEXTUREPALETTE, cTarget->m_TexturePalette);
    DDX_Check(pDX, IDC_FLIPEMULATION, cTarget->m_FlipEmulation);
    DDX_Check(pDX, IDC_SETCOMPATIBILITY, cTarget->m_SetCompatibility);
    DDX_Check(pDX, IDC_AEROBOOST, cTarget->m_AEROBoost);
    DDX_CBIndex(pDX, IDC_FILTER_ID, cTarget->m_FilterId);
    DDX_CBIndex(pDX, IDC_RENDERER_ID, cTarget->m_RendererId);
}

BOOL CTabDirectX::OnInitDialog() {
    CDialog::OnInitDialog();
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    CComboBox *cFilters = (CComboBox *)this->GetDlgItem(IDC_FILTER_ID);
    dxw_Filter_Type *filters;
    dxw_Renderer_Type *renderers;
    cFilters->ResetContent();
    filters = GetFilterList();
    for (int i = 0; filters[i].name; i++)
        cFilters->AddString(filters[i].name);
    cFilters->SetCurSel(cTarget->m_FilterId);
    CComboBox *cRenderers = (CComboBox *)this->GetDlgItem(IDC_RENDERER_ID);
    cRenderers->ResetContent();
    renderers = GetRendererList();
    for (int i = 0; renderers[i].name; i++)
        cRenderers->AddString(renderers[i].name);
    cRenderers->SetCurSel(cTarget->m_RendererId);
    cFilters->EnableWindow((renderers[cTarget->m_RendererId].flags & DXWRF_SUPPORTFILTERS) ? 1 : 0);
    return TRUE;
}

BEGIN_MESSAGE_MAP(CTabDirectX, CDialog)
    //{{AFX_MSG_MAP(CTabDirectX)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabDirectX message handlers

