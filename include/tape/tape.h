#pragma once

#include <chrono>
#include <cstdint>

namespace tape {

class ITape {
  public:
    struct Config {
        std::chrono::milliseconds read_delay_{0};
        std::chrono::milliseconds write_delay_{0};
        std::chrono::milliseconds move_delay_{0};
        std::chrono::milliseconds rewind_delay_{0};
    };

    explicit ITape(const Config& config) noexcept;

    virtual std::int32_t read() = 0;

    virtual void write(std::int32_t value) = 0;

    virtual void move_left() = 0;

    virtual void move_right() = 0;

    virtual bool is_begin() const noexcept = 0;

    virtual bool is_end() const noexcept = 0;

    virtual void rewind_to_begin() = 0;

    virtual void rewind_to_end() = 0;

    const Config& get_config() const noexcept;

    virtual std::size_t size() const noexcept = 0;

    virtual bool empty() const noexcept = 0;

    virtual ~ITape() = default;

  protected:
    Config config_;
};

} // namespace tape
