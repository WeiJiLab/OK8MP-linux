#!/bin/bash

# Determine the absolute path to the executable
# EXE will have the PWD removed so we can concatenate with the PWD safely
PWD=`pwd`
EXE=`echo $0 | sed s=$PWD==`
EXEPATH="$PWD"/"$EXE"
clear
cat << EOM

################################################################################

This script will create a bootable SD card from custom or pre-built binaries.

The script must be run with root permissions and from the bin directory of
the SDK

Example:
 $ sudo ./create-sdcard.sh

Formatting can be skipped if the SD card is already formatted and
partitioned properly.

################################################################################

EOM

AMIROOT=`whoami | awk {'print $1'}`
if [ "$AMIROOT" != "root" ] ; then

	echo "	**** Error *** must run script with sudo"
	echo ""
	exit
fi

# find the avaible SD cards
echo " "
echo "Availible Drives to write images to: "
echo " "
ROOTDRIVE=`mount | grep 'on / ' | awk {'print $1'} |  cut -c6-8`
echo "#  major   minor    size   name "
cat /proc/partitions | grep -v $ROOTDRIVE | grep '\<sd.\>' | grep -n ''
echo " "

ENTERCORRECTLY=0
while [ $ENTERCORRECTLY -ne 1 ]
do
	read -p 'Enter Device Number: ' DEVICEDRIVENUMBER
	echo " "
	DEVICEDRIVENAME=`cat /proc/partitions | grep -v 'sda' | grep '\<sd.\>' | grep -n '' | grep "${DEVICEDRIVENUMBER}:" | awk '{print $5}'`
	echo "$DEVICEDRIVENAME"


	DRIVE=/dev/$DEVICEDRIVENAME
	DEVICESIZE=`cat /proc/partitions  | grep -v 'sda' | grep '\<sd.\>' | grep -n '' | grep "${DEVICEDRIVENUMBER}:" | awk '{print $4}'`


	if [ -n "$DEVICEDRIVENAME" ]
	then
		ENTERCORRECTLY=1
	else
		echo "Invalid selection"
	fi

	echo ""
done

echo "$DEVICEDRIVENAME was selected"
#Check the size of disk to make sure its under 16GB
if [ $DEVICESIZE -gt 17000000 ] ; then
cat << EOM

################################################################################

		**********WARNING**********

	Selected Device is greater then 16GB
	Continuing past this point will erase data from device
	Double check that this is the correct SD Card

################################################################################

EOM
	ENTERCORRECTLY=0
	while [ $ENTERCORRECTLY -ne 1 ]
	do
		read -p 'Would you like to continue [y/n] : ' SIZECHECK
		echo ""
		echo " "
		ENTERCORRECTLY=1
		case $SIZECHECK in
		"y")  ;;
		"n")  exit;;
		*)  echo "Please enter y or n";ENTERCORRECTLY=0;;
		esac
		echo ""
	done

fi

echo ""

DRIVE=/dev/$DEVICEDRIVENAME

echo "Checking the device is unmounted"
for i in `ls -1 $DRIVE?`; do
	echo "unmounting device '$i'"
	umount $i 2>/dev/null
done

ENTERCORRECTLY=0
while [ $ENTERCORRECTLY -ne 1 ]
do
	read -p 'Would you like to re-partition the drive anyways [y/n] : ' CASEPARTITION
	echo ""
	echo " "
	ENTERCORRECTLY=1
	case $CASEPARTITION in
	"y")  echo "Now partitioning $DEVICEDRIVENAME ...";PARTITION=0;;
	"n")  echo "Abort partitioning";
			exit ;;
	*)  echo "Please enter y or n";ENTERCORRECTLY=0;;
	esac
	echo ""
done

PARTITION=1

if [ "$PARTITION" -eq "1" ]
then

# Set the PARTS value as well
PARTS=1
cat << EOM

################################################################################

		Now making 1 partitions

################################################################################

EOM
dd if=/dev/zero of=$DRIVE bs=1024 count=1

SIZE=`fdisk -l $DRIVE | grep Disk | awk '{print $5}'`

echo DISK SIZE - $SIZE bytes

CYLINDERS=`echo $SIZE/255/63/512 | bc`

fdisk $DRIVE << EOF
n
p
1
16384

t
b
w
EOF

cat << EOM

################################################################################

		Partitioning Boot

################################################################################
EOM
	mkfs.vfat -F 32 -n "boot" ${DRIVE}1
fi

echo "Buring th sdfuse.bin to sdcard"
if [ -e imx-boot-secret.bin ] ; then
dd if=imx-boot-secret.bin of=${DRIVE} bs=1K seek=32 conv=fsync
elif [ -e imx-boot.bin ] ; then
dd if=imx-boot.bin of=${DRIVE} bs=1K seek=32 conv=fsync
else
dd if=../images/imx-boot.bin of=${DRIVE} bs=1K seek=32 conv=fsync
fi

echo ""
echo "Syncing...."
echo ""
sync

for i in `ls -1 $DRIVE?`; do
	echo "unmounting device '$i'"
	umount $i 2>/dev/null
done
