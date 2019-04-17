#!/usr/bin/expect -f
#/usr/bin/make clean
#/usr/bin/make
#/bin/sh echo "remake ok"
spawn scp httpd.cgi root@192.168.4.7:/www/cgi-bin/httpd.cgi
expect "password: "
send "20170406usr\r"
set timeout 100
send "exit\r"
expect eof



