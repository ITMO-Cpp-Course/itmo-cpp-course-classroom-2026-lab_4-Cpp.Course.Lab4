#include "lab4_hpp/resource_error.hpp"
#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace lab4::resource
{
FileErrors::FileErrors(const std::string& message) : message{message} {}

const char* FileErrors::what() const noexcept
{
    return message.c_str();
}
}; // namespace lab4::resource