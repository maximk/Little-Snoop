// LittleSnoopDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LittleSnoop.h"
#include "LittleSnoopDlg.h"

#include "LittlePoster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLittleSnoopDlg dialog


CLittleSnoopDlg::CLittleSnoopDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLittleSnoopDlg::IDD, pParent)
	, m_nNewUser(0)
	, m_sUser(_T(_T("")))
	, m_sEmail(_T(_T("")))
	, m_sPassword(_T(_T("")))
	, m_sPassword2(_T(_T("")))
	, m_bAgreeToTerms(FALSE)
	, m_sExistingUser(_T(_T("")))
	, m_sExistingPassword(_T(_T("")))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_LITTLESNOOP);
}

void CLittleSnoopDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_NEWUSER, m_nNewUser);
	DDX_Text(pDX, IDC_EDIT_USER, m_sUser);
	DDX_Text(pDX, IDC_EDIT_EMAIL, m_sEmail);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_sPassword);
	DDX_Text(pDX, IDC_EDIT_PASSWORD2, m_sPassword2);
	DDX_Check(pDX, IDC_CHECK_AGREETERMS, m_bAgreeToTerms);
	DDX_Text(pDX, IDC_EDIT_EXISTINGUSER, m_sExistingUser);
	DDX_Text(pDX, IDC_EDIT_EXISTINGPASSWORD, m_sExistingPassword);
}

BEGIN_MESSAGE_MAP(CLittleSnoopDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RADIO_NEWUSER, OnBnClickedRadioNewuser)
	ON_BN_CLICKED(IDC_RADIO_EXISTING, OnBnClickedRadioExisting)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, OnBnClickedButtonRegister)
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnBnClickedButtonTest)
END_MESSAGE_MAP()


// CLittleSnoopDlg message handlers

BOOL CLittleSnoopDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLittleSnoopDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLittleSnoopDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//EOF

void CLittleSnoopDlg::OnBnClickedRadioNewuser()
{
	GetDlgItem(IDC_EDIT_USER)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_EMAIL)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_AGREETERMS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_REGISTER)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT_EXISTINGUSER)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_EXISTINGPASSWORD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(FALSE);
}

void CLittleSnoopDlg::OnBnClickedRadioExisting()
{
	GetDlgItem(IDC_EDIT_USER)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_EMAIL)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_AGREETERMS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_REGISTER)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_EXISTINGUSER)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_EXISTINGPASSWORD)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(TRUE);
}

void CLittleSnoopDlg::OnBnClickedButtonRegister()
{
	UpdateData(TRUE);

	if (m_sUser == _T(""))
	{
		AfxMessageBox("Please enter the user name.");
		GetDlgItem(IDC_EDIT_USER)->SetFocus();
		return;
	}

	if (m_sEmail == _T(""))
	{
		AfxMessageBox("Please enter your e-mail.");
		GetDlgItem(IDC_EDIT_EMAIL)->SetFocus();
		return;
	}

	if (m_sPassword == _T(""))
	{
		AfxMessageBox("Please enter the password.");
		GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
		return;
	}

	if (m_sPassword != m_sPassword2)
	{
		AfxMessageBox("Passwords do not match.");
		GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
		return;
	}

	if (!m_bAgreeToTerms)
	{
		AfxMessageBox("You need to agree to Terms of Use to register. Terms of Use are available at snoopon.me/_doc/terms_of_use.html");
		GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
		return;
	}

	CString doc;
	doc.Format("{\"display_name\":\"%s\",\"password\":\"%s\","
			"\"email\":\"%s\",\"inv_id\":\"LittleSnoop\"}",
		m_sUser, m_sPassword, m_sEmail);

	CLittlePoster poster;
	if (poster.post(CLittlePoster::REGISTER, doc, "") < 300)
	{
		AfxMessageBox("Registration succeeded. Little Snoop will now start to send screen snapshots according to schedule.");
		EndDialog(IDOK);
	}
	else
	{
		AfxMessageBox("Registration failed.");
	}
}

//EOF

void CLittleSnoopDlg::OnBnClickedButtonTest()
{
	UpdateData(TRUE);

	if (m_sExistingUser == _T(""))
	{
		AfxMessageBox("Please enter the user name.");
		GetDlgItem(IDC_EDIT_EXISTINGUSER)->SetFocus();
		return;
	}

	if (m_sExistingPassword == _T(""))
	{
		AfxMessageBox("Please enter the password.");
		GetDlgItem(IDC_EDIT_EXISTINGPASSWORD)->SetFocus();
		return;
	}

	CLittlePoster poster;
	if (poster.testCredentials(m_sExistingUser, m_sExistingPassword))
	{
		AfxMessageBox("Your credentials are ok.");
		EndDialog(IDOK);
	}
	else
	{
		AfxMessageBox("Credentials test failed.");
	}
}
