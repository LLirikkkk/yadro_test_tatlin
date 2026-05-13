#pragma once

#include "sort/tape-sorter.h"
#include "tape/tape.h"

#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>

namespace tape {

/**
 * @brief Loads configuration from a text file.
 *
 * Reads a file with key=value pairs, validates correctness and converts values into <code>ITape::Config</code> and
 * <code>TapeSorter::Config</code> structures.
 */
class ConfigLoader {
  private:
    static constexpr std::size_t MAX_CONFIG_LINE_SIZE = 1024;
    static constexpr std::size_t MAX_CONFIG_FILE_SIZE = MAX_CONFIG_LINE_SIZE * 6;
    static constexpr std::size_t REQUIRED_KEYS_COUNT = 5;

  public:
    /**
     * @brief Parses the configuration file.
     * @param config_path Path to the configuration file.
     * @throws std::runtime_error If the file does not exist, is too large or is in invalid format.
     */
    void read_config(const std::filesystem::path& config_path);

    /**
     * @brief Returns the parsed tape configuration.
     * @return Constant reference to the ITape::Config structure.
     */
    const ITape::Config& get_tape_config() const noexcept;

    /**
     * @brief Returns the parsed sorter configuration.
     * @return Constant reference to the TapeSorter::Config structure.
     */
    const TapeSorter::Config& get_tape_sorter_config() const noexcept;

  private:
    void parse_line(
        std::string_view line,
        std::unordered_set<std::string>& seen_keys,
        const std::filesystem::path& config_path
    );

    static std::pair<std::string_view, std::size_t> parse_key_and_value(
        std::string_view line,
        const std::unordered_set<std::string>& seen_keys,
        const std::filesystem::path& config_path
    );

    static std::size_t
    parse_value(std::string_view value_sv, std::string_view key, const std::filesystem::path& config_path);

    static bool is_duplicate_key(std::string_view key, const std::unordered_set<std::string>& seen_keys);

    ITape::Config tape_config_;
    TapeSorter::Config tape_sorter_config_;
};

} // namespace tape
