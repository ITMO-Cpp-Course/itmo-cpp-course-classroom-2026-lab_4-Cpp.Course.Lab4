#define _CRT_SECURE_NO_WARNINGS
#include "handle.hpp"
#include "resource_error.hpp"
namespace lab4::resource
{
FileHandle::FileHandle() : file_(nullptr) {}
FileHandle::FileHandle(const std::string& name, const std::string& mode) : file_(nullptr)
{
    open(name, mode);
}
FileHandle::~FileHandle()
{
    if (file_ != nullptr)
    {
        std::fclose(file_);
    }
}
FileHandle::FileHandle(FileHandle&& other) noexcept : file_(other.file_)
{
    other.file_ = nullptr;
}
FileHandle& FileHandle::operator=(FileHandle&& other) noexcept
{
    if (this != &other)
    {
        close();
        file_ = other.file_;
        other.file_ = nullptr;
    }
    return *this;
}
bool FileHandle::is_open() const
{
    return file_ != nullptr;
}
void FileHandle::open(const std::string& name, const std::string& mode)
{
    if (is_open())
    {
        close();
    }
    file_ = std::fopen(name.c_str(), mode.c_str());
    if (file_ == nullptr)
    {
        throw ResourceError("Failed to open file: " + name);
    }
}
void FileHandle::close()
{
    if (file_ != nullptr)
    {
        if (std::fclose(file_) != 0)
        {
            throw ResourceError("Failed to close file");
        }
        file_ = nullptr;
    }
}
} // namespace lab4::resource