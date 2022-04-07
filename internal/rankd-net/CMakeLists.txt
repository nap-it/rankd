project(rankd-net
        VERSION 0.1.0
        DESCRIPTION "Network Rank daemon library"
        LANGUAGES CXX C)

set(HEADER_LIST
        )
set(SOURCE_LIST
        )

# Make the rankd-net library (librankd-net).
add_library(rankd-net lib.cpp ${HEADER_LIST} ${SOURCE_LIST})
target_include_directories(rankd-net PUBLIC include)
#target_link_libraries(rankd-net PRIVATE )

# All users to compile this library need, at least, C++20.
target_compile_features(rankd-net PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place.
source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})
