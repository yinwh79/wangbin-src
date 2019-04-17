/***********************************************************************************
Copy right:		2015-2025 USR IOT Tech.
Author:			Wangbin
Version:		V1.0
Date:			2015-09
Description:	g_AtCmd thread.
***********************************************************************************/
#ifndef	__AT_CMD_H
#define	__AT_CMD_H


#ifdef	__AT_CMD_C
#define	AT_CMD_EXT
#else
#define	AT_CMD_EXT extern
#endif


#include "usr_define.h"


#define ACK_OK "\r\n+OK\r\n"
#define ACK_ERR1 "\r\n+ERR:1\r\n"
#define ACK_ERR2 "\r\n+ERR:2\r\n"
#define ACK_ERR3 "\r\n+ERR:3\r\n"
#define ACK_ERR4 "\r\n+ERR:4\r\n"
#define ACK_ERR_DBG "\r\n+ERR:debug\r\n"


typedef struct
{
	AT_CMD_RSP_TYPE cmd_rsp_type;
	BOOL bCmdReady;
    char CmdBuf[CMD_BUF_SIZE];
    uint16_t CmdCnt;
} _AT_CMD;


typedef struct
{
	char* para_str; // parameter for command
	uint8_t para_sum; // sum of parameters
	AT_CMD_TYPE type;  // command type
	int portnum;
} AT_CMD_PARA;


typedef void (*pfn_AtCmdHandler) (AT_CMD_PARA * pParam);


typedef struct
{
	const char* pName; // command name
	const pfn_AtCmdHandler pFunc; // function point.
	const char* pInfo; // command discription
} COMMAND_TABLE_T;



AT_CMD_EXT _AT_CMD g_AtCmd;
AT_CMD_EXT char g_MEIDCode[21];
AT_CMD_EXT char g_ICCIDCode[21];
AT_CMD_EXT char g_InnerRspBuf[11*KB];




/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
AT_CMD_EXT void AtCmd_Init( void );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
AT_CMD_EXT void AtCmd_Dispatch( char* at_cmd_str);//, AT_CMD_RSP_TYPE rsp_type );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
AT_CMD_EXT BOOL AtCmd_MatchCmdName( char* cmd_str, char* cmd_name );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
AT_CMD_EXT AT_CMD_TYPE AtCmd_GetCmdType(char* cmd_str);


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
AT_CMD_EXT int AtCmd_GetParaSum(char* cmd_str);


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
AT_CMD_EXT BOOL AtCmd_GetPara( int index, char* para, void* val, int val_len, PARA_TYPE type );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
AT_CMD_EXT BOOL AtCmd_Respond( char* rsp );


extern char *AtCmd_getrsp_ptr(void);
extern int echo_settemp(int val);


#endif
