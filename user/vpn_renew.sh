#!/bin/sh

#pptp connection 
#pptp test.usr.cn debug user lihuibinvpn password lihuibinvpn file options.pptp
# pptp test.usr.cn debug user lihuibinvpn password lihuibinvpn require-mppe-128 mtu 1500 lcp-echo-interval 300 &
printf "start: pptp-vpn \n"
pptpen=`uci get -q network.pptp.enabled`
if [ $pptpen == "ON" ]
then
        printf "trying to connect pptp server at: "
        printf ~~~~pptpserveraddr~~~~$`uci get -q network.pptp.serveraddr`$`uci get -q network.pptp.username`$`uci get -q network.pptp.password`~~\n
        pptp `uci get -q network.pptp.serveraddr` debug user `uci get -q network.pptp.username` password `uci get -q network.pptp.password` \
        `uci get -q network.pptp.dataencrypt` \
        `uci get -q network.pptp.auth` \
        mtu `uci get -q network.pptp.mtu` \
        lcp-echo-interval `uci get -q network.pptp.lcpecho` &
        printf "~~~~pptpend~~~"
fi

#gre vpn
printf "start: gre-vpn \n"
green=`uci get -q network.gre.enabled`
if [ $green == "ON" ]
then
        printf "gre vpn start\n"
        ip tunnel add `uci get -q network.gre.tunnelname` mode gre \
        remote `uci get -q network.gre.rteip` local `uci get -q network.gre.localip` ttl 255
        ip link set `uci get -q network.gre.tunnelname` up
        ip addr add `uci get -q network.gre.virtualip` peer `uci get -q network.gre.rtevirtualip` dev `uci get -q network.gre.tunnelname`
fi



