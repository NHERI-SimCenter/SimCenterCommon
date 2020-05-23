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

SOURCES += mainCommon.cpp\
        MainWindowCommon.cpp\
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
    GeneralInformationWidget.cpp \
    FramesectionInputWidget.cpp \
    SlabsectionInputWidget.cpp \
    WallsectionInputWidget.cpp \
    ConnectionInputWidget.cpp \
    PointInputWidget.cpp

HEADERS  += MainWindowCommon.h\
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
    GeneralInformationWidget.h \
    FramesectionInputWidget.h \
    SlabsectionInputWidget.h \
    WallsectionInputWidget.h \
    ConnectionInputWidget.h \
    PointInputWidget.h


#FORMS    += mainwindowCommon.ui

#RESOURCES += \
#    schema.qrc



