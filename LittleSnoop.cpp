// LittleSnoop.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "LittleSnoop.h"
#include "SnoopOptions.h"
#include "DummyFrame.h"

#include "ScreenSnapper.h"

#include "gdiplus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLittleSnoopApp

BEGIN_MESSAGE_MAP(CLittleSnoopApp, CWinApp)
	ON_COMMAND(IDM_EXIT, OnExit)
	ON_UPDATE_COMMAND_UI(IDM_EXIT, OnUpdateExitUI)

	ON_COMMAND(IDM_CAPTUREPOST, OnCapturePost)
	ON_COMMAND(IDM_REGISTERNEW, OnRegisterNew)
	ON_COMMAND(IDM_MYSNOOPONME, OnMySnoopOnMe)
	ON_UPDATE_COMMAND_UI(IDM_MYSNOOPONME, OnUpdateMySnoopOnMeUI)
	ON_COMMAND(IDM_SETTINGS, OnSettings)
	ON_UPDATE_COMMAND_UI(IDM_SETTINGS, OnUpdateSettingsUI)
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

BOOL CLittleSnoopApp::showNotBoundBalloon()
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_pMainWnd->GetSafeHwnd();
	nid.uID = 0;
	nid.uFlags = NIF_INFO;
	nid.uTimeout = 1;
	nid.dwInfoFlags = NIIF_NONE;

	CString info;
	if (info.LoadString(IDS_NOT_BOUND_TEXT))
		StringCchCopyN(nid.szInfo, sizeof(nid.szInfo),
			info, info.GetLength());
	else 
		nid.szInfo[0] = (TCHAR)'\0';

	CString title;
	if (title.LoadString(IDS_NOT_BOUND_TITLE))
		StringCchCopyN(nid.szInfoTitle, sizeof(nid.szInfoTitle),
			title, title.GetLength());
	else 
		nid.szInfoTitle[0] = (TCHAR)'\0';

	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void CLittleSnoopApp::OnExit()
{
	// dump settings to registry
	CSnoopOptions::update();

	// Get rid of the cute puppy icon in the tray
	ASSERT(removeTrayIcon());	

	// destroying a dummy window magically closes the application
	m_pMainWnd->DestroyWindow();
}

void CLittleSnoopApp::OnUpdateExitUI(CCmdUI *pCmdUI)
{
}

void CLittleSnoopApp::OnCapturePost()
{
	LPCTSTR accepts[] = {"application/json", NULL};

	CString host = CSnoopOptions::m_sCaptureHost;
	int port = CSnoopOptions::m_nCapturePort;

	CInternetSession *session = new CInternetSession();
	CHttpConnection *connection = session->GetHttpConnection(host, (INTERNET_PORT)port);

	CHttpFile *f = connection->OpenRequest(CHttpConnection::HTTP_VERB_GET,
		CSnoopOptions::m_sSettingsPath + _T("/") + CSnoopOptions::m_sLittleSnoopId, 0, 1, accepts);
	f->SendRequest();

	DWORD status;
	f->QueryInfoStatusCode(status);

	if (status == 404)
	{
		f->Close();
		showNotBoundBalloon();
	}
	else
	{
		CString settings;
		f->ReadString(settings);
		f->Close();

		CSnoopOptions::parseJsonSettings(settings);

		if (CSnoopOptions::m_nEnabled)
		{
			CScreenSnapper snapper;
			CString doc = snapper.snap();

			if (doc == "")	// all monitors are black -- nothing to post
				return;

			CString body;
			body.Format("{\"ls_id\":\"%s\",\"screens\":%s}",
				CSnoopOptions::m_sLittleSnoopId, doc);

			CHttpFile *r = connection->OpenRequest(CHttpConnection::HTTP_VERB_POST,
				CSnoopOptions::m_sCapturePath);
			r->AddRequestHeaders(_T("Content-Type: application/json\r\n"));
			r->SendRequestEx(body.GetLength());
			r->WriteString(body);
			r->EndRequest();

			r->Close();
		}
	}

	connection->Close();
	session->Close();
}

void CLittleSnoopApp::OnRegisterNew()
{
	CString url;
	if (CSnoopOptions::m_nCapturePort == 80)
		url.Format("http://%s%s?ls_id=%s", CSnoopOptions::m_sCaptureHost,
			CSnoopOptions::m_sInstalledPath, CSnoopOptions::m_sLittleSnoopId);
	else
		url.Format("http://%s:%d%s?ls_id=%s", CSnoopOptions::m_sCaptureHost,
			CSnoopOptions::m_nCapturePort, CSnoopOptions::m_sInstalledPath,
			CSnoopOptions::m_sLittleSnoopId);

	ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

void CLittleSnoopApp::OnMySnoopOnMe()
{
	CString url;
	if (CSnoopOptions::m_nCapturePort == 80)
		url.Format("http://%s%s?ls_id=%s", CSnoopOptions::m_sCaptureHost,
			CSnoopOptions::m_sPortaPath, CSnoopOptions::m_sLittleSnoopId);
	else
		url.Format("http://%s:%d%s?ls_id=%s", CSnoopOptions::m_sCaptureHost,
			CSnoopOptions::m_nCapturePort, CSnoopOptions::m_sPortaPath,
			CSnoopOptions::m_sLittleSnoopId);

	ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

void CLittleSnoopApp::OnUpdateMySnoopOnMeUI(CCmdUI *pCmdUI)
{
}

void CLittleSnoopApp::OnSettings()
{
	CString url;
	if (CSnoopOptions::m_nCapturePort == 80)
		url.Format("http://%s%s?ls_id=%s&room=settings", CSnoopOptions::m_sCaptureHost,
			CSnoopOptions::m_sPortaPath, CSnoopOptions::m_sLittleSnoopId);
	else
		url.Format("http://%s:%d%s?ls_id=%s&room=settings", CSnoopOptions::m_sCaptureHost,
			CSnoopOptions::m_nCapturePort, CSnoopOptions::m_sPortaPath,
			CSnoopOptions::m_sLittleSnoopId);

	ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

void CLittleSnoopApp::OnUpdateSettingsUI(CCmdUI *pCmdUI)
{
}

//EOF
