#-------------------------------------------------
#
# Project created by QtCreator 2017-08-08T14:26:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Common
TEMPLATE = app


SOURCES += mainCommon.cpp\
        mainWindowCommon.cpp \
    HeaderWidget.cpp \
    FooterWidget.cpp

HEADERS  += mainWindowCommon.h \
    HeaderWidget.h \
    FooterWidget.h

FORMS    += mainWindowCommon.ui

RESOURCES += \
    images.qrc \
    style.qss \
    sim_logo_footer.png

DISTFILES += \
    style.qss
