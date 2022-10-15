#!/bin/sh 

help() {

bn=`basename $0`
cat << EOF

Version: 1.6
Last change: generate super.img when flash images with dynamic partition feature
V1.4 change: add support imx8mn chips

Usage: $bn <option> device_node

options:
  -h                displays this help message
  -s                only get partition size
  -np               not partition.
  -f soc_name       flash android image file with soc_name. Below table lists soc_name values and bootloader offset.
                           ┌────────────────────────────┬───────────────┐
                           │   soc_name                 │     offset    │
                           ├────────────────────────────┼───────────────┤
                           │ imx6dl/imx6q/imx6qp        │      1k       │
                           │ imx6sx/imx7d/imx7ulp       │               │
                           ├────────────────────────────┼───────────────┤
                           │ imx8mm/imx8mq              │      33k      │
                           ├────────────────────────────┼───────────────┤
                           │imx8qm/imx8qxp/imx8mn/imx8mp│      32k      │
                           └────────────────────────────┴───────────────┘
  -a                only flash image to slot_a
  -b                only flash image to slot_b
  -c card_size      optional setting: 7 / 14 / 28
                        If not set, use partition-table.img (default)
                        If set to  7, use partition-table-7GB.img  for  8GB SD card
                        If set to 14, use partition-table-14GB.img for 16GB SD card
                        If set to 28, use partition-table-28GB.img for 32GB SD card
                    Make sure the corresponding file exist for your platform.
  -u uboot_feature  flash uboot or spl&bootloader image files with "uboot_feature" in their names.
  -d dtb_feature    flash dtbo, recovery and vbmeta image files with "dtb_feature" in their names.
  -D directory      specify the directory which contains the images to be flashed.
  -m                flash mcu image
  -o force_offset   force set uboot offset
EOF

}

# parse command line
moreoptions=1
node="na"
soc_name=""
force_offset=""
cal_only=0
card_size=0
bootloader_offset=1
mcu_image_offset=5120
vaild_gpt_size=17
not_partition=0
slot=""
systemimage_file="system_raw.img"
system_extimage_file="system_ext.img"
vendor_file="vendor_raw.img"
product_file="product_raw.img"
partition_file="partition-table.img"
super_file="super_raw.img"
logo_file="logo_raw.img"
vendorboot_file="vendor_boot.img"
g_sizes=0
support_dtbo=0
flash_mcu=0
RED='\033[0;31m'
STD='\033[0;0m'

image_directory=""
dtb_feature=""
uboot_feature=""
support_dual_bootloader=0
support_dualslot=0
support_dynamic_partition=0
support_vendor_boot=0
has_system_ext_partition=0
node_device_major=""
node_device_minor=0
current_device_major=""
current_device_minor=0
minor_difference=0
current_device_base_name=""


while [ "$moreoptions" = 1 -a $# -gt 0 ]; do
    case $1 in
        -h) help; exit ;;
        -s) cal_only=1 ;;
        -f) soc_name=$2; shift;;
        -c) card_size=$2; shift;;
        -np) not_partition=1 ;;
        -a) slot="_a" ;;
        -b) slot="_b" ;;
        -m) flash_mcu=1 ;;
        -o) force_offset=$2; shift;;
        -u) uboot_feature=-$2; shift;;
        -d) dtb_feature=$2; shift;;
        -D) image_directory=$2; shift;;
        *)  moreoptions=0; node=$1 ;;
    esac
    [ "$moreoptions" = 0 ] && [ $# -gt 1 ] && help && exit
    [ "$moreoptions" = 1 ] && shift
done

# check required applications are installed
command -v hdparm >/dev/null 2>&1 || { echo -e >&2 "${RED}Missing hdparm app. Please make sure it is installed. Exiting.${STD}" ; exit 1 ; }
command -v gdisk >/dev/null 2>&1 || { echo -e >&2 "${RED}Missing gdisk app. Please make sure it is installed. Exiting.${STD}" ; exit 1 ; }

if [ ${card_size} -ne 0 ] && [ ${card_size} -ne 7 ] && [ ${card_size} -ne 14 ] && [ ${card_size} -ne 28 ]; then
    help; exit 1;
fi

# imx8qxp RevB0 chips, imx8qm RevB0 chips, imx8mp and imx8mn chips, bootloader offset is 32KB on SD card
if [ "${soc_name}" = "imx8qxp" -o "${soc_name}" = "imx8qm" -o "${soc_name}" = "imx8mn" -o "${soc_name}" = "imx8mp" ]; then
    bootloader_offset=32
fi

# imx8mq chips and imx8mm chips, bootloader offset is 33KB on SD card
if [ "${soc_name}" = "imx8mq" -o "${soc_name}" = "imx8mm" ]; then
    bootloader_offset=33
fi

if [ "${force_offset}" != "" ]; then
    bootloader_offset=${force_offset}
fi

# exit if the block device file specified by ${node} doesn't exist
if [ ! -e ${node} ]; then
    help
    exit 1
fi

# Process of the uboot_feature parameter
if [[ "${uboot_feature}" = *"trusty"* ]] || [[ "${uboot_feature}" = *"secure"* ]]; then
    echo -e >&2 ${RED}Do not flash the image with Trusty OS to SD card${STD}
    help
    exit 1
fi
if [[ "${uboot_feature}" = *"dual"* ]]; then
    support_dual_bootloader=1;
fi


# dual bootloader support will use different gpt. Android Automative only boot from eMMC, judgement here is complete
if [ ${support_dual_bootloader} -eq 1 ]; then
    if [ ${card_size} -gt 0 ]; then
        partition_file="partition-table-${card_size}GB-dual.img";
    else
        partition_file="partition-table-dual.img";
    fi
else
    if [ ${card_size} -gt 0 ]; then
        partition_file="partition-table-${card_size}GB.img";
    else
        partition_file="partition-table.img";
    fi
fi


# for specified directory, make sure there is a slash at the end
if [[ "${image_directory}" = "" ]]; then
    image_directory=`pwd`;
fi
image_directory="${image_directory%/}/";

if [ ! -f "${image_directory}${partition_file}" ]; then
    echo -e >&2 "${RED}File ${partition_file} not found. Please check. Exiting${STD}"
    return 1
fi


# dump partitions
if [ "${cal_only}" -eq "1" ]; then
    gdisk -l ${node} 2>/dev/null | grep -A 20 "Number  "
    exit 0
fi

function get_partition_size
{
    start_sector=`gdisk -l ${node} | grep -w $1 | awk '{print $2}'`
    end_sector=`gdisk -l ${node} | grep -w $1 | awk '{print $3}'`
    # 1 sector = 512 bytes. This will change unit from sector to MBytes.
    let "g_sizes=($end_sector - $start_sector + 1) / 2048"
}

function get_current_device_base_name
{
    if [ -z ${current_device_base_name} ]; then
        #get the minor number of the node
        node_device_major=`ls -l ${node} | awk '{print $5}'`
        node_device_minor=`ls -l ${node} | awk '{print $6}'`
        all_device_info=`ls -l ${node}*`

        # use '\n' as delimiter
        OLDIFS=$IFS
        IFS=$'\n'
        # find the first partition, and retrieve the base name
        for current_device_info in $all_device_info ; do
            current_device_major=`echo ${current_device_info} | awk '{print $5}'`
            current_device_minor=`echo ${current_device_info} | awk '{print $6}'`
            minor_difference=`expr $current_device_minor - $node_device_minor`

            if [ ${node_device_major} = ${current_device_major} ]; then
                if [ 1 -eq $minor_difference ]; then
                    current_device_base_name=`echo ${current_device_info} | awk '{print $10}'`
                    current_device_base_name=${current_device_base_name%1}
                    IFS=$OLDIFS
                    return 0
                fi
            fi
        done
        # restore the delimeter
        IFS=$OLDIFS
        echo -e >&2 "${RED}Failed to find the first partition on ${node}.${STD}"
        exit 1
    fi
}

function format_partition
{
    num=`gdisk -l ${node} | grep -w $1 | awk '{print $1}'`
    if [ ${num} -gt 0 ] 2>/dev/null; then
        get_current_device_base_name
        echo "format_partition: $1:${current_device_base_name}${num} ext4"
        mkfs.ext4 -F ${current_device_base_name}${num} -L$1
    fi
}

function erase_partition
{
    num=`gdisk -l ${node} | grep -w $1 | awk '{print $1}'`
    if [ ${num} -gt 0 ] 2>/dev/null; then
        get_current_device_base_name
        get_partition_size $1
        echo "erase_partition: $1 : ${current_device_base_name}${num} ${g_sizes}M"
        dd if=/dev/zero of=${current_device_base_name}${num} bs=1048576 conv=fsync count=$g_sizes
    fi
}

function flash_partition
{
    for num in `gdisk -l ${node} | grep -E -w "$1|$1_a|$1_b" | awk '{print $1}'`
    do
        if [ $? -eq 0 ]; then
            if [ "$(echo ${1} | grep "bootloader_")" != "" ]; then
                img_name=${uboot_proper_file}
            elif [ ${support_vendor_boot} -eq 1 ] && [ $(echo ${1} | grep "vendor_boot") != "" ] 2>/dev/null; then
                img_name="${vendorboot_file}"
            elif [ "$(echo ${1} | grep "system_ext")" != "" ]; then
                img_name=${system_extimage_file}
            elif [ "$(echo ${1} | grep "system")" != "" ]; then
                img_name=${systemimage_file}
            elif [ "$(echo ${1} | grep "vendor")" != "" ]; then
                img_name=${vendor_file}
            elif [ "$(echo ${1} | grep "product")" != "" ]; then
                img_name=${product_file}
            elif [ ${support_dtbo} -eq 1 ] && [ $(echo ${1} | grep "boot") != "" ] 2>/dev/null; then
                img_name="boot.img"
            elif [ "$(echo ${1} | grep -E "dtbo|vbmeta|recovery")" != "" -a "${dtb_feature}" != "" ]; then
                img_name="${1%_*}-${soc_name}-${dtb_feature}.img"
            elif [ "$(echo ${1} | grep "super")" != "" ]; then
                img_name=${super_file}
            elif [ "$(echo ${1} | grep "logo")" != "" ]; then
                img_name=${logo_file}
            else
                img_name="${1%_*}-${soc_name}.img"
            fi
            # check whether the image files to be flashed exist or not
            if [ ! -f "${image_directory}${img_name}" ]; then
                echo -e >&2 "${RED}File ${img_name} not found. Please check. Exiting${STD}"
                return 1
            fi
            get_current_device_base_name
            echo "flash_partition: ${img_name} ---> ${current_device_base_name}${num}"

            if [ "$(echo ${1} | grep "vendor_boot")" != "" ]; then
                dd if=${image_directory}${img_name} of=${current_device_base_name}${num} bs=10M conv=fsync
            else
                dd if=${image_directory}${img_name} of=${current_device_base_name}${num} bs=10M conv=fsync
            fi
        fi
    done
}

function format_android
{
    echo "formating android images"
    format_partition metadata
    format_partition cache
    erase_partition presistdata
    erase_partition fbmisc
    erase_partition misc
    format_partition userdata
}

function make_partition
{
    echo "make gpt partition for android: ${partition_file}"
    dd if=${image_directory}${partition_file} of=${node} bs=1024 count=${vaild_gpt_size} conv=fsync
}

function flash_android
{
    boot_partition="boot"${slot}
    recovery_partition="recovery"${slot}
    system_partition="system"${slot}
    system_ext_partition="system_ext"${slot}
    vendor_partition="vendor"${slot}
    product_partition="product"${slot}
    vbmeta_partition="vbmeta"${slot}
    dtbo_partition="dtbo"${slot}
    super_partition="super"
    vendor_boot_partition="vendor_boot"${slot}
    logo_partition="logo"
    gdisk -l ${node} 2>/dev/null | grep -q "dtbo" && support_dtbo=1
    gdisk -l ${node} 2>/dev/null | grep -q "super" && support_dynamic_partition=1
    gdisk -l ${node} 2>/dev/null | grep -q "vendor_boot" && support_vendor_boot=1
    gdisk -l ${node} 2>/dev/null | grep -q "system_ext" && has_system_ext_partition=1

    if [ ${support_dual_bootloader} -eq 1 ]; then
        bootloader_file=spl-${soc_name}${uboot_feature}.bin
        uboot_proper_file=bootloader-${soc_name}${uboot_feature}.img
        bootloader_partition="bootloader"${slot}
        flash_partition ${bootloader_partition} || exit 1
    else
        bootloader_file=u-boot-${soc_name}${uboot_feature}.imx
    fi

    if [ "${support_dtbo}" -eq "1" ] ; then
        flash_partition ${dtbo_partition} || exit 1
    fi
    if [ "${support_vendor_boot}" -eq "1" ] ; then
        flash_partition ${vendor_boot_partition} || exit 1
    fi
    flash_partition ${boot_partition}  || exit 1
    flash_partition ${recovery_partition}  || exit 1
    if [ ${support_dynamic_partition} -eq 0 ]; then
        flash_partition ${system_partition} || exit 1
        if [ ${has_system_ext_partition} -eq 1 ]; then
            flash_partition ${system_ext_partition} || exit 1
        fi
        flash_partition ${vendor_partition} || exit 1
        flash_partition ${product_partition} || exit 1
    else
        sleep 1
        flash_partition ${super_partition} || exit 1
    fi
    flash_partition ${vbmeta_partition} || exit 1
    flash_partition ${logo_partition} || exit 1
    echo "erase_partition: uboot : ${node}"
    echo "flash_partition: ${bootloader_file} ---> ${node}"
    first_partition_offset=`gdisk -l ${node} | grep ' 1 ' | awk '{print $2}'`
    # the unit of first_partition_offset is sector size which is 512 Byte.
    count_bootloader=`expr ${first_partition_offset} / 2 - ${bootloader_offset}`
    echo "the bootloader partition size: ${count_bootloader}"
    dd if=/dev/zero of=${node} bs=1k seek=${bootloader_offset} conv=fsync count=${count_bootloader}
    dd if=${image_directory}${bootloader_file} of=${node}boot0 bs=1k  conv=fsync
    if [ "${flash_mcu}" -eq "1" ] ; then
        if [ "${soc_name}" = "imx7ulp" ]; then
            echo -e >&2 "${RED}Caution:${STD}"
            echo -e >&2 "        mcu image for imx7ulp_evk is in SPI flash on board, not in SD card, use uboot commands to flash it."
        else
            mcu_image=${soc_name#*-}"_mcu_demo.img"
            echo "flash_partition: ${mcu_image} ---> ${node}"
            dd if=${image_directory}${mcu_image} of=${node} bs=1k seek=${mcu_image_offset} conv=fsync
        fi
    fi
}

if [ "${not_partition}" -ne "1" ] ; then
    # invoke make_partition to write first 17KB in partition table image to sdcard start
    make_partition || exit 1
    # unmount partitions and then force to re-read the partition table of the specified device
    sleep 3
    for i in `cat /proc/mounts | grep "${node}" | awk '{print $2}'`; do umount $i; done
    hdparm -z ${node}
    # backup the GPT table to last LBA for sd card. execute "gdisk ${node}" with the input characters
    # redirect standard OUTPUT to /dev/null to reduce some ouput
    echo -e 'r\ne\nY\nw\nY\nY' |  gdisk ${node} 1>/dev/null

    # use "boot_b" to check whether dual slot is supported
    gdisk -l ${node} | grep -E -w "boot_b" 2>&1 > /dev/null && support_dualslot=1

    format_android
fi

flash_android || exit 1

if [ "${slot}" = "_b" ]; then
    echo -e >&2 "${RED}Caution:${STD}"
    echo -e >&2 "       This script can't generate metadata to directly boot from b slot, fastboot command may need to be used to boot from b slot."
fi

echo
echo ">>>>>>>>>>>>>> Flashing successfully completed <<<<<<<<<<<<<<"

exit 0

# For MFGTool Notes:
# MFGTool use mksdcard-android.tar store this script
# if you want change it.
# do following:
#   tar xf mksdcard-android.sh.tar
#   vi mksdcard-android.sh 
#   [ edit want you want to change ]
#   rm mksdcard-android.sh.tar; tar cf mksdcard-android.sh.tar mksdcard-android.sh
