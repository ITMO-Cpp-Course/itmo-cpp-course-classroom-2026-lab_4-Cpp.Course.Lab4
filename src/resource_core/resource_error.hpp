#pragma once

#include <stdexcept>
#include <string>

namespace lab4::resource
{

class ResourceError : public std::runtime_error
{
  public:
    explicit ResourceError(const std::string& error);
};

} // namespace lab4::resource