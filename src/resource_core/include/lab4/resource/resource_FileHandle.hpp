#pragma once
#include <cstddef>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
namespace lab4::resource
{
class FileHandle
{
  public:
    explicit FileHandle(const std::string& path, int flags);
    explicit FileHandle(int fd) noexcept;

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle(FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;

    ~FileHandle();

    bool is_valid() const noexcept;

    long read(char* buffer, std::size_t count);
    long write(const char* buffer, std::size_t count);

    bool operator==(const FileHandle& other) const;
    bool operator!=(const FileHandle& other) const;

  private:
    int fd_;
    void close() noexcept;
};

} // namespace lab4::resource
