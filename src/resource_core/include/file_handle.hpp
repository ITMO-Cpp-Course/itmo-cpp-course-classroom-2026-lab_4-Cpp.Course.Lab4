#pragma once
#include "resource_error.hpp"
#include <string>

namespace lab4::resource
{

class FileHandle
{
  public:
    explicit FileHandle(const std::string& path, const std::string& mode); // чтобы без лишних копирований
    ~FileHandle();

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete; // запретили любое копирование

    FileHandle(FileHandle&& other) noexcept; // не будет ничего, что может упасть
    FileHandle& operator=(FileHandle&& other) noexcept;

    [[nodiscard]] FILE* get() const; // чтобы кушали С-функции
    [[nodiscard]] bool is_open() const;

  private:
    FILE* file_;
    std::string path_;
};

} // namespace lab4::resource