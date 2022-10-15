set -e

rawsize=8192
fatsize=85184
ext4size=6815744

totalsize=`expr $rawsize + $fatsize + $ext4size + 1`

if [ -e $SDK_PATH/images/rootfs.ext4 ];then
        rm $SDK_PATH/images/rootfs.ext4
fi

if [ -e $SDK_PATH/images/ok8mp-linux-fs.sdcard ];then
        rm $SDK_PATH/images/ok8mp-linux-fs.sdcard*
fi

dd if=/dev/zero of=$SDK_PATH/images/rootfs.ext4 bs=1K count=0 seek=$ext4size
echo $totalbytes
chown -h -R 0:0 $DESTDIR
find $DESTDIR -name .gitignore -exec rm {} \;
#$SDK_PATH/tools/bin/mkfs.ext4 -F -i 4096 $SDK_PATH/images/rootfs.ext4 -d $DESTDIR
#$SDK_PATH/tools/bin/fsck.ext4 -pvfD $SDK_PATH/images/rootfs.ext4
mkfs.ext4 -F -i 4096 $SDK_PATH/images/rootfs.ext4 -d $DESTDIR
fsck.ext4 -pvfD $SDK_PATH/images/rootfs.ext4

find $DESTDIR -type d -empty -exec touch {}/.gitignore \;

fatstart=$rawsize
fatend=`expr $rawsize + $fatsize`
ext4start=`expr $fatend`
ext4end=`expr $fatend + $ext4size`
echo $ext4end

dd if=/dev/zero of=$SDK_PATH/images/ok8mp-linux-fs.sdcard bs=1K count=0 seek=$totalsize
parted -s $SDK_PATH/images/ok8mp-linux-fs.sdcard mklabel msdos
parted -s $SDK_PATH/images/ok8mp-linux-fs.sdcard unit KiB mkpart primary fat32 $fatstart $fatend
parted -s $SDK_PATH/images/ok8mp-linux-fs.sdcard unit KiB mkpart primary $ext4start $ext4end
parted $SDK_PATH/images/ok8mp-linux-fs.sdcard unit B print

#dd if=$SDK_PATH/images/flash_sd_emmc.bin of=$SDK_PATH/images/ok8mp-linux-fs.sdcard conv=notrunc seek=33 bs=1K
echo $fatstartbytes
echo $ext4startbytes
dd if=$SDK_PATH/images/boot.img of=$SDK_PATH/images/ok8mp-linux-fs.sdcard conv=notrunc,fsync seek=1K bs=$fatstart
dd if=$SDK_PATH/images/rootfs.ext4 of=$SDK_PATH/images/ok8mp-linux-fs.sdcard conv=notrunc,fsync seek=1K bs=$ext4start
split -b 2G $SDK_PATH/images/ok8mp-linux-fs.sdcard $SDK_PATH/images/ok8mp-linux-fs.sdcard.
rm -rf $SDK_PATH/images/rootfs.ext4
#rm -rf $SDK_PATH/images/ok8mp-linux-fs.sdcard

