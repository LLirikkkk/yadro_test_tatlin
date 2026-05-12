#include "sort/tape-sorter.h"

#include "exceptions/exceptions.h"
#include "tape/file-tape.h"
#include "utils/temp-dir-guard.h"

#include <queue>

#include <algorithm>
#include <filesystem>
#include <span>

namespace tape {

TapeSorter::TapeSorter(const Config& config) noexcept
    : config_(config) {}

void TapeSorter::sort(ITape& input, ITape& output) {
    if (input.size() != output.size()) {
        throw TapeException("Sizes of input tape and output tape are not equal");
    }

    input.rewind_to_begin();
    output.rewind_to_begin();

    if (input.empty()) {
        return;
    }

    const std::size_t elements_in_block = get_elements_in_block();
    if (elements_in_block <= 1) {
        throw TapeException("Could not sort tape, because memory limit is exceeded");
    }

    reset_unique_index_in_tmp_dir();
    auto temp_dir_guard = detail::TempDirGuard("tmp");

    auto temp_tapes_info = get_sorted_temp_tapes(input, elements_in_block);
    merge_sorted_temp_tapes(temp_tapes_info, output);
}

std::vector<TapeSorter::TapeInfo> TapeSorter::get_sorted_temp_tapes(ITape& input, const std::size_t elements_in_block) {
    std::vector<TapeInfo> temp_tapes_info;
    temp_tapes_info.reserve((input.size() + elements_in_block - 1) / elements_in_block);
    while (!input.is_end()) {
        auto block = read_tape(input, elements_in_block);
        std::ranges::sort(block.begin(), block.end(), std::less{});

        auto path = get_unique_path_in_tmp_dir();
        FileTape temp_tape(path, block.size(), input.get_config());

        write_tape(temp_tape, block);
        temp_tapes_info.emplace_back(path, input.get_config());
    }

    return temp_tapes_info;
}

void TapeSorter::merge_sorted_temp_tapes(std::vector<TapeInfo>& temp_tapes_info, ITape& output) {
    while (temp_tapes_info.size() > 1) {
        std::size_t merge_ways = std::min(temp_tapes_info.size(), get_elements_in_block());
        auto merged_temp_tape_info = k_way_merge_temp_tapes(std::span(temp_tapes_info.begin(), merge_ways));
        auto removed_left_bound_it = temp_tapes_info.begin();
        auto removed_right_bound_it = temp_tapes_info.begin();
        std::advance(removed_right_bound_it, merge_ways);
        temp_tapes_info.erase(removed_left_bound_it, removed_right_bound_it);
        temp_tapes_info.push_back(std::move(merged_temp_tape_info));
    }

    FileTape result(temp_tapes_info.front().path_, temp_tapes_info.front().config_);
    while (!result.is_end()) {
        output.write(result.read());
        output.move_right();
        result.move_right();
    }
}

TapeSorter::TapeInfo TapeSorter::k_way_merge_temp_tapes(std::span<TapeInfo> temp_tapes_info) {
    using TapeValue = std::int32_t;
    using TapeIndex = std::size_t;

    auto temp_tapes = get_tapes_from_tapes_info(temp_tapes_info);

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

    auto merged_temp_tape_path = get_unique_path_in_tmp_dir();
    FileTape merged_temp_tape(merged_temp_tape_path, merged_size, temp_tapes.front().get_config());
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

    merged_temp_tape.rewind_to_begin();

    return {.path_ = merged_temp_tape_path, .config_ = merged_temp_tape.get_config()};
}

std::vector<FileTape> TapeSorter::get_tapes_from_tapes_info(std::span<TapeInfo> tapes) {
    std::vector<FileTape> temp_tapes;
    temp_tapes.reserve(tapes.size());
    for (auto& [path, config] : tapes) {
        temp_tapes.emplace_back(path, config);
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

std::string TapeSorter::get_unique_path_in_tmp_dir() noexcept {
    return std::filesystem::path("tmp").append(std::to_string(get_unique_index_in_tmp_dir())).string();
}

std::size_t TapeSorter::get_elements_in_block() const noexcept {
    return config_.memory_limit_ / ELEMENT_SIZE;
}

std::size_t TapeSorter::get_unique_index_in_tmp_dir() noexcept {
    return unique_index_in_tmp_dir_++;
}

void TapeSorter::reset_unique_index_in_tmp_dir() noexcept {
    unique_index_in_tmp_dir_ = 0;
}

} // namespace tape
