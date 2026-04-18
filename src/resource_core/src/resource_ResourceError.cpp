#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <lab4/resource/resource_FileHandle.hpp>
#include <lab4/resource/resource_ResourceError.hpp>
#include <lab4/resource/resource_ResourceManager.hpp>
#include <unistd.h>
#include <vector>
namespace lab4::resource
{
ResourceError::ResourceError(const std::string& message) : std::runtime_error(message) {}
ResourceError::ResourceError(const char* message) : std::runtime_error(message) {}

}; // namespace lab4::resource
