#include "file_handle.hpp"

namespace lab4::resource
{

FileHandle::FileHandle(const std::string& path, const std::string& mode)
    : file_(std::fopen(path.c_str(), mode.c_str())), path_(path)
{
    if (!file_) // проверка на nullptr
    {
        throw ResourceError("Failed to open file: " + path);
    }
}

FileHandle::~FileHandle()
{
    if (file_) // fclose(NULL) не определен
    {
        std::fclose(file_);
    }
}

FileHandle::FileHandle(FileHandle&& other) noexcept
    : file_(other.file_), path_(std::move(other.path_)) // лишний раз не копируем, выигрываем
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
        path_ = std::move(other.path_);
        other.file_ = nullptr;
    }
    return *this;
}

FILE* FileHandle::get() const
{
    return file_;
}

bool FileHandle::is_open() const
{
    return file_ != nullptr;
}

} // namespace lab4::resource