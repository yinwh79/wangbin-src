#!/usr/bin/expect -f
#/usr/bin/make clean
#/usr/bin/make
#/bin/sh echo "remake ok"
spawn scp libatcmd.a root@192.168.4.7:/lib/libatcmd.a
expect "password: "
send "20170406usr\r"
set timeout 100
send "exit\r"
expect eof



