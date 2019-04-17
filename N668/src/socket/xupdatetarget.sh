#!/usr/bin/expect -f
#/usr/bin/make clean
#/usr/bin/make
#/bin/sh echo "remake ok"
spawn scp USR_N668 root@192.168.4.7:/mnt/user/N668_firm/USR_N668
expect "password: "
send "20170406usr\r"
set timeout 100
send "exit\r"
expect eof



