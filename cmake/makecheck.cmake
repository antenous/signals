# https://gitlab.kitware.com/cmake/community/wikis/doc/tutorials/EmulateMakeCheck

cmake_minimum_required(VERSION 3.10)
include_guard(GLOBAL)

if(NOT TARGET check AND CMAKE_TESTING_ENABLED)
    set(cmd ${CMAKE_CTEST_COMMAND})

    if(CMAKE_CONFIGURATION_TYPES)
        list(APPEND cmd -C $<CONFIG>)
    endif()

    list(APPEND cmd --output-on-failure)

    add_custom_target(check
        COMMAND ${CMAKE_COMMAND} -E env GTEST_COLOR=yes ${cmd})
endif()
