#pragma once

#include "tape.h"

#include <fstream>
#include <string_view>
#include <filesystem>

namespace tape {

class FileTape : public ITape {
  private:
    static constexpr std::streamoff ELEMENT_SIZE = sizeof(std::int32_t);

  public:
    explicit FileTape(const std::filesystem::path& path, const Config& config);

    FileTape(const std::filesystem::path& path, std::size_t number_of_elements, const Config& config);

    FileTape(const FileTape&) = delete;

    FileTape& operator=(const FileTape&) = delete;

    FileTape(FileTape&&) noexcept = default;

    FileTape& operator=(FileTape&&) noexcept = default;

    std::int32_t read() override;

    void write(std::int32_t value) override;

    void move_left() override;

    void move_right() override;

    bool is_begin() const noexcept override;

    bool is_end() const noexcept override;

    void rewind_to_begin() override;

    void rewind_to_end() override;

    std::size_t size() const noexcept override;

    bool empty() const noexcept override;

    ~FileTape() override = default;

  private:
    std::streamoff offset_bytes_ = 0;
    std::streamoff size_bytes_ = 0;
    std::fstream file_;

    std::streamoff get_file_size(const std::filesystem::path& path);

    void seek_read_position(std::streamoff offset, std::ios_base::seekdir seekdir);

    void seek_write_position(std::streamoff offset, std::ios_base::seekdir seekdir);

    void create_empty_tape(const std::filesystem::path& path, std::streamoff size_bytes);

    void fill_tape(std::streamoff size_bytes);

    void cleanup_failed_creation(const std::filesystem::path& path) noexcept;
};

} // namespace tape
