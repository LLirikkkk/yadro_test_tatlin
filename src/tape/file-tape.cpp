#include "tape/file-tape.h"

#include "exceptions/exceptions.h"

#include <filesystem>
#include <format>

namespace tape {

FileTape::FileTape(std::string_view path)
    : file_(std::filesystem::path(path), std::ios_base::in | std::ios_base::out | std::ios_base::binary) {
    if (file_.fail()) {
        throw TapeException(std::format("Could not open file: {}", path));
    }

    file_.clear();
    file_.seekg(0, std::ios_base::end);
    if (file_.fail()) {
        throw TapeException(std::format("Could not read file: {}", path));
    }

    size_bytes_ = file_.tellg();
    if (size_bytes_ == -1) {
        throw TapeException(std::format("Could not read file: {}", path));
    }

    if (size_bytes_ % ELEMENT_SIZE != 0) {
        throw TapeException(std::format("Invalid data in file: {}", path));
    }
}

std::int32_t FileTape::read() {
    file_.clear();

    if (is_end()) {
        throw TapeException("Could not read right bound of tape");
    }

    file_.seekg(offset_bytes_, std::ios_base::beg);
    if (file_.fail()) {
        throw TapeException(std::format("Could not read tape"));
    }

    std::int32_t read = 0;
    file_.read(reinterpret_cast<char*>(&read), sizeof(read));
    if (file_.fail()) {
        throw TapeException(std::format("Could not read tape"));
    }

    return read;
}

void FileTape::write(const std::int32_t value) {
    file_.clear();

    if (is_end()) {
        throw TapeException(std::format("Could not write right bound of tape"));
    }

    file_.seekp(offset_bytes_, std::ios_base::beg);
    if (file_.fail()) {
        throw TapeException(std::format("Could not write tape"));
    }

    file_.write(reinterpret_cast<const char*>(&value), sizeof(value));
    if (file_.fail()) {
        throw TapeException(std::format("Could not write tape"));
    }
}

void FileTape::move_left() {
    if (is_begin()) {
        throw TapeException("Could not move head of tape left, because left bound is reached");
    }

    offset_bytes_ -= ELEMENT_SIZE;
}

void FileTape::move_right() {
    if (is_end()) {
        throw TapeException("Could not move head of tape right, because right bound is reached");
    }

    offset_bytes_ += ELEMENT_SIZE;
}

bool FileTape::is_begin() const noexcept {
    return offset_bytes_ == 0;
}

bool FileTape::is_end() const noexcept {
    return offset_bytes_ == size_bytes_;
}

} // namespace tape
