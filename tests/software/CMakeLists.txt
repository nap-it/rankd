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
target_link_libraries(relyum-monitor netsnmp)