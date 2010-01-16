// LittleSnoop.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "LittleSnoop.h"

#include "IconHookFrame.h"

// remove -- needed for temp code, call to rand()
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Gdiplus;

CString g_sKey = _T("LitteSnoop");
CMutex *g_pSingleInstanceMutex = NULL;

// CLittleSnoopApp

BEGIN_MESSAGE_MAP(CLittleSnoopApp, CWinApp)
	ON_COMMAND(IDM_STARTTIMER, OnStartTimer)
	ON_COMMAND(IDM_STOPTIMER, OnStopTimer)
	ON_COMMAND(IDM_POSTCAPTURE, OnPostCapture)
	ON_COMMAND(IDM_SETTINGS, OnSettings)
	ON_COMMAND(IDM_EXIT, OnExit)
END_MESSAGE_MAP()


// CLittleSnoopApp construction

CLittleSnoopApp::CLittleSnoopApp()
: m_nTimerId(0)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CLittleSnoopApp object

CLittleSnoopApp theApp;


// CLittleSnoopApp initialization

BOOL CLittleSnoopApp::InitInstance()
{
	CWinApp::InitInstance();

	GdiplusStartupInput input;
	GdiplusStartup(&m_lGdiplusToken, &input, NULL);

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

    // verify that single instance is running
    CString name = GetProfileString(g_sKey,
        _T("MutexName"), _T("SnoopOnMe5"));
    g_pSingleInstanceMutex = new CMutex(FALSE, name);
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        AfxMessageBox(IDS_EALREADYRUNNING);
        return FALSE;
    }

    // setup hidden window to hook in tray icon
    CIconHookFrame *pHook = new CIconHookFrame;
	if (!pHook)
		return FALSE;
	m_pMainWnd = pHook;
	// create the hook window
    if(!pHook->Create(NULL, NULL, 0, CRect(10, 10, 100, 100)))
        return FALSE;

    //pHook->ShowWindow(SW_SHOWNORMAL);
    //pHook->UpdateWindow();

    // setup tray icon
    HICON hIcon = LoadIcon(IDI_TRAYICON);
    ASSERT(setupTaskbarIcon(m_pMainWnd->GetSafeHwnd(), hIcon));

	// create the assistant
	m_pAssistant = new CAssistant();
	m_pAssistant->loadOptions();

	// start the application's message pump
    return TRUE;
}

int CLittleSnoopApp::ExitInstance()
{
	m_pAssistant->updateOptions();
	delete m_pAssistant;

    delete g_pSingleInstanceMutex;

	GdiplusShutdown(m_lGdiplusToken);

    return CWinApp::ExitInstance();
}

BOOL CLittleSnoopApp::setupTaskbarIcon(HWND hWnd, HICON hIcon)
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = hWnd;
	nid.uID = 0;
	nid.uFlags = NIF_MESSAGE | NIF_TIP | NIF_ICON;
	nid.uCallbackMessage = WM_USER_NOTIFYICON;
	nid.hIcon = hIcon;
    
    CString tip;
    if (tip.LoadString(IDS_ICONTIP))
		StringCchCopyN(nid.szTip, sizeof(nid.szTip),
            tip, tip.GetLength());
    else 
        nid.szTip[0] = (TCHAR)'\0';

	return Shell_NotifyIcon(NIM_ADD, &nid);
}

BOOL CLittleSnoopApp::removeTaskbarIcon(HWND hWnd)
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = hWnd;
	nid.uID = 0;

	return Shell_NotifyIcon(NIM_DELETE, &nid);
}

void CLittleSnoopApp::OnStartTimer()
{
	m_nTimerId = m_pMainWnd->SetTimer(1, 2000, NULL);
}

void CLittleSnoopApp::OnStopTimer()
{
	m_pMainWnd->KillTimer(1);
}

void CLittleSnoopApp::OnPostCapture()
{
	CSize sizes[MAX_SCREENS];
	Bitmap *snapshots[MAX_SCREENS];
	Bitmap *thumbnails[MAX_SCREENS];

	int n = m_pAssistant->captureScreen(snapshots, thumbnails, sizes, MAX_SCREENS);

	if (n > 0)
	{
		CLSID clsid;
		CAssistant::GetEncoderClsid(L"image/png", &clsid);
		snapshots[0]->Save(L"\\Tmp\\TestCap2.png", &clsid, NULL);
	}

	//ASSERT(m_pAssistant->captureScreen2(m_pMainWnd->GetDesktopWindow()));
	//ASSERT(m_pAssistant->postScreenshot());
}

void CLittleSnoopApp::OnSettings()
{
	m_pAssistant->showSettingsDialog();
}

void CLittleSnoopApp::OnExit()
{
    ASSERT(removeTaskbarIcon(m_pMainWnd->GetSafeHwnd()));
    m_pMainWnd->DestroyWindow();
}
