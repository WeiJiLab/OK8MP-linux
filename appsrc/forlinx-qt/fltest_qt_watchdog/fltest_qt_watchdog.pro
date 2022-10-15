#-------------------------------------------------
#
# Project created by QtCreator 2013-11-26T14:43:58
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fltest_qt_watchdog
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
# install
target.path = ../../../OK8MP-linux-fs/rootfs/usr/bin/
INSTALLS += target

