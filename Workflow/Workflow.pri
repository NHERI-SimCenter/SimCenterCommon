#-------------------------------------------------
#
# Project created by fmk
#
#-------------------------------------------------

#message($$_PRO_FILE_PWD_)
#message($$PWD)

INCLUDEPATH += $$PWD/UQ
INCLUDEPATH += $$PWD/WORKFLOW
INCLUDEPATH += $$PWD/SIM
INCLUDEPATH += $$PWD/EXECUTION
INCLUDEPATH += $$PWD/EVENTS
INCLUDEPATH += $$PWD/ANALYSIS
INCLUDEPATH += $$PWD/GRAPHICS
INCLUDEPATH += $$PWD/EDP


#INCLUDEPATH += "../SimCenterCommon/Workflow"
#INCLUDEPATH += "../QUO_Methods"

SOURCES += $$PWD/UQ/DakotaResults.cpp \
    $$PWD/UQ/GaussianProcessInputWidget.cpp \
    $$PWD/UQ/LatinHypercubeInputWidget.cpp \
    $$PWD/UQ/DakotaResultsSampling.cpp \
    $$PWD/UQ/ImportanceSamplingInputWidget.cpp \
    $$PWD/UQ/MonteCarloInputWidget.cpp \
    $$PWD/UQ/PCEInputWidget.cpp \
    $$PWD/UQ/UQ_MethodInputWidget.cpp \
    $$PWD/UQ/InputWidgetSampling.cpp \
    $$PWD/UQ/InputWidgetUQ.cpp \
    $$PWD/WORKFLOW/MainWindowWorkflowApp.cpp \
    $$PWD/WORKFLOW/WorkflowAppWidget.cpp \
    $$PWD/WORKFLOW/CustomizedItemModel.cpp \
    $$PWD/SIM/OpenSeesBuildingModel.cpp \
    $$PWD/SIM/MDOF_BuildingModel.cpp \
    $$PWD/SIM/SIM_Selection.cpp \
    $$PWD/SIM/InputWidgetBIM.cpp \
    $$PWD/SIM/OpenSeesParser.cpp \
    $$PWD/ANALYSIS/InputWidgetOpenSeesAnalysis.cpp \
    $$PWD/EXECUTION/RunLocalWidget.cpp \
    $$PWD/EXECUTION/AgaveCurl.cpp \
    $$PWD/EXECUTION/Application.cpp \
    $$PWD/EXECUTION/LocalApplication.cpp \
    $$PWD/EXECUTION/RemoteApplication.cpp \
    $$PWD/EXECUTION/RemoteService.cpp \
    $$PWD/EXECUTION/RemoteJobManager.cpp \
    $$PWD/EVENTS/ExistingSimCenterEvents.cpp \
    $$PWD/EVENTS/InputWidgetExistingEvent.cpp \
    $$PWD/EXECUTION/UserDefinedApplication.cpp \
    $$PWD/GRAPHICS/Controller2D.cpp \
    $$PWD/GRAPHICS/GlWidget2D.cpp \
    $$PWD/GRAPHICS/MyTableWidget.cpp \
    $$PWD/EDP/UserDefinedEDP.cpp \
    $$PWD/EDP/EDP.cpp

HEADERS += $$PWD/UQ/DakotaResults.h \
    $$PWD/UQ/GaussianProcessInputWidget.h \
    $$PWD/UQ/LatinHypercubeInputWidget.h \
    $$PWD/UQ/DakotaResultsSampling.h \
    $$PWD/UQ/ImportanceSamplingInputWidget.h \
    $$PWD/UQ/MonteCarloInputWidget.h \
    $$PWD/UQ/PCEInputWidget.h \
    $$PWD/UQ/UQ_MethodInputWidget.h \
    $$PWD/UQ/InputWidgetSampling.h \
    $$PWD/UQ/InputWidgetUQ.h \
    $$PWD/WORKFLOW/MainWindowWorkflowApp.h \
    $$PWD/WORKFLOW/WorkflowAppWidget.h \
    $$PWD/WORKFLOW/CustomizedItemModel.h \
    $$PWD/SIM/OpenSeesBuildingModel.h \
    $$PWD/SIM/MDOF_BuildingModel.h \
    $$PWD/SIM/SIM_Selection.h \
    $$PWD/SIM/InputWidgetBIM.h \
    $$PWD/SIM/OpenSeesParser.h \
    $$PWD/ANALYSIS/InputWidgetOpenSeesAnalysis.h \
    $$PWD/EXECUTION/RunLocalWidget.h \
    $$PWD/EXECUTION/AgaveCurl.h \
    $$PWD/EXECUTION/Application.h \
    $$PWD/EXECUTION/LocalApplication.h \
    $$PWD/EXECUTION/RemoteApplication.h \
    $$PWD/EXECUTION/RemoteService.h \
    $$PWD/EXECUTION/RemoteJobManager.h \
    $$PWD/EXECUTION/UserDefinedApplication.h \
    $$PWD/EVENTS/ExistingSimCenterEvents.h \
    $$PWD/EVENTS/InputWidgetExistingEvent.h \
    $$PWD/GRAPHICS/Controller2D.h \
    $$PWD/GRAPHICS/GlWidget2D.h \
    $$PWD/GRAPHICS/MyTableWidget.h \
    $$PWD/EDP/UserDefinedEDP.h \
    $$PWD/EDP/EDP.h

