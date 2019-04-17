#!/usr/bin/expect -f
spawn scp -r ./USR_N668 root@192.168.13.8:/root/USR_N668
set timeout 3
expect {
"yes/no" {send "yes\r";exp_continue}
}
expect "root@192.168.13.8's password:"
set timeout 3
send "20170406usr\r"
set timeout 100
send "exit\r"
expect eof



