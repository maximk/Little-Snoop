#pragma once



// CAssistant command target

class CAssistant : public CObject
{
public:
	CAssistant();
	virtual ~CAssistant();

	BOOL captureScreen(CWnd *wndDesktop);
	BOOL postScreenshot();
};


