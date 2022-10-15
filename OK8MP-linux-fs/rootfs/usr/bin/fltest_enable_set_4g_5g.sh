#! /bin/sh

## 4G/5G PWR
if [ ! -d /sys/class/gpio/gpio96 ]; then
echo 96 > /sys/class/gpio/export
fi

## 4G RST
if [ ! -d /sys/class/gpio/gpio97 ]; then
echo 97 > /sys/class/gpio/export
fi

## 5G PWR
if [ ! -d /sys/class/gpio/gpio98 ]; then
echo 98 > /sys/class/gpio/export
fi

## 5G PWRRST
if [ ! -d /sys/class/gpio/gpio99 ]; then
echo 99 > /sys/class/gpio/export
fi

echo out > /sys/class/gpio/gpio96/direction
echo out > /sys/class/gpio/gpio97/direction
echo out > /sys/class/gpio/gpio98/direction
echo out > /sys/class/gpio/gpio99/direction

## enable 4G/5G PWR
echo 1 > /sys/class/gpio/gpio96/value

case $1 in
	4g)
	## 4G rst
	echo 0 > /sys/class/gpio/gpio97/value
	echo 1 > /sys/class/gpio/gpio97/value
	echo 0 > /sys/class/gpio/gpio97/value
	echo "set 4g "
	;;
	5g)
	echo 0 > /sys/class/gpio/gpio98/value
	echo 0 > /sys/class/gpio/gpio99/value
	echo 1 > /sys/class/gpio/gpio99/value
	echo 0 > /sys/class/gpio/gpio99/value
	echo "set 5g"	
	;;
	*)
	;;
esac
echo "wait 10s ...."
sleep 10

