TARGET = fltest_qt_books
TEMPLATE = app
INCLUDEPATH += .

HEADERS     = bookdelegate.h bookwindow.h initdb.h
RESOURCES   = books.qrc
SOURCES     = bookdelegate.cpp main.cpp bookwindow.cpp
FORMS       = bookwindow.ui

QT += sql widgets widgets
requires(qtConfig(tableview))

# install
target.path = ../../../OK8MP-linux-fs/rootfs/usr/bin/
INSTALLS += target

