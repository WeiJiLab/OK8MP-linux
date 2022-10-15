QT += widgets

TARGET = fltest_qt_qopenglwidget

SOURCES += main.cpp \
           glwidget.cpp \
           mainwindow.cpp \
           bubble.cpp

HEADERS += glwidget.h \
           mainwindow.h \
           bubble.h

RESOURCES += texture.qrc

# install
target.path = ../../../OK8MP-linux-fs/rootfs/usr/bin/
INSTALLS += target

