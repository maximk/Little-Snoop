// IconHookFrame.cpp : implementation file
//

#include "stdafx.h"
#include "IconHookFrame.h"

#include "Resource.h"

// CIconHookFrame

CIconHookFrame::CIconHookFrame()
{
}

CIconHookFrame::~CIconHookFrame()
{
}


BEGIN_MESSAGE_MAP(CIconHookFrame, CMiniFrameWnd)
    ON_MESSAGE(WM_USER_NOTIFYICON, OnNotifyIcon)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CIconHookFrame message handlers

void CIconHookFrame::OnTimer(UINT_PTR nTimerId)
{
	SendMessage(WM_COMMAND, IDM_POSTCAPTURE);
}

LRESULT CIconHookFrame::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	POINT pt;

    if (lParam == WM_RBUTTONDOWN)
    {
        if (!GetCursorPos(&pt))
	        return -1;

        if (!SetForegroundWindow())
	        return -1;

        CMenu dummy;
        if (!dummy.LoadMenu(IDR_ICONMENU))
            return -1;

        CMenu *popup = dummy.GetSubMenu(0);
        popup->TrackPopupMenuEx(TPM_RIGHTALIGN,
	        pt.x, pt.y, AfxGetMainWnd(), NULL);
    }

    return 0;
}
