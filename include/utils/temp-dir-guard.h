#pragma once
#include <filesystem>

namespace tape::detail {

class TempDirGuard {
  public:
    explicit TempDirGuard(std::filesystem::path dir);

    std::filesystem::path add_file();

    void remove_file(const std::filesystem::path& path) const;

    ~TempDirGuard();

  private:
    std::filesystem::path get_unique_path_in_tmp_dir();

    std::size_t get_unique_index_in_tmp_dir() noexcept;

    std::filesystem::path dir_;
    std::size_t unique_index_in_dir_ = 0;
};

} // namespace tape::detail
