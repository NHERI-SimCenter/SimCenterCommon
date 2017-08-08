#-------------------------------------------------
#
# Project created by QtCreator 2017-08-08T14:26:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Common
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    HeaderWidget.cpp \
    FooterWidget.cpp

HEADERS  += mainwindow.h \
    HeaderWidget.h \
    FooterWidget.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc

DISTFILES += \
    style.qss
