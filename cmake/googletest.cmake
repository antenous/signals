cmake_minimum_required(VERSION 3.11)
include_guard(GLOBAL)
include(FetchContent)

if(CMAKE_TESTING_ENABLED)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.17.0)

    FetchContent_MakeAvailable(googletest)
endif()
