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


void Param_Init( void )
{
    fd_UsrFactory = 0;
    fd_UserDefault = 0;
    fd_UserRun = 0;
    
	Param_ReadWorkParam();
	Param_ReadConstParam();
}


void Param_ReadWorkParam( void )
{
    int ret;
    BOOL bRead;
    char para_buf[MAX_USR_PARA_SIZE];

    bRead = FALSE;
    
    
    if( fd_UserRun <= 0 )
    {
        fd_UserRun = open( FILE_USER_RUN_CFG, O_RDONLY );
	
    	if( fd_UserRun > 0 )
    	{
    	    ret = read( fd_UserRun, para_buf, MAX_USR_PARA_SIZE );
            close( fd_UserRun );
            fd_UserRun = 0;

            if( ret == sizeof(g_SysParam) )
            {
                memcpy( g_SysParam.buf, para_buf, ret );

                if ( ( g_SysParam.para.first_burning_flag1 == FLASH_TRUE ) && 
                     ( g_SysParam.para.first_burning_flag2 == FLASH_TRUE ) )
                {
                    bRead = TRUE;
                }
            }
            else
            {
            }
    	}
   		else
        {
        }
	}
    else
    {
   		bRead = TRUE;
	}
    
	
	//need to restore usr factory parameters
	if ( !bRead )
	{
		Param_ReloadUsrParam();
	}
}


void Param_SaveWorkParam( void )
{
	int ret, need_write;
    char *ptr;
    char cmd[200];

    //make one copy of changing file
    sprintf( cmd, "cp %s %s", FILE_USER_RUN_CFG, FILE_TEMP_CFG );
    system( cmd );
    

    need_write = sizeof(g_SysParam);
    ptr = (char*)g_SysParam.buf;
    
    if( fd_UserRun <= 0 ) // not open
    {
        fd_UserRun = open( FILE_TEMP_CFG, O_WRONLY | O_CREAT | O_TRUNC );

        do
        {
            ret = write( fd_UserRun, ptr, need_write );
            
            if ( ret == -1 )
            {
                break; //a dead error
            }
            else if ( ret == need_write )
            {
                break; //complete writing
            }
            else if ( ret > 0 )
            {
                ptr += ret;
                need_write -= ret;
            }  
        }
        while( ret );

        close( fd_UserRun );
        fd_UserRun = 0;

        //delete the old file
        sprintf( cmd, "rm -f %s", FILE_USER_RUN_CFG );
        system( cmd );

        //copy a new one
        sprintf( cmd, "cp %s %s", FILE_TEMP_CFG, FILE_USER_RUN_CFG );
        system( cmd );

	}
    else
    {
	}

    //delete the copy of changing file
    sprintf( cmd, "rm -f %s", FILE_TEMP_CFG );
    system( cmd );
}


void Param_SaveAsUserDefault( void )
{
	int ret, need_write;
    char *ptr;
    char cmd[200];

    //make one copy of changing file
    sprintf( cmd, "cp %s %s", FILE_USER_DEFAULT_CFG, FILE_TEMP_CFG );
    system( cmd );

    need_write = sizeof(g_SysParam);
    ptr = (char*)g_SysParam.buf;
    
    if( fd_UserDefault <= 0 ) // not open
    {
        fd_UserDefault = open( FILE_TEMP_CFG, O_WRONLY | O_CREAT | O_TRUNC );

        do
        {   
            ret = write( fd_UserDefault, ptr, need_write );
            
            if ( ret == -1 )
            {
                break; //a dead error
            }
            else if ( ret == need_write )
            {
                break;
            }
            else if ( ret > 0 )
            {
                ptr += ret;
                need_write -= ret;  
            }  
        }
        while( ret );

        close( fd_UserDefault );
        fd_UserDefault = 0;

        //delete the old file
        sprintf( cmd, "rm -f %s", FILE_USER_DEFAULT_CFG );
        system( cmd );

        //copy a new one
        sprintf( cmd, "cp %s %s", FILE_TEMP_CFG, FILE_USER_DEFAULT_CFG );
        system( cmd );

	}
    else
    {
	}

    //delete the copy of changing file
    sprintf( cmd, "rm -f %s", FILE_TEMP_CFG );
    system( cmd );

    //save to user running parameters
    Param_SaveWorkParam();
}


void Param_ReloadUserDefault( void )
{
	int ret;
    char para_buf[MAX_USR_PARA_SIZE];
    
    
    if( fd_UserDefault <= 0 )
    {
        fd_UserDefault = open( FILE_USER_DEFAULT_CFG, O_RDONLY );
	
    	if( fd_UserDefault > 0 )
    	{
    	    ret = read( fd_UserDefault, para_buf, MAX_USR_PARA_SIZE );
            close( fd_UserDefault );
            fd_UserDefault = 0;

            if( ret == sizeof(g_SysParam) )
            {
                memcpy( g_SysParam.buf, para_buf, ret );
                Param_SaveWorkParam();
            }
            else
            {
                return;
            }
    	}
   		else
        {
            return;
        }
	}
    else
    {
   		return;
	}
}


void Param_ReloadUsrParam( void )
{
	g_SysParam.para = usr_factory_t;
	Param_SaveWorkParam();
	Param_SaveAsUserDefault();
}


void Param_ReadConstParam( void )
{
	int ret;
    char para_buf[MAX_USR_PARA_SIZE];
    
    
    if( fd_ConstPara <= 0 )
    {
        fd_ConstPara = open( FILE_CONST_PARA, O_RDONLY );
	
    	if( fd_ConstPara > 0 )
    	{
    	    ret = read( fd_ConstPara, para_buf, MAX_USR_PARA_SIZE );
            close( fd_ConstPara );
            fd_ConstPara = 0;

            if( ret == sizeof(g_SysParamUC) )
            {
                memcpy( g_SysParamUC.buf, para_buf, ret );
            }
            else
            {
            }
    	}
   		else
        {
            g_SysParamUC.para = usr_factory_c;
            Param_SaveConstParam();
        }
	}
    else
    {
	}
}


void Param_SaveConstParam( void )
{
	int ret, need_write;
    char *ptr;
    char cmd[200];

    //make one copy of changing file
    sprintf( cmd, "cp %s %s", FILE_CONST_PARA, FILE_TEMP_CFG );
    system( cmd );
    

    need_write = sizeof(g_SysParamUC);
    ptr = (char*)g_SysParamUC.buf;
    
    if( fd_ConstPara <= 0 ) // not open
    {
        fd_ConstPara = open( FILE_TEMP_CFG, O_WRONLY | O_CREAT | O_TRUNC );

        do
        {
            ret = write( fd_ConstPara, ptr, need_write );
            
            if ( ret == -1 )
            {
                break; //a dead error
            }
            else if ( ret == need_write )
            {
                break; //complete writing
            }
            else if ( ret > 0 )
            {
                ptr += ret;
                need_write -= ret;  
            }  
        }
        while( ret );

        close( fd_ConstPara );
        fd_ConstPara = 0;

        //delete the old file
        sprintf( cmd, "rm -f %s", FILE_CONST_PARA );
        system( cmd );

        //copy a new one
        sprintf( cmd, "cp %s %s", FILE_TEMP_CFG, FILE_CONST_PARA );
        system( cmd );

	}
    else
    {
	}

    //delete the copy of changing file
    sprintf( cmd, "rm -f %s", FILE_TEMP_CFG );
    system( cmd );
}


void Param_ReadDeviceSerialNum( uint32_t *sn )
{
	/*int i;
	uint32_t address = SERIAL_NUM_ADDR / 2;
	Nvmctrl *const nvm_module = NVMCTRL;
	uint16_t *p = (uint16_t*)sn;
	
	//wait NVMCTRL is idle, not programming or erasing
	while( !(NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY) ); 
	
	nvm_module->STATUS.reg |= NVMCTRL_STATUS_MASK; // Clear error flags

	for ( i = 0; i < 8; i ++ )
	{
		p[i] = NVM_MEMORY[address++];
	}*/

    sn[0] = 0x11111111;
    sn[1] = 0x22222222;
    sn[2] = 0x33333333;
    sn[3] = 0x44444444;
}


BOOL Param_CheckEncrypt( void )
{
	uint32_t sn[4];
	uint32_t decrypt_val[2];
	uint32_t encrypt_key[4] = { 0x12345678, 0xAA55AA55, 0x87654321, 0x55AA55AA };
	
	decrypt_val[0] = g_SysParamUC.para.encrypt.code[0];
	decrypt_val[1] = g_SysParamUC.para.encrypt.code[1];
	TeaDecrypt( encrypt_key, decrypt_val );
	
	Param_ReadDeviceSerialNum( sn );
	
	if ( sn[0] == decrypt_val[0] && sn[1] == decrypt_val[1] )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
