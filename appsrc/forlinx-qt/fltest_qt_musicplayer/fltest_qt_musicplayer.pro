TEMPLATE = app
TARGET = fltest_qt_musicplayer

QT += widgets multimedia

HEADERS = \
    musicplayer.h \
    volumebutton.h

SOURCES = \
    main.cpp \
    musicplayer.cpp \
    volumebutton.cpp

RC_ICONS = images/qt-logo.ico

# install
target.path = ../../../OK8MP-linux-fs/rootfs/usr/bin/
INSTALLS += target

