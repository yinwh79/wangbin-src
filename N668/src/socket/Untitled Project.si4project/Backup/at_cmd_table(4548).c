/***********************************************************************************
Copy right:	2014-2024 USR IOT Tech.
Author:			Bin Wang
Version:			V1.0
Date:				2014-08
Description:	g_AtCmd thread.1111
***********************************************************************************/
#define __AT_CMD_TABLE_C

#include "at_cmd.h"
#include "param.h"
#include "at_cmd_table.h"

static char cmd_para_buf[10*KB];

#define FACTORY_PARAMSIZE	64
#define FACTORY_MAC			0
#define FACTORY_ENCRYPT		FACTORY_PARAMSIZE*1
#define FACTORY_MANUF_VER	FACTORY_PARAMSIZE*2
#define FACTORY_USERVER		FACTORY_PARAMSIZE*3
static char block_factory[10*KB] = {0};

extern unsigned int echoon;

const COMMAND_TABLE_T g_CommandTable[] =
{
	{ "AT+E", AtCmd_Handler_E, "enable/disable echo" },
	{ "AT+Z", AtCmd_Handler_Z, "restart module" },
	{ "AT+VER", AtCmd_Handler_VER, "firmware version" },
	{ "AT+HW_VER", AtCmd_Handler_HW_VER, "hardware version" },
	{ "AT+MAC", AtCmd_Handler_MAC, "MAC address of br0" },
	//{ "AT+ICCID", AtCmd_Handler_ICCID, "query ICCID of SIM card" },
	//{ "AT+STMSG", AtCmd_Handler_STMSG, "start message" },
	{ "AT+RELD", AtCmd_Handler_RELD, "reload user factory param" },
	{ "AT+DNS", AtCmd_Handler_DNS, "query/set dns server" },
	{ "AT+WEBU", AtCmd_Handler_CMDPW, "query/set web server" },
	{ "AT+WEBPORT", AtCmd_Handler_WEBPORT, "query/set web port" },
	{ "AT+WANN", AtCmd_Handler_WANN, "query WAN port ip param" },
	{ "AT+UART", AtCmd_Handler_UART, "query/set uart1 param" },
	{ "AT+UART1", AtCmd_Handler_UART, "-" },
	{ "AT+UART2", AtCmd_Handler_UART, "-" },
	{ "AT+UART3", AtCmd_Handler_UART, "-" },
	{ "AT+UART4", AtCmd_Handler_UART, "-" },
	{ "AT+UART5", AtCmd_Handler_UART, "-" },
	{ "AT+UART6", AtCmd_Handler_UART, "-" },
	{ "AT+UART7", AtCmd_Handler_UART, "-" },
	{ "AT+UART8", AtCmd_Handler_UART, "-" },
	{ "AT+UARTTL", AtCmd_Handler_UARTTL, "query/set uart packet time and length" },
	{ "AT+UARTTL1", AtCmd_Handler_UARTTL, "-" },
	{ "AT+UARTTL2", AtCmd_Handler_UARTTL, "-" },
	{ "AT+UARTTL3", AtCmd_Handler_UARTTL, "-" },
	{ "AT+UARTTL4", AtCmd_Handler_UARTTL, "-" },
	{ "AT+UARTTL5", AtCmd_Handler_UARTTL, "-" },
	{ "AT+UARTTL6", AtCmd_Handler_UARTTL, "-" },
	{ "AT+UARTTL7", AtCmd_Handler_UARTTL, "-" },
	{ "AT+UARTTL8", AtCmd_Handler_UARTTL, "-" },
	
	{ "AT+RFCEN", AtCmd_Handler_RFCEN, "enable/disable uart1 RFC2217" },
	{ "AT+RFCEN1", AtCmd_Handler_RFCEN, "-" },
	{ "AT+RFCEN2", AtCmd_Handler_RFCEN, "-" },
	{ "AT+RFCEN3", AtCmd_Handler_RFCEN, "-" },
	{ "AT+RFCEN4", AtCmd_Handler_RFCEN, "-" },
	{ "AT+RFCEN5", AtCmd_Handler_RFCEN, "-" },
	{ "AT+RFCEN6", AtCmd_Handler_RFCEN, "-" },
	{ "AT+RFCEN7", AtCmd_Handler_RFCEN, "-" },
	{ "AT+RFCEN8", AtCmd_Handler_RFCEN, "-" },

	{ "AT+SOCK", AtCmd_Handler_SOCKA, "query/set socket A1 param" },
	{ "AT+SOCKA", AtCmd_Handler_SOCKA, "query/set socket A1 param" },
	{ "AT+SOCKA1", AtCmd_Handler_SOCKA, "query/set socket A1 param" },
	{ "AT+SOCKB", AtCmd_Handler_SOCKB, "query/set socket B1 param" },
	{ "AT+SOCKB1", AtCmd_Handler_SOCKB, "query/set socket B1 param" },
	{ "AT+SOCKA2", AtCmd_Handler_SOCKA, "-" },
	{ "AT+SOCKB2", AtCmd_Handler_SOCKB, "-" },
	{ "AT+SOCKA3", AtCmd_Handler_SOCKA, "-" },
	{ "AT+SOCKB3", AtCmd_Handler_SOCKB, "-" },
	{ "AT+SOCKA4", AtCmd_Handler_SOCKA, "-" },
	{ "AT+SOCKB4", AtCmd_Handler_SOCKB, "-" },
	{ "AT+SOCKA5", AtCmd_Handler_SOCKA, "-" },
	{ "AT+SOCKB5", AtCmd_Handler_SOCKB, "-" },
	{ "AT+SOCKA6", AtCmd_Handler_SOCKA, "-" },
	{ "AT+SOCKB6", AtCmd_Handler_SOCKB, "-" },
	{ "AT+SOCKA7", AtCmd_Handler_SOCKA, "-" },
	{ "AT+SOCKB7", AtCmd_Handler_SOCKB, "-" },
	{ "AT+SOCKA8", AtCmd_Handler_SOCKA, "-" },
	{ "AT+SOCKB8", AtCmd_Handler_SOCKB, "-" },
	{ "AT+SOCKPORT", AtCmd_Handler_SOCKPORTA, "query/set socket port" },	
	{ "AT+SOCKPORTA", AtCmd_Handler_SOCKPORTA, "query/set socket A1 port param" },
	{ "AT+SOCKPORTA1", AtCmd_Handler_SOCKPORTA, "query/set socket A1 port param" },
	{ "AT+SOCKPORTB1", AtCmd_Handler_SOCKPORTB, "query/set socket B1 port param" },
	{ "AT+SOCKPORTA2", AtCmd_Handler_SOCKPORTA, "-" },
	{ "AT+SOCKPORTB2", AtCmd_Handler_SOCKPORTB, "-" },
	{ "AT+SOCKPORTA3", AtCmd_Handler_SOCKPORTA, "-" },
	{ "AT+SOCKPORTB3", AtCmd_Handler_SOCKPORTB, "-" },
	{ "AT+SOCKPORTA4", AtCmd_Handler_SOCKPORTA, "-" },
	{ "AT+SOCKPORTB4", AtCmd_Handler_SOCKPORTB, "-" },
	{ "AT+SOCKPORTA5", AtCmd_Handler_SOCKPORTA, "-" },
	{ "AT+SOCKPORTB5", AtCmd_Handler_SOCKPORTB, "-" },
	{ "AT+SOCKPORTA6", AtCmd_Handler_SOCKPORTA, "-" },
	{ "AT+SOCKPORTB6", AtCmd_Handler_SOCKPORTB, "-" },
	{ "AT+SOCKPORTA7", AtCmd_Handler_SOCKPORTA, "-" },
	{ "AT+SOCKPORTB7", AtCmd_Handler_SOCKPORTB, "-" },
	{ "AT+SOCKPORTA8", AtCmd_Handler_SOCKPORTA, "-" },
	{ "AT+SOCKPORTB8", AtCmd_Handler_SOCKPORTB, "-" },
	{ "AT+SOCKLK", AtCmd_Handler_SOCKALK, "query the connection status of uart1 - socket A" },
	{ "AT+SOCKLKA", AtCmd_Handler_SOCKALK, "query the connection status of uart1 - socket A" },
	{ "AT+SOCKLKA1", AtCmd_Handler_SOCKALK, "query the connection status of uart1 - socket A" },
	//{ "AT+SOCKLKB1", AtCmd_Handler_SOCKBLK, "query the connection status of uart1 - socket B" },
	{ "AT+SOCKLKA2", AtCmd_Handler_SOCKALK, "-" },
	//{ "AT+SOCKLKB2", AtCmd_Handler_SOCKBLK, "-" },
	{ "AT+SOCKLKA3", AtCmd_Handler_SOCKALK, "-" },
	//{ "AT+SOCKLKB3", AtCmd_Handler_SOCKBLK, "-" },
	{ "AT+SOCKLKA4", AtCmd_Handler_SOCKALK, "-" },
	//{ "AT+SOCKLKB4", AtCmd_Handler_SOCKBLK, "-" },
	{ "AT+SOCKLKA5", AtCmd_Handler_SOCKALK, "-" },
	//{ "AT+SOCKLKB5", AtCmd_Handler_SOCKBLK, "-" },
	{ "AT+SOCKLKA6", AtCmd_Handler_SOCKALK, "-" },
	//{ "AT+SOCKLKB6", AtCmd_Handler_SOCKBLK, "-" },
	{ "AT+SOCKLKA7", AtCmd_Handler_SOCKALK, "-" },
	//{ "AT+SOCKLKB7", AtCmd_Handler_SOCKBLK, "-" },
	{ "AT+SOCKLKA8", AtCmd_Handler_SOCKALK, "-" },
	//{ "AT+SOCKLKB8", AtCmd_Handler_SOCKBLK, "-" },

	{ "AT+REGEN", AtCmd_Handler_REGEN, "enable/disable register login function" },
	{ "AT+REGEN1", AtCmd_Handler_REGEN, "-" },
	{ "AT+REGEN2", AtCmd_Handler_REGEN, "-" },
	{ "AT+REGEN3", AtCmd_Handler_REGEN, "-" },
	{ "AT+REGEN4", AtCmd_Handler_REGEN, "-" },
	{ "AT+REGEN5", AtCmd_Handler_REGEN, "-" },
	{ "AT+REGEN6", AtCmd_Handler_REGEN, "-" },
	{ "AT+REGEN7", AtCmd_Handler_REGEN, "-" },
	{ "AT+REGEN8", AtCmd_Handler_REGEN, "-" },
	{ "AT+REGTCP", AtCmd_Handler_REGTCP, "query/set login with ascii/hex" },
	{ "AT+REGTCP1", AtCmd_Handler_REGTCP, "-" },
	{ "AT+REGTCP2", AtCmd_Handler_REGTCP, "-" },
	{ "AT+REGTCP3", AtCmd_Handler_REGTCP, "-" },
	{ "AT+REGTCP4", AtCmd_Handler_REGTCP, "-" },
	{ "AT+REGTCP5", AtCmd_Handler_REGTCP, "-" },
	{ "AT+REGTCP6", AtCmd_Handler_REGTCP, "-" },
	{ "AT+REGTCP7", AtCmd_Handler_REGTCP, "-" },
	{ "AT+REGTCP8", AtCmd_Handler_REGTCP, "-" },
	{ "AT+REGCLOUD", AtCmd_Handler_REGCLOUD, "query/set cloud ID and password" },
	{ "AT+REGCLOUD1", AtCmd_Handler_REGCLOUD, "-" },
	{ "AT+REGCLOUD2", AtCmd_Handler_REGCLOUD, "-" },
	{ "AT+REGCLOUD3", AtCmd_Handler_REGCLOUD, "-" },
	{ "AT+REGCLOUD4", AtCmd_Handler_REGCLOUD, "-" },
	{ "AT+REGCLOUD5", AtCmd_Handler_REGCLOUD, "-" },
	{ "AT+REGCLOUD6", AtCmd_Handler_REGCLOUD, "-" },
	{ "AT+REGCLOUD7", AtCmd_Handler_REGCLOUD, "-" },
	{ "AT+REGCLOUD8", AtCmd_Handler_REGCLOUD, "-" },
	{ "AT+REGUSR", AtCmd_Handler_REGUSR, "query/set register content" },
	{ "AT+REGUSR1", AtCmd_Handler_REGUSR, "-" },
	{ "AT+REGUSR2", AtCmd_Handler_REGUSR, "-" },
	{ "AT+REGUSR3", AtCmd_Handler_REGUSR, "-" },
	{ "AT+REGUSR4", AtCmd_Handler_REGUSR, "-" },
	{ "AT+REGUSR5", AtCmd_Handler_REGUSR, "-" },
	{ "AT+REGUSR6", AtCmd_Handler_REGUSR, "-" },
	{ "AT+REGUSR7", AtCmd_Handler_REGUSR, "-" },
	{ "AT+REGUSR8", AtCmd_Handler_REGUSR, "-" },
	{ "AT+REGUSER", AtCmd_Handler_REGUSER, "query/set register content with hex/ascii" },
	{ "AT+REGUSER1", AtCmd_Handler_REGUSER, "-" },
	{ "AT+REGUSER2", AtCmd_Handler_REGUSER, "-" },
	{ "AT+REGUSER3", AtCmd_Handler_REGUSER, "-" },
	{ "AT+REGUSER4", AtCmd_Handler_REGUSER, "-" },
	{ "AT+REGUSER5", AtCmd_Handler_REGUSER, "-" },
	{ "AT+REGUSER6", AtCmd_Handler_REGUSER, "-" },
	{ "AT+REGUSER7", AtCmd_Handler_REGUSER, "-" },
	{ "AT+REGUSER8", AtCmd_Handler_REGUSER, "-" },
	
	{ "AT+HEARTEN", AtCmd_Handler_HEARTEN, "enable/disable heart beat function" },
	{ "AT+HEARTEN1", AtCmd_Handler_HEARTEN, "-" },
	{ "AT+HEARTEN2", AtCmd_Handler_HEARTEN, "-" },
	{ "AT+HEARTEN3", AtCmd_Handler_HEARTEN, "-" },
	{ "AT+HEARTEN4", AtCmd_Handler_HEARTEN, "-" },
	{ "AT+HEARTEN5", AtCmd_Handler_HEARTEN, "-" },
	{ "AT+HEARTEN6", AtCmd_Handler_HEARTEN, "-" },
	{ "AT+HEARTEN7", AtCmd_Handler_HEARTEN, "-" },
	{ "AT+HEARTEN8", AtCmd_Handler_HEARTEN, "-" },
	{ "AT+HEARTTP", AtCmd_Handler_HEARTTP, "query/set the dirction to send" },
	{ "AT+HEARTTP1", AtCmd_Handler_HEARTTP, "-" },
	{ "AT+HEARTTP2", AtCmd_Handler_HEARTTP, "-" },
	{ "AT+HEARTTP3", AtCmd_Handler_HEARTTP, "-" },
	{ "AT+HEARTTP4", AtCmd_Handler_HEARTTP, "-" },
	{ "AT+HEARTTP5", AtCmd_Handler_HEARTTP, "-" },
	{ "AT+HEARTTP6", AtCmd_Handler_HEARTTP, "-" },
	{ "AT+HEARTTP7", AtCmd_Handler_HEARTTP, "-" },
	{ "AT+HEARTTP8", AtCmd_Handler_HEARTTP, "-" },
	{ "AT+HEARTTM", AtCmd_Handler_HEARTTM, "query/set heart sending period" },
	{ "AT+HEARTTM1", AtCmd_Handler_HEARTTM, "-" },
	{ "AT+HEARTTM2", AtCmd_Handler_HEARTTM, "-" },
	{ "AT+HEARTTM3", AtCmd_Handler_HEARTTM, "-" },
	{ "AT+HEARTTM4", AtCmd_Handler_HEARTTM, "-" },
	{ "AT+HEARTTM5", AtCmd_Handler_HEARTTM, "-" },
	{ "AT+HEARTTM6", AtCmd_Handler_HEARTTM, "-" },
	{ "AT+HEARTTM7", AtCmd_Handler_HEARTTM, "-" },
	{ "AT+HEARTTM8", AtCmd_Handler_HEARTTM, "-" },
	{ "AT+HEARTDT", AtCmd_Handler_HEARTDT, "query/set user defined heart data" },
	{ "AT+HEARTDT1", AtCmd_Handler_HEARTDT, "-" },
	{ "AT+HEARTDT2", AtCmd_Handler_HEARTDT, "-" },
	{ "AT+HEARTDT3", AtCmd_Handler_HEARTDT, "-" },
	{ "AT+HEARTDT4", AtCmd_Handler_HEARTDT, "-" },
	{ "AT+HEARTDT5", AtCmd_Handler_HEARTDT, "-" },
	{ "AT+HEARTDT6", AtCmd_Handler_HEARTDT, "-" },
	{ "AT+HEARTDT7", AtCmd_Handler_HEARTDT, "-" },
	{ "AT+HEARTDT8", AtCmd_Handler_HEARTDT, "-" },
	{ "AT+HEARTUSER", AtCmd_Handler_HEARTUSER, "query/set user defined heart data" },
	{ "AT+HEARTUSER1", AtCmd_Handler_HEARTUSER, "-" },
	{ "AT+HEARTUSER2", AtCmd_Handler_HEARTUSER, "-" },
	{ "AT+HEARTUSER3", AtCmd_Handler_HEARTUSER, "-" },
	{ "AT+HEARTUSER4", AtCmd_Handler_HEARTUSER, "-" },
	{ "AT+HEARTUSER5", AtCmd_Handler_HEARTUSER, "-" },
	{ "AT+HEARTUSER6", AtCmd_Handler_HEARTUSER, "-" },
	{ "AT+HEARTUSER7", AtCmd_Handler_HEARTUSER, "-" },
	{ "AT+HEARTUSER8", AtCmd_Handler_HEARTUSER, "-" },
	
	{ "AT+HTPTP", AtCmd_Handler_HTPTP, "query/set HTTP request type" },
	{ "AT+HTPTP1", AtCmd_Handler_HTPTP, "-" },
	{ "AT+HTPTP2", AtCmd_Handler_HTPTP, "-" },
	{ "AT+HTPTP3", AtCmd_Handler_HTPTP, "-" },
	{ "AT+HTPTP4", AtCmd_Handler_HTPTP, "-" },
	{ "AT+HTPTP5", AtCmd_Handler_HTPTP, "-" },
	{ "AT+HTPTP6", AtCmd_Handler_HTPTP, "-" },
	{ "AT+HTPTP7", AtCmd_Handler_HTPTP, "-" },
	{ "AT+HTPTP8", AtCmd_Handler_HTPTP, "-" },
	{ "AT+HTPURL", AtCmd_Handler_HTPURL, "query/set HTTP request URL" },
	{ "AT+HTPURL1", AtCmd_Handler_HTPURL, "-" },
	{ "AT+HTPURL2", AtCmd_Handler_HTPURL, "-" },
	{ "AT+HTPURL3", AtCmd_Handler_HTPURL, "-" },
	{ "AT+HTPURL4", AtCmd_Handler_HTPURL, "-" },
	{ "AT+HTPURL5", AtCmd_Handler_HTPURL, "-" },
	{ "AT+HTPURL6", AtCmd_Handler_HTPURL, "-" },
	{ "AT+HTPURL7", AtCmd_Handler_HTPURL, "-" },
	{ "AT+HTPURL8", AtCmd_Handler_HTPURL, "-" },
	//{ "AT+HTPSV", AtCmd_Handler_HTPSV, "query/set HTTP server address and port" },
	{ "AT+HTPHEAD", AtCmd_Handler_HTPHD, "query/set HTTP request head" },
	{ "AT+HTPHEAD1", AtCmd_Handler_HTPHD, "-" },
	{ "AT+HTPHEAD2", AtCmd_Handler_HTPHD, "-" },
	{ "AT+HTPHEAD3", AtCmd_Handler_HTPHD, "-" },
	{ "AT+HTPHEAD4", AtCmd_Handler_HTPHD, "-" },
	{ "AT+HTPHEAD5", AtCmd_Handler_HTPHD, "-" },
	{ "AT+HTPHEAD6", AtCmd_Handler_HTPHD, "-" },
	{ "AT+HTPHEAD7", AtCmd_Handler_HTPHD, "-" },
	{ "AT+HTPHEAD8", AtCmd_Handler_HTPHD, "-" },
	//{ "AT+HTPTO", AtCmd_Handler_HTPTO, "query/set HTTP request time out" },
	{ "AT+HTPCHD", AtCmd_Handler_HTPCHD, "query/set if filter HTTP head" },
	{ "AT+HTPCHD1", AtCmd_Handler_HTPCHD, "-" },
	{ "AT+HTPCHD2", AtCmd_Handler_HTPCHD, "-" },
	{ "AT+HTPCHD3", AtCmd_Handler_HTPCHD, "-" },
	{ "AT+HTPCHD4", AtCmd_Handler_HTPCHD, "-" },
	{ "AT+HTPCHD5", AtCmd_Handler_HTPCHD, "-" },
	{ "AT+HTPCHD6", AtCmd_Handler_HTPCHD, "-" },
	{ "AT+HTPCHD7", AtCmd_Handler_HTPCHD, "-" },
	{ "AT+HTPCHD8", AtCmd_Handler_HTPCHD, "-" },
	
	{ "AT+RSTIM", AtCmd_Handler_RSTIM, "reset timeout" },
	{ "AT+MID", AtCmd_Handler_MID, "module name/host name" },
	
	{ "AT+PLANG", AtCmd_Handler_PLANG, "query/set the language of web" },
	//{ "AT+MODNET", AtCmd_Handler_MODNET, "query/set network of module" },
	{ "AT+UPTIME", AtCmd_Handler_UPTIME, "query up time" },
	{ "AT+H", AtCmd_Handler_H, "command help" },
	{ "AT+SEARCH", AtCmd_Handler_SEARCH, "query search port and key words" },
	//{ "AT+PING", AtCmd_Handler_PING, NULL },
	{ "AT+TRAFFIC", AtCmd_Handler_TRAFFIC, "query/set traffic" },
	{ "AT+MONITOR", AtCmd_Handler_MONITOR, "query/set monitor enabled" },
	{ "AT+UPDATE", AtCmd_Handler_UPDATE, "query/set update enabled" },

	// time revlent
	{ "AT+NTPTM", AtCmd_Handler_NTPTM, "query current time" },
	{ "AT+NTPTML", AtCmd_Handler_NTPTML, "query last ntpsucc time" },
	{ "AT+NTPUTC", AtCmd_Handler_NTPUTC, "query UTC time stamp" },
	{ "AT+NTPZONE", AtCmd_Handler_NTPZONE, "query/set timezone" },
	{ "AT+NTPSVR", AtCmd_Handler_NTPIP, "query/set ntpserver address" },
	{ "AT+NTPRF", AtCmd_Handler_NTPRF, "query/set ntp interval" },
	// vpn 
	{ "AT+PPTP", AtCmd_Handler_VPNPPTP, "query/set pptp main param" },
	{ "AT+PPTPDTL", AtCmd_Handler_VPNPPTPDTL, "query/set pptp detail param" },
	{ "AT+GRE", AtCmd_Handler_VPNGRE, "query/set gre param" },
	{ "AT+DDNS", AtCmd_Handler_DDNS, "query/set ddns param" },
	{ "AT+DDNSEN", AtCmd_Handler_DDNSEN, "query/set ddns enabled" },
	
	// non-revel commands
	{ "AT+PDTIME", AtCmd_Handler_PDTIME, "query production time" },
	{ "AT+CLEAR", AtCmd_Handler_CLEAR, NULL },
	//{ "AT+CFGTF", AtCmd_Handler_CFGTF, NULL },
	{ "AT+LINUXCMP", AtCmd_Handler_LINUXCMP, NULL },	
	{ "AT+WRMAC", AtCmd_Handler_WRMAC, NULL },
	{ "AT+MANUF_VER", AtCmd_Handler_MANUF_VER, NULL },
	{ "AT+USERVER", AtCmd_Handler_USERVER, NULL },
	{ "AT+HWVER", AtCmd_Handler_HWVER, NULL },
	
	{ NULL, NULL, NULL }, // this line is necessary
};



static void AtCmd_ReadIMEI( char *buf, int buf_size );
static void AtCmd_ReadICCID( char *buf, int buf_size );
static void AtCmd_GetSockState( int sock_id, char *buf, int buf_size );
static void AtCmd_ReadCSQ( char *buf, int buf_size );
static void AtCmd_ReadNetMode( char *buf, int buf_size );


void AtCmd_GetSockState( int sock_id, char *buf, int buf_size )
{
	int fd, ret;

	if( SOCKA == sock_id )
	{
		fd = open( SOCKA_STATE_PATH, O_RDONLY );
	}
	else if ( SOCKB == sock_id )
	{
		fd = open( SOCKB_STATE_PATH, O_RDONLY );
	}
	else if ( SOCKC == sock_id )
	{
		fd = open( SOCKC_STATE_PATH, O_RDONLY );
	}
	else if ( SOCKD == sock_id )
	{
		fd = open( SOCKD_STATE_PATH, O_RDONLY );
	}
	else
	{
		sprintf( buf, "OFF" );
		return;
	}
	
	
	if( fd > 0 )
	{
	    ret = read( fd, buf, buf_size );
        close( fd );
		buf[ret] = 0;
	}
	else
    {
        sprintf( buf, "OFF" );
    }
}


void AtCmd_Handler_E(AT_CMD_PARA* pParam)
{
	char buffer[16] = {0};
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get system.@system[0].echo_on -q", buffer, sizeof(buffer));
			echoon = atoi(buffer);
			
			if ( echoon )
			{
				sprintf( cmd_para_buf, "\r\n+OK=ON\r\n" );
			}
			else
			{
				sprintf( cmd_para_buf, "\r\n+OK=OFF\r\n" );
			}
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)cmd_para_buf, sizeof( cmd_para_buf ), PARA_TYPE_STRING ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				if ( strnstr( cmd_para_buf, "ON", strlen("ON") ) && ( strlen( cmd_para_buf ) == 2 ) )
				{
					system("uci set system.@system[0].echo_on=1 -q");
					strcpy( cmd_para_buf, ACK_OK );
					echoon = 1;
				}
				else if ( strnstr( cmd_para_buf, "OFF", strlen("OFF") ) && ( strlen( cmd_para_buf ) == 3 ) )
				{
					system("uci set system.@system[0].echo_on=0 -q");
					strcpy( cmd_para_buf, ACK_OK );
					echoon = 0;					
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
				}
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

// reboot 
// 2 OK returns ? 
void AtCmd_Handler_Z(AT_CMD_PARA* pParam)  
{ 
	char buffer[128] = "=";
	int i = 0;

	switch ( pParam->type )
	{ 
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			for(i=0; (i<10) && (strstr(buffer, "=")!=0); i++)
			{
				system("uci commit");
				sleep(1);
				shell_get_noent("uci changes", buffer, sizeof(buffer));
				printf(buffer);
			}
			sprintf( cmd_para_buf, "%s", ACK_OK );
			
			SYS_RESTART();
			break;
		}
		
		default:
		{
			sprintf( cmd_para_buf, "%s", ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_VER(AT_CMD_PARA* pParam)
{
	char buf[64];
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("cat /etc/ver", buf, sizeof(buf));
			
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n",  buf );
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_HW_VER(AT_CMD_PARA* pParam)
{
	char buf[64];
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("cat /etc/hwver", buf, sizeof(buf));
			
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n",  buf );
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_MAC(AT_CMD_PARA* pParam)
{
	char buf[30];
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("ifconfig br0 | grep HWaddr | awk -F 'HWaddr' '{print $2}' | sed 's/ //g' | sed 's/://g'", buf, sizeof(buf));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n",  buf );
			break; 
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}



void AtCmd_Handler_RELD(AT_CMD_PARA* pParam)
{
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf( cmd_para_buf, "%s", ACK_OK );
			AtCmd_Respond( cmd_para_buf );
			SYS_RELOAD();						
			break;
		}
		
		default:
		{
			sprintf( cmd_para_buf, "%s", ACK_ERR3 );
			AtCmd_Respond( cmd_para_buf );
			break;
		}
	}
}

void AtCmd_Handler_CMDPW(AT_CMD_PARA* pParam)
{
	char username[USERPWD_MAX_LEN] = {0};
	char pwd[USERPWD_MAX_LEN] = {0};
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get system.@system[0].username -q", username, sizeof(username));
			shell_get("uci get system.@system[0].password -q", pwd, sizeof(pwd));
			sprintf( cmd_para_buf, "\r\n+OK=%s,%s\r\n", username, pwd);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 1 ) || ( pParam->para_sum == 2 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)username, sizeof(username), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 2, pParam->para_str, (void*)pwd, sizeof(pwd), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;					
				}
				
				if ( pParam->para_sum == 2 )
				{
					sprintf( cmd_para_buf, "uci set system.@system[0].username='%s' -q", username );
					system(cmd_para_buf);					
					sprintf( cmd_para_buf, "uci set system.@system[0].password='%s' -q", pwd );
					system(cmd_para_buf);
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;										
				}
				
				////Param_SaveWorkParam();
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_WEBPORT(AT_CMD_PARA* pParam)
{
	char webport[16] = {0};
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get httpd.main.http_port -q", webport, sizeof(webport));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", webport);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 1 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)webport, sizeof(webport), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				
				if ( pParam->para_sum == 1 )
				{
					sprintf( cmd_para_buf, "uci set httpd.main.http_port='%s' -q", webport );
					system(cmd_para_buf);
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;										
				}
				
				////Param_SaveWorkParam();
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_DNS(AT_CMD_PARA* pParam)
{
	char dns[16] = {0};
	char dns2[16] = {0};
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get network.lan.dns -q", dns, sizeof(dns));
			shell_get("uci get network.lan.dns2 -q", dns2, sizeof(dns2));
			if(strcmp(dns2, "")==0)
			{
				sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", dns);
			}
			else{
				sprintf( cmd_para_buf, "\r\n+OK=%s,%s\r\n", dns, dns2);
			}
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 1 ) || ( pParam->para_sum == 2 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)dns, sizeof(dns), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 2, pParam->para_str, (void*)dns2, sizeof(dns2), PARA_TYPE_STRING ) )
				{
					//strcpy( cmd_para_buf, ACK_ERR4 );
					//break;					
				}
				
				if ( (pParam->para_sum == 2 ) || (pParam->para_sum == 1))
				{
					sprintf( cmd_para_buf, "uci set network.lan.dns='%s' -q", dns );
					system(cmd_para_buf);
					sprintf( cmd_para_buf, "uci set network.lan.dns2='%s' -q", dns2 );
					system(cmd_para_buf);
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;										
				}
				
				////Param_SaveWorkParam();
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
	
}


void AtCmd_Handler_UART(AT_CMD_PARA* pParam)
{
	char tmp[10];
	char baud[16] = {0};
	char databit[16] = {0};
	char stopbit[16] = {0};
	char flow[16] = {0};
	char parity[16] = {0};
	unsigned int temp = 0;
	int portnum = 0;
	char cmd_buf[128] = {0};
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmd_buf, "uci get ser2net%d.@ser2net[0].baud -q", pParam->portnum);
			shell_get(cmd_buf, baud, sizeof(baud));
			sprintf(cmd_buf, "uci get ser2net%d.@ser2net[0].databit -q", pParam->portnum);
			shell_get(cmd_buf, databit, sizeof(databit));
			sprintf(cmd_buf, "uci get ser2net%d.@ser2net[0].stopbit -q", pParam->portnum);
			shell_get(cmd_buf, stopbit, sizeof(stopbit));
			sprintf(cmd_buf, "uci get ser2net%d.@ser2net[0].paritybit -q", pParam->portnum);
			shell_get(cmd_buf, parity, sizeof(parity));

			sprintf( cmd_para_buf, "\r\n+OK=%s,%s,%s,%s,%s\r\n", baud, databit, stopbit, parity, "NFC"); // baud rate

			break;
		}
		
		case AT_CMD_SET:
		{
			if ( (pParam->para_sum != 5 ) && (pParam->para_sum != 4))
			{
				strcpy( cmd_para_buf, ACK_ERR3 );
				break;
			}
			// valid param num 
			if( !AtCmd_GetPara( 1, pParam->para_str, (void*)(baud), sizeof( baud ), PARA_TYPE_STRING ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			}
			temp = atoi(baud);
			if ( (temp < BAUD_MIN_SIZE) || (temp > BAUD_MAX_SIZE) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			}
			sprintf( cmd_para_buf, "uci set ser2net%d.@ser2net[0].baud='%s' -q", pParam->portnum, baud); 
			system(cmd_para_buf);
			//printf("at set portnum: %d \n",  pParam->portnum);
			
			if( !AtCmd_GetPara( 2, pParam->para_str, (void*)(databit), sizeof( databit ), PARA_TYPE_STRING ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			}
			temp = atoi(databit);
			if ( (temp < UART_DATA_BITS_5) || (temp > UART_DATA_BITS_8) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			}
			
			sprintf( cmd_para_buf, "uci set ser2net%d.@ser2net[0].databit='%s' -q", pParam->portnum, databit); 
			system(cmd_para_buf);
			
			if( !AtCmd_GetPara( 3, pParam->para_str, (void*)(stopbit), sizeof( stopbit ), PARA_TYPE_STRING ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			}
			temp = atoi(stopbit);
			if ( (temp < UART_STOP_BITS_1) || (temp > UART_STOP_BITS_2) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			}
			sprintf( cmd_para_buf, "uci set ser2net%d.@ser2net[0].stopbit='%s' -q", pParam->portnum, stopbit); 
			system(cmd_para_buf);
			
			if( !AtCmd_GetPara( 4, pParam->para_str, (void*)(parity), sizeof( parity ), PARA_TYPE_STRING ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			}
			if ( !strncasecmp( parity, "NONE", strlen("NONE")) || !strncasecmp( parity, "Odd", strlen("Odd"))
					|| !strncasecmp( parity, "Even", strlen("Even")) || !strncasecmp( parity, "Mark", strlen("Mark"))
					|| !strncasecmp( parity, "Space", strlen("Space")))
			{
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			}
//#include "string.h"			
			
			strupr(parity);
			sprintf( cmd_para_buf, "uci set ser2net%d.@ser2net[0].paritybit='%s' -q", pParam->portnum, parity); 
			system(cmd_para_buf);
			
			////Param_SaveWorkParam();
			strcpy( cmd_para_buf, ACK_OK );
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_UARTTL(AT_CMD_PARA* pParam)
{
	unsigned int temp = 0;
	int portnum = 0;
	char packtime[16] = {0};
	char packlen[16] = {0};
	char cmdbuf[128] = {0};
		
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].packtime -q", pParam->portnum);
			shell_get(cmdbuf, packtime, sizeof(packtime));
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].packlen -q", pParam->portnum);
			shell_get(cmdbuf, packlen, sizeof(packlen));

			sprintf( cmd_para_buf, "\r\n+OK=%s,%s\r\n",  packtime, packlen); // 

			break;
		}
		
		case AT_CMD_SET:
		{
			if ( (pParam->para_sum != 2 ))
			{
				strcpy( cmd_para_buf, ACK_ERR3 );
				break;
			}
			// valid param num 
			if( !AtCmd_GetPara( 1, pParam->para_str, (void*)(packtime), sizeof( packtime ), PARA_TYPE_STRING ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			}
			temp = atoi(packtime);
			if ( (temp > 1000) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			}			
			sprintf( cmd_para_buf, "uci set ser2net%d.@ser2net[0].packtime='%s' -q", pParam->portnum, packtime); 
			system(cmd_para_buf);
			
			if( !AtCmd_GetPara( 2, pParam->para_str, (void*)(packlen), sizeof( packlen ), PARA_TYPE_STRING ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			}
			temp = atoi(packlen);
			if ( (temp > 1500) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			}
			sprintf( cmd_para_buf, "uci set ser2net%d.@ser2net[0].packlen='%s' -q", pParam->portnum, packlen); 
			system(cmd_para_buf);
			
			////Param_SaveWorkParam();
			strcpy( cmd_para_buf, ACK_OK );
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_RFCEN(AT_CMD_PARA* pParam)
{
	char rfcen[16] = {0};
	unsigned int temp = 0;
	char cmd_buf[128] = {0};

	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			// pParam->portnum
			sprintf(cmd_buf, "uci get ser2net%d.@ser2net[0].syncbaud -q", pParam->portnum);
			shell_get(cmd_buf, rfcen, sizeof(rfcen));			
			temp = atoi(rfcen);
			printf("temp is %02X\r\n",temp);
			if ( temp )
			{
				sprintf( cmd_para_buf, "\r\n+OK=ON\r\n" );
			}
			else
			{
				sprintf( cmd_para_buf, "\r\n+OK=OFF\r\n" );
			}
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum != 1 ) || !AtCmd_GetPara( 1, pParam->para_str, (void*)cmd_para_buf, sizeof( cmd_para_buf ), 	PARA_TYPE_STRING ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				if ( strnstr( cmd_para_buf, "ON", strlen("ON") ) && ( strlen( cmd_para_buf ) == 2 ) )
				{
					sprintf(cmd_buf, "uci set ser2net%d.@ser2net[0].syncbaud=1 -q", pParam->portnum);
					
					system(cmd_buf);
					strcpy( cmd_para_buf, ACK_OK );
				}
				else if ( strnstr( cmd_para_buf, "OFF", strlen("OFF") ) && ( strlen( cmd_para_buf ) == 3 ) )
				{
					sprintf(cmd_buf, "uci set ser2net%d.@ser2net[0].syncbaud=0 -q", pParam->portnum);
					
					system(cmd_buf);
					strcpy( cmd_para_buf, ACK_OK );
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
				}
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_WANN(AT_CMD_PARA* pParam)
{
	char ipmode[16] = {0};
	char ipaddr[32] = {0};
	char netmask[32] = {0};
	char gateway[32] = {0};
	char cmdbuf[UCI_CMD_SIZE] = {0};
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get -q network.lan.proto", ipmode, sizeof(ipmode));
			if(strncasecmp(ipmode, "DHCP", strlen("DHCP"))==0)
			{
				shell_get("ifconfig br0 | grep 'inet addr' | awk '{print $2}' | awk -F 'addr:' '{print $2}'", ipaddr, sizeof(ipaddr));
				shell_get("ifconfig br0 | grep 'inet addr' | awk '{print $4}' | awk -F 'Mask:' '{print $2}'", netmask, sizeof(netmask));
				shell_get("route -n | sed -n '3p' | awk '{print $2}'", gateway, sizeof(gateway));
			}
			else
			{
				shell_get("uci get -q network.lan.ipaddr", ipaddr, sizeof(ipaddr));
				shell_get("uci get -q network.lan.netmask", netmask, sizeof(netmask)); 
				shell_get("uci get -q network.lan.gateway", gateway, sizeof(gateway));
			}
			sprintf( cmd_para_buf, "\r\n+OK=%s,%s,%s,%s\r\n", ipmode, ipaddr, netmask, gateway );
			break;
		}
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 1 ) && AtCmd_GetPara( 1, pParam->para_str, (void*)ipmode, sizeof( ipmode ), PARA_TYPE_STRING ) )
			{
				if(strncasecmp(ipmode, "dhcp", strlen("dhcp")))
				{
					strcpy( cmd_para_buf, ACK_ERR_DBG );
					break;
				}
				strupr(ipmode);				
				
				sprintf(cmdbuf, "uci set network.lan.proto='%s' -q", ipmode);
				system(cmdbuf);
				strcpy( cmd_para_buf, ACK_OK );
			}
			else if( ( pParam->para_sum == 4 ))
			{
				AtCmd_GetPara( 1, pParam->para_str, (void*)ipmode, sizeof( ipmode ), PARA_TYPE_STRING ) ;
				strupr(ipmode);				
				sprintf(cmdbuf, "uci set network.lan.proto='%s' -q", ipmode);
				system(cmdbuf);
				AtCmd_GetPara( 2, pParam->para_str, (void*)ipaddr, sizeof( ipaddr ), PARA_TYPE_STRING ) ;
				sprintf(cmdbuf, "uci set network.lan.ipaddr='%s' -q", ipaddr);
				system(cmdbuf);
				AtCmd_GetPara( 3, pParam->para_str, (void*)netmask, sizeof( netmask ), PARA_TYPE_STRING ) ;
				sprintf(cmdbuf, "uci set network.lan.netmask='%s' -q", netmask);
				system(cmdbuf);
				AtCmd_GetPara( 4, pParam->para_str, (void*)gateway, sizeof( gateway ), PARA_TYPE_STRING ) ;
				sprintf(cmdbuf, "uci set network.lan.gateway='%s' -q", gateway);
				system(cmdbuf);
				
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_SOCKA(AT_CMD_PARA* pParam)
{
	char tmp[10];
	uint16_t num;
	char shellbuf[SOCK_MAX_ADDR_LEN] = {0};
	char workmode[10] = {0};
	char remoteaddr[SOCK_MAX_ADDR_LEN] = {0};	
	char port[10] = {0};
	int sockettype = 0;
		
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(shellbuf, "uci get -q ser2net%d.@ser2net[0].workmode -q", pParam->portnum);
			shell_get(shellbuf, workmode, sizeof(workmode));
			sprintf(shellbuf, "uci get -q ser2net%d.@ser2net[0].remoteaddr -q", pParam->portnum);
			shell_get(shellbuf, remoteaddr, sizeof(remoteaddr));
			if((strncasecmp(workmode, "TCPS", strlen("TCPS"))==0) || (strncasecmp(workmode, "UDPS", strlen("UDPS"))==0))
			{
				sprintf(shellbuf, "uci get -q ser2net%d.@ser2net[0].localport -q", pParam->portnum);
				shell_get(shellbuf, port, sizeof(port));
			}
			else
			{
				sprintf(shellbuf, "uci get -q ser2net%d.@ser2net[0].remoteport -q", pParam->portnum);
				shell_get(shellbuf, port, sizeof(port));
			}
			
			sprintf( cmd_para_buf, "\r\n+OK=%s,%s,%s\r\n", workmode, remoteaddr, port); // remote port
			break;
		}
		
		case AT_CMD_SET:
		{			
			if ( pParam->para_sum != 3 )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				if( !AtCmd_GetPara( 1, pParam->para_str, (void*)(workmode), sizeof( workmode ), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				
				if ( strnstr( workmode, "TCPC", strlen("TCPC") ) || strnstr( workmode, "HTPC", strlen("HTPC") )
					 || strnstr( workmode, "SSLC", strlen("SSLC") ) || strnstr( workmode, "TCPS", strlen("TCPS") )
					|| strnstr( workmode, "UDPC", strlen("UDPC") ) || strnstr( workmode, "UDPS", strlen("UDPS") ))
				{
					sprintf(shellbuf, "uci set ser2net%d.@ser2net[0].workmode='%s' -q", pParam->portnum, workmode);
					system(shellbuf);					
				}							
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				
				if( !AtCmd_GetPara( 2, pParam->para_str, (void*)(remoteaddr), sizeof( remoteaddr ), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}				
				sprintf(shellbuf, "uci set ser2net%d.@ser2net[0].remoteaddr='%s' -q", pParam->portnum, remoteaddr);
				system(shellbuf);
				
				if( !AtCmd_GetPara( 3, pParam->para_str, (void*)(port), sizeof( port ), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				num = atoi(port);
				if( (num < 1) || (num > 65535))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				if((strncasecmp(workmode, "TCPS", strlen("TCPS"))==0) || (strncasecmp(workmode, "UDPS", strlen("UDPS"))==0))
				{
					sprintf(shellbuf, "uci set ser2net%d.@ser2net[0].localport='%s' -q", pParam->portnum, port);		
				}
				else
				{
					sprintf(shellbuf, "uci set ser2net%d.@ser2net[0].remoteport='%s' -q", pParam->portnum, port);		
				}
				system(shellbuf);
				////Param_SaveWorkParam();
				
				strcpy( cmd_para_buf, ACK_OK );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_SOCKB(AT_CMD_PARA* pParam)
{
	char tmp[10];
	uint16_t num;
	char shellbuf[SOCK_MAX_ADDR_LEN] = {0};
	char workmode[10] = {0};
	char remoteaddr[SOCK_MAX_ADDR_LEN] = {0};	
	char port[10] = {0};
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(shellbuf, "uci get -q ser2net%d.@ser2net[0].workmodeb -q", pParam->portnum);
			shell_get(shellbuf, workmode, sizeof(workmode));
			sprintf(shellbuf, "uci get -q ser2net%d.@ser2net[0].remoteaddrb -q", pParam->portnum);
			shell_get(shellbuf, remoteaddr, sizeof(remoteaddr));
			sprintf(shellbuf, "uci get -q ser2net%d.@ser2net[0].remoteportb -q", pParam->portnum);
			shell_get(shellbuf, port, sizeof(port));
			
			sprintf( cmd_para_buf, "\r\n+OK=%s,%s,%s\r\n", workmode, remoteaddr, port); // remote port
			break;
		}
		
		case AT_CMD_SET:
		{			
			if ( pParam->para_sum != 3 )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				if( !AtCmd_GetPara( 1, pParam->para_str, (void*)(workmode), sizeof( workmode ), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				
				if ( strnstr( workmode, "TCPC", strlen("TCPC") ) || strnstr( workmode, "HTPC", strlen("HTPC") )
					 || strnstr( workmode, "SSLC", strlen("SSLC") ) || strnstr( workmode, "TCPS", strlen("TCPS") )
					|| strnstr( workmode, "UDPC", strlen("UDPC") ) || strnstr( workmode, "UDPS", strlen("UDPS") ))
				{
					sprintf(shellbuf, "uci set ser2net%d.@ser2net[0].workmodeb='%s' -q", pParam->portnum, workmode);
					system(shellbuf);					
				}							
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				
				if( !AtCmd_GetPara( 2, pParam->para_str, (void*)(remoteaddr), sizeof( remoteaddr ), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}				
				sprintf(shellbuf, "uci set ser2net%d.@ser2net[0].remoteaddrb='%s' -q", pParam->portnum, remoteaddr);
				system(shellbuf);
				
				if( !AtCmd_GetPara( 3, pParam->para_str, (void*)(port), sizeof( port ), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				num = atoi(port);
				if( (num < 1) || (num > 65535))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				
				sprintf(shellbuf, "uci set ser2net%d.@ser2net[0].remoteportb='%s' -q", pParam->portnum, port);		
				system(shellbuf);
				////Param_SaveWorkParam();
				strcpy( cmd_para_buf, ACK_OK );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_SOCKPORTA(AT_CMD_PARA* pParam)
{
	char tmp[10];
	uint16_t num;
	char shellbuf[SOCK_MAX_ADDR_LEN] = {0};
	char port[10] = {0};
	int sockettype = 0;
		
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(shellbuf, "uci get -q ser2net%d.@ser2net[0].localport -q", pParam->portnum);
			shell_get(shellbuf, port, sizeof(port));
			
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", port); // remote port
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( pParam->para_sum != 1 )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				if( !AtCmd_GetPara( 1, pParam->para_str, (void*)(port), sizeof( port ), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				num = atoi(port);
				if( (num < 1) || (num > 65535))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				
				sprintf(shellbuf, "uci set ser2net%d.@ser2net[0].localport='%s' -q", pParam->portnum, port);		
				system(shellbuf);
				////Param_SaveWorkParam();
				
				strcpy( cmd_para_buf, ACK_OK );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_SOCKPORTB(AT_CMD_PARA* pParam)
{
	char tmp[10];
	uint16_t num;
	char shellbuf[SOCK_MAX_ADDR_LEN] = {0};
	char port[10] = {0};
	int sockettype = 0;
		
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(shellbuf, "uci get -q ser2net%d.@ser2net[0].localportb -q", pParam->portnum);
			shell_get(shellbuf, port, sizeof(port));
			
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", port); // remote port
			break;
		}
		
		case AT_CMD_SET:
		{			
			if ( pParam->para_sum != 1 )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				if( !AtCmd_GetPara( 1, pParam->para_str, (void*)(port), sizeof( port ), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				num = atoi(port);
				if( (num < 1) || (num > 65535))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				
				sprintf(shellbuf, "uci set ser2net%d.@ser2net[0].localportb='%s' -q", pParam->portnum, port);		
				system(shellbuf);
				////Param_SaveWorkParam();
				
				strcpy( cmd_para_buf, ACK_OK );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_SOCKALK(AT_CMD_PARA* pParam)
{
	char buf[16];
	char cmdbuf[UCI_CMD_SIZE] = {0};
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "cat /tmp/ser2net%d.status | grep socketa | awk '{print $2}'", pParam->portnum);
			shell_get(cmdbuf, buf, sizeof(buf));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", buf );
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_SOCKBLK(AT_CMD_PARA* pParam)
{
	char buf[16];
	char cmdbuf[UCI_CMD_SIZE] = {0};
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "cat /tmp/ser2net%d.status | grep socketb | awk '{print $2}'", pParam->portnum);
			shell_get(cmdbuf, buf, sizeof(buf));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", buf );
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_REGEN(AT_CMD_PARA* pParam)
{
	char buf[16] = {0};
	char cmdbuf[UCI_CMD_SIZE] = {0};
	unsigned int temp = 0;
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].loginen -q", pParam->portnum);
			shell_get(cmdbuf, buf, sizeof(buf));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", buf );
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( pParam->para_sum != 1 )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				AtCmd_GetPara( 1, pParam->para_str, (void*)buf, sizeof( buf ), PARA_TYPE_STRING );
				strupr(buf);
				sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].loginen='%s' -q", pParam->portnum, buf);
				system(cmdbuf);
				strcpy( cmd_para_buf, ACK_OK );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_REGTCP(AT_CMD_PARA* pParam)
{
	uint16_t num16;
	char buf[16] = {0};
	unsigned int temp = 0;
	char cmdbuf[UCI_CMD_SIZE] = {0};
	
	switch( pParam->type )
	{ 
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].loginwhen -q" , pParam->portnum);			
			shell_get(cmdbuf, buf, sizeof(buf));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n",buf );
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum != 1 ) ) 
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				AtCmd_GetPara( 1, pParam->para_str, (void*)(buf), sizeof(buf), PARA_TYPE_STRING );
				strupr(buf);
				sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].loginwhen='%s' -q" , pParam->portnum, buf);
				system(cmdbuf);
				//g_SysParam.para.reg_pack.d2d_id = num16;
				//Param_SaveWorkParam();
				strcpy( cmd_para_buf, ACK_OK );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_REGCLOUD(AT_CMD_PARA* pParam)
{
	char user[USR_PASS_CLOUD_ID_SIZE+1] = {0};
	char pwd[USR_PASS_CLOUD_PW_SIZE+1] = {0};
	char temp[30];
	char cmdbuf[UCI_CMD_SIZE] = {0};
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].clouduser -q", pParam->portnum);
			shell_get(cmdbuf, user, sizeof(user));		
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].cloudpwd -q", pParam->portnum);
			shell_get(cmdbuf, pwd, sizeof(pwd));												
			sprintf( cmd_para_buf, "\r\n+OK=%s,%s\r\n", user, pwd );
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum != 2 ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			}
			else
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)user, sizeof(user), PARA_TYPE_STRING ) || (strlen( user ) != USR_PASS_CLOUD_ID_SIZE) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].clouduser='%s' -q", pParam->portnum, user );
				system(cmdbuf);
				if ( !AtCmd_GetPara( 2, pParam->para_str, (void*)pwd, sizeof(pwd), PARA_TYPE_STRING ) || (strlen( pwd ) != USR_PASS_CLOUD_PW_SIZE ))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].cloudpwd='%s' -q", pParam->portnum, pwd );
				system(cmdbuf);

				strcpy( cmd_para_buf, ACK_OK );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_REGUSR(AT_CMD_PARA* pParam)
{
	char temp[80];
	char cmdbuf[UCI_CMD_SIZE] = {0};
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].loginpkt -q", pParam->portnum);
			shell_get(cmdbuf, temp, sizeof(temp));		
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", temp );
			break;
		}
		 
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum != 1 ) && ( pParam->para_sum != 2 ))
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			}
			else
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)temp, sizeof(temp), PARA_TYPE_STRING ))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].loginpkt='%s' -q", pParam->portnum, temp );
				system(cmdbuf);

				strcpy( cmd_para_buf, ACK_OK );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_REGUSER(AT_CMD_PARA* pParam)
{
	char temp[80];
	char aschex[16];
	char cmdbuf[UCI_CMD_SIZE] = {0};
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].loginpkt -q", pParam->portnum);
			shell_get(cmdbuf, temp, sizeof(temp));
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].logintype -q", pParam->portnum);
			shell_get(cmdbuf, aschex, sizeof(aschex));
			sprintf( cmd_para_buf, "\r\n+OK=%s,%s\r\n", temp, aschex );
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum != 1 ) && ( pParam->para_sum != 2 ))
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			} 
			else
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)aschex, sizeof(aschex), PARA_TYPE_STRING ))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].loginpkt='%s' -q", pParam->portnum, aschex );
				system(cmdbuf);
				if ( !AtCmd_GetPara( 2, pParam->para_str, (void*)aschex, sizeof(aschex), PARA_TYPE_STRING ))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].logintype='%s' -q", pParam->portnum, aschex );
				system(cmdbuf);

				strcpy( cmd_para_buf, ACK_OK );
			} 
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_HEARTEN(AT_CMD_PARA* pParam)
{
	char buf[HEART_DATA_MAX_LEN] = {0};
	unsigned int temp = 0;
	char cmdbuf[UCI_CMD_SIZE] = {0};
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].hearten -q", pParam->portnum);
			shell_get(cmdbuf, buf, sizeof(buf));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", buf);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum != 1 ) || !AtCmd_GetPara( 1, pParam->para_str, (void*)buf, sizeof( buf ), PARA_TYPE_STRING ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				if ( (strncasecmp( buf, "ON", strlen("ON") )==0) 
						|| (strncasecmp( buf, "OFF", strlen("OFF") )==0) )
				{
					strupr(buf);
					sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].hearten='%s' -q", pParam->portnum, buf);
					system(cmdbuf);					
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;					
				}
				strcpy( cmd_para_buf, ACK_OK );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_HEARTDT(AT_CMD_PARA* pParam)
{
	char buf[HEART_DATA_MAX_LEN] = {0};
	unsigned int temp = 0;
	char cmdbuf[UCI_CMD_SIZE] = {0};

	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].heartpkt -q", pParam->portnum);
			shell_get(cmdbuf, buf, sizeof(buf));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", buf );
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 1 ) )
			{
				AtCmd_GetPara( 1, pParam->para_str, (void*)buf, HEART_DATA_MAX_LEN, PARA_TYPE_STRING);
				sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].heartpkt='%s' -q", pParam->portnum, buf );
				system(cmdbuf);
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_HEARTUSER(AT_CMD_PARA* pParam)
{
	char buf[HEART_DATA_MAX_LEN] = {0};
	unsigned int temp = 0;
	char cmdbuf[UCI_CMD_SIZE] = {0};
	char aschex[16] = {0};

	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].heartpkt -q", pParam->portnum);
			shell_get(cmdbuf, buf, sizeof(buf));
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].heart_aschex -q", pParam->portnum);
			shell_get(cmdbuf, aschex, sizeof(aschex));
			sprintf( cmd_para_buf, "\r\n+OK=%s,%s\r\n", buf, aschex );
			break;
		}
		
		case AT_CMD_SET:
		{ 
			if ( ( pParam->para_sum != 1 ) && ( pParam->para_sum != 2 ))
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
				break;
			}
			else
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)aschex, sizeof(aschex), PARA_TYPE_STRING ))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].heartpkt='%s' -q", pParam->portnum, aschex );
				system(cmdbuf);
				if ( !AtCmd_GetPara( 2, pParam->para_str, (void*)aschex, sizeof(aschex), PARA_TYPE_STRING ))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].heart_aschex='%s' -q", pParam->portnum, aschex );
				system(cmdbuf);

				strcpy( cmd_para_buf, ACK_OK );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_HEARTTP(AT_CMD_PARA* pParam)
{
	char buf[16] = {0};
	unsigned int temp = 0;
	char cmdbuf[UCI_CMD_SIZE] = {0};
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].hearttp -q", pParam->portnum);
			shell_get(cmdbuf, buf, sizeof(buf));
			
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", buf );
			break;
		}
		 
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum != 1 ) || !AtCmd_GetPara( 1, pParam->para_str, (void*)buf, sizeof( buf ), PARA_TYPE_STRING ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				if((strncasecmp(buf, "net", strlen("net"))==0) || (strncasecmp(buf, "com", strlen("com"))==0))
				{
					strupr(buf);
					sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].hearttp='%s' -q", pParam->portnum, buf );
					system(cmdbuf);
					strcpy( cmd_para_buf, ACK_OK );
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
				}				
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_HEARTTM(AT_CMD_PARA* pParam)
{
	uint32_t num;
	char buf[16] = {0};
	unsigned int temp = 0;
	char cmdbuf[128] = {0};
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].hearttime -q", pParam->portnum);
			shell_get(cmdbuf, buf, sizeof(buf));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", buf );
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum != 1 ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				AtCmd_GetPara( 1, pParam->para_str, (void*)buf, sizeof(buf), PARA_TYPE_STRING );
				num = atoi(buf);
				
				if ( (num >= 1) && (num <= HEART_MAX_PERIOD) )
				{
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				
				sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].hearttime='%s' -q", pParam->portnum, buf );
				system(cmdbuf);
				strcpy( cmd_para_buf, ACK_OK );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_HTPTP(AT_CMD_PARA* pParam)
{
	char buf[16] = {0};
	unsigned int temp = 0;
	char cmdbuf[128] = {0};

	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{	
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].htptp -q", pParam->portnum);
			shell_get(cmdbuf, buf, sizeof(buf));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n",  buf);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum != 1 ) || !AtCmd_GetPara( 1, pParam->para_str, (void*)buf, sizeof( buf ), PARA_TYPE_STRING ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				strupr(buf);
				if ( strnstr( buf, "GET", strlen("GET") ) || strnstr( buf, "POST", strlen("POST") ))
				{

				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;					
				}
				sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].htptp='%s' -q", pParam->portnum, buf );
				system(cmdbuf);
				strcpy( cmd_para_buf, ACK_OK );				
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_HTPURL(AT_CMD_PARA* pParam)
{
	char buf[HTTP_MAX_URL_LEN] = {0};
	unsigned int temp = 0;
	char cmdbuf[UCI_CMD_SIZE] = {0};

	//printf("htpurl sum: %d type: %d \n", pParam->para_sum, pParam->type);

	switch ( pParam->type )
	{ 
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].htpurl -q", pParam->portnum);
			shell_get(cmdbuf, buf, sizeof(buf));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", buf );
			break;
		}
		  
		case AT_CMD_SET:
		{ 
				printf("htpurl sum: %d \n", pParam->para_sum);
		
			if ( ( pParam->para_sum != 1 ) || !AtCmd_GetPara( 1, pParam->para_str, (void*)buf, sizeof(buf), PARA_TYPE_STRING ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				printf("htpurl get: %s \n", buf);
				sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].htpurl='%s' -q", pParam->portnum, buf );
				system(cmdbuf);
				//memset(cmd_para_buf, 0, sizeof(cmd_para_buf));
				strcpy( cmd_para_buf, ACK_OK );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_HTPHD(AT_CMD_PARA* pParam)
{
	int len;
	char buf[HTTP_MAX_URL_LEN] = {0};
	unsigned int temp = 0;
	char cmdbuf[UCI_CMD_SIZE] = {0};
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].htphead -q", pParam->portnum);
			shell_get(cmdbuf, buf, sizeof(buf));			
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", buf );
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum != 1 )  )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				AtCmd_GetPara( 1, pParam->para_str, (void*)buf, sizeof(buf), PARA_TYPE_STRING );
				len = strlen( buf );
				
				if ( ( (len > 0) && ( len < 9 ) )  )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				
				//sprintf( g_SysParam.para.httpd.head, "%s", cmd_para_buf );
				sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].htphead='%s' -q", pParam->portnum, buf );
				system(cmdbuf);
				
				strcpy( cmd_para_buf, ACK_OK );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_HTPCHD(AT_CMD_PARA* pParam)
{
	char buf[16] = {0};
	unsigned int temp = 0;
	char cmdbuf[128] = {0};
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf(cmdbuf, "uci get ser2net%d.@ser2net[0].htpchd -q", pParam->portnum);
			shell_get(cmdbuf, buf, sizeof(buf));						
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", buf );
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum != 1 ) || !AtCmd_GetPara( 1, pParam->para_str, (void*)buf, sizeof( buf ), PARA_TYPE_STRING ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				strupr(buf);
				if ( (strncasecmp( buf, "ON", strlen("ON"))==0 ) || (strncasecmp( buf, "OFF", strlen("OFF"))==0 ))
				{
					sprintf(cmdbuf, "uci set ser2net%d.@ser2net[0].htpchd='%s' -q", pParam->portnum, buf );
					system(cmdbuf);
					strcpy( cmd_para_buf, ACK_OK );
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
				}
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}




void AtCmd_Handler_PLANG(AT_CMD_PARA* pParam)
{
	char buf[16] = {0};
	unsigned int temp = 0;
	char cmdbuf[128] = {0};

	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get system.@system[0].lang -q", buf, sizeof(buf));												
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", buf );
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum != 1 ) || !AtCmd_GetPara( 1, pParam->para_str, (void*)buf, sizeof( buf ), PARA_TYPE_STRING ) )
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			else
			{
				strupr(buf);
				if ( strlen(buf) > 0 && strlen(buf) < sizeof(buf) )
				{
					sprintf(cmdbuf, "uci set system.@system[0].lang='%s' -q", buf );
					system(cmdbuf);
					strcpy( cmd_para_buf, ACK_OK );
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
				}
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_UPTIME(AT_CMD_PARA* pParam)
{
	char buf[128];
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("cat /proc/uptime| awk -F. '{run_days=$1 / 86400;run_hour=($1 % 86400)/3600;run_minute=($1 % 3600)/60;run_second=$1 % 60;printf \"%ds,%dd:%dh:%dm:%ds\",$1,run_days,run_hour,run_minute,run_second }'", 
			buf, sizeof(buf));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", buf );
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_H(AT_CMD_PARA* pParam)
{
	char buf[128];
	int i = 0;
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf( cmd_para_buf, "\r\n+OK=");

			for(i=0; (g_CommandTable[i].pInfo != '\0' ) && ((strlen(cmd_para_buf)+1024) < sizeof(cmd_para_buf)); i++)
			{
				sprintf( cmd_para_buf, "%s%s-%s\r\n", cmd_para_buf, g_CommandTable[i].pName, g_CommandTable[i].pInfo );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_SEARCH(AT_CMD_PARA* pParam)
{
	char port[128];
	char cmd[128] = {0};
	char cmdbuf[128] = {0};
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get system.search.port -q",  port, sizeof(port));
			shell_get("uci get system.search.cmd -q",  cmd, sizeof(cmd));
			sprintf( cmd_para_buf, "\r\n+OK=%s,%s\r\n", port, cmd );
			break;
		}
		
		case AT_CMD_SET:		
			if ( ( pParam->para_sum == 1 ) || ( pParam->para_sum == 2 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)port, sizeof(port), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 2, pParam->para_str, (void*)cmd, sizeof(cmd), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;					
				}
				
				if ( pParam->para_sum == 2 )
				{
					sprintf( cmdbuf, "uci set system.search.port='%s' -q", port );
					system(cmdbuf);
					sprintf( cmdbuf, "uci set system.search.cmd='%s' -q", cmd );
					system(cmdbuf);
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;										
				}
				
				////Param_SaveWorkParam();
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
		break;
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_RSTIM(AT_CMD_PARA* pParam)
{
	char timeout[128];
	char cmdbuf[128] = {0};
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get system.reset.timeout -q",  timeout, sizeof(timeout));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", timeout );
			break;
		}
		
		case AT_CMD_SET:		
			if ( ( pParam->para_sum == 1 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)timeout, sizeof(timeout), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				
				if ( pParam->para_sum == 1 )
				{
					sprintf( cmdbuf, "uci set system.reset.timeout='%s' -q", timeout );
					system(cmdbuf);
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;										
				}
				
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
		break;
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_MID(AT_CMD_PARA* pParam)
{
	char hostname[128];
	char cmdbuf[128] = {0};
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get system.@system[0].hostname -q",  hostname, sizeof(hostname));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", hostname );
			break;
		}
		
		case AT_CMD_SET:		
			if ( ( pParam->para_sum == 1 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)hostname, sizeof(hostname), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				
				if ( pParam->para_sum == 1 )
				{
					sprintf( cmdbuf, "uci set system.@system[0].hostname='%s' -q", hostname );
					system(cmdbuf);
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;										
				}
				
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
		break;
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}


// httpd string transform
int ping_test(char *inputstring)
{
	char rep[128] = {0};
	int i = 0;
	int k = 0;
	int fd_file = 0;	
	static char shell_ret[128] = {0};
	
	memset(shell_ret, 0, sizeof(shell_ret));
	
	// write content into filename
	fd_file=open((const char *)"/tmp/ping.input", O_WRONLY|O_CREAT|O_APPEND | O_TRUNC);
	if(fd_file < 0)
	{
		printf("file open fail \n");
		return -1;
	}
	
	write(fd_file, inputstring, strlen(inputstring));
	close(fd_file); 	
		
	// exec shell cmd
	strtok(inputstring, "\r");
	shell_get_noent(inputstring, shell_ret, sizeof(shell_ret));
	
	// write content into filename
	// fd_file=open((const char *)"/tmp/ping.ret", O_WRONLY|O_CREAT|O_APPEND | O_TRUNC);
	// if(fd_file < 0)
	// {
		// printf("file open fail \n");
		// return -1;
	// }
	
	// write(fd_file, shell_ret, strlen(shell_ret));
	
	// close(fd_file); 	
	
	return shell_ret;
	
}


void AtCmd_Handler_PING(AT_CMD_PARA* pParam)
{
	char pingaddr[64] = {0};
	char cmdbuf[256] = {0};
	char pingret[256] = {0};
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf( cmd_para_buf, "\r\n+OK\r\n");
			break;
		}
		
		case AT_CMD_SET:
			if ( ( pParam->para_sum == 1 )  )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)pingaddr, sizeof(pingaddr), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				
				if ( pParam->para_sum == 1 )
				{
					// bytes from
					//sprintf(cmdbuf, "ping -w2 -c2 -q %s | grep received | awk -F ',' '{print $2}' | awk '{print $1}'>&2", pingaddr);
					sprintf(cmdbuf, "ping -w2 -c2 -q www.usr.cn | grep received | awk '{print $4}'");
					printf("at+ping send: %s\n", cmdbuf);
					//system(cmdbuf);
					shell_get_noent(cmdbuf,  pingret, sizeof(pingret));
					printf("at+ping get: %s\n", pingret);
					if(atoi(pingret) > 0)
					{
						strcpy( cmd_para_buf, ACK_OK );
					}
					else
					{
						strcpy( cmd_para_buf, ACK_ERR4 );
					}
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;										
				}
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
		break;
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

// #define FACTORY_PARAMSIZE	64
// #define FACTORY_MAC			0
// #define FACTORY_ENCRYPT		1
// #define FACTORY_MANUF_VER	2
// #define FACTORY_USERVER		3
// static char block_factory[10*KB] = {0};

void AtCmd_Handler_PDTIME(AT_CMD_PARA* pParam)
{
	char buf[FACTORY_PARAMSIZE] = {0};;
	char mac_buf[FACTORY_PARAMSIZE] = {0};
	char enc_buf[FACTORY_PARAMSIZE] = {0};
	int temp = 0;
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			factory_read(block_factory, sizeof(block_factory));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", block_factory + FACTORY_MANUF_VER);
			break;
		}
		
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_WRMAC(AT_CMD_PARA* pParam)
{
	char out_buf[FACTORY_PARAMSIZE] = {0};
	char mac_buf[FACTORY_PARAMSIZE*2] = {0};
	char enc_buf[FACTORY_PARAMSIZE*2] = {0};
	char temp_buf[FACTORY_PARAMSIZE] = {0};
	int i = 0;
	char temp = 0;
	char cmdbuf[128] = {0};
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			factory_read(block_factory, sizeof(block_factory));
			sprintf( cmd_para_buf, "\r\n+OK=%02X%02X%02X%02X%02X%02X\r\n", 
					block_factory[FACTORY_MAC], block_factory[FACTORY_MAC+1],block_factory[FACTORY_MAC+2],
					block_factory[FACTORY_MAC+3],block_factory[FACTORY_MAC+4],block_factory[FACTORY_MAC+5]);
			// write /tmp getmac
			sprintf(out_buf, "echo %02X%02X%02X%02X%02X%02X > /tmp/getmac0", 
					block_factory[FACTORY_MAC], block_factory[FACTORY_MAC+1],block_factory[FACTORY_MAC+2],
					block_factory[FACTORY_MAC+3],block_factory[FACTORY_MAC+4],block_factory[FACTORY_MAC+5]);
			system(out_buf);
			sprintf(out_buf, "echo %02X%02X%02X%02X%02X%02X > /tmp/getmac1", 
					block_factory[FACTORY_MAC], block_factory[FACTORY_MAC+1],block_factory[FACTORY_MAC+2],
					block_factory[FACTORY_MAC+3],block_factory[FACTORY_MAC+4],block_factory[FACTORY_MAC+5]+1);
			system(out_buf);
			
			// check if encrypted
			memcpy(enc_buf, block_factory+FACTORY_MAC, 6);
			swap(enc_buf, 6);
			DecryptInit();
			EncryptBlock(enc_buf, 6);
			if(memcmp(enc_buf, block_factory+FACTORY_ENCRYPT, 6)==0)
			{
				temp = 1;
			}
			else
			{
				temp = 0;
				//system("rm -rf /root/USR_N668");
			}
			sprintf(out_buf, "echo %s > /tmp/getmac0_dec", temp? "yes": "no");
			system(out_buf);
			
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 2 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)mac_buf, sizeof(mac_buf), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 2, pParam->para_str, (void*)enc_buf, sizeof(enc_buf), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				
				if ( pParam->para_sum == 2 )
				{
					// write mac addr & enc 
					factory_read(block_factory, sizeof(block_factory));

					HexString2Asc(mac_buf, out_buf, strlen(mac_buf), sizeof(out_buf));
					memcpy(block_factory+FACTORY_MAC, out_buf, FACTORY_PARAMSIZE);
					
					HexString2Asc(enc_buf, out_buf, strlen(enc_buf), sizeof(out_buf));
					memcpy(block_factory+FACTORY_ENCRYPT, out_buf, FACTORY_PARAMSIZE);
					
					HexString2Asc(mac_buf, temp_buf, strlen(mac_buf), sizeof(temp_buf));
					swap(temp_buf, 6);
					DecryptInit();
					EncryptBlock(temp_buf, 6);
					
					//  check mac and enc correct
					if((strncasecmp(enc_buf, "FFFFFFFFFFFF", strlen("FFFFFFFFFFFF"))==0))
					{
						printf("correct mac&enc fffffffffffff \n");
						//
						factory_write(block_factory, sizeof(block_factory));
						strcpy( cmd_para_buf, ACK_OK );
					}
					else if((memcmp(temp_buf, out_buf, 6)==0) )
					{
						printf("correct mac&enc \n");
						//
						factory_write(block_factory, sizeof(block_factory));
						strcpy( cmd_para_buf, ACK_OK );
					}
					else
					{
						printf("incorrect mac&enc \n");
						strcpy( cmd_para_buf, ACK_ERR4 );					
					}
					
					sprintf(out_buf, "echo %02X%02X%02X%02X%02X%02X > /tmp/getcal0", 
							temp_buf[0], temp_buf[0+1],temp_buf[0+2],
							temp_buf[0+3],temp_buf[0+4],temp_buf[0+5]);
					system(out_buf);
					
					
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;										
				}
				
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_MANUF_VER(AT_CMD_PARA* pParam)
{
	char key[FACTORY_PARAMSIZE] = {0};;
	char manuf_ver[FACTORY_PARAMSIZE] = {0};
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			factory_read(block_factory, sizeof(block_factory));
			//shell_get("cat /etc/ver", key, sizeof(key));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", block_factory + FACTORY_MANUF_VER);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 2 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)key, sizeof(key), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 2, pParam->para_str, (void*)manuf_ver, sizeof(manuf_ver), PARA_TYPE_STRING ) )
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;
				}
				
				if ( (pParam->para_sum == 2) && (strncasecmp(key, "USRWR", strlen("USRWR"))==0) )
				{
					// write mac addr & enc 
					factory_read(block_factory, sizeof(block_factory));

					memcpy(block_factory + FACTORY_MANUF_VER, manuf_ver, FACTORY_PARAMSIZE);

					factory_write(block_factory, sizeof(block_factory));
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;										
				}
				
				////Param_SaveWorkParam();
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_USERVER(AT_CMD_PARA* pParam)
{
	char fir_ver[FACTORY_PARAMSIZE] = {0};
	char sec_ver[FACTORY_PARAMSIZE] = {0};

	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			factory_read(block_factory, sizeof(block_factory));
			//shell_get("cat /etc/ver", sec_ver, sizeof(sec_ver));
			sprintf( cmd_para_buf, "\r\n+OK=%s,general\r\n", block_factory + FACTORY_USERVER);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 1 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)fir_ver, sizeof(fir_ver), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				
				if ( pParam->para_sum == 1 )
				{
					// write mac addr & enc 
					factory_read(block_factory, sizeof(block_factory));

					memcpy(block_factory+FACTORY_USERVER, fir_ver, FACTORY_PARAMSIZE);

					factory_write(block_factory, sizeof(block_factory));
				}
				else
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					break;										
				}
				
				////Param_SaveWorkParam();
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_HWVER(AT_CMD_PARA* pParam)
{
	int temp = 0;
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", "V1.0.x");
			break;
		}
		
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_CLEAR(AT_CMD_PARA* pParam)
{
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			SYS_CLEAR();
			sprintf( cmd_para_buf, "%s", ACK_OK );
			AtCmd_Respond( cmd_para_buf );			
			break;
		}
		
		default:
		{
			sprintf( cmd_para_buf, "%s", ACK_ERR3 );
			AtCmd_Respond( cmd_para_buf );
			break;
		}
	}
}

// { "AT+TRAFFIC", AtCmd_Handler_TRAFFIC, "query/set traffic" },
void AtCmd_Handler_TRAFFIC(AT_CMD_PARA* pParam)
{
	int temp = 0;
	char upstream[32] = {0};
	char downstream[32] = {0};
	static char time_last[32] = {0};
	char time_cur[32] = {0};
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("ifconfig br0 | grep 'RX bytes' | awk '{print $2}' | awk -F ':' '{print $2}'", upstream, sizeof(upstream));
			shell_get("ifconfig br0 | grep 'RX bytes' | awk '{print $6}' | awk -F ':' '{print $2}'",downstream,sizeof(downstream));
			shell_get("date +%s", time_cur, sizeof(time_cur));
			if(strcmp(time_last, "")==0)
			{
				strcpy(time_last, time_cur);
			}
			sprintf( cmd_para_buf, "\r\n+OK=%sB,%sB,%s,%s\r\n", downstream, upstream, time_last, time_cur );
			
			strcpy(time_last, time_cur);
			
			break;
		}
		
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
}
// { "AT+MONITOR", AtCmd_Handler_MONITOR, "query/set monitor enabled" },
// sprintf(pout, "%sAT+MONITOR\\r\\n\\r\\n+MONITOR:on,%s,%d,%d\\r\\n", pout,socket_monitor.serveraddr_mon,socket_monitor.remoteport_mon,socket_monitor.monitor_intvl); 		  
void AtCmd_Handler_MONITOR(AT_CMD_PARA* pParam)
{
	char enabled[16] = {0}; 
	char serveraddr[SOCK_MAX_ADDR_LEN] = {0};
	char port[16] = {0};
	char interval[16] = {0};
	char cmdbuf[UCI_CMD_SIZE] = {0};
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get -q remotem.@remotem[0].menabled", enabled, sizeof(enabled));
			shell_get("uci get -q remotem.@remotem[0].mserveraddr", serveraddr, sizeof(serveraddr));
			shell_get("uci get -q remotem.@remotem[0].mport", port, sizeof(port));
			shell_get("uci get -q remotem.@remotem[0].minterval", interval, sizeof(interval));
			sprintf( cmd_para_buf, "\r\n+OK=%s,%s,%s,%s\r\n", enabled, serveraddr, port, interval);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 4 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)enabled, sizeof(enabled), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 2, pParam->para_str, (void*)serveraddr, sizeof(serveraddr), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 3, pParam->para_str, (void*)port, sizeof(port), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 4, pParam->para_str, (void*)interval, sizeof(interval), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				strupr(enabled);
				sprintf( cmdbuf, "uci set remotem.@remotem[0].menabled='%s' -q", enabled );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set remotem.@remotem[0].mserveraddr='%s' -q", serveraddr );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set remotem.@remotem[0].mport='%s' -q", port );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set remotem.@remotem[0].minterval='%s' -q", interval );
				system(cmdbuf);
				
				////Param_SaveWorkParam();
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}	
	AtCmd_Respond( cmd_para_buf );
}

// { "AT+UPDATE", AtCmd_Handler_UPDATE, "query/set update enabled" },
  // sprintf(pout, "%sAT+UPDATE\\r\\n\\r\\n+UPDATE:on,%s,%d,%d\\r\\n", pout,socket_monitor.serveraddr,socket_monitor.remoteport,socket_monitor.updateinfo_intvl); 		  
void AtCmd_Handler_UPDATE(AT_CMD_PARA* pParam)
{
	char enabled[16] = {0}; 
	char serveraddr[SOCK_MAX_ADDR_LEN] = {0};
	char port[16] = {0};
	char interval[16] = {0};
	char cmdbuf[UCI_CMD_SIZE] = {0};

	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get -q remotem.@remotem[0].uenabled", enabled, sizeof(enabled));
			shell_get("uci get -q remotem.@remotem[0].userveraddr", serveraddr, sizeof(serveraddr));
			shell_get("uci get -q remotem.@remotem[0].uport", port, sizeof(port));
			shell_get("uci get -q remotem.@remotem[0].uinterval", interval, sizeof(interval));
			//shell_get("uci get -q remotem.@remotem[0].mheartpkt", heartpkt, sizeof(mheartpkt));
			//shell_get("uci get -q remotem.@remotem[0].minterval_mon", interval_mon, sizeof(minterval_mon));
			sprintf( cmd_para_buf, "\r\n+OK=%s,%s,%s,%s\r\n", enabled, serveraddr, port, interval);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 4 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)enabled, sizeof(enabled), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 2, pParam->para_str, (void*)serveraddr, sizeof(serveraddr), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 3, pParam->para_str, (void*)port, sizeof(port), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 4, pParam->para_str, (void*)interval, sizeof(interval), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				strupr(enabled);
				sprintf( cmdbuf, "uci set remotem.@remotem[0].uenabled='%s' -q", enabled );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set remotem.@remotem[0].userveraddr='%s' -q", serveraddr );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set remotem.@remotem[0].uport='%s' -q", port );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set remotem.@remotem[0].uinterval='%s' -q", interval );
				system(cmdbuf);
				
				////Param_SaveWorkParam();
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}	
	AtCmd_Respond( cmd_para_buf );
}

// { "AT+LINUXCMP", AtCmd_Handler_LINUXCMP, "query/set linux compatitle command" },
void AtCmd_Handler_LINUXCMP(AT_CMD_PARA* pParam)
{
	char buffer[128] = {0}; 
	char ret[128] = {0}; 
	char cmdbuf[UCI_CMD_SIZE] = {0};

	switch( pParam->type )
	{
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 1 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)buffer, sizeof(buffer), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				//
				shell_get(buffer, ret, sizeof(ret));
				
				sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", ret);
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_NTPTM(AT_CMD_PARA* pParam)
{
	char time[64] = {0};
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			// date "+%Y-%m-%d %H:%M:%S %a"
			shell_get("/etc/date_get", time, sizeof(time));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", time);
			break;
		}		
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
} 
 
void AtCmd_Handler_NTPTML(AT_CMD_PARA* pParam)
{
	char time[64] = {0};
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			// date "+%Y-%m-%d %H:%M:%S %a"
			shell_get("cat /tmp/ntpd_last.log", time, sizeof(time));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", time);
			break;
		}		
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
} 

void AtCmd_Handler_NTPUTC(AT_CMD_PARA* pParam)
{
	char time[64] = {0};
	
	switch ( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			// date "+%Y-%m-%d %H:%M:%S %a"
			shell_get("date +%s", time, sizeof(time));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", time);
			break;
		}		
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	
	AtCmd_Respond( cmd_para_buf );
} 

void AtCmd_Handler_NTPZONE(AT_CMD_PARA* pParam)
{
	char timezone[16] = {0}; 
	char cmdbuf[UCI_CMD_SIZE] = {0};
	
	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get -q system.@system[0].timezone", timezone, sizeof(timezone));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", timezone);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 1 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)timezone, sizeof(timezone), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				strupr(timezone);
				sprintf( cmdbuf, "uci set -q system.@system[0].timezone='%s'", timezone );
				system(cmdbuf);
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_NTPIP(AT_CMD_PARA* pParam)
{
	char ntpserver1[SOCK_MAX_ADDR_LEN] = {0}; 
	char ntpserver2[SOCK_MAX_ADDR_LEN] = {0}; 
	char ntpserver3[SOCK_MAX_ADDR_LEN] = {0}; 
	char cmdbuf[UCI_CMD_SIZE] = {0};

	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get -q system.@system[0].ntpserver1", ntpserver1, sizeof(ntpserver1));
			shell_get("uci get -q system.@system[0].ntpserver2", ntpserver2, sizeof(ntpserver2));
			shell_get("uci get -q system.@system[0].ntpserver3", ntpserver3, sizeof(ntpserver3));
			sprintf( cmd_para_buf, "\r\n+OK=%s,%s,%s\r\n", ntpserver1, ntpserver2, ntpserver3);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 3 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)ntpserver1, sizeof(ntpserver1), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 2, pParam->para_str, (void*)ntpserver2, sizeof(ntpserver2), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				} 
				if ( !AtCmd_GetPara( 3, pParam->para_str, (void*)ntpserver3, sizeof(ntpserver3), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				sprintf( cmdbuf, "uci set system.@system[0].ntpserver1='%s' -q", ntpserver1 );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set system.@system[0].ntpserver2='%s' -q", ntpserver2 );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set system.@system[0].ntpserver3='%s' -q", ntpserver3 );
				system(cmdbuf);
				
				////Param_SaveWorkParam();
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_NTPRF(AT_CMD_PARA* pParam)
{
	char interval[16] = {0}; 
	char cmdbuf[UCI_CMD_SIZE] = {0};

	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get -q system.@system[0].ntpintvl", interval, sizeof(interval));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", interval);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 1 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)interval, sizeof(interval), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				sprintf( cmdbuf, "uci set system.@system[0].ntpintvl='%s' -q", interval );
				system(cmdbuf);
				
				////Param_SaveWorkParam();
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_VPNPPTP(AT_CMD_PARA* pParam)
{
	char enabled[16] = {0}; 
	char serveraddr[SOCK_MAX_ADDR_LEN] = {0}; 
	char username[APN_USER_MAX_LEN] = {0}; 
	char password[APN_USER_MAX_LEN] = {0}; 
	char cmdbuf[UCI_CMD_SIZE] = {0};

	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get -q network.pptp.enabled", enabled, sizeof(enabled));
			shell_get("uci get -q network.pptp.serveraddr", serveraddr, sizeof(serveraddr));
			shell_get("uci get -q network.pptp.username", username, sizeof(username));
			shell_get("uci get -q network.pptp.password", password, sizeof(password));
			sprintf( cmd_para_buf, "\r\n+OK=%s,%s,%s,%s\r\n", enabled, serveraddr, username, password);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 4 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)enabled, sizeof(enabled), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 2, pParam->para_str, (void*)serveraddr, sizeof(serveraddr), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 3, pParam->para_str, (void*)username, sizeof(username), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 4, pParam->para_str, (void*)password, sizeof(password), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				strupr(enabled);
				sprintf( cmdbuf, "uci set network.pptp.enabled='%s' -q", enabled );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set network.pptp.serveraddr='%s' -q", serveraddr );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set network.pptp.username='%s' -q", username );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set network.pptp.password='%s' -q", password );
				system(cmdbuf);
				
				////Param_SaveWorkParam();
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}	
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_VPNPPTPDTL(AT_CMD_PARA* pParam)
{
	char dataencrypt[64] = {0}; 
	char auth[64] = {0}; 
	char lcpecho[16] = {0}; 
	char mtu[16] = {0}; 
	char cmdbuf[UCI_CMD_SIZE] = {0};

	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get -q network.pptp.dataencrypt", dataencrypt, sizeof(dataencrypt));
			shell_get("uci get -q network.pptp.auth", auth, sizeof(auth));
			shell_get("uci get -q network.pptp.lcpecho", lcpecho, sizeof(lcpecho));
			shell_get("uci get -q network.pptp.mtu", mtu, sizeof(mtu));
			sprintf( cmd_para_buf, "\r\n+OK=%s,%s,%s,%s\r\n", dataencrypt, auth, lcpecho, mtu);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 4 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)dataencrypt, sizeof(dataencrypt), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 2, pParam->para_str, (void*)auth, sizeof(auth), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 3, pParam->para_str, (void*)lcpecho, sizeof(lcpecho), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 4, pParam->para_str, (void*)mtu, sizeof(mtu), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				strlwr(dataencrypt);
				strlwr(auth);
				strlwr(lcpecho);
				sprintf( cmdbuf, "uci set network.pptp.dataencrypt='%s' -q", dataencrypt );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set network.pptp.auth='%s' -q", auth );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set network.pptp.lcpecho='%s' -q", lcpecho );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set network.pptp.mtu='%s' -q", mtu );
				system(cmdbuf);
				
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}
	AtCmd_Respond( cmd_para_buf );
}

void AtCmd_Handler_VPNGRE(AT_CMD_PARA* pParam)
{
	char enabled[16] = {0}; 
	char rteip[SOCK_MAX_ADDR_LEN] = {0}; 
	char localip[SOCK_MAX_ADDR_LEN] = {0}; 
	char rtevirtualip[SOCK_MAX_ADDR_LEN] = {0}; 
	char virtualip[SOCK_MAX_ADDR_LEN] = {0}; 
	char cmdbuf[UCI_CMD_SIZE] = {0};

	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get -q network.gre.enabled", enabled, sizeof(enabled));
			shell_get("uci get -q network.gre.rteip", rteip, sizeof(rteip));
			shell_get("uci get -q network.gre.localip", localip, sizeof(localip));
			shell_get("uci get -q network.gre.rtevirtualip", rtevirtualip, sizeof(rtevirtualip));
			shell_get("uci get -q network.gre.virtualip", virtualip, sizeof(virtualip));
			sprintf( cmd_para_buf, "\r\n+OK=%s,%s,%s,%s,%s\r\n", enabled, rteip, localip, rtevirtualip, virtualip);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 5 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)enabled, sizeof(enabled), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 2, pParam->para_str, (void*)rteip, sizeof(rteip), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 3, pParam->para_str, (void*)localip, sizeof(localip), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 4, pParam->para_str, (void*)rtevirtualip, sizeof(rtevirtualip), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 5, pParam->para_str, (void*)virtualip, sizeof(virtualip), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				strupr(enabled);

				sprintf( cmdbuf, "uci set network.gre.enabled='%s' -q", enabled );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set network.gre.rteip='%s' -q", rteip );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set network.gre.localip='%s' -q", localip );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set network.gre.rtevirtualip='%s' -q", rtevirtualip );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set network.gre.virtualip='%s' -q", virtualip );
				system(cmdbuf);
				
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_DDNS(AT_CMD_PARA* pParam)
{
	char service_name[SOCK_MAX_ADDR_LEN] = {0}; 
	char username[SOCK_MAX_ADDR_LEN] = {0}; 
	char password[SOCK_MAX_ADDR_LEN] = {0}; 
	char cmdbuf[UCI_CMD_SIZE] = {0};

	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get -q ddns.myddns.service_name", service_name, sizeof(service_name));
			shell_get("uci get -q ddns.myddns.username", username, sizeof(username));
			shell_get("uci get -q ddns.myddns.password", password, sizeof(password));
			sprintf( cmd_para_buf, "\r\n+OK=%s,%s,%s,unknown\r\n", service_name, username, password);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 3 ) || ( pParam->para_sum == 4 ))
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)service_name, sizeof(service_name), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 2, pParam->para_str, (void*)username, sizeof(username), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				if ( !AtCmd_GetPara( 3, pParam->para_str, (void*)password, sizeof(password), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				sprintf( cmdbuf, "uci set ddns.myddns.service_name='%s' -q", service_name );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set ddns.myddns.username='%s' -q", username );
				system(cmdbuf);
				sprintf( cmdbuf, "uci set ddns.myddns.password='%s' -q", password );
				system(cmdbuf);
				
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}	
	AtCmd_Respond( cmd_para_buf );
}


void AtCmd_Handler_DDNSEN(AT_CMD_PARA* pParam)
{
	char enabled[16] = {0}; 
	char cmdbuf[UCI_CMD_SIZE] = {0};

	switch( pParam->type )
	{
		case AT_CMD_READ:
		case AT_CMD_EXE:
		{
			shell_get("uci get -q ddns.myddns.enabled", enabled, sizeof(enabled));
			sprintf( cmd_para_buf, "\r\n+OK=%s\r\n", enabled);
			break;
		}
		
		case AT_CMD_SET:
		{
			if ( ( pParam->para_sum == 1 ) )
			{
				if ( !AtCmd_GetPara( 1, pParam->para_str, (void*)enabled, sizeof(enabled), PARA_TYPE_STRING))
				{
					strcpy( cmd_para_buf, ACK_ERR4 );
					 break;
				}
				strupr(enabled);
				sprintf( cmdbuf, "uci set ddns.myddns.enabled='%s' -q", enabled );
				system(cmdbuf);
				
				strcpy( cmd_para_buf, ACK_OK );
			}
			else
			{
				strcpy( cmd_para_buf, ACK_ERR4 );
			}
			break;
		}
		
		default:
		{
			strcpy( cmd_para_buf, ACK_ERR3 );
			break;
		}
	}	
	AtCmd_Respond( cmd_para_buf );
}


