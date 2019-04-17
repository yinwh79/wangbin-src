/***********************************************************************************
Copy right:		2015-2025 USR IOT Tech.
Author:			Wangbin
Version:		V1.0
Date:			2015-09
Description:	g_AtCmd thread.
***********************************************************************************/
#ifndef	__AT_CMD_TABLE_H
#define	__AT_CMD_TABLE_H


#ifdef	__AT_CMD_TABLE_C
#define	AT_CMD_TABLE_EXT
#else
#define	AT_CMD_TABLE_EXT extern
#endif


#include "public.h"
#include "at_cmd.h"


#define CSQ_STATE_PATH		"/mnt/tmp/csq"
#define NET_STATE_PATH		"/mnt/tmp/4g_mode"
#define IMEI_PATH			"/mnt/tmp/imei"
#define ICCID_PATH			"/mnt/tmp/iccid"
#define SOCKA_STATE_PATH	"/mnt/tmp/sockalk"
#define SOCKB_STATE_PATH	"/mnt/tmp/sockblk"
#define SOCKC_STATE_PATH	"/mnt/tmp/sockclk"
#define SOCKD_STATE_PATH	"/mnt/tmp/sockdlk"



#ifndef	__AT_CMD_TABLE_C
extern const COMMAND_TABLE_T g_CommandTable[];
#endif


/******************************************************
Input:		AT_CMD_PARA* pParam
Output:	no
Return:	no
Description: all at commands excute functions
********************************************************/
AT_CMD_TABLE_EXT void AtCmd_Handler_WANN(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_REGTCP(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HEARTTP(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HTPCHD(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_REGCLOUD(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_MAC(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_WRMAC(AT_CMD_PARA* pParam);


AT_CMD_TABLE_EXT void AtCmd_Handler_Z(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_E(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_VER(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SN(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_IMEI(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_ICCID(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_STMSG(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_RELD(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_DNS(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_CLEAR(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_CFGTF(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_WKMOD(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_CMDPW(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_UART(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_UARTFT(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_UARTFL(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_RFCEN(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_APN(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_CSQ(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKA(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKB(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKC(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKD(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKAEN(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKBEN(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKCEN(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKDEN(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKASL(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKBSL(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKCSL(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKDSL(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKALK(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKBLK(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKCLK(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKDLK(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKATO(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKBTO(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKCTO(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKDTO(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKIND(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_REGEN(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_REGTP(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_REGID(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_REGDT(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_REGSND(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HEARTEN(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HEARTDT(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HEARTSND(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HEARTTM(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HTPTP(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HTPURL(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HTPSV(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HTPHD(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HTPTO(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HTPFLT(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_CLOUD(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_CISMSSEND(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HDVER(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_CIP(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_NETMODE(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SCRIPT(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_PLANG(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_MODNET(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_UPTIME(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_NETCARD(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_ETH0(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_PINGADDR(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_WEBPORT(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_UARTTL(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SEARCH(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKPORTA(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_SOCKPORTB(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_PDTIME(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_REGUSR(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_RSTIM(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_H(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_MID(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_PING(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_CLEAR(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_MANUF_VER(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_USERVER(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HWVER(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_UPDATE(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_MONITOR(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_TRAFFIC(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_LINUXCMP(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HW_VER(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_NTPRF(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_NTPIP(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_NTPZONE(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_NTPUTC(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_NTPTML(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_NTPTM(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_VPNGRE(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_VPNPPTPDTL(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_VPNPPTP(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_DDNSEN(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_DDNS(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_HEARTUSER(AT_CMD_PARA* pParam);
AT_CMD_TABLE_EXT void AtCmd_Handler_REGUSER(AT_CMD_PARA* pParam);


#endif


