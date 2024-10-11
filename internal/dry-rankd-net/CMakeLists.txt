project(dry-rankd-net
        VERSION 0.1.0
        DESCRIPTION "Dry-run network Rank daemon library for Simuzilla"
        LANGUAGES CXX C)

set(HEADER_LIST
        include/dry-net-lib.h
        include/data-sources/interfaces.h
        include/data-sources/neighbors.h
        include/data-sources/routes.h
)
set(SOURCE_LIST
        data-sources/interfaces.cpp
        data-sources/neighbors.cpp
        data-sources/routes.cpp

)

add_library(dry-rankd-net lib.cpp ${HEADER_LIST} ${SOURCE_LIST})
target_include_directories(dry-rankd-net PUBLIC include vendor)

# All users to compile this library need, at least, C++20.
target_compile_features(dry-rankd-net PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place.
source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})
