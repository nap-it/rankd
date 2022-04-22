add_executable(memory-monitor memory-monitor.cpp)
target_link_libraries(memory-monitor rankd-comp)

add_executable(cpu-monitor cpu-monitor.cpp)
target_link_libraries(cpu-monitor rankd-comp)

add_executable(network-monitor network-monitor.cpp)
target_link_libraries(network-monitor rankd-comp)

add_executable(storage-monitor storage-monitor.cpp)
target_link_libraries(storage-monitor rankd-comp)