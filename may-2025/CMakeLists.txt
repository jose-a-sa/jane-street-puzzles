# May 2025

add_executable(number_cross_5 number_cross_5.cpp)
target_link_libraries(number_cross_5 PRIVATE spdlog::spdlog)

add_executable(number_cross_5_single number_cross_5_single.cpp)
target_link_libraries(number_cross_5_single PRIVATE spdlog::spdlog)

add_executable(test_number_cross_5 test_number_cross_5.cpp)
target_link_libraries(test_number_cross_5 PRIVATE spdlog::spdlog CLI11::CLI11 GTest::gtest_main)
gtest_discover_tests(test_number_cross_5)
