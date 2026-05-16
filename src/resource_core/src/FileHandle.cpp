#include "resource_core/FileHandle.hpp"

namespace lab4::resource
{

FileHandle::FileHandle(const std::string& path)
{
    file_pointer = std::fopen(path.c_str(), "r");
    if (!file_pointer)
    {
        throw ResourceError("Not open: " + path);
    }
}

FileHandle::~FileHandle()
{
    if (file_pointer)
    {
        std::fclose(file_pointer);
    }
}

FILE* FileHandle::get() const
{
    return file_pointer;
}

} // namespace lab4::resource