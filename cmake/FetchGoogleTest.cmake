if(GTest_FOUND)
    message(STATUS "GoogleTest ${GTest_VERSION} already found in ${GTest_DIR}")
    return()
endif()

if(NOT DEFINED googletest_FETCH_VERSION)
    set(googletest_FETCH_VERSION "v1.15.2")
endif()


include(FetchContent)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG ${googletest_FETCH_VERSION}
)

if(WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "For Windows: Prevent overriding the parent project's compiler/linker settings" FORCE)
endif()

FetchContent_MakeAvailable(googletest)

message(STATUS "Built GoogleTest ${googletest_FETCH_VERSION} in ${googletest_BINARY_DIR}")
