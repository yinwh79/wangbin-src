#!/bin/sh

printf "start: ntp client \n"

killall ntpd

echo "1970-00-00 00:00:00" > /tmp/ntpd_last.log
if [ `uci get system.@system[0].ntpenabled -q` -eq "0" ]
then 
	echo ntpd disabled !
	exit
fi
ntpd -p `uci get system.@system[0].ntpserver1 -q` -p `uci get system.@system[0].ntpserver2 -q` -p `uci get system.@system[0].ntpserver3 -q`
echo `date "+%Y-%m-%d %H:%M:%S"` > /tmp/ntpd_last.log

echo ntpd success !

