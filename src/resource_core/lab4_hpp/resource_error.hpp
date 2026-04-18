#pragma once
#include "lab4_hpp/resource_error.hpp"
#include <exception>
#include <fstream>
#include <iostream>
#include <string>

namespace lab4::resource
{
class FileErrors : public std::exception
{
  public:
    FileErrors(const std::string& message) : message{message} {}

    const char* what() const noexcept override {}

  private:
    const std::string message;
};

} // namespace lab4::resource
