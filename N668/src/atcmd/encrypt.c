#include <stdint.h>
#include <stdbool.h>
#include "string.h"
#include "encrypt.h"

//
// PC1_128 key
//
const unsigned char Key[16]={
	//0x1C,0xFE,0xAC,0x04,0x93,0x1A,0x1B,0xCB,0x96,0xFB,0xE8,0x3A,0xB1,0xF9,0x3E,0x88,
	//0xBC,0xB9,0x6F,0xBE,0x1C,0xFA,0xB1,0xF0,0x49,0x31,0xA1,0x83,0x93,0xE8,0x8E,0xAC
	//0xBC,0xB9,0x6F,0xBE,0x1C,0xFA,0xB1,0xF0,0x49,0x31,0xA1,0x83,0x93,0xE8,0x8E,0xCC
  0X66, 0X43, 0X96, 0X46, 0X3A, 0X99,0X39, 0XBD, 0XBF, 0X2B, 0X3D, 0X44, 0XD8, 0XB0, 0X4B, 0XB4 
  //664396463A9939BDBF2B3D44D8B04BB4 
};

unsigned char DecryptKey[16] = {0};

unsigned int inter, cfc, cfd;
unsigned int si, x1a2;
unsigned int x1a0[9];
void DecryptInit(void)
{
  unsigned char tmp;
  si = 0;                                        
  x1a2 = 0; 
  for(tmp=0; tmp<sizeof(DecryptKey); tmp++)
  {
		DecryptKey[tmp] = Key[tmp];
  }
}

void exchange(unsigned int *a, unsigned int *b)
{
	unsigned int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

// encrupt one byte
void PC1assemble128()
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

//
void EncryptBlock(unsigned char *buf, unsigned int nSize)
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

static unsigned char str2hex(const char cDigit)
{
    if((cDigit >= '0') && (cDigit <= '9'))
    {
        return(cDigit - '0');
    }
    else
    {
        if((cDigit >= 'a') && (cDigit <= 'f'))
        {
            return((cDigit - 'a') + 10);
        }
        else
        {
            if((cDigit >= 'A') && (cDigit <= 'F'))
            {
                return((cDigit - 'A') + 10);
            }
        }
    }

    //
    // If we get here, we were passed an invalid hex digit so return 0xFF.
    //
    return(0xFF);
}

// get the mac array 
static unsigned long decode_mac_string(const  char *pcEncoded, char *pcDecoded, unsigned long ulLen)
{
    unsigned long ulLoop;
    unsigned long ulCount;
    unsigned char i;

    ulCount = 0;
    ulLoop = 0;
    i = 0;

    //
    // Keep going until we run out of input or fill the output buffer.
    //
    while(pcEncoded[ulLoop] && (ulCount <=(ulLen - 1)))
    {
        switch(pcEncoded[ulLoop])
        {
            //
            // '-'
            //
            case '-':
            {
                ulLoop++;
                break;
            }
            //
            // For all other characters, just invert and copy.
            //
            default:
            {
				if(i==0)
				{
					pcDecoded[ulCount] = str2hex(pcEncoded[ulLoop++])<<4;
					i++;	
				}
				else
				{
					pcDecoded[ulCount] |= str2hex(pcEncoded[ulLoop++]);
					ulCount++;
					i = 0;	
				}
				
                break;
            }
        }
		
		if(ulCount >= 6)
		{
			break;
		}
    }

    return(ulCount);
}







