#include "file_handle.hpp"
#include "resource_error.hpp"
#include <cstdio>
#include <string>

namespace lab4::resource
{

FileHandle::FileHandle() : file_(nullptr) {}

FileHandle::FileHandle(const std::string& path, const std::string& mode) : file_(nullptr)
{
    open(path, mode);
}

FileHandle::~FileHandle()
{
    try
    {
        close(); // Здесь может вылететь ResourceError
    }
    catch (...)
    {
        // Ошибка поймана внутри деструктора и не идет дальше.
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
        if (file_)
        {
            std::fclose(file_);
            file_ = nullptr;
        }
        file_ = other.file_;
        other.file_ = nullptr;
    }
    return *this;
}

void FileHandle::open(const std::string& path, const std::string& mode)
{
    if (file_)
    {
        fclose(file_);
        file_ = nullptr;
    }
    FILE* f = std::fopen(path.c_str(), mode.c_str());
    if (!f)
    {
        throw ResourceError("Failed to open file: " + path);
    }
    file_ = f;
}

void FileHandle::close()
{
    if (file_)
    {
        FILE* t = file_; // Сохраняем во временную переменную
        file_ = nullptr;

        if (std::fclose(t) != 0)
        {
            throw ResourceError("Failed to close file properly");
        }
    }
}
bool FileHandle::is_open() const
{
    return file_ != nullptr;
}

} // namespace lab4::resource