project(rank-prelude
        VERSION 0.1.0
        DESCRIPTION "Rank prelude library"
        LANGUAGES CXX C)

set(HEADER_LIST
    include/structs/capabilities.h
    include/structs/current_capabilities.h
    include/structs/handler.h
    include/structs/handler_state.h
    include/structs/message.h
    include/structs/messages/acc.h
    include/structs/messages/bid.h
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
)

set(SOURCE_LIST
        structs/capabilities.cpp
        structs/current_capabilities.cpp
        structs/handler.cpp
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
)

set(RANK_PRELUDE include)
add_library(rank-prelude prelude.cpp ${SOURCE_LIST})
target_include_directories(rank-prelude PUBLIC include)
#target_link_libraries(rank-prelude PRIVATE)

target_compile_features(rank-prelude PUBLIC cxx_std_20)

source_group(TREE "${PROJECT_SOURCE_DIR}" PREFIX "Header Files" FILES ${HEADER_LIST})