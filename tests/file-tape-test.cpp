#include "tape/file-tape.h"

#include "exceptions/exceptions.h"
#include "utils/utils.h"

#include <gtest/gtest.h>

namespace {

class FileTapeTest : public ::testing::Test {
  protected:
    tape::test::TestDir dir_;
    std::filesystem::path path_ = dir_.path_ / "file.bin";

    void write_values_to_file(const std::vector<std::int32_t>& values) const {
        tape::test::write_binary_ints(path_, values);
    }
};

} // namespace

namespace tape::test {

TEST_F(FileTapeTest, OpensExistingTape) {
    write_values_to_file({1, 2, 3});

    FileTape tape(path_, ITape::Config{});

    EXPECT_FALSE(tape.empty());
    EXPECT_EQ(tape.size(), 3);
    EXPECT_TRUE(tape.is_begin());
    EXPECT_FALSE(tape.is_end());
}

TEST_F(FileTapeTest, OpensEmptyTape) {
    write_values_to_file({});

    FileTape tape(path_, ITape::Config{});

    EXPECT_TRUE(tape.empty());
    EXPECT_EQ(tape.size(), 0);
    EXPECT_TRUE(tape.is_begin());
    EXPECT_TRUE(tape.is_end());
}

TEST_F(FileTapeTest, CreateTapeWithRequestedSize) {
    {
        FileTape tape(path_, 3, ITape::Config{});

        EXPECT_FALSE(tape.empty());
        EXPECT_EQ(tape.size(), 3);
        EXPECT_TRUE(tape.is_begin());
        EXPECT_FALSE(tape.is_end());
    }

    EXPECT_EQ(read_binary_ints(path_), (std::vector<std::int32_t>{0, 0, 0}));
}

TEST_F(FileTapeTest, CreateTapeWithRequestedSizeZero) {
    FileTape tape(path_, 0, ITape::Config{});

    EXPECT_TRUE(tape.empty());
    EXPECT_EQ(tape.size(), 0);
    EXPECT_TRUE(tape.is_begin());
    EXPECT_TRUE(tape.is_end());
}

TEST_F(FileTapeTest, ReadsValuesSequentially) {
    write_values_to_file({1, 2, 3});

    FileTape tape(path_, ITape::Config{});

    EXPECT_FALSE(tape.empty());
    EXPECT_EQ(tape.size(), 3);
    EXPECT_TRUE(tape.is_begin());
    EXPECT_FALSE(tape.is_end());

    EXPECT_EQ(tape.read(), 1);
    EXPECT_EQ(tape.read(), 1);

    tape.move_right();

    EXPECT_EQ(tape.read(), 2);

    tape.move_right();

    EXPECT_EQ(tape.read(), 3);

    tape.move_right();

    EXPECT_FALSE(tape.is_begin());
    EXPECT_TRUE(tape.is_end());
}

TEST_F(FileTapeTest, MovesLeftAndRight) {
    write_values_to_file({1, 2, 3});

    FileTape tape(path_, ITape::Config{});

    EXPECT_TRUE(tape.is_begin());
    EXPECT_FALSE(tape.is_end());
    EXPECT_EQ(tape.read(), 1);

    tape.move_right();

    EXPECT_FALSE(tape.is_begin());
    EXPECT_FALSE(tape.is_end());
    EXPECT_EQ(tape.read(), 2);

    tape.move_right();

    EXPECT_FALSE(tape.is_begin());
    EXPECT_FALSE(tape.is_end());
    EXPECT_EQ(tape.read(), 3);

    tape.move_right();

    EXPECT_FALSE(tape.is_begin());
    EXPECT_TRUE(tape.is_end());

    tape.move_left();

    EXPECT_FALSE(tape.is_begin());
    EXPECT_FALSE(tape.is_end());
    EXPECT_EQ(tape.read(), 3);

    tape.move_left();

    EXPECT_FALSE(tape.is_begin());
    EXPECT_FALSE(tape.is_end());
    EXPECT_EQ(tape.read(), 2);

    tape.move_left();

    EXPECT_TRUE(tape.is_begin());
    EXPECT_FALSE(tape.is_end());
    EXPECT_EQ(tape.read(), 1);
}

TEST_F(FileTapeTest, WriteValuesToTape) {
    {
        FileTape tape(path_, 3, ITape::Config{});

        tape.write(1);
        tape.move_right();
        tape.write(2);
        tape.move_right();
        tape.write(3);
    }

    EXPECT_EQ(read_binary_ints(path_), (std::vector<std::int32_t>{1, 2, 3}));
}

TEST_F(FileTapeTest, OverwritesExistingValues) {
    write_values_to_file({1, 2, 3});

    {
        FileTape tape(path_, ITape::Config{});

        tape.move_right();

        EXPECT_EQ(tape.read(), 2);

        tape.write(10);

        EXPECT_EQ(tape.read(), 10);
    }

    EXPECT_EQ(read_binary_ints(path_), (std::vector<std::int32_t>{1, 10, 3}));
}

TEST_F(FileTapeTest, RewindsToBeginAndEnd) {
    write_values_to_file({1, 2, 3});

    FileTape tape(path_, ITape::Config{});

    tape.move_right();
    tape.move_right();

    EXPECT_FALSE(tape.is_begin());
    EXPECT_EQ(tape.read(), 3);

    tape.rewind_to_begin();

    EXPECT_TRUE(tape.is_begin());
    EXPECT_EQ(tape.read(), 1);

    tape.rewind_to_end();

    EXPECT_TRUE(tape.is_end());
}

TEST_F(FileTapeTest, ThrowsWhenMovingLeftFromBegin) {
    write_values_to_file({1});

    FileTape tape(path_, ITape::Config{});

    EXPECT_TRUE(tape.is_begin());
    EXPECT_THROW(tape.move_left(), TapeException);
}

TEST_F(FileTapeTest, ThrowsWhenMovingRightFromEnd) {
    write_values_to_file({1});

    FileTape tape(path_, ITape::Config{});

    tape.move_right();

    EXPECT_TRUE(tape.is_end());
    EXPECT_THROW(tape.move_right(), TapeException);
}

TEST_F(FileTapeTest, ThrowsWhenReadingAtEnd) {
    write_values_to_file({1});

    FileTape tape(path_, ITape::Config{});

    tape.move_right();

    EXPECT_TRUE(tape.is_end());
    EXPECT_THROW(tape.read(), TapeException);
}

TEST_F(FileTapeTest, ThrowsWhenWritingAtEnd) {
    write_values_to_file({1});

    FileTape tape(path_, ITape::Config{});

    tape.move_right();

    EXPECT_TRUE(tape.is_end());
    EXPECT_THROW(tape.write(1), TapeException);
}

TEST_F(FileTapeTest, ThrowsOnInvalidExistingTape) {
    {
        std::ofstream out(path_, std::ios::out | std::ios::binary | std::ios::trunc);
        out.put('\x01');
    }

    EXPECT_THROW(FileTape(path_, ITape::Config{}), TapeException);
}

} // namespace tape::test
