#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS // in order to use fopen instead of fopen_s
#endif

#include "lab4/resource/FileHandler.hpp"
#include <lab4/resource/ResourceError.hpp>
#include <cstdio>

namespace lab4::resource
{

FileHandler::FileHandler(const std::string& filepath_) : file{nullptr}, filepath{}
{
    openFile(filepath_);
}

FileHandler::~FileHandler() noexcept
{
    closeFile();
}

FileHandler::FileHandler(FileHandler&& other) noexcept : file{other.file}, filepath{other.filepath}
{
    other.file = nullptr;
    other.filepath = "";
}

FileHandler& FileHandler::operator=(FileHandler&& other) noexcept
{
    if (this != &other)
    {
        closeFile();
        file = other.file;
        filepath = other.filepath;
        other.file = nullptr;
        other.filepath = "";
    }

    return *this;
}

void FileHandler::open(const std::string& filepath_)
{
    if (file)
    {
        close();
    }
    openFile(filepath_);
}

void FileHandler::close()
{
    if (!file)
    {
        return;
    }
    closeFile();
}

void FileHandler::openFile(const std::string& filepath_)
{
    std::FILE* f = fopen(filepath_.c_str(), "r");

    if (!f)
    {
        throw ResourceError("Failed to open file!");
    }

    file = f;
    filepath = filepath_;
}

void FileHandler::closeFile() noexcept
{
    if (file)
    {
        std::fclose(file);
        file = nullptr;
        filepath = "";
    }
}

std::FILE* FileHandler::getRawPointer() const
{
    return file;
}

std::string FileHandler::getFilepath() const
{
    return filepath;
}

} // namespace lab4::resource