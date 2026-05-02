#ifndef LAB_4_OVCINNIKOVIVAN86_RESOURCE_ERROR_HPP
#define LAB_4_OVCINNIKOVIVAN86_RESOURCE_ERROR_HPP
#include <string>
#include <stdexcept>
namespace lab4::resource
{
class ResourceError : public std::runtime_error {
public:
    explicit ResourceError(const std::string& message)
        : std::runtime_error(message) {}
    explicit ResourceError(const char* message)
        : std::runtime_error(message) {}
};

}
#endif // LAB_4_OVCINNIKOVIVAN86_RESOURCE_ERROR_HPP
