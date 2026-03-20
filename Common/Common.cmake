# Common.cmake
# Usage:
#   include(path/to/Common.cmake)
#   simcenter_add_common(<target>)

set(COMMON_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(simcenter_add_common target)
  set(dir "${COMMON_MODULE_DIR}")

  target_include_directories(${target} PRIVATE
    "${dir}"
    "${dir}/../SimCenterCommon/Common"
    "${dir}/../Common"
    "${dir}/../Utils"
  )

  set(SOURCES
    "${dir}/HeaderWidget.cpp"
    "${dir}/SC_MovieWidget.cpp"
    "${dir}/Utils/JsonFunctions.cpp"
    "${dir}/Utils/RelativePathResolver.cpp"
    "${dir}/Utils/FileOperations.cpp"
    "${dir}/Utils/SimCenterConfigFile.cpp"
    "${dir}/Utils/dialogabout.cpp"
    "${dir}/Utils/ProgramOutputDialog.cpp"
    "${dir}/Utils/PythonProcessHandler.cpp"
    "${dir}/SectionTitle.cpp"
    "${dir}/FooterWidget.cpp"
    "${dir}/SimCenterWidget.cpp"
    "${dir}/SC_ComboBox.cpp"
    "${dir}/SC_AssetInputDelegate.cpp"
    "${dir}/SC_CheckBox.cpp"
    "${dir}/SC_DoubleLineEdit.cpp"
    "${dir}/SC_StringLineEdit.cpp"
    "${dir}/SC_IntLineEdit.cpp"
    "${dir}/SC_QRadioButton.cpp"
    "${dir}/SC_TableEdit.cpp"
    "${dir}/SC_FileEdit.cpp"
    "${dir}/SC_DirEdit.cpp"
    "${dir}/SC_ResultsWidget.cpp"
    "${dir}/SC_MultipleLineChart.cpp"
    "${dir}/SC_Chart.cpp"
    "${dir}/SC_TimeSeriesResultChart.cpp"
    "${dir}/SimCenterAppWidget.cpp"
    "${dir}/NoArgSimCenterApp.cpp"
    "${dir}/SimCenterAppSelection.cpp"
    "${dir}/SimCenterEventAppSelection.cpp"
    "${dir}/SimCenterAppEventSelection.cpp"
    "${dir}/SimCenterAppMulti.cpp"
    "${dir}/SimCenterPreferences.cpp"
    "${dir}/SimCenterDirWatcher.cpp"
    "${dir}/NoneWidget.cpp"
    "${dir}/GoogleAnalytics.cpp"
  )
  
  set(HEADERS
    "${dir}/HeaderWidget.h"
    "${dir}/SC_MovieWidget.h"
    "${dir}/Utils/RelativePathResolver.h"
    "${dir}/Utils/FileOperations.h"
    "${dir}/Utils/JsonFunctions.h"
    "${dir}/Utils/dialogabout.h"
    "${dir}/Utils/ProgramOutputDialog.h"
    "${dir}/Utils/SimCenterConfigFile.h"
    "${dir}/Utils/PythonProcessHandler.h"
    "${dir}/SectionTitle.h"
    "${dir}/FooterWidget.h"
    "${dir}/SimCenterWidget.h"
    "${dir}/SC_ComboBox.h"
    "${dir}/SC_AssetInputDelegate.h"
    "${dir}/SC_CheckBox.h"
    "${dir}/SC_DoubleLineEdit.h"
    "${dir}/SC_StringLineEdit.h"
    "${dir}/SC_IntLineEdit.h"
    "${dir}/SC_QRadioButton.h"
    "${dir}/SC_TableEdit.h"
    "${dir}/SC_FileEdit.h"
    "${dir}/SC_DirEdit.h"
    "${dir}/SC_ResultsWidget.h"
    "${dir}/SC_MultipleLineChart.h"
    "${dir}/SC_Chart.h"
    "${dir}/SC_TimeSeriesResultChart.h"
    "${dir}/NoArgSimCenterApp.h"
    "${dir}/SimCenterAppSelection.h"
    "${dir}/SimCenterAppMulti.h"
    "${dir}/SimCenterEventAppSelection.h"
    "${dir}/SimCenterAppEventSelection.h"
    "${dir}/SimCenterAppWidget.h"
    "${dir}/SimCenterPreferences.h"
    "${dir}/SimCenterDirWatcher.h"
    "${dir}/NoneWidget.h"
    "${dir}/GoogleAnalytics.h"
  )

  target_sources(${target} PRIVATE
     ${SOURCES}
     ${HEADERS}
     "${dir}/Utils/dialogabout.ui"
     "${dir}/images1.qrc"
  )


  # Ensure AUTOUIC/AUTORCC/AUTOMOC are ON for the target using this module
  # set_property(TARGET ${target} PROPERTY AUTOMOC ON)
  # set_property(TARGET ${target} PROPERTY AUTOUIC ON)
  # set_property(TARGET ${target} PROPERTY AUTORCC ON)

  # ZipUtils.pri equivalent
  include("${dir}/ZipUtils/ZipUtils.cmake")
  simcenter_add_ziputils(${target})
  
endfunction()
