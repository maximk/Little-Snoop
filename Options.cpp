//
//

#include "stdafx.h"

#include "Options.h"

TCHAR g_szRegistryKey[MAX_OPT_STRING];
LPCTSTR g_szProfileName = _T("LittleSnoop");

TCHAR g_szLittleSnoopId[MAX_OPT_STRING];
TCHAR g_szCaptureHost[MAX_OPT_STRING];
int g_nCapturePort;
TCHAR g_szCapturePath[MAX_OPT_STRING];
TCHAR g_szSettingsPath[MAX_OPT_STRING];
TCHAR g_szInstalledPath[MAX_OPT_STRING];
TCHAR g_szPortaPath[MAX_OPT_STRING];

int g_nEnabled;
int g_nSchedule;

BOOL GetOptionString(LPCTSTR szSection, LPCTSTR szEntry, LPCTSTR szDefault, LPTSTR szBuffer, int nSize);
UINT GetOptionInt(LPCTSTR szSection, LPCTSTR szEntry,	int nDefault);
BOOL WriteOptionString(LPCTSTR szSection, LPCTSTR szEntry, LPCTSTR szValue);
BOOL WriteOptionInt(LPCTSTR szSection, LPCTSTR szEntry,	int nValue);
HKEY GetSectionKey(LPCTSTR szSection);
HKEY GetAppRegistryKey();

void GenerateUuid(TCHAR *szBuffer, int nSize);

void SetRegistryKey(LPCTSTR szRegistryKey)
{
	_tcsncpy(g_szRegistryKey, szRegistryKey, MAX_OPT_STRING);
}

void LoadOptions()
{
	TCHAR *szSection = _T("General");

	GetOptionString(szSection, _T("LsId"), _T(""), g_szLittleSnoopId, MAX_OPT_STRING);
	if (g_szLittleSnoopId[0] == 0)
		GenerateUuid(g_szLittleSnoopId, MAX_OPT_STRING);
	GetOptionString(szSection, _T("CaptureHost"), _T("snoopon.me"), g_szCaptureHost, MAX_OPT_STRING);

	g_nCapturePort = GetOptionInt(szSection, _T("CapturePort"), 80);

	GetOptionString(szSection, _T("CapturePath"), _T("/captures"), g_szCapturePath, MAX_OPT_STRING);
	GetOptionString(szSection, _T("SettingsPath"), _T("/settings"), g_szSettingsPath, MAX_OPT_STRING);
	GetOptionString(szSection, _T("InstaleldPath"), _T("/installed"), g_szInstalledPath, MAX_OPT_STRING);
	GetOptionString(szSection, _T("PortaPath"), _T("/porta"), g_szPortaPath, MAX_OPT_STRING);

	g_nEnabled = 1;
	g_nSchedule = 5;
}

void UpdateOptions()
{
	TCHAR *szSection = _T("General");
	WriteOptionString(szSection, _T("LsId"), g_szLittleSnoopId);
	WriteOptionString(szSection, _T("CaptureHost"), g_szCaptureHost);

	WriteOptionInt(szSection, _T("CapturePort"), g_nCapturePort);

	WriteOptionString(szSection, _T("CapturePath"), g_szCapturePath);
	WriteOptionString(szSection, _T("SettingsPath"), g_szSettingsPath);
	WriteOptionString(szSection, _T("InstalledPath"), g_szInstalledPath);
	WriteOptionString(szSection, _T("PortaPath"), g_szPortaPath);
}

BOOL GetOptionString(LPCTSTR szSection, LPCTSTR szEntry, LPCTSTR szDefault, LPTSTR szBuffer, int nSize)
{
	_ASSERT(szSection != NULL);
	_ASSERT(szEntry != NULL);
	if (g_szRegistryKey != NULL)
	{
		HKEY hSecKey = GetSectionKey(szSection);
		if (hSecKey != NULL)
		{
			DWORD dwType, dwCount;
			LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)szEntry, NULL, &dwType,
				NULL, &dwCount);
			if (lResult == ERROR_SUCCESS)
			{
				_ASSERT(dwType == REG_SZ);
				lResult = RegQueryValueEx(hSecKey, (LPTSTR)szEntry, NULL, &dwType,
					(LPBYTE)szBuffer, &nSize);
			}
			RegCloseKey(hSecKey);
			if (lResult == ERROR_SUCCESS)
			{
				_ASSERT(dwType == REG_SZ);
				return TRUE;
			}
		}
	}

	_tcsncpy(szBuffer, szDefault, nSize);
	return TRUE;
}

UINT GetOptionInt(LPCTSTR szSection, LPCTSTR szEntry,	int nDefault)
{
	_ASSERT(szSection != NULL);
	_ASSERT(szEntry != NULL);
	if (g_szRegistryKey != NULL) // use registry
	{
		HKEY hSecKey = GetSectionKey(szSection);
		DWORD dwValue;
		DWORD dwType;
		DWORD dwCount = sizeof(DWORD);
		LONG lResult;
		if (hSecKey == NULL)
			return nDefault;
		
		lResult = RegQueryValueEx(hSecKey, (LPTSTR)szEntry, NULL, &dwType,
			(LPBYTE)&dwValue, &dwCount);
		RegCloseKey(hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			_ASSERT(dwType == REG_DWORD);
			_ASSERT(dwCount == sizeof(dwValue));
			return (UINT)dwValue;
		}
		return nDefault;
	}
	return 0;
}

BOOL WriteOptionString(LPCTSTR szSection, LPCTSTR szEntry, LPCTSTR szValue)
{
	_ASSERT(szSection != NULL);
	if (g_szRegistryKey != NULL)
	{
		LONG lResult;
		if (szEntry == NULL) //delete whole section
		{
			HKEY hAppKey = GetAppRegistryKey();
			if (hAppKey == NULL)
				return FALSE;
			lResult = RegDeleteKey(hAppKey, szSection);
			RegCloseKey(hAppKey);
		}
		else if (szValue == NULL)
		{
			HKEY hSecKey = GetSectionKey(szSection);
			if (hSecKey == NULL)
				return FALSE;
			// necessary to cast away const below
			lResult = RegDeleteValue(hSecKey, (LPTSTR)szEntry);
			RegCloseKey(hSecKey);
		}
		else
		{
			HKEY hSecKey = GetSectionKey(szSection);
			if (hSecKey == NULL)
				return FALSE;
			lResult = RegSetValueEx(hSecKey, szEntry, 0, REG_SZ,
				(LPBYTE)szValue, (lstrlen(szValue)+1)*sizeof(TCHAR));
			RegCloseKey(hSecKey);
		}
		return lResult == ERROR_SUCCESS;
	}
	return FALSE;
}

BOOL WriteOptionInt(LPCTSTR szSection, LPCTSTR szEntry,	int nValue)
{
	_ASSERT(szSection != NULL);
	_ASSERT(szEntry != NULL);
	if (g_szRegistryKey != NULL)
	{
		HKEY hSecKey = GetSectionKey(szSection);
		LONG lResult;
		if (hSecKey == NULL)
			return FALSE;
		lResult = RegSetValueEx(hSecKey, szEntry, 0, REG_DWORD,
			(LPBYTE)&nValue, sizeof(nValue));
		RegCloseKey(hSecKey);
		return lResult == ERROR_SUCCESS;
	}
	return FALSE;
}

// returns key for:
//      HKEY_CURRENT_USER\"Software"\RegistryKey\AppName\szSection
// creating it if it doesn't exist.
// responsibility of the caller to call RegCloseKey() on the returned HKEY
HKEY GetSectionKey(LPCTSTR szSection)
{
	DWORD dw;
	HKEY hSectionKey = NULL;
	HKEY hAppKey = GetAppRegistryKey();
	if (hAppKey == NULL)
		return NULL;

	RegCreateKeyEx(hAppKey, szSection, 0, REG_NONE,
		REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
		&hSectionKey, &dw);
	RegCloseKey(hAppKey);
	return hSectionKey;
}

// returns key for HKEY_CURRENT_USER\"Software"\RegistryKey\ProfileName
// creating it if it doesn't exist
// responsibility of the caller to call RegCloseKey() on the returned HKEY
HKEY GetAppRegistryKey()
{
	HKEY hAppKey = NULL;
	HKEY hSoftKey = NULL;
	HKEY hCompanyKey = NULL;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("software"), 0, KEY_WRITE|KEY_READ,
		&hSoftKey) == ERROR_SUCCESS)
	{
		DWORD dw;
		if (RegCreateKeyEx(hSoftKey, g_szRegistryKey, 0, REG_NONE,
			REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
			&hCompanyKey, &dw) == ERROR_SUCCESS)
		{
			RegCreateKeyEx(hCompanyKey, g_szProfileName, 0, REG_NONE,
				REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
				&hAppKey, &dw);
		}
	}
	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);
	if (hCompanyKey != NULL)
		RegCloseKey(hCompanyKey);

	return hAppKey;
}

void GenerateUuid(TCHAR *szBuffer, int nSize)
{
	UUID uuid;
	if (UuidCreate(&uuid) == RPC_S_OK)
	{
		unsigned char *str;
		if (UuidToString(&uuid, &str) == RPC_S_OK)
		{
			strncpy(szBuffer, str, nSize);
			RpcStringFree(&str);
		}
	}
}

//void CSnoopOptions::parseJsonSettings(CString settings)
//{
//	// {"enabled":1,"schedule":5}
//
//	int i = settings.Find("enabled");
//	if (i >= 0)
//		m_nEnabled = atoi(settings.Mid(i + 7 + 1 + 1));	// enabled + " + :
//	i = settings.Find("schedule");
//	if (i >= 0)
//		m_nSchedule = atoi(settings.Mid(i + 8 + 1 + 1));	// schedule + " + :
//}

void ParseOptionsFromJson(const char *json)
{
	const char *p = strstr(json, "enabled");
	if (p != NULL)
		g_nEnabled = atoi(p + 7 + 1 + 1);	// enabled + " + :
	p = strstr(json, "schedule");
		g_nSchedule = atoi(p + 8 + 1 + 1);	// schedule + " + :
}

//EOF
