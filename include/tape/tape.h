#pragma once
#include <cstdint>
#include <optional>

namespace tape {

class ITape {
  public:
    virtual std::optional<std::int32_t> read() const = 0;

    virtual bool write(std::int32_t value) = 0;

    virtual void move_left() = 0;

    virtual void move_right() = 0;

    virtual std::size_t size() const noexcept = 0;

    virtual std::size_t offset() const noexcept = 0;

    virtual ~ITape() = default;
};

} // namespace tape
