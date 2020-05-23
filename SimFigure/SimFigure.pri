################################################################
# SimFigure Widget Library
# Copyright (C) 2019   Peter Mackenzie-Helnwein
# Copyright (C) 2019   SimCenter UC-Berkeley
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the BSD License, Version 2.0
################################################################

#message($$_PRO_FILE_PWD_)
message($$PWD)

config += depend_includepath

INCLUDEPATH += $$PWD

INCLUDEPATH += "../source"
INCLUDEPATH += "../SimFigure"
INCLUDEPATH += "../SimFigure/source"
INCLUDEPATH += "../SimCenterCommon/SimFigure"
INCLUDEPATH += "../SimCenterCommon/SimFigure/source"

#INCLUDEPATH += "./../widgets/Common/"

SOURCES += $$PWD/source/simfigure.cpp

HEADERS += $$PWD/source/simfigure.h

FORMS   += $$PWD/source/simfigure.ui




