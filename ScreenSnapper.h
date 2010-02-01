#pragma once



#define MAX_SCREENS 3

// CScreenSnapper command target

class CScreenSnapper : public CObject
{
public:
	CScreenSnapper();
	virtual ~CScreenSnapper();

	CString snap(void);

	static CSize getShrinkSize(CSize sz);
	static CSize getThumbSize(CSize sz);

private:
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
};


