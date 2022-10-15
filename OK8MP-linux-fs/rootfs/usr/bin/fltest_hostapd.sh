#!/bin/sh

UAP=`ifconfig -a | grep uap0 | grep -v grep | wc -l`
if [ $UAP = 0 ]; then
	modprobe moal mod_para=nxp/wifi_mod_para.conf
fi

killall wpa_supplicant
killall hostapd
killall udhcpd

echo 1 > /proc/sys/net/ipv4/ip_forward
iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE

hostapd /etc/hostapd-2.4g.conf &
#hostapd /etc/hostapd-5g.conf &
sleep 1
ifconfig mlan0 up
ifconfig uap0 up
ifconfig uap0 192.168.2.1

if [ ! -f /etc/udhcpd.leases ]; then
	touch /etc/udhcpd.leases
fi

if [ ! -d /var/lib/misc/ ]; then
	mkdir /var/lib/misc/ -p
fi

if [ ! -f /var/lib/misc/udhcpd.leases ]; then
	touch /var/lib/misc/udhcpd.leases
fi


udhcpd -S /etc/udhcpd.conf -f &

