/***********************************************************************************
Copy right:		2015-2025 USR IOT Tech.
Author:			Wangbin
Version:		V1.0
Date:			2015-09
Description:	save parameters.
***********************************************************************************/
#ifndef	__PARAM_H
#define	__PARAM_H


#ifdef	__PARAM_C
#define	PARAM_EXT
#else
#define	PARAM_EXT extern
#endif


#include "public.h"
//#include "at_cmd.h"
#include "usr_define.h"

#define FILE_USR_FACTORY_CFG    "/mnt/user/cfg/usr_factory.cfg"
#define FILE_USER_RUN_CFG       "/mnt/user/cfg/user_run.cfg"
#define FILE_USER_DEFAULT_CFG   "/mnt/user/cfg/user_default.cfg"
#define FILE_CONST_PARA         "/mnt/user/cfg/const_para.cfg"
#define FILE_TEMP_CFG           "/mnt/user/cfg/temp.cfg"


#define MAX_CON_PARA_SIZE       256
#define MAX_USR_PARA_SIZE       (2*KB)



//uart parameters
typedef struct
{
	uint32_t			baud_rate; // baud rate
	UART_PARITY_TYPE	parity_type; // parity checking
	UART_DATA_BITS		data_bits; // data bits
	UART_STOP_BITS		stop_bits; // stop bits
	UART_FLOW_TYPE	    flow_type; // flow control type
} SYS_PARAM_UART;


//socket parameters
typedef struct
{
	BOOL				enable;		// enable socket
	SOCK_TYPE			type;			// TCP or UDP
	TCP_LINK_TYPE		link_type;	// long link or short link
	char				addr[SOCK_MAX_ADDR_LEN+1]; //server address
	uint16_t			port; //remote port
	uint32_t			timeout; // short link timeout
}SYS_PARAM_SOCK;


//HTTPD CLIENT;
typedef struct
{
	HTTP_TYPE	type; // http request type, GET or POST
	char				head[HTTP_MAX_HEAD_LEN+1];
	char				url[HTTP_MAX_URL_LEN+1]; // http request URL
	char				server_addr[HTTP_MAX_ADDR_LEN+1]; //http server address
	uint16_t		server_port; //http server port
	uint32_t		timeout; //http response timeout
	BOOL			filter; // if filter result
}SYS_PARAM_HTTPD;


typedef struct
{
	char		apn[APN_NAME_MAX_LEN+1]; // apn name
	char		user[APN_USER_MAX_LEN+1]; //apn user name
	char		psw[APN_PSW_MAX_LEN+1]; // apn password
}SYS_PARAM_APN;


typedef struct
{
	BOOL						enable; // enable or disable sending register pack
	REG_DATA_TYPE		data_type; // user defined data, D2D ID, ICCID
	REG_WHEN_SEND	when_send; // send register pack when establishing connection or in front of user's meaningful data
	char							user_data[REG_DATA_MAX_LEN+1]; // user defined data
	uint8_t						data_len; // user defined data len
	uint16_t					d2d_id; // D2D ID
	char cloud_id[USR_PASS_CLOUD_ID_SIZE+1];
	char cloud_pw[USR_PASS_CLOUD_PW_SIZE+1];
}SYS_PARAM_REG_PACK;


typedef struct
{
	BOOL					enable; // enable or disable heart beat
	HRT_DATA_TYPE	data_type;	 // user defined data, D2D ID, ICCID
	HRT_SEND_DEST	send_dest; // send data to network or uart
	char						user_data[HEART_DATA_MAX_LEN+1]; // user defined data
	uint8_t					data_len; // user defined data len
	uint32_t				period; // sending period
}SYS_PARAM_HRT_PACK;


typedef struct
{
	BOOL		echo_en; // enable echo
	BOOL		rfc_en; // enable RFC2117
	BOOL		nat_en; // enable net AT command
	BOOL		uat_en; // enable uart AT command
	BOOL		call_en; // enable call
	BOOL		cache_en; // enable cache
	BOOL		sms_limit; // enable limit SMS source phone number
	BOOL		sms_show_phone; // enable show phone number
	BOOL		sock_ind; // enable show socket indication
	BOOL		sms_pass; // enable pass SMS when wkmod=NET or HTTPD
	WKMOD	    wkmod; // woke mode
	char		hello_msg[HELLO_MSG_MAX_LEN+1]; // welcome message
	char		phone_num[PHONE_NUM_MAX_LEN+1]; // destination phone number
	char		psw[CMD_PSW_MAX_LEN+1]; //the password of uart command, network command, and SMS command
	char        script[SCRIPT_NAME_SIZE+1]; //the 4G module dial script
	char        ping_addr[SOCK_MAX_ADDR_LEN+1]; //ping internet address
	SMS_CODE    sms_code; //SMS code mode, ASCII or UCS2
	uint16_t    pack_period; // uart data pack period
	uint16_t    pack_length; // uart data pack length
	char		plang[10]; //language of web page
	MOD_NETWORK	module_network; //module network selection
	NET_CARD	first_net; //default network
	ETH_MODE	eth0_mode; //set ETH0 mode
} SYS_PARAM_BASIC;


//system parameters
typedef struct
{
	uint8_t					first_burning_flag1; // check if it is first time to run after burning
	BOOL					test_en; // if factory test is enable
	uint16_t				encrypt; // firmware encrypt
	SYS_PARAM_UART			uart2; // uart2 parameters
	SYS_PARAM_UART			uartX; // uartX parameters
	SYS_PARAM_SOCK			sock[SOCK_SUM]; // socketA parameters
	SYS_PARAM_HTTPD			httpd; // httpd parameters
	SYS_PARAM_APN			apn; // apn parameters
	SYS_PARAM_REG_PACK      reg_pack; // register pack parameters
	SYS_PARAM_HRT_PACK	    hrt_pack; //heart pack parameters
	SYS_PARAM_BASIC			basic; // system basic parameters
	uint8_t					first_burning_flag2; // check if it is first time to run after burning
} SYS_PARAM_T;


typedef struct
{
	uint32_t code[2];
} SYS_ENCRYPT;

typedef struct
{
	char sn[SN_MAX_LEN+1];
	SYS_ENCRYPT encrypt;
} SYS_PARAM_C;


typedef union
{
	SYS_PARAM_T para;
	uint8_t buf[MAX_USR_PARA_SIZE];
} SYS_PARAM_UT;


typedef union
{
	SYS_PARAM_C para;
	uint8_t buf[MAX_CON_PARA_SIZE];
} SYS_PARAM_UC;




PARAM_EXT SYS_PARAM_UT g_SysParam;
PARAM_EXT SYS_PARAM_UC g_SysParamUC;



#ifndef __PARAM_C
PARAM_EXT const SYS_PARAM_T	 usr_factory_t;
PARAM_EXT const SYS_PARAM_C	 usr_factory_c;
#else
PARAM_EXT const SYS_PARAM_T	 usr_factory_t =
{
	FLASH_TRUE, // first_burning_flag1
	FALSE, // test_en
	FLASH_TRUE, // encrypt
	{	// uart parameters
		115200,
		UART_CHK_NONE,
		UART_DATA_BITS_8,
		UART_STOP_BITS_1,
		UART_FLOW_NFC,
	},
	{	// uart parameters
		115200,
		UART_CHK_NONE,
		UART_DATA_BITS_8,
		UART_STOP_BITS_1,
		UART_FLOW_NFC,
	},
	{{	// socket A parameters
		TRUE,
		SOCK_TYPE_TCP_CLIENT,
		TCP_LONG_LINK,
		{"test.usr.cn"},
		2317,
		10,
	},
	{	// socket B parameters
		FALSE,
		SOCK_TYPE_TCP_CLIENT,
		TCP_LONG_LINK,
		{"test.usr.cn"},
		2317,
		10,
	},
	{	// socket C parameters
		FALSE,
		SOCK_TYPE_TCP_CLIENT,
		TCP_LONG_LINK,
		{"test.usr.cn"},
		2317,
		10,
	},
	{	// socket D parameters
		FALSE,
		SOCK_TYPE_TCP_CLIENT,
		TCP_LONG_LINK,
		{"test.usr.cn"},
		2317,
		10,
	}},
	{	// httpd parameters
		HTTP_TYPE_GET,
		"Connection: close[0D][0A]",
		"/1.php[3F]",
		"test.usr.cn",
		80,
		10,
		FALSE,
	},
	{	// apn parameters
		"CMNET",
		"",
		"",
	},
	{	// register pack parameters
		FALSE,
		REG_DATA_TYPE_USER,
		REG_SEND_WITH_DATA,
		"7777772E7573722E636E",
		20,
		1111,
		"12345678001122334455",
		"0000test",
	},
	{	// heart pack parameters
		FALSE,
		REG_DATA_TYPE_USER,
		HRT_SEND_TO_NET,
		"7777772E7573722E636E",
		20,
		30,
	},
	{	// system basic parameters
		TRUE, // echo_en
		FALSE, // rfc_en
		TRUE, // nat_en
		TRUE, // uat_en
		FALSE, // call_en
		FALSE, //cache_en
		TRUE, // enable limit SMS source phone number
		FALSE, // enable show SMS deliver number
		FALSE, // enable show socket indication
		FALSE, // enable pass SMS when wkmod=NET or HTTPD
		WKMOD_NET, // work mode
		"USR_3G", // welcome message
		"4000255652", // destination phone number
		"www.usr.cn#", // command password
		"G402tf", //4G module dial script
		"61.135.169.121", //ping internet address
		SMS_CODE_UCS2, // SMS code
		200, // uart data pack period
		1000, // uart data pack length
		"en", //language of web page
		MOD_AUTO, //module network selection
		NET_CARD_4G_FIRST, //default network
		ETH_MODE_WAN, //set ETH0 mode
	},
	FLASH_TRUE, // first_burning_flag2
};

PARAM_EXT const SYS_PARAM_C	 usr_factory_c = 
{
	"NONE",					//sn code
	{ {	0xFFFFFFFF, 0xFFFFFFFF,}	}, // encrypt code
};

#endif



/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PARAM_EXT void Param_Init( void );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PARAM_EXT void Param_ReadWorkParam( void );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PARAM_EXT void Param_SaveWorkParam( void );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PARAM_EXT void Param_SaveAsUserDefault( void );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PARAM_EXT void Param_ReloadUserDefault( void );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PARAM_EXT void Param_ReloadUsrParam( void );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PARAM_EXT void Param_ReadConstParam( void );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PARAM_EXT void Param_SaveConstParam( void );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PARAM_EXT void Param_ReadDeviceSerialNum( uint32_t *sn );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PARAM_EXT BOOL Param_CheckEncrypt( void );


#endif
