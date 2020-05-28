cmake_minimum_required(VERSION 3.5)

project(ices)

# set compilation
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_definitions(-Wall -Wextra -O2)

# useful macros
include(cmake/util.cmake)

# find dependencies
find_package(OpenCV REQUIRED)
find_package(Threads)

# common include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/HCISUPSDK/incCn
    ${OpenCV_INCLUDE_DIRS}
)

# find all thirdparty libraries
file(GLOB_RECURSE
    thirdparty_libs
    ${PROJECT_SOURCE_DIR}/thirdparty/HCISUPSDK/*.so
    ${PROJECT_SOURCE_DIR}/thirdparty/HCISUPSDK/*/*.so
    )

# find all source files
file(GLOB_RECURSE
    src_files
    ${PROJECT_SOURCE_DIR}/src/*.cc
    ${PROJECT_SOURCE_DIR}/src/*/*.cc
    )

# common library
add_library(
    ${PROJECT_NAME}
    SHARED
    ${src_files}
)
target_link_libraries(
    ${PROJECT_NAME}
    ${OpenCV_LIBS}
    ${thirdparty_libs}
    Threads::Threads
)

# tests
add_simple_test(test_demo)
