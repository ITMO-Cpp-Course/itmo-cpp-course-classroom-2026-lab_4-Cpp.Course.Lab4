#include "lab4/resource/ResourceError.hpp"

namespace lab4::resource
{

ResourceError::ResourceError(const char* msg) : message{msg} {}

const char* ResourceError::what() const noexcept
{
    return message.c_str();
}

} // namespace lab4::resource