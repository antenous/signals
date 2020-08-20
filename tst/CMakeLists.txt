set(test "test-${PROJECT_NAME}")
add_executable(${test}
    Connection_test.cpp
    Disconnectable_test.cpp
    Signal_test.cpp
    Slot_test.cpp)
target_compile_features(${test} PRIVATE cxx_std_17)
target_compile_options(${test} PRIVATE -Wall -Werror -Wextra -pedantic)
target_link_libraries(${test} PRIVATE gtest_main signals)
add_coverage(${test})
add_dependencies(check ${test})
add_test(NAME ${PROJECT_NAME} COMMAND ${test})
