   
#include <stdio.h>  
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
#include <pthread.h>  
#include <stdint.h>
#include <stdbool.h>
#include "macros.h"
#include <syslog.h>

static char mac[6] = {0};
static char mac_ascii[16] = {0};

//注册包注册方式

enum
{
	FIRST = 1,
	EVERY,
	ALL,
};

// 
static void thissocket_init(socket_s *s)
{
	int i = 0;
	char buf[64] = {0};
	int len = 0;
	
	for(i=0; i<CONNECTION_MAX; i++)
	{
		s->client_fd[i] = -1;
	}
	s->ms_cnt = 0;
	s->sec_cnt = 0;
	s->reconn_cnt = 0;
	s->reset_cnt = 0;

	s->keepAlive = 1;
	s->keepidle = 60; // 如该连接在60秒内没有任何数据往来,则进行探测
	s->keepinterval = 5;// 探测时发包的时间间隔为5 秒
	s->keepcount = 3; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.
	
	s->tcp_nodelay = 1;
	s->select_timeouts.tv_sec = 1;
	s->select_timeouts.tv_usec = 0; // 
	
	s->loginwhen = 0;

	if((strncasecmp(portParam.loginen, "usr", strlen("usr")) == 0) || (strncasecmp(portParam.loginen, "mac", strlen("mac")) == 0) ) 
	{
		if(strncasecmp(portParam.loginwhen, "first", strlen("first"))==0)
		{
			s->loginwhen = FIRST;
		}
		else if(strncasecmp(portParam.loginwhen, "every", strlen("every"))==0)
		{
			s->loginwhen = EVERY;
		}
		else if(strncasecmp(portParam.loginwhen, "all", strlen("all"))==0)
		{
			s->loginwhen = ALL;
		}
	}
	shell_get("ifconfig br0 | grep HWaddr | awk -F 'HWaddr' '{print $2}' | sed 's/://g' | sed 's/ //g'", buf, sizeof(buf));

	len = (sizeof(mac_ascii) > strlen(buf) )? strlen(buf) : sizeof(mac_ascii);
	memcpy(mac_ascii, buf , len);
	mac_ascii[len] = '\0';
	
	HexString2Asc(buf, mac, strlen(buf), sizeof(mac));
	
	memcpy(s->macaddr, mac, sizeof(mac));
	
	return;
}

static int udp_heart(int fd, unsigned int sec, struct sockaddr_in *target, socklen_t fromlen)
{
	char heart[80] = {0};
	int len = 0;
	
	// heart disabled
	if(strncasecmp(portParam.hearten, "off", strlen("off"))==0)
	{
		return -1;
	}
	
	if(portParam.hearttime && ((sec % portParam.hearttime) != 0) && portParam.heart_len)
	{
		return -2;
	}

	// check heart content 
	if(strncasecmp(portParam.heart_aschex, "ascii", strlen("ascii"))==0)
	{
		len = strlen(portParam.heartpkt);
		len = (sizeof(heart) > len)? len : sizeof(heart);
		memcpy(heart, portParam.heartpkt, len);
	}
	else
	{
		len = HexString2Asc(portParam.heartpkt, heart, strlen(portParam.heartpkt), sizeof(heart));		
	}

	//
	if(strncasecmp(portParam.heart_netcom, "net", strlen("net"))==0)
	{
		//send(fd, portParam.heartpkt, portParam.heart_len, 0);
		sendto(fd, heart, len, 0, (struct sockaddr*)target, fromlen);
	}
	else if((get_atflag()==0) && (strncasecmp(portParam.heart_netcom, "com", strlen("com"))==0))// send to serial 
	{
		pthread_mutex_lock(&write_lock);
		write(fd_serial, heart, len);
		pthread_mutex_unlock(&write_lock);												
	}	
	return 0;
}


static int udp_login_beforedata(int fd, char *in)
{
	int temp = 0;

	if(strncasecmp(portParam.loginen, "mac", strlen("mac")) == 0 ) 
	{
		if(strncasecmp(portParam.logintype, "ascii", strlen("ascii"))==0)
		{
			temp = strlen(mac_ascii);
			memcpy(in, mac_ascii, temp);
		}
		else
		{
			memcpy(in, mac, 6);
		    temp = 6;
		}
	}
	else	//usr注册包
	{
		if(strncasecmp(portParam.logintype, "ascii", strlen("ascii"))==0)
		{
			//send(fd, portParam.loginpkt, strlen(portParam.loginpkt), 0);
			temp = strlen(portParam.loginpkt);
		    temp = (80 > temp)? temp : 80;
			memcpy(in, portParam.loginpkt, temp);
		}
		else
		{
			temp = strlen(portParam.loginpkt);
			temp = (80 > temp)? temp : 80;
			HexString2Asc(portParam.loginpkt, in, temp, 80);
			temp = temp / 2;
			//send(fd, buffer, temp/2, 0);
		}
	}
	return temp;
}

// thread for udp
int thread_udp(void *arg)
{
	fd_set writeset;
	struct sockaddr_in remote_ip;
	struct sockaddr_in remote_udp_ip[2];	
	struct sockaddr_in udp_src_ip;
	socklen_t fromlen = sizeof(udp_src_ip);	
	int ret = 0;
	int max_fd = 0;
	fd_set readset;
	int new_sockid = 0;
	unsigned int temp = 0;
	struct hostent *host;
	struct timeval select_timeouts = {0, 1};
    int client_fd[2];
	socket_s this;
	char *pmalloc = 0;
	char buffer[80] = {0};
	//char log_path[64] = {0};
	//char log_buf[32] = {0};

	syslog(LOG_INFO, "udp task started \n");

	// init socket sessions
	thissocket_init(&this);
	log_socklk("socketa", "connect"); 

    while(1)
    {
        client_fd[0] = -1;
		
		if(this.reset_cnt >= portParam.reset_timeout)
		{
			this.reset_cnt = 0;
			log_resettimeout(portParam.uartnum, FALSE);
		}
        
		if(!(host = gethostbyname((char *)portParam.remoteaddr)))
		{
			printf("dns resolve == NULL\n");
			sleep(10);
			this.reset_cnt += 10;
			continue;
		}
        memset(&remote_udp_ip[0], 0, sizeof(remote_udp_ip[0]));
        remote_udp_ip[0].sin_family = AF_INET;
        remote_udp_ip[0].sin_port = htons(portParam.remoteport);
		remote_udp_ip[0].sin_addr = *((struct in_addr *)host->h_addr);

        // establish a udp socket
        memset(&remote_ip, 0, sizeof(remote_ip));
        remote_ip.sin_family = AF_INET;
        remote_ip.sin_port = htons(portParam.localport);
        remote_ip.sin_addr.s_addr = INADDR_ANY;
        client_fd[0] = socket(AF_INET, SOCK_DGRAM, 0);
        if(client_fd[0] == -1)
        {
            sleep(10);
			this.reset_cnt += 10;
            printf("failed to create socket \n");
            continue;
        }
        ret = bind(client_fd[0], (struct sockaddr *)&remote_ip, sizeof(remote_ip));
        if (ret == -1)
        {
            printf("failed to bind socket \n");
            sleep(10);			
			this.reset_cnt += 10;
			close(client_fd[0]);
			
            continue;
        }
		
		// set remote 
		udp_src_ip = remote_udp_ip[0];
		
		if((portParam.workmode == WORK_MODE_UDP_CLIENT) && 
				((this.loginwhen == FIRST) || (this.loginwhen == ALL)))
		{
			if(strncasecmp(portParam.loginen, "MAC", strlen("MAC"))==0)
			{
				if(strncasecmp(portParam.logintype, "ascii", strlen("ascii"))==0)
					sendto(client_fd[0], mac_ascii, strlen(mac_ascii), 0, (struct sockaddr*)&remote_udp_ip[0], sizeof(remote_udp_ip[0]));
				else
					sendto(client_fd[0], mac, 6, 0, (struct sockaddr*)&remote_udp_ip[0], sizeof(remote_udp_ip[0]));
			}
			//usr注册包
			else if(strncasecmp(portParam.loginen, "USR", strlen("USR"))==0)
			{
				if((this.loginwhen == FIRST) || (this.loginwhen == ALL))
				{
					if(strncasecmp(portParam.logintype, "ascii", strlen("ascii"))==0)
					{
						sendto(client_fd[0], portParam.loginpkt, strlen(portParam.loginpkt), 0, (struct sockaddr*)&remote_udp_ip[0], sizeof(remote_udp_ip[0]));
					}
					else
					{
						temp = strlen(portParam.loginpkt);
						temp = (sizeof(buffer) > temp)? temp : sizeof(buffer);
						HexString2Asc(portParam.loginpkt, buffer, temp, sizeof(buffer));
						sendto(client_fd[0], buffer, temp/2, 0, (struct sockaddr*)&remote_udp_ip[0], sizeof(remote_udp_ip[0]));
					}
				}
			}
			else	////udp不支持cloud注册
				;
		}
		
        FD_ZERO(&readset);
        FD_SET(client_fd[0], &readset);
        max_fd = 0;

        while(1)
        {
            if(client_fd[0] > max_fd)
                max_fd = client_fd[0];
            if(client_fd[0]>0)
            {
                FD_SET(client_fd[0], &readset);
            }
			if(socket_tx.valid[0])
			{
				socket_tx.valid[0] = 0;
				FD_ZERO(&writeset);
				FD_SET(client_fd[0], &writeset);
			} 

			select_timeouts.tv_sec = SELECT_TIMEOUT_SEC;
			select_timeouts.tv_usec = SELECT_TIMEOUT_USEC;
            ret = select(max_fd+1, &readset, &writeset, NULL, &select_timeouts);

            if(ret > 0)
            {
                if(client_fd[0]<0)
                {
                    continue;
                }
                new_sockid = client_fd[0];
                if(FD_ISSET(new_sockid, &readset))
                {
                    socket_rx.len = recvfrom(new_sockid, socket_rx.data, sizeof(socket_rx.data), 0,
										(struct sockaddr *)&udp_src_ip, &fromlen);

					if(socket_rx.len > 0)
					{
						
						ret = xfrc2217(fd_serial, socket_rx.data, socket_rx.len);
						if(ret != 0)
						{
							// if queue mode selected
							ret = socket_rx.len;
							pthread_mutex_lock(&write_lock);
							if(get_atflag()==0)
								write(fd_serial, socket_rx.data, ret);
							pthread_mutex_unlock(&write_lock);
						}
						
						log_resettimeout(portParam.uartnum, TRUE);
						
						this.reconn_cnt = 0;
						this.sec_cnt = 0;
						this.reset_cnt = 0;
					}					
                }
				// writefd
				if(FD_ISSET(client_fd[0], &writeset))
				{			
					//syslog(LOG_INFO, "select writeset\n");
					FD_CLR(client_fd[0], &writeset);
					
					ret = RingBufUsed(&g_sRxBuf[0]);
					if(ret <= 0) continue;
					if((portParam.workmode == WORK_MODE_UDP_CLIENT) && 
							((this.loginwhen == EVERY) || (this.loginwhen == ALL)))
					{
						pmalloc = malloc(ret+80);
						if(pmalloc)
						{
							temp = udp_login_beforedata(client_fd[0], pmalloc);
							RingBufRead(&g_sRxBuf[0], pmalloc+temp, ret);								
							sendto(client_fd[0], pmalloc, ret+temp, 0, (struct sockaddr*)&remote_udp_ip[0], fromlen);
							free(pmalloc);
						}
					}
					else
					{
						pmalloc = malloc(ret);
						if(pmalloc)
						{
							RingBufRead(&g_sRxBuf[0], pmalloc, ret);
							
							if(portParam.workmode == WORK_MODE_UDP_CLIENT)
							{
								sendto(client_fd[0], pmalloc, ret, 0, (struct sockaddr*)&remote_udp_ip[0], fromlen);
							}
							else
							{
								sendto(client_fd[0], pmalloc, ret, 0, (struct sockaddr*)&udp_src_ip, fromlen);
							}
							free(pmalloc);
						}
					}				
				}				
            }
			else
			{
				this.ms_cnt++;
				if((this.ms_cnt % 100) == 0)
				{
					this.sec_cnt++;
					this.reconn_cnt++;
					this.reset_cnt++;
					
					// net heart or // comm heart
					if((portParam.workmode == WORK_MODE_UDP_CLIENT))
						udp_heart(client_fd[0], this.sec_cnt, &remote_udp_ip[0], fromlen);
					
					// data timeout
					if((this.reset_cnt >= portParam.reset_timeout))
					{
						this.reset_cnt = 0;
						log_resettimeout(portParam.uartnum, FALSE);
					}						
				}
			}	
        }
        sleep(10);
    }
}
 
// thread for udp 
int thread_udpb(void *arg)
{
	fd_set writeset;
	struct sockaddr_in remote_ip;
	struct sockaddr_in remote_udp_ip[2];	
	struct sockaddr_in udp_src_ip;
	socklen_t fromlen = sizeof(udp_src_ip);	
	int ret = 0;
	int max_fd = 0;
	fd_set readset;
	int new_sockid = 0;
	struct hostent *host;
	struct timeval select_timeouts = {0, 1};
    int client_fd[2];
	socket_s this;
	char buffer[80] = {0};
	unsigned int temp = 0;
	char *pmalloc = 0;

	syslog(LOG_INFO, "udp task started \n");
	log_socklk("socketb", "connect"); 

	// init socket sessions
	thissocket_init(&this);

    while(1)
    {
        client_fd[0] = -1;
        
		while(!(host = gethostbyname((char *)portParam.remoteaddrb)))
		{
			printf("dns resolve == NULL\n");
			sleep(10);
		}
        memset(&remote_udp_ip[0], 0, sizeof(remote_udp_ip[0]));
        remote_udp_ip[0].sin_family = AF_INET;
        remote_udp_ip[0].sin_port = htons(portParam.remoteportb);
		remote_udp_ip[0].sin_addr = *((struct in_addr *)host->h_addr);

        // establish a udp socket
        memset(&remote_ip, 0, sizeof(remote_ip));
        remote_ip.sin_family = AF_INET;
        remote_ip.sin_port = htons(portParam.remoteportb);
        remote_ip.sin_addr.s_addr = INADDR_ANY;
        client_fd[0] = socket(AF_INET, SOCK_DGRAM, 0);
        if(client_fd[0] == -1)
        {
            sleep(10);
            printf("failed to create socket \n");			
            continue;
        }
        ret = bind(client_fd[0], (struct sockaddr *)&remote_ip, sizeof(remote_ip));
        while (ret == -1)
        {
            printf("failed to bind socket \n");
            sleep(10);			
			ret = bind(client_fd[0], (struct sockaddr *)&remote_ip, sizeof(remote_ip));
            continue;
        }
		
		// set remote 
		udp_src_ip = remote_udp_ip[0];


		if((portParam.workmodeb == WORK_MODE_UDP_CLIENT) && 
				((this.loginwhen == FIRST) || (this.loginwhen == ALL)))
		{
			if(strncasecmp(portParam.loginen, "MAC", strlen("MAC"))==0)
			{
				if(strncasecmp(portParam.logintype, "ascii", strlen("ascii"))==0)
					sendto(client_fd[0], mac_ascii, strlen(mac_ascii), 0, (struct sockaddr*)&remote_udp_ip[0], sizeof(remote_udp_ip[0]));
				else
					sendto(client_fd[0], mac, 6, 0, (struct sockaddr*)&remote_udp_ip[0], sizeof(remote_udp_ip[0]));
			}
			//usr注册包
			else if(strncasecmp(portParam.loginen, "USR", strlen("USR"))==0)
			{
				if((this.loginwhen == FIRST) || (this.loginwhen == ALL))
				{
					if(strncasecmp(portParam.logintype, "ascii", strlen("ascii"))==0)
					{
						sendto(client_fd[0], portParam.loginpkt, strlen(portParam.loginpkt), 0, (struct sockaddr*)&remote_udp_ip[0], sizeof(remote_udp_ip[0]));
					}
					else
					{
						temp = strlen(portParam.loginpkt);
						temp = (sizeof(buffer) > temp)? temp : sizeof(buffer);
						HexString2Asc(portParam.loginpkt, buffer, temp, sizeof(buffer));
						sendto(client_fd[0], buffer, temp/2, 0, (struct sockaddr*)&remote_udp_ip[0], sizeof(remote_udp_ip[0]));
					}
				}
			}
			else	////udp不支持cloud注册
				;
		}

		
		
        FD_ZERO(&readset);
        FD_SET(client_fd[0], &readset);
        max_fd = 0;

        while(1)
        {
            if(client_fd[0] > max_fd)
                max_fd = client_fd[0];
            if(client_fd[0]>0)
            {
                FD_SET(client_fd[0], &readset);
            }
			if(socket_tx.valid[1])
			{
				socket_tx.valid[1] = 0;
				FD_ZERO(&writeset);
				FD_SET(client_fd[0], &writeset);
			} 

			select_timeouts.tv_sec = SELECT_TIMEOUT_SEC;
			select_timeouts.tv_usec = SELECT_TIMEOUT_USEC;
            ret = select(max_fd+1, &readset, &writeset, NULL, &select_timeouts);

            if(ret > 0)
            {
                if(client_fd[0]<0)
                {
                    continue;
                }
                new_sockid = client_fd[0];
                if(FD_ISSET(new_sockid, &readset))
                {
                    socket_rx.len = recvfrom(new_sockid, socket_rx.data, sizeof(socket_rx.data), 0,
							(struct sockaddr *)&udp_src_ip, &fromlen);
						
					if(socket_rx.len > 0)
					{
						ret = xfrc2217(fd_serial, socket_rx.data, socket_rx.len);
						if(ret != 0)
						{
							// if queue mode selected
							ret = socket_rx.len;
							pthread_mutex_lock(&write_lock);
							if(get_atflag()==0)
								write(fd_serial, socket_rx.data, ret);
							pthread_mutex_unlock(&write_lock);
						}
					}					
                }
				// writefd
				if(FD_ISSET(client_fd[0], &writeset))
				{			
					FD_CLR(client_fd[0], &writeset);
					
					ret = RingBufUsed(&g_sRxBuf[1]);
									
					if(ret <= 0) continue;
					if((portParam.workmodeb == WORK_MODE_UDP_CLIENT) && 
							((this.loginwhen == EVERY) || (this.loginwhen == ALL)))
					{
						pmalloc = malloc(ret+80);
						if(pmalloc)
						{
							temp = udp_login_beforedata(client_fd[0], pmalloc);
							RingBufRead(&g_sRxBuf[1], pmalloc+temp, ret);								
							sendto(client_fd[0], pmalloc, ret+temp, 0, (struct sockaddr*)&remote_udp_ip[0], fromlen);
							free(pmalloc);
						}
					}
					else
					{
						pmalloc = malloc(ret);
						if(pmalloc)
						{
							RingBufRead(&g_sRxBuf[1], pmalloc, ret);															
							//send(this.client_fd[0], pmalloc, ret, 0);
							if(portParam.workmodeb == WORK_MODE_UDP_CLIENT)
							{
								sendto(client_fd[0], pmalloc, ret, 0, (struct sockaddr*)&remote_udp_ip[0], fromlen);
							}
							else
							{
								sendto(client_fd[0], pmalloc, ret, 0, (struct sockaddr*)&udp_src_ip, fromlen);
							}
							free(pmalloc);
						}
					}				
				}				
            }
			else
			{
				this.ms_cnt++;
				if((this.ms_cnt % 100) == 0)
				{
					this.sec_cnt++;
					// net heart or // comm heart
					udp_heart(client_fd[0], this.sec_cnt, &udp_src_ip, fromlen);
					// data timeout
					this.reconn_cnt++;
					if((portParam.reconnecttimeout) && (this.reconn_cnt >= portParam.reconnecttimeout))
					{
					}
				}
			}	
        }
        sleep(10);
    }
}
