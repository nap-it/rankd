cmake_minimum_required(VERSION 3.16)

project(rankd
        VERSION 0.1.0
        DESCRIPTION "Implementation of a service to retrieve information on a machine where a server instance is being ran."
        LANGUAGES CXX C)

message("-> Initiating CMake for cmd/rank/cmd/rankd project")

# Check if this project is being called on from Simuzilla's integration.
if(FROM_SIMUZILLA)
    message("-- Preparing cmake project with considerations for Simuzilla.")
    add_subdirectory(internal)
    return()
endif()

set(EXTERNALLY_SOURCED OFF)

include(ExternalProject)

# Define build type. If none is specified, then DEBUG is chosen.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
    message(STATUS "Build type not specified: Use Debug by default")
endif()

## Load pkg-config.
find_package(PkgConfig REQUIRED)

## Load vendor packages.
set(GLOBAL_VENDOR_DIRS "${PROJECT_SOURCE_DIR}/vendor")

add_subdirectory(internal)
add_subdirectory(cmd)
add_subdirectory(tests)

set(EXTERNALLY_SOURCED ON)