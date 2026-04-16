#pragma once
#include <cstdio>
#include <string>
namespace lab4::resource
{
class FileHandle
{
  private:
    FILE* file_;

  public:
    FileHandle();
    FileHandle(const std::string& name, const std::string& mode);
    ~FileHandle();

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle(FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;

    bool is_open() const;
    void close();
    void open(const std::string& name, const std::string& mode);
};
} // namespace lab4::resource