project(rankd-comp
        VERSION 0.1.0
        DESCRIPTION "Computational Rank daemon library"
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

add_subdirectory(vendor/libsensors-cpp)
add_subdirectory(vendor/pfs)

# Make the rankd-comp library (librankd-comp).
set(RANKD_COMP include)
if(NOT EXTERNALLY_SOURCED)
        add_library(rankd-comp lib.cpp ${HEADER_LIST} ${SOURCE_LIST})
endif()
target_include_directories(rankd-comp PUBLIC include vendor/pfs/include vendor/libsensors-cpp/include vendor/rapidjson)
target_link_libraries(rankd-comp PRIVATE pfs sensors-c++)

# All users to compile this library need, at least, C++20.
target_compile_features(rankd-comp PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place.
source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})
