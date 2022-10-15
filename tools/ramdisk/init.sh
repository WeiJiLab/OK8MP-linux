#!/bin/sh 
# openvt -f -c 1 /update.sh
/update.sh
echo none > /sys/class/leds/heartbeat/trigger
while true
do 
##  led
	echo 255 > /sys/class/leds/heartbeat/brightness
	sleep 1
	echo 0 > /sys/class/leds/heartbeat/brightness
	sleep 1
done

