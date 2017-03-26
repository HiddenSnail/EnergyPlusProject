QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EnergyPlusProject

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    handlemachine.h \
    json/autolink.h \
    json/config.h \
    json/features.h \
    json/forwards.h \
    json/json.h \
    json/json_batchallocator.h \
    json/json_tool.h \
    json/reader.h \
    json/value.h \
    json/writer.h \
    mainwindow.h \
    utils.h

SOURCES += \
    main.cpp \
    handlemachine.cpp \
    json/json_internalarray.inl \
    json/json_internalmap.inl \
    json/json_reader.cpp \
    json/json_value.cpp \
    json/json_valueiterator.inl \
    json/json_writer.cpp \
    mainwindow.cpp \
    utils.cpp

DISTFILES += \
    README.md \
    json/sconscript \
    cityfiles/Harbin.json

FORMS += \
    mainwindow.ui
