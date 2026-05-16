#include "resource_core/resource_err.hpp"

namespace lab4::resource
{

ResourceError::ResourceError(const std::string& mesenge_err) : std::runtime_error(mesenge_err) {}

} // namespace lab4::resource