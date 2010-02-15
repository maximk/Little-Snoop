#pragma comment (lib, "Gdiplus.lib")

// ScreenSnapper.cpp : implementation file
//

#include "stdafx.h"
#include "LittleSnoop.h"
#include "ScreenSnapper.h"
#include "Base64.h"

#include "gdiplus.h"

#define ARGB_IS_BLACK(argb)		(((argb) & 0xffffff) == 0x000000)

// CScreenSnapper

CScreenSnapper::CScreenSnapper()
{
}

CScreenSnapper::~CScreenSnapper()
{
}

// CScreenSnapper member functions

CSize CScreenSnapper::getShrinkSize(CSize sz)
{
	// Standard sizes:
	const int w1 = 1024;	// -> 320x240
	const int h1 = 768;
	const int w2 = 1280;	// -> 320x256
	const int h2 = 1024;
	const int w3 = 1440;	// -> 320x200
	const int h3 = 900;		

	int d1 = (w1 - sz.cx)*(w1 - sz.cx) - (h1 - sz.cy)*(h1 - sz.cy);
	int d2 = (w2 - sz.cx)*(w2 - sz.cx) - (h2 - sz.cy)*(h2 - sz.cy);
	int d3 = (w3 - sz.cx)*(w3 - sz.cx) - (h3 - sz.cy)*(h3 - sz.cy);

	if (d1 < d2 && d1 < d3)
		return CSize(320, 240);
	else if (d2 < d3)
		return CSize(320, 256);
	else
		return CSize(320, 200);
}

CSize CScreenSnapper::getThumbSize(CSize sz)
{
	// Standard sizes:
	const int w1 = 1024;	// -> 320x240
	const int h1 = 768;
	const int w2 = 1280;	// -> 320x256
	const int h2 = 1024;
	const int w3 = 1440;	// -> 320x200
	const int h3 = 900;		

	int d1 = (w1 - sz.cx)*(w1 - sz.cx) - (h1 - sz.cy)*(h1 - sz.cy);
	int d2 = (w2 - sz.cx)*(w2 - sz.cx) - (h2 - sz.cy)*(h2 - sz.cy);
	int d3 = (w3 - sz.cx)*(w3 - sz.cx) - (h3 - sz.cy)*(h3 - sz.cy);

	if (d1 < d2 && d1 < d3)
		return CSize(80, 60);
	else if (d2 < d3)
		return CSize(80, 64);
	else
		return CSize(80, 50);
}

BOOL bitmapIsAllBlack(Gdiplus::Bitmap *bitmap)
{
	// Heuristic check if bitmap is all black
	// Check 8 random pixels

	UINT w = bitmap->GetWidth();
	UINT h = bitmap->GetHeight();

	for (int n = 0; n < 8; n++)
	{
		INT x = w * rand() / (RAND_MAX+1);
		INT y = h * rand() / (RAND_MAX+1);

		Gdiplus::Color color;
		bitmap->GetPixel(x, y, &color);
		if (!ARGB_IS_BLACK(color.GetValue()))
			return FALSE;
	}

	return TRUE;
}

struct CaptureContext
{
	int count;
	int maxCount;

	CSize *origSizes;
	CSize *shrinkSizes;
	CSize *thumbSizes;

	Gdiplus::Image **snapshots;
	Gdiplus::Image **thumbnails;
};

BOOL CALLBACK captureOneScreen(HMONITOR hMonitor,
							   HDC hdcMonitor,
                               LPRECT lprcMonitor,
							   LPARAM dwData)
{
	CaptureContext *context = (CaptureContext *)dwData;

	if (context->count >= context->maxCount)
		return FALSE;

	MONITORINFOEX monitorInfo;
	memset(&monitorInfo, 0x0, sizeof(MONITORINFOEX));
	monitorInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &monitorInfo);

	CSize origSize(lprcMonitor->right - lprcMonitor->left,
		lprcMonitor->bottom - lprcMonitor->top);

	// Create bitmaps for the current screen
	Gdiplus::Bitmap *bmp = new Gdiplus::Bitmap(origSize.cx, origSize.cy);

	// Get graphics object for the bitmap and initialize it
	Gdiplus::Graphics *graphics = Gdiplus::Graphics::FromImage(bmp);

	// Get HDCs for source and destination...
	HDC hdcDestination = graphics->GetHDC();
	HDC hdcSource = ::CreateDC(NULL, monitorInfo.szDevice, NULL, NULL);

	// Move the bits to snapshot and thumbnail...
	StretchBlt(hdcDestination, 0, 0, origSize.cx, origSize.cy, 
		hdcSource, 0, 0, origSize.cx, origSize.cy, SRCCOPY | CAPTUREBLT);
	    
	// Cleanup source and destination HDC...
	DeleteDC(hdcSource);                    
	graphics->ReleaseHDC(hdcDestination);

	// Black monitors not included -- happens if monitor is in text mode etc
	if (bitmapIsAllBlack(bmp))
	{
		delete bmp;
		return TRUE;
	}

	CSize shrinkSize = CScreenSnapper::getShrinkSize(origSize);
	CSize thumbSize =  CScreenSnapper::getThumbSize(origSize);

	// Shrink image size to render text invisible; also make a thumbnail image
	Gdiplus::Image *shrunk = bmp->GetThumbnailImage(shrinkSize.cx, shrinkSize.cy);
	Gdiplus::Image *thumb = bmp->GetThumbnailImage(thumbSize.cx, thumbSize.cy);
	delete bmp;

	context->origSizes[context->count] = origSize;
	context->shrinkSizes[context->count] = shrinkSize;
	context->thumbSizes[context->count] = thumbSize;

	context->snapshots[context->count] = shrunk;
	context->thumbnails[context->count] = thumb;
	context->count++;

	return TRUE;
}

CString CScreenSnapper::snap(void)
{
	CSize origSizes[MAX_SCREENS];
	CSize shrinkSizes[MAX_SCREENS];
	CSize thumbSizes[MAX_SCREENS];

	Gdiplus::Image *snapshots[MAX_SCREENS];
	Gdiplus::Image *thumbnails[MAX_SCREENS];

	CaptureContext context;
	context.count = 0;
	context.maxCount = MAX_SCREENS;
	context.origSizes = origSizes;
	context.shrinkSizes = shrinkSizes;
	context.thumbSizes = thumbSizes;
	context.snapshots = snapshots;
	context.thumbnails = thumbnails;

	::EnumDisplayMonitors(NULL, NULL, captureOneScreen, (LPARAM)&context);

	if (context.count == 0)	// usually means that all monitors are black
		return "";

	CLSID clsid;
	GetEncoderClsid(L"image/png", &clsid);

	CString screen_docs[MAX_SCREENS];
	for (int i = 0; i < context.count; i++)
	{
		LPSTREAM str;
		CreateStreamOnHGlobal(NULL, TRUE, &str);
		snapshots[i]->Save(str, &clsid);
		CString encodedPng1 = CBase64::encode(str);
		str->Release();
		CreateStreamOnHGlobal(NULL, TRUE, &str);
		thumbnails[i]->Save(str, &clsid);
		CString encodedPng2 = CBase64::encode(str);
		str->Release();

		CString screen_doc;
		screen_doc.Format("{\"orig_width\":%d,\"orig_height\":%d,"
			"\"width\":%d,\"height\":%d,"
			"\"thumb_width\":%d,\"thumb_height\":%d,"
			"\"snapshot\":\"%s\",\"thumbnail\":\"%s\"}",
			origSizes[i].cx, origSizes[i].cy,
			shrinkSizes[i].cx, shrinkSizes[i].cy,
			thumbSizes[i].cx, thumbSizes[i].cy,
			encodedPng1, encodedPng2);

		screen_docs[i] = screen_doc;
	}

	// cleanup images; all information is in screen_docs now
	for (int i = 0; i < context.count; i++)
	{
		delete snapshots[i];
		delete thumbnails[i];
	}

	CString doc;
	doc.Format("[%s", screen_docs[0]);

	for (int i = 1; i < context.count; i++)
		doc.AppendFormat(",%s", screen_docs[i]);

	doc += "]";
	return doc;
}

int CScreenSnapper::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

   Gdiplus::GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

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
