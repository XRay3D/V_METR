#-------------------------------------------------
#
# Project created by QtCreator 2013-07-04T20:46:04
#
#-------------------------------------------------

QT += core gui serialport widgets

TARGET = V_METR

TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    hw/irt.cpp \
    hw/commutator.cpp \
    hw/common_interfaces.cpp \
    hw/mi.cpp \
    mytablemodel.cpp \
    layoutmodel.cpp \
    tableview.cpp

HEADERS  += mainwindow.h \
    interface.h \
    hw/common_interfaces.h \
    hw/irt.h \
    hw/commutator.h \
    hw/mi.h \
    mytablemodel.h \
    layoutmodel.h \
    tableview.h

FORMS    += mainwindow.ui

include(../elemer_ascii/elemer_ascii.pri)
