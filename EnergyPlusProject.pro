QT += \
    core gui \
    charts \
    testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS_RELEASE += -g
QMAKE_CFLAGS_RELEASE += -g
QMAKE_LFLAGS_RELEASE = -mthreads -Wl

TARGET = EnergyPlusProject

TEMPLATE = app

INCLUDEPATH += \
    $$PWD/include/

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    include/global/debug.h \
    include/global/stdafx.h \
    include/login/loginwindow.h \
    include/mainwindow/mainwindow.h \
    utils/csvreader/csvreader.h \
    utils/pathmanager/pathmanager.h \
    utils/custom_validator.hpp \
    utils/mcinfo/mcinfo.h \
    include/login/usraccount.h \
    include/mainwindow/set_rate_diaglog.h \
    include/qerrorobject.h \
    utils/error/error.h \
    utils/no_class_funcs.h \
    include/core/ephandler.h \
    include/core/handlemachine.h \
    include/core/all.h \
    include/core/column.h \
    include/core/roomstate.h \
    include/core/sheet.h \
    include/core/processor.h \
    include/mainwindow/quarter_dialog.h \
    utils/custom_widget.h \
    include/login/logo.h \
    include/chart/drilldownchart.h \
    include/chart/drilldownslice.h \
    include/chart/datastage.h \
    test/ColumnTest.h

SOURCES += \
    src/login/loginwindow.cpp \
    src/mainwindow/mainwindow.cpp \
    src/main.cpp \
    utils/csvreader/csvreader.cpp \
    utils/pathmanager/pathmanager.cpp \
    utils/mcinfo/mcinfo.cpp \
    src/login/usraccount.cpp \
    src/mainwindow/set_rate_dialog.cpp \
    src/qerrorobject.cpp \
    utils/error/error.cpp \
    utils/no_class_funcs.cpp \
    src/core/ephandler.cpp \
    src/core/handlemachine.cpp \
    src/core/roomstate.cpp \
    src/core/sheet.cpp \
    src/core/processor.cpp \
    src/mainwindow/quarter_dialog.cpp \
    utils/custom_widget.cpp \
    src/login/logo.cpp \
    src/chart/drilldownchart.cpp \
    src/chart/drilldownslice.cpp \
    src/chart/datastage.cpp \
    test/ColumnTest.cpp

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
    ui/set_rate_dialog.ui \
    ui/quarter_dialog.ui

RESOURCES += \
    res.qrc
