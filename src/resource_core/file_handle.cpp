#include "file_handle.hpp"
#include "resource_error.hpp"
#include <utility>
namespace lab4::resource
{
FileHandle::FileHandle(const std::string& path, const std::string& mode)
{
    file_ptr = std::fopen(path.c_str(), mode.c_str());
    if (!file_ptr)
    {
        throw ResourceError("Не удалось открыть файл по пути: " + path);
    }
}
FileHandle::~FileHandle()
{
    if (file_ptr)
    {
        std::fclose(file_ptr);
        file_ptr = nullptr;
    }
}

std::FILE* FileHandle::Get() const
{
    return file_ptr;
}
} // namespace lab4::resource