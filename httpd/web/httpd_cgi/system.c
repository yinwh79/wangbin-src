/***********************************************************************************
Copy right:	2014-2024 USR IOT Tech.
Author:			Bin Wang
Version:			V1.0
Date:				2014-08
Description:	led.
***********************************************************************************/
#define __SYSTEM_C


#include "system.h"


void System_GetRunTime( char *buf, int buf_size )
{
	FILE *fp;
	
    fp = popen( "cat /proc/uptime | awk '{print $1}'", "r" );
		
	if( fp )
	{
		memset( buf, 0, buf_size );
		fgets( buf, buf_size, fp );
		
		pclose(fp);
	}
}


