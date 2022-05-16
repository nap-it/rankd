project(rankd-time
        VERSION 0.1.0
        DESCRIPTION "Time-related Rank daemon library"
        LANGUAGES CXX C)

set(HEADER_LIST
        include/relyum/snmp.h
        include/relyum/hardware-structs.h
        include/time-lib.h
        )
set(SOURCE_LIST
        relyum/hardware-structs.cpp
        relyum/snmp.cpp
        )

# Make the rankd-time library (librankd-net).
add_library(rankd-time lib.cpp ${HEADER_LIST} ${SOURCE_LIST})
target_include_directories(rankd-time PUBLIC include)
target_link_libraries(rankd-time PRIVATE netsnmp)

# All users to compile this library need, at least, C++20.
target_compile_features(rankd-time PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place.
source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})
