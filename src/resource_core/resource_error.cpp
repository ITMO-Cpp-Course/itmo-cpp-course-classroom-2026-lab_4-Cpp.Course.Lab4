#include "resource_error.hpp"

namespace lab4::resource
{

ResourceError::ResourceError(const std::string& text) : std::runtime_error(text) {}

} // namespace lab4::resource