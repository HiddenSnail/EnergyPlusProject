QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EnergyPlusProject

TEMPLATE = app

INCLUDEPATH += \
    $$PWD/include/

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    include/global/debug.h \
    include/global/stdafx.h \
    include/login/loginwindow.h \
    include/mainwindow/energyform.h \
    include/mainwindow/handlemachine.h \
    include/mainwindow/mainwindow.h \
    utils/csvreader/csvreader.h \
    utils/pathmanager/pathmanager.h \
    utils/custom_validator.hpp \
    utils/custom_widget.hpp \
    utils/mcinfo/mcinfo.h \
    include/login/usraccount.h \
    include/mainwindow/set_rate_diaglog.h

SOURCES += \
    src/login/loginwindow.cpp \
    src/mainwindow/handlemachine.cpp \
    src/mainwindow/mainwindow.cpp \
    src/main.cpp \
    utils/csvreader/csvreader.cpp \
    utils/pathmanager/pathmanager.cpp \
    utils/mcinfo/mcinfo.cpp \
    src/login/usraccount.cpp \
    src/mainwindow/set_rate_dialog.cpp

DISTFILES += \
    README.md \
    documents/plan.md \
    res/icons/login.jpg \
    res/icons/language_1.png \
    res/icons/language_2.png \
    res/icons/logo.png \
    res/stylesheet/login.qss \
    res/stylesheet/main.qss \
    doc/project_dir.txt \
    doc/plan.md

FORMS += \
    ui/loginwindow.ui \
    ui/mainwindow.ui \
    ui/set_rate_dialog.ui

RESOURCES += \
    res.qrc
