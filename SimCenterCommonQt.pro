#This project will build common as a static library

#Making sure we are not using multi configuration
CONFIG -= debug_and_release

CONFIG += conan_basic_setup
include($$OUT_PWD/conanbuildinfo.pri)

#Adding the widgets and network modules as prerequisites
QT += widgets network charts

#Include the common pri file
include($$PWD/Common/Common.pri)
include($$PWD/InputSheetBM/InputSheetBM.pri)
include($$PWD/RandomVariables/RandomVariables.pri)
include($$PWD/Workflow/Workflow.pri)

#Builing a library
TEMPLATE = lib

#Configure the library to be static
CONFIG += staticlib
