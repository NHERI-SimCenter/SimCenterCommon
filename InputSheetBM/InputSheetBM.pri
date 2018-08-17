#-------------------------------------------------
#
# Project created by fmk
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
$$PWD/BimClasses.cpp 




HEADERS += \
$$PWD/InputWidgetSheetSIM.h \
$$PWD/ClineInputWidget.h \
$$PWD/BeamInputWidget.h \
$$PWD/FloorInputWidget.h \
$$PWD/BraceInputWidget.h \
$$PWD/ColumnInputWidget.h \
$$PWD/SteelInputWidget.h \
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
$$PWD/BimClasses.h

INCLUDEPATH+="/usr/local/jansson/include"
LIBS+="/usr/local/jansson/lib/libjansson.a"
