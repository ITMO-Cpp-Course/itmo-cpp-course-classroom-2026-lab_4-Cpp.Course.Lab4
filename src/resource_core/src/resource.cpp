#include "lab4/resource.hpp"
#include <cstring>
#include <stdexcept>

namespace lab4::resource
{

ResourceError::ResourceError(const std::string& msg) : message(msg) {}

const char* ResourceError::what() const noexcept
{
    return message.c_str();
}

FileHandle::FileHandle(const std::string& filename, const std::string& mode)
    : mode_(mode), filename_(filename), file_(nullptr)
{
    file_ = std::fopen(filename.c_str(), mode.c_str());
    if (!file_)
    {
        throw ResourceError("Failed to open file: " + filename + " (error: " + std::strerror(errno) + ")");
    }
}

FileHandle::FileHandle(FileHandle&& other) noexcept : filename_(std::move(other.filename_)), file_(other.file_)
{
    other.file_ = nullptr;
}

FileHandle& FileHandle::operator=(FileHandle&& other) noexcept
{
    if (this != &other)
    {
        close();
        filename_ = std::move(other.filename_);
        file_ = other.file_;
        other.file_ = nullptr;
    }
    return *this;
}

FileHandle::~FileHandle()
{
    close();
}

bool FileHandle::is_open() const noexcept
{
    return file_ != nullptr;
}

std::string FileHandle::name() const
{
    return filename_;
}

std::string FileHandle::read_line()
{
    if (!is_open())
    {
        throw ResourceError("Attempt to read from closed file: " + filename_);
    }

    std::string line;
    int ch;
    while ((ch = std::fgetc(file_)) != EOF)
    {
        if (ch == '\n')
        {
            break;
        }
        line.push_back(static_cast<char>(ch));
    }

    if (ch == EOF && !std::feof(file_))
    {
        throw ResourceError("Read error in file: " + filename_);
    }

    return line;
}

void FileHandle::write_line(const std::string& data)
{
    if (!is_open())
    {
        throw ResourceError("Attempt to write to closed file: " + filename_);
    }
    if (!can_write())
    {
        throw ResourceError("File opened for reading only: " + filename_);
    }
    if (std::fprintf(file_, "%s\n", data.c_str()) < 0)
    {
        throw ResourceError("Write error in file: " + filename_);
    }
}
void FileHandle::close()
{
    if (file_)
    {
        std::fclose(file_);
        file_ = nullptr;
    }
}

std::shared_ptr<FileHandle> ResourceManager::get(const std::string& filename, const std::string& mode)
{
    auto it = cache_.find(filename);
    if (it != cache_.end())
    {
        if (auto shared = it->second.lock())
        {
            return shared;
        }
        else
        {
            cache_.erase(it);
        }
    }
    auto handle = std::make_shared<FileHandle>(filename, mode);
    cache_[filename] = handle;
    return handle;
}
} // namespace lab4::resource