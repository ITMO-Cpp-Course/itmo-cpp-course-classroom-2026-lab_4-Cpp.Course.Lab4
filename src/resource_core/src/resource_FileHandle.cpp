#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <lab4/resource/resource_FileHandle.hpp>
#include <lab4/resource/resource_ResourceError.hpp>
#include <lab4/resource/resource_ResourceManager.hpp>
#include <unistd.h>
#include <vector>
namespace lab4::resource
{
bool FileHandle::is_valid() const noexcept
{
    return fd_ >= 0;
}

FileHandle::FileHandle(const std::string& path, int flags) : fd_(::open(path.c_str(), flags))
{
    if (fd_ < 0)
    {
        throw ResourceError("Failed to open file: " + std::string(std::strerror(errno)));
    }
}

FileHandle::FileHandle(int fd) noexcept : fd_(fd) {}

FileHandle::FileHandle(FileHandle&& other) noexcept
{
    fd_ = other.fd_;
    other.fd_ = -1;
}

FileHandle& FileHandle::operator=(FileHandle&& other) noexcept
{
    if (this != &other)
    {
        close();
        fd_ = other.fd_;
        fd_ = -1;
    }
    return *this;
}

FileHandle::~FileHandle()
{
    close();
}

void FileHandle::close() noexcept
{
    if (is_valid())
    {
        ::close(fd_);
        fd_ = -1;
    }
}

long FileHandle::read(char* buffer, std::size_t count)
{
    long bytes = ::read(fd_, static_cast<void*>(buffer), count);
    if (bytes < 0)
    {
        throw ResourceError("Read error: " + std::string(std::strerror(errno)));
    }
    return bytes;
}

long FileHandle::write(const char* buffer, std::size_t count)
{
    long bytes = ::write(fd_, static_cast<const void*>(buffer), count);
    if (bytes < 0)
    {
        throw ResourceError("Write error: " + std::string(std::strerror(errno)));
    }
    return bytes;
}
bool FileHandle::operator==(const FileHandle& other) const
{
    return fd_ == other.fd_;
}
bool FileHandle::operator!=(const FileHandle& other) const
{
    return fd_ != other.fd_;
}

}; // namespace lab4::resource
