#-------------------------------------------------
#
# Project created by fmk
#
#-------------------------------------------------

#message($$_PRO_FILE_PWD_)
#message($$PWD)

INCLUDEPATH+=$$PWD

INCLUDEPATH+=../Common

SOURCES += $$PWD/RandomVariableDistribution.cpp \
    $$PWD/NormalDistribution.cpp \
    $$PWD/RandomVariable.cpp \
    $$PWD/LognormalDistribution.cpp \
    $$PWD/BetaDistribution.cpp \
    $$PWD/RandomVariablesContainer.cpp \
    $$PWD/UniformDistribution.cpp \
    $$PWD/ConstantDistribution.cpp \
    $$PWD/ContinuousDesignDistribution.cpp \
    $$PWD/WeibullDistribution.cpp \
    $$PWD/GumbelDistribution.cpp \
    $$PWD/UserDef.cpp \
    $$PWD/LineEditRV.cpp


HEADERS += $$PWD/RandomVariableDistribution.h \
    $$PWD/NormalDistribution.h \
    $$PWD/RandomVariable.h \
    $$PWD/LognormalDistribution.h \
    $$PWD/BetaDistribution.h \
    $$PWD/RandomVariablesContainer.h \
    $$PWD/UniformDistribution.h \
    $$PWD/ConstantDistribution.h \
    $$PWD/ContinuousDesignDistribution.h \
    $$PWD/WeibullDistribution.h \
    $$PWD/GumbelDistribution.h \
    $$PWD/UserDef.h \
    $$PWD/LineEditRV.h

