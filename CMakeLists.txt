cmake_minimum_required(VERSION 3.10)

# Set the project name
project(JaneStreetPuzzles)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_CXX_FLAGS "-O3")

include(cmake/FetchFmtLib.cmake)
include(cmake/FetchSpdLog.cmake)
include(cmake/FetchCLI11.cmake)
include(cmake/FetchGoogleTest.cmake)

enable_testing()
include(GoogleTest)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(march-2025)
add_subdirectory(may-2025)
