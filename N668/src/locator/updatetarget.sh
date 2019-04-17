#!/usr/bin/expect -f
#/usr/bin/make clean
#/usr/bin/make
#/bin/sh echo "remake ok"
spawn scp locator root@192.168.4.8:/root/locator
expect "password: "
send "20170406usr\r"
set timeout 100
send "exit\r"
expect eof



