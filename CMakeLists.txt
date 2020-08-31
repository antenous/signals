cmake_minimum_required(VERSION 3.15)
project(signals
    VERSION 0.0.1
    DESCRIPTION "A simple signals and slots library"
    LANGUAGES CXX)

if(NOT DEFINED SIGNALS_STANDALONE_PROJECT)
    set(SIGNALS_STANDALONE_PROJECT OFF)

    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(SIGNALS_STANDALONE_PROJECT ON)
    endif()
endif()

include(CMakeDependentOption)
cmake_dependent_option(SIGNALS_TEST "Enable tests" OFF "NOT SIGNALS_STANDALONE_PROJECT" ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(colordiagnostics)

if(SIGNALS_TEST)
    enable_testing()
    include(coverage)
    include(googletest)
    include(makecheck)
endif()

add_library(signals INTERFACE)
target_include_directories(signals INTERFACE include)

if(SIGNALS_TEST)
    add_subdirectory(tst EXCLUDE_FROM_ALL)
    check_coverage(check-coverage EXCLUDES '*googletest*')
endif()
