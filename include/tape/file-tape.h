#pragma once

#include "tape.h"

#include <fstream>

namespace tape {

class FileTape : public ITape {
  public:
    explicit FileTape(const std::string& file);

    std::int32_t read() const override;

    void write(std::int32_t value) override;

    void move_left() override;

    void move_right() override;

    bool is_begin() const noexcept override;

    bool is_end() const noexcept override;

    ~FileTape() override;

  private:
    std::size_t offset_ = 0;
    std::fstream file_;
};

} // namespace tape
