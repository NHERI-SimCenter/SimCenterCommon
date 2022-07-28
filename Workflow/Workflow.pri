#-------------------------------------------------
#
# Project created by fmk
#
#-------------------------------------------------

#message($$_PRO_FILE_PWD_)
#message($$PWD)

INCLUDEPATH += $$PWD/UQ
INCLUDEPATH += $$PWD/UQ/common
INCLUDEPATH += $$PWD/UQ/dakota
INCLUDEPATH += $$PWD/UQ/simcenterUQ
INCLUDEPATH += $$PWD/UQ/customUQ
INCLUDEPATH += $$PWD/UQ/ucsd
INCLUDEPATH += $$PWD/WORKFLOW
INCLUDEPATH += $$PWD/WORKFLOW/Utils
INCLUDEPATH += $$PWD/WORKFLOW/ModelViewItems
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

SOURCES += $$PWD/UQ/UQ_EngineSelection.cpp \
    $$PWD/UQ/UQ_Results.cpp \
    $$PWD/UQ/UQ_Engine.cpp \
    $$PWD/UQ/UQ_Method.cpp \
    $$PWD/UQ/simcenterUQ/PLoMSimuWidget.cpp \
    $$PWD/UQ/UQpyEngine.cpp \
    $$PWD/UQ/customUQ/CustomUQ_Results.cpp \
    $$PWD/UQ/customUQ/UQ_JsonEngine.cpp \ 
    $$PWD/UQ/common/ResultsDataChart.cpp \
    $$PWD/UQ/dakota/DakotaEngine.cpp \
    $$PWD/UQ/dakota/DakotaInputSampling.cpp \
    $$PWD/UQ/dakota/DakotaResultsSampling.cpp \
    $$PWD/UQ/dakota/MonteCarloInputWidget.cpp \
    $$PWD/UQ/dakota/MultiFidelityMonteCarlo.cpp \
    $$PWD/UQ/dakota/LatinHypercubeInputWidget.cpp \
    $$PWD/UQ/dakota/ImportanceSamplingInputWidget.cpp \
    $$PWD/UQ/dakota/GaussianProcessInputWidget.cpp \
    $$PWD/UQ/dakota/PCEInputWidget.cpp \
    $$PWD/UQ/dakota/DakotaInputBayesianCalibration.cpp \
    $$PWD/UQ/dakota/DakotaResultsBayesianCalibration.cpp \
    $$PWD/UQ/dakota/DakotaResultsCalibration.cpp \
    $$PWD/UQ/dakota/DakotaInputCalibration.cpp \
    $$PWD/UQ/dakota/DakotaInputSensitivity.cpp \
    $$PWD/UQ/dakota/DakotaResultsSensitivity.cpp \
    $$PWD/UQ/dakota/DakotaInputReliability.cpp \
    $$PWD/UQ/dakota/DakotaResultsReliability.cpp \
    $$PWD/UQ/dakota/LocalReliabilityWidget.cpp \
    $$PWD/UQ/dakota/GlobalReliabilityWidget.cpp \
    $$PWD/UQ/ucsd/UCSD_Engine.cpp \
    $$PWD/UQ/ucsd/UCSD_TMMC.cpp \
    $$PWD/UQ/ucsd/UCSD_Results.cpp \
    $$PWD/UQ/ucsd/BayesPlots.cpp \
    $$PWD/UQ/simcenterUQ/SimCenterUQEngine.cpp \
    $$PWD/UQ/simcenterUQ/SimCenterUQInputSampling.cpp \
    $$PWD/UQ/simcenterUQ/SimCenterUQResultsSampling.cpp \
    $$PWD/UQ/simcenterUQ/SimCenterUQInputSensitivity.cpp \
    $$PWD/UQ/simcenterUQ/SimCenterUQResultsSensitivity.cpp \
    $$PWD/UQ/simcenterUQ/SimCenterUQInputSurrogate.cpp \
    $$PWD/UQ/simcenterUQ/SimCenterUQResultsSurrogate.cpp \
    $$PWD/UQ/simcenterUQ/SurrogateNoDoEInputWidget.cpp \
    $$PWD/UQ/simcenterUQ/SurrogateDoEInputWidget.cpp \
    $$PWD/UQ/simcenterUQ/SurrogateMFInputWidget.cpp \
    $$PWD/UQ/simcenterUQ/PLoMInputWidget.cpp \
    $$PWD/UQ/simcenterUQ/SimCenterUQInputPLoM.cpp \
    $$PWD/UQ/simcenterUQ/SimCenterUQResultsPLoM.cpp \
    $$PWD/WORKFLOW/MainWindowWorkflowApp.cpp \
    $$PWD/WORKFLOW/WorkflowAppWidget.cpp \
    $$PWD/WORKFLOW/SimCenterComponentSelection.cpp \
    $$PWD/WORKFLOW/CustomizedItemModel.cpp \
    $$PWD/WORKFLOW/JsonConfiguredWidget.cpp \
    $$PWD/WORKFLOW/SimCenterComboBox.cpp \
    $$PWD/WORKFLOW/SimCenterFileInput.cpp \
    $$PWD/WORKFLOW/SimCenterRVLineEdit.cpp \
    $$PWD/WORKFLOW/SimCenterSpinBox.cpp \
    $$PWD/WORKFLOW/SimCenterDoubleSpinBox.cpp \
    $$PWD/WORKFLOW/SimCenterLineEdit.cpp \
    $$PWD/WORKFLOW/Utils/ExampleDownloader.cpp \
    $$PWD/WORKFLOW/Utils/NetworkDownloadManager.cpp \
    $$PWD/WORKFLOW/ModelViewItems/CheckableTreeModel.cpp \
    $$PWD/WORKFLOW/ModelViewItems/SimCenterTreeView.cpp \
    $$PWD/WORKFLOW/ModelViewItems/TreeItem.cpp \
    $$PWD/WORKFLOW/ModelViewItems/TreeViewStyle.cpp \
    $$PWD/WORKFLOW/ModelViewItems/LayerTreeModel.cpp \
    $$PWD/WORKFLOW/ModelViewItems/LayerTreeItem.cpp \    
    $$PWD/SIM/OpenSeesBuildingModel.cpp \
    $$PWD/SIM/MDOF_BuildingModel.cpp \
    $$PWD/SIM/SteelBuildingModel.cpp \
    $$PWD/SIM/ConcreteBuildingModel.cpp \
    $$PWD/SIM/SIM_Selection.cpp \
    $$PWD/SIM/InputWidgetBIM.cpp \
    $$PWD/SIM/OpenSeesParser.cpp \
    $$PWD/ANALYSIS/FEA_Selection.cpp \
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
    $$PWD/EVENTS/SimCenterUnitsCombo.cpp \
    $$PWD/EVENTS/SimCenterIntensityMeasureCombo.cpp \
    $$PWD/EVENTS/SimCenterIntensityMeasureWidget.cpp \
    $$PWD/EXECUTION/UserDefinedApplication.cpp \
    $$PWD/GRAPHICS/Controller2D.cpp \
    $$PWD/GRAPHICS/GlWidget2D.cpp \
    $$PWD/GRAPHICS/MyTableWidget.cpp \
    $$PWD/GRAPHICS/GraphicView2D.cpp \
    $$PWD/GRAPHICS/SimCenterGraphPlot.cpp \
    $$PWD/GRAPHICS/qcustomplot.cpp \
    $$PWD/EDP/EDP_EarthquakeSelection.cpp \
    $$PWD/EDP/StandardEarthquakeEDP.cpp \
    $$PWD/EDP/EDP_Selection.cpp \
    $$PWD/EDP/StandardEDP.cpp \    
    $$PWD/EDP/UserDefinedEDP.cpp \
    $$PWD/EDP/EDP.cpp

HEADERS += $$PWD/UQ/UQ_EngineSelection.h \
    $$PWD/UQ/UQ_Results.h \
    $$PWD/UQ/UQ_Engine.h \
    $$PWD/UQ/UQ_Method.h \
    $$PWD/UQ/UQpyEngine.h \
    $$PWD/UQ/customUQ/CustomUQ_Results.h \
    $$PWD/UQ/customUQ/UQ_JsonEngine.h \
    $$PWD/UQ/common/ResultsDataChart.h \
    $$PWD/UQ/dakota/DakotaEngine.h \
    $$PWD/UQ/dakota/DakotaInputSampling.h \
    $$PWD/UQ/dakota/DakotaResultsSampling.h \
    $$PWD/UQ/dakota/MonteCarloInputWidget.h \
    $$PWD/UQ/dakota/MultiFidelityMonteCarlo.h \
    $$PWD/UQ/dakota/LatinHypercubeInputWidget.h \
    $$PWD/UQ/dakota/ImportanceSamplingInputWidget.h \
    $$PWD/UQ/dakota/GaussianProcessInputWidget.h \
    $$PWD/UQ/dakota/PCEInputWidget.h \
    $$PWD/UQ/dakota/DakotaInputBayesianCalibration.h \
    $$PWD/UQ/dakota/DakotaResultsBayesianCalibration.h \
    $$PWD/UQ/dakota/DakotaResultsCalibration.h \
    $$PWD/UQ/dakota/DakotaInputCalibration.h \
    $$PWD/UQ/dakota/DakotaInputSensitivity.h \
    $$PWD/UQ/dakota/DakotaResultsSensitivity.h \
    $$PWD/UQ/dakota/DakotaInputReliability.h \
    $$PWD/UQ/dakota/DakotaResultsReliability.h \
    $$PWD/UQ/dakota/LocalReliabilityWidget.h \
    $$PWD/UQ/dakota/GlobalReliabilityWidget.h \
    $$PWD/UQ/simcenterUQ/PLoMSimuWidget.h \
    $$PWD/UQ/ucsd/UCSD_Engine.h \
    $$PWD/UQ/ucsd/UCSD_TMMC.h \
    $$PWD/UQ/ucsd/UCSD_Results.h \
    $$PWD/UQ/ucsd/BayesPlots.h \
    $$PWD/UQ/simcenterUQ/SimCenterUQEngine.h \
    $$PWD/UQ/simcenterUQ/SimCenterUQInputSampling.h \
    $$PWD/UQ/simcenterUQ/SimCenterUQResultsSampling.h \
    $$PWD/UQ/simcenterUQ/SimCenterUQInputSensitivity.h \
    $$PWD/UQ/simcenterUQ/SimCenterUQResultsSensitivity.h \
    $$PWD/UQ/simcenterUQ/SimCenterUQInputSurrogate.h \
    $$PWD/UQ/simcenterUQ/SimCenterUQResultsSurrogate.h \
    $$PWD/UQ/simcenterUQ/SurrogateNoDoEInputWidget.h \
    $$PWD/UQ/simcenterUQ/SurrogateDoEInputWidget.h \
    $$PWD/UQ/simcenterUQ/SurrogateMFInputWidget.h \
    $$PWD/UQ/simcenterUQ/PLoMInputWidget.h \
    $$PWD/UQ/simcenterUQ/SimCenterUQInputPLoM.h \
    $$PWD/UQ/simcenterUQ/SimCenterUQResultsPLoM.h \
    $$PWD/WORKFLOW/MainWindowWorkflowApp.h \
    $$PWD/WORKFLOW/WorkflowAppWidget.h \
    $$PWD/WORKFLOW/SimCenterComponentSelection.h \
    $$PWD/WORKFLOW/CustomizedItemModel.h \
    $$PWD/WORKFLOW/JsonConfiguredWidget.h \
    $$PWD/WORKFLOW/SimCenterComboBox.h \
    $$PWD/WORKFLOW/SimCenterFileInput.h \
    $$PWD/WORKFLOW/SimCenterRVLineEdit.h \
    $$PWD/WORKFLOW/SimCenterSpinBox.h \
    $$PWD/WORKFLOW/SimCenterDoubleSpinBox.h \
    $$PWD/WORKFLOW/SimCenterLineEdit.h \
    $$PWD/WORKFLOW/Utils/ExampleDownloader.h \
    $$PWD/WORKFLOW/Utils/NetworkDownloadManager.h \
    $$PWD/WORKFLOW/ModelViewItems/CheckableTreeModel.h \
    $$PWD/WORKFLOW/ModelViewItems/SimCenterTreeView.h \
    $$PWD/WORKFLOW/ModelViewItems/TreeItem.h \
    $$PWD/WORKFLOW/ModelViewItems/TreeViewStyle.h \ 
    $$PWD/WORKFLOW/ModelViewItems/LayerTreeModel.h \
    $$PWD/WORKFLOW/ModelViewItems/LayerTreeItem.h \    
    $$PWD/SIM/OpenSeesBuildingModel.h \
    $$PWD/SIM/MDOF_BuildingModel.h \
    $$PWD/SIM/ReadWriteRVJSON.h \
    $$PWD/SIM/SteelBuildingModel.h \
    $$PWD/SIM/ConcreteBuildingModel.h \
    $$PWD/SIM/SIM_Selection.h \
    $$PWD/SIM/InputWidgetBIM.h \
    $$PWD/SIM/OpenSeesParser.h \
    $$PWD/ANALYSIS/FEA_Selection.h \
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
    $$PWD/EVENTS/SimCenterIntensityMeasureWidget.h \
    $$PWD/EVENTS/SimCenterIntensityMeasureCombo.h \
    $$PWD/EVENTS/SimCenterUnitsCombo.h \
    $$PWD/GRAPHICS/Controller2D.h \
    $$PWD/GRAPHICS/GlWidget2D.h \
    $$PWD/GRAPHICS/MyTableWidget.h \
    $$PWD/GRAPHICS/GraphicView2D.h \
    $$PWD/GRAPHICS/SimCenterGraphPlot.h \
    $$PWD/GRAPHICS/qcustomplot.h \
    $$PWD/EDP/EDP_EarthquakeSelection.h \
    $$PWD/EDP/StandardEarthquakeEDP.h \
    $$PWD/EDP/UserDefinedEDP.h \
    $$PWD/EDP/EDP_Selection.h \
    $$PWD/EDP/StandardEDP.h \    
    $$PWD/EDP/EDP.h

