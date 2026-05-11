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

void TapeSorter::sort(ITape& input, ITape& output) const {
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

    auto temp_tapes = get_sorted_temp_tapes(input, elements_in_block);
    merge_sorted_temp_tapes(temp_tapes, output);
}

std::vector<FileTape> TapeSorter::get_sorted_temp_tapes(ITape& input, const std::size_t elements_in_block) {
    std::vector<FileTape> temp_tapes;
    temp_tapes.reserve((input.size() + elements_in_block - 1) / elements_in_block);

    while (!input.is_end()) {
        auto block = read_tape(input, elements_in_block);
        std::ranges::sort(block.begin(), block.end(), std::less{});

        temp_tapes.emplace_back(get_unique_path_in_tmp_dir().string(), block.size(), input.get_config());

        write_tape(temp_tapes.back(), block);
        temp_tapes.back().rewind_to_begin();
    }

    return temp_tapes;
}

void TapeSorter::merge_sorted_temp_tapes(std::vector<FileTape>& temp_tapes, ITape& output) const {
    while (temp_tapes.size() > 1) {
        std::size_t merge_ways = std::min(temp_tapes.size(), get_elements_in_block());
        auto merged_temp_tape = merge_sorted_temp_tapes_impl(std::span(temp_tapes.begin(), merge_ways));
        auto removed_left_bound_it = temp_tapes.begin();
        auto removed_right_bound_it = temp_tapes.begin();
        std::advance(removed_right_bound_it, merge_ways);
        temp_tapes.erase(removed_left_bound_it, removed_right_bound_it);
        temp_tapes.push_back(std::move(merged_temp_tape));
    }

    while (!temp_tapes.front().is_end()) {
        output.write(temp_tapes.front().read());
        output.move_right();
        temp_tapes.front().move_right();
    }
}

FileTape TapeSorter::merge_sorted_temp_tapes_impl(std::span<FileTape> temp_tapes) {
    using TapeValue = std::int32_t;
    using TapeIndex = std::size_t;

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

    FileTape merged_temp_tape(get_unique_path_in_tmp_dir().string(), merged_size, temp_tapes.front().get_config());
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

    return merged_temp_tape;
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

std::filesystem::path TapeSorter::get_unique_path_in_tmp_dir() noexcept {
    return std::filesystem::path("tmp").append(std::to_string(get_unique_index_in_tmp_dir()));
}

std::size_t TapeSorter::get_elements_in_block() const noexcept {
    return config_.memory_limit_ / ELEMENT_SIZE;
}

std::size_t TapeSorter::get_unique_index_in_tmp_dir() noexcept {
    return UNIQUE_INDEX_IN_TMP_DIR++;
}

void TapeSorter::reset_unique_index_in_tmp_dir() noexcept {
    UNIQUE_INDEX_IN_TMP_DIR = 0;
}

} // namespace tape
