// SettingsPage1.cpp : implementation file
//

#include "stdafx.h"
#include "LittleSnoop.h"
#include "SettingsPage1.h"


// CSettingsPage1 dialog

IMPLEMENT_DYNAMIC(CSettingsPage1, CPropertyPage)
CSettingsPage1::CSettingsPage1()
	: CPropertyPage(CSettingsPage1::IDD)
	, m_bStopTemporarily(FALSE)
	, m_nSchedule(0)
{
}

CSettingsPage1::~CSettingsPage1()
{
}

void CSettingsPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_bStopTemporarily);
	DDX_Radio(pDX, IDC_RADIO1, m_nSchedule);
}


BEGIN_MESSAGE_MAP(CSettingsPage1, CPropertyPage)
END_MESSAGE_MAP()


// CSettingsPage1 message handlers
