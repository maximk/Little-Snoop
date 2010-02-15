#pragma once



// CLittlePoster command target

class CLittlePoster : public CObject
{
public:
	CLittlePoster();
	virtual ~CLittlePoster();

	static const int CAPTURE = 1;
	static const int REGISTER = 2;
	static const int TEST = 3;

	DWORD post(CString path, CString body, CString sDispPath = "");
	//bool testCredentials(CString sUser, CString sPassword);
};


