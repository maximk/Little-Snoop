#pragma once



// CSnoopOptions command target

//#define DEFAULT_CAPTURE_HOST	_T("snoopon.me")
//#define DEFAULT_CAPTURE_PORT	80

#define DEFAULT_CAPTURE_HOST	_T("ec2-67-202-57-11.compute-1.amazonaws.com")
#define DEFAULT_CAPTURE_PORT	8000

#define DEFAULT_CAPTURE_PATH	_T("/_capture")
#define DEFAULT_REGISTER_PATH	_T("/_register")
#define DEFAULT_TEST_PATH		_T("/_test")

#define DEFAULT_USER		_T("")
#define DEFAULT_PASSWORD	_T("")

#define DEFAULT_SCHEDULE	10

#define DEFAULT_ENABLED		1

class CSnoopOptions : public CObject
{
public:
	static void load();
	static void update();

public:
	// Host:port to post captured screen to (snoopon.me:80)
	static CString m_sCaptureHost;
	static int m_nCapturePort;
	static CString m_sCapturePath;
	static CString m_sRegisterPath;
	static CString m_sTestPath;

	// User name and password as registered with snoopon.me service
	static CString m_sUser;
	static CString m_sPassword;

	// Schedule type:
	//	5 - every 5 min
	//	10 - every 10 min
	//	30 - every half an hour
	static int m_nSchedule;

	static int m_nEnabled;

	// Captured image dimensions determined for each screen
	// separately:

	// The fixed width of shrinked images is 320px
	// The width of a thumbnail image is 80px
};

//EOF
