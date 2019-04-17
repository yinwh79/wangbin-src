#ifndef __REMOTE_MANAGER_H
#define __REMOTE_MANAGER_H

#define TRUE	1
#define FALSE	0
#define PARAM_MAX_LEN	200
int switch_monitor;

typedef struct
{
	char update_enabled[16];
	char serveraddr[PARAM_MAX_LEN];
	unsigned int remoteport;
	unsigned int localport;
	unsigned int updateinfo_intvl;
	
	char monitor_enabled[16];
	char serveraddr_mon[PARAM_MAX_LEN];
	unsigned short remoteport_mon;
	unsigned short localport_mon;	
	unsigned short keepalive;
	unsigned short tcpnodelay;
	char login[PARAM_MAX_LEN];
	unsigned short login_len;
	char heartpkt[PARAM_MAX_LEN];
	unsigned int heartpkt_len;
	unsigned int heartpkt_intvl;
	unsigned int monitor_intvl;

	unsigned char wan_mac[30];
	int tcp_anew;
	int udp_anew;
	int all_anew;
	
}thread_socket;

 

#endif

