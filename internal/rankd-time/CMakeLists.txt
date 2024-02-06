project(rankd-time
        VERSION 0.1.0
        DESCRIPTION "Time-related Rank daemon library"
        LANGUAGES CXX C)

set(HEADER_LIST
        include/data-sources/configuration/configuration.h
        include/data-sources/latency/latency.h
        include/data-sources/latency/standards.h
        include/data-sources/reliability/reliability.h
        include/data-sources/synchronization/synchronization.h
        include/standards/ieee.h
        include/utils/kernel.h
        )
set(SOURCE_LIST
        data-sources/configuration/configuration.cpp
        data-sources/latency/latency.cpp
        data-sources/reliability/reliability.cpp
        data-sources/synchronization/synchronization.cpp
        utils/kernel.cpp
        )

add_subdirectory(vendor)

# Read options to enable features.
if (RELYUM)
    set(_RELYUM_BOARD ON)
    add_definitions(-D_RELYUM_BOARD)
    message("-- rankd-time is being compiled as a Relyum-targeted project")
endif()
if (SNMP)
    set(_SNMP_REQUEST_INFO ON)
    add_definitions(-D_SNMP_REQUEST_INFO)
    message("-- rankd-time library is being executed with SNMP version ON")
endif()
if (EXCELFORE_GPTP)
    set(_EXCELFORE_GPTP ON)
    add_definitions(-D_EXCELFORE_GPTP)
    message("-- rankd-time library is being prepared to lead with Excelfore's gPTP version")
endif()

# Make the rankd-time library (librankd-net).
set(RANKD_TIME include)
add_library(rankd-time SHARED lib.cpp ${HEADER_LIST} ${SOURCE_LIST})
target_include_directories(rankd-time PUBLIC include vendor/ptp vendor/rapidjson)
target_link_libraries(rankd-time pmc)
if (EXCELFORE_GPTP)
    target_link_libraries(rankd-time x4gptp2)
endif()

# All users to compile this library need, at least, C++20.
target_compile_features(rankd-time PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place.
source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})
