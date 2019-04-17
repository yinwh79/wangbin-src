#!/bin/sh

#set mac address of eth0 & eth1
printf "start: Network\n"
/root/locator getmac
ifconfig eth0 hw ether `cat /tmp/getmac0`
ifconfig eth1 hw ether `cat /tmp/getmac1`

echo "1" > /proc/sys/net/ipv4/ip_forward
ifconfig eth0 0.0.0.0 up
ifconfig eth1 0.0.0.0 up
brctl addbr br0
brctl addif br0 eth0
brctl addif br0 eth1
#ifconfig br0 192.168.1.7 netmask 255.255.255.0 broadcast 192.168.1.255 up
#route add default gw 192.168.1.1

# set Staticip / Dhcp, and submask  
printf "start: Network set local ip ...\n"
lan_proto=`uci get network.lan.proto -q`
echo $lan_proto $`uci get network.lan.ipaddr -q` $`uci get -q network.lan.gateway` 
if [ $lan_proto == "STATIC" ]
then
	ifconfig br0 `uci get -q network.lan.ipaddr` netmask `uci get -q network.lan.netmask` up
	route add -net 0.0.0.0 netmask 0.0.0.0 gw `uci get -q network.lan.gateway`
	
	# set resolv.conf
	printf "start: resolv.conf copy\n"	
	echo "nameserver "`uci get network.lan.dns -q` > /etc/resolv.conf.static
	echo "nameserver "`uci get network.lan.dns2 -q` >> /etc/resolv.conf.static
	cp -a /etc/resolv.conf.static /tmp/resolv.conf

elif [ $lan_proto == "DHCP" ]
then	
	ip link set br0 up 
	udhcpc -i br0 -S /tmp/syslogd.txt -q -t 5 -n
	ifconfig br0 up
elif [ $lan_proto == "PPPOE" ]
then	
	ip link set br0 up 
	# config pppoe interface
	pppoe -p /mnt/user/pppoe/pppoe.conf
else
	ip link set br0 up 
	udhcpc -i br0 -S /tmp/syslogd.txt -q -t 5 -n
	ifconfig br0 up
fi

# start vpn client
/mnt/user/vpn_renew.sh





