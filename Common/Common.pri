#-------------------------------------------------
#
# Project created by fmk
#
#-------------------------------------------------


INCLUDEPATH += $$PWD
INCLUDEPATH += "../SimCenterCommon/Common"
INCLUDEPATH += "../Common"

include($$PWD/ZipUtils/ZipUtils.pri)

SOURCES += $$PWD/HeaderWidget.cpp \
    $$PWD/sectiontitle.cpp \
    $$PWD/FooterWidget.cpp \
    $$PWD/SimCenterWidget.cpp \
    $$PWD/SimCenterAppWidget.cpp \
    $$PWD/SimCenterPreferences.cpp \
    $$PWD/GoogleAnalytics.cpp

HEADERS += $$PWD/HeaderWidget.h \
    $$PWD/sectiontitle.h \
    $$PWD/FooterWidget.h \
    $$PWD/SimCenterWidget.h \
    $$PWD/SimCenterAppWidget.h \
    $$PWD/SimCenterPreferences.h \
    $$PWD/GoogleAnalytics.h

RESOURCES += \
    $$PWD/images1.qrc \

