QT += core testlib gui widgets

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += c++14

TEMPLATE = app

include(../external/catch/catch.pri)
include(../external/trompeloeil/trompeloeil.pri)
include(../Common/Common.pri)

PROJECT_DIR = $$PWD/../Common

INCLUDEPATH += \
    $$PROJECT_DIR


PROJECT_SOURCES += \
       
PROJECT_HEADERS += \
       
LIBS += \

RESOURCES += \

SOURCES +=  \
    *.cpp \
    $$PROJECT_SOURCES

HEADERS += \
    *.h \
    $$PROJECT_HEADERS
