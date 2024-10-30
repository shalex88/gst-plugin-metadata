include(cmake/SetVersion.cmake)
include(cmake/EnableVcpkg.cmake)
cmake_minimum_required(VERSION 3.22)
include(cmake/SetVersion.cmake)
project(gstmetadata LANGUAGES CXX VERSION ${VERSION})

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_DIR "source")
set(INCLUDE_DIR "include")

include_directories(${INCLUDE_DIR} ${SOURCE_DIR})
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "${SOURCE_DIR}/*.cpp")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_check_modules(GSTREAMER-BASE REQUIRED IMPORTED_TARGET gstreamer-base-1.0)

function(link_common_libraries target)
    target_link_libraries(${target}
        PRIVATE
        #ADD your libs here
        PkgConfig::GSTREAMER
        PkgConfig::GSTREAMER-BASE
    )
endfunction(link_common_libraries)

link_common_libraries(${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME} DESTINATION ${GSTREAMER_LIBRARY_DIRS}/gstreamer-1.0/)
include(cmake/CreatePackage.cmake)
