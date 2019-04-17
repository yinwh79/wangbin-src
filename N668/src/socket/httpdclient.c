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
#include "utils.h"

// httpd header
struct 
{
	unsigned char data[HTTPD_CLIENT_HEADER];
	unsigned int data_idx;
	unsigned int len; 
	unsigned char send_flag;
}dev_info; 

// 
static int thissocket_init(socket_s *s)
{
	int i = 0;
	
	for(i=0; i<CONNECTION_MAX; i++)
	{
		s->client_fd[i] = -1;
	}
	s->ms_cnt = 0;
	s->sec_cnt = 0;
	s->reconn_cnt = 0;
	s->reset_cnt = 0;


	s->keepAlive = 1;
	s->tcp_nodelay = 1;
	s->select_timeouts.tv_sec = 0;
	s->select_timeouts.tv_usec = 100000; // 100ms
	
	return 0;
}

// initialize httpd header
unsigned int init_httphdr(unsigned char *pcomm, unsigned int content_len)
{
	int len = 0;
	unsigned char *pdata = 0;
	unsigned char octstr[10] = {0};

	memset((char *)dev_info.data, 0, sizeof(dev_info.data));
	// if GET
    if(strncasecmp(portParam.htptp, "GET", strlen("GET"))==0) // 0 get;  1 post
    {
        sprintf((char *)dev_info.data,"GET %s", portParam.htpurl);
        sprintf((char *)dev_info.data,"%s%s",dev_info.data," HTTP/1.1\r\n");
        sprintf((char *)dev_info.data,"%s%s\r\n",dev_info.data, portParam.htphead);
        sprintf((char *)dev_info.data,"%s%s",dev_info.data,"User_Agent: Mozilla/4.0\r\n");              
        sprintf((char *)dev_info.data,"%sHost: %s:%d\r\n\r\n", dev_info.data, 
                    portParam.remoteaddr, portParam.remoteport);
        sprintf((char *)dev_info.data,"%sContent-Length: %3d%s",dev_info.data, content_len, "\r\n");
        dev_info.data_idx = strlen((const char *)dev_info.data); 
		len = ((sizeof(dev_info.data)-dev_info.data_idx) > content_len)?  content_len :  sizeof(dev_info.data)-dev_info.data_idx; 
		pdata = (unsigned char *)strstr((const char *)dev_info.data, "Content-Length: ");
		if(pdata)
		{
			pdata = pdata + strlen("Content-Length: ");
			sprintf((char *)octstr, "%3d", len);
			memcpy((char *)pdata, octstr, strlen((const char *)octstr));
		}
		// add comm data
	    //sprintf((char *)dev_info.data,"%s%s",dev_info.data, pcomm);
		memcpy((char *)dev_info.data + dev_info.data_idx , pcomm, len);
		//dev_info.data[sizeof(dev_info.data)-1] = 0;
		dev_info.len = dev_info.len;	
		return (dev_info.data_idx + len);   
    }

	sprintf((char *)dev_info.data,"POST %s",portParam.htpurl);
	sprintf((char *)dev_info.data,"%s%s",dev_info.data," HTTP/1.1\r\n");
	sprintf((char *)dev_info.data,"%s%s\r\n",dev_info.data, portParam.htphead);
	sprintf((char *)dev_info.data,"%s%s",dev_info.data,"User_Agent: Mozilla/4.0\r\n");
	//sprintf((char *)dev_info.data,"%s%s",dev_info.data, http_hdr);    
	sprintf((char *)dev_info.data,"%sContent-Length: %3d%s",dev_info.data, content_len, "\r\n");
	sprintf((char *)dev_info.data,"%sHost: %s:%d\r\n\r\n", dev_info.data, 
				portParam.remoteaddr, portParam.remoteport);
	dev_info.data_idx = strlen((const char *)dev_info.data);
	len = ((sizeof(dev_info.data)-dev_info.data_idx) > content_len)?  content_len : sizeof(dev_info.data)-dev_info.data_idx;

	pdata = (unsigned char *)strstr((const char *)dev_info.data, "Content-Length: ");
	if(pdata)
	{
		pdata = pdata + strlen("Content-Length: ");
		sprintf((char *)octstr, "%3d", len);
		memcpy((char *)pdata, octstr, strlen((const char *)octstr));
	}
	// add comm data
	//sprintf((char *)dev_info.data,"%s%s",dev_info.data, pcomm);
	memcpy((char *)dev_info.data + dev_info.data_idx, pcomm, len);
	dev_info.len = dev_info.data_idx + len;
	return dev_info.len;

}

// remove httpd header
int httpd_rm_hdr(char *in, char *out, int in_len, char *rm_flag)
{
	char *pent = 0;
	int date_len = 0;
	int temp = 0;
	unsigned char *pdata = 0;
	
	//in_len = strlen(in);
	
	if(strncasecmp(rm_flag, "off", strlen("off"))==0)
	{
		memcpy(out, in, in_len);
		return in_len;
	}

	pdata = (unsigned char *)strstr((const char *)in, "Content-Length: ");
	if(pdata)
	{
		pdata = pdata + strlen("Content-Length: ");
		date_len = atoi(pdata);

		memcpy(out, in + in_len - date_len, date_len);
		return date_len;
	}
	else
	{
		return 0;
	}

}

// thread for tcpc
int thread_httpdclient(void *arg)
{
	struct hostent *host;
	struct sockaddr_in remote_ip;
	int ret = 0;
	int max_fd = 0;
	int keepAlive = 1;// KeepAlive
	int tcp_nodelay = 1;
	struct timeval select_timeouts = {0, 1};
    int client_fd[CONNECTION_PCB_MAX];
	unsigned int idx = 0;
	fd_set readset_tcpsvr;
	fd_set writeset_tcpsvr;
	fd_set socketSet;	
	char *pmalloc = 0;
	unsigned int temp = 0;
	int httpd_send_len = 0;
	socket_s this;

	//printf("httpd client started\n");
	for(idx=0; idx<CONNECTION_PCB_MAX; idx++)
	{
		client_fd[idx] = -1;
	}
	thissocket_init(&this);	
	//sprintf(log_path, "/tmp/log_reset%s", portParam.uartnum);
	//sprintf(log_buf, "%s", portParam.uartnum);	

    while(1)
    {
		// get data from serial 
		while(socket_tx.valid[0] == 0)
		{
			usleep(100000);
			this.ms_cnt++;
			if((this.ms_cnt % 10) == 0)
			{
				this.reset_cnt++;
				if(this.reset_cnt >= portParam.reset_timeout)
				{
					this.reset_cnt = 0;
					log_resettimeout(portParam.uartnum, FALSE);
				}
			}			
		}
	   
		temp = RingBufUsed(&g_sRxBuf[0]);		
		pmalloc = malloc(temp); 
		socket_tx.valid[0] = 0;
		if(pmalloc)
		{
			RingBufRead(&g_sRxBuf[0], pmalloc, temp);  
			httpd_send_len = init_httphdr(pmalloc, temp);
			free(pmalloc);
		}		 
		
        host = gethostbyname((char *)portParam.remoteaddr);

		while(host ==NULL)
		{
			//printf("dns resolve == NULL\n");
			sleep(10);
			host = gethostbyname((char *)portParam.remoteaddr);
			continue;
		}
        memset(&remote_ip, 0, sizeof(remote_ip));
        remote_ip.sin_family = AF_INET;
        remote_ip.sin_port = htons(portParam.remoteport);
		remote_ip.sin_addr = *((struct in_addr *)host->h_addr);

        if((remote_ip.sin_addr.s_addr==0xffffffff)||(remote_ip.sin_addr.s_addr==0))
        {
			//printf("remote addr is ff or 00\n");
			sleep(10);
			continue;
        }
        
        client_fd[0] = socket(AF_INET, SOCK_STREAM, 0);
        if(client_fd[0]<0)
        {
			//printf("create socket fail\n");
			sleep(10);
			continue;
        }
        // set keepalive = 1; tcp nodelay = 1
        ret = connect(client_fd[0], (struct sockaddr *)&remote_ip, sizeof(remote_ip));

        if(ret<0)
        {
			close(client_fd[0]);
			//printf("connect fail\n");
			sleep(5);
			continue;
        }
        else
        {
			FD_ZERO(&socketSet);
			FD_SET(client_fd[0], &socketSet);
			
			send(client_fd[0], dev_info.data, httpd_send_len, 0);
			//printf("httpdc send: \n%s\n", dev_info.data);

			while(1)
			{
				FD_ZERO(&readset_tcpsvr);
				FD_ZERO(&writeset_tcpsvr);
				readset_tcpsvr = socketSet;
				max_fd = client_fd[0];
				
				select_timeouts.tv_sec = 0;
				select_timeouts.tv_usec = 10000;
				ret = select(max_fd+1, &readset_tcpsvr, NULL, NULL, &select_timeouts);
				if(ret > 0)
				{
					if(FD_ISSET(client_fd[0], &readset_tcpsvr))
					{
						//ret = recv(client_fd[0], p_rev, TR_BUFFER_SIZE, 0);
						socket_rx.len = recv(client_fd[0], socket_rx.data, sizeof(socket_rx.data), 0);
						if(socket_rx.len <= 0)
						{
							close(client_fd[0]);
							this.reconn_cnt = 0;
							//printf("socket closed\n");
							break;
						}
						else
						{
							//socket_rx.data[socket_rx.len] = 0;
							//printf("httpdc rev length: %d\n", socket_rx.len);
							temp = socket_rx.len;
							pmalloc = malloc(temp);
							if(pmalloc)
							{
								// remove httpd header??								
								temp = httpd_rm_hdr(socket_rx.data, pmalloc, socket_rx.len, portParam.htpchd);
								//printf("httpdc rev filter len %d: %s\n", temp, pmalloc);
								
								// send to serial
								pthread_mutex_lock(&write_lock);
								if (get_atflag() == 0)
									write(fd_serial, pmalloc, temp);
								pthread_mutex_unlock(&write_lock);
								this.reconn_cnt = 0;
								
								free(pmalloc);
							}
							
							log_resettimeout(portParam.uartnum, TRUE);
							
							this.reconn_cnt = 0;
							this.sec_cnt = 0;
							this.reset_cnt = 0;
						}
					}
				}
				else
				{
					this.ms_cnt++;
				}
				
				// seconds task
				if(this.ms_cnt >= 100)
				{
					this.ms_cnt = 0;
					// no data timeout
					this.sec_cnt++;
					this.reconn_cnt++;
					this.reset_cnt++;

					if(this.reconn_cnt >= 3)
					{
						this.reconn_cnt = 0;
						close(client_fd[0]);
						//printf("socket closed\n");
						break;
					}
				}
				
            }
        }
    }

	return 0;	
}
