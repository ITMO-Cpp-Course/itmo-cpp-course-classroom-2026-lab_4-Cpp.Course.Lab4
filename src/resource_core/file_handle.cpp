#include "resource_core/file_handle.hpp"

#include <utility>

namespace lab4::resource
{

FileHandle::FileHandle(const std::string& path, const std::string& mode)
    : file_(std::fopen(path.c_str(), mode.c_str())), path_(path)
{
    if (!file_)
    {
        throw ResourceError("Failed to open file: " + path);
    }
}

FileHandle::~FileHandle()
{
    close();
}

FileHandle::FileHandle(FileHandle&& other) noexcept : file_(other.file_), path_(std::move(other.path_))
{
    other.file_ = nullptr;
}

FileHandle& FileHandle::operator=(FileHandle&& other) noexcept
{
    if (this != &other)
    {
        close();
        file_ = other.file_;
        path_ = std::move(other.path_);
        other.file_ = nullptr;
    }
    return *this;
}

bool FileHandle::is_open() const noexcept
{
    return file_ != nullptr;
}

const std::string& FileHandle::path() const noexcept
{
    return path_;
}

std::string FileHandle::read_all()
{
    if (!file_)
    {
        throw ResourceError("Cannot read: file is not open");
    }
    std::fseek(file_, 0, SEEK_END);
    long const size = std::ftell(file_);
    if (size < 0)
    {
        throw ResourceError("Failed to determine file size: " + path_);
    }
    std::fseek(file_, 0, SEEK_SET);

    std::string result(static_cast<std::size_t>(size), '\0');
    std::fread(result.data(), 1, static_cast<std::size_t>(size), file_);
    return result;
}

void FileHandle::write(const std::string& data)
{
    if (!file_)
    {
        throw ResourceError("Cannot write: file is not open");
    }
    if (std::fwrite(data.data(), 1, data.size(), file_) != data.size())
    {
        throw ResourceError("Write failed for file: " + path_);
    }
    std::fflush(file_);
}

void FileHandle::close() noexcept
{
    if (file_)
    {
        std::fclose(file_);
        file_ = nullptr;
    }
}

} // namespace lab4::resource
