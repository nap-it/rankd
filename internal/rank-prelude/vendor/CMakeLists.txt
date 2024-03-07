add_subdirectory(libyang-cpp)
add_subdirectory(cbor-cpp)

set(RANK_PRELUDE_LIBBPF_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/libbpf-bootstrap CACHE INTERNAL "RANK_PRELUDE_LIBBPF_ROOT")
message("-- Set libbpf-bootstrap library path as ${RANK_PRELUDE_LIBBPF_ROOT}")

# Build vendored libbpf.
include(ExternalProject)
ExternalProject_Add(libbpf
        PREFIX libbpf
        SOURCE_DIR ${RANK_PRELUDE_LIBBPF_ROOT}/libbpf/src
        CONFIGURE_COMMAND ""
        BUILD_COMMAND make
        BUILD_STATIC_ONLY=1
        OBJDIR=${CMAKE_CURRENT_BINARY_DIR}/libbpf/libbpf
        DESTDIR=${CMAKE_CURRENT_BINARY_DIR}/libbpf
        INCLUDEDIR=
        LIBDIR=
        UAPIDIR=
        install install_uapi_headers
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND ""
        STEP_TARGETS build
)
ExternalProject_Add(bpftool
        PREFIX bpftool
        SOURCE_DIR ${RANK_PRELUDE_LIBBPF_ROOT}/bpftool/src
        CONFIGURE_COMMAND ""
        BUILD_COMMAND make bootstrap
        OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/bpftool/
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND ""
        STEP_TARGETS build
)
# Set BpfObject input parameters -- note this is usually not necessary unless
# you're in a highly vendored environment (like libbpf-bootstrap)
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    set(ARCH "x86")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    set(ARCH "arm")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    set(ARCH "arm64")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "ppc64le")
    set(ARCH "powerpc")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "mips")
    set(ARCH "mips")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "riscv64")
    set(ARCH "riscv")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "loongarch64")
    set(ARCH "loongarch")
endif()

set(BPFOBJECT_BPFTOOL_EXE ${CMAKE_CURRENT_BINARY_DIR}/bpftool/bootstrap/bpftool CACHE INTERNAL "BPFOBJECT_BPFTOOL_EXE")
set(BPFOBJECT_VMLINUX_H ${RANK_PRELUDE_LIBBPF_ROOT}/vmlinux/${ARCH}/vmlinux.h CACHE INTERNAL "BPFOBJECT_VMLINUX_H")
set(LIBBPF_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/libbpf CACHE INTERNAL "LIBBPF_INCLUDE_DIRS")
set(LIBBPF_LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/libbpf/libbpf.a CACHE INTERNAL "LIBBPF_LIBRARIES")