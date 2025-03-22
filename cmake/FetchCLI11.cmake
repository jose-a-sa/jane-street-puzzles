if(cli11_FOUND)
    message(STATUS "CLI11 ${cli11_VERSION} already found in ${cli11_DIR}")
    return()
endif()

if(NOT DEFINED cli11_FETCH_VERSION)
    set(cli11_FETCH_VERSION "v2.4.2")
endif()

include(FetchContent)

FetchContent_Declare(cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG ${cli11_FETCH_VERSION}
)

FetchContent_MakeAvailable(cli11)

message(STATUS "Built CLI11 ${cli11_FETCH_VERSION} in ${cli11_BINARY_DIR}")

