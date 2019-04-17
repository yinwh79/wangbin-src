#include <stdio.h>  
#include <stdlib.h>  
#include <sys/socket.h>  
#include <sys/un.h>  
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
#include <syslog.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MAXBUF 1024

// SER2NET_LOGPATH

// ssl show certs
void ShowCerts(SSL * ssl)
{  
    X509 *cert;  
    char *line;  

    cert = SSL_get_peer_certificate(ssl);  
    if (cert != NULL) 
	{  
        printf("Digital cert info:\n");  
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);  
        printf("Cert: %s\n", line);  
        free(line);  
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);  
        printf("Owner: %s\n", line);  
        free(line);  
        X509_free(cert);  
    } else  
        printf("No cert info\n");  
}

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
 
// thread for ssl client
int thread_sslclient(void *arg)
{
	struct hostent *host;
	int ret = 0;
	int i = 0;
	fd_set socketSet;	
	char *pmalloc = 0;
	unsigned int temp = 0;
	socket_s this;
	char *pma = 0;

    int len;    
    SSL_CTX *ctx;    
    SSL *ssl;     

	syslog(LOG_INFO, "ssl client started\n");
	
	// init socket sessions
	thissocket_init(&this);
	// log msg

    while(1)
    {
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
			this.reset_cnt += 10;
			
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
        setsockopt(this.client_fd[0], SOL_SOCKET, SO_KEEPALIVE, (void*)&this.keepAlive,sizeof(this.keepAlive));
        setsockopt(this.client_fd[0], IPPROTO_TCP, TCP_NODELAY, (void *)&this.tcp_nodelay, sizeof(this.tcp_nodelay));

		syslog(LOG_INFO, "ssl client connect to %08X, %d\n", this.remote_ip[0].sin_addr, this.remote_ip[0].sin_port);
		
        ret = connect(this.client_fd[0], (struct sockaddr *)&this.remote_ip[0], sizeof(this.remote_ip[0]));
        if(ret<0)
        {
			syslog(LOG_INFO, "ssl client connect error\n");        
			close(this.client_fd[0]);
			sleep(10);
			this.reset_cnt += 10;
			continue;
        }
        else
        {
			syslog(LOG_INFO, "ssl client connected\n");        
			
			FD_ZERO(&socketSet);
			FD_SET(this.client_fd[0], &socketSet);
			// clear comm buffer
			if(portParam.commbuffer==0)
			{
				for(i=0; i<SOCKET_CLIENT_MAX; i++)
				{
					socket_tx.valid[i] = 0;
				}
				socket_tx.len = 0;
			}
			while(1)
			{
				// serial to ethernet
				// check if serial data received
				if(socket_tx.valid[0] && (this.client_fd[0]>0))
				{
					socket_tx.valid[0] = 0;
					temp = RingBufUsed(&g_sRxBuf[0]);		

					pmalloc = malloc(temp);
					if(pmalloc)
					{
						RingBufRead(&g_sRxBuf[0], pmalloc, temp);
						// ssl start
						// ssl init 
						SSL_library_init();
						OpenSSL_add_all_algorithms();
						SSL_load_error_strings();
						ctx = SSL_CTX_new(SSLv23_client_method());
						if (ctx == NULL) {
							ERR_print_errors_fp(stdout);    
							exit(1);
						}
						// new ssl 
						ssl = SSL_new(ctx);    
						SSL_set_fd(ssl, this.client_fd[0]);    
						if (SSL_connect(ssl) == -1)
							ERR_print_errors_fp(stderr);
						else
						{
							printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
							ShowCerts(ssl);
						}

						// send cmd
						/* 
							GET https://www.alipay.com/
							User-Agent: Mozilla/4.0 
							Host: www.alipay.com
						*/
						len = SSL_write(ssl, pmalloc, temp);
						if (len < 0) 
							printf("message send failed！error id %d，error message '%s' \r\n", errno, strerror(errno));
						else    
							printf("message send success，total send %d byte data！  \r\n", len);
						free(pmalloc);

						// receive data
						pma = malloc(10 * MAXBUF);
						len = SSL_read(ssl, pma, 10 * MAXBUF);
						if (len > 0) 
						{
							// send to serial 
							pthread_mutex_lock(&write_lock);
							if (get_atflag() == 0)
								write(fd_serial, pma, len);
							pthread_mutex_unlock(&write_lock);
							printf("recive message succes total %d byte data \r\n", len);
							
							log_resettimeout(portParam.uartnum, TRUE);							
							this.reconn_cnt = 0;
							this.sec_cnt = 0;
							this.reset_cnt = 0;
						}
						else
						{
							printf("recive message failed %d，error message is '%s'  \r\n",errno, strerror(errno));
						}
						
						free(pma);
						SSL_shutdown(ssl);    
						SSL_free(ssl);    
						close(this.client_fd[0]);    
						SSL_CTX_free(ctx);    
						printf("ssl client closed, id: %d  \n", this.client_fd[0]);
						
						sleep(1);
						this.reset_cnt++;

						break;
					}
				}
            }
        }
    }

	return 0;
}


/*
// 
int thread_sslc_test(int argc, char **argv)    
{
    int sockfd, len;    
    struct sockaddr_in dest;    
    char buffer[MAXBUF + 1];    
    SSL_CTX *ctx;    
    SSL *ssl;    

    if (argc != 3)
	{
		printf("err param ：\n\t\t%s IPaddress port\n\teg:\t%s 127.0.0.1 80\n",argv[0], argv[0]);    
		exit(0); 
	}

    SSL_library_init();    
    OpenSSL_add_all_algorithms();    
    SSL_load_error_strings();    
    ctx = SSL_CTX_new(SSLv23_client_method());    
    if (ctx == NULL) {    
        ERR_print_errors_fp(stdout);    
        exit(1);    
    }    
            
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {    
        perror("Socket");    
        exit(errno);    
    }    
    printf("socket created\n");    
            
    FD_ZERO(&dest);
    dest.sin_family = AF_INET;
    dest.sin_port = htons(atoi(argv[2]));    
    if (inet_aton(argv[1], (struct in_addr *) &dest.sin_addr.s_addr) == 0) {    
        perror(argv[1]);    
        exit(errno);    
    }    
    printf("address created\n");    
    if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0) 
	{    
		perror("Connect ");    
		exit(errno);
	}
	printf("server connected\n");    
       
	ssl = SSL_new(ctx);    
	SSL_set_fd(ssl, sockfd);    
       
	if (SSL_connect(ssl) == -1)    
		ERR_print_errors_fp(stderr);    
	else 
	{
        printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
        ShowCerts(ssl);
    }

	FD_ZERO(buffer);
    len = SSL_read(ssl, buffer, MAXBUF);
    if (len > 0)
        printf("recive message succes:'%s'，total%dbytle data \r\n",buffer, len);
    else {
        printf
        ("recive message failed %d，error message is '%s'  \r\n",errno, strerror(errno));
        goto finish;
    }

	FD_ZERO(buffer);    
    strcpy(buffer, "from client->server");    
            
    len = SSL_write(ssl, buffer, strlen(buffer));    
    if (len < 0)    
        printf("message'%s'send failed！error code id %d，error message is '%s'  \r\n",buffer, errno, strerror(errno));    
    else    
        printf("message'%s'send success，total send %d bytle data！  \r\n",buffer, len);    
            
finish:    
    SSL_shutdown(ssl);    
    SSL_free(ssl);    
    close(sockfd);    
    SSL_CTX_free(ctx);    
    return 0;    
}    
*/