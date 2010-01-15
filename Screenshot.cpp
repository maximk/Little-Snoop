// Screenshot.cpp : implementation file
//

#include "stdafx.h"
#include "LittleSnoop.h"
#include "Screenshot.h"


// CScreenshot

CScreenshot::CScreenshot(CSize szOriginal, CSize szShrunk, CSize szThumb,
	BYTE *pShrunkData, INT_PTR nShrunkSize,
	BYTE *pThumbData, INT_PTR nThumbSize) :
	m_szOriginal(szOriginal),
	m_szShrunk(szShrunk),
	m_szThumb(szThumb)
{
	m_tWhen = CTime::GetCurrentTime();

	m_arrSnapshot.SetSize(nShrunkSize);
	for (int i = 0; i < nShrunkSize; i++)
		m_arrSnapshot.SetAt(i, pShrunkData[i]);

	m_arrThumbnail.SetSize(nThumbSize);
	for (int i = 0; i < nShrunkSize; i++)
		m_arrThumbnail.SetAt(i, pThumbData[i]);
}

CScreenshot::~CScreenshot()
{
}


// CScreenshot member functions
