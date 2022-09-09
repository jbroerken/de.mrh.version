#########################################################################
#
#  CMAKE
#
#########################################################################

###
#  Minimum Version
#  ---------------
#  The CMake version required.
###
cmake_minimum_required(VERSION 3.1)

###
#  CMake Configuration
#  -------------------
#  Configuration settings for CMake.
#
#  NOTE:
#  These settings have to be applied before the project() setting!
###
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD 14)

###
#  Project Info
#  ------------
#  General simple information about our project.
###
project(de.mrh.version VERSION 1.0.2
                       DESCRIPTION "MRH version info application"
                       LANGUAGES CXX)

#########################################################################
#
#  PATHS
#
#########################################################################

###
#  Binary Paths
#  ------------
#  The paths for our created binary file(s).
###
set(BIN_DIR_PATH "${CMAKE_SOURCE_DIR}/bin/")
file(MAKE_DIRECTORY ${BIN_DIR_PATH})

###
#  Build Paths
#  -----------
#  The paths for the cmake build.
###
set(BUILD_DIR_PATH "${CMAKE_SOURCE_DIR}/build/")
file(MAKE_DIRECTORY ${BUILD_DIR_PATH})

###
#  Source Paths
#  ------------
#  The paths to the source files to use.
#  Add OS specific source files in their own list.
###
set(SRC_DIR_PATH "${CMAKE_SOURCE_DIR}/src/")
             
set(SRC_LIST_APP "${SRC_DIR_PATH}/Revision.h"
                 "${SRC_DIR_PATH}/Main.cpp")
                 
set(SRC_LIST_MODULE "${SRC_DIR_PATH}/Module/Version/Version.cpp"
                    "${SRC_DIR_PATH}/Module/Version/Version.h")

#########################################################################
#
#  TARGET
#
#########################################################################

###
#  Target
#  ------
#  The target(s) to build.
#
#  NOTE: 
#  MRH applications are required to be named App.so, 
#  and application services Service.so!
#  They are build as shared objects.
###
add_library(MRH_App SHARED ${SRC_LIST_APP}
                           ${SRC_LIST_MODULE})
set_target_properties(MRH_App
                      PROPERTIES
                      PREFIX ""
                      OUTPUT_NAME "App"
                      SUFFIX ".so"
                      ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR_PATH}
                      LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR_PATH}
                      RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR_PATH})

###
#  Required Libraries
#  ------------------
#  Libraries required by this application.
###
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Threads REQUIRED)
find_library(libmrh NAMES mrh REQUIRED)
find_library(libmrhbf NAMES mrhbf REQUIRED)
find_library(libmrhevdata NAMES mrhevdata REQUIRED)
find_library(libmrhab NAMES mrhab REQUIRED)

target_link_libraries(MRH_App PUBLIC Threads::Threads)
target_link_libraries(MRH_App PUBLIC mrh)
target_link_libraries(MRH_App PUBLIC mrhbf)
target_link_libraries(MRH_App PUBLIC mrhevdata)
target_link_libraries(MRH_App PUBLIC mrhab)

###
#  Source Definitions
#  ------------------
#  Preprocessor source definitions.
###
target_compile_definitions(MRH_App PRIVATE MRH_VERSION_FILE_PATH="/usr/local/etc/mrh/MRH_Version.conf")