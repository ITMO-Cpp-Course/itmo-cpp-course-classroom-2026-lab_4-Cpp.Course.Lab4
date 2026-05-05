#ifndef LAB_4_OVCINNIKOVIVAN86_RESOURCE_ERROR_HPP
#define LAB_4_OVCINNIKOVIVAN86_RESOURCE_ERROR_HPP
#include <stdexcept>
#include <string>
namespace lab4::resource
{
class ResourceError : public std::runtime_error
{
  public:
    explicit ResourceError(const std::string& message) : std::runtime_error(message) {}
    explicit ResourceError(const char* message) : std::runtime_error(message) {}
};

} // namespace lab4::resource
#endif // LAB_4_OVCINNIKOVIVAN86_RESOURCE_ERROR_HPP
