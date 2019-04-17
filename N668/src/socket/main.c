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
#include <pthread.h>
#include <time.h> 
#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>
#include "macros.h"
#include "tcpc.h"
#include "tcps.h"
#include "udp.h"
#include "httpdclient.h"
#include "sslc.h"
#include <syslog.h>
#include <sys/select.h>

// 
SOCKET_DATA socket_tx;
SOCKET_DATA socket_rx;
pthread_mutex_t write_lock;

int fd_serial = 0;
char *dev[10]={"/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3", "/dev/ttyS4", "/dev/ttyS5",
				"/dev/ttyS6", "/dev/ttyS7", "/dev/ttyS8", };
static char ring_buf[TR_BUFFER_SIZE] = {0};
static char ring_bufb[TR_BUFFER_SIZE] = {0};

PORTPARAM portParam;
int atcmd_flag = 0;

// get at cmd
int get_atflag(void)
{
	return atcmd_flag;
}

//
int workmode_atoi(char *workmodestr)
{
	int work_int = 0;
	
	if(strncasecmp(workmodestr, "udpc", strlen("udpc"))==0)
	{
		work_int = WORK_MODE_UDP_CLIENT;
	}
	else if(strncasecmp(workmodestr, "tcpc", strlen("tcpc"))==0)
	{
		work_int = WORK_MODE_TCP_CLIENT;
	}
	else if(strncasecmp(workmodestr, "udps", strlen("udps"))==0)
	{
		work_int = WORK_MODE_UDP_SERVER;
	}
	else if(strncasecmp(workmodestr, "tcps", strlen("tcps"))==0)
	{
		work_int = WORK_MODE_TCP_SERVER;
	}
	else if(strncasecmp(workmodestr, "htpc", strlen("htpc"))==0)
	{
		work_int = WORK_MODE_HTTPD_CLIENT;
	}
	else if(strncasecmp(workmodestr, "sslc", strlen("sslc"))==0)
	{
		work_int = WORK_MODE_SSL_CLIENT;
	}	
	else 
	{
		work_int = WORK_MODE_NONE;
	}
	return work_int;
}

int param_init(char *confile, unsigned int uartnum)
{
	char ret_buf[UCI_STRING_MAX] = {0};
	
	// uart 
	portParam.baud = atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].baud", ret_buf, sizeof(ret_buf), uartnum));
	portParam.databit = atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].databit", ret_buf, sizeof(ret_buf), uartnum));
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].paritybit", ret_buf, sizeof(ret_buf), uartnum);
	portParam.paritybit = ret_buf[0];
	portParam.stopbit = atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].stopbit", ret_buf, sizeof(ret_buf), uartnum));
	// socketA
	portParam.workmode = workmode_atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].workmode", ret_buf, sizeof(ret_buf), uartnum));
	atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].remoteaddr", portParam.remoteaddr, sizeof(portParam.remoteaddr), uartnum));
	portParam.remoteport = atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].remoteport", ret_buf, sizeof(ret_buf), uartnum));
	portParam.localport = atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].localport", ret_buf, sizeof(ret_buf), uartnum));
	// modbustcp
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].modbustcp", ret_buf, sizeof(ret_buf), uartnum);
	if(strncasecmp(ret_buf, "ON", strlen("ON"))==0)
	{
		portParam.modbustcp = 1;
	}
	else
	{
		portParam.modbustcp = 0;
	}
	//sync baud
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].syncbaud", ret_buf, sizeof(ret_buf), uartnum);
	if(strncasecmp(ret_buf, "ON", strlen("ON"))==0)
	{
		portParam.syncbaud = 1;
	}
	else
	{
		portParam.syncbaud = 0;
	}
	// socketB
	portParam.workmodeb = workmode_atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].workmodeb", ret_buf, sizeof(ret_buf), uartnum));
	atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].remoteaddrb", portParam.remoteaddrb, sizeof(portParam.remoteaddrb), uartnum));
	portParam.remoteportb = atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].remoteportb", ret_buf, sizeof(ret_buf), uartnum));
	
	// login
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].loginen", portParam.loginen, sizeof(portParam.loginen), uartnum);
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].logintype", portParam.logintype, sizeof(portParam.logintype), uartnum);
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].loginpkt", portParam.loginpkt, sizeof(portParam.loginpkt), uartnum);
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].loginwhen", portParam.loginwhen, sizeof(portParam.loginwhen), uartnum);
	portParam.login_len = atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].login_len", ret_buf, sizeof(ret_buf), uartnum));
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].d2did", portParam.cloudid, sizeof(portParam.cloudid), uartnum);
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].d2dpwd", portParam.cloudpwd, sizeof(portParam.cloudpwd), uartnum);
	// heart
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].hearten", portParam.hearten, sizeof(portParam.hearten), uartnum);
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].heartpkt", portParam.heartpkt, sizeof(portParam.heartpkt), uartnum);
	// hearttp net or com
	// hearttype ascii or hex
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].hearttp", portParam.heart_netcom, sizeof(portParam.heart_netcom), uartnum);	
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].heart_aschex", portParam.heart_aschex, sizeof(portParam.heart_aschex), uartnum);
	portParam.hearttime = atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].hearttime", ret_buf, sizeof(ret_buf), uartnum));
	portParam.heart_len = atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].heart_len", ret_buf, sizeof(ret_buf), uartnum));
	// reconnect 
	portParam.reconnecttimeout = atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].reconnecttimeout", ret_buf, sizeof(ret_buf), uartnum));

	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].htptp", portParam.htptp, sizeof(portParam.htptp), uartnum);
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].htpurl", portParam.htpurl, sizeof(portParam.htpurl), uartnum);
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].htphead", portParam.htphead, sizeof(portParam.htphead), uartnum);
	shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].htpchd", portParam.htpchd, sizeof(portParam.htpchd), uartnum);

	// packtime and packlen
	portParam.packtime = atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].packtime", ret_buf, sizeof(ret_buf), uartnum));
	portParam.packlen = atoi(shell_get_ser2net("uci get -q ser2net%d.@ser2net[0].packlen", ret_buf, sizeof(ret_buf), uartnum));

	portParam.reset_timeout = atoi(shell_get_ser2net("uci get -q system.reset.timeout", ret_buf, sizeof(ret_buf), uartnum));
	if(portParam.reset_timeout < RESET_TIMEOUT_MAX)
	{
		portParam.reset_timeout = RESET_TIMEOUT_MAX;
	}
	
	// try to see if decryted

	#if 1
	shell_get("cat /tmp/getmac0_dec", portParam.decrypted, sizeof(portParam.decrypted));
	if(strncasecmp(portParam.decrypted, "yes", strlen("yes")) != 0)
	{
		portParam.workmode = WORK_MODE_NONE;
		portParam.workmodeb = WORK_MODE_NONE;
	}

	#endif
	
	
	return 0;
}

int packtime_us = 0;
#define PACKTIME_MIN	10000   // 10ms

int packtime_get(unsigned int baudrate, int nbytes)
{
	int pack_time = 0;

	pack_time = portParam.packtime * 1000;//10 * ((1000000*10*nbytes)/baudrate);

	if(pack_time <= PACKTIME_MIN)
	{
		pack_time = PACKTIME_MIN;
	}
	
	return pack_time;
}

int packtime_get_bybaud(unsigned int baudrate, int nbytes)
{
	int pack_time = 0;

	pack_time = 10 * ((1000000*10*nbytes)/baudrate);

	if(pack_time <= PACKTIME_MIN)
	{
		pack_time = PACKTIME_MIN;
	}
	
	return pack_time;
}


//int sec;
void sigroutine(int signo){
	int i = 0;
	int temp = 0;
	char *pmalloc = 0;

    switch (signo){
        case SIGALRM:
		
		temp = RingBufUsed(&g_sRxBuf[0]);
		//printf("sigroutine：%d\n", temp);
		if(temp)
		{
			// at cmd
			if(atcmd_flag==1)
			{
				pmalloc = malloc(temp+1);
				if(pmalloc)
				{
					RingBufRead(&g_sRxBuf[0], pmalloc, temp);
				}
				else
				{
					RingBufFlush(&g_sRxBuf[0]);
					break;
				}
				
				// check if exit 
				if((temp>=strlen("at+entm\r"))
					&&(strncasecmp(pmalloc, "at+entm\r", strlen("at+entm\r"))==0))
				{
					write(fd_serial, "\r\n+OK\r\n", strlen("\r\n+OK\r\n"));
					RingBufFlush(&g_sRxBuf[0]);
					RingBufFlush(&g_sRxBuf[1]);
					atcmd_flag = 0;
				}
				else
				{
					pmalloc[temp] = 0;
					printf ("at-rev: %d, %s\n", temp, pmalloc);
					
					AtCmd_Dispatch(pmalloc);
					write(fd_serial, AtCmd_getrsp_ptr(), strlen(AtCmd_getrsp_ptr()));
				}	
				free(pmalloc);
			}
			else
			{
				for(i=0; i<SOCKET_CLIENT_MAX; i++)
				{
					socket_tx.valid[i] = 1;
				}
			}
		}
		break;
		
        case SIGVTALRM:
		printf("unknown signal -- SIGVTALRM \n");
		//signal(SIGVTALRM, sigroutine);
		break;
    }
    return;
}

void init_commtimer(void)
{
	struct itimerval value, ovalue;          //(1)

	signal(SIGALRM, sigroutine);

	value.it_value.tv_sec = packtime_us / 1000000;
	value.it_value.tv_usec = packtime_us % 1000000;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &value, &ovalue);     //(2)
	
}

int speed_arr[] = { 
B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B600, B300, 
B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B600, B300, };  
int name_arr[] = {
115200, 57600, 38400,  19200,  9600,  4800,  2400,  1200, 600, 300, 
115200, 57600, 38400, 19200,  9600, 4800, 2400, 1200,  600, 300, };  

void set_speed(int fd, int speed){
	int   i;   
	int   status;   
	struct termios   Opt;  
	tcgetattr(fd, &Opt);   

    Opt.c_iflag &= ~ (INLCR | ICRNL | IGNCR);
    Opt.c_oflag &= ~(ONLCR | OCRNL);
    Opt.c_iflag &= ~(IXON);

	for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) {   
		if  (speed == name_arr[i]) {
		  tcflush(fd, TCIOFLUSH);       
		  cfsetispeed(&Opt, speed_arr[i]);    
		  cfsetospeed(&Opt, speed_arr[i]);     
		  status = tcsetattr(fd, TCSANOW, &Opt);    
		  if  (status != 0) {          
			perror("tcsetattr fd1");    
			return;       
		  }
		  tcflush(fd,TCIOFLUSH);     
		}    
	} 
}

int set_Parity(int fd,int databits,int parity,int stopbits)
{
	struct termios options; 
	if  ( tcgetattr( fd,&options)  !=  0) { 
		perror("SetupSerial 1");     
		return(FALSE);  
	}
	options.c_cflag &= ~CSIZE; 
	switch (databits) 
	{
	case 7:
		options.c_cflag |= CS7; 
		break;
	case 8:
		options.c_cflag |= CS8;
		break;
	default:
		fprintf(stderr,"Unsupported data size\n"); 
		return (FALSE);
	}
	
	switch (parity) 
	{   
	case 'n':
	case 'N':    
		options.c_cflag &= ~PARENB;   /* Clear parity enable */
		options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
		break;  
	case 'o':   
	case 'O':     
		options.c_cflag |= (PARODD | PARENB); 
		options.c_iflag |= INPCK;             // Disnable parity checking 
		break;  
	case 'e':  
	case 'E':   
		options.c_cflag |= PARENB;   
		options.c_cflag &= ~PARODD;   
		options.c_iflag |= INPCK;       // Disnable parity checking */
		break;
	case 'S': 
	case 's':  /*as no parity*/   
	    options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;  
	default:   
		fprintf(stderr,"Unsupported parity\n");    
		return (FALSE);  
	}  
	
	switch (stopbits)
	{   
		case 1:    
			options.c_cflag &= ~CSTOPB;  
			break;  
		case 2:    
			options.c_cflag |= CSTOPB;  
		   break;
		default:    
		 fprintf(stderr,"Unsupported stop bits\n");  
		 return (FALSE); 
	} 
	/* Set input parity option */ 
	if (parity != 'n')   
		options.c_iflag |= INPCK; 

	options.c_cflag |= (CLOCAL | CREAD);     
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);       
	options.c_oflag &= ~OPOST;     
	options.c_oflag &= ~(ONLCR | OCRNL);   //  
	options.c_iflag &= ~(ICRNL | INLCR);     
	options.c_iflag &= ~(IXON | IXOFF | IXANY);    //
	tcflush(fd, TCIFLUSH);     
	options.c_cc[VTIME] = 0;        //     
	options.c_cc[VMIN] = 1; 

	if (tcsetattr(fd,TCSANOW,&options) != 0)
	{
		perror("SetupSerial 3");   
		return (FALSE);  
	} 
	return (TRUE);  
}

int set_Parity_rxtime(int fd,int databits,int parity,int stopbits)
{
	struct termios options; 
	
	printf("set parity : %d, %c, %d \n", databits, parity, stopbits);
	
	if  ( tcgetattr( fd,&options)  !=  0) { 
		perror("SetupSerial 1");     
		return(FALSE);  
	}
	options.c_cflag &= ~CSIZE; 
	switch (databits) 
	{
	case 7:
		options.c_cflag |= CS7; 
		break;
	case 8:
		options.c_cflag |= CS8;
		break;
	default:
		fprintf(stderr,"Unsupported data size\n"); 
		return (FALSE);
	}
	
	switch (parity) 
	{   
	case 'n':
	case 'N':    
		options.c_cflag &= ~PARENB;   /* Clear parity enable */
		options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
		break;  
	case 'o':
	case 'O':
		options.c_cflag |= (PARODD | PARENB); 
		options.c_iflag |= INPCK;             // Disnable parity checking 
		break;  
	case 'e':  
	case 'E':   
		options.c_cflag |= PARENB;   
		options.c_cflag &= ~PARODD;   
		options.c_iflag |= INPCK;       // Disnable parity checking */
		break;
	case 'S': 
	case 's':  /*as no parity*/   
	    options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;
	default:
		fprintf(stderr,"Unsupported parity\n");    
		return (FALSE);  
	}  
	
	switch (stopbits)
	{   
		case 1:    
			options.c_cflag &= ~CSTOPB;  
			break;  
		case 2:    
			options.c_cflag |= CSTOPB;  
		   break;
		default:    
		 fprintf(stderr,"Unsupported stop bits\n");  
		 return (FALSE); 
	} 
	/* Set input parity option */ 
	if (parity != 'n')   
		options.c_iflag |= INPCK; 

	options.c_cflag |= (CLOCAL | CREAD);     
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);       
	options.c_oflag &= ~OPOST;     
	options.c_oflag &= ~(ONLCR | OCRNL);   //  
	options.c_iflag &= ~(ICRNL | INLCR);     
	options.c_iflag &= ~(IXON | IXOFF | IXANY);    //
	tcflush(fd, TCIFLUSH);     
	options.c_cc[VTIME] = 3;        //     
	options.c_cc[VMIN] = 3; 

	if (tcsetattr(fd,TCSANOW,&options) != 0)   
	{ 
		perror("SetupSerial 3");   
		return (FALSE);  
	} 
	return (TRUE);  
}

// 
int get_jjja_char(int *atflag, char *buf, int buf_len)
{
	int already_recved = 0;
	static char jjja_buf[3] = {0};
	static int jjja_len = 0;
	int nread = 0;
	int i = 0;

	// +++
	if((memcmp(buf, "+++", buf_len) != 0))
	{
		jjja_len = 0;
		return -1;
	}
	if((buf_len < 3) && ((jjja_len+buf_len) < 3) )
	{
		memcpy(jjja_buf + jjja_len, buf, buf_len);
		jjja_len += buf_len;
		printf("recved:%s\n", buf);
		
		return -2;
	}
	else if(((jjja_len+buf_len) == 3) && (*atflag==0))
	{
		// valid +++ , pass
		memcpy(jjja_buf + jjja_len, buf, buf_len);		
		printf("valid +++ \n");
	}
	else
	{
		jjja_len = 0;
		return -1;
	}
	
	// a 
	if((memcmp(jjja_buf, "+++", 3)==0) )
	{
		write(fd_serial, "a", strlen("a"));
		set_Parity_rxtime(fd_serial, portParam.databit, portParam.paritybit, portParam.stopbit);
		i=0;
		while(i<20)
		{
			i++;
			sleep(0.1);
			nread=read(fd_serial, buf, sizeof(buf));
			if(nread) break;
		}
		RingBufWrite(&g_sRxBuf[0], buf, nread);
		if((nread==1)&&(buf[0]=='a'))
		{
			write(fd_serial, "+ok", strlen("+ok"));
			*atflag = 1;
			RingBufFlush(&g_sRxBuf[0]);
			RingBufFlush(&g_sRxBuf[1]);
		}
		else
		{
			*atflag = 0;
			
			init_commtimer();
		}
		set_Parity(fd_serial, portParam.databit, portParam.paritybit, portParam.stopbit);
		jjja_len = 0;
		packtime_us = packtime_get_bybaud(portParam.baud, 5);	
		
		
		return 0;
	}
	else
	{
		return -1;
	}
}
// the main process
int main(int argc,char *argv[])
{
	int nread;
	pthread_t thread_sid[3];
	unsigned int i = 0;
	unsigned char serial_num = 0;
	char serial_name[50] = {0};
	char serial_buf[1024] = {0};
	int nread_len = 0;

	atcmd_flag = 0;

	// ./USR_N668 1 /dev/ttyS1 
	// ./USR_N668 2 /dev/ttyS2 
	// UART_MAX_NUM
	syslog(LOG_INFO, "N668 task Input param: %s - %s - %s - %s\n", argv[0], argv[1], argv[2], argv[3]);
	serial_num = atoi(argv[1]);
	if((serial_num==0)||(serial_num>UART_MAX_NUM))
	{
		syslog(LOG_INFO, "Error serialnum: %d\n", serial_num);
		return -1;
	} 
	memset(serial_name, 0, sizeof(serial_name));
	strcpy(serial_name, argv[2]);
	syslog(LOG_INFO, "Get serialnum: %d, name: %s\n", serial_num, serial_name);
	
	// get uart parameters
	param_init(0, serial_num);
	portParam.uartnum = serial_num;
	
	pthread_mutex_init(&write_lock, NULL);
	
	syslog(LOG_INFO, "socketA start: %d, name: %d\n", serial_num, portParam.workmode);
	
	//SocketA
	switch(portParam.workmode)
	{
		case WORK_MODE_UDP_CLIENT: // udp client
			pthread_create(&thread_sid[1], NULL, thread_udp, (void *)0);
			break;
		case WORK_MODE_TCP_CLIENT:// tcp client
			pthread_create(&thread_sid[1], NULL, thread_tcpclient, (void *)0);
			break;
		case WORK_MODE_UDP_SERVER:// udp server
			pthread_create(&thread_sid[1], NULL, thread_udp, (void *)0);
			break;
		case WORK_MODE_TCP_SERVER:// tcp server
			pthread_create(&thread_sid[1], NULL, thread_tcpserver, (void *)(0));
			break; 
		case WORK_MODE_HTTPD_CLIENT:// 
			pthread_create(&thread_sid[1], NULL, thread_httpdclient, (void *)(0));
			break;
		case WORK_MODE_SSL_CLIENT:
			pthread_create(&thread_sid[1], NULL, thread_sslclient, (void *)(0));
		default:
			syslog(LOG_INFO, "workmode error: %d\n", portParam.workmode);		
			break;
	} 

	// SocketB
	switch(portParam.workmodeb)
	{
		case WORK_MODE_UDP_CLIENT: // udp client
			pthread_create(&thread_sid[2], NULL, thread_udpb, (void *)1);
			break;
		case WORK_MODE_TCP_CLIENT:// tcp client
			pthread_create(&thread_sid[2], NULL, thread_tcpclientb, (void *)1);
			break;
/* 		case WORK_MODE_UDP_SERVER:// udp server
			pthread_create(&thread_sid[2], NULL, thread_udp, (void *)1);
			break;
		case WORK_MODE_TCP_SERVER:// tcp server
			pthread_create(&thread_sid[2], NULL, thread_tcpserver, (void *)(1));
			break;
		//case WORK_MODE_HTTPD_CLIENT:// httpd client
		//	pthread_create(&thread_sid[2], NULL, thread_httpdclient, (void *)(&portParam));
		//	break;
 */		default:
			syslog(LOG_INFO, "workmodeb error: %d\n", portParam.workmodeb);
			break;
	}

	// thread for serial tx & rx
	syslog(LOG_INFO, "\n UART Opening... \n");
	if((serial_num==0)||(serial_num>UART_MAX_NUM))
	{
		syslog(LOG_INFO, "Error serialnum: %d\n", serial_num);
		return -1;
	}
	fd_serial = open(dev[portParam.uartnum-1], O_RDWR|O_NOCTTY);//|O_NDELAY);
	if(fd_serial == -1)
	{
		syslog(LOG_INFO, "ttyS%d open fail\n", portParam.uartnum);
		write(fd_serial, "uart open fail\n", strlen("uart open fail\n"));		
		return -1;
	}
	else
	{
		syslog(LOG_INFO, "ttyS%d open success!............\n", portParam.uartnum);
		set_speed(fd_serial, portParam.baud);
		set_Parity(fd_serial, portParam.databit, portParam.paritybit, portParam.stopbit);
	}
	
	// get pack time
	RingBufInit(&g_sRxBuf[0], ring_buf, sizeof(ring_buf));
	RingBufInit(&g_sRxBuf[1], ring_bufb, sizeof(ring_bufb));
	packtime_us = packtime_get(portParam.baud, 5);	
	
	printf("Ser2net-%d, packtime:%d us \n", serial_num, packtime_us);

	while(1)
	{
		nread=read(fd_serial, serial_buf, sizeof(serial_buf));
		// printf("ser rev: %d\n", nread);
		
		if(nread>0)
		{
			// g_sRxBuf[0]
			RingBufWrite(&g_sRxBuf[0], serial_buf, nread);
			if(portParam.workmodeb != WORK_MODE_NONE)
			{
				RingBufWrite(&g_sRxBuf[1], serial_buf, nread);
			}
			//
			nread_len = RingBufUsed(&g_sRxBuf[0]);
			
			//printf ("ser-rev: %d,%d,%d,%s\n", atcmd_flag, nread, nread_len, serial_buf);
			// check if ->atcmd
			// int get_jjja_char(int *atflag, char *buf, int buf_len)
			if(get_jjja_char(&atcmd_flag, serial_buf, nread) == 0)
			{
				/* write(fd_serial, "a", strlen("a"));
				set_Parity_rxtime(fd_serial, portParam.databit, portParam.paritybit, portParam.stopbit);
				i=0;
				while(i<20)
				{
					i++;
					sleep(0.1);
					nread=read(fd_serial, serial_buf, sizeof(serial_buf));
					if(nread) break;
				}
				RingBufWrite(&g_sRxBuf[0], serial_buf, nread);
				if((nread==1)&&(serial_buf[0]=='a'))
				{
					write(fd_serial, "+ok", strlen("+ok"));
					atcmd_flag = 1;
					RingBufFlush(&g_sRxBuf[0]);
					RingBufFlush(&g_sRxBuf[1]);					
				}
				else
				{
					atcmd_flag = 0;
					
					init_commtimer();
				}
				set_Parity(fd_serial, portParam.databit, portParam.paritybit, portParam.stopbit); */
			}
			else
			{
				init_commtimer();
				
				if((nread_len >= portParam.packlen) && (atcmd_flag==0))
				{
					for(i=0; i<SOCKET_CLIENT_MAX; i++)
					{
						socket_tx.valid[i] = 1;
					}
					
				}
			}
			
		}
	}		

	
	// exit thread 
	pthread_join(thread_sid[1], NULL);
	pthread_join(thread_sid[2], NULL);
	pthread_exit(NULL);

	return 0;  
} 
