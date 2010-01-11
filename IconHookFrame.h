#pragma once


// CIconHookFrame frame

#define WM_USER_NOTIFYICON (WM_USER + 100)

class CIconHookFrame : public CMiniFrameWnd
{
public:
    CIconHookFrame();           // protected constructor used by dynamic creation
	virtual ~CIconHookFrame();

protected:
    afx_msg LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};


