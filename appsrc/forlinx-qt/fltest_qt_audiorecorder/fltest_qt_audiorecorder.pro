TEMPLATE = app
TARGET = fltest_qt_audiorecorder

QT += multimedia

win32:INCLUDEPATH += $$PWD

HEADERS = \
    audiorecorder.h \
    qaudiolevel.h

SOURCES = \
    main.cpp \
    audiorecorder.cpp \
    qaudiolevel.cpp

FORMS += audiorecorder.ui

# install
target.path = ../../../OK8MP-linux-fs/rootfs/usr/bin/
INSTALLS += target


QT+=widgets
