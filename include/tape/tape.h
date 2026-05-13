#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>

namespace tape {

/**
 * @brief Interface for a tape device.
 *
 * Supports sequential read/write, moving the head left/right, and rewinding to the beginning/end. All operations
 * can have configured delays.
 */
class ITape {
  public:
    /**
     * @brief Configuration of operation delays.
     */
    struct Config {
        std::chrono::milliseconds read_delay_{0};
        std::chrono::milliseconds write_delay_{0};
        std::chrono::milliseconds move_delay_{0};
        std::chrono::milliseconds rewind_delay_{0};
    };

    /**
     * @brief Sets the provided configuration.
     * @param config Delay configuration.
     */
    explicit ITape(const Config& config) noexcept;

    /**
     * @brief Reads the element at the current position.
     * @return The value at the current position.
     * @throws TapeException If the head is at the end.
     */
    virtual std::int32_t read() = 0;

    /**
     * @brief Writes a value to the current position.
     * @param value Value to write.
     * @throws TapeException If the head is at the end.
     */
    virtual void write(std::int32_t value) = 0;

    /**
     * @brief Moves the head one position to the left.
     * @throws TapeException If the head is already at the beginning.
     */
    virtual void move_left() = 0;

    /**
     * @brief Moves the head one position to the right.
     * @throws TapeException If the head is already at the end.
     */
    virtual void move_right() = 0;

    /**
     * @brief Checks whether the head is at the beginning of the tape.
     * @return <code>true</code> if the position is 0, <code>false</code> otherwise.
     */
    virtual bool is_begin() const noexcept = 0;

    /**
     * @brief Checks whether the head is at the end of the tape (past the last element).
     * @return <code>true</code> if the position equals to the tape size, <code>false</code> otherwise.
     */
    virtual bool is_end() const noexcept = 0;

    /**
     * @brief Rewinds the tape to the beginning.
     */
    virtual void rewind_to_begin() = 0;

    /**
     * @brief Rewinds the tape to the end (past the last element).
     */
    virtual void rewind_to_end() = 0;

    /**
     * @brief Returns the tape configuration.
     * @return Constant reference to the Config.
     */
    const Config& get_config() const noexcept;

    /**
     * @brief Returns the number of elements on the tape.
     * @return Tape size in elements.
     */
    virtual std::size_t size() const noexcept = 0;

    /**
     * @brief Checks whether the tape is empty.
     * @return <code>true</code> if size() == 0, <code>false</code> otherwise.
     */
    virtual bool empty() const noexcept = 0;

    virtual ~ITape() = default;

  protected:
    Config config_;
};

} // namespace tape
