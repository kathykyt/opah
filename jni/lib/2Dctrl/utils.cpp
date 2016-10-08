#include "utils.h"

static int hex2num( char c )
{
	if ( c >= '0' && c <= '9' )
		return(c - '0');
	if ( c >= 'a' && c <= 'f' )
		return(c - 'a' + 10);
	if ( c >= 'A' && c <= 'F' )
		return(c - 'A' + 10);
	return(-1);
}


int hex2byte( const char *hex )
{
	int a, b; a = hex2num( *hex++ ); if ( a < 0 )
		return(-1);
	b = hex2num( *hex++ ); if ( b < 0 )
		return(-1);
	return( (a << 4) | b);
}

int hexstr2bin( const char *hex, unsigned char *buf, int len )
{
	int		i;
	int		a;
	const char	*ipos	= hex;
	unsigned char		*opos	= buf;

	for ( i = 0; i < len; i++ )
	{
		a = hex2byte( ipos );
		if ( a < 0 )
			return(-1);
		*opos++ = a;
		ipos	+= 2;
	}
	return(0);
}


int ssid_decode(char *buf, int maxlen, char *str)
{
	const char *pos = str;
	int len = 0;
	int val;

	while (*pos) {
		if (len == maxlen)
			break;
		switch (*pos) {
		case '\\':
			pos++;
			switch (*pos) {
			case '\\':
				buf[len++] = '\\';
				pos++;
				break;
			case '"':
				buf[len++] = '"';
				pos++;
				break;
			case 'n':
				buf[len++] = '\n';
				pos++;
				break;
			case 'r':
				buf[len++] = '\r';
				pos++;
				break;
			case 't':
				buf[len++] = '\t';
				pos++;
				break;
			case 'e':
				buf[len++] = '\e';
				pos++;
				break;
			case 'x':
				pos++;
				val = hex2byte(pos);
				if (val < 0) {
					val = hex2num(*pos);
					if (val < 0)
						break;
					buf[len++] = val;
					pos++;
				} else {
					buf[len++] = val;
					pos += 2;
				}
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				val = *pos++ - '0';
				if (*pos >= '0' && *pos <= '7')
					val = val * 8 + (*pos++ - '0');
				if (*pos >= '0' && *pos <= '7')
					val = val * 8 + (*pos++ - '0');
				buf[len++] = val;
				break;
			default:
				break;
			}
			break;
		default:
			buf[len++] = *pos++;
			break;
		}
	}

	return len;
}


