add_library(tape_core)

target_compile_features(tape_core PUBLIC cxx_std_20)
set_target_properties(tape_core PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(
        tape_core

        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_sources(
        tape_core

        PRIVATE
        sort/tape-sorter.cpp
        tape/file-tape.cpp
        tape/tape.cpp
        utils/temp-dir-guard.cpp
        exceptions/exceptions.cpp

        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include
        FILES
        ${PROJECT_SOURCE_DIR}/include/sort/tape-sorter.h
        ${PROJECT_SOURCE_DIR}/include/tape/tape.h
        ${PROJECT_SOURCE_DIR}/include/tape/file-tape.h
        ${PROJECT_SOURCE_DIR}/include/utils/temp-dir-guard.h
        ${PROJECT_SOURCE_DIR}/include/exceptions/exceptions.h
)
