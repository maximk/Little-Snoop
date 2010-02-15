// LittlePoster.cpp : implementation file
//

#include "stdafx.h"
#include "LittleSnoop.h"
#include "LittlePoster.h"
#include ".\littleposter.h"

// CLittlePoster

CLittlePoster::CLittlePoster()
{
}

CLittlePoster::~CLittlePoster()
{
}


// CLittlePoster member functions

DWORD CLittlePoster::post(CString path, CString body)
{
	CString host = CSnoopOptions::m_sCaptureHost;
	int port = CSnoopOptions::m_nCapturePort;
	LPCTSTR accepts[] = {_T("application/json"),NULL};
	DWORD status;

	CInternetSession *session = new CInternetSession();
	CHttpConnection *connection =
		session->GetHttpConnection(host, (INTERNET_PORT)port);

	CHttpFile *f = connection->OpenRequest(CHttpConnection::HTTP_VERB_GET,
		m_sSettingsPath + _T("/") + m_sLittleSnoopId, 0, 1, accepts);
	f->SendRequest();

	f->QueryInfoStatusCode(&status);

	if (status == 404)
	{
		f->Close();
	}
	CString settings = f->ReadString();
	f->Close();




	CString url = m_sSettingsPath + _T("/") + m_sLittleSnoopId;
	CStdioFile *settings = connection->O

	CHttpFile *file =
		connection->OpenRequest(CHttpConnection::HTTP_VERB_POST, path);

	try {
		file->AddRequestHeaders(_T("Content-Type: application/json\r\n"));
		file->SendRequestEx(body.GetLength());
		file->WriteString(body);
		file->EndRequest();

		file->QueryInfoStatusCode(status);
	}
	catch(CInternetException *e)
	{
		//CString str, msg;
		//str.Format("%d", e->m_dwError);
		//AfxFormatString1(msg, IDS_INTERNETERROR, str);
		//AfxMessageBox(msg);
		status = 666;
		e->Delete();
	}

	file->Close();
	connection->Close();
	session->Close();

	delete file;
	delete connection;
	delete session;

	return status;
}

//bool CLittlePoster::testCredentials(CString sUser, CString sPassword)
//{
//	CString doc;
//	doc.Format("{\"password\":\"%s\"}", sPassword);
//
//	return post(CLittlePoster::TEST, doc, sUser) < 300;
//}

//EOF
