/***********************************************************************************
Copy right:		2015-2025 USR IOT Tech.
Author:			Wangbin
Version:		V1.0
Date:			2015-09
Description:	led setting.
***********************************************************************************/
#ifndef	__SYSTEM_H
#define	__SYSTEM_H


#ifdef	__SYSTEM_C
#define	SYSTEM_EXT
#else
#define	SYSTEM_EXT extern
#endif


#include "public.h"
#include "usr_define.h"



/******************************************************
Input:	no
Output:	no
Return:	no
Description: no
********************************************************/
SYSTEM_EXT void System_GetRunTime( char *buf, int buf_size );



#endif
