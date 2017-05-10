QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EnergyPlusProject

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    handlemachine.h \
    mainwindow.h \
    debug.h \
    global.h \
    pathmanager.h \
    csvreader.h \
    energyform.h \
    loginwindow.h \
    mac_address.hpp \
    convert_pip.hpp \
    userid_creator.hpp \
    stdafx.h \
    passowrd_checker.hpp \
    utils/custom_widget.hpp \
    utils/thread_pool.hpp

SOURCES += \
    main.cpp \
    handlemachine.cpp \
    mainwindow.cpp \
    pathmanager.cpp \
    csvreader.cpp \
    loginwindow.cpp

DISTFILES += \
    README.md \
    cityfiles/Harbin.json \
    documents/plan.md

FORMS += \
    mainwindow.ui \
    login.ui

RESOURCES += \
    res.qrc
