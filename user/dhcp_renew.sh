#!/bin/sh
[ `uci get network.lan.proto -q` == "DHCP" ] && udhcpc -i br0 -S /tmp/syslogd.txt -q -t 5 -n

