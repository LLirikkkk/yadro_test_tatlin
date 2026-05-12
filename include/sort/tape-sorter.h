#pragma once

#include "tape/file-tape.h"
#include "tape/tape.h"
#include "utils/temp-dir-guard.h"

#include <cstddef>
#include <filesystem>
#include <span>
#include <vector>

namespace tape {

class TapeSorter {
  private:
    static constexpr std::size_t ELEMENT_SIZE = sizeof(std::int32_t);
    static constexpr std::size_t MAX_MERGE_WAYS = 16;

    struct TapeInfo {
        std::string path_;
    };

  public:
    struct Config {
        std::size_t memory_limit_ = 0;
    };

    explicit TapeSorter(const Config& config) noexcept;

    void sort(ITape& input, ITape& output) const;

  private:
    static std::vector<TapeInfo>
    get_sorted_temp_tapes(ITape& input, std::size_t elements_in_block, detail::TempDirGuard& dir_guard);

    void merge_temp_tapes(
        std::vector<TapeInfo>& temp_tapes_info,
        ITape& output,
        detail::TempDirGuard& dir_guard,
        const ITape::Config& config
    ) const;

    static TapeInfo k_way_merge_temp_tapes(
        std::span<TapeInfo> temp_tapes_info,
        detail::TempDirGuard& dir_guard,
        const ITape::Config& config
    );

    static std::vector<FileTape> get_tapes_from_tapes_info(std::span<TapeInfo> tapes, const ITape::Config& config);

    static std::vector<std::int32_t> read_tape(ITape& input, std::size_t n);

    static void write_tape(ITape& output, std::span<std::int32_t> buff);

    std::size_t max_elements_in_ram() const noexcept;

    Config config_;
};

} // namespace tape
