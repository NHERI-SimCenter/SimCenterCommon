# SimCenterQGIS.cmake
# Usage:
#   include(path/to/SimCenterQGIS.cmake)
#   simcenter_add_qgis(<target>)

set(SC_QGIS_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(QGIS_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../QGIS")

function(simcenter_add_qgis target)

  set(dir "${SC_QGIS_MODULE_DIR}")
  
  target_include_directories(${target} PRIVATE
    "${dir}"
    "${QGIS_SOURCE_DIR}/external/nmea"
    "${QGIS_SOURCE_DIR}/src/core"
    "${QGIS_SOURCE_DIR}/src/core/geometry"
    "${QGIS_SOURCE_DIR}/src/core/vector"
    "${QGIS_SOURCE_DIR}/src/core/elevation"
    "${QGIS_SOURCE_DIR}/src/core/proj"
    "${QGIS_SOURCE_DIR}/src/core/expression"
    "${QGIS_SOURCE_DIR}/src/core/editform"
    "${QGIS_SOURCE_DIR}/src/core/project"
    "${QGIS_SOURCE_DIR}/src/core/providers"
    "${QGIS_SOURCE_DIR}/src/core/processing"
    "${QGIS_SOURCE_DIR}/src/core/network"
    "${QGIS_SOURCE_DIR}/src/core/metadata"
    "${QGIS_SOURCE_DIR}/src/core/maptools"
    "${QGIS_SOURCE_DIR}/src/core/settings"
    "${QGIS_SOURCE_DIR}/src/core/symbology"
    "${QGIS_SOURCE_DIR}/src/core/sensor"
    "${QGIS_SOURCE_DIR}/src/core/labeling"
    "${QGIS_SOURCE_DIR}/src/core/textrenderer"
    "${QGIS_SOURCE_DIR}/src/core/maprenderer"
    "${QGIS_SOURCE_DIR}/src/core/layertree"
    "${QGIS_SOURCE_DIR}/src/core/layout"    
    "${QGIS_SOURCE_DIR}/src/core/auth"
    "${QGIS_SOURCE_DIR}/src/core/raster"
    "${QGIS_SOURCE_DIR}/src/core/mesh"
    "${QGIS_SOURCE_DIR}/src/core/classification"
    "${QGIS_SOURCE_DIR}/src/ui"
    "${QGIS_SOURCE_DIR}/src/app"
    "${QGIS_SOURCE_DIR}/src/app/decorations"
    "${QGIS_SOURCE_DIR}/src/app/devtools"
    "${QGIS_SOURCE_DIR}/src/app/georeferencer"
    "${QGIS_SOURCE_DIR}/src/app/labeling"
    "${QGIS_SOURCE_DIR}/src/app/layout"
    "${QGIS_SOURCE_DIR}/src/app/pluginmanager"
    "${QGIS_SOURCE_DIR}/src/app/gps"
    "${QGIS_SOURCE_DIR}/src/app/sensor"
    "${QGIS_SOURCE_DIR}/src/app/dwg"
    "${QGIS_SOURCE_DIR}/src/app/maptools"
    "${QGIS_SOURCE_DIR}/src/app/mesh"
    "${QGIS_SOURCE_DIR}/src/app/locator"
    "${QGIS_SOURCE_DIR}/src/app/options"
    "${QGIS_SOURCE_DIR}/src/app/pointcloud"
    "${QGIS_SOURCE_DIR}/src/app/settings"
    "${QGIS_SOURCE_DIR}/src/app/tiledscene"
    "${QGIS_SOURCE_DIR}/src/app/vectortile"
    "${QGIS_SOURCE_DIR}/src/gui"
    "${QGIS_SOURCE_DIR}/src/gui/actions"
    "${QGIS_SOURCE_DIR}/src/gui/layertree"    
    "${QGIS_SOURCE_DIR}/src/gui/maptools"
    "${QGIS_SOURCE_DIR}/src/gui/codeeditors"
    "${QGIS_SOURCE_DIR}/src/gui/proj"
    "${QGIS_SOURCE_DIR}/src/gui/ogr"    
    "${QGIS_SOURCE_DIR}/src/gui/attributetable"
    "${QGIS_SOURCE_DIR}/src/plugins"
    "${QGIS_SOURCE_DIR}/src/native"
    "${QGIS_SOURCE_DIR}/build/src/core"
    "${QGIS_SOURCE_DIR}/build/src/gui"
    "${QGIS_SOURCE_DIR}/build/src/app"    
    "${QGIS_SOURCE_DIR}/build/src/ui"
    "${QGIS_SOURCE_DIR}/build"
    "${QGIS_SOURCE_DIR}/build/vcpkg_installed/arm64-osx/include"
  )

  set(SOURCES
        "${dir}/QGISVisualizationWidget.cpp"
        "${dir}/VisualizationWidget.cpp"
        "${dir}/GISLegendView.cpp"
        "${dir}/PopUpWidget.cpp"
        "${dir}/SimCenterMapcanvasWidget.cpp"
        "${dir}/GISSelectable.cpp"
  )
  
  set(HEADERS
        "${dir}/QGISVisualizationWidget.h"
        "${dir}/VisualizationWidget.h"
        "${dir}/GISLegendView.h"
        "${dir}/PopUpWidget.h"
        "${dir}/SimCenterMapcanvasWidget.h"
        "${dir}/GISSelectable.h"
  )

  target_sources(${target} PRIVATE
     ${SOURCES}
     ${HEADERS}
  )

endfunction()


