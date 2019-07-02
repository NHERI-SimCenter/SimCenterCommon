#-------------------------------------------------
#
# Project created by fmk
# additions to jansson includes by pmackenz (18/06/2019)
#
#-------------------------------------------------

#message($$_PRO_FILE_PWD_)
#message($$PWD)

INCLUDEPATH+=$$PWD

SOURCES += \
$$PWD/InputWidgetSheetSIM.cpp \
$$PWD/ClineInputWidget.cpp \
$$PWD/BeamInputWidget.cpp \
$$PWD/FloorInputWidget.cpp \
$$PWD/BraceInputWidget.cpp \
$$PWD/ColumnInputWidget.cpp \
$$PWD/SteelInputWidget.cpp \
$$PWD/SteelWSectionInputWidget.cpp \
$$PWD/SteelTubeSectionInputWidget.cpp \
$$PWD/ConcreteInputWidget.cpp \
$$PWD/JsonValidator.cpp \
$$PWD/SpreadsheetWidget.cpp \
$$PWD/SimCenterTableWidget.cpp \
$$PWD/Cell.cpp \
$$PWD/GeneralInformationWidget.cpp \
$$PWD/FramesectionInputWidget.cpp \
$$PWD/SlabsectionInputWidget.cpp \
$$PWD/WallsectionInputWidget.cpp \
$$PWD/ConnectionInputWidget.cpp \
$$PWD/PointInputWidget.cpp \
$$PWD/BimClasses.cpp \
$$PWD/Materials.cpp \
$$PWD/FrameSections.cpp \
$$PWD/WallSections.cpp \
$$PWD/Beam.cpp \
$$PWD/Column.cpp \
$$PWD/Wall.cpp \
$$PWD/WallInputWidget.cpp



HEADERS += \
$$PWD/InputWidgetSheetSIM.h \
$$PWD/ClineInputWidget.h \
$$PWD/BeamInputWidget.h \
$$PWD/FloorInputWidget.h \
$$PWD/BraceInputWidget.h \
$$PWD/ColumnInputWidget.h \
$$PWD/SteelInputWidget.h \
$$PWD/SteelWSectionInputWidget.h \
$$PWD/SteelTubeSectionInputWidget.h \
$$PWD/ConcreteInputWidget.h \
$$PWD/JsonValidator.h \
$$PWD/SpreadsheetWidget.h \
$$PWD/SimCenterTableWidget.h \
$$PWD/Cell.h \
$$PWD/GeneralInformationWidget.h \
$$PWD/FramesectionInputWidget.h \
$$PWD/SlabsectionInputWidget.h \
$$PWD/WallsectionInputWidget.h \
$$PWD/ConnectionInputWidget.h \
$$PWD/SimCenterTableWidget.h \
$$PWD/PointInputWidget.h \
$$PWD/BimClasses.h \
$$PWD/Materials.h \
$$PWD/FrameSections.h \
$$PWD/WallSections.h \
$$PWD/Beam.h \
$$PWD/Column.h \
$$PWD/Wall.h \
$$PWD/WallInputWidget.h

macx{
    exists("/usr/local/jansson/lib/libjansson.a"){
        INCLUDEPATH += "/usr/local/jansson/include"
        LIBS+="/usr/local/jansson/lib/libjansson.a"
    }
    exists("/usr/local/lib/libjansson.a"){
        INCLUDEPATH += "/usr/local/include"
        LIBS+="/usr/local/lib/libjansson.a"
    }
}

unix:!macx{
    LIBS+="/usr/lib/x86_64-linux-gnu/libjansson.a"
}
