#pragma once

#include <tchar.h>

#define MAX_OPT_STRING 256

extern LPCTSTR g_szProfileName;

extern TCHAR g_szLittleSnoopId[MAX_OPT_STRING];
extern TCHAR g_szCaptureHost[MAX_OPT_STRING];
extern int g_nCapturePort;
extern TCHAR g_szCapturePath[MAX_OPT_STRING];
extern TCHAR g_szSettingsPath[MAX_OPT_STRING];
extern TCHAR g_szInstalledPath[MAX_OPT_STRING];
extern TCHAR g_szPortaPath[MAX_OPT_STRING];

extern int g_nEnabled;
extern int g_nSchedule;

void SetRegistryKey(LPCTSTR szRegistryKey);
void LoadOptions();
void SaveOptions();
void ParseOptionsFromJson(const char *json);

//EOF
