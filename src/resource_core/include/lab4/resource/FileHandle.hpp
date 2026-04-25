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
    FileHandle(FileHandle&& other) noexcept = default;
    FileHandle& operator=(FileHandle&& other) noexcept = default;
    const std::string& getPath() const;
    bool isOpen() const;
};
} // namespace lab4::resource
