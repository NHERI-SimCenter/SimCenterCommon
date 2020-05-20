#-------------------------------------------------
#
# Project created by QtCreator 2017-05-18T08:41:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RandomVariablesGen
TEMPLATE = app

include(RandomVariables.pri)
include(../Common/Common.pri)

SOURCES += mainRV.cpp \
        MainWindowRV.cpp

HEADERS  += MainWindowRV.h

FORMS    += mainwindowRV.ui
