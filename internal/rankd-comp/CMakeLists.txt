project(rankd-comp
        VERSION 0.1.0
        DESCRIPTION "Computational Rank daemon library"
        LANGUAGES CXX C)

set(HEADER_LIST
        include/structs/cpu.h
        include/structs/memory.h
        include/structs/network.h
        )
set(SOURCE_LIST
        structs/cpu.cpp
        structs/memory.cpp
        structs/network.cpp
        )

add_subdirectory(vendor/pfs)

# Make the rankd-comp library (librankd-comp).
add_library(rankd-comp lib.cpp ${HEADER_LIST} ${SOURCE_LIST})
target_include_directories(rankd-comp PUBLIC include vendor/pfs/include)
target_link_libraries(rankd-comp PRIVATE pfs)

# All users to compile this library need, at least, C++20.
target_compile_features(rankd-comp PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place.
source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})
