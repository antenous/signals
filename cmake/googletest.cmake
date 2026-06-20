cmake_minimum_required(VERSION 3.11)
include_guard(GLOBAL)
include(FetchContent)

if(CMAKE_TESTING_ENABLED)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.17.0)

    FetchContent_MakeAvailable(googletest)

    # Keep GoogleTest out of the default ALL target. The check target still
    # builds it through the test binaries' link dependencies.
    #
    # When the minimum CMake version reaches 3.28, replace this with the
    # FetchContent_Declare(EXCLUDE_FROM_ALL) option above.
    set_target_properties(gtest gtest_main gmock gmock_main
        PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()
