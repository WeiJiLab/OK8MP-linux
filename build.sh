#!/bin/bash
SDK_PATH=`pwd`
DESTDIR=`realpath OK8MP-linux-fs/rootfs`
export SDK_PATH
export DESTDIR


function build_uboot()
{
	if [ -d OK8MP-linux-uboot ] ; then
		if [ ! -e images/secret/imx-boot-secret$1.bin ] ; then
			mkdir -p images/secret
			pushd OK8MP-linux-uboot
			sed -i  's/\/\/#define A1006_CIPHER_WRITE/#define A1006_CIPHER_WRITE/g' 	board/forlinx/ok8mp-c/nxp_a1006/a1006.h
			sed -i  's/\/\/#define EEP_FORCE_UPDATE/#define EEP_FORCE_UPDATE/g' 		board/forlinx/ok8mp-c/eep.c
			sed -i  's/\/\/#define EEP_EMMC_PAD_UPDATE/#define EEP_EMMC_PAD_UPDATE/g' 	board/forlinx/ok8mp-c/eep.c
			sync
			if [ "$1" == "_4G" ] ; then
				make OK8MP-C_defconfig
			else
				make OK8MP-C$1_defconfig
			fi
			make ARCH=arm -j${MAKE_JOBS} CROSS_COMPILE=aarch64-poky-linux-
			if [ "$?" == "1" ] ; then
				echo "uboot build error"
				exit
			fi
			popd
			#copy output binary
			cp OK8MP-linux-uboot/u-boot-nodtb.bin images/uboot/
			cp OK8MP-linux-uboot/arch/arm/dts/OK8MP-C.dtb images/uboot/
			cp OK8MP-linux-uboot/spl/u-boot-spl.bin images/uboot/
			imx-boot
			cp tools/imx-boot/iMX8M/flash.bin images/secret/imx-boot-secret$1.bin
		fi

		pushd OK8MP-linux-uboot
		sed -i  's/^#define A1006_CIPHER_WRITE/\/\/#define A1006_CIPHER_WRITE/g' 	board/forlinx/ok8mp-c/nxp_a1006/a1006.h
		sed -i  's/^#define EEP_FORCE_UPDATE/\/\/#define EEP_FORCE_UPDATE/g' 		board/forlinx/ok8mp-c/eep.c
		sed -i  's/^#define EEP_EMMC_PAD_UPDATE/\/\/#define EEP_EMMC_PAD_UPDATE/g' 	board/forlinx/ok8mp-c/eep.c
		sync

		if [ "$1" == "_4G" ] ; then
			make OK8MP-C_defconfig
		else
			make OK8MP-C$1_defconfig
		fi
		make ARCH=arm -j${MAKE_JOBS} CROSS_COMPILE=aarch64-poky-linux-
		if [ "$?" == "1" ] ; then
			echo "uboot build error"
			exit
		fi
		popd
		#copy output binary
		cp OK8MP-linux-uboot/u-boot-nodtb.bin images/uboot/
		cp OK8MP-linux-uboot/arch/arm/dts/OK8MP-C.dtb images/uboot/
		cp OK8MP-linux-uboot/spl/u-boot-spl.bin images/uboot/
		imx-boot $1
	fi
}


function imx-boot()
{
	pushd tools/imx-boot/
	make SOC=iMX8MP flash_evk
	popd
	cp tools/imx-boot/iMX8M/flash.bin images/imx-boot$1.bin
}
function mkramdisk()
{
	./tools/mkramdisk.sh
}

function build_kernel()
{
	if [ -d OK8MP-linux-kernel ] ; then
		pushd OK8MP-linux-kernel
		make OK8MP-C_defconfig ARCH=arm64
		make ARCH=arm64 -j${MAKE_JOBS} CROSS_COMPILE=aarch64-poky-linux- dtbs Image
		make ARCH=arm64 -j${MAKE_JOBS}  CROSS_COMPILE=aarch64-poky-linux- modules
		make ARCH=arm64 CROSS_COMPILE=aarch64-poky-linux- modules_install INSTALL_MOD_PATH=$DESTDIR
		popd

		pushd OK8MP-linux-kernel/extra/vvcam/v4l2/
		make 
		make modules_install INSTALL_MOD_PATH=$DESTDIR
		popd

		pushd OK8MP-linux-kernel/extra/basler-camera-driver
		make
		make modules_install INSTALL_MOD_PATH=$DESTDIR
		popd

		pushd OK8MP-linux-kernel/extra/cryptodev
        make
        make modules_install INSTALL_MOD_PATH=$DESTDIR
        popd

        pushd OK8MP-linux-kernel/extra/jailhouse
        make
        make modules_install INSTALL_MOD_PATH=$DESTDIR
        popd

		
		
		#copy output binary
		cp OK8MP-linux-kernel/arch/arm64/boot/dts/freescale/OK8MP-C.dtb images/kernel
		cp OK8MP-linux-kernel/arch/arm64/boot/Image images/kernel
	fi

}

function build_m7_freertos_env_init()
{
	ENV_HOME=`pwd`
	Result=`env | grep ARMGCC_DIR`
	if [ 0"$Result" = "0" ]; then
		export ARMGCC_DIR=${ENV_HOME}/OK8MP-FreeRTOS/tools/gcc-arm-none-eabi-10-2020-q4-major
		export PATH=$PATH:$ARMGCC_DIR
	fi
}


function build_m7_freertos()
{
	if [ -d OK8MP-FreeRTOS ]; then
		pushd OK8MP-FreeRTOS
		RTOS_HOME_DIR=`pwd`
		if [ ! -d OK8MP_RTOS_BIN ]; then
			mkdir OK8MP_RTOS_BIN
		else
			rm $RTOS_HOME_DIR/OK8MP_RTOS_BIN/* -f
		fi

		##  HelloWorld
		cd boards/evkmimx8mp/demo_apps/hello_world/armgcc
		./build_release.sh
		cp release/* $RTOS_HOME_DIR/OK8MP_RTOS_BIN
		cd $RTOS_HOME_DIR
	
		## gpio led
		cd boards/evkmimx8mp/driver_examples/gpio/led_output/armgcc
		./build_release.sh
		cp release/* $RTOS_HOME_DIR/OK8MP_RTOS_BIN
		cd $RTOS_HOME_DIR
		
		## rpmsg
		cd boards/evkmimx8mp/multicore_examples/rpmsg_lite_pingpong_rtos/linux_remote/armgcc
		./build_release.sh
		cp release/* $RTOS_HOME_DIR/OK8MP_RTOS_BIN
		cd $RTOS_HOME_DIR

		## swtimer
		cd boards/evkmimx8mp/rtos_examples/freertos_swtimer/armgcc
		./build_release.sh
		cp release/* $RTOS_HOME_DIR/OK8MP_RTOS_BIN
		cd $RTOS_HOME_DIR

		cp $RTOS_HOME_DIR/OK8MP_RTOS_BIN/* $DESTDIR/lib/firmware/

		popd

	fi
	return 0
}

function clean_m7_freertos()
{
	if [ -d OK8MP-FreeRTOS ]; then
		pushd OK8MP-FreeRTOS
		RTOS_HOME_DIR=`pwd`
		
		##  HelloWorld
		cd boards/evkmimx8mp/demo_apps/hello_world/armgcc
		./clean.sh
		cd $RTOS_HOME_DIR
	
		## gpio led
		cd boards/evkmimx8mp/driver_examples/gpio/led_output/armgcc
		./clean.sh
		cd $RTOS_HOME_DIR
		
		## rpmsg
		cd boards/evkmimx8mp/multicore_examples/rpmsg_lite_pingpong_rtos/linux_remote/armgcc
		./clean.sh
		cd $RTOS_HOME_DIR

		## swtimer
		cd boards/evkmimx8mp/rtos_examples/freertos_swtimer/armgcc
		./clean.sh
		cd $RTOS_HOME_DIR

		rm $RTOS_HOME_DIR/OK8MP_RTOS_BIN -rf
		popd

	fi
	return 0
}


case $1 in 
	"uboot")
		if [ "$2" != "" ]; then
			build_uboot _$2
		else
			build_uboot 
		fi
	;;
	"imx-boot")
		imx-boot
	;;
	"kernel")
		build_kernel
		rm -rf images/boot.img
		mkfs.vfat -n "Boot OK8MP" -S 512 -C ${SDK_PATH}/images/boot.img 85184
		mcopy -i ${SDK_PATH}/images/boot.img ${SDK_PATH}/images/kernel/Image ::/Image
		mcopy -i ${SDK_PATH}/images/boot.img ${SDK_PATH}/images/kernel/OK8MP-C.dtb ::/OK8MP-C.dtb
		mcopy -i ${SDK_PATH}/images/boot.img ${SDK_PATH}/images/logo-1024x600.bmp ::/logo-1024x600.bmp
		mcopy -i ${SDK_PATH}/images/boot.img ${SDK_PATH}/images/logo-1280x800.bmp ::/logo-1280x800.bmp
		mcopy -i ${SDK_PATH}/images/boot.img ${DESTDIR}/lib/firmware/hello_world.bin ::/forlinx_m7_tcm_firmware.bin
	;;
	"mkfw")
		fakeroot -- ${SDK_PATH}/tools/fakeroot.fs _$2
	;;
	"ramdisk")
		mkramdisk
	;;

	"apps")

		pushd appsrc/forlinx-qt
		qmake 
		make -j MAKE_JOBS 
		make install
		popd

		pushd appsrc/forlinx-cmd
		make
		make install
		popd

	;;

	"freertos_demo")
		build_m7_freertos_env_init		
		build_m7_freertos
	;;

	"clean_freertos_demo")
		clean_m7_freertos	
	;;
	
	"clean_apps")
		pushd appsrc/forlinx-qt
		make distclean
		popd

		pushd appsrc/forlinx-cmd
		make clean
		popd
	;;

	"all")
		./build.sh freertos_demo
		./build.sh kernel
		./build.sh apps
		./build.sh ramdisk
		./build.sh mkfw $2
	;;

	"full")
		./build.sh clean
		cp -fr ./rootfs_ext/*   OK8MP-linux-fs/rootfs/
		./build.sh all $2
	;;
	"clean")
		clean_m7_freertos	
		
		rm -r OK8MP-linux-fs/rootfs/opt/imx-gpu-sdk
		rm -r OK8MP-linux-fs/rootfs/opt/pylon6
		rm -r OK8MP-linux-fs/rootfs/opt/ltp 
		rm -r OK8MP-linux-fs/rootfs/usr/include 

		pushd appsrc/forlinx-qt
		make distclean
		popd

		pushd appsrc/forlinx-cmd
		make clean
		popd

		pushd OK8MP-linux-kernel
		make distclean
		popd
		
		pushd OK8MP-linux-fs/rootfs/lib/modules
		rm -r 5.4.*
		popd 

	;;

	*)
	;;
esac
