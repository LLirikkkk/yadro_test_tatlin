#include "utils.h"

#include <format>
#include <fstream>
#include <iostream>

namespace tape::test {

TestDir::TestDir()
    : path_("tmp-tests") {
    std::filesystem::create_directories(path_);
}

TestDir::~TestDir() {
    std::error_code ec;
    std::filesystem::remove_all(path_, ec);

    if (ec) {
        std::cerr << std::format("Could not remove directory:{}, because: {}", path_.string(), ec.message())
                  << std::endl;
    }
}

void write_binary_ints(const std::filesystem::path& path, std::span<const std::int32_t> values) {
    std::ofstream fout(path, std::ios_base::out | std::ios::binary | std::ios::trunc);
    if (fout.fail()) {
        throw std::runtime_error("Could not create test file");
    }

    for (const auto value : values) {
        fout.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
}

std::vector<std::int32_t> read_binary_ints(const std::filesystem::path& path) {
    std::ifstream fin(path, std::ios_base::in | std::ios_base::binary);
    if (fin.fail()) {
        throw std::runtime_error("Could not open test file");
    }

    std::vector<std::int32_t> values;
    std::int32_t value = 0;
    while (fin.read(reinterpret_cast<char*>(&value), sizeof(value))) {
        values.push_back(value);
    }

    if (fin.bad()) {
        throw std::runtime_error("Could not read test file");
    }

    return values;
}

} // namespace tape::test
