// Assistant.cpp : implementation file
//

#include "stdafx.h"
#include "LittleSnoop.h"
#include "Assistant.h"


// CAssistant

CAssistant::CAssistant()
{
}

CAssistant::~CAssistant()
{
}

BOOL CAssistant::captureScreen(CWnd *wndDesktop)
{
	CSize capSz(320, 240);	//TODO: do some smart shrinking probably look at the default screen font size

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
	bm.CreateCompatibleBitmap(&dc, capSz.cx, capSz.cy);
	CBitmap * oldbm = memDC.SelectObject(&bm);

	//TODO: StretchBlt mode may not be right; image is jagged
	memDC.StretchBlt(0, 0, capSz.cx, capSz.cy, &dc, 0, 0, sz.cx, sz.cy, SRCCOPY);

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
	CHttpConnection *connection = session->GetHttpConnection("localhost", (INTERNET_PORT)5984);	//TODO: hard-coded
	CHttpFile *file = connection->OpenRequest(CHttpConnection::HTTP_VERB_POST, "som"); //TODO: ditto

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
