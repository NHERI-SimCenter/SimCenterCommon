# Common.cmake
# Usage:
#   include(path/to/Common.cmake)
#   simcenter_add_common(<target>)

set(COMMON_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(simcenter_add_inputsheet target)
  set(dir "${COMMON_MODULE_DIR}")


  target_include_directories(${target} PRIVATE
    "${dir}"
  )

  # --- SOURCES ---
 set(SOURCES
    "${dir}/GeneralInformationWidget.cpp"
 )

 set(HEADERS
    "${dir}/GeneralInformationWidget.h"
 )

 target_sources(${target} PRIVATE ${SOURCES} ${HEADERS})

endfunction()
    

