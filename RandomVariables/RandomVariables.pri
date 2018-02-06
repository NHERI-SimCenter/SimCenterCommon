#-------------------------------------------------
#
# Project created by fmk
#
#-------------------------------------------------

#message($$_PRO_FILE_PWD_)
#message($$PWD)

INCLUDEPATH+=../Common

SOURCES += $$PWD/RandomVariableDistribution.cpp \
    $$PWD/NormalDistribution.cpp \
    $$PWD/RandomVariable.cpp \
    $$PWD/LognormalDistribution.cpp \
    $$PWD/BetaDistribution.cpp \
    $$PWD/RandomVariableInputWidget.cpp \
    $$PWD/UniformDistribution.cpp \
    $$PWD/ConstantDistribution.cpp \
    $$PWD/ContinuousDesignDistribution.cpp


HEADERS += $$PWD/RandomVariableDistribution.h \
    $$PWD/NormalDistribution.h \
    $$PWD/RandomVariable.h \
    $$PWD/LognormalDistribution.h \
    $$PWD/BetaDistribution.h \
    $$PWD/RandomVariableInputWidget.h \
    $$PWD/UniformDistribution.h \
    $$PWD/ConstantDistribution.h \
    $$PWD/ContinuousDesignDistribution.h \
    $$PWD/ConstantDistribution.h

