#pragma once
#include "resource_core/resource_err.hpp"
#include <cstdio>
#include <string>

namespace lab4::resource
{

class FileHandle
{
  private:
    FILE* file_pointer;

  public:
    explicit FileHandle(const std::string& path);
    ~FileHandle();

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FILE* get() const;
};

} // namespace lab4::resource