TEMPLATE=app
TARGET=fltest_qt_declarative_camera

QT += quick qml multimedia

SOURCES += qmlcamera.cpp
RESOURCES += declarative-camera.qrc

# install
target.path = ../../../OK8MP-linux-fs/rootfs/usr/bin/
INSTALLS += target


winrt {
    WINRT_MANIFEST.capabilities_device += webcam microphone
}
