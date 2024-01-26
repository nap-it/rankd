project(rank-prelude-tests)

add_executable(test_utils_identity utils/identity_tester.cpp)
target_include_directories(test_utils_identity PUBLIC ../include)
target_link_libraries(test_utils_identity PRIVATE rank-prelude)

add_executable(test_vendor_libyangcpp vendor/libyang-cpp/main.cpp)
target_include_directories(test_vendor_libyangcpp PUBLIC ../vendor/libyang-cpp/include)
target_link_libraries(test_vendor_libyangcpp PRIVATE yang)