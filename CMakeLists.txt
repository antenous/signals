cmake_minimum_required(VERSION 3.15)
project(signals
    VERSION 1.0.0
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

add_library(signals
    src/Connection.cpp
    src/ScopedConnection.cpp)
add_library(signals::signals ALIAS signals)
target_compile_features(signals PRIVATE cxx_std_17)
target_compile_options(signals PRIVATE -Wall -Werror -Wextra -pedantic)
target_include_directories(signals PUBLIC
    $<BUILD_INTERFACE:${signals_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(SIGNALS_STANDALONE_PROJECT)
    include(install)
    include(uninstall)
    target_install(signals)
endif()

if(SIGNALS_TEST)
    enable_testing()
    include(coverage)
    include(googletest)
    include(makecheck)

    add_coverage(signals)
    add_subdirectory(tst EXCLUDE_FROM_ALL)

    if(SIGNALS_STANDALONE_PROJECT)
        check_coverage(check-coverage EXCLUDES '*googletest*')
    endif()
endif()
