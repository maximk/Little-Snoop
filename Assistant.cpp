// Assistant.cpp : implementation file
//

#include "stdafx.h"
#include "LittleSnoop.h"
#include "Assistant.h"
#include "SettingsDlg.h"

#define MAX_OPTION_SIZE 100

// CAssistant

CAssistant::CAssistant()
{
}

CAssistant::~CAssistant()
{
}

BOOL CAssistant::loadOptions()
{
	CWinApp *pApp = AfxGetApp();

	m_sSnoopOnMeHost = pApp->GetProfileString(AfxGetAppName(), "SnoopOnMeHost", "snoopon.me");
	m_nSnoopOnMePort = pApp->GetProfileInt(AfxGetAppName(), "SnoopOnMePort", 80);

	m_sUser = pApp->GetProfileString(AfxGetAppName(), "UserName", "nobody");
	m_sPassword = pApp->GetProfileString(AfxGetAppName(), "Password", "password");

	m_nSchedule = pApp->GetProfileInt(AfxGetAppName(), "Schedule", 1);

	m_nShrinkWidth = pApp->GetProfileInt(AfxGetAppName(), "ShrinkWidth", 320);
	m_nShrinkHeight = pApp->GetProfileInt(AfxGetAppName(), "ShrinkHeight", 240);

	m_nThumbWidth = pApp->GetProfileInt(AfxGetAppName(), "ThumbWidth", 80);
	m_nThumbHeight = pApp->GetProfileInt(AfxGetAppName(), "ThumbHeight", 60);

	return TRUE;
}

BOOL CAssistant::updateOptions()
{
	CWinApp *pApp = AfxGetApp();

	ASSERT(pApp->WriteProfileString(AfxGetAppName(), "SnoopOnMeHost", m_sSnoopOnMeHost));
	ASSERT(pApp->WriteProfileInt(AfxGetAppName(), "SnoopOnMePort", m_nSnoopOnMePort));

	ASSERT(pApp->WriteProfileString(AfxGetAppName(), "UserName", m_sUser));
	ASSERT(pApp->WriteProfileString(AfxGetAppName(), "Password", m_sPassword));

	ASSERT(pApp->WriteProfileInt(AfxGetAppName(), "Schedule", m_nSchedule));

	ASSERT(pApp->WriteProfileInt(AfxGetAppName(), "ShrinkWidth", m_nShrinkWidth));
	ASSERT(pApp->WriteProfileInt(AfxGetAppName(), "ShrinkHeight", m_nShrinkHeight));

	ASSERT(pApp->WriteProfileInt(AfxGetAppName(), "ThumbWidth", m_nThumbWidth));
	ASSERT(pApp->WriteProfileInt(AfxGetAppName(), "ThumbHeight", m_nThumbHeight));

	return TRUE;
}

BOOL CAssistant::captureScreen(CWnd *wndDesktop)
{
	CDC dc;
	HDC hdc = ::GetWindowDC(wndDesktop->m_hWnd);
	dc.Attach(hdc);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	CBitmap bm;
	CRect r;
	wndDesktop->GetWindowRect(&r);

	//CString s;
	//wndDesktop->GetWindowText(s);
	CSize sz(r.Width(), r.Height());
	bm.CreateCompatibleBitmap(&dc, m_nShrinkWidth, m_nShrinkHeight);
	CBitmap * oldbm = memDC.SelectObject(&bm);

	//TODO: StretchBlt mode may not be right; image is jagged
	memDC.StretchBlt(0, 0, m_nShrinkWidth, m_nShrinkHeight, &dc, 0, 0, sz.cx, sz.cy, SRCCOPY);

	wndDesktop->OpenClipboard();
	::EmptyClipboard();
	::SetClipboardData(CF_BITMAP, bm.m_hObject);
	CloseClipboard();

	memDC.SelectObject(oldbm);
	bm.Detach();  // make sure bitmap not deleted with CBitmap object
	::ReleaseDC(wndDesktop->m_hWnd, dc.Detach());
	return TRUE;
}

BOOL CAssistant::postScreenshot()
{
	// curl -X POST -H "Content-Type: application/json"
	// -d "{\"u\":\"angel1\",\"snap\":\"100\"}" http://localhost:5984/som

	CString doc("{\"snap\":100}");

	CInternetSession *session = new CInternetSession();
	CHttpConnection *connection =
		session->GetHttpConnection(m_sSnoopOnMeHost, (INTERNET_PORT)m_nSnoopOnMePort);
	CHttpFile *file =
		connection->OpenRequest(CHttpConnection::HTTP_VERB_POST, m_sUser);

	file->AddRequestHeaders("Content-Type: application/json\r\n");
	file->SendRequestEx(doc.GetLength());
	file->WriteString(doc);
	file->EndRequest();

	file->Close();
	connection->Close();
	session->Close();

	delete file;
	delete connection;
	delete session;
	return TRUE;
}

// CAssistant member functions

void CAssistant::showSettingsDialog()
{
	CSettingsDlg dlg;

	dlg.sHostName = m_sSnoopOnMeHost;
	dlg.nPort = m_nSnoopOnMePort;
	dlg.sUserName = m_sUser;
	dlg.sPassword = m_sPassword;
	dlg.nSchedule = m_nSchedule;
	dlg.nShrinkWidth = m_nShrinkWidth;
	dlg.nShrinkHeight = m_nShrinkHeight;
	dlg.nThumbWidth = m_nThumbWidth;
	dlg.nThumbHeight = m_nThumbHeight;

	if (dlg.DoModal() == IDOK)
	{
		m_sSnoopOnMeHost = dlg.sHostName;
		m_nSnoopOnMePort = dlg.nPort;
		m_sUser = dlg.sUserName;
		m_sPassword = dlg.sPassword;
		m_nSchedule = dlg.nSchedule;
		m_nShrinkWidth = dlg.nShrinkWidth;
		m_nShrinkHeight = dlg.nShrinkHeight;
		m_nThumbWidth = dlg.nThumbWidth;
		m_nThumbHeight = dlg.nThumbHeight;
	}
}

//EOF
