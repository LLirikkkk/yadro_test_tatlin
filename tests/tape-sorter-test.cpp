#include "sort/tape-sorter.h"

#include "exceptions/exceptions.h"
#include "gtest/gtest.h"
#include "tape/file-tape.h"
#include "utils/utils.h"

namespace {

class TapeSorterTest : public ::testing::Test {
  protected:
    void write_input(const std::vector<std::int32_t>& values) const {
        tape::test::write_binary_ints(input_path_, values);
    }

    std::vector<std::int32_t> read_output() const {
        return tape::test::read_binary_ints(output_path_);
    }

    void run_sort(const std::vector<std::int32_t>& input_values, const std::size_t memory_limit) const {
        write_input(input_values);

        tape::FileTape input_tape(input_path_, tape::ITape::Config{});
        tape::FileTape output_tape(output_path_, input_tape.size(), tape::ITape::Config{});

        tape::TapeSorter sorter(tape::TapeSorter::Config{.memory_limit_ = memory_limit});
        sorter.sort(input_tape, output_tape);
    }

    tape::test::TestDir dir_;
    std::filesystem::path input_path_ = dir_.path_ / "input.bin";
    std::filesystem::path output_path_ = dir_.path_ / "output.bin";
};

} // namespace

namespace tape::test {

TEST_F(TapeSorterTest, SortsEmptyTape) {
    run_sort({}, 1024);

    EXPECT_EQ(read_output(), (std::vector<std::int32_t>{}));
}

TEST_F(TapeSorterTest, SortsSingleElementTape) {
    run_sort({1}, 1024);

    EXPECT_EQ(read_output(), (std::vector<std::int32_t>{1}));
}

TEST_F(TapeSorterTest, SortsAlreadySortedTape) {
    run_sort({1, 2, 3, 4, 5}, 1024);

    EXPECT_EQ(read_output(), (std::vector<std::int32_t>{1, 2, 3, 4, 5}));
}

TEST_F(TapeSorterTest, SortsReversedSortedTape) {
    run_sort({5, 4, 3, 2, 1}, 1024);

    EXPECT_EQ(read_output(), (std::vector<std::int32_t>{1, 2, 3, 4, 5}));
}

TEST_F(TapeSorterTest, SortsNegativeValues) {
    run_sort({-10, 3, -5, 0, -1}, 1024);

    EXPECT_EQ(read_output(), (std::vector<std::int32_t>{-10, -5, -1, 0, 3}));
}

TEST_F(TapeSorterTest, SortsDuplicateValues) {
    run_sort({5, 1, 5, 2, 5, 1}, 1024);

    EXPECT_EQ(read_output(), (std::vector<std::int32_t>{1, 1, 2, 5, 5, 5}));
}

TEST_F(TapeSorterTest, SortsWhenEverythingFitsInMemory) {
    constexpr std::size_t memory_limit = 100 * sizeof(std::int32_t);

    run_sort({9, 4, 1, 7, 3, 8}, memory_limit);

    EXPECT_EQ(read_output(), (std::vector<std::int32_t>{1, 3, 4, 7, 8, 9}));
}

TEST_F(TapeSorterTest, SortsWithMultipleBlocks) {
    constexpr std::size_t memory_limit = 3 * sizeof(std::int32_t);

    run_sort({9, 4, 1, 7, 3, 8, 2, 6, 5}, memory_limit);

    EXPECT_EQ(read_output(), (std::vector<std::int32_t>{1, 2, 3, 4, 5, 6, 7, 8, 9}));
}

TEST_F(TapeSorterTest, SortsWithMultiPassMerge) {
    constexpr std::size_t memory_limit = 2 * sizeof(std::int32_t);

    std::vector<std::int32_t> values;
    values.reserve(100);
    for (std::int32_t i = 100; i >= 1; --i) {
        values.push_back(i);
    }

    run_sort(values, memory_limit);

    std::vector<std::int32_t> expected;
    expected.reserve(100);
    for (std::int32_t i = 1; i <= 100; ++i) {
        expected.push_back(i);
    }

    EXPECT_EQ(read_output(), expected);
}

TEST_F(TapeSorterTest, ThrowsWhenTapesSizesDiffer) {
    write_input({2, 3, 1});

    FileTape input_tape(input_path_, ITape::Config{});
    FileTape output_tape(output_path_, 2, ITape::Config{});

    TapeSorter sorter(TapeSorter::Config{.memory_limit_ = 1024});

    EXPECT_THROW(sorter.sort(input_tape, output_tape), TapeException);
}

TEST_F(TapeSorterTest, ThrowsWhenMemoryLimitTooSmall) {
    write_input({2, 3, 1});

    FileTape input_tape(input_path_, ITape::Config{});
    FileTape output_tape(output_path_, input_tape.size(), ITape::Config{});

    TapeSorter sorter(TapeSorter::Config{.memory_limit_ = sizeof(std::int32_t)});

    EXPECT_THROW(sorter.sort(input_tape, output_tape), TapeException);
}

} // namespace tape::test
