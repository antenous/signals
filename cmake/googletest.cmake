cmake_minimum_required(VERSION 3.11)
include_guard(GLOBAL)
include(FetchContent)

if(CMAKE_TESTING_ENABLED)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        93f08be653c36ddc6943e9513fc14c7292b4d007)

    FetchContent_GetProperties(googletest)

    if(NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
endif()
