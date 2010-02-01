#pragma once


// CSettingsPage1 dialog

class CSettingsPage1 : public CPropertyPage
{
	DECLARE_DYNAMIC(CSettingsPage1)

public:
	CSettingsPage1();
	virtual ~CSettingsPage1();

// Dialog Data
	enum { IDD = IDD_PROPPAGE1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bStopTemporarily;
	int m_nSchedule;
};
