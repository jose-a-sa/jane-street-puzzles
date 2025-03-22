cmake_minimum_required(VERSION 3.10)

# Set the project name
project(JaneStreetPuzzles)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-O3")

add_compile_definitions(SPDLOG_FMT_EXTERNAL=1)

include(cmake/FetchFmtLib.cmake)
include(cmake/FetchSpdLog.cmake)

# Add an executable
add_executable(mirrors_3 march-2025/mirrors_3.cpp)
target_link_libraries(mirrors_3 PRIVATE fmt::fmt spdlog::spdlog)
