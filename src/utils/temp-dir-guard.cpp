#include "utils/temp-dir-guard.h"

#include <utility>

namespace tape::detail {

TempDirGuard::TempDirGuard(std::filesystem::path dir)
    : dir_(std::move(dir)) {
    std::filesystem::create_directories(dir_);
}

TempDirGuard::~TempDirGuard() {
    std::error_code ec;
    std::filesystem::remove_all(dir_, ec);
}

} // namespace tape::detail
