#include "exceptions/exceptions.h"

namespace tape {

TapeException::TapeException(const std::string& msg)
    : runtime_error(msg) {}

} // namespace tape
