#pragma once

#include <cstdint>

namespace tape {

class ITape {
  public:
    virtual std::int32_t read() = 0;

    virtual void write(std::int32_t value) = 0;

    virtual void move_left() = 0;

    virtual void move_right() = 0;

    virtual bool is_begin() const noexcept = 0;

    virtual bool is_end() const noexcept = 0;

    virtual ~ITape() = default;
};

} // namespace tape
