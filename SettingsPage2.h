#pragma once


// CSettingsPage2 dialog

class CSettingsPage2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CSettingsPage2)

public:
	CSettingsPage2();
	virtual ~CSettingsPage2();

// Dialog Data
	enum { IDD = IDD_PROPPAGE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_sUserName;
	CString m_sPassword;
	CString m_sSnoopOnMeURI;
	afx_msg void OnBnClickedButton1();
};
