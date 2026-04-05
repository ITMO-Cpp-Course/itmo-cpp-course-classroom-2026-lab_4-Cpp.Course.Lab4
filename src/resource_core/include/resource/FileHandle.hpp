#pragma once

#include <cstdio>
#include <memory>
#include <string>

namespace lab4::resource
{

class FileHandle
{
  public:
    FileHandle();
    explicit FileHandle(const std::string& path, const std::string& mode = "r");

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle(FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;

    ~FileHandle();

    void open(const std::string& path, const std::string& mode);
    void close();
    bool is_open() const;
    const std::string& path() const;
    void write(const std::string& data);
    std::string read_all();
    FILE* get() const;

  protected:
    struct FileCloser
    {
        void operator()(FILE* f) const;
    };

    std::unique_ptr<FILE, FileCloser> file_;
    std::string path_;
    std::string mode_;
};

} // namespace lab4::resource
