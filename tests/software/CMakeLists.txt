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
set(TC_HEADER_LIST tc-utils/color.h tc-utils/json_print.h tc-utils/list.h tc-utils/ll_map.h tc-utils/rtm_map.h tc-utils/utils.h tc-utils/libnetlink.h)
set(TC_SOURCE_LIST tc-utils/color.cpp tc-utils/ll_map.cpp tc-utils/libnetlink.cpp)
include_directories(tc-tryout tc-utils)
add_executable(tc-tryout tc-tryout.cpp ${TC_HEADER_LIST} ${TC_SOURCE_LIST})
target_link_libraries(tc-tryout PkgConfig::LIBNL_PKG)