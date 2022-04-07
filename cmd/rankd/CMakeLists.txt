project(rankd
        VERSION 0.1.0
        DESCRIPTION "Rank daemon executable"
        LANGUAGES CXX C)

if(CMAKE_BUILD_TYPE MATCHES Release)
    message("-- Processing cmd/rankd in Release mode.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3")
else()
    message("-- Processing cmd/rankd in Debug mode.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fsanitize=address -fno-omit-frame-pointer -g -O0")
endif()

set(HEADER_LIST
        )
set(SOURCE_LIST
        )

# Make the rankd executable.
add_executable(rankd main.cpp ${HEADER_LIST} ${SOURCE_LIST})
target_include_directories(rankd PUBLIC include ${VENDOR_DIRS})
#target_link_libraries(rankd rankd-comp rankd-time rankd-net)

# All users of this library will need, at least, C++20.
target_compile_features(rankd PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place.
source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})
