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
#include <stdint.h>
#include "macros.h"


int comm_heart(int fd, char *heart, unsigned int len)
{
	write(fd, heart, len);
	
	return len;
}

// xrfc2217
int xfrc2217(int commfd, char *pdata, unsigned int len)
{
	unsigned int i = 0;
	unsigned char sum = 0;
	unsigned char ucData = 0;
	unsigned int baud;
	unsigned int databit;
	unsigned int stopbit;
	unsigned int paritybit;

	if(len != 8)
	{
		return -2;
	}
	
	// judge
	if((len==8)&&(pdata[0]==0x55)&&(pdata[1]==0xaa)&&(pdata[2]==0x55))
	{
		for(sum=0,i=0; i<4; i++)
		{
			sum += pdata[3+i];
		}
		if(sum==pdata[7])
		{
			// baud
			baud = ((unsigned int)(pdata[3])<<16) +((unsigned int)(pdata[4])<<8) +(unsigned int)(pdata[5]);

			ucData = pdata[6];
			//datasize
			switch(ucData & 0x03)
			{
				case 0x00:
					databit = 5;
					break;
				case 0x01:
					databit = 6;
					break;	
				case 0x02:
					databit = 7;
					break;	
				case 0x03:
					databit = 8;
					break;	
				default:
					databit = 8;
					break;		
			}
			// stopbit
			switch((ucData & 0x04) >> 2)
			{
				case 0x00:// 1 stopbits
					stopbit = 1;
					break;
				case 0x01:// 2 stopbits
					stopbit = 2;
					break;
				default:
					stopbit = 1;
					break;
			}
			//parity
			if((ucData & 0x08))
			{
				switch((ucData & 0x30) >> 4)
				{
					case 0x00:// odd
						paritybit = 'O';
						break;
					case 0x01:// even
						paritybit = 'E';
						break;
					case 0x02:// mark
						paritybit = 'M';
						break;
					case 0x03:// clear
						paritybit = 'S';
						break;
					default:
						paritybit = 'N';
						break;
				}
			}
			else
			{
				paritybit = 'N';
			}
			// set current
			set_speed(commfd, baud);
			set_Parity(fd_serial, databit, paritybit, stopbit);

			return(0);
		}
	}
	
	return -1;
}


//
// Modbus TCP struct typedef
// Example 1 
// ModbusTCP request 
// 15 01 usTransmitFlag
// 00 00 usProtocolFlag
// 00 06 usLength
// ff    ucUnitFlag
// 03 	 ucFuncCode
// 00 05 Start address
// 00 01 register quantity

// Example 2
// 
// 000000000006 010400010012
// receive
// 000000000027 010424000000 000000000000 000000000000 
// 000000000000 000000000000 000000000000 000000
typedef struct 
{
	//
	// MBAP head
	//
	//
	// usTransmitFlag, 2bytes. indicating specific Modbus frame id when POLL / ACK
	//
	unsigned short usTransmitFlag;
	//
	// ProtocolFlag
	//  = 0 Modbus Protocol /  = 1 UNI-TE protocol
	// when response, copy this value
	//
	unsigned short usProtocolFlag;
	//
	// follow Byte count(total bytes)
	//
	unsigned short usLength;
	//
	// unit flag, define continue other target device
	// original slave address
	//
	unsigned char ucUnitFlag;// salve address
}modbustcp_hdr;

//
// define one variables for each port
//
union
{
	modbustcp_hdr header;
	unsigned char header_data[sizeof(modbustcp_hdr)];
}modbustcp_frame;

//
// Modbus Data
//
struct 
{
	unsigned char ucFuncCode;
	unsigned short usLength;
}
g_ModbusData = 
{
0, 
0
};

// this is table for CRC16 
static unsigned int ccitt_table[256] = 
{
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280,
	0xC241, 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481,
	0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81,
	0x0E40, 0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880,
	0xC841, 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81,
	0x1A40, 0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80,
	0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680,
	0xD641, 0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081,
	0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281,
	0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480,
	0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80,
	0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881,
	0x3840, 0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80,
	0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81,
	0x2C40, 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681,
	0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080,
	0xE041, 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281,
	0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480,
	0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80,
	0xAE41, 0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881,
	0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80,
	0xBA41, 0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81,
	0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681,
	0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080,
	0xB041, 0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280,
	0x9241, 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481,
	0x5440, 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81,
	0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880,
	0x9841, 0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81,
	0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80,
	0x8C41, 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680,
	0x8641, 0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081,
	0x4040, 
	};

//
// CRC16 
//
unsigned int crc16(unsigned char *q, int len)
{
	unsigned int crc = 0xffff;
	
	while (len-- > 0)
	crc = ccitt_table[(crc ^ *q++) & 0xff] ^ (crc >> 8);

	return crc;
}

//
// crc16 one byte
//
unsigned short crc16_oneByte(unsigned short crcbefore, unsigned char data)
{
	unsigned int crc = 0xffff;

	crc = ccitt_table[(crcbefore ^ data) & 0xff] ^ (crcbefore >> 8);
	return crc;
}

//
// rev_modbustcp_frame
// parse a modbustcp frame
//
// Modbus TCP struct typedef
// Example 1 
// ModbusTCP request 
// 15 01 usTransmitFlag
// 00 00 usProtocolFlag
// 00 06 usLength
// ff    ucUnitFlag
// 03 	 ucFuncCode
// 00 05 Start address
// 00 01 register quantity
// Example 2
// 000000000006 010400010012
// 000000000027 010424000000 000000000000 000000000000 
// 000000000000 000000000000 000000000000 000000
int rev_modbustcp_frame(char *datain, int len)
{
	unsigned int i = 0;
	unsigned short crcval = 0;
	unsigned int ret = 0;
	unsigned int temp = 0;
	
	// data length in frame byte
	temp = ((unsigned int)datain[4]<<8) + ((unsigned int)datain[5]);	
	if((len == (temp + 6))&&(temp <= 1460))
	{
		memcpy((char *)(char *)&modbustcp_frame.header, (char *)(datain), 6);

		for(i=0; i<temp; i++)
		{
			datain[i] = datain[6+i];
		}
		
		crcval = crc16(datain, temp);
		datain[temp] = crcval & 0x00ff;
		datain[temp+1] = (crcval>>8)&0xff;
		ret = temp + 2;
	}
	else
	{
		return 0;
	}
	if(ret >= 1460)
	{
		ret = 1460;
	}
	
	return ret;
}

//recv modbus frame 
int rev_modbus_frame(char *datain, int len)
{
	unsigned int i = 0;
	unsigned short crcval = 0;
	unsigned int ret = 0;
	unsigned int temp = 0;
	unsigned int mod_excrc_len = 0;

	mod_excrc_len = len-2;
	crcval = crc16(datain, mod_excrc_len);
	temp = ((unsigned short)datain[len-2]) + ((unsigned short)datain[len-1]<<8);
	if(crcval == temp)
	{
		modbustcp_frame.header_data[4] = ((mod_excrc_len)>>8)&0x00ff;
		modbustcp_frame.header_data[5] = ((mod_excrc_len))&0x00ff;				
		for(i=0; i<mod_excrc_len; i++)
		{
			datain[6+mod_excrc_len-i-1] = datain[mod_excrc_len-i-1];
		}
		
		memcpy((char *)datain, (char *)&modbustcp_frame.header, 6);
		ret = len - 2 + 6;
	}
	else
	{
		ret = 0;
	}
	
	if(ret >= 1460)
	{
		ret = 1460;
	}
	
	return ret;
}


//
// usr cloud
//
unsigned char cloud_data[50] =  {0};
unsigned char key_locking[6];

static const unsigned char Key[16]=	
{
        //0x1C,0xFE,0xAC,0x04,0x93,0x1A,0x1B,0xCB,0x96,0xFB,0xE8,0x3A,0xB1,0xF9,0x3E,0x88,
        0x66,0x75,0x63,0x6B,0x20,0x67,0x66,0x77,0x66,0x75,0x63,0x6B,0x20,0x67,0x66,0x77    
		//6675636B206766776675636B20676677
};
static const unsigned char Key2[16]=
{ 
  0X66, 0X43, 0X96, 0X46, 0X3A, 0X99,0X39, 0XBD, 0XBF, 0X2B, 0X3D, 0X44, 0XD8, 0XB0, 0X4B, 0XB4     
// 664396463A9939BDBF2B3D44D8B04BB4   
};
unsigned char DecryptKey[16];
unsigned int inter, cfc, cfd;
unsigned int si, x1a2;
unsigned int x1a0[9];

static void DecryptInit(unsigned char *keyx_word)
{
	unsigned char tmp;
	si = 0;
	x1a2 = 0;
	for(tmp=0; tmp<16; tmp++)
	{
		DecryptKey[tmp]=keyx_word[tmp];
	}
}

static void exchange(unsigned int *a, unsigned int *b)
{
	unsigned int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

// encrupt one byte
static void PC1assemble128()
{
	unsigned char i;
	unsigned int ax, bx, cx, dx;
	inter = 0;
	x1a0[0] = 0;
	for(i = 0; i < 8; i++)
	{
		x1a0[i + 1] = x1a0[i] ^ ((DecryptKey[i * 2] * 256) + DecryptKey[i * 2 + 1]);

		dx = x1a2 + i;
		ax = x1a0[i + 1];
		cx = 0x015A;
		bx = 0x4E35;

		exchange(&ax, &si);
		exchange(&ax, &dx);

		if (ax != 0)
		{
			ax = ax * bx;
		}
		exchange(&ax, &cx);

		if (ax != 0)
		{
			ax = ax * si;
			cx = ax + cx;
		}
		exchange(&ax, &si);

		ax = ax * bx;
		dx = cx + dx;
		ax = ax + 1;

		x1a2 = dx;
		x1a0[i + 1] = ax;

		inter = inter ^ (ax ^ dx);
	}
}

static void EncryptBlock(unsigned char *buf, unsigned int nSize)
{
	unsigned int n;
	unsigned char i;

	for( n = nSize; n > 0; n--)
	{
		PC1assemble128();
		cfc = inter >> 8;
		cfd = inter & 255;
		//
		// difference between encrypt and decrypt
		// if encrypt, need for() then *buf
		// if decrypt, switch turn
		//
		for(i = 0; i < 16; i++)
			DecryptKey[i] = DecryptKey[i] ^ (*buf);

		*buf = *buf ^ (cfc ^ cfd);
		buf++;
	}
}

int Usr_Get_Cloud_Reg(int socketfd, char *deviceid, char *pwd)
{
//	unsigned int ret = 0;
	
	memset(cloud_data,0x30,50);
	cloud_data[0]= rand()&0xff;
	cloud_data[1]= rand()&0xff;
	memcpy(cloud_data+2, deviceid, 20);
	memcpy(cloud_data+22, pwd, 8); 
	DecryptInit((unsigned char*)Key);
	EncryptBlock(cloud_data,50); 
	
	send(socketfd, cloud_data, 50, 0);
	
	return 50;
}

// shell_get_ser2net
char * shell_get_ser2net(const char *shellcmd, char *buffer_out, int buffer_out_len, int uartnum)
{
	char buffer[200] = {0};

	sprintf(buffer, shellcmd, uartnum);

	shell_get((const char *)buffer, buffer_out, buffer_out_len);

	return buffer_out;
}

//
int log_socklk(char *sta_name, char *sta_val)
{
#if SER2NET_LOG
	static char log_buffer[1024] = {0};
	int fd_dev = 0;
	char log_path[32] = {0};
	
	sprintf(log_path, "/tmp/ser2net%d.status", portParam.uartnum);
	
	fd_dev=open((const char *)log_path, O_WRONLY|O_CREAT|O_APPEND | O_TRUNC);
	if(fd_dev < 0)
	{
		printf("file open fail \n"); 
		return -1;
	}
	sprintf(log_buffer, "%s %s\n", sta_name, sta_val);

	write(fd_dev, log_buffer, strlen(log_buffer));
	write(fd_dev, "\r\n", strlen("\r\n"));
	
	close(fd_dev);

#endif
	return 0;
}

//
static int log_wfile(char *filename, char *content, int wlen)
{
	static char log_buffer[1024] = {0};
	int fd_dev = 0;

	if(wlen >= sizeof(log_buffer))
	{
		return -2;
	}
	
	fd_dev=open((const char *)filename, O_WRONLY|O_CREAT|O_APPEND | O_TRUNC);
	if(fd_dev < 0)
	{
		printf("file open fail \n"); 
		return -1;
	}
	
	write(fd_dev, content, wlen);
	
	close(fd_dev);

	return 0;
}

int log_resettimeout(int uartnum, int datarecved)
{
	char log_path[64] = {0};
	char log_buf[32] = {0};
	static int datarecved_last = 0xff;

	if(datarecved == datarecved_last)
	{
		return 0;
	}
	datarecved_last = datarecved;
	
	sprintf(log_path, "/tmp/log_reset%d", portParam.uartnum);
	
	if(datarecved)
	{
		sprintf(log_buf, "%d", portParam.uartnum);	
		log_wfile(log_path, log_buf, strlen(log_buf));
	}
	else
	{
		sprintf(log_buf, "0");	
		log_wfile(log_path, log_buf, strlen(log_buf));
	}
	
	return 0;
}



