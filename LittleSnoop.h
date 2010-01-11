// LittleSnoop.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CLittleSnoopApp:
// See LittleSnoop.cpp for the implementation of this class
//

class CLittleSnoopApp : public CWinApp
{
public:
	CLittleSnoopApp();

// Overrides
public:
	virtual BOOL InitInstance();
    virtual int ExitInstance();

// Implementation
    BOOL setupTaskbarIcon(HWND hWnd, HICON hIcon);
    BOOL removeTaskbarIcon(HWND hWnd);

// Commands
	afx_msg void OnExit();
	DECLARE_MESSAGE_MAP()
};

extern CLittleSnoopApp theApp;