/***********************************************************************************
Copy right:		2015-2025 USR IOT Tech.
Author:			Wangbin
Version:		V1.0
Date:			2015-09
Description:	g_AtCmd thread.
***********************************************************************************/
#ifndef	__USR_TYPE_H
#define	__USR_TYPE_H


#include "public.h"


//TEA
#define ENCRYPT_ROUND	32  //
#define ENCRYPT_DELTA	0x9e3779b9 /* sqr(5)-1 * 2^31 */

//buffer parameters
#define KB                      1024
#define UART_BUF_SIZE			(10*KB)
#define CMD_BUF_SIZE            KB
#define HTML_BUF_SIZE		(64*KB)
#define INPUT_BUF_SIZE		(512*KB)
// 9A0000 - 10000 = 990000(HEX, )  990000/400 = 9792KB(OCT)
#define FIRMWARE_SIZE		(9792*KB)


#define BACKLOG					5     //listen队列中等待的连接数   
#define TCPC_BUF_SIZE			1460 //tcp client buffer size
#define SOCK_BUF_SIZE			UART_BUF_SIZE //socket buffer size

#define TCPC_LEN_SIZE			100  //tcp client length size
#define SOCK_LEN_SIZE			100  //socket length size



//socket
#define SOCK_MAX_ADDR_LEN		100
#define SOCK_MAX_SUM			2
#define IP_MAX_LEN				17
#define SOCK_MAX_TIMEOUT		100

//http
#define HTTP_MAX_URL_LEN		100
#define HTTP_MAX_ADDR_LEN		100
#define HTTP_MAX_HEAD_LEN		250
#define HTTP_MAX_TIMEOUT		600

//uart data pack
#define PACK_MIN_TIME	 		50
#define PACK_MAX_TIME	 		60000
#define PACK_MIN_LEN	 		1
#define PACK_MAX_LEN	 		1000


#define SMS_MAX_LEN 			140


//apn
#define APN_NAME_MAX_LEN		50
#define APN_USER_MAX_LEN		20
#define APN_PSW_MAX_LEN			20

//register and herat beat
#define REG_DATA_MAX_LEN		80
#define REG_PACK_LEN			50
#define HEART_DATA_MAX_LEN		80
#define HEART_MAX_PERIOD		6000

// cloud
#define CLOUD_RED_DATA_SIZE		REG_PACK_LEN
#define USR_PASS_CLOUD_ID_SIZE	20
#define USR_PASS_CLOUD_PW_SIZE	8

#define CMD_PSW_MAX_LEN			11
#define HELLO_MSG_MAX_LEN		20
#define PHONE_NUM_MAX_LEN		20
#define SN_MAX_LEN				30

#define AT_CMD_MAX_LEN			PACK_MAX_LEN
#define AT_CMD_MAX_PARA_SUM		20

#define SCRIPT_NAME_SIZE        20

#define BAUD_MAX_SIZE			460800
#define BAUD_MIN_SIZE			1200


#define SOCK_SUM				4
#define SOCKA					1
#define SOCKB					2
#define SOCKC					3
#define SOCKD					4
#define MIN_SOCK_ID				SOCKA
#define MAX_SOCK_ID				SOCKD
#define SOCK_NONE				0
#define CLIENT_SIZE  			8









#define SYS_RESTART()             system("reboot")


//define uart parameters
typedef enum
{
	UART_CHK_NONE	= 0,
	UART_CHK_ODD	= 1,
	UART_CHK_EVEN	= 2,
	UART_CHK_SPACE	= 3,
} UART_PARITY_TYPE;

typedef enum
{
	UART_DATA_BITS_5 = 5,
	UART_DATA_BITS_6 = 6,
	UART_DATA_BITS_7 = 7,
	UART_DATA_BITS_8 = 8,
} UART_DATA_BITS;

typedef enum
{
	UART_STOP_BITS_1 = 1,
	UART_STOP_BITS_2 = 2,
} UART_STOP_BITS;

typedef enum
{
	UART_FLOW_NFC	= 0,
	UART_FLOW_CRTS	= 1,
	UART_FLOW_XON	= 2,
	UART_FLOW_RS485	= 3,
} UART_FLOW_TYPE;



//define socket parameters
typedef enum
{
	SOCK_TYPE_TCP_CLIENT	= 0,
	SOCK_TYPE_UDP_CLIENT	= 1,
	SOCK_TYPE_TCP_SERVER	= 2,
	SOCK_TYPE_UDP_SERVER	= 3,
} SOCK_TYPE;

typedef enum
{
	TCP_LONG_LINK	= 1,
	TCP_SHORT_LINK	= 2,
} TCP_LINK_TYPE;


typedef enum
{
	SOCK_STATUS_DISCONNECTED	= 0,
	SOCK_STATUS_GETTING_IP		= 1,
	SOCK_STATUS_GOT_IP			= 2,
	SOCK_STATUS_CONNECTING		= 3,
	SOCK_STATUS_CONNECTED		= 4,
	//SOCK_STATUS_DISCONNECT	= 6,
	//SOCK_STATUS_DISCONNECTING	= 7,
} SOCK_STATUS;


typedef enum
{
	PPP_STATUS_DISCONNECTED		= 0,
	PPP_STATUS_CONNECTING		= 1,
	PPP_STATUS_CONNECTED		= 2,
} PPP_STATUS;


typedef enum
{
	HTTPD_STATUS_GET_REQ		= 1,
	HTTPD_STATUS_CONNECTED		= 2,
	HTTPD_STATUS_SENDED			= 3,
	HTTPD_STATUS_RESPOND		= 4,
} HTTPD_STATUS;


//define http parameters
typedef enum
{
	HTTP_TYPE_GET	= 1,
	HTTP_TYPE_POST	= 2,
} HTTP_TYPE;


//define register parameters
typedef enum
{
	REG_DATA_TYPE_USER	= 1,
	REG_DATA_TYPE_ICCID	= 2,
	REG_DATA_TYPE_IMEI	= 3,
	REG_DATA_TYPE_CLOUD	= 4,
} REG_DATA_TYPE;



//define register parameters
typedef enum
{
	REG_SEND_WHEN_LINK	= 1,
	REG_SEND_WITH_DATA	= 2,
} REG_WHEN_SEND;


//define heart parameters
typedef enum _hrt_data_type
{
	HRT_DATA_TYPE_ICCID	= 1,
	HRT_DATA_TYPE_D2DID	= 2,
	HRT_DATA_TYPE_USER	= 3,
} HRT_DATA_TYPE;


//define register parameters
typedef enum
{
	HRT_SEND_TO_NET	= 1,
	HRT_SEND_TO_COM	= 2,
} HRT_SEND_DEST;


//define +++a status
typedef enum
{
	PLUS_3_A_STATUS_NONE	= 0,
	PLUS_3_A_STATUS_PLUS1	= 1,
	PLUS_3_A_STATUS_PLUS2	= 2,
	PLUS_3_A_STATUS_PLUS3	= 3,
	PLUS_3_A_STATUS_A		= 4,
} PLUS_3_A_STATUS;


//define woke mode, AT mode or throughput mode
typedef enum
{
	AT_CMD_MODE		= 0,
	TRANS_MODE		= 1,
} AT_OR_THROUGHPUT_MODE;


//define woke mode, network transmit, SMS transmit, HTTPD
typedef enum
{
	WKMOD_SMS		= 0,
	WKMOD_NET		= 1,
	WKMOD_HTTPD		= 2,
} WKMOD;


//define woke mode, network transmit, SMS transmit, HTTPD
typedef enum
{
	AT_CMD_ERR	= 0,
	AT_CMD_SET	= 1,
	AT_CMD_TEST	= 2,
	AT_CMD_READ	= 3,
	AT_CMD_EXE	= 4,
} AT_CMD_TYPE;


//define woke mode, network transmit, SMS transmit, HTTPD
typedef enum
{
	PARA_TYPE_UINT8		= 1,
	PARA_TYPE_UINT16	= 2,
	PARA_TYPE_UINT32	= 3,
	PARA_TYPE_STRING	= 4,
} PARA_TYPE;



//define woke mode, network transmit, SMS transmit, HTTPD
typedef enum
{
	ME200_UNIT_STATUS_NONE		= 0,
	ME200_UNIT_STATUS_CR1		= 1,
	ME200_UNIT_STATUS_LF1		= 2,
	ME200_UNIT_STATUS_CR2		= 3,
	ME200_UNIT_STATUS_LF2		= 4,
} ME200_GET_UNIT_STATUS;


// ^IPSTAT command return type
typedef enum
{
	IP_STAT_RESERVE			= 0,
	IP_STAT_DISCONNECTED	= 1,
	IP_STAT_CONNECTED		= 2,
	PPP_STAT_DISCONNECTED	= 3,
	PPP_STAT_CONNECTED		= 4,
} IP_STAT;


// at command response type
typedef enum
{
	AT_CMD_RSP_TYPE_CMD		= 0,
	AT_CMD_RSP_TYPE_UART	= 1,
	AT_CMD_RSP_TYPE_INNER	= 2,
	AT_CMD_RSP_TYPE_SMS		= 3,
	AT_CMD_RSP_TYPE_NET1	= 4,
	AT_CMD_RSP_TYPE_NET2	= 5,
	AT_CMD_RSP_TYPE_NET3	= 6,
	AT_CMD_RSP_TYPE_NET4	= 7,
} AT_CMD_RSP_TYPE;


// SMS code mode
typedef enum
{
	SMS_CODE_GSM7BIT	= 0,
	SMS_CODE_ASCII		= 1,
	SMS_CODE_UCS2		= 6,
} SMS_CODE;


// 4G module network status
typedef enum
{
	STATUS_4G_IDLE      = 0,
	STATUS_4G_MT_RDY	= 1,
	STATUS_4G_SIM_RDY	= 2,
	STATUS_4G_REG_OK	= 3,
	STATUS_4G_PDP_OK	= 4,
	STATUS_4G_ECM_OK	= 5,
} STATUS_4G;


// 4G module network selection
typedef enum
{
	MOD_AUTO      	= 0,
	MOD_GSM_ONLY	= 1,
	MOD_TD_ONLY		= 2,
	MOD_WCDMA_ONLY	= 3,
	MOD_CDMA_ONLY	= 4,
} MOD_NETWORK;


// 4G or ETH first
typedef enum
{
	NET_CARD_4G_FIRST = 0,
	NET_CARD_ETH_FIRST	= 1,
	NET_CARD_4G_ONLY	= 2,
	NET_CARD_ETH_ONLY	= 3,
} NET_CARD;



// ETH mode
typedef enum
{
	ETH_MODE_WAN	= 0,
	ETH_MODE_LAN	= 1,
} ETH_MODE;



#endif

