cmake_minimum_required(VERSION 3.16)

project(rankd
        VERSION 0.1.0
        DESCRIPTION "Implementation of a service to retrieve information on a machine where a server instance is being ran."
        LANGUAGES CXX C)

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

## Set API protobuf directory.
set(API_PROTO_DIR "${PROJECT_SOURCE_DIR}/api/proto")
add_subdirectory(api)

add_subdirectory(internal)
add_subdirectory(cmd)
add_subdirectory(tests)