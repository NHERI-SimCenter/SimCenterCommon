#This project will build common as a static library

#Adding the widgets and network modules as prerequisites
QT += widgets network

#Include the common pri file
include($$PWD/common.pri)

#Builing a library
TEMPLATE = lib

#Configure the library to be static
CONFIG += staticlib
