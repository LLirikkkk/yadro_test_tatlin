#pragma once
#include <stdexcept>
#include <string>

namespace tape {

/**
 * @brief Exception thrown by tape-related or sorting errors.
 */
class TapeException : public std::runtime_error {
  public:
    /**
     * @brief Sets the provided error message.
     * @param msg Description of the error.
     */
    explicit TapeException(const std::string& msg);

    ~TapeException() override = default;
};

} // namespace tape
