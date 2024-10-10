project(rank-prelude
        VERSION 0.1.0
        DESCRIPTION "Rank prelude library"
        LANGUAGES CXX C)

set(HEADER_LIST
        include/prelude.h
        include/constants.h
        include/structs/capabilities.h
        include/structs/current_capabilities.h
        include/structs/dispatcher.h
        include/structs/dispatchers/receiver_simulation.h
        include/structs/dispatchers/receiver_l2.h
        include/structs/dispatchers/receiver_l3.h
        include/structs/dispatchers/receiver_dds.h
        include/structs/dispatchers/sender.h
        include/structs/handler.h
        include/structs/handler_state.h
        include/structs/message.h
        include/structs/messages/acc.h
        include/structs/messages/bid.h
        include/structs/messages/constants.h
        include/structs/messages/ear.h
        include/structs/messages/header.h
        include/structs/messages/mar.h
        include/structs/messages/message_type.h
        include/structs/messages/ref.h
        include/structs/messages/rep.h
        include/structs/process.h
        include/structs/requesting_capabilities.h
        include/structs/reservation.h
        include/structs/reservation_state.h
        include/structs/resources.h
        include/structs/timeout_handler.h
        include/structs/timeout_handler_spec.h
        include/utils/hashing.h
        include/utils/marshalling.h
        include/utils/marshalling/serialization.h
        include/utils/marshalling/transformation.h
        include/utils/marshalling/validation.h
        include/utils/marshalling/validation/yang.h
        include/utils/neighboring.h
        include/utils/identity.h
        include/utils/identity/network.h
        include/utils/identity/session.h
        include/utils/messaging.h
)

set(SOURCE_LIST
        structs/capabilities.cpp
        structs/current_capabilities.cpp
        structs/dispatcher.cpp
        structs/dispatchers/receiver_simulation.cpp
        structs/dispatchers/receiver_l2.cpp
        structs/dispatchers/receiver_l3.cpp
        structs/dispatchers/receiver_dds.cpp
        structs/dispatchers/sender.cpp
        structs/handler.cpp
        structs/identifier.cpp
        structs/message.cpp
        structs/messages/acc.cpp
        structs/messages/bid.cpp
        structs/messages/ear.cpp
        structs/messages/header.cpp
        structs/messages/mar.cpp
        structs/messages/ref.cpp
        structs/messages/rep.cpp
        structs/process.cpp
        structs/requesting_capabilities.cpp
        structs/reservation.cpp
        structs/resources.cpp
        structs/timeout_handler.cpp
        structs/timeout_handler_spec.cpp
        utils/identity.cpp
        utils/identity/network.cpp
        utils/identity/session.cpp
        utils/marshalling.cpp
        utils/marshalling/transformation.cpp
        utils/marshalling/serialization.cpp
        utils/marshalling/validation.cpp
        utils/marshalling/validation/yang.cpp
        utils/messaging.cpp
        utils/neighboring.cpp
)

# Add external projects (vendor).
if(NOT EXTERNALLY_SOURCED)
    add_subdirectory(vendor)
endif()

if (NOT FROM_SIMUZILLA)
    # Compile and produce the needed eBPF skeletons.
    list(APPEND CMAKE_MODULE_PATH ${RANK_PRELUDE_LIBBPF_ROOT}/tools/cmake)
    find_package(BpfObject REQUIRED)
    if(NOT EXTERNALLY_SOURCED)
        bpf_object(rank_l2 structs/dispatchers/bpf/rank_l2.bpf.c)
        add_dependencies(rank_l2_skel libbpf-build bpftool-build)
    endif()

    if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/rank_l2.skel.h)
        configure_file(${CMAKE_CURRENT_BINARY_DIR}/rank_l2.skel.h ${CMAKE_CURRENT_SOURCE_DIR}/include/structs/dispatchers/bpf/rank_l2.skel.h COPYONLY)
    endif()

endif()

set(RANK_PRELUDE include)

#if(NOT EXTERNALLY_SOURCED)
add_library(rank-prelude STATIC prelude.cpp ${SOURCE_LIST} ${HEADER_LIST})
#endif()

target_include_directories(rank-prelude PUBLIC include)
target_include_directories(rank-prelude PUBLIC
        vendor
        vendor/libyang-cpp/include
        vendor/rapidjson
        vendor/doctest/doctest
        vendor/cbor-cpp/src
        vendor/pcap-plus-plus/Common++/header
        vendor/pcap-plus-plus/Packet++/header
        vendor/pcap-plus-plus/Pcap++/header
)
if (GLOBAL_PROJECT_SCOPE)
    target_include_directories(rank-prelude PUBLIC ${GLOBALLY_RANKD_INTERNAL_COMP_INCLUDES})
    target_include_directories(rank-prelude PUBLIC ${GLOBALLY_RANKD_INTERNAL_NET_INCLUDES})
    target_include_directories(rank-prelude PUBLIC ${GLOBALLY_RANKD_INTERNAL_TIME_INCLUDES})
else()
    target_include_directories(rank-prelude PUBLIC ../rankd-comp/include)
    target_include_directories(rank-prelude PUBLIC ../rankd-net/include)
    target_include_directories(rank-prelude PUBLIC ../rankd-time/include)
endif()

if (NOT FROM_SIMUZILLA)
    target_link_libraries(rank-prelude PRIVATE rank_l2_skel)
endif()
target_link_libraries(rank-prelude PRIVATE pthread)
target_link_libraries(rank-prelude PRIVATE cborcpp)
target_link_libraries(rank-prelude PRIVATE Pcap++ Packet++ Common++ pcap)
target_link_libraries(rank-prelude PRIVATE rankd-comp rankd-net rankd-time)

target_compile_features(rank-prelude PUBLIC cxx_std_20)

if(NOT EXTERNALLY_SOURCED)
    add_subdirectory(tests)
endif()

source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})