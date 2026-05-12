add_executable(tape)

target_compile_features(tape PUBLIC cxx_std_20)
set_target_properties(tape PROPERTIES CXX_EXTENSIONS OFF)

target_sources(
        tape

        PRIVATE
        main.cpp
        config-loader.cpp

        PUBLIC
        FILE_SET HEADERS
        FILES
        config-loader.h
)

target_link_libraries(
        tape

        PRIVATE
        tape_core
)
