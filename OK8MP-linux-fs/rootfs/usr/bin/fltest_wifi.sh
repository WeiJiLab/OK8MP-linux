#!/bin/sh

function usage()
{
    echo "Usage: -i <wifi> -s <ssid> -p <password>"
    echo "eg: ./wifi.sh -i mlan0 -s bjforlinx -p 12345678 "
    echo "eg: ./wifi.sh -i mlan0 -s bjforlinx -p NONE "
    echo " -i : mlan0 or mlan1"
    echo " -s : wifi ssid"
    echo " -p : wifi password or NONE"
}

function parse_args()
{
    while true; do
        case "$1" in
            -i ) wifi=$2;echo wifi $wifi;shift 2 ;;
            -s ) ssid=$2;echo ssid $ssid;shift 2 ;;
            -p ) pasw=$2;echo pasw $pasw;shift 2 ;;
            -h ) usage; exit 1 ;;
            * ) break ;;
        esac
    done
}

if [ $# != 6 ]
then
    usage;
    exit 1;
fi

parse_args $@

if [ -e /etc/wpa_supplicant.conf ]
then
    rm /etc/wpa_supplicant.conf
fi
    echo \#PSK/TKIP >> /etc/wpa_supplicant.conf
	echo ctrl_interface=/var/run/wpa_supplicant >>/etc/wpa_supplicant.conf
	echo ctrl_interface_group=0 >>/etc/wpa_supplicant.conf
	echo update_config=1 >>/etc/wpa_supplicant.conf
	echo network={ >>/etc/wpa_supplicant.conf
    echo ssid=\"$ssid\" >>/etc/wpa_supplicant.conf
	echo scan_ssid=1 >>/etc/wpa_supplicant.conf
    if [ $pasw == NONE ]
	then
		echo key_mgmt=NONE >>/etc/wpa_supplicant.conf
	else
		echo psk=\"$pasw\" >>/etc/wpa_supplicant.conf
		echo key_mgmt=WPA-EAP WPA-PSK IEEE8021X NONE >>/etc/wpa_supplicant.conf
		echo group=CCMP TKIP WEP104 WEP40 >>/etc/wpa_supplicant.conf
	fi
    echo } >>/etc/wpa_supplicant.conf

ifconfig -a|grep mlan0 |grep -v grep  > /dev/null
if [ $? -eq 0 ]
then
	ifconfig mlan0 down > /dev/null
fi

ifconfig -a|grep mlan1 |grep -v grep  > /dev/null
if [ $? -eq 0 ]
then
	ifconfig mlan1 down > /dev/null
fi

ifconfig -a|grep eth0 |grep -v grep  > /dev/null
if [ $? -eq 0 ]
then
	ifconfig eth0 down > /dev/null
fi

ifconfig -a|grep eth1 |grep -v grep  > /dev/null
if [ $? -eq 0 ]
then
	ifconfig eth1 down > /dev/null
fi

ps -fe|grep wpa_supplicant |grep -v grep > /dev/null
if [ $? -eq 0 ]
then
	kill -9 $(pidof wpa_supplicant)
fi

sleep 1
ifconfig $wifi up > /dev/null 
sleep 1

(wpa_supplicant -Dnl80211,wext -i$wifi -c/etc/wpa_supplicant.conf  >/dev/null) &
echo "waiting..."
sleep 3
wpa_cli -i$wifi status |grep COMPLETED |grep -v grep >/dev/null
if [ $? -eq 0 ]
then
	udhcpc -i $wifi
	echo "Finshed!" 
else
	echo "try to connect again..."
	sleep 3
	wpa_cli -i$wifi status |grep COMPLETED |grep -v grep >/dev/null
		if [ $? -eq 0 ]
		then
        		udhcpc -i $wifi
				echo "nameserver 114.114.114.114" > /etc/resolv.conf
       			echo "Finshed!"
		else
        		echo "************************************************"
       	 		echo "connect faild,please check the passward and ssid"
  		      	kill -9 $(pidof wpa_supplicant)
			exit 1
		fi
fi
