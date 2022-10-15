#!/bin/sh 

export QT_GSTREAMER_CAMERABIN_VIDEOSRC_DEVICE="/dev/video2"
export QT_GSTREAMER_CAMERABIN_VIDEOSRC_FILTER="video/x-raw,format=YUY2,framerate=30/1"
export QT_GSTREAMER_CAMERABIN_VIDEOSRC_IO_MODE=2

/usr/bin/fltest_qt_declarative_camera
