cmake_minimum_required(VERSION 3.15)
project(signals
    VERSION 2.1.1
    DESCRIPTION "A simple signals and slots library"
    HOMEPAGE_URL "https://gitlab.com/antenous/signals"
    LANGUAGES CXX)

if(NOT DEFINED SIGNALS_STANDALONE_PROJECT)
    set(SIGNALS_STANDALONE_PROJECT OFF)

    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(SIGNALS_STANDALONE_PROJECT ON)
    endif()
endif()

set(SIGNALS_TEST_DEFAULT ${SIGNALS_STANDALONE_PROJECT})
option(SIGNALS_TEST "Enable tests" ${SIGNALS_TEST_DEFAULT})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(colordiagnostics)
include(install)

add_library(signals
    src/Connection.cpp
    src/ScopedConnection.cpp)
add_library(signals::signals ALIAS signals)
target_compile_features(signals PUBLIC cxx_std_20)
target_compile_options(signals PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
        -Wall -Werror -Wextra -pedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>)
target_include_directories(signals PUBLIC
    $<BUILD_INTERFACE:${signals_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(SIGNALS_STANDALONE_PROJECT)
    include(package)
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
