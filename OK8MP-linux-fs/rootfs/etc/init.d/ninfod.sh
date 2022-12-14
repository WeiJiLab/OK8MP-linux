#!/bin/sh

NINFOD=//sbin/ninfod
PID=/var/run/ninfod.pid

if ! [ -x $NINFOD ]; then
	exit 0
fi

case "$1" in
    start)
	echo -n "Starting node information daemon:"
	echo -n " ninfod" ; 
	$NINFOD 
	echo "."
	;;
    stop)
	echo -n "Stopping node information daemon:"
	echo -n " ninfod" ; 
	kill `cat $PID`
	echo "."
	;;
    restart)
	echo -n "Restarting node information daemon:"
	echo -n " ninfod"
	kill `cat $PID`
	$NINFOD
	echo "."
	;;
    *)
	echo "Usage: /etc/init.d/ninfod {start|stop|restart}"
	exit 1
	;;
esac

exit 0

