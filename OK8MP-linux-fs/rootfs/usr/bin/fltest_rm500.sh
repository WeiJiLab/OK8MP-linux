#!/bin/sh
fltest_enable_set_4g_5g.sh 5g
ifconfig usb0 up
sleep 1
/usr/bin/quectel-CM
