#pragma once
#include <cstddef>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
namespace lab4::resource
{
class ResourceError : public std::runtime_error
{
  public:
    explicit ResourceError(const std::string& message);

    explicit ResourceError(const char* message);
};

} // namespace lab4::resource
