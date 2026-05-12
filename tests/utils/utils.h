#pragma once

#include <filesystem>
#include <span>
#include <vector>

namespace tape::test {

struct TestDir {
    TestDir();

    ~TestDir();

    std::filesystem::path path_;
};

void write_binary_ints(const std::filesystem::path& path, std::span<const std::int32_t> values);

std::vector<std::int32_t> read_binary_ints(const std::filesystem::path& path);

} // namespace tape::test
