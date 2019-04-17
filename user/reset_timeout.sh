#!/bin/sh
[ `cat /proc/uptime | awk '{print $1}' | awk -F '.' '{print $1}'` -gt "600" ] && [ `cat /tmp/log_reset1` == "0" ] && [ `cat /tmp/log_reset2` == "0" ] && [ `cat /tmp/log_reset3` == "0" ] && [ `cat /tmp/log_reset4` == "0" ] && [ `cat /tmp/log_reset5` == "0" ] && [ `cat /tmp/log_reset6` == "0" ] && [ `cat /tmp/log_reset7` == "0" ] && [ `cat /tmp/log_reset8` == "0" ] && reboot  


