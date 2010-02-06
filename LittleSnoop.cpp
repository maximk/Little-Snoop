// LittleSnoop.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "LittleSnoop.h"
#include "LittleSnoopDlg.h"
#include "SnoopOptions.h"
#include "DummyFrame.h"
#include "SettingsPage1.h"
#include "SettingsPage2.h"

#include "ScreenSnapper.h"
#include "LittlePoster.h"

#include "gdiplus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLittleSnoopApp

BEGIN_MESSAGE_MAP(CLittleSnoopApp, CWinApp)
	ON_COMMAND(IDM_ENABLE, OnEnable)
	ON_UPDATE_COMMAND_UI(IDM_ENABLE, OnUpdateEnableUI)
	ON_COMMAND(IDM_SETTINGS, OnSettings)
	ON_UPDATE_COMMAND_UI(IDM_SETTINGS, OnUpdateSettingsUI)
	ON_COMMAND(IDM_EXIT, OnExit)
	ON_UPDATE_COMMAND_UI(IDM_EXIT, OnUpdateExitUI)

	ON_COMMAND(IDM_POSTCAPTURE, OnCapturePost)
	ON_COMMAND(IDM_REGISTERNEW, OnRegisterNew)
END_MESSAGE_MAP()


// CLittleSnoopApp construction

CLittleSnoopApp::CLittleSnoopApp()
: m_pSingleInstanceMutex(NULL)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CLittleSnoopApp object

CLittleSnoopApp theApp;


// CLittleSnoopApp initialization

BOOL CLittleSnoopApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	// InitCommonControls();

	CWinApp::InitInstance();

	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartup(&m_lGdiplusToken, &input, NULL);

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Snoop on.me"));

	// Little Snoop does not show any window except a few dialogs
	// when requested. However MFC requires that m_pMainWnd must
	// point to some window. Thus we create a CDummyFrame instance
	// for this. The additional benefit is that m_pMainWnd provides
	// a message map needed to route taskbar icon and timer events.

	CRuntimeClass* pRuntimeClass = RUNTIME_CLASS(CDummyFrame);
	CDummyFrame *dummy = (CDummyFrame *)pRuntimeClass->CreateObject();
	if (!dummy->Create(NULL, NULL, 0, CRect(10, 10, 100, 100)))
		return FALSE;
	m_pMainWnd = dummy;

	// For testing purposes the dummy window may be made visible
    //m_pMainWnd->ShowWindow(SW_SHOWNORMAL);
    //m_pMainWnd->UpdateWindow();

	// Insure only one instance of Little Snoop is running
	// Try to create a named mutex. If unsuccessful it means
	// that Little Snoop is already running
	m_pSingleInstanceMutex = new CMutex(FALSE, _T("SnoopOnMeLitteSnoop"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox(IDS_EALREADYRUNNING);
		return FALSE;
	}

	// Create options object passed around to place it is needed
	// Load option values from the registry
	CSnoopOptions::load();

	// Set up the Little Snoop tray icon
    ASSERT(setupTrayIcon());

	// Start timer
	m_pMainWnd->SetTimer(1, CSnoopOptions::m_nSchedule*60*1000, NULL);

	// start the application's message pump
    return TRUE;
}

int CLittleSnoopApp::ExitInstance()
{
	// Delete a named mutex used to check that only one instance
	// of Little Snoop is running
	delete m_pSingleInstanceMutex;

	Gdiplus::GdiplusShutdown(m_lGdiplusToken);

	return 0;	//success
}

BOOL CLittleSnoopApp::setupTrayIcon()
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_pMainWnd->GetSafeHwnd();
	nid.uID = 0;
	nid.uFlags = NIF_MESSAGE | NIF_TIP | NIF_ICON;
	nid.uCallbackMessage = WM_USER_NOTIFYICON;
	nid.hIcon = LoadIcon(IDI_LITTLESNOOP);
    
    CString tip;
    if (tip.LoadString(IDS_ICONTIP))
		StringCchCopyN(nid.szTip, sizeof(nid.szTip),
            tip, tip.GetLength());
    else 
        nid.szTip[0] = (TCHAR)'\0';

	return Shell_NotifyIcon(NIM_ADD, &nid);
}

BOOL CLittleSnoopApp::removeTrayIcon()
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_pMainWnd->GetSafeHwnd();
	nid.uID = 0;

	return Shell_NotifyIcon(NIM_DELETE, &nid);
}

void CLittleSnoopApp::OnEnable()
{
	CSnoopOptions::m_nEnabled = !CSnoopOptions::m_nEnabled;
}

void CLittleSnoopApp::OnSettings()
{
	CSettingsPage2 page1;
	CSettingsPage1 page2;

	page1.m_sUserName = CSnoopOptions::m_sUser;
	page1.m_sPassword = CSnoopOptions::m_sPassword;
	if (CSnoopOptions::m_nSchedule == 5)
		page2.m_nSchedule = 0;
	else if (CSnoopOptions::m_nSchedule == 10)
		page2.m_nSchedule = 1;
	else
		page2.m_nSchedule = 2;
	page2.m_bStopTemporarily = !CSnoopOptions::m_nEnabled;

	if (CSnoopOptions::m_nCapturePort != 80)
		page1.m_sSnoopOnMeURI.Format("http://%s:%d%s",
			CSnoopOptions::m_sCaptureHost,
			CSnoopOptions::m_nCapturePort,
			CSnoopOptions::m_sCapturePath);
	else
		page1.m_sSnoopOnMeURI.Format("http://%s%s",
			CSnoopOptions::m_sCaptureHost,
			CSnoopOptions::m_sCapturePath);

	CPropertySheet dlg(_T("Little Snoop Settings"));
	dlg.AddPage(&page1);
	dlg.AddPage(&page2);
	if (dlg.DoModal() == IDOK)
	{
		CSnoopOptions::m_sUser = page1.m_sUserName;
		CSnoopOptions::m_sPassword = page1.m_sPassword;
		if (page2.m_nSchedule == 0)
			CSnoopOptions::m_nSchedule = 5;
		else if (page2.m_nSchedule == 1)
			CSnoopOptions::m_nSchedule = 10;
		else
			CSnoopOptions::m_nSchedule = 30;
		CSnoopOptions::m_nEnabled = !page2.m_bStopTemporarily;

		CSnoopOptions::update();
	}
}

void CLittleSnoopApp::OnExit()
{
	// Get rid of the cute puppy icon in the tray
	ASSERT(removeTrayIcon());	

	// destroying a dummy window magically closes the application
	m_pMainWnd->DestroyWindow();
}

void CLittleSnoopApp::OnCapturePost()
{
	if (CSnoopOptions::m_sUser == _T("") || CSnoopOptions::m_sPassword == _T(""))
	{
		NOTIFYICONDATA nid;
		nid.cbSize = sizeof(nid);
		nid.hWnd = m_pMainWnd->GetSafeHwnd();
		nid.uID = 0;
		nid.uFlags = NIF_INFO;
		nid.uTimeout = 1;
		nid.dwInfoFlags = NIIF_NONE;

		CString info;
		if (info.LoadString(IDS_NO_CREDENTIALS))
			StringCchCopyN(nid.szInfo, sizeof(nid.szInfo),
				info, info.GetLength());
		else 
			nid.szInfo[0] = (TCHAR)'\0';

		CString title;
		if (title.LoadString(IDS_NO_CREDENTIALS_TITLE))
			StringCchCopyN(nid.szInfoTitle, sizeof(nid.szInfoTitle),
				title, title.GetLength());
		else 
			nid.szInfoTitle[0] = (TCHAR)'\0';

		ASSERT(Shell_NotifyIcon(NIM_MODIFY, &nid));
	}
	else
	{
		CScreenSnapper snapper;
		CString doc = snapper.snap();

		if (doc == "")	// all monitors are black -- nothing to post
			return;
		
		CLittlePoster poster;
		poster.post(CLittlePoster::CAPTURE, doc, CSnoopOptions::m_sUser);
		// return value ignored -- for now
	}
}

void CLittleSnoopApp::OnRegisterNew()
{
	CLittleSnoopDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		if (dlg.m_nNewUser == 0)
		{
			CSnoopOptions::m_sUser = dlg.m_sUser;
			CSnoopOptions::m_sPassword = dlg.m_sPassword;
		}
		else
		{
			CSnoopOptions::m_sUser = dlg.m_sExistingUser;
			CSnoopOptions::m_sPassword = dlg.m_sExistingPassword;
		}

		CSnoopOptions::update();
	}
	else
	{
		CSnoopOptions::m_nEnabled = 0; //
	}
}

void CLittleSnoopApp::OnUpdateEnableUI(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(CSnoopOptions::m_nEnabled);
}

void CLittleSnoopApp::OnUpdateSettingsUI(CCmdUI *pCmdUI)
{
}

void CLittleSnoopApp::OnUpdateExitUI(CCmdUI *pCmdUI)
{
}

//EOF
