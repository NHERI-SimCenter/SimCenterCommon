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
    $$PWD/Utils/RelativePathResolver.cpp \
    $$PWD/Utils/dialogabout.cpp \
    $$PWD/sectiontitle.cpp \
    $$PWD/FooterWidget.cpp \
    $$PWD/SimCenterWidget.cpp \
    $$PWD/SimCenterAppWidget.cpp\
    $$PWD/NoArgSimCenterApp.cpp\
    $$PWD/SimCenterAppSelection.cpp \
    $$PWD/SimCenterAppEventSelection.cpp \
    $$PWD/SimCenterPreferences.cpp \
    $$PWD/GoogleAnalytics.cpp

HEADERS += $$PWD/HeaderWidget.h \
    $$PWD/Utils/RelativePathResolver.h \
    $$PWD/Utils/dialogabout.h \
    $$PWD/sectiontitle.h \
    $$PWD/FooterWidget.h \
    $$PWD/SimCenterWidget.h \
    $$PWD/NoArgSimCenterApp.h \
    $$PWD/SimCenterAppSelection.h \
    $$PWD/SimCenterAppEventSelection.h \    
    $$PWD/SimCenterAppWidget.h \
    $$PWD/SimCenterPreferences.h \
    $$PWD/GoogleAnalytics.h

FORMS += \
    $$PWD/Utils/dialogabout.ui

RESOURCES += \
    $$PWD/images1.qrc \

