// SettingsPage2.cpp : implementation file
//

#include "stdafx.h"
#include "LittleSnoop.h"
#include "SettingsPage2.h"
#include "LittlePoster.h"


// CSettingsPage2 dialog

IMPLEMENT_DYNAMIC(CSettingsPage2, CPropertyPage)
CSettingsPage2::CSettingsPage2()
	: CPropertyPage(CSettingsPage2::IDD)
	, m_sUserName(_T(""))
	, m_sPassword(_T(""))
	, m_sSnoopOnMeURI(_T(""))
{
}

CSettingsPage2::~CSettingsPage2()
{
}

void CSettingsPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_sUserName);
	DDX_Text(pDX, IDC_EDIT2, m_sPassword);
	DDX_Text(pDX, IDC_EDIT3, m_sSnoopOnMeURI);
}


BEGIN_MESSAGE_MAP(CSettingsPage2, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


// CSettingsPage2 message handlers

void CSettingsPage2::OnBnClickedButton1()
{
	UpdateData(TRUE);

	CLittlePoster poster;
	if (poster.testCredentials(m_sUserName, m_sPassword))
	{
		AfxMessageBox("Your credentials are ok.");
	}
	else
	{
		AfxMessageBox("Credentials verification failed.");
	}
}
