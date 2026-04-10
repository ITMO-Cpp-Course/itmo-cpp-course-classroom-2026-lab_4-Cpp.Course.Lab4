#pragma once

#include "resource_error.hpp"

#include <cstdio>
#include <string>

namespace lab4::resource
{

class FileHandle
{
  public:
    FileHandle(const std::string& path, const std::string& mode);
    ~FileHandle();

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle(FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;

    [[nodiscard]] bool is_open() const noexcept;
    [[nodiscard]] const std::string& path() const noexcept;

    std::string read_all();
    void write(const std::string& data);
    void close() noexcept;

  private:
    std::FILE* file_;
    std::string path_;
};

} // namespace lab4::resource
