if(benchmark_FOUND)
    message(STATUS "Benchmark ${benchmark_VERSION} already found in ${benchmark_DIR}")
    return()
endif()

if(NOT DEFINED benchmark_FETCH_VERSION)
    set(benchmark_FETCH_VERSION "v1.9.1")
endif()


include(FetchContent)

FetchContent_Declare(benchmark
        GIT_REPOSITORY https://github.com/google/benchmark
        GIT_TAG ${benchmark_FETCH_VERSION}
)

if(WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "For Windows: Prevent overriding the parent project's compiler/linker settings" FORCE)
endif()

set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(benchmark)

message(STATUS "Built Benchmark ${benchmark_FETCH_VERSION} in ${benchmark_BINARY_DIR}")
