#pragma once

#include <cstdio>
#include <string>

#include "resource_error.hpp"

namespace lab4::resource
{

class FileHandle
{
  public:
    explicit FileHandle(const std::string& name, const std::string& mode);
    ~FileHandle();

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle(FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;

    bool is_open() const noexcept
    {
        return file_ != nullptr;
    }
    const std::string& get_name() const noexcept
    {
        return name_;
    }

    void read(void* data, std::size_t size);
    void write(const void* data, std::size_t size);
    void seek(long delta, int origin);
    void close();

  private:
    std::FILE* file_;
    std::string name_;
};

} // namespace lab4::resource