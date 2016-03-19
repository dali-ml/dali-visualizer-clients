cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

project("Dali Visualizer")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(HiRedis REQUIRED)
find_package(Dali REQUIRED)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} --std=c++11 -O3 -g -fPIC" )

add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/redox)
add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/json11)

include_directories(${HIREDIS_INCLUDE_DIRS})
include_directories(${DALI_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/third_party/redox/include)
include_directories(${PROJECT_SOURCE_DIR}/third_party/redox/include/redox)
include_directories(${PROJECT_SOURCE_DIR}/third_party/sole)
include_directories(${PROJECT_SOURCE_DIR}/third_party/json11)


file(GLOB_RECURSE DaliVisualizerHeaders "${PROJECT_SOURCE_DIR}/dali_visualizer/*.h")
file(GLOB_RECURSE DaliVisualizerSources "${PROJECT_SOURCE_DIR}/dali_visualizer/*.cpp")
# sole has no build script - it is a sole lonely c++ file.
list(APPEND DaliVisualizerSources "${PROJECT_SOURCE_DIR}/third_party/sole/sole.cpp")

include_directories(${PROJECT_SOURCE_DIR})

add_library(dali_visualizer SHARED ${DaliVisualizerSources} ${DaliVisualizerHeaders})
target_link_libraries(dali_visualizer json11static ${DALI_LIBRARIES} redox_static ${HIREDIS_LIBRARIES})

INSTALL(TARGETS dali_visualizer DESTINATION lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/dali_visualizer  DESTINATION include
        FILES_MATCHING PATTERN "*.h")
