//
//
//

#include "stdafx.h"

#include "Options.h"
#include "Base64.h"

#define MAX_SCREENS 8

LPCTSTR DoSnapScreensToJson();

typedef struct {
	int count;
	int maxCount;
	SIZE origSizes[MAX_SCREENS];
	SIZE shrinkSizes[MAX_SCREENS];
	SIZE thumbSizes[MAX_SCREENS];
	FIBITMAP *snapshots[MAX_SCREENS];
	FIBITMAP *thumbnails[MAX_SCREENS];
} CaptureContext;

LPCTSTR SnapScreensToJson()
{
	LPCTSTR szJson;
	HMODULE hFreeImageLib = LoadLibrary(_T("FreeImage.dll"));
	if (hFreeImageLib == NULL)
		return _T("");

	szJson = DoSnapScreensToJson();

	FreeLibrary(hFreeImageLib);
	return szJson;
}

SIZE GetShrinkSize(SIZE sz)
{
	SIZE result;

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
	{
		result.cx = 320;
		result.cy = 240;
	}
	else if (d2 < d3)
	{
		result.cx = 320;
		result.cy = 256;
	}
	else
	{
		result.cx = 320;
		result.cy = 200;
	}

	return result;
}

SIZE GetThumbSize(SIZE sz)
{
	SIZE result;

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
	{
		result.cx = 80;
		result.cy = 60;
	}
	else if (d2 < d3)
	{
		result.cx = 80;
		result.cy = 64;
	}
	else
	{
		result.cx = 80;
		result.cy = 50;
	}

	return result;
}


BOOL CALLBACK captureOneScreen(HMONITOR hMonitor,
							   HDC hdcMonitor,
                               LPRECT lprcMonitor,
							   LPARAM dwData)
{
	CaptureContext *context = (CaptureContext *)dwData;
	MONITORINFOEX monitorInfo;
	HDC hdcSource, hdcDestination;
	HBITMAP hbmp, saved;
	BITMAP bm;
	SIZE origSize, shrinkSize, thumbSize;
	FIBITMAP *dib, *dib24;

	if (context->count >= context->maxCount)
		return FALSE;

	memset(&monitorInfo, 0, sizeof(MONITORINFOEX));
	monitorInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, (LPMONITORINFO) &monitorInfo);

	origSize.cx = lprcMonitor->right - lprcMonitor->left;
	origSize.cy = lprcMonitor->bottom - lprcMonitor->top;

	hdcSource = CreateDC(NULL, monitorInfo.szDevice, NULL, NULL);
	hdcDestination = CreateCompatibleDC(hdcSource);
	hbmp = CreateCompatibleBitmap(hdcSource, origSize.cx, origSize.cy);
	saved = (HBITMAP)SelectObject(hdcDestination, hbmp);

	StretchBlt(hdcDestination, 0, 0, origSize.cx, origSize.cy, 
		hdcSource, 0, 0, origSize.cx, origSize.cy, SRCCOPY | CAPTUREBLT);

	GetObject(hbmp, sizeof(BITMAP), (LPSTR) &bm);
	dib = FreeImage_Allocate(bm.bmWidth, bm.bmHeight, bm.bmBitsPixel, 0, 0, 0);

	GetDIBits(hdcDestination, hbmp, 0, FreeImage_GetHeight(dib), 
		FreeImage_GetBits(dib), FreeImage_GetInfo(dib), DIB_RGB_COLORS);
	dib24 = FreeImage_ConvertTo24Bits(dib);		// _Rescale does not work for 32 bits
	FreeImage_Unload(dib);

	SelectObject(hdcDestination, saved);
	DeleteObject(hbmp);

	DeleteDC(hdcDestination);
	DeleteDC(hdcSource);

	// Black monitors not included -- happens if monitor is in text mode etc
	//if (bitmapIsAllBlack(bmp))
	//{
	//	delete bmp;
	//	return TRUE;
	//}

	shrinkSize = GetShrinkSize(origSize);
	thumbSize =  GetThumbSize(origSize);

	context->origSizes[context->count] = origSize;
	context->shrinkSizes[context->count] = shrinkSize;
	context->thumbSizes[context->count] = thumbSize;

	// Shrink image size to render text invisible; also make a thumbnail image
	context->snapshots[context->count] = FreeImage_Rescale(dib24, shrinkSize.cx, shrinkSize.cy, FILTER_BILINEAR);
	context->thumbnails[context->count] = FreeImage_Rescale(dib24, thumbSize.cx, thumbSize.cy, FILTER_BILINEAR);
	context->count++;

	FreeImage_Unload(dib24);
	return TRUE;
}

LPCTSTR DoSnapScreensToJson()
{
	char buf[256];

	FIMEMORY *json = NULL;
	BYTE *data;
	DWORD size;
	LPCTSTR result;
	int i;

	CaptureContext context;
	context.count = 0;
	context.maxCount = MAX_SCREENS;

	EnumDisplayMonitors(NULL, NULL, captureOneScreen, (LPARAM)&context);

	if (context.count == 0)	// usually means that all monitors are black
		return NULL;

	json = FreeImage_OpenMemory(0, 0);

	_snprintf(buf, sizeof(buf), "{\"ls_id\":\"%s\",\"screens\":[", g_szLittleSnoopId);
	FreeImage_WriteMemory(buf, 1, (unsigned int)strlen(buf), json);

	for (i = 0; i < context.count; i++)
	{
		FIMEMORY *snapPng = FreeImage_OpenMemory(0, 0);
		FIMEMORY *thumbPng = FreeImage_OpenMemory(0, 0);

		FreeImage_SaveToMemory(FIF_PNG, context.snapshots[i], snapPng, 0);
		FreeImage_Unload(context.snapshots[i]);
		FreeImage_SaveToMemory(FIF_PNG, context.thumbnails[i], thumbPng, 0);
		FreeImage_Unload(context.thumbnails[i]);

		_snprintf(buf, sizeof(buf),
			
			"%s{\"orig_width\":%d,\"orig_height\":%d,"
			"\"width\":%d,\"height\":%d,"
			"\"thumb_width\":%d,\"thumb_height\":%d,"
			"\"snapshot\":\"",

			(i > 0) ?"," :"",
			context.origSizes[i].cx, context.origSizes[i].cy,
			context.shrinkSizes[i].cx, context.shrinkSizes[i].cy,
			context.thumbSizes[i].cx, context.thumbSizes[i].cy);

		FreeImage_WriteMemory(buf, 1, (unsigned int)strlen(buf), json);
		FreeImage_SeekMemory(snapPng, 0L, SEEK_SET);
		Base64EncodeMemory(snapPng, json);
		FreeImage_CloseMemory(snapPng);
		FreeImage_WriteMemory("\",\"thumbnail\":\"", 1, 15, json);	//Beware: 15
		FreeImage_SeekMemory(thumbPng, 0L, SEEK_SET);
		Base64EncodeMemory(thumbPng, json);
		FreeImage_CloseMemory(thumbPng);
		FreeImage_WriteMemory("\"}", 1, 2, json);
	}

	FreeImage_WriteMemory("]}\0", 1, 3, json);	// null-terminate

	FreeImage_AcquireMemory(json, &data, &size);
	result = (LPCTSTR) _strdup((const char *)data);	//XXX: conversion needed for Unicode
	FreeImage_CloseMemory(json);

	return result;
}

//EOF
