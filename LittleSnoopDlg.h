// LittleSnoopDlg.h : header file
//

#pragma once


// CLittleSnoopDlg dialog
class CLittleSnoopDlg : public CDialog
{
// Construction
public:
	CLittleSnoopDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LITTLESNOOP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	void EnableControls();

public:
	int m_nNewUser;
	CString m_sUser;
	CString m_sEmail;
	CString m_sPassword;
	CString m_sPassword2;
	BOOL m_bAgreeToTerms;
	CString m_sExistingUser;
	CString m_sExistingPassword;
	afx_msg void OnBnClickedRadioNewuser();
	afx_msg void OnBnClickedRadioExisting();
	afx_msg void OnBnClickedButtonRegister();
	afx_msg void OnBnClickedButtonTest();
};
