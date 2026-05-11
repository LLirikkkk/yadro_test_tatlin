#pragma once

#include "tape/file-tape.h"
#include "tape/tape.h"

#include <cstddef>
#include <filesystem>
#include <vector>

namespace tape {

class TapeSorter {
  private:
    static constexpr std::size_t ELEMENT_SIZE = sizeof(std::int32_t);
    static std::size_t UNIQUE_INDEX_IN_TMP_DIR;

  public:
    struct Config {
        std::size_t memory_limit_ = 0;
    };

    explicit TapeSorter(const Config& config) noexcept;

    void sort(ITape& input, ITape& output) const;

  private:
    static std::vector<FileTape> get_sorted_temp_tapes(ITape& input, std::size_t elements_in_block);

    void merge_sorted_temp_tapes(std::vector<FileTape>& temp_tapes, ITape& output) const;

    static FileTape merge_sorted_temp_tapes_impl(std::span<FileTape> temp_tapes);

    static std::vector<std::int32_t> read_tape(ITape& input, std::size_t n);

    static void write_tape(ITape& output, std::span<std::int32_t> buff);

    std::size_t get_elements_in_block() const noexcept;

    static std::filesystem::path get_unique_path_in_tmp_dir() noexcept;

    static std::size_t get_unique_index_in_tmp_dir() noexcept;

    static void reset_unique_index_in_tmp_dir() noexcept;

    Config config_;
};

} // namespace tape
