#!/bin/sh

                                            
if [ `uci get ddns.myddns.enabled -q` == "ON" ]
then 
printf "start: ddns \n"   
	echo szHost=`uci get ddns.myddns.service_name -q` > /etc/phddns.conf  
	echo szUserID=`uci get ddns.myddns.username -q` >> /etc/phddns.conf   
	echo szUserPWD=`uci get ddns.myddns.password -q` >> /etc/phddns.conf  
	echo nicName=`uci get ddns.myddns.ip_interface -q` >> /etc/phddns.conf
	echo szLog=/tmp/phddns.log >> /etc/phddns.conf                  
	echo enable=`uci get ddns.myddns.enabled -q` >> /etc/phddns.conf
	/root/phddns -c /etc/phddns.conf -d
else
	printf "ddns not enabled! \n"
fi
 
