# https://gitlab.kitware.com/cmake/community/wikis/doc/tutorials/EmulateMakeCheck

cmake_minimum_required(VERSION 3.10)
include_guard(GLOBAL)

if(NOT TARGET check AND CMAKE_TESTING_ENABLED)
    set(cmd GTEST_COLOR=yes ${CMAKE_CTEST_COMMAND} --output-on-failure)

    if(CMAKE_HOST_WIN32)
        set(cmd ${CMAKE_CTEST_COMMAND} -C $<CONFIG> --output-on-failure)
    endif()

    add_custom_target(check COMMAND ${cmd})
endif()

