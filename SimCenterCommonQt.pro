#This project will build common as a static library

CONFIG += conan_basic_setup
include($$OUT_PWD/conanbuildinfo.pri)

#Adding the widgets and network modules as prerequisites
QT += widgets network

#Include the common pri file
include($$PWD/Common/common.pri)
include($$PWD/InputSheetBM/InputSheetBM.pri)
include($$PWD/RandomVariables/RandomVariables.pri)

#Builing a library
TEMPLATE = lib

#Configure the library to be static
CONFIG += staticlib
