/***************************************************************************        
    FileName : server.c
    FileFunc :
    Version  : V0.1
    Author   : Sunrier
    Date     : 2012-06-15    
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


enum
{
	FIRST = 1,
	EVERY,
	ALL,
};
	

char mac[6] = {0};
static char mac_ascii[16] = {0};


static int setsocket_option(int fd, socket_s *s)
{
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&(s->tcp_nodelay), sizeof(s->tcp_nodelay));
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&(s->keepAlive),sizeof(s->keepAlive));
	setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, (void*)&(s->keepidle), sizeof(s->keepidle ));
	setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (void *)&(s->keepinterval), sizeof(s->keepinterval ));
	setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (void *)&(s->keepcount), sizeof(s->keepcount ));
}

//
static void thissocket_init(socket_s *s)
{
	int i = 0 , len = 0;	
	char buf[64] = {0};
	
	for(i=0; i<CONNECTION_MAX; i++)
	{
		s->client_fd[i] = -1;
	}
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
	s->select_timeouts.tv_usec = SELECT_TIMEOUT_USEC; // 10ms
	
	shell_get("ifconfig br0 | grep HWaddr | awk -F 'HWaddr' '{print $2}' | sed 's/://g' | sed 's/ //g'", buf, sizeof(buf));

	len = (sizeof(mac_ascii) > strlen(buf) )? strlen(buf) : sizeof(mac_ascii);
	memcpy(mac_ascii, buf , len);
	mac_ascii[len] = '\0';
	
	HexString2Asc(buf, mac, strlen(buf), sizeof(mac));
	
	memcpy(s->macaddr, mac, sizeof(mac));
	
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
}

// int Asc2HexString( char* pAsc, char* pHex, int AscLen, int HexLen )
// int HexString2Asc( char* pHex, char* pAsc, int HexLen, int AscLen )
static int tcp_heart(int fd, unsigned int sec)
{
	char heart[80] = {0};
	int len = 0;

	//printf("tcp heart: %s\n", portParam.heartpkt);
	
	// heart disabled
	if(strncasecmp(portParam.hearten, "off", strlen("off"))==0)
	{
		return -1;
	}
	if(portParam.hearttime && ((sec % portParam.hearttime) != 0) && strlen(portParam.heartpkt))
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
	
	if(strncasecmp(portParam.heart_netcom, "net", strlen("net"))==0)
	{
		send(fd, heart, len, 0); 
	}
	else if((get_atflag()==0) && (strncasecmp(portParam.heart_netcom, "com", strlen("com"))==0))
	{
		pthread_mutex_lock(&write_lock);
		write(fd_serial, heart, len);
		pthread_mutex_unlock(&write_lock);
	}
	
	printf("tcp heart: %s\n", heart);
	
	return 0;
}

static int tcp_login_beforedata(int fd, char *in)
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

// login packet on ethernet
static int tcp_login(int fd)
{
	int temp = 0;
	char buffer[80] = {0};
	// 			
	if(strncasecmp(portParam.loginen, "off", strlen("off"))==0)
	{
		return 0;
	}
	//send login packet 
	// cloud 
	if(strncasecmp(portParam.loginen, "cloud", strlen("cloud"))==0)
	{
		Usr_Get_Cloud_Reg(fd, portParam.cloudid, portParam.cloudpwd);
	}
	else
	{
		if((strncasecmp(portParam.loginwhen, "first", strlen("first")) == 0) || (strncasecmp(portParam.loginwhen, "all", strlen("all")) == 0) )
		{
		    if(strncasecmp(portParam.loginen, "usr", strlen("usr"))==0) 
			{
				if(strncasecmp(portParam.logintype, "ascii", strlen("ascii"))==0)
				{
					send(fd, portParam.loginpkt, strlen(portParam.loginpkt), 0);
				}
				else
				{
					temp = strlen(portParam.loginpkt);
					HexString2Asc(portParam.loginpkt, buffer, temp, sizeof(buffer));
					send(fd, buffer, temp/2, 0);
				}
			}
			// send mac
			else if(strncasecmp(portParam.loginen, "mac", strlen("mac"))==0)
			{
				if(strncasecmp(portParam.logintype, "ascii", strlen("ascii"))==0)
					send(fd, mac_ascii, strlen(mac_ascii), 0);
				else	
					send(fd, mac, 6, 0);
			}
		}
	}


	return 0;
}
			
// thread for tcpc
int thread_tcpclient(void *arg)
{
	fd_set writeset;
	int max_fd = 0;	
	struct hostent *host;
	int ret = 0;
	int i = 0;
	fd_set readset;
	//fd_set socketSet;	
	char *pmalloc = 0;
	char *plogin = 0;
	unsigned int temp = 0;
	socket_s this;
	//char log_path[64] = {0};
	//char log_buf[32] = {0};

	syslog(LOG_INFO, "tcp client started\n");	
	// init socket sessions
	// portParam.uartnum
	thissocket_init(&this);
	log_socklk("socketa", "disconnect"); 
	//sprintf(log_path, "/tmp/log_reset%s", portParam.uartnum);
	//sprintf(log_buf, "%s", portParam.uartnum);	

    while(1)
    {
		// check if sec_cnt exceed reset_timeout
		if(this.reset_cnt >= portParam.reset_timeout)
		{
			this.reset_cnt = 0;
			log_resettimeout(portParam.uartnum, FALSE);
		}
		
		// get remote ip address 
		host = gethostbyname((char *)portParam.remoteaddr);
		if(host == NULL)
		{
			sleep(10);
			this.reset_cnt += 10;
			continue;
		}
		
        memset(&this.remote_ip[0], 0, sizeof(this.remote_ip[0]));
        this.remote_ip[0].sin_family = AF_INET;
        this.remote_ip[0].sin_port = htons(portParam.remoteport);
		this.remote_ip[0].sin_addr = *((struct in_addr *)host->h_addr);
		
        if((this.remote_ip[0].sin_addr.s_addr==0xffffffff)||(this.remote_ip[0].sin_addr.s_addr==0))
        {
			sleep(10);
			this.reset_cnt+= 10;
			continue;
        }
        // create socket
        this.client_fd[0] = socket(AF_INET, SOCK_STREAM, 0);
        if(this.client_fd[0]<0)
        {
			sleep(10);
			this.reset_cnt += 10;
			continue;
        }
        // set keepalive = 1; tcp nodelay = 1
        setsocket_option(this.client_fd[0], &this);
		// bind local port
		if(portParam.localport != 0)
		{
			this.local_ip.sin_family = AF_INET;
			this.local_ip.sin_addr.s_addr = htonl(INADDR_ANY);
			this.local_ip.sin_port = htons(portParam.localport);
			this.address_len = sizeof(this.local_ip);

			bind(this.client_fd[0],(struct sockaddr *)&this.local_ip, this.address_len);  
		}
		syslog(LOG_INFO, "tcp client %d connect to %08X, %d\n", 
								portParam.uartnum, this.remote_ip[0].sin_addr, portParam.remoteport);        
		ret = connect(this.client_fd[0], (struct sockaddr *)&this.remote_ip[0], sizeof(this.remote_ip[0]));
		
        if(ret<0)
        {
			close(this.client_fd[0]);
			sleep(10);
			this.reset_cnt += 10;
			
			continue;
        }
        else
        {
			syslog(LOG_INFO, "tcp client connected\n");        
			log_socklk("socketa", "connect"); 
			
			// clear comm buffer
			if(portParam.commbuffer==0)
			{
				for(i=0; i<SOCKET_CLIENT_MAX; i++)
				{
					socket_tx.valid[i] = 0;
				}
				socket_tx.len = 0;
				RingBufFlush(&g_sRxBuf[0]);				
			}
			// login
			tcp_login(this.client_fd[0]);

			while(1)
			{
				FD_ZERO(&readset);
				FD_SET(this.client_fd[0], &readset);
				if(socket_tx.valid[0])
				{
					socket_tx.valid[0] = 0;
					FD_ZERO(&writeset);
					FD_SET(this.client_fd[0], &writeset);
				}
				max_fd = this.client_fd[0];
				
				this.select_timeouts.tv_sec = SELECT_TIMEOUT_SEC;
				this.select_timeouts.tv_usec = SELECT_TIMEOUT_USEC; 
				ret = select(max_fd+1, &readset, &writeset, NULL, &this.select_timeouts);
				
				if(ret > 0)
				{
					if(FD_ISSET(this.client_fd[0], &readset))
					{
						socket_rx.len = recv(this.client_fd[0], socket_rx.data, sizeof(socket_rx.data), 0);
						if(socket_rx.len <= 0)
						{
							close(this.client_fd[0]); 
							log_socklk("socketa", "disconnect"); 
							break;
						}
						else
						{
							ret = xfrc2217(fd_serial, socket_rx.data, socket_rx.len);
							if(ret != 0)
							{
								// if queue mode selected
								if(portParam.modbustcp)
								{
									ret = rev_modbustcp_frame(socket_rx.data, socket_rx.len);
								}
								else
								{
									ret = socket_rx.len;
								}
								pthread_mutex_lock(&write_lock);
								if (get_atflag() == 0)
									write(fd_serial, socket_rx.data, ret);
								pthread_mutex_unlock(&write_lock);
							}
							
							log_resettimeout(portParam.uartnum, TRUE);
							
							this.reconn_cnt = 0;
							this.sec_cnt = 0;
							this.reset_cnt = 0;
						}
					}
					if(FD_ISSET(this.client_fd[0], &writeset))
					{
						//syslog(LOG_INFO, "select writeset\n");
						FD_ZERO(&writeset);
						
						ret = RingBufUsed(&g_sRxBuf[0]);
						if(ret <= 0) continue;
						{
							pmalloc = malloc(ret + 16);
							if(pmalloc)
							{
								RingBufRead(&g_sRxBuf[0], pmalloc, ret);
								if(portParam.modbustcp)
								{
									ret = rev_modbus_frame(pmalloc, ret);
								}
								//
								if((this.loginwhen == EVERY) || (this.loginwhen == ALL))
								{
									plogin = malloc(ret + 16 + 80);
									if(plogin)
									{
										temp = tcp_login_beforedata(this.client_fd[0], plogin);
										memcpy(plogin + temp , pmalloc , ret);
										send(this.client_fd[0], plogin, ret + temp, 0);
										free(plogin);
										
									}
								}
								else
								{
									send(this.client_fd[0], pmalloc, ret, 0);								
								}
								
								free(pmalloc);
							}
						}

					}
				}
				else if(ret < 0)
				{
					close(this.client_fd[0]);
					log_socklk("socketa", "disconnect"); 
					break;
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
						if(get_atflag()==0)
							tcp_heart(this.client_fd[0], this.sec_cnt);
						// data timeout
						if((portParam.reconnecttimeout) && (this.reconn_cnt >= portParam.reconnecttimeout))
						{
							this.reconn_cnt = 0;
							close(this.client_fd[0]);
							break;
						}
						if((this.reset_cnt >= portParam.reset_timeout))
						{
							this.reset_cnt = 0;
							log_resettimeout(portParam.uartnum, FALSE);
						}						
					}
				}
            }
        }
		sleep(5);
    }

	return 0;
}

// thread for tcpc
int thread_tcpclientb(void *arg)
{
	fd_set writeset;
	int max_fd = 0;	
	struct hostent *host;
	int ret = 0;
	fd_set readset;
	//fd_set socketSet;	
	char *pmalloc = 0;
	char *plogin = 0;
	socket_s this;
	unsigned int temp = 0;

	syslog(LOG_INFO, "tcp clientb started\n");
	
	// init socket sessions
	thissocket_init(&this);
	log_socklk("socketb", "disconnect"); 

    while(1)
    {
		// get remote ip address
		while(1)
		{
			host = gethostbyname((char *)portParam.remoteaddrb);
			if(host == NULL)
			{
				sleep(10);
				//syslog(LOG_INFO, "dns resolve == NULL\n");
			}
			else
			{
				//syslog(LOG_INFO, "dns resolve == %s\n", (char *)portParam.remoteaddrb);				
				break;
			}
		}
		
        memset(&this.remote_ip[0], 0, sizeof(this.remote_ip[0]));
        this.remote_ip[0].sin_family = AF_INET;
        this.remote_ip[0].sin_port = htons(portParam.remoteportb);
		this.remote_ip[0].sin_addr = *((struct in_addr *)host->h_addr);
		
        if((this.remote_ip[0].sin_addr.s_addr==0xffffffff)||(this.remote_ip[0].sin_addr.s_addr==0))
        {
			sleep(10);
			continue;
        }
        // create socket
        this.client_fd[0] = socket(AF_INET, SOCK_STREAM, 0);
        if(this.client_fd[0]<0)
        {
			sleep(10);
			continue;
        }
        // set keepalive = 1; tcp nodelay = 1
        setsockopt(this.client_fd[0], IPPROTO_TCP, TCP_NODELAY, (void *)&this.tcp_nodelay, sizeof(this.tcp_nodelay));
        setsockopt(this.client_fd[0], SOL_SOCKET, SO_KEEPALIVE, (void*)&this.keepAlive,sizeof(this.keepAlive));
		setsockopt(this.client_fd[0], SOL_TCP, TCP_KEEPIDLE, (void*)&this.keepidle , sizeof(this.keepidle ));
		setsockopt(this.client_fd[0], SOL_TCP, TCP_KEEPINTVL, (void *)&this.keepinterval , sizeof(this.keepinterval ));
		setsockopt(this.client_fd[0], SOL_TCP, TCP_KEEPCNT, (void *)&this.keepcount , sizeof(this.keepcount ));
		
		//syslog(LOG_INFO, "tcp client connect to %08X, %d\n", this.remote_ip[0].sin_addr, this.remote_ip[0].sin_port);        
		ret = connect(this.client_fd[0], (struct sockaddr *)&this.remote_ip[0], sizeof(this.remote_ip[0]));
		
        if(ret<0)
        {
			//syslog(LOG_INFO, "tcp client connect error\n");        
			
			close(this.client_fd[0]);
			sleep(10);
			continue;
        }
        else
        {
			
			// clear comm buffer
			log_socklk("socketb", "connect"); 
 			// login
			//tcp_login(this.client_fd[0]);
			tcp_login(this.client_fd[0]);

			while(1)
			{
				FD_ZERO(&readset);
				FD_SET(this.client_fd[0], &readset);
				if(socket_tx.valid[1])
				{
					socket_tx.valid[1] = 0;
					FD_ZERO(&writeset);
					FD_SET(this.client_fd[0], &writeset);
				}
				max_fd = this.client_fd[0];
				
				this.select_timeouts.tv_sec = SELECT_TIMEOUT_SEC;
				this.select_timeouts.tv_usec = SELECT_TIMEOUT_USEC; 
				ret = select(max_fd+1, &readset, &writeset, NULL, &this.select_timeouts);
				//syslog(LOG_INFO, "select ret:%d\n", ret);					
				
				if(ret > 0)
				{
					if(FD_ISSET(this.client_fd[0], &readset))
					{
						socket_rx.len = recv(this.client_fd[0], socket_rx.data, sizeof(socket_rx.data), 0);
						if(socket_rx.len <= 0)
						{
							close(this.client_fd[0]); 
							break;
						}
						else
						{
							ret = xfrc2217(fd_serial, socket_rx.data, socket_rx.len);
							if(ret != 0)
							{
								// if queue mode selected
								if(portParam.modbustcp)
								{
									ret = rev_modbustcp_frame(socket_rx.data, socket_rx.len);
								}
								else
								{
									ret = socket_rx.len;
								}
								pthread_mutex_lock(&write_lock);
								if (get_atflag() == 0)
									write(fd_serial, socket_rx.data, ret);
								pthread_mutex_unlock(&write_lock);
							}
							this.reconn_cnt = 0; 
						}
					}
					if(FD_ISSET(this.client_fd[0], &writeset))
					{
						//syslog(LOG_INFO, "select writeset\n");
						FD_ZERO(&writeset);
						
						ret = RingBufUsed(&g_sRxBuf[1]);
						if(ret <= 0) continue;
						{
							pmalloc = malloc(ret + 16);
							if(pmalloc)
							{
								RingBufRead(&g_sRxBuf[0], pmalloc, ret);
								if(portParam.modbustcp)
								{
									ret = rev_modbus_frame(pmalloc, ret);
								}
								//
								if((this.loginwhen == EVERY) || (this.loginwhen == ALL))
								{
									plogin = malloc(ret + 16 + 80);
									if(plogin)
									{
										temp = tcp_login_beforedata(this.client_fd[0], plogin);
										memcpy(plogin + temp , pmalloc , ret);
										send(this.client_fd[0], plogin, ret + temp, 0);
										free(plogin);
										
									}
								}
								else
								{
									send(this.client_fd[0], pmalloc, ret, 0);								
								}
								
								free(pmalloc);
							}
						}
					}
				}
				else if(ret < 0)
				{
					close(this.client_fd[0]);
					log_socklk("socketb", "disconnect"); 					
					break;
				}
				else
				{
					this.ms_cnt++;
					if((this.ms_cnt % 100) == 0)
					{
						this.sec_cnt++;
						// net heart or // comm heart
						if (get_atflag() == 0)
							tcp_heart(this.client_fd[0], this.sec_cnt);
						// data timeout
						this.reconn_cnt++;
						if((portParam.reconnecttimeout) && (this.reconn_cnt >= portParam.reconnecttimeout))
						{
							this.reconn_cnt = 0;
							//log_socklk(iphex2str(this.remote_ip[0].sin_addr.s_addr), 0, "not connected"); 							
							close(this.client_fd[0]);
							break;
						}
					}
				}	
            }
        }
		sleep(5);
    }

	return 0;
}


