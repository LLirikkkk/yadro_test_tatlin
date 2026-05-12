#include "utils/temp-dir-guard.h"

#include "exceptions/exceptions.h"

#include <format>
#include <iostream>
#include <utility>

namespace tape::detail {

TempDirGuard::TempDirGuard(std::filesystem::path dir)
    : dir_(std::move(dir)) {
    std::filesystem::create_directories(dir_);
}

std::filesystem::path TempDirGuard::add_file() {
    return get_unique_path_in_tmp_dir();
}

void TempDirGuard::remove_file(const std::filesystem::path& path) const {
    if (path.parent_path() != dir_) {
        throw TapeException("Path is not in directory that this guard bounded to");
    }

    std::error_code ec;
    std::filesystem::remove(path, ec);
    if (ec) {
        std::cerr << std::format("Could not remove file: {}, because: {}", path.string(), ec.message()) << std::endl;
    }
}

TempDirGuard::~TempDirGuard() {
    std::error_code ec;
    std::filesystem::remove_all(dir_, ec);
    if (ec) {
        std::cerr << std::format("Could not remove directory: dir, because: {}", ec.message()) << std::endl;
    }
}

std::filesystem::path TempDirGuard::get_unique_path_in_tmp_dir() {
    return dir_ / std::to_string(get_unique_index_in_tmp_dir());
}

std::size_t TempDirGuard::get_unique_index_in_tmp_dir() noexcept {
    return unique_index_in_dir_++;
}

} // namespace tape::detail
