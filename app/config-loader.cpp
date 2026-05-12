#include "config-loader.h"

#include <chrono>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>
#include <string>

namespace utils {

void ConfigLoader::read_config(const std::filesystem::path& config_path) {
    std::error_code ec;
    auto file_size = std::filesystem::file_size(config_path, ec);
    if (ec) {
        throw std::runtime_error(
            std::format("Could not get size of file: {}, because: {}", config_path.string(), ec.message())
        );
    }

    if (file_size > MAX_CONFIG_FILE_SIZE) {
        throw std::runtime_error(std::format("Config file is too large: {}", config_path.string()));
    }

    std::ifstream fin(config_path, std::ios::in | std::ios::binary);
    if (fin.fail()) {
        throw std::runtime_error(std::format("Could not open config file: {}", config_path.string()));
    }

    std::unordered_set<std::string> seen_keys;
    std::string line;
    while (std::getline(fin, line)) {
        parse_line(line, seen_keys, config_path);
    }

    if (fin.bad()) {
        throw std::runtime_error(std::format("Could not read config file: {}", config_path.string()));
    }

    if (seen_keys.size() != REQUIRED_KEYS_COUNT) {
        throw std::runtime_error(std::format("Invalid number of keys in config file: {}", config_path.string()));
    }
}

const tape::ITape::Config& ConfigLoader::get_tape_config() const noexcept {
    return tape_config_;
}

const tape::TapeSorter::Config& ConfigLoader::get_tape_sorter_config() const noexcept {
    return tape_sorter_config_;
}

void ConfigLoader::parse_line(
    std::string_view line,
    std::unordered_set<std::string>& seen_keys,
    const std::filesystem::path& config_path
) {
    if (line.length() > MAX_CONFIG_LINE_SIZE) {
        throw std::runtime_error(std::format("Line is too large in config file: {}", config_path.string()));
    }

    auto [key, value] = parse_key_and_value(line, seen_keys, config_path);
    if (key == "read_delay_ms") {
        tape_config_.read_delay_ = std::chrono::milliseconds(value);
    } else if (key == "write_delay_ms") {
        tape_config_.write_delay_ = std::chrono::milliseconds(value);
    } else if (key == "move_delay_ms") {
        tape_config_.move_delay_ = std::chrono::milliseconds(value);
    } else if (key == "rewind_delay_ms") {
        tape_config_.rewind_delay_ = std::chrono::milliseconds(value);
    } else if (key == "memory_limit_bytes") {
        tape_sorter_config_.memory_limit_ = value;
    } else {
        throw std::runtime_error(std::format("Unknown config key: '{}'", key));
    }

    seen_keys.insert(std::string(key));
}

std::pair<std::string_view, std::size_t> ConfigLoader::parse_key_and_value(
    std::string_view line,
    const std::unordered_set<std::string>& seen_keys,
    const std::filesystem::path& config_path
) {
    auto separator_index = line.find('=');
    if (separator_index == std::string::npos) {
        throw std::runtime_error(std::format("Config file is in invalid format: {}", config_path.string()));
    }

    auto key = line.substr(0, separator_index);
    if (is_duplicate_key(key, seen_keys)) {
        throw std::runtime_error(std::format("Duplicate key: {}, in config file: {}", key, config_path.string()));
    }

    auto value = parse_value(line.substr(separator_index + 1), key, config_path);

    return {key, value};
}

std::size_t
ConfigLoader::parse_value(std::string_view value_sv, std::string_view key, const std::filesystem::path& config_path) {
    if (value_sv.empty()) {
        throw std::runtime_error(std::format("Empty value for key: {}, in config file: {}", key, config_path.string()));
    }

    if (value_sv.front() == '-') {
        throw std::runtime_error(
            std::format("Negative value for key: {}, in config file: {}", key, config_path.string())
        );
    }

    std::size_t value = 0;
    std::size_t parsed = 0;
    try {
        value = std::stoull(std::string(value_sv), &parsed);
    } catch (const std::exception& e) {
        throw std::runtime_error(
            std::format(
                "Cannot convert value of key: {}, because: {}, in config file: {}",
                key,
                e.what(),
                config_path.string()
            )
        );
    }

    if (parsed != value_sv.length()) {
        throw std::runtime_error(
            std::format("Invalid value for key: {}, in config file: {}", key, config_path.string())
        );
    }

    return value;
}

bool ConfigLoader::is_duplicate_key(std::string_view key, const std::unordered_set<std::string>& seen_keys) {
    return seen_keys.contains(std::string(key));
}

} // namespace utils
