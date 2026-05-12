#include "sort/tape-sorter.h"

#include "exceptions/exceptions.h"
#include "tape/file-tape.h"
#include "tape/tape.h"
#include "utils/temp-dir-guard.h"

#include <queue>

#include <algorithm>
#include <filesystem>
#include <span>

namespace tape {

TapeSorter::TapeSorter(const Config& config) noexcept
    : config_(config) {}

void TapeSorter::sort(ITape& input, ITape& output) const {
    if (input.size() != output.size()) {
        throw TapeException("Sizes of input tape and output tape are not equal");
    }

    input.rewind_to_begin();
    output.rewind_to_begin();

    if (input.empty()) {
        return;
    }

    const std::size_t elements_in_block = max_elements_in_ram();
    if (elements_in_block <= 1) {
        throw TapeException("Could not sort tape, because memory limit is exceeded");
    }

    auto temp_dir_guard = detail::TempDirGuard("tmp");

    auto temp_tapes_info = get_sorted_temp_tapes(input, elements_in_block, temp_dir_guard);
    merge_temp_tapes(temp_tapes_info, output, temp_dir_guard, input.get_config());
}

std::vector<TapeSorter::TapeInfo>
TapeSorter::get_sorted_temp_tapes(ITape& input, const std::size_t elements_in_block, detail::TempDirGuard& dir_guard) {
    std::vector<TapeInfo> temp_tapes_info;
    temp_tapes_info.reserve((input.size() + elements_in_block - 1) / elements_in_block);
    while (!input.is_end()) {
        auto block = read_tape(input, elements_in_block);
        std::ranges::sort(block.begin(), block.end(), std::less{});

        auto path = dir_guard.add_file().string();
        FileTape temp_tape(path, block.size(), input.get_config());

        write_tape(temp_tape, block);
        temp_tapes_info.emplace_back(path);
    }

    return temp_tapes_info;
}

void TapeSorter::merge_temp_tapes(
    std::vector<TapeInfo>& temp_tapes_info,
    ITape& output,
    detail::TempDirGuard& dir_guard,
    const ITape::Config& config
) const {
    while (temp_tapes_info.size() > 1) {
        std::size_t merge_ways = std::min({temp_tapes_info.size(), max_elements_in_ram(), MAX_MERGE_WAYS});

        std::vector<std::filesystem::path> file_paths;
        file_paths.reserve(merge_ways);
        for (std::size_t i = 0; i < merge_ways; ++i) {
            file_paths.emplace_back(temp_tapes_info[i].path_);
        }

        auto merged_temp_tape_info =
            k_way_merge_temp_tapes(std::span(temp_tapes_info.begin(), merge_ways), dir_guard, config);
        auto removed_left_bound_it = temp_tapes_info.begin();
        auto removed_right_bound_it = temp_tapes_info.begin();
        std::advance(removed_right_bound_it, merge_ways);
        temp_tapes_info.erase(removed_left_bound_it, removed_right_bound_it);

        for (const auto& file_path : file_paths) {
            dir_guard.remove_file(file_path);
        }

        temp_tapes_info.push_back(std::move(merged_temp_tape_info));
    }

    FileTape result(temp_tapes_info.front().path_, config);
    while (!result.is_end()) {
        output.write(result.read());
        output.move_right();
        result.move_right();
    }
}

TapeSorter::TapeInfo TapeSorter::k_way_merge_temp_tapes(
    std::span<TapeInfo> temp_tapes_info,
    detail::TempDirGuard& dir_guard,
    const ITape::Config& config
) {
    using TapeValue = std::int32_t;
    using TapeIndex = std::size_t;

    auto temp_tapes = get_tapes_from_tapes_info(temp_tapes_info, config);

    std::priority_queue<std::pair<TapeValue, TapeIndex>, std::vector<std::pair<TapeValue, TapeIndex>>, std::greater<>>
        pq;
    std::size_t merged_size = 0;
    for (std::size_t i = 0; i < temp_tapes.size(); ++i) {
        if (!temp_tapes[i].is_end()) {
            pq.emplace(temp_tapes[i].read(), i);
            temp_tapes[i].move_right();
        }

        merged_size += temp_tapes[i].size();
    }

    auto merged_temp_tape_path = dir_guard.add_file().string();
    FileTape merged_temp_tape(merged_temp_tape_path, merged_size, config);
    while (!pq.empty()) {
        auto [tape_value, tape_index] = pq.top();
        pq.pop();

        merged_temp_tape.write(tape_value);
        merged_temp_tape.move_right();
        if (!temp_tapes[tape_index].is_end()) {
            pq.emplace(temp_tapes[tape_index].read(), tape_index);
            temp_tapes[tape_index].move_right();
        }
    }

    return {.path_ = merged_temp_tape_path};
}

std::vector<FileTape> TapeSorter::get_tapes_from_tapes_info(std::span<TapeInfo> tapes, const ITape::Config& config) {
    std::vector<FileTape> temp_tapes;
    temp_tapes.reserve(tapes.size());
    for (auto& [path_] : tapes) {
        temp_tapes.emplace_back(path_, config);
    }

    return temp_tapes;
}

std::vector<std::int32_t> TapeSorter::read_tape(ITape& input, const std::size_t n) {
    std::vector<std::int32_t> buff;
    buff.reserve(n);
    for (std::size_t i = 0; i < n && !input.is_end(); ++i, input.move_right()) {
        buff.push_back(input.read());
    }

    return buff;
}

void TapeSorter::write_tape(ITape& output, std::span<std::int32_t> buff) {
    for (std::size_t i = 0; i < buff.size() && !output.is_end(); ++i, output.move_right()) {
        output.write(buff[i]);
    }
}

std::size_t TapeSorter::max_elements_in_ram() const noexcept {
    return config_.memory_limit_ / ELEMENT_SIZE;
}

} // namespace tape
