// SnoopOptions.cpp : implementation file
//

#include "stdafx.h"
#include "LittleSnoop.h"
#include "SnoopOptions.h"


// CSnoopOptions

CString CSnoopOptions::m_sCaptureHost = _T("");
int CSnoopOptions::m_nCapturePort = 0;
CString CSnoopOptions::m_sCapturePath = _T("");
CString CSnoopOptions::m_sRegisterPath = _T("");
CString CSnoopOptions::m_sTestPath = _T("");
CString CSnoopOptions::m_sUser = _T("");
CString CSnoopOptions::m_sPassword = _T("");
int CSnoopOptions::m_nSchedule = 0;
int CSnoopOptions::m_nEnabled = 0;

void CSnoopOptions::load()
{
	CWinApp *pApp = AfxGetApp();
	LPCTSTR lpszSection = _T("General");

	m_sCaptureHost = pApp->GetProfileString(lpszSection, _T("CaptureHost"), DEFAULT_CAPTURE_HOST);
	m_nCapturePort = pApp->GetProfileInt(lpszSection, _T("CapturePort"), DEFAULT_CAPTURE_PORT);
	m_sCapturePath = pApp->GetProfileString(lpszSection, _T("CapturePath"), DEFAULT_CAPTURE_PATH);
	m_sRegisterPath = pApp->GetProfileString(lpszSection, _T("RegisterPath"), DEFAULT_REGISTER_PATH);
	m_sTestPath = pApp->GetProfileString(lpszSection, _T("TestPath"), DEFAULT_TEST_PATH);

	m_sUser = pApp->GetProfileString(lpszSection, _T("UserName"), DEFAULT_USER);
	m_sPassword = pApp->GetProfileString(lpszSection, _T("Password"), DEFAULT_PASSWORD);

	m_nSchedule = pApp->GetProfileInt(lpszSection, _T("Schedule"), DEFAULT_SCHEDULE);

	m_nEnabled = pApp->GetProfileInt(lpszSection, _T("Enabled"), DEFAULT_ENABLED);
}

void CSnoopOptions::update()
{
	CWinApp *pApp = AfxGetApp();
	LPCTSTR lpszSection = _T("General");

	ASSERT(pApp->WriteProfileString(lpszSection, _T("CaptureHost"), m_sCaptureHost));
	ASSERT(pApp->WriteProfileInt(lpszSection, _T("CapturePort"), m_nCapturePort));
	ASSERT(pApp->WriteProfileString(lpszSection, _T("CapturePath"), m_sCapturePath));
	ASSERT(pApp->WriteProfileString(lpszSection, _T("RegisterPath"), m_sRegisterPath));
	ASSERT(pApp->WriteProfileString(lpszSection, _T("TestPath"), m_sTestPath));

	ASSERT(pApp->WriteProfileString(lpszSection, _T("UserName"), m_sUser));
	ASSERT(pApp->WriteProfileString(lpszSection, _T("Password"), m_sPassword));

	ASSERT(pApp->WriteProfileInt(lpszSection, _T("Schedule"), m_nSchedule));

	ASSERT(pApp->WriteProfileInt(lpszSection, _T("Enabled"), m_nEnabled));
}

// CSnoopOptions member functions
