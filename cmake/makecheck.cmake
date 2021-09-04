# https://gitlab.kitware.com/cmake/community/wikis/doc/tutorials/EmulateMakeCheck

cmake_minimum_required(VERSION 3.10)
include_guard(GLOBAL)

if(NOT TARGET check AND CMAKE_TESTING_ENABLED)
    add_custom_target(check
        COMMAND GTEST_COLOR=yes ${CMAKE_CTEST_COMMAND} --output-on-failure)
endif()

