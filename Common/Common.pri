#-------------------------------------------------
#
# Project created by fmk
#
#-------------------------------------------------

#message($$_PRO_FILE_PWD_)
#message($$PWD)

INCLUDEPATH += $$PWD

INCLUDEPATH += "../SimCenterCommon/Common"
INCLUDEPATH += "../Common"

#INCLUDEPATH += "./../widgets/Common/"

SOURCES += $$PWD/HeaderWidget.cpp \
    $$PWD/sectiontitle.cpp \
    $$PWD/FooterWidget.cpp \
    $$PWD/SimCenterWidget.cpp \
    $$PWD/SimCenterAppWidget.cpp \
    $$PWD/GoogleAnalytics.cpp

HEADERS += $$PWD/HeaderWidget.h \
    $$PWD/sectiontitle.h \
    $$PWD/FooterWidget.h \
    $$PWD/SimCenterWidget.h \
    $$PWD/SimCenterAppWidget.h \
    $$PWD/GoogleAnalytics.h

RESOURCES += \
    $$PWD/images1.qrc \

