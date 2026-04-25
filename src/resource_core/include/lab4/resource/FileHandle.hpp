#pragma once
#include <fstream>
#include <lab4/resource/ResourceError.hpp>
#include <string>
namespace lab4::resource
{
class FileHandle
{
  private:
    std::string path_;
    std::fstream file_;

  public:
    explicit FileHandle(const std::string& path);
    ~FileHandle();
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
    const std::string& getPath() const;
    bool isOpen() const;
    FileHandle(FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;
};
} // namespace lab4::resource
