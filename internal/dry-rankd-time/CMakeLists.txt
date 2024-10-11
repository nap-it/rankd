project(dry-rankd-time
        VERSION 0.1.0
        DESCRIPTION "Dry-run time Rank daemon library for Simuzilla"
        LANGUAGES CXX C)

set(HEADER_LIST
        include/dry-time-lib.h
        include/data-sources/configuration/configuration.h
        include/data-sources/latency/latency.h
        include/data-sources/latency/standards.h
        include/data-sources/reliability/reliability.h
        include/data-sources/synchronization/standards.h
        include/data-sources/synchronization/synchronization.h
)
set(SOURCE_LIST
        data-sources/configuration/configuration.cpp
        data-sources/latency/latency.cpp
        data-sources/latency/standards.cpp
        data-sources/reliability/reliability.cpp
        data-sources/synchronization/synchronization.cpp
)

add_library(dry-rankd-time lib.cpp ${HEADER_LIST} ${SOURCE_LIST})
target_include_directories(dry-rankd-time PUBLIC include vendor)

# All users to compile this library need, at least, C++20.
target_compile_features(dry-rankd-time PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place.
source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})
