/***********************************************************************************
Copy right:	2014-2024 USR IOT Tech.
Author:			Bin Wang
Version:			V1.0
Date:				2014-08
Description:	g_AtCmd thread.
***********************************************************************************/
#define __AT_CMD_C

#include "at_cmd.h"
#include "at_cmd_table.h"
#include "param.h"

unsigned int echoon = 0;

void AtCmd_Init( void )
{
	char buffer[16] = {0};
	
	memset( &g_AtCmd, 0, sizeof(g_AtCmd) );
	// echo is on ?
	shell_get("uci get system.@system[0].echo_on -q", buffer, sizeof(buffer));
	echoon = atoi(buffer);
	//printf("at cmd echo_on: %d\n", echoon);
}

int echo_settemp(int val)
{
	echoon = val;
	return echoon;
}

void AtCmd_Dispatch( char* at_cmd_str)//, AT_CMD_RSP_TYPE rsp_type )
{
	int i, index = 0;
	pfn_AtCmdHandler pfn = NULL;
	AT_CMD_PARA para;
	char* pPara;
	char rsp_buf[1024];
	char cmd_buf[1024];
	int cmd_cnt = 0;
	char portnum[3] = {0};

	AtCmd_Init();
	
	memset(g_InnerRspBuf, 0, sizeof(g_InnerRspBuf));
	
	if(strlen( at_cmd_str ) >= sizeof(cmd_buf))
		return ;
	
	for ( i = 0; i < strlen( at_cmd_str ); i ++ )
	{
		if( (at_cmd_str[i] == ';') || (at_cmd_str[i] == '\r') || (at_cmd_str[i] == '\n') )
		{
			if( cmd_cnt == 0 )
			{
				index = 0;
				continue;
			}
			
			cmd_buf[cmd_cnt++] = '\r';
			cmd_buf[cmd_cnt++] = '\0';

			// add echo string
			if(echoon) 
			{
				strcat(g_InnerRspBuf, cmd_buf);
				//printf("g_InnerRspBuf:%s \n", g_InnerRspBuf);
				//printf("at_cmd_str:%s \n", cmd_buf);
			}
			
			//g_AtCmd.cmd_rsp_type = rsp_type;

			if( ( strlen( cmd_buf ) < 3 ) || ( 'A' != toupper( *cmd_buf ) ) || ( 'T' != toupper( *(cmd_buf+1) ) ) )
			{
				sprintf( rsp_buf, ACK_ERR1 );
				AtCmd_Respond( rsp_buf );
				cmd_cnt = 0;
				index = 0;
				continue;
			}
			
			while ( g_CommandTable[index].pName != NULL )
			{
				if ( AtCmd_MatchCmdName( (char*)cmd_buf, (char*)g_CommandTable[index].pName) )
				{
					pfn = g_CommandTable[index].pFunc; // find the right command
					break;
				}
				index ++;
			}
			
			if ( pfn != NULL )
			{
				// get command type
				para.type = AtCmd_GetCmdType( cmd_buf );
				para.para_sum = AtCmd_GetParaSum( cmd_buf );
				pPara = strchr( cmd_buf, '=' );
				
				if( pPara )
				{
					para.para_str = pPara+1;
					// find which commport
					portnum[0] = *(pPara-2);					
					portnum[1] = *(pPara-1);
					portnum[2] = 0;
					if((portnum[0] <= '9') && (portnum[0] >= '0'))
					{
						para.portnum = atoi(portnum)>=1 ? atoi(portnum): 1;					
					}
					else
					{
						para.portnum = atoi(portnum+1)>=1 ? atoi(portnum+1): 1;					
					}
				}
				else
				{
					para.para_str = NULL;
					// find which commport
					pPara = strchr( cmd_buf, '\r' );
					if(pPara)
					{ 
						para.portnum = atoi(pPara-1);
						if((para.portnum==0) || (para.portnum>MAX_PORT))
						{
							para.portnum = atoi(pPara-2);
							if((para.portnum==0) || (para.portnum>MAX_PORT))
							{
								para.portnum = 1;
							}							
						}
					}
				}
				//printf("at portnum: %d \n",  para.portnum);
				
				(*pfn)( &para );
			}
			else
			{
				sprintf( rsp_buf, ACK_ERR2 );
				AtCmd_Respond( rsp_buf );
			}

			cmd_cnt = 0;
			index = 0;
		}
		else
		{
			cmd_buf[cmd_cnt++] = at_cmd_str[i];
		}
	}
}


BOOL AtCmd_MatchCmdName( char* at_cmd_str, char* cmd_name )
{
	int i, cmd_str_len, cmd_name_len;
	char* pEqual;
	char* pQues;
	char* pCR;
	char* pLF;
	char *pEnd;
	
	cmd_name_len = strlen( cmd_name );
	pEqual = strchr( at_cmd_str, '=' );
	pQues = strchr( at_cmd_str, '?' );
	pCR = strchr( at_cmd_str, '\r');
	pLF = strchr( at_cmd_str, '\n');
	if( !pCR && !pLF ) return FALSE;
	
	if ( !pCR ) pEnd = pLF;
	else if ( !pLF ) pEnd = pCR;
	else pEnd = ( pCR < pLF )?pCR:pLF;
	
	if ( pEqual )
	{
		cmd_str_len = (int)( pEqual - at_cmd_str );
	}
	else if ( pQues )
	{
		cmd_str_len = (int)( pQues - at_cmd_str );
	}
	else
	{
		cmd_str_len = (int)( pEnd - at_cmd_str );
	}
	
	if( cmd_name_len != cmd_str_len ) return FALSE;
	
	for ( i = 0; i < cmd_name_len; i ++ )
	{
		if ( toupper( *(at_cmd_str+i) ) != toupper( *(cmd_name+i) ) )
		{
			return FALSE;
		}
	}
	
	return TRUE;
}


AT_CMD_TYPE AtCmd_GetCmdType(char* at_cmd_str)
{
	char *pEnd = NULL;
	int len = strlen( at_cmd_str );
	char* pCR = strchr( at_cmd_str, '\r');
	char* pLF = strchr( at_cmd_str, '\n');
	
	if( len < 3 || ( !pCR && !pLF ) ) return AT_CMD_ERR;
	
	if ( !pCR ) pEnd = pLF;
	else if ( !pLF ) pEnd = pCR;
	else pEnd = ( pCR < pLF )?pCR:pLF;
	
	/* 
	if( *(pEnd-1) == '?' )
	{
		if ( *(pEnd-2) == '=' )
		{
			return AT_CMD_TEST;
		}
		else
		{
			return AT_CMD_READ;
		}
	} 
	else 
	{
		if ( strchr( at_cmd_str, '=' ) != NULL )
		{
			return AT_CMD_SET;
		}
		else
		{ 
			return AT_CMD_EXE;
		}
	}
	*/
	if ( strchr( at_cmd_str, '=' ) != NULL )
	{
		return AT_CMD_SET;
	}
	else
	{
		return AT_CMD_READ;
	}

}


int AtCmd_GetParaSum(char* at_cmd_str)
{
	int sum = 0;
	char* p = NULL;
	
	p = strchr( at_cmd_str, '=' );
	if( !p ) return 0;
	p ++;
	
	while( p )
	{
		if( ( *p == ',' ) || ( *p == '\r' ) || ( *p == '\n' ) ) sum ++;
		if( ( *p == '\r' ) || ( *p == '\n' ) ) break;
		p++;
	}
	
	return sum;
}


BOOL AtCmd_GetPara( int index, char* para, void* val, int val_len, PARA_TYPE type )
{
	int32_t num32;
	int i, len, pos;
	char *p[AT_CMD_MAX_PARA_SUM];
	int para_len[AT_CMD_MAX_PARA_SUM];
	char data[11];
	
	pos = 0;
	len = strlen( para );
	
	if ( !para || !len || ( index < 1 ) || ( index > AT_CMD_MAX_PARA_SUM ) ) return FALSE; //no parameters
	for ( i = 0; i < AT_CMD_MAX_PARA_SUM; i ++ )	p[i] = NULL; // clean
	len = 0;
	p[pos] = para;
	
	for ( i = 0; i < (int)strlen( para ); i ++ )
	{
		if ( *(para+i) == ',' )
		{
			para_len[pos++] = len;
			p[pos] = para+i+1;
			len = 0;
		}
		else if ( ( *(para+i) == '\r' ) || ( *(para+i) == '\n' ) )
		{
			para_len[pos] = len;
			break;
		}
		else
		{
			len ++;
		}
	}
	
	i = index - 1;
	if ( p[i] != NULL )
	{
		switch( type )
		{
			case PARA_TYPE_UINT8:
			{
				if ( para_len[i] > 3 ) return FALSE;
				memcpy( data, p[i] , para_len[i] );
				data[para_len[i]] = '\0';
				num32 = atoi_uint32( data );
				if ( (num32 < 0) || ( num32 > 255 ) ) return FALSE;
				*(( uint8_t *)val) = (uint8_t)num32;
				
				return TRUE;
			}
			
			case PARA_TYPE_UINT16:
			{
				if ( para_len[i] > 5 ) return FALSE;
				memcpy( data, p[i] , para_len[i] );
				data[para_len[i]] = '\0';
				num32 = atoi_uint32( data );
				if ( (num32 < 0) || ( num32 > 65535 ) ) return FALSE;
				*(( uint16_t *)val) = (uint16_t)num32;
				return TRUE;
			}
			
			case PARA_TYPE_UINT32:
			{
				if ( para_len[i] > 10 ) return FALSE;
				memcpy( data, p[i] , para_len[i] );
				data[para_len[i]] = '\0';
				*(( uint32_t *)val) = atoi_uint32( data );
				return TRUE;
			}
			
			case PARA_TYPE_STRING:
			{
				if ( (para_len[i]+1) > val_len ) return FALSE;
				memcpy( (char*)val, p[i], para_len[i] );
				*( (char*)val + para_len[i] ) = '\0';

				return TRUE;
			}
			
			default:
			return FALSE;
		}
	}
	else return FALSE;
}


BOOL AtCmd_Respond( char* rsp )
{
	strcat( g_InnerRspBuf, rsp );
	
	return TRUE;
}

char *AtCmd_getrsp_ptr(void)
{
	return g_InnerRspBuf;
}

