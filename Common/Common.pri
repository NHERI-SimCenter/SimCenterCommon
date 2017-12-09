#-------------------------------------------------
#
# Project created by fmk
#
#-------------------------------------------------

#message($$_PRO_FILE_PWD_)
#message($$PWD)

INCLUDEPATH += "../widgets/Common"
INCLUDEPATH += "../Common"

#INCLUDEPATH += "./../widgets/Common/"

SOURCES += $$PWD/HeaderWidget.cpp \
    $$PWD/sectiontitle.cpp \
    $$PWD/FooterWidget.cpp \
    $$PWD/SimCenterWidget.cpp

HEADERS += $$PWD/HeaderWidget.h \
    $$PWD/sectiontitle.h \
    $$PWD/FooterWidget.h \
    $$PWD/SimCenterWidget.h

RESOURCES += \
    $$PWD/images1.qrc \

