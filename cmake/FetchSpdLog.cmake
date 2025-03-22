if(spdlog_FOUND)
    message(STATUS "spdlog ${spdlog_VERSION} already found in ${spdlog_DIR}")
    return()
endif()

if(NOT DEFINED spdlog_FETCH_VERSION)
    set(spdlog_FETCH_VERSION "v1.15.1")
endif()

if(fmt_FOUND OR fmt_POPULATED)
    set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "Use external fmt library" FORCE)
endif()

include(FetchContent)
FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG ${spdlog_FETCH_VERSION}
)
FetchContent_MakeAvailable(spdlog)

message(STATUS "Built spdlog ${spdlog_FETCH_VERSION} in ${spdlog_BINARY_DIR}")
