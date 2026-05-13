#pragma once

#include "tape.h"

#include <filesystem>
#include <fstream>
#include <string_view>

namespace tape {

/**
 * @brief Implementation of ITape using a binary file as storage.
 */
class FileTape : public ITape {
  private:
    static constexpr std::streamoff ELEMENT_SIZE = sizeof(std::int32_t);

  public:
    /**
     * @brief Open an existing binary file as a tape.
     * @param path Path to the binary file.
     * @param config Delay configuration.
     * @throws TapeException If the file cannot be opened, or it cannot be interpreted as tape.
     */
    explicit FileTape(const std::filesystem::path& path, const Config& config);

    /**
     * @brief Creates a new binary file of the given size filled with zeros and opens it as a tape.
     * @param path Path to the new binary file.
     * @param number_of_elements Number of elements on the tape.
     * @param config Delay configuration.
     * @throws TapeException If file creation or filling fails.
     */
    FileTape(const std::filesystem::path& path, std::size_t number_of_elements, const Config& config);

    FileTape(const FileTape&) = delete;

    FileTape& operator=(const FileTape&) = delete;

    FileTape(FileTape&&) noexcept = default;

    FileTape& operator=(FileTape&&) noexcept = default;

    /**
     * @brief Reads the element at the current position.
     * @return The value at the current position.
     * @throws TapeException If the head is at the end or a read error occurs.
     */
    std::int32_t read() override;

    /**
     * @brief Writes a value to the current position.
     * @param value Value to write.
     * @throws TapeException If the head is at the end or a write error occurs.
     */
    void write(std::int32_t value) override;

    /**
     * @brief Moves the head one position to the left.
     * @throws TapeException If the head is already at the beginning.
     */
    void move_left() override;

    /**
     * @brief Moves the head one position to the right.
     * @throws TapeException If the head is already at the end.
     */
    void move_right() override;

    /**
     * @brief Checks whether the head is at the beginning of the tape.
     * @return <code>true</code> if the position is 0, <code>false</code> otherwise.
     */
    bool is_begin() const noexcept override;

    /**
     * @brief Checks whether the head is at the end of the tape (past the last element).
     * @return <code>true</code> if the position equals to the tape size, <code>false</code> otherwise.
     */
    bool is_end() const noexcept override;

    /**
     * @brief Rewinds the tape to the beginning.
     */
    void rewind_to_begin() override;

    /**
     * @brief Rewinds the tape to the end (past the last element).
     */
    void rewind_to_end() override;

    /**
     * @brief Returns the number of elements on the tape.
     * @return Tape size in elements.
     */
    std::size_t size() const noexcept override;

    /**
     * @brief Checks whether the tape is empty.
     * @return <code>true</code> if size() == 0, <code>false</code> otherwise.
     */
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
