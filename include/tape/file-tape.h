#pragma once

#include "tape.h"

#include <fstream>

namespace tape {

class FileTape : public ITape {
  private:
    static constexpr std::streamoff ELEMENT_SIZE = sizeof(std::int32_t);

  public:
    explicit FileTape(std::string_view path);

    FileTape(std::string_view path, std::size_t number_of_elements);

    std::int32_t read() override;

    void write(std::int32_t value) override;

    void move_left() override;

    void move_right() override;

    bool is_begin() const noexcept override;

    bool is_end() const noexcept override;

    ~FileTape() override = default;

  private:
    std::streamoff offset_bytes_ = 0;
    std::streamoff size_bytes_ = 0;
    std::fstream file_;

    void seek_read_position(std::streamoff offset, std::ios_base::seekdir seekdir);

    void seek_write_position(std::streamoff offset, std::ios_base::seekdir seekdir);

    void create_empty_tape(std::string_view path, std::streamoff size_bytes);

    void fill_tape(std::streamoff size_bytes);

    void cleanup_failed_creation(std::string_view path) noexcept;
};

} // namespace tape
