#!/bin/sh
fltest_enable_set_4g_5g.sh 4g
ifconfig wwan0 up
echo "ATE0" > /dev/ttyUSB2 
sleep 1
#echo "AT^NDISDUP=1,1,\"cmnet\""> /dev/ttyUSB2 
#echo "AT^NDISDUP=1,1,\"3gnet\""> /dev/ttyUSB2 
echo "AT^NDISDUP=1,1,\"ctnet\"\r\n"> /dev/ttyUSB2
sleep 2
udhcpc -i wwan0
