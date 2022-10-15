TEMPLATE = app
TARGET = fltest_qt_qmlvideofx

QT += quick multimedia

SOURCES += filereader.cpp main.cpp
HEADERS += filereader.h trace.h

RESOURCES += qmlvideofx.qrc

include($$PWD/../snippets/performancemonitor/performancemonitordeclarative.pri)

# install
target.path = ../../../OK8MP-linux-fs/rootfs/usr/bin/
INSTALLS += target


ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml

QMAKE_INFO_PLIST = Info.plist
