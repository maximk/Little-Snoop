#pragma once



// CAssistant command target

class CAssistant : public CObject
{
public:
	CAssistant();
	virtual ~CAssistant();

	BOOL loadOptions();
	BOOL updateOptions();

	BOOL captureScreen(CWnd *wndDesktop);
	BOOL postScreenshot();

	void showSettingsDialog();

private:
	// configuration options

	// Host:port to post captured screen to (snoopon.me:80)
	CString m_sSnoopOnMeHost;
	int m_nSnoopOnMePort;

	// User name and password as registered with snoopon.me service
	CString m_sUser;
	CString m_sPassword;

	// Schedule type:
	//	1 - every 5 min
	//	2 - every 15 min
	//	3 - every hour
	int m_nSchedule;

	// Dimensions of the captured screen, small enough to render
	// texts unreadable (320x240)
	// TODO: do some smart shrinking probably look at the default
	// screen font size
	int m_nShrinkWidth;
	int m_nShrinkHeight;

	// Dimensions of a thumbnail image (80x60)
	int m_nThumbWidth;
	int m_nThumbHeight;
};

//EOF
