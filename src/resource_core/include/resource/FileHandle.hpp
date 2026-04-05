#pragma once

#include <string>
#include <memory>
#include <cstdio>

namespace lab4::resource {

class FileHandle {
public:
    FileHandle();
    explicit FileHandle(const std::string& path, const std::string& mode = "r");

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle(FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;

    ~FileHandle();

private:
    struct FileCloser {
        void operator()(FILE* f) const;
    };

    std::unique_ptr<FILE, FileCloser> file_;
    std::string path_;
};

} // namespace lab4::resource
