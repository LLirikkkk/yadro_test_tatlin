cmake_minimum_required(VERSION 3.28)

project(yadro_test_tatlin VERSION 1.0 LANGUAGES CXX)

option(BUILD_APP "Build application" ON)
option(BUILD_TESTS "Build tests" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(src)

if (BUILD_APP)
    add_subdirectory(app)
endif ()

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()
