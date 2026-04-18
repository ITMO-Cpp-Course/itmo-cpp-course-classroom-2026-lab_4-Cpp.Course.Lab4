#include "lab4_hpp/file_handle.hpp"
#include "lab4_hpp/resource_error.hpp"
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

namespace lab4::resource
{
FileHandle::FileHandle(const std::string FileName_, std::ios::openmode mode) : FileName(FileName_), openned(false)
{
    Open(FileName_, mode);
}
FileHandle::~FileHandle()
{
    try
    {
        Close();
    }
    catch (...)
    {
    }
}

FileHandle::FileHandle(FileHandle&& other) noexcept
    : FileName(std::move(other.FileName)), File(std::move(other.File)), openned(other.openned)
{
    other.openned = false;
}
FileHandle& FileHandle::operator=(FileHandle&& other) noexcept
{
    if (this != &other)
    {

        Close();
        FileName = std::move(other.FileName);
        File = std::move(other.File);
        openned = other.openned;
        other.openned = false;
    }
    return *this;
}
bool FileHandle::IsOpen() const
{
    return openned && File.is_open();
}
void FileHandle::Close()
{
    if (openned)
    {
        File.close();
        openned = false;
        FileName.clear();
    }
}
void FileHandle::Open(const std::string& FileName_, std::ios::openmode mode)
{
    if (openned)
    {
        Close();
    }

    File.open(FileName_, mode);

    if (!File.is_open())
    {
        throw FileErrors("Failed to open file: " + FileName_);
    }

    FileName = FileName_;
    openned = true;
}
void FileHandle::EnsureOpen() const
{
    if (!IsOpen())
    {
        throw FileErrors("File is not open: " + FileName);
    }
}

std::string FileHandle::ReadLine()
{
    EnsureOpen();

    std::string line;
    if (!std::getline(File, line))
    {
        if (File.eof())
        {
            return "";
        }
        throw FileErrors("Failed to read line from file: " + FileName);
    }
    return line;
}
void FileHandle::WriteLine(const std::string& line)
{
    EnsureOpen();
    File << line << std::endl;
    if (File.fail())
    {
        throw FileErrors("Failed to write line to file: " + FileName);
    }
}
void FileHandle::Write(const std::string& data)
{
    EnsureOpen();
    File << data;
    if (File.fail())
    {
        throw FileErrors("Failed to write to file: " + FileName);
    }
};
std::string FileHandle::ReadAll()
{
    EnsureOpen();
    File.seekg(0, std::ios::beg);

    std::string result;
    std::string line;

    while (std::getline(File, line))
    {
        result += line;
        result += '\n';
    }

    if (File.fail() && !File.eof())
    {
        throw FileErrors("Failed to read entire file: " + FileName);
    }

    return result;
}
const std::string& FileHandle::GetFileName() const
{
    return FileName;
}
} // namespace lab4::resource