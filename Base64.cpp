//
//
//

#include "stdafx.h"

/*
** Translation Table as described in RFC1113
*/
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
** encodeblock
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
void encodeblock( unsigned char in[3], unsigned char out[4], int len )
{
    out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

/*
** encode
**
** base64 encode a stream adding padding and line breaks as per spec.
*/

void Base64EncodeMemory(FIMEMORY *input, FIMEMORY *output)
{
    while(1) {
	    unsigned char in[3], out[4];
		int len;
		in[0] = in[1] = in[2] = 0;
		len = FreeImage_ReadMemory(in, 1, 3, input);
		if (len == 0)
			break;
        encodeblock( in, out, len );
		FreeImage_WriteMemory(out, 1, 4, output);
    }
}

//EOF
