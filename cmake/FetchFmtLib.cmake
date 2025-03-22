if(fmt_FOUND)
    message(STATUS "fmtlib ${fmt_VERSION} already found in ${fmt_DIR}")
    return()
endif()

if(NOT DEFINED fmt_FETCH_VERSION)
    set(fmt_FETCH_VERSION "11.0.2")
endif()


include(FetchContent)

FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG ${fmt_FETCH_VERSION}
)

FetchContent_MakeAvailable(fmt)

message(STATUS "Built fmtlib ${fmt_FETCH_VERSION} in ${fmt_BINARY_DIR}")

