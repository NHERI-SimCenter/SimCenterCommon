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
    $$PWD/Utils/JsonFunctions.cpp \
    $$PWD/Utils/RelativePathResolver.cpp \
    $$PWD/Utils/FileOperations.cpp \
    $$PWD/Utils/SimCenterConfigFile.cpp \    
    $$PWD/Utils/dialogabout.cpp \
    $$PWD/Utils/ProgramOutputDialog.cpp \
    $$PWD/Utils/PythonProcessHandler.cpp \
    $$PWD/SectionTitle.cpp \
    $$PWD/FooterWidget.cpp \
    $$PWD/SimCenterWidget.cpp \
    $$PWD/SC_ComboBox.cpp \
    $$PWD/SC_CheckBox.cpp \
    $$PWD/SC_DoubleLineEdit.cpp \
    $$PWD/SC_StringLineEdit.cpp \
    $$PWD/SC_IntLineEdit.cpp \
	$$PWD/SC_QRadioButton.cpp \
    $$PWD/SC_TableEdit.cpp \
    $$PWD/SC_FileEdit.cpp \
    $$PWD/SC_DirEdit.cpp \    
    $$PWD/SC_ResultsWidget.cpp \
	$$PWD/SC_Chart.cpp \	
	$$PWD/SC_TimeSeriesResultChart.cpp \	
    $$PWD/SimCenterAppWidget.cpp\
    $$PWD/NoArgSimCenterApp.cpp\
    $$PWD/SimCenterAppSelection.cpp \
    $$PWD/SimCenterEventAppSelection.cpp \
    $$PWD/SimCenterAppEventSelection.cpp \        
    $$PWD/SimCenterAppMulti.cpp \    
    $$PWD/SimCenterPreferences.cpp \
    $$PWD/SimCenterDirWatcher.cpp \    
    $$PWD/NoneWidget.cpp \    
    $$PWD/GoogleAnalytics.cpp

HEADERS += $$PWD/HeaderWidget.h \
    $$PWD/Utils/RelativePathResolver.h \
    $$PWD/Utils/FileOperations.h \
    $$PWD/Utils/JsonFunctions.h \
    $$PWD/Utils/dialogabout.h \
    $$PWD/Utils/ProgramOutputDialog.h \
    $$PWD/Utils/SimCenterConfigFile.h \        
    $$PWD/Utils/PythonProcessHandler.h \      
    $$PWD/SectionTitle.h \
    $$PWD/FooterWidget.h \
    $$PWD/SimCenterWidget.h \
    $$PWD/SC_ComboBox.h \
    $$PWD/SC_CheckBox.h \
    $$PWD/SC_DoubleLineEdit.h \
    $$PWD/SC_StringLineEdit.h \
    $$PWD/SC_IntLineEdit.h \
	$$PWD/SC_QRadioButton.h \
    $$PWD/SC_TableEdit.h \
    $$PWD/SC_FileEdit.h \
    $$PWD/SC_DirEdit.h \    
    $$PWD/SC_ResultsWidget.h \
	$$PWD/SC_Chart.h \    
	$$PWD/SC_TimeSeriesResultChart.h \
    $$PWD/NoArgSimCenterApp.h \
    $$PWD/SimCenterAppSelection.h \
    $$PWD/SimCenterAppMulti.h \    
    $$PWD/SimCenterEventAppSelection.h \
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

