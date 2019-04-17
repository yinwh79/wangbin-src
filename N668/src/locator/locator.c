/***************************************************************************        
    FileName : server.c    
    FileFunc :     
    Version  : V0.1         
    Author   : Sunrier         
    Date     : 2012-06-15    
    Descp    :       
***************************************************************************/   
#include <stdlib.h>  
#include <sys/types.h>  
#include <sys/socket.h>   
#include <sys/un.h>  
#include <unistd.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <string.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/signal.h>  
#include <signal.h>
#include <time.h>
#include <sys/time.h>
//#include <ifaddrs.h> 
#include <pthread.h>  
#include <stdio.h>  
#include <stdint.h>
#include <stdbool.h>
#include "string.h"
//#include "encrypt.h"
//#include "public.h"

// variables
#define NETCARD0_NAME 	"br0"
#define NETCARD0_IP		"ifconfig br0 | grep 'inet addr' | awk '{print $2}' | awk -F 'addr:' '{print $2}'"
#define NETCAED0_MAC	"ifconfig br0 | grep HWaddr | awk -F 'HWaddr' '{print $2}' | sed 's/ //g' | sed 's/://g'"


// locator command
char search_atcmd[128] = {0};
char search_atcmdack[256] = "WWW.USR.CN";

// search port
int search_port = 48899;

// locator cmd return
const unsigned char m4_search_cmd[] = {0xff, 0x01, 0x01, 0x02};

// 
int locator_init(void)
{
	char port[16] = {0};

	shell_get("uci get system.search.cmd -q", search_atcmd, sizeof(search_atcmd));
	shell_get("uci get system.search.port -q", port, sizeof(port));
	if((atoi(port) >=1) && (atoi(port) < 65535))
	{
		search_port = atoi(port);
	}
	
	printf("locator init %s, %d\n", search_atcmd, search_port);
}

#define TRUE	1
#define FALSE	0
#define 	TR_BUFFER_SIZE	1460

unsigned char p_rev[TR_BUFFER_SIZE] = {0};
struct sockaddr_in udp_src_ip;
socklen_t fromlen = sizeof(udp_src_ip);
// unsigned char en_buffer[6] = {0x11, 0x22};

#include   <sys/ioctl.h> 
#include   <sys/socket.h> 
#include   <netinet/in.h> 
#include   <net/if.h> 
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

unsigned char locator_buf[]={0xFF,0x24, 0x01, 0x00, 0x4B, 0xC0, 0xA8, 0x00, 0x4D, 0xD8, 0xB0, 0x4C, 
	0x00, 0x04, 0xC9, 0x40, 0x1F, 0x01, 0x00, 0x55, 0x53, 0x52, 0x2D, 0x54, 0x43, 0x50, 0x32, 0x33, 0x32, 0x2D, 0x34, 0x30, 0x31, 0x00, 0x00, 0xEF};

// 
int getlocalip_1(unsigned char *search_buf) 
{
    char buf[128] = {0};
    char buf1[128] = {0};
	char ipaddr[20] = {0};
	char hostname[64] = {0};
	char macaddr[32] = {0};
	char firmver[32] = {0};
    struct hostent *hent;
    int i;
	unsigned int localip = 0;
	unsigned int cnt = 0;

	//
	memset(search_atcmdack, 0, sizeof(search_atcmdack));
	// ip
	shell_get(NETCARD0_IP, ipaddr, sizeof(ipaddr));
	localip = inet_addr(ipaddr);

	if((localip!=0)&&(localip != 0xffffffff))
	{
	//	break;
	}
	memcpy((char *)search_buf+5, (char *)&localip, 4);
	// hostname
	shell_get("hostname", hostname, sizeof(hostname));
	memcpy((char *)search_buf+19, (char *)hostname, 15);
	
	// get mac addr
	shell_get(NETCAED0_MAC, macaddr, sizeof(macaddr));
	HexString2Asc(macaddr, buf1, strlen(macaddr), sizeof(buf1));
	memcpy((char *)search_buf+9, (char *)buf1, 6);
	//
	shell_get("cat /etc/ver", firmver, sizeof(firmver));

	// search_atcmdack "\r\n+OK=192.168.1.7,USR-N540,D8B04CD01234,V1.0.1\r\n"
	sprintf(search_atcmdack, "%s,%s,%s,%s", ipaddr, hostname, macaddr, firmver);
	
    return 0;
}

//
// udp function 
//
int main(int argc,char *argv[])
{  
	struct sockaddr_in remote_ip;
	int ret = 0;
	int max_fd = 0;
	fd_set readset;
	struct   ifreq   ifreq; 
  	struct timeval select_timeouts = {0, 100};
	int nb = 0;
	const int opt = 1;	
	int client_fd[1] = {-1};
	int atcmd_timeout = 0;
	int atcmd_enable = 0;
	unsigned int atcmd_ip = 0;
	
	printf("locator started\n");		

	if((argv[1]!=NULL) && (strncasecmp(argv[1], "getmac", strlen("getmac"))==0))
	{
		AtCmd_Dispatch("at+wrmac\r\n");
		
		printf("locator getmac: %s", AtCmd_getrsp_ptr());
		return -1;
	} 
	
	// EncryptBlock(en_buffer, 6); 
	
	locator_init();

    while(1)
    {
        client_fd[0] = -1;
		sleep(1);
		//printf("locator started 1 \n");

        memset(&remote_ip, 0, sizeof(remote_ip));
        remote_ip.sin_family = AF_INET;
        remote_ip.sin_port = htons(search_port);
        remote_ip.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_ANY; INADDR_BROADCAST

        client_fd[0] = socket(AF_INET, SOCK_DGRAM, 0);
        if(client_fd[0] == -1)
        {
            sleep(1);
            printf("failed to create socket \n");
            continue;
        }

        ret = bind(client_fd[0], (struct sockaddr *)&remote_ip, sizeof(remote_ip));
        if (ret == -1)
        {
            printf("failed to bind socket, return\n");
			close(client_fd[0]);
            sleep(1);
            return;
        }

		nb = setsockopt(client_fd[0], SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
		if(nb == -1)  
		{  
			//cout<<"set socket error..."<<endl;  
			return false;  
		} 
		
		printf("loop begin \n");
        FD_ZERO(&readset);
        FD_SET(client_fd[0], &readset);
        max_fd = 0;
		
		// get ip
		getlocalip_1(locator_buf);
		// get mac
		strncpy(ifreq.ifr_name, NETCARD0_NAME, sizeof(ifreq.ifr_name)-1);
		ioctl(client_fd[0],SIOCGIFHWADDR,&ifreq);
		memcpy((char *)locator_buf+9, (char *)&(ifreq.ifr_hwaddr.sa_data), 6);

		//
		AtCmd_Init();
		
        while(1)
        {
			//printf("loop locator \n");
				
            if(client_fd[0] > max_fd)
                max_fd = client_fd[0];
            if(client_fd[0]>0)
            {
                FD_SET(client_fd[0], &readset);
            }
			//select_timeouts = {0, 100};
			select_timeouts.tv_sec = 10;
			select_timeouts.tv_usec = 100000;

            ret = select(max_fd+1, &readset, NULL, NULL, &select_timeouts);
			if(ret > 0)
			{
				if(FD_ISSET(client_fd[0], &readset))
				{
					memset(p_rev, 0, sizeof(p_rev));
              		ret = recvfrom(client_fd[0], p_rev, TR_BUFFER_SIZE, 0, (struct sockaddr *)&udp_src_ip, &fromlen);
					//printf("received len:%d, content: %s \n", ret, p_rev);

					if((ret > 0)&&(memcmp(p_rev, m4_search_cmd, sizeof(m4_search_cmd))==0))
					{
						sendto(client_fd[0], locator_buf, 36, 0, (struct sockaddr*)&udp_src_ip, fromlen);
					}					
					// locator cmd return
					// "192.168.1.7,USR-N510,D8B04CD01234,3031"
					else if((ret > 0)&&(strncasecmp(p_rev, search_atcmd, strlen(search_atcmd))==0))
					{
						getlocalip_1(locator_buf);
						
						struct sockaddr_in addrto;  
						bzero(&addrto, sizeof(struct sockaddr_in));  
						addrto.sin_family=AF_INET;  
						addrto.sin_addr.s_addr=htonl(INADDR_BROADCAST);  
						addrto.sin_port=htons(search_port);  
						int nlen=sizeof(addrto);
					
						ret = sendto(client_fd[0], search_atcmdack, strlen(search_atcmdack), 0, (struct sockaddr*)&addrto, nlen);
						printf("searchcmdat recved: %s ack:%d\n", p_rev, ret);
					
						atcmd_enable = 1; 
						atcmd_ip = addrto.sin_addr.s_addr;
					}
					// at cmd handle
					else if((ret > 0) && (atcmd_enable == 1))
					{
						//printf("at recved %s\n", p_rev);
						
						if(ret>=(TR_BUFFER_SIZE-1))
						{
							p_rev[TR_BUFFER_SIZE-1] = 0;
						}
						else{
							p_rev[ret] = 0;
						}
						
						AtCmd_Dispatch(p_rev);
 						sendto(client_fd[0], AtCmd_getrsp_ptr(), strlen(AtCmd_getrsp_ptr()), 0, (struct sockaddr*)&udp_src_ip, fromlen);
					}
					//else
					//{
					//	sleep(1);						
					//}
				}
			}
			else
			{
				sleep(1);
				if(atcmd_timeout++ >= 30)
				{
					atcmd_timeout = 0;
					atcmd_enable = 0;
				}
			}
        }
        return 0;

    }
}

