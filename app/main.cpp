#include "../include/config/config-loader.h"
#include "sort/tape-sorter.h"
#include "tape/file-tape.h"

#include <exception>
#include <filesystem>
#include <format>
#include <iostream>
#include <string>

int main(int argc, char* const* argv) {
    if (argc != 3) {
        std::cerr << "Arguments <input file> <output file> required" << std::endl;
        return 1;
    }

    try {
        if (!std::filesystem::exists("config.txt")) {
            std::cerr << "Could not find config.txt" << std::endl;
            return 1;
        }

        tape::ConfigLoader loader;
        loader.read_config("config.txt");

        tape::FileTape input_tape(argv[1], loader.get_tape_config());
        tape::FileTape output_tape(argv[2], input_tape.size(), loader.get_tape_config());

        tape::TapeSorter sorter(loader.get_tape_sorter_config());

        sorter.sort(input_tape, output_tape);
    } catch (const std::exception& e) {
        std::cerr << std::format("Error: {}", e.what()) << std::endl;
        return 1;
    }

    return 0;
}
