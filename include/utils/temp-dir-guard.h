#pragma once
#include <filesystem>

namespace tape::detail {

/**
 * @brief RAII guard for a temporary directory.
 *
 * Creates the directory on construction and recursively removes it on destruction. Provides methods to create
 * unique temporary files inside that directory.
 */
class TempDirGuard {
  public:
    /**
     * @brief Creates the temporary directory.
     * @param dir Path to the directory.
     */
    explicit TempDirGuard(std::filesystem::path dir);

    /**
     * @brief Generates a unique path for a new temporary file.
     * @return Path to a file inside the managed directory.
     */
    std::filesystem::path add_file();

    /**
     * @brief Removes a file it is inside the managed directory.
     * @param path Path to the file.
     * @throws TapeException If the file is not inside the directory.
     * @note On removal error, prints a message to std::cerr, but does not throw.
     */
    void remove_file(const std::filesystem::path& path) const;

    /**
     * @brief Recursively removes the temporary directory and all its contents.
     */
    ~TempDirGuard();

  private:
    std::filesystem::path get_unique_path_in_tmp_dir();

    std::size_t get_unique_index_in_tmp_dir() noexcept;

    std::filesystem::path dir_;
    std::size_t unique_index_in_dir_ = 0;
};

} // namespace tape::detail
