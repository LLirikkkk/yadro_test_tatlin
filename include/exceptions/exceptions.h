#pragma once
#include <stdexcept>

namespace tape {

class TapeException : public std::runtime_error {
  public:
    explicit TapeException(const std::string& msg);

    ~TapeException() override = default;
};

} // namespace tape
