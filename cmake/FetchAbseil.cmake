if(NOT DEFINED abseil_FETCH_VERSION)
    set(abseil_FETCH_VERSION "20240722.1")
endif()

include(FetchContent)

FetchContent_Declare(abseil
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp/
    GIT_TAG ${abseil_FETCH_VERSION}
)

set(ABSL_PROPAGATE_CXX_STD ON)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "") # up to you
set(BUILD_TESTING OFF CACHE BOOL "") # to disable abseil test
set(ABSL_ENABLE_INSTALL ON CACHE BOOL "") # now you can enable install rules even in subproject...

add_compile_options(-Wno-error=gcc-compat)

FetchContent_MakeAvailable(abseil)
