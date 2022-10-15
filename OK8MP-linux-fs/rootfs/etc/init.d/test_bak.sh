#!/bin/bash

MEMLOG="/laohua/memlog.txt"
TIMELOG="/laohua/time.txt"
POWERONCOUNTLOG="/laohua/poweron.txt"
LEDLOG="/laohua/ledlog.txt"
#MEMSIZE=`cat proc/meminfo | grep MemTotal | busybox awk '{printf$2}'`


TIME=0
POWERONCOUNT=0
FAILED=0
NEEDSTOP=0
HASRESET=0

function has_reset()
{
	HASRESET=`cat ${TIMELOG}`
	echo "test time?${HASRESET}"
}

function has_error()
{
	HASERROR=`grep "error" ${MEMLOG} -ci`
}

function has_failure(){
	HASFAILURE=`grep "FAILURE" ${MEMLOG} -ci`
}

function has_beenkilled(){
	HASBEENKILLED=`grep "Killed" ${MEMLOG} -ci`
}

function has_unlocked(){
	HASUNLOCKED=`grep "unlocked" ${MEMLOG} -ci`
}

function led_alwayson(){
	echo "1" > ${LEDLOG}	
	sync
	busybox killall -q led.sh
	sleep 5
	echo 1 > /sys/class/leds/led1/brightness
}

function led_normal(){
	busybox killall -q led.sh
	sleep 5
	/etc/init.d/led.sh &
	sleep 8
}

function check_result_withreset(){
		has_reset
		has_error
		has_failure
		has_beenkilled
		has_unlocked
		echo "test time?${HASRESET}"
		echo "error?${HASERROR}"
		echo "failure?${HASFAILURE}"
		echo "killed?${HASBEENKILLED}"
		echo "unlocked?${HASUNLOCKED}"

		if [ $((HASRESET)) -gt 0 ] || [ $((HASERROR)) -gt 0 ] || [ $((HASFAILURE)) -gt 0 ] || [ $((HASBEENKILLED)) -gt 0 ] || [ $((HASUNLOCKED)) -gt 0 ] ; then
				FAILED=1	
				NEEDSTOP=1	
				sync
				sleep 1 
				echo "has reset or error or failure or been killed or unlocked"
				led_alwayson
		fi

}
function check_result(){
		has_error
		has_failure
		has_beenkilled
		has_unlocked
		echo "error?${HASERROR}"
		echo "failure?${HASFAILURE}"
		echo "killed?${HASBEENKILLED}"
		echo "unlocked?${HASUNLOCKED}"

		if [ $((HASRESET)) -gt 0 ] || [ $((HASERROR)) -gt 0 ] || [ $((HASFAILURE)) -gt 0 ] || [ $((HASBEENKILLED)) -gt 0 ] || [ $((HASUNLOCKED)) -gt 0 ] ; then
				FAILED=1	
				NEEDSTOP=1	
				sync
				sleep 1 
				echo "has reset or error or failure or been killed or unlocked"
				led_alwayson
		else
				led_normal
				echo ">k">>${MEMLOG}
				sync
		fi

}

function power_on_count(){

	if [ -s ${POWERONCOUNTLOG} ] ; then
		POWERONCOUNT=`cat ${POWERONCOUNTLOG}`
	else
		POWERONCOUNT=0
	fi

	POWERONCOUNT=`busybox expr $POWERONCOUNT + 1`
	echo ${POWERONCOUNT} > ${POWERONCOUNTLOG} 
	sync
}

function main_loop(){

	touch ${MEMLOG}
	touch ${TIMELOG}
	touch ${LEDLOG}
	touch ${POWERONCOUNTLOG}

	echo none > /sys/class/leds/heartbeat/trigger
	sync
	led_normal
		
	power_on_count

	echo "P:${POWERONCOUNT}">>${MEMLOG}
	sync

	check_result_withreset

	# mplayer  -fs -vo fbdev /forlinx/video/test.mp4 -loop 0 < /dev/null 1>/dev/null 2>&1 &
#		mplayer /forlinx/video/xm.mp4 -fs -x 800 -y 480 -loop 0 < /dev/null >/dev/null 2>&1 &
#		[ $? -eq 0 ] || led_alwayson

	while :
		do
			TIME=`busybox expr $TIME + 1`
			echo ">${TIME}">>${MEMLOG}
			sync
			memtester 300M 1 >/dev/null 2>>${MEMLOG}
			[ $? -eq 0 ] || led_alwayson
			sync

			check_result

			echo ${TIME} > ${TIMELOG} 
			echo "test time?${TIME}"
			sync

		done
}
mkdir -p /laohua
main_loop

