#pragma once
#include <filesystem>

namespace tape::detail {

class TempDirGuard {
  public:
    explicit TempDirGuard(std::filesystem::path dir);

    ~TempDirGuard();

  private:
    std::filesystem::path dir_;
};

} // namespace tape::detail
