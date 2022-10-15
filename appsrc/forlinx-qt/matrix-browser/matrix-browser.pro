TEMPLATE = app

QT +=  webkitwidgets
TARGET = matrix-browser

SOURCES += main.cpp

# install
target.path = ../../../OK8MP-linux-fs/rootfs/usr/bin/
INSTALLS += target
