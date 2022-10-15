#!/bin/bash

LEDLOG="/laohua/ledlog.txt"

NEEDSTOP=0
count=0

function need_stop(){
	sync
	NEEDSTOP=`cat ${LEDLOG}`
}
function enable_stop(){
	echo "2" > ${LEDLOG}
	sync
}

function led_run(){
	touch ${LEDLOG}
	echo "0" > ${LEDLOG}
#while :
#	do
			need_stop
			if [ $((NEEDSTOP)) -eq 0 ] ; then
					enable_stop
					count=5300
					while [ $((count)) -gt 0 ] 
						do
							echo 0 > /sys/class/leds/heartbeat/brightness  
								sleep 1
							echo 1 > /sys/class/leds/heartbeat/brightness  
								sleep 1
								count=`busybox expr $count - 1`
						done
			else
				echo 1 > /sys/class/leds/heartbeat/brightness  
				sleep 3
			fi
			exit 0
#	done
}

led_run

