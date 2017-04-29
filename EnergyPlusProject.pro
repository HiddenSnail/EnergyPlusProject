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
    energyform.h

SOURCES += \
    main.cpp \
    handlemachine.cpp \
    mainwindow.cpp \
    pathmanager.cpp \
    csvreader.cpp

DISTFILES += \
    README.md \
    cityfiles/Harbin.json

FORMS += \
    mainwindow.ui

RESOURCES +=
