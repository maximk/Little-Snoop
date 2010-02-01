// DummyFrame.cpp : implementation file
//

#include "stdafx.h"
#include "LittleSnoop.h"
#include "DummyFrame.h"


// CDummyFrame

IMPLEMENT_DYNCREATE(CDummyFrame, CMiniFrameWnd)

CDummyFrame::CDummyFrame()
{
}

CDummyFrame::~CDummyFrame()
{
}


BEGIN_MESSAGE_MAP(CDummyFrame, CMiniFrameWnd)
	ON_MESSAGE(WM_USER_NOTIFYICON, OnNotifyIcon)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDummyFrame message handlers

void CDummyFrame::OnTimer(UINT_PTR nTimerId)
{
	if (CSnoopOptions::m_nEnabled)
		SendMessage(WM_COMMAND, IDM_POSTCAPTURE);
	SetTimer(1, CSnoopOptions::m_nSchedule*60*1000, NULL);
}

LRESULT CDummyFrame::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	POINT pt;

    if (lParam == WM_RBUTTONDOWN)
    {
        if (!GetCursorPos(&pt))
	        return -1;

        if (!SetForegroundWindow())
	        return -1;

        CMenu dummy;
        if (!dummy.LoadMenu(IDR_MENU1))
            return -1;

        CMenu *popup = dummy.GetSubMenu(0);
        popup->TrackPopupMenuEx(TPM_RIGHTALIGN,
	        pt.x, pt.y, AfxGetMainWnd(), NULL);
    }
	else if (lParam == NIN_BALLOONUSERCLICK)
	{
		SendMessage(WM_COMMAND, IDM_REGISTERNEW);
	}

    return 0;
}

//EOF
