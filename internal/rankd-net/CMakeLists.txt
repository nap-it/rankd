project(rankd-net
        VERSION 0.1.0
        DESCRIPTION "Network Rank daemon library"
        LANGUAGES CXX C)

# Look for DDS features by FastDDS.
if (NOT fastcdr_FOUND)
    find_package(fastcdr REQUIRED)
endif ()

if (NOT fastrtps_FOUND)
    find_package(fastrtps REQUIRED)
endif ()

file(GLOB DDS_DESCRIPTOR_HEADERS "include/data-sources/yanpit/dds/descriptors/*.h")
file(GLOB DDS_DESCRIPTOR_SOURCES "data-sources/yanpit/dds/descriptors/*.cxx")

set(HEADER_LIST
        include/net-lib.h
        include/data-sources/interfaces.h
        include/data-sources/neighbors.h
        include/data-sources/routes.h
        include/data-sources/yanpit.h
        include/data-sources/yanpit/dds/dds.h
        include/data-sources/yanpit/dds/localnet.h
        include/data-sources/yanpit/dds/ncm.h
        include/data-sources/yanpit/dds/yanpit.h
        ${DDS_DESCRIPTOR_HEADERS}
        include/data-sources/yanpit/structs/yanpit-interfaces.h
        include/utils/addresses.h
)
set(SOURCE_LIST
        data-sources/interfaces.cpp
        data-sources/neighbors.cpp
        data-sources/routes.cpp
        data-sources/yanpit.cpp
        ${DDS_DESCRIPTOR_SOURCES}
        data-sources/yanpit/structs/yanpit-interfaces.cpp
        utils/addresses.cpp
        )

# Make the rankd-net library (librankd-net).
set(RANKD_NET include)
add_library(rankd-net lib.cpp ${HEADER_LIST} ${SOURCE_LIST})
target_include_directories(rankd-net PUBLIC include vendor/rapidjson)
target_link_libraries(rankd-net PRIVATE fastrtps fastcdr)

# All users to compile this library need, at least, C++20.
target_compile_features(rankd-net PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place.
source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})
