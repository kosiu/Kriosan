CODECFORSRC = UTF-8
CODECFORTR = UTF-8

include(../common.pri)
contains( PLATFORM, arm ){
    LIBS += -lustandardkbddriver
    INCLUDEPATH += ../ustandardkbddriver
    QTPLUGIN += ustandardkbddriver
}

# ------------------------------
SOURCES += main.cpp \
    operationscreen.cpp \
    scriptwidget.cpp \
    datagenerator.cpp \
    menuscreen.cpp \
    infoscreen.cpp \
    inspectionscreen.cpp \
    sensorscreen.cpp \
    selectscreen.cpp \
    buzzer.cpp \
 realtimeclock.cpp \
 heater.cpp \
 i2cdevice.cpp \
 adcdevice.cpp \
 filter.cpp
TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt \
    static
TARGET = preview-$${PLATFORM}
DESTDIR = ../bin
HEADERS += operationscreen.h \
    scriptwidget.h \
    item.h \
    i2c-dev.h \
    datagenerator.h \
    menuscreen.h \
    types.h \
    infoscreen.h \
    inspectionscreen.h \
    sensorscreen.h \
    selectscreen.h \
    buzzer.h \
 realtimeclock.h \
 heater.h \
 i2cdevice.h \
 adcdevice.h \
 filter.h
FORMS += operationscreen.ui \
    menuscreen.ui \
    infoscreen.ui \
    inspectionscreen.ui \
    sensorsscreen.ui \
    selectscreen.ui
QT += svg \
    xml \
    script \
    gui
RESOURCES += resources.qrc
DISTFILES += butla.qjs \
    butla.svg \
    moc.qjs \
    moc.svg
TRANSLATIONS = kriosan_en.ts \
    kriosan_de.ts \
    kriosan_cz.ts \
    kriosan_sk.ts

