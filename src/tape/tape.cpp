#include "tape/tape.h"

namespace tape {

ITape::ITape(const Config& config) noexcept
    : config_(config) {}

const ITape::Config& ITape::get_config() const noexcept {
    return config_;
}

} // namespace tape
