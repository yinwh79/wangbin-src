/***************************************************************************        
    FileName : server.c
    FileFunc :
    Descp    :       
***************************************************************************/   
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
#include <pthread.h>  
#include <stdint.h>
#include <stdbool.h>
#include "macros.h"
#include <syslog.h>

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else 
#define PRINTF(...)  
#endif

// heart 
static int tcp_heart(int fd, unsigned int sec)
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
		memcpy(heart, portParam.heartpkt, len);
	}
	else
	{
		len = HexString2Asc(portParam.heartpkt, heart, strlen(portParam.heartpkt), sizeof(heart));		
	}
	
	//
	if(strncasecmp(portParam.heart_netcom, "net", strlen("net"))==0)
	{
		send(fd, heart, len, 0);
	}
	else if((get_atflag()==0) 
			&& (strncasecmp(portParam.heart_netcom, "com", strlen("com"))==0))// send to serial 
	{
		pthread_mutex_lock(&write_lock);
		write(fd_serial, heart, len);
		pthread_mutex_unlock(&write_lock);												
	}	
	return 0;
}

static int setsocket_option(int fd, socket_s *s)
{
	int err_stutas=0;
	s->tcp_nodelay=1;

	err_stutas=setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&(s->tcp_nodelay), 1);
	if(err_stutas=-1)
	{
		printf("Error:  TCP_NODELAY failed\n");		
	}
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&(s->keepAlive),sizeof(s->keepAlive));
	setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, (void*)&(s->keepidle), sizeof(s->keepidle ));
	setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (void *)&(s->keepinterval), sizeof(s->keepinterval ));
	setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (void *)&(s->keepcount), sizeof(s->keepcount ));
	
}

static void thissocket_init(socket_s *s)
{
	int i = 0;
	
	for(i=0; i<CONNECTION_MAX; i++)
	{
		s->client_fd[i] = -1;
	}
	s->server_fd = -1;
	s->ms_cnt = 0;
	s->sec_cnt = 0;
	s->reconn_cnt = 0;
	s->reset_cnt = 0;

	s->keepAlive = 1;
	s->keepidle = KEEPALIVE_IDLE; // 如该连接在60秒内没有任何数据往来,则进行探测
	s->keepinterval = KEEPALIVE_INTVL;// 探测时发包的时间间隔为5 秒
	s->keepcount = KEEPALIVE_COUNT; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.	
	
	s->tcp_nodelay = 1;
	s->select_timeouts.tv_sec = SELECT_TIMEOUT_SEC;
	s->select_timeouts.tv_usec = SELECT_TIMEOUT_USEC; // 
}


// thread for tcps
int thread_tcpserver(void *arg)
{
	fd_set writeset;	
	int ret = 0;
	int max_fd = 0;
	int i = 0;
	int j = 0;
	int new_sockid = 0;
	int client_num = 0;
	fd_set readset;
	char *pmalloc = 0;
	socket_s this;
	fd_set socketSet;	
	int reset_timeout_flag = 0;

	syslog(LOG_INFO, "tcp server task started on: %d\n", portParam.localport);
	log_socklk("socketa", "disconnect"); 
	//sprintf(log_path, "/tmp/log_reset%s", portParam.uartnum);
	//sprintf(log_buf, "%s", portParam.uartnum);	

	// init socket sessions
	thissocket_init(&this);
	
	unlink("Server_socket");   
	
	this.server_fd = socket(AF_INET,SOCK_STREAM, 0);
	//set_socket_nonblock(s);
	this.local_ip.sin_family = AF_INET;
	this.local_ip.sin_addr.s_addr = htonl(INADDR_ANY);
	this.local_ip.sin_port = htons(portParam.localport);
	this.address_len = sizeof(this.local_ip);

	ret = bind(this.server_fd,(struct sockaddr *)&this.local_ip, this.address_len);  
	while(ret<0)
	{
		sleep(10);
		this.reset_cnt += 10;
		if(this.reset_cnt >= portParam.reset_timeout)
		{
			this.reset_cnt = 0;
			log_resettimeout(portParam.uartnum, FALSE);
		}
		
		ret = bind(this.server_fd,(struct sockaddr *)&this.local_ip,this.address_len); 
		continue;
	}
	listen(this.server_fd,5);
	FD_ZERO(&socketSet);
	FD_SET(this.server_fd, &socketSet);
	
	while(1)
	{
		readset = socketSet;
		if(socket_tx.valid[0])
		{
			socket_tx.valid[0] = 0;
			writeset = socketSet;
		}
		
		max_fd = this.server_fd;
		for(i=0; i<CONNECTION_MAX; i++)
		{
		    if(this.client_fd[i] > max_fd)
		        max_fd = this.client_fd[i];
		}
		
		this.select_timeouts.tv_sec = SELECT_TIMEOUT_SEC;
		this.select_timeouts.tv_usec = SELECT_TIMEOUT_USEC; 
		ret = select(max_fd+1, &readset, &writeset, NULL, &this.select_timeouts);	
		/*
		将writeset放入select，可避免由于网络在selecttimes时间内不间断发送网络数据造成的串口数据无法转发的问题
		*/
		
		if(ret  > 0)
		{
		    if(FD_ISSET(this.server_fd, &readset))
		    {
		        new_sockid = accept(this.server_fd, (struct sockaddr *)&this.remote_ip[0], &this.address_len);
		        if(new_sockid<0)
		        {
		            continue;
		        }
		        if(client_num<CONNECTION_MAX)
		        {
		            for(i=0;i<CONNECTION_MAX;i++)
		            {
		                if(this.client_fd[i]<0)
		                {
		                    this.client_fd[i] = new_sockid;
		                    FD_SET(new_sockid, &socketSet);
							log_socklk("socketa", "connect"); 
														
							if(portParam.commbuffer==0)
							{
								for(i=0; i<SOCKET_CLIENT_MAX; i++)
								{
									socket_tx.valid[i] = 0;
								}
								RingBufFlush(&g_sRxBuf[0]);								
							}
		                    // set keepalive = 1; tcp nodelay = 1 
							setsocket_option(this.client_fd[i], &this);
							client_num++;
		                    break;
		                }
		            }
		        }
		        else
		        {
		            	//syslog(LOG_INFO, "close %d\n", temp);
		            	close(new_sockid);
		        }
		    }

		    for(j=0; j<CONNECTION_MAX; j++)
		    {
				if(this.client_fd[j]<0)
				{
					continue;
				}
				if(FD_ISSET(this.client_fd[j], &readset))
				{
					socket_rx.len = recv(this.client_fd[j], socket_rx.data, sizeof(socket_rx.data), 0);
					if(socket_rx.len<=0)
					{
						close(this.client_fd[j]);
						FD_CLR(this.client_fd[j], &socketSet);
						this.client_fd[j] = -1;
						if(client_num>0)
						{
							client_num--;							
						}
						if(client_num<=0)
						{
							log_socklk("socketa", "disconnect"); 
						}
					}
					else if((socket_rx.len > 0)&&(fd_serial>0))
					{
						log_resettimeout(portParam.uartnum, TRUE);
						
						this.reconn_cnt = 0;
						this.sec_cnt = 0;
						this.reset_cnt = 0;
						
						ret = xfrc2217(fd_serial, socket_rx.data, socket_rx.len);
						if(ret != 0)
						{
							pthread_mutex_lock(&write_lock);
							ret = socket_rx.len;
							if(portParam.modbustcp)
							{
								ret = rev_modbustcp_frame(socket_rx.data, socket_rx.len);
							}
							if (get_atflag() == 0)
								write(fd_serial, socket_rx.data, ret);
							pthread_mutex_unlock(&write_lock);
						}
					}
				}
		    }
			
			for(j=0; j<CONNECTION_MAX; j++)
			{
				if(this.client_fd[j]<=0)
				{
					continue;
				}
			
				if(FD_ISSET(this.client_fd[j], &writeset))
				{
					//FD_CLR(this.client_fd[j], &writeset);
					FD_ZERO(&writeset);

					PRINTF("uart to net\n");
					
					ret = RingBufUsed(&g_sRxBuf[0]);
					if(ret > portParam.packlen)
					{
						ret = portParam.packlen;
						writeset = socketSet;
					}
					if(ret)
					{
						pmalloc = malloc(ret+16);
						if(pmalloc)
						{
							RingBufRead(&g_sRxBuf[0], pmalloc, ret);
							if(portParam.modbustcp)
							{
								ret = rev_modbus_frame(pmalloc, ret);
							}
							for(; j<CONNECTION_MAX; j++)
							{
								if(this.client_fd[j]>0)
									send(this.client_fd[j], pmalloc, ret, 0);	
							}
							free(pmalloc);
						}
					}
					break;
				}
			}
		}
/* 		else if(ret < 0)
		{
		}
 */		else
		{
			this.ms_cnt++;
			if((this.ms_cnt % 100) == 0)
			{
				this.sec_cnt++;
				this.reconn_cnt++;
				this.reset_cnt++;
				
				if((this.reset_cnt >= portParam.reset_timeout))
				{
					this.reset_cnt = 0;
					log_resettimeout(portParam.uartnum, FALSE);
				}						

			}
		}
	}
	
	pthread_exit(NULL);
}


