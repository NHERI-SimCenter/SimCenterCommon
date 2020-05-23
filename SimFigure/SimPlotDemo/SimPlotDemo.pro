#-------------------------------------------------
#
# Project created by QtCreator 2019-05-24T14:59:47
#
#-------------------------------------------------

QT       += core gui
unix: QT += svg opengl concurrent
win32: QT += opengl printsupport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport network

TARGET   = PileGroupTool
TEMPLATE = app
VERSION  = 2.1.0

TARGET = SimPlotDemo
TEMPLATE = app

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

unix:  QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-unused-variable -std=c++11
win32: QMAKE_CXXFLAGS += /Y- -wd"4100"

#unix: DEPENDPATH += /usr/local/qwt-6.1.4-svn
#win32: DEPENDPATH += C:\Qwt-6.1.3

win32: include(C:\qwt-6.1.3\features\qwt.prf)
unix: include(/usr/local/qwt-6.1.4/features/qwt.prf)

include(../../../SimCenterCommon/SimFigure/SimFigure.pri)

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp 

HEADERS += \
        mainwindow.h 

FORMS += \
        mainwindow.ui 

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    TODO.txt
