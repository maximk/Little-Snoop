// Assistant.cpp : implementation file
//

#include "stdafx.h"

#include "LittleSnoop.h"
#include "Assistant.h"
#include "SettingsDlg.h"

#define MAX_OPTION_SIZE 100

using namespace Gdiplus;

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

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

//BOOL CAssistant::captureScreen(CWnd *wndDesktop)
//{
//	CDC dc;
//	HDC hdc = ::GetWindowDC(wndDesktop->m_hWnd);
//	dc.Attach(hdc);
//
//	CDC memDC;
//	memDC.CreateCompatibleDC(&dc);
//
//	CBitmap bm;
//	CRect r;
//	wndDesktop->GetWindowRect(&r);
//
//	//CString s;
//	//wndDesktop->GetWindowText(s);
//	CSize sz(r.Width(), r.Height());
//	bm.CreateCompatibleBitmap(&dc, m_nShrinkWidth, m_nShrinkHeight);
//	CBitmap * oldbm = memDC.SelectObject(&bm);
//
//	//TODO: StretchBlt mode may not be right; image is jagged
//	memDC.StretchBlt(0, 0, m_nShrinkWidth, m_nShrinkHeight, &dc, 0, 0, sz.cx, sz.cy, SRCCOPY);
//
//	wndDesktop->OpenClipboard();
//	::EmptyClipboard();
//	::SetClipboardData(CF_BITMAP, bm.m_hObject);
//	CloseClipboard();
//
//	memDC.SelectObject(oldbm);
//	bm.Detach();  // make sure bitmap not deleted with CBitmap object
//	::ReleaseDC(wndDesktop->m_hWnd, dc.Detach());
//	return TRUE;
//}

struct CaptureContext
{
	int count;
	int max_count;
	CSize *sizes;
	Bitmap **snapshots;
	Bitmap **thumbnails;
	int shrinkWidth;
	int shrinkHeight;
};

BOOL CALLBACK captureOneScreen(HMONITOR hMonitor,
							   HDC hdcMonitor,
                               LPRECT lprcMonitor,
							   LPARAM dwData)
{
	CaptureContext *context = (CaptureContext *)dwData;

	if (context->count >= context->max_count)
		return FALSE;

	MONITORINFOEX monitorInfo;
	memset(&monitorInfo, 0x0, sizeof(MONITORINFOEX));
	monitorInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &monitorInfo);

	// Create a bitmap for the current screen
	Bitmap *bm = new Bitmap(context->shrinkWidth, context->shrinkHeight);

	// Get a graphics object for the bitmap and initialize it...
	Graphics *g = Graphics::FromImage(bm);     

	g->SetCompositingQuality(CompositingQualityHighSpeed);	//TODO: check other settings

	// Get HDCs for source and destination...
	HDC hdcDestination = g->GetHDC();
	HDC hdcSource = ::CreateDC(NULL, monitorInfo.szDevice, NULL, NULL);

	// Move the bits...
	StretchBlt(hdcDestination, 0, 0, context->shrinkWidth, context->shrinkHeight, 
					hdcSource, 0, 0, lprcMonitor->right, lprcMonitor->bottom, SRCCOPY | CAPTUREBLT);
	    
	// Cleanup source and destination HDC...
	DeleteDC(hdcSource);                    
	g->ReleaseHDC(hdcDestination);

	CSize size(lprcMonitor->right, lprcMonitor->bottom);
	context->sizes[context->count] = size;
	context->snapshots[context->count] = bm;
	context->thumbnails[context->count] = NULL;
	context->count++;

	return TRUE;
}

int CAssistant::captureScreen(Bitmap *snaps[], Bitmap *thumbs[], CSize sizes[], int max_count)
{
	CaptureContext context;
	context.count = 0;
	context.max_count = max_count;
	context.sizes = sizes;
	context.snapshots = snaps;
	context.thumbnails = thumbs;
	context.shrinkWidth = m_nShrinkWidth;
	context.shrinkHeight = m_nShrinkHeight;

	::EnumDisplayMonitors(NULL, NULL, captureOneScreen, (LPARAM)&context);

	return context.count;
}

BOOL CAssistant::postScreenshot(Bitmap *snaps[], Bitmap *thumbs[], CSize sizes[], int count)
{
	CString boundary = _T("FFF3F395A90B452BB8BEDC878DDBD152");

	CInternetSession *session = new CInternetSession();
	//CHttpConnection *connection =
	//	session->GetHttpConnection(m_sSnoopOnMeHost, (INTERNET_PORT)m_nSnoopOnMePort);
	CHttpConnection *connection =
		session->GetHttpConnection("localhost", (INTERNET_PORT)5984);
	//CHttpFile *file =
	//	connection->OpenRequest(CHttpConnection::HTTP_VERB_POST, m_sUser);
	CHttpFile *file =
		connection->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("som2"));

	// user:
	// when:
	// orig_width1:
	// orig_height1:
	// width1:
	// height1:
	// snapshot1:

	CString encodedPng = "gdheyfjfkgur94jssye73847";

	CString doc;
	doc.Format(_T("{"
		"\"user\":\"%s\","
		"\"when\":\"just_in_time\","
		"\"orig_width1\":%d,"
		"\"orig_height1\":%d,"
		"\"width1\":%d,"
		"\"height1\":%d,"
		"\"snapshot1\":\"%s\""
		"}"),
		m_sUser,
		sizes[0].cx,
		sizes[0].cy,
		snaps[0]->GetWidth(),
		snaps[0]->GetHeight(),
		encodedPng);

	file->AddRequestHeaders(_T("Content-Type: application/json\r\n"));
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

int CAssistant::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

//EOF
