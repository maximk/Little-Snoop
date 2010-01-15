#pragma once



// CScreenshot command target

class CScreenshot : public CObject
{
public:
	CScreenshot(CSize szOriginal, CSize szShrunk, CSize szThumb,
		BYTE *pShrunkData, INT_PTR nShrunkSize,
		BYTE *pThmbData, INT_PTR nThumbSize);
	virtual ~CScreenshot();

private:
	CTime m_tWhen;
	CSize m_szOriginal;
	CSize m_szShrunk;
	CSize m_szThumb;

	CByteArray m_arrSnapshot;
	CByteArray m_arrThumbnail;
};

//EOF
