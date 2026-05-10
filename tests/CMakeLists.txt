include(Dependencies)

add_executable(tape_tests)

target_compile_features(tape_tests PUBLIC cxx_std_20)
set_target_properties(tape_tests PROPERTIES CXX_EXTENSIONS OFF)

target_sources(
        tape_tests

        PRIVATE
        tape-sorter-test.cpp
)

find_package(GTest REQUIRED)

target_link_libraries(
        tape_tests

        PRIVATE
        tape_core
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tape_tests)
