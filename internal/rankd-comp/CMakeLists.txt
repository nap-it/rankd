project(rankd-comp
        VERSION 0.1.0
        DESCRIPTION "Computational Rank daemon library"
        LANGUAGES CXX C)

if(APPLE)
    message("-- Preparing rankd-comp environment as Mac OS X")
    set(HEADER_LIST
            include/comp-lib.h
            include/macos/cpu.h
            include/utils/strings.h
            )
    set(SOURCE_LIST
            data-sources/macos/cpu.cpp
            utils/strings.cpp
            )

    # Make the rankd-comp library (librankd-comp).
    add_library(rankd-comp lib.cpp ${HEADER_LIST} ${SOURCE_LIST})
    target_include_directories(rankd-comp PUBLIC include include/macos)
    target_link_libraries(rankd-comp PRIVATE)


elseif (UNIX)
    message("-- Preparing rankd-comp environment as Linux")
    set(HEADER_LIST
            include/comp-lib.h
            include/linux/cpu.h
            include/utils/strings.h
            )
    set(SOURCE_LIST
            data-sources/linux/cpu.cpp
            utils/strings.cpp
            )

    add_subdirectory(vendor/pfs)
    # Make the rankd-comp library (librankd-comp).
    add_library(rankd-comp lib.cpp ${HEADER_LIST} ${SOURCE_LIST})
    target_include_directories(rankd-comp PUBLIC include include/linux vendor/pfs/include)
    target_link_libraries(rankd-comp PRIVATE pfs)
endif()

# All users to compile this library need, at least, C++20.
target_compile_features(rankd-comp PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place.
source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})
