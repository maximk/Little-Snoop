#pragma once


#include "gdiplus.h"

#define MAX_SCREENS 8

// CScreenSnapper command target

class CScreenSnapper : public CObject
{
public:
	CScreenSnapper();
	virtual ~CScreenSnapper();

	CString snap(void);

	static CSize getShrinkSize(CSize sz);
	static CSize getThumbSize(CSize sz);

	static BOOL bitmapIsAllBlack(Gdiplus::Bitmap *bitmap);

private:
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
};


