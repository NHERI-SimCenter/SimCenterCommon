#-------------------------------------------------
#
# Project created by fmk
# additions to jansson includes by pmackenz (18/06/2019)
#
#-------------------------------------------------

#message($$_PRO_FILE_PWD_)
#message($$PWD)

INCLUDEPATH+=$$PWD

SOURCES += $$PWD/GeneralInformationWidget.cpp

HEADERS += $$PWD/GeneralInformationWidget.h

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



