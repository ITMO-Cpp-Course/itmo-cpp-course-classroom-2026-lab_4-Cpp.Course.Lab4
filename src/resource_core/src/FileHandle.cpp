#include <iostream>
#include <lab4/resource/FileHandle.hpp>
#include <lab4/resource/ResourceError.hpp>
namespace lab4::resource
{
FileHandle::FileHandle(const std::string& path) : path_(path)
{
    file_.open(path_, std::ios::in);
    if (!file_.is_open())
    {
        std::cerr << "Error: Could not open file: " << path_ << std::endl;
        throw ResourceError(path_);
    }
}
FileHandle::~FileHandle()
{
    if (file_.is_open())
    {
        file_.close();
    }
}
const std::string& FileHandle::getPath() const
{
    return path_;
}
bool FileHandle::isOpen() const
{
    return file_.is_open();
}
} // namespace lab4::resource
