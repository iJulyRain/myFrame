#include "common.h"
#include <iconv.h>

static int code_convert(char *from_charset, char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	iconv_t cd;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset, from_charset);
	if(cd == (iconv_t)-1)
	{
		perror("iconv_open:");
		return -1;
	}
	memset(outbuf, 0, outlen);

	if(iconv(cd, pin, &inlen, pout, &outlen) == -1)
	{
		perror("iconv");
	}

	iconv_close(cd);
	return 0;
}

int gbk_2_utf8(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("GBK", "UTF-8//TRANSLIT", inbuf, inlen, outbuf, outlen);
}

int utf8_2_gbk(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("UTF-8", "GBK//TRANSLIT", inbuf, inlen, outbuf, outlen);
}
