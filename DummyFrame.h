#pragma once


// CDummyFrame frame

#define WM_USER_NOTIFYICON		(WM_USER + 100)

class CDummyFrame : public CMiniFrameWnd
{
	DECLARE_DYNCREATE(CDummyFrame)
protected:
	CDummyFrame();           // protected constructor used by dynamic creation
	virtual ~CDummyFrame();

protected:
	afx_msg void OnTimer(UINT_PTR nTimerId);
    afx_msg LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};


