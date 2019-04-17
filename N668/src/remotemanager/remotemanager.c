#include <stdio.h>  
#include <stdlib.h>  
#include <sys/socket.h>  
#include <sys/un.h>   
#include <unistd.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <string.h> 
#include <netdb.h> 
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/signal.h>  
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>
#include <syslog.h>
#include <pthread.h> 
#include "remotemanager.h"

// for thread transparent args
int tcp_connect=0;
thread_socket socket_monitor;

time_t now; 
struct tm *timenow; 

//
int pro_up=0,pro_down=0;
long int pro_sec=0;
char pro_times[30]="";


// get key value from string
int http_jason_getkeyval(char *input, char *key, char *value)
{
	unsigned int i = 0;
	char *p = 0;
	char *pa = 0;
	char *pb = 0;
	unsigned short len = 0;

	// find the key string within input
	p = strstr((const char *)input, key);
	if(p == NULL)
	{
		return -1;
	}
	pa = p + strlen(key);
	p = strstr(pa, ":\"");
	pa = p + strlen(":\"");
	if(pa==0)
	{
		return -2;
	}
	pb = strstr(pa, "\"");
	if(pb==0)
	{
		return -3;
	}
	len = pb - pa;
	if(len > 1000)
	{
		return -4;
	}
	memcpy(value, pa, len);
	value[len] = 0;
	
	//sscanf(p, "\":\"%s\",", value);
	
	return 0;
}

// 
int getparam(void)
{
	FILE * fp;
	FILE * fp_mac;
	char buffer[200] = {0};
	unsigned char *pcheck = NULL;
	unsigned int i = 0;
	unsigned int temp = 0;
    unsigned char end_data[]={0x02,0x03,0x00};
	printf("get param start--\n");
	
	// get update param
	shell_get("uci get remotem.@remotem[0].uenabled -q", socket_monitor.update_enabled, sizeof(socket_monitor.update_enabled));
	shell_get("uci get remotem.@remotem[0].userveraddr -q", socket_monitor.serveraddr, sizeof(socket_monitor.serveraddr));	
	shell_get("uci get remotem.@remotem[0].uport -q", buffer, sizeof(buffer));
	socket_monitor.remoteport = atoi(buffer);
	shell_get("uci get remotem.@remotem[0].uinterval -q", buffer, sizeof(buffer));
	socket_monitor.updateinfo_intvl = atoi(buffer);

	// get monitor param
	shell_get("uci get remotem.@remotem[0].menabled -q", socket_monitor.monitor_enabled, sizeof(socket_monitor.monitor_enabled));
	shell_get("uci get remotem.@remotem[0].mserveraddr -q", socket_monitor.serveraddr_mon, sizeof(socket_monitor.serveraddr_mon));
	shell_get("uci get remotem.@remotem[0].mport -q", buffer, sizeof(buffer));
	socket_monitor.remoteport_mon = atoi(buffer);
	shell_get("uci get remotem.@remotem[0].minterval -q", buffer, sizeof(buffer));	
	socket_monitor.heartpkt_intvl = atoi(buffer);
	shell_get("uci get remotem.@remotem[0].minterval_mon -q", buffer, sizeof(buffer));	
	socket_monitor.monitor_intvl = atoi(buffer);
	shell_get("uci get remotem.@remotem[0].mheartpkt -q", socket_monitor.heartpkt, sizeof(socket_monitor.heartpkt));	
	socket_monitor.tcp_anew=0;
	// get mac address
	shell_get("ifconfig br0 | grep HWaddr | awk -F 'HWaddr' '{print $2}' | sed 's/://g' | sed 's/ //g'", socket_monitor.wan_mac, sizeof(socket_monitor.wan_mac));
	
	printf("get param end--\n");
    memcpy(socket_monitor.heartpkt+strlen(socket_monitor.heartpkt),end_data,3);	
}

// get lte router status 
#define INFO_UPDATE		0
#define INFO_MONITOR	1

// UPDATE REG PACKET
const char update_jason[] = "{\"msg\":\"upgradeDevInfo\",\"id\":\"%s\",\"devRang\":\"%s\",\"devType\":\"%s\""
							",\"softVer\":\"%s\",\"HardVer\":\"%s\"}";
char monitor_query_const[] = "AT+MAC\r\nAT+VER\r\nAT+HW_VER\r\nAT+WANN\r\nAT+UPTIME\r\nAT+TRAFFIC\r\n"
							"AT+UART1\r\nAT+SOCKLKA1\r\nAT+UART2\r\nAT+SOCKLKA2\r\nAT+UART3\r\nAT+SOCKLKA3\r\nAT+UART4\r\nAT+SOCKLKA4\r\n"
							"AT+UART5\r\nAT+SOCKLKA5\r\nAT+UART6\r\nAT+SOCKLKA6\r\nAT+UART7\r\nAT+SOCKLKA7\r\nAT+UART8\r\nAT+SOCKLKA8\r\n";
static char jason_buffer[500] = "";

// ack to query commands
static char * ack_query(int cmd)
{
	unsigned char *pcheck = NULL;
	unsigned char fwrevision_buf[50] = {0}; 
	unsigned char hostname_buf[50] = {0};
	unsigned char group[50]="";//"M2M";

	unsigned int i = 0;
	unsigned int j = 0;
	unsigned char *p1 = 0;

	// get mac addr
	shell_get("cat /etc/ver", fwrevision_buf, sizeof(fwrevision_buf));
	shell_get("uci get system.@system[0].group -q", group, sizeof(group));
	shell_get("uci get system.@system[0].hostname -q", hostname_buf, sizeof(hostname_buf));

	memset(jason_buffer, 0, sizeof(jason_buffer));
	// get update information
	if(cmd == INFO_UPDATE)
	{
		sprintf(jason_buffer, update_jason, socket_monitor.wan_mac, group, hostname_buf, fwrevision_buf, "unknown");
	}
	// get current running info
	else if(cmd == INFO_MONITOR)
	{
		AtCmd_Dispatch(monitor_query_const);
		strcpy(jason_buffer, AtCmd_getrsp_ptr());
	}
											
	jason_buffer[sizeof(jason_buffer)-1] = 0;

	return jason_buffer;
	
}

//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
// ------  remote update         ------------------
// ------  based on udp client  ------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------

struct sockaddr_in udp_src_ip;
struct sockaddr_in remote_udp_ip[2];
socklen_t fromlen = sizeof(udp_src_ip);

//
// udp function 
//
// commands to update system
//
// root@USR-G800:~# wget ftp://wangbin:666666@115.28.232.174:2121/openwrt-ramips-mt7620-mt7620-squashfs-sysupgrade.bin -O /tmp/openwrt-ramips-mt7620-
// mt7620-squashfs-sysupgrade.bin
// Connecting to 115.28.232.174:2121 (115.28.232.174:2121)
// openwrt-ramips-mt762 100% |*************************************************************************************************| 10240k  0:00:00 ETA
// root@USR-G800:~# sysupgrade -n /tmp/openwrt-ramips-mt7620-mt7620-squashfs-sysupgrade.bin
//
// example str from server
// {"msg":"update","file":"USR-G800_FW_V1.0.9_161008.bin","account":"wangbin","password":"666666","ftpHost":"115.28.232.174:2121","otherOption":"AT"}
char ftp_filepath[150] = {0};
char ftp_username[150] = {0};
char ftp_password[150] = {0};
char ftp_serveraddr[150] = {0};
char ftp_atcmd[150] = {0};

char ftp_str[200] = {0};
unsigned char key_update[] = {0x75,0x73,0x72,0x79,0x63,0x73,0x6A,0x6A,
     0x6B,0x66,0x75,0x71,0x71,0x77,0x69,0x6F};

void *thread_remoteupdate(void *arg)
{
	thread_socket thissock = *(thread_socket*)arg;
	struct sockaddr_in remote_ip;
	int ret = 0;
	int client_fd = 0;
	int max_fd = 0;
	
	fd_set readset;
	int j = 0;
	int new_sockid = 0;
	unsigned int temp = 0;
	unsigned int len = 0;
	unsigned int if_id = 0;
	unsigned char packet_syncbaud = 0;
	unsigned char sum = 0;
	unsigned int i = 0;
	struct hostent *host;
	int nread = 0;
	unsigned int monitor_cnt = 0;
	struct timeval select_timeouts = {0, 1};
	unsigned char *p1 = 0;	
	unsigned char buffer[2000] = {0};
	unsigned char set_buffer[100] = {0};
	char *pcheck = NULL;
	int bind_num;
    while(1)
	  {
	    
	    while(1)
	    {
	        client_fd = -1;
	        
	        host = NULL;
			while(host ==NULL)
			{
				sleep(10);
				host = gethostbyname((char *)socket_monitor.serveraddr);
				printf("UDP: dns resolve == %s -> %d\n", socket_monitor.serveraddr, *((struct in_addr *)host->h_addr));			
			} 
			
	        memset(&remote_udp_ip[0], 0, sizeof(remote_udp_ip[0]));
	        remote_udp_ip[0].sin_family = AF_INET;
	        remote_udp_ip[0].sin_port = htons(socket_monitor.remoteport);
			remote_udp_ip[0].sin_addr = *((struct in_addr *)host->h_addr);

	        // establish a udp socket
	        memset(&remote_ip, 0, sizeof(remote_ip));
	        remote_ip.sin_family = AF_INET;
	        remote_ip.sin_port = htons(socket_monitor.remoteport);
	        remote_ip.sin_addr.s_addr = INADDR_ANY;
			
	        client_fd = socket(AF_INET, SOCK_DGRAM, 0);
	        if(client_fd == -1)
	        {
	            sleep(1);
	            printf("failed to create socket \n");			
	            continue;
	        }
			
	        ret = bind(client_fd, (struct sockaddr *)&remote_ip, sizeof(remote_ip));
			bind_num=0;
	        while ((ret == -1)&&(bind_num<=10))
	        {
	            bind_num++;
	            printf("failed to bind socket \n");
	            sleep(10);			
				ret = bind(client_fd, (struct sockaddr *)&remote_ip, sizeof(remote_ip));
	            continue;
	        }
	        if(ret==-1)
	        	{
	        	close(client_fd);
				break;
			    }
			// send update info
			sleep(1);
			socket_monitor.udp_anew=0;
			p1 = ack_query(INFO_UPDATE);
 
			ret=sendto(client_fd, p1, strlen((const char *)p1), 0, (struct sockaddr*)&remote_udp_ip[0], fromlen);
			printf("remoteupdate send login: %s\n", p1);
			
	        FD_ZERO(&readset);
	        FD_SET(client_fd, &readset);
	        max_fd = 0;

	        while(1)
	        {
	            if(client_fd > max_fd)
	                max_fd = client_fd;
	            if(client_fd>0)
	            {
	                FD_SET(client_fd, &readset);
	            }

				select_timeouts.tv_sec =1; //socket_monitor.updateinfo_intvl>10? socket_monitor.updateinfo_intvl: 10;
				select_timeouts.tv_usec = 1000;
	            ret = select(max_fd+1, &readset, NULL, NULL, &select_timeouts);

	            if(ret > 0)
	            {
	                if(client_fd<0)
	                {
	                    continue;
	                }

	                new_sockid = client_fd;

	                if(FD_ISSET(new_sockid, &readset))
	                {  
	                    memset(buffer, 0, sizeof(buffer));
	                    ret = recvfrom(client_fd, buffer, sizeof(buffer), 0,
								(struct sockaddr *)&udp_src_ip, &fromlen);
						if(ret > 0)
						{
                            buffer[ret] = '\0';
							printf("get from net: %s\r\n", buffer);					
							// going to update, according to recved ip and path
							if(strstr(buffer, "\"msg\":\"upgradeNeed\""))
							{
								// format get string parameters
								// {"msg":"upgradeNeed","file":"USR-G800_FW_V1.0.9_161017.bin","account":"wangbin",
								// "password":"666666","ftpHost":"115.28.232.174:2122","content":"AT"}	
								//  the final command 
								// system("wget ftp://wangbin:666666@115.28.232.174:2121/USR-G800_FW_V1.0.9_161008.bin"
								//	" -O /tmp/firmware_ftpdownload.bin");							
								// system((const char *)ftp_str);							
								http_jason_getkeyval(buffer, "ftpHost", ftp_serveraddr);
								http_jason_getkeyval(buffer, "file", ftp_filepath);
								http_jason_getkeyval(buffer, "account", ftp_username);
								http_jason_getkeyval(buffer, "password", ftp_password);
								http_jason_getkeyval(buffer, "content", ftp_atcmd);
								
								printf("get keyval: %s, file: %s, ftp_username: %s, ftp_password: %s, ftp_atcmd: %s\r\n", 
											ftp_serveraddr, ftp_filepath, ftp_username, ftp_password, ftp_atcmd);
								sprintf(ftp_str, "wget ftp://%s:%s@%s/%s -O /tmp/vmlinux.ub",
											ftp_username, ftp_password, ftp_serveraddr, ftp_filepath);
								printf("get update info from server:%s- atcmd: %s\r\n", ftp_str, ftp_atcmd);
								memset(buffer, 0, sizeof(buffer));
								shell_get((const char *)ftp_str, buffer, sizeof(buffer));
								printf("----upgradeNeed return : %s--\n",buffer);
								memset(buffer, 0, sizeof(buffer));
								
								shell_get("if [ -f /tmp/vmlinux.ub ]; then echo firm_valid; fi", buffer, sizeof(buffer));
								// firmware_ftpdownload 100%
								if(strcmp(buffer, "firm_valid")==0)
								{
									// ack to ftp update command							
									sprintf(buffer, "{\"msg\":\"upgradeDevRes\",\"id\":\"%s\","
													"\"content\":\"%s download success\"}", 
													socket_monitor.wan_mac, ftp_filepath);
	 								sendto(client_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&remote_udp_ip[0], fromlen);
									printf("----About to upgrade : --\n");
									
									system("sleep 2; /etc/firm_up");
								}
								else 
								{
									sprintf(buffer, "{\"msg\":\"updateInfo\",\"info\":\"%s download failed\"}", ftp_filepath);
	 								sendto(client_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&remote_udp_ip[0], fromlen);
								}
							}
							// fw revision correct, update completed. shut down
							else if(strstr(buffer, "\"msg\":\"upgradeIgnore\""))
							{   char udp_ack_update[100]="";
							    memset(ftp_atcmd,0,strlen(ftp_atcmd));
							    http_jason_getkeyval(buffer, "content", ftp_atcmd);
								printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~``%s\n", ftp_atcmd);
								if(strlen(ftp_atcmd)>=3)
								{
									AtCmd_Dispatch(ftp_atcmd);
									strcpy(udp_ack_update, AtCmd_getrsp_ptr());
								}
								printf("---------------------------------\n");
								//p1 = ack_query(INFO_MONITOR);
								printf("sending update_info at pkt: %s\r\n", udp_ack_update);
									memset(buffer, 0, sizeof(buffer));
								//sendto(client_fd, p1, strlen((const char *)p1), 0, (struct sockaddr*)&remote_udp_ip[0], fromlen);
							}
							
						}
	                }
	            }
				else
				{
				    //printf("udp++\n");
				    if((strncasecmp(socket_monitor.update_enabled, "off", strlen("off"))==0)||socket_monitor.udp_anew==1||socket_monitor.all_anew==1)
				    	{  
				    	   printf("leave udp select\n");
	                       close(client_fd);
						   break;
					    }

					if(monitor_cnt++>= socket_monitor.updateinfo_intvl)

					{
					  monitor_cnt=0;
					  p1 = ack_query(INFO_UPDATE);
					  //DecryptInit();
					  sendto(client_fd, p1, strlen((const char *)p1), 0, (struct sockaddr*)&remote_udp_ip[0], fromlen);
						printf("remoteupdate send: %s\n", p1);
					}
				}
	        }//select
	   
		   if((strncasecmp(socket_monitor.update_enabled, "off", strlen("off"))==0)||socket_monitor.udp_anew==1||socket_monitor.all_anew==1)
			{  
			   
			   socket_monitor.udp_anew=0;
			   printf("leave udp socket\n");
			   if(socket_monitor.all_anew==1)
					 {
						socket_monitor.all_anew=0;
						//system("/etc/init.d/network stop");
						//system("/etc/init.d/network start");
						 system("/etc/init.d/network restart");
						sleep(60);
					 }

			   
	           close(client_fd);
			   break;
		    }
		  sleep(10);

			
	    }//socket
	     printf(" udp socket leaved\n");
	}

}
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
// ------  remote monitor         ------------------
// ------    based on tcp client  ------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
fd_set readset_tcpsvr;
fd_set writeset_tcpsvr;
fd_set socketSet;

char ret_buffer[1000] = {0};

int main(int argc,char *argv[])
{
	//thread_socket thissock = *(thread_socket*)arg;
	socklen_t len;
	struct hostent *host;
	struct sockaddr_in local_ip, client_ip, remote_ip;
	int client_fd = 0;
	int max_fd = 0;
	int ret = 0;
	int i = 0;
	unsigned int j = 0;
	struct timeval select_timeouts = {0, 1};
	unsigned char buffer[2000] = {0};
	unsigned char tcp_cmd[150] = {0};
	unsigned char *p1 = 0;
	unsigned int heartpkt_cnt = 0;
	unsigned int monitor_cnt = 0;
	unsigned char *pmalloc = 0;
	char *preplace = 0;

	pthread_t thread_sid_update;	

	// 
	getparam();
	printf("getparam passed--\n");
	//AtCmd_Dispatch("at+e=on\r\n");
	echo_settemp(1);
	
	// check if update
	if((strncasecmp(socket_monitor.update_enabled, "off", strlen("off"))==0))
	{
		printf("thread_remoteupdate not enabled, exit\n");
		//pthread_exit("thread_remoteupdate end\n");		
	}
	else
	{
		if(pthread_create(&thread_sid_update, NULL, thread_remoteupdate, (void *)&socket_monitor))
		{
			perror("failed to create thread update\n");
		}
	}
	
	while((strncasecmp(socket_monitor.monitor_enabled, "off", strlen("off"))==0))
	{
		//system("/etc/init.d/network stop");
		//system("/etc/init.d/network restart");
		sleep(60);
	}
	
	printf("thread_client_monitor started\n");
	
	while(1)
	{

		while(1)
		{
			// get ip address
			host = NULL;
			while(host ==NULL)
			{
				sleep(10);
				host = gethostbyname((char *)socket_monitor.serveraddr_mon);
				printf("TCP: dns resolve == %s -> %d\n", socket_monitor.serveraddr_mon, *((struct in_addr *)host->h_addr));			
			} 
			
			memset(&remote_ip, 0, sizeof(remote_ip));
			remote_ip.sin_family = AF_INET;
			remote_ip.sin_port = htons(socket_monitor.remoteport_mon);
			remote_ip.sin_addr = *((struct in_addr *)host->h_addr);

			if((remote_ip.sin_addr.s_addr==0xffffffff)||(remote_ip.sin_addr.s_addr==0))
			{
				printf("remote addr is ff or 00\n");
				sleep(10);
				continue;
			}
			
			client_fd = socket(AF_INET, SOCK_STREAM, 0);
			if(client_fd<0)
			{
				printf("create socket fail\n");
				sleep(10);
				continue;
			}
			// set keepalive = 1; tcp nodelay = 1 
			setsockopt(client_fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&socket_monitor.keepalive,sizeof(socket_monitor.keepalive));
			setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, (void *)&socket_monitor.tcpnodelay, sizeof(socket_monitor.tcpnodelay));

			ret = connect(client_fd, (struct sockaddr *)&remote_ip, sizeof(remote_ip));
			if(ret<0)
			{
				close(client_fd);
				printf("connect %08X:%d fail \n", remote_ip.sin_addr.s_addr, remote_ip.sin_port);
				//sleep(60);
				continue;
			}
			else
			{	
				tcp_connect=1;
				socket_monitor.tcp_anew=0;
				memset(ret_buffer, 0, sizeof(ret_buffer));
				sprintf(ret_buffer, "%s%c%c", socket_monitor.wan_mac, 2, 3);
				
				send(client_fd, ret_buffer, strlen((const char *)ret_buffer), 0);
				sleep(1);
				pmalloc = malloc(1000);
				if(pmalloc)
				{
					memset(ret_buffer, 0, sizeof(ret_buffer));		
					//at_main(monitor_query_const, ret, pmalloc); 
					// enable echo
					AtCmd_Dispatch(monitor_query_const);
					strcpy(pmalloc, AtCmd_getrsp_ptr());									
					
					sprintf(ret_buffer, "{\"msg\":\"remoteAutoUploadRes\",\"id\":\"%s\",\"content\":\"%s\"}%c%c", socket_monitor.wan_mac, pmalloc,2,3);
								
					ret=send(client_fd, ret_buffer, strlen((const char *)ret_buffer), 0); 							
					printf("ret2==%s\n",ret_buffer);
					free(pmalloc);
					if(ret<=0)
					{
					  close(client_fd);
					  break;
					}
				}

				FD_ZERO(&socketSet);
				FD_SET(client_fd, &socketSet);

				while(1)
				{
					FD_ZERO(&readset_tcpsvr);
					FD_ZERO(&writeset_tcpsvr);
					readset_tcpsvr = socketSet;

					max_fd = client_fd;
					select_timeouts.tv_sec = 1;
					select_timeouts.tv_usec = 10000;
					
					ret = select(max_fd+1, &readset_tcpsvr, NULL, NULL, &select_timeouts);
					if(ret > 0)
					{
						if(FD_ISSET(client_fd, &readset_tcpsvr))
						{
							ret = recv(client_fd, buffer, sizeof(buffer), 0);
							
							if(ret <= 0)
							{
								close(client_fd);
								printf("socket closed\n");
								break;
							} 
							else
							{
								printf("%d\n",buffer);
								buffer[ret] = '\0';
								printf("recved data from tcp: %s\n", buffer);
								if(buffer[ret-1]!=0x03&&buffer[ret-2]!=0x02)
									continue;
								// check cmd valid
								if((strncmp(buffer, "{\"msg\":\"remoteQuery\"", strlen("{\"msg\":\"remoteQuery\""))==0))
								{
									pmalloc = malloc(1000);
									if(pmalloc)
									{   
										memset(tcp_cmd, 0, sizeof(tcp_cmd));	
										http_jason_getkeyval(buffer, "content", tcp_cmd);
																				
										strcat(tcp_cmd, "\r\n");
										printf("valid at cmd: %s", tcp_cmd);
										
										AtCmd_Dispatch(tcp_cmd);
										strcpy(pmalloc, AtCmd_getrsp_ptr());
										
										memset(ret_buffer, 0, sizeof(ret_buffer));
										sprintf(ret_buffer, "{\"msg\":\"remoteQueryRes\",\"id\":\"%s\",\"content\":\"%s\"}%c%c", socket_monitor.wan_mac, pmalloc,2,3);
									
										// send ack
										//p1 = ack_query(INFO_MONITOR);
										ret=send(client_fd, ret_buffer, strlen((const char *)ret_buffer), 0); 
										printf("ret1==%s\n",ret_buffer);

										free(pmalloc);

										if(ret<=0)
											break;
									}
								}
							}
						}
					}
					else
					{  
					  // printf("tcp++\n");
					   if((strncasecmp(socket_monitor.monitor_enabled, "off", strlen("off"))==0)
							||socket_monitor.tcp_anew==1||socket_monitor.all_anew==1)
						{
							printf("tcp leave select\n");
							close(client_fd);
							break;
						}
			   
						// send heartpkt 30 seconds
						if(monitor_cnt++>= socket_monitor.monitor_intvl)
						{
							//printf("sending monitor pkt before: \r\n");					
							monitor_cnt = 0;
							heartpkt_cnt = 0;
							
							pmalloc = malloc(1000);
							if(pmalloc)
							{
								memset(ret_buffer, 0, sizeof(ret_buffer));		
								AtCmd_Dispatch(monitor_query_const);
								strcpy(pmalloc, AtCmd_getrsp_ptr());									
								
								sprintf(ret_buffer, "{\"msg\":\"remoteAutoUploadRes\",\"id\":\"%s\",\"content\":\"%s\"}%c%c", socket_monitor.wan_mac, pmalloc,2,3);
								
								ret=send(client_fd, ret_buffer, strlen((const char *)ret_buffer), 0); 
								
								printf("ret2x==%d\n",ret);
								free(pmalloc);
								if(ret<=0)
									{
									 close(client_fd);
									 break;
									 }
							}

						}
						else if(heartpkt_cnt++>= socket_monitor.heartpkt_intvl)
						{
							heartpkt_cnt = 0;
							
							ret=send(client_fd, socket_monitor.heartpkt, strlen((const char *)socket_monitor.heartpkt), 0);
							//printf("sending heartpkt: ret3==%d\n",ret);
							if(ret<=0)
							{
								close(client_fd);
								break;
							}
						}					
					}
				}//¼àÌý
				if((strncasecmp(socket_monitor.monitor_enabled, "off", strlen("off"))==0)
						||socket_monitor.tcp_anew==1||socket_monitor.all_anew==1)
				 {
					
					socket_monitor.tcp_anew=0;
					printf("tcp leave socket\n");
					if(socket_monitor.all_anew==1)
					{
						 socket_monitor.all_anew=0;
						 //system("/etc/init.d/network stop");
						 //system("/etc/init.d/network restart");
						 sleep(60);
					}
					break;	
				 }
			}
			printf("$$$$$$$$$$$$$$$$$$$$\n");		
			sleep(10);	
		}//socket
		printf("tcp socket leaved\n");
		
	}

	return 0;
}









