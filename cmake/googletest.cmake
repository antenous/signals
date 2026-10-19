cmake_minimum_required(VERSION 3.28)
include_guard(GLOBAL)
include(FetchContent)

if(CMAKE_TESTING_ENABLED)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    # Keep GoogleTest out of the default ALL target. The check target still
    # builds it through the test binaries' link dependencies.
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.18.0
        EXCLUDE_FROM_ALL)

    FetchContent_MakeAvailable(googletest)
endif()
