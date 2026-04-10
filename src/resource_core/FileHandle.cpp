#include "resource_core/FileHandle.hpp"
#include "resource_core/ResourceError.hpp"
#include <cstring>

namespace lab4::resource
{
FileHandle::FileHandle() : file_(nullptr) {}

FileHandle::FileHandle(const std::string& filename, const std::string& mode) : file_(nullptr), filename_(filename)
{
    open(filename, mode);
}

FileHandle::~FileHandle()
{
    close();
}

FileHandle::FileHandle(FileHandle&& other) noexcept : file_(other.file_), filename_(std::move(other.filename_))
{
    other.file_ = nullptr;
}

FileHandle& FileHandle::operator=(FileHandle&& other) noexcept
{
    if (this != &other)
    {
        close();
        file_ = other.file_;
        filename_ = std::move(other.filename_);
        other.file_ = nullptr;
    }
    return *this;
}

bool FileHandle::is_open() const
{
    return file_ != nullptr;
}

void FileHandle::check_open() const
{
    if (!is_open())
    {
        throw ResourceError("File is not open: " + filename_);
    }
}

void FileHandle::open(const std::string& filename, const std::string& mode)
{ // открытие файла
    if (is_open())
    {
        throw ResourceError("File already open: " + filename_);
    }

    file_ = std::fopen(filename.c_str(), mode.c_str()); // перевод в с строку
    if (!file_)
    {
        throw ResourceError("Failed to open file: " + filename + " (error: " + std::strerror(errno) + ")");
    }
    filename_ = filename; // в конце, чтобы не возникало противоречий из-за того, что файла нет
}

std::string FileHandle::read_line()
{
    check_open();

    std::string line;
    char buffer[4096];

    if (std::fgets(buffer, sizeof(buffer), file_))
    {
        line = buffer;
        // кдаляем символ новой строки, если он есть
        if (!line.empty() && line.back() == '\n')
        {
            line.pop_back();
        }
    }
    else if (std::feof(file_))
    {
        throw ResourceError("End of file reached: " + filename_);
    }
    else
    {
        throw ResourceError("Failed to read from file: " + filename_);
    }

    return line;
}

void FileHandle::write_line(const std::string& line)
{
    check_open();

    if (std::fprintf(file_, "%s\n", line.c_str()) < 0)
    {
        throw ResourceError("Failed to write to file: " + filename_);
    }
    std::fflush(file_);
}

std::FILE* FileHandle::get_native_handle() const
{
    return file_;
}

void FileHandle::close()
{
    if (file_)
    {
        std::fclose(file_);
        file_ = nullptr;
    }
}

} // namespace lab4::resource
