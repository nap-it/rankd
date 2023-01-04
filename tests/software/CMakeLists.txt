add_executable(memory-monitor memory-monitor.cpp)
target_link_libraries(memory-monitor rankd-comp)

add_executable(cpu-monitor cpu-monitor.cpp)
target_link_libraries(cpu-monitor rankd-comp)

add_executable(network-monitor network-monitor.cpp)
target_link_libraries(network-monitor rankd-comp)

add_executable(storage-monitor storage-monitor.cpp)
target_link_libraries(storage-monitor rankd-comp)

add_executable(sensors-monitor sensors-monitor.cpp)
target_link_libraries(sensors-monitor rankd-comp sensors-c++)

add_executable(os-monitor os-monitor.cpp)
target_link_libraries(os-monitor rankd-comp)

add_executable(relyum-monitor relyum-snmp-monitor.cpp)
target_link_libraries(relyum-monitor rankd-time netsnmp linuxptp)

add_executable(ptp-monitor ptp-monitor.cpp)
target_link_libraries(ptp-monitor rankd-time)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBNL_PKG REQUIRED IMPORTED_TARGET libnl-genl-3.0)
ExternalProject_Add(iproute2
        PREFIX iproute2
        URL "https://code.nap.av.it.pt/api/v4/projects/441/repository/archive?private_token=glpat-dWyowyiEGiDsQYFyHg8T"
        DOWNLOAD_NO_PROGRESS ON
        CONFIGURE_COMMAND ""
        BUILD_COMMAND make all
        BUILD_IN_SOURCE 1
        BUILD_ALWAYS 1
        INSTALL_COMMAND ""
        LOG_DOWNLOAD 1
        LOG_BUILD 1
        )
ExternalProject_Get_Property(iproute2 SOURCE_DIR)
SET(IPROUTE2_INCLUDE_DIR ${SOURCE_DIR})
SET(IPROUTE2_LIB_DIR ${SOURCE_DIR}/lib)

add_library(tc-nap STATIC IMPORTED)
set_target_properties(tc-nap PROPERTIES IMPORTED_LOCATION ${SOURCE_DIR}/tc/libtc.a)

add_library(iproute2-util-nap STATIC IMPORTED)
set_target_properties(iproute2-util-nap PROPERTIES IMPORTED_LOCATION ${IPROUTE2_LIB_DIR}/libutil.a)

add_library(iproute2-netlink-nap STATIC IMPORTED)
set_target_properties(iproute2-netlink-nap PROPERTIES IMPORTED_LOCATION ${IPROUTE2_LIB_DIR}/libnetlink.a)

include_directories(${IPROUTE2_INCLUDE_DIR}/include ${IPROUTE2_INCLUDE_DIR})
link_directories(${SOURCE_DIR}/tc ${IPROUTE2_LIB_DIR})

add_executable(tc-tryout tc-tryout.cpp)
target_link_libraries(tc-tryout PkgConfig::LIBNL_PKG tc-nap iproute2-util-nap iproute2-util-nap)
add_dependencies(tc-tryout iproute2)