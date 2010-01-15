// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LittleSnoop.h"
#include "SettingsDlg.h"


// CSettingsDlg dialog

IMPLEMENT_DYNAMIC(CSettingsDlg, CDialog)
CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDlg::IDD, pParent)
	, sHostName(_T(""))
	, nPort(0)
	, sUserName(_T(""))
	, sPassword(_T(""))
	, nSchedule(0)
	, nShrinkWidth(0)
	, nShrinkHeight(0)
	, nThumbWidth(0)
	, nThumbHeight(0)
{
}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_HOST_EDIT, sHostName);
	DDX_Text(pDX, IDC_PORT_EDIT, nPort);
	DDX_Text(pDX, IDC_USER_EDIT, sUserName);
	DDX_Text(pDX, IDC_PASSWORD_EDIT, sPassword);
	DDX_Text(pDX, IDC_PORT_EDIT2, nSchedule);
	DDV_MinMaxInt(pDX, nSchedule, 1, 3);
	DDX_Text(pDX, IDC_SHRINK_WIDTH, nShrinkWidth);
	DDX_Text(pDX, IDC_SHRINK_HEIGHT, nShrinkHeight);
	DDX_Text(pDX, IDC_THUMB_WIDTH, nThumbWidth);
	DDX_Text(pDX, IDC_THUMB_HEIGHT, nThumbHeight);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
END_MESSAGE_MAP()


// CSettingsDlg message handlers
