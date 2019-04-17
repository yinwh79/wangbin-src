/***********************************************************************************
Copy right:	2014-2024 USR IOT Tech.
Author:			Bin Wang
Version:			V1.0
Date:				2014-08
Description:	timer0 thread.
***********************************************************************************/
#define __PUBLIC_C


#include "public.h"


int atoi_uint32( char *s )
{
	uint32_t i, len, num;
	
	len = strlen(s);
	num = 0;
	
	if( (len == 0) || (len > 10) ) return 0;
	if( (len == 10) && (*s > '4') ) return 0;
	
	for( i = 0; i < len; i ++ )
	{
		if( (*(s+i) < '0') || (*(s+i) > '9') ) return 0;
		num = num*10 + ( *(s+i) - '0' );
	}
	
	return num;
}

int atoi_skipchar( char *s )
{
	uint32_t i, len, num;
	
	len = strlen(s);
	num = 0;
	
	if( (len == 0) || (len > 10) ) return 0;
	if( (len == 10) && (*s > '4') ) return 0;
	
	for( i = 0; i < len; i ++ )
	{
		if( (*(s+i) < '0') || (*(s+i) > '9') ) return 0;
		num = num*10 + ( *(s+i) - '0' );
	}
	
	return num;
}

//note: s is the string compared, find is the string comparing
char* strnstr( char *s, const char *find, int slen )
{
	int i, flen;

	flen = strlen( find );
	if ( flen > slen ) return NULL;
	
	for ( i = 0; i < slen; i ++ )
	{
		*(s+i) = toupper( (int)*(s+i) );
	}
	
	for ( i=0; i<=slen-flen; i++ )
	{
		if ( s[i] == *find )
		{
			if ( strncmp( s+i, find, slen-i ) == 0 ) return (char*)( s + i );
		}
	}
	
	return NULL;
}


BOOL CmpStringFront( char *cmped, char *cmping )
{
	uint32_t i;
	
	if ( strlen( cmped ) < strlen( cmping ) ) return FALSE;
	
	for ( i = 0; i < strlen( cmping ); i ++ )
	{
		if ( *(cmped+i) != *(cmping+i) ) return FALSE;
	}
	
	return TRUE;
}


BOOL IsDomain( char* s )
{
	while( s++ != NULL )
	{
		if ( ( (*s < '0') || (*s > '9') ) && (*s != '.') )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}


BOOL IsUnicode( char* s )
{
	while( s++ != NULL )
	{
		if ( *s & 0x80 )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}


int HexString2Asc( char* pHex, char* pAsc, int HexLen, int AscLen )
{
	int i, j;
	int len;
	char data, temp;
	
	len = HexLen/2*2;
	if ( len > 2*AscLen ) len = 2*AscLen;
	if ( len <= 0 ) return 0;
	j = 0;
	
	for ( i = 0; i < len; i+=2 )
	{
		data = *(pHex+i);
		if ( ( data >= '0') && ( data <= '9' ) ) temp = data - '0';
		else if ( ( data >= 'a') && ( data <= 'f' ) ) temp = data - 'a' + 10;
		else if ( ( data >= 'A') && ( data <= 'F' ) ) temp = data - 'A' + 10;
		else break;
		
		data = *(pHex+i+1);
		if ( ( data >= '0') && ( data <= '9' ) ) temp = (temp << 4) + data - '0';
		else if ( ( data >= 'a') && ( data <= 'f' ) ) temp = (temp << 4) + data - 'a' + 10;
		else if ( ( data >= 'A') && ( data <= 'F' ) ) temp = (temp << 4) + data - 'A' + 10;
		else break;
		
		*(pAsc+i/2) = temp;
		j ++;
	}
	
	return j;
}


int Asc2HexString( char* pAsc, char* pHex, int AscLen, int HexLen )
{
	int i;
	int len;
	char high, low;
	
	len = HexLen/2;
	if ( len > AscLen ) len = AscLen;
	if ( len <= 0 ) return 0;
	
	for ( i = 0; i < len; i++ )
	{
		high = *(pAsc+i) >> 4;
		low = *(pAsc+i) & 0x0F;
		
		if ( high < 10 ) *(pHex+2*i) = '0'+high;
		else *(pHex+2*i) = 'A'-10+high;
		
		if ( low < 10 ) *(pHex+2*i+1) = '0'+low;
		else *(pHex+2*i+1) = 'A'-10+low;
	}
	
	return len*2;
}


// tea encrypt
void TeaEncrypt( uint32_t *key, uint32_t *val )
{
	int i;
	uint32_t y = val[0], z = val[1];
	uint32_t sum = 0;         /* set up */
	uint32_t delta = 0x9e3779b9;                 /* a key schedule constant */
	uint32_t a = key[0], b = key[1], c = key[2], d = key[3];   /* cache key */

	for ( i=0; i < 32; i++ )
	{
		sum += delta;
		y += ((z<<4) + a) ^ (z + sum) ^ ((z>>5) + b);
		z += ((y<<4) + c) ^ (y + sum) ^ ((y>>5) + d);/* end cycle */
	}

	val[0] = y;
	val[1] = z;
}


// tea decrypt
void TeaDecrypt( uint32_t *key, uint32_t *val )
{
	uint32_t y = val[0], z = val[1];
	uint32_t sum = 0xC6EF3720, i; /* set up */
	uint32_t delta = 0x9e3779b9;                  /* a key schedule constant */
	uint32_t a = key[0], b = key[1], c = key[2], d = key[3];    /* cache key */

	for(i=0; i<32; i++)
	{
		z -= ((y<<4) + c) ^ (y + sum) ^ ((y>>5) + d);
		y -= ((z<<4) + a) ^ (z + sum) ^ ((z>>5) + b);
		sum -= delta;
	}

	val[0] = y;
	val[1] = z;
}


void IccidExchage( uint8_t *code )
{
    int i;
    uint8_t temp;

    for ( i = 0; i < 10; i ++ )
    {
        temp = code[2*i];
        code[2*i] = code[2*i+1];
        code[2*i+1] = temp;
    }
}


int PostStrDecode( char *str )
{
	int i, cnt, len;
	int state = 0;
	char c;
	
	len = strlen(str);
	cnt = 0;

	for( i = 0; i < len; i ++ )
	{
		switch( state )
		{
			case 0:
			{
				if( str[i] == '%' )
				{
					state = 1;
				}
				else
				{
					str[cnt++] = str[i];
				}

				break;
			}

			case 1:
			{
				if ( ( str[i] >= '0') && ( str[i] <= '9' ) ) c = str[i] - '0';
				else if ( ( str[i] >= 'a') && ( str[i] <= 'f' ) ) c = str[i] - 'a' + 10;
				else if ( ( str[i] >= 'A') && ( str[i] <= 'F' ) ) c = str[i] - 'A' + 10;
				else c = 0;

				state ++;
				break;
			}

			case 2:
			{
				if ( ( str[i] >= '0') && ( str[i] <= '9' ) ) c = str[i] - '0' + c*16;
				else if ( ( str[i] >= 'a') && ( str[i] <= 'f' ) ) c = str[i] - 'a' + 10 + c*16;
				else if ( ( str[i] >= 'A') && ( str[i] <= 'F' ) ) c = str[i] - 'A' + 10 + c*16;
				else c = 0 + c*16;

				str[cnt++] = c;
				state = 0;
				break;
			}
		}
	}

	str[cnt] = '\0';

	return cnt;
}

//#include "system.h"
int shell_get(const char *shellcmd, char *buffer_out, unsigned int buffer_out_len)
{
	FILE * fp; 

	fp=popen(shellcmd, "r");
	if(fp)
	{
		memset(buffer_out, 0, buffer_out_len);
		fgets(buffer_out, buffer_out_len, fp);
		strtok(buffer_out, "\r\n");
		
		if(strlen(buffer_out) >= buffer_out_len)
			buffer_out[buffer_out_len] = 0;
		pclose(fp);
	}
	else
	{
		return -1;
	}

	return 0;
}

int shell_get_noent(const char *shellcmd, char *buffer_out, unsigned int buffer_out_len)
{
	FILE * fp; 
	unsigned int offset_len = 0;
	char *penter = 0;

	fp=popen(shellcmd, "r");
	if(fp)
	{
		memset(buffer_out, 0, buffer_out_len);
		while(fgets(buffer_out+offset_len, buffer_out_len-offset_len, fp) != NULL)
		{
			offset_len = strlen(buffer_out);
		}
		penter = strrchr(buffer_out, '\r');
		if(penter)
		{
			*penter = 0;
		}
		penter = strrchr(buffer_out, '\n');
		if(penter)
		{
			*penter = 0;
		}		
		if(strlen(buffer_out) >= buffer_out_len)
			buffer_out[buffer_out_len] = 0;
		
		pclose(fp);
	}
	else
	{
		return -1;
	}

	return 0;
}

static unsigned char
ConfigHexDigit(const char cDigit)
{
    if((cDigit >= '0') && (cDigit <= '9'))
    {
        return(cDigit - '0');
    }
    else
    {
        if((cDigit >= 'a') && (cDigit <= 'f'))
        {
            return((cDigit - 'a') + 10);
        }
        else
        {
            if((cDigit >= 'A') && (cDigit <= 'F'))
            {
                return((cDigit - 'A') + 10);
            }
        }
    }

    //
    // If we get here, we were passed an invalid hex digit so return 0xFF.
    //
    return(0xFF);
}


#define HTTPD_DEBUG 	1
//
int Write_file(char *filename, char *content, unsigned int writelen)
{

#if HTTPD_DEBUG

	int fd_dev = 0;
	
	fd_dev=open((const char *)filename, O_WRONLY|O_CREAT|O_APPEND);//O_TRUNC);
	if(fd_dev < 0)
	{
		printf("file open fail \n");
		return -1;
	}
	//write(fd_dev, "test cmd\r\n", strlen("test cmd\r\n"));
	write(fd_dev, content, strlen(content));
	write(fd_dev, "\r\n", strlen("\r\n"));
	
	close(fd_dev);

#endif
}


int swap(char * in, int len)
{
	int i = 0;
	int temp = 0;
	
	for(i=0; i<3; i++)
	{
		temp = in[i];
		in[i] = in[len-1-i];
		in[len-1-i] = temp;
	}
	return 0;
}

 
char *iphex2str(unsigned int ipaddr)
{
	static unsigned char ipstr[32] = {0};
	
	sprintf(ipstr, "%d.%d.%d.%d", (ipaddr)&0xff, (ipaddr >> 8) & 0xff, (ipaddr >> 16) & 0xff, (ipaddr >> 24) & 0xff);
	
	return ipstr;
}


//
// remove specific char in string
// 
char *str_delchar(char *input, char c)
{
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int len = 0;

	if(input == NULL)
	{
		return input;
	}
	
	len = strlen(input);
	for(i=0; i<len; i++)
	{
		if(input[i] == c)
		{
			
		}
		else
		{
			input[j++] = input[i];
		}
	}
	//input[j++]=0x02;
	//input[j++]=0x03;
	input[j] = 0;
	return input;
}

char *strupr(char *input)
{
	int i = 0;
	
	if(strlen(input) >= 10240)
		return 0;
	
	for(i=0; i<strlen(input); i++)
	{
		input[i] = toupper(input[i]);
	}
	return input;
}

char *strlwr(char *input)
{
	int i = 0;
	
	if(strlen(input) >= 10240)
		return 0;
	
	for(i=0; i<strlen(input); i++)
	{
		input[i] = tolower(input[i]);
	}
	return input;
}

