cmake_minimum_required(VERSION 3.10)

# Set the project name
project(JaneStreetPuzzles)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_CXX_FLAGS "-O3")

include(cmake/FetchFmtLib.cmake)
include(cmake/FetchSpdLog.cmake)
include(cmake/FetchCLI11.cmake)
#include(cmake/FetchGoogleTest.cmake)

# find_package(fmt)
# find_package(spdlog)
# find_package(CLI11)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(2025/march)
add_subdirectory(2025/may)
add_subdirectory(2025/june)
