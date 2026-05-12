#pragma once

#include "tape/file-tape.h"
#include "tape/tape.h"

#include <cstddef>
#include <filesystem>
#include <span>
#include <vector>

#ifdef __unix__
#include <sys/resource.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <stdio.h>
#endif

namespace tape {

class TapeSorter {
  private:
    static constexpr std::size_t ELEMENT_SIZE = sizeof(std::int32_t);

    struct TapeInfo {
        std::string path_;
        ITape::Config config_;
    };

  public:
    struct Config {
        std::size_t memory_limit_ = 0;
    };

    explicit TapeSorter(const Config& config) noexcept;

    void sort(ITape& input, ITape& output);

  private:
    std::vector<TapeInfo> get_sorted_temp_tapes(ITape& input, std::size_t elements_in_block);

    void merge_temp_tapes(std::vector<TapeInfo>& temp_tapes_info, ITape& output);

    TapeInfo k_way_merge_temp_tapes(std::span<TapeInfo> temp_tapes);

    static std::vector<FileTape> get_tapes_from_tapes_info(std::span<TapeInfo> tapes);

    static std::vector<std::int32_t> read_tape(ITape& input, std::size_t n);

    static void write_tape(ITape& output, std::span<std::int32_t> buff);

    std::size_t get_elements_in_block() const noexcept;

    std::string get_unique_path_in_tmp_dir() noexcept;

    std::size_t get_unique_index_in_tmp_dir() noexcept;

    void reset_unique_index_in_tmp_dir() noexcept;

    static std::size_t get_fd_limit() noexcept;

    std::size_t unique_index_in_tmp_dir_ = 0;
    Config config_;
};

} // namespace tape
