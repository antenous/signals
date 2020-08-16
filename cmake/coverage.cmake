# Copyright (c) 2019 Antero Nousiainen
#
# Mark targets for coverage with `add_coverage()`:
#
#   add_coverage(<test target>)
#
# In toplevel CMakeLists.txt add `check_coverage()` after all other targets have been defined:
#
#   check_coverage(<coverage target> [EXCLUDES <excludes> ...])
#
# Example:
#
#   include(coverage)
#
#   enable_testing()
#   add_executable(testrunner test.cpp)
#   add_test(NAME ${PROJECT_NAME}_test COMMAND testrunner)
#   add_coverage(testrunner)
#
#   check_coverage(check-coverage EXCLUDES '*googletest*')
#
# To create a code coverage report run the below commands:
#
#   $ cmake -S . -B build/ -DCODE_COVERAGE=On
#   $ cmake --build build/ --target check-coverage
#
# NOTE! Code coverage may fail if a source file is removed. In this case revert
#       back to a phase that had the missing file, run `clean` target to remove
#       the coverage files and return back to build the coverage.
# NOTE! Enabling code coverage enforces the build type to be "Debug"

cmake_minimum_required(VERSION 3.15)
include_guard(GLOBAL)

include(CMakeDependentOption)
cmake_dependent_option(CODE_COVERAGE "Enable code coverage" OFF "CMAKE_TESTING_ENABLED" OFF)

if(CODE_COVERAGE)

    message("Code Coverage Report enabled")
    set(Coverage_DEPENDENCIES "" CACHE INTERNAL "")

    # Enforce a Debug build type
    set(CMAKE_BUILD_TYPE Debug)

    find_program(LCOV lcov)
    if(NOT LCOV)
        message(FATAL_ERROR "Unable to find lcov")
    else()
        message(STATUS "Found lcov: ${LCOV}")
    endif()

    find_program(GENHTML genhtml)
    if(NOT GENHTML)
        message(FATAL_ERROR "Unable to find genhtml")
    else()
        message(STATUS "Found genhtml: ${GENHTML}")
    endif()

    find_program(GCOVR gcovr)
    if(NOT GCOVR)
        message("Unable to find gcovr, coverage summary not available")
    else()
        message(STATUS "Found gcovr: ${GCOVR}")
    endif()

endif()

# Add a test target dependency to the code coverage generation
function(add_coverage target)
    if(CODE_COVERAGE)

        set(Coverage_DEPENDENCIES ${Coverage_DEPENDENCIES} ${target} CACHE INTERNAL "")
        target_compile_options(${target} PRIVATE -O0 --coverage)
        target_link_options(${target} PUBLIC --coverage)

        # Set gcc coverage files to be removed with `clean` target
        get_target_property(src ${target} SOURCES)
        get_target_property(dir ${target} BINARY_DIR)

        foreach(source ${src})
            foreach(ext "gcda" "gcno")
                list(APPEND coverage_files "${dir}/CMakeFiles/${target}.dir/${source}.${ext}")
            endforeach()
        endforeach()

        set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES "${coverage_files}")

    endif()
endfunction()

# Code coverage target
# https://wiki.documentfoundation.org/Development/Lcov
function(check_coverage target)
    if(CODE_COVERAGE)

        set(options)
        set(oneValueArgs)
        set(multiValueArgs EXCLUDES)
        cmake_parse_arguments(Coverage "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

        set(report ${PROJECT_NAME}-coverage)
        set(excludes '/usr/include/*' ${Coverage_EXCLUDES})

        add_custom_target(${target}
            COMMENT "Creating code coverage report"

            # Cleanup and run baseline lcov
            COMMAND ${LCOV} --zerocounters --directory . --quiet
            COMMAND ${LCOV} --capture --initial --directory . --output-file ${report}.base --quiet

            # Run the tests
            COMMAND ${CMAKE_CTEST_COMMAND}

            # Run lcov again after tests
            COMMAND ${LCOV} --capture --directory . --output-file ${report}.info --quiet

            # Combine lcov tracefiles
            COMMAND ${LCOV} --add-tracefile ${report}.base --add-tracefile ${report}.info --output-file ${report}.total --quiet

            # Remove unwanted stuff from tracefile
            COMMAND ${LCOV} --remove ${report}.total ${excludes} --output-file ${report}.info.cleaned --quiet

            # Generate HTML reports.
            COMMAND ${GENHTML} --demangle-cpp --title ${PROJECT_NAME} --output-directory=${report} ${report}.info.cleaned --quiet

            BYPRODUCTS
                ${report}
                ${report}.base
                ${report}.info
                ${report}.total
                ${report}.info.cleaned
        )

        add_dependencies(${target} ${Coverage_DEPENDENCIES})

        if(GCOVR)
            set(gcovr_excludes ${Coverage_EXCLUDES})
            list(TRANSFORM gcovr_excludes REPLACE "\\*" ".*")
            list(TRANSFORM gcovr_excludes PREPEND "--exclude;")

            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${GCOVR} -r ${PROJECT_SOURCE_DIR} ${gcovr_excludes})
        endif()

        add_custom_command(TARGET ${target} POST_BUILD
            COMMENT "View ${PROJECT_BINARY_DIR}/${report}/index.html to see the full report")

    endif()
endfunction()
