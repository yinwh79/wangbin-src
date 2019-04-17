/* GPIO Driver Test/Example Program
 *
 * Compile with:
 *  gcc -s -Wall -Wstrict-prototypes gpio.c -o gpiotest
 *
 *
 * Note :
 *   PORT NAME[PIN] = GPIO [id]	
 *   PORTA[ 0]      = gpio[ 0x00]
 *   PORTA[ 1]      = gpio[ 0x01]	  
 *                  :
 *   PORTA[31]      = gpio[ 0x1F]
 *   PORTB[ 0]      = gpio[ 0x20]
 *                  :
 *   PORTB[31]      = gpio[ 0x3F]
 *                  :
 *                  :
 *                  :
 *   PORTI[ 0]      = gpio[ 0xC0]
 *                  :
 *                  :
 *   PORTI[31]      = gpio[ 0xDF]
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fcntl.h"

// 
#define GPIO_EXPORT           "/sys/class/gpio/export"  
#define GPIO_RST_DIR_VAL      	"out"  
#define GPIO_RST_DIR_IN      	"in"  
#define GPIO_RST_VAL_H        "1"
#define GPIO_RST_VAL_L        "0"
#define KEY_PRESSED				0

//
#define GPIO_LEDWORK			196//0xC4 // PG4
#define GPIO_LEDREADY			46//0x2E // PB14
#define GPIO_LEDNC				47//0x2F // PB15
#define GPIO_WDT				36//0x24 // PB4
#define GPIO_RELOAD				197// 0x2F  197-PG5



/* 
// wdt feed
#define GPIO_RST_PIN_VAL_WDT      "36"   
#define GPIO_RST_DIR_WDT          "/sys/class/gpio/gpio36/direction"
#define GPIO_RST_VAL_WDT          "/sys/class/gpio/gpio36/value" */


// variables
FILE *fp;
char io[]={'A','B','C','D','E','F','G','H','I'};

int gpio_init(int ionum, char *dir)
{
	int fd;
	char path[128] = {0};

	//open port : /sys/class/gpio# echo 48 > export
	fd = open(GPIO_EXPORT, O_WRONLY);
	printf("gpio init %d, dir:%s.\n", ionum, dir);
	if(fd == -1)
	{
	   printf("ERR: LEDWORK pin open.\n");
	   return EXIT_FAILURE;
	}
	sprintf(path, "%d", ionum);
	write(fd, path ,sizeof(path)); 	
	close(fd);
	
	// set direction
	sprintf(path, "/sys/class/gpio/gpio%d/direction", ionum);
	fd = open(path, O_WRONLY);
	printf("pin direction :%s open.\n", path);
	if(fd == -1)
	{
		return EXIT_FAILURE;
	}
	write(fd, dir, sizeof(dir));
	close(fd);
	
}

int gpio_write(int ionum, char * val)
{
	int fd;
	char path[128] = {0};

	// gpio write
	sprintf(path, "/sys/class/gpio/gpio%d/value", ionum);	
	fd = open(path, O_RDWR);
	if(fd == -1)
	{
		printf("ERR: gpio %d open error.\n", ionum);
		return EXIT_FAILURE;
	}
	
	write(fd, val, strlen(val));
	close(fd);

}

int gpio_read(int ionum)
{
	int fd;
	char buffer[64];
	int value;
	char str[256];

	sprintf(str,"/sys/class/gpio/gpio%d/value",ionum);
	
	if ((fp = fopen(str, "rb")) == NULL) {
		printf("Cannot open value file.\n");
		return -1;
	} else {
		fread(buffer, sizeof(char), sizeof(buffer) - 1, fp);
		value = atoi(buffer);

		//printf("reload pin: %d\n", value);
	}
	fclose(fp);
	
	return value;
}

int main(void)
{
	int fd;
	int reld_cnt = 0;
	int value = 0;
	int i = 0;
	
	// init
	gpio_init(GPIO_LEDWORK, "out");
	gpio_init(GPIO_LEDREADY, "out");
	gpio_init(GPIO_LEDNC, "out");
	gpio_init(GPIO_WDT, "out");
	gpio_init(GPIO_RELOAD, "in");
	// set init state
	gpio_write(GPIO_LEDREADY, "1");
	gpio_write(GPIO_LEDNC, "0");

	while (1) {
			
		value = gpio_read(GPIO_RELOAD);
		// check if reload pin ready
		if(value == KEY_PRESSED)
		{
			reld_cnt++;
			printf("reload keep pressed: %d, %d\n", value, reld_cnt);
			
			if(reld_cnt >= 5)
			{
				reld_cnt = 0;
				
				for(i=0; i<5; i++)
				{
					gpio_write(GPIO_LEDWORK, "1");
					gpio_write(GPIO_LEDREADY, "1");
					usleep(200000);
					gpio_write(GPIO_LEDWORK, "0");
					gpio_write(GPIO_LEDREADY, "0");
					usleep(200000);
					
				}
				system("/etc/reset_param");
				sleep(2);
			}
		}
		else{
			reld_cnt = 0;
		}
			
		
	  	// led work && wdt
		gpio_write(GPIO_LEDWORK, "1");
		gpio_write(GPIO_WDT, "1");
		usleep(500000);

		gpio_write(GPIO_LEDWORK, "0");
		gpio_write(GPIO_WDT, "0");
		usleep(500000);
		
	}

	return 0;
}

