#! /bin/bash

RTOS_HOME_DIR=`pwd`

if [ ! -d OK8MP_RTOS_BIN ]; then
	mkdir OK8MP_RTOS_BIN
fi

for file in `find boards/ -name "armgcc"`
do
	cd $file
	[ $? -ne 0 ] && echo "build ramfs Failed" && exit -1
#./build_release.sh
	./clean.sh
	cp release/* $RTOS_HOME_DIR/OK8MP_RTOS_BIN
	cd $RTOS_HOME_DIR
done
