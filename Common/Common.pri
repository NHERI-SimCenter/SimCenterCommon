#-------------------------------------------------
#
# Project created by fmk
#
#-------------------------------------------------


INCLUDEPATH += $$PWD
INCLUDEPATH += "../SimCenterCommon/Common"
INCLUDEPATH += "../Common"
INCLUDEPATH += "../Utils"

include($$PWD/ZipUtils/ZipUtils.pri)

SOURCES += $$PWD/HeaderWidget.cpp \
    $$PWD/Utils/RelativePathResolver.cpp \
    $$PWD/Utils/FileOperations.cpp \
    $$PWD/Utils/dialogabout.cpp \
    $$PWD/Utils/ProgramOutputDialog.cpp \
    $$PWD/Utils/PythonProcessHandler.cpp \
    $$PWD/SectionTitle.cpp \
    $$PWD/FooterWidget.cpp \
    $$PWD/SimCenterWidget.cpp \
    $$PWD/SC_ComboBox.cpp \
    $$PWD/SC_DoubleLineEdit.cpp \
    $$PWD/SC_IntLineEdit.cpp \
    $$PWD/SC_TableEdit.cpp \                
    $$PWD/ModularPython.cpp \
    $$PWD/SimCenterAppWidget.cpp\
    $$PWD/NoArgSimCenterApp.cpp\
    $$PWD/SimCenterAppSelection.cpp \
    $$PWD/SimCenterAppMulti.cpp \    
    $$PWD/SimCenterAppEventSelection.cpp \
    $$PWD/SimCenterPreferences.cpp \
    $$PWD/SimCenterDirWatcher.cpp \    
    $$PWD/NoneWidget.cpp \    
    $$PWD/GoogleAnalytics.cpp

HEADERS += $$PWD/HeaderWidget.h \
    $$PWD/Utils/RelativePathResolver.h \
    $$PWD/Utils/FileOperations.h \
    $$PWD/Utils/dialogabout.h \
    $$PWD/Utils/ProgramOutputDialog.h \
    $$PWD/Utils/PythonProcessHandler.h \
    $$PWD/SectionTitle.h \
    $$PWD/FooterWidget.h \
    $$PWD/SimCenterWidget.h \
    $$PWD/SC_ComboBox.h \
    $$PWD/SC_DoubleLineEdit.h \
    $$PWD/SC_IntLineEdit.h \
    $$PWD/SC_TableEdit.h \     
    $$PWD/ModularPython.h \
    $$PWD/NoArgSimCenterApp.h \
    $$PWD/SimCenterAppSelection.h \
    $$PWD/SimCenterAppMulti.h \    
    $$PWD/SimCenterAppEventSelection.h \    
    $$PWD/SimCenterAppWidget.h \
    $$PWD/SimCenterPreferences.h \
    $$PWD/SimCenterDirWatcher.h \        
    $$PWD/NoneWidget.h \
    $$PWD/GoogleAnalytics.h

FORMS += \
    $$PWD/Utils/dialogabout.ui

RESOURCES += \
    $$PWD/images1.qrc \

