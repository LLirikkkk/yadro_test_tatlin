#include "tape/file-tape.h"

#include "exceptions/exceptions.h"

#include <array>
#include <filesystem>
#include <format>
#include <limits>
#include <thread>

namespace tape {

FileTape::FileTape(std::string_view path, const Config& config)
    : ITape(config)
    , file_(std::filesystem::path(path), std::ios_base::in | std::ios_base::out | std::ios_base::binary) {
    if (file_.fail()) {
        throw TapeException(std::format("Could not open file: {}", path));
    }

    size_bytes_ = get_file_size(path);
}

FileTape::FileTape(std::string_view path, const std::size_t number_of_elements, const Config& config)
    : ITape(config) {
    if (number_of_elements > std::numeric_limits<std::streamoff>::max() / ELEMENT_SIZE) {
        throw TapeException("Number of elements exceeded maximum possible size of tape");
    }

    size_bytes_ = static_cast<std::streamoff>(number_of_elements) * ELEMENT_SIZE;
    create_empty_tape(path, size_bytes_);
}

std::int32_t FileTape::read() {
    if (is_end()) {
        throw TapeException("Could not read right bound of tape");
    }

    std::this_thread::sleep_for(config_.read_delay_);

    seek_read_position(offset_bytes_, std::ios_base::beg);

    std::int32_t read = 0;
    file_.read(reinterpret_cast<char*>(&read), sizeof(read));
    if (file_.fail()) {
        throw TapeException(std::format("Could not read tape"));
    }

    return read;
}

void FileTape::write(const std::int32_t value) {
    if (is_end()) {
        throw TapeException(std::format("Could not write right bound of tape"));
    }

    std::this_thread::sleep_for(config_.write_delay_);

    seek_write_position(offset_bytes_, std::ios_base::beg);

    file_.write(reinterpret_cast<const char*>(&value), sizeof(value));
    if (file_.fail()) {
        throw TapeException(std::format("Could not write tape"));
    }
}

void FileTape::move_left() {
    if (is_begin()) {
        throw TapeException("Could not move head of tape left, because left bound is reached");
    }

    std::this_thread::sleep_for(config_.move_delay_);

    offset_bytes_ -= ELEMENT_SIZE;
}

void FileTape::move_right() {
    if (is_end()) {
        throw TapeException("Could not move head of tape right, because right bound is reached");
    }

    std::this_thread::sleep_for(config_.move_delay_);

    offset_bytes_ += ELEMENT_SIZE;
}

bool FileTape::is_begin() const noexcept {
    return offset_bytes_ == 0;
}

bool FileTape::is_end() const noexcept {
    return offset_bytes_ == size_bytes_;
}

void FileTape::rewind_to_begin() {
    std::this_thread::sleep_for(config_.rewind_delay_);
    offset_bytes_ = 0;
}

void FileTape::rewind_to_end() {
    std::this_thread::sleep_for(config_.rewind_delay_);
    offset_bytes_ = size_bytes_;
}

std::size_t FileTape::size() const noexcept {
    return size_bytes_ / ELEMENT_SIZE;
}

bool FileTape::empty() const noexcept {
    return size() == 0;
}

std::streamoff FileTape::get_file_size(std::string_view path) {
    seek_read_position(0, std::ios_base::end);

    const std::streamoff size_bytes = file_.tellg();
    if (size_bytes == -1) {
        throw TapeException(std::format("Could not read file: {}", path));
    }

    if (size_bytes % ELEMENT_SIZE != 0) {
        throw TapeException(std::format("Invalid data in file: {}", path));
    }

    return size_bytes;
}

void FileTape::seek_read_position(const std::streamoff offset, std::ios_base::seekdir seekdir) {
    file_.clear();
    file_.seekg(offset, seekdir);
    if (file_.fail()) {
        throw TapeException(std::format("Could not seek read position"));
    }
}

void FileTape::seek_write_position(const std::streamoff offset, std::ios_base::seekdir seekdir) {
    file_.clear();
    file_.seekp(offset, seekdir);
    if (file_.fail()) {
        throw TapeException(std::format("Could not seek write position"));
    }
}

void FileTape::create_empty_tape(std::string_view path, const std::streamoff size_bytes) {
    try {
        file_ = std::fstream(
            std::filesystem::path(path),
            std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc
        );
        if (file_.fail()) {
            throw TapeException(std::format("Could not create file: {}", path));
        }

        fill_tape(size_bytes);
    } catch (...) {
        cleanup_failed_creation(path);
        throw;
    }
}

void FileTape::fill_tape(const std::streamoff size_bytes) {
    static constexpr std::size_t BUFFER_SIZE = 4096;
    constexpr std::array<char, BUFFER_SIZE> zeros{};

    std::streamoff remaining = size_bytes;
    while (remaining > 0) {
        const auto block = std::min(remaining, static_cast<std::streamoff>(BUFFER_SIZE));
        file_.write(zeros.data(), block);
        if (file_.fail()) {
            throw TapeException("Could not write tape");
        }

        remaining -= block;
    }
}

void FileTape::cleanup_failed_creation(std::string_view path) noexcept {
    file_.close();

    std::error_code ec;
    std::filesystem::remove(path, ec);
}

} // namespace tape
