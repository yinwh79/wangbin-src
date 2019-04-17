#ifndef _UTILS_H
#define _UTILS_H

#include "macros.h"

extern int eth_login(int fd, char *login, unsigned int len);
extern int eth_heart(int fd, char *heart, unsigned int len);
extern int comm_heart(int fd, char *heart, unsigned int len);
extern int xfrc2217(int commfd, char *pdata, unsigned int len);
extern int rev_modbustcp_frame(char *datain, int len);
extern int rev_modbus_frame(char *datain, int len);
extern int Usr_Get_Cloud_Reg(int socketfd, char *deviceid, char *pwd);
extern char * shell_get_ser2net(const char *shellcmd, char *buffer_out, int buffer_out_len, int uartnum);
extern int log_socklk(char *sta_name, char *sta_val);
extern char *iphex2str(unsigned int ipaddr);
extern int log_resettimeout(int uartnum, int datarecved);

#endif // UTILS_H
