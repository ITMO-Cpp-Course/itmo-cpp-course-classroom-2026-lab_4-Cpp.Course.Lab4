#pragma once

#include <exception>
#include <string>

namespace lab4::resource
{

class ResourceError : public std::exception
{
  public:
    ResourceError(const char* msg);

    const char* what() const noexcept;

  private:
    std::string message;
};

} // namespace lab4::resource