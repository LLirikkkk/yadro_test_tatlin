#include "config/config-loader.h"

#include "gtest/gtest.h"
#include "utils/utils.h"

#include <fstream>

namespace {

class ConfigLoaderTest : public ::testing::Test {
  protected:
    void write_config(std::string_view content) const {
        std::ofstream fout(config_path_);
        fout << content;
    }

    tape::test::TestDir dir_;
    std::filesystem::path config_path_ = dir_.path_ / "config.txt";

    tape::ConfigLoader loader_;
};

} // namespace

namespace tape::test {

TEST_F(ConfigLoaderTest, ReadsValidConfig) {
    write_config(
        "read_delay_ms=1\n"
        "write_delay_ms=1\n"
        "move_delay_ms=1\n"
        "rewind_delay_ms=1\n"
        "memory_limit_bytes=1024\n"
    );

    loader_.read_config(config_path_);

    const auto& tape_config = loader_.get_tape_config();
    const auto& tape_sorter_config = loader_.get_tape_sorter_config();

    EXPECT_EQ(tape_config.read_delay_.count(), 1);
    EXPECT_EQ(tape_config.write_delay_.count(), 1);
    EXPECT_EQ(tape_config.move_delay_.count(), 1);
    EXPECT_EQ(tape_config.rewind_delay_.count(), 1);

    EXPECT_EQ(tape_sorter_config.memory_limit_, 1024);
}

TEST_F(ConfigLoaderTest, ThrowsOnEmptyFile) {
    write_config("");

    EXPECT_THROW(loader_.read_config(config_path_), std::runtime_error);
}

TEST_F(ConfigLoaderTest, ThrowsOnMissingSeparator) {
    write_config(
        "read_delay_ms=1\n"
        "write_delay_ms=1\n"
        "move_delay_ms 1\n"
        "rewind_delay_ms=1\n"
        "memory_limit_bytes=1024\n"
    );

    EXPECT_THROW(loader_.read_config(config_path_), std::runtime_error);
}

TEST_F(ConfigLoaderTest, ThrowsOnEmptyValue) {
    write_config(
        "read_delay_ms=1\n"
        "write_delay_ms=1\n"
        "move_delay_ms=\n"
        "rewind_delay_ms=1\n"
        "memory_limit_bytes=1024\n"
    );

    EXPECT_THROW(loader_.read_config(config_path_), std::runtime_error);
}

TEST_F(ConfigLoaderTest, ThrowsOnNegativeValue) {
    write_config(
        "read_delay_ms=1\n"
        "write_delay_ms=1\n"
        "move_delay_ms=-1\n"
        "rewind_delay_ms=1\n"
        "memory_limit_bytes=1024\n"
    );

    EXPECT_THROW(loader_.read_config(config_path_), std::runtime_error);
}

TEST_F(ConfigLoaderTest, ThrowsOnInvalidValue) {
    write_config(
        "read_delay_ms=1\n"
        "write_delay_ms=1\n"
        "move_delay_ms=abc\n"
        "rewind_delay_ms=1\n"
        "memory_limit_bytes=1024\n"
    );

    EXPECT_THROW(loader_.read_config(config_path_), std::runtime_error);
}

TEST_F(ConfigLoaderTest, ThrowsOnTrailingGarbage) {
    write_config(
        "read_delay_ms=1\n"
        "write_delay_ms=1\n"
        "move_delay_ms=1abc\n"
        "rewind_delay_ms=1\n"
        "memory_limit_bytes=1024\n"
    );

    EXPECT_THROW(loader_.read_config(config_path_), std::runtime_error);
}

TEST_F(ConfigLoaderTest, ThrowsOnDuplicateKeys) {
    write_config(
        "read_delay_ms=1\n"
        "write_delay_ms=1\n"
        "move_delay_ms=1\n"
        "rewind_delay_ms=1\n"
        "memory_limit_bytes=1024\n"
        "read_delay_ms=2\n"
    );

    EXPECT_THROW(loader_.read_config(config_path_), std::runtime_error);
}

TEST_F(ConfigLoaderTest, ThrowsOnUnknownKey) {
    write_config(
        "read_delay_ms=1\n"
        "write_delay_ms=1\n"
        "move_delay_ms=1\n"
        "rewind_delay_ms=1\n"
        "memory_limit_bytes=1024\n"
        "unknown_key=123\n"
    );

    EXPECT_THROW(loader_.read_config(config_path_), std::runtime_error);
}

TEST_F(ConfigLoaderTest, ThrowsOnMissingKeys) {
    write_config(
        "read_delay_ms=1\n"
        "write_delay_ms=1\n"
    );

    EXPECT_THROW(loader_.read_config(config_path_), std::runtime_error);
}

TEST_F(ConfigLoaderTest, ThrowsOnTooLongLine) {
    std::string long_line(5000, '1');

    write_config("read_delay_ms=" + long_line + "\n");

    EXPECT_THROW(loader_.read_config(config_path_), std::runtime_error);
}

} // namespace tape::test
