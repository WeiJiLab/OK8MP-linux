#!/bin/sh 

SECTION=config 
IMGDIR="/mnt"
FILENAME=$IMGDIR/config.ini
mount -t sysfs none /sys
mount -t proc none /proc

while true
do
if [ ! -b /dev/mmcblk1p1 ] ; then
	echo "mmcblk1p1 not found"
	sleep 1
else 
	break
fi
done

mount /dev/mmcblk1p1 $IMGDIR 

if [ ! -e $FILENAME ]
then
	echo "$FILENAME not found."
	exit
fi

sed -e 's//\n/g' $FILENAME > ./config.ini
FILENAME=./config.ini

KEY=Board
Board=`awk -F '=' '/\['$SECTION'\]/{a=1}a==1&&$1~/'$KEY'/{print $2;exit}' $FILENAME`
KEY=OS
OS=`awk -F '=' '/\['$SECTION'\]/{a=1}a==1&&$1~/'$KEY'/{print $2;exit}' $FILENAME`

echo "=================================================================="
cat << EOM
+---------------+------------------------------------------------+
|Board		|$Board						
|OS		|$OS						
+---------------+------------------------------------------------+
EOM


linux_flash()
{
	echo "=================================================================="
	echo "[linux flash]"
	echo "=================================================================="
	echo "flashing imx-boot wait..."
	dd if=$IMGDIR/imx-boot.bin of=/dev/mmcblk2boot0 conv=fsync
	echo "flash imx-boot to emmc done..."

	systems=`ls /mnt/ok8mp-linux-fs.sdcard.a*`
	offsets=0

	echo "flashing kernel and rootfs wait..."
	for img in $systems
	do
		echo "$img $offsets"
		dd if=$img of=/dev/mmcblk2 bs=512M seek=$offsets
		if [ "$?" == "1" ] ; then
			echo "sd/emmc flash error"
			exit
		fi
		echo "flash $img, done"
		offsets=$(($offsets+4))
	done
#	dd if=$IMGDIR/rootfs.sdcard of=/dev/mmcblk2 conv=fsync 
	echo "flash kernel and rootfs to emmc done"



	if [ -e $IMGDIR/flash_qspi.bin ];then
		if [ -b /dev/mtdblock0 ];then
			dd if=$IMGDIR/flash_qspi.bin of=/dev/mtdblock0 conv=fsync
			echo "flash uboot to qspi flash ok..."
		fi
	fi
	sync

	echo ">>>>>>>>>>>>>> Flashing successfully completed <<<<<<<<<<<<<<"
}

android_flash()
{
	echo "=================================================================="
	echo "[android flash]"
	echo "=================================================================="
	/imx-sdcard-partition.sh -f imx8mp -a -D $IMGDIR /dev/mmcblk2
	sync
}

starttime=`date +'%Y-%m-%d %H:%M:%S'`

echo 0 > /sys/block/mmcblk2boot0/force_ro
if [ "$OS" == "Linux" ];then
	linux_flash
elif [ "$OS" == "Android" ];then
	android_flash
else
	echo "$OS not support!!!"
	exit
fi
echo 1 > /sys/block/mmcblk2boot0/force_ro
mmc bootpart enable 1 1 /dev/mmcblk2

umount /mnt

if [ "$OS" == "Linux" ];then
fdisk /dev/mmcblk2 << EOF
d
2

n
p
2
1460

w
EOF
fi

sync
sync
sync
endtime=`date +'%Y-%m-%d %H:%M:%S'`
start_seconds=$(date --date="$starttime" +%s);
end_seconds=$(date --date="$endtime" +%s);
echo "[Done] "$((end_seconds-start_seconds))"s"

