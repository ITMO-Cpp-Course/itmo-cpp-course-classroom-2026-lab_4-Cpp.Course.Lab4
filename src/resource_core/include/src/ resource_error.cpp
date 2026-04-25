#include "resource_core/resource_error.hpp"
#include <cstring>

namespace lab4::resource {

ResourceError::ResourceError(const std::string& message, int code)
    : std::runtime_error(code != 0
              ? message + ": " + std::strerror(code)
              : message)
    , code_(code)
{}

} // namespace lab4::resource