#!/bin/bash
 
dd if=/dev/zero of=$SDK_PATH/images/ramdisk.ext4 bs=1M count=0 seek=32
mkfs.ext4 -F -i 4096 $SDK_PATH/images/ramdisk.ext4 -d $SDK_PATH/tools/ramdisk
gzip --best -c $SDK_PATH/images/ramdisk.ext4 > $SDK_PATH/images/ramdisk.ext4.gz

mkimage -n "ramdisk" -A arm -O linux -T ramdisk -C gzip -d $SDK_PATH/images/ramdisk.ext4.gz $SDK_PATH/images/ramdisk.img
rm $SDK_PATH/images/ramdisk.ext4 $SDK_PATH/images/ramdisk.ext4.gz
#pushd $SDK_PATH/tools/ramdisk/
#find . | cpio -o -Hnewc  | gzip -9 > $SDK_PATH/tools/ramdisk.cpio.gz
#popd

#$SDK_PATH/tools/bin/mkimage -n "ramdisk" -A arm -O linux -T ramdisk -C none -d $SDK_PATH/tools/ramdisk.cpio.gz $SDK_PA#TH/images/ramdisk.img
 
#rm $SDK_PATH/tools/ramdisk.cpio.gz
  
