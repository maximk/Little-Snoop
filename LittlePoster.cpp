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

DWORD CLittlePoster::post(int nService, CString body, CString sDispPath)
{
	CString host = CSnoopOptions::m_sCaptureHost;
	int port = CSnoopOptions::m_nCapturePort;
	CString path;
	DWORD status;

	if (nService == CLittlePoster::CAPTURE)
		path = CSnoopOptions::m_sCapturePath;
	else if (nService == CLittlePoster::REGISTER)
		path = CSnoopOptions::m_sRegisterPath;
	else if (nService == CLittlePoster::TEST)
		path = CSnoopOptions::m_sTestPath;
	else
		return 500;

	CInternetSession *session = new CInternetSession();
	CHttpConnection *connection =
		session->GetHttpConnection(host, (INTERNET_PORT)port);
	CHttpFile *file =
		connection->OpenRequest(CHttpConnection::HTTP_VERB_POST, path + _T("/") + sDispPath);

	try {
		file->AddRequestHeaders(_T("Content-Type: application/json\r\n"));
		file->SendRequestEx(body.GetLength());
		file->WriteString(body);
		file->EndRequest();

		file->QueryInfoStatusCode(status);
	}
	catch(CInternetException *e)
	{
		CString str, msg;
		str.Format("%d", e->m_dwError);
		AfxFormatString1(msg, IDS_INTERNETERROR, str);
		AfxMessageBox(msg);

		status = 666;
	}

	file->Close();
	connection->Close();
	session->Close();

	delete file;
	delete connection;
	delete session;

	return status;
}

bool CLittlePoster::testCredentials(CString sUser, CString sPassword)
{
	CString doc;
	doc.Format("{\"password\":\"%s\"}", sPassword);

	return post(CLittlePoster::TEST, doc, sUser) < 300;
}

//EOF
