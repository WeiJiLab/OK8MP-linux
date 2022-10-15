#!/bin/bash
#
# Start the isp_media_server in the configuration from user
# (c) NXP 2020
#

RUN_SCRIPT=`realpath -s $0`
RUN_SCRIPT_PATH=`dirname $RUN_SCRIPT`
echo "RUN_SCRIPT=$RUN_SCRIPT"
echo "RUN_SCRIPT_PATH=$RUN_SCRIPT_PATH"

LOAD_MODULES="0" # do not load modules, they are automatically loaded if present in /lib/modules
LOCAL_RUN="0" # search modules in /lib/modules and libraries in /usr/lib

# an array with the modules to load, insertion order
declare -a MODULES=("imx8-media-dev" "vvcam-video" "vvcam-dwe" "vvcam-isp")

USAGE="Usage:\n"
USAGE+="run.sh -c <isp_config> &\n"
USAGE+="Supported configurations:\n"
USAGE+="\tbasler_1080p60    - single basler camera on MIPI-CSI1, 1920x1080, 60 fps\n"
USAGE+="\tbasler_4k30       - single basler camera on MIPI-CSI1, 3840x2160, 30 fps\n"
USAGE+="\tbasler_1080p60hdr - single basler camera on MIPI-CSI1, 1920x1080, 60 fps, HDR configuration\n"
USAGE+="\tbasler_4k30hdr    - single basler camera on MIPI-CSI1, 3840x2160, 30 fps, HDR configuration\n"


# parse command line arguments
while [ "$1" != "" ]; do
    case $1 in
        -c | --config )
            shift
            ISP_CONFIG=$1
        ;;
        -l | --local )
            LOCAL_RUN="1"
            # search modules and libraries near this script
            # this should work with the flat structure from VSI/Basler
            # but also with the output from make_isp_build_*.sh
        ;;
        -lm | --load-modules )
            LOAD_MODULES="1"
        ;;
        * )
            echo -e "$USAGE" >&2
            exit 1
    esac
    shift
done

# write the sensonr config file
write_sensor_cfg_file () {
    local SENSOR_FILE="$1"
    local CAM_NAME="$2"
    local MODE="$3"

    case $CAM_NAME in
        basler-vvcam )
        cat > $SENSOR_FILE <<-EOF_SENSOR_BASLER
name="basler-vvcam"
drv ="DAA3840_30MC_4K.drv"
mode = $MODE

[mode.0]
xml = "DAA3840_30MC_4K.xml"
dwe = "dewarp_config/daA3840_30mc_4K.json"

[mode.1]
xml = "DAA3840_30MC_1080P.xml"
dwe = "dewarp_config/daA3840_30mc_1080P.json"

[mode.2]
xml = "DAA3840_30MC_4K.xml"
dwe = "dewarp_config/daA3840_30mc_4K.json"

[mode.3]
xml = "DAA3840_30MC_1080P.xml"
dwe = "dewarp_config/daA3840_30mc_1080P.json"
EOF_SENSOR_BASLER
        ;;

        ov2775 )
        cat > $SENSOR_FILE <<-EOF_SENSOR_OV
name="ov2775"
drv ="OV2775.drv"
mode = $MODE
[mode.0]
xml = "OV2775.xml"
dwe = "dewarp_config/sensor_dwe_1080P_config.json"
[mode.1]
xml = "OV2775.xml"
dwe = "dewarp_config/sensor_dwe_1080P_config.json"
[mode.2]
xml = "OV2775.xml"
dwe = "dewarp_config/sensor_dwe_1080P_config.json"
[mode.3]
xml = "OV2775_8M_02_720p.xml"
dwe = "dewarp_config/sensor_dwe_720P_config.json
EOF_SENSOR_OV
        ;;
    
        * )
        echo "Unknown camera $CAM_NAME"
        exit 1
        ;;
esac
 
}

# write the sensonr config file
set_libs_path () {
    local ONE_LIB="$1"
    LIB_PATH=`find $RUN_SCRIPT_PATH -name $ONE_LIB | head -1`
    if [ ! -f "$LIB_PATH" ]; then
        LIB_PATH=`find $RUN_SCRIPT_PATH/../../../usr -name $ONE_LIB | head -1`
        if [ ! -f "$LIB_PATH" ]; then
            echo "$ONE_LIB not found in $RUN_SCRIPT_PATH"
            echo "$ONE_LIB not found in $RUN_SCRIPT_PATH/../../../usr"
            exit 1
        fi
    fi
    LIB_PATH=`realpath $LIB_PATH`
    export LD_LIBRARY_PATH=`dirname $LIB_PATH`:/usr/lib:$LD_LIBRARY_PATH
    echo "LD_LIBRARY_PATH set to $LD_LIBRARY_PATH"
}

load_module () {
    local MODULE="$1.ko"
    local MODULE_PARAMS="$2"
    
    # return directly if already loaded.
    MODULENAME=`echo $1 | sed 's/-/_/g'`
    echo $MODULENAME
    if lsmod | grep "$MODULENAME" ; then
        echo "$1 already loaded."
        return 0
    fi
    
    if [ "$LOCAL_RUN" == "1" ]; then
        MODULE_SEARCH=$RUN_SCRIPT_PATH
        MODULE_PATH=`find $MODULE_SEARCH -name $MODULE | head -1`
        if [ "$MODULE_PATH" == "" ]; then
            MODULE_SEARCH=$RUN_SCRIPT_PATH/../../../modules
            MODULE_PATH=`find $MODULE_SEARCH -name $MODULE | head -1`
            if [ "$MODULE_PATH" == "" ]; then
                echo "Module $MODULE not found in $RUN_SCRIPT_PATH"
                echo "Module $MODULE not found in $MODULE_SEARCH"
                exit 1
            fi
        fi
        MODULE_PATH=`realpath $MODULE_PATH`
    else
        MODULE_SEARCH=/lib/modules/`uname -r`
        MODULE_PATH=`find $MODULE_SEARCH -name $MODULE | head -1`
        if [ "$MODULE_PATH" == "" ]; then
            echo "Module $MODULE not found in $MODULE_SEARCH"
            exit 1
        fi
        MODULE_PATH=`realpath $MODULE_PATH`
    fi
    insmod $MODULE_PATH $MODULE_PARAMS  || exit 1
    echo "Loaded $MODULE_PATH $MODULE_PARAMS"
}

load_modules () {
    # remove any previous instances of the modules
    n=${#MODULES_TO_REMOVE[*]}
    for (( i = n-1; i >= 0; i-- ))
    do
        echo "Removing ${MODULES_TO_REMOVE[i]}..."
        rmmod ${MODULES_TO_REMOVE[i]} &>/dev/null
        #LSMOD_STATUS=`lsmod | grep "${MODULES[i]}"`
        #echo "LSMOD_STATUS=$LSMOD_STATUS"
        if lsmod | grep "${MODULES_TO_REMOVE[i]}" ; then
            echo "Removing ${MODULES_TO_REMOVE[i]} failed!"
            exit 1
        fi
    done
    
    # and now clean load the modules
    for i in "${MODULES[@]}"
    do
        echo "Loading module $i ..."
        load_module $i
    done
    
}

echo "Trying configuration \"$ISP_CONFIG\"..."
case "$ISP_CONFIG" in
    basler_4k30 )
        MODULES=("basler-camera-driver-vvcam" "${MODULES[@]}")
        MODULES_TO_REMOVE=("ov2775")
        CAM_NAME="basler-vvcam"
        MODE="0"
        write_sensor_cfg_file "Sensor0_Entry.cfg" $CAM_NAME $MODE
    ;;
    basler_1080p60 )
        MODULES=("basler-camera-driver-vvcam" "${MODULES[@]}")
        MODULES_TO_REMOVE=("ov2775")
        CAM_NAME="basler-vvcam"
        MODE="1"
        write_sensor_cfg_file "Sensor0_Entry.cfg" $CAM_NAME $MODE
    ;;
    basler_4k30hdr )
        MODULES=("basler-camera-driver-vvcam" "${MODULES[@]}")
        MODULES_TO_REMOVE=("ov2775")
        CAM_NAME="basler-vvcam"
        MODE="2"
        write_sensor_cfg_file "Sensor0_Entry.cfg" $CAM_NAME $MODE
    ;;
    basler_1080p60hdr )
        MODULES=("basler-camera-driver-vvcam" "${MODULES[@]}")
        MODULES_TO_REMOVE=("ov2775")
        CAM_NAME="basler-vvcam"
        MODE="3"
        write_sensor_cfg_file "Sensor0_Entry.cfg" $CAM_NAME $MODE
    ;;
    ov2775_1080p30 )
        MODULES=("ov2775" "${MODULES[@]}")
        MODULES_TO_REMOVE=("basler-camera-driver-vvcam")
        CAM_NAME="ov2775"
        MODE="0"
        write_sensor_cfg_file "Sensor0_Entry.cfg" $CAM_NAME $MODE
    ;;
    ov2775_1080p30hdr )
        MODULES=("ov2775" "${MODULES[@]}")
        MODULES_TO_REMOVE=("basler-camera-driver-vvcam")
        CAM_NAME="ov2775"
        MODE="1"
        write_sensor_cfg_file "Sensor0_Entry.cfg" $CAM_NAME $MODE
    ;;
    ov2775_720p )
        MODULES=("ov2775" "${MODULES[@]}")
        MODULES_TO_REMOVE=("basler-camera-driver-vvcam")
        CAM_NAME="ov2775"
        MODE="2"
        write_sensor_cfg_file "Sensor0_Entry.cfg" $CAM_NAME $MODE
    ;;
    *)
        echo "ISP configuration \"$ISP_CONFIG\" unsupported."
        echo -e "$USAGE" >&2
        exit 1
    ;;
esac

if [ ! -f $JSON_FILE ]; then
    echo "Configuration file does not exist: $JSON_FILE" >&2
    exit 1
fi

PIDS_TO_KILL=`pgrep -f video_test\|isp_media_server`
if [ ! -z "$PIDS_TO_KILL" ]
then
    echo "Killing preexisting instances of video_test and isp_media_server:"
    echo `ps $PIDS_TO_KILL`
    pkill -f video_test\|isp_media_server
fi

# Need a sure way to wait untill all the above processes terminated
sleep 1

if [ "$LOAD_MODULES" == "1" ]; then
    load_modules
fi

if [ "$LOCAL_RUN" == "1" ]; then
    set_libs_path "libmedia_server.so"
fi

echo "Starting single camera isp_media_server with configuration"
./isp_media_server CAMERA0

# this should now work
# gst-launch-1.0 -v v4l2src device=/dev/video0 ! waylandsink
