set(test "test-${PROJECT_NAME}")
add_executable(${test}
    Connection_test.cpp
    Disconnectable_test.cpp
    Event_test.cpp
    ScopedConnection_test.cpp
    Signal_test.cpp
    Slot_test.cpp)
target_compile_features(${test} PRIVATE cxx_std_20)
target_compile_options(${test} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
        -Wall -Werror -Wextra -pedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>)
target_link_libraries(${test} PRIVATE signals GTest::gmock_main)
add_coverage(${test})
add_dependencies(check ${test})
add_test(NAME ${PROJECT_NAME} COMMAND ${test})
