// SnoopOptions.cpp : implementation file
//

#include "stdafx.h"
#include "LittleSnoop.h"
#include "SnoopOptions.h"


// CSnoopOptions

CString CSnoopOptions::m_sLittleSnoopId = _T("");
CString CSnoopOptions::m_sCaptureHost = _T("");
int CSnoopOptions::m_nCapturePort = 0;
CString CSnoopOptions::m_sCapturePath = _T("");
CString CSnoopOptions::m_sSettingsPath = _T("");
CString CSnoopOptions::m_sInstalledPath = _T("");
CString CSnoopOptions::m_sPortaPath = _T("");

BOOL CSnoopOptions::m_nEnabled = DEFAULT_ENABLED;
int CSnoopOptions::m_nSchedule = DEFAULT_SCHEDULE;

void CSnoopOptions::load()
{
	CWinApp *pApp = AfxGetApp();
	LPCTSTR lpszSection = _T("General");

	m_sLittleSnoopId = pApp->GetProfileString(lpszSection, _T("LsId"));

	if (m_sLittleSnoopId.GetLength() == 0)
		m_sLittleSnoopId = generate_lsid();

	m_sCaptureHost = pApp->GetProfileString(lpszSection, _T("CaptureHost"), DEFAULT_CAPTURE_HOST);
	m_nCapturePort = pApp->GetProfileInt(lpszSection, _T("CapturePort"), DEFAULT_CAPTURE_PORT);
	m_sCapturePath = pApp->GetProfileString(lpszSection, _T("CapturePath"), DEFAULT_CAPTURE_PATH);
	m_sSettingsPath = pApp->GetProfileString(lpszSection, _T("SettingsPath"), DEFAULT_SETTINGS_PATH);
	m_sInstalledPath = pApp->GetProfileString(lpszSection, _T("InstalledPath"), DEFAULT_INSTALLED_PATH);
	m_sPortaPath = pApp->GetProfileString(lpszSection, _T("PortaPath"), DEFAULT_PORTA_PATH);
}

void CSnoopOptions::update()
{
	CWinApp *pApp = AfxGetApp();
	LPCTSTR lpszSection = _T("General");

	ASSERT(pApp->WriteProfileString(lpszSection, _T("LsId"), m_sLittleSnoopId));
	ASSERT(pApp->WriteProfileString(lpszSection, _T("CaptureHost"), m_sCaptureHost));
	ASSERT(pApp->WriteProfileInt(lpszSection, _T("CapturePort"), m_nCapturePort));
	ASSERT(pApp->WriteProfileString(lpszSection, _T("CapturePath"), m_sCapturePath));
	ASSERT(pApp->WriteProfileString(lpszSection, _T("SettingsPath"), m_sSettingsPath));
	ASSERT(pApp->WriteProfileString(lpszSection, _T("InstalledPath"), m_sInstalledPath));
	ASSERT(pApp->WriteProfileString(lpszSection, _T("PortaPath"), m_sPortaPath));
}

void CSnoopOptions::parseJsonSettings(CString settings)
{
	// {"enabled":1,"schedule":5}

	int i = settings.Find("enabled");
	if (i >= 0)
		m_nEnabled = atoi(settings.Mid(i + 7 + 1 + 1));	// enabled + " + :
	i = settings.Find("schedule");
	if (i >= 0)
		m_nSchedule = atoi(settings.Mid(i + 8 + 1 + 1));	// schedule + " + :
}

// CSnoopOptions member functions

CString CSnoopOptions::generate_lsid()
{
	CString id = "";
	UUID uuid;

	if (::UuidCreate(&uuid) == RPC_S_OK)
	{
		unsigned char *str;
		if (::UuidToString(&uuid, &str) == RPC_S_OK)
		{
			id = CString(str);
			::RpcStringFree(&str);
		}
	}

	return id;
}

//EOF
