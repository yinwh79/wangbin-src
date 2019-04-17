/***********************************************************************************
Copy right:		2015-2025 USR IOT Tech.
Author:			Wangbin
Version:		V1.0
Date:			2015-09
Description:	define serial fifo structure.
***********************************************************************************/
#ifndef	__PUBLIC_H
#define	__PUBLIC_H


#ifdef		__PUBLIC_C
#define		PUBLIC_EXT
#else
#define		PUBLIC_EXT extern
#endif


#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <limits.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>




typedef unsigned char       uint8_t;
typedef signed char         int8_t;
typedef unsigned short int  uint16_t;
typedef signed short int    int16_t;
typedef unsigned int        uint32_t;
typedef signed int          int32_t;
typedef unsigned long long  uint64_t;
typedef signed long long    int64_t;
#define BOOL    uint8_t
#define TRUE	1
#define FALSE	0

#ifndef NULL
#define NULL	0
#endif


#define FLASH_TRUE	0xA5
#define FLASH_FALSE	0x00


//typedef struct sockaddr_in SOCK_ADDR



/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PUBLIC_EXT int atoi_uint32( char *s );



/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PUBLIC_EXT char* strnstr( char *s, const char *find, int slen );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PUBLIC_EXT BOOL CmpStringFront( char *cmped, char *cmping );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PUBLIC_EXT BOOL IsDomain( char* s );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PUBLIC_EXT BOOL IsUnicode( char* s );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PUBLIC_EXT int HexString2Asc( char* pHex, char* pAsc, int HexLen, int AscLen );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PUBLIC_EXT int Asc2HexString( char* pAsc, char* pHex, int AscLen, int HexLen );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PUBLIC_EXT void TeaDecrypt( uint32_t *key, uint32_t *val );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PUBLIC_EXT void TeaEncrypt( uint32_t *key, uint32_t *val );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PUBLIC_EXT void IccidExchage( uint8_t *code );


/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
PUBLIC_EXT int PostStrDecode( char *str );



/******************************************************
Input:		no
Output:	no
Return:	no
Description: no
********************************************************/
//PUBLIC_EXT int FindInvertedPosition( char *src_tail, char *dst, int src_size, int dst_size );




#endif
