# Find the Dali library.
#
# This module defines
#  DALI_FOUND                - True if Dali was found.
#  DALI_INCLUDE_DIRS         - Include directories for Dali headers.
#  DALI_LIBRARIES            - Libraries for Dali.
#

set(DALI_FOUND FALSE)
set(DALI_INCLUDE_DIRS)
set(DALI_LIBRARIES)


if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColorReset "${Esc}[m")
  set(Red         "${Esc}[31m")
endif(NOT WIN32)

# find zlib
find_package(ZLIB REQUIRED)
list(APPEND DALI_LIBRARIES ${ZLIB_LIBRARIES})

# find cuda
find_package(CUDA)
if(CUDA_FOUND STREQUAL TRUE)
    list(APPEND DALI_INCLUDE_DIRS ${CUDA_INCLUDE_DIRS})
endif(CUDA_FOUND STREQUAL TRUE)

find_library(DALI_CORE_LIBRARIES dali)
if(DALI_CORE_LIBRARIES)
    list(APPEND DALI_LIBRARIES ${DALI_CORE_LIBRARIES})
    message("-- Found Dali: " ${DALI_CORE_LIBRARIES})
    set(DALI_FOUND TRUE)
else(DALI_CORE_LIBRARIES)
    if(Dali_FIND_QUIETLY)
        message(STATUS "Failed to find Dali   " ${REASON_MSG} ${ARGN})
    elseif(Dali_FIND_REQUIRED)
        message(FATAL_ERROR "${Red} Failed to find Dali   ${ColorReset}" ${REASON_MSG} ${ARGN})
    else()
        # Neither QUIETLY nor REQUIRED, use no priority which emits a message
        # but continues configuration and allows generation.
        message("-- Failed to find Dali   " ${REASON_MSG} ${ARGN})
    endif()
endif(DALI_CORE_LIBRARIES)

if(DALI_FOUND)
    MARK_AS_ADVANCED(DALI_INCLUDE_DIRS DALI_LIBRARIES)
endif(DALI_FOUND)
