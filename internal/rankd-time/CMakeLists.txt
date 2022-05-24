project(rankd-time
        VERSION 0.1.0
        DESCRIPTION "Time-related Rank daemon library"
        LANGUAGES CXX C)

set(HEADER_LIST
        include/relyum/services.h
        include/relyum/snmp.h
        include/relyum/structs.h
        include/relyum/timesync.h
        include/time-lib.h
        )
set(SOURCE_LIST
        relyum/structs.cpp
        relyum/snmp.cpp
        relyum/timesync.cpp
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

# Make the rankd-time library (librankd-net).
add_library(rankd-time SHARED lib.cpp ${HEADER_LIST} ${SOURCE_LIST})
target_include_directories(rankd-time PUBLIC include vendor/ptp)
target_link_libraries(rankd-time netsnmp linuxptp)

# All users to compile this library need, at least, C++20.
target_compile_features(rankd-time PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place.
source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})
