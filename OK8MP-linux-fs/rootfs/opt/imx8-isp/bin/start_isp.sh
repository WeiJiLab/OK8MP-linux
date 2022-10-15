#!/bin/sh
#
# Start the isp_media_server in the configuration for Basler daA3840-30mc
#
# (c) NXP 2020
#

RUNTIME_DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
DEVICE_TREE_BASLER=$(grep basler-camera-vvcam /sys/firmware/devicetree/base/soc@0/*/i2c@*/*/compatible -l 2> /dev/null)

SENSOR_MODE=${SENSOR_MODE:-basler_1080p60}


# check if the basler device has been enabled in the device tree
if [ -f "$DEVICE_TREE_BASLER" ]; then

	echo "Starting isp_media_server for Basler daA3840-30mc in mode $SENSOR_MODE"

	cd $RUNTIME_DIR

	exec ./run.sh -c $SENSOR_MODE -lm

else
	# no device tree found exit with code no device or address
	echo "No device tree found for Basler, check dtb file!" >&2
	exit 6
fi
