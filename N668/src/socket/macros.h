#ifndef _MACROS_H
#define _MACROS_H

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
#include "ringbuf.h"
#include "tcpc.h"
#include "udp.h"
#include "httpdclient.h"
#include "utils.h"

//
#define SER2NET_LOG 			1
//#define SER2NET_LOGPATH_1		"/tmp/ser2net1.status"

// 
#define FALSE 0
#define TRUE  1
// max define
#define UART_MAX_NUM			8
#define CONNECTION_MAX			16
#define CONNECTION_PCB_MAX		16
#define SOCKET_CLIENT_MAX		2
#define TR_BUFFER_SIZE			10000
#define thread_max 				64
#define LOGIN_STRING_MAX		128
#define HEART_STRING_MAX		128
#define HTTPD_CLIENT_HEADER		512	
#define UCI_STRING_MAX			256 // must bigger than heart and login string 
#define SELECT_TIMEOUT_SEC		0
#define SELECT_TIMEOUT_USEC		10000
#define RESET_TIMEOUT_MAX		600

// workmode
#define WORK_MODE_UDP_CLIENT   	0
#define WORK_MODE_TCP_CLIENT  	1
#define WORK_MODE_UDP_SERVER  	2
#define WORK_MODE_TCP_SERVER  	3
#define WORK_MODE_HTTPD_CLIENT  4
#define WORK_MODE_WEB_UART		5
#define WORK_MODE_TCP_AUTO		6
#define WORK_MODE_SSL_CLIENT	7
#define WORK_MODE_NONE			255
// #define WORK_MODE_COMM_CMD		7

#define KEEPALIVE_IDLE			60
#define KEEPALIVE_INTVL			5
#define KEEPALIVE_COUNT			3

// comm mode
#define SERIAL_FLOW_CONTROL_NONE ((unsigned char)1)
#define SERIAL_FLOW_CONTROL_HW   ((unsigned char)3)
#define SERIAL_FLOW_CONTROL_RS485 ((unsigned char)5)


// socket status
typedef enum
{
    //
    //! The TCP session is idle.  No connection has been attempted, nor has it
    //! been configured to listen on any port.
    //
    STATE_TCP_IDLE,
    //
    //! The TCP session is listening (server mode).
    //
    STATE_TCP_LISTEN,
    //
    //! The TCP session is connecting (client mode).
    //
    STATE_TCP_CONNECTING,
    //
    //! The TCP session is connected.
    //
    STATE_TCP_CONNECTED,
    //
    // The TCP Session have just error
    //
    STATE_TCP_ERROR,
}
tTCPState;

typedef struct{
unsigned char data[TR_BUFFER_SIZE];
unsigned int len;
unsigned int valid[SOCKET_CLIENT_MAX];
}SOCKET_DATA;

typedef struct{
	struct sockaddr_in local_ip;
	unsigned int address_len;  	
	struct sockaddr_in remote_ip[CONNECTION_MAX];
    int client_fd[CONNECTION_MAX];
	int server_fd;
	int keepAlive;
	int keepidle; // 如该连接在60秒内没有任何数据往来,则进行探测
	int keepinterval;// 探测时发包的时间间隔为5 秒
	int keepcount; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.
	
	int tcp_nodelay;
	struct timeval select_timeouts;
	
	unsigned int ms_cnt;
	unsigned int sec_cnt;
	
	unsigned int reconn_cnt;
	
	unsigned int reset_cnt;
	
	unsigned char macaddr[32];
	
	int loginwhen;
}socket_s;


// 
typedef struct
{
	// socketA
	unsigned int workmode;
	char remoteaddr[LOGIN_STRING_MAX];
	unsigned int remoteport;
	unsigned int localport;
	char htptp[16];
	char htpurl[HTTPD_CLIENT_HEADER];
	char htphead[HTTPD_CLIENT_HEADER];
	char htpchd[16];
	// socketB
	unsigned int workmodeb;
	char remoteaddrb[LOGIN_STRING_MAX];
	unsigned int remoteportb;
	// timeout
	unsigned int reconnecttimeout;	
	unsigned int modbustcp;
	int reset_timeout;
	
	// login
	char loginen[16];
	char logintype[16];
	char loginpkt[LOGIN_STRING_MAX];
	char	loginwhen[16];
	unsigned int login_len;
	char cloudid[LOGIN_STRING_MAX];
	char cloudpwd[LOGIN_STRING_MAX];	
	// heart	// commheart
	char hearten[16];
	char heart_netcom[16];
	char heart_aschex[16];
	char heartpkt[HEART_STRING_MAX];
	unsigned int heart_len;
	unsigned int hearttime;
	
	// uart
	unsigned int baud;
	unsigned int databit;
	unsigned int stopbit;
	unsigned int paritybit;
	unsigned int flowcontrol;	//rs485/rtscts
	unsigned int syncbaud;
	// uart packet
	unsigned int commbuffer;
	unsigned int packtime;
	unsigned int packlen;
	// uart num
	unsigned int uartnum;
	char decrypted[16];
}PORTPARAM;


extern SOCKET_DATA socket_tx;
extern SOCKET_DATA socket_rx;
extern int fd_serial;
extern pthread_mutex_t write_lock;

extern void Thread_serialtx(void *arg);
extern void Thread_serialrx(void *arg);
extern PORTPARAM portParam;

extern fd_set writeset;
extern int max_fd;

extern int get_atflag(void);

#endif // _MACROS_H

