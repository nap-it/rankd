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

add_executable(ptp-ptpmgmt-monitor ptp2-monitor.cpp)
target_link_libraries(ptp-ptpmgmt-monitor ptpmgmt)

# add_executable(ptp-monitor ptp-monitor.cpp)
# target_link_libraries(ptp-monitor rankd-time)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBNL_PKG REQUIRED IMPORTED_TARGET libnl-genl-3.0)
add_executable(tc-tryout tc-tryout.cpp)
target_compile_features(tc-tryout PUBLIC cxx_std_20)
target_link_libraries(tc-tryout PkgConfig::LIBNL_PKG)