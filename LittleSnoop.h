// LittleSnoop.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "Assistant.h"

// Maximum number of physical screens
#define MAX_SCREENS 8


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
	afx_msg void OnStartTimer();
	afx_msg void OnStopTimer();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPostCapture();
	afx_msg void OnSettings();
	afx_msg void OnExit();
	DECLARE_MESSAGE_MAP()

private:
	ULONG_PTR m_lGdiplusToken;
	UINT_PTR m_nTimerId;
	CAssistant *m_pAssistant;
};

extern CLittleSnoopApp theApp;