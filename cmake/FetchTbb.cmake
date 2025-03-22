if(tbb_FOUND)
    message(STATUS "TBB ${tbb_VERSION} already found in ${tbb_DIR}")
    return()
endif()

if(NOT DEFINED tbb_FETCH_VERSION)
    set(tbb_FETCH_VERSION "v2022.0.0")
endif()

include(FetchContent)

set(BUILD_SHARED_LIBS ON)
FetchContent_Declare(tbb
    GIT_REPOSITORY https://github.com/uxlfoundation/oneTBB
    GIT_TAG ${tbb_FETCH_VERSION}
)

FetchContent_MakeAvailable(tbb)

message(STATUS "Built TBB ${tbb_FETCH_VERSION} in ${tbb_BINARY_DIR}")
