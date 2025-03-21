cmake_minimum_required(VERSION 3.5)

project(ices)

# set compilation
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_definitions(-Wall -Wextra -g)

option(USE_EHOME "Use EHome 4.0 SDK instead of ISUP SDK" OFF)
if (USE_EHOME)
    message("Use EHome SDK")
    add_definitions(-DUSE_EHOME)
    set(SDK_NAME "HCEHOMESDK")
else (USE_EHOME)
    message("Use ISUP SDK")
    set(SDK_NAME "HCISUPSDK")
endif (USE_EHOME)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
    message("Build with Linux")
    set(OS_NAME "linux64")
else ()
    message("Unsupported system: ${CMAKE_HOST_SYSTEM_NAME}")
endif (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")

# useful macros
include(cmake/util.cmake)

# find dependencies
find_package(OpenCV REQUIRED)
find_package(Threads)

# common include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/HCPLAYM4SDK/${OS_NAME}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/${SDK_NAME}/${OS_NAME}/include
    ${OpenCV_INCLUDE_DIRS}
)

# find all thirdparty libraries
file(GLOB_RECURSE
    playm4_libs
    ${PROJECT_SOURCE_DIR}/thirdparty/HCPLAYM4SDK/${OS_NAME}/lib/*.so
    )
file(GLOB_RECURSE
    sdk_libs
    ${PROJECT_SOURCE_DIR}/thirdparty/${SDK_NAME}/${OS_NAME}/lib/*.so
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
    ${playm4_libs}
    ${sdk_libs}
    Threads::Threads
)

# installation
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib)

file(GLOB_RECURSE
    playm4_headers
    ${PROJECT_SOURCE_DIR}/thirdparty/HCPLAYM4SDK/${OS_NAME}/include/*.h
    )
install(FILES ${playm4_headers}
    DESTINATION include)
install(FILES ${playm4_libs}
    DESTINATION lib)

file(GLOB_RECURSE
    sdk_headers
    ${PROJECT_SOURCE_DIR}/thirdparty/${SDK_NAME}/${OS_NAME}/include/*.h
    )
install(FILES ${sdk_headers}
    DESTINATION include)
install(FILES ${sdk_libs}
    DESTINATION lib)

# tests
add_simple_test(test_ehome_server)
