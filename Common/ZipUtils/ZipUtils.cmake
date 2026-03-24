# ZipUtils.cmake
# Usage:
#   include(path/to/ZipUtils.cmake)
#   simcenter_add_ziputils(<target>)

set(COMMON_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(simcenter_add_ziputils target)

  set(dir "${COMMON_MODULE_DIR}")

  target_include_directories(${target} PRIVATE
    "${dir}"
  )

  target_sources(${target} PRIVATE
    "${dir}/zip.h"
    "${dir}/unzip.h"
    "${dir}/ioapi.h"
    "${dir}/ZipUtils.h"

    "${dir}/zip.c"
    "${dir}/unzip.c"
    "${dir}/ioapi.c"
    "${dir}/ZipUtils.cpp"
  )

  # Link zlib (qmake did: -lz on linux/mac)
  find_package(ZLIB REQUIRED)

  # Most FindZLIB provide ZLIB::ZLIB
  if(TARGET ZLIB::ZLIB)
    target_link_libraries(${target} PRIVATE ZLIB::ZLIB)
  else()
    # BUT just in case ...
    target_link_libraries(${target} PRIVATE ${ZLIB_LIBRARIES})
    target_include_directories(${target} PRIVATE ${ZLIB_INCLUDE_DIRS})
  endif()
endfunction()

