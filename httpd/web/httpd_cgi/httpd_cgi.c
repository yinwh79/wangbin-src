/*
 * Copyright (c) 2007 Denys Vlasenko <vda.linux@googlemail.com>
 *
 * Licensed under GPLv2, see file LICENSE in this source tree.
 */

/*
 * This program is a CGI application. It outputs directory index page.
 * Put it into cgi-bin/index.cgi and chmod 0755.
 */

/* Build a-la
i486-linux-uclibc-gcc \
-static -static-libgcc \
-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
-Wall -Wshadow -Wwrite-strings -Wundef -Wstrict-prototypes -Werror \
-Wold-style-definition -Wdeclaration-after-statement -Wno-pointer-sign \
-Wmissing-prototypes -Wmissing-declarations \
-Os -fno-builtin-strlen -finline-limit=0 -fomit-frame-pointer \
-ffunction-sections -fdata-sections -fno-guess-branch-probability \
-funsigned-char \
-falign-functions=1 -falign-jumps=1 -falign-labels=1 -falign-loops=1 \
-march=i386 -mpreferred-stack-boundary=2 \
-Wl,-Map -Wl,link.map -Wl,--warn-common -Wl,--sort-common -Wl,--gc-sections \
httpd_indexcgi.c -o index.cgi
*/

/* We don't use printf, as it pulls in >12 kb of code from uclibc (i386). */
/* Currently malloc machinery is the biggest part of libc we pull in. */
/* We have only one realloc and one strdup, any idea how to do without? */

/* Size (i386, static uclibc, approximate):
 *   text    data     bss     dec     hex filename
 *  13036      44    3052   16132    3f04 index.cgi
 *   2576       4    2048    4628    1214 index.cgi.o
 */

/*
HTTP/1.0 200 OK
Content-type: text/html
Date: Fri, 02 Jan 1970 17:05:04 GMT
Connection: close
Accept-Ranges: bytes
Last-Modified: Fri, 02 Jan 1970 02:09:06 GMT
Content-length: 2747

*/

#define _GNU_SOURCE 1  /* for strchrnul */

#include "param.h"
#include "usr_define.h"
#include <syslog.h>


#define HTTP_SUCCESS 0
#define HTTP_FAIL 1


#define INSTEAD_SUM 20
#define INSTEAD_HEAD_FLAG "<!--#"
#define INSTEAD_TAIL_FLAG "-->"

#define HTML_HEAD	"Content-Type: text/html\r\n" \
					"Connection: close\r\n"
#define JSON_HEAD	"Content-Type: application/json\r\n" \
					"Connection: close\r\n"

#define DEFAULT_INDEX_PATH 		"/www/index"
#define UPGRADE_FIRM_SUCC_PATH 	"/www/upconfirm.html"
#define UPGRADE_FIRM_FAIL_PATH 	"/www/upgrade.html"

#define ROOT_PATH 			"/www/"
#define FIRM_PATH 			"/tmp/vmlinux.ub"
#define FIRM_UPGRADE() 		system("echo FIRM_UPGRADE > /tmp/upgrade_log; echo 1 > /etc/reload_flag;flashcp /tmp/vmlinux.ub /dev/mtd3; reboot -f");
#define FIRM_RELOAD()		system("echo FIRM_RELOAD > /tmp/upgrade_log;/etc/reset_param");  

#define FIRM_QUIT() 		system("rm -rf /tmp/vmlinux.ub") 
#define FIRM_REMOVE() 		system("rm -rf /tmp/vmlinux.ub")

#define SRC_FILE_HEAD 	"src_file="

#define rsp_html1 "<html><body>123</body></html>"

static BOOL GetHtmlWithSSI( char *file_path, char *dst_buf );
static BOOL SaveNewFirmware( char *data, int len );

static char html_buf[HTML_BUF_SIZE];
static char http_buf[HTML_BUF_SIZE];

//
// get shell cmd return into buffer
// 
#include "system.h"
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
		if(buffer_out[offset_len-2]=='\r')
		{
			buffer_out[offset_len-2] = '\0';
		}
		if(buffer_out[offset_len-1]=='\n')
		{
			buffer_out[offset_len-1] = '\0';
		}
			
		/*
		penter = strrchr(buffer_out, '\n');
		if(penter)
		{
			*penter = 0;
		}		
		
		penter = strrchr(buffer_out, '\r');
		if(penter)
		{
			*penter = 0;
		}
		*/
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


BOOL GetHtmlWithSSI( char *file_path, char *dst_buf )
{
	FILE *fp;
	int i, cnt, instead_cnt, rsp_para_num;
	char *instead_head[INSTEAD_SUM];
	char *instead_tail[INSTEAD_SUM];
	char *new_head, *ihead, *itail, *data, *ques;
	char cmd_all[256];
	char rsp_para[256];
	char line_buf[KB];
	char *pPara, *para_str;
	char ssi_data[500] = {0};
	
	fp = fopen( file_path, "r" );
	if (!fp) return FALSE;
	
	while( fgets(line_buf, sizeof(line_buf), fp) )
	{
		cnt = 0;
		instead_cnt = 0;
		new_head = line_buf;

		//check the sum of instead
		for( i = 0; i < INSTEAD_SUM; i ++ )
		{
			ihead = strstr(new_head, INSTEAD_HEAD_FLAG);

			if( ihead )
			{
				itail = strstr(ihead, INSTEAD_TAIL_FLAG);
			}

			if ( ihead && itail && (ihead+5 < itail) ) //find one instead
			{
				instead_head[instead_cnt] = ihead;
				instead_tail[instead_cnt] = itail;
				instead_cnt ++;
				new_head = itail + 3;
			}
			else
			{
				break;
			}
		}

		if( instead_cnt > 0 )
		{
			for( i = 0; i < instead_cnt; i ++ )
			{
				*instead_head[i] = '\0';
				*instead_tail[i] = '\0';
			}
			
			
			for( i = 0; i < instead_cnt; i ++ )
			{
				if( i == 0 )
				{
					data = line_buf;
				}
				else
				{
					data = instead_tail[i-1] + 3;
				}
				
				strcat( dst_buf, data );

				strcpy( cmd_all, instead_head[i]+5 );

				ques = strstr( cmd_all, "uci get" );
				
				if( ques )
				{
					shell_get_noent(ques, ssi_data, sizeof(ssi_data));

					strcat( dst_buf, ssi_data );
				}
				else
				{
					ques = strstr( cmd_all, "shellcmd:" );
					if(ques)
					{
						ques += strlen("shellcmd:");
					}
					shell_get_noent(ques, ssi_data, sizeof(ssi_data));
					strcat( dst_buf, ssi_data );
					
					strcat( cmd_all, "\r" );
				}
			}

			if( *(instead_tail[instead_cnt-1]+3) != 0 ) strcat( dst_buf, instead_tail[instead_cnt-1]+3 );
		}
		else
		{
			strcat( dst_buf, line_buf );
		}
	}
	
	
	fclose(fp);
	return TRUE;
}

int uci_httpd_convert(char * uci_head, char *param)
{
	char uci_buf[128] = {0};
	char *pfind = 0;
	char *pfind_hdr = 0;
	//char uci_param_hdr[50] = {0};
	int param_len = 0;
	int i = 0;
	char cmd_buf[500] = {0};

	// start converting 
	param_len = strlen(param);
	pfind = param;
	pfind_hdr = param;
	
	for( i = 0; i < param_len; i ++ )
	{
		if( (pfind[i] == '&') ) 
		{
			pfind[i] = '\0';

			// form uci set command string
			cgi2uci(pfind_hdr, uci_buf, sizeof(uci_buf));
			
			memset(cmd_buf, 0, sizeof(cmd_buf));
			
			sprintf(cmd_buf, "uci set -q %s.%s", uci_head, uci_buf);
			syslog(LOG_INFO, "uci_httpd_convert: %s \n", cmd_buf);
			
			system(cmd_buf);
			
			pfind_hdr = pfind + i + 1 ;
		}
		// last param
		else if (i == (param_len-1))
		{
			// form uci set command string
			cgi2uci(pfind_hdr, uci_buf, sizeof(uci_buf));
			
			memset(cmd_buf, 0, sizeof(cmd_buf));			
			sprintf(cmd_buf, "uci set -q %s.%s", uci_head, uci_buf);
			
			system(cmd_buf);
			
			pfind_hdr = pfind + i + 1 ;
		}
		// maybe last param
		else
		{
			
		}
	}
	system("uci commit");

	return 0;
	
}

#define TEXTAREA_MAX_LEN 2048
#define TEXTAREA_MAX_ROW	10

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

// cgi string to internal
int cgi_string_convert(char *input, char *out, int outsize)
{
	int i = 0;
	int k = 0; 
	int string_len = 0;
	int first_eq = 0;
	
	string_len = strlen(input);
	if(string_len >= outsize)
	{
		return -1;
	}

	for(i=0,k=0; (i<string_len)&&(input[i]!=0)&&(k<outsize); )
	{
		if(input[i]=='%')
		{
			out[k] = ConfigHexDigit(input[i+1])<<4;
			out[k] += ConfigHexDigit(input[i+2]);
			i += 3;
		}
		else if(input[i]=='+')
		{
			out[k] = ' ';
			i++;
		}
		else
		{
			out[k] = input[i];
			i++;
		}
		k++;
	}
	out[k] = 0;
	
	return 0;
}


// cgi string to internal
int cgi2uci(char *input, char *out, int outsize)
{
	int i = 0;
	int k = 0; 
	int string_len = 0;
	int first_eq = 0;
	
	string_len = strlen(input);
	if(string_len >= outsize)
	{
		return -1;
	}

	for(i=0,k=0; (i<string_len)&&(input[i]!=0)&&(k<outsize); )
	{
		if((input[i]=='=') && (first_eq==0))
		{
			first_eq = 1;
			out[k++] = input[i];			
			out[k] = '\'';
			i++;
		}
		else if(input[i]=='%')
		{
			out[k] = ConfigHexDigit(input[i+1])<<4;
			out[k] += ConfigHexDigit(input[i+2]);
			i += 3;
		}
		else if(input[i]=='+')
		{
			out[k] = ' ';
			i++;
		}
		else
		{
			out[k] = input[i];
			i++;
		}
		k++;
	}
	out[k++] = '\'';
	out[k] = 0;
	
	return 0;
}


// httpd string transform
int textarea_transform(char *inputstring, char * writefile_head, int row)
{
	char rep[TEXTAREA_MAX_LEN] = {0};
	char rep_convert[TEXTAREA_MAX_LEN] = {0};
	int fd_file = 0;	
	char shell_ret[512] = {0};

	cgi_string_convert(inputstring, rep, sizeof(rep));
	
	// write content into filename
	fd_file=open((const char *)writefile_head, O_WRONLY|O_CREAT|O_APPEND | O_TRUNC);
	if(fd_file < 0)
	{
		printf("file open fail \n");
		return -1;
	}
	
	write(fd_file, rep, strlen(rep));
	close(fd_file); 	
		
	// exec shell cmd
	if(strstr(writefile_head, "/tmp/diagnostics.input") && (row==0))
	{
		strtok(rep, "\r");
		
		// if string started with "ping"
		if((strncasecmp(rep, "ping", strlen("ping"))==0)
				|| (strncasecmp(rep, "cat", strlen("cat"))==0))
		{
			shell_get_noent(rep, shell_ret, sizeof(shell_ret));
		
			// write content into filename
			fd_file=open((const char *)"/tmp/diagnostics.ret", O_WRONLY|O_CREAT|O_APPEND | O_TRUNC);
			if(fd_file < 0)
			{
				printf("file open fail \n");
				return -1;
			}
			
			write(fd_file, shell_ret, strlen(shell_ret));
			
			close(fd_file); 	
		}
	}
	return 0;
}

// get file content from html
int writefile_httpd_convert(char * writefile_head, char *param)
{
	char *pfind = 0;
	char *pfind_hdr = 0;
	unsigned int param_len = 0;
	unsigned int i = 0;
	char *peq = 0;
	int row = 0;

	// start converting 
	param_len = strlen(param);
	pfind = param;
	pfind_hdr = param;

	for( i = 0; i < param_len; i ++ )
	{
		if( (pfind[i] == '&' ) || (i == (param_len-1)))
		{
			if(pfind[i] == '&')
			{
				pfind[i] = '\0';
			}
			// transform the http string 
			// "+" -> " " ; "%xx" -> "c"; 
			peq = strstr(pfind_hdr, "=");
			if(peq)
			{
				peq += 1;
				textarea_transform(peq, writefile_head, row);
				row++;
			}
			
			pfind_hdr = pfind + i + 1 ;
		}
		
	}

	return 0;
	
}

//decode and process data
int DecodeAndProcessData(char *uri, char *input)
{
	int i;
	char src_file[64];
	char path[256];
	char *pSrcFile;
	char *uci_hdr = 0;
	char uri_real[100] = {0};
	char shell_ret[256] = {0};
	char cgi_string[64] = {0};
	char cgi_string_temp[64] = {0};

	if( strstr(input, ".json") )
	{
		// transform string 
		cgi_string_convert(input, shell_ret, sizeof(shell_ret));
		//
		pSrcFile = strstr( shell_ret, SRC_FILE_HEAD );
		if(pSrcFile)
		{
			pSrcFile += strlen(SRC_FILE_HEAD);
		}
		else 
			return HTTP_FAIL;
		for(i=0; i<strlen(pSrcFile); i++)
		{
			if(pSrcFile[i] == '&')
			{
				pSrcFile[i] = '\0';
				break;
			}
		}
		strcpy( src_file, pSrcFile );
		memset(path, 0, sizeof(path));
		strcpy( path, ROOT_PATH );
		strcat( path, src_file);

		if ( !GetHtmlWithSSI( path, html_buf ) ) return HTTP_FAIL;
	
		sprintf( http_buf, "%sContent-Length: %d\r\n\r\n", JSON_HEAD, strlen(html_buf) );
		strcat( http_buf, html_buf );
		fputs( http_buf, stdout );
		
		return HTTP_SUCCESS; 
	}

	// get real file path and save to uri_real, remove char '?'
	strcpy(uri_real, uri);
	strtok(uri_real, "?");
	strcpy( path, ROOT_PATH );
	strcat( path, uri_real);

	//get uci header after html path
	uci_hdr = strstr(uri, "?");
	if(uci_hdr)
	{
		uci_hdr = uci_hdr + 1;
	}
	else
	{
		return HTTP_FAIL;
	}

	// if write filepath
	if(uci_hdr[0] == '/')
	{ 
		writefile_httpd_convert(uci_hdr, input);
	}
	// run shell cmd
	else if(uci_hdr[0] == '~')
	{
		memset(cgi_string_temp, 0, sizeof(cgi_string_temp));
		memcpy(cgi_string_temp, uci_hdr+1, sizeof(cgi_string_temp)-1);

		memset(cgi_string, 0, sizeof(cgi_string));		
		cgi_string_convert(cgi_string_temp, cgi_string, sizeof(cgi_string));
 
		sprintf(cgi_string_temp, "echo 2-%s >> /tmp/httpd_1", cgi_string);
		system(cgi_string_temp);
 
		//shell_get(cgi_string, shell_ret, sizeof(shell_ret));
		system(cgi_string);
		sprintf(cgi_string, "echo 3-%s >> /tmp/httpd_1", cgi_string); 
		system(cgi_string);
	} 
	else
	{
		// get input and form uci commands
		uci_httpd_convert(uci_hdr, input);
	}
		
	if ( !GetHtmlWithSSI( path, html_buf ) ) return HTTP_FAIL;

	sprintf( http_buf, "%sContent-Length: %d\r\n\r\n", HTML_HEAD, strlen(html_buf) );	
	strcat( http_buf, html_buf );
	fputs( http_buf, stdout );
	
	return HTTP_SUCCESS;
}


BOOL SaveNewFirmware( char *data, int len )
{
	int fd_firm, ret;

	FIRM_REMOVE();
	
	fd_firm = open( FIRM_PATH, O_WRONLY|O_CREAT|O_TRUNC );
	
	if( fd_firm > 0 )
	{
	    ret = write( fd_firm , data, len );

		if( ret == len )
		{
			return TRUE;
		}
		else
		{
			FIRM_QUIT();
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

int httpd_get_key(char *in, char *key, char *val)
{
	char *p = 0;
	char *peq = 0;
	int val_len = 0;
	char *pand = 0;

	p = strstr(in, "/cgi-bin/httpd.cgi");
	if(p == NULL) return -1;

	p = strstr(in, "?");
	if(p == NULL) return -1;
	
	p = strstr(p, key);
	if(p == NULL) return -2;
	
	pand = strstr(p, "&");
	
	peq = strstr(p, "=");
	if(peq == NULL) return -3;
	val_len = pand - peq - 1;

	strncpy((char *)val, (char *)peq+1, val_len);
	
	return 0;
}
char unixstr[32] = {0};

int settime(char * unixstr)
{
	char timestr[128] = {0};
	char ret[128] = {0};
	
	sprintf(timestr, "date -d @%s \"+%%Y-%%m-%%d %%H:%%M:%%S\"", unixstr);
	shell_get(timestr, ret, sizeof(ret));
	
	memset(timestr, 0, sizeof(timestr));
	sprintf(timestr, "date -s \"%s\"", ret);
	shell_get(timestr, ret, sizeof(ret));
	syslog(LOG_INFO, "settime cmd: %s \n ret :%s \n", timestr, ret);
	
	return 0;
}

char debug_buf[5196] = {0};

int main(int argc, char *argv[])
{
	int i, x, len;
	char *uri;
	char *pRequestMethod;
	char *pContentLength;
	char *pContentType;
	char *pName, *pLen, *pEnd, *pLang, *pReload;
	int ContentLength;
	char path[256];
	char test[100] = {0};
	char *InputBuffer = 0;
	int ret = 0;
	
	pName = NULL;
	pLang = NULL;
	pLen = NULL;
	pEnd = NULL;

	system("rm -rf /tmp/httpdcgi_log");
	
	Param_Init(); //read parameters
	
	pRequestMethod = getenv( "REQUEST_METHOD" );
	
	uri = getenv("REQUEST_URI");	
	sprintf(debug_buf,"\n argc=%d, argv[0]=%s \n arg[1]=%s REQUEST_METHOD: %s, REQUEST_URI: %s\n", 
					argc, argv[0], argv[1], pRequestMethod, uri);

	//syslog(LOG_INFO, debug_buf);

	if ( argc <= 0 ) return HTTP_FAIL; //something wrong

	if ( pRequestMethod )
	{
		if ( strcasecmp(pRequestMethod,"GET") == 0 )
		{
			// get key val == settime
			//syslog(LOG_INFO, "GET :%s \n", uri);
			ret = httpd_get_key(uri, "settime", unixstr);
			//syslog(LOG_INFO, "settime :%s, %d - %s \n", uri, ret, unixstr);			
			if(ret == 0)
			{
				settime(unixstr);
			}
			
			uri = getenv("REQUEST_URI");
			if (!uri) return HTTP_FAIL;
			if ( (argc < 2) || !argv[1] ) return HTTP_FAIL; //something wrong
			
			sprintf( path, "%s%s", ROOT_PATH, argv[1] );

			if( !GetHtmlWithSSI( path, html_buf ) )
			{
				return HTTP_FAIL; //get file error
			}
			
			
			sprintf( http_buf, "%sContent-Length: %d\r\n\r\n", HTML_HEAD, strlen(html_buf) );
			strcat( http_buf, html_buf );
			fputs( http_buf, stdout );
							
			return HTTP_SUCCESS;
		}
		else if ( strcasecmp(pRequestMethod,"POST") == 0 )
		{
			pContentLength = getenv( "CONTENT_LENGTH" ); //get CONTENT_LENGTH from env
			
			if ( pContentLength != NULL )
			{
				ContentLength = atoi( pContentLength );
			}
			else
			{
				ContentLength = 0;
				return HTTP_FAIL;
			}
			
			if ( ContentLength > FIRMWARE_SIZE )
			{
				ContentLength = FIRMWARE_SIZE - 1;
			}

			//get form data from stdin
			i = 0;

			InputBuffer = malloc(ContentLength);
			if(InputBuffer == 0)
			{
				return HTTP_FAIL;
			}

			while ( i < ContentLength )
			{    
				x = fgetc(stdin);
				if ( x == EOF ) break;
				InputBuffer[i++] = x;
			}

			InputBuffer[i] = '\0';
			ContentLength = i;

			memset(test, 0, sizeof(test));
			sprintf(test, "ContentLength: %d \n", ContentLength);

			//check content type
			pContentType = getenv( "CONTENT_TYPE" );
			
			if ( strstr(pContentType,"multipart/form-data") )
			{
				pReload = strstr(InputBuffer, "saveconfig" );
				if(pReload)
				{
					pReload = strstr( pReload, "\r\n\r\n" );
					pReload += 4;
					if(memcmp(pReload, "on\r\n", strlen("on\r\n"))==0)
					{
					}
					else{
						system("echo 1 > /mnt/user/reload_flag");
					}
				}
				else
				{
						system("echo 1 > /mnt/user/reload_flag");
				}
				
				pName = strstr( InputBuffer, "octet-stream" );
				
				if( pName )
				{
					
					//
					pName = strstr( pName, "\r\n\r\n" );
					pName += 4;

					len = ContentLength - (pName-InputBuffer);
					if((len >= FIRMWARE_SIZE) || (len <= 1024))
					{
						free(InputBuffer);
						
						return HTTP_FAIL;
					}
					
					memset(test, 0, sizeof(test));
					sprintf(test, "content_length: %d \n", len);
					
					if( len )
					{
						if ( SaveNewFirmware( pName, len ) )
						{
							sprintf( path, "%s%s", ROOT_PATH, argv[1] );							
							//sprintf( path, "%s", UPGRADE_FIRM_SUCC_PATH);
							GetHtmlWithSSI( path, html_buf );
						}
						else
						{
							sprintf( path, "%s", UPGRADE_FIRM_FAIL_PATH);
							GetHtmlWithSSI( path, html_buf );
						}
					}
					else
					{
						sprintf( path, "%s", UPGRADE_FIRM_FAIL_PATH);
						GetHtmlWithSSI( path, html_buf );
					}
					
					sprintf( http_buf, "%sContent-Length: %d\r\n\r\n", HTML_HEAD, strlen(html_buf) );
					strcat( http_buf, html_buf );
					fputs( http_buf, stdout );
				}
				else
				{
					free(InputBuffer);
				
					return HTTP_FAIL;
				}
				
			}
			else
			{				
				DecodeAndProcessData(uri,  InputBuffer );
				free(InputBuffer);
				
				return HTTP_SUCCESS;
				
			}
		}
		else
		{
			free(InputBuffer);
		
			return HTTP_FAIL;
		}
	}

	free(InputBuffer);

	return HTTP_SUCCESS;
}
