#include "file_handle.hpp"

#include <cerrno>
#include <cstring>

namespace lab4::resource
{

FileHandle::FileHandle(const std::string& name, const std::string& mode) : file_(nullptr), name_(name)
{
    file_ = std::fopen(name.c_str(), mode.c_str());
    if (!file_)
    {
        throw ResourceError("Failed to open '" + name + "': " + std::strerror(errno));
    }
}

FileHandle::~FileHandle()
{
    if (file_)
    {
        std::fclose(file_);
    }
}

FileHandle::FileHandle(FileHandle&& other) noexcept : file_(other.file_), name_(std::move(other.name_))
{
    other.file_ = nullptr;
}

FileHandle& FileHandle::operator=(FileHandle&& other) noexcept
{
    if (this != &other)
    {
        if (file_)
        {
            std::fclose(file_);
        }
        file_ = other.file_;
        name_ = std::move(other.name_);
        other.file_ = nullptr;
    }
    return *this;
}

void FileHandle::read(void* data, std::size_t size)
{
    if (!file_)
    {
        throw ResourceError("File is not open: " + name_);
    }

    std::size_t bytes_read = std::fread(data, 1, size, file_);
    if (bytes_read != size)
    {
        if (std::ferror(file_))
        {
            throw ResourceError("Read failed '" + name_ + "': " + std::strerror(errno));
        }
        else if (std::feof(file_))
        {
            throw ResourceError("File truncated '" + name_ + "'");
        }
    }
}

void FileHandle::write(const void* data, std::size_t size)
{
    if (!file_)
    {
        throw ResourceError("File is not open: " + name_);
    }

    std::size_t bytes_written = std::fwrite(data, 1, size, file_);
    if (bytes_written != size)
    {
        throw ResourceError("Write failed '" + name_ + "': " + std::strerror(errno));
    }
}

void FileHandle::seek(long delta, int origin)
{
    if (!file_)
    {
        throw ResourceError("File is not open: " + name_);
    }

    if (std::fseek(file_, delta, origin) != 0)
    {
        throw ResourceError("Seek failed '" + name_ + "': " + std::strerror(errno));
    }
}

void FileHandle::close()
{
    if (file_)
    {
        if (std::fclose(file_) != 0)
        {
            throw ResourceError("Close failed '" + name_ + "': " + std::strerror(errno));
        }
        file_ = nullptr;
    }
    else
    {
        throw ResourceError("File is not open: " + name_);
    }
}

} // namespace lab4::resource