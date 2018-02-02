#-------------------------------------------------
#
# Project created by QtCreator 2017-06-06T06:31:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestTreeModel
TEMPLATE = app

include(../RandomVariables/RandomVariables.pri)

INCLUDEPATH+="Users/fmk/NHERI/widgets/Common"
#INCLUDEPATH+="/Users/mauricemanning/Dev/code/simcenter/widgets/Common"

SOURCES += main.cpp\
        MainWindow.cpp\
        InputWidgetSheetBM.cpp \
    ClineInputWidget.cpp \
    BeamInputWidget.cpp \
    FloorInputWidget.cpp \
    BraceInputWidget.cpp \
    ColumnInputWidget.cpp \
    SteelInputWidget.cpp \
    ConcreteInputWidget.cpp \
    ../Common/SimCenterWidget.cpp \
    ../Common/sectiontitle.cpp \
    JsonValidator.cpp \
    SpreadsheetWidget.cpp \
    SimCenterTableWidget.cpp \
    Cell.cpp \
    GeneralInformationWidget.cpp

HEADERS  += MainWindow.h\
        InputWidgetSheetBM.h \
    ClineInputWidget.h \
    BeamInputWidget.h \
    FloorInputWidget.h \
    BraceInputWidget.h \
    ColumnInputWidget.h \
    SteelInputWidget.h \
    ConcreteInputWidget.h \
    ../Common/SimCenterWidget.h \
    ../Common/sectiontitle.h \
    JsonValidator.h \
    SpreadsheetWidget.h \
    SimCenterTableWidget.h \
    Cell.h \
    GeneralInformationWidget.h


#FORMS    += mainwindow.ui

RESOURCES += \
    schema.qrc



