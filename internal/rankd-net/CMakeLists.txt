project(rankd-net
        VERSION 0.1.0
        DESCRIPTION "Network Rank daemon library"
        LANGUAGES CXX C)

find_package(Protobuf REQUIRED)

set(HEADER_LIST
        include/net-lib.h
        include/data-sources/yanpit.h
        include/data-sources/yanpit/consts.h
        include/data-sources/yanpit/iperf.pb.h
        include/data-sources/yanpit/localnet.pb.h
        include/data-sources/yanpit/ncm.pb.h
        )
set(SOURCE_LIST
        data-sources/yanpit.cpp
        data-sources/yanpit/iperf.pb.cc
        data-sources/yanpit/localnet.pb.cc
        data-sources/yanpit/ncm.pb.cc
        )

# Make the rankd-net library (librankd-net).
#set(RANKD_NET include)
include_directories(${Protobuf_INCLUDE_DIRS})

add_library(rankd-net lib.cpp ${HEADER_LIST} ${SOURCE_LIST})
target_include_directories(rankd-net PUBLIC include vendor/restbed/source)
#target_link_libraries(rankd-net PRIVATE )

# All users to compile this library need, at least, C++20.
target_compile_features(rankd-net PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place.
source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})
