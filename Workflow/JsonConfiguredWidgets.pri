#-------------------------------------------------
#
# Project created by fmk
#
#-------------------------------------------------

#message($$_PRO_FILE_PWD_)
#message($$PWD)

INCLUDEPATH+=$$PWD

INCLUDEPATH+=../Common

SOURCES += $$PWD/WORKFLOW/JsonConfiguredWidget.cpp \
    $$PWD/WORKFLOW/SimCenterComboBox.cpp \
    $$PWD/WORKFLOW/SimCenterFileInput.cpp \
    $$PWD/WORKFLOW/SimCenterRVLineEdit.cpp \
    $$PWD/WORKFLOW/SimCenterLineEdit.cpp \
    $$PWD/WORKFLOW/SimCenterDoubleSpinBox.cpp \
    $$PWD/WORKFLOW/SimCenterSpinBox.cpp \
    $$PWD/WORKFLOW/WaveBackgroundWidget.cpp
    
HEADERS += $$PWD/WORKFLOW/JsonConfiguredWidget.h \
    $$PWD/WORKFLOW/SimCenterComboBox.h \
    $$PWD/WORKFLOW/SimCenterFileInput.h \
    $$PWD/WORKFLOW/SimCenterRVLineEdit.h \
    $$PWD/WORKFLOW/SimCenterLineEdit.h \
    $$PWD/WORKFLOW/SimCenterDoubleSpinBox.h \
    $$PWD/WORKFLOW/SimCenterSpinBox.h \
    $$PWD/WORKFLOW/AnimatedStackedWidget.h \
    $$PWD/WORKFLOW/WaveBackgroundWidget.h