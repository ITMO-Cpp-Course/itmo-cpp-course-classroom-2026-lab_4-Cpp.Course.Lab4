#include "resource_error.hpp"

namespace lab4::resource
{

ResourceError::ResourceError(const std::string& error) : std::runtime_error(error) {}

} // namespace lab4::resource