project(rankd
        VERSION 0.1.0
        DESCRIPTION "Rank daemon executable"
        LANGUAGES CXX C)

if(CMAKE_BUILD_TYPE MATCHES Release)
    message("-- Processing cmd/rankd in Release mode.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
else()
    message("-- Processing cmd/rankd in Debug mode.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g -O0")
endif()

set(HEADER_LIST
        include/internals.h
        include/api/api.h
        include/cli/options.h
        include/components/engine.h
        include/components/engine-computing.h
        include/components/engine-network.h
        include/components/engine-time.h
        include/configs/configs.h
        )
set(SOURCE_LIST
        api/api.cpp
        cli/options.cpp
        components/engine-computing.cpp
        components/engine-network.cpp
        components/engine-time.cpp
        configs/configs.cpp
        )

# Add vendor project directories.
add_subdirectory(vendor/restbed)

# Make the rankd executable.
add_executable(rankd main.cpp ${HEADER_LIST} ${SOURCE_LIST})
target_include_directories(rankd PUBLIC include vendor ${rankd-comp} ${rankd-time} ${rankd-net})
target_link_libraries(rankd pthread rankd-comp rankd-time rankd-net restbed-static)

# All users of this library will need, at least, C++20.
target_compile_features(rankd PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place.
source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})
