project(rankd-comp
        VERSION 0.1.0
        DESCRIPTION "Dry-run computational Rank daemon library for Simuzilla"
        LANGUAGES CXX C)

set(HEADER_LIST
        include/comp-lib.h
        include/data-sources/cpu.h
        include/data-sources/memory.h
        include/data-sources/os.h
        include/data-sources/sensors.h
        include/data-sources/storage.h
        include/utils/strings.h
)
set(SOURCE_LIST
        data-sources/cpu.cpp
        data-sources/memory.cpp
        data-sources/os.cpp
        data-sources/sensors.cpp
        data-sources/storage.cpp
        utils/strings.cpp
)

# Make the rankd-comp library (librankd-comp).
set(RANKD_COMP include)
add_library(dry-rankd-comp lib.cpp ${HEADER_LIST} ${SOURCE_LIST})
target_include_directories(rankd-comp PUBLIC include vendor)
#target_link_libraries(rankd-comp PRIVATE )

# All users to compile this library need, at least, C++20.
target_compile_features(rankd-comp PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place.
source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})
