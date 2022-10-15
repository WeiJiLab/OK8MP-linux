#!/bin/sh

machine_type="`cat /etc/hostname`"

filename="/media/forlinx/video/1080p_30fps_h265.mp4"

AUDIO_SINK="alsasink"

if [ ! -f $filename ]; then
        echo "Video clip not found"
        exit 1
fi

echo ""
echo "Run H.265 Decoding on VPU"
echo ""
gst-launch-1.0 filesrc location=$filename typefind=true ! \
	video/quicktime ! aiurdemux name=demux demux. ! queue max-size-buffers=0 \
	max-size-time=0 ! vpudec ! imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=1024, \
	height=600 ! waylandsink demux. ! queue max-size-buffers=0 max-size-time=0 \
	! decodebin ! audioconvert ! audioresample ! pulsesink

