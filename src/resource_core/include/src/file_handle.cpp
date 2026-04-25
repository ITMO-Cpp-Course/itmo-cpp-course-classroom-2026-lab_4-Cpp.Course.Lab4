#include "resource_core/file_handle.hpp"
#include "resource_core/resource_error.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cerrno>
#include <utility>

namespace lab4::resource {

namespace {
    // Преобразует строковый режим в флаги open() и (опционально) mode_t для создания.
    std::pair<int, mode_t> parse_mode(const std::string& mode) {
        int flags = 0;
        mode_t create_mode = 0644; // по умолчанию, если потребуется O_CREAT

        if (mode == "r") {
            flags = O_RDONLY;
        } else if (mode == "r+") {
            flags = O_RDWR;
        } else if (mode == "w") {
            flags = O_WRONLY | O_CREAT | O_TRUNC;
        } else if (mode == "w+") {
            flags = O_RDWR | O_CREAT | O_TRUNC;
        } else if (mode == "a") {
            flags = O_WRONLY | O_CREAT | O_APPEND;
        } else if (mode == "a+") {
            flags = O_RDWR | O_CREAT | O_APPEND;
        } else {
            throw ResourceError("Unsupported file open mode: " + mode);
        }
        return {flags, create_mode};
    }
} // unnamed namespace

FileHandle::FileHandle(const std::string& path, const std::string& mode)
    : path_(path)
{
    auto [flags, create_mode] = parse_mode(mode);
    fd_ = ::open(path.c_str(), flags, create_mode);
    if (fd_ == -1) {
        throw ResourceError("Failed to open file '" + path + "'", errno);
    }
}

FileHandle::~FileHandle() {
    if (fd_ != -1) {
        ::close(fd_); // в деструкторе игнорируем ошибку
    }
}

FileHandle::FileHandle(FileHandle&& other) noexcept
    : fd_(std::exchange(other.fd_, -1))
    , path_(std::move(other.path_))
{}

FileHandle& FileHandle::operator=(FileHandle&& other) noexcept {
    if (this != &other) {
        close();
        fd_ = std::exchange(other.fd_, -1);
        path_ = std::move(other.path_);
    }
    return *this;
}

void FileHandle::close() {
    if (fd_ != -1) {
        if (::close(fd_) == -1) {
            throw ResourceError("Failed to close file '" + path_ + "'", errno);
        }
        fd_ = -1;
        path_.clear();
    }
}

} // namespace lab4::resource