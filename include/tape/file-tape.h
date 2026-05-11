#pragma once

#include "tape.h"

#include <fstream>

namespace tape {

class FileTape : public ITape {
  private:
    static constexpr std::size_t ELEMENT_SIZE = sizeof(std::int32_t);

  public:
    explicit FileTape(std::string_view path);

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
};

} // namespace tape
