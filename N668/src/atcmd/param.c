/***********************************************************************************
Copy right:	2014-2024 USR IOT Tech.
Author:			Bin Wang
Version:			V1.0
Date:				2014-08
Description:	save parameters.
***********************************************************************************/
#define __PARAM_C


#include "param.h"


static int fd_UsrFactory, fd_UserDefault, fd_UserRun, fd_ConstPara;

#define FACTORY_MAC_ADDR	0
#define FACTORY_MAC_WPATH		"/tmp/factory_write"
#define FACTORY_MAC_RPATH		"/tmp/factory_read"

//
int file_write(char *filename, char *content, unsigned int writelen)
{
	int fd_dev = 0;
	
	fd_dev=open((const char *)filename, O_WRONLY|O_CREAT|O_APPEND|O_TRUNC);
	if(fd_dev < 0)
	{
		printf("file open fail \n");
		return -1;
	}
	write(fd_dev, content, writelen);	
	close(fd_dev);

}

// read out file
int file_read(char *filename, char *out, int len)
{
	int fd_dev = 0;
	int i = 0;
	
	fd_dev=open(filename,O_RDONLY);
	if(fd_dev < 0)
	{
		printf("file open fail \n");
	}
	read(fd_dev, out, len);
	close(fd_dev);
	
}

int factory_write(char *in, int paramlen)
{
	int i = 0;
	char buffer[1024] = {0};
	
	// read back
	//factory_read(buffer, sizeof(buffer));
	// form txt
	file_write(FACTORY_MAC_WPATH, in, paramlen);
	sprintf(buffer, "flashcp %s /dev/mtd2", FACTORY_MAC_WPATH);
	// write to mtd2
	system(buffer);
	return 0;
}

int factory_read(char *out, int paramlen)
{
	int i = 0;
	char buffer[1024] = {0};
	char shell_ret[1024] = {0};
	
	// mtd_debug read /dev/mtd2 0 128 /tmp/readmtd
	sprintf(buffer, "mtd_debug read /dev/mtd2 0 %d %s", sizeof(buffer), FACTORY_MAC_RPATH);
	// get 
	shell_get(buffer, shell_ret, sizeof(shell_ret));
	//read mac addr
	file_read(FACTORY_MAC_RPATH, out, paramlen);
	
	//memset(out, buffer, paramlen);
	
	return 0;
}

