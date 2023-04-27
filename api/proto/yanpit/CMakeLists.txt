find_package(Protobuf REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
        "iperf.proto"
        "localnet.proto"
        "ncm.proto")

# Dummy executable to force the generation of *.cc and *.h files.
add_executable(protobuf-generator main.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(protobuf-generator ${Protobuf_LIBRARIES})