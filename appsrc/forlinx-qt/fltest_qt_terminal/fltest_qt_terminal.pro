QT += widgets serialport

TARGET = fltest_qt_terminal
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    console.cpp

HEADERS += \
    mainwindow.h \
    settingsdialog.h \
    console.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    terminal.qrc

# install
target.path = ../../../OK8MP-linux-fs/rootfs/usr/bin/
INSTALLS += target

