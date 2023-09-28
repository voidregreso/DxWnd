#pragma once


// CCDChargerDialog dialog

class CCDChargerDialog : public CDialog {
    DECLARE_DYNAMIC(CCDChargerDialog)

public:
    CCDChargerDialog(void);   // standard constructor
    virtual ~CCDChargerDialog();

    // Dialog Data
    enum { IDD = IDD_CDCHARGER };
    int iTimeSlider;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
protected:
    virtual void OnOK();
private:
    int m_CDIndex;
};
