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
INCLUDEPATH += $$PWD/EVENTS/earthquake
INCLUDEPATH += $$PWD/EVENTS/earthquake/StochasticMotionInput/include
INCLUDEPATH += $$PWD/ANALYSIS
INCLUDEPATH += $$PWD/GRAPHICS
INCLUDEPATH += $$PWD/EDP


#INCLUDEPATH += "../SimCenterCommon/Workflow"
#INCLUDEPATH += "../QUO_Methods"

SOURCES += $$PWD/UQ/DakotaResults.cpp \
    $$PWD/UQ/GaussianProcessInputWidget.cpp \
    $$PWD/UQ/LatinHypercubeInputWidget.cpp \
    $$PWD/UQ/DakotaResultsSampling.cpp \
    $$PWD/UQ/DakotaResultsReliability.cpp \
    $$PWD/UQ/DakotaResultsSensitivity.cpp \
    $$PWD/UQ/ImportanceSamplingInputWidget.cpp \
    $$PWD/UQ/MonteCarloInputWidget.cpp \
    $$PWD/UQ/PCEInputWidget.cpp \
    $$PWD/UQ/UQ_MethodInputWidget.cpp \
    $$PWD/UQ/DakotaInputSampling.cpp \
    $$PWD/UQ/DakotaInputReliability.cpp \
    $$PWD/UQ/DakotaInputSensitivity.cpp \
    $$PWD/UQ/InputWidgetUQ.cpp \
    $$PWD/UQ/UQ_Results.cpp \
    $$PWD/UQ/UQ_Engine.cpp \
    $$PWD/UQ/DakotaEngine.cpp \
    $$PWD/UQ/FORMInputWidget.cpp \
    $$PWD/UQ/SORMInputWidget.cpp \
    $$PWD/UQ/UQ_EngineSelection.cpp \
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
    $$PWD/EVENTS/earthquake/EarthquakeEventSelection.cpp \
    $$PWD/EVENTS/earthquake/ExistingPEER_Events.cpp \
    $$PWD/EVENTS/earthquake/SHAMotionWidget.cpp \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/src/StochasticMotionInputWidget.cpp \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/src/StochasticModelWidget.cpp \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/src/VlachosEtAlModel.cpp \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/src/DabaghiDerKiureghianPulse.cpp \
    $$PWD/EVENTS/earthquake/peerNGA/PeerLoginDialog.cpp \
    $$PWD/EVENTS/earthquake/peerNGA/PeerNgaRecordsWidget.cpp \
    $$PWD/EVENTS/earthquake/peerNGA/RecordSelectionPlot.cpp \
    $$PWD/EVENTS/earthquake/peerNGA/PeerNgaWest2Client.cpp \
    $$PWD/EXECUTION/UserDefinedApplication.cpp \
    $$PWD/GRAPHICS/Controller2D.cpp \
    $$PWD/GRAPHICS/GlWidget2D.cpp \
    $$PWD/GRAPHICS/MyTableWidget.cpp \
    $$PWD/GRAPHICS/GraphicView2D.cpp \
    $$PWD/EDP/UserDefinedEDP.cpp \
    $$PWD/EDP/EDP.cpp

HEADERS += $$PWD/UQ/DakotaResults.h \
    $$PWD/UQ/GaussianProcessInputWidget.h \
    $$PWD/UQ/LatinHypercubeInputWidget.h \
    $$PWD/UQ/DakotaResultsSampling.h \
    $$PWD/UQ/DakotaResultsReliability.h \
    $$PWD/UQ/DakotaResultsSensitivity.h \
    $$PWD/UQ/DakotaInputReliability.h \
    $$PWD/UQ/DakotaInputSensitivity.h \
    $$PWD/UQ/ImportanceSamplingInputWidget.h \
    $$PWD/UQ/MonteCarloInputWidget.h \
    $$PWD/UQ/PCEInputWidget.h \
    $$PWD/UQ/UQ_MethodInputWidget.h \
    $$PWD/UQ/DakotaInputSampling.h \
    $$PWD/UQ/InputWidgetUQ.h \
    $$PWD/UQ/UQ_Results.h \
    $$PWD/UQ/UQ_Engine.h \
    $$PWD/UQ/DakotaEngine.h \
    $$PWD/UQ/FORMInputWidget.h \
    $$PWD/UQ/SORMInputWidget.h \
    $$PWD/UQ/UQ_EngineSelection.h \
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
    $$PWD/EVENTS/earthquake/EarthquakeEventSelection.h \
    $$PWD/EVENTS/earthquake/ExistingPEER_Events.h \
    $$PWD/EVENTS/earthquake/SHAMotionWidget.h \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/include/StochasticMotionInputWidget.h \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/include/StochasticModelWidget.h \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/include/VlachosEtAlModel.h \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/include/DabaghiDerKiureghianPulse.h \
    $$PWD/EVENTS/earthquake/peerNGA/PeerLoginDialog.h \
    $$PWD/EVENTS/earthquake/peerNGA/PeerNgaRecordsWidget.h \
    $$PWD/EVENTS/earthquake/peerNGA/RecordSelectionPlot.h \
    $$PWD/EVENTS/earthquake/peerNGA/PeerNgaWest2Client.h \
    $$PWD/GRAPHICS/Controller2D.h \
    $$PWD/GRAPHICS/GlWidget2D.h \
    $$PWD/GRAPHICS/MyTableWidget.h \
    $$PWD/GRAPHICS/GraphicView2D.h \
    $$PWD/EDP/UserDefinedEDP.h \
    $$PWD/EDP/EDP.h

