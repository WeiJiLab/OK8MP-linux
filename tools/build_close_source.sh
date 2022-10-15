#! /bin/bash

PACK_HOME=`pwd`
export SPACK_HOME

SDK_GIT="git@192.168.1.180:OK8MP-linux-sdk.git"
UBOOT_GIT="git@192.168.1.180:OK8MP-linux-uboot.git"
KERNEL_GIT="git@192.168.1.180:OK8MP-linux-kernel.git"
ROOTFS_GIT="git@192.168.1.180:OK8MP-linux-fs.git"

SDK_DIR="OK8MP-linux-sdk"
UBOOT_DIR="OK8MP-linux-uboot"
KERNEL_DIR="OK8MP-linux-kernel"
ROOTFS_DIR="OK8MP-linux-fs"

git clone -b master $SDK_GIT
git clone -b master $UBOOT_GIT	$SDK_DIR/$UBOOT_DIR
git clone -b master $KERNEL_GIT $SDK_DIR/$KERNEL_DIR
git clone -b master $ROOTFS_GIT $SDK_DIR/$ROOTFS_DIR

[ ! -d $SDK_DIR ] && echo "Failed to download $SDK_GIT" && exit 0

# delete sdk .git
cd  $SDK_DIR
echo "$SDK_DIR" > commit_id.txt
git log -3 >> commit_id.txt

# delete uboot .git
cd $UBOOT_DIR
echo "$UBOOT_DIR" >> ../commit_id.txt
git log -3 >> ../commit_id.txt
cd -

# delete kernel .git
cd $KERNEL_DIR
echo "$KERNEL_DIR" >> ../commit_id.txt
git log -3 >> ../commit_id.txt
cd -

# delete rootfs .git
cd $ROOTFS_DIR
echo "$ROOTFS_DIR" >> ../commit_id.txt
git log -3 >> ../commit_id.txt
cd -

echo toolchain will be install in /opt/
source environment-setup-aarch64-poky-linux
./build.sh all


rm -rf images/secret/
rm -rf images/boot.img
rm -rf images/uboot
rm -rf tools/imx-boot/iMX8M/flash.bin
rm -rf tools/imx-boot/iMX8M/u-boot-spl-ddr.bin
rm -rf tools/imx-boot/iMX8M/u-boot.itb

cp commit_id.txt images/
cp images ../user_images_release -rf

rm $UBOOT_DIR -rf

echo "clean build kernel file"
cd $KERNEL_DIR
make distclean
make clean
cd -

echo "clean build apps file"
./build.sh clean_apps
./build.sh clean_freertos_demo

pushd $ROOTFS_DIR/
	git clean -f .
	rm -rf rootfs/lib/modules/*
popd
git clean -f OK8MP-FreeRTOS/


rm .git -rf
rm $KERNEL_DIR/.git -rf
rm $ROOTFS_DIR/.git -rf

rm images/kernel/* images/ok8mp-linux-fs* images/ramdisk.img -rf
cd $PACK_HOME

echo 
echo 
echo packing sdk waitting....
tar jcf - $SDK_DIR | split -d -b 2000m - $SDK_DIR.tar.bz2.

echo 
echo Release user sdk is:
echo `ls $SDK_DIR.tar.bz2.*`
md5sum $SDK_DIR.tar.bz2.* > sdk_md5sum.txt


echo "md5sum images"
md5sum user_images_release/imx-boot.bin			>  user_images_release/md5sums.txt		
md5sum user_images_release/commit_id.txt		>> user_images_release/md5sums.txt	
md5sum user_images_release/ok8mp-linux-fs.sdcard.ad	>> user_images_release/md5sums.txt	
md5sum user_images_release/logo-1280x800.bmp		>> user_images_release/md5sums.txt	
md5sum user_images_release/ramdisk.img			>> user_images_release/md5sums.txt		
md5sum user_images_release/ok8mp-linux-fs.sdcard.aa	>> user_images_release/md5sums.txt		
md5sum user_images_release/ok8mp-linux-fs.sdcard.ac	>> user_images_release/md5sums.txt			
md5sum user_images_release/ok8mp-linux-fs.sdcard.ab	>> user_images_release/md5sums.txt			
md5sum user_images_release/kernel/OK8MP-C.dtb		>> user_images_release/md5sums.txt			
md5sum user_images_release/kernel/Image			>> user_images_release/md5sums.txt		
md5sum user_images_release/config.ini			>> user_images_release/md5sums.txt		
md5sum user_images_release/logo-1024x600.bmp		>> user_images_release/md5sums.txt			

echo 
echo 
echo Release images in `realpath ./user_images_release`

