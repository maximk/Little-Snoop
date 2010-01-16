#pragma once



// CBase64 command target

class CBase64 : public CObject
{
public:
	CBase64();
	virtual ~CBase64();

	static CString CBase64::encode(unsigned char const* bytes_to_encode, unsigned int in_len);
	static CString CBase64::encode(IStream *stream_to_encode);
};


