// Base64.cpp : implementation file
//

#include "stdafx.h"
#include "LittleSnoop.h"
#include "Base64.h"


// CBase64

CBase64::CBase64()
{
}

CBase64::~CBase64()
{
}

// CBase64 member functions

static const char base64_chars[] = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

CString CBase64::encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  CString ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

CString CBase64::encode(IStream *stream_to_encode)
{
	ULARGE_INTEGER new_pos;
	LARGE_INTEGER zero;
	zero.QuadPart = 0;
	stream_to_encode->Seek(zero, STREAM_SEEK_END, &new_pos);
	DWORD size = new_pos.LowPart;

	BYTE *data = new BYTE[size];
	stream_to_encode->Seek(zero, STREAM_SEEK_SET, NULL);
	stream_to_encode->Read(data, size, NULL);

	CString encodedData =  encode(data, size);
	delete data;

	return encodedData;
}

//EOF