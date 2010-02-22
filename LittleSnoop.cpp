// LittleSnoop.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

#include "Options.h"
#include "SnapScreen.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND hwndMain;									// the main (dummy) window table

HMENU g_hIconPopup;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

BOOL SetupTrayIcon();
BOOL RemoveTrayIcon();
BOOL ShowNotBoundBalloon();

HINSTANCE GotoJustInstalled();
HINSTANCE GotoSnoopOnMe();
HINSTANCE GotoSnoopOnMeSettings();

int SplitResponse(char *buf, int nread, int *nleft);

BOOL UpdateOptionsPostScreens();
BOOL UpdateOptionsFromProfile();
BOOL PostScreensAsJson(SOCKET sock, LPCTSTR json);



#define WM_USER_NOTIFYICON		(WM_USER + 100)



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LITTLESNOOP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_LITTLESNOOP);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_LITTLESNOOP);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;		//(LPCTSTR)IDC_LITTLESNOOP;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HMENU dummy;
	
	hInst = hInstance; // Store instance handle in our global variable

	hwndMain = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!hwndMain)
	{
		return FALSE;
	}

   	dummy = LoadMenu(hInst, MAKEINTRESOURCE(IDC_LITTLESNOOP));
	_ASSERT(dummy != NULL);
	g_hIconPopup = GetSubMenu(dummy, 0);
	_ASSERT(g_hIconPopup != NULL);

   ShowWindow(hwndMain, SW_HIDE);
   UpdateWindow(hwndMain);

   SetRegistryKey(_T("Snoop on.me"));
   LoadOptions();

   return SetupTrayIcon();
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_CREATE:
		SetTimer(hWnd, 1, 5*60*1000, NULL);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_MYSNOOPONME:
			GotoSnoopOnMe();
			break;
		case IDM_SETTINGS:
			GotoSnoopOnMeSettings();
			break;
		case IDM_POST:
			UpdateOptionsPostScreens();
			break;
		case IDM_INSTALLED:
			GotoJustInstalled();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		RemoveTrayIcon();

		PostQuitMessage(0);
		break;
	case WM_USER_NOTIFYICON:
	{
		POINT pt;

		if (lParam == WM_RBUTTONDOWN)
		{
			_ASSERT(GetCursorPos(&pt));
			_ASSERT(SetForegroundWindow(hwndMain));

			_ASSERT(TrackPopupMenuEx(g_hIconPopup, TPM_RIGHTALIGN, pt.x, pt.y, hwndMain, NULL));
		}
		else if (lParam == WM_LBUTTONDBLCLK)
		{
			SendMessage(hwndMain, WM_COMMAND, IDM_MYSNOOPONME, 0);
		}
		else if (lParam == NIN_BALLOONUSERCLICK)
		{
			SendMessage(hwndMain, WM_COMMAND, IDM_INSTALLED, 0);
		}

		break;
	}
	case WM_TIMER:
		SendMessage(hWnd, WM_COMMAND, IDM_POST, 0);
		SetTimer(hWnd, 1, g_nSchedule*60*1000, NULL);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

BOOL SetupTrayIcon()
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwndMain;
	nid.uID = 0;
	nid.uFlags = NIF_MESSAGE | NIF_TIP | NIF_ICON;
	nid.uCallbackMessage = WM_USER_NOTIFYICON;
	nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_LITTLESNOOP));
	LoadString(hInst, IDS_ICONTIP, nid.szTip, sizeof(nid.szTip)/sizeof(TCHAR));
	return Shell_NotifyIcon(NIM_ADD, &nid);
}

BOOL RemoveTrayIcon()
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwndMain;
	nid.uID = 0;
	return Shell_NotifyIcon(NIM_DELETE, &nid);
}

BOOL ShowNotBoundBalloon()
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwndMain;
	nid.uID = 0;
	nid.uFlags = NIF_INFO;
	nid.uTimeout = 1;
	nid.dwInfoFlags = NIIF_NONE;
 
	LoadString(hInst, IDS_NOT_BOUND_TEXT, nid.szInfo, sizeof(nid.szInfo)/sizeof(TCHAR));
	LoadString(hInst, IDS_NOT_BOUND_TITLE, nid.szInfoTitle, sizeof(nid.szInfoTitle)/sizeof(TCHAR));

	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

HINSTANCE GotoJustInstalled()
{
	TCHAR szUrl[256];
	if (g_nCapturePort == 80)
		_sntprintf(szUrl, sizeof(szUrl)/sizeof(TCHAR), _T("http://%s%s?ls_id=%s"),
			g_szCaptureHost, g_szInstalledPath, g_szLittleSnoopId);
	else
		_sntprintf(szUrl, sizeof(szUrl)/sizeof(TCHAR), _T("http://%s:%d%s?ls_id=%s"),
			g_szCaptureHost, g_nCapturePort, g_szInstalledPath, g_szLittleSnoopId);

	return ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
}

HINSTANCE GotoSnoopOnMe()
{
	TCHAR szUrl[256];
	if (g_nCapturePort == 80)
		_sntprintf(szUrl, sizeof(szUrl)/sizeof(TCHAR), _T("http://%s%s?ls_id=%s"),
			g_szCaptureHost, g_szPortaPath, g_szLittleSnoopId);
	else
		_sntprintf(szUrl, sizeof(szUrl)/sizeof(TCHAR), _T("http://%s:%d%s?ls_id=%s"),
			g_szCaptureHost, g_nCapturePort, g_szPortaPath, g_szLittleSnoopId);

	return ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
}

HINSTANCE GotoSnoopOnMeSettings()
{
	TCHAR szUrl[256];
	if (g_nCapturePort == 80)
		_sntprintf(szUrl, sizeof(szUrl)/sizeof(TCHAR), _T("http://%s%s?ls_id=%s&room=settings"),
			g_szCaptureHost, g_szPortaPath, g_szLittleSnoopId);
	else
		_sntprintf(szUrl, sizeof(szUrl)/sizeof(TCHAR), _T("http://%s:%d%s?ls_id=%s&room=settings"),
			g_szCaptureHost, g_nCapturePort, g_szPortaPath, g_szLittleSnoopId);

	return ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
}

BOOL UpdateOptionsPostScreens()
{
	BOOL result = TRUE;
	char buf[64];
	WSADATA wsa;
	struct addrinfo *ai;
	SOCKET sock;

	WSAStartup(MAKEWORD(2, 2), &wsa);

	_itoa(g_nCapturePort, buf, 10);
	if (getaddrinfo(g_szCaptureHost, buf, NULL, &ai) != 0) //XXX: Unicode dependency
	{
		WSACleanup();
		return FALSE;
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	result = (connect(sock, ai->ai_addr, (int)ai->ai_addrlen) == 0);
	if (result)
		result = UpdateOptionsFromProfile(sock);
	if (result && g_nEnabled)
	{
		LPCTSTR json = SnapScreensToJson();
		if (json != NULL)
		{
			result = PostScreensAsJson(sock, json);
			free((void *)json);
		}
	}

	freeaddrinfo(ai);
	closesocket(sock);
	WSACleanup();

	return result;
}

BOOL UpdateOptionsFromProfile(SOCKET sock)
{
	char buf[1024];
	int nleft, nsent, nrecv, bodyoff;

	_snprintf(buf, sizeof(buf), "GET %s/%s HTTP/1.1\r\n"
		"Accept: application/json\r\n\r\n",
		g_szSettingsPath, g_szLittleSnoopId);

	nleft = (int)strlen(buf);
	nsent = 0;
	while (nleft > 0)
	{
		int n = send(sock, buf+nsent, nleft, 0);
		if (n < 0)
			return FALSE;
		nsent += n;
		nleft -= n;
	}
	
	nleft = sizeof(buf);
	nrecv = 0;
	while ((bodyoff = SplitResponse(buf, nrecv, &nleft)) == -1)
	{
		int n = recv(sock, buf+nrecv, nleft, 0);
		if (n < 0)
			return FALSE;
		nrecv += n;
		nleft -= n;

		if (nleft == 0)
			return FALSE;
	}

	if (strstr(buf, " 404 ") != NULL)
	{
		// Little Snoop not bound yet
		ShowNotBoundBalloon();	// XXX: hookish
		return FALSE;	// FALSE means 'stop', not 'error'
	}

	while (nleft > 0)
	{
		int n = recv(sock, buf+nrecv, nleft, 0);
		if (n < 0)
			return FALSE;
		nrecv += n;
		nleft -= n;
	}

	buf[nrecv] = 0;
	ParseOptionsFromJson(buf+bodyoff);

	return TRUE;
}

BOOL PostScreensAsJson(SOCKET sock, LPCTSTR json)
{
	char buf[1024];
	int nleft, nsent, nrecv, bodyoff;
	int body_len = lstrlen(json);

	_snprintf(buf, sizeof(buf), "POST %s HTTP/1.1\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: %d\r\n\r\n",
		g_szCapturePath, body_len);

	nleft = (int)strlen(buf);
	nsent = 0;
	while (nleft > 0)
	{
		int n = send(sock, buf+nsent, nleft, 0);
		nsent += n;
		nleft -= n;
	}

	nleft = body_len;
	nsent = 0;
	while (nleft > 0)
	{
		int n = send(sock, json+nsent, nleft, 0);	//XXX: !Unicode
		if (n < 0)
			return FALSE;
		nsent += n;
		nleft -= n;
	}
	
	nleft = sizeof(buf);
	nrecv = 0;
	while ((bodyoff = SplitResponse(buf, nrecv, &nleft)) == -1)
	{
		int n = recv(sock, buf+nrecv, nleft, 0);
		if (n < 0)
			return FALSE;
		nrecv += n;
		nleft -= n;

		if (nleft == 0)
			return FALSE;
	}

	while (nleft > 0)
	{
		int n = recv(sock, buf+nrecv, nleft, 0);
		if (n < 0)
			return FALSE;
		nrecv += n;
		nleft -= n;
	}

	// The response is discarded, sorry
	return TRUE;
}

int SplitResponse(char *buf, int nread, int *nleft)
{
	int i;

	// detect the boundary between headers and body
	// extract Content-Length
	// calculate how many bytes of body are left to read

	for (i = 0; i <= nread-4; i++)
	{
		if (strncmp(buf+i, "\r\n\r\n", 4) == 0)
		{
			const char *cl;
			int bodyoff;
			buf[i+2] = buf[i+3] = 0;
			bodyoff = i+4;

			cl = strstr(buf, "Content-Length:");
			if (cl)
			{
				int len = atoi(cl+16);	//XXX
				*nleft = len-(nread-bodyoff);
				return bodyoff;
			}
			else
				*nleft = 0;
			return bodyoff;
		}
	}

	return -1;
}

//EOF
