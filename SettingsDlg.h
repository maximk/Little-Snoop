#pragma once


// CSettingsDlg dialog

class CSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingsDlg)

public:
	CSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsDlg();

// Dialog Data
	enum { IDD = IDD_SETTINGS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString sHostName;
	int nPort;
	CString sUserName;
	CString sPassword;
	int nSchedule;
	int nShrinkWidth;
	int nShrinkHeight;
	int nThumbWidth;
	int nThumbHeight;
};
