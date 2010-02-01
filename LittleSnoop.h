// LittleSnoop.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "SnoopOptions.h"

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
	//afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnEnable();
	afx_msg void OnSettings();
	afx_msg void OnExit();
	afx_msg void OnCapturePost();
	afx_msg void OnRegisterNew();

	afx_msg void OnUpdateEnableUI(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSettingsUI(CCmdUI *pCmdUI);
	afx_msg void OnUpdateExitUI(CCmdUI *pCmdUI);

	DECLARE_MESSAGE_MAP()

private:
	ULONG_PTR m_lGdiplusToken;

	CMutex *m_pSingleInstanceMutex;

private:
	BOOL setupTrayIcon();
	BOOL removeTrayIcon();
};

extern CLittleSnoopApp theApp;